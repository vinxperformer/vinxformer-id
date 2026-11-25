# VINX|FORMER ID

**Personal performance firmware. Built for one. Shared with everyone.**

VINX|FORMER ID is Vinx Scorza's experimental fork of [Vinx PER|FORMER](https://github.com/VinxScorza/performer), itself based on the [Mebitek](https://github.com/mebitek/performer) and [Westlicht](https://github.com/westlicht/performer) PER|FORMER projects. VINX|FORMER ID and Vinx PER|FORMER are separate projects. The [VINX|FORMER ID website](https://vinxperformer.github.io/vinxformer-id/) explains the direction of this fork; the [firmware hub](https://vinxperformer.github.io/) links both projects.

This source identifies as **VINX|FORMER ID v0.1.9**. The [v0.1.9 firmware release](https://github.com/vinxperformer/vinxformer-id/releases/tag/v0.1.9) includes the DAT. The source base is Vinx PER|FORMER `v0.4.6-beta.2` (`b1e95b58`).

## What changes in VINX|FORMER ID compared with Vinx PER|FORMER

- **Eight CV modulators.** Mod 1 drives CV 1 through Mod 8 driving CV 8. Each starts Off; switching it On replaces that output's track CV without changing its Gate or sequence. Sine, Triangle, Saw Up, Saw Down, Square, Random, and ADSR are adapted from [modulove/performer](https://github.com/modulove/performer). Output ranges are selectable from 0–1/2/3/4/5 V or ±1/2/3/4/5 V.
- **Permutation on selected Note tracks.** Reroll step position and pitch on one or several tracks at once. Compare ORIGINAL and PERMUTED before applying; each reroll starts from the captured original. MOVE and PITCH default to ±3.
- **Acid held notes.** Acid Phrase and Acid Eucl Phrase can generate same-pitch ties without adding Slide. TIE controls their frequency; 90% of generated groups are 2–3 steps when enough adjacent steps are available. In Note Steps, `SHIFT + F3` (third function key) ties or unties the selected range. With SHIFT held and two or more steps selected, the footer labels that key TIE or UNTIE according to the current range; otherwise it remains LENGTH.
- **Random Shape on Curve tracks.** At each track-loop boundary, write new shapes into the active steps so they are visible in the sequencer. Only Shape changes. The per-track switch is saved with the project; switching it Off leaves the last shapes in place.
- **Random Project Scale.** Press `SHIFT + PERF` repeatedly to audition other musical scales; `SHIFT + PATT` returns to the stored scale. The override does not rewrite steps or the saved Project scale.
- **Reversible Project scale changes.** Changing the Project scale leaves stored Note steps intact. Returning to a scale restores the same pitches if those steps were not edited; the Project root stays fixed across musical scales.
- **Removed track types.** Stochastic and Logic Tracks are no longer available in VINX|FORMER ID.
- **Removed controller paths.** Launch Control XL support and external 16-step editing are gone. The supported controller profiles are Launchpad Pro Mk1 and Launchpad Mini Mk3.
- **Removed simulator releases.** VINX|FORMER ID ships neither a desktop nor a web simulator; simulator code remains available for development tests. The v0.1.9 desktop build identifies itself from the firmware version.

## If you come from Mebitek

VINX|FORMER ID retains the [Vinx PER|FORMER](https://github.com/VinxScorza/performer) workflow changes built on the [Mebitek fork](https://github.com/mebitek/performer), alongside the ID changes above:

- **Generate with an explicit commit.** Random, Acid, Chaos, and Euclidean use source/preview comparison and Apply or Cancel. Acid can write one layer or a Gate + Note + Slide phrase; Chaos can target a sequence or the current pattern.
- **Edit without losing step structure.** Selected-step Copy, Paste, and Duplicate preserve selection gaps and extend Last Step when needed. Tied Note steps share pitch edits; Curve steps have separate Gate Offset and Gate Length controls.
- **Change playback without rewriting the sequence.** The Fill divisor override temporarily changes an independent track's playback division. Its stored divisor remains intact. Note, Curve, and Arp tracks can output a sustained Gate or a Trigger with a configurable length.
- **Control incoming MIDI.** Project filters can independently accept Notes, CC, Program Change, Pitch Bend, and Aftertouch; disabling Notes still allows Note Off to release active notes.
- **Work from Launchpad.** Pro Mk1 and Mini Mk3 expose a track-aware Generators Mode for selection, preview, reroll, cancel, and apply. ID keeps those two profiles only.

## What remains from Mebitek and Westlicht

**From [Westlicht PER|FORMER](https://github.com/westlicht/performer):**

- Eight tracks with up to 64 steps per sequence, 16 patterns, Song, and a temporary pattern snapshot.
- Note, Curve, and MIDI/CV track engines; eight CV/Gate output pairs, MIDI, clocking, scales, and project storage.
- CV/MIDI routing, step conditions, probability, and the core panel sequencing workflow.

**From [Mebitek PER|FORMER](https://github.com/mebitek/performer/blob/master/CHANGELOG.md):**

- Arpeggiator Track, pattern follow, and quick pattern chaining.
- Sequence save/load slots, selected-step movement and undo, and expanded Curve CV recording.
- Launchpad performance controls and panel shortcuts to Project, Routing, MIDI Output, and User Scale pages.

See the [VINX|FORMER ID changelog](doc/ID_CHANGELOG.md), [v0.1.9 release notes](doc/RELEASE_0.1.9.md), and [technical report](doc/VINXFORMER.md) for version history, installation, controls, and measured memory use.

## Project compatibility

**Back up the SD card before testing this firmware.** VINX|FORMER ID rejects projects containing Stochastic or Logic Tracks and resets the loaded state to the default project. It does not overwrite the rejected file. Use Vinx PER|FORMER to edit or export those tracks. Projects saved by v0.1.9 use the same `Version43` format as v0.1.7; older ID or Vinx PER|FORMER builds may not open them. `Version41` and `Version42` projects still load, with all modulators initially Off.

## Build

Clone with submodules:

```sh
git clone --recursive https://github.com/vinxperformer/vinxformer-id.git
cd vinxformer-id
```

The STM32 build uses ARM GCC 6.3.1. Install the project toolchain and configure a release build with `make setup_stm32`, then run `cmake --build build/stm32/release --parallel 2`. The generated `UPDATE.DAT` is the bootloader update file. A build and software tests do not establish hardware compatibility.

The original [Westlicht build documentation](https://github.com/westlicht/performer#development) describes the inherited platform and toolchain. The local [site check](scripts/check_docs_ci.py) validates the single-page VINX|FORMER ID website.

## License

MIT. See [LICENSE](LICENSE) and the notices in the source tree. Original PER|FORMER copyright remains with Simon Kallweit; fork-specific work includes Vinx Scorza's contributions.
