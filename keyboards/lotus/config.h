// Copyright 2022 Yoichiro Tanaka (@yoichiro)
// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <config_common.h>

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1

#define LED_CAPS_LOCK_PIN GP24
//#define LED_NUM_LOCK_PIN GP24

//#define SERIAL_DEBUG

#define WS2812_PIO_USE_PIO1

#define RGBLIGHT_LED_MAP { 0, 1, 2, 3, 4, 5, \
                           11, 10, 9, 8, 7, 6 }

#define DRIVER_COUNT 1
#define RGB_MATRIX_LED_COUNT 78

// PWM single one backlight configuration
// The RP2040 datasheet says GPIO25 maps to PWM channel 4B
#define BACKLIGHT_PWM_DRIVER    PWMD4
#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_B

/* define if matrix has ghost (lacks anti-ghosting diodes) */
//#define MATRIX_HAS_GHOST

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
