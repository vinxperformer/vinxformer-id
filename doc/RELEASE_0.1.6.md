# VINX|FORMER ID v0.1.6

This release follows v0.1.1. It adds **Random Shape** to Curve tracks and supersedes the local v0.1.2–v0.1.5 DAT files.

## Random Shape

On a Curve track, set **Track → Random Shape → On**. At the start of each loop of that track, the firmware chooses a new Shape from the full Curve catalog for every active step of the current pattern. The new shapes appear in the sequencer. Run Mode stays as selected. Forward, Backward, Pendulum and PingPong use the track's loop boundary; Random and Random Walk use the active First–Last span of step advances. A musical measure boundary alone does not reroll the shapes. A 64-step Forward loop with Last Step 64 and Reset Measure off rerolls after 64 steps, including with Divisor 1/4.

Only Shape changes. Min, Max, Gate, Shape Variation, probability, and the other step fields are not edited. Shape Variation and Fill can still override the heard shape on individual steps. Turning Random Shape Off leaves the last generated shapes in the sequencer.

## Projects and installation

Random Shape On/Off is saved **per Curve track**. If saved On, it returns On when the project is loaded and generates new shapes when playback reaches the start of the loop. Existing `Version41` projects load with Random Shape Off. Saving in v0.1.6 writes `Version42`; older ID or Vinx PER|FORMER firmware may not reopen that saved copy. Back up the SD card and projects before saving with this build.

Download `UPDATE.DAT` from the [v0.1.6 release assets](https://github.com/vinxperformer/vinxformer-id/releases/tag/v0.1.6), copy it to the SD card, and use the module's normal bootloader update procedure. The image identifies as **VINX|FORMER ID v0.1.6**.

- DAT: 634,836 bytes; firmware payload: 634,820 bytes.
- SHA-256: `9f2be3dec4294d370c7140035d29c387f50ab60e9c18f2a9effe15c5fefa9827`.
- Payload MD5 footer: verified.
- STM32 build and project save/load tests: passed, including `Version41` fallback to Off.

The DAT is attached to the GitHub release and is not included in Git. The [technical report](VINXFORMER.md) records implementation details and measured memory use.
