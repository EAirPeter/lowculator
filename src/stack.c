#include "stack.h"

#include <stdlib.h>

typedef struct X_Node_ X_Node;

struct Stack_ {
    X_Node             *top;
    DestroyFunction    *des;
    size_t              siz;
    Stack              *nex;
};

struct X_Node_ {
    void               *val;
    X_Node             *nex;
};

Stack   *x_stks;
X_Node  *x_nods;

static  inline X_Node *X_CreateNode(void *val, X_Node *nex) {
    X_Node *res = x_nods;
    if (res)
        x_nods = res->nex;
    else
        res = (X_Node *) malloc(sizeof(X_Node));
    res->val = val;
    res->nex = nex;
    return res;
}

static inline X_Node *X_DestroyNode(X_Node *nod) {
    X_Node *res = nod->nex;
    nod->nex = x_nods;
    x_nods = nod;
    return res;
}

Stack *SCreate(DestroyFunction *des) {
    Stack *res = x_stks;
    if (res)
        x_stks = res->nex;
    else
        res = (Stack *) malloc(sizeof(Stack));
    res->top = nullptr;
    res->des = des;
    res->siz = 0;
    res->nex = nullptr;
    return res;
}

void SDestroy(Stack *stk) {
    for (X_Node *i = stk->top; i; ) {
        X_Node *p = i->nex;
        stk->des(i->val);
        X_DestroyNode(i);
        i = p;
    }
    stk->nex = x_stks;
    x_stks = stk;
}

void SPush(Stack *stk, void *val) {
    stk->top = X_CreateNode(val, stk->top);
    ++stk->siz;
}

void *SPop(Stack *stk) {
    if (!stk->top)
        return nullptr;
    void *res = stk->top->val;
    stk->top = X_DestroyNode(stk->top);
    --stk->siz;
    return res;
}

void *STop(Stack *stk) {
    return stk->top ? stk->top->val : nullptr;
}

bool SEmpty(Stack *stk) {
    return !stk->top;
}

size_t SSize(Stack *stk) {
    return stk->siz;
}

void SStartup() {
    x_stks = nullptr;
    x_nods = nullptr;
}

void SCleanup() {
    for (Stack *i = x_stks; i; ) {
        Stack *p = i->nex;
        free(i);
        i = p;
    }
    for (X_Node *i = x_nods; i; ) {
        X_Node *p = i->nex;
        free(i);
        i = p;
    }
}

