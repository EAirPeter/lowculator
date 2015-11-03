#include "error.h"

#include <stdio.h>
#include <string.h>

char    x_emsg[256];
int     x_elno = E_SUCCESS;

#define X_MSGPRINTF(format_, ...)  \
    sprintf(x_emsg, "At %d:%d, " format_, lne, col, __VA_ARGS__)
#define X_MSGSTRING(string_)       \
    sprintf(x_emsg, "At %d:%d, " string_, lne, col)

const char *EMessage() {
    return x_emsg;
}

int ELast() {
    return x_elno;
}

int ESuccess() {
    strcpy(x_emsg, "Succeess.");
    return x_elno = E_SUCCESS;
}

int ESyntaxExpectChar(int lne, int col, int fnd, int exp) {
    X_MSGPRINTF("expected \'%c\', but '\'%c\' found.", exp, fnd);
    return x_elno = E_SYNTAX;
}

int ESyntaxExpectName(int lne, int col, int fnd) {
    X_MSGPRINTF("expected FUNCTION_NAME but \'%c\' found.", fnd);
    return x_elno = E_SYNTAX;
}

int ESyntaxExpectNumber(int lne, int col, int fnd) {
    X_MSGPRINTF("expected NUMBER but \'%c\' found.", fnd);
    return x_elno = E_SYNTAX;
}

int ESyntaxUnexpectedChar(int lne, int col, int fnd) {
    X_MSGPRINTF("unexpected \'%c\'.", fnd);
    return x_elno = E_SYNTAX;
}

int ESyntaxUnexpectedEOF(int lne, int col) {
    X_MSGSTRING("unexpected EOF reached.");
    return x_elno = E_SYNTAX;
}

int EUnsupportedLength(int lne, int col) {
    X_MSGSTRING("the function name is too long.");
    return x_elno = E_UNSUPPORTED;
}

