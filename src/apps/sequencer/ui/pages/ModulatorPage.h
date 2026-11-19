#pragma once

#include "BasePage.h"

class ModulatorPage : public BasePage {
public:
    ModulatorPage(PageManager &manager, PageContext &context);

    void draw(Canvas &canvas) override;
    void updateLeds(Leds &leds) override;
    void keyPress(KeyPressEvent &event) override;
    void encoder(EncoderEvent &event) override;

private:
    enum class Parameter {
        None, On, Range, Shape, Rate, Depth, Offset, Phase, Mode,
        RandomMode, Smooth, Gate, Attack, Decay, Sustain, Release, Amplitude
    };

    Parameter parameter(int function) const;
    static const char *name(Parameter parameter);
    void edit(Parameter parameter, int value, bool fine);

    int _selected = 0;
    int _page = 0;
    int _function = 0;
};
