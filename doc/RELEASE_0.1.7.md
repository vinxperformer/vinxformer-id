# VINX|FORMER ID v0.1.7

This update adds eight independent CV modulators to VINX|FORMER ID. The modulator design and waveform engine are adapted from [modulove/performer](https://github.com/modulove/performer) (MIT license).

## Using the modulators

Hold **PAGE** and press **Track 6** on the module. Press Track 1–8 to choose Mod 1–8. Each Mod controls the CV output with the same number: Mod 1→CV 1, through Mod 8→CV 8. All eight default **Off**. Turn one On to replace that CV output with its modulation signal; turn it Off to return that output to its existing track assignment. Gate outputs and sequences are unchanged. The 16 patterns and Song remain available.

Choose F1–F5 and turn the encoder to edit a field; Left/Right switches parameter pages. Pressing the encoder while ON/OFF is selected also toggles the Mod. Sine, Triangle, Saw Up, Saw Down, Square, Random and ADSR are available. LFO controls include musical Rate, Depth, Offset, Phase and Free/Sync/Retrigger. Random has Clocked/Triggered modes and Slew. ADSR has Attack, Decay, Sustain, Release and Level. Gate-triggered modes can follow any of the eight tracks.

Each output can use a unipolar 0–1/2/3/4/5 V or bipolar ±1/2/3/4/5 V range; the default is ±1 V. Settings are saved with the project. Older `Version41` and `Version42` projects load with every Mod Off. Saving with v0.1.7 writes `Version43`; an earlier firmware may not open that saved project, so back up the SD card first.

## Installation and verification

Copy the accompanying `UPDATE.DAT` to the SD card and use the module's normal bootloader update procedure. The firmware identifies as **VINX|FORMER ID v0.1.7**.

- DAT size: 642,060 bytes; firmware payload: 642,044 bytes.
- SHA-256: `b5f7a22dc6839f7b6099cb3bf2530ee6b9cb0b1fa51f9fca98f95fb7e040726f`.
- STM32 build and focused project persistence/ADSR tests: passed.
- Physical module and controller behavior: not yet tested.

The [technical report](VINXFORMER.md) records memory use and implementation details.
