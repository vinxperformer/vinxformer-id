# Vinx|former track removal and memory report

This local development branch starts from Vinx PER|FORMER `v0.4.6-beta.2` (firmware source at commit `b1e95b58`). Stochastic Track and Logic Track are no longer selectable. Their runtime engines, sequence pages, generator branches, clipboard actions, and Launchpad UI branches have been removed. The Note track's Logic connection controls are hidden.

The firmware identity is `VINX|FORMER ID v0.1.1`. The startup screen adapts the supplied [full-color mockup](branding/README.md) to the module's grayscale 256 × 64 display. System → Update shows the same identity. The numeric version fields in the image tag are `0.1.1`; these are separate from the project file format version.

The resident bootloader may append `(0.1.1)` to the identity while displaying the installed or pending update. That formatting is part of the bootloader already on the module and is not replaced by the sequencer `UPDATE.DAT`.

The legacy mode identifiers and serialization code remain so that old project data is read without mistaking another mode for it. A project containing either removed track mode is rejected and reset to the default project; the file on the SD card is not rewritten by that failed load. Back up projects before testing this build. Projects containing only Note, Curve, MIDI/CV, and Arp tracks remain on the `Version41` format. The original firmware is needed to edit or export removed tracks.

## Measured STM32 memory

Both builds used the same ARM GCC 6.3.1 toolchain, Release configuration, STM32 target, and linked `sequencer` target. The baseline uses the unmodified source at `b1e95b58`; the branch is compared against it with `arm-none-eabi-size`. Measurements are build artifact sizes, not a hardware test.

| Build | Flash (`text + data`) | RAM (`data + bss`) |
| --- | ---: | ---: |
| Baseline | 760,056 bytes | 165,708 bytes |
| VINX|FORMER ID v0.1.1 after external 16-step mode removal | 633,744 bytes | 161,680 bytes |

The branch frees **126,312 bytes of flash** and **4,028 bytes of RAM** against the baseline. Removing the external 16-step mode saved **5,176 bytes of flash** and **4 bytes of RAM** compared with the previous VINX|FORMER ID v0.1.1 build. The RAM saving is in CCM RAM. These figures include the new version string, compatibility guard, controller pruning, Permutation, and Random Scale.

The `.bss` change is entirely in CCM RAM; the regular RAM `.bss` stays at 111,528 bytes. The linker script allocates 960 KiB of ROM, 128 KiB of regular RAM, and 64 KiB of CCM RAM to this application.

The source is compiled from a temporary path without spaces because the bundled `libopencm3` makefiles do not handle the workspace path's spaces. The temporary build copy is not the deliverable; changes live in this repository.

## Controllers

The firmware retains Launchpad Pro Mk1 and Launchpad Mini Mk3 controller profiles. The external 16-step editing mode and its Launch Control XL controller profile have been removed. VINX|FORMER ID does not distribute desktop or web simulators; the source still contains a simulator test harness, including a Launchpad Mk2 adapter for existing UI regression tests.

## VINX|FORMER ID site

The `docs/` directory now contains a single-page VINX|FORMER ID statement based on the supplied color and copy reference. It gives only a short account of the differences from Vinx PER|FORMER. The inherited manual, simulator website, controller templates, and BeatStep Pro script are not part of the VINX|FORMER ID site. The original Vinx PER|FORMER repository and site remain separate.

## Pattern snapshot

On the Pattern page, F3 (`SNAP`) copies the currently playing pattern of each of the eight tracks into a temporary snapshot slot and switches playback to those copies. Edit or perform with the copies while the original patterns stay intact. F4 (`COMMIT`) writes the copies back to their original pattern slots; F3 (`REVERT`) discards them and returns to the originals. While a snapshot is active, hold S1–S16 and press F4 to commit all eight copies to that numbered pattern, or hold S1–S16 and press F3 to discard the copies and switch to that numbered pattern. Snapshot state is temporary and is not serialized to the project file.

## Permutation generator

Permutation is available on Note tracks from the Generators chooser and from GRID 5 in Launchpad Generators Mode on Launchpad Pro Mk1 and Launchpad Mini Mk3. The generator targets the currently selected pattern number on each chosen Note track, starting with the selected track only. Press T1–T8 on the module or the eight Launchpad scene buttons to toggle Note tracks without changing the editing track. Other track types cannot be selected.

The Steps overlay in Launchpad Generators Mode labels GRID 5 as `PERM`.

MOVE sets the maximum displacement of a step swap (1–8 steps), PITCH sets the maximum random pitch change (0–12 scale steps), and AMOUNT sets the chance of moving each eligible event (0–100%). MOVE and PITCH both default to ±3; AMOUNT defaults to 50%. Pitch steps follow each sequence's active scale: they are semitones only in a chromatic scale. Events that are tied to another step stay in place. Each step participates in at most one swap, and swaps stay inside its track's active first-to-last-step range, so the number of gated events is preserved.

The page starts on ORIGINAL. Use PREV/NEXT to choose a parameter and turn the encoder to edit it. F3 or encoder press generates NEW PERM; F1 compares ORIGINAL and PERMUTED; F4 cancels and restores all selected tracks; F5 applies the preview to all selected tracks. On Launchpad, TOP 5 compares, TOP 6 generates, TOP 7 cancels, and TOP 8 applies. Changing a parameter or the track selection invalidates the current preview and requires another NEW PERM. Each generation starts from the original captured when the page opened, rather than compounding earlier previews.

The STM32 and simulator builds and focused unit/UI tests pass. Hardware behavior on the two Launchpads is not yet verified.

## Project scale changes

Changing the Project scale now changes how stored note degrees are interpreted during playback without rewriting sequence steps. Previously, switching between chromatic scales converted every note in Note sequences using the Project scale to the nearest degree of the destination scale. Rounding and clamping made that conversion lossy, so a return to the initial scale could not restore the original notes. With this change, switching away and back restores the same note data and pitches, provided the sequence was not edited meanwhile. Sequence-specific scale changes still use their existing conversion behavior.

The Project root note is preserved across scale changes. Note and Arp playback now apply that root to all musical scales, including 5/7/19/22/24-tet. Previously, non-chromatic TET scales ignored the root, so a D root sounded as C at degree zero. Built-in and user Voltage scales retain their CV-oriented behavior and do not add a musical root offset. A sequence with its own root-note setting continues to use that override instead of the Project root.

## Random Project Scale

Hold SHIFT and press PERF from any page to choose another musical preset scale. Repeat for further choices; each press avoids the currently effective scale. Hold SHIFT and press PATT to return to the original Project scale, even after multiple random choices. Neither shortcut opens its usual page. The shortcuts show the chosen/restored scale briefly on the display.

Random Scale is a runtime override. It leaves the stored Project scale, root note, and sequence steps unchanged. Project saves write the original stored scale, and loading or clearing a project cancels the override. Selecting a scale manually on the Project page replaces the stored scale and cancels the override. The random pool contains the 19 built-in musical scales from Semitones through 24-tet; Voltage and User Scales are excluded.
