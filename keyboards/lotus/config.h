// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <config_common.h>

#define MATRIX_COLS 16
#define MATRIX_ROWS 8

#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1

#define LED_CAPS_LOCK_PIN GP24
//#define LED_NUM_LOCK_PIN GP24

#define SERIAL_DEBUG

#define WS2812_PIO_USE_PIO1

// PWM single one backlight configuration
// The RP2040 datasheet says GPIO25 maps to PWM channel 4B
// On the Raspberry Pi Pico this is the green LED on the board, good for prototyping
#define BACKLIGHT_PWM_DRIVER    PWMD4
#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_B

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
