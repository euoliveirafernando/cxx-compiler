#ifndef _ssa_h_
#define _ssa_h_

extern FILE * input;
extern token kind;

extern char identifier[];
extern double number;
extern char * string;
extern char character;

u_long id;

node * zero;
node * one;

extern void get_next_token(void);
extern void expect(u_char c);

extern node * expr(node * x, table * t);

extern list * create_list(void);

extern void table_insert(table * t, node * x);
extern node * table_lookup(table * t);

extern void * cxx_alloc(size_t size);

extern void error(const char * format, ...);

#endif