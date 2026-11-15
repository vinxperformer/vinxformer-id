#include "ControllerManager.h"

ControllerManager::ControllerManager(Model &model, Engine &engine) :
    _model(model),
    _engine(engine)
{
    _port = MidiPort::UsbMidi;
}

void ControllerManager::connect(uint16_t vendorId, uint16_t productId) {
    auto info = findController(vendorId, productId);
    if (!info) {
        return;
    }

    if (_controller) {
        disconnect();
    }

    switch (info->type) {
    case ControllerInfo::Type::Launchpad:
        _controller = _controllerContainer.create<LaunchpadController>(*this, _model, _engine, *info);
        break;
    }
}

void ControllerManager::disconnect() {
    clearPerformanceFillDivisorEditTracks();
    if (_controller) {
        _controllerContainer.destroy(_controller);
        _controller = nullptr;
    }
}

void ControllerManager::update() {
    if (_controller) {
        _controller->update();
    }
}

void ControllerManager::setUiPageKind(UiPageKind uiPageKind) {
    _uiPageKind = uiPageKind;
    if (_controller) {
        _controller->uiPageChanged();
    }
}

bool ControllerManager::recvMidi(MidiPort port, uint8_t cable, const MidiMessage &message) {
    if (port != _port) {
        return false;
    }

    if (_controller) {
        _controller->recvMidi(cable, message);
        return true;
    }

    return false;
}

void ControllerManager::setPerformanceFillDivisorEditTrack(int trackIndex, bool active) {
    if (trackIndex < 0 || trackIndex >= CONFIG_TRACK_COUNT) {
        return;
    }

    const uint8_t bit = 1 << trackIndex;
    if (active) {
        _performanceFillDivisorEditTracks |= bit;
    } else {
        _performanceFillDivisorEditTracks &= ~bit;
    }
}

void ControllerManager::clearPerformanceFillDivisorEditTracks() {
    _performanceFillDivisorEditTracks = 0;
}

bool ControllerManager::editPerformanceFillDivisor(int value) {
    if (!_performanceFillDivisorEditTracks) {
        return false;
    }

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        if (_performanceFillDivisorEditTracks & (1 << trackIndex)) {
            _model.project().editTrackFillDivisorOverride(trackIndex, value);
        }
    }
    return true;
}

bool ControllerManager::sendMidi(uint8_t cable, const MidiMessage &message) {
    return _engine.sendMidi(_port, cable, message);
}
