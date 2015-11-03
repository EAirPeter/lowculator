#ifndef LOWCULATOR_PARSE_H_
#define LOWCULATOR_PARSE_H_

#include "common.h"

#define ET_NUMBER   0
#define ET_OPERATOR 1
#define ET_FUNCALL  2

typedef struct Element_ Element;

struct Element_ {
    int      type;
    Element *next;
    union {
        struct {
            long double val;
        } num;
        struct {
            int val;
        } opr;
        struct {
            char    *name;
            Element *pars;
        } fcl;
    };
};

void        PInit();

long double PEval(int line, const char *expr);

#endif

