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

static inline void X_FClose(FILE *f) {
    if (f != stdin && f != stdout)
        errno = fclose(f);
}

int CNextLine(char *buff, size_t size) {
    while (x_cur && !fgets(buff, (int) size, x_cur))
        if (!CEof())
            exit(errno);
    if (!x_cur)
        return EOF;
    return E_SUCCESS;
}

int CEof() {
    if (x_cur)
        X_FClose(x_cur);
    else
        exit(errno);
    if (errno)
        exit(errno);
    x_cur = (FILE *) SPopPtr(x_sfl);
    //EContext
    return E_SUCCESS;
}

int CFile(const char *filename) {
    FILE *f = nullptr;
    if (!strcmp(filename, "--"))
        f = stdin;
    else
        f = fopen(filename, "r");
    if (!f)
        exit(errno);
    SPushPtr(x_sfl, x_cur);
    x_cur = f;
    //EContext
    return E_SUCCESS;
}

int COutput(const char *filename) {
    X_FClose(x_out);
    if (errno)
        exit(errno);
    if (!strcmp(filename, "--"))
        x_out = stdout;
    else
        x_out = fopen(filename, "w");
    if (!x_out)
        exit(errno);
    return E_SUCCESS;
}

int CPanic(int line, int column, const char *val) {
    if (!strcmp(val, "on")) {
        x_pnc = true;
        return E_SUCCESS;
    }
    if (!strcmp(val, "off")) {
        x_pnc = false;
        return E_SUCCESS;
    }
    return ESyntaxUndefined(line, column + (int) strlen(val), val);
}

_Noreturn int CExit(int line, int column, const char *expr) {
    exit((int) lroundl(PEval(line, column, expr)));
}

int CPrecision(int line, int column, const char *expr) {
    int prec = (int) lroundl(PEval(line, column, expr));
    if (errno)
        return errno;
    if (prec >= 0 && prec < 20)
        sprintf(x_fmt, "%%.%dLf\n", prec);
    else {
        errno = EDOM;
        return EMath(line);
    }
    return E_SUCCESS;
}

int CDirective(int line, int column, const char *dire) {
    int err = DProcess(line, column, dire);
    if (err) {
        if (fputs(EMessage(), x_out) < 0)
            exit(E_IO);
        if (fputc('\n', x_out) < 0)
            exit(E_IO);
        if (x_pnc)
            exit(err);
    }
    return E_SUCCESS;
}

int CEvaluate(int line, int column, const char *expr) {
    long double res = PEval(line, column, expr);
    int err = errno;
    if (err) {
        if (fputs(EMessage(), x_out) < 0)
            exit(E_IO);
        if (fputc('\n', x_out) < 0)
            exit(E_IO);
        if (x_pnc)
            exit(err);
    }
    else if (fprintf(x_out, x_fmt, res) < 0)
        exit(E_IO);
    return E_SUCCESS;
}

void CStartup() {
    x_sfl = SCreate();
    x_out = stdout;
    x_pnc = false;
    strcpy(x_fmt, "%.8Lf\n");
}

void CCleanup() {
    X_FClose(x_cur);
    FILE *f;
    while ((f = SPopPtr(x_sfl)))
        X_FClose(f);
}

