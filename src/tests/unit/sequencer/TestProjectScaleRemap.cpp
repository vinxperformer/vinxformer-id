#include "UnitTest.h"

#include "apps/sequencer/model/Project.h"
#include "apps/sequencer/model/Scale.h"

#include <array>
#include <cmath>

namespace {

bool closeEnough(float a, float b, float eps = 0.18f) {
    return std::fabs(a - b) <= eps;
}

} // namespace

UNIT_TEST("ProjectScaleRemap") {
    CASE("Repeated Random Scale restores the original Project scale without changing steps") {
        Project project;
        project.clear();
        project.setTrackMode(0, Track::TrackMode::Note);
        project.setScale(2); // Minor is the saved scale.
        project.setRootNote(7);
        auto &sequence = project.noteSequence(0, 0);
        sequence.setScale(-1);
        sequence.step(0).setNote(17);
        int previous = project.scale();

        for (uint32_t randomValue = 0; randomValue < 100; ++randomValue) {
            const int chosen = project.randomizePerformanceScale(randomValue);
            expectTrue(chosen >= 0 && chosen < Scale::MusicalPresetCount);
            expectTrue(chosen != previous);
            expectEqual(project.scale(), chosen);
            expectEqual(project.storedScale(), 2);
            expectEqual(project.rootNote(), 7);
            expectEqual(sequence.step(0).note(), 17);
            previous = chosen;
        }

        project.restorePerformanceScale();
        expectTrue(!project.performanceScaleActive());
        expectEqual(project.scale(), 2);
        expectEqual(sequence.step(0).note(), 17);

        project.randomizePerformanceScale(42);
        project.setScale(10); // Manual Scale selection replaces the saved scale.
        expectTrue(!project.performanceScaleActive());
        expectEqual(project.storedScale(), 10);
    }

    CASE("Minor to 22-tet to Gypsy to Arabian to Major Blues to Minor is exact") {
        Project project;
        project.clear();
        project.setTrackMode(0, Track::TrackMode::Note);
        auto &sequence = project.noteSequence(0, 0);
        sequence.setScale(-1); // Follow the Project scale.
        project.setScale(2); // Minor

        std::array<int, 64> originalNotes = {};
        std::array<float, 64> originalVolts = {};
        for (int i = 0; i < 64; ++i) {
            auto &step = sequence.step(i);
            step.setGate(true);
            step.setNote(i - 20);
            originalNotes[i] = step.note();
            originalVolts[i] = project.selectedScale().noteToVolts(step.note());
        }

        for (int scaleIndex : {17, 10, 11, 3, 2}) {
            project.setScale(scaleIndex);
            for (int i = 0; i < 64; ++i) {
                expectEqual(sequence.step(i).note(), originalNotes[i]);
                expectTrue(sequence.step(i).gate());
            }
        }

        for (int i = 0; i < 64; ++i) {
            expectTrue(project.selectedScale().noteToVolts(sequence.step(i).note()) == originalVolts[i]);
        }
    }

    CASE("Project scale changes preserve stored notes and restore their pitches on return") {
        Project project;
        project.clear();
        project.setSelectedPatternIndex(0);
        project.setRootNote(0);

        const int trackIndex = 2; // Track 3 in UI terms
        project.setTrackMode(trackIndex, Track::TrackMode::Note);

        auto &sequence = project.noteSequence(trackIndex, 0);
        sequence.setScale(-1); // Use project-level scale

        std::array<int, 4> indices = {0, 1, 2, 3};
        std::array<int, 4> inputNotes = {1, 17, 29, 41};
        std::array<float, 4> expectedVolts = {};
        const auto &oldScale = Scale::get(project.scale()); // default: semitones

        for (size_t i = 0; i < indices.size(); ++i) {
            auto &step = sequence.step(indices[i]);
            step.setGate(true);
            step.setNote(inputNotes[i]);
            expectedVolts[i] = oldScale.noteToVolts(inputNotes[i]);
        }

        // Every intermediate scale must leave the sequence data untouched.
        for (int scaleIndex : {1, 4, 0, 1, 0}) {
            project.setScale(scaleIndex);
            for (size_t i = 0; i < indices.size(); ++i) {
                expectEqual(sequence.step(indices[i]).note(), inputNotes[i]);
            }
        }

        // Playback uses the active scale, so the initial pitches return exactly.
        for (size_t i = 0; i < indices.size(); ++i) {
            expectTrue(closeEnough(project.selectedScale().noteToVolts(sequence.step(indices[i]).note()), expectedVolts[i], 0.0001f));
        }
    }

    CASE("Sequence-level note scale remap preserves octave register") {
        Project project;
        project.clear();
        project.setSelectedTrackIndex(0);
        project.setSelectedPatternIndex(0);
        project.setTrackMode(0, Track::TrackMode::Note);

        auto &sequence = project.selectedNoteSequence();
        sequence.setScale(0); // Semitones
        sequence.setRootNote(0);

        std::array<int, 4> indices = {0, 1, 2, 3};
        std::array<int, 4> inputNotes = {4, 16, 28, 40};
        std::array<float, 4> expectedVolts = {};
        const auto &oldScale = Scale::get(0);

        for (size_t i = 0; i < indices.size(); ++i) {
            auto &step = sequence.step(indices[i]);
            step.setGate(true);
            step.setNote(inputNotes[i]);
            expectedVolts[i] = oldScale.noteToVolts(inputNotes[i]);
        }

        sequence.setScale(1); // Major
        const auto &newScale = Scale::get(1);

        for (size_t i = 0; i < indices.size(); ++i) {
            const auto &step = sequence.step(indices[i]);
            float actualVolts = newScale.noteToVolts(step.note());
            expectTrue(closeEnough(actualVolts, expectedVolts[i]));
        }

        expectTrue(sequence.step(indices[2]).note() > 6);
        expectTrue(sequence.step(indices[3]).note() > 6);
    }
}
