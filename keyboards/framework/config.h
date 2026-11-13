// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Allow (emulated) EEPROM reset using VIA
#define VIA_EEPROM_ALLOW_RESET

#define DYNAMIC_KEYMAP_LAYER_COUNT 10

// Increase to 16K for more storage space
// With 64K backing size, this spread the wear level over 4 blocks
#define WEAR_LEVELING_LOGICAL_SIZE      (4*4096)
// Increase to 64K backing size to allow more wear leveling
#define WEAR_LEVELING_BACKING_SIZE      (16 * 4096)
// We use 1MB flash
#define PICO_FLASH_SIZE_BYTES           (1 * 1024 * 1024)
#define WEAR_LEVELING_RP2040_FLASH_SIZE PICO_FLASH_SIZE_BYTES
// Keep the last 4K sector free for the serial number
#define WEAR_LEVELING_RP2040_FLASH_BASE ((PICO_FLASH_SIZE_BYTES) - (WEAR_LEVELING_BACKING_SIZE + 4096))


// Prints every second how many matrix scans were done (Frequency in Hz)
// Only during debugging, since it prevents the host from going to sleep
//#define DEBUG_MATRIX_SCAN_RATE

// Power saving: Minimum time between the start of two matrix scans.
// The MCU sleeps (WFI) in between. USB polls the keyboard every 1ms, so
// scanning faster than that doesn't reduce latency. Set to 0 to scan as fast
// as possible (old behavior).
#ifndef FRAMEWORK_SCAN_INTERVAL_US
#    define FRAMEWORK_SCAN_INTERVAL_US 1000
#endif
// Same, but while the host is suspended and we only scan to detect wake-up
// keypresses. Keep it well below the debounce time plus a quick keypress:
// The debounce algorithm needs two scans, at least DEBOUNCE ms apart, that
// see the key pressed before it's reported.
#ifndef FRAMEWORK_SUSPEND_SCAN_INTERVAL_US
#    define FRAMEWORK_SUSPEND_SCAN_INTERVAL_US 10000
#endif
// Gate unused RP2040 clocks while the CPU waits during suspend. USB resume,
// the scheduler timer and matrix ADC remain clocked. Restore the original
// sleep clock mask on wake. Define this to compare with the old WFI behavior.
//#define FRAMEWORK_NO_SUSPEND_CLOCK_GATING
// While the lid is closed keypresses are ignored, only check the lid state
#ifndef FRAMEWORK_LID_CLOSED_POLL_MS
#    define FRAMEWORK_LID_CLOSED_POLL_MS 20
#endif

// Power saving: Put the RGB LED controller(s) into hardware shutdown when all
// LEDs have been off for this long (RGB disabled, brightness 0, timeout, ...)
// Define FRAMEWORK_NO_RGB_AUTO_SHUTDOWN to only shut down when suspended.
#ifndef FRAMEWORK_RGB_SHUTDOWN_DELAY_MS
#    define FRAMEWORK_RGB_SHUTDOWN_DELAY_MS 1000
#endif

// Power saving: Turn the keyboard backlight (white and RGB) off after this
// many milliseconds without a keypress. The next keypress turns it back on.
// 0 disables the timeout (default, same behavior as before).
// For example 5 minutes: (5 * 60 * 1000)
#ifndef FRAMEWORK_BACKLIGHT_TIMEOUT_MS
#    define FRAMEWORK_BACKLIGHT_TIMEOUT_MS 0
#endif

// Power saving (experimental): Run the RP2040 at 48MHz from the USB PLL
// instead of 125MHz and power down the system PLL.
// Makes the matrix scan take longer, see FRAMEWORK_SCAN_INTERVAL_US.
//#define FRAMEWORK_SYS_CLOCK_48MHZ

// Power saving (experimental): Read the ADC by polling its registers instead
// of through the ChibiOS driver (IRQ + context switches per conversion).
// Makes the matrix scan several times faster. The time between switching the
// mux and sampling changes slightly, see FRAMEWORK_FAST_ADC_SETTLE_CYCLES.
//#define FRAMEWORK_FAST_ADC

// Prototype for Framework Laptop 16 with Raspberry Pi Pico
// It'll disable column drive because we need to remap some pins to there
// SDB is mapped to GP22
//#define PICO_FL16

// Enable serial number. Calls into dyn_serial_number_string* functions
#define SERIAL_NUMBER

#if KEYBOARD_framework_ansi
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_framework_copilot
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_framework_iso
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_framework_jis
    #define LED_CAPS_LOCK_PIN GP24
#elif KEYBOARD_framework_numpad
    #define LED_NUM_LOCK_PIN GP24
#elif KEYBOARD_framework_macropad
#endif

// PWM single one backlight configuration
// GPIO25 maps to PWM channel 4B of the RP2040
// On the Raspberry Pi Pico this is the green LED on the board, good for prototyping
#define BACKLIGHT_PWM_DRIVER    PWMD4
#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_B
#define BACKLIGHT_BREATHING
// Change PWM frequency to 24kHz, the default of 2048Hz causes loud noise
#define BACKLIGHT_PWM_PERIOD BACKLIGHT_PWM_COUNTER_FREQUENCY / 24000

// I2C for the RBG controller
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP26
#define I2C1_SCL_PIN GP27

// RGB config

// Turn off RGB LED when USB suspended
#define RGB_DISABLE_WHEN_USB_SUSPENDED

// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
// 0 = Off
// 1 = Solid Color
// 2
#define ENABLE_RGB_MATRIX_ALPHAS_MODS
#define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_BAND_SAT
#define ENABLE_RGB_MATRIX_BAND_VAL
#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
// 10
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
// 20
#define ENABLE_RGB_MATRIX_DUAL_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#define ENABLE_RGB_MATRIX_RAINDROPS
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#define ENABLE_RGB_MATRIX_HUE_BREATHING
#define ENABLE_RGB_MATRIX_HUE_PENDULUM
#define ENABLE_RGB_MATRIX_HUE_WAVE
#define ENABLE_RGB_MATRIX_PIXEL_RAIN
#define ENABLE_RGB_MATRIX_PIXEL_FLOW
// 30
#define ENABLE_RGB_MATRIX_PIXEL_FRACTAL

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#define ENABLE_RGB_MATRIX_DIGITAL_RAIN


#define RGB_MATRIX_KEYPRESSES
// enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
// 33
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
// 40
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#define ENABLE_RGB_MATRIX_SPLASH
#define ENABLE_RGB_MATRIX_MULTISPLASH
#define ENABLE_RGB_MATRIX_SOLID_SPLASH
#define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH

#define ISSI_CUSTOM_PWM_FREQ

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
