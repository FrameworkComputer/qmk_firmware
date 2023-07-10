// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "framework.h"

void keyboard_post_init_kb(void) {
  keyboard_post_init_user();

  // Enable debug output
  debug_enable = true;
  debug_matrix = true;
  debug_keyboard = true;
}

/**
 * Hook into early keyboard initialization
*/
void keyboard_pre_init_kb(void) {
    keyboard_pre_init_user();

    // Mark boot as done.
    // Before this, when holding down both alt keys QSPI_SS is pulled low to put
    // the RP2040 in bootloader mode during reset.
    setPinOutput(BOOT_DONE_GPIO);
    writePinLow(BOOT_DONE_GPIO);

    //// TODO: Do we ever need to disable it to save power?
    setPinOutput(MUX_ENABLE_GPIO);
    writePinHigh(MUX_ENABLE_GPIO);

#ifdef RGB_MATRIX_ENABLE
    setPinOutput(IS31FL3743A_ENABLE_GPIO);
    writePinHigh(IS31FL3743A_ENABLE_GPIO);
#endif
    setPinInput(SLEEP_GPIO);
}

/**
 * Called by QMK when the keyboard suspends
 */
void suspend_power_down_kb(void) {
  suspend_power_down_user();

#ifdef RGB_MATRIX_ENABLE
  writePinLow(IS31FL3743A_ENABLE_GPIO);
#endif
}

/**
 * Called by QMK when the keyboard wakes up from suspend
 */
void suspend_wakeup_init_kb(void) {
  suspend_wakeup_init_user();

#ifdef RGB_MATRIX_ENABLE
    setPinOutput(IS31FL3743A_ENABLE_GPIO);
    writePinHigh(IS31FL3743A_ENABLE_GPIO);
#endif
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  process_record_user(keycode, record);

#ifdef RGB_MATRIX_ENABLE
  uint8_t h;
  uint8_t s;
  uint8_t v;
  uint8_t new_v;
#endif

  switch (keycode) {
    // Implement step brightness for RGB backlight
#ifdef RGB_MATRIX_ENABLE
    case BL_STEP:
      if (record->event.pressed) {
        h = rgb_matrix_get_hue();
        s = rgb_matrix_get_sat();
        v = rgb_matrix_get_val();
        switch (v) {
          default: // Default when user set to a different level
          case 0:
            new_v = 85;
            break;
          case 85:
            new_v = 170;
            break;
          case 170:
            new_v = 255;
            break;
          case 255:
            new_v = 0;
            break;
        }
        rgb_matrix_sethsv_noeeprom(h, s, new_v);
      }
      return true;
#endif
    case KC_SCRN:
      // Simulate press WIN+P
      // Works (at least) on Windows and GNOME
      if (record->event.pressed) {
        register_code(KC_LGUI);
        register_code(KC_P);
      } else {
        unregister_code(KC_P);
        unregister_code(KC_LGUI);
      }
      return false; // Skip all further processing of this key
    default:
      return true; // Process all other keycodes normally
  }
}
