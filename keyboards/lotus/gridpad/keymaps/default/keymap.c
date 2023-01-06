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
    // Re-arranged in LED ID order so it matches with the physical location array
    {0, CS1_SW1, CS2_SW1, CS3_SW1}, // LED 1
    {0, CS1_SW2, CS2_SW2, CS3_SW2}, // LED 2
    {0, CS1_SW3, CS2_SW3, CS3_SW3}, // LED 3
    {0, CS1_SW4, CS2_SW4, CS3_SW4}, // LED 4

    {0, CS4_SW1, CS5_SW1, CS6_SW1}, // LED 5
    {0, CS4_SW3, CS5_SW3, CS6_SW3}, // LED 6
    {0, CS4_SW4, CS5_SW4, CS6_SW4}, // LED 7
    {0, CS4_SW2, CS5_SW2, CS6_SW2}, // LED 8

    {0, CS7_SW1, CS8_SW1, CS9_SW1}, // LED 9
    {0, CS7_SW2, CS8_SW2, CS9_SW2}, // LED 10
    {0, CS7_SW3, CS8_SW3, CS9_SW3}, // LED 11
    {0, CS7_SW4, CS8_SW4, CS9_SW4}, // LED 12

    {0, CS10_SW1, CS11_SW1, CS12_SW1}, // LED 13
    {0, CS10_SW2, CS11_SW2, CS12_SW2}, // LED 14
    {0, CS10_SW3, CS11_SW3, CS12_SW3}, // LED 15
    {0, CS10_SW4, CS11_SW4, CS12_SW4}, // LED 16

    {0, CS13_SW2, CS14_SW2, CS15_SW2}, // LED 17
    {0, CS13_SW3, CS14_SW3, CS15_SW3}, // LED 18
    {0, CS13_SW1, CS14_SW1, CS15_SW1}, // LED 19
    {0, CS13_SW4, CS14_SW4, CS15_SW4}, // LED 20

    {0, CS16_SW1, CS17_SW1, CS18_SW1}, // LED 21
    {0, CS16_SW2, CS17_SW2, CS18_SW2}, // LED 22
    {0, CS16_SW3, CS17_SW3, CS18_SW3}, // LED 23
    {0, CS16_SW4, CS17_SW4, CS18_SW4}, // LED 24
};

led_config_t g_led_config = { {
  // Key Matrix to LED Index
  { 5,  8,  7, 10,  1,  2,  4, 12, },
  { 9, 21, 22, 24, 16, 17, 19, 20, },
  {13,  6,  3,  0, 18,  0, 14, 11, },
  { 0,  0,  0,  0, 23,  0,  0,  0, },
}, {
  // LED Index to Physical Position
  {  73,  10 }, // LED 1
  {  73,  24 }, // LED 2
  {  73,   0 }, // LED 3
  {  73,  37 }, // LED 4
  {   0,  10 }, // LED 5
  {   0,   0 }, // LED 6
  {   0,  37 }, // LED 7
  {   0,  24 }, // LED 8
  {   0,  64 }, // LED 9
  {   0,  50 }, // LED 10
  {  73,  64 }, // LED 11
  {  73,  50 }, // LED 12
  { 223,  64 }, // LED 13
  { 223,  50 }, // LED 14
  { 150,  64 }, // LED 15
  { 150,  50 }, // LED 16
  { 223,  24 }, // LED 17
  { 223,   0 }, // LED 18
  { 223,  10 }, // LED 19
  { 223,  37 }, // LED 20
  { 150,  10 }, // LED 21
  { 150,  24 }, // LED 22
  { 150,   0 }, // LED 23
  { 150,  37 }, // LED 24
}, {
  // LED Index to Flag
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4
} };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
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
    [0] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,
        KC_F5,   KC_F6,   KC_F7,   KC_F8,
        KC_F9,   KC_F10,  KC_F11,  KC_F12,
        KC_F13,  KC_F14,  KC_F15,  KC_F16,
        KC_F17,  KC_F18,  KC_F19,  KC_F20,
        KC_F21,  KC_F22,  KC_F23,  KC_F24
    ),
};
