#include "bfm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bfm g_bfm;

bool bfm_init(u8 *bytecode_data, u32 bytecode_size, int cells)
{
    bool success = true;

    g_bfm.bytecode.data = bytecode_data;
    g_bfm.bytecode.size = bytecode_size;
    g_bfm.registers.ip = g_bfm.bytecode.data;

    g_bfm.registers.bp = malloc(cells);
    if (!g_bfm.registers.bp)
    {
        fprintf(stderr, "Not enough memory!\n");
        success = false;
        goto end;
    }
    g_bfm.registers.sp = g_bfm.registers.bp;

    memset(g_bfm.registers.bp, 0, cells);

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
                    u32 offset =
                        (g_bfm.registers.ip[0])         |
                        (g_bfm.registers.ip[1] << 8)    |
                        (g_bfm.registers.ip[2] << 16)   |
                        (g_bfm.registers.ip[3] << 24);
                    g_bfm.registers.ip += 4;

                    g_bfm.registers.ip = g_bfm.bytecode.data + offset;
                }
                break;
            case INSTR_JMPZ:
                {
                    u32 offset =
                        (g_bfm.registers.ip[0])         |
                        (g_bfm.registers.ip[1] << 8)    |
                        (g_bfm.registers.ip[2] << 16)   |
                        (g_bfm.registers.ip[3] << 24);
                    g_bfm.registers.ip += 4;

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
    free(g_bfm.bytecode.data - 7);
    free(g_bfm.registers.bp);
}
