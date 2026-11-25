# VINX|FORMER ID v0.1.9

This release adds short held notes to Acid generation and makes the manual tie command visible in Note Steps. It includes the local v0.1.8 work and follows the public v0.1.7 release.

## Acid held notes

Acid Phrase and Acid Eucl Phrase can generate same-pitch ties without adding Slide. In the generator context menu, TIE controls the chance that an eligible onset starts a held group (0% Off, default 25%). When adjacent steps are available, 90% of groups use 2–3 steps and 10% use 4–5; two-step groups are favored. The preview displays the joins before Apply.

## Manual tie toggle

In a Note track's Steps page, hold SHIFT to select at least two steps, then release the step pads. Hold SHIFT and press the **third function key (F3)**. The selected span becomes one held pitch. Press the same shortcut again to remove the ties; the pitches stay as edited and intermediate gates return to half-step length. While SHIFT is held with two or more steps selected, the third footer label reads TIE or UNTIE according to the current span. Otherwise it reads LENGTH.

## Projects and installation

The project format remains `Version43`, as in v0.1.7. Projects saved by this version may not open in older firmware. Back up the SD card and projects before updating. Copy the attached `UPDATE.DAT` to the SD-card root and use the normal bootloader update procedure. The firmware identifies as **VINX|FORMER ID v0.1.9**.

- DAT: 645,792 bytes; firmware payload: 645,776 bytes.
- SHA-256: `7b362bf270a4c7c6d30e0ef7058d2945416c8f894a654746dc9ee5a950ede72a`.
- Payload MD5 footer: verified.
- STM32 Release build, desktop simulator build, focused unit tests, and virtual Note Steps smoke test: passed.
- Hardware installation and button behavior: not yet verified.

The [technical report](VINXFORMER.md) records the measured memory use. The desktop simulator remains a development tool and is not part of the release assets.
