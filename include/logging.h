#include <stdio.h>
#include "structs.h"

#ifndef LOGGING_H
#define LOGGING_H

void clear_terminal();
void erase_log();
void write_log_entry(const char *log_entry);
void read_log();
void log_message(const char *fmt, ...);
bool read_user_input(char *dst, size_t dst_size);

#endif