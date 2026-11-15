// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "framework.h"
#include "os_detection.h"
#include "hardware/structs/clocks.h"
#include "hardware/regs/clocks.h"
#include "hardware/structs/watchdog.h"
#include "hardware/structs/psm.h"
#include "hardware/regs/psm.h"
#include "bootloader.h"

bool is_suspended = false;

// Clocks that stay enabled while the MCU sleeps in WFI (CLOCKS SLEEP_EN0/1).
// WFI alone leaves the reset-default clock tree running. With these masks and
// SLEEPDEEP set, everything else (SRAM, ROM, XIP, bus fabric and the unused
// peripherals) stops until the next interrupt. WAKE_EN is unchanged: hardware
// restores all clocks before executing the handler, including the stack and
// vectors in SRAM and code in XIP.
//
// Required: The timer (scheduler tick), watchdog (tick generator, failsafe)
// and USB clocks must never be gated. ADC and I2C keep running because their
// ChibiOS drivers sleep the thread while waiting for the completion IRQ.
#define SLEEP_EN0_REQUIRED (CLOCKS_SLEEP_EN0_CLK_SYS_ADC_BITS | \
                            CLOCKS_SLEEP_EN0_CLK_ADC_ADC_BITS | \
                            CLOCKS_SLEEP_EN0_CLK_SYS_I2C1_BITS)
#define SLEEP_EN1_REQUIRED (CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS | \
                            CLOCKS_SLEEP_EN1_CLK_SYS_WATCHDOG_BITS | \
                            CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_BITS | \
                            CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS)
// Conservative: Additionally keep the clocks of all other peripherals that
// are configured. An aggressive mask coincided with an unrequested wake in
// bench testing during suspend, so keep GPIO/pad/SIO clocked until their wake
// behavior has been validated independently of the memory/bus clock savings.
#define SLEEP_EN0_CONSERVATIVE (SLEEP_EN0_REQUIRED | \
                                CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS | \
                                CLOCKS_SLEEP_EN0_CLK_SYS_SIO_BITS | \
                                CLOCKS_SLEEP_EN0_CLK_SYS_IO_BITS | \
                                CLOCKS_SLEEP_EN0_CLK_SYS_PADS_BITS | \
                                CLOCKS_SLEEP_EN0_CLK_SYS_DMA_BITS)
#define SLEEP_EN1_CONSERVATIVE (SLEEP_EN1_REQUIRED | \
                                CLOCKS_SLEEP_EN1_CLK_SYS_UART0_BITS | \
                                CLOCKS_SLEEP_EN1_CLK_PERI_UART0_BITS)

#ifndef FRAMEWORK_NO_AWAKE_CLOCK_GATING
/**
 * Gate unused clocks whenever the MCU sleeps in WFI, also while awake.
 *
 * The main loop sleeps between matrix scans (see matrix_scan_rate_limit) and
 * the drivers sleep while waiting for their interrupts, so most of the time
 * is spent in WFI.
 */
static void awake_clock_gating_init(void) {
#    ifdef FRAMEWORK_AWAKE_CLOCK_GATING_LEAN
    // GPIO is only read and written while the core runs, its clocks can stop
    // during WFI without changing the pad configuration or output levels.
    // The UART is unused, the console goes through USB and nothing uses DMA.
    // Unlike in suspend, the PWM counter has to keep running for the white
    // backlight.
    clocks_hw->sleep_en0 = SLEEP_EN0_REQUIRED | CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS;
    clocks_hw->sleep_en1 = SLEEP_EN1_REQUIRED;
#    else
    clocks_hw->sleep_en0 = SLEEP_EN0_CONSERVATIVE;
    clocks_hw->sleep_en1 = SLEEP_EN1_CONSERVATIVE;
#    endif
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __DSB();
}
#endif

#ifndef FRAMEWORK_NO_SUSPEND_CLOCK_GATING
static uint32_t suspend_sleep_en0;
static uint32_t suspend_sleep_en1;
static uint32_t suspend_sleepdeep;

