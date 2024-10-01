#ifndef _list_h_
#define _list_h_

extern token kind;

extern char identifier[];

extern void get_next_token(void);

extern void * cxx_alloc(size_t size);

extern void error(const char * format, ...);

#endif