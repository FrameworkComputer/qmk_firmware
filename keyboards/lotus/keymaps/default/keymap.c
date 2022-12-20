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
     *         ├────┬───┴┬──┴─┬─┴──┬┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │    │    │    │                │   │   │    │ ↑ │    │
     * 11 keys │Ctrl│ FN │GUI │Alt │                │Alt│Ctl│ ←  ├───┤  → │
     *         │    │    │    │    │                │   │   │    │ ↓ │    │
     *         └────┴────┴────┴────┴────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    [0] = LAYOUT_78_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,
        KC_LCTL, MO(1),   KC_LGUI, KC_LALT,          KC_SPC,                             KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),
     /*
     * Function layer
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │FN lk│Mut│vDn│vUp│Prv│Ply│Nxt│bDn│bUp│Scn│Air│Prt│App│Ins │
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┤
     * 14 keys │   │   │   │   │   │   │   │   │   │   │   │   │   │      │
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬────┤
     * 14 keys │     │   │   │   │   │   │   │   │   │   │   │   │   │    │
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴────┤
     * 13 keys │      │   │   │   │   │   │   │   │   │   │   │   │       │
     *         ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───────┤
     * 12 keys │        │   │   │   │   │   │   │   │   │   │   │         │
     *         ├────┬───┴┬──┴─┬─┴──┬┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │    │    │    │                │   │   │    │PgU│    │
     * 11 keys │    │    │    │    │Toggle Backlight│   │   │Home├───┤End │
     *         │    │    │    │    │                │   │   │    │PgD│    │
     *         └────┴────┴────┴────┴────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    // TODO: Screen toggle, airplane mode
    // TODO: Other unlabeled keys
    [1] = LAYOUT_78_ansi(
        TG(1), KC_KB_MUTE, KC_KB_VOLUME_DOWN, KC_KB_VOLUME_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_BRID, KC_BRIU, KC_TRNS, KC_TRNS, KC_PSCR, KC_MSEL, KC_INS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
        KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          BL_TOGG,                            KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP, KC_RGHT
    )
};

/**
 * Keyscan mockup/pseudo-code below. Prefixed with zoid_ to avoid hooking into system
*/
#define LOTUS_COLS 14

void adc_gpio_init(int gpio) {
    // TODO: Implement
}
void adc_select_input(int adc_channel) {
    // TODO: Implement
}

// Mux GPIOs
#define MUX_A 1
#define MUX_B 2
#define MUX_C 3
#define MUX_ENABLE 4
// Mux output
#define ADC_IN 28

void gpio_set(int gpio, int enable) {
    // TODO: Implement
}

/**
 * Tell the mux to select a specific column
 *
 * Splits the positive integer (<=7) into its three component bits.
*/
void mux_select_col(int col) {
    int bits[] = {
        (col & 0x1) > 0,
        (col & 0x4) > 0,
        (col & 0x8) > 0
    };
    gpio_set(MUX_A, bits[0]);
    gpio_set(MUX_B, bits[1]);
    gpio_set(MUX_C, bits[2]);
}
uint16_t adc_read(void) { return 0; }

void zoid_keyscan(void) {
    for (int col = 0; col < LOTUS_COLS; col++) {
        // TODO: Map col index to GPIO
        // Drive column high so we can measure the resistors
        gpio_set(col, 1);

        // Read ADC for this row
        mux_select_col(col);
        uint16_t adc_value = adc_read();

        // Interpret ADC value as rows
        printf("%04X", adc_value);

        // Drive column low again
        gpio_set(col, 1);
    }
}

void adc_init(void) {
    gpio_set(MUX_ENABLE, 1);
}
// TODO: Do we need a de-init? Probably not.

void zoid_keyscan_init(void) {
    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
}