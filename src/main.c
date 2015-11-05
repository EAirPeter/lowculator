#include <stdio.h>

#define UNREF(x_) ((void) (sizeof(x_)))
#define UNREF_PTR(x_) UNREF((void *) x_)

int main(int argc, char *argv[]) {
    puts("hello world");
    UNREF(argc);
    UNREF_PTR(argv);
}

