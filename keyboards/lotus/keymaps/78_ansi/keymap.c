// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │Esc  │F1 │F2 │F3 │F4 │F5 │F6 │F7 │F8 │F9 │F10│F11│F12│ Del│
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┤
     * 14 keys │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │Backsp│
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬────┤
     * 14 keys │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │ \  │
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴────┤
     * 13 keys │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │ Enter │
     *         ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───────┤
     * 12 keys │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │  Shift  │
     *         ├────┬───┼───┼───┼───┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │   │   │   │                   │   │   │    │↑  │    │
     * 11 keys │Ctrl│FN │GUI│Alt│                   │Alt│Ctl│ ←  ├───┤  → │
     *         │    │   │   │   │                   │   │   │    │  ↓│    │
     *         └────┴───┴───┴───┴───────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    [0] = LAYOUT_78_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,
        KC_LCTL, MO(1),   KC_LGUI, KC_LALT,          KC_SPC,                    KC_RALT, KC_RCTL, KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT
    ),
     /*
     * Function layer
     *         ┌─────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬────┐
     * 14 keys │FN lk│Mut│vDn│vUp│Prv│Ply│Nxt│bDn│bUp│Scn│Air│Prt│App│Ins │
     *         ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┤
     * 14 keys │   │   │   │   │   │   │   │   │   │   │   │   │   │      │
     *         ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬────┤
     * 14 keys │     │   │   │   │   │   │   │   │   │   │Pau│   │   │    │
     *         ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴────┤
     * 13 keys │      │   │SRq│   │   │   │   │   │ScL│   │   │   │       │
     *         ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───────┤
     * 12 keys │        │   │   │   │   │Brk│   │   │   │   │   │         │
     *         ├────┬───┼───┼───┼───┴───┴───┴───┴───┼───┼───┼───┴┬───┬────┤
     *         │    │   │   │   │                   │   │   │    │PgU│    │
     * 11 keys │    │   │   │   │ Toggle Backlight  │   │   │Home├───┤End │
     *         │    │   │   │   │                   │   │   │    │PgD│    │
     *         └────┴───┴───┴───┴───────────────────┴───┴───┴────┴───┴────┘
     * 78 total
     */
    // TODO: Screen toggle, airplane mode
    // TODO: Other unlabeled keys
    [1] = LAYOUT_78_ansi(
        TG(1), KC_KB_MUTE, KC_KB_VOLUME_DOWN, KC_KB_VOLUME_UP, KC_MPRV, KC_MPLY, KC_MNXT, KC_BRID, KC_BRIU, KC_SCRN, KC_TRNS, KC_PSCR, KC_MSEL, KC_INS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PAUS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_SYRQ, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_SCRL, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
        KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_BRK,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          BL_TOGG,                            KC_TRNS, KC_TRNS, KC_HOME, KC_PGUP, KC_PGDN, KC_END
    )
};

#define CS19_SW12 0x00
#define CS20_SW12 0x01
#define CS21_SW12 0x02
#define CS22_SW12 0x03
#define CS23_SW12 0x04
#define CS24_SW12 0x05
#define CS25_SW12 0x06
#define CS26_SW12 0x07
#define CS27_SW12 0x08
#define CS28_SW12 0x09
#define CS29_SW12 0x0A
#define CS30_SW12 0x0B
#define CS31_SW12 0x0C
#define CS32_SW12 0x0D
#define CS33_SW12 0x0E
#define CS34_SW12 0x0F
#define CS35_SW12 0x10
#define CS36_SW12 0x11

#define CS19_SW13 0x12
#define CS20_SW13 0x13
#define CS21_SW13 0x14
#define CS22_SW13 0x15
#define CS23_SW13 0x16
#define CS24_SW13 0x17
#define CS25_SW13 0x18
#define CS26_SW13 0x19
#define CS27_SW13 0x1A
#define CS28_SW13 0x1B
#define CS29_SW13 0x1C
#define CS30_SW13 0x1D
#define CS31_SW13 0x1E
#define CS32_SW13 0x1F
#define CS33_SW13 0x20
#define CS34_SW13 0x21
#define CS35_SW13 0x22
#define CS36_SW13 0x23

#define CS19_SW14 0x24
#define CS20_SW14 0x25
#define CS21_SW14 0x26
#define CS22_SW14 0x27
#define CS23_SW14 0x28
#define CS24_SW14 0x29
#define CS25_SW14 0x2A
#define CS26_SW14 0x2B
#define CS27_SW14 0x2C
#define CS28_SW14 0x2D
#define CS29_SW14 0x2E
#define CS30_SW14 0x2F
#define CS31_SW14 0x30
#define CS32_SW14 0x31
#define CS33_SW14 0x32
#define CS34_SW14 0x33
#define CS35_SW14 0x34
#define CS36_SW14 0x35

