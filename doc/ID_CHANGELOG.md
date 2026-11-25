# VINX|FORMER ID changelog

This is the history of VINX|FORMER ID. The inherited Vinx PER|FORMER history remains in [CHANGELOG.md](../CHANGELOG.md). Versions v0.1.2–v0.1.5 were local builds.

## v0.1.9 — release, 25 November 2026

- In Note Steps, holding SHIFT with two or more selected steps changes the third function key label from LENGTH to TIE or UNTIE. The label follows the actual state of the selected range; the shortcut remains SHIFT + F3. The normal LENGTH label returns when SHIFT is released.
- Update the desktop development simulator title to use the firmware identity and version.
- [Release notes](RELEASE_0.1.9.md).

## v0.1.8 — local build, 24 November 2026

- Acid Phrase and Acid Eucl Phrase can generate same-pitch held notes. The context menu's TIE setting controls how often a gated onset starts a held group (Off at 0%; default 25%). Within available adjacent steps, 90% of groups are 2–3 steps and 10% are 4–5; two-step groups are favored. These ties do not add Slide.
- Tied steps keep the gate through the next step boundary even when track length bias would shorten it. The generator preview joins held steps visually.
- In Note Steps, SHIFT + F3 (LENGTH, third function key) toggles a held note across the range from the first to the last selected step. A second press removes the ties and gives the intermediate steps the normal half-step gate length; the pitches stay as edited.

## v0.1.7 — release, 19 November 2026

- Add eight modulators adapted from [modulove/performer](https://github.com/modulove/performer), with fixed Mod 1→CV 1 through Mod 8→CV 8 assignment, individual On/Off, and Off by default.
- Keep modulove's Sine, Triangle, Saw Up, Saw Down, Square, Random and ADSR shapes and their controls. Add selectable Curve-style unipolar or bipolar 1–5 V ranges. Gate outputs and patterns remain independent.
- Open the page with PAGE + Track 6. Save modulator settings in `Version43` projects; older projects load with all modulators Off.
- Make ADSR timing follow the actual tempo and fix release to start from the level reached when the gate fell.
- [Installation and verification notes](RELEASE_0.1.7.md).

## v0.1.6 — release, 18 November 2026

- Save Random Shape On/Off separately for each Curve track in the project.
- Read older `Version41` projects with Random Shape Off; saving now writes `Version42`. Older firmware may not reopen the newly saved project.
- Keep the v0.1.5 loop behavior: new shapes are written into the active steps at the start of the Curve track's loop.
- [Installation and verification notes](RELEASE_0.1.6.md).

## v0.1.5 — local build

- Stop unrelated musical measure updates from restarting Random Shape's cycle. A 64-step Curve loop with Reset Measure off keeps its shapes through step 16 and changes at the actual loop boundary.

## v0.1.4 — local build

- Follow the Curve track's own loop boundary in Forward, Backward, Pendulum and PingPong, including when Random Shape is enabled partway through a loop. Random and Random Walk use the active First–Last span as a cycle.

## v0.1.3 — local build

- Write generated Shape values into the active steps so they appear in the sequencer. Switching Random Shape Off leaves the latest generated shapes in place.

## v0.1.2 — local build, superseded

- Introduced per-track Random Shape using the full Curve shape catalog. This first implementation affected playback without writing the shapes into the sequencer; v0.1.3 corrected that behavior.

## v0.1.1 — published release

- First published VINX|FORMER ID build. See its [release notes](RELEASE_0.1.1.md) for the differences from Vinx PER|FORMER.
