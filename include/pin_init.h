#include "pico/stdlib.h"

#ifndef PIN_INIT_H
#define PIN_INIT_H

void init_gpio_in(uint gpio);
void init_gpio_out(uint gpio);
void init_i2c(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate);

#endif