// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

//#include "quantum.h"

void *dyn_serial_number_string(void);
uint16_t dyn_serial_number_string_len(void);
// The serial number is written to the last 4K block in the first 1M of flash
#define FLASH_OFFSET 0x10000000
#define LAST_4K_BLOCK 0xff000
#define SERIALNUM_LEN 18

// 18-digit serial number. Must be programmed at factory
// TODO: Figure out how to do that. Probably a script to compile qmk
// Must be wide-string (UTF-16)
// KDK = Keyboard RGB   Backlight
// KDW = Keyboard White Backlight
// KDN = Keyboard RGB   Gridpad
// KDM = Keyboard White Numpad
#define FALLBACK_SERIAL_NUMBER L"FRAKDKEN0100000000"
_Static_assert(sizeof(FALLBACK_SERIAL_NUMBER) == (SERIALNUM_LEN+1)*2, "Fallback serial number must be same length");
