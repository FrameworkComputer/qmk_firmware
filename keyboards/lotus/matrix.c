// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdint.h>
#include "debug.h"
#include "matrix.h"
#include "print.h"
#include "quantum.h"

#define ADC_THRESHOLD 3

/**
 * Tell RP2040 ADC controller to initialize a specific GPIO for ADC input
*/
void adc_gpio_init(int gpio) {
    assert(gpio >= 26 && gpio <= 29);
    // TODO: Implement
}

/**
 * Tell RP2040 ADC controller to read from a specific ADC channel
*/
void adc_select_input(int adc_channel) {
    assert(adc_channel >= 0 && adc_channel <= 4);
    // TODO: Implement
}

// Mux GPIOs
#define MUX_A 1
#define MUX_B 2
#define MUX_C 3
#define MUX_ENABLE 4
// Mux output
#define ADC_IN 28

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
    // X4 - KSI4
    // X5 - KSI5
    // X6 - KSI6
    // X7 - KSI7
    int index = 0;
    switch (row) {
        case 0:
            index = 2;
        case 1:
            index = 0;
        case 2:
            index = 1;
        default:
            index = row;
    }

    int bits[] = {
        (index & 0x1) > 0,
        (index & 0x4) > 0,
        (index & 0x8) > 0
    };
    writePin(MUX_A, bits[0]);
    writePin(MUX_B, bits[1]);
    writePin(MUX_C, bits[2]);
}
static uint16_t adc_read(void) { return 0; }

/**
 * Based on the adc value, update the matrix for this column
 * */
static bool interpret_adc_row(matrix_row_t cur_matrix[], uint16_t adc_value, int col, int row) {
    bool changed = false;

    // TODO: Convert adc value to voltage
    uint16_t voltage = adc_value;

    // By default the voltage is high (3.3V)
    // When a key is pressed it causes the voltage to go down.
    // But because every key is connected in a matrix, pressing multiple keys
    // changes the voltage at every key again. So we can't check for a specific
    // voltage but need to have a threshold.
    uint8_t key_state = 0;
    if (voltage < ADC_THRESHOLD) {
        key_state = 1;
    }

    printf("Col %d - Row %d - ADC value:%04X, Voltage: %d\n", col, row, adc_value, voltage);
    cur_matrix[row] |= key_state ? 0 : (1 << col);

    return changed;
}

void drive_col(int col, bool high) {
    assert(col >= 0 && col <= MATRIX_COLS);
    int gpio = 0;
    switch (col) {
        case 0:
            gpio = 8;
            break;
        case 1:
            gpio = 9;
            break;
        case 2:
            gpio = 10;
            break;
        case 3:
            gpio = 11;
            break;
        case 4:
            gpio = 12;
            break;
        case 5:
            gpio = 13;
            break;
        case 6:
            gpio = 14;
            break;
        case 7:
            gpio = 15;
            break;
        case 8:
            gpio = 21;
            break;
        case 9:
            gpio = 20;
            break;
        case 10:
            gpio = 19;
            break;
        case 11:
            gpio = 18;
            break;
        case 12:
            gpio = 17;
            break;
        case 13:
            gpio = 16;
            break;
        default:
            // Not supposed to happen
            assert(false);
            return;
    }

    if (high) {
        // TODO: Could set up the pins with `setPinOutputOpenDrain` instead
        writePinHigh(gpio);
    } else {
        writePinLow(gpio);
    }
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        // Drive column low so we can measure the resistors on each row in this column
        drive_col(col, false);
        for (int row = 0; row <= MATRIX_ROWS; row++) {

            // Read ADC for this row
            mux_select_row(row);

            wait_us(30); // Wait for column select and ADC to settle

            uint16_t adc_value = adc_read();

            // Interpret ADC value as rows
            changed |= interpret_adc_row(current_matrix, adc_value, col, row);
        }

        // Drive column high again
        drive_col(col, true);
    }

    return changed;
}

/**
 * Enable the ADC MUX
 * 
 * TODO: Do we need a de-init? Probably not.
*/
static void adc_mux_init(void) {
    writePinHigh(MUX_ENABLE);
}

/**
 * Overriding behavior of matrix_init from quantum/matrix.c
*/
void matrix_init_custom(void) {
    print("Initializing Lotus\n");
    adc_mux_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
}
