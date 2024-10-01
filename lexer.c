#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ctype.h"
#include "token.h"
#include "lexer.h"

char get_next_char(void)
{
    char c = lookahead;

    if (lookahead != EOF) {
        lookahead = fgetc(input);

        if (c == '\n')
            line_number++;
    }

    return c;
}

static bool next_char(char c)
{
    if (lookahead == c) {
        get_next_char();
        return true;
    } else
        return false;
}

static void skip_spaces(void)
{
    do get_next_char();
    while (space(lookahead));
}

static void skip_line_comment(void)
{
    while (inline(lookahead))
        get_next_char();
}

static void skip_block_comment(void)
{
    char c;

    do if (lookahead == EOF)
        error("unterminated comment");
    else
        c = get_next_char();
    while (!(c == '*' && next_char('/')));
}

static token get_word(int i, size_t size)
{
    for (int j = 0; j < size; j++)
        if (strcmp(identifier, reserved[i][j].word) == 0)
            return reserved[i][j].kind;
    
    return token_ident;
}

static token get_keyword(size_t size)
{
    switch (size) {
        case 2:
            return get_word(0, 2);
        case 3:
            return get_word(1, 3);
        case 4:
            return get_word(2, 7);
        case 5:
            return get_word(3, 5);
        case 6:
            return get_word(4, 5);
        case 7:
            return get_word(5, 1);
        case 8:
            return get_word(6, 4);
        default:
            return token_ident;
    }
}

static token get_identifier(void)
{
    u_int i = 0;
    
    bzero(identifier, max);

    do if (i < max)
        identifier[i++] = get_next_char();
    else
        error("identifier too long");
    while (alphanum(lookahead));

    return get_keyword(i);
}

static u_long get_base(void)
{
    if (next_char('0'))
        if (next_char_2('b', 'B'))
            return bin_base;
        else if (octdigit(lookahead))
            return oct_base;
        else if (next_char_2('x', 'X'))
            return hex_base;
    return dec_base;
}

static int get_number(u_int base)
{
    int i = 0;
    
    if (base == bin_base && !(bindigit(lookahead)))
        error("'%c' is not bin digit", lookahead);
    else if (base == hex_base && !(hexdigit(lookahead)))
        error("'%c' is not hex digit", lookahead);
    
    while (digit(lookahead, base))
        i = i * base + get_digit(base);
    
    return i;
}

static double get_mantissa(u_int base)
{
    double i;
    double j = 0;

    for (i = base; digit(lookahead, base); i *= base)
        j += get_digit(base) / i;

    return j;
}

static double get_exponent(u_int base)
{
    int i;
    int j = 1;
    bool negate = false;

    if (next_char('+'));
    else if (next_char('-'))
        negate = true;

    if (!decdigit(lookahead))
        error("expected exponent digit");

    i = get_number(dec_base);

    if (base == bin_base)
        j <<= i;
    else for (int k = 0; k < i; k++)
        j *= 10;
    
    if (negate)
        return (double) 1 / j;
    else
        return j;
}

static token get_integer_suffix(bool floating)
{
    token suffix;
    
    suffix = token_integer_literal;
    while (integer_suffix(lookahead))
        if (next_char_2('l', 'L'))
            if (suffix & long_long_suffix)
                error("more than two long suffixes");
            else if (suffix & long_suffix)
                if (floating)
                    error("invalid long long suffix");
                else
                    suffix |= long_long_suffix;
            else
                suffix |= long_suffix;
        else if (next_char_2('u', 'U'))
            if (floating)
                error("invalid unsigned suffix");
            else if (suffix & unsigned_suffix)
                error("two or more unsigned suffixes");
            else
                suffix |= unsigned_suffix;
    
    return suffix;
}

static token get_suffix(bool floating)
{
    token suffix;
    
    if (next_char_2('f', 'F'))
        if (floating)
            suffix = token_single_floating_point;
        else
            error("invalid float suffix on integer");
    else if (next_char_2('d', 'D'))
        if (floating)
            suffix = token_double_floating_point;
        else
            error("invalid double suffix on integer");
    else if (floating)
        suffix = token_double_floating_point;
    else
        suffix = get_integer_suffix(floating);
    
    return suffix;
}

static token get_constant(bool floating)
{
    u_int base = dec_base;

    if (floating)
        goto mantissa;

    base = get_base();

    number = get_number(base);

    if (base == bin_base)
        goto done;

    if (next_char('.')) {
        mantissa:

        number += get_mantissa(base);

        floating = true;
    }
    
    if (base == hex_base) {
        if (next_char_2('p', 'P'))
            number *= get_exponent(bin_base);
        else if (floating)
            error("exponent required");
    } else if (next_char_2('e', 'E'))
        number *= get_exponent(dec_base);
    
    done:
        return get_suffix(floating);
}

