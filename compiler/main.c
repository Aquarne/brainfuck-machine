#include "bfc.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv)
{
    const char *ifile = NULL;
    const char *ofile = NULL;
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
                                sscanf(argv[i+1], "%d", &new_cells) == 0)
                            {
                                fprintf(stderr, "Invalid value for '%s' - missing argument\n", s);
                                return 1;
                            }

                            if (new_cells < 0)
                            {
                                fprintf(stderr, "Invalid value for '%s' - cannot be negative\n", s);
                                return 1;
                            }

                            if (new_cells == 0)
                            {
                                fprintf(stderr, "Invalid value for '%s' - cannot be zero\n", s);
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

                        case 'o':
                        {
                            if (s[2])
                            {
                                goto invalid_opt;
                            }

                            if (i+1 >= argc)
                            {
                                fprintf(stderr, "Invalid value for '%s' - missing argument\n", s);
                                return 1;
                            }

                            ofile = argv[i+1];
                            i++;

                            continue;
                        }
                    }

                invalid_opt:
                    fprintf(stderr, "Invalid option '%s'\n", s);
                    return 1;
                }
                break;

                default:
                    if (!ifile)
                        ifile = s;
                    break;
            }
        }
    }

    if (!ifile)
    {
        fprintf(stderr, "Input file not provided!\n");
        return 1;
    }

    if (!ofile)
    {
        ofile = "out.bfe";
    }

    if (!bfc_init(cells, ifile, ofile))
    {
        return 1;
    }

    if (!bfc_compile())
    {
        bfc_terminate();
        return 1;
    }

    bfc_terminate();

    return 0;
}
