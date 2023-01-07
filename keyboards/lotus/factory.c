// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "matrix.h"

enum factory_commands {
    f_bootloader    = 0x00,
    f_emu_keypress  = 0x01, // Next byte is keycode
    f_backlight     = 0x02, // Next byte is on/off boolean
    f_adc           = 0x03, // ADC trigger
    // RGB control
    f_rgb_enable    = 0x04, // Enable RGB
    f_rgb_dimmer    = 0x05, // RGB brightness lower
    f_rgb_brighter  = 0x06, // RGB brightness higher
    f_rgb_cycle_hue = 0x0A, // Cycle through the hues
    f_rgb_next      = 0x0B, // Next RGB pattern
    f_rgb_prev      = 0x0C, // Previous RGB pattern
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
            factory_trigger_adc();
            break;
#ifdef RGB_MATRIX_ENABLE
        case f_rgb_enable:
            print("rgb factory enable&solid\n");
            rgb_matrix_enable();
            rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            break;
        case f_rgb_dimmer:
            print("RGB dimmer\n");
            rgb_matrix_decrease_val();
            break;
        case f_rgb_brighter:
            print("RGB brighter\n");
            rgb_matrix_increase_val();
            break;
        // Don't seem to work, not sure why
        //case 7:
        //    print("RGB red\n");
        //    rgb_matrix_set_color_all(255, 0, 0);
        //    break;
        //case 8:
        //    print("RGB green\n");
        //    rgb_matrix_set_color_all(0, 255, 0);
        //    break;
        //case 9:
        //    print("RGB blue\n");
        //    rgb_matrix_set_color_all(0, 0, 255);
        //    break;
        case f_rgb_cycle_hue:
            print("rgb_matrix_increase_hue\n");
            rgb_matrix_increase_hue();
        case f_rgb_next:
            print("rgb_matrix_step\n");
            rgb_matrix_step();
            break;
        case f_rgb_prev:
            print("rgb_matrix_step_reverse\n");
            rgb_matrix_step_reverse();
            break;
#endif
        default:
            uprintf("Unknown factory command: %u\n", factory_command_id);
            break;
    }
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
