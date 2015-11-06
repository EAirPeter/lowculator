#ifndef LOWCULATOR_STACK_H_
#define LOWCULATOR_STACK_H_

#include "common.h"
#include "result.h"

typedef struct Stack_ Stack;

Result      SStartup();

void        SCleanup();

Stack      *SCreate();

void        SDestroy(Stack *stk);

Result      SPushInt(Stack *stk, int         int_);

Result      SPushLdb(Stack *stk, long double ldb_);

Result      SPushPtr(Stack *stk, void       *ptr_);

int         SPopInt(Stack *stk);

long double SPopLdb(Stack *stk);

void       *SPopPtr(Stack *stk);

int         STopInt(Stack *stk);

long double STopLdb(Stack *stk);

void       *STopPtr(Stack *stk);

bool        SEmpty(Stack *stk);

size_t      SSize(Stack *stk);

#endif

