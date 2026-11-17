#pragma once

#include "model/CurveSequence.h"
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

    bool advance(Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {
        const bool newCycle = _stepCount == 0;
        if (newCycle) _seed = rng.next();
        _stepCount = (_stepCount + 1) % cycleLength(runMode, firstStep, lastStep);
        return newCycle;
    }

    void apply(CurveSequence &sequence) const {
        for (int index = sequence.firstStep(); index <= sequence.lastStep(); ++index) {
            auto &step = sequence.step(index);
            int nextShape = shape(index);
            if (nextShape == step.shape()) nextShape = (nextShape + 1) % int(Curve::Last);
            step.setShape(nextShape);
        }
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
