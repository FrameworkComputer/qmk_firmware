# Lotus Keyboard

Keyboard input module for Lotus.

* Keyboard Maintainer: [Daniel Schaefer](https://github.com/JohnAZoidberg)
* Hardware Supported: Framework Lotus Keyboard, Raspberry Pi Pico MCU.
* Hardware Availability: ...

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb lotus -km default

Flashing example for this keyboard:

    qmk flash -kb lotus -km default -bl avrdude

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

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
    - [ ] ANSI (78 keys)
    - [ ] ISO (79 keys)
    - [ ] JIS (84 keys)
  - [ ] Keyboard with per-key RGB
  - [ ] Numpad with 1-zone backlight
  - [ ] Numpad with per-key RGB

# Notes

- GPIO
  - Keyboard Column driver pins
    - GPIO8 (Column 0)
    - GPIO9
    - GPIO10
    - GPIO11
    - GPIO12
    - GPIO13
    - GPIO14
    - GPIO15
    - GPIO16
    - GPIO17
    - GPIO18
    - GPIO19
    - GPIO20
    - GPIO21
    - GPIO22
    - GPIO23 (Column 14)
  - SGM48751
    - GPIO1 - Analog Mux Control
    - GPIO2 - Analog Mux Control
    - GPIO3 - Analog Mux Control
    - GPIO4 - Enable
  - RGB Backlight
    - GPIO26 - I2C SDA
    - GPIO27 - I2C SCL