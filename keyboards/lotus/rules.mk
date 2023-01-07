VIA_ENABLE = yes
RAW_ENABLE = no

# Enabled in info.json
# EXTRAKEY_ENABLE = yes          # Audio control and System control

# Keep it enabled for all, at the moment. Because The rpi-pico has an LED that I can use for debugging
BACKLIGHT_ENABLE = yes
BACKLIGHT_DRIVER = pwm

# Custom matrix scanning code via ADC
CUSTOM_MATRIX = lite
SRC += matrix.c

SRC += factory.c

QUANTUM_LIB_SRC += max7219.c spi_master.c

DEFAULT_FOLDER = lotus/ansi
