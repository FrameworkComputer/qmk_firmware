// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// clang-format off
// TODO: Should have attribute __flash. But won't compile if I add it
const is31_led g_is31_leds[RGB_MATRIX_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *   driver
 *   |    R location
 *   |    |          G location
 *   |    |          |          B location
 *   |    |          |          | */
    // Re-arranged in LED ID order so it matches with the physical location array
    {0,  CS3_SW6,  CS2_SW6,  CS1_SW6}, // LED 1
    {0,  CS3_SW8,  CS2_SW8,  CS1_SW8}, // LED 2
    {0,  CS3_SW1,  CS2_SW1,  CS1_SW1}, // LED 3
    {0,  CS3_SW2,  CS2_SW2,  CS1_SW2}, // LED 4
    {0,  CS3_SW3,  CS2_SW3,  CS1_SW3}, // LED 5
    {0,  CS3_SW4,  CS2_SW4,  CS1_SW4}, // LED 6
    {0, CS18_SW4, CS17_SW4, CS16_SW4}, // LED 7
    {0,  CS3_SW5,  CS2_SW5,  CS1_SW5}, // LED 8
    {0,  CS3_SW7,  CS2_SW7,  CS1_SW7}, // LED 9

    {0,  CS6_SW3,  CS5_SW3,  CS4_SW3}, // LED 10
    {0,  CS6_SW7,  CS5_SW7,  CS4_SW7}, // LED 11
    {0,  CS6_SW1,  CS5_SW1,  CS4_SW1}, // LED 12
    {0,  CS6_SW2,  CS5_SW2,  CS4_SW2}, // LED 13
    {0,  CS6_SW4,  CS5_SW4,  CS4_SW4}, // LED 14
    {0,  CS6_SW5,  CS5_SW5,  CS4_SW5}, // LED 15
    {0,  CS6_SW6,  CS5_SW6,  CS4_SW6}, // LED 16
    {0,  CS6_SW8,  CS5_SW8,  CS4_SW8}, // LED 17
    {0, CS18_SW5, CS17_SW5, CS16_SW5}, // LED 18

    {0,  CS9_SW1,  CS8_SW1,  CS7_SW1}, // LED 19
    {0,  CS9_SW2,  CS8_SW2,  CS7_SW2}, // LED 20
    {0,  CS9_SW3,  CS8_SW3,  CS7_SW3}, // LED 21
    {0,  CS9_SW4,  CS8_SW4,  CS7_SW4}, // LED 22
    {0,  CS9_SW5,  CS8_SW5,  CS7_SW5}, // LED 23
    {0,  CS9_SW6,  CS8_SW6,  CS7_SW6}, // LED 24
    {0,  CS9_SW7,  CS8_SW7,  CS7_SW7}, // LED 25
    {0,  CS9_SW8,  CS8_SW8,  CS7_SW8}, // LED 26
    {0, CS18_SW6, CS17_SW6, CS16_SW6}, // LED 27

    {0, CS12_SW1, CS11_SW1, CS10_SW1}, // LED 28
    {0, CS12_SW2, CS11_SW2, CS10_SW2}, // LED 29
    {0, CS12_SW3, CS11_SW3, CS10_SW3}, // LED 30
    {0, CS12_SW4, CS11_SW4, CS10_SW4}, // LED 31
    {0, CS12_SW5, CS11_SW5, CS10_SW5}, // LED 32
    {0, CS12_SW6, CS11_SW6, CS10_SW6}, // LED 33
    {0, CS12_SW7, CS11_SW7, CS10_SW7}, // LED 34
    {0, CS12_SW8, CS11_SW8, CS10_SW8}, // LED 35
    {0, CS18_SW7, CS17_SW7, CS16_SW7}, // LED 36

    {0, CS15_SW2, CS14_SW2, CS13_SW2}, // LED 37
    {0, CS15_SW3, CS14_SW3, CS13_SW3}, // LED 38
    {0, CS15_SW6, CS14_SW6, CS13_SW6}, // LED 39
    {0, CS15_SW7, CS14_SW7, CS13_SW7}, // LED 40
    {0, CS18_SW8, CS17_SW8, CS16_SW8}, // LED 41
    {0, CS15_SW1, CS14_SW1, CS13_SW1}, // LED 42
    {0, CS15_SW4, CS14_SW4, CS13_SW4}, // LED 43
    {0, CS15_SW5, CS14_SW5, CS13_SW5}, // LED 44
    {0, CS15_SW8, CS14_SW8, CS13_SW8}, // LED 45

    {0, CS18_SW1, CS17_SW1, CS16_SW1}, // LED 46
    {0, CS18_SW2, CS17_SW2, CS16_SW2}, // LED 47
    {0, CS18_SW3, CS17_SW3, CS16_SW3}, // LED 48

    {0, CS21_SW1, CS20_SW1, CS19_SW1}, // LED 49
    {0, CS21_SW2, CS20_SW2, CS19_SW2}, // LED 50
    {0, CS21_SW3, CS20_SW3, CS19_SW3}, // LED 51
    {0, CS21_SW4, CS20_SW4, CS19_SW4}, // LED 52
    {0, CS21_SW5, CS20_SW5, CS19_SW5}, // LED 53
    {0, CS21_SW6, CS20_SW6, CS19_SW6}, // LED 54
    {0, CS21_SW7, CS20_SW7, CS19_SW7}, // LED 55
    {0, CS21_SW8, CS20_SW8, CS19_SW8}, // LED 56
    {0, CS39_SW1, CS38_SW1, CS37_SW1}, // LED 57

    {0, CS24_SW2, CS23_SW2, CS22_SW2}, // LED 58
    {0, CS24_SW3, CS23_SW3, CS22_SW3}, // LED 59
    {0, CS24_SW4, CS23_SW4, CS22_SW4}, // LED 60
    {0, CS24_SW5, CS23_SW5, CS22_SW5}, // LED 61
    {0, CS24_SW6, CS23_SW6, CS22_SW6}, // LED 62
    {0, CS24_SW7, CS23_SW7, CS22_SW7}, // LED 63
    {0, CS24_SW8, CS23_SW8, CS22_SW8}, // LED 64
    {0, CS24_SW9, CS23_SW9, CS22_SW9}, // LED 65
    {0, CS39_SW2, CS38_SW2, CS37_SW2}, // LED 66

    {0, CS27_SW1, CS26_SW1, CS25_SW1}, // LED 67
    {0, CS27_SW2, CS26_SW2, CS25_SW2}, // LED 68
    {0, CS27_SW3, CS26_SW3, CS25_SW3}, // LED 69
    {0, CS27_SW4, CS26_SW4, CS25_SW4}, // LED 70
    {0, CS27_SW5, CS26_SW5, CS25_SW5}, // LED 71
    {0, CS27_SW6, CS26_SW6, CS25_SW6}, // LED 72
    {0, CS27_SW7, CS26_SW6, CS25_SW7}, // LED 73
    {0, CS39_SW3, CS38_SW3, CS37_SW3}, // LED 74
    {0, CS27_SW3, CS26_SW3, CS25_SW3}, // LED 75

    {0, CS30_SW1, CS29_SW1, CS28_SW1}, // LED 76
    {0, CS30_SW2, CS29_SW2, CS28_SW2}, // LED 77
    {0, CS30_SW3, CS29_SW3, CS28_SW3}, // LED 78
    {0, CS30_SW4, CS29_SW4, CS28_SW4}, // LED 79
    {0, CS30_SW5, CS29_SW5, CS28_SW5}, // LED 80
    {0, CS30_SW6, CS29_SW6, CS28_SW6}, // LED 81
    {0, CS30_SW7, CS29_SW7, CS28_SW7}, // LED 82
    {0, CS30_SW8, CS29_SW8, CS28_SW8}, // LED 83
    {0, CS39_SW4, CS38_SW4, CS37_SW4}, // LED 84

    {0, CS33_SW1, CS32_SW1, CS31_SW1}, // LED 85
    {0, CS33_SW2, CS32_SW2, CS31_SW2}, // LED 86
    {0, CS33_SW3, CS32_SW3, CS31_SW3}, // LED 87
    {0, CS33_SW4, CS32_SW4, CS31_SW4}, // LED 88
    {0, CS33_SW5, CS32_SW5, CS31_SW5}, // LED 89
    {0, CS33_SW6, CS32_SW6, CS31_SW6}, // LED 90
    {0, CS33_SW7, CS32_SW7, CS31_SW7}, // LED 91
    {0, CS33_SW8, CS32_SW8, CS31_SW8}, // LED 92
    {0, CS39_SW5, CS38_SW5, CS37_SW5}, // LED 93

    {0, CS36_SW1, CS35_SW1, CS34_SW1}, // LED 94
    {0, CS36_SW2, CS35_SW2, CS34_SW2}, // LED 95
    {0, CS36_SW3, CS35_SW3, CS34_SW3}, // LED 96
    {0, CS36_SW4, CS35_SW4, CS34_SW4}, // LED 97
};

