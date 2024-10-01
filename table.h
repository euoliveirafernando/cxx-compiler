#ifndef _table_h_
#define _table_h_

struct table
{
    list ** hash;
    struct table * next;
};

typedef struct table table;

#endif