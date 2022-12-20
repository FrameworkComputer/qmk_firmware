// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdint.h>
#include "debug.h"
#include "matrix.h"
#include "print.h"
#include "quantum.h"

#define LOTUS_COLS 14

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
static void mux_select_col(int col) {
    int bits[] = {
        (col & 0x1) > 0,
        (col & 0x4) > 0,
        (col & 0x8) > 0
    };
    writePin(MUX_A, bits[0]);
    writePin(MUX_B, bits[1]);
    writePin(MUX_C, bits[2]);
}
static uint16_t adc_read(void) { return 0; }

/**
 * Based on the adc value, update the matrix for this column
 * */
static bool interpret_adc_col(matrix_row_t cur_matrix[], uint16_t adc_value, int col) {
    bool changed = false;

    printf("Col %d - ADC value:%04X", col, adc_value);
    for(uint8_t row = 0; row < MATRIX_ROWS; row++) {
        // TODO: Decode adc_value and set each row in this column
        uint8_t key_state = 0;

        cur_matrix[row] |= key_state ? 0 : (1 << col);
    }

    return changed;
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (int col = 0; col < LOTUS_COLS; col++) {
        // TODO: Map col index to GPIO
        // Drive column high so we can measure the resistors
        writePinHigh(col);
        wait_us(30); // Probably good idea to wait a bit

        // Read ADC for this row
        mux_select_col(col);
        uint16_t adc_value = adc_read();

        // Interpret ADC value as rows
        changed |= interpret_adc_col(current_matrix, adc_value, col);

        // Drive column low again
        writePinLow(col);
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
    adc_mux_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
}