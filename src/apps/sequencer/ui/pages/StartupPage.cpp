#include "StartupPage.h"

#include "Config.h"

#include "ui/pages/Pages.h"

#include "model/FileManager.h"

#include "core/fs/FileSystem.h"
#include "core/utils/StringBuilder.h"

#include "os/os.h"

StartupPage::StartupPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
    _startTicks = os::ticks();
}

void StartupPage::draw(Canvas &canvas) {
    if (_state == State::Initial) {
        _state = State::Loading;
        _engine.suspend();
        FileManager::task([this] () {
#if defined(__EMSCRIPTEN__)
            // Web simulator policy: keep only the built-in demo project.
            for (int slot = 0; slot < 128; ++slot) {
                FixedStringBuilder<32> path;
                path("PROJECTS/%03d.PRO", slot + 1);
                fs::remove(path);
            }
            fs::remove("LAST.DAT");
            return FileManager::writeProject(_model.project(), 0);
#else
            return FileManager::readLastProject(_model.project());
#endif
        }, [this] (fs::Error result) {
            _engine.resume();
            _state = State::Ready;
        });
    }

    if (relTime() > 1.f && _state == State::Ready) {
        close();
    }

    canvas.setColor(Color::None);
    canvas.fill();

    canvas.setColor(Color::Bright);

    canvas.setFont(Font::Tiny);
    canvas.drawTextCentered(0, 0, Width, 8, "PERSONAL PERFORMANCE FIRMWARE");
    canvas.setFont(Font::Small);
    canvas.drawTextCentered(0, 10, Width, 14, "VINX|FORMER ID");
    canvas.setFont(Font::Tiny);
    canvas.drawTextCentered(0, 23, Width, 8, "v0.1.7");

    canvas.setColor(Color::Medium);
    canvas.hline(0, 32, Width);
    canvas.setColor(Color::Bright);
    canvas.drawTextCentered(0, 34, Width, 8, "BUILT FOR ONE.");
    canvas.drawTextCentered(0, 42, Width, 8, "SHARED WITH EVERYONE.");
    canvas.setColor(Color::Medium);
    canvas.drawTextCentered(0, 51, Width, 7, "NO ROADMAP. JUST EXPLORATION.");
    canvas.drawTextCentered(0, 58, Width, 6, "MY WORKFLOW. MY RULES.");

    int w = std::floor(relTime() * Width);
    canvas.setColor(Color::Bright);
    canvas.fillRect(0, 31, w, 2);
}

void StartupPage::updateLeds(Leds &leds) {
    leds.clear();

    int progress = std::floor(relTime() * 8.f);
    for (int i = 0; i < 8; ++i) {
        bool active = i <= progress;
        leds.set(MatrixMap::fromTrack(i), active, active);
        leds.set(MatrixMap::fromStep(i), active, active);
        leds.set(MatrixMap::fromStep(i + 8), active, active);
    }
}

float StartupPage::time() const {
    return (os::ticks() - _startTicks) / float(os::time::ms(1000));
}
