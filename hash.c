#include <string.h>
#include "ctype.h"
#include "graph.h"
#include "opcode.h"
#include "table.h"
#include "var.h"
#include "hash.h"

#define max 2048

table * create_table(table * next)
{
    table * t;

    t = cxx_alloc(sizeof (table));

    t->hash = cxx_alloc(sizeof (list * [max]));

    for (int i = 0; i < max; i++)
        t->hash[i] = create_list();

    t->next = next;

    return t;
}

static u_long hash(char * name)
{
    size_t length;
    u_long value = 0;

    length = strlen(name);

    for (int i = 0; i < length; i++) {
        value <<= 8;
        value += name[i];
    }

    return value % max;
}

static char * get_name(variable * v)
{
    return v->name;
}

void table_insert(table * t, node * x)
{
    u_long value;
    
    value = hash(get_name(x->sink));
    list_insert(t->hash[value], x);
}

void table_insert_list(table * t, list * x)
{
    if (x != NULL)
        for (node * i = x->next; i != NULL; i = i->next) {
            if (!get_name(i->sink))
                error("param name");
            table_insert(t, i);
            if (i->next == x->next)
                break;
        }
}

static node * lookup(table * t, u_long value)
{
    node * x;
    
    if (t == NULL)
        error("'%s' not declared", identifier);
    else {
        x = list_lookup(t->hash[value]);
        
        if (x == NULL)
            return lookup(t->next, value);
        else
            return x;
    }
}

node * table_lookup(table * t)
{
    u_long value;
    node * x;
    
    value = hash(identifier);
    x = lookup(t, value);
    
    return x;
}