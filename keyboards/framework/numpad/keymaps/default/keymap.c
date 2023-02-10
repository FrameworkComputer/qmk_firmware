// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#define NUM_FN LT(0,KC_C)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
     *         ┌────┬────┬────┬────┐
     *  4 keys │Esc │Calc│ =  │ <- │
     *         ├────┼────┼────┼────┤
     *  4 keys │Num │ /  │ *  │ -  │
     *         ├────┼────┼────┼────┤
     *  3 keys │ 7  │ 8  │ 9  │    │
     *         ├────┼────┼────┤    │
     *  4 keys │ 4  │ 5  │ 6  │ +  │
     *         ├────┼────┼────┼────┤
     *  3 keys │ 1  │ 2  │ 3  │    │
     *         ├────┴────┼────┤    │
     *  3 keys │ 0       │ .  │Entr│
     *         └─────────┴────┴────┴
     * 21 total
     */
    [0] = LAYOUT(
        KC_ESC,  KC_CALC, KC_EQL,  KC_DEL,
        NUM_FN,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_P7,   KC_P8,   KC_P9,
        KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
        KC_P1,   KC_P2,   KC_P3,
            KC_P0,        KC_PDOT, KC_PENT
    ),
    [PAD_FN] = LAYOUT(
        _______, _______, _______, _______,
        _______,   BL_UP, _______, _______,
         BL_OFF, BL_BRTG,   BL_ON,
        _______, BL_DOWN, _______, _______,
        _______, _______, _______,
            _______,      _______, _______
    ),
};
