// Copyright 2022-2023 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

#if KEYBOARD_framework_numpad
#    include "numpad.h"
#endif

enum framework_keycodes {
    // Function lock
    FN_LOCK = QK_KB,
};

extern bool bios_mode;

#define SLEEP_GPIO GP0
#define MUX_ENABLE_GPIO GP4
#define BOOT_DONE_GPIO GP5
// Pin SDB to enable the RGB controller
#ifdef PICO_FL16
// 22 only on RPi Pico because it doesn't have GP29
#    define IS31FL3743A_ENABLE_GPIO GP22
#else
#    define IS31FL3743A_ENABLE_GPIO GP29
#endif
