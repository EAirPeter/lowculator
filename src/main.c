#define LOWCULATOR_MAIN_C_

#include "main.h"

#include "control.h"
#include "parse.h"

#include <string.h>

#define BUF_SIZE 1024U

static char buf[BUF_SIZE];

static bool suppressed;

static Result ProcessFile(const char *filename) {
    Result res = R_SUCCE;
    size_t line = 0;
    if ((res = CFile(filename)))
        return res;
    while (!(res = CNextLine(&line, buf, BUF_SIZE))) {
        char *lf = strchr(buf, '\n');
        if (lf)
            *lf = '\0';
        if (*buf == '#')
            res = CDirective(line, 0, buf);
        else {
            char *c = buf;
            while (*c && (*c == ' ' || *c == '\t'))
                ++c;
            if (*c)
                res = CEvaluate(line, (size_t) (c - buf), c);
        }
        if (res) {
            Result rex = CPrintResultMsg(res);
            if (rex)
                return rex;
            if (c_panic)
                break;
        }
    }
    if (!res || !c_panic)
        RContext("(cmdline)");
    suppressed = true;
    return res;
}

Result Main(int argc, char *argv[]) {
    Result res = R_SUCCE;
    if (argc <= 1)
        res = ProcessFile("--");
    else {
        RContext("(cmdline)");
        size_t line = 0;
        for (char **i = argv + 1; *i; ++i, ++line) {
            suppressed = false;
            char *opt = *i;
            if (i[1] && *opt == '-') {
                ++opt;
                switch (*opt) {
                case 'c':
                    res = CPrecision(line, 0, *++i);
                    break;
                case 'e':
                    res = CEvaluate(line, 0, *++i);
                    break;
                case 'f':
                    res = ProcessFile(*++i);
                    if (R_TYP(res) == RI_EOFR)
                        res = R_SUCCE;
                    break;
                case 'o':
                    res = COutput(*++i);
                    break;
                case 'p':
                    res = CPanic(line, 0, *++i);
                    break;
                case 'x':
                    res = CExit(line, 0, *++i);
                    break;
                default:
                    res = R_MAKER(line, 0, RS_ILLA);
                    break;
                }
            }
            else
                res = R_MAKER(line, 0, RS_ILLA);
            if (res) {
                if (!suppressed) {
                    Result rex = CPrintResultMsg(res);
                    if (rex)
                        return rex;
                }
                if (c_panic)
                    break;
            }
        }
    }
    return res;
}

Result MStartup() {
    return R_SUCCE;
}

void MCleanup() {
}
