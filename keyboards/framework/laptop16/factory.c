// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "raw_hid.h"
#include "matrix.h"
#include "factory.h"
#include "framework.h"
#if defined(RGB_MATRIX_ENABLE)
#    include "rgb_matrix.h"
#endif
#ifdef SERIAL_NUMBER
#    include "dyn_serial.h"
#endif

enum factory_commands {
    f_emu_keypress  = 0x01, // Next byte is keycode
    f_serialnum     = 0x04, // Read device serial number
    f_bios_mode     = 0x05, // Read device serial number
    f_factory_mode  = 0x06, // Read device serial number
    f_lock          = 0x07, //
    f_unlock        = 0x08, //
    f_read          = 0x09, //
    f_write         = 0x0A, //
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

extern char ascii_serialnum[SERIALNUM_LEN + 1];

#include "pico/bootrom.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/structs/ssi.h"
#include "hardware/structs/ioqspi.h"
#define FLASHCMD_WRITE_STATUS 0x01
#define FLASHCMD_PAGE_PROGRAM 0x02
#define FLASHCMD_READ_STATUS 0x05
#define FLASHCMD_WRITE_ENABLE 0x06
#define FLASHCMD_WRITE_ENABLE_VOLATILE 0x50
extern void __no_inline_not_in_flash_func(flash_put_get)(const uint8_t *tx, uint8_t *rx, size_t count, size_t rx_skip);
extern void __no_inline_not_in_flash_func(flash_enable_xip_via_boot2)(void);
extern void __no_inline_not_in_flash_func(flash_wait_ready)(void);
extern void __no_inline_not_in_flash_func(flash_enable_write)(void);
extern void __no_inline_not_in_flash_func(flash_put_cmd_addr)(uint8_t cmd, uint32_t addr);
extern void __no_inline_not_in_flash_func(_flash_do_cmd)(uint8_t cmd, const uint8_t *tx, uint8_t *rx, size_t count);
void __no_inline_not_in_flash_func(flash_cs_force)(bool high);
void __no_inline_not_in_flash_func(pico_program_bulk)(uint32_t flash_address, void *buffer, size_t buffer_len) {
    rom_connect_internal_flash_fn connect_internal_flash = (rom_connect_internal_flash_fn)rom_func_lookup_inline(ROM_FUNC_CONNECT_INTERNAL_FLASH);
    rom_flash_exit_xip_fn         flash_exit_xip         = (rom_flash_exit_xip_fn)rom_func_lookup_inline(ROM_FUNC_FLASH_EXIT_XIP);
    rom_flash_flush_cache_fn      flash_flush_cache      = (rom_flash_flush_cache_fn)rom_func_lookup_inline(ROM_FUNC_FLASH_FLUSH_CACHE);
    //assert(connect_internal_flash && flash_exit_xip && flash_flush_cache);
    uprintf("cif: %p, fex: %p, ffc: %p\r\n", connect_internal_flash, flash_exit_xip, flash_flush_cache);

    __compiler_memory_barrier();

    connect_internal_flash();
    flash_exit_xip();
    flash_enable_write();

    flash_put_cmd_addr(FLASHCMD_PAGE_PROGRAM, flash_address);
    flash_put_get((uint8_t *)buffer, NULL, buffer_len, 4);
    flash_wait_ready();

    flash_flush_cache();
    flash_enable_xip_via_boot2();
}


void test_factory(uint8_t factory_command_id) {
    //uint8_t id_out;
    uint8_t buffer[4];
    uint8_t stat;
    uint32_t ints = save_and_disable_interrupts();

    buffer[0] = 0x01;
    buffer[1] = 0x02;
    buffer[2] = 0x03;
    buffer[3] = 0x04;
    switch (factory_command_id) {
        case f_lock:
            printf("Before Lock\r\n");
            flash_cs_force(0);
            flash_cs_force(1);
            //flash_enable_write();
            // _flash_do_cmd(FLASHCMD_READ_STATUS, NULL, &stat, 1);
            // // Protect very last 4K block
            // stat |= 1 << 2;    // BP0==1 instead of 64K
            // stat &= ~(1 << 3); // BP1=0
            // stat &= ~(1 << 4); // BP2=0
            // stat &= ~(1 << 5); // Protect top of flash
            // stat |= 1 << 6;    // SEC1==4K instead of 64K
            // _flash_do_cmd(FLASHCMD_WRITE_STATUS, &stat, NULL, 1);
            printf("After Lock\r\n");
            break;
        case f_unlock:
            printf("Before Unlock\r\n");
            flash_enable_write();
            _flash_do_cmd(FLASHCMD_READ_STATUS, NULL, &stat, 1);
            // Protect very last 4K block
            stat &= ~(1 << 2);    // BP0==1 instead of 64K
            stat &= ~(1 << 3); // BP1=0
            stat &= ~(1 << 4); // BP2=0
            stat &= ~(1 << 5); // TB=0 Protect top of flash
            stat |= 1 << 6;    // SEC1==4K instead of 64K
            _flash_do_cmd(FLASHCMD_WRITE_STATUS, &stat, NULL, 1);
            printf("After unlock\r\n");
            break;
        case f_read:
            printf("Read\r\n");

            void *serialnum_ptr = (void*) (FLASH_OFFSET + LAST_4K_BLOCK);
            uint8_t *sn_raw = serialnum_ptr;
            printf("Serialnum: %02x %02x %02x %02x\r\n", sn_raw[0], sn_raw[1], sn_raw[2], sn_raw[3]);

            //flash_get_unique_id(&id_out);
            //printf("Unique flash ID: %02x\r\n", id_out);
            break;
        case f_write:
            printf("Before Write\r\n");
            pico_program_bulk(FLASH_OFFSET + LAST_4K_BLOCK, &buffer[0], sizeof(buffer));
            printf("After Write\r\n");
            break;
        default:
            uprintf("Unknown factory command: %u\n", factory_command_id);
            break;
    }
    restore_interrupts (ints);
}


void handle_factory_command(uint8_t *data) {
    uint8_t  factory_command_id = data[0];
    uint8_t *command_data       = &(data[1]);
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
            if (command_data[0] == 0x01)
                bios_mode = true;
            else
                bios_mode = false;
            break;
        case f_factory_mode:
            enable_factory_mode(command_data[0] == 0x01);
            break;
        case f_lock:
        case f_unlock:
        case f_read:
        case f_write:
            test_factory(factory_command_id);
            break;
        default:
            uprintf("Unknown factory command: %u\n", factory_command_id);
            break;
    }
}

__attribute__((weak)) void enable_factory_mode(bool enable) {}

bool handle_hid(uint8_t *data, uint8_t length) {
    uint8_t  command_id   = data[0];
    uint8_t *command_data = &(data[1]);

    // uprintf("raw_hid_receive(command: %X, length: %d)\n", command_id, length);

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
            // uprintf("Unrecognized command ID: %u\n", command_id);
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
