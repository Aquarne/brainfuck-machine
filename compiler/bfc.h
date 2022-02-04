#ifndef BFC_H
#define BFC_H

#include "defs.h"

bool bfc_init(int cells, const char *ipath, const char *opath);
bool bfc_compile();
void bfc_terminate();

#endif
