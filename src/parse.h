#ifndef LOWCULATOR_PARSE_H_
#define LOWCULATOR_PARSE_H_

#include "common.h"
#include "result.h"

Result      PStartup();

void        PCleanup();

Result      PEval(long double *ans, size_t line, size_t column, const char *expr);

#endif

