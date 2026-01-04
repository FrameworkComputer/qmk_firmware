# Xavier's fork of the Framework QMK Firmware

This is a keyboard firmware based on the [Framework's fork of QMK firmware](https://github.com/FrameworkComputer/qmk_firmware/tree/fl16-bisect).

Currently, it is based on the FL16-bisect branch.

### Pre-compiled firmware download

You *can* compile the binary yourself. But you can also not.
Compiled binary is available at folder "[UF2](/UF2)".

### Notable Changes
1. Disables Via (disables any functionality through the Framework online interface)
2. Keyboard specific:
   * Numpad: Backlight breathing toggle now go to "`Esc` + `=`"
   * Numpad: Backlight brightness toggle now go to "`Esc` + `⟵`"

## Documentation

For QMK help, [see the official documentation on docs.qmk.fm](https://docs.qmk.fm).

To enter bootloader mode, on:
Large keyboards: Press `Left Alt` and `Right Alt` while turning on.
Numpad/RGB numpad: Press "`1`" and "`6`" at the same time.

To upload firmware, copy and paste any `.uf2` binary into the USB drive. Once done, the device will restart.

If you are switching over from mainline framework firmware, you must clear the flash first.



### Clearing the flash

The first time you flash the firmware, you must first uplaod the `EraseFlash.uf2`. This will take a while. This is because the previous Via-enabled firmware will write a key map onto emulated EEPROM, overriding the mapping of our modified firmware.

After clearing the flash, the keyboard(s) will appear in bootloader mode.

## Supported Keyboards

* [Framework ANSI](/keyboards/framework/ansi)
* [Framework ANSI with CoPilot](/keyboards/framework/copilot)
* [Framework ISO](/keyboards/framework/iso)
* [Framework JIS](/keyboards/framework/jis)
* [Framework macropad](/keyboards/framework/macropad)
* [Framework Numpad](/keyboards/framework/numpad)

Support for all other community keyboards are removed; this is only for the Framework 16. For others, see [Main QMK Github](https://github.com/qmk/qmk_firmware)

## Maintainers

QMK Framework is maintained by [John A Zoidberg](https://github.com/JohnAZoidberg). This branch is maintained by me.

## Official Website

[Custom Numpad 0.3.2 on Framework Community](https://community.frame.work/t/custom-numpad-0-2-8-firmware-compiling-fw16-bisect/79364/1)

# Known Issues

If you ever encountered a build issue saying 
```
⚠ "git describe --abbrev=6 --dirty --always --tags" returned error code 128
⚠ "git describe --abbrev=6 --dirty --always --tags" returned error code 128
builddefs/build_keyboard.mk:62: *** multiple target patterns.  Stop.
```
QMK have known issue of "forgetting" the correct config. See [issue](https://github.com/qmk/qmk_firmware/issues/249660 on QMK Firmware.

## Current QMK Environment:
```
Ψ QMK Doctor is checking your environment.
Ψ CLI version: 1.1.6
Ψ QMK home: /mnt/Volume_A1/qmk_firmware
Ψ Detected Linux (Ubuntu 24.04.3 LTS).
Ψ Git branch: fl16-bisect
Ψ Repo version: v0.3.0
⚠ Git has unstashed/uncommitted changes.
⚠ The official repository does not seem to be configured as git remote "upstream".
Ψ All dependencies are installed.
Ψ Found arm-none-eabi-gcc version 13.2.1
Ψ Found avr-gcc version 7.3.0
Ψ Found avrdude version 7.1
Ψ Found dfu-programmer version 0.6.1
Ψ Found dfu-util version 0.11
Ψ Submodules are up to date.
Ψ Submodule status:
Ψ - lib/chibios: 2023-04-15 13:48:04 +0000 --  (11edb16)
Ψ - lib/chibios-contrib: 2023-07-17 11:39:05 +0200 --  (da78eb3)
Ψ - lib/googletest: 2021-06-11 06:37:43 -0700 --  (e2239ee)
Ψ - lib/lufa: 2022-08-26 12:09:55 +1000 --  (549b973)
Ψ - lib/vusb: 2022-06-13 09:18:17 +1000 --  (819dbc1)
Ψ - lib/printf: 2022-06-29 23:59:58 +0300 --  (c2e3b4e)
Ψ - lib/pico-sdk: 2023-02-12 20:19:37 +0100 --  (a3398d8)
Ψ - lib/lvgl: 2022-04-11 04:44:53 -0600 --  (e19410f)
Ψ QMK is ready to go, but minor problems were found
```
