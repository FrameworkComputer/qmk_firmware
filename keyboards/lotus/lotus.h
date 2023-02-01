// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

#ifdef KEYBOARD_lotus_ansi
    #include "ansi.h"
#elif KEYBOARD_lotus_iso
    #include "iso.h"
#elif KEYBOARD_lotus_jis
    #include "jis.h"
#elif KEYBOARD_lotus_numpad
    #include "numpad.h"
#elif KEYBOARD_lotus_gridpad
    #include "gridpad.h"
#endif

enum lotus_keycodes {
  // Custom keycode to change screen modes (e.g. enable external screen)
  KC_SCRN = SAFE_RANGE,
};

#define SLEEP_GPIO GP0
#define MUX_ENABLE_GPIO GP4
#define BOOT_DONE_GPIO GP5
// Pin SDB to enable the RGB controller
#ifdef PICO_LOTUS
// 22 only on RPi Pico because it doesn't have GP29
#define IS31FL3743A_ENABLE_GPIO GP22
#else
#define IS31FL3743A_ENABLE_GPIO GP29
#endif

void *lotus_serial_number_string(void);
uint16_t lotus_serial_number_string_len(void);
// The serial number is written to the last 4K block in the first 1M of flash
#define FLASH_OFFSET 0x10000000
#define LAST_4K_BLOCK 0xff000
#define SERIALNUM_LEN 18

// 18-digit serial number. Must be programmed at factory
// TODO: Figure out how to do that. Probably a script to compile qmk
// Must be wide-string (UTF-16)
// LDL = Keyboard White Backlight
#define FALLBACK_SERIAL_NUMBER L"FRALDLENA100000000"
_Static_assert(sizeof(FALLBACK_SERIAL_NUMBER) == (SERIALNUM_LEN+1)*2, "Fallback serial number must be same length");
