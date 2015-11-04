#ifndef LOWCULATOR_ERROR_H_
#define LOWCULATOR_ERROR_H_

#include "common.h"

#define E_SUCCESS       0
#define E_SYNTAX        1
#define E_MATH          2

void EStartup();

void ECleanup();

const char *EMessage();

int ELast();

int ESet(int err);

int ESuccess();

int ESyntaxExpectChar       (int lne, int col, int fnd, int exp);

int ESyntaxExpectName       (int lne, int col, int fnd);

int ESyntaxExpectNumber     (int lne, int col, int fnd);

int ESyntaxUnexpectedChar   (int lne, int col, int fnd);

int ESyntaxUnexpectedEOF    (int lne, int col);

int ESyntaxUnexpectedLength (int lne, int col);

int ESyntaxUndefined        (int lne, int col, const char *tok);

int ESyntaxImproper         (int lne, int col);

int ESyntaxIllegal          (int lne);

int EMath                   (int lne);

#endif

