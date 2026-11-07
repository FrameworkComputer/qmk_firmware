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
bool prev_lid_closed = false;

/**
 * Limit how often the matrix is scanned.
 *
 * Without this the QMK main loop never yields, so the CPU is busy 100% of the
 * time and the WFI idle hook (CORTEX_ENABLE_WFI_IDLE) almost never runs.
 * By sleeping the main thread until the next scan is due, ChibiOS switches to
 * the idle thread, which executes WFI until the next interrupt (systick, USB,
 * ...).
 *
 * The timestamp is taken at the start of the scan, so the scan time itself
 * counts towards the interval. If the scan and the rest of the main loop take
 * longer than the interval, no sleep happens at all.
 */
static void matrix_scan_rate_limit(uint32_t interval_us) {
    static systime_t last_scan = 0;
    static bool      first     = true;

    if (interval_us > 0 && !first) {
        sysinterval_t interval = TIME_US2I(interval_us);
        sysinterval_t elapsed  = chVTTimeElapsedSinceX(last_scan);
        // chThdSleep must not be called with 0 (TIME_IMMEDIATE)
        if (elapsed < interval) {
            chThdSleep(interval - elapsed);
        }
    }

    first     = false;
    last_scan = chVTGetSystemTimeX();
}

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
static bool interpret_adc_row(matrix_row_t cur_matrix[], adc10ksample_t voltage, int col, int row, adc10ksample_t threshold) {
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

#ifdef FRAMEWORK_MATRIX_DEBUG_CONTINUOUS
    // Print on every scan while the key is held down. Very noisy and costs a
    // lot of CPU time (~1000 prints per second per key).
    if (key_state) {
        uprintf("Col %d - Row %d - State: %d, Voltage: ", col, row, key_state);
        print_as_float(voltage);
    }
#endif

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
    // Only print when the state changes, not on every scan while held
    if (changed) {
        uprintf("Key%s at KSO%d, KSI%d - %d.%04dV\n", key_state ? "press" : "release", col, row, voltage/10000, voltage%10000);
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

#ifdef FRAMEWORK_FAST_ADC
// Busy-wait cycles between selecting the mux input and starting the
// conversion, to give the analog input time to settle.
// The ChibiOS driver path took roughly this long between the mux switch and
// starting the conversion (estimated, not measured).
#    ifndef FRAMEWORK_FAST_ADC_SETTLE_CYCLES
#        define FRAMEWORK_FAST_ADC_SETTLE_CYCLES 64
#    endif

/**
 * Set up the RP2040 ADC for polled single conversions on ADC_CH2_PIN.
 *
 * Bypasses the ChibiOS ADC driver (adcStart is never called, so the FIFO
 * and its IRQ stay disabled). The ADC was already taken out of reset by
 * adc_lld_init during halInit.
 */
static void fast_adc_init(void) {
    hal_lld_peripheral_unreset(RESETS_ALLREG_ADC);
    ADC->CS = ADC_CS_EN | (((uint32_t)pinToMux(ADC_CH2_PIN).input << ADC_CS_AINSEL_Pos) & ADC_CS_AINSEL_Msk);
    while (!(ADC->CS & ADC_CS_READY)) {
    }
}

/**
 * Do a single conversion by polling the ADC.
 *
 * With the ChibiOS driver every conversion goes through an IRQ and two
 * thread context switches, which cost more CPU time than the 2us
 * (96 cycles of the 48MHz ADC clock) conversion itself. Polling is shorter
 * in total, so the full matrix scan finishes a lot faster and the MCU can go
 * back to sleep sooner.
 */
static uint16_t fast_adc_read(void) {
    for (volatile uint32_t i = 0; i < FRAMEWORK_FAST_ADC_SETTLE_CYCLES / 4; i++) {
    }
    ADC->SET.CS = ADC_CS_START_ONCE;
    // READY goes low when the conversion starts and high when it's done
    while (!(ADC->CS & ADC_CS_READY)) {
    }
    // 12 bit result, rest of the code expects 10 bit
    return (ADC->RESULT & 0xFFF) >> 2;
}
#endif

/**
 * Read a value from the ADC and print some debugging details
 */
static adc10ksample_t read_adc(void) {
#ifdef FRAMEWORK_FAST_ADC
    uint16_t val = fast_adc_read();
#else
    // Can't use analogReadPin because it gets rid of the internal pullup on
    // this pin, that we configure in matrix_init_custom
    // uint16_t val = analogReadPin(ADC_CH2_PIN);
    uint16_t val = adc_read(pinToMux(ADC_CH2_PIN));
#endif
    return to_voltage(val);
}

/**
 * Quickly check whether any key at all is pressed.
 *
 * Drives all columns low at once and reads every row only once. That's
 * MATRIX_ROWS ADC conversions instead of MATRIX_ROWS * MATRIX_COLS for a full
 * scan (8 instead of 128 on the keyboard, 4 instead of 32 on the numpad).
 *
 * The check can't miss a key that a full scan would find:
 * Every row is pulled up and only connected to the (actively driven) columns
 * through the pressed keys. So the row voltage is a weighted average of the
 * pull-up voltage and the voltages of the columns that it's connected to. With
 * all columns driven low, that average can only be lower than in the full scan
 * where all but one column are driven high. Since the full scan uses
 * thresholds of at most ADC_THRESHOLD, if no row is below ADC_THRESHOLD here,
 * the full scan would not detect any key either.
 *
 * False positives (e.g. a row without external pull-up that hasn't charged up
 * yet) are harmless, they just cause a full scan.
 */
static bool any_key_pressed(void) {
    bool pressed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        drive_col(col, false);
    }

    for (int row = 0; row < MATRIX_ROWS; row++) {
        mux_select_row(row);
        if (read_adc() < ADC_THRESHOLD) {
            pressed = true;
            break;
        }
    }

    // Deselect all columns again
    for (int col = 0; col < MATRIX_COLS; col++) {
        drive_col(col, true);
    }

    return pressed;
}

/**
 * Handle the host going to sleep or the keyboard being idle
 * TODO: If the host is asleep the keyboard should reduce the scan rate and turn backlight off.
 *
 * TODO: If the host is awake but the keyboard is idle it should enter a low-power state
 */
bool handle_idle(void) {
    bool lid_closed = !readPin(SLEEP_GPIO);
    bool lid_changed = lid_closed != prev_lid_closed;
    prev_lid_closed = lid_closed;

    /* If lid is closed and the keyboard is not currently suspended, we need to
     * suspend the keyboard.
     *
     * This does not normally happen, unless the user has configured their OS
     * to not suspend if lid is closed.
     *
     * There is no case where the lid is closed that the keyboard should stay active.
     **/
    if (lid_closed && !is_suspended) {
        suspend_power_down();
    }

    /* If the lid is open and the system is transitioning between sleep states,
     * QMK automatically handles suspending by monitoring USB suspend state.
     *
     * The only case where we need custom behavior is when the system was awake
     * with lid closed and then the lid is opened. In this case we need to wake
     * the keyboard.
     */
    if (!lid_closed && lid_changed) {
        suspend_wakeup_init();
    }

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

    // Ignore keypresses if lid closed
    return lid_closed;
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    adc10ksample_t voltages[MATRIX_ROWS][MATRIX_COLS] = {};

    handle_idle();

    /* If lid is closed, ignore keypresses. Otherwise pressing the on the lid (e.g. in
     * a backpack) might cause the system to wake up.
     **/
    if (!readPin(SLEEP_GPIO)) {
        // Nothing to do but to wait for the lid to open, don't busy-loop
        chThdSleepMilliseconds(FRAMEWORK_LID_CLOSED_POLL_MS);
        return false;
    }

    // While the host is asleep there's no need to scan as quickly, just
    // fast enough to wake it up on a keypress.
    matrix_scan_rate_limit(is_suspended ? FRAMEWORK_SUSPEND_SCAN_INTERVAL_US : FRAMEWORK_SCAN_INTERVAL_US);

    // Most of the time no key is pressed. Save power and time by not doing the
    // full scan in that case.
    if (!any_key_pressed()) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            if (current_matrix[row]) {
                current_matrix[row] = 0;
                changed = true;
            }
        }
        return changed;
    }

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
            // Debug for keyboard. Row 5 and 6 don't seem to work
            //print("\n");
            // Read ADC for this row
            mux_select_row(row);

            // Wait for column select to settle and propagate to ADC
            //wait_us(500 * 1000);

            voltages[row][col] = read_adc();
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
            adc10ksample_t threshold = ADC_THRESHOLD;
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
#ifdef FRAMEWORK_FAST_ADC
    fast_adc_init();
#endif

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
