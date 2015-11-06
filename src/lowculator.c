#define LOWCULATOR_LOWCULATOR_C_

#include "common.h"

#include "control.h"
#include "directives.h"
#include "functions.h"
#include "main.h"
#include "parse.h"
#include "result.h"
#include "stack.h"

#include <stdlib.h>

static void Cleanup() {
    MCleanup();
    CCleanup();
    PCleanup();
    FCleanup();
    DCleanup();
    SCleanup();
    RCleanup();
}

#define SUP(mod_) if ((res = CONCAT(mod_, Startup)())) return res

static Result Startup() {
    Result res = R_SUCCE;
    SUP(R);
    SUP(S);
    SUP(D);
    SUP(F);
    SUP(P);
    SUP(C);
    SUP(M);
    if (atexit(Cleanup))
        return R_ITRNL;
    return R_SUCCE;
}

int main(int argc, char *argv[]) {
    Result res = R_SUCCE;
    if ((res = Startup()))
        return (int) res;
    if ((res = Main(argc, argv)))
        return (int) res;
    return (int) R_SUCCE;
}

