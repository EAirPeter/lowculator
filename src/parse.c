#include "parse.h"

#include "error.h"
#include "functions.h"
#include "stack.h"

#include <math.h>
#include <string.h>
#include <tgmath.h>

#include "ctype.inl.h"

int         x_lne;
int         x_col;
int         x_sca;
const char *x_src;
const char *x_cur;
bool        x_lav;

#define BUF_SIZE    256U

long double x_pval;
char        x_pstr[BUF_SIZE];

int         x_pri[256];

#define ARG_MAX     4U

#define LNE         (x_lne)
#define COL         (x_col + (int)(x_cur - x_src))

#define E_ECHAR(c_) \
    ESyntaxExpectChar       (LNE, COL, *x_cur,  c_)
#define E_ENAME()   \
    ESyntaxExpectName       (LNE, COL, *x_cur)
#define E_ENUMBER() \
    ESyntaxExpectNumber     (LNE, COL, *x_cur)
#define E_UCHAR()   \
    ESyntaxUnexpectedChar   (LNE, COL, *x_cur)
#define E_UTERM()   \
    ESyntaxUnexpectedTerm   (LNE, COL)
#define E_ULEN()    \
    ESyntaxUnexpectedLength (LNE, COL)
#define E_UNDEF()   \
    ESyntaxUndefined        (LNE, COL, x_pstr)
#define E_IMPROPER()\
    ESyntaxImproper         (LNE, COL)
#define E_ILLEGAL() \
    ESyntaxIllegal          (LNE)
#define E_HMATH()   \
    EMath                   (LNE)

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
    while (*x_cur && *x_cur != ',' && *x_cur != ')') {
        switch (*x_cur) {
            case '+':
            case '-':
                if (!x_lav && CIsDigD(XX_Peek())) {
                    if ((res = X_ParseNumber()))
                        XEE_RET(res);
                    SPushLdb(sval, x_pval);
                    x_lav = true;
                    break;
                }
            case '*':
            case '/':
            case '%':
            case '^':
                if (!x_lav)
                    XEE_RET(E_UCHAR());
                if ((res = XXX_PushOpr(sopr, sval, *x_cur)))
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
                SPushLdb(sval, x_pval);
                XX_Next();
                x_lav = true;
                break;
            default:
                if (CIsDigD(*x_cur)) {
                    if ((res = X_ParseNumber()))
                        XEE_RET(res);
                    SPushLdb(sval, x_pval);
                    x_lav = true;
                } else if(CIsAlpha(*x_cur)) {
                    //=>X_EvalFunctionCall
                    if ((res = X_ParseName()))
                        XEE_RET(res);
                    MathFunction *fun = FGetFunction(x_pstr);
                    if (!fun)
                        XEE_RET(E_UNDEF());
                    if (XX_ParseChar('(')) {
                        for (len = 0; len < ARG_MAX; ++len) {
                            if (!XX_Next())
                                XEE_RET(E_UTERM());
                            if (*x_cur == ')')
                                break;
                            if ((res = X_EvalExpression()))
                                XEE_RET(res);
                            buf[len] = x_pval;
                            if (!XX_ParseChar(',')) {
                                ++len;
                                break;
                            }
                        }
                        if (!XX_ParseChar(')'))
                            E_ECHAR(')');
                        XX_Next();
                        x_pval = fun(buf, len);
                    }
                    else
                        x_pval = fun(nullptr, 0);
                    if (errno)
                        XEE_RET(E_IMPROPER());
                    if (!isfinite(x_pval))
                        XEE_RET(E_HMATH());
                    SPushLdb(sval, x_pval);
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
    x_pval = STopLdb(sval);
    XEE_RET(E_SUCCESS);
}

int X_ParseName() {
    if (!CIsAlpha(*x_cur))
        return E_ENAME();
    const char *from = x_cur;
    while (CIsName(XX_Read()));
    ptrdiff_t len = x_cur - from;
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
    if (*x_cur == '.') {
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
    if (*x_cur == '.' && XX_Read() != 'E' && *x_cur != 'e')
        return E_ENUMBER();
    if (*x_cur == 'E' || *x_cur == 'e') {
        XX_Read();
        if ((res = X_ParseInteger()))
            return res;
        val *= powl(sca, x_pval);
    }
    if (CIsName(*x_cur))
        return E_UCHAR();
    if (CIsWS(*x_cur))
        XX_Next();
    if (!CIsStructural(*x_cur))
        return E_UCHAR();
    x_pval = val;
    return E_SUCCESS;
}

int X_ParseInteger() {
    bool neg = *x_cur == '-';
    if (neg || *x_cur == '+')
        XX_Read();
    if (!*x_cur)
        return E_UTERM();
    if (*x_cur == '0') {
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
                if (CIsDigO(*x_cur))
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
    if (*x_cur == '0') {
        XX_Read();
        x_pval = 0.0L;
        return E_SUCCESS;
    }
    int dig = 0;
    long double val = 0.0L;
    if ((dig = XXX_ToDigit(*x_cur)) == -1)
        return E_ENUMBER();
    while (dig != -1) {
        val = val * x_sca + dig;
        dig = XXX_ToDigit(XX_Read());
    }
    x_pval = neg ? -val : val;
    return E_SUCCESS;
}

bool XX_ParseChar(int chr) {
    if (CIsWS(*x_cur))
        XX_Next();
    return *x_cur == chr;
}

int XX_Read() {
    return *x_cur ? *(++x_cur) : 0;
}

int XX_Next() {
    if (*x_cur) {
        while (*(++x_cur) && CIsWS(*x_cur));
        return *x_cur;
    }
    return 0;
}

int XX_Peek() {
    return *x_cur ? *(x_cur + 1) : 0;
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
    while (!SEmpty(sopr) && XXX_ShouldPop(STopInt(sopr), opr)) {
        if (SSize(sval) < 2)
            return E_ILLEGAL();
        long double rhs = SPopLdb(sval);
        long double lhs = SPopLdb(sval);
        long double res = 0.0L;
        switch (SPopInt(sopr)) {
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
            case '%':
                res = fmodl(lhs, rhs);
                break;
            case '^':
                res = powl(lhs, rhs);
                break;
            default:
                return E_ILLEGAL();
        }
        if (!isfinite(res))
            return E_HMATH();
        SPushLdb(sval, res);
    }
    SPushInt(sopr, opr);
    return E_SUCCESS;
}

long double PEval(int line, int column, const char *expr) {
    x_lne = line;
    x_col = column;
    x_src = expr;
    x_cur = x_src;
    x_lav = false;
    if (CIsWS(*x_cur))
        XX_Next();
    X_EvalExpression();
    return x_pval;
}

void PStartup() {
    INIT_CTYPE;
    memset(x_pri, -1, sizeof(x_pri));
    x_pri[0] = 0;
    x_pri['+'] = x_pri['-'] = (1 << 1) + 0;
    x_pri['*'] = x_pri['/'] = x_pri['%'] = (2 << 1) + 0;
    x_pri['^'] = (3 << 1) + 1;
}

void PCleanup() {
}

