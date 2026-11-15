#include "UnitTest.h"

#include "apps/sequencer/engine/generators/PermutationGenerator.h"

#include <bitset>

namespace {
int gateCount(const NoteSequence &sequence) {
    int count = 0;
    for (int step = sequence.firstStep(); step <= sequence.lastStep(); ++step) {
        if (sequence.step(step).gate()) ++count;
    }
    return count;
}
}

UNIT_TEST("Permutation") {
    CASE("Default movement and pitch ranges are both three steps") {
        Project project;
        project.setTrackMode(0, Track::TrackMode::Note);
        std::bitset<CONFIG_STEP_COUNT> selection;
        ChaosSequenceBuilder builder(project, selection, ChaosSequenceBuilder::Scope::Pattern);
        PermutationGenerator::Params params;
        expectEqual(int(params.move), 3);
        expectEqual(int(params.pitch), 3);
        params.move = 8;
        params.pitch = 12;
        PermutationGenerator generator(builder, params, 0);
        generator.init();
        expectEqual(int(params.move), 3);
        expectEqual(int(params.pitch), 3);
    }

    CASE("Local swaps preserve events, limit travel, and restore all tracks") {
        Project project;
        project.setTrackMode(0, Track::TrackMode::Note);
        project.setTrackMode(1, Track::TrackMode::Note);
        project.setTrackMode(2, Track::TrackMode::Curve);
        project.setSelectedTrackIndex(0);
        auto &first = project.noteSequence(0, 0);
        auto &second = project.noteSequence(1, 0);
        first.setFirstStep(0);
        first.setLastStep(7);
        second.setFirstStep(0);
        second.setLastStep(7);
        for (int step = 0; step < 8; ++step) {
            first.step(step).setGate(step % 2 == 0);
            first.step(step).setNote(20 + step);
            second.step(step).setGate(step % 2 == 1);
            second.step(step).setNote(40 + step);
        }
        const NoteSequence originalFirst = first;
        const NoteSequence originalSecond = second;
        std::bitset<CONFIG_STEP_COUNT> selection;
        ChaosSequenceBuilder builder(project, selection, ChaosSequenceBuilder::Scope::Pattern);
        PermutationGenerator::Params params;
        params.seed = 0x12345678u;
        params.move = 1;
        params.pitch = 0;
        params.amount = 100;
        PermutationGenerator generator(builder, params, 0);
        expectTrue(generator.trackEnabled(0));
        expectTrue(!generator.trackEnabled(1));
        expectTrue(!generator.trackAvailable(2));

        generator.showPreview();
        expectEqual(gateCount(originalFirst), gateCount(first));
        for (int step = 0; step < 8; ++step) {
            if (!first.step(step).gate()) continue;
            const int originalStep = first.step(step).note() - 20;
            expectTrue(originalStep >= 0 && originalStep < 8);
            expectTrue(originalFirst.step(originalStep).gate());
            expectTrue(originalStep - step >= -1 && originalStep - step <= 1);
        }
        for (int step = 0; step < 8; ++step) {
            expectEqual(int(originalSecond.step(step).gate()), int(second.step(step).gate()));
            expectEqual(originalSecond.step(step).note(), second.step(step).note());
        }

        generator.toggleTrack(1);
        generator.showPreview();
        expectEqual(gateCount(originalFirst), gateCount(first));
        expectEqual(gateCount(originalSecond), gateCount(second));
        generator.showOriginal();
        for (int step = 0; step < 8; ++step) {
            expectEqual(int(originalFirst.step(step).gate()), int(first.step(step).gate()));
            expectEqual(originalFirst.step(step).note(), first.step(step).note());
            expectEqual(int(originalSecond.step(step).gate()), int(second.step(step).gate()));
            expectEqual(originalSecond.step(step).note(), second.step(step).note());
        }
    }

    CASE("Tied events stay in place") {
        Project project;
        project.setTrackMode(0, Track::TrackMode::Note);
        auto &sequence = project.noteSequence(0, 0);
        sequence.setFirstStep(0);
        sequence.setLastStep(3);
        for (int step = 0; step < 4; ++step) {
            sequence.step(step).setGate(true);
            sequence.step(step).setNote(10 + step);
        }
        sequence.step(1).setTieForward(true);
        std::bitset<CONFIG_STEP_COUNT> selection;
        ChaosSequenceBuilder builder(project, selection, ChaosSequenceBuilder::Scope::Pattern);
        PermutationGenerator::Params params;
        params.seed = 0x11223344u;
        params.move = 1;
        params.pitch = 0;
        params.amount = 100;
        PermutationGenerator generator(builder, params, 0);
        generator.showPreview();
        expectEqual(11, sequence.step(1).note());
        expectEqual(12, sequence.step(2).note());
        expectTrue(sequence.step(1).tieForward());
    }
}
