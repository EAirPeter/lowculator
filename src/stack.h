#ifndef LOWCULATOR_STACK_H_
#define LOWCULATOR_STACK_H_

#include "common.h"

typedef struct Stack_ Stack;

typedef void (DestroyFunction)(void *);

void    SStartup();

void    SCleanup();

Stack  *SCreate (DestroyFunction *des);

void    SDestroy(Stack *stk);

void    SPush   (Stack *stk, void *val);

void   *SPop    (Stack *stk);

void   *STop    (Stack *stk);

bool    SEmpty  (Stack *stk);

size_t  SSize   (Stack *stk);

#endif

