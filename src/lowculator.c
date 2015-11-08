#define LOWCULATOR_LOWCULATOR_C_

#include "common.h"

#include "control.h"
#include "directives.h"
#include "functions.h"
#include "main.h"
#include "parse.h"
#include "result.h"
#include "stack.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void Cleanup() {
    fputs("Cleaning up...\n", stderr);
    MCleanup();
    CCleanup();
    PCleanup();
    FCleanup();
    DCleanup();
    SCleanup();
    RCleanup();
}

static void SignalHandler(int sig) {
    fprintf(stderr, "Signal %d caught. ", sig);
    exit((int) R_ITRNL);
}

#define SUP(mod_) if ((res = CONCAT(mod_, Startup)())) return res

static Result Startup() {
    fputs("Starting up...\n", stderr);
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
    signal(SIGINT , SignalHandler);
    signal(SIGTERM, SignalHandler);
    return R_SUCCE;
}

int main(int argc, char *argv[]) {
    Result res = R_SUCCE;
    if ((res = Startup())) {
        fputs("Startup failed.\n", stderr);
        abort();
    }
    if ((res = Main(argc, argv)))
        return (int) res;
    return (int) R_SUCCE;
}

