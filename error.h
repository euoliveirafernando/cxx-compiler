#ifndef _error_h_
#define _error_h_

#define color_error "\x1b[1;31m"
#define color_reset "\x1b[0m"

extern FILE * input;
extern const char * filename;
extern unsigned int line_number;

#endif