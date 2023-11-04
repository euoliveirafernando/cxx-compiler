#ifndef _token_h_
#define _token_h_

enum token {
#include "token"
};

typedef enum token token;

#define typespec(x)     x == token_void || x == token_char || x == token_int ||                 \
                        x == token_float || x == token_double
#define modifier(x)     x == token_short || x == token_long || x == token_signed ||             \
                        x == token_unsigned
#define storage(x)      x == token_static || x == token_extern || x == token_auto ||            \
                        x == token_register
#define qualifier(x)    x == token_volatile || x == token_const
#define declaration(x)  typespec(x) || modifier(x) || storage(x) || qualifier(x)
#define primary_expr(x) x == token_integer_literal || x == token_single_floating_point ||       \
                        x == token_double_floating_point || x == token_character_constant ||    \
                        x == token_string_literal
#define post_expr(x)    x == token_inc || x == token_dec || x == '(' || x == '[' ||             \
                        x == token_ptr || x == '.'
#define prod_expr(x)    x == '*' || x == '/' || x == '%'
#define sum_expr(x)     x == '+' || x == '-'
#define shift_expr(x)   x == token_left_shift || x == token_right_shift
#define rel_expr(x)     x == '<' || x == '>' || x == token_less_equal ||                        \
                        x == token_greater_equal
#define rel_eq_expr(x)  x == token_equal || x == token_not_equal
#define assign_expr(x)  x == '=' || x == token_add_assign || x == token_sub_assign ||           \
                        x == token_mul_assign || x == token_div_assign ||                       \
                        x == token_mod_assign || x == token_left_shift_assign ||                \
                        x == token_right_shift_assign || x == token_bitand_assign ||            \
                        x == token_xor_assign || x == token_bitor_assign

#endif