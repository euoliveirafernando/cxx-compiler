#ifndef _cxx_h_
#define _cxx_h_

extern FILE * output;

extern graph * parse_file(char * name);

extern void gen(graph * g);

extern void error(const char * format, ...);

#endif