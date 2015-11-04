#ifndef LOWCULATOR_STACK_H_
#define LOWCULATOR_STACK_H_

#include "common.h"

typedef struct Stack_ Stack;

typedef union SElement_ SElement;

union SElement_ {
    int         opr;
    long double val;
};

void        SStartup();

void        SCleanup();

Stack      *SCreate();

void        SDestroy(Stack *stk);

void        SPushOpr(Stack *stk, int         opr);

void        SPushVal(Stack *stk, long double val);

int         SPopOpr(Stack *stk);

long double SPopVal(Stack *stk);

int         STopOpr(Stack *stk);

long double STopVal(Stack *stk);

bool        SEmpty(Stack *stk);

size_t      SSize(Stack *stk);

#endif

