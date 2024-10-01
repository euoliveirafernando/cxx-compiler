#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ctype.h"
#include "graph.h"
#include "opcode.h"
#include "var.h"
#include "val.h"
#include "gen.h"

static void gen_op(node * x);

static void print(const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);
}

static void gen_val(value * v)
{
}

static void gen_var(variable * v)
{
}

static void gen_un(node * x)
{
    gen_op(x->pred->v);
}

static void gen_bin(node * x)
{
    gen_op(x->pred->next->v);
    gen_op(x->pred->v);
}

static void gen_ter(node * x)
{
    gen_op(x->pred->next->next->v);
    gen_op(x->pred->next->v);
    gen_op(x->pred->v);
}

static void gen_if_then(node * x)
{
    gen_op(x->pred->v);
}

static void gen_goto(node * x)
{
    gen_op(x->pred->v);
}

static void gen_return(node * x)
{
    gen_op(x->pred->v);
}

static void gen_load(node * x)
{
    if (x->op == cvar)
        gen_var(x->pred->v->data);
}

static void gen_store(node * x)
{
    gen_op(x->pred->next->v);
    gen_op(x->pred->v);
}

static void gen_op(node * x)
{
    if (x->op == if_then)
        gen_if_then(x);
    else if (x->op == cgoto)
        gen_goto(x);
    else if (x->op == creturn)
        gen_return(x);
    else if (x->op == cload)
        gen_load(x);
    else if (x->op == cstore)
        gen_store(x);
    else if (x->op == subscript) {
        gen_un(x);
    } else if (x->op == ref) {
        gen_un(x);
    } else if (x->op == neg) {
        gen_un(x);
    } else if (x->op == not) {
        gen_un(x);
    } else if (x->op == bitnot) {
        gen_un(x);
    } else if (x->op == deref) {
        gen_un(x);
    } else if (x->op == address_of) {
        gen_un(x);
    } else if (x->op == size_of) {
        gen_un(x);
    } else if (x->op == bitand) {
        gen_un(x);
    } else if (x->op == mul) {
        gen_bin(x);
    } else if (x->op == div) {
        gen_bin(x);
    } else if (x->op == mod) {
        gen_bin(x);
    } else if (x->op == add) {
        gen_bin(x);
    } else if (x->op == sub) {
        gen_bin(x);
    } else if (x->op == left_shift) {
        gen_bin(x);
    } else if (x->op == right_shift) {
        gen_bin(x);
    } else if (x->op == less) {
        gen_bin(x);
    } else if (x->op == greater) {
        gen_bin(x);
    } else if (x->op == less_equal) {
        gen_bin(x);
    } else if (x->op == greater_equal) {
        gen_bin(x);
    } else if (x->op == equal) {
        gen_bin(x);
    } else if (x->op == not_equal) {
        gen_bin(x);
    } else if (x->op == xor) {
        gen_bin(x);
    } else if (x->op == bitor) {
        gen_bin(x);
    } else if (x->op == and) {
        gen_bin(x);
    } else if (x->op == or) {
        gen_bin(x);
    } else if (x->op == cond) {
        gen_ter(x);
    } else if (x->op == cval)
        gen_val(x->data);
}

static void gen_code(edge * e)
{
    if (e != NULL) {
        gen_op(e->v);
        gen_code(e->next);
    }
}

static void gen_block(node * x)
{
    if (x != NULL) {
        gen_code(x->pred);
        gen_block(x->next);
    }
}

void gen(graph * g)
{
    gen_block(g->entry->next);
}