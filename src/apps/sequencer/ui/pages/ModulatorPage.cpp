#include "ModulatorPage.h"

#include "engine/WaveformGenerator.h"
#include "ui/painters/WindowPainter.h"
#include "core/utils/StringBuilder.h"

ModulatorPage::ModulatorPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context) {}

ModulatorPage::Parameter ModulatorPage::parameter(int function) const {
    const auto shape = _project.modulator(_selected).shape();
    if (_page == 0) {
        const Parameter common[] = {Parameter::On, Parameter::Range, Parameter::Shape,
            shape == Modulator::Shape::ADSR ? Parameter::Attack :
                shape == Modulator::Shape::Random ? Parameter::RandomMode : Parameter::Rate,
            shape == Modulator::Shape::ADSR ? Parameter::Decay : Parameter::Depth};
        return common[function];
    }
    if (shape == Modulator::Shape::ADSR) {
        const Parameter fields[] = {Parameter::Sustain, Parameter::Release,
            Parameter::Amplitude, Parameter::Gate, Parameter::None};
        return fields[function];
    }
    if (shape == Modulator::Shape::Random) {
        const Parameter fields[] = {Parameter::Offset, Parameter::Smooth,
            Parameter::Rate, Parameter::Gate, Parameter::None};
        return fields[function];
    }
    const Parameter fields[] = {Parameter::Offset, Parameter::Phase,
        Parameter::Mode, Parameter::Gate, Parameter::None};
    return fields[function];
}

const char *ModulatorPage::name(Parameter parameter) {
    switch (parameter) {
    case Parameter::On: return "ON/OFF";
    case Parameter::Range: return "RANGE";
    case Parameter::Shape: return "SHAPE";
    case Parameter::Rate: return "RATE";
    case Parameter::Depth: return "DEPTH";
    case Parameter::Offset: return "OFFSET";
    case Parameter::Phase: return "PHASE";
    case Parameter::Mode: return "MODE";
    case Parameter::RandomMode: return "MODE";
    case Parameter::Smooth: return "SLEW";
    case Parameter::Gate: return "GATE";
    case Parameter::Attack: return "ATTACK";
    case Parameter::Decay: return "DECAY";
    case Parameter::Sustain: return "SUSTAIN";
    case Parameter::Release: return "RELEASE";
    case Parameter::Amplitude: return "LEVEL";
    case Parameter::None: return "";
    }
    return "";
}

void ModulatorPage::draw(Canvas &canvas) {
    const auto &modulator = _project.modulator(_selected);
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine,
        FixedStringBuilder<32>("MOD %d / CV %d %s", _selected + 1, _selected + 1,
            modulator.enabled() ? "ON" : "OFF"));

    const char *labels[CONFIG_FUNCTION_KEY_COUNT];
    for (int i = 0; i < CONFIG_FUNCTION_KEY_COUNT; ++i) labels[i] = name(parameter(i));
    WindowPainter::drawFooter(canvas, labels, pageKeyState(), _function);

    const Parameter selected = parameter(_function);
    FixedStringBuilder<32> value;
    switch (selected) {
    case Parameter::On: value("%s", modulator.enabled() ? "ON" : "OFF"); break;
    case Parameter::Range: value("%s", Types::voltageRangeName(modulator.range())); break;
    case Parameter::Shape: value("%s", Modulator::shapeName(modulator.shape())); break;
    case Parameter::Rate: modulator.printRate(value); break;
    case Parameter::Depth: modulator.printDepth(value); break;
    case Parameter::Offset: modulator.printOffset(value); break;
    case Parameter::Phase: modulator.printPhase(value); break;
    case Parameter::Mode: modulator.printMode(value); break;
    case Parameter::RandomMode: modulator.printRandomMode(value); break;
    case Parameter::Smooth: modulator.printSmooth(value); break;
    case Parameter::Gate: modulator.printGateTrack(value); break;
    case Parameter::Attack: modulator.printAttack(value); break;
    case Parameter::Decay: modulator.printDecay(value); break;
    case Parameter::Sustain: modulator.printSustain(value); break;
    case Parameter::Release: modulator.printRelease(value); break;
    case Parameter::Amplitude: modulator.printAmplitude(value); break;
    case Parameter::None: break;
    }

    canvas.setColor(Color::Medium);
    canvas.setFont(Font::Tiny);
    canvas.drawText(6, 14, FixedStringBuilder<32>("%s  %d/2", name(selected), _page + 1));
    canvas.drawText(154, 14, "LEFT/RIGHT: PAGE");
    canvas.setColor(Color::Bright);
    canvas.setFont(Font::Small);
    canvas.drawTextCentered(4, 25, 248, 12, value);

    if (modulator.shape() != Modulator::Shape::ADSR &&
        modulator.shape() != Modulator::Shape::Random) {
        for (int x = 0; x < 112; ++x) {
            uint16_t phase = uint16_t((uint32_t(x) * 65536u / 112u +
                uint32_t(modulator.phase()) * 65536u / 360u) & 0xffffu);
            int sample = WaveformGenerator::generate(modulator.shape(), phase);
            int y = 43 - sample * 6 / 127;
            canvas.vline(8 + x, y, 1);
        }
    }
    const int level = _engine.modulatorEngine().currentValue(_selected);
    canvas.setColor(Color::Medium);
    canvas.drawRect(135, 39, 113, 9);
    canvas.setColor(Color::Bright);
    canvas.hline(137, 43, level * 109 / 127);
}

