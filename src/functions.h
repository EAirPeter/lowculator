#ifndef LOWCULATOR_FUNCTIONS_H_
#define LOWCULATOR_FUNCTIONS_H_

#include "common.h"

typedef long double (MathFunction)(long double *argv, size_t argc);

void            FStartup();

void            FCleanup();

MathFunction   *FGetFunction(const char *name);

bool            FAddFunction(const char *name, MathFunction *func);

#endif

