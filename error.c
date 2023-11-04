#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

void error(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    if (filename != NULL)
        printf("%s: %d: ", filename, line_number);
    printf(color_error "error: " color_reset);
    vprintf(format, args);
    printf("\n");
    va_end(args);
    if (input != NULL)
        fclose(input);
    exit(EXIT_FAILURE);
}