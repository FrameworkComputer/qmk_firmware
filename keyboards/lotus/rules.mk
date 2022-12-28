VIA_ENABLE = yes

# Enabled in info.json
# EXTRAKEY_ENABLE = yes          # Audio control and System control

SERIAL_DRIVER = vendor

# Keep it enabled for all, at the moment. Because The rpi-pico has an LED that I can use for debugging
BACKLIGHT_ENABLE = yes
BACKLIGHT_DRIVER = pwm

# Custom matrix scanning code via ADC
CUSTOM_MATRIX = lite
SRC += matrix.c

# Debugging (enabled in info.json)
#CONSOLE_ENABLE = yes
