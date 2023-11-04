#ifndef _expr_h_
#define _expr_h_

#define lvalue(x)   x == cload || x == deref || x == subscript || x == ref

extern token kind;

extern node * one;

extern bool next_token(token t);
extern void expect(u_char c);

extern type * btype(u_long specs);
extern bool is_void_btype(type * i);

extern u_long declspecs(bool use_storage);
extern type * abstract(type * i);
extern void link_type(variable * v, type * i);

extern node * create_node(u_long op);
extern void create_edge(node * u, node * v);
extern node * create_value(void);
extern node * un(u_long op, node * u);
extern node * bin(node * u, u_long op, node * v);
extern node * ter(node * u, node * v, node * w);
extern node * load(table * t);
extern node * store(node * u, node * v);

extern void error(const char * format, ...);

#endif