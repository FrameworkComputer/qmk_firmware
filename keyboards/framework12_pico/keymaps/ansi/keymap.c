/* Copyright 2021 QMK
 * Copyright 2022 DHowett
 * Copyright 2022 Arya
 * From https://github.com/CRImier/qmk_firmware
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

typedef union {
  uint32_t raw;
  struct {
    bool     fnlock_enabled :1;
  };
} user_config_t;

user_config_t user_config;

enum _ext_keycode {
	FK_RFKL = SAFE_RANGE, // RF Kill (Airplane Mode)
	FK_BRND, // Brightness Down
	FK_BRNU, // Brightness Up
	FK_BKLT, // Keyboard Backlight
	FK_FN,
};

enum _layers {
	_BASE,
	_FN_ANY,
	_FN_PRESSED,
};

#define FK_FLCK TG(_FN_ANY)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐
     * │Esc│F1 │F2 │F3 │F4 │F5 │F6 │F7 │F8 │F9 │F10│F11│F12│  Del  │
     * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───────┤
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │ Enter  │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┤
     * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │Shift │ ↑ │
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴──┬┴──┬┴──┬───┼───┼───┐
     * │Ctrl│GUI │Alt │         Space          │Alt│GUI│Ctl│ ← │ ↓ │ → │
     * └────┴────┴────┴────────────────────────┴───┴───┴───┴───┴───┴───┘
     */
    [_BASE] = LAYOUT_framework_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,
        KC_LCTL, FK_FN, KC_LGUI, KC_LALT,               KC_SPC,                 KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT, KC_UP
    ),
    [_FN_ANY] = LAYOUT_framework_ansi(
        _______, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPRV, KC_MPLY, KC_MNXT, KC_BRID, KC_BRIU, _______, FK_RFKL, KC_PSCR, KC_MSEL, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______,             _______,                _______, _______, _______, _______, _______, _______
    ),
    [_FN_PRESSED] = LAYOUT_framework_ansi(
        FK_FLCK, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_INS,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PAUS, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, KC_SCRL, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          QK_BOOT,
        _______, _______, _______, _______,             BL_STEP,                _______, _______, KC_HOME, KC_PGDN, KC_END,  KC_PGUP
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
	bool fnlock_enabled = !layer_state_cmp(state, _FN_ANY);
    user_config.fnlock_enabled = fnlock_enabled;
    eeconfig_update_user(user_config.raw);
	return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
        switch (keycode) {
                case FK_FN: { // FN
			// The FN key toggles the top row (F1-F12)
			// which we keep in its own layer.
			// We use XOR here to turn it off while held if function lock
			// has turned it on
			layer_invert(_FN_ANY);
			if (record->event.pressed) {
				layer_on(_FN_PRESSED);
	    		} else {
				layer_off(_FN_PRESSED);
            }

			return false;
    		}
        }
	return true;
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    if (user_config.fnlock_enabled) {
			layer_on(_FN_ANY);
	}
}
