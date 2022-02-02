#include "bfm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bfm g_bfm;

enum bytecode_instructions
{
    INSTR_RSHIFT = 1,   // Move to right.
    INSTR_RSHIFTN,      // Move to right n times.
    INSTR_LSHIFT,       // Move to left.
    INSTR_LSHIFTN,      // Move to left n times.
    INSTR_INCRM,        // Increment cell.
    INSTR_INCRMN,       // Increment cell n times.
    INSTR_DECRM,        // Decrement cell.
    INSTR_DECRMN,       // Decrement cell n times.
    INSTR_PRINT,        // Output cell value to console.
    INSTR_INPUT,        // Input value to cell.
    INSTR_JMP,          // Jump to an instruction.
    INSTR_JMPZ          // Jump to an instruction if cell value is zero.
};

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

bool bfm_init(const char *file, int cells)
{
    bool success = true;

    g_bfm.registers.bp = malloc(cells);
    g_bfm.registers.sp = g_bfm.registers.bp;

    memset(g_bfm.registers.bp, 0, cells);

    g_bfm.bytecode.data = read_file(file, &g_bfm.bytecode.size);
    if (!g_bfm.bytecode.data)
    {
        success = false;
        goto end;
    }
    g_bfm.registers.ip = g_bfm.bytecode.data;

end:
    return success;
}

void bfm_run()
{
    u8 *end = g_bfm.bytecode.data + g_bfm.bytecode.size;
    
    while (g_bfm.registers.ip < end)
    {
        u8 op = *(g_bfm.registers.ip++);
        switch (op)
        {
            case INSTR_RSHIFT:
                g_bfm.registers.sp++;
                break;
            case INSTR_RSHIFTN:
                g_bfm.registers.sp += *(g_bfm.registers.ip++);
                break;
            case INSTR_LSHIFT:
                g_bfm.registers.sp--;
                break;
            case INSTR_LSHIFTN:
                g_bfm.registers.sp -= *(g_bfm.registers.ip++);
                break;
            case INSTR_INCRM:
                (*g_bfm.registers.sp)++;
                break;
            case INSTR_INCRMN:
                *g_bfm.registers.sp += *(g_bfm.registers.ip++);
                break;
            case INSTR_DECRM:
                (*g_bfm.registers.sp)--;
                break;
            case INSTR_DECRMN:
                *g_bfm.registers.sp -= *(g_bfm.registers.ip++);
                break;
            case INSTR_PRINT:
                fputc(*g_bfm.registers.sp, stdout);
                break;
            case INSTR_INPUT:
                fscanf(stdin, "%c", g_bfm.registers.sp);
                break;
            case INSTR_JMP:
                {
                    u32 offset = *(u32*)(g_bfm.registers.ip);
                    g_bfm.registers.ip += 4;
                    g_bfm.registers.ip = g_bfm.bytecode.data + offset;
                }
                break;
            case INSTR_JMPZ:
                {
                    u32 offset = *(u32*)(g_bfm.registers.ip);
                    g_bfm.registers.ip += 4;;
                    if (!(*g_bfm.registers.sp))
                    {
                        g_bfm.registers.ip = g_bfm.bytecode.data + offset;
                    }
                }
                break;
        }
    }
}

void bfm_terminate()
{
    free(g_bfm.bytecode.data);
    free(g_bfm.registers.bp);
}
