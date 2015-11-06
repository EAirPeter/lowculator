#ifndef LOWCULATOR_RESULT_H_
#define LOWCULATOR_RESULT_H_

#include "common.h"

#define R_BUFFSIZE  256U

typedef uint32_t    Result;
extern  int         r_in0;
extern  int         r_in1;
extern  char        r_str[R_BUFFSIZE];

/*              FEDCBA9876543210FEDCBA9876543210
 * RESULTCODE:  LLLLLLLLLLLLLLLLCCCCCCCCCCEEEEEE
 * L (16bits):  line number
 * C (10bits):  column number
 * E ( 6bits):  result type
 * 
 * If an operation success, it shall return R_SUCCE.
 * If the return value is not R_SUCCE, an error shall ocurred.
 */

#define R_LNE(code_)    ( ((Result) (code_)) >> 16U)
#define R_COL(code_)    ((((Result) (code_)) >>  6U) & 0x3FFU)
#define R_TYP(code_)    ( ((Result) (code_))         & 0x03FU)
#define R_MAKER(lne_, col_, typ_)   (   \
    (((Result) (lne_)) << 16U)      |   \
    (((Result) (col_)) <<  6U)      |   \
     ((Result) (typ_))              )

#define R_SUCCE 0x0U

#define R_SYNTX 0x1U
#define R_MATHC 0x2U
#define R_IOERR 0x3U

#define R_ITRNL 0xFFFFFFFFU;

#define XR_DEFSUB(base_, code_ )    ((base_) | ((code_) << 2U))
#define XR_SSUB(code_)              XR_DEFSUB(R_SYNTX, (code_))
#define XR_MSUB(code_)              XR_DEFSUB(R_MATHC, (code_))
#define XR_ISUB(code_)              XR_DEFSUB(R_IOERR, (code_))

#define RS_RESV XR_SSUB(0x0U)
#define RS_ECHR XR_SSUB(0x1U)
#define RS_ENAM XR_SSUB(0x2U)
#define RS_ENUM XR_SSUB(0x3U)
#define RS_UCHR XR_SSUB(0x4U)
#define RS_ULEN XR_SSUB(0x5U)
#define RS_UTRM XR_SSUB(0x6U)
#define RS_NDEF XR_SSUB(0x7U)
#define RS_IMPR XR_SSUB(0x8U)
#define RS_ILLE XR_SSUB(0x9U)
#define RS_ILLD XR_SSUB(0xAU)
#define RS_ILLA XR_SSUB(0xBU)

#define RM_RESV XR_MSUB(0x0U)
#define RM_DOMN XR_MSUB(0x1U)
#define RM_RANG XR_MSUB(0x2U)

#define RI_RESV XR_ISUB(0x0U)
#define RI_FOPN XR_ISUB(0x1U)
#define RI_FCLS XR_ISUB(0x2U)
#define RI_OPIN XR_ISUB(0x3U)
#define RI_OPOU XR_ISUB(0x4U)
#define RI_EOFR XR_ISUB(0x5U)

#define RI_ITNL XR_ISUB(0xFU)

Result      RStartup();

void        RCleanup();

Result      RContext(const char *ctx);

Result      RMathType(long double val);

const char *RMessage(Result code);

#endif

