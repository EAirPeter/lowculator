#ifndef LOWCULATOR_PARSE_H_
#define LOWCULATOR_PARSE_H_

#include "common.h"

void        PStartup();

void        PCleanup();

long double PEval(int line, const char *expr);

#endif

