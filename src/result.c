#define LOWCULATOR_RESULT_C_

#include "result.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int     r_in0;
int     r_in1;
char    r_str[R_BUFFSIZE];

static char x_msg[R_BUFFSIZE << 2U];
static char x_ctx[R_BUFFSIZE];
static char x_wrp[256][8];

#define MSGPRINTF(format_, ...)   \
    sprintf(x_msg, "In %s, at %d:%d, " format_, x_ctx, lne, col, __VA_ARGS__)
#define MSGSTRING(string_)        \
    sprintf(x_msg, "In %s, at %d:%d, " string_, x_ctx, lne, col)

#define WR(c_)  x_wrp[(unsigned char) (c_)]
#define WR0     WR(r_in0)
#define WR1     WR(r_in1)

const char *RMessage(Result code) {
    if (!code)
        return strcpy(r_str, "Success.");
    Result lne = R_LNE(code);
    Result col = R_COL(code);
    Result typ = R_TYP(code);
    switch (typ) {
    case RS_RESV:
        MSGSTRING("some syntax error occured.");
        break;
    case RS_ECHR:
        MSGPRINTF("expected %s, but %s found.", WR1, WR0);
        break;
    case RS_ENAM:
        MSGPRINTF("expected NAME, but %s found.", WR0);
        break;
    case RS_ENUM:
        MSGPRINTF("expected NUMBER, but %s found.", WR0);
        break;
    case RS_UCHR:
        MSGPRINTF("unexpected %s found.", WR0);
        break;
    case RS_ULEN:
        MSGSTRING("the length of string is too long.");
        break;
    case RS_UTRM:
        MSGSTRING("unexpected termination.");
        break;
    case RS_NDEF:
        MSGPRINTF("undefined name \'%s\' found.", r_str);
        break;
    case RS_IMPR:
        MSGSTRING("improper use of the function or constant.");
        break;
    case RS_ILLE:
        MSGSTRING("the expression is ill-formed.");
        break;
    case RS_ILLD:
        MSGSTRING("the directive is ill-formed.");
        break;
    case RS_ILLA:
        MSGSTRING("invalid argument.");
        break;
    case RM_RESV:
        MSGSTRING("some math error occured.");
        break;
    case RM_DOMN:
        MSGSTRING("domain error occured.");
        break;
    case RM_RANG:
        MSGSTRING("range error occured.");
        break;
    case RI_RESV:
        MSGSTRING("some I/O error occured.");
        break;
    case RI_FOPN:
        MSGPRINTF("failed to open the file \"%s\".", r_str);
        break;
    case RI_FCLS:
        MSGPRINTF("failed to close the file \"%s\".", r_str);
        break;
    case RI_OPIN:
        MSGSTRING("an input operation failed.");
        break;
    case RI_OPOU:
        MSGSTRING("an output operation failed.");
        break;
    case RI_EOFR:
        MSGSTRING("EOF reached.");
        break;
    case RI_ITNL:
        strcpy(x_msg, "Internal error occured.");
        break;
    default:
        strcpy(x_msg, "Unknown error occured.");
        break;
    }
    return x_msg;
}

Result RMathType(long double val) {
    if (isfinite(val))
        return R_SUCCE;
    if (errno == EDOM)
        return RM_DOMN;
    if (errno == ERANGE)
        return RM_RANG;
    return RM_RESV;
}

Result RContext(const char *ctx) {
    if (strlen(ctx) >= R_BUFFSIZE)
        return RS_ULEN;
    strcpy(x_ctx, ctx);
    return R_SUCCE;
}

Result RStartup() {
    r_in0 = 0;
    r_in1 = 0;
    strcpy(r_str, "");
    strcpy(x_msg, "");
    strcpy(x_ctx, "(unknown)");
    for (int i = 0; i < 256; ++i)
        if (isprint(i))
            sprintf(WR(i), "\'%c\'", i);
        else
            sprintf(WR(i), "\'\\x%02x\'", i);
    return R_SUCCE;
}

void RCleanup() {
}

