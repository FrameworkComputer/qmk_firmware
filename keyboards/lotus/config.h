// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <config_common.h>

// 18-digit serial number. Must be programmed at factory
// TODO: Figure out how to do that. Probably a script to compile qmk
#define SERIAL_NUMBER "FRALDLvvccxxxxxxxx"

#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1

#ifdef KEYBOARD_lotus_ansi
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_lotus_iso
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_lotus_jis
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_lotus_numpad
    #define LED_NUM_LOCK_PIN GP24
#elif KEYBOARD_lotus_gridpad
#endif

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
