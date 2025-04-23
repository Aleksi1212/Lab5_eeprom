#include "pico/stdlib.h"
#include "structs.h"

#ifndef LED_H
#define LED_H

void read_led_states_from_eeprom(ledstate_t ls[3], uint16_t addresses[][2]);
void handle_led_state(uint sw, bool *was_pressed, ledstate_t *ls, int i);

#endif