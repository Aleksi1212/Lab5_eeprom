#include <stdint.h>
#include "pico/stdlib.h"

#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

typedef struct ledstate_t {
    const uint led;
    uint8_t state;
    uint8_t not_state;
} ledstate_t;

#endif