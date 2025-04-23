#include <stdio.h>
#include "structs.h"
#include "logging.h"

void clear_terminal()
{
    printf("\033[2J\033[H");
    fflush(stdout);
}