// Saves the masks used while awake (see awake_clock_gating_init, or the reset
// default with everything enabled) and only ever removes clocks from them.
static void suspend_clock_gating_enter(void) {
    suspend_sleep_en0 = clocks_hw->sleep_en0;
    suspend_sleep_en1 = clocks_hw->sleep_en1;
    suspend_sleepdeep = SCB->SCR & SCB_SCR_SLEEPDEEP_Msk;

#ifdef FRAMEWORK_SUSPEND_CLOCK_GATING_LEAN
    // GPIO is only polled after the timer has woken us up, its clocks can stop
    // during WFI without changing the pad configuration or output levels.
    // The UART is unused, the console goes through USB. PWM doesn't need to
    // make progress while the backlight is off and nothing uses DMA.
    clocks_hw->sleep_en0 = suspend_sleep_en0 & SLEEP_EN0_REQUIRED;
    clocks_hw->sleep_en1 = suspend_sleep_en1 & SLEEP_EN1_REQUIRED;
#else
    clocks_hw->sleep_en0 = suspend_sleep_en0 & SLEEP_EN0_CONSERVATIVE;
    clocks_hw->sleep_en1 = suspend_sleep_en1 & SLEEP_EN1_CONSERVATIVE;
#endif
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __DSB();
}

static void suspend_clock_gating_leave(void) {
    clocks_hw->sleep_en0 = suspend_sleep_en0;
    clocks_hw->sleep_en1 = suspend_sleep_en1;
    SCB->SCR = (SCB->SCR & ~SCB_SCR_SLEEPDEEP_Msk) | suspend_sleepdeep;
    __DSB();
}
#endif

#ifdef FRAMEWORK_SUSPEND_SLOW_CLOCK
#    ifdef FRAMEWORK_SYS_CLOCK_48MHZ
#        error "FRAMEWORK_SUSPEND_SLOW_CLOCK restores 125MHz, don't combine it with FRAMEWORK_SYS_CLOCK_48MHZ"
#    endif
#    include "usb_main.h"
#    include "hardware/clocks.h"
#    include "hardware/pll.h"

#    if FRAMEWORK_SUSPEND_SLOW_CLOCK_DIV < 1 || FRAMEWORK_SUSPEND_SLOW_CLOCK_DIV > 16
#        error "FRAMEWORK_SUSPEND_SLOW_CLOCK_DIV must be between 1 and 16"
#    endif

// Set by the USB ISR on every new suspend, cleared whenever the fast clock is
// restored. Prevents going slow again in the same suspend after the clock was
// restored for a remote wakeup, while the host is still resuming the bus.
static volatile bool slow_clock_armed  = false;
static volatile bool slow_clock_active = false;

#    ifdef FRAMEWORK_SUSPEND_LOW_VOLTAGE
#        include "hardware/structs/vreg_and_chip_reset.h"
#        include "hardware/regs/vreg_and_chip_reset.h"
#        include "hardware/structs/timer.h"

// VREG VSEL: 0.05V steps, 0x6 = 0.85V ... 0xb = 1.10V (reset value)
#        define VREG_VSEL_1V10 0xb
#        if FRAMEWORK_SUSPEND_VSEL < 0x8 || FRAMEWORK_SUSPEND_VSEL > VREG_VSEL_1V10
#            error "FRAMEWORK_SUSPEND_VSEL must be between 0x8 (0.95V) and 0xb (1.10V)"
#        endif

static void core_voltage_set(uint32_t vsel) {
    hw_write_masked(&vreg_and_chip_reset_hw->vreg, vsel << VREG_AND_CHIP_RESET_VREG_VSEL_LSB, VREG_AND_CHIP_RESET_VREG_VSEL_BITS);
}

/**
 * Back to 1.10V. Must have settled before the system clock goes back to
 * 125MHz, so busy-wait for 1ms and then for the regulator to report that it's
 * in regulation (at most 10ms in total). Uses the raw system timer, which is
 * clocked from clk_ref and doesn't depend on clk_sys or interrupts.
 */
