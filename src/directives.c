#define LOWCULATOR_DIRECTIVES_C_

#include "directives.h"

#include "control.h"
#include "error.h"
#include "parse.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static size_t       x_lne;
static size_t       x_col;
static const char  *x_src;
static const char  *x_cur;

#define LNE             (x_lne)
#define COL             (x_col + (size_t) (x_cur - x_src))

#define E_DIR()         R_MAKER(LNE, COL, RS_ILLD)

#define DIRSTR(dir_)    CONCAT(x_DIR, dir_)
#define DIRLEN(dir_)    CONCAT(x_LEN, dir_)
#define DIRFUN(dir_)    CONCAT(X_DIR, dir_)

#define INITDIR(dir_)   DIRLEN(dir_) = strlen(DIRSTR(dir_))

#define CHECKDIR(dir_)                                      \
    if (!strncmp(x_cur, DIRSTR(dir_), DIRLEN(dir_))) do {   \
        x_cur += DIRLEN(dir_);                              \
        return DIRFUN(dir_)();                              \
    } while (false)         

#define DEFDIR(dir_, ...)                   \
    const char *DIRSTR(dir_) = #dir_;       \
    size_t      DIRLEN(dir_);               \
    __VA_ARGS__ static Result DIRFUN(dir_)()

DEFDIR(eof) {
    return CEof();
}

DEFDIR(evaluate) {
    while (*x_cur && !isgraph(*x_cur))
        ++x_cur;
    return CEvaluate(LNE, COL, x_cur);
}

DEFDIR(exit, _Noreturn) {
    while (*x_cur && !isgraph(*x_cur))
        ++x_cur;
    CExit(LNE, COL, x_cur);
}

DEFDIR(file) {
    while (*x_cur && !isgraph(*x_cur))
        ++x_cur;
    return CFile(x_cur);
}

DEFDIR(output) {
    while (*x_cur && !isgraph(*x_cur))
        ++x_cur;
    return COutput(x_cur);
}

DEFDIR(panic) {
    while (*x_cur && !isgraph(*x_cur))
        ++x_cur;
    return CPanic(LNE, COL, x_cur);
}

DEFDIR(precision) {
    while (*x_cur && !isgraph(*x_cur))
        ++x_cur;
    return CPrecision(LNE, COL, x_cur);
}

Result DProcess(size_t line, size_t column, const char *dire) {
    x_lne = line;
    x_col = column;
    x_src = dire;
    x_cur = x_src;
    if (*x_cur != '#')
        return E_DIR();
    ++x_cur;
    CHECKDIR(eof);
    CHECKDIR(evaluate);
    CHECKDIR(exit);
    CHECKDIR(file);
    CHECKDIR(output);
    CHECKDIR(panic);
    CHECKDIR(precision);
    return E_DIR();
}

Result DStartup() {
    INITDIR(eof);
    INITDIR(evaluate);
    INITDIR(exit);
    INITDIR(file);
    INITDIR(output);
    INITDIR(panic);
    INITDIR(precision);
    return R_SUCCE;
}

void DCleanup() {
}

