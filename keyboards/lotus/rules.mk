VIA_ENABLE = yes
RAW_ENABLE = no

# Enabled in info.json
# EXTRAKEY_ENABLE = yes          # Audio control and System control

# Keep it enabled for all, at the moment. Because The rpi-pico has an LED that I can use for debugging
BACKLIGHT_ENABLE = yes
BACKLIGHT_DRIVER = pwm

POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = analog_joystick

# Custom matrix scanning code via ADC
CUSTOM_MATRIX = lite
SRC += matrix.c

SRC += factory.c

DEFAULT_FOLDER = lotus/ansi