static void core_voltage_restore(void) {
    core_voltage_set(VREG_VSEL_1V10);
    uint32_t start = timer_hw->timerawl;
    while (true) {
        uint32_t elapsed = timer_hw->timerawl - start;
        if (elapsed >= 10000 || (elapsed >= 1000 && (vreg_and_chip_reset_hw->vreg & VREG_AND_CHIP_RESET_VREG_ROK_BITS))) {
            break;
        }
    }
}

/**
 * The regulator setting survives a watchdog reset. If that happens while the
 * voltage is lowered (hang, development failsafe), restore it before
 * anything switches to 125MHz (__late_init of the bootloader double tap
 * detection, halInit). Overrides the weak ChibiOS startup hook, which runs
 * right after reset from the ring oscillator at a few MHz, before .data and
 * .bss are set up. The timer isn't ticking yet.
 */
void __cpu_init(void) {
    if ((vreg_and_chip_reset_hw->vreg & VREG_AND_CHIP_RESET_VREG_VSEL_BITS) != (VREG_VSEL_1V10 << VREG_AND_CHIP_RESET_VREG_VSEL_LSB)) {
        core_voltage_set(VREG_VSEL_1V10);
        for (volatile uint32_t i = 0; i < 20000; i++) {
        }
    }
}
#    endif

/**
 * Run clk_sys at 48MHz from the USB PLL and power down the system PLL while
 * the host has suspended the bus.
 *
 * Only the system clock changes. USB and ADC keep their 48MHz from PLL_USB
 * and the system timer and watchdog their tick from clk_ref (XOSC), so bus
 * state detection, the scan interval and the ADC timing are unaffected.
 * clk_peri follows clk_sys, it only feeds the unused UART.
 *
 * Only on a real suspend of a configured device. The RP2040 also reports a
 * suspend when it's connected and the bus is idle before the host resets it
 * and enumeration must not run with a slow system clock. Neither must the
 * lid-closed idle path, where USB is still live.
 *
 * FRAMEWORK_SUSPEND_SLOW_CLOCK_DIV divides the 48MHz further. 24MHz (2) works.
 * At 12MHz (4, or straight from clk_ref) the main loop hangs in the suspend
 * matrix scan. With the scan skipped 12MHz runs fine, so most likely ADC
 * conversions don't complete with clk_sys that far below the 48MHz clk_adc.
 *
 * Called on every pass of the suspend loop. The USB ISR can restore the clock
 * at any time, so the state check and the switch are done with interrupts
 * disabled.
 */
static void suspend_slow_clock_enter(void) {
    if (slow_clock_active || !slow_clock_armed) {
        return;
    }
    chSysLock();
    if (slow_clock_armed && !slow_clock_active && USB_DRIVER.state == USB_SUSPENDED && USB_DRIVER.saved_state == USB_ACTIVE) {
        // 48MHz from the USB PLL, which has to keep running for USB anyway.
        // Goes through clk_ref (glitchless) while changing the aux mux.
        clock_configure(clk_sys,
                        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                        CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                        48 * MHZ, 48 * MHZ / FRAMEWORK_SUSPEND_SLOW_CLOCK_DIV);
        pll_deinit(pll_sys);
        SystemCoreClock   = clock_get_hz(clk_sys);
#    ifdef FRAMEWORK_SUSPEND_LOW_VOLTAGE
        core_voltage_set(FRAMEWORK_SUSPEND_VSEL);
#    endif
        slow_clock_active = true;
    }
    chSysUnlock();
}

/**
 * Restore the 125MHz system clock. Can be called from any context and when
 * the clock isn't slow.
 */
