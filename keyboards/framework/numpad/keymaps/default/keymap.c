// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
     *         ┌────┬────┬────┬────┐
     *  4 keys │Esc │Calc│ =  │ <- │
     *         ├────┼────┼────┼────┤
     *  4 keys │ Num│ /  │ *  │ -  │
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
        KC_ESC,  KC_CALC, KC_EQL,  KC_BSPC,
        TG(1),   KC_PSLS, KC_PAST, KC_PMNS,
        KC_P7,   KC_P8,   KC_P9,
        KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
        KC_P1,   KC_P2,   KC_P3,
            KC_P0,        KC_PDOT, KC_PENT
    ),
     /*
     *         ┌────┬────┬────┬────┐
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  3 keys │Home│ ↑  │PgUp│    │
     *         ├────┼────┼────┤    │
     *  4 keys │ ←  │    │ →  │    │
     *         ├────┼────┼────┼────┤
     *  3 keys │End │ ↓  │PdDn│    │
     *         ├────┴────┼────┤    │
     *  3 keys │ Insert  │Del │    │
     *         └─────────┴────┴────┴
     * 21 total
     */
    [1] = LAYOUT(
        _______, _______, _______, _______,
        _______, ______,  _______, _______,
        KC_HOME, KC_UP,   KC_PGUP,
        KC_LEFT, _______, KC_RGHT, _______,
        KC_END,  KC_DOWN, KC_PGDN,
            KC_INS,       KC_DEL,  _______

    )

};
