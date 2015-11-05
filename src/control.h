#ifndef LOWCULATOR_CONTROL_H_
#define LOWCULATOR_CONTROL_H_

#include "common.h"

void CStartup();

void CCleanup();

int CEof();

int CDirective(int line, int column, const char *dire);

int CEvaluate(int line, int column, const char *expr);

int CFile(const char *filename);

int CNextLine(char *buff, size_t size);

int COutput(const char *filename);

int CPanic(int line, int column, const char *val);

int CPrecision(int line, int prec);

#endif