static void suspend_slow_clock_leave(void) {
    syssts_t sts = chSysGetStatusAndLockX();
    slow_clock_armed = false;
    if (slow_clock_active) {
#    ifdef FRAMEWORK_SUSPEND_LOW_VOLTAGE
        // Adds 1ms or a bit more, also when called from the USB ISR
        core_voltage_restore();
#    endif
        // Same as the pico-sdk clocks_init, which ChibiOS uses at boot.
        // Waits for the PLL to lock, which is far shorter than the 20ms of
        // resume signaling or the 10ms of a bus reset.
        pll_init(pll_sys, 1, 1500 * MHZ, 6, 2);
        clock_configure(clk_sys,
                        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                        CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                        125 * MHZ, 125 * MHZ);
        SystemCoreClock   = clock_get_hz(clk_sys);
        slow_clock_active = false;
    }
    chSysRestoreStatusX(sts);
}

/**
 * Runs in the USB ISR, before the event is handled.
 *
 * Anything other than a suspend means that the host is talking to us again
 * (resume, reset, configuration). The system clock must be back at full speed
 * before that continues and can't wait for the main loop to notice.
 */
void usb_event_isr_kb(usbevent_t event) {
    if (event == USB_EVENT_SUSPEND) {
        slow_clock_armed = true;
    } else {
        suspend_slow_clock_leave();
    }
}

/**
 * A key was pressed while suspended. The USB controller times the resume
 * signaling, so restore the clock before it starts.
 */
void usb_remote_wakeup_pre_kb(void) {
    suspend_slow_clock_leave();
}
#endif

#if defined(FRAMEWORK_SUSPEND_LOW_VOLTAGE) && !defined(FRAMEWORK_SUSPEND_SLOW_CLOCK)
#    error "FRAMEWORK_SUSPEND_LOW_VOLTAGE needs FRAMEWORK_SUSPEND_SLOW_CLOCK"
#endif

#if FRAMEWORK_DEV_FAILSAFE_S > 0
#    define FAILSAFE_MAGIC 0xF16FA115
// The RP2040 watchdog counts down twice per tick, so this is ~8.4s
#    define FAILSAFE_WATCHDOG_LOAD 0xFFFFFF

/**
 * Development failsafe for testing risky firmware without physical access.
 *
 * Arms the hardware watchdog. It's only fed from the matrix scan and only
 * until FRAMEWORK_DEV_FAILSAFE_S seconds after boot. When the firmware hangs,
 * USB never gets configured or the deadline passes, the watchdog resets the
 * MCU and the next boot goes straight into the ROM bootloader, where a known
 * good firmware can be flashed.
 */
static void failsafe_init(void) {
    if (watchdog_hw->reason && watchdog_hw->scratch[0] == FAILSAFE_MAGIC) {
        watchdog_hw->scratch[0] = 0;
        bootloader_jump();
    }
    watchdog_hw->scratch[0] = FAILSAFE_MAGIC;
    hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
    // Reset everything except the oscillators, same as pico-sdk
    psm_hw->wdsel = PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS);
    watchdog_hw->load = FAILSAFE_WATCHDOG_LOAD;
    hw_set_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
}

void framework_failsafe_feed(void) {
    if (timer_read32() < (uint32_t)FRAMEWORK_DEV_FAILSAFE_S * 1000) {
        watchdog_hw->load = FAILSAFE_WATCHDOG_LOAD;
    }
}
#endif

void keyboard_post_init_kb(void) {
  keyboard_post_init_user();

  // Enable debug output
  debug_enable = true;
  debug_matrix = true;
  debug_keyboard = true;
}

#ifdef FRAMEWORK_SYS_CLOCK_48MHZ
#include "hardware/clocks.h"
#include "hardware/pll.h"

/**
 * Run the system clock at 48MHz from the USB PLL instead of 125MHz from the
 * system PLL, and power down the system PLL.
 *
 * The dynamic power of the core, bus fabric and peripherals scales with the
 * clock frequency, and this applies even while the core is in WFI because the
 * clocks keep running.
 *
 * Must run before any peripheral driver that derives its timing from clk_sys
 * or clk_peri is started (I2C, PWM, UART). USB, ADC and the system timer
 * have their own clocks (clk_usb, clk_adc from PLL_USB and clk_ref from XOSC)
 * and are not affected.
 *
 * Note: The RP2040 PWM counter is clocked from clk_sys, so the backlight PWM
 * frequency changes. Duty cycle steps are unchanged.
 */
