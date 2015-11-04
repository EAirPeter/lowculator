#include "functions.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lcmath.inl"

typedef struct X_Node_ X_Node;

struct X_Node_ {
    char           *key;
    MathFunction   *val;
    X_Node         *nex;
};

#define BUCKET_SIZE 100003U

X_Node *x_map[BUCKET_SIZE];

uint32_t X_Hash(const char *str) {
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

bool FAddFunction(const char *name, MathFunction *func) {
    if (!name || !func)
        return false;
    uint32_t hash = X_Hash(name);
    if (x_map[hash])
        for (X_Node *i = x_map[hash]; i; i = i->nex)
            if (!strcmp(i->key, name))
                return false;
    X_Node *p = (X_Node *) malloc(sizeof(X_Node));
    p->key = strcpy((char *) malloc(strlen(name)), name);
    p->val = func;
    p->nex = x_map[hash];
    x_map[hash] = p;
    return true;
}

void FStartup() {
    memset(x_map, 0, sizeof(x_map));
    ADD_MATH;
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

