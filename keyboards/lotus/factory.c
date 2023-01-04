// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "raw_hid.h"
#include "matrix.h"

enum factory_commands {
    f_bootloader   = 0x00,
    f_emu_keypress = 0x01, // Next byte is keycode
    f_backlight    = 0x02, // Next byte is on/off boolean
    f_adc          = 0x03, // ADC trigger
};

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
            tap_code(command_data[0]);
            break;
        case f_backlight:
            // Need a separate command because tap_code won't work for
            // commands larger than 1 byte. The extra byte in tap_code16
            // is just for modifiers.
            uprintf("backlight: %u\n", command_data[0]);
            if (command_data[0] == 0) {
                backlight_disable();
            } else {
                backlight_enable();
            }
            break;
        case f_adc:
            trigger_adc();
            break;
        default:
            uprintf("Unknown factory command: %u\n", factory_command_id);
            break;
    }
}

bool handle_hid(uint8_t *data, uint8_t length) {
    uint8_t command_id = data[0];
    uint8_t *command_data = &(data[1]);

    uprintf("raw_hid_receive(command: %X, length: %d)\n", command_id, length);

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
            uprintf("Unrecognized command ID: %u\n", command_id);
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
bool raw_hid_receive(uint8_t *data, uint8_t length) {
  return handle_hid(data, length);
}
#endif