void ModulatorPage::updateLeds(Leds &leds) {
    for (int i = 0; i < CONFIG_MODULATOR_COUNT; ++i) {
        leds.set(MatrixMap::fromTrack(i), _project.modulator(i).enabled(), i == _selected);
    }
}

void ModulatorPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    if (key.pageModifier()) return;
    if (key.isTrackSelect()) {
        _selected = key.trackSelect();
        _page = 0;
        _function = 0;
        event.consume();
    } else if (key.isLeft() || key.isRight()) {
        _page = key.isRight() ? 1 : 0;
        _function = 0;
        event.consume();
    } else if (key.isFunction()) {
        _function = key.function();
        event.consume();
    } else if (key.isEncoder() && parameter(_function) == Parameter::On) {
        auto &modulator = _project.modulator(_selected);
        modulator.setEnabled(!modulator.enabled());
        event.consume();
    }
}

void ModulatorPage::encoder(EncoderEvent &event) {
    edit(parameter(_function), event.value(), event.pressed());
    event.consume();
}

void ModulatorPage::edit(Parameter parameter, int value, bool fine) {
    if (value == 0) return;
    auto &modulator = _project.modulator(_selected);
    switch (parameter) {
    case Parameter::On: modulator.setEnabled(value > 0); break;
    case Parameter::Range: {
        int range = clamp(int(modulator.range()) + value, 0, int(Types::VoltageRange::Last) - 1);
        modulator.setRange(Types::VoltageRange(range));
        break;
    }
    case Parameter::Shape: modulator.editShape(value, fine); break;
    case Parameter::Rate: modulator.editRate(value, fine); break;
    case Parameter::Depth: modulator.editDepth(value, fine); break;
    case Parameter::Offset: modulator.editOffset(value, fine); break;
    case Parameter::Phase: modulator.editPhase(value, fine); break;
    case Parameter::Mode: modulator.editMode(value, fine); break;
    case Parameter::RandomMode: modulator.editRandomMode(value, fine); break;
    case Parameter::Smooth: modulator.editSmooth(value, fine); break;
    case Parameter::Gate: modulator.editGateTrack(value, fine); break;
    case Parameter::Attack: modulator.editAttack(value, fine); break;
    case Parameter::Decay: modulator.editDecay(value, fine); break;
    case Parameter::Sustain: modulator.editSustain(value, fine); break;
    case Parameter::Release: modulator.editRelease(value, fine); break;
    case Parameter::Amplitude: modulator.editAmplitude(value, fine); break;
    case Parameter::None: break;
    }
}
