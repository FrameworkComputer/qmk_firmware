// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "dyn_serial.h"
#include "usb_descriptor.h"
#include "usb_descriptor_common.h"

// Prefix string literal with L for descriptors
#ifdef SERIAL_NUMBER
USB_Descriptor_String_t PROGMEM SerialNumberString = {
    .Header = {
        .Size                   = sizeof(FALLBACK_SERIAL_NUMBER),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = FALLBACK_SERIAL_NUMBER
};

char ascii_serialnum[SERIALNUM_LEN+1];

void *dyn_serial_number_string(void) {
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
uint16_t dyn_serial_number_string_len(void) {
  return pgm_read_byte(&SerialNumberString.Header.Size);
}
#endif
