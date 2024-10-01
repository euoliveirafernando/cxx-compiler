#include <stdlib.h>
#include <stdbool.h>
#include "ctype.h"
#include "token.h"
#include "graph.h"
#include "table.h"
#include "var.h"
#include "type.h"

static void cannot_be_used(u_long u, char * a, u_long v, char * b)
{
    if (u & v)
        error("'%s' cannot be used with '%s'", a, b);
}

static void cannot_be_used_1(u_long u, char * a)
{
    cannot_be_used(u, a, cvoid, "void");
    cannot_be_used(u, a, cfloat, "float");
    cannot_be_used(u, a, cdouble, "double");
}

static void cannot_be_used_2(u_long u, char * a)
{
    cannot_be_used(u, a, cchar, "char");
    cannot_be_used(u, a, cint, "int");
}

static void cannot_be_used_3(u_long u, char * a)
{
    cannot_be_used(u, a, cshort, "short");
    cannot_be_used(u, a, clong, "long");
    cannot_be_used(u, a, clonglong, "long long");
}

static void cannot_be_used_4(u_long u, char * a)
{
    cannot_be_used(u, a, csigned, "signed");
    cannot_be_used(u, a, cunsigned, "unsigned");
}

static void cannot_be_used_5(u_long u, char * a)
{
    cannot_be_used(u, a, cstatic, "static");
    cannot_be_used(u, a, cextern, "extern");
    cannot_be_used(u, a, cauto, "auto");
    cannot_be_used(u, a, cregister, "register");
}

u_long dtype(u_long specs)
{
    if (next_token(token_void)) {
        cannot_be_used_1(specs, "void");
        cannot_be_used_2(specs, "void");
        cannot_be_used_3(specs, "void");
        cannot_be_used_4(specs, "void");
        return specs | cvoid;
    } else if (next_token(token_char)) {
        cannot_be_used_1(specs, "char");
        cannot_be_used_2(specs, "char");
        cannot_be_used_3(specs, "char");
        return specs | cchar;
    } else if (next_token(token_int)) {
        cannot_be_used_1(specs, "int");
        cannot_be_used_2(specs, "int");
        return specs | cint;
    } else if (next_token(token_float)) {
        cannot_be_used_1(specs, "float");
        cannot_be_used_2(specs, "float");
        cannot_be_used_3(specs, "float");
        cannot_be_used_4(specs, "float");
        return specs | cfloat;
    } else if (next_token(token_double)) {
        cannot_be_used_1(specs, "double");
        cannot_be_used_2(specs, "double");
        cannot_be_used(specs, "double", cshort, "short");
        cannot_be_used(specs, "double", clonglong, "long long");
        cannot_be_used_4(specs, "double");
        return specs | cdouble;
    }
}

u_long mods(u_long specs)
{
    if (next_token(token_short)) {
        cannot_be_used_1(specs, "short");
        cannot_be_used(specs, "short", cchar, "char");
        cannot_be_used_3(specs, "short");
        return specs | cshort;
    } else if (next_token(token_long)) {
        cannot_be_used(specs, "long", cvoid, "void");
        cannot_be_used(specs, "long", cchar, "char");
        cannot_be_used(specs, "long", cshort, "short");
        cannot_be_used(specs, "long", cfloat, "float");
        cannot_be_used(specs, "long", clonglong, "long long");
        return specs & clong ? specs & ~clong | clonglong :
                                            specs | clong;
    } else if (next_token(token_signed)) {
        cannot_be_used_1(specs, "signed");
        cannot_be_used_4(specs, "signed");
        return specs | csigned;
    } else if (next_token(token_unsigned)) {
        cannot_be_used_1(specs, "unsigned");
        cannot_be_used_1(specs, "unsigned");
        return specs | cunsigned;
    }
}

u_long class(u_long specs)
{
    if (next_token(token_static)) {
        cannot_be_used_5(specs, "static");
        return specs | cstatic;
    } else if (next_token(token_extern)) {
        cannot_be_used_5(specs, "extern");
        return specs | cextern;
    } else if (next_token(token_auto)) {
        cannot_be_used_5(specs, "auto");
        return specs | cauto;
    } else if (next_token(token_register)) {
        cannot_be_used_5(specs, "register");
        return specs | cregister;
    }
}

u_long quals(u_long specs)
{
    if (next_token(token_const))
        return specs | cconst;
    else if (next_token(token_volatile))
        return specs | cvolatile;
}

type * get_type(u_long w)
{
    return &b_type[w];
}

static type * get_btype(u_long specs)
{
    if (specs & cvoid)
        return get_type(c_void);
    else if (specs & cchar)
        if (specs & cunsigned)
            return get_type(c_unsigned_char);
        else
            return get_type(c_char);
    else if (specs & cint)
        if (specs & cunsigned)
            if (specs & clonglong)
                return get_type(c_unsigned_long_long_int);
            else if (specs & clong)
                return get_type(c_unsigned_long_int);
            else if (specs & cshort)
                return get_type(c_unsigned_short_int);
            else
                return get_type(c_unsigned_int);
        else if (specs & clonglong)
            return get_type(c_long_long_int);
        else if (specs & clong)
            return get_type(c_long_int);
        else if (specs & cshort)
            return get_type(c_short_int);
        else
            return get_type(c_int);
    else if (specs & cdouble)
        if (specs & clong)
            return get_type(c_long_double);
        else
            return get_type(c_double);
    else if (specs & cfloat)
        return get_type(c_float);
}

static type * ctype(type * i, u_long a)
{
    type * j;
    
    j = cxx_alloc(sizeof (type));
    j->type = a;
    j->next = i;
    
    return j;
}

type * btype(u_long specs)
{
    type * i;

    i = get_btype(specs);
    if (specs & cstatic)
        i = ctype(i, c_static);
    else if (specs & cextern)
        i = ctype(i, c_extern);
    else if (specs & cregister)
        i = ctype(i, c_register);
    else if (specs & cvolatile)
        i = ctype(i, c_volatile);
    else if (specs & cconst)
        i = ctype(i, c_const);

    return i;
}

type * ptype(u_long w, type * i)
{
    type * j;
    
    j = cxx_alloc(sizeof (type));
    j->type = w;
    j->next = i;
    
    return j;
}

bool void_checking(type * i)
{
    if (i != NULL)
        if (i->type == c_void)
            return true;
    return false;
}

bool is_void_btype(type * i)
{
    if (void_checking(i))
        if (kind == ')')
            return true;
    return false;
}

static int conv_rank(type * i)
{
    switch (i->type) {
        case c_char:
        case c_unsigned_char:
            return 1;
        case c_short_int:
        case c_unsigned_short_int:
            return 2;
        case c_int:
        case c_unsigned_int:
            return 3;
        case c_long_int:
        case c_unsigned_long_int:
            return 4;
        case c_long_long_int:
        case c_unsigned_long_long_int:
            return 5;
        case c_float:
            return 6;
        case c_double:
            return 7;
        case c_long_double:
            return 8;
    }
}

static bool is_fp(type * i)
{
    switch (i->type) {
        case c_float:
        case c_double:
        case c_long_double:
            return true;
        default:
            return false;
    }
}

static bool is_signed(type * i)
{
    switch (i->type) {
        case c_char:
        case c_short_int:
        case c_int:
        case c_long_int:
        case c_long_long_int:
            return true;
        default:
            return false;
    }
}

static bool is_unsigned(type * i)
{
    switch (i->type) {
        case c_unsigned_char:
        case c_unsigned_short_int:
        case c_unsigned_int:
        case c_unsigned_long_int:
        case c_unsigned_long_long_int:
            return true;
        default:
            return false;
    }
}

static bool is_narrow(type * i, type * j)
{
    if (conv_rank(i) < conv_rank(j))
        return true;
    else
        return false;
}

type * promote(type * i, type * j)
{
    if (is_narrow(i, j))
        return j;
    else
        return i;
}

type * convert(type * i, type * j)
{
    if (is_fp(i) || is_fp(j))
        return promote(i, j);
    else {
        i = promote(i, &b_type[c_int]);
        j = promote(j, &b_type[c_int]);

        if (is_signed(i) && is_unsigned(j))
            return promote(j, i);
        else
            return promote(i, j);
    }
}