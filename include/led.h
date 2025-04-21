#include "pico/stdlib.h"

#ifndef LED_H
#define LED_H

void handle_led_state(uint sw, bool *pressed, bool *led_state);

#endif