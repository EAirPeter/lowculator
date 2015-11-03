#ifndef LOWCULATOR_LCCTYPE_INL_
#define LOWCULATOR_LCCTYPE_INL_

#include <stdint.h>
#include <string.h>

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

inline  bool    CIsAlpha(int chr) {
    return cx_type[chr] & C_ALP;
}

inline  bool    CIsAUpr(int chr) {
    return cx_type[chr] & C_UPR;
}

inline  bool    CIsALwr(int chr) {
    return cx_type[chr] & C_LWR;
}

inline  bool    CIsDigB(int chr) {
    return cx_type[chr] & C_DIB;
}

inline  bool    CIsDigO(int chr) {
    return cx_type[chr] & C_DIO;
}

inline  bool    CIsDigD(int chr) {
    return cx_type[chr] & C_DID;
}

inline  bool    CIsDigH(int chr) {
    return cx_type[chr] & C_DIH;
}

inline  bool    CIsName(int chr) {
    return cx_type[chr] & C_NAM;
}

inline  bool    CIsStructural(int chr) {
    return cx_type[chr] & C_STR;
}

inline  bool    CIsWS(int chr) {
    return cx_type[chr] & C_WSP;
}

void CInit() {
    memset(cx_type, 0, sizeof(cx_type));
    for (int i = '0'; i <= '1'; ++i)
        cx_type[i] |= C_DIB;
    for (int i = '0'; i <= '7'; ++i)
        cx_type[i] |= C_DIO;
    for (int i = 'A'; i <= 'F'; ++i)
        cx_type[i] |= C_DIG | C_DIH;
    for (int i = 'a'; i <= 'f'; ++i)
        cx_type[i] |= C_DIG | C_DIH;
    for (int i = '0'; i <= '9'; ++i)
        cx_type[i] |= C_NAM | C_DIG | C_DID | C_DIH;
    for (int i = 'A'; i <= 'Z'; ++i)
        cx_type[i] |= C_NAM | C_ALP | C_UPR;
    for (int i = 'a'; i <= 'z'; ++i)
        cx_type[i] |= C_NAM | C_ALP | C_LWR;
    cx_type['_'] |= C_NAM;
    int oprs[5] = {'+', '-', '*', '/', '^'};
    for (int i = 0; i < 5; ++i)
        cx_type[oprs[i]] |= C_STR | C_OPR;
    int strs[2] = {'(', ')'};
    for (int i = 0; i < 2; ++i)
        cx_type[strs[i]] |= C_STR;
    int wsps[3] = {' ', '\t'};
    for (int i = 0; i < 2; ++i)
        cx_type[wsps[i]] |= C_WSP;
}

#endif

