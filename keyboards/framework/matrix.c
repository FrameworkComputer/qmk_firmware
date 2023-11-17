// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdint.h>
#include "debug.h"
#include "analog.h"
#include "print.h"
#include "quantum.h"
#include "hal_adc.h"
#include "chprintf.h"

#include "matrix.h"
#include "framework.h"

#define adc10ksample_t int

// Mux GPIOs
#define MUX_A GP1
#define MUX_B GP2
#define MUX_C GP3
#define MUX_ENABLE GP4

// Rows to ADC input
#define KSI0 2
#define KSI1 0
#define KSI2 1
#define KSI3 3

// Columns to GPIOs
#define KSO0  GP8
#define KSO1  GP9
#define KSO2  GP10
#define KSO3  GP11
#define KSO4  GP12
#define KSO5  GP13
#define KSO6  GP14
#define KSO7  GP15
#define KSO8  GP21
#define KSO9  GP20
#define KSO10 GP19
#define KSO11 GP18
#define KSO12 GP17
#define KSO13 GP16
#define KSO14 GP23
#define KSO15 GP22

#define ADC_CH2_PIN  GP28

// Voltage threshold - anything below that counts as pressed
// 29000 = 2.9V * 10000
const adc10ksample_t ADC_THRESHOLD = (adc10ksample_t) 29000;

bool have_slept = false;

adc10ksample_t to_voltage(adcsample_t sample) {
  int voltage = sample * 33000;
  return voltage / 1023;
}

void print_as_float(adc10ksample_t sample) {
  int digits = sample / 10000;
  int decimals = sample % 10000;
  uprintf("%d.%02d\n", digits, decimals);
}

/**
 * Tell RP2040 ADC controller to initialize a specific GPIO for ADC input
*/
void adc_gpio_init(int gpio) {
    assert(gpio >= GP26 && gpio <= GP28);
    // Enable pull-up on GPIO input so that we always have high input
    // Even on the rows that don't have the external pull-up.
    // Otherwise they would be floating.
    #define PAL_MODE_ADC_PULLUP           (PAL_MODE_INPUT_ANALOG | PAL_RP_PAD_PUE)
    palSetLineMode(gpio, PAL_MODE_ADC_PULLUP);
}

/**
 * Tell the mux to select a specific column
 *
 * Splits the positive integer (<=7) into its three component bits.
*/
static void mux_select_row(int row) {
    assert(col >= 0 && col <= 7);

    // Not in order - need to remap them

    // X0 - KSI1
    // X1 - KSI2
    // X2 - KSI0
    // X3 - KSI3
    // Only for keyboard, not for num-/grid-pad
    // X4 - KSI4
    // X5 - KSI5
    // X6 - KSI6
    // X7 - KSI7
    int index = 0;
    switch (row) {
        case 0:
            index = 2;
            break;
        case 1:
            index = 0;
            break;
        case 2:
            index = 1;
            break;
        default:
            index = row;
            break;
    }

    int bits[] = {
        (index & 0x1) > 0,
        (index & 0x2) > 0,
        (index & 0x4) > 0
    };
    (void)bits;
    //uprintf("Mux A: %d, B: %d, C: %d, KSI%d, X%d\n", bits[0], bits[1], bits[2], row, index);
    writePin(MUX_A, bits[0]);
    writePin(MUX_B, bits[1]);
    writePin(MUX_C, bits[2]);
}

/**
 * Based on the ADC value, update the matrix for this column
 * */
static bool interpret_adc_row(matrix_row_t cur_matrix[], adc10ksample_t voltage, int col, int row) {
    bool changed = false;

    // By default the voltage is high (3.3V)
    // When a key is pressed it causes the voltage to go down.
    // But because every key is connected in a matrix, pressing multiple keys
    // changes the voltage at every key again. So we can't check for a specific
    // voltage but need to have a threshold.
    bool key_state = false;
    if (voltage < ADC_THRESHOLD) {
        key_state = true;
    }

    if (key_state) {
        //uprintf("Col %d - Row %d - State: %d, Voltage: ", col, row, key_state);
        //print_as_float(voltage);
    }

// Don't update  matrix on Pico to avoid messing with the debug system
// Can't attach the matrix anyways
//#ifdef PICO_FL16
    //(void)key_state;
    //return false;
//#endif

    matrix_row_t new_row = cur_matrix[row];
    if (key_state) {
        new_row |= (1 << col);
    } else {
        new_row &= ~(1 << col);
    }
    changed = cur_matrix[row] != new_row;
    if (key_state) {
        uprintf("Keypress at KSO%d, KSI%d - %d.%dV - [X] \n", col, row, voltage/10000, voltage%10000);
    } else {
        uprintf("Keypress at KSO%d, KSI%d - %d.%dV - [ ] \n", col, row, voltage/10000, voltage%10000);
    }

    cur_matrix[row] = new_row;

    return changed;
}

/**
 * Drive the GPIO for a column low or high.
 */
