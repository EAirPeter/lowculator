CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

ADD_EXECUTABLE("lowculator" "main.c")

INSTALL (TARGETS "lowculator"
    RUNTIME
        DESTINATION "bin"
        CONFIGURATIONS Release
)

