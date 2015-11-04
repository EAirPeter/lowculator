#include "parse.h"

#include "error.h"
#include "functions.h"
#include "stack.h"

#include <math.h>
#include <string.h>
#include <tgmath.h>

#include "lcctype.inl"

int         x_lne;
int         x_col;
int         x_sca;
const char *x_src;
bool        x_lav;

#define BUF_SIZE  256U

long double x_pval;
char        x_pstr[BUF_SIZE];

int         x_pri[256];

#define ARG_MAX   4U

#define E_ECHAR(c_) \
    ESyntaxExpectChar       (x_lne, x_col, *x_src,  c_)
#define E_ENAME()   \
    ESyntaxExpectName       (x_lne, x_col, *x_src)
#define E_ENUMBER() \
    ESyntaxExpectNumber     (x_lne, x_col, *x_src)
#define E_UCHAR()   \
    ESyntaxUnexpectedChar   (x_lne, x_col, *x_src)
#define E_UEOF()    \
    ESyntaxUnexpectedEOF    (x_lne, x_col)
#define E_ULEN()    \
    ESyntaxUnexpectedLength (x_lne, x_col)
#define E_UNDEF()   \
    ESyntaxUndefined        (x_lne, x_col, x_pstr)
#define E_IMPROPER()\
    ESyntaxImproper         (x_lne, x_col)
#define E_ILLEGAL() \
    ESyntaxIllegal          (x_lne)
#define E_HMATH()   \
    EMath                   (x_lne)

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
inline  bool    XXX_ShouldPop(int instack, int topush);
inline  int     XXX_PushOpr(Stack *sopr, Stack *sval, int opr);

#define XEE_RET(v_) do {SDestroy(sval); SDestroy(sopr); return v_;} while(false)

int X_EvalExpression() {
    long double buf[ARG_MAX];
    int res = E_SUCCESS;
    size_t len;
    Stack *sval = SCreate();
    Stack *sopr = SCreate();
    while (*x_src && *x_src != ')') {
        switch (*x_src) {
            case '+':
            case '-':
                if (!x_lav && CIsDigD(XX_Peek())) {
                    if ((res = X_ParseNumber()))
                        XEE_RET(res);
                    SPushVal(sval, x_pval);
                    x_lav = true;
                    break;
                }
            case '*':
            case '/':
            case '^':
                if (!x_lav)
                    XEE_RET(E_UCHAR());
                if ((res = XXX_PushOpr(sopr, sval, *x_src)))
                    XEE_RET(res);
                XX_Next();
                x_lav = false;
                break;
            case '(':
                XX_Next();
                if ((res = X_EvalExpression()))
                    XEE_RET(res);
                if (!XX_ParseChar(')'))
                    XEE_RET(E_ECHAR(')'));
                SPushVal(sval, x_pval);
                XX_Next();
                x_lav = true;
                break;
            case ')':
                //Never reach here
                break;
            default:
                if (CIsDigD(*x_src)) {
                    if ((res = X_ParseNumber()))
                        XEE_RET(res);
                    SPushVal(sval, x_pval);
                    x_lav = true;
                } else if(CIsAlpha(*x_src)) {
                    //=>X_EvalFunctionCall
                    if ((res = X_ParseName()))
                        XEE_RET(res);
                    MathFunction *fun = FGetFunction(x_pstr);
                    if (!fun)
                        XEE_RET(E_UNDEF());
                    if (XX_ParseChar('(')) {
                        for (len = 0; len < ARG_MAX; ++len) {
                            if (!XX_Next())
                                XEE_RET(E_UEOF());
                            if (*x_src == ')')
                                break;
                            if ((res = X_EvalExpression()))
                                XEE_RET(res);
                            buf[len] = x_pval;
                            XX_ParseChar(',');
                        }
                        if (!XX_ParseChar(')'))
                            E_ECHAR(')');
                        XX_Next();
                        x_pval = fun(buf, len);
                    }
                    else
                        x_pval = fun(nullptr, 0);
                    if (ELast())
                        XEE_RET(E_IMPROPER());
                    if (x_pval == NAN || x_pval == -NAN)
                        XEE_RET(E_HMATH());
                    SPushVal(sval, x_pval);
                    x_lav = true;
                }
                else
                    XEE_RET(E_UCHAR());
                break;
        }
    }
    if ((res = XXX_PushOpr(sopr, sval, 0)))
        XEE_RET(res);
    if (!x_lav || SSize(sopr) != 1 || SSize(sval) != 1)
        XEE_RET(E_ILLEGAL());
    x_pval = STopVal(sval);
    XEE_RET(E_SUCCESS);
}

int X_ParseName() {
    if (!CIsAlpha(*x_src))
        return E_ENAME();
    const char *from = x_src;
    while (CIsName(XX_Read()));
    ptrdiff_t len = x_src - from;
    if (len > BUF_SIZE)
        return E_ULEN();
    strncpy(x_pstr, from, (size_t) len);
    x_pstr[len] = '\0';
    return E_SUCCESS;
}

