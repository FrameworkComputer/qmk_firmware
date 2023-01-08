#include "analog.h"
#include "matrix.h"

int16_t analogReadPin(pin_t pin) {
    return custom_analog_read_pin(pin);
}