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

    bool sw_0_pressed = false,
        sw_1_pressed = false,
        sw_2_pressed = false;

    bool led_state[3] = { 1, 1, 1};

    uint leds[3] = { LED_1, LED_2, LED_3 };


    uint16_t addresses[3][2] = {
        { EEPROM_MAX_ADDRESS - 5, EEPROM_MAX_ADDRESS - 6},
        { EEPROM_MAX_ADDRESS - 3, EEPROM_MAX_ADDRESS - 2 },
        { EEPROM_MAX_ADDRESS - 1, EEPROM_MAX_ADDRESS }
    };

    for (int i = 0; i < 3; i++) {
        uint8_t byte = eeprom_read_byte(addresses[i][0]);
        uint8_t inverted_byte = eeprom_read_byte(addresses[i][1]);

        if ((byte == 0 || byte == 1) && byte == ~inverted_byte) {
            led_state[i] = byte;
        }
    }

    while (true)
    {
        handle_led_state(SW_0, &sw_0_pressed, &led_state[2]);
        handle_led_state(SW_1, &sw_1_pressed, &led_state[1]);
        handle_led_state(SW_2, &sw_2_pressed, &led_state[0]);
    
        for (int i = 0; i < 3; i++) {
            gpio_put(leds[i], led_state[i]);

            eeprom_write_bytes(addresses[i][0], led_state[i]);
            eeprom_write_bytes(addresses[i][1], (uint8_t)(~led_state[i]));
        }
    }
    

    return 0;
}
