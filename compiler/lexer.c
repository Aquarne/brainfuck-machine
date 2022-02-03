#include "lexer.h"

static bool is_bf_char(u8 c)
{
    return
        c == '>' ||
        c == '<' ||
        c == '+' ||
        c == '-' ||
        c == '.' ||
        c == ',' ||
        c == '[' ||
        c == ']';
}

bool lexer_scan(struct lexer *lexer, struct bf_token *out)
{
    bool success = true;

    while (lexer->current < lexer->end &&
           !is_bf_char(*lexer->current))
    {
        lexer->current++;
    }

    if (lexer->current >= lexer->end)
    {
        *out = (struct bf_token)
        {
            .type = TOKEN_EOF,
            .count = 0
        };
        goto end;
    }

    u8 c = *(lexer->current++);
    switch (c)
    {
    #define COMMON_CASE(c, t) \
        case c: \
            { \
                u8 count = 1; \
                while (lexer->current < lexer->end && \
                       count != 255) \
                { \
                    u8 ch = *lexer->current; \
                    if (is_bf_char(ch) && ch != c) \
                    { \
                        break; \
                    } \
                    \
                    count += ch == c; \
                    lexer->current++; \
                } \
                \
                *out = (struct bf_token) \
                { \
                    .type = t, \
                    .count = count \
                }; \
            } \
            break;

        COMMON_CASE('>', TOKEN_RSHIFT)
        COMMON_CASE('<', TOKEN_LSHIFT)
        COMMON_CASE('+', TOKEN_PLUS)
        COMMON_CASE('-', TOKEN_MINUS)
        COMMON_CASE('.', TOKEN_PERIOD)
        COMMON_CASE(',', TOKEN_COMMA)

        case '[':
            *out = (struct bf_token)
            {
                .type = TOKEN_LBRACKET,
                .count = 1
            };
            break;
        case ']':
            *out = (struct bf_token)
            {
                .type = TOKEN_RBRACKET,
                .count = 1
            };
            break;

    #undef COMMON_CASE
    }

end:
    return success;
}