#define CS19_SW15 0x36
#define CS20_SW15 0x37
#define CS21_SW15 0x38
#define CS22_SW15 0x39
#define CS23_SW15 0x3A
#define CS24_SW15 0x3B
#define CS25_SW15 0x3C
#define CS26_SW15 0x3D
#define CS27_SW15 0x3E
#define CS28_SW15 0x3F
#define CS29_SW15 0x40
#define CS30_SW15 0x41
#define CS31_SW15 0x42
#define CS32_SW15 0x43
#define CS33_SW15 0x44
#define CS34_SW15 0x45
#define CS35_SW15 0x46
#define CS36_SW15 0x47

#define CS19_SW16 0x48
#define CS20_SW16 0x49
#define CS21_SW16 0x4A
#define CS22_SW16 0x4B
#define CS23_SW16 0x4C
#define CS24_SW16 0x4D
#define CS25_SW16 0x4E
#define CS26_SW16 0x4F
#define CS27_SW16 0x50
#define CS28_SW16 0x51
#define CS29_SW16 0x52
#define CS30_SW16 0x53
#define CS31_SW16 0x54
#define CS32_SW16 0x55
#define CS33_SW16 0x56
#define CS34_SW16 0x57
#define CS35_SW16 0x58
#define CS36_SW16 0x59

#define CS19_SW17 0x5A
#define CS20_SW17 0x5B
#define CS21_SW17 0x5C
#define CS22_SW17 0x5D
#define CS23_SW17 0x5E
#define CS24_SW17 0x5F
#define CS25_SW17 0x60
#define CS26_SW17 0x61
#define CS27_SW17 0x62
#define CS28_SW17 0x63
#define CS29_SW17 0x64
#define CS30_SW17 0x65
#define CS31_SW17 0x66
#define CS32_SW17 0x67
#define CS33_SW17 0x68
#define CS34_SW17 0x69
#define CS35_SW17 0x6A
#define CS36_SW17 0x6B

#define CS19_SW18 0x6C
#define CS20_SW18 0x6D
#define CS21_SW18 0x6E
#define CS22_SW18 0x6F
#define CS23_SW18 0x70
#define CS24_SW18 0x71
#define CS25_SW18 0x72
#define CS26_SW18 0x73
#define CS27_SW18 0x74
#define CS28_SW18 0x75
#define CS29_SW18 0x76
#define CS30_SW18 0x77
#define CS31_SW18 0x78
#define CS32_SW18 0x79
#define CS33_SW18 0x7A
#define CS34_SW18 0x7B
#define CS35_SW18 0x7C
#define CS36_SW18 0x7D

#define CS19_SW19 0x7E
#define CS20_SW19 0x7F
#define CS21_SW19 0x80
#define CS22_SW19 0x81
#define CS23_SW19 0x82
#define CS24_SW19 0x83
#define CS25_SW19 0x84
#define CS26_SW19 0x85
#define CS27_SW19 0x86
#define CS28_SW19 0x87
#define CS29_SW19 0x88
#define CS30_SW19 0x89
#define CS31_SW19 0x8A
#define CS32_SW19 0x8B
#define CS33_SW19 0x8C
#define CS34_SW19 0x8D
#define CS35_SW19 0x8E
#define CS36_SW19 0x8F

#define CS19_SW20 0x90
#define CS20_SW20 0x91
#define CS21_SW20 0x92
#define CS22_SW20 0x93
#define CS23_SW20 0x94
#define CS24_SW20 0x95
#define CS25_SW20 0x96
#define CS26_SW20 0x97
#define CS27_SW20 0x98
#define CS28_SW20 0x99
#define CS29_SW20 0x9A
#define CS30_SW20 0x9B
#define CS31_SW20 0x9C
#define CS32_SW20 0x9D
#define CS33_SW20 0x9E
#define CS34_SW20 0x9F
#define CS35_SW20 0xA0
#define CS36_SW20 0xA1

