#include "UnitTest.h"

#include "apps/sequencer/engine/generators/AcidGenerator.h"
#include "apps/sequencer/engine/generators/ChaosGenerator.h"
#include "apps/sequencer/engine/generators/EuclideanGenerator.h"
#include "apps/sequencer/engine/generators/RandomGenerator.h"
#include "apps/sequencer/engine/generators/SequenceBuilder.h"
#include "apps/sequencer/model/Project.h"

#include <bitset>
#include <cmath>

namespace {
int noteLayerValue(const NoteSequenceBuilder &builder, int index) {
    const auto range = NoteSequence::layerRange(NoteSequence::Layer::Note);
    return int(std::round(builder.value(index) * (range.max - range.min) + range.min));
}

void selectFirstSteps(std::bitset<CONFIG_STEP_COUNT> &selected, int count) {
    for (int i = 0; i < count; ++i) {
        selected.set(i);
    }
}
}

UNIT_TEST("GeneratorSemantics") {

    CASE("Chaos init preserves seed and scope while resetting generator defaults") {
        Project project;
        project.clear();
        project.setSelectedTrackIndex(0);
        project.setSelectedPatternIndex(0);
        project.setTrackMode(0, Track::TrackMode::Note);

        std::bitset<CONFIG_STEP_COUNT> selected;
        ChaosSequenceBuilder builder(project, selected, ChaosSequenceBuilder::Scope::Pattern);

        ChaosGenerator::Params params;
        params.seed = 0x12345678u;
        params.amount = 37;
        params.targetMask = (1u << int(ChaosGenerator::Target::Gate)) |
                            (1u << int(ChaosGenerator::Target::Slide));
        params.scope = ChaosGenerator::Scope::Pattern;

        ChaosGenerator generator(builder, params, selected);
        generator.init();

        expectEqual(uint32_t(0x12345678u), generator.seed());
        expectTrue(generator.patternScope());
        expectEqual(100, generator.amount());
        expectTrue(generator.allTargetsEnabled());
    }

    CASE("Chaos pattern scope backs up a full Wreck target without heap-backed step buffers") {
        Project project;
        project.clear();
        project.setSelectedTrackIndex(0);
        project.setSelectedPatternIndex(0);

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            project.setTrackMode(trackIndex, Track::TrackMode::Note);
            auto &sequence = project.noteSequence(trackIndex, 0);
            sequence.setFirstStep(0);
            sequence.setLastStep(CONFIG_STEP_COUNT - 1);
            for (int stepIndex = 0; stepIndex < CONFIG_STEP_COUNT; ++stepIndex) {
                auto &step = sequence.step(stepIndex);
                step.setGate(true);
                step.setNote(63);
            }
        }

        std::bitset<CONFIG_STEP_COUNT> selected;
        ChaosSequenceBuilder builder(project, selected, ChaosSequenceBuilder::Scope::Pattern);

        expectEqual(CONFIG_TRACK_COUNT, builder.noteTrackCount());
        for (int trackSlot = 0; trackSlot < builder.noteTrackCount(); ++trackSlot) {
            expectEqual(CONFIG_STEP_COUNT, builder.targetStepCount(trackSlot));
        }

        ChaosGenerator::Params params;
        params.seed = 0x13572468u;
        params.amount = 100;
        params.targetMask = (1u << int(ChaosGenerator::Target::Note));
        params.scope = ChaosGenerator::Scope::Pattern;
        params.pivotNote = 0;
        params.span = 48;

        ChaosGenerator generator(builder, params, selected);
        generator.showPreview();

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &sequence = project.noteSequence(trackIndex, 0);
            for (int stepIndex = 0; stepIndex < CONFIG_STEP_COUNT; ++stepIndex) {
                expectTrue(sequence.step(stepIndex).note() >= -24);
                expectTrue(sequence.step(stepIndex).note() <= 24);
            }
        }

        generator.showOriginal();

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &sequence = project.noteSequence(trackIndex, 0);
            for (int stepIndex = 0; stepIndex < CONFIG_STEP_COUNT; ++stepIndex) {
                expectEqual(63, sequence.step(stepIndex).note());
            }
        }
    }

    CASE("Euclidean randomize params rerolls values while keeping them in valid ranges") {
        NoteSequence sequence;
        sequence.clear();

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);

        EuclideanGenerator::Params params;
        params.steps = 16;
        params.beats = 4;
        params.offset = 0;

        EuclideanGenerator generator(builder, params);

        const int initialSteps = generator.steps();
        const int initialBeats = generator.beats();
        const int initialOffset = generator.offset();

        bool changed = false;

        for (int i = 0; i < 8; ++i) {
            generator.randomizeParams();
            generator.update();

            expectTrue(generator.steps() >= 1);
            expectTrue(generator.steps() <= CONFIG_STEP_COUNT);
            expectTrue(generator.beats() >= 1);
            expectTrue(generator.beats() <= generator.steps());
            expectTrue(generator.offset() >= 0);
            expectTrue(generator.offset() < CONFIG_STEP_COUNT);

            if (generator.steps() != initialSteps ||
                generator.beats() != initialBeats ||
                generator.offset() != initialOffset) {
                changed = true;
            }
        }

        expectTrue(changed);
    }

    CASE("Acid Euclidean Phrase uses Euclidean gates inside the target window") {
        NoteSequence sequence;
        sequence.clear();
        sequence.setFirstStep(0);
        sequence.setLastStep(15);

        for (int i = 0; i < 16; ++i) {
            auto &step = sequence.step(i);
            step.setGate(true);
            step.setNote(0);
        }

        std::bitset<CONFIG_STEP_COUNT> selected;
        selectFirstSteps(selected, 16);

        AcidSequenceBuilder builder(sequence, NoteSequence::Layer::Note, AcidSequenceBuilder::ApplyMode::EuclideanPhrase, selected);

        AcidGenerator::Params params;
        params.seed = 0x2468ACE0u;
        params.steps = 8;
        params.beats = 3;
        params.offset = 0;
        params.range = 35;
        params.slide = 0;
        params.tie = 0;
        params.variation = 100;

        AcidGenerator generator(builder, params, selected);

        const auto &preview = builder.previewSequence();
        int gateCount = 0;
        for (int i = 0; i < 16; ++i) {
            gateCount += preview.step(i).gate() ? 1 : 0;
        }

        expectEqual(6, gateCount);
    }

    CASE("Acid Phrase generates held notes without slide or crossing the selection") {
        NoteSequence sequence;
        sequence.clear();
        sequence.setFirstStep(0);
        sequence.setLastStep(15);
        sequence.step(3).setGate(true);
        sequence.step(3).setNote(7);

        std::bitset<CONFIG_STEP_COUNT> selected;
        for (int i = 4; i < 16; ++i) {
            selected.set(i);
        }

        AcidSequenceBuilder builder(sequence, NoteSequence::Layer::Note, AcidSequenceBuilder::ApplyMode::Phrase, selected);
        AcidGenerator::Params params;
        params.seed = 0x76543210u;
        params.density = 100;
        params.slide = 0;
        params.tie = 100;
        params.variation = 100;

        AcidGenerator generator(builder, params, selected);
        const auto &preview = builder.previewSequence();
        int tiedLinks = 0;
        for (int i = 4; i < 16; ++i) {
            const auto &step = preview.step(i);
            if (!step.tieForward()) {
                continue;
            }
            ++tiedLinks;
            expectTrue(i < 15);
            expectTrue(step.gate());
            expectTrue(step.length() == NoteSequence::Length::Max);
            expectTrue(!step.slide());
            expectTrue(preview.step(i + 1).gate());
            expectTrue(preview.step(i + 1).note() == step.note());
            expectTrue(!preview.step(i + 1).slide());
        }
        expectTrue(tiedLinks > 0);
        expectTrue(preview.step(3) == sequence.step(3));

        generator.setTie(0);
        generator.update();
        for (int i = 4; i < 16; ++i) {
            expectTrue(!builder.previewSequence().step(i).tieForward());
        }
    }

    CASE("Acid Euclidean Phrase can extend an onset into a held note") {
        NoteSequence sequence;
        sequence.clear();
        sequence.setFirstStep(0);
        sequence.setLastStep(15);

        std::bitset<CONFIG_STEP_COUNT> selected;
        selectFirstSteps(selected, 16);
        AcidSequenceBuilder builder(sequence, NoteSequence::Layer::Note, AcidSequenceBuilder::ApplyMode::EuclideanPhrase, selected);

        AcidGenerator::Params params;
        params.seed = 0x1A2B3C4Du;
        params.steps = 16;
        params.beats = 8;
        params.slide = 0;
        params.tie = 100;

        AcidGenerator generator(builder, params, selected);
        const auto &preview = builder.previewSequence();
        int tiedLinks = 0;
        for (int i = 0; i < 15; ++i) {
            if (preview.step(i).tieForward()) {
                ++tiedLinks;
                expectTrue(preview.step(i).note() == preview.step(i + 1).note());
                expectTrue(!preview.step(i + 1).slide());
            }
        }
        expectTrue(tiedLinks > 0);
    }

    CASE("Acid Euclidean Phrase NEW RAND keeps generated params in valid ranges") {
        NoteSequence sequence;
        sequence.clear();
        sequence.setFirstStep(0);
        sequence.setLastStep(15);

        std::bitset<CONFIG_STEP_COUNT> selected;
        selectFirstSteps(selected, 16);

        AcidSequenceBuilder builder(sequence, NoteSequence::Layer::Note, AcidSequenceBuilder::ApplyMode::EuclideanPhrase, selected);

        AcidGenerator::Params params;
        params.seed = 0x1234ABCDu;
        AcidGenerator generator(builder, params, selected);

        for (int i = 0; i < 16; ++i) {
            generator.randomizeContextParams();
            generator.update();

            expectTrue(generator.steps() >= 1);
            expectTrue(generator.steps() <= CONFIG_STEP_COUNT);
            expectTrue(generator.beats() >= 1);
            expectTrue(generator.beats() <= generator.steps());
            expectTrue(generator.offset() >= 0);
            expectTrue(generator.offset() < generator.steps());
            expectTrue(generator.range() >= 0);
            expectTrue(generator.range() <= 100);
            expectTrue(generator.slide() >= 0);
            expectTrue(generator.slide() <= 25);
            expectEqual(100, generator.variation());
        }
    }

    CASE("Acid Euclidean Phrase offset follows the current step window") {
        NoteSequence sequence;
        sequence.clear();

        std::bitset<CONFIG_STEP_COUNT> selected;
        selectFirstSteps(selected, 16);

        AcidSequenceBuilder builder(sequence, NoteSequence::Layer::Note, AcidSequenceBuilder::ApplyMode::EuclideanPhrase, selected);

        AcidGenerator::Params params;
        params.steps = 16;
        params.beats = 4;
        params.offset = 15;

        AcidGenerator generator(builder, params, selected);
        expectEqual(15, generator.offset());

        generator.setSteps(4);
        expectEqual(4, generator.steps());
        expectEqual(3, generator.offset());

        generator.setOffset(12);
        expectEqual(3, generator.offset());
    }

    CASE("Random note layer range stays close to the original register at low range") {
        NoteSequence sequence;
        sequence.clear();
        sequence.setFirstStep(0);
        sequence.setLastStep(15);

        for (int i = 0; i < 16; ++i) {
            auto &step = sequence.step(i);
            step.setGate(true);
            step.setNote(12);
        }

        std::bitset<CONFIG_STEP_COUNT> selected;
        selectFirstSteps(selected, 16);

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);

        RandomGenerator::Params params;
        params.seed = 0x12345678u;
        params.smooth = 0;
        params.bias = 0;
        params.scale = 5;
        params.variation = 100;

        RandomGenerator generator(builder, params, selected);

        for (int i = 0; i < 16; ++i) {
            const int note = noteLayerValue(builder, i);
            expectTrue(note >= 11);
            expectTrue(note <= 13);
        }
    }

    CASE("Random note layer bias shifts the musical window") {
        NoteSequence sequence;
        sequence.clear();
        sequence.setFirstStep(0);
        sequence.setLastStep(15);

        for (int i = 0; i < 16; ++i) {
            auto &step = sequence.step(i);
            step.setGate(true);
            step.setNote(0);
        }

        std::bitset<CONFIG_STEP_COUNT> selected;
        selectFirstSteps(selected, 16);

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);

        RandomGenerator::Params params;
        params.seed = 0x87654321u;
        params.smooth = 0;
        params.bias = 3;
        params.scale = 5;
        params.variation = 100;

        RandomGenerator generator(builder, params, selected);

        for (int i = 0; i < 16; ++i) {
            const int note = noteLayerValue(builder, i);
            expectTrue(note >= 8);
            expectTrue(note <= 10);
        }
    }

    CASE("Random generic mapping is unchanged for non-note layers") {
        NoteSequence sequence;
        sequence.clear();

        std::bitset<CONFIG_STEP_COUNT> selected;
        selected.set(0);

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);

        const float generated = builder.randomGeneratorValue(0, 0, 0, 5, selected);
        const float expected = 64.f / 255.f;
        expectTrue(std::fabs(generated - expected) < 0.0001f);
    }
}
