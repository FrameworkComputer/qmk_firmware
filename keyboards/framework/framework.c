// Copyright 2022 Framework Computer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "framework.h"
#include "os_detection.h"

bool is_suspended = false;

void keyboard_post_init_kb(void) {
  keyboard_post_init_user();

  // Enable debug output
  debug_enable = true;
  debug_matrix = true;
  debug_keyboard = true;
}

/**
 * Hook into early keyboard initialization
*/
void keyboard_pre_init_kb(void) {
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

  is_suspended = true;

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
