#ifndef _decl_h_
#define _decl_h_

extern token kind;
extern char identifier[];

extern bool next_token(token t);
extern void expect(u_char c);
extern node * compound(node * x, table * t);

extern u_long dtype(u_long specs);
extern u_long mods(u_long specs);
extern u_long class(u_long specs);
extern u_long quals(u_long specs);
extern type * get_type(u_long w);
extern type * btype(u_long specs);
extern type * ptype(u_int kind, type * i);
extern bool void_checking(type * i);
extern bool is_void_btype(type * i);

extern node * create_variable(bool ansi, type * i);

extern list * create_list(void);
extern void list_insert(list * x, node * u);
extern node * list_lookup(list * x);

extern table * create_table(table * next);
extern void table_insert(table * t, node * x);
extern void table_insert_list(table * t, list * x);

extern void error(const char * format, ...);

#endif