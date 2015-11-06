#include "control.h"

#include "directives.h"
#include "error.h"
#include "parse.h"
#include "stack.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE    256U

char    x_fmt[BUF_SIZE];

FILE   *x_cur;
FILE   *x_out;
Stack  *x_sfl;

bool    x_pnc;

static inline Result X_FClose(FILE *f) {
    if (f != stdin && f != stdout)
        return fclose(f) ? RI_FCLS : R_SUCCE;
    return R_SUCCE;
}

Result CNextLine(char *buff, size_t size) {
    Result res = R_SUCCE;
    while (x_cur && !fgets(buff, (int) size, x_cur))
        if ((res = CEof()))
           return res;
    if (!x_cur)
        return RI_EOFR;
    return R_SUCCE;
}

Result CEof() {
    Result res = R_SUCCE;
    if ((res = x_cur ? X_FClose(x_cur) : RI_EOFR))
        return res;
    x_cur = (FILE *) SPopPtr(x_sfl);
    //EContext
    return R_SUCCE;
}

Result CFile(const char *filename) {
    FILE *f = nullptr;
    if (!strcmp(filename, "--"))
        f = stdin;
    else
        f = fopen(filename, "r");
    if (!f)
        return RI_FOPN;
    if (SPushPtr(x_sfl, x_cur))
        return R_ITRNL;
    x_cur = f;
    //EContext
    return R_SUCCE;
}

Result COutput(const char *filename) {
    Result res = R_SUCCE;
    if ((res = X_FClose(x_out)))
        return res;
    if (!strcmp(filename, "--"))
        x_out = stdout;
    else
        x_out = fopen(filename, "w");
    if (!x_out)
        return RI_FOPN;
    return R_SUCCE;
}

Result CPanic(size_t line, size_t column, const char *val) {
    if (!strcmp(val, "on")) {
        x_pnc = true;
        return R_SUCCE;
    }
    if (!strcmp(val, "off")) {
        x_pnc = false;
        return R_SUCCE;
    }
    size_t len = strlen(val);
    if (len >= R_BUFFSIZE)
        return R_MAKER(line, column + len, RS_ULEN);
    strcpy(r_str, val);
    return R_MAKER(line, column + len, RS_NDEF);
}

_Noreturn Result CExit(size_t line, size_t column, const char *expr) {
    long double ans;
    if (PEval(&ans, line, column, expr))
        CTerminate(0);
    CTerminate((int) lroundl(ans));
}

_Noreturn Result CTerminate(int exitcode) {
    exit(exitcode);
}

Result CPrecision(size_t line, size_t column, const char *expr) {
    long double ans;
    Result res = R_SUCCE;
    if ((res = PEval(&ans, line, column, expr)))
        return res;
    int prec = (int) lroundl(ans);
    if (prec >= 0 && prec < 20)
        sprintf(x_fmt, "%%.%dLf\n", prec);
    else
        return RS_ILLD;
    return R_SUCCE;
}

Result CDirective(size_t line, size_t column, const char *dire) {
    Result res = DProcess(line, column, dire);
    if (res) {
        if (fputs(RMessage(res), x_out) < 0)
            return RI_OPOU;
        if (fputc('\n', x_out) < 0)
            return RI_OPOU;
    }
    return R_SUCCE;
}

Result CEvaluate(size_t line, size_t column, const char *expr) {
    long double ans;
    Result res = PEval(&ans, line, column, expr);
    if (res) {
        if (fputs(RMessage(res), x_out) < 0)
            return RI_OPOU;
        if (fputc('\n', x_out) < 0)
            return RI_OPOU;
    }
    else if (fprintf(x_out, x_fmt, res) < 0)
        return RI_OPOU;
    return R_SUCCE;
}

Result CStartup() {
    if (!(x_sfl = SCreate()))
        return R_ITRNL;
    x_out = stdout;
    x_pnc = false;
    strcpy(x_fmt, "%.8Lf\n");
    return R_SUCCE;
}

void CCleanup() {
    X_FClose(x_cur);
    FILE *f;
    while ((f = SPopPtr(x_sfl)))
        X_FClose(f);
}

