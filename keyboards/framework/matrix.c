// Copyright 2023 Stefan Kerkmann (KarlK90)
// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

// We use the pico-sdk adc implementation as it uses a simple busy wait for the
// ADC readout, which has much less overhead compared to the ChibiOS
// implementation which sends the running thread sleeping and wakes it using an
// interrupt.
#include "hardware/adc.h"
// Ugly workaround for pico-sdk namespace conflicts with ChibiOS
#undef GPIO_IN
#undef GPIO_OUT
#include "framework.h"

// ADC pin
static const uint32_t adc_channel = 2U;
static const pin_t    adc_pin     = GP28;

// Keyboard matrix
static const pin_t col_pins[MATRIX_COLS] = {
    GP8,  // Col 0 - KSO0
    GP9,  // Col 1 - KSO1
    GP10, // Col 2 - KSO2
    GP11, // Col 3 - KSO3
    GP12, // Col 4 - KSO4
    GP13, // Col 5 - KSO5
    GP14, // Col 6 - KSO6
    GP15, // Col 7 - KSO7
#if MATRIX_COLS > 8
    GP21, // Col 8 - KSO8
    GP20, // Col 9 - KSO9
    GP19, // Col 10 - KSO10
    GP18, // Col 11 - KSO11
    GP17, // Col 12 - KSO12
    GP16, // Col 13 - KSO13
    GP23, // Col 14 - KSO14
    GP22  // Col 15 - KSO15
#endif
};

// ADC mux pins
static const pin_t mux_pins[] = {
    GP1, // Mux A
    GP2, // Mux B
    GP3  // Mux C
};
static const pin_t mux_enable = GP4;

// As the mux pins are in a consecutive order GP1, GP2 and GP3 integers up to
// the number 7 can be directly mapped onto the GPIO port register. We just
// have to shift the row number to the left by 1.
static const ioportmask_t row_mux_mask                  = (1 << GP1 | 1 << GP2 | 1 << GP3);
static const ioportmask_t row_mux_mask_lut[MATRIX_ROWS] = {
    2 << 1, // Row 0 - KSI2
    0 << 1, // Row 1 - KSI0
    1 << 1, // Row 2 - KSI1
    3 << 1, // Row 3 - KSI3
#if MATRIX_ROWS > 4
    4 << 1, // Row 4 - KSI4
    5 << 1, // Row 5 - KSI5
    6 << 1, // Row 6 - KSI6
    7 << 1  // Row 7 - KSI7
#endif
};

/**
 * @brief Helper function to wait until a pin  has reached the wanted target
 * state. This only works for Push-Pull pins with enabled input stage.
 */
static void __time_critical_func(matrix_wait_for_pin)(pin_t pin, uint8_t target_state) {
    rtcnt_t start = chSysGetRealtimeCounterX();
    rtcnt_t end   = start + MS2RTC(REALTIME_COUNTER_CLOCK, 20);
    while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), start, end)) {
        if (readPin(pin) == target_state) {
            return;
        }
    }
}

/**
 * @brief Helper function to wait until the IO port has reached the wanted
 * target state. This only works for Push-Pull pins with enabled input stage.
 */
static void __time_critical_func(matrix_wait_for_port)(ioportmask_t target_bitmask) {
    rtcnt_t start = chSysGetRealtimeCounterX();
    rtcnt_t end   = start + MS2RTC(REALTIME_COUNTER_CLOCK, 20);
    while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), start, end)) {
        if (palReadGroup(IOPORT1, target_bitmask, 0) == target_bitmask) {
            return;
        }
    }
}

/**
 * @brief Starts the ADC peripheral and initializes the input pin.
 *
 */
static void setup_adc(void) {
    // Start ADC peripheral
    adc_init();

    // Setup ADC pin: Enable pull-up on GPIO input so that we always have high
    // input Even on the rows that don't have the external pull-up. Otherwise
    // they would be floating.
    palSetLineMode(adc_pin, PAL_MODE_INPUT_ANALOG | PAL_RP_PAD_PUE);
    adc_select_input(adc_channel);
}

/**
 * @brief Prepares the used ADC muxing pins for output.
 *
 */
static void setup_adc_pinmux(void) {
    // Setup ADC pinmuxing
    setPinOutput(mux_enable);
    writePinLow(mux_enable);
    // TODO: MUX activated in keyboard init code, should this be moved here?

    for (int mux = 0; mux < ARRAY_SIZE(mux_pins); mux++) {
        setPinOutput(mux_pins[mux]);
    }
}

/**
 * @brief Prepares the used keyboard matrix GPIO pins for output and disables
 * unused pins.
 *
 */
static void setup_matrix(void) {
    // KS0 - KSO7 for Keyboard and Numpad
    // KS08 - KS015 for Keyboard only
    for (int col = 0; col < MATRIX_COLS; col++) {
        setPinOutput(col_pins[col]);
        writePinHigh(col_pins[col]);
    }

    // Set unused pins to input to avoid interfering. They're hooked up to rows 5 and 6.
    setPinInput(GP6);
    setPinInput(GP7);
}

/**
 * Overriding behavior of matrix_init from quantum/matrix.c
 */
void matrix_init_custom(void) {
    setup_adc();
    setup_adc_pinmux();
    setup_matrix();
    backlight_enable(); // To signal "live-ness"
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
 */
bool matrix_scan_custom(matrix_row_t current_matrix[MATRIX_ROWS]) {
    // Go through every matrix column (KSO) and drive them low individually
    // Then go through every matrix row (KSI), select it with the mux and check
    // their ADC value
    bool changed = false;
    for (int col = 0; col < MATRIX_COLS; col++) {
        // Select column by driving it low so we can measure the
        // resistors on each row in this column.
        writePinLow(col_pins[col]);
        matrix_wait_for_pin(col_pins[col], 0);

        for (int row = 0; row < MATRIX_ROWS; row++) {
            // To read the ADC for this row we have to:
            // 1. Select the correct MUX configuration for this row
            palWriteGroup(IOPORT1, row_mux_mask, 0, row_mux_mask_lut[row]);
            matrix_wait_for_port(row_mux_mask_lut[row]);

            // 2. Read the current ADC voltage for that row By default the
            // voltage is high (3.3V) When a key is pressed it causes the
            // voltage to go down. But because every key is connected in a
            // matrix, pressing multiple keys changes the voltage at every key
            // again. So we can't check for a specific voltage but need to have
            // a threshold.
            const matrix_row_t old_row = current_matrix[row];

            // For a pressed threshold of 2.9V = (2.9 / 3.3) * 2**10 = ~900;
            const bool pressed = adc_read() <= 900U;
            if (pressed) {
                current_matrix[row] |= (1 << col);
            } else {
                current_matrix[row] &= ~(1 << col);
            }

            changed |= current_matrix[row] != old_row;
        }

        // De-select the column by driving it high again
        writePinHigh(col_pins[col]);
        matrix_wait_for_pin(col_pins[col], 1);
    }

    return changed;
}
