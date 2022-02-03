#include "bfm.h"
#include "bfe.h"

#include <stdio.h>

int main(int argc, const char **argv)
{
    const char *file = NULL;
    int cells = 0;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            const char *s = argv[i];
            switch (s[0])
            {
                case '-':
                {
                    switch (s[1])
                    {
                        case 'n':
                        {
                            if (s[2])
                            {
                                goto invalid_opt;
                            }

                            int new_cells;
                            if (i+1 >= argc ||
                                sscanf(argv[i+1], "%d", &new_cells) == 0 ||
                                new_cells < 0)
                            {
                                fprintf(stderr, "Invalid value for '%s' - cannot be negative\n", s);
                                return 1;
                            }

                            if (new_cells >= 0xFFFFFFU)
                            {
                                fprintf(stderr, "Invalid value for '%s' - over the limit\n", s);
                                return 1;
                            }

                            cells = new_cells;
                            i++;

                            continue;
                        }
                    }

                invalid_opt:
                    fprintf(stderr, "Invalid option '%s'\n", s);
                    return 1;
                }

                default:
                    if (!file)
                        file = s;
                    break;
            }
        }
    }

    if (!file)
    {
        fprintf(stderr, "File not specified!\n");
        return 1;
    }

    struct bfe_file bfe_file;
    if (!bfe_file_read(file, &bfe_file))
    {
        return 1;
    }

    if (!cells)
    {
        cells = bfe_file.header.cells;
    }

    if (!bfm_init(bfe_file.bytecode.data, bfe_file.bytecode.size, cells))
    {
        return 1;
    }

    bfm_run();
    bfm_terminate();

    return 0;
}
