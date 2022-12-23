EXTRAKEY_ENABLE = yes          # Audio control and System control

SERIAL_DRIVER = vendor

# Backlight for one variant
BACKLIGHT_ENABLE = yes
BACKLIGHT_DRIVER = pwm

# TODO: Enable RGB matrix for one variant
RGB_MATRIX_ENABLE = no
RGB_MATRIX_DRIVER = IS31FL3745

# Custom matrix scanning code via ADC
CUSTOM_MATRIX = lite
SRC += matrix.c

# Debugging (enabled in info.json)
#CONSOLE_ENABLE = yes
