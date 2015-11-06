CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

ADD_EXECUTABLE("lowculator"
    "control.c"
    "directives.c"
    "functions.c"
    "lowculator.c"
    "main.c"
    "parse.c"
    "result.c"
    "stack.c"
)

INSTALL (TARGETS "lowculator"
    RUNTIME
        DESTINATION "bin"
        CONFIGURATIONS Release
)

