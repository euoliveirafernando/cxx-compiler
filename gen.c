#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ctype.h"
#include "graph.h"
#include "opcode.h"
#include "gen.h"

void byte(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(output, format, args);
    fprintf(output, " ");
    va_end(args);
}

void gen(graph * g)
{
}