#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

#define ATTRIBUTE(x) __attribute__((x))

#define OK    0
#define ERROR 1

#define EPSILON 0.001f

#define EXIT()                                              \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        _exit(ERROR);                                       \
    } while (FALSE)

#define EXIT_WITH(x)                                                \
    do {                                                            \
        printf("%s:%s:%d `%s`\n", __FILE__, __func__, __LINE__, x); \
        _exit(ERROR);                                               \
    } while (FALSE)

#define EXIT_IF(condition)         \
    do {                           \
        if (condition) {           \
            EXIT_WITH(#condition); \
        }                          \
    } while (FALSE)

#define NEAR_ZERO(x) ((-EPSILON < x) && (x < EPSILON))

#endif
