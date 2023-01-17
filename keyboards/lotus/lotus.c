// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "lotus.h"

// Prefix string literal with L for descriptors
#define USBCONCAT(a, b) a##b
#define USBSTR(s) USBCONCAT(L, s)
USB_Descriptor_String_t PROGMEM SerialNumberString = {
    .Header = {
        .Size                   = sizeof(FALLBACK_SERIAL_NUMBER),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = FALLBACK_SERIAL_NUMBER
};

char ascii_serialnum[SERIALNUM_LEN+1];

void *lotus_serial_number_string(void) {
  // Exit early, if it was previously read and converted
  if (ascii_serialnum[0] != '\0' || ascii_serialnum[0] == 0xFF) {
      return &SerialNumberString;
  }

  // Read ASCII serial number from memory-mapped flash
  char *serialnum_ptr = (char*) (FLASH_OFFSET + LAST_4K_BLOCK);
  memcpy(ascii_serialnum, serialnum_ptr, SERIALNUM_LEN);

  // Just keep fallback serialnumber if the flash is erased
  if (ascii_serialnum[0] == 0xFF) {
      return &SerialNumberString;
  }

  // Convert to UCS-2, which is equivalent to UTF-16, if the input is ASCII
  for (int i = 0; i < SERIALNUM_LEN; i++) {
      if (ascii_serialnum[i] > 128) {
          dprintf("Serial number character %d is not valid ASCII.", ascii_serialnum[i]);
          SerialNumberString.Header.Size = i * 2;
          break;
      }
      SerialNumberString.UnicodeString[i] = ascii_serialnum[i];
  }

  return &SerialNumberString;
}
uint16_t lotus_serial_number_string_len(void) {
  return pgm_read_byte(&SerialNumberString.Header.Size);
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
