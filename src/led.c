#include "pico/stdlib.h"
#include "led.h"

void handle_led_state(uint sw, bool *pressed, bool *led_state)
{
    if (*pressed && gpio_get(sw) == 0) {
        *pressed = false;
    }
    else if (!(*pressed) && gpio_get(sw) > 0) {
        *led_state = !(*led_state);
        *pressed = true;
        sleep_ms(10);
    }
}