// TODO: Should have attribute __flash. But won't compile if I add it
const is31_led g_is31_leds[RGB_MATRIX_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *   driver
 *   |  R location
 *   |  |        G location
 *   |  |        |        B location
 *   |  |        |        | */
    // TODO: Probably have to order them by LED ID?
    {0, CS1_SW1, CS2_SW1, CS3_SW1}, // LED 3
    {0, CS1_SW2, CS2_SW2, CS3_SW2}, // LED 4
    {0, CS1_SW3, CS2_SW3, CS3_SW3}, // LED 5
    {0, CS1_SW4, CS2_SW4, CS3_SW4}, // LED 6
    {0, CS1_SW5, CS2_SW5, CS3_SW5}, // LED 8
    {0, CS1_SW6, CS2_SW6, CS3_SW6}, // LED 1
    {0, CS1_SW7, CS2_SW7, CS3_SW7}, // LED 9
    {0, CS1_SW8, CS2_SW8, CS3_SW8}, // LED 2
    {0, CS1_SW9, CS2_SW9, CS3_SW9}, // LED 7

    {0, CS4_SW1, CS5_SW1, CS6_SW1}, // LED 12
    {0, CS4_SW2, CS5_SW2, CS6_SW2}, // LED 13
    {0, CS4_SW3, CS5_SW3, CS6_SW3}, // LED 10
    {0, CS4_SW4, CS5_SW4, CS6_SW4}, // LED 14
    {0, CS4_SW5, CS5_SW5, CS6_SW5}, // LED 15
    {0, CS4_SW6, CS5_SW6, CS6_SW6}, // LED 16
    {0, CS4_SW7, CS5_SW7, CS6_SW7}, // LED 11
    {0, CS4_SW8, CS5_SW8, CS6_SW8}, // LED 17
    {0, CS4_SW9, CS5_SW9, CS6_SW9}, // LED 18

    {0, CS7_SW1, CS8_SW1, CS9_SW1}, // LED 19
    {0, CS7_SW2, CS8_SW2, CS9_SW2}, // LED 20
    {0, CS7_SW3, CS8_SW3, CS9_SW3}, // LED 21
    {0, CS7_SW4, CS8_SW4, CS9_SW4}, // LED 22
    {0, CS7_SW5, CS8_SW5, CS9_SW5}, // LED 23
    {0, CS7_SW6, CS8_SW6, CS9_SW6}, // LED 24
    {0, CS7_SW7, CS8_SW7, CS9_SW7}, // LED 25
    {0, CS7_SW8, CS8_SW8, CS9_SW8}, // LED 26
    {0, CS7_SW9, CS8_SW9, CS9_SW9}, // LED 27

    {0, CS10_SW1, CS11_SW1, CS12_SW1}, // LED 28
    {0, CS10_SW2, CS11_SW2, CS12_SW2}, // LED 29
    {0, CS10_SW3, CS11_SW3, CS12_SW3}, // LED 30
    {0, CS10_SW4, CS11_SW4, CS12_SW4}, // LED 31
    {0, CS10_SW5, CS11_SW5, CS12_SW5}, // LED 32
    {0, CS10_SW6, CS11_SW6, CS12_SW6}, // LED 33
    {0, CS10_SW7, CS11_SW7, CS12_SW7}, // LED 34
    {0, CS10_SW8, CS11_SW8, CS12_SW8}, // LED 35
    {0, CS10_SW9, CS11_SW9, CS12_SW9}, // LED 36

    {0, CS13_SW1, CS14_SW1, CS15_SW1}, // LED 42
    {0, CS13_SW2, CS14_SW2, CS15_SW2}, // LED 37
    {0, CS13_SW3, CS14_SW3, CS15_SW3}, // LED 38
    {0, CS13_SW4, CS14_SW4, CS15_SW4}, // LED 43
    {0, CS13_SW5, CS14_SW5, CS15_SW5}, // LED 44
    {0, CS13_SW6, CS14_SW6, CS15_SW6}, // LED 39
    {0, CS13_SW7, CS14_SW7, CS15_SW7}, // LED 40
    {0, CS13_SW8, CS14_SW8, CS15_SW8}, // LED 45
    {0, CS13_SW9, CS14_SW9, CS15_SW9}, // LED 41

    {0, CS16_SW1, CS17_SW1, CS18_SW1}, // LED 46
    {0, CS16_SW2, CS17_SW2, CS18_SW2}, // LED 47
    {0, CS16_SW3, CS17_SW3, CS18_SW3}, // LED 48

    {1, CS19_SW12, CS20_SW12, CS21_SW12}, // LED 49
    {1, CS19_SW13, CS20_SW13, CS21_SW13}, // LED 50
    {1, CS19_SW14, CS20_SW14, CS21_SW14}, // LED 51
    {1, CS19_SW15, CS20_SW15, CS21_SW15}, // LED 52
    {1, CS19_SW16, CS20_SW16, CS21_SW16}, // LED 53
    {1, CS19_SW17, CS20_SW17, CS21_SW17}, // LED 54
    {1, CS19_SW18, CS20_SW18, CS21_SW18}, // LED 55
    {1, CS19_SW19, CS20_SW19, CS21_SW19}, // LED 56
    {1, CS19_SW20, CS20_SW20, CS21_SW20}, // LED 57

    {1, CS22_SW12, CS23_SW12, CS24_SW12}, // LED 58
    {1, CS22_SW13, CS23_SW13, CS24_SW13}, // LED 59
    {1, CS22_SW14, CS23_SW14, CS24_SW14}, // LED 60
    {1, CS22_SW15, CS23_SW15, CS24_SW15}, // LED 61
    {1, CS22_SW16, CS23_SW16, CS24_SW16}, // LED 62
    {1, CS22_SW17, CS23_SW17, CS24_SW17}, // LED 63
    {1, CS22_SW18, CS23_SW18, CS24_SW18}, // LED 64
    {1, CS22_SW19, CS23_SW19, CS24_SW19}, // LED 65
    {1, CS22_SW20, CS23_SW20, CS24_SW20}, // LED 66

    {1, CS25_SW12, CS26_SW12, CS27_SW12}, // LED 67
    {1, CS25_SW13, CS26_SW13, CS27_SW13}, // LED 68
    {1, CS25_SW14, CS26_SW14, CS27_SW14}, // LED 75
    {1, CS25_SW15, CS26_SW15, CS27_SW15}, // LED 69
    {1, CS25_SW16, CS26_SW16, CS27_SW16}, // LED 70
    {1, CS25_SW17, CS26_SW17, CS27_SW17}, // LED 71
    {1, CS25_SW18, CS26_SW18, CS27_SW18}, // LED 72
    {1, CS25_SW19, CS26_SW19, CS27_SW19}, // LED 73
    {1, CS25_SW20, CS26_SW20, CS27_SW20}, // LED 74

    {1, CS28_SW12, CS29_SW12, CS30_SW12}, // LED 76
    {1, CS28_SW13, CS29_SW13, CS30_SW13}, // LED 77
    {1, CS28_SW14, CS29_SW14, CS30_SW14}, // LED 78
    {1, CS28_SW15, CS29_SW15, CS30_SW15}, // LED 89
    {1, CS28_SW16, CS29_SW16, CS30_SW16}, // LED 80
    {1, CS28_SW17, CS29_SW17, CS30_SW17}, // LED 81
    {1, CS28_SW18, CS29_SW18, CS30_SW18}, // LED 82
    {1, CS28_SW19, CS29_SW19, CS30_SW19}, // LED 83
    {1, CS28_SW20, CS29_SW20, CS30_SW20}, // LED 84

    {1, CS31_SW12, CS32_SW12, CS33_SW12}, // LED 85
    {1, CS31_SW13, CS32_SW13, CS33_SW13}, // LED 86
    {1, CS31_SW14, CS32_SW14, CS33_SW14}, // LED 87
    {1, CS31_SW15, CS32_SW15, CS33_SW15}, // LED 88
    {1, CS31_SW16, CS32_SW16, CS33_SW16}, // LED 89
    {1, CS31_SW17, CS32_SW17, CS33_SW17}, // LED 90
    {1, CS31_SW18, CS32_SW18, CS33_SW18}, // LED 91
    {1, CS31_SW19, CS32_SW19, CS33_SW19}, // LED 92
    {1, CS31_SW20, CS32_SW20, CS33_SW20}, // LED 93

    {1, CS34_SW12, CS35_SW12, CS36_SW12}, // LED 94
    {1, CS34_SW13, CS35_SW13, CS36_SW13}, // LED 95
    {1, CS34_SW14, CS35_SW14, CS36_SW14}, // LED 96
    {1, CS34_SW15, CS35_SW15, CS36_SW15}, // LED 97
};

