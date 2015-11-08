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

bool c_panic = false;

#define BUF_SIZE    256U

static const char *x_fnl = "(file name too long)";

static inline void X_CopyFileName(char *dst, const char *src, size_t bsz) {
    if (strlen(src) < bsz)
        strcpy(dst, src);
    else
        strcpy(dst, x_fnl);
}

typedef struct X_Element_ X_Element;

struct X_Element_ {
    FILE       *file;
    char        name[BUF_SIZE];
    size_t      line;
    X_Element  *next;
};

static X_Element *x_els = nullptr;

static inline void X_DestroyElement(X_Element *ele) {
    ele->next = x_els;
    x_els = ele;
}

static inline X_Element *X_CreateElement(FILE *file, const char *name) {
    X_Element *res = x_els;
    if (res)
        x_els = res->next;
    else
        res = (X_Element *) malloc(sizeof(X_Element));
    if (!res)
        return nullptr;
    res->file = file;
    if (!res->name) {
        X_DestroyElement(res);
        return nullptr;
    }
    strcpy(res->name, name);
    res->line = 0;
    return res;
}

static char         x_fmt[BUF_SIZE] = {};

static X_Element   *x_cur = nullptr;

static char         x_ofn[BUF_SIZE] = {};
static FILE        *x_out = nullptr;
static Stack       *x_sel = nullptr;

static inline Result X_FClose(FILE *f, const char *filename) {
    if (f != stdin && f != stdout && fclose(f)) {
        X_CopyFileName(r_str, filename, R_BUFFSIZE);
        return RI_FCLS;
    }
    return R_SUCCE;
}

Result CNextLine(size_t *line, char *buff, size_t size) {
    Result res = R_SUCCE;
    while (x_cur) {
        if (x_cur->file == stdin && fputs(">>> ", stdout) < 0)
            return RI_OPOU;
        if (fgets(buff, (int) size, x_cur->file))
            break;
        else if ((res = CEof()))
            return res;
    }
    if (!x_cur)
        return RI_EOFR;
    *line = ++x_cur->line;
    return R_SUCCE;
}

Result CEof() {
    Result res = R_SUCCE;
    if (x_cur) {
        if ((res = X_FClose(x_cur->file, x_cur->name)))
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
        if (!(f = fopen(filename, "r"))) {
            X_CopyFileName(r_str, filename, R_BUFFSIZE);
            return RI_FOPN;
        }
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
    FILE *f = nullptr;
    if (!strcmp(filename, "--"))
        f = stdout;
    else {
        if (!(f = fopen(filename, "w"))) {
            X_CopyFileName(r_str, filename, R_BUFFSIZE);
            return RI_FOPN;
        }
    }
    if ((res = X_FClose(x_out, x_ofn)))
        return res;
    if (f == stdout)
        strcpy(x_ofn, "(stdout)");
    else
        X_CopyFileName(x_ofn, filename, BUF_SIZE);
    x_out = f;
    return R_SUCCE;
}

Result CPanic(size_t line, size_t column, const char *val) {
    if (!strcmp(val, "on")) {
        c_panic = true;
        return R_SUCCE;
    }
    if (!strcmp(val, "off")) {
        c_panic = false;
        return R_SUCCE;
    }
    size_t len = strlen(val);
    if (len >= R_BUFFSIZE)
        return R_MAKER(line, column + len, RS_ULEN);
    strcpy(r_str, val);
    return R_MAKER(line, column + len, RS_NDEF);
}

_Noreturn Result CExit(size_t line, size_t column, const char *expr) {
    long double ans = 0.0L;
    if (*expr && PEval(&ans, line, column, expr))
        CTerminate(RS_ILLD);
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

Result CPrintResultMsg(Result code) {
    if (fputs(RMessage(code), stderr) < 0)
        return RI_OPOU;
    if (fputc('\n', stderr) < 0)
        return RI_OPOU;
    return R_SUCCE;
} 

Result CDirective(size_t line, size_t column, const char *dire) {
    return  DProcess(line, column, dire);
}

Result CEvaluate(size_t line, size_t column, const char *expr) {
    long double ans;
    Result res = PEval(&ans, line, column, expr);
    if (res)
        return res;
    if (fprintf(x_out, x_fmt, ans) < 0)
        return RI_OPOU;
    return R_SUCCE;
}

Result CStartup() {
    c_panic = false;
    x_els = nullptr;
    if (!(x_sel = SCreate()))
        return R_ITRNL;
    strcpy(x_ofn, "(stdout)");
    x_out = stdout;
    strcpy(x_fmt, "%.8Lf\n");
    return R_SUCCE;
}

void CCleanup() {
    while (x_cur)
        if (CEof())
            break;
    if (x_out && x_out != stdout)
        fclose(x_out);
    for (X_Element *i = x_cur; i; ) {
        X_DestroyElement(i);
        i = (X_Element *) SPopPtr(x_sel);
    }
    SDestroy(x_sel);
    for (X_Element *i = x_els; i; ) {
        X_Element *p = i->next;
        free(i);
        i = p;
    }
}

