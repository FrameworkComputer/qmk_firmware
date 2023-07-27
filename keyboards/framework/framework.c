// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "framework.h"
#include "os_detection.h"

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

  //os_variant_t os = detected_host_os();
  //switch (os) {
  //    case OS_UNSURE:
  //      rgb_matrix_sethsv(0, 0, 255); // White
  //      break;
  //    case OS_LINUX:
  //      // Works on laptop and android
  //      rgb_matrix_sethsv(213, 255, 255); // purple
  //      break;
  //    case OS_WINDOWS:
  //      // works
  //      rgb_matrix_sethsv(170, 255, 255); // blue
  //      break;
  //    case OS_MACOS:
  //      rgb_matrix_sethsv(85, 255, 255); // green
  //      break;
  //    case OS_IOS:
  //      // works on M1 mac
  //      rgb_matrix_sethsv(43, 255, 255); // yellow
  //      break;
  //    default:
  //      rgb_matrix_sethsv(125, 255, 255); // cyan
  //      break;
  //}

  switch (keycode) {
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
    case STORE_SETUPS:
        if (record->event.pressed) {
            store_setups_in_eeprom();
        }
        return false;
    case PRINT_SETUPS:
        if (record->event.pressed) {
            print_stored_setups();
        }
        return false;
    default:
      return true; // Process all other keycodes normally
  }
}
