#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "constants.h"
#include "pin_init.h"
#include "led.h"
#include "eeprom.h"
#include "logging.h"

int main(void) {
    stdio_init_all();
    clear_terminal();

    printf("BOOT\n");

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

    ledstate_t led_states[LED_COUNT] = {
        { LED_1, 0, 0 },
        { LED_2, 0, 0 },
        { LED_3, 0, 0 }
    };

    uint16_t addresses[3][2] = {
        { EEPROM_MAX_ADDRESS - 5, EEPROM_MAX_ADDRESS - 6 },
        { EEPROM_MAX_ADDRESS - 3, EEPROM_MAX_ADDRESS - 2 },
        { EEPROM_MAX_ADDRESS - 1, EEPROM_MAX_ADDRESS }
    };

    read_led_states_from_eeprom(led_states, addresses);

    char input[10];

    while (true)
    {
        for (int sw_i = 0; sw_i < SW_COUNT; sw_i++) {
            handle_led_state(
                sw_states[sw_i][0],
                (bool*)(&sw_states[sw_i][1]),
                &led_states[sw_i],
                sw_i+1
            );
        }
        
        for (int led_i = 0; led_i < LED_COUNT; led_i++) {
            gpio_put(led_states[led_i].led, led_states[led_i].state);
            
            eeprom_write_bytes(addresses[led_i][0], led_states[led_i].state);
            eeprom_write_bytes(addresses[led_i][1], led_states[led_i].not_state);
        }

        if (read_user_input(input, sizeof(input))) {
            if (strcmp(input, "read") == 0) {
                read_log();
            } else if (strcmp(input, "erase") == 0) {
                erase_log();
                write_log_entry("Log erased");
            } else {
                fprintf(stderr, "Invalid command\n");
            }
        }

        sleep_ms(10);
    }
    

    return 0;
}
