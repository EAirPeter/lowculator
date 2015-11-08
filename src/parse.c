#define LOWCULATOR_PARSE_C_

#include "parse.h"

#include "functions.h"
#include "stack.h"

#include <math.h>
#include <string.h>
#include <tgmath.h>

#include "ctype.inl.h"

static size_t       x_lne = 0;
static size_t       x_col = 0;
static int          x_sca = 0;
static const char  *x_src = nullptr;
static const char  *x_cur = nullptr;
static bool         x_lav = false;

#define BUF_SIZE    256U

static long double  x_pval = 0.0L;
static char         x_pstr[BUF_SIZE] = {};

typedef enum X_Operator_ X_Operator;

enum X_Operator_ {
    OPR_TRM = 0,
    BIN_ADD,
    BIN_SUB,
    BIN_MUL,
    BIN_DIV,
    BIN_MOD,
    BIN_PWR,
    UNA_PLS,
    UNA_MNS,
    OPR_COUNT,
};

#define OPR_PRI(o_) ( ((uint32_t) (o_)) >> 2U)
#define OPR_OPE(o_) ((((uint32_t) (o_)) >> 1U) & 1U)
#define OPR_ASS(o_) ( ((uint32_t) (o_))        & 1U)
#define MAKEOPR(p_, o_, a_)     (   \
    (((uint32_t) (p_)) << 2U)   |   \
    (((uint32_t) (o_)) << 1U)   |   \
     ((uint32_t) (a_))          )

static uint32_t     x_ops[OPR_COUNT] = {};

#define ARG_MAX     4U

#define LNE         (x_lne)
#define COL         (x_col + (size_t)(x_cur - x_src))
#define ERR(type_)  R_MAKER(LNE, COL, (type_))

//FWD
static Result   X_EvalExpression();

static Result   X_ParseInteger();
static Result   X_ParseName();
static Result   X_ParseNumber();

static inline bool      XX_ParseChar(int chr);
static inline int       XX_Peek();
static inline int       XX_Read();
static inline int       XX_Next();

static inline int       XXX_ToDigit(int chr);
static inline Result    XXX_PushOpr(Stack *sopr, Stack *sval, int opr);

#define XEE_RET(v_) do {SDestroy(sval); SDestroy(sopr); return v_;} while(false)

#define XEE_PBU(bin_, una_)                         \
    if (x_lav) {                                    \
        if ((res = XXX_PushOpr(sopr, sval, bin_)))  \
            XEE_RET(res);                           \
    }                                               \
    else {                                          \
        if ((res = XXX_PushOpr(sopr, sval, una_)))  \
            XEE_RET(res);                           \
    }                                               \
    XX_Next();                                      \
    x_lav = false

#define XEE_PBE(bin_)                               \
    if (x_lav) {                                    \
        if ((res = XXX_PushOpr(sopr, sval, bin_)))  \
            XEE_RET(res);                           \
    }                                               \
    else {                                          \
        r_in0 = *x_cur;                             \
        XEE_RET(ERR(RS_UCHR));                      \
    }                                               \
    XX_Next();                                      \
    x_lav = false

static Result X_EvalExpression() {
    long double buf[ARG_MAX];
    Result res = R_SUCCE;
    size_t len;
    Stack *sval = SCreate();
    Stack *sopr = SCreate();
    while (*x_cur && *x_cur != ',' && *x_cur != ')') {
        switch (*x_cur) {
        case '+':
            XEE_PBU(BIN_ADD, UNA_PLS);
            break;
        case '-':
            XEE_PBU(BIN_SUB, UNA_MNS);
            break;
        case '*':
            XEE_PBE(BIN_MUL);
            break;
        case '/':
            XEE_PBE(BIN_DIV);
            break;
        case '%':
            XEE_PBE(BIN_MOD);
            break;
        case '^':
            XEE_PBE(BIN_PWR);
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
                XEE_RET(*x_cur ? ERR(RS_UCHR) : ERR(RS_UTRM));
            }
            break;
        }
    }
    if ((res = XXX_PushOpr(sopr, sval, 0)))
        XEE_RET(res);
    if (!x_lav || SSize(sopr) != 1 || SSize(sval) != 1)
        XEE_RET(ERR(RS_ILLE));
    x_pval = *STopLdb(sval);
    XEE_RET(R_SUCCE);
}

