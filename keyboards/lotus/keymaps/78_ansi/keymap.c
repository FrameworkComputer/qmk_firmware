// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │Esc  │F1 │F2 │F3 │F4 │F5 │F6 │F7 │F8 │F9 │F10│F11│F12│ Del│
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┤
     * 14 keys │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │Backsp│
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬────┤
     * 14 keys │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │ \  │
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴────┤
     * 13 keys │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │ Enter │
     *         ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───────┤
     * 12 keys │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │  Shift  │
     *         ├────┬───┼───┼───┼───┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │   │   │   │                   │   │   │    │↑  │    │
     * 11 keys │Ctrl│FN │GUI│Alt│                   │Alt│Ctl│ ←  ├───┤  → │
     *         │    │   │   │   │                   │   │   │    │  ↓│    │
     *         └────┴───┴───┴───┴───────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    [0] = LAYOUT_78_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,
        KC_LCTL, MO(1),   KC_LGUI, KC_LALT,          KC_SPC,                    KC_RALT, KC_RCTL, KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT
    ),
     /*
     * Function layer
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │FN lk│Mut│vDn│vUp│Prv│Ply│Nxt│bDn│bUp│Scn│Air│Prt│App│Ins │
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┤
     * 14 keys │   │   │   │   │   │   │   │   │   │   │   │   │   │      │
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬────┤
     * 14 keys │     │   │   │   │   │   │   │   │   │   │Pau│   │   │    │
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴────┤
     * 13 keys │      │   │SRq│   │   │   │   │   │ScL│   │   │   │       │
     *         ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───────┤
     * 12 keys │        │   │   │   │   │Brk│   │   │   │   │   │         │
     *         ├────┬───┼───┼───┼───┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │   │   │   │                   │   │   │    │PgU│    │
     * 11 keys │    │   │   │   │ Toggle Backlight  │   │   │Home├───┤End │
     *         │    │   │   │   │                   │   │   │    │PgD│    │
     *         └────┴───┴───┴───┴───────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    // TODO: Screen toggle, airplane mode
    // TODO: Other unlabeled keys
    [1] = LAYOUT_78_ansi(
        TG(1), KC_KB_MUTE, KC_KB_VOLUME_DOWN, KC_KB_VOLUME_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_BRID, KC_BRIU, KC_TRNS, KC_TRNS, KC_PSCR, KC_MSEL, KC_INS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PAUS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_SYRQ, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_SCRL, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
        KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_BRK,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          BL_TOGG,                            KC_TRNS, KC_TRNS, KC_HOME, KC_PGUP, KC_PGDN, KC_END
    )
};

// TODO: Need to define the LED matrix.
// TODO: Should have attribute __flash. But won't compile if I add it
const is31_led g_is31_leds[RGB_MATRIX_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *   driver
 *   |  R location
 *   |  |        G location
 *   |  |        |        B location
 *   |  |        |        | */
    {0, CS1_SW1, CS2_SW1, CS3_SW1}
};

led_config_t g_led_config = { {
  // Key Matrix to LED Index
  {32, 73, 6, 85, 0, 30, 34, 78, 76, 81, 79, 0, 89, 80, 53, 0, },
  {0, 0, 0, 48, 95, 28, 36, 77, 82, 33, 0, 0, 35, 91, 54, 68, },
  {0, 0, 94, 0, 3, 20, 40, 41, 56, 0, 27, 0, 0, 64, 0, 92, },
  {0, 47, 2, 0, 21, 22, 8, 9, 60, 0, 25, 75, 0, 0, 0, 71, },
  {0, 0, 17, 0, 45, 42, 10, 15, 69, 0, 23, 0, 0, 62, 63, 0, },
  {0, 0, 16, 0, 13, 14, 12, 11, 70, 0, 18, 0, 0, 61, 65, 0, },
  {0, 0, 0, 0, 19, 4, 5, 7, 67, 0, 59, 90, 0, 58, 55, 0, },
  {0, 0, 37, 0, 0, 26, 44, 50, 52, 0, 51, 0, 0, 49, 38, 0, },
}, {
  // Key Matrix to LED Index
  { 11, 23 },
  { 0, 23 },
  { 57, 22 },
  { 41, 22 },
  { 73, 22 },
  { 25, 22 },
  { 121, 22 },
  { 89, 22 },
  { 105, 22 },
  { 82, 10 },
  { 114, 10 },
  { 66, 10 },
  { 50, 10 },
  { 34, 10 },
  { 98, 10 },
  { 18, 10 },
  { 2, 10 },
  { 130, 10 },
  { 56, 0 },
  { 40, 0 },
  { 72, 0 },
  { 24, 0 },
  { 88, 0 },
  { 10, 0 },
  { 104, 0 },
  { 0, 0 },
  { 120, 0 },
  { 38, 47 },
  { 22, 47 },
  { 54, 47 },
  { 5, 48 },
  { 70, 47 },
  { 1, 48 },
  { 86, 47 },
  { 0, 60 },
  { 102, 47 },
  { 30, 34 },
  { 62, 34 },
  { 7, 35 },
  { 94, 34 },
  { 110, 34 },
  { 46, 34 },
  { 13, 36 },
  { 78, 34 },
  { 1, 36 },
  { 8, 60 },
  { 38, 59 },
  { 54, 59 },
  { 174, 34 },
  { 126, 34 },
  { 142, 34 },
  { 158, 34 },
  { 190, 34 },
  { 205, 34 },
  { 202, 22 },
  { 219, 22 },
  { 223, 36 },
  { 185, 22 },
  { 137, 22 },
  { 153, 22 },
  { 169, 22 },
  { 162, 10 },
  { 194, 10 },
  { 178, 10 },
  { 209, 11 },
  { 222, 11 },
  { 181, 0 },
  { 136, 0 },
  { 168, 0 },
  { 146, 10 },
  { 199, 0 },
  { 186, 0 },
  { 213, 0 },
  { 224, 0 },
  { 152, 0 },
  { 166, 47 },
  { 118, 47 },
  { 134, 47 },
  { 150, 47 },
  { 182, 47 },
  { 197, 48 },
  { 202, 58 },
  { 203, 48 },
  { 217, 48 },
  { 150, 59 },
  { 108, 60 },
  { 121, 60 },
  { 134, 60 },
  { 166, 59 },
  { 183, 64 },
  { 202, 62 },
  { 220, 64 },
  { 222, 48 },
  { 22, 59 },
  { 70, 60 },
  { 83, 60 },
  { 95, 60 }
}, {
  // LED Index to Flag
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4
} };
