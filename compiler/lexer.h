#ifndef LEXER_H
#define LEXER_H

#include "defs.h"

enum bf_token_type
{
    TOKEN_RSHIFT,
    TOKEN_LSHIFT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_PERIOD,
    TOKEN_COMMA,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,

    TOKEN_EOF
};

struct bf_token
{
    enum bf_token_type type;
    u8 count;
};

struct lexer
{
    u8 *current;
    u8 *end;
};

bool lexer_scan(struct lexer *lexer, struct bf_token *out);

#endif