static void sys_clock_48mhz(void) {
    // clock_configure switches clk_sys to clk_ref first (glitchless), then
    // changes the aux mux to PLL_USB and switches back.
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ, 48 * MHZ);
    // clk_peri is already sourced from clk_sys, but reconfigure it so that
    // clock_get_hz(clk_peri) reports the right frequency to the drivers.
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    48 * MHZ, 48 * MHZ);
    // Nothing uses the system PLL anymore
    pll_deinit(pll_sys);

    SystemCoreClock = 48 * MHZ;
}
#endif

/**
 * Hook into early keyboard initialization
*/
void keyboard_pre_init_kb(void) {
#if FRAMEWORK_DEV_FAILSAFE_S > 0
    failsafe_init();
#endif
#ifdef FRAMEWORK_SYS_CLOCK_48MHZ
    sys_clock_48mhz();
#endif
#ifndef FRAMEWORK_NO_AWAKE_CLOCK_GATING
    awake_clock_gating_init();
#endif

    keyboard_pre_init_user();

    // Mark boot as done.
    // Before this, when holding down both alt keys QSPI_SS is pulled low to put
    // the RP2040 in bootloader mode during reset.
    setPinOutput(BOOT_DONE_GPIO);
    writePinLow(BOOT_DONE_GPIO);

    //// TODO: Do we ever need to disable it to save power?
    setPinOutput(MUX_ENABLE_GPIO);
    writePinHigh(MUX_ENABLE_GPIO);

    setPinOutput(IS31FL3743A_ENABLE_GPIO);
#if defined(RGB_MATRIX_ENABLE)
    writePinHigh(IS31FL3743A_ENABLE_GPIO);
#else
    writePinLow(IS31FL3743A_ENABLE_GPIO);
#endif
    setPinInput(SLEEP_GPIO);
}

#ifdef RGB_MATRIX_ENABLE
// From drivers/led/issi/is31flcommon.c
extern uint8_t g_pwm_buffer[DRIVER_COUNT][ISSI_MAX_LEDS];
extern bool    g_pwm_buffer_update_required[DRIVER_COUNT];

// Mirrors the state of the SDB pin. Set high in keyboard_pre_init_kb
static bool     rgb_driver_on  = true;
static uint32_t rgb_dark_since = 0;

/**
 * Turn the IS31FL3743A LED controller(s) on or off via the SDB pin.
 *
 * In hardware shutdown the controller draws only a few uA instead of several
 * mA, even when all LEDs are off.
 */
static void rgb_driver_power(bool on) {
    if (on == rgb_driver_on) {
        return;
    }
    rgb_driver_on = on;

    if (on) {
        writePinHigh(IS31FL3743A_ENABLE_GPIO);
        // The controller keeps its configuration in hardware shutdown, that's
        // what the suspend handling relied on already. But PWM writes that
        // happened while it was shut down might have been dropped. Force
        // re-sending the full PWM buffer on the next flush to be safe.
        for (int i = 0; i < DRIVER_COUNT; i++) {
            g_pwm_buffer_update_required[i] = true;
        }
    } else {
        writePinLow(IS31FL3743A_ENABLE_GPIO);
    }
}

/**
 * Check whether all LEDs are currently set to off in the PWM buffer.
 *
 * This is the data that's sent to the controller, so it covers all reasons
 * for the LEDs being dark: RGB disabled, brightness 0, RGB_MATRIX_TIMEOUT,
 * solid black, ...
 */
static bool rgb_all_dark(void) {
    for (int i = 0; i < DRIVER_COUNT; i++) {
        for (int led = 0; led < ISSI_MAX_LEDS; led++) {
            if (g_pwm_buffer[i][led]) {
                return false;
            }
        }
    }
    return true;
}
#endif

