#include "parse.h"

#include "error.h"

#include "lcctype.inl"

#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

int           x_lne;
int           x_col;
int           x_sca;
const char   *x_src;

#define BUF_SIZE  256

long double   x_pval;
char          x_pstr[BUF_SIZE];


#define ARG_MAX   4

#define E_ECHAR(c_)         \
    ESyntaxExpectChar(x_lne, x_col, *x_src, c_)
#define E_ENAME()           \
    ESyntaxExpectName(x_lne, x_col, *x_src)
#define E_ENUMBER()         \
    ESyntaxExpectNumber(x_lne, x_col, *x_src)
#define E_UCHAR()           \
    ESyntaxUnexpectedChar(x_lne, x_col, *x_src)
#define E_UEOF()            \
    ESyntaxUnexpectedEOF(x_lne, x_col)
#define E_USLEN()           \
    EUnsupportedLength(x_lne, x_col)

typedef long double (*Function_)(long double *, int);

Function_ FUNMANAGER(const char *);

//FWD
        int     X_EvalExpression();

        int     X_ParseInteger();
        int     X_ParseName();
        int     X_ParseNumber();

inline  bool    XX_ParseChar(int chr);
inline  int     XX_Peek();
inline  int     XX_Read();
inline  int     XX_Next();

inline  int     XXX_ToDigit(int chr);

int X_EvalExpression() {
    long double buf[ARG_MAX];
    int res = E_SUCCESS;
    int tmp;
    while (*x_src && *x_src != ')') {
        switch (*x_src) {
        case '+':
        case '-':
            if (CIsDigD(XX_Peek())) {
                if ((res = X_ParseNumber()))
                    return res;
                //pushvalue: x_pval
                break;
            }
        case '*':
        case '/':
        case '^':
            //pushopr
            XX_Next();
            break;
        case '(':
            XX_Next();
            if ((res = X_EvalExpression()))
                return res;
            //pushvalue: x_pval
            if (!XX_ParseChar(')'))
                return E_ECHAR(')');
            XX_Next();
            break;
        case ')':
            //Never reach here
            break;
        default:
            if (CIsDigD(*x_src)) {
                if ((res = X_ParseNumber()))
                    return res;
                //pushvalue: x_pval
            } else if(CIsAlpha(*x_src)) {
                //=>X_EvalFunctionCall
                if ((res = X_ParseName()))
                    return res;
                Function_ fun = FUNMANAGER(x_pstr);
                if (!XX_ParseChar('('))
                    return E_ECHAR('(');
                for (tmp = 0; tmp < ARG_MAX; ++tmp) {
                    if (!XX_Next())
                        return E_UEOF();
                    if (*x_src == ')')
                        break;
                    if ((res = X_EvalExpression()))
                        return res;
                    buf[tmp] = x_pval;
                    if (!XX_ParseChar(','))
                        break;
                }
                if (!XX_ParseChar(')'))
                    E_ECHAR(')');
                XX_Next();
                x_pval = fun(buf, tmp);
                if (ELast())
                    return ELast();
                //pushvalue: x_pval
            }
            break;
        }
    }
    return E_SUCCESS;
}

int X_ParseName() {
    if (!CIsAlpha(*x_src))
        return E_ENAME();
    const char *from = x_src;
    while (CIsName(XX_Read()));
    ptrdiff_t len = x_src - from;
    if (len > BUF_SIZE)
        return E_USLEN();
    strncpy(x_pstr, from, (size_t) len);
    x_pstr[len] = '\0';
    return E_SUCCESS;
}

int X_ParseNumber() {
    int res = E_SUCCESS;
    if ((res = X_ParseInteger()))
        return res;
    long double val = x_pval;
    if (*x_src == '.') {
        int dig;
        long double coe = 1.0L / x_sca;
        if (val < 0)
            while ((dig = XXX_ToDigit(XX_Read())) != -1) {
                val -= coe * dig;
                coe /= x_sca;
            }
        else
            while ((dig = XXX_ToDigit(XX_Read())) != -1) {
                val += coe * dig;
                coe /= x_sca;
            }
        if (CIsDigD(*x_src))
            return E_ENUMBER();
    }
    if (*x_src == 'E' || *x_src == 'e') {
        if ((res = X_ParseInteger()))
            return res;
        val *= powl(x_sca, x_pval);
    }
    if (CIsName(*x_src))
        return E_UCHAR();
    if (CIsWS(*x_src))
        XX_Next();
    x_pval = val;
    return E_SUCCESS;
}

int X_ParseInteger() {
    bool neg = *x_src == '-';
    if (neg || *x_src == '+')
        XX_Read();
    if (!*x_src)
        return E_UEOF();
    if (*x_src == '0') {
        switch (XX_Read()) {
        case 'b':
            x_sca = 2;
            XX_Read();
            break;
        case 'x':
            x_sca = 16;
            XX_Read();
            break;
        case '.':
            x_sca = 10;
            break;
        default:
            if (CIsDigO(*x_src))
                x_sca = 8;
            else if (CIsName(*x_src))
                return E_ENUMBER();
            x_sca = 10;
            break;
        }
    }
    else
        x_sca = 10;
    int dig = 0;
    long double val = 0.0L;
    if (*x_src == '0' && CIsName(XX_Read()))
        return E_UCHAR();
    while ((dig = XXX_ToDigit(*x_src)) != -1) {
        val = val * x_sca + dig;
        XX_Read();
    }
    if (CIsName(*x_src))
        return E_UCHAR();
    x_pval = neg ? -val : val;
    return E_SUCCESS;
}

bool XX_ParseChar(int chr) {
    if (CIsWS(*x_src))
        XX_Next();
    return *x_src == chr;
}

int XX_Read() {
    return *x_src ? *(++x_src) : 0;
}

int XX_Next() {
    while (*x_src && CIsWS(*x_src))
        ++x_col;
    return *x_src;
}

int XX_Peek() {
    return *x_src ? *(x_src + 1) : 0;
}

int XXX_ToDigit(int chr) {
    if (CIsDigD(chr))
        chr &= 0x0f;
    else if (CIsAUpr(chr))
        chr -= 'A';
    else if (CIsALwr(chr))
        chr -= 'a';
    else
        return -1;
    if (chr < x_sca)
        return chr;
    return -1;
}

long double PEval(int line, const char *expr) {
    x_lne = line;
    x_col = 0;
    x_src = expr;
    XX_Next();
    if (!X_EvalExpression())
        ESuccess();
    return x_pval;
}

