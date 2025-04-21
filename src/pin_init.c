#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pin_init.h"

void init_gpio_in(uint gpio)
{
    gpio_init(gpio);
	gpio_set_dir(gpio, GPIO_IN);
  	gpio_pull_up(gpio);
}

void init_gpio_out(uint gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
}

void init_i2c(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate)
{
    i2c_init(i2c, baudrate);

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
}