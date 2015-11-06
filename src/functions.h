#ifndef LOWCULATOR_FUNCTIONS_H_
#define LOWCULATOR_FUNCTIONS_H_

#include "common.h"
#include "result.h"

typedef Result
    (MathFunction)(long double *ans, long double *argv, size_t argc);

Result          FStartup();

void            FCleanup();

MathFunction   *FGetFunction(const char *name);

Result          FAddFunction(const char *name, MathFunction *func);

#endif

