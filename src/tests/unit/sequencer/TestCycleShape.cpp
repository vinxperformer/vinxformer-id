#include "UnitTest.h"

#include "apps/sequencer/engine/CycleShape.h"
#include "apps/sequencer/engine/SequenceState.h"
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
        expectTrue(cycle.advance(Types::RunMode::Forward, 0, 3, 0, rng));
        const uint32_t firstSeed = cycle.seed();
        int firstShapes[4];
        for (int step = 0; step < 4; ++step) {
            firstShapes[step] = cycle.shape(step);
            expectTrue(firstShapes[step] >= 0 && firstShapes[step] < Curve::Last);
        }
        for (int advance = 1; advance < 4; ++advance) {
            expectTrue(!cycle.advance(Types::RunMode::Forward, 0, 3, 0, rng));
            expectEqual(cycle.seed(), firstSeed);
            for (int step = 0; step < 4; ++step) expectEqual(cycle.shape(step), firstShapes[step]);
        }
        expectTrue(cycle.advance(Types::RunMode::Forward, 0, 3, 1, rng));
        expectTrue(cycle.seed() != firstSeed);
        cycle.reset();
        expectEqual(cycle.seed(), uint32_t(0));
    }

    CASE("Reroll follows the track's actual loop when enabled mid-loop") {
        Random shapeRng(19);
        Random stepRng(23);
        CycleShape cycle;
        SequenceState state;
        state.reset();
        state.advanceAligned(2, Types::RunMode::Forward, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::Forward, 0, 3, state.iteration(), shapeRng));
        state.advanceAligned(3, Types::RunMode::Forward, 0, 3, stepRng);
        expectTrue(!cycle.advance(Types::RunMode::Forward, 0, 3, state.iteration(), shapeRng));
        state.advanceAligned(4, Types::RunMode::Forward, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::Forward, 0, 3, state.iteration(), shapeRng));

        cycle.reset();
        state.reset();
        state.advanceAligned(6, Types::RunMode::Pendulum, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::Pendulum, 0, 3, state.iteration(), shapeRng));
        state.advanceAligned(7, Types::RunMode::Pendulum, 0, 3, stepRng);
        expectTrue(!cycle.advance(Types::RunMode::Pendulum, 0, 3, state.iteration(), shapeRng));
        state.advanceAligned(8, Types::RunMode::Pendulum, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::Pendulum, 0, 3, state.iteration(), shapeRng));

        cycle.reset();
        state.reset();
        state.advanceAligned(4, Types::RunMode::PingPong, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::PingPong, 0, 3, state.iteration(), shapeRng));
        state.advanceAligned(5, Types::RunMode::PingPong, 0, 3, stepRng);
        expectTrue(!cycle.advance(Types::RunMode::PingPong, 0, 3, state.iteration(), shapeRng));
        state.advanceAligned(6, Types::RunMode::PingPong, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::PingPong, 0, 3, state.iteration(), shapeRng));

        cycle.reset();
        state.reset();
        state.advanceFree(Types::RunMode::Backward, 0, 3, stepRng);
        state.advanceFree(Types::RunMode::Backward, 0, 3, stepRng);
        state.advanceFree(Types::RunMode::Backward, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::Backward, 0, 3, state.iteration(), shapeRng));
        state.advanceFree(Types::RunMode::Backward, 0, 3, stepRng);
        expectTrue(!cycle.advance(Types::RunMode::Backward, 0, 3, state.iteration(), shapeRng));
        state.advanceFree(Types::RunMode::Backward, 0, 3, stepRng);
        expectTrue(cycle.advance(Types::RunMode::Backward, 0, 3, state.iteration(), shapeRng));
    }

    CASE("Random run modes use an active-span step cycle") {
        Random rng(31);
        CycleShape cycle;
        expectTrue(cycle.advance(Types::RunMode::Random, 4, 6, 0, rng));
        expectTrue(!cycle.advance(Types::RunMode::Random, 4, 6, 0, rng));
        expectTrue(!cycle.advance(Types::RunMode::Random, 4, 6, 0, rng));
        expectTrue(cycle.advance(Types::RunMode::Random, 4, 6, 0, rng));
    }

    CASE("Each new cycle updates only active step shapes") {
        CurveTrack track;
        auto &sequence = track.sequence(0);
        sequence.setFirstStep(1);
        sequence.setLastStep(2);
        for (int index = 0; index < 4; ++index) {
            auto &step = sequence.step(index);
            step.setShape(Curve::Triangle);
            step.setShapeVariation(Curve::Bell);
            step.setShapeVariationProbability(3);
            step.setMin(31);
            step.setMax(190);
            step.setGate(5);
            step.setGateProbability(2);
        }
        const auto outsideBefore = sequence.step(0);
        const auto firstBefore = sequence.step(1);
        const auto secondBefore = sequence.step(2);
        track.setRandomShape(true);
        Random rng(17);
        CycleShape cycle;
        expectTrue(cycle.advance(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), 0, rng));
        cycle.apply(sequence);
        expectTrue(sequence.step(0) == outsideBefore);
        expectTrue(sequence.step(1).shape() != firstBefore.shape());
        expectTrue(sequence.step(2).shape() != secondBefore.shape());
        expectEqual(sequence.step(1).min(), firstBefore.min());
        expectEqual(sequence.step(1).max(), firstBefore.max());
        expectEqual(sequence.step(1).gate(), firstBefore.gate());
        expectEqual(sequence.step(1).shapeVariation(), firstBefore.shapeVariation());
        expectEqual(sequence.step(1).shapeVariationProbability(), firstBefore.shapeVariationProbability());
        expectEqual(sequence.step(1).gateProbability(), firstBefore.gateProbability());
        const auto previousFirst = sequence.step(1).shape();
        expectTrue(!cycle.advance(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), 0, rng));
        expectEqual(sequence.step(1).shape(), previousFirst);
        expectTrue(cycle.advance(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), 1, rng));
        cycle.apply(sequence);
        expectTrue(sequence.step(1).shape() != previousFirst);
        const int lastShape = sequence.step(1).shape();
        track.setRandomShape(false);
        expectEqual(sequence.step(1).shape(), lastShape);
    }
}
