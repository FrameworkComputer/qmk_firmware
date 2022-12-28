// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

enum lotus_keycodes {
  // Custom keycode to change screen modes (e.g. enable external screen)
  KC_SCRN = SAFE_RANGE,
};

// The layouts are all mostly the same, except for the extra keys that the different layouts have
#define LAYOUT_78_ansi( \
  K110,  K112,  K113,  K114,  K115,  K116,  K117,  K118,  K119,  K120,  K121,  K122,  K126,   K76, \
    K1,    K2,    K3,    K4,    K5,    K6,    K7,    K8,    K9,   K10,   K11,   K12,   K13,   K15, \
   K16,   K17,   K18,   K19,   K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29, \
   K30,   K31,   K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K40,   K41,      K43,     \
   K44,   K46,   K47,   K48,   K49,   K50,   K51,   K52,   K53,   K54,   K55,        K57,          \
   K58,   K59,  K127,   K60,                    K61,       K62,   K64,   K79,    K83, K84,    K89  \
) \
{ \
  {   K48,   K76,   K17,   K62,  KC_NO,  K47,   K49,   K52,   K54,   K57,  K53,  KC_NO,   K64,   K55,   K41, KC_NO }, \
  { KC_NO, KC_NO, KC_NO,   K60,   K61,   K46,   K50,   K51,   K84,   K44, KC_NO, KC_NO,   K58,   K83,   K43,  K119 }, \
  { KC_NO, KC_NO,   K59, KC_NO,   K19,  K113,   K35,   K36,   K29, KC_NO,  K118, KC_NO, KC_NO,   K12, KC_NO,   K89 }, \
  { KC_NO,  K127,   K16, KC_NO,  K115,  K112,   K21,   K22,   K25, KC_NO,  K117,  K120, KC_NO, KC_NO, KC_NO,  K126 }, \
  { KC_NO, KC_NO,    K1, KC_NO,   K30,   K32,    K6,    K7,  K121, KC_NO,  K116, KC_NO, KC_NO,   K11,   K13, KC_NO }, \
  { KC_NO, KC_NO,    K2, KC_NO,    K4,    K3,    K5,    K8,   K10, KC_NO,    K9, KC_NO, KC_NO,   K26,   K15, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO,  K114,   K18,   K20,   K23,  K122, KC_NO,   K24,   K79, KC_NO,   K27,   K28, KC_NO }, \
  { KC_NO, KC_NO,  K31,  KC_NO, KC_NO,  K110,   K34,   K37,   K39, KC_NO,   K38, KC_NO, KC_NO,   K40,   K33, KC_NO }  \
}

// K29/K42 swapped in ANSI/ISO
#define LAYOUT_79_iso(  \
  K110,  K112,  K113,  K114,  K115,  K116,  K117,  K118,  K119,  K120,  K121,  K122,  K126,   K76, \
    K1,    K2,    K3,    K4,    K5,    K6,    K7,    K8,    K9,   K10,   K11,   K12,   K13,   K15, \
   K16,   K17,   K18,   K19,   K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,        \
   K30,   K31,   K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K40,   K41,   K42,   K43, \
   K44,   K45,   K46,   K47,   K48,   K49,   K50,   K51,   K52,   K53,   K54,   K55,      K57,     \
   K58,   K59,  K127,   K60,                    K61,       K62,   K64,   K79,   K83,   K84,   K89  \
) \
{ \
  {   K48,  K76,   K17,    K62, KC_NO,   K47,   K49,   K52,   K54,   K57,   K53, KC_NO,   K64,   K55,   K41, KC_NO }, \
  { KC_NO, KC_NO, KC_NO,   K60,   K61,   K46,   K50,   K51,   K84,   K44, KC_NO, KC_NO,   K58,   K83,   K43,  K119 }, \
  { KC_NO, KC_NO,  K59,  KC_NO,   K19,  K113,   K35,   K36,   K42, KC_NO,  K118, KC_NO, KC_NO,   K12, KC_NO,   K89 }, \
  { KC_NO, K127,   K16,  KC_NO,  K115,  K112,   K21,   K22,   K25, KC_NO,  K117,  K120, KC_NO, KC_NO, KC_NO,  K126 }, \
  { KC_NO, KC_NO,   K1,  KC_NO,   K30,   K32,    K6,    K7,  K121, KC_NO,  K116,        KC_NO,   K11,   K13, KC_NO }, \
  { KC_NO, KC_NO,   K2,  KC_NO,    K4,    K3,    K5,    K8,   K10, KC_NO,    K9,   K45, KC_NO,   K26,   K15, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO,  K114,   K18,   K20,   K23,  K122, KC_NO,   K24,   K79, KC_NO,   K27,   K28, KC_NO }, \
  { KC_NO, KC_NO,  K31,  KC_NO, KC_NO,  K110,   K34,   K37,   K39, KC_NO,   K38, KC_NO, KC_NO,   K40,   K33, KC_NO }, \
}

