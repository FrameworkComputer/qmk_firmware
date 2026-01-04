// Copyright 2022 Framework Computer
// SPDX-License-Identifier: L-2.0-or-later

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "debug.h"
#include "analog.h"
#include "print.h"
#include "quantum.h"
#include "hal_adc.h"
#include "chprintf.h"

#define adc12ksample_t int

// Rows to ADC input
#define KSI0 A0
#define KSI1 A1
#define KSI2 A2
#define KSI3 A3
#define KSI4 A4
#define KSI5 A5
#define KSI6 A6
#define KSI7 A7

// Columns to IOs
#define KSO0  C14
#define KSO1  C15
// Skip past ADC0-7
#define KSO2  B0
#define KSO3  B1
// Skip BOOT1
#define KSO4  B10
// Continue on other side
#define KSO5  B12
#define KSO6  B13
#define KSO7  B14
#define KSO8  B15
#define KSO9  A8
// Skip A9
#define KSO10 A10
// Skip A11-12
#define KSO11 A15
#define KSO12 B3
#define KSO13 B4
#define KSO14 B5
#define KSO15 B6
#define KSO16 B7
#define KSO17 B8

// Voltage threshold - anything below that counts as pressed
// 29000 = 2.9V * 10000
const adc12ksample_t ADC_THRESHOLD = (adc12ksample_t) 29000;

adc12ksample_t to_voltage(uint16_t sample) {
  int voltage = sample * 33000;
  // 12 bit ADC
  return voltage / 4095;
}

void print_as_float(adc12ksample_t sample) {
  // int digits = sample / 10000;
  // int decimals = sample % 10000;
  // uprintf("%d.%02d\n", digits, decimals);
}

/**
 * Tell RP2040 ADC controller to initialize a specific IO for ADC input
*/
void adc_gpio_init(int gpio) {
    // Enable pull-up on IO input so that we always have high input
    // Even on the rows that don't have the external pull-up.
    // Otherwise they would be floating.
    #define PAL_MODE_ADC_PULLUP           (PAL_MODE_INPUT_ANALOG | PAL_MODE_INPUT_PULLUP)
    palSetLineMode(gpio, PAL_MODE_ADC_PULLUP);
}

/**
 * Tell the mux to select a specific column
 *
 * Splits the positive integer (<=7) into its three component bits.
*/

/**
 * Based on the ADC value, update the matrix for this column
 * */
static bool interpret_adc_row(matrix_row_t cur_matrix[], adc12ksample_t voltage, int col, int row, adc12ksample_t threshold) {
    bool changed = false;

    // By default the voltage is high (3.3V)
    // When a key is pressed it causes the voltage to go down.
    // But because every key is connected in a matrix, pressing multiple keys
    // changes the voltage at every key again. So we can't check for a specific
    // voltage but need to have a threshold.
    bool key_state = false;
    if (voltage < threshold) {
        key_state = true;
    }

    if (key_state) {
        uprintf("Col %d - Row %d - State: %d, Voltage: ", col, row, key_state);
        print_as_float(voltage);
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
        uprintf("Keypress at KSO%d, KSI%d - %d.%dV\n", col, row, voltage/10000, voltage%10000);
    }
    cur_matrix[row] = new_row;

    return changed;
}

/**
 * Drive the IO for a column low or high.
 */
void drive_col(int col, bool high) {
    assert(col >= 0 && col <= MATRIX_COLS);
    int gpio = 0;
    switch (col) {
        case 0:
            gpio = KSO0;
            break;
        case 1:
            gpio = KSO1;
            break;
        case 2:
            gpio = KSO2;
            break;
        case 3:
            gpio = KSO3;
            break;
        case 4:
            gpio = KSO4;
            break;
        case 5:
            gpio = KSO5;
            break;
        case 6:
            gpio = KSO6;
            break;
        case 7:
            gpio = KSO7;
            break;
        case 8:
            gpio = KSO8;
            break;
        case 9:
            gpio = KSO9;
            break;
        case 10:
            gpio = KSO10;
            break;
        case 11:
            gpio = KSO11;
            break;
        case 12:
            gpio = KSO12;
            break;
        case 13:
            gpio = KSO13;
            break;
        case 14:
            gpio = KSO14;
            break;
        case 15:
            gpio = KSO15;
            break;
        case 16:
            gpio = KSO16;
            break;
        case 17:
            gpio = KSO17;
            break;
        default:
            // Not supposed to happen
            assert(false);
            return;
    }

// Don't drive columns on pico because we're using these IOs for other purposes
//#ifdef PICO_FL16
//    (void)gpio;
//    return;
//#endif

    //uprintf("Driving col %s %d, %d\n", high ? "HIGH" : "LOW ", col, gpio);
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
static adc12ksample_t read_adc(int row) {
    // Can't use analogReadPin because it gets rid of the internal pullup on
    // this pin, that we configure in matrix_init_custom
    pin_t pin = KSI0;
    switch (row) {
    case 0:
        pin = KSI0;
        break;
    case 1:
        pin = KSI1;
        break;
    case 2:
        pin = KSI2;
        break;
    case 3:
        pin = KSI3;
        break;
    case 4:
        pin = KSI4;
        break;
    case 5:
        pin = KSI5;
        break;
    case 6:
        pin = KSI6;
        break;
    case 7:
        pin = KSI7;
        break;
        // Not supposed to happen
        assert(false);
    }
    uint16_t val = adc_read(pinToMux(pin));
    return to_voltage(val);
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    return false;
    bool changed = false;

    adc12ksample_t voltages[MATRIX_ROWS][MATRIX_COLS] = {};

    //wait_us(500 * 1000);
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
            voltages[row][col] = read_adc(row);
        }

        // Drive column high again
        drive_col(col, true);
    }

    for (int row = 0; row < MATRIX_ROWS; row++) {
        uint8_t pressed_in_row = 0;
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (voltages[row][col] < ADC_THRESHOLD) {
                pressed_in_row += 1;
            }
        }
        for (int col = 0; col < MATRIX_COLS; col++) {
            adc12ksample_t threshold = ADC_THRESHOLD;
            switch (pressed_in_row) {
                case 0:
                case 1:
                    threshold = 10000; // 1.0V
                    break;
                case 2:
                    threshold = 20000; // 2.0V
                    break;
                case 3:
                    threshold = 25000; // 2.5V
                    break;
                default:
                    threshold = ADC_THRESHOLD;
                    break;
            }
            // Interpret ADC value as rows
            changed |= interpret_adc_row(current_matrix, voltages[row][col], col, row, threshold);
        }
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
 * Overriding behavior of matrix_init from quantum/matrix.c
*/
void matrix_init_custom(void) {
    return;
    adc_gpio_init(KSI0);
    adc_gpio_init(KSI1);
    adc_gpio_init(KSI2);
    adc_gpio_init(KSI3);
    adc_gpio_init(KSI4);
    adc_gpio_init(KSI5);
    adc_gpio_init(KSI6);
    adc_gpio_init(KSI7);

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
    setPinOutput(KSO16);
    setPinOutput(KSO17);
}
