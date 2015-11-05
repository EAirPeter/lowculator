#ifndef LOWCULATOR_ERROR_H_
#define LOWCULATOR_ERROR_H_

#include "common.h"

#include <errno.h>

#ifdef      EINVAL
#   define  E_SYNTAX        EINVAL
#else
#   define  E_SYNTAX        1
#endif

#define E_SUCCESS       0

void EStartup();

void ECleanup();

const char *EMessage();

int EContext(const char *ctxname);

int ESuccess();

int ESyntaxDirective        (int lne, int col);

int ESyntaxExpectChar       (int lne, int col, int fnd, int exp);

int ESyntaxExpectName       (int lne, int col, int fnd);

int ESyntaxExpectNumber     (int lne, int col, int fnd);

int ESyntaxUnexpectedChar   (int lne, int col, int fnd);

int ESyntaxUnexpectedTerm   (int lne, int col);

int ESyntaxUnexpectedLength (int lne, int col);

int ESyntaxUndefined        (int lne, int col, const char *tok);

int ESyntaxImproper         (int lne, int col);

int ESyntaxIllegal          (int lne);

int EMath                   (int lne);

#endif

