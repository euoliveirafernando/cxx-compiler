#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "graph.h"
#include "cxx.h"

#define usage   "cxx compiler -- copyright (c) 2023\n"\
                "usage:    cxx <source> -o <object>\n"

static void program(char * name)
{
    graph * g;
    
    g = parse_file(name);
    
    gen(g);
}

static void parse(int argc, char ** argv)
{
    if (strcmp(argv[2], "-o") == 0) {
        output = fopen(argv[3], "w+");
        
        if (output == NULL)
            error("%s: unable to create file", argv[3]);
    } else
        error("missing object name");
    
    program(argv[1]);
}

int main(int argc, char ** argv)
{
    if (argc == 4)
        parse(argc, argv);
    else
        printf(usage);

    return 0;
}