// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdint.h>
#include "debug.h"
#include "matrix.h"
#include "analog.h"
#include "print.h"
#include "quantum.h"
#include "hal_adc.h"
#include "chprintf.h"
#include "matrix.h"

// Use raw ChibiOS ADC functions instead of those from QMK
// Using the QMK functions doesn't work yet
#define CHIBIOS_ADC TRUE

bool letsgo = false;
uint32_t prev_matrix_ts = 0;
float adc_voltage;
float temperature;

enum sample_state {
  s_never, // Never received a sample
  s_waiting, // Waiting for a new sample (at least one received)
  s_ready, // Received a sample, ready to be consumed
};
#define ADC_RESOLUTION 10
#define ADC_GRP_NUM_CHANNELS 2
#define ADC_GRP_BUF_DEPTH 2
static adcsample_t prev_samples[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP_NUM_CHANNELS * 2)];
static adcsample_t samples[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP_NUM_CHANNELS * 2)];
static enum sample_state adc_state;

// 3.3V with 12bit resolution
const float CONV_FACTOR = 3.3f / (1<<12);

// Mux GPIOs
#define MUX_A GP1
#define MUX_B GP2
#define MUX_C GP3
#define MUX_ENABLE GP4

#define ADC_CH0_PIN  GP26
#define ADC_CH1_PIN  GP27
#define ADC_CH2_PIN  GP28
#define ADC_CH3_PIN  GP29
#define ADC_THRESHOLD 3.0 / CONV_FACTOR

#define CALC_DIGITS 12
char calc_result[CALC_DIGITS+1] = "";

/*
 * Print two digits
 */
void print_float(float f) {
  // dtostrf doesn't seem to be available
  //dtostrf(temp, CALC_DIGITS, 2, calc_result);
  //uprintf("temp: %s\n", calc_result);
  int digits = (int)f;
  int decimals = (int)(f * 100) % 100;
  uprintf("%d.%d\n", digits, decimals);
}

float average_samples(adcsample_t s[], int channel) {
  float sum = 0;
  for (int i = 0; i < ADC_GRP_NUM_CHANNELS; i++) {
    sum += (float)s[channel + (i * ADC_GRP_BUF_DEPTH)];
  }
  return sum / ADC_GRP_BUF_DEPTH;
}

void print_samples(adcsample_t s[]) {
  // Samples go from 0 to 4095
  uprintf("Raw ADC samples: %d, %d, %d, %d\n", samples[0], samples[1], samples[2], samples[3]);

  print("Temp: ");
  print_float(temperature);
   
  uprintf("ADC Voltage: ");
  print_float(adc_voltage);
}

void handle_sample(void) {
  float adv_avg = average_samples(samples, 0);
  float temp_avg = average_samples(samples, 1);

  // Convert them to voltage (0 to 3.3V)
  float adc_v = adv_avg * CONV_FACTOR;
  float temp_v = temp_avg * CONV_FACTOR;
  // Convert to real temperature based on RP2040 datasheet
  temperature = 27.0 - (temp_v - 0.706)/0.001721;
  adc_voltage = adc_v;

  //print("Current:")
  //print_samples(samples);
}

/*
 * Call back function which called when ADC is finished.
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
}

const ADCConversionGroup adcConvGroup = {
  .circular     = false,
  .num_channels = ADC_GRP_NUM_CHANNELS,
  .end_cb       = &adc_end_callback,
  .error_cb     = &adc_error_callback,
  // CH2 is the keyboard matrix, CH4 is the temp sensor
  .channel_mask = RP_ADC_CH2 | RP_ADC_CH4,
};

// If adcConvGroup.circular is true, this will just keep going
void trigger_adc(void) {
    adcStartConversion(&ADCD1, &adcConvGroup,
                       samples, ADC_GRP_BUF_DEPTH);
}

void factory_trigger_adc(void) {
    if (!letsgo) {
      print("Factory triggered ADC\n");
      letsgo = true;
    }
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

    uprintf("Col %d - Row %d - ADC value:%04X, Voltage: %d\n", col, row, adc_value, voltage);
// Don't update  matrix on Pico to avoid messing with the debug system
// Can't attach the matrix anyways
#ifdef PICO_LOTUS
    (void)key_state;
#else
    cur_matrix[row] |= key_state ? 0 : (1 << col);
#endif

    return changed;
}

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
        default:
            // Not supposed to happen
            assert(false);
            return;
    }

// Don't drive columns on pico because we're using these GPIOs for other purposes
#ifdef PICO_LOTUS
    (void)gpio;
#else
    if (high) {
        // TODO: Could set up the pins with `setPinOutputOpenDrain` instead
        writePinHigh(gpio);
    } else {
        writePinLow(gpio);
    }
#endif
}

/**
 * Overriding behavior of matrix_scan from quantum/matrix.c
*/
bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    print("scan\n");
    uint32_t current_ts = timer_read32();
    if (prev_matrix_ts) {
      uint32_t delta = current_ts - prev_matrix_ts;
      uprintf("%lu ms (%ld Hz)\n", delta, 1000 / delta);
    }
    prev_matrix_ts = current_ts;
    chThdSleepMilliseconds(5);
#if !CHIBIOS_ADC
    uint16_t val = analogReadPin(ADC_CH2_PIN);
    adc_voltage = val * CONV_FACTOR;
#else
    if (letsgo) {
      factory_trigger_adc();
    }

    // Interrupt-driven
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
    //  trigger_adc();
    //}
#endif // CHIBIOS_ADC
    uprintf("Temperature: ");
    print_float(temperature);
    uprintf("ADC Voltage: ");
    print_float(adc_voltage);

    for (int col = 0; col < MATRIX_COLS; col++) {
        break;
        // Drive column low so we can measure the resistors on each row in this column
        drive_col(col, false);
        for (int row = 0; row <= MATRIX_ROWS; row++) {

            // Read ADC for this row
            mux_select_row(row);

            wait_us(30); // Wait for column select and ADC to settle

            uint16_t adc_value = 0;//adc_read();

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
    backlight_enable(); // To signal "live-ness"

    adc_mux_init();
    adc_gpio_init(ADC_CH2_PIN);
    adc_state = s_never;

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

    // Start automatic conversion
    //chThdSleepMilliseconds(100);
    //trigger_adc();

    // TODO: Not sure we ever need to stop. Perhaps to save power.
    // adcStopConversion(&ADCD1);
}
