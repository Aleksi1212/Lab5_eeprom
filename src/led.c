#include "pico/stdlib.h"
#include <stdio.h>
#include "led.h"

void handle_led_state(uint sw, bool *was_pressed, bool *led_state)
{
    bool pressed = !gpio_get(sw);

    if (pressed && !(*was_pressed)) {
        *was_pressed = true;
    }

    if (!pressed && *was_pressed) {
        printf("toggled\n");
        *led_state = !(*led_state);
        *was_pressed = false;
        sleep_ms(10);
    }
}