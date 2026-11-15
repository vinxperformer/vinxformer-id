#include "PermutationGenerator.h"

#include "core/math/Math.h"
#include "core/utils/Random.h"

#include <array>
#include <ctime>

PermutationGenerator::PermutationGenerator(SequenceBuilder &builder, Params &params, int selectedTrack) :
    Generator(builder), _multi(static_cast<ChaosSequenceBuilder &>(builder)), _params(params)
{
    for (int slot = 0; slot < _multi.noteTrackCount(); ++slot) {
        _availableTracks |= uint8_t(1u << _multi.trackIndex(slot));
    }
    if (selectedTrack >= 0 && selectedTrack < CONFIG_TRACK_COUNT) {
        _trackMask = uint8_t(1u << selectedTrack) & _availableTracks;
    }
}

const char *PermutationGenerator::paramName(int index) const {
    switch (index) {
    case 0: return "Seed";
    case 1: return "Move";
    case 2: return "Pitch";
    case 3: return "Amount";
    default: return nullptr;
    }
}

void PermutationGenerator::editParam(int index, int value, bool shift) {
    (void)shift;
    switch (index) {
    case 0: if (value != 0) randomizeSeed(); break;
    case 1: _params.move = uint8_t(clamp(int(_params.move) + value, 1, 8)); break;
    case 2: _params.pitch = uint8_t(clamp(int(_params.pitch) + value, 0, 12)); break;
    case 3: _params.amount = uint8_t(clamp(int(_params.amount) + value, 0, 100)); break;
    default: break;
    }
}

void PermutationGenerator::printParam(int index, StringBuilder &str) const {
    switch (index) {
    case 0: str("%08X", _params.seed); break;
    case 1: str("+/- %d", _params.move); break;
    case 2: str("+/- %d", _params.pitch); break;
    case 3: str("%d%%", _params.amount); break;
    default: break;
    }
}

void PermutationGenerator::init() {
    _params.move = 3;
    _params.pitch = 3;
    _params.amount = 50;
}

void PermutationGenerator::randomizeSeed() {
    static uint32_t entropy = 0x6D2B79F5u;
    entropy = entropy * 1664525u + 1013904223u + uint32_t(clock());
    _params.seed = entropy;
}

bool PermutationGenerator::trackAvailable(int trackIndex) const {
    return trackIndex >= 0 && trackIndex < CONFIG_TRACK_COUNT && (_availableTracks & (1u << trackIndex));
}

void PermutationGenerator::toggleTrack(int trackIndex) {
    if (trackAvailable(trackIndex)) {
        _trackMask ^= uint8_t(1u << trackIndex);
    }
}

void PermutationGenerator::showOriginal() {
    _multi.showOriginal();
}

void PermutationGenerator::showPreview() {
    _multi.showOriginal();
    for (int slot = 0; slot < _multi.noteTrackCount(); ++slot) {
        const int track = _multi.trackIndex(slot);
        if (!trackEnabled(track)) continue;

        const int count = _multi.targetStepCount(slot);
        std::array<bool, CONFIG_STEP_COUNT> used = {};
        Random rng(_params.seed ^ (0x9E3779B9u * uint32_t(track + 1)));
        auto tied = [&] (int index) {
            return _multi.originalStep(slot, index).tieForward() ||
                   (index > 0 && _multi.originalStep(slot, index - 1).tieForward());
        };
        for (int i = 0; i < count; ++i) {
            if (used[i] || tied(i) || !_multi.originalStep(slot, i).gate() ||
                int(rng.next() % 100u) >= _params.amount) continue;

            std::array<int, CONFIG_STEP_COUNT> candidates = {};
            int candidateCount = 0;
            const int sourceStep = _multi.targetStepIndex(slot, i);
            for (int j = 0; j < count; ++j) {
                if (j == i || used[j] || tied(j)) continue;
                const int distance = _multi.targetStepIndex(slot, j) - sourceStep;
                if (distance >= -int(_params.move) && distance <= int(_params.move)) {
                    candidates[candidateCount++] = j;
                }
            }
            if (candidateCount == 0) continue;
            const int j = candidates[rng.next() % uint32_t(candidateCount)];
            auto &source = _multi.liveStep(slot, i);
            auto &destination = _multi.liveStep(slot, j);
            const auto temporary = source;
            source = destination;
            destination = temporary;
            used[i] = used[j] = true;

            if (_params.pitch > 0) {
                for (auto *step : { &source, &destination }) {
                    if (!step->gate()) continue;
                    const int delta = 1 + int(rng.next() % _params.pitch);
                    step->setNote(step->note() + (rng.nextBinary() ? delta : -delta));
                }
            }
        }
    }
    _multi.showPreview();
}

void PermutationGenerator::update() {
    if (showingPreview()) showPreview();
}
