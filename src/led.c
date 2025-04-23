#include "pico/stdlib.h"
#include <stdio.h>

#include "constants.h"
#include "led.h"
#include "eeprom.h"
#include "structs.h"
#include "logging.h"

static void set_led_state(ledstate_t *ls, uint8_t value)
{
    ls->state = value;
    ls->not_state = ~value;
}
static bool led_state_is_valid(ledstate_t *ls)
{
    return ls->state == (uint8_t)(~ls->not_state);
}

void read_led_states_from_eeprom(ledstate_t ls[3], uint16_t addresses[][2])
{
    int invalid_count = 0;

    for (int led_i = 0; led_i < LED_COUNT; led_i++) {
        uint8_t byte = eeprom_read_byte(addresses[led_i][0]);
        uint8_t inverted_byte = eeprom_read_byte(addresses[led_i][1]);

        if ((byte == 0 || byte == 1) && byte == (uint8_t)(~inverted_byte)) {
            set_led_state(&ls[led_i], byte);
        }

        if (!led_state_is_valid(&ls[led_i])) {
            invalid_count++;
        }
    }

    if (invalid_count == 3) {
        set_led_state(&ls[0], 0);
        set_led_state(&ls[1], 1);
        set_led_state(&ls[2], 0);
    }

    for (int led_i = 0; led_i < LED_COUNT; led_i++) {
        LOG("Led_%d: %s", led_i+1, ls[led_i].state ? "ON" : "OFF");
    }
    printf("\n");
}

void handle_led_state(uint sw, bool *was_pressed, ledstate_t *ls, int i)
{
    bool pressed = !gpio_get(sw);

    if (pressed && !(*was_pressed)) {
        *was_pressed = true;
    }

    if (!pressed && *was_pressed) {
        set_led_state(ls, !((*ls).state));
        *was_pressed = false;

        LOG("Led_%d: %s", i, ls->state ? "ON" : "OFF");
    }
}