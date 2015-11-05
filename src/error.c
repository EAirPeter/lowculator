#include "error.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 256

char    x_emsg[BUF_SIZE];
int     x_elno = E_SUCCESS;
char    x_cont[BUF_SIZE];
char    x_wrap[256][8];

#define X_MSGPRINTF(format_, ...)   \
    sprintf(x_emsg, "In %s, at %d:%d, " format_, x_cont, lne, col, __VA_ARGS__)
#define X_MSGSTRING(string_)        \
    sprintf(x_emsg, "In %s, At %d:%d, " string_, x_cont, lne, col)
#define X_MSGSTRINL(string_)        \
    sprintf(x_emsg, "In %s, At %d:?, " string_, x_cont, lne)

#define WR(c_) x_wrap[(unsigned char) (c_)]

const char *EMessage() {
    return x_emsg;
}

int ELast() {
    return x_elno;
}

int ESet(int err) {
    return x_elno = err;
}

int EContext(const char *ctxname) {
    return elno = strcpy_s(x_cont, BUF_SIZE, filename) ? E_SYNTAX : E_SUCCESS;
}

int ESuccess() {
    strcpy(x_emsg, "Succeess.");
    return x_elno = E_SUCCESS;
}

int ESyntaxDirective(int lne, int col) {
    X_MSGSTRING("invalid directive.");
    return x_elno = E_SYNTAX;
}

int ESyntaxExpectChar(int lne, int col, int fnd, int exp) {
    X_MSGPRINTF("expected %s, but %s found.", WR(exp), WR(fnd));
    return x_elno = E_SYNTAX;
}

int ESyntaxExpectName(int lne, int col, int fnd) {
    X_MSGPRINTF("expected FUNCTION_NAME but %s found.", WR(fnd));
    return x_elno = E_SYNTAX;
}

int ESyntaxExpectNumber(int lne, int col, int fnd) {
    X_MSGPRINTF("expected NUMBER but %s found.", WR(fnd));
    return x_elno = E_SYNTAX;
}

int ESyntaxUnexpectedChar(int lne, int col, int fnd) {
    if (!fnd)
        return ESyntaxUnexpectedTerm(lne, col);
    X_MSGPRINTF("unexpected %s.", WR(fnd));
    return x_elno = E_SYNTAX;
}

int ESyntaxUnexpectedTerm(int lne, int col) {
    X_MSGSTRING("unexpected termination.");
    return x_elno = E_SYNTAX;
}

int ESyntaxUnexpectedLength(int lne, int col) {
    X_MSGSTRING("the function name is too long.");
    return x_elno = E_SYNTAX;
}

int ESyntaxUndefined(int lne, int col, const char *tok) {
    X_MSGPRINTF("undefined identifier \'%s\' found.", tok);
    return x_elno = E_SYNTAX;
}

int ESyntaxImproper(int lne, int col) {
    X_MSGSTRING("improper use of the function or constant.");
    return x_elno = E_SYNTAX;
}

int ESyntaxIllegal(int lne) {
    X_MSGSTRINL("the expression is ill-formed.");
    return x_elno = E_SYNTAX;
}

int EMath(int lne) { 
    switch (errno) {
        case EDOM:
            X_MSGSTRINL("domain error occured");
            break;
        case ERANGE:
            X_MSGSTRINL("range error occured");
            break;
        default:
            break;
    }
    return x_elno = E_MATH;
}

void EStartup() {
    EContext("(unknown)");
    for (int i = 0; i < 256; ++i)
        if (isprint(i))
            sprintf(WR(i), "\'%c\'", i);
        else
            sprintf(WR(i), "\'\\x%02x\'", i);
}

void ECleanup() {
}

