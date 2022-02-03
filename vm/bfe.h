#ifndef BFE_H
#define BFE_H

#include "defs.h"

struct bfe_file
{
    struct
    {
        u32 cells;
    } header;

    struct
    {
        u8 *data;
        u32 size;
    } bytecode;
};

bool bfe_file_read(const char *path, struct bfe_file *out);

#endif
