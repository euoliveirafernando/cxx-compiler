#include <stdbool.h>
#include <string.h>
#include "ctype.h"
#include "token.h"
#include "graph.h"
#include "table.h"
#include "var.h"
#include "decl.h"

static void parse_old_kr_args(list * x);
static void parse_ansi_args(list * x);

u_long declspecs(bool use_storage)
{
    u_long specs;
    
    specs = 0x0L;
    do if (typespec(kind))
        specs = dtype(specs);
    else if (modifier(kind))
        specs = mods(specs);
    else if (storage(kind))
        if (use_storage == true)
            specs = class(specs);
        else
            error("storage class in declaration");
    else if (qualifier(kind))
        specs = quals(specs);
    while (declaration(kind));
    if (!(basic_type(specs)))
        specs |= cint;
    
    return specs;
}

static type * parse_pointer(type * i)
{
    type * j;
    
    j = ptype(pointer, i);
    while (qualifier(kind))
        j->qual = quals(j->qual);
    
    return j;
}

static type * parse_array(type * i)
{
    type * j;
    
    j = ptype(array, i);
    expect(']');
    
    return j;
}

static type * parse_function(type * i)
{
    type * j;
    
    j = ptype(function, i);
    if (next_token(')'))
        return j;
    j->sink = create_list();
    if (declaration(kind))
        parse_ansi_args(j->sink);
    else do
        parse_old_kr_args(j->sink);
    while (next_token(','));
    expect(')');
    
    return j;
}

static type * pre_declarator(type * i)
{
    while (next_token('*'))
        i = parse_pointer(i);
    return i;
}

static type * post_declarator(type * i)
{
    if (next_token('['))
        do i = parse_array(i);
        while (next_token('['));
    else if (next_token('('))
        i = parse_function(i);
    return i;
}

static bool returning(type * i, type * j, u_long w)
{
    type * k;
    
    if (i == j || i->type != w)
        return false;
    
    for (k = i; k != j; k = k->next)
        if (k->type == pointer)
            return false;
    
    return true;
}

static void type_check(type * i, type * j, type * k)
{
    if (i->type == array && i != j) {
        if (void_checking(k))
            error("array of voids");
        else if (returning(j, k, function))
            error("function returning an array");
    } else if (i->type == function && i != j) {
        if (returning(j, k, array))
            error("array of functions");
        else if (returning(j, k, function))
            error("function returning a function");
    } else if (void_checking(i))
        error("void type");
}

static void declarator(variable * v, type * prev)
{
    type * i;
    type * j;
    
    j = v->type;
    i = post_declarator(j);
    type_check(i, j, prev);
    v->type = i;
}

type * abstract(type * i)
{
    type * j;
    type * k;
    
    j = pre_declarator(i);
    if (next_token('(')) {
        k = abstract(j);
        expect(')');
    } else
        k = j;
    i = post_declarator(k);
    type_check(i, k, j);
    
    return i;
}

void link_type(variable * v, type * i)
{
    if (v != NULL)
        v->type = i;
}

static node * link_var(list * x, type * i)
{
    node * u;
    
    u = list_lookup(x);
    
    if (u == NULL)
        error("'%s' no such", identifier);
    
    link_type(u->sink, i);
    
    return u;
}

static node * old_kr(list * x, type * i)
{
    node * u;
    
    i = pre_declarator(i);
    if (next_token('(')) {
        u = old_kr(x, i);
        expect(')');
    } else if (kind == token_ident)
        u = link_var(x, i);
    else
        error("expected id or '('");
    declarator(u->sink, i);
    
    return u;
}

static node * direct(bool ansi, type * i)
{
    node * u;

    i = pre_declarator(i);
    if (next_token('(')) {
        u = direct(ansi, i);
        expect(')');
    } else if (kind == token_ident)
        u = create_variable(false, i);
    else if (ansi == true)
        u = create_variable(ansi, i);
    else
        error("expected id or '('");
    declarator(u->sink, i);

    return u;
}

static void parse_old_kr_args(list * x)
{
    node * u;
    
    if (kind != token_ident)
        error("expected id");
    
    u = create_variable(false, NULL);
    
    list_insert(x, u);
}

static type * next_arg(list * x)
{
    type * i;
    
    i = NULL;
    if (next_token(','))
        if (next_token(token_etc))
            list_insert(x, x->next);
        else if (declaration(kind))
            i = btype(declspecs(false));
        else
            error("expected typename");
    
    return i;
}

static void parse_ansi_args(list * x)
{
    node * u;
    type * i;
    
    i = btype(declspecs(false));
    if (is_void_btype(i))
        return;
    else do {
        u = direct(true, i);
        list_insert(x, u);
    } while (i = next_arg(x));
}

static void old_kr_args(list * x)
{
    type * i;
    
    i = btype(declspecs(false));
    do old_kr(x, i);
    while (next_token(','));
    expect(';');
}

static bool is_old_kr_null_type(variable * v)
{
    return v->type == NULL;
}

static bool is_old_kr(list * x)
{
    if (x != NULL)
        if (x->next != NULL)
            if (is_old_kr_null_type(x->next->sink))
                return true;
    return false;
}

static bool define(variable * v)
{
    if (v->type->type != function)
        return false;
    else if (declaration(kind)) {
        if (!is_old_kr(v->type->sink))
            error("old kr prototype");
        do old_kr_args(v->type->sink);
        while (declaration(kind));
        expect('{');
        return true;
    } else if (next_token('{'))
        return true;
    else
        return false;
}

static void old_kr_default_to_int(variable * v)
{
    if (v->type == NULL)
        v->type = get_type(c_int);
}

static void default_to_int(list * x)
{
    if (x != NULL)
        for (node * i = x->next; i != NULL; i = i->next)
            old_kr_default_to_int(i->sink);
}

static void realign(table * t, variable * v)
{
    default_to_int(v->type->sink);
    table_insert_list(t, v->type->sink);
}

static void verify_old_kr(variable * v)
{
    if (v->type->type == function)
        if (is_old_kr(v->type->sink))
            default_to_int(v->type->sink);
}

static node * next_decl(table * t, type * i)
{
    node * u;
    
    if (!next_token(',')) {
        expect(';');
        return NULL;
    }
    u = direct(false, i);
    table_insert(t, u);
    
    return u;
}

node * decl(node * x, table * t)
{
    node * u;
    type * i;
    
    if (t->next == NULL)
        i = btype(declspecs(false));
    else
        i = btype(declspecs(true));
    u = direct(false, i);
    table_insert(t, u);
    if (define(u->sink)) {
        t = create_table(t);
        realign(t, u->sink);
        x = compound(x, t);
    } else do if (next_token('='));
    else
        verify_old_kr(u->sink);
    while (u = next_decl(t, i));
}