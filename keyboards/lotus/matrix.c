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
#include "lotus.h"

// Use raw ChibiOS ADC functions instead of those from QMK
// Using the QMK functions doesn't work yet
#define CHIBIOS_ADC TRUE

#define adc10ksample_t int

bool letsgo = false;
uint32_t prev_matrix_ts = 0;
adc10ksample_t adc_voltage;
adc10ksample_t temperature;

enum sample_state {
  s_never, // Never received a sample
  s_waiting, // Waiting for a new sample (at least one received)
  s_ready, // Received a sample, ready to be consumed
};
#define ADC_RESOLUTION 10
#define ADC_GRP_NUM_CHANNELS 2
#define ADC_GRP_BUF_DEPTH 2
static adcsample_t prev_samples[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH)];
static adcsample_t samples[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH)];
static enum sample_state adc_state;

// Mux GPIOs
#define MUX_A GP1
#define MUX_B GP2
#define MUX_C GP3
#define MUX_ENABLE GP4

#define ADC_CH2_PIN  GP28
// Voltage threshold - TODO: Need to adjust
const adc10ksample_t ADC_THRESHOLD = (adc10ksample_t) 3.0 * 10000;

adc10ksample_t to_voltage(adcsample_t sample) {
  // 1241 = (1 << 12) * 10000 / (3.3 * 10000)
  int voltage = sample * 10000;
  return voltage / 1241;
}

adc10ksample_t to_temp(adcsample_t sample) {
  int temp = sample * 10000;
  // Scaled up by 10000
  //temperature = 27.0 - (temp - 0.706)/0.001721;
  return 270000 - (((temp / 8) - 7060) /17);//.21;
}
void print_as_float(adc10ksample_t sample) {
  int digits = sample / 10000;
  int decimals = sample % 10000;
  uprintf("%d.%02d\n", digits, decimals);
}

/**
 * Average the mulitiple samples due to depth>1 together to a single value
 */
adcsample_t average_samples(adcsample_t s[], int channel) {
  adcsample_t sum = 0;
  assert(ADC_GRP_BUF_DEPTH == 2);

  for (int i = 0; i < ADC_GRP_NUM_CHANNELS; i++) {
    sum += s[channel + (i * ADC_GRP_BUF_DEPTH)];
  }

  return sum / ADC_GRP_NUM_CHANNELS;
}

void print_samples(adcsample_t s[]) {
  // Samples go from 0 to 4095
  //uprintf("Raw ADC samples: %d, %d, %d, %d\n", samples[0], samples[1], samples[2], samples[3]);

  print("Temp: ");
  print_as_float(temperature);
   
  uprintf("ADC Voltage: ");
  print_as_float(adc_voltage);
}

/**
 * Convert the ADC samples to meaningful values
 *
 * Should be called after receiving a set of samples from the ADC
 */
void handle_sample(void) {
  adcsample_t adv_avg = average_samples(samples, 0);
  adcsample_t temp_avg = average_samples(samples, 1);

  // Uses global variables because it might be called from an interrupt handler
  // But we might not want to act upon them from there.
  // Convert to real temperature based on RP2040 datasheet
  temperature = to_temp(temp_avg);
  adc_voltage = to_voltage(adv_avg);

  //print_samples(samples);
}

/*
 * Call back function which is called when ADC is finished.
 */
void adc_end_callback(ADCDriver *adcp) {
  (void)adcp;
  adc_state = s_ready;
  //print("adc_end_callback\n");

  //handle_sample();
}

/*
 * Call back function which called when ADC gives some error.
 */
void adc_error_callback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  uprintf("error: %ld\n", err);
  assert(false);
}

const ADCConversionGroup adcConvGroup = {
  .circular     = false,
  .num_channels = ADC_GRP_NUM_CHANNELS,
  .end_cb       = &adc_end_callback,
  .error_cb     = &adc_error_callback,
  // CH2 is the keyboard matrix, CH4 is the temp sensor
  .channel_mask = RP_ADC_CH2 | RP_ADC_CH4,
};

/**
 * Trigger an ADC conversion. When done, the callback is called.
 *
 * Never blocks.
 * If adcConvGroup.circular is true, callbacks will keep coming.
 */
void trigger_adc(void) {
    adcStartConversion(&ADCD1, &adcConvGroup,
                       samples, ADC_GRP_BUF_DEPTH);
}

