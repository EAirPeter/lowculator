#ifndef LOWCULATOR_MAIN_H_
#define LOWCULATOR_MAIN_H_

#include "common.h"
#include "result.h"

Result  MStartup();

void    MCleanup();

Result  Main(int argc, char *argv[]);

#endif

