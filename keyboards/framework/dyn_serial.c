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

struct __attribute__((__packed__)) serialnum_raw {
  uint8_t sn_rev;
  uint8_t serialnum[SERIALNUM_LEN];
  uint8_t crc32[4];
};

uint32_t crc32b(uint8_t *message, size_t msg_len) {
   uint8_t byte;
   uint32_t crc;
   uint32_t mask;

   crc = 0xFFFFFFFF;
   for (size_t i = 0; i < msg_len; i++) {
      byte = message[i];
      crc = crc ^ byte;
      for (int j = 0; j < 8; j++) {
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
   }
   return ~crc;
}

void *dyn_serial_number_string(void) {
  // Read ASCII serial number from memory-mapped flash
  void *serialnum_ptr = (void*) (FLASH_OFFSET + LAST_4K_BLOCK);
  struct serialnum_raw *serialnum_raw = serialnum_ptr;

  // Only rev 1 supported
  if (serialnum_raw->sn_rev != 1) {
      return &SerialNumberString;
  }

  uint32_t crc32 = crc32b(serialnum_ptr, SERIALNUM_LEN+1);
  // Make sure it's little-endian
  uint32_t crc32_read = (serialnum_raw->crc32[3] << 24) +
    (serialnum_raw->crc32[2] << 16) +
    (serialnum_raw->crc32[1] << 8) +
    serialnum_raw->crc32[0];

  // CRC invalid, fall back to defult serial number
  if (crc32_read != crc32) {
      return &SerialNumberString;
  }

  // Convert to UCS-2, which is equivalent to UTF-16, if the input is ASCII
  memcpy(ascii_serialnum, serialnum_raw->serialnum, SERIALNUM_LEN);
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
