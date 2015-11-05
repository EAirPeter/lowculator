#include "common.h"

#include "control.h"
#include "directives.h"
#include "error.h"
#include "functions.h"
#include "main.h"
#include "parse.h"
#include "stack.h"

#include <stdlib.h>

void Cleanup() {
    MCleanup();
    CCleanup();
    PCleanup();
    FCleanup();
    DCleanup();
    ECleanup();
    SCleanup();
}

void Startup() {
    SStartup();
    EStartup();
    DStartup();
    FStartup();
    PStartup();
    CStartup();
    MStartup();
    atexit(Cleanup);
}

int main(int argc, char *argv[]) {
    Startup();
    return Main(argc, argv);
}

