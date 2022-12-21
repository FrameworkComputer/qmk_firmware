// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_japanese.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │Esc  │F1 │F2 │F3 │F4 │F5 │F6 │F7 │F8 │F9 │F10│F11│F12│ Del│
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴┬───┤
     * 15 keys │Z↔H│ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ ^ │¥ │Bck│
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬┴───┤
     * 13 keys │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ @ │ [ │Entr│
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐   │
     * 14 keys │ Eisū │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ : │ ] │   │
     *         ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┤
     * 13 keys │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │ \ │Shift│
     *         ├────┬───┼───┼───┼──┬┴───┴───┴─┬─┴┬──┼───┼───┼───┴┬──┴┬────┤
     *         │    │   │   │   │M │          │H │K │   │   │    │↑  │    │
     * 14 keys │Ctrl│FN │GUI│Alt│u │          │e │↔ │Alt│Ctl│ ←  ├───┤  → │
     *         │    │   │   │   │h │          │n │H │   │   │    │  ↓│    │
     *         └────┴───┴───┴───┴──┴──────────┴──┴──┴───┴───┴────┴───┴────┘
     * 83 total
     */
    [0] = LAYOUT_83_jis(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        JP_ZKHK, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, JP_CIRC, JP_YEN,  KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    JP_AT,   KC_LBRC,
        JP_EISU, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_RBRC, KC_ENT,
            KC_LSFT,      KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, JP_BSLS,     KC_RSFT,
        KC_LCTL, MO(1),   KC_LGUI, KC_LALT, JP_MHEN,     KC_SPC,       JP_HENK, JP_KANA, KC_RALT, KC_RCTL, KC_LEFT, KC_UP,   KC_DOWN, KC_RGHT
    ),
     /*
     * Function layer
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │FN lk│Mut│vDn│vUp│Prv│Ply│Nxt│bDn│bUp│Scn│Air│Prt│App│Ins │
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┤
     * 14 keys │   │   │   │   │   │   │   │   │   │   │   │   │   │      │
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬────┤
     * 13 keys │     │   │   │   │   │   │   │   │   │   │   │   │   │    │
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐   │
     * 14 keys │      │   │   │   │   │   │   │   │   │   │   │   │   │   │
     *         ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴───┤
     * 13 keys │    │   │   │   │   │   │   │   │   │   │   │   │         │
     *         ├────┼───┼───┼───┼───┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │   │   │   │                   │   │   │    │PgU│    │
     * 11 keys │    │   │   │   │ Toggle Backlight  │   │   │Home├───┤End │
     *         │    │   │   │   │                   │   │   │    │PgD│    │
     *         └────┴───┴───┴───┴───────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    // TODO: Screen toggle, airplane mode
    // TODO: Other unlabeled keys
    [1] = LAYOUT_83_jis(
        TG(1), KC_KB_MUTE, KC_KB_VOLUME_DOWN, KC_KB_VOLUME_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_BRID, KC_BRIU, KC_TRNS, KC_TRNS, KC_PSCR, KC_MSEL, KC_INS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
            KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     BL_TOGG,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HOME, KC_PGUP, KC_PGDN, KC_END
    )
};