led_config_t g_led_config = { {
  // Key Matrix to LED Index
  {    31,     73,      5,     84, NO_LED,     29,     33,     77,     75,     92,     78, NO_LED,     88,     79,     52, NO_LED, },
  {NO_LED, NO_LED, NO_LED,     47,     94,     27,     35,     76,     90,     32, NO_LED, NO_LED,     34,     81,     56,     67, },
  {NO_LED, NO_LED,     93, NO_LED,      2,     19,     39,     40,     55, NO_LED,     26, NO_LED, NO_LED,     63, NO_LED,     91, },
  {NO_LED,     46,      1, NO_LED,     20,     21,      7,      8,     59, NO_LED,     24,     74, NO_LED,     70, NO_LED, NO_LED, },
  {NO_LED, NO_LED,     16, NO_LED,     44,     41,      9,     14,     68, NO_LED,     22, NO_LED, NO_LED,     61,     62, NO_LED, },
  {NO_LED, NO_LED,     15, NO_LED,     12,     13,     11,     10,     69, NO_LED,     17, NO_LED, NO_LED,     60,     65, NO_LED, },
  {NO_LED, NO_LED, NO_LED, NO_LED,     18,      3,      4,      6,     66, NO_LED,     58,     89, NO_LED,     57,     54, NO_LED, },
  {NO_LED, NO_LED,     36, NO_LED, NO_LED,     25,     43,     49,     51, NO_LED,     50, NO_LED, NO_LED,     48,     37, NO_LED, },
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
// clang-format on
