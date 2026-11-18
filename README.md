# VINX|FORMER ID

**Personal performance firmware. Built for one. Shared with everyone.**

VINX|FORMER ID is Vinx Scorza's experimental fork of [Vinx PER|FORMER](https://github.com/VinxScorza/performer), itself based on the [Mebitek](https://github.com/mebitek/performer) and [Westlicht](https://github.com/westlicht/performer) PER|FORMER projects. VINX|FORMER ID and Vinx PER|FORMER are separate projects. The [VINX|FORMER ID website](https://vinxperformer.github.io/vinxformer-id/) explains the direction of this fork; the [firmware hub](https://vinxperformer.github.io/) links both projects.

This local source identifies as **VINX|FORMER ID v0.1.6**. The [v0.1.1 firmware release](https://github.com/vinxperformer/vinxformer-id/releases/tag/v0.1.1) remains the latest published release. Hardware installation and Launchpad behavior of v0.1.6 have not been verified. The source base is Vinx PER|FORMER `v0.4.6-beta.2` (`b1e95b58`).

## What changes in VINX|FORMER ID compared with Vinx PER|FORMER

- Stochastic and Logic Tracks are removed from the interface and runtime.
- Only Launchpad Pro Mk1 and Launchpad Mini Mk3 controller profiles are supported. External 16-step editing and Launch Control XL support are removed.
- Permutation rerolls timing and pitch on one or more selected Note tracks. Each generation starts from the original, with a reversible Original/Permuted preview. MOVE and PITCH default to ±3.
- On Curve tracks, Track → Random Shape writes a new shape into every active step at the start of each loop of that Curve track, using the full shape catalog. Musical measure boundaries do not restart its loop. The new shapes are visible in the sequencer. Turning the option off keeps the latest shapes; saving the project preserves both those shapes and each Curve track's On/Off setting.
- `SHIFT + PERF` chooses another Project musical scale; repeated presses choose again. `SHIFT + PATT` restores the stored Project scale.
- Changing the Project scale does not rewrite sequence notes. Returning to a scale restores the same notes when the sequence has not been edited in the meantime. The Project root is retained across musical scales.
- No desktop or web simulator is distributed for VINX|FORMER ID. Simulator code remains in the repository for development tests.

See the [VINX|FORMER ID changelog](doc/ID_CHANGELOG.md), [v0.1.6 local update notes](doc/RELEASE_0.1.6.md), and [technical report](doc/VINXFORMER.md) for version history, installation, controls, and measured memory use.

## Project compatibility

**Back up the SD card before testing this firmware.** VINX|FORMER ID rejects projects containing Stochastic or Logic Tracks and resets the loaded state to the default project. It does not overwrite the rejected file. Use Vinx PER|FORMER to edit or export those tracks. Projects saved by v0.1.6 use the `Version42` format; older ID or Vinx PER|FORMER builds may not open them. Earlier `Version41` projects still load, with Random Shape initially Off.

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
