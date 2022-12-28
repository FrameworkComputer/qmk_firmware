// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "lotus.h"

#define BOOT_DONE_GPIO GP5

void keyboard_post_init_user(void) {
  // Enable debug output
  debug_enable = true;
  debug_matrix = true;
  debug_keyboard = true;
}

/**
 * Hook into early keyboard initialization
*/
void keyboard_pre_init_user(void) {
    // Mark boot as done.
    // Before this, when holding down both alt keys QSPI_SS is pulled low to put
    // the RP2040 in bootloader mode during reset.
    setPinOutput(BOOT_DONE_GPIO);
    writePinHigh(BOOT_DONE_GPIO);
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  process_record_user();

  switch (keycode) {
    case KC_SCRN:
      // Simulate press WIN+P
      // Works (at least) on Windows and GNOME
      if (record->event.pressed) {
        register_code(KC_LGUI);
        register_code(KC_P);
      } else {
        unregister_code(KC_LGUI);
        unregister_code(KC_P);
      }
      return false; // Skip all further processing of this key
    default:
      return true; // Process all other keycodes normally
  }
}