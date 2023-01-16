// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "lotus.h"

// Prefix string literal with L for descriptors
#define USBCONCAT(a, b) a##b
#define USBSTR(s) USBCONCAT(L, s)
USB_Descriptor_String_t PROGMEM SerialNumberString = {
    .Header = {
        .Size                   = sizeof(USBSTR(SERIAL_NUMBER)),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = USBSTR(SERIAL_NUMBER)
};

char ascii_serialnum[SERIALNUM_LEN+1];
char utf16_serialnum[(SERIALNUM_LEN+1) * 2];

void *lotus_serial_number_string(void) {
  if (utf16_serialnum[0] != '\0') {
    return utf16_serialnum;
  }
  ascii_serialnum[SERIALNUM_LEN] = '\0';
  utf16_serialnum[SERIALNUM_LEN*2] = '\0';
  utf16_serialnum[SERIALNUM_LEN*2 + 1] = '\0';
  char *serialnum_ptr = (char*) (FLASH_OFFSET + LAST_4K_BLOCK);

  memcpy(ascii_serialnum, serialnum_ptr, SERIALNUM_LEN);
  for (int c = 0; c < SERIALNUM_LEN; c++) {
      utf16_serialnum[c*2] = ascii_serialnum[c];
  }

  //SerialNumberString.UnicodeString = &utf16_serialnum[0];
  memcpy(SerialNumberString.UnicodeString, utf16_serialnum, SERIALNUM_LEN*2);
  SerialNumberString.Header.Size = SERIALNUM_LEN * 2;

  return &SerialNumberString;
}
uint16_t lotus_serial_number_string_len(void) {
  return SERIALNUM_LEN*2;
}

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
    writePinHigh(BOOT_DONE_GPIO);

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
