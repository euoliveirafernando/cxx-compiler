#ifndef _graph_h_
#define _graph_h_

struct edge;

struct node
{
    u_long op;
    u_long id;
    struct edge * pred;
    void * data;
    struct node * next;
};

typedef struct node node;

struct edge
{
    node * u;
    node * v;
    struct edge * next;
};

typedef struct edge edge;

struct list
{
    node * next;
};

typedef struct list list;

struct graph
{
    list * entry;
    struct graph * next;
};

typedef struct graph graph;

#endif