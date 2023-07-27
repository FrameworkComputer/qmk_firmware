// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#define MATRIX_COLS 16
#define MATRIX_ROWS 8

#define DRIVER_COUNT 1
#define DRIVER_ADDR_1 0b0110000
#define RGB_MATRIX_LED_COUNT 97
 // Disable software shutdown (SW1-SW9 enabled)
#define ISSI_CONFIGURATION 0x01

// Limit current to ensure max current draw is just about 500mA
// when white att 100% brightness
#define ISSI_GLOBALCURRENT 104
