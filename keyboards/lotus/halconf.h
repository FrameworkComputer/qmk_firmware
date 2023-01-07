// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define HAL_USE_SIO TRUE

// For single-zone backlight
#define HAL_USE_PWM TRUE

#define HAL_USE_ADC TRUE

// For MAX7219
#define HAL_USE_SPI TRUE
#define SPI_USE_WAIT TRUE
#define SPI_SELECT_MODE SPI_SELECT_MODE_PAD

#include_next <halconf.h>
