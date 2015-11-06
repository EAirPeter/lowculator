#include "main.h"

#include "control.h"
#include "parse.h"

#include <unistd.h>

Result Main(int argc, char *argv[]) {
    if (argc == 1)
        CFile("--");
    else {
        int flg;
        int count = 0;
        while ((flg = getopt(argc, argv, ":c:e:f:o:p:x:")) != -1) {
            ++count;
            switch (flg) {
            case 'c':
                break;
            case 'e':
                break;
            case 'f':
                break;
            case 'o':
                break;
            case 'p':
                break;
            case 'x':
                break;
            case '?':
                break;
            case ':':
                break;
            default:
                break;
            }
        }
    }
    return R_SUCCE;
}

Result MStartup() {
    return R_SUCCE;
}

void MCleanup() {
}
