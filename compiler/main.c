#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

static u8* read_file(const char *path, u32 *size)
{
    u8 *buffer = NULL;

    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file '%s'\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    rewind(file);

    buffer = malloc(file_size);
    if (!buffer)
    {
        fprintf(stderr, "Not enough memory!\n");
        goto end;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size)
    {
        fprintf(stderr, "Failed to read file '%s' properly!\n", path);
        free(buffer);
        buffer = NULL;
        goto end;
    }

    *size = (u32)file_size;

end:
    fclose(file);

    return buffer;
}

static char token_to_char(enum bf_token_type type)
{
    switch (type)
    {
        case TOKEN_RSHIFT: return '>';
        case TOKEN_LSHIFT: return '<';
        case TOKEN_PLUS: return '+';
        case TOKEN_MINUS: return '-';
        case TOKEN_PERIOD: return '.';
        case TOKEN_COMMA: return ',';
        case TOKEN_LBRACKET: return '[';
        case TOKEN_RBRACKET: return ']';
    }

    return 'X';
}

int main(int argc, const char **argv)
{
    u32 size;
    u8 *src = read_file(argv[1], &size);
    if (!src)
    {
        return 1;
    }

    struct lexer lexer = (struct lexer)
    {
        .current = src,
        .end = src + size
    };

    struct bf_token token;
    while (lexer_scan(&lexer, &token))
    {
        if (token.type == TOKEN_EOF)
        {
            break;
        }

        printf("%3hhu | %c\n", token.count, token_to_char(token.type));
    }

    free(src);

    return 0;
}
