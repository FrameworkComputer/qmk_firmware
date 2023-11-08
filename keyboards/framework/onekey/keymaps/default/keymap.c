// Copyright 2022-2023 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "onekey.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
     *         ┌────┬────┬────┐
     *  3 keys │ 7  │ 8  │ 9  │
     *         ├────┼────┼────┤
     *  3 keys │ 4  │ 5  │ 6  │
     *         ├────┼────┼────┤
     *  3 keys │ 1  │ 2  │ 3  │
     *         └────┴────┴────┘
     *  9 total
     */
    [0] = LAYOUT(
        KC_7,   KC_8,   KC_9,
        KC_4,   KC_5,   KC_6,
        KC_1,   KC_2,   KC_3
    )
};
