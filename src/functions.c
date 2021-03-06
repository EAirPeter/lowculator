#define LOWCULATOR_FUNCTIONS_C_

#include "functions.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "math.inl.h"

typedef struct X_Node_ X_Node;

struct X_Node_ {
    char           *key;
    MathFunction   *val;
    X_Node         *nex;
};

#define BUCKET_SIZE 100003U

static X_Node *x_map[BUCKET_SIZE] = {};

static uint32_t X_Hash(const char *str) {
    uint32_t res = 0xAAAAAAAA;
    for (uint32_t i = 0; *str; ++str, i ^= 1)
        res ^= (i & 1) ?
            (~((res << 11) + ((uint32_t) *str ^ (res >> 5)))) :
            (  (res <<  7) ^ ((uint32_t) *str * (res >> 3)) ) ;
    return res % BUCKET_SIZE;
}

MathFunction *FGetFunction(const char *name) {
    X_Node *cur = x_map[X_Hash(name)];
    while (cur && strcmp(cur->key, name))
        cur = cur->nex;
    return cur ? cur->val : nullptr;
}

Result FAddFunction(const char *name, MathFunction *func) {
    if (!name || !func)
        return R_ITRNL;
    uint32_t hash = X_Hash(name);
    if (x_map[hash])
        for (X_Node *i = x_map[hash]; i; i = i->nex)
            if (!strcmp(i->key, name))
                return R_ITRNL;
    X_Node *p = (X_Node *) malloc(sizeof(X_Node));
    if (!p)
        return R_ITRNL;
    p->key = (char *) malloc(strlen(name) + 1);
    if (!p->key) {
        free(p);
        return R_ITRNL;
    }
    strcpy(p->key, name);
    p->val = func;
    p->nex = x_map[hash];
    x_map[hash] = p;
    return R_SUCCE;
}

Result FStartup() {
    memset(x_map, 0, sizeof(x_map));
    ADD_MATH;
    return R_SUCCE;
}

void FCleanup() {
    for (size_t i = 0; i < BUCKET_SIZE; ++i)
        for (X_Node *j = x_map[i]; j; ) {
            free(j->key);
            X_Node *p = j->nex;
            free(j);
            j = p;
        }
}

