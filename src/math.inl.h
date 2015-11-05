#include "error.h"

#include <math.h>
#include <tgmath.h>

#define PARS_1              argv[0]
#define PARS_2              PARS_1, argv[1]
#define PARS_3              PARS_2, argv[2]
#define PARS_4              PARS_3, argv[3]

#define FUNNAME(name_)      CONCAT(X_WRAPPED, name_)

#define DEFFUN(name_, mapped_, pars_)                                   \
    long double FUNNAME(name_)(long double *argv, size_t argc) {        \
        if (argc != (pars_)) {                                          \
            ESet(E_SYNTAX);                                             \
            return 0.0L;                                                \
        }                                                               \
        ESet(E_SUCCESS);                                                \
        return mapped_(CONCAT(PARS_, pars_));                           \
        UNUSED(argv);                                                   \
    }

#define ADDFUN(name_)       FAddFunction(#name_, FUNNAME(name_))

DEFFUN(abs      , fabsl     , 1)
DEFFUN(mod      , fmodl     , 2)
DEFFUN(ma       , fmal      , 3)
DEFFUN(remainder, remainderl, 2)
DEFFUN(max      , fmaxl     , 2)
DEFFUN(min      , fminl     , 2)
DEFFUN(dim      , fdiml     , 2)
DEFFUN(exp      , expl      , 1)
DEFFUN(exp2     , exp2l     , 1)
DEFFUN(expm1    , expm1l    , 1)
DEFFUN(ln       , logl      , 1)
DEFFUN(log2     , log2l     , 1)
DEFFUN(lg       , log10l    , 1)
DEFFUN(ln1p     , log1pl    , 1)
DEFFUN(pow      , powl      , 2)
DEFFUN(sqrt     , sqrtl     , 1)
DEFFUN(cbrt     , cbrtl     , 1)
DEFFUN(hypot    , hypotl    , 2)
DEFFUN(sin      , sinl      , 1)
DEFFUN(cos      , cosl      , 1)
DEFFUN(tan      , tanl      , 1)
DEFFUN(asin     , asinl     , 1)
DEFFUN(acos     , acosl     , 1)
DEFFUN(atan     , atanl     , 1)
DEFFUN(atan2    , atan2l    , 2)
DEFFUN(sinh     , sinhl     , 1)
DEFFUN(cosh     , coshl     , 1)
DEFFUN(tanh     , tanhl     , 1)
DEFFUN(asinh    , asinhl    , 1)
DEFFUN(acosh    , acoshl    , 1)
DEFFUN(atanh    , atanhl    , 1)
DEFFUN(erf      , erfl      , 1)
DEFFUN(erfc     , erfcl     , 1)
DEFFUN(tgamma   , tgammal   , 1)
DEFFUN(lgamma   , lgammal   , 1)
DEFFUN(ceil     , ceill     , 1)
DEFFUN(floor    , floorl    , 1)
DEFFUN(trunc    , truncl    , 1)
DEFFUN(round    , roundl    , 1)

#define ADD_WRAPPED do {\
    ADDFUN(abs);        \
    ADDFUN(mod);        \
    ADDFUN(ma);         \
    ADDFUN(remainder);  \
    ADDFUN(max);        \
    ADDFUN(min);        \
    ADDFUN(dim);        \
    ADDFUN(exp);        \
    ADDFUN(exp2);       \
    ADDFUN(expm1);      \
    ADDFUN(ln);         \
    ADDFUN(log2);       \
    ADDFUN(lg);         \
    ADDFUN(ln1p);       \
    ADDFUN(pow);        \
    ADDFUN(sqrt);       \
    ADDFUN(cbrt);       \
    ADDFUN(hypot);      \
    ADDFUN(sin);        \
    ADDFUN(cos);        \
    ADDFUN(tan);        \
    ADDFUN(asin);       \
    ADDFUN(acos);       \
    ADDFUN(atan);       \
    ADDFUN(atan2);      \
    ADDFUN(sinh);       \
    ADDFUN(cosh);       \
    ADDFUN(tanh);       \
    ADDFUN(asinh);      \
    ADDFUN(acosh);      \
    ADDFUN(atanh);      \
    ADDFUN(erf);        \
    ADDFUN(erfc);       \
    ADDFUN(tgamma);     \
    ADDFUN(lgamma);     \
    ADDFUN(ceil);       \
    ADDFUN(floor);      \
    ADDFUN(trunc);      \
    ADDFUN(round);      \
} while (false)

#define COVNAME(name_)  CONCAT(x_CON, name_)
#define COFNAME(name_)  CONCAT(X_CONST, name_)

#define DEFCON(name_)                                                   \
    long double COVNAME(name_);                                         \
    long double COFNAME(name_)(long double *argv, size_t argc) {        \
        if (argc) {                                                     \
            ESet(E_SYNTAX);                                             \
            return 0.0L;                                                \
        }                                                               \
        ESet(E_SUCCESS);                                                \
        return COVNAME(name_);                                          \
        UNUSED(argv);                                                   \
    }

#define ADDCON(name_, expr_) do {           \
    COVNAME(name_) = (expr_);               \
    FAddFunction(#name_, COFNAME(name_));   \
} while (false)

DEFCON(pi)
DEFCON(e)

#define ADD_CONST do {          \
    ADDCON(pi, acosl(-1.0L));   \
    ADDCON(e, expl(1.0L));      \
} while (false)

#define ADD_MATH do {   \
    ADD_WRAPPED;        \
    ADD_CONST;          \
} while (false)

