# VINX|FORMER ID

**Personal performance firmware. Built for one. Shared with everyone.**

VINX|FORMER ID is Vinx Scorza's experimental fork of [Vinx PER|FORMER](https://github.com/VinxScorza/performer), itself based on the [Mebitek](https://github.com/mebitek/performer) and [Westlicht](https://github.com/westlicht/performer) PER|FORMER projects. ID and Vinx PER|FORMER are separate projects. The [ID website](https://vinxperformer.github.io/vinxformer-id/) explains the direction of this fork; the [firmware hub](https://vinxperformer.github.io/) links both projects.

This source identifies as **VINX|FORMER ID v0.1.1**. The [v0.1.1 firmware prerelease](https://github.com/vinxperformer/vinxformer-id/releases/tag/v0.1.1) is experimental: hardware installation and Launchpad behavior are not yet verified. The source base is Vinx PER|FORMER `v0.4.6-beta.2` (`b1e95b58`).

## What changes in ID

- Stochastic and Logic Tracks are removed from the interface and runtime.
- Only Launchpad Pro Mk1 and Launchpad Mini Mk3 controller profiles are supported. External 16-step editing and Launch Control XL support are removed.
- Permutation rerolls timing and pitch on one or more selected Note tracks. Each generation starts from the original, with a reversible Original/Permuted preview. MOVE and PITCH default to ±3.
- `SHIFT + PERF` chooses another Project musical scale; repeated presses choose again. `SHIFT + PATT` restores the stored Project scale.
- Changing the Project scale does not rewrite sequence notes. Returning to a scale restores the same notes when the sequence has not been edited in the meantime. The Project root is retained across musical scales.
- No desktop or web simulator is distributed for ID. Simulator code remains in the repository for development tests.

See [the ID technical report](doc/VINXFORMER.md) for the controls, measured memory use, and implementation details.

## Project compatibility

**Back up the SD card before testing this firmware.** ID rejects projects containing Stochastic or Logic Tracks and resets the loaded state to the default project. It does not overwrite the rejected file. Use Vinx PER|FORMER to edit or export those tracks. Projects using the retained track modes continue to use the `Version41` file format; older firmware versions may not open files saved in that format.

## Build

Clone with submodules:

```sh
git clone --recursive https://github.com/vinxperformer/vinxformer-id.git
cd vinxformer-id
```

The STM32 build uses ARM GCC 6.3.1. Install the project toolchain and configure a release build with `make setup_stm32`, then run `cmake --build build/stm32/release --parallel 2`. The generated `UPDATE.DAT` is the bootloader update file. A build and software tests do not establish hardware compatibility.

The original [Westlicht build documentation](https://github.com/westlicht/performer#development) describes the inherited platform and toolchain. The local [site check](scripts/check_docs_ci.py) validates the single-page ID website.

## License

MIT. See [LICENSE](LICENSE) and the notices in the source tree. Original PER|FORMER copyright remains with Simon Kallweit; fork-specific work includes Vinx Scorza's contributions.
