#pragma once

#include "NoteSequence.h"

#include <bitset>

namespace NoteTie {

enum class Result {
    NoRange,
    Tied,
    Untied,
};

inline bool selectedAreHeld(const NoteSequence &sequence, const std::bitset<CONFIG_STEP_COUNT> &selected) {
    int first = -1;
    int last = -1;
    for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
        if (selected[i]) {
            if (first < 0) first = i;
            last = i;
        }
    }
    if (first < 0 || last <= first) return false;

    const int headNote = sequence.step(first).note();
    for (int i = first; i < last; ++i) {
        const auto &step = sequence.step(i);
        const auto &next = sequence.step(i + 1);
        if (!step.gate() || !step.tieForward() || step.length() != NoteSequence::Length::Max ||
            !next.gate() || next.note() != headNote) return false;
    }
    return true;
}

inline Result toggleSelected(NoteSequence &sequence, const std::bitset<CONFIG_STEP_COUNT> &selected) {
    int first = -1;
    int last = -1;
    for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
        if (selected[i]) {
            if (first < 0) {
                first = i;
            }
            last = i;
        }
    }
    if (first < 0 || last <= first) {
        return Result::NoRange;
    }

    const int headNote = sequence.step(first).note();
    const bool held = selectedAreHeld(sequence, selected);

    if (first > 0) {
        sequence.step(first - 1).setTieForward(false);
    }

    if (held) {
        for (int i = first; i <= last; ++i) {
            auto &step = sequence.step(i);
            step.setTieForward(false);
            if (i < last) {
                step.setLength(NoteSequence::Length::Max / 2);
            }
        }
        return Result::Untied;
    }

    for (int i = first; i <= last; ++i) {
        auto &step = sequence.step(i);
        step.setGate(true);
        step.setNote(headNote);
        step.setTieForward(i < last);
        if (i < last) {
            step.setLength(NoteSequence::Length::Max);
        }
    }
    return Result::Tied;
}

} // namespace NoteTie