#if FRAMEWORK_BACKLIGHT_TIMEOUT_MS > 0
static bool     backlight_timed_out = false;
static uint32_t backlight_wake_time = 0;

/**
 * Turn the keyboard backlight (white and/or RGB) off after
 * FRAMEWORK_BACKLIGHT_TIMEOUT_MS without any keypress and back on with the
 * next keypress. The keypress is still sent to the host as normal.
 *
 * Opening the lid or the host resuming counts as activity too, so the
 * backlight is on when the user comes back to the laptop.
 *
 * Doesn't touch the saved backlight settings.
 */
static void backlight_timeout_task(void) {
    // QMK turns the backlight off while suspended and restores it on resume
    if (is_suspended) {
        return;
    }

    uint32_t idle_ms = MIN(last_input_activity_elapsed(), timer_elapsed32(backlight_wake_time));
    bool timed_out = idle_ms > FRAMEWORK_BACKLIGHT_TIMEOUT_MS;
    if (timed_out == backlight_timed_out) {
        return;
    }
    backlight_timed_out = timed_out;

#    ifdef RGB_MATRIX_ENABLE
    // Same mechanism QMK uses for USB suspend: Render all LEDs off and stop
    // running effects. housekeeping_task_kb then shuts down the controller.
    rgb_matrix_set_suspend_state(timed_out);
#    endif
#    ifdef BACKLIGHT_ENABLE
    if (timed_out) {
#        ifdef BACKLIGHT_BREATHING
        if (is_breathing()) {
            breathing_disable();
        }
#        endif
        backlight_set(0);
    } else {
        backlight_set(get_backlight_level());
#        ifdef BACKLIGHT_BREATHING
        if (is_backlight_breathing()) {
            breathing_enable();
        }
#        endif
    }
#    endif
}
#endif

/**
 * Runs once per main loop iteration, after keyboard_task.
 */
void housekeeping_task_kb(void) {
#if FRAMEWORK_BACKLIGHT_TIMEOUT_MS > 0
    backlight_timeout_task();
#endif

#if defined(RGB_MATRIX_ENABLE) && !defined(FRAMEWORK_NO_RGB_AUTO_SHUTDOWN)
    // While suspended, the controller is kept off by suspend_power_down_kb
    if (!is_suspended) {
        // RGB matrix renders new values into the PWM buffer in one main loop
        // iteration and flushes them over I2C in a later one. So this turns
        // the controller back on before the new values are sent to it.
        if (!rgb_all_dark()) {
            rgb_dark_since = timer_read32();
            rgb_driver_power(true);
        } else if (rgb_driver_on && timer_elapsed32(rgb_dark_since) > FRAMEWORK_RGB_SHUTDOWN_DELAY_MS) {
            // Delay to avoid toggling on every frame of animations that briefly go dark (e.g. breathing)
            rgb_driver_power(false);
        }
    }
#endif
}

/**
 * Called by QMK when the keyboard suspends
 */
void suspend_power_down_kb(void) {
  suspend_power_down_user();

#ifndef FRAMEWORK_NO_SUSPEND_CLOCK_GATING
  if (!is_suspended) {
    suspend_clock_gating_enter();
  }
#endif
  is_suspended = true;
#ifdef FRAMEWORK_SUSPEND_SLOW_CLOCK
  suspend_slow_clock_enter();
#endif

#ifdef RGB_MATRIX_ENABLE
#  ifndef NO_SUSPEND_POWER_DOWN
  rgb_driver_power(false);
#  endif
#endif
}

/**
 * Called by QMK when the keyboard wakes up from suspend
 */
