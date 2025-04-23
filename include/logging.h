#include <stdio.h>
#include "structs.h"

#ifndef LOGGING_H
#define LOGGING_H

#define LOG(fmt, ...)                                                          \
  do {                                                                         \
    /* Create a temporary buffer for the formatted log entry */                \
    char log_buffer[128];                                                      \
                                                                               \
    /* Include timestamp in the formatted string */                            \
    int len = snprintf(log_buffer, sizeof(log_buffer),                         \
                        "Time since boot (%lld s) --- " fmt,                   \
                       (uint64_t)(time_us_64() / 1000000), ##__VA_ARGS__);     \
                                                                               \
    /* Ensure the string fits in the buffer */                                 \
    if ((uint32_t)len >= sizeof(log_buffer)) {                                 \
      log_buffer[sizeof(log_buffer) - 1] =                                     \
          '\0'; /* Null-terminate if overflow */                               \
    }                                                                          \
                                                                               \
    printf("%s\n", log_buffer);                                                \
                                                                               \
  } while (0)

void clear_terminal();

#endif