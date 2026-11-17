#pragma once

#include "model/Curve.h"
#include "model/Types.h"
#include "core/utils/Random.h"

#include <cstdint>

class CycleShape {
public:
    void reset() {
        _stepCount = 0;
        _seed = 0;
    }

    static int cycleLength(Types::RunMode runMode, int firstStep, int lastStep) {
        const int steps = lastStep - firstStep + 1;
        if (steps <= 1) return 1;
        switch (runMode) {
        case Types::RunMode::Pendulum: return 2 * steps;
        case Types::RunMode::PingPong: return 2 * steps - 2;
        default: return steps;
        }
    }

    void advance(Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {
        if (_stepCount == 0) _seed = rng.next();
        _stepCount = (_stepCount + 1) % cycleLength(runMode, firstStep, lastStep);
    }

    int shape(int stepIndex) const {
        uint32_t value = _seed ^ (uint32_t(stepIndex) * 0x9e3779b9u);
        value ^= value >> 16;
        value *= 0x7feb352du;
        value ^= value >> 15;
        value *= 0x846ca68bu;
        value ^= value >> 16;
        return int(value % uint32_t(Curve::Last));
    }

    uint32_t seed() const { return _seed; }

private:
    int _stepCount = 0;
    uint32_t _seed = 0;
};
