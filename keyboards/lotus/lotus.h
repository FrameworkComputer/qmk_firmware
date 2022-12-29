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
