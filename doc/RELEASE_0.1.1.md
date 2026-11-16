# VINX|FORMER ID v0.1.1 — experimental firmware

**Built for one. Shared with everyone.**

## Compared with Vinx PER|FORMER

- Stochastic and Logic Tracks have been removed.
- The supported controller profiles are Launchpad Pro Mk1 and Launchpad Mini Mk3. Launch Control XL support and the external 16-step editing mode have been removed.
- The new Permutation generator can work on multiple selected Note tracks. Each generation rerolls step position and pitch from the original sequence; Original/Permuted preview lets you compare before applying. MOVE and PITCH default to ±3.
- `SHIFT + PERF` chooses another Project musical scale. Repeated presses choose again; `SHIFT + PATT` returns to the stored Project scale.
- Project scale changes no longer rewrite sequence notes. Returning to a scale restores the same notes, provided the sequence has not been edited meanwhile. The Project root stays the same across musical scales.
- No desktop or web simulator is distributed with VINX|FORMER ID.

## Before installing

Back up your SD card and projects. VINX|FORMER ID rejects projects containing Stochastic or Logic Tracks and resets the loaded state to the default project; the rejected file on the SD card is not overwritten. Use Vinx PER|FORMER to edit or export those tracks. Other supported track modes continue to use the `Version41` project format.

Copy `UPDATE.DAT` to the SD card and use the module's normal bootloader update procedure. This DAT identifies as `VINX|FORMER ID v0.1.1`.

- `UPDATE.DAT`: 633,760 bytes
- SHA-256: `9eeb06c3faed59861d944465dce2483eda06d20ea3aa26afd8d146937a0d4bf8`
- Firmware payload MD5 footer: verified

The STM32 build and focused software tests passed. The release DAT matches the current sequencer source snapshot. Hardware installation, SD-card behavior, and the two Launchpads remain unverified.

More detail: [VINX|FORMER ID technical report](https://github.com/vinxperformer/vinxformer-id/blob/main/doc/VINXFORMER.md).
