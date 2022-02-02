#include "bfm.h"

#include <stdio.h>

int main(int argc, const char **argv)
{
    const char *file = NULL;
    int cells = 30000;

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
                                new_cells <= 0)
                            {
                                fprintf(stderr, "Invalid value for '%s'\n", s);
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

    if (!bfm_init(file, cells))
    {
        return 1;
    }

    bfm_run();
    bfm_terminate();

    return 0;
}
