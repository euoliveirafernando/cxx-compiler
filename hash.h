#ifndef _hash_h_
#define _hash_h_

extern char identifier[];

extern node * create_node(u_long op);

extern list * create_list(void);
extern void list_insert(list * x, node * u);
extern node * list_lookup(list * x);

extern void * cxx_alloc(size_t size);

extern void error(const char * format, ...);

#endif