static char get_hex_char(void)
{
    char c;
    
    if (!(hexdigit(lookahead)))
        error("'%c' is not hex digit", lookahead);

    c = get_hex_digit;

    if (hexdigit(lookahead))
        c = c * hex_base + (get_hex_digit);

    return c;
}

static char get_oct_char(void)
{
    char c;
    
    c = get_num_digit;
    
    if (octdigit(lookahead))
        c = c * oct_base + get_num_digit;
    if (octdigit(lookahead))
        c = c * oct_base + get_num_digit;
    
    return c;
}

static char get_character(void)
{
    if (next_char('\\'))
        if (next_char('a')) return '\a';
        else if (next_char('b')) return '\b';
        else if (next_char('f')) return '\f';
        else if (next_char('n')) return '\n';
        else if (next_char('r')) return '\r';
        else if (next_char('v')) return '\v';
        else if (next_char('\\')) return '\\';
        else if (next_char('\'')) return '\'';
        else if (next_char('\"')) return '\"';
        else if (next_char('x')) return get_hex_char();
        else if (octdigit(lookahead)) return get_oct_char();
        else if (next_char('e')) return '\e';
        else
            error("unknown scape sequence '\\%c'", lookahead);
    else
        return get_next_char();
}

static token get_character_constant(void)
{
    if (!next_char('\'')) {
        if (inline(lookahead))
            character = get_character();
        if (!(inline(lookahead)))
            error("unterminated character constant");
        if (!(inline(lookahead)))
            error("multi-character character constant");
    } else
        error("empty character constant");
    
    return token_character_constant;
}

static token get_string_literal(void)
{
    unsigned int i = 0;
    
    string = cxx_alloc(sizeof (char));
    
    for (i = 0; !next_char('\"'); i++)
        if (inline(lookahead)) {
            string = cxx_realloc(string, i + 2);
            string[i] = get_character();
        } else
            error("unterminated string");
    
    string[i] = '\0';
    
    return token_string_literal;
}

token lexan(void)
{
    loop:
    if (space(lookahead)) {
        skip_spaces();
        goto loop;
    } else if (alpha(lookahead))
        return get_identifier();
    else if (decdigit(lookahead))
        return get_constant(false);
    else if (next_char('\''))
        return get_character_constant();
    else if (next_char('\"'))
        return get_string_literal();
    else if (next_char('('))
        return '(';
    else if (next_char(')'))
        return ')';
    else if (next_char('['))
        return '[';
    else if (next_char(']'))
        return ']';
    else if (next_char('{'))
        return '{';
    else if (next_char('}'))
        return '}';
    else if (next_char('.'))
        if (decdigit(lookahead))
            return get_constant(true);
        else if (next_char('.'))
            if (next_char('.'))
                return token_etc;
            else
                error("expected '...'");
        else
            return '.';
    else if (next_char('+'))
        return operator_2('+', token_inc, token_add_assign);
    else if (next_char('-'))
        if (next_char('>'))
            return token_ptr;
        else
            return operator_2('-', token_dec, token_sub_assign);
    else if (next_char('!'))
        return operator_1(token_not_equal, '!');
    else if (next_char('~'))
        return '~';
    else if (next_char('*'))
        return operator_1(token_mul_assign, '*');
    else if (next_char('&'))
        return operator_2('&', token_and, token_bitand_assign);
    else if (next_char('/'))
        if (next_char('/')) {
            skip_line_comment();
            goto loop;
        } else if (next_char('*')) {
            skip_block_comment();
            goto loop;
        } else
            return operator_1(token_div_assign, '/');
    else if (next_char('%'))
        return operator_1(token_mod_assign, '%');
    else if (next_char('<'))
        return operator_3('<', token_left_shift_assign, token_left_shift, token_less_equal);
    else if (next_char('>'))
        return operator_3('>', token_right_shift_assign, token_right_shift, token_greater_equal);
    else if (next_char('^'))
        return operator_1(token_xor_assign, '^');
    else if (next_char('|'))
        return operator_2('|', token_or, token_bitor_assign);
    else if (next_char('?'))
        return '?';
    else if (next_char(':'))
        return ':';
    else if (next_char('='))
        return operator_1(token_equal, '=');
    else if (next_char(','))
        return ',';
    else if (next_char(';'))
        return ';';
    else if (next_char(EOF))
        return eof;
    else
        error("unrecognized character '\\x%x'", lookahead);
}