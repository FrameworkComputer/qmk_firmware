/* Copyright 2017 Jason Williams
 * Copyright 2018 Jack Humbert
 * Copyright 2018 Yiancar
 * Copyright 2020 MelGeek
 * Copyright 2021 MasterSpoon
 * Copyright 2023 Framework Computer Inc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "aw20xxx.h"
#include "i2c_master.h"
#include "wait.h"
#include "print.h"
#include "debug.h"
#include <string.h>

// Set defaults for Timeout and Persistence
#ifndef AW20_TIMEOUT
#    define AW20_TIMEOUT 100
#endif
#ifndef AW20_PERSISTENCE
#    define AW20_PERSISTENCE 0
#endif

// Transfer buffer for TWITransmitData()
uint8_t g_twi_transfer_buffer[20];

// These buffers match the PWM & scaling registers.
// Storing them like this is optimal for I2C transfers to the registers.
uint8_t g_pwm_buffer[DRIVER_COUNT][AW20_MAX_LEDS];
bool    g_pwm_buffer_update_required[DRIVER_COUNT] = {false};

uint8_t g_scaling_buffer[DRIVER_COUNT][AW20_SCALING_SIZE];
bool    g_scaling_buffer_update_required[DRIVER_COUNT] = {false};

// For writing of mulitple register entries to make use of address auto increment
// Once the controller has been called and we have written the first bit of data
// the controller will move to the next register meaning we can write sequential blocks.
bool AW20_write_multi_registers(uint8_t addr, uint8_t *source_buffer, uint8_t buffer_size, uint8_t transfer_size, uint8_t start_reg_addr) {
    // Split the buffer into chunks to transfer
    for (int i = 0; i < buffer_size; i += transfer_size) {
        // Set the first entry of transfer buffer to the first register we want to write
        g_twi_transfer_buffer[0] = i + start_reg_addr;
        // Copy the section of our source buffer into the transfer buffer after first register address
        memcpy(g_twi_transfer_buffer + 1, source_buffer + i, transfer_size);

#if AW20_PERSISTENCE > 0
        for (uint8_t i = 0; i < AW20_PERSISTENCE; i++) {
            if (i2c_transmit(addr << 1, g_twi_transfer_buffer, transfer_size + 1, AW20_TIMEOUT) != 0) {
                return false;
            }
        }
#else
        if (i2c_transmit(addr << 1, g_twi_transfer_buffer, transfer_size + 1, AW20_TIMEOUT) != 0) {
            return false;
        }
#endif
    }
    return true;
}

bool AW20_read(uint8_t addr, uint8_t *value) {
    return i2c_transmit(addr << 1, value, 1, AW20_TIMEOUT) != 0;
}

bool AW20_write(uint8_t addr, uint8_t reg, uint8_t data) {
    // Set register address and register data ready to write
    g_twi_transfer_buffer[0] = reg;
    g_twi_transfer_buffer[1] = data;

//#if AW20_PERSISTENCE > 0
//    for (uint8_t i = 0; i < AW20_PERSISTENCE; i++) {
//        if (i2c_transmit(addr << 1, g_twi_transfer_buffer, 2, AW20_TIMEOUT) == 0) break;
//    }
//#else
    if (i2c_transmit(addr << 1, g_twi_transfer_buffer, 2, AW20_TIMEOUT) != 0) {
        return false;
    }
    return true;
}

uint8_t AW20_set_page(uint8_t addr, uint8_t page) {
    return AW20_write(addr, AW20198_REG_PAGE, page);
}

uint8_t AW20_set_config(uint8_t addr, uint8_t regaddr, uint8_t value) {
    uint8_t ret = AW20_set_page(addr, AW20198_PAGE_FUNC);
    if (ret) {
        return ret;
    }

    return AW20_write(addr, regaddr, value);
}

uint8_t  AW20_reset(uint8_t addr) {
    return AW20_set_config(addr, AW20_REG_RESET, AW20198_RESET_MAGIC);
}

void AW20_common_init(uint8_t addr, uint8_t ssr) {
    //uint8_t chip_id;
    //uint8_t rv;

    // chip_hwen()

    print("Initiating AW20198 reset");
    AW20_reset(addr); // soft_rst

    // chip_swen

    //rv = AW_20_read(AW20_REG_RSTN, &chip_id);

    // set_global_current(0xFF)
    // i2c_writebyte(MICXR, 0x04)

//    // Setup phase, need to take out of software shutdown and configure
//    // AW20_SSR_x is passed to allow Master / Slave setting where applicable
//
//    // Unlock the command register & select Function Register
//    IS31FL_unlock_register(addr, AW20_PAGE_FUNCTION);
//    // Set Configuration Register to remove Software shutdown
//    AW20_write(addr, AW20_REG_CONFIGURATION, AW20_CONFIGURATION);
//    // Set Golbal Current Control Register
//    AW20_write(addr, AW20_REG_GLOBALCURRENT, AW20_GLOBALCURRENT);
//    // Set Pull up & Down for SWx CSy
//    AW20_write(addr, AW20_REG_PULLDOWNUP, AW20_PULLDOWNUP);
//// Set Tempature Status
//#ifdef AW20_REG_TEMP
//    AW20_write(addr, AW20_REG_TEMP, AW20_TEMP);
//#endif
//    // Set Spread Spectrum Register, passed through as sets SYNC function
//    AW20_write(addr, AW20_REG_SSR, ssr);
//// Set PWM Frequency Enable Register if applicable
//#ifdef AW20_REG_PWM_ENABLE
//    AW20_write(addr, AW20_REG_PWM_ENABLE, AW20_PWM_ENABLE);
//#endif
//// Set PWM Frequency Register if applicable
//#ifdef AW20_REG_PWM_SET
//    AW20_write(addr, AW20_REG_PWM_SET, AW20_PWM_SET);
//#endif
//
    // Wait 10ms to ensure the device has woken up.
    wait_ms(10);
}

void AW20_common_update_pwm_register(uint8_t addr, uint8_t index) {
    if (g_pwm_buffer_update_required[index]) {
        // Queue up the correct page
        //IS31FL_unlock_register(addr, AW20_PAGE_PWM);
        AW20_set_page(addr, AW20198_PAGE_PWM);

        // Hand off the update to AW20_write_multi_registers
        AW20_write_multi_registers(addr, g_pwm_buffer[index], AW20_MAX_LEDS, AW20_PWM_TRF_SIZE, AW20_PWM_REG_1ST);

        // Update flags that pwm_buffer has been updated
        g_pwm_buffer_update_required[index] = false;
    }
}

#ifdef AW20_MANUAL_SCALING
void AW20_set_manual_scaling_buffer(void) {
    for (int i = 0; i < AW20_MANUAL_SCALING; i++) {
        aw20_led scale = g_aw20_scaling[i];
#    ifdef RGB_MATRIX_ENABLE
        if (scale.driver >= 0 && scale.driver < RGB_MATRIX_LED_COUNT) {
            aw20_led led = g_aw20_leds[scale.driver];

            g_scaling_buffer[led.driver][led.r] = scale.r;
            g_scaling_buffer[led.driver][led.g] = scale.g;
            g_scaling_buffer[led.driver][led.b] = scale.b;
#    elif defined(LED_MATRIX_ENABLE)
        if (scale.driver >= 0 && scale.driver < LED_MATRIX_LED_COUNT) {
            aw20_led led = g_aw20_leds[scale.driver];

            g_scaling_buffer[led.driver][led.v] = scale.v;
#    endif
            g_scaling_buffer_update_required[led.driver] = true;
        }
    }
}
#endif

void AW20_common_update_scaling_register(uint8_t addr, uint8_t index) {
    if (g_scaling_buffer_update_required[index]) {
        // Queue up the correct page
        //IS31FL_unlock_register(addr, AW20_PAGE_SCALING);
        AW20_set_page(addr, AW20198_PAGE_SCALE);

        // Hand off the update to AW20_write_multi_registers
        AW20_write_multi_registers(addr, g_scaling_buffer[index], AW20_SCALING_SIZE, AW20_SCALING_TRF_SIZE, AW20_SCL_REG_1ST);

        // Update flags that scaling_buffer has been updated
        g_scaling_buffer_update_required[index] = false;
    }
}

#ifdef RGB_MATRIX_ENABLE
// Colour is set by adjusting PWM register
void AW20_RGB_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index >= 0 && index < RGB_MATRIX_LED_COUNT) {
        aw20_led led = g_aw20_leds[index];

        g_pwm_buffer[led.driver][led.r]          = red;
        g_pwm_buffer[led.driver][led.g]          = green;
        g_pwm_buffer[led.driver][led.b]          = blue;
        g_pwm_buffer_update_required[led.driver] = true;
    }
}

void AW20_RGB_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        AW20_RGB_set_color(i, red, green, blue);
    }
}

// Setup Scaling register that decides the peak current of each LED
void AW20_RGB_set_scaling_buffer(uint8_t index, bool red, bool green, bool blue) {
    aw20_led led = g_aw20_leds[index];
    if (red) {
        g_scaling_buffer[led.driver][led.r] = AW20_SCAL_RED;
    } else {
        g_scaling_buffer[led.driver][led.r] = AW20_SCAL_RED_OFF;
    }
    if (green) {
        g_scaling_buffer[led.driver][led.g] = AW20_SCAL_GREEN;
    } else {
        g_scaling_buffer[led.driver][led.g] = AW20_SCAL_GREEN_OFF;
    }
    if (blue) {
        g_scaling_buffer[led.driver][led.b] = AW20_SCAL_BLUE;
    } else {
        g_scaling_buffer[led.driver][led.b] = AW20_SCAL_BLUE_OFF;
    }
    g_scaling_buffer_update_required[led.driver] = true;
}

#elif defined(LED_MATRIX_ENABLE)
// LED Matrix Specific scripts
void AW20_simple_set_scaling_buffer(uint8_t index, bool value) {
    aw20_led led = g_aw20_leds[index];
    if (value) {
        g_scaling_buffer[led.driver][led.v] = AW20_SCAL_LED;
    } else {
        g_scaling_buffer[led.driver][led.v] = AW20_SCAL_LED_OFF;
    }
    g_scaling_buffer_update_required[led.driver] = true;
}

void AW20_simple_set_brightness(int index, uint8_t value) {
    if (index >= 0 && index < LED_MATRIX_LED_COUNT) {
        aw20_led led = g_aw20_leds[index];
        g_pwm_buffer[led.driver][led.v] = value;
        g_pwm_buffer_update_required[led.driver] = true;
    }
}

void AW20_simple_set_brigntness_all(uint8_t value) {
    for (int i = 0; i < LED_MATRIX_LED_COUNT; i++) {
        AW20_simple_set_brightness(i, value);
    }
}
#endif
