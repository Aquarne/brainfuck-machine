#include "bfe.h"

#include <stdio.h>
#include <stdlib.h>

static u8* read_file(const char *path, u32 *size);

bool bfe_file_read(const char *path, struct bfe_file *out)
{
    bool success = true;

    u32 cells = 0;
    u32 size = 0;

    u8 *data = read_file(path, &size);
    if (!data )
    {
        success = false;
        goto end;
    }

    if (size < 7       ||
        data[0] != '.' ||
        data[1] != 'B' ||
        data[2] != 'F' ||
        data[3] != 'E')
    {
        fprintf(stderr, "'%s' is not a Brainfuck executable!\n", path);
        free(data);
        success = false;
        goto end;
    }
    data += 4;

    cells =
        (data[0])        |
        (data[1] << 8)   |
        (data[2] << 16);
    data += 3;

    *out = (struct bfe_file)
    {
        .header = {
            .cells = cells
        },
        .bytecode = {
            .data = data,
            .size = size - 4 - 3
        }
    };

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