led_config_t g_led_config = { {
  // Key Matrix to LED Index
  {32, 73,  6, 85,  0, 30, 34, 78, 76, 81, 79,  0, 89, 80, 53,  0, },
  { 0,  0,  0, 48, 95, 28, 36, 77, 82, 33,  0,  0, 35, 91, 54, 68, },
  { 0,  0, 94,  0,  3, 20, 40, 41, 56,  0, 27,  0,  0, 64,  0, 92, },
  { 0, 47,  2,  0, 21, 22,  8,  9, 60,  0, 25, 75,  0,  0,  0, 71, },
  { 0,  0, 17,  0, 45, 42, 10, 15, 69,  0, 23,  0,  0, 62, 63,  0, },
  { 0,  0, 16,  0, 13, 14, 12, 11, 70,  0, 18,  0,  0, 61, 65,  0, },
  { 0,  0,  0,  0, 19,  4,  5,  7, 67,  0, 59, 90,  0, 58, 55,  0, },
  { 0,  0, 37,  0,  0, 26, 44, 50, 52,  0, 51,  0,  0, 49, 38,  0, },
}, {
  // LED Index to Physical Position
  {  11,  23 }, // LED 1
  {   0,  23 }, // LED 2
  {  57,  22 }, // LED 3
  {  41,  22 }, // LED 4
  {  73,  22 }, // LED 5
  {  25,  22 }, // LED 6
  { 121,  22 }, // LED 7
  {  89,  22 }, // LED 8
  { 105,  22 }, // LED 9
  {  82,  10 }, // LED 10
  { 114,  10 }, // LED 11
  {  66,  10 }, // LED 12
  {  50,  10 }, // LED 13
  {  34,  10 }, // LED 14
  {  98,  10 }, // LED 15
  {  18,  10 }, // LED 16
  {   2,  10 }, // LED 17
  { 130,  10 }, // LED 18
  {  56,   0 }, // LED 19
  {  40,   0 }, // LED 20
  {  72,   0 }, // LED 21
  {  24,   0 }, // LED 22
  {  88,   0 }, // LED 23
  {  10,   0 }, // LED 24
  { 104,   0 }, // LED 25
  {   0,   0 }, // LED 26
  { 120,   0 }, // LED 27
  {  38,  47 }, // LED 28
  {  22,  47 }, // LED 29
  {  54,  47 }, // LED 30
  {   5,  48 }, // LED 31
  {  70,  47 }, // LED 32
  {   1,  48 }, // LED 33
  {  86,  47 }, // LED 34
  {   0,  60 }, // LED 35
  { 102,  47 }, // LED 36
  {  30,  34 }, // LED 37
  {  62,  34 }, // LED 38
  {   7,  35 }, // LED 39
  {  94,  34 }, // LED 40
  { 110,  34 }, // LED 41
  {  46,  34 }, // LED 42
  {  13,  36 }, // LED 43
  {  78,  34 }, // LED 44
  {   1,  36 }, // LED 45
  {   8,  60 }, // LED 46
  {  38,  59 }, // LED 47
  {  54,  59 }, // LED 48
  { 174,  34 }, // LED 49
  { 126,  34 }, // LED 50
  { 142,  34 }, // LED 51
  { 158,  34 }, // LED 52
  { 190,  34 }, // LED 53
  { 205,  34 }, // LED 54
  { 202,  22 }, // LED 55
  { 219,  22 }, // LED 56
  { 223,  36 }, // LED 57
  { 185,  22 }, // LED 58
  { 137,  22 }, // LED 59
  { 153,  22 }, // LED 60
  { 169,  22 }, // LED 61
  { 162,  10 }, // LED 62
  { 194,  10 }, // LED 63
  { 178,  10 }, // LED 64
  { 209,  11 }, // LED 65
  { 222,  11 }, // LED 66
  { 181,   0 }, // LED 67
  { 136,   0 }, // LED 68
  { 168,   0 }, // LED 69
  { 146,  10 }, // LED 70
  { 199,   0 }, // LED 71
  { 186,   0 }, // LED 72
  { 213,   0 }, // LED 73
  { 224,   0 }, // LED 74
  { 152,   0 }, // LED 75
  { 166,  47 }, // LED 76
  { 118,  47 }, // LED 77
  { 134,  47 }, // LED 78
  { 150,  47 }, // LED 79
  { 182,  47 }, // LED 80
  { 197,  48 }, // LED 81
  { 202,  58 }, // LED 82
  { 203,  48 }, // LED 83
  { 217,  48 }, // LED 84
  { 150,  59 }, // LED 85
  { 108,  60 }, // LED 86
  { 121,  60 }, // LED 87
  { 134,  60 }, // LED 88
  { 166,  59 }, // LED 89
  { 183,  64 }, // LED 90
  { 202,  62 }, // LED 91
  { 220,  64 }, // LED 92
  { 222,  48 }, // LED 93
  {  22,  59 }, // LED 94
  {  70,  60 }, // LED 95
  {  83,  60 }, // LED 96
  {  95,  60 }, // LED 97
}, {
  // LED Index to Flag
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4
} };
