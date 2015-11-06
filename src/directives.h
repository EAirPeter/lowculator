#ifndef LOWCULATOR_DIRECTIVES_H_
#define LOWCULATOR_DIRECTIVES_H_

#include "common.h"
#include "result.h"

Result  DStartup();

void    DCleanup();

Result  DProcess(size_t line, size_t column, const char *dire);

#endif

