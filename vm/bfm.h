#ifndef BFM_H
#define BFM_H

#include "defs.h"

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

struct bfm
{
    struct
    {
        u8 *ip;
        u8 *bp;
        u8 *sp;
    } registers;
    
    struct
    {
        u8 *data;
        u32 size;
    } bytecode;
};

extern struct bfm g_bfm;

bool bfm_init(u8 *bytecode_data, u32 bytecode_size, int cells);
void bfm_run();
void bfm_terminate();

#endif
