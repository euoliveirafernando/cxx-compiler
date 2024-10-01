#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ctype.h"
#include "token.h"
#include "graph.h"
#include "opcode.h"
#include "table.h"
#include "var.h"
#include "val.h"
#include "ssa.h"

node * create_node(u_long op)
{
    node * x;
    
    x = cxx_alloc(sizeof (node));

    x->op = op;
    x->id = is_op(op) ? id++ : 0;
    
    return x;
}

void create_edge(node * u, node * v)
{
    edge * e;

    if (u == NULL || v == NULL)
        return;

    e = cxx_alloc(sizeof (edge));

    e->u = u;
    e->v = v;
    e->next = u->pred;
    u->pred = e;
}

node * create_block(node * x)
{
    node * u;
    
    u = create_node(block);
    if (x != NULL)
        x->next = u;
    
    return u;
}

node * create_if(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    
    u = create_node(if_then);
    v = create_node(not);
    w = expr(x, t);
    create_edge(x, u);
    create_edge(u, v);
    create_edge(v, w);
    
    return u;
}

node * create_goto(node * x)
{
    node * u;
    
    u = create_node(cgoto);
    create_edge(x, u);
    
    return u;
}

void create_return(node * x, table * t)
{
    node * u;
    node * v;
    
    u = create_node(creturn);
    v = expr(x, t);
    create_edge(x, u);
    create_edge(u, v);
}

node * parse_branch(node * x, table * t)
{
    node * u;
    
    expect('(');
    u = create_if(x, t);
    expect(')');
    
    return u;
}

node * parse_if_then(node * x, table * t)
{
    node * u;
    node * v;
    
    expect('(');
    u = create_node(if_then);
    v = expr(x, t);
    create_edge(x, u);
    create_edge(u, v);
    expect(')');
    
    return u;
}

void reuse_block(node * u, node * v)
{
}

node * create_value(void)
{
    node * u;
    value * v;
    
    u = create_node(cval);
    v = cxx_alloc(sizeof (value));
    v->number = number;
    u->data = v;
    get_next_token();
    
    return u;
}

node * create_variable(bool ansi, type * i)
{
    node * u;
    variable * v;
    
    u = create_node(cvar);
    v = cxx_alloc(sizeof (variable));
    if (ansi == false) {
        v->name = strdup(identifier);
        get_next_token();
    }
    v->type = i;
    u->data = v;
    
    return u;
}

node * un(u_long op, node * u)
{
    node * x;
    
    x = create_node(op);
    
    create_edge(x, u);
    
    return x;
}

node * bin(node * u, u_long op, node * v)
{
    node * x;
    
    x = create_node(op);
    
    create_edge(x, u);
    create_edge(x, v);
    
    return x;
}

node * ter(node * u, node * v, node * w)
{
    node * x;
    
    x = create_node(cond);
    
    create_edge(x, u);
    create_edge(x, v);
    create_edge(x, w);
    
    return x;
}

node * load(table * t)
{
    node * x;
    node * v;
    
    x = create_node(cload);
    
    v = table_lookup(t);
    
    if (v == NULL)
        error("'%s' not declared", identifier);
    
    create_edge(x, v);
    
    return x;
}

node * store(node * u, node * v)
{
    node * x;
    
    x = create_node(cstore);
    
    create_edge(x, u);
    create_edge(x, v);
    
    return x;
}

graph * create_graph(const char * name)
{
    graph * g;

    input = fopen(name, "r");

    if (input == NULL)
        error("%s: no such file", name);

    g = cxx_alloc(sizeof (graph));
    
    g->entry = create_list();
    g->entry->next = create_block(NULL);
    
    return g;
}