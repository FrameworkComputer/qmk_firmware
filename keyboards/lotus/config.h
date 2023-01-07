// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <config_common.h>

// Prototype for Lotus with Raspberry Pi Pico
// It'll disable column drive because we need to remap some pins to there
// SDB is mapped to GP22
#define PICO_LOTUS

// 18-digit serial number. Must be programmed at factory
// TODO: Figure out how to do that. Probably a script to compile qmk
#define SERIAL_NUMBER "FRALDLvvccxxxxxxxx"

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

// PWM single one backlight configuration
// The RP2040 datasheet says GPIO25 maps to PWM channel 4B
// On the Raspberry Pi Pico this is the green LED on the board, good for prototyping
#define BACKLIGHT_PWM_DRIVER    PWMD4
#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_B

// I2C for the RBG controller
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP26
#define I2C1_SCL_PIN GP27

#define RGB_DISABLE_WHEN_USB_SUSPENDED
// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
// #define ENABLE_RGB_MATRIX_ALPHAS_MODS
// #define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
// #define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_BREATHING
// #define ENABLE_RGB_MATRIX_BAND_SAT
// #define ENABLE_RGB_MATRIX_BAND_VAL
// #define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
// #define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
// #define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#define ENABLE_RGB_MATRIX_DUAL_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
// #define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
// #define ENABLE_RGB_MATRIX_RAINDROPS
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
// #define ENABLE_RGB_MATRIX_HUE_BREATHING
// #define ENABLE_RGB_MATRIX_HUE_PENDULUM
// #define ENABLE_RGB_MATRIX_HUE_WAVE
#define ENABLE_RGB_MATRIX_PIXEL_RAIN
// #define ENABLE_RGB_MATRIX_PIXEL_FLOW
// #define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
// enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#define ENABLE_RGB_MATRIX_DIGITAL_RAIN
// enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
// #define ENABLE_RGB_MATRIX_SOLID_REACTIVE
// #define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
// #define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
// #define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
// #define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#define ENABLE_RGB_MATRIX_SPLASH
// #define ENABLE_RGB_MATRIX_MULTISPLASH
#define ENABLE_RGB_MATRIX_SOLID_SPLASH
#define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH

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

// For MAX...
#define SPI_DRIVER SPID0
#define SPI_SCK_PIN GP18
#define SPI_MOSI_PIN GP19
// Unused
#define SPI_MISO_PIN GP16
#define MAX_CS GP17

// Configure our MAX7219's
#define MAX7219_LOAD MAX_CS
#define MAX7219_CONTROLLERS 1
//#define MAX7219_LED_INTENSITY 1  // Max: 15

// Define this to disable the startup test
//#define MAX7219_NO_STARTUP_TEST

/* This controls the speed of the sign, lower is faster. This is the minimal
 * time between animation frames, in ms. Actual time between frames will
 * always be slightly longer due to other keyboard tasks.
 */
#define MAX7219_SCROLL_TIME 100

/* This setting controls how big the scrollable area for your message sign
 * is. If you set it to 0 your display will not work. If you set it to 1
 * you will have no buffer area, and you will only be able to display a
 * total of 6 characters. Every number after that increases the buffer area
 * by 32 columns.
 *
 * You can calculate how big to make this for the number of characters you
 * want to display:
 *
 *     <number of characters in message> * 6 / 32 + 1
 *
 * You do not need to tune this unless you are trying to save ram.
 */
//#define MAX7219_BUFFER_MULTIPLIER 24

// You can only define one of these at a time:

// Define this to test all LEDs. Keyboard functions will not work.
//#define MAX7219_LED_TEST

// Define this to iterate through LEDs 1 by 1. Keyboard functions will not work.
//#define MAX7219_LED_ITERATE

// Define this to show a simple animation. Keyboard functions will not work.
//#define MAX7219_LED_DANCE

// Define this to show all the characters available
//#define MAX7219_LED_FONTTEST

// Define this to show Clueboard on the sign
//#define MAX7219_LED_CLUEBOARD

// Define this to show the Konami code on the sign
//#define MAX7219_LED_KONAMI

// Define this to show QMK on the sign
//#define MAX7219_LED_QMK_POWERED

// Define this to treat the message board like an etch-a-sketch
//#define DRAWING_TOY_MODE

// Define this if you don't want any of the above
//#define MAX7219_LED_CUSTOM

#define MAX7219_LED_FRAMEWORK
