#include "bfc.h"
#include "lexer.h"

#include "../vm/bfm.h"

#include <stdio.h>
#include <stdlib.h>

#define LOOP_MAX_NEST   256

struct bfc
{
    struct lexer lexer;
    const char *opath;
    u32 size;
    u8 *src;

    int cells;

    u32 loop_top;
    u32 loops[LOOP_MAX_NEST];

    struct bf_token current;
    struct bf_token next;

    struct
    {
        u8 *data;
        u32 size;
        u32 capacity;
    } bytecode;
};

static struct bfc g_bfc;

static u8* read_file(const char *path, u32 *size);
static bool write_file(const char *path, u32 size, u8 *data);

static bool write_instr(u8 instr, u32 val);

static void move();

bool bfc_init(int cells, const char *ipath, const char *opath)
{
    bool success = true;

    u32 size;
    u8 *src = read_file(ipath, &size);
    if (!src)
    {
        success = false;
        goto end;
    }

    g_bfc.src = src;
    g_bfc.size = size;
    g_bfc.opath = opath;
    g_bfc.lexer.current = src;
    g_bfc.lexer.end = src + size;

    g_bfc.cells = cells;

    g_bfc.loop_top = 0;

    g_bfc.bytecode.data = NULL;
    g_bfc.bytecode.size = 0;
    g_bfc.bytecode.capacity = 0;

    move();

end:
    return success;
}

bool bfc_compile()
{
    bool success = true;

    while (move(), g_bfc.current.type != TOKEN_EOF)
    {
        switch (g_bfc.current.type)
        {
            case TOKEN_RSHIFT:
                success = write_instr(
                    g_bfc.current.count > 1 ?
                        INSTR_RSHIFTN :
                        INSTR_RSHIFT,
                    g_bfc.current.count
                );
                break;
            case TOKEN_LSHIFT:
                success = write_instr(
                    g_bfc.current.count > 1 ?
                        INSTR_LSHIFTN :
                        INSTR_LSHIFT,
                    g_bfc.current.count
                );
                break;
            case TOKEN_PLUS:
                success = write_instr(
                    g_bfc.current.count > 1 ?
                        INSTR_INCRMN :
                        INSTR_INCRM,
                    g_bfc.current.count
                );
                break;
            case TOKEN_MINUS:
                success = write_instr(
                    g_bfc.current.count > 1 ?
                        INSTR_DECRMN :
                        INSTR_DECRM,
                    g_bfc.current.count
                );
                break;
            case TOKEN_PERIOD:
                success = write_instr(
                    INSTR_PRINT,
                    1
                );
                break;
            case TOKEN_COMMA:
                success = write_instr(
                    INSTR_INPUT,
                    1
                );
                break;
            case TOKEN_LBRACKET:
                if (g_bfc.loop_top >= LOOP_MAX_NEST)
                {
                    fprintf(stderr, "Maximum number of loop nesting reached!\n");
                    success = false;
                    goto end;
                }

                g_bfc.loops[g_bfc.loop_top] =
                    g_bfc.bytecode.size;
                g_bfc.loop_top++;

                success = write_instr(
                    INSTR_JMPZ,
                    0
                );
                break;
            case TOKEN_RBRACKET:
                if (!g_bfc.loop_top)
                {
                    fprintf(stderr, "Uneven brackets!\n");
                    success = false;
                    goto end;
                }

                g_bfc.loop_top--;
                u32 offset = g_bfc.loops[g_bfc.loop_top];
                success = write_instr(
                    INSTR_JMP,
                    offset
                );

                u32 offset_end = g_bfc.bytecode.size;
                g_bfc.bytecode.data[offset+1] =
                    offset_end & 0xFF;
                g_bfc.bytecode.data[offset+2] =
                    (offset_end >> 8) & 0xFF;
                g_bfc.bytecode.data[offset+3] =
                    (offset_end >> 16) & 0xFF;
                g_bfc.bytecode.data[offset+4] =
                    (offset_end >> 24) & 0xFF;
                break;
        }
    }

    success = write_file(
        g_bfc.opath,
        g_bfc.bytecode.size,
        g_bfc.bytecode.data
    );

end:
    return success;
}

void bfc_terminate()
{
    free(g_bfc.bytecode.data);
    free(g_bfc.src);
}

static void move()
{
    g_bfc.current = g_bfc.next;
    lexer_scan(&g_bfc.lexer, &g_bfc.next);
}

static bool write_instr(u8 instr, u32 val)
{
    bool success = true;

    if (g_bfc.bytecode.size + 5 > g_bfc.bytecode.capacity)
    {
        g_bfc.bytecode.capacity =
            g_bfc.bytecode.capacity < 1024 ?
                1024 :
                2 * g_bfc.bytecode.capacity;
        g_bfc.bytecode.data =
            realloc(
                g_bfc.bytecode.data,
                g_bfc.bytecode.capacity
            );
    }

    switch (instr)
    {
        case INSTR_RSHIFT:
        case INSTR_LSHIFT:
        case INSTR_INCRM:
        case INSTR_DECRM:
        case INSTR_PRINT:
        case INSTR_INPUT:
            g_bfc.bytecode.data[g_bfc.bytecode.size] =
                instr;
            g_bfc.bytecode.size++;
            break;
        case INSTR_RSHIFTN:
        case INSTR_LSHIFTN:
        case INSTR_INCRMN:
        case INSTR_DECRMN:
            g_bfc.bytecode.data[g_bfc.bytecode.size] =
                instr;
            g_bfc.bytecode.data[g_bfc.bytecode.size+1] =
                val & 0xFF;
            g_bfc.bytecode.size += 2;
            break;
        case INSTR_JMP:
        case INSTR_JMPZ:
            g_bfc.bytecode.data[g_bfc.bytecode.size] =
                instr;
            g_bfc.bytecode.data[g_bfc.bytecode.size+1] =
                val & 0xFF;
            g_bfc.bytecode.data[g_bfc.bytecode.size+2] =
                (val >> 8) & 0xFF;
            g_bfc.bytecode.data[g_bfc.bytecode.size+3] =
                (val >> 16) & 0xFF;
            g_bfc.bytecode.data[g_bfc.bytecode.size+4] =
                (val >> 24) & 0xFF;
            g_bfc.bytecode.size += 5;
            break;
        default:
            fprintf(stderr, "Invalid instruction %hhX\n", instr);
            success = false;
            break;
    }

end:
    return success;
}

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

static bool write_file(const char *path, u32 size, u8 *data)
{
    bool success = true;

    FILE *file = fopen(path, "wb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file '%s'\n", path);
        return false;
    }

    u8 header[7] = {
        '.', 'B', 'F', 'E',
        g_bfc.cells & 0xFF,
        (g_bfc.cells >> 8) & 0xFF,
        (g_bfc.cells >> 16) & 0xFF
    };
    const size_t header_size = sizeof(header);

    size_t bytes_written = fwrite(header, 1, header_size, file);
    if (bytes_written != header_size)
    {
        fprintf(stderr, "Failed to write header to file '%s'\n", path);
        success = false;
        goto end;
    }

    bytes_written = fwrite(data, 1, size, file);
    if (bytes_written != size)
    {
        fprintf(stderr, "Failed to write to file '%s'\n", path);
        success = false;
        goto end;
    }

end:
    fclose(file);

    return success;
}
