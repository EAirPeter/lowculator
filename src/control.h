#ifndef LOWCULATOR_CONTROL_H_
#define LOWCULATOR_CONTROL_H_

#include "common.h"

int         CEof();

int         CEvaluate(int line, int column, const char *expr);

int         CFile(const char *filename);

int         COutput(const char *filename);

int         CPanic(int line, int column, const char *val);

int         CPrecision(int prec);

#endif

