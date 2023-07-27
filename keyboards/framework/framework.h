// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

#ifdef KEYBOARD_framework_ansi
    #include "ansi.h"
#elif KEYBOARD_framework_iso
    #include "iso.h"
#elif KEYBOARD_framework_jis
    #include "jis.h"
#elif KEYBOARD_framework_numpad
    #include "numpad.h"
#elif KEYBOARD_framework_gridpad
    #include "gridpad.h"
#endif

enum framework_keycodes {
  // Custom keycode to change screen modes (e.g. enable external screen)
  KC_SCRN = SAFE_RANGE,
  FN_LOCK,
  STORE_SETUPS,
  PRINT_SETUPS,
};

#define SLEEP_GPIO GP0
#define MUX_ENABLE_GPIO GP4
#define BOOT_DONE_GPIO GP5
// Pin SDB to enable the RGB controller
#ifdef PICO_FL16
// 22 only on RPi Pico because it doesn't have GP29
#define IS31FL3743A_ENABLE_GPIO GP22
#else
#define IS31FL3743A_ENABLE_GPIO GP29
#endif
