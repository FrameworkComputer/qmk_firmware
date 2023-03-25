// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "framework.h"

void keyboard_post_init_kb(void) {
    keyboard_post_init_user();

    // Enable debug output
    debug_enable   = true;
    debug_matrix   = true;
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
    writePinLow(BOOT_DONE_GPIO);

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
                unregister_code(KC_P);
                unregister_code(KC_LGUI);
            }
            return false; // Skip all further processing of this key
        default:
            return true; // Process all other keycodes normally
    }
}

/**
 * Handle the host going to sleep or the keyboard being idle
 * If the host is asleep the keyboard should reduce the scan rate and turn backlight off.
 *
 * If the host is awake but the keyboard is idle it should enter a low-power state
 */
void handle_idle(void) {
    bool            asleep      = !readPin(SLEEP_GPIO);
    static int      prev_asleep = -1;
    static uint32_t host_sleep  = 0;
    /* reduce the scan speed to 10Hz */
    if (prev_asleep != asleep) {
        prev_asleep = asleep;
        if (asleep) {
            suspend_power_down_quantum();
        } else {
            suspend_wakeup_init_quantum();
        }
    }
    if (asleep) {
        if (timer_elapsed32(host_sleep) < 100) {
            port_wait_for_interrupt();
        } else {
            host_sleep = timer_read32();
        }
    }
}

void housekeeping_task_kb(void) {
    handle_idle();
}