/**
 * Trigger a single, blocking ADC conversion
 */
void factory_trigger_adc(void) {
    if (!letsgo) {
      print("Factory triggered ADC\n");
      letsgo = true;
    }
    // adcConvert brings frequency from 6kHz down to 360Hz
    adcConvert(&ADCD1, &adcConvGroup,
               samples, ADC_GRP_BUF_DEPTH);
    //print("After adcConvert\n");
    memcpy(prev_samples, samples, sizeof(samples));
    handle_sample();
    //print("After handle_sample");
}

/**
 * Tell RP2040 ADC controller to initialize a specific GPIO for ADC input
*/
void adc_gpio_init(int gpio) {
    assert(gpio >= GP26 && gpio <= GP28);
    palSetLineMode(gpio, PAL_MODE_INPUT_ANALOG);
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
    uprintf("Mux A: %d, B: %d, C: %d, KSI%d, X%d\n", bits[0], bits[1], bits[2], row, index);
    writePin(MUX_A, bits[0]);
    writePin(MUX_B, bits[1]);
    writePin(MUX_C, bits[2]);
}

#if 0
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
    uint8_t key_state = 0;
    if (voltage < ADC_THRESHOLD) {
        key_state = 1;
    }

    uprintf("Col %d - Row %d - State: %d, Voltage: ", col, row, key_state);
    print_as_float(voltage);

// Don't update  matrix on Pico to avoid messing with the debug system
// Can't attach the matrix anyways
//#ifdef PICO_LOTUS
    (void)key_state;
    return false;
//#endif

    matrix_row_t new_row = cur_matrix[row] | key_state ? (1 << col) : 0;
    changed = cur_matrix[row] == new_row;
    cur_matrix[row] = new_row;

    return changed;
}
#endif

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
//#ifdef PICO_LOTUS
//    (void)gpio;
//    return;
//#endif

    uprintf("Driving col %s %d, GP%d\n", high ? "HIGH" : "LOW ", col, gpio);
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
static void read_adc(void) {
#if !CHIBIOS_ADC
    uint16_t val = analogReadPin(ADC_CH2_PIN);
    adc_voltage = to_voltage(val);
#else
    if (letsgo) {
      factory_trigger_adc();
    }

    // Interrupt-driven
    // Even the "blocking" one suspends the thread, so we shouldn't need this
    //if (adc_state == s_ready) {
    //  print("new sample\n");

    //  memcpy(prev_samples, samples, sizeof(samples));

    //  adc_state = s_waiting;

    //  // Works if both are commented out.
    //  // Doesn't work if we don't sleep at all or sleep less than 300ms.
    //  // Then we seemingly are stuck, no prints and doesn't respond to raw HID commands.
    //  //
    //  // Also works if we never trigger ADC at all
    //  //hThdSleepMilliseconds(300);
    //  // Trigger non-blocking ADC read that will be handled by and interrupt callback
    //  trigger_adc();
    //}
#endif // CHIBIOS_ADC

    //uprintf("Temperature: ");
    //print_as_float(temperature);
    //uprintf("ADC Voltage: ");
    //print_as_float(adc_voltage);
}

/**
 * Handle the host going to sleep or the keyboard being idle
 * If the host is asleep the keyboard should reduce the scan rate and turn backlight off.
 *
 * If the host is awake but the keyboard is idle it should enter a low-power state
 */
