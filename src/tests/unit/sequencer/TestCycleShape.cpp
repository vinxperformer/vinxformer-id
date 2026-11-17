#include "UnitTest.h"

#include "apps/sequencer/engine/CycleShape.h"
#include "apps/sequencer/model/CurveTrack.h"

UNIT_TEST("Cycle Shape") {
    CASE("Cycle lengths follow the active run mode") {
        expectEqual(CycleShape::cycleLength(Types::RunMode::Forward, 4, 11), 8);
        expectEqual(CycleShape::cycleLength(Types::RunMode::Backward, 4, 11), 8);
        expectEqual(CycleShape::cycleLength(Types::RunMode::Pendulum, 4, 11), 16);
        expectEqual(CycleShape::cycleLength(Types::RunMode::PingPong, 4, 11), 14);
        expectEqual(CycleShape::cycleLength(Types::RunMode::Random, 4, 11), 8);
        expectEqual(CycleShape::cycleLength(Types::RunMode::RandomWalk, 4, 11), 8);
        expectEqual(CycleShape::cycleLength(Types::RunMode::PingPong, 4, 4), 1);
    }

    CASE("Shapes remain stable within a cycle and reroll at the boundary") {
        Random rng(0x12345678u);
        CycleShape cycle;
        cycle.advance(Types::RunMode::Forward, 0, 3, rng);
        const uint32_t firstSeed = cycle.seed();
        int firstShapes[4];
        for (int step = 0; step < 4; ++step) {
            firstShapes[step] = cycle.shape(step);
            expectTrue(firstShapes[step] >= 0 && firstShapes[step] < Curve::Last);
        }
        for (int advance = 1; advance < 4; ++advance) {
            cycle.advance(Types::RunMode::Forward, 0, 3, rng);
            expectEqual(cycle.seed(), firstSeed);
            for (int step = 0; step < 4; ++step) expectEqual(cycle.shape(step), firstShapes[step]);
        }
        cycle.advance(Types::RunMode::Forward, 0, 3, rng);
        expectTrue(cycle.seed() != firstSeed);
        cycle.reset();
        expectEqual(cycle.seed(), uint32_t(0));
    }

    CASE("Live mode never rewrites stored step layers") {
        CurveTrack track;
        track.sequence(0).step(0).setShape(Curve::Triangle);
        track.sequence(0).step(0).setMin(31);
        track.sequence(0).step(0).setMax(190);
        track.sequence(0).step(0).setGate(5);
        const auto original = track.sequence(0).step(0);
        track.setRandomShape(true);
        Random rng(17);
        CycleShape cycle;
        cycle.advance(Types::RunMode::Forward, 0, 3, rng);
        cycle.shape(0);
        expectTrue(track.sequence(0).step(0) == original);
        track.setRandomShape(false);
        expectTrue(track.sequence(0).step(0) == original);
    }
}
