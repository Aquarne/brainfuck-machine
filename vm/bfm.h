#ifndef BFM_H
#define BFM_H

#include "defs.h"

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

bool bfm_init(const char *file, int cells);
void bfm_run();
void bfm_terminate();

#endif
