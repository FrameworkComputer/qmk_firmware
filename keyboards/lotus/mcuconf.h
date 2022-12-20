// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <mcuconf.h>

#undef RP_SIO_USE_UART0
#define RP_SIO_USE_UART0 TRUE
#undef RP_SIO_USE_UART1
#define RP_SIO_USE_UART1 FALSE

// For RGB backlight
#undef RP_ADC_USE_ADC1
#define RP_ADC_USE_ADC1 TRUE

// For single-zone backlight
#undef RP_PWM_USE_PWM4
#define RP_PWM_USE_PWM4 TRUE
