#include "UnitTest.h"

#include "apps/sequencer/engine/ModulatorEngine.h"
#include "apps/sequencer/model/Project.h"
#include "apps/sequencer/model/ProjectVersion.h"

#include <cstring>
#include <vector>

UNIT_TEST("Modulator") {
    CASE("All eight CV modulators start disabled and survive a project round trip") {
        Project original;
        for (int i = 0; i < CONFIG_MODULATOR_COUNT; ++i) {
            expectTrue(!original.modulator(i).enabled());
        }
        auto &first = original.modulator(0);
        first.setEnabled(true);
        first.setRange(Types::VoltageRange::Unipolar5V);
        first.setShape(Modulator::Shape::Random);
        first.setRandomMode(Modulator::RandomMode::Triggered);
        first.setGateTrack(3);
        first.setSmooth(750);
        auto &last = original.modulator(7);
        last.setEnabled(true);
        last.setShape(Modulator::Shape::ADSR);
        last.setAttack(400);
        last.setSustain(64);

        std::vector<uint8_t> data;
        VersionedSerializedWriter writer([&data] (const void *source, size_t len) {
            const auto *bytes = static_cast<const uint8_t *>(source);
            data.insert(data.end(), bytes, bytes + len);
        }, ProjectVersion::Latest);
        original.write(writer);

        size_t offset = 0;
        VersionedSerializedReader reader([&data, &offset] (void *target, size_t len) {
            expectTrue(offset + len <= data.size());
            std::memcpy(target, data.data() + offset, len);
            offset += len;
        }, ProjectVersion::Latest);
        Project restored;
        expectTrue(restored.read(reader));
        expectEqual(offset, data.size());
        expectTrue(restored.modulator(0).enabled());
        expectEqual(restored.modulator(0).range(), Types::VoltageRange::Unipolar5V);
        expectEqual(restored.modulator(0).shape(), Modulator::Shape::Random);
        expectEqual(restored.modulator(0).randomMode(), Modulator::RandomMode::Triggered);
        expectEqual(restored.modulator(0).gateTrack(), 3);
        expectEqual(restored.modulator(0).smooth(), 750);
        for (int i = 1; i < 7; ++i) expectTrue(!restored.modulator(i).enabled());
        expectTrue(restored.modulator(7).enabled());
        expectEqual(restored.modulator(7).shape(), Modulator::Shape::ADSR);
        expectEqual(restored.modulator(7).attack(), 400);
        expectEqual(restored.modulator(7).sustain(), 64);
    }

    CASE("ADSR release reaches zero after its selected duration") {
        Modulator modulator;
        modulator.clear();
        modulator.setEnabled(true);
        modulator.setShape(Modulator::Shape::ADSR);
        modulator.setAttack(0);
        modulator.setDecay(0);
        modulator.setSustain(100);
        modulator.setRelease(500);

        ModulatorEngine engine;
        engine.reset();
        engine.tick(0, modulator, 0, true, 120.f);
        engine.tick(1, modulator, 0, true, 120.f);
        expectEqual(engine.currentValue(0), 100);
        engine.tick(2, modulator, 0, false, 120.f);
        for (int tick = 3; tick <= 22; ++tick) engine.tick(tick, modulator, 0, false, 120.f);
        expectTrue(engine.currentValue(0) < 100);
        for (int tick = 23; tick <= 193; ++tick) engine.tick(tick, modulator, 0, false, 120.f);
        expectEqual(engine.currentValue(0), 0);
    }

    CASE("Version42 projects load with all modulators Off") {
        Project original;
        original.modulator(0).setEnabled(true);
        std::vector<uint8_t> data;
        VersionedSerializedWriter writer([&data] (const void *source, size_t len) {
            const auto *bytes = static_cast<const uint8_t *>(source);
            data.insert(data.end(), bytes, bytes + len);
        }, ProjectVersion::Latest);
        original.write(writer);
        uint32_t oldChecksum;
        std::memcpy(&oldChecksum, data.data() + data.size() - sizeof(uint32_t), sizeof(oldChecksum));

        std::vector<uint8_t> modulatorBytes;
        VersionedSerializedWriter modWriter([&modulatorBytes] (const void *source, size_t len) {
            const auto *bytes = static_cast<const uint8_t *>(source);
            modulatorBytes.insert(modulatorBytes.end(), bytes, bytes + len);
        }, ProjectVersion::Latest);
        original.modulator(0).write(modWriter);
        const size_t modulatorSize = modulatorBytes.size() - sizeof(uint32_t);
        const size_t modulatorStart = data.size() - sizeof(uint32_t) - CONFIG_MODULATOR_COUNT * modulatorSize;
        // Project's FNV checksum excludes the nested sequence checksums, so undo
        // the appended modulator bytes from the saved checksum directly.
        for (size_t i = data.size() - sizeof(uint32_t); i > modulatorStart; --i) {
            oldChecksum = (oldChecksum * 0x359c449bu) ^ data[i - 1];
        }
        data.erase(data.begin() + modulatorStart,
            data.end() - sizeof(uint32_t));
        const uint32_t oldVersion = ProjectVersion::Version42;
        std::memcpy(data.data(), &oldVersion, sizeof(oldVersion));
        std::memcpy(data.data() + data.size() - sizeof(uint32_t), &oldChecksum, sizeof(oldChecksum));

        size_t offset = 0;
        VersionedSerializedReader reader([&data, &offset] (void *target, size_t len) {
            expectTrue(offset + len <= data.size());
            std::memcpy(target, data.data() + offset, len);
            offset += len;
        }, ProjectVersion::Latest);
        Project restored;
        expectTrue(restored.read(reader));
        expectEqual(offset, data.size());
        for (int i = 0; i < CONFIG_MODULATOR_COUNT; ++i) {
            expectTrue(!restored.modulator(i).enabled());
        }
    }
}
