#ifndef LOWCULATOR_COMMON_H_
#define LOWCULATOR_COMMON_H_

#include <stdbool.h>
#include <stddef.h>

#if !defined(__cplusplus) || __cplusplus < 201103L
#   define nullptr  NULL
#endif

#define UNUSED(x_) ((void) sizeof((ptrdiff_t) x_))

#endif

