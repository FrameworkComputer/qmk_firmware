# Lotus Keyboard

Keyboard input module for Lotus.

* Keyboard Maintainer: [Daniel Schaefer](https://github.com/JohnAZoidberg)
* Hardware Supported: Framework Lotus Keyboard, Raspberry Pi Pico MCU.
* Hardware Availability: Use Raspberry Pi Pico for now

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb lotus -km 78_ansi
    qmk compile -kb lotus -km 79_iso
    qmk compile -kb lotus -km 83_jis

Flashing example for this keyboard:

    qmk compile -kb lotus -km 78_ansi
    qmk compile -kb lotus -km 79_iso
    qmk compile -kb lotus -km 83_jis

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **On Raspberry Pi Pico**: Hold down bootsel button when plugging in
* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available

## Features

- Variant with per-key RGB and variant with 1-zone white backlight
- NKRO

## TODO

- [ ] Enable backlight with IS31FL3745 (see `docs/feature_rgb_matrix.md`)
- [ ] Split variants
  - [ ] Keyboard with 1-zone backlight
    - [x] ANSI (78 keys)
    - [x] ISO (79 keys)
    - [x] JIS (84 keys)
  - [ ] Keyboard with per-key RGB
  - [ ] Numpad with 1-zone backlight
  - [ ] Numpad with per-key RGB

# Notes

- GPIO
  - Keyboard Column driver pins
    - GPIO8  (Column  0)
    - GPIO9  (Column  1)
    - GPIO10 (Column  2)
    - GPIO11 (Column  3)
    - GPIO12 (Column  4)
    - GPIO13 (Column  5)
    - GPIO14 (Column  6)
    - GPIO15 (Column  7)
    - GPIO16 (Column 13)
    - GPIO17 (Column 12)
    - GPIO18 (Column 11)
    - GPIO19 (Column 10)
    - GPIO20 (Column  9)
    - GPIO21 (Column  8)
    - GPIO22 (Column 15)
    - GPIO23 (Column 14)
  - SGM48751
    - GPIO1 - Analog Mux Control
    - GPIO2 - Analog Mux Control
    - GPIO3 - Analog Mux Control
    - GPIO4 - Enable
  - RGB Backlight
    - GPIO26 - I2C SDA
    - GPIO27 - I2C SCL
