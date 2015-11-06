#define LOWCULATOR_CONTROL_C_

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

typedef struct X_Element_ X_Element;

struct X_Element_ {
    FILE       *file;
    char       *name;
    X_Element  *next;
};

X_Element *x_els;

inline void X_DestroyElement(X_Element *ele) {
    if (ele->name) {
        free(ele->name);
        ele->name = nullptr;
    }
    ele->next = x_els;
    x_els = ele;
}

inline X_Element *X_CreateElement(FILE *file, const char *name) {
    X_Element *res = x_els;
    if (res)
        x_els = res->next;
    else
        res = (X_Element *) malloc(sizeof(X_Element));
    if (!res)
        return nullptr;
    res->file = file;
    res->name = (char *) malloc(strlen(name) + 1);
    if (!res->name) {
        X_DestroyElement(res);
        return nullptr;
    }
    strcpy(res->name, name);
    return res;
}

char    x_fmt[BUF_SIZE];

X_Element *x_cur;

FILE   *x_out;
Stack  *x_sel;

bool    x_pnc;

static inline Result X_FClose(FILE *f) {
    if (f != stdin && f != stdout)
        return fclose(f) ? RI_FCLS : R_SUCCE;
    return R_SUCCE;
}

Result CNextLine(char *buff, size_t size) {
    Result res = R_SUCCE;
    while (x_cur && !fgets(buff, (int) size, x_cur->file))
        if ((res = CEof()))
           return res;
    if (!x_cur)
        return RI_EOFR;
    return R_SUCCE;
}

Result CEof() {
    Result res = R_SUCCE;
    if (x_cur) {
        if ((res = X_FClose(x_cur->file)))
            return res;
        X_DestroyElement(x_cur);
        x_cur = (X_Element *) SPopPtr(x_sel);
    }
    else
        return RI_EOFR;
    if (x_cur && (res = RContext(x_cur->name)))
        return res;
    return R_SUCCE;
}

Result CFile(const char *filename) {
    FILE *f = nullptr;
    X_Element *e = nullptr;
    if (!strcmp(filename, "--"))
        e = X_CreateElement(stdin, "(stdin)");
    else {
        if (!(f = fopen(filename, "r")))
            return RI_FOPN;
        e = X_CreateElement(f, filename);
    }
    if (!e)
        return R_ITRNL;
    if (SPushPtr(x_sel, x_cur))
        return R_ITRNL;
    x_cur = e;
    Result res = R_SUCCE;
    if ((res = RContext(x_cur->name)))
        return res;
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
    x_els = nullptr;
    if (!(x_sel = SCreate()))
        return R_ITRNL;
    x_out = stdout;
    x_pnc = false;
    strcpy(x_fmt, "%.8Lf\n");
    return R_SUCCE;
}

void CCleanup() {
    while (x_cur)
        if (CEof())
            break;
    X_Element *e = x_cur;
    while (e) {
        X_DestroyElement(e);
        e = (X_Element *) SPopPtr(x_sel);
    }
    SDestroy(x_sel);
}