int X_ParseNumber() {
    int res = E_SUCCESS;
    if ((res = X_ParseInteger()))
        return res;
    long double val = x_pval;
    int sca = x_sca;
    if (*x_src == '.') {
        int dig;
        long double coe = 1.0L / x_sca;
        if ((dig = XXX_ToDigit(XX_Read())) == -1)
            return E_ENUMBER();
        if (val < 0)
            while (dig != -1) {
                val -= coe * dig;
                coe /= sca;
                dig = XXX_ToDigit(XX_Read());
            }
        else
            while (dig != -1) {
                val += coe * dig;
                coe /= sca;
                dig = XXX_ToDigit(XX_Read());
            }
    }
    if (*x_src == '.' && XX_Read() != 'E' && *x_src != 'e')
        return E_ENUMBER();
    if (*x_src == 'E' || *x_src == 'e') {
        XX_Read();
        if ((res = X_ParseInteger()))
            return res;
        val *= powl(sca, x_pval);
    }
    if (CIsName(*x_src))
        return E_UCHAR();
    if (CIsWS(*x_src))
        XX_Next();
    if (!CIsStructural(*x_src))
        return E_UCHAR();
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
                x_pval = 0.0L;
                return E_SUCCESS;
            default:
                if (CIsDigO(*x_src))
                    x_sca = 8;
                else {
                    x_sca = 10;
                    x_pval = 0.0L;
                    return E_SUCCESS;
                }
                break;
        }
    }
    else
        x_sca = 10;
    if (*x_src == '0') {
        XX_Read();
        x_pval = 0.0L;
        return E_SUCCESS;
    }
    int dig = 0;
    long double val = 0.0L;
    if ((dig = XXX_ToDigit(*x_src)) == -1)
        return E_ENUMBER();
    while (dig != -1) {
        val = val * x_sca + dig;
        dig = XXX_ToDigit(XX_Read());
    }
    x_pval = neg ? -val : val;
    return E_SUCCESS;
}

bool XX_ParseChar(int chr) {
    if (CIsWS(*x_src))
        XX_Next();
    return *x_src == chr;
}

int XX_Read() {
    if (*x_src) {
        ++x_col;
        return *(++x_src);
    }
    return 0;
}

int XX_Next() {
    if (*x_src) {
        while (*(++x_src) && CIsWS(*x_src))
            ++x_col;
        return *x_src;
    }
    return 0;
}

int XX_Peek() {
    return *x_src ? *(x_src + 1) : 0;
}

int XXX_ToDigit(int chr) {
    if (CIsDigD(chr))
        chr &= 0x0f;
    else if (CIsAUpr(chr))
        chr -= 'A' - 10;
    else if (CIsALwr(chr))
        chr -= 'a' - 10;
    else
        return -1;
    if (chr < x_sca)
        return chr;
    return -1;
}

bool XXX_ShouldPop(int instack, int topush) {
    //return x_pri[topush] & 1 ? x_pri[instack] > x_pri[topush] : x_pri[instack] >= x_pri[topush];
    return x_pri[instack] >= x_pri[topush];
}

int XXX_PushOpr(Stack *sopr, Stack *sval, int opr) {
    while (!SEmpty(sopr) && XXX_ShouldPop(STopOpr(sopr), opr)) {
        if (SSize(sval) < 2)
            return E_ILLEGAL();
        long double rhs = SPopVal(sval);
        long double lhs = SPopVal(sval);
        long double res = 0.0L;
        switch (SPopOpr(sopr)) {
            case '+':
                res = lhs + rhs;
                break;
            case '-':
                res = lhs - rhs;
                break;
            case '*':
                res = lhs * rhs;
                break;
            case '/':
                res = lhs / rhs;
                break;
            case '^':
                res = powl(lhs, rhs);
                break;
            default:
                return E_ILLEGAL();
        }
        if (res == NAN || res == -NAN)
            return E_HMATH();
        SPushVal(sval, res);
    }
    SPushOpr(sopr, opr);
    return E_SUCCESS;
}

long double PEval(int line, const char *expr) {
    x_lne = line;
    x_col = 1;
    x_src = expr;
    x_lav = false;
    if (CIsWS(*x_src))
        XX_Next();
    if (!X_EvalExpression())
        ESuccess();
    return x_pval;
}

void PStartup() {
    CStartup();
    memset(x_pri, -1, sizeof(x_pri));
    x_pri[0] = 0;
    x_pri['+'] = x_pri['-'] = (1 << 1) + 0;
    x_pri['*'] = x_pri['/'] = (2 << 1) + 0;
    x_pri['^'] = (3 << 1) + 1;
}

void PCleanup() {
    CCleanup();
}

