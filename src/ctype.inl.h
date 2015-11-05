#include <stdint.h>

#define C_DIG   0x00000001 
#define C_DIB   0x00000002
#define C_DIO   0x00000004
#define C_DID   0x00000008
#define C_DIH   0x00000010
#define C_ALP   0x00000020
#define C_UPR   0x00000040
#define C_LWR   0x00000080
#define C_OPR   0x00000100
#define C_STR   0x00000200
#define C_WSP   0x00000400
#define C_NAM   0x00000800

uint32_t        cx_type[256];

static inline bool CIsAlpha(int chr) {
    return cx_type[chr] & C_ALP;
}

static inline bool CIsAUpr(int chr) {
    return cx_type[chr] & C_UPR;
}

static inline bool CIsALwr(int chr) {
    return cx_type[chr] & C_LWR;
}

static inline bool CIsDigB(int chr) {
    return cx_type[chr] & C_DIB;
}

static inline bool CIsDigO(int chr) {
    return cx_type[chr] & C_DIO;
}

static inline bool CIsDigD(int chr) {
    return cx_type[chr] & C_DID;
}

static inline bool CIsDigH(int chr) {
    return cx_type[chr] & C_DIH;
}

static inline bool CIsName(int chr) {
    return cx_type[chr] & C_NAM;
}

static inline bool CIsStructural(int chr) {
    return cx_type[chr] & C_STR;
}

static inline bool CIsWS(int chr) {
    return cx_type[chr] & C_WSP;
}

#define INIT_CTYPE do {                                 \
    memset(cx_type, 0, sizeof(cx_type));                \
    for (int i = '0'; i <= '1'; ++i)                    \
        cx_type[i] |= C_DIB;                            \
    for (int i = '0'; i <= '7'; ++i)                    \
        cx_type[i] |= C_DIO;                            \
    for (int i = 'A'; i <= 'F'; ++i)                    \
        cx_type[i] |= C_DIG | C_DIH;                    \
    for (int i = 'a'; i <= 'f'; ++i)                    \
        cx_type[i] |= C_DIG | C_DIH;                    \
    for (int i = '0'; i <= '9'; ++i)                    \
        cx_type[i] |= C_NAM | C_DIG | C_DID | C_DIH;    \
    for (int i = 'A'; i <= 'Z'; ++i)                    \
        cx_type[i] |= C_NAM | C_ALP | C_UPR;            \
    for (int i = 'a'; i <= 'z'; ++i)                    \
        cx_type[i] |= C_NAM | C_ALP | C_LWR;            \
    cx_type['_'] |= C_NAM;                              \
    int oprs[] = {'+', '-', '*', '/', '%', '^', -1};    \
    for (int *p = oprs; *p != -1; ++p)                  \
        cx_type[*p] |= C_STR | C_OPR;                   \
    int strs[] = {'\0', ',', '(', ')', -1};             \
    for (int *p = strs; *p != -1; ++p)                  \
        cx_type[*p] |= C_STR;                           \
    int wsps[] = {' ', '\t', -1};                       \
    for (int *p = wsps; *p != -1; ++p)                  \
        cx_type[*p] |= C_WSP;                           \
} while (false)
