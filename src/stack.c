#include "stack.h"

#include <stdlib.h>

typedef struct X_Node_ X_Node;

struct Stack_ {
    X_Node             *top;
    size_t              siz;
    Stack              *nex;
};

struct X_Node_ {
    SElement            ele;
    X_Node             *nex;
};

Stack   *x_stks;
X_Node  *x_nods;

static inline X_Node *X_CreateNode(X_Node *nex) {
    X_Node *res = x_nods;
    if (res)
        x_nods = res->nex;
    else
        res = (X_Node *) malloc(sizeof(X_Node));
    res->nex = nex;
    return res;
}

static inline X_Node *X_DestroyNode(X_Node *nod) {
    X_Node *res = nod->nex;
    nod->nex = x_nods;
    x_nods = nod;
    return res;
}

Stack *SCreate() {
    Stack *res = x_stks;
    if (res)
        x_stks = res->nex;
    else
        res = (Stack *) malloc(sizeof(Stack));
    res->top = nullptr;
    res->siz = 0;
    res->nex = nullptr;
    return res;
}

void SDestroy(Stack *stk) {
    for (X_Node *i = stk->top; i; ) {
        X_Node *p = i->nex;
        X_DestroyNode(i);
        i = p;
    }
    stk->nex = x_stks;
    x_stks = stk;
}

void SPushOpr(Stack *stk, int opr) {
    stk->top = X_CreateNode(stk->top);
    stk->top->ele.opr = opr;
    ++stk->siz;
}

void SPushVal(Stack *stk, long double val) {
    stk->top = X_CreateNode(stk->top);
    stk->top->ele.val = val;
    ++stk->siz;
}

int SPopOpr(Stack *stk) {
    if (!stk->top)
        return 0;
    int res = stk->top->ele.opr;
    stk->top = X_DestroyNode(stk->top);
    --stk->siz;
    return res;
}

long double SPopVal(Stack *stk) {
    if (!stk->top)
        return 0.0L;
    long double res = stk->top->ele.val;
    stk->top = X_DestroyNode(stk->top);
    --stk->siz;
    return res;
}

int STopOpr(Stack *stk) {
    return stk->top ? stk->top->ele.opr : 0;
}

long double STopVal(Stack *stk) {
    return stk->top ? stk->top->ele.val : 0.0L;
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

