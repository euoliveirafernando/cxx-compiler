#ifndef _lexer_h_
#define _lexer_h_

#define max 2048

FILE * input;
const char * filename;
static char lookahead;
unsigned int line_number;
token kind;

char identifier[max];
double number;
char * string;
char character;

struct keyword
{
    token kind;
    
    char * word;
};

typedef struct keyword keyword;

extern void * cxx_alloc(size_t size);
extern void * cxx_realloc(void * data, size_t size);

extern void error(const char * format, ...);

keyword reserved[][10] =
{
#include "keyword"
};

char get_next_char(void);
static bool next_char(char c);

#define next_char_2(a, b)   next_char(a) || next_char(b)

#define bin_base            2
#define oct_base            8
#define dec_base            10
#define hex_base            16
#define space(x)            x == ' ' || x == '\t' || x == '\n'
#define integer_suffix(x)   x == 'l' || x == 'L' || x == 'u' || x == 'U'
#define bindigit(x)         x == 0 || x == 1
#define octdigit(x)         x >= '0' && x <= '7'
#define decdigit(x)         x >= '0' && x <= '9'
#define hexupper(x)         x >= 'A' && x <= 'F'
#define hexlower(x)         x >= 'a' && x <= 'f'
#define hexdigit(x)         hexupper(x) || hexlower(x) || decdigit(x)
#define digit(x, y)         y == bin_base ? bindigit(x) :                   \
                            y == oct_base ? octdigit(x) :                   \
                            y == dec_base ? decdigit(x) :                   \
                            hexdigit(x)
#define upper(x)            x >= 'A' && x <= 'Z'
#define lower(x)            x >= 'a' && x <= 'z'
#define special(x)          x == '$' || x == '_'
#define alpha(x)            upper(x) || lower(x) || special(x)
#define alphanum(x)         alpha(x) || decdigit(x)
#define inline(x)           x != EOF && x != '\n'

#define get_num_digit       get_next_char() - '0'
#define get_hex_digit       hexupper(lookahead) ? get_next_char() - 'A' + 10 :  \
                            hexlower(lookahead) ? get_next_char() - 'a' + 10 :  \
                                                                get_num_digit
#define get_digit(x)        (x == hex_base ? get_hex_digit : get_num_digit)

#define operator_1(x, y)        next_char('=') ? x : y
#define operator_2(x, y, z)     next_char(x) ? y : operator_1(z, x)
#define operator_3(x, y, z, w)  next_char(x) ? operator_1(y, z) : operator_1(w, x)

#endif