#define LOWCULATOR_PARSE_C_

#include "parse.h"

#include "functions.h"
#include "stack.h"

#include <math.h>
#include <string.h>
#include <tgmath.h>

#include "ctype.inl.h"

size_t      x_lne;
size_t      x_col;
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
#define COL         (x_col + (size_t)(x_cur - x_src))
#define ERR(type_)  R_MAKER(LNE, COL, (type_))

//FWD
Result  X_EvalExpression();

Result  X_ParseInteger();
Result  X_ParseName();
Result  X_ParseNumber();

inline  bool    XX_ParseChar(int chr);
inline  int     XX_Peek();
inline  int     XX_Read();
inline  int     XX_Next();

inline  int     XXX_ToDigit(int chr);
inline  bool    XXX_ShouldPop(int instack, int topush);
inline  Result  XXX_PushOpr(Stack *sopr, Stack *sval, int opr);

#define XEE_RET(v_) do {SDestroy(sval); SDestroy(sopr); return v_;} while(false)

Result X_EvalExpression() {
    long double buf[ARG_MAX];
    Result res = R_SUCCE;
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
            if (!x_lav) {
                r_in0 = *x_cur;
                XEE_RET(ERR(RS_UCHR));
            }
            if ((res = XXX_PushOpr(sopr, sval, *x_cur)))
                XEE_RET(res);
            XX_Next();
            x_lav = false;
            break;
        case '(':
            XX_Next();
            if ((res = X_EvalExpression()))
                XEE_RET(res);
            if (!XX_ParseChar(')')) {
                r_in0 = *x_cur;
                r_in1 = ')';
                XEE_RET(*x_cur ? ERR(RS_ECHR) : ERR(RS_UTRM));
            }
            if (SPushLdb(sval, x_pval))
                XEE_RET(R_ITRNL);
            XX_Next();
            x_lav = true;
            break;
        default:
            if (CIsDigD(*x_cur)) {
                if ((res = X_ParseNumber()))
                    XEE_RET(res);
                if (SPushLdb(sval, x_pval))
                    XEE_RET(R_ITRNL);
                x_lav = true;
            }
            else if(CIsAlpha(*x_cur)) {
                //=>X_EvalFunctionCall
                if ((res = X_ParseName()))
                    XEE_RET(res);
                MathFunction *fun = FGetFunction(x_pstr);
                if (!fun) {
                    strcpy(r_str, x_pstr);
                    XEE_RET(ERR(RS_NDEF));
                }
                if (XX_ParseChar('(')) {
                    for (len = 0; len < ARG_MAX; ++len) {
                        if (!XX_Next())
                            XEE_RET(ERR(RS_UTRM));
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
                    if (!XX_ParseChar(')')) {
                        r_in0 = *x_cur;
                        r_in1 = ')';
                        XEE_RET(*x_cur ? ERR(RS_ECHR) : ERR(RS_UTRM));
                    }
                    XX_Next();
                    if ((res = fun(&x_pval, buf, len)))
                        XEE_RET(ERR(res));
                }
                else
                    if ((res = fun(&x_pval, nullptr, 0)))
                        XEE_RET(ERR(res));
                if (SPushLdb(sval, x_pval))
                    XEE_RET(R_ITRNL);
                x_lav = true;
            }
            else {
                r_in0 = *x_cur;
                XEE_RET(*x_cur ? ERR(RS_UTRM) : ERR(RS_UTRM));
            }
            break;
        }
    }
    if ((res = XXX_PushOpr(sopr, sval, 0)))
        XEE_RET(res);
    if (!x_lav || SSize(sopr) != 1 || SSize(sval) != 1)
        XEE_RET(ERR(RS_ILLE));
    x_pval = STopLdb(sval);
    XEE_RET(R_SUCCE);
}

Result X_ParseName() {
    if (!CIsAlpha(*x_cur)) {
        r_in0 = *x_cur;
        return *x_cur ? ERR(RS_ENAM) : ERR(RS_UTRM);
    }
    const char *from = x_cur;
    while (CIsName(XX_Read()));
    ptrdiff_t len = x_cur - from;
    if (len > BUF_SIZE)
        return ERR(RS_ULEN);
    strncpy(x_pstr, from, (size_t) len);
    x_pstr[len] = '\0';
    return R_SUCCE;
}

