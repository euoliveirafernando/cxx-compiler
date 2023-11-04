#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ctype.h"
#include "token.h"
#include "graph.h"
#include "table.h"
#include "var.h"
#include "list.h"

list * create_list(void)
{
    list * x;

    x = cxx_alloc(sizeof (list));

    return x;
}

static void redeclared(variable * u, variable * v)
{
    if (strcmp(u->name, v->name) == 0)
        error("'%s' redeclared", u->name);
}

static bool declared(variable * v)
{
    if (strcmp(v->name, identifier) == 0) {
        get_next_token();
        return true;
    } else
        return false;
}

static void insert(node * u, node * v)
{
    redeclared(u->sink, v->sink);
    if (u->next == NULL)
        u->next = v;
    else
        insert(u->next, v);
}

void list_insert(list * x, node * u)
{
    if (x->next == NULL)
        x->next = u;
    else
        insert(x->next, u);
}

static node * lookup(node * u)
{
    if (u == NULL)
        return NULL;
    else {
        if (declared(u->sink))
            return u;
        return lookup(u->next);
    }
}

node * list_lookup(list * x)
{
    if (x->next == NULL)
        return NULL;
    else
        return lookup(x->next);
}