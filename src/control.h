#ifndef LOWCULATOR_CONTROL_H_
#define LOWCULATOR_CONTROL_H_

#include "common.h"
#include "result.h"

extern bool c_panic;

Result  CStartup();

void    CCleanup();

Result  CEof();

Result  CDirective(size_t line, size_t column, const char *dire);

Result  CEvaluate(size_t line, size_t column, const char *expr);

_Noreturn
Result  CExit(size_t line, size_t column, const char *expr);

Result  CFile(const char *filename);

Result  CNextLine(size_t *line, char *buff, size_t size);

Result  COutput(const char *filename);

Result  CPanic(size_t line, size_t column, const char *val);

Result  CPrecision(size_t line, size_t column, const char *expr);

Result  CPrintResultMsg(Result code);

_Noreturn
Result  CTerminate(int exitcode);

#endif

