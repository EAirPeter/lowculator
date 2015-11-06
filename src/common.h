#ifndef LOWCULATOR_COMMON_H_
#define LOWCULATOR_COMMON_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__cplusplus) || __cplusplus < 201103L
#   define nullptr  NULL
#endif

#define UNUSED(x_) ((void) sizeof((ptrdiff_t) x_))

#define CONCAT_(a_, b_)     a_ ## b_
#define CONCAT(a_, b_)      CONCAT_(a_, b_)

#endif

