#include "common.h"
#include "error.h"
#include "functions.h"
#include "parse.h"

#include <stdlib.h>

void Cleanup() {
    ECleanup();
    FCleanup();
    PCleanup();
}

void Startup() {
    if (atexit(Cleanup))
        _Exit(-1);
    if (at_quick_exit(Cleanup))
        _Exit(-1);
    EStartup();
    FStartup();
    PStartup();
}

int main(int argc, char *argv[]) {
    Startup();
    return 0;
    UNUSED(argc);
    UNUSED(argv);
}

