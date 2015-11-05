#ifndef LOWCULATOR_DIRECTIVES_H_
#define LOWCULATOR_DIRECTIVES_H_

#include "common.h"

void DStartup();

void DCleanup();

int  DProcess(int line, int column, const char *dire);

#endif

