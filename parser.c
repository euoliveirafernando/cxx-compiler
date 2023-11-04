#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ctype.h"
#include "token.h"
#include "graph.h"
#include "opcode.h"
#include "table.h"
#include "var.h"
#include "parser.h"

static node * parse(node * x, table * t);

void get_next_token(void)
{
    kind = lexan();
}

bool next_token(token t)
{
    if (kind == t) {
        get_next_token();
        return true;
    } else if (kind == eof)
        error("unexpected end of file");
    else
        return false;
}

void expect(u_char c)
{
    if (!next_token(c))
        error("expected '%c'", c);
}

node * compound(node * x, table * t)
{
    while (!next_token('}'))
        x = parse(x, t);
    
    return x;
}

static node * parse_asm(node * x, table * t)
{
    expect('(');
    if (kind == token_string_literal)
        error("expected asm string");
    if (next_token(':'))
        while (next_token(','));
    expect(')');
    expect(';');
    return x;
}

static node * parse_expr(node * x, table * t)
{
    expr(x, t);
    expect(';');
    return x;
}

static node * parse_if(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    node * i;
    node * j;
    node * k;
    
    j = parse_branch(x, t);
    u = create_block(x);
    v = parse(u, t);
    if (next_token(token_else)) {
        k = create_goto(v);
        w = create_block(v);
        i = parse(w, t);
        i = create_block(i);
        create_edge(j, w);
        create_edge(k, i);
        return i;
    }
    v = create_block(v);
    create_edge(j, v);
    
    return v;
}

static node * parse_while(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    node * i;
    node * j;
    
    u = create_block(x);
    i = parse_branch(u, t);
    v = create_block(u);
    w = create_block(v);
    v = parse(v, t);
    j = create_goto(v);
    create_edge(i, w);
    create_edge(j, u);
    
    return v;
}

static node * parse_do(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    node * i;
    node * j;
    
    u = create_block(x);
    v = create_block(u);
    w = create_block(v);
    i = parse(u, t);
    if (!next_token(token_while))
        error("expected while");
    j = parse_if_then(v, t);
    expect(';');
    reuse_block(i, v);
    create_edge(j, u);
    
    return w;
}

static node * parse_for(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    node * i;
    node * j;
    node * k;
    
    expect('(');
    u = create_block(x);
    v = create_block(u);
    w = create_block(v);
    i = create_block(w);
    if (declaration(kind))
        decl(x, t);
    else if (!next_token(';'))
        parse_expr(x, t);
    if (!next_token(';')) {
        j = create_if(u, t);
        expect(';');
    } else
        j = NULL;
    if (!next_token(')')) {
        expr(w, t);
        expect(')');
    }
    v = parse(v, t);
    k = create_goto(w);
    reuse_block(v, w);
    create_edge(j, i);
    create_edge(k, u);
    
    return i;
}

static node * parse_goto(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    
    u = create_block(x);
    v = create_goto(x);
    if (kind == token_ident)
        w = create_variable(false, NULL);
    else if (next_token('*'))
        w = expr(x, t);
    else
        error("expected id or '*'");
    create_edge(v, w);
    expect(';');
    
    return u;
}

static node * parse_return(node * x, table * t)
{
    node * u;
    
    u = create_block(x);
    create_return(x, t);
    expect(';');
    
    return u;
}

static node * statement(node * x, table * t)
{
    if (next_token(token_if))
        return parse_if(x, t);
    else if (next_token(token_while))
        return parse_while(x, t);
    else if (next_token(token_do))
        return parse_do(x, t);
    else if (next_token(token_for))
        return parse_for(x, t);
    else if (next_token(token_goto))
        return parse_goto(x, t);
    else if (next_token(token_return))
        return parse_return(x, t);
    else if (next_token('{')) {
        t = create_table(t);
        return compound(x, t);
    } else
        return parse_expr(x, t);
}

static node * parse(node * x, table * t)
{
    if (next_token(token_asm))
        return parse_asm(x, t);
    else if (declaration(kind))
        return decl(x, t);
    else if (next_token(';'))
        return x;
    else if (t->next != NULL)
        return statement(x, t);
    else
        error("no storage class");
}

graph * parse_file(char * name)
{
    graph * g;
    table * t;

    g = create_graph(name);
    t = create_table(NULL);

    filename = name;
    line_number = 1;
    
    get_next_char();
    get_next_token();
    
    while (!next_token(eof))
        g->entry->next = parse(g->entry->next, t);
    
    return g;
}