#include <stdlib.h>
#include <stdbool.h>
#include "ctype.h"
#include "token.h"
#include "graph.h"
#include "opcode.h"
#include "table.h"
#include "var.h"
#include "expr.h"

static node * unary(node * x, table * t);
static node * assign(node * x, table * t);
node * expr(node * x, table * t);

static void expect_lvalue(u_long op)
{
    if (!(lvalue(op)))
        error("lvalue expected");
}

static type * typename(void)
{
    type * i;
    
    i = btype(declspecs(false));
    if (is_void_btype(i))
        return NULL;
    else
        i = abstract(i);
    
    return i;
}

static node * cast(node * x, table * t)
{
    node * u;
    type * i;
    
    if (declaration(kind)) {
        i = typename();
        expect(')');
        
        u = unary(x, t);
        link_type(u->sink, i);
    } else {
        u = expr(x, t);
        expect(')');
    }
    
    return u;
}

static node * primary(node * x, table * t)
{
    node * u;
    
    if (kind == token_ident)
        u = load(t);
    else if (next_token('('))
        u = cast(x, t);
    else if (primary_expr(kind))
        u = create_value();
    else
        error("expected expression");
    
    return u;
}

static node * inc(node * x, table * t)
{
    node * u;
    node * v;
    
    u = bin(x, add, one);
    v = store(x, u);

    return v;
}

static node * dec(node * x, table * t)
{
    node * u;
    node * v;
    
    u = bin(x, sub, one);
    v = store(x, u);

    return v;
}

static node * call(table * t)
{
    node * u;
    
    if (next_token(')'))
        return NULL;
    
    u = create_node(phi);
    
    expr(u, t);
    
    expect(')');
    
    return u;
}

static node * subs(node * x, table * t)
{
    node * u;
    
    u = assign(x, t);
    u = bin(x, subscript, u);
    
    expect(']');
    
    return u;
}

static node * ptr(node * x, table * t)
{
    node * u;
    node * v;
    
    u = load(t);
    v = bin(x, ref, u);
    
    return v;
}

static node * mem(node * x, table * t)
{
    node * u;
    node * v;
    
    u = load(t);
    v = bin(x, ref, u);
    
    return v;
}

static node * postfix(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = primary(x, t);
    while (post_expr(kind)) {
        expect_lvalue(u->op);
        if (next_token(token_inc))
            u = inc(u, t);
        else if (next_token(token_dec))
            u = dec(u, t);
        else if (next_token('('))
            u = call(t);
        else if (next_token('['))
            u = subs(u, t);
        else if (next_token(token_ptr))
            u = ptr(u, t);
        else if (next_token('.'))
            u = mem(u, t);
        create_edge(x, u);
    }
    
    return u;
}

static node * unary(node * x, table * t)
{
    node * u;
    u_long op;
    
    if (next_token(token_inc))
        op = add;
    else if (next_token(token_dec))
        op = sub;
    else if (next_token('+'))
        return unary(x, t);
    else if (next_token('-'))
        op = neg;
    else if (next_token('!'))
        op = not;
    else if (next_token('~'))
        op = bitnot;
    else if (next_token('*'))
        op = deref;
    else if (next_token('&'))
        op = address_of;
    else if (next_token(token_sizeof))
        op = size_of;
    else
        return postfix(x, t);
    u = unary(x, t);
    expect_lvalue(u->op);
    if (op == add)
        u = inc(u, t);
    else if (op == sub)
        u = dec(u, t);
    else
        u = un(op, u);
    create_edge(x, u);
    
    return u;
}

static node * prod(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = unary(x, t);
    while (prod_expr(kind)) {
        if (next_token('*'))
            op = mul;
        else if (next_token('/'))
            op = div;
        else if (next_token('%'))
            op = mod;
        v = unary(x, t);
        u = bin(u, op, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * sum(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = prod(x, t);
    while (sum_expr(kind)) {
        if (next_token('+'))
            op = add;
        else if (next_token('-'))
            op = sub;
        v = prod(x, t);
        u = bin(u, op, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * shift(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = sum(x, t);
    while (shift_expr(kind)) {
        if (next_token(token_left_shift))
            op = left_shift;
        else if (next_token(token_right_shift))
            op = right_shift;
        v = sum(x, t);
        u = bin(u, op, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * rel(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = shift(x, t);
    while (rel_expr(kind)) {
        if (next_token('<'))
            op = less;
        else if (next_token('>'))
            op = greater;
        else if (next_token(token_less_equal))
            op = less_equal;
        else if (next_token(token_greater_equal))
            op = greater_equal;
        v = shift(x, t);
        u = bin(u, op, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * rel_eq(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = rel(x, t);
    while (rel_eq_expr(kind)) {
        if (next_token(token_equal))
            op = equal;
        else if (next_token(token_not_equal))
            op = not_equal;
        v = rel(x, t);
        u = bin(u, op, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * bit_and(node * x, table * t)
{
    node * u;
    node * v;
    
    u = rel_eq(x, t);
    while (next_token('&')) {
        v = rel_eq(x, t);
        u = bin(u, bitand, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * bit_xor(node * x, table * t)
{
    node * u;
    node * v;
    
    u = bit_and(x, t);
    while (next_token('^')) {
        v = bit_and(x, t);
        u = bin(u, xor, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * bit_or(node * x, table * t)
{
    node * u;
    node * v;
    
    u = bit_xor(x, t);
    while (next_token('|')) {
        v = bit_xor(x, t);
        u = bin(u, bitor, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * _and(node * x, table * t)
{
    node * u;
    node * v;
    
    u = bit_or(x, t);
    while (next_token(token_and)) {
        v = bit_or(x, t);
        u = bin(u, and, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * _or(node * x, table * t)
{
    node * u;
    node * v;
    
    u = _and(x, t);
    while (next_token(token_or)) {
        v = _and(x, t);
        u = bin(u, or, v);
        create_edge(x, u);
    }
    
    return u;
}

static node * con(node * x, table * t)
{
    node * u;
    node * v;
    node * w;
    
    u = _or(x, t);
    if (next_token('?')) {
        v = assign(x, t);
        expect(':');
        w = con(x, t);
        u = ter(u, v, w);
        create_edge(x, u);
    }
    
    return u;
}

static node * assign(node * x, table * t)
{
    node * u;
    node * v;
    u_long op;
    
    u = con(x, t);
    if (assign_expr(kind)) {
        if (next_token('='))
            op = cstore;
        else if (next_token(token_add_assign))
            op = add;
        else if (next_token(token_sub_assign))
            op = sub;
        else if (next_token(token_mul_assign))
            op = mul;
        else if (next_token(token_div_assign))
            op = div;
        else if (next_token(token_mod_assign))
            op = mod;
        else if (next_token(token_left_shift_assign))
            op = left_shift;
        else if (next_token(token_right_shift_assign))
            op = right_shift;
        else if (next_token(token_bitand_assign))
            op = bitand;
        else if (next_token(token_xor_assign))
            op = xor;
        else if (next_token(token_bitor_assign))
            op = bitor;
        v = assign(x, t);
        if (op != cstore)
            v = bin(u, op, v);
        expect_lvalue(u->op);
        u = store(u, v);
        create_edge(x, u);
    }
    
    return u;
}

node * expr(node * x, table * t)
{
    node * u;
    
    do u = assign(x, t);
    while (next_token(','));
    
    return u;
}