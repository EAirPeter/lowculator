#ifndef LOWCULATOR_STACK_H_
#define LOWCULATOR_STACK_H_

#include "common.h"

typedef struct Stack_ Stack;

void        SStartup();

void        SCleanup();

Stack      *SCreate();

void        SDestroy(Stack *stk);

void        SPushInt(Stack *stk, int         int_);

void        SPushLdb(Stack *stk, long double ldb_);

void        SPushPtr(Stack *stk, void       *ptr_);

int         SPopInt(Stack *stk);

long double SPopLdb(Stack *stk);

void       *SPopPtr(Stack *stk);

int         STopInt(Stack *stk);

long double STopLdb(Stack *stk);

void       *STopPtr(Stack *stk);

bool        SEmpty(Stack *stk);

size_t      SSize(Stack *stk);

#endif

