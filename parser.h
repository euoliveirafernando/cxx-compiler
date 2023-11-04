#ifndef _parser_h_
#define _parser_h_

extern const char * filename;
extern unsigned int line_number;
extern token kind;

extern char identifier[];

extern node * zero;
extern node * one;

extern char get_next_char(void);
extern token lexan(void);

extern node * decl(node * x, table * t);

extern node * expr(node * x, table * t);

extern node * create_node(u_long op);
extern void create_edge(node * u, node * v);
extern node * create_block(node * x);
extern node * create_if(node * x, table * t);
extern node * create_goto(node * x);
extern void create_return(node * x, table * t);
extern node * parse_branch(node * x, table * t);
extern node * parse_if_then(node * x, table * t);
extern void reuse_block(node * u, node * v);
extern node * create_number(double number);
extern node * create_variable(bool ansi, type * i);
extern graph * create_graph(const char * file);

extern table * create_table(table * next);

extern void error(const char * format, ...);

#endif