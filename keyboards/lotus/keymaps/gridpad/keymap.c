// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const is31_led g_is31_leds[RGB_MATRIX_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *   driver
 *   |  R location
 *   |  |        G location
 *   |  |        |        B location
 *   |  |        |        | */
    // TODO: probably need to re-arrange them as below in g_led_config
    // LED IDs from pos diagram missing in schematic: 77, 87, 96, 78, 95, 79, 19
    // LED from Schematic below. Those without X are missing in pos diagram.
    {0, CS1_SW1, CS2_SW1, CS3_SW1}, // LED 1      X
    {0, CS1_SW2, CS2_SW2, CS3_SW2}, // LED 2      X
    {0, CS1_SW3, CS2_SW3, CS3_SW3}, // LED 3
    {0, CS1_SW4, CS2_SW4, CS3_SW4}, // LED 4

    {0, CS4_SW1, CS5_SW1, CS6_SW1}, // LED 10     X
    {0, CS4_SW2, CS5_SW2, CS6_SW2}, // LED 14     X
    {0, CS4_SW3, CS5_SW3, CS6_SW3}, // LED 11     X
    {0, CS4_SW4, CS5_SW4, CS6_SW4}, // LED 12     X

    {0, CS7_SW1, CS8_SW1, CS9_SW1}, // LED 19     X
    {0, CS7_SW2, CS8_SW2, CS9_SW2}, // LED 20     X
    {0, CS7_SW3, CS8_SW3, CS9_SW3}, // LED 21     X
    {0, CS7_SW4, CS8_SW4, CS9_SW4}, // LED 22     X

    {0, CS10_SW1, CS11_SW1, CS12_SW1}, // LED 33
    {0, CS10_SW2, CS11_SW2, CS12_SW2}, // LED 34  X
    {0, CS10_SW3, CS11_SW3, CS12_SW3}, // LED 28  X
    {0, CS10_SW4, CS11_SW4, CS12_SW4}, // LED 29  X

    {0, CS13_SW1, CS14_SW1, CS15_SW1}, // LED 41
    {0, CS13_SW2, CS14_SW2, CS15_SW2}, // LED 37
    {0, CS13_SW3, CS14_SW3, CS15_SW3}, // LED 38
    {0, CS13_SW4, CS14_SW4, CS15_SW4}, // LED 42  X

    {0, CS16_SW1, CS17_SW1, CS18_SW1}, // LED 46  X
    {0, CS16_SW2, CS17_SW2, CS18_SW2}, // LED 47  X
    {0, CS16_SW3, CS17_SW3, CS18_SW3}, // LED 48
    {0, CS16_SW4, CS17_SW4, CS18_SW4}, // LED 49
};

led_config_t g_led_config = { {
  // Key Matrix to LED Index
  {  10,   14,   12,   20,    1,    2,   47,   22, },
  {  19,   46,   96,   95,   29,   78,   87,   42, },
  {  34,   11,   12,    0,   77,    0,   79,   21, },
  {   0,    0,    0,    0,   46,    0,    0,    0, },
}, {
  // LED Index to Physical Potision
  {  73,  10 }, // LED 1
  {  73,  24 }, // LED 2
  {   0,  10 }, // LED 10
  {   0,   0 }, // LED 11
  {   0,  37 }, // LED 12
  {   0,  24 }, // LED 14
  {   0,  64 }, // LED 19
  {   0,  50 }, // LED 20
  {  73,  64 }, // LED 21
  {  73,  50 }, // LED 22
  { 150,  64 }, // LED 28
  { 150,  50 }, // LED 29
  { 223,  64 }, // LED 34
  { 223,  37 }, // LED 42
  { 150,  10 }, // LED 46
  {  73,  37 }, // LED 47
  { 223,   0 }, // LED 77
  { 223,  24 }, // LED 78
  { 223,  50 }, // LED 79
  { 223,  10 }, // LED 87
  { 150,  37 }, // LED 95
  { 150,  24 }, // LED 96
}, {
  // LED Index to Flag
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4
} };


#define NUMPAD_ROWS 4
#define NUMPAD_COLS 8
const uint16_t PROGMEM keymaps[][NUMPAD_ROWS][NUMPAD_COLS] = {
     /*
     *         ┌────┬────┬────┬────┐
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  4 keys │    │    │    │    │
     *         ├────┼────┼────┼────┤
     *  4 keys │    │    │    │    │
     *         └────┴────┴────┴────┴
     * 24 total
     *
     * Default mapping to F keys, because there happen to be just enough to fit.
     * The user is expected to remap them by recompiling or with VIA.
     */
    [0] = LAYOUT_gridpad(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,
        KC_F5,   KC_F6,   KC_F7,   KC_F8,
        KC_F9,   KC_F10,  KC_F11,  KC_F12,
        KC_F13,  KC_F14,  KC_F15,  KC_F16,
        KC_F17,  KC_F18,  KC_F19,  KC_F20,
        KC_F21,  KC_F22,  KC_F23,  KC_F24
    ),
};