void suspend_wakeup_init_kb(void) {
#ifdef FRAMEWORK_SUSPEND_SLOW_CLOCK
  // Backstop, the USB ISR has normally restored the clock already
  suspend_slow_clock_leave();
#endif
#ifndef FRAMEWORK_NO_SUSPEND_CLOCK_GATING
  if (is_suspended) {
    suspend_clock_gating_leave();
  }
#endif
  suspend_wakeup_init_user();

  is_suspended = false;

#ifdef RGB_MATRIX_ENABLE
#  ifndef NO_SUSPEND_POWER_DOWN
    rgb_driver_power(true);
    // Give the RGB effect time to render before deciding it's dark
    rgb_dark_since = timer_read32();
#  endif
#endif

#if FRAMEWORK_BACKLIGHT_TIMEOUT_MS > 0
  // QMK restored the backlight. Restart the timeout from now.
  backlight_timed_out = false;
  backlight_wake_time = timer_read32();
#endif
}

// If in BIOS mode, no matter what the keys have been remapped to, always send them as the F keys
bool bios_mode = true;
void set_bios_mode(bool val) {
  bios_mode = val;

#ifdef BIOS_HOTKEY_DEBUG
  if (bios_mode) {
    // Red
#if defined(RGB_MATRIX_ENABLE)
    rgb_matrix_sethsv_noeeprom(0, 0xFF, 0xFF);
    rgb_matrix_mode_noeeprom(1);
#endif
    writePin(GP24, 1);
  } else {
    // White
#if defined(RGB_MATRIX_ENABLE)
    rgb_matrix_sethsv_noeeprom(0, 0, 0xFF);
    rgb_matrix_mode_noeeprom(1);
#endif
    writePin(GP24, 0);
  }
#endif
}
bool handle_bios_hotkeys(uint16_t keycode, keyrecord_t *record) {
  // Not in bios mode, no special handling, handle as normal
  if (!bios_mode)
    return true;

  if (record->event.key.col == 5 && record->event.key.row == 3) {
      if (record->event.pressed) {
        register_code(KC_F1);
      } else {
        unregister_code(KC_F1);
      }
      return false;
  }
  if (record->event.key.col == 5 && record->event.key.row == 2) {
      if (record->event.pressed) {
        register_code(KC_F2);
      } else {
        unregister_code(KC_F2);
      }
      return false;
  }
  if (record->event.key.col == 4 && record->event.key.row == 6) {
      if (record->event.pressed) {
        register_code(KC_F3);
      } else {
        unregister_code(KC_F3);
      }
      return false;
  }
  if (record->event.key.col == 4 && record->event.key.row == 3) {
      if (record->event.pressed) {
        register_code(KC_F4);
      } else {
        unregister_code(KC_F4);
      }
      return false;
  }
  if (record->event.key.col == 10 && record->event.key.row == 4) {
      if (record->event.pressed) {
        register_code(KC_F5);
      } else {
        unregister_code(KC_F5);
      }
      return false;
  }
  if (record->event.key.col == 10 && record->event.key.row == 3) {
      if (record->event.pressed) {
        register_code(KC_F6);
      } else {
        unregister_code(KC_F6);
      }
      return false;
  }
  if (record->event.key.col == 10 && record->event.key.row == 2) {
      if (record->event.pressed) {
        register_code(KC_F7);
      } else {
        unregister_code(KC_F7);
      }
      return false;
  }
  if (record->event.key.col == 15 && record->event.key.row == 1) {
      if (record->event.pressed) {
        register_code(KC_F8);
      } else {
        unregister_code(KC_F8);
      }
      return false;
  }
  if (record->event.key.col == 11 && record->event.key.row == 3) {
      if (record->event.pressed) {
        register_code(KC_F9);
      } else {
        unregister_code(KC_F9);
      }
      return false;
  }
  if (record->event.key.col == 8 && record->event.key.row == 4) {
      if (record->event.pressed) {
        register_code(KC_F10);
      } else {
        unregister_code(KC_F10);
      }
      return false;
  }
  if (record->event.key.col == 8 && record->event.key.row == 6) {
      if (record->event.pressed) {
        register_code(KC_F11);
      } else {
        unregister_code(KC_F11);
      }
      return false;
  }
  if (record->event.key.col == 13 && record->event.key.row == 3) {
      if (record->event.pressed) {
        register_code(KC_F12);
      } else {
        unregister_code(KC_F12);
      }
      return false;
  }

  return true;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  process_record_user(keycode, record);

  os_variant_t os = detected_host_os();
  set_bios_mode(true);
#if defined(OS_DETECTION_RGB_DEBUG) && defined(RGB_MATRIX_ENABLE)
  rgb_matrix_mode_noeeprom(1);
  switch (os) {
      case OS_UNSURE:
        rgb_matrix_sethsv(0, 0, 255); // White
        break;
      case OS_LINUX:
        // Works on laptop and android
        rgb_matrix_sethsv(213, 255, 255); // purple
        break;
      case OS_WINDOWS:
        rgb_matrix_sethsv(170, 255, 255); // blue
        break;
      case OS_MACOS:
        rgb_matrix_sethsv(85, 255, 255); // green
        break;
      case OS_UEFI:
      case OS_IOS:
        // works on M1 mac
        rgb_matrix_sethsv(43, 255, 255); // yellow
        break;
      default:
        rgb_matrix_sethsv(125, 255, 255); // cyan
        break;
  }
#endif
  switch (os) {
      case OS_UNSURE:
        break;
      case OS_LINUX:
        set_bios_mode(false);
        break;
      case OS_WINDOWS:
        set_bios_mode(false);
        break;
      case OS_MACOS:
        break;
      case OS_UEFI:
      case OS_IOS:
        set_bios_mode(true);
        // works on M1 mac
        break;
  }
//#endif

  // Enable NKRO only in OS. Can cause issues with some pre-boot environments
  // Framework UEFI is ok, but I think grub can't handle it properly
  if (!bios_mode) {
    keymap_config.nkro = 1;
  }

  if (!handle_bios_hotkeys(keycode, record)) {
    return false;
  }

#ifdef RGB_MATRIX_ENABLE
  uint8_t h;
  uint8_t s;
  uint8_t v;
  uint8_t new_v;
#endif

  switch (keycode) {
    // Implement step brightness for RGB backlight
#ifdef RGB_MATRIX_ENABLE
    case RGB_MODE_FORWARD:
    case RGB_MODE_REVERSE:
      // Turn on if it was off
      if (!rgb_matrix_is_enabled()) {
        rgb_matrix_enable();
      }
      return true;
    case BL_STEP:
      if (record->event.pressed) {
        h = rgb_matrix_get_hue();
        s = rgb_matrix_get_sat();
        v = rgb_matrix_get_val();
        switch (v) {
          default: // Default when user set to a different level
          case 0:
            new_v = 85;
            break;
          case 85:
            new_v = 170;
            break;
          case 170:
            new_v = 255;
            break;
          case 255:
            new_v = 0;
            break;
        }
        rgb_matrix_sethsv(h, s, new_v);
      }
      return true;
#endif
    case KC_SCRN:
      // Simulate press WIN+P
      // Works (at least) on Windows and GNOME
      if (record->event.pressed) {
        register_code(KC_LGUI);
        register_code(KC_P);
      } else {
        unregister_code(KC_P);
        unregister_code(KC_LGUI);
      }
      return false; // Skip all further processing of this key
    // Copilot key
    // Simulate press WIN+SHIFT+F23
    case KC_CPLT:
      if (record->event.pressed) {
        register_code(KC_LGUI);
        register_code(KC_LSFT);
        register_code(KC_F23);
      } else {
        unregister_code(KC_F23);
        unregister_code(KC_LSFT);
        unregister_code(KC_LGUI);
      }
      return false; // Skip all further processing of this key
    default:
      return true; // Process all other keycodes normally
  }

  return true;
}

#ifdef BIOS_HOTKEY_DEBUG
// Ignore capslock updates in order to manually control it
bool led_update_kb(led_t led_state) {
    return false;
}
#endif