void drive_col(int col, bool high) {
    assert(col >= 0 && col <= MATRIX_COLS);
    int gpio = 0;
    switch (col) {
        case 0:
            gpio = GP8;
            break;
        case 1:
            gpio = GP9;
            break;
        case 2:
            gpio = GP10;
            break;
        case 3:
            gpio = GP11;
            break;
        case 4:
            gpio = GP12;
            break;
        case 5:
            gpio = GP13;
            break;
        case 6:
            gpio = GP14;
            break;
        case 7:
            gpio = GP15;
            break;
        case 8:
            gpio = GP21;
            break;
        case 9:
            gpio = GP20;
            break;
        case 10:
            gpio = GP19;
            break;
        case 11:
            gpio = GP18;
            break;
        case 12:
            gpio = GP17;
            break;
        case 13:
            gpio = GP16;
            break;
        case 14:
            gpio = GP23;
            break;
        case 15:
            gpio = GP22;
            break;
        default:
            // Not supposed to happen
            assert(false);
            return;
    }

// Don't drive columns on pico because we're using these GPIOs for other purposes
//#ifdef PICO_FL16
//    (void)gpio;
//    return;
//#endif

    //uprintf("Driving col %s %d, GP%d\n", high ? "HIGH" : "LOW ", col, gpio);
    if (high) {
        // TODO: Could set up the pins with `setPinOutputOpenDrain` instead
        writePinHigh(gpio);
    } else {
        writePinLow(gpio);
    }
}

/**
 * Read a value from the ADC and print some debugging details
 */
static adc10ksample_t read_adc(void) {
    // Can't use analogReadPin because it gets rid of the internal pullup on
    // this pin, that we configure in matrix_init_custom
    // uint16_t val = analogReadPin(ADC_CH2_PIN);
    uint16_t val = adc_read(pinToMux(ADC_CH2_PIN));
    return to_voltage(val);
}

/**
 * Handle the host going to sleep or the keyboard being idle
 * If the host is asleep the keyboard should reduce the scan rate and turn backlight off.
 *
 * If the host is awake but the keyboard is idle it should enter a low-power state
 */
bool handle_idle(void) {
    bool asleep = !readPin(SLEEP_GPIO);
    static uint8_t prev_asleep = -1;
    if (prev_asleep != asleep) {
        prev_asleep = asleep;
    }
#ifdef RGB_MATRIX_ENABLE
    if (rgb_matrix_get_suspend_state() != asleep) {
        rgb_matrix_set_suspend_state(asleep);
    }
#endif
#ifdef BACKLIGHT_ENABLE
    if (is_backlight_enabled() != !asleep) {
        if (asleep) {
            backlight_disable();
            have_slept = true;
        } else if (have_slept) {
            // For some reason this will not set the proper value right after
            // turning on. But the quantum code will have set it properly
            // already, so there's no need to run this. Unless we actually wake
            // up from sleep.
            backlight_enable_old_level();
        }
    }
#endif

    // TODO: Try this again later, but for now USB suspend should be fine
    // This seems to cause issues with waking up the host by keypresses
    //  static int host_sleep = 0;
    //  /* reduce the scan speed to 10Hz */
    //  if (prev_asleep != asleep) {
    //      prev_asleep = asleep;
    //      if (!asleep) {
    //          suspend_power_down_quantum();
    //      } else {
    //          suspend_wakeup_init_quantum();
    //      }
    //  }
    //  if (!asleep) {
    //      if (timer_elapsed32(host_sleep) < 100) {
    //          port_wait_for_interrupt();
    //          return true;
    //      } else {
    //          host_sleep = timer_read32();
    //      }
    //  }
    return false;
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    if (handle_idle()) {
        return false;
    }

    wait_us(500 * 1000);
    // Drive all high to deselect them
    for (int col = 0; col < MATRIX_COLS; col++) {
        drive_col(col, true);
    }

    // Go through every matrix column (KSO) and drive them low individually
    // Then go through every matrix row (KSI), select it with the mux and check their ADC value
    for (int col = 0; col < MATRIX_COLS; col++) {
        // Drive column low so we can measure the resistors on each row in this column
        drive_col(col, false);
        for (int row = 0; row < MATRIX_ROWS; row++) {
            // Debug for keyboard. Row 5 and 6 don't seem to work
            //print("\n");
            // Read ADC for this row
            mux_select_row(row);

            // Wait for column select to settle and propagate to ADC
            //wait_us(500 * 1000);

            // Interpret ADC value as rows
            adc10ksample_t voltage = read_adc();
            changed |= interpret_adc_row(current_matrix, voltage, col, row);
            break;
        }
        break;

        // Drive column high again
        drive_col(col, true);
    }

   return changed;
}

//bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//  // If console is enabled, it will print the matrix position and status of each key pressed
//#ifdef CONSOLE_ENABLE
//    uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
//#endif
//  return true;
//}

/**
 * Enable the ADC MUX
 *
 * TODO: Do we need a de-init? Probably not.
*/
static void adc_mux_init(void) {
    setPinOutput(MUX_ENABLE);
    writePinLow(MUX_ENABLE);

    setPinOutput(MUX_A);
    setPinOutput(MUX_B);
    setPinOutput(MUX_C);
}

/**
 * Overriding behavior of matrix_init from quantum/matrix.c
*/
void matrix_init_custom(void) {

    adc_mux_init();
    adc_gpio_init(ADC_CH2_PIN);

    // KS0 - KSO7 for Keyboard and Numpad
    setPinOutput(KSO0);
    setPinOutput(KSO1);
    setPinOutput(KSO2);
    setPinOutput(KSO3);
    setPinOutput(KSO4);
    setPinOutput(KSO5);
    setPinOutput(KSO6);
    setPinOutput(KSO7);
    // KS08 - KS015 for Keyboard only
    setPinOutput(KSO8);
    setPinOutput(KSO9);
    setPinOutput(KSO10);
    setPinOutput(KSO11);
    setPinOutput(KSO12);
    setPinOutput(KSO13);
    setPinOutput(KSO14);
    setPinOutput(KSO15);

    // Set unused pins to input to avoid interfering. They're hooked up to rows 5 and 6
    setPinInput(GP6);
    setPinInput(GP7);
}
