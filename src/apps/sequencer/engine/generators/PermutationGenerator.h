#pragma once

#include "Generator.h"
#include "SequenceBuilder.h"

class PermutationGenerator : public Generator {
public:
    struct Params {
        uint32_t seed = 0;
        uint8_t move = 3;
        uint8_t pitch = 3;
        uint8_t amount = 50;
    };

    PermutationGenerator(SequenceBuilder &builder, Params &params, int selectedTrack);

    Mode mode() const override { return Mode::Permutation; }
    int paramCount() const override { return 4; }
    const char *paramName(int index) const override;
    void editParam(int index, int value, bool shift) override;
    void printParam(int index, StringBuilder &str) const override;
    void init() override;
    void update() override;
    void showOriginal() override;
    void showPreview() override;

    void randomizeSeed();
    bool trackAvailable(int trackIndex) const;
    bool trackEnabled(int trackIndex) const { return (_trackMask & (1u << trackIndex)) != 0; }
    void toggleTrack(int trackIndex);
    uint8_t trackMask() const { return _trackMask; }
    int move() const { return _params.move; }
    int pitch() const { return _params.pitch; }
    int amount() const { return _params.amount; }

private:
    ChaosSequenceBuilder &_multi;
    Params &_params;
    uint8_t _availableTracks = 0;
    uint8_t _trackMask = 0;
};