Result X_ParseNumber() {
    Result res = R_SUCCE;
    if ((res = X_ParseInteger()))
        return res;
    long double val = x_pval;
    int sca = x_sca;
    if (*x_cur == '.') {
        int dig;
        long double coe = 1.0L / x_sca;
        if ((dig = XXX_ToDigit(XX_Read())) == -1) {
            r_in0 = *x_cur;
            return *x_cur ? ERR(RS_ENUM) : ERR(RS_UTRM);
        }
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
    if (*x_cur == '.' && XX_Read() != 'E' && *x_cur != 'e') {
        r_in0 = *x_cur;
        return ERR(RS_ENUM);
    }
    if (*x_cur == 'E' || *x_cur == 'e') {
        XX_Read();
        if ((res = X_ParseInteger()))
            return res;
        val *= powl(sca, x_pval);
    }
    if (CIsName(*x_cur)) {
        r_in0 = *x_cur;
        return ERR(RS_UCHR);
    }
    if (CIsWS(*x_cur))
        XX_Next();
    if (!CIsStructural(*x_cur)) {
        r_in0 = *x_cur;
        return *x_cur ? ERR(RS_UTRM) : ERR(RS_UCHR);
    }
    x_pval = val;
    return R_SUCCE;
}

Result X_ParseInteger() {
    bool neg = *x_cur == '-';
    if (neg || *x_cur == '+')
        XX_Read();
    if (!*x_cur)
        return ERR(RS_UTRM);
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
            return R_SUCCE;
        default:
            if (CIsDigO(*x_cur))
                x_sca = 8;
            else {
                x_sca = 10;
                x_pval = 0.0L;
                return R_SUCCE;
            }
            break;
        }
    }
    else
        x_sca = 10;
    if (*x_cur == '0') {
        XX_Read();
        x_pval = 0.0L;
        return R_SUCCE;
    }
    int dig = 0;
    long double val = 0.0L;
    if ((dig = XXX_ToDigit(*x_cur)) == -1) {
        r_in0 = *x_cur;
        return *x_cur ? ERR(RS_UCHR) : ERR(RS_UTRM);
    }
    while (dig != -1) {
        val = val * x_sca + dig;
        dig = XXX_ToDigit(XX_Read());
    }
    x_pval = neg ? -val : val;
    return R_SUCCE;
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

Result XXX_PushOpr(Stack *sopr, Stack *sval, int opr) {
    Result res = R_SUCCE;
    while (!SEmpty(sopr) && XXX_ShouldPop(STopInt(sopr), opr)) {
        if (SSize(sval) < 2)
            return RS_ILLE;
        long double rhs = SPopLdb(sval);
        long double lhs = SPopLdb(sval);
        long double ans = 0.0L;
        switch (SPopInt(sopr)) {
        case '+':
            ans = lhs + rhs;
            break;
        case '-':
            ans = lhs - rhs;
            break;
        case '*':
            ans = lhs * rhs;
            break;
        case '/':
            ans = lhs / rhs;
            break;
        case '%':
            ans = fmodl(lhs, rhs);
            break;
        case '^':
            ans = powl(lhs, rhs);
            break;
        default:
            return R_ITRNL;
        }
        if ((res = RMathType(ans)))
            return ERR(res);
        if (SPushLdb(sval, ans))
            return R_ITRNL;
    }
    if (SPushInt(sopr, opr))
        return R_ITRNL;
    return R_SUCCE;
}

Result PEval(long double *ans, size_t line, size_t column, const char *expr) {
    x_lne = line;
    x_col = column;
    x_src = expr;
    x_cur = x_src;
    x_lav = false;
    if (CIsWS(*x_cur))
        XX_Next();
    Result res = X_EvalExpression();
    *ans = x_pval;
    return res;
}

Result PStartup() {
    INIT_CTYPE;
    memset(x_pri, -1, sizeof(x_pri));
    x_pri[0] = 0;
    x_pri['+'] = x_pri['-'] = (1 << 1) + 0;
    x_pri['*'] = x_pri['/'] = x_pri['%'] = (2 << 1) + 0;
    x_pri['^'] = (3 << 1) + 1;
    return R_SUCCE;
}

void PCleanup() {
}