// JIS is based off ANSI with a few extra keys
#define LAYOUT_83_jis( \
  K110,  K112,  K113,  K114,  K115,  K116,  K117,  K118,  K119,  K120,  K121,  K122,  K126,   K76,       \
    K1,    K2,    K3,    K4,    K5,    K6,    K7,    K8,    K9,   K10,   K11,   K12,   K13,   K14,   K15,\
   K16,   K17,   K18,   K19,   K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,              \
   K30,   K31,   K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K40,   K41,   K42,   K43,       \
   K44,   K46,   K47,   K48,   K49,   K50,   K51,   K52,   K53,   K54,   K55,   K56,      K57,           \
   K58,   K59,  K127,   K60,  K131,   K61,  K132,  K133,   K62,   K64,   K79,   K83,   K84,   K89        \
) \
{ \
  {  K48,   K76,   K17,   K62, KC_NO,   K47,   K49,   K52,   K54,   K57,   K53,  K133,   K64,   K55,   K41,   K14 }, \
  { KC_NO, KC_NO, KC_NO,  K60,   K61,   K46,   K50,   K51,   K84,   K44, KC_NO,  K132,   K58,   K83,   K43,  K119 }, \
  { KC_NO, KC_NO,  K59, KC_NO,   K19,  K113,   K35,   K36,   K42, KC_NO,  K118, KC_NO, KC_NO,   K12, KC_NO,   K89 }, \
  { KC_NO, K127,   K16, KC_NO,  K115,  K112,   K21,   K22,   K25, KC_NO,  K117,  K120, KC_NO, KC_NO, KC_NO,  K126 }, \
  { KC_NO, KC_NO,   K1, KC_NO,   K30,   K32,    K6,    K7,  K121, KC_NO,  K116,   K56, KC_NO,   K11,   K13, KC_NO,}, \
  { KC_NO, KC_NO,   K2, KC_NO,    K4,    K3,    K5,    K8,   K10, KC_NO,    K9, KC_NO, KC_NO,   K26,   K15, KC_NO,}, \
  { KC_NO, KC_NO, K131, KC_NO,  K114,   K18,   K20,   K23,  K122, KC_NO,   K24,   K79, KC_NO,   K27,   K28, KC_NO,}, \
  { KC_NO, KC_NO,  K31, KC_NO, KC_NO,  K110,   K34,   K37,   K39, KC_NO,   K38, KC_NO, KC_NO,   K40,   K33, KC_NO,} \
}

// Same as gridpad with a few keys removed
#define LAYOUT_numpad( \
    H1,    H2,    H3,    H4, \
   K90,   K95,  K100,  K105, \
   K91,   K96,  K101,        \
   K92,   K97,  K102,  K106, \
   K93,   K98,  K103,        \
       K99,     K104,  K108  \
) \
{ \
  {   K90,   K91,   K92,   K93,   K95,   K96,   K97,   K98 }, \
  {   K99,  K100,  K101,  K102,  K103,  K104,  K105,  K106 }, \
  {  K108,    H1,    H2, KC_NO,    H4, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO,    H3, KC_NO, KC_NO, KC_NO }, \
}

#define LAYOUT_gridpad( \
    H1,    H2,    H3,    H4, \
   K90,   K95,  K100,  K105, \
   K91,   K96,  K101,  K107, \
   K92,   K97,  K102,  K106, \
   K93,   K98,  K103,  K109, \
   K99,  K110,  K104,  K108  \
) \
{ \
  {   K90,   K91,   K92,   K93,   K95,   K96,   K97,   K98 }, \
  {   K99,  K100,  K101,  K102,  K103,  K104,  K105,  K106 }, \
  {  K108,    H1,    H2, KC_NO,    H4,  K107,  K109,  K110 }, \
  { KC_NO, KC_NO, KC_NO, KC_NO,    H3, KC_NO, KC_NO, KC_NO }, \
}
