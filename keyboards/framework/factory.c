// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "raw_hid.h"
#include "matrix.h"
#include "factory.h"
#include "framework.h"
#if defined(RGB_MATRIX_ENABLE)
#include "rgb_matrix.h"
#endif
#ifdef SERIAL_NUMBER
#include "dyn_serial.h"
#endif

enum factory_commands {
    f_emu_keypress  = 0x01, // Next byte is keycode
    f_serialnum     = 0x04, // Read device serial number
    f_bios_mode     = 0x05, // Read device serial number
    f_factory_mode  = 0x06, // Read device serial number
    f_bootloader    = 0xFE,
};

#if defined(RGB_MATRIX_ENABLE)
extern uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];

void emulate_rgb_keycode_press(uint16_t target_keycode) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t keycode = keymaps[0][row][col];
            process_rgb_matrix(row, col, keycode == target_keycode);
        }
    }
}
#endif

extern char ascii_serialnum[SERIALNUM_LEN+1];

void handle_factory_command(uint8_t *data) {
    uint8_t factory_command_id = data[0];
    uint8_t *command_data = &(data[1]);
    uprintf("handle_factory_command(command: %u)\n", factory_command_id);

    switch (factory_command_id) {
        case f_bootloader:
            print("bootloader_jump\n");
            bootloader_jump();
            break;
        case f_emu_keypress:
            uprintf("Emulating keycode: %u\n", command_data[0]);
#if defined(RGB_MATRIX_ENABLE)
            emulate_rgb_keycode_press(command_data[0]);
#else
            tap_code(command_data[0]);
#endif
            break;
        case f_serialnum:
            print("Reading Device serial number\n");


#ifdef SERIAL_NUMBER
            uprintf("Serial number: %s\n", ascii_serialnum);
#else
            uprintf("Serial number unavailable\n");
#endif
            break;
        case f_bios_mode:
            if (command_data[0] == 0x01) {
                bios_mode = true;
                // Red
                rgb_matrix_sethsv_noeeprom(0, 0xFF, 0xFF);
                rgb_matrix_mode_noeeprom(1);
            } else {
                // White
                rgb_matrix_sethsv_noeeprom(0, 0, 0xFF);
                rgb_matrix_mode_noeeprom(1);
                bios_mode = false;
            }
            break;
        case f_factory_mode:
            enable_factory_mode(command_data[0] == 0x01);
            break;
        default:
            uprintf("Unknown factory command: %u\n", factory_command_id);
            break;
    }
}

__attribute__ ((weak))
void enable_factory_mode(bool enable) {
}

bool handle_hid(uint8_t *data, uint8_t length) {
    uint8_t command_id = data[0];
    uint8_t *command_data = &(data[1]);

    //uprintf("raw_hid_receive(command: %X, length: %d)\n", command_id, length);

    switch (command_id) {
        case 0x01:
            print("protocol_ver\n");
#ifndef VIA_ENABLE
            command_data[0] = 0x0B >> 8;
            command_data[1] = 0x0B & 0xFF;
            raw_hid_send(data, length);
#endif
            break;
        case 0x0B:
            // Take over the bootloader jump command ID from via and use it for our purposes
            handle_factory_command(command_data);
            // Don't let VIA handle it
            return true;
        default:
            //uprintf("Unrecognized command ID: %u\n", command_id);
            break;
    }
    // Continue with default HID handling
    return false;
}

// Add hooks to handle raw HID commands.
// Need add both functions to make it work when
// either RAW_ENABLE or VIA_ENABLE are enabled.
bool via_command_kb(uint8_t *data, uint8_t length) {
  return handle_hid(data, length);
}
#ifndef VIA_ENABLE
void raw_hid_receive(uint8_t *data, uint8_t length) {
  handle_hid(data, length);
}
#endif
