#include "UnitTest.h"

#include "apps/sequencer/model/CurveTrack.h"
#include "apps/sequencer/model/Project.h"
#include "apps/sequencer/model/ProjectVersion.h"

#include <cstring>
#include <vector>

UNIT_TEST("Curve Track Persistence") {
    CASE("Project save and load keeps each Curve track's setting") {
        Project original;
        original.setTrackMode(0, Track::TrackMode::Curve);
        original.setTrackMode(1, Track::TrackMode::Curve);
        original.track(0).curveTrack().setRandomShape(true);
        original.track(1).curveTrack().setRandomShape(false);

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
        expectTrue(restored.track(0).curveTrack().randomShape());
        expectTrue(!restored.track(1).curveTrack().randomShape());
        expectEqual(offset, data.size());
    }

    CASE("Random Shape survives a Version42 save and load") {
        CurveTrack original;
        original.setRandomShape(true);
        original.sequence(0).step(0).setShape(Curve::Bell);

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
        CurveTrack restored;
        restored.read(reader);

        expectTrue(restored.randomShape());
        expectEqual(restored.sequence(0).step(0).shape(), int(Curve::Bell));
        expectEqual(offset, data.size());
    }

    CASE("Version41 projects default Random Shape to Off") {
        CurveTrack original;
        original.setRandomShape(true);

        std::vector<uint8_t> data;
        VersionedSerializedWriter writer([&data] (const void *source, size_t len) {
            const auto *bytes = static_cast<const uint8_t *>(source);
            data.insert(data.end(), bytes, bytes + len);
        }, ProjectVersion::Latest);
        original.write(writer);

        // The new field is appended at the end of CurveTrack's Version42 data.
        data.pop_back();
        const uint32_t oldVersion = ProjectVersion::Version41;
        std::memcpy(data.data(), &oldVersion, sizeof(oldVersion));

        size_t offset = 0;
        VersionedSerializedReader reader([&data, &offset] (void *target, size_t len) {
            expectTrue(offset + len <= data.size());
            std::memcpy(target, data.data() + offset, len);
            offset += len;
        }, ProjectVersion::Latest);
        CurveTrack restored;
        restored.setRandomShape(true);
        restored.read(reader);

        expectTrue(!restored.randomShape());
        expectEqual(offset, data.size());
    }
}
