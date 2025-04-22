#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "constants.h"
#include "pin_init.h"
#include "led.h"
#include "eeprom.h"

int main(void) {
    stdio_init_all();

    init_gpio_in(SW_0);
    init_gpio_in(SW_1);
    init_gpio_in(SW_2);

    init_gpio_out(LED_1);
    init_gpio_out(LED_2);
    init_gpio_out(LED_3);

    init_i2c(EEPROM_PORT, EEPROM_SDA_PIN, EEPROM_SCL_PIN, EEPROM_BAUDRATE);

    uint sw_states[SW_COUNT][2] = {
        { SW_0, 0 },
        { SW_1, 0 },
        { SW_2, 0 }
    };

    uint led_states[LED_COUNT][2] = {
        { LED_1, 0 },
        { LED_2, 0 },
        { LED_3, 0 }
    };

    uint16_t addresses[3][2] = {
        { EEPROM_MAX_ADDRESS - 5, EEPROM_MAX_ADDRESS - 6},
        { EEPROM_MAX_ADDRESS - 3, EEPROM_MAX_ADDRESS - 2 },
        { EEPROM_MAX_ADDRESS - 1, EEPROM_MAX_ADDRESS }
    };

    for (int i = 0; i < 3; i++) {
        uint8_t byte = eeprom_read_byte(addresses[i][0]);
        uint8_t inverted_byte = eeprom_read_byte(addresses[i][1]);

        if ((byte == 0 || byte == 1) && byte == (uint8_t)(~inverted_byte)) {
            led_states[i][1] = byte;
        }
    }

    while (true)
    {
        for (int sw_i = 0; sw_i < SW_COUNT; sw_i++) {
            handle_led_state(
                sw_states[sw_i][0],
                (bool*)(&sw_states[sw_i][1]),
                (bool*)(&led_states[sw_i][1])
            );

        }

        for (int led_i = 0; led_i < LED_COUNT; led_i++) {
            gpio_put(led_states[led_i][0], led_states[led_i][1]);

            eeprom_write_bytes(addresses[led_i][0], led_states[led_i][1]);
            eeprom_write_bytes(addresses[led_i][1], (uint8_t)(~led_states[led_i][1]));
        }
    }
    

    return 0;
}