void handle_idle(void) {
    bool asleep = readPin(SLEEP_GPIO);
    (void)asleep;
    //uprintf("Host asleep: %d\n", asleep);

    // TODO: Implement idle behavior
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    //print("scan\n");
    wait_us(500 * 1000);

    uint32_t current_ts = timer_read32();
    if (prev_matrix_ts) {
      //uint32_t delta = current_ts - prev_matrix_ts;
      //uprintf("%lu ms (%ld Hz)\n", delta, 1000 / delta);
    }
    prev_matrix_ts = current_ts;

    handle_idle();

    // Drive all
    for (int col = 0; col < MATRIX_COLS; col++) {
        drive_col(col, true);
    }
    drive_col(4, false);

    // Striped red/black
    //
    // Shows low when not pressing. High when pressing. REVERSED!
    //
    // If no col is driven high.     No cols work. (Row always low)
    // If all cols are driven high.  Col 4 works. (high when pressed)
    // If just col 4 is driven high. Col 4 works. (high when pressed)
    //mux_select_row(3);

    // Black
    // Shows low when not pressing. High when pressing. REVERSED!
    //
    // Correctly works only for the col that is driven high. REVERSED!
    // If all cols are driven high. All cols work.   (high when pressed)
    // If no col is driven high.    No cols work     (high when pressed)
    mux_select_row(2);

    // Red
    //
    // High when not pressing. Low when pressing.
    // If no col is driven low.     No cols work.
    // If all cols are driven low.  All cols work. (low when pressed)
    // If just col 0 is driven low. Only col 0/GP8 works
    // If just col 1 is driven low. Only col 1/GP9 works
    // If just col 2 is driven low. Only col 2/GP10 works
    // If just col 3 is driven low. Only col 3/GP11 works
    // If just col 4 is driven low. Only col 4/GP12 works
    // If just col 5 is driven low. Only col 5/GP13 works
    // If just col 6 is driven low. Only col 6/GP14 works
    // If just col 7 is driven low. Only col 7/GP15 works
    //mux_select_row(1);

    // Blue.
    // High when not pressing. Low when pressing.
    // 
    // If no col is driven low.     No cols work.
    // If all cols are driven low.  All cols work. (low when pressed)
    // If just col 0 is driven low. Only col 0/GP8 works
    // If just col 1 is driven low. Only col 1/GP9 works
    // If just col 2 is driven low. Only col 2/GP10 works
    // If just col 3 is driven low. Only col 3/GP11 works
    // If just col 4 is driven low. Only col 4/GP12 works
    // If just col 5 is driven low. Only col 5/GP13 works
    // If just col 6 is driven low. Only col 6/GP14 works
    // If just col 7 is driven low. Only col 7/GP15 works
    //mux_select_row(0);

    wait_us(100);
    read_adc();
    uprintf("ADC Voltage: ");
    print_as_float(adc_voltage);

    //for (int col = 0; col < MATRIX_COLS; col++) {
    //    // Drive column high so we can measure the resistors on each row in this column
    //    drive_col(col, true);
    //    for (int row = 0; row < MATRIX_ROWS; row++) {
    //        print("\n");
    //        // Read ADC for this row
    //        mux_select_row(row);

    //        // Wait for column select to settle and propagate to ADC
    //        wait_us(500 * 1000);

    //        read_adc();

    //        // Interpret ADC value as rows
    //        changed |= interpret_adc_row(current_matrix, adc_voltage, col, row);
    //    }

    //    // Drive column low again
    //    //drive_col(col, false);
    //}

   return changed;
}

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
    print("Initializing Lotus\n");
    backlight_enable(); // To signal "live-ness"

    adc_mux_init();
    adc_gpio_init(ADC_CH2_PIN);
    adc_state = s_never;

    // KS0 - KSO7 for Keyboard and Numpad
    setPinOutput(GP8);
    setPinOutput(GP9);
    setPinOutput(GP10);
    setPinOutput(GP11);
    setPinOutput(GP12);
    setPinOutput(GP13);
    setPinOutput(GP14);
    setPinOutput(GP15);

    // KS08 - KS015 for Keyboard only
    setPinOutput(GP21);
    writePinLow(GP21);
    setPinOutput(GP20);
    writePinLow(GP20);
    setPinOutput(GP19);
    writePinLow(GP19);
    setPinOutput(GP18);
    writePinLow(GP18);
    setPinOutput(GP17);
    writePinLow(GP17);
    setPinOutput(GP16);
    writePinLow(GP16);
    setPinOutput(GP23);
    writePinLow(GP23);
    setPinOutput(GP22);
    writePinLow(GP22);

    const ADCConfig adcConfig = {
        // Default clock divider
        .div_int  = 0,
        .div_frac = 0,
        // Don't shift FIFO results
        .shift    = false,
    };
    adcStart(&ADCD1, &adcConfig);

    // For testing enable temp sensor
    adcRPEnableTS(&ADCD1);

    // Start ADC conversion immediately. Don't wait for factory command to enable
    // For debugging it's sometimes useful to start later because the first
    // prints won't show on the console. So if it hangs here, it's hard to tell
    // what's going wrong.
    letsgo = true;

    // Start automatic conversion
    //chThdSleepMilliseconds(100);
    //trigger_adc();

    // TODO: Not sure we ever need to stop. Perhaps to save power.
    // adcStopConversion(&ADCD1);
}