static Result X_ParseName() {
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

static Result X_ParseNumber() {
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
    if (CIsExpSep(*x_cur)) {
        XX_Read();
        bool neg = *x_cur == '-';
        if (neg || *x_cur == '+')
            XX_Read();
        if ((res = X_ParseInteger()))
            return res;
        val *= powl(sca, neg ? -x_pval : x_pval);
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

static Result X_ParseInteger() {
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
    x_pval = val;
    return R_SUCCE;
}

static bool XX_ParseChar(int chr) {
    if (CIsWS(*x_cur))
        XX_Next();
    return *x_cur == chr;
}

static int XX_Read() {
    return *x_cur ? *(++x_cur) : 0;
}

static int XX_Next() {
    if (*x_cur) {
        while (*(++x_cur) && CIsWS(*x_cur));
        return *x_cur;
    }
    return 0;
}

static int XX_Peek() {
    return *x_cur ? *(x_cur + 1) : 0;
}

static int XXX_ToDigit(int chr) {
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

static inline bool XXX_ShouldPop(int instack, int topush) {
    return OPR_ASS(x_ops[topush]) ?
        OPR_PRI(x_ops[instack]) > OPR_PRI(x_ops[topush]) :
        OPR_PRI(x_ops[instack]) >= OPR_PRI(x_ops[topush]);
    //return x_ops[instack] >= x_ops[topush];
}

#define XXXPO_EVOB(e_)                                  \
    if (SSize(sval) < 2)    return ERR(RS_ILLE);        \
    tmp = SPopLdb(sval); ans = STopLdb(sval); e_; break
#define XXXPO_EVOU(e_)                                  \
    if (SEmpty(sval))       return ERR(RS_ILLE);        \
    ans = STopLdb(sval); e_; break

static Result XXX_PushOpr(Stack *sopr, Stack *sval, int opr) {
    Result res = R_SUCCE;
    long double tmp;
    long double *ans;
    while (!SEmpty(sopr) && XXX_ShouldPop(*STopInt(sopr), opr)) {
        switch (SPopInt(sopr)) {
        case OPR_TRM:
            return R_ITRNL;
        case BIN_ADD:
            XXXPO_EVOB(*ans += tmp);
        case BIN_SUB:
            XXXPO_EVOB(*ans -= tmp);
        case BIN_MUL:
            XXXPO_EVOB(*ans *= tmp);
        case BIN_DIV:
            XXXPO_EVOB(*ans /= tmp);
        case BIN_MOD:
            XXXPO_EVOB(*ans = fmodl(*ans, tmp));
        case BIN_PWR:
            XXXPO_EVOB(*ans = powl(*ans, tmp));
        case UNA_PLS:
            XXXPO_EVOU();
        case UNA_MNS:
            XXXPO_EVOU(*ans = -*ans);
        default:
            return R_ITRNL;
        }
        if ((res = RMathType(*ans)))
            return ERR(res);
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
    x_ops[OPR_TRM] = MAKEOPR(0, 0, 0);
    x_ops[BIN_ADD] = x_ops[BIN_SUB] = MAKEOPR(1, 2, 0);
    x_ops[BIN_MUL] = x_ops[BIN_DIV] = x_ops[BIN_MOD] = MAKEOPR(2, 2, 0);
    x_ops[UNA_PLS] = x_ops[UNA_MNS] = MAKEOPR(3, 1, 1);
    x_ops[BIN_PWR] = MAKEOPR(3, 2, 1);
    return R_SUCCE;
}

void PCleanup() {
}

