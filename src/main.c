#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/util/queue.h"

#include "constants.h"
#include "pin_init.h"
#include "led.h"
#include "eeprom.h"


static queue_t queue;

void interrupt_callback(uint gpio, uint32_t events)
{
    if ((events & GPIO_IRQ_EDGE_RISE)) {
        absolute_time_t now = time_us_64();
        queue_try_add(&queue, &now);
    }
}

int main(void) {
    stdio_init_all();

    init_gpio_in(SW_0);
    init_gpio_in(SW_1);
    init_gpio_in(SW_2);

    init_gpio_out(LED_1);
    init_gpio_out(LED_2);
    init_gpio_out(LED_3);

    init_i2c(EEPROM_PORT, EEPROM_SDA_PIN, EEPROM_SCL_PIN, EEPROM_BAUDRATE);

    queue_init(&queue, sizeof(absolute_time_t), 10);

    // gpio_set_irq_enabled_with_callback(SW_0, GPIO_IRQ_EDGE_RISE, true, &interrupt_callback);
    // gpio_set_irq_enabled_with_callback(SW_1, GPIO_IRQ_EDGE_RISE, true, &interrupt_callback);
    // gpio_set_irq_enabled_with_callback(SW_2, GPIO_IRQ_EDGE_RISE, true, &interrupt_callback);

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
    
    uint64_t last_interrupt_time = { 0 };
    uint64_t now = { 0 };

    while (true)
    {
        for (int sw_i = 0; sw_i < SW_COUNT; sw_i++) {
            handle_led_state(
                sw_states[sw_i][0],
                (bool*)(&sw_states[sw_i][1]),
                &led_states[sw_i]
            );
        }
        
        for (int led_i = 0; led_i < LED_COUNT; led_i++) {
            gpio_put(led_states[led_i].led, led_states[led_i].state);
            
            eeprom_write_bytes(addresses[led_i][0], led_states[led_i].state);
            eeprom_write_bytes(addresses[led_i][1], led_states[led_i].not_state);
            
        }

        sleep_ms(10);

        // while (queue_try_remove(&queue, &now)) {
        //     if (absolute_time_diff_us(last_interrupt_time, now) > 200 * 1000) {
        //         for (int led_i = 0; led_i < LED_COUNT; led_i++) {
        //             printf("Led_%d state: %u\n", led_i+1, (uint8_t)led_states[led_i].not_state);
        //         }
        //         printf("Time since power on: %u\n", last_interrupt_time);

        //         last_interrupt_time = now;
        //     }
        // }
    }
    

    return 0;
}
