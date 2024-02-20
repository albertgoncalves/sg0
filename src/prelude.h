#ifndef PRELUDE_H
#define PRELUDE_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define STATIC_ASSERT(condition) _Static_assert(condition, "!(" #condition ")")

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;

STATIC_ASSERT(sizeof(u64) == sizeof(f64));
STATIC_ASSERT(sizeof(u64) == sizeof(void*));

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

#define ATTRIBUTE(x) __attribute__((x))

#define OK    0
#define ERROR 1

#define VSYNC 1

#define EXIT()                                                       \
    do {                                                             \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        _exit(ERROR);                                                \
    } while (FALSE)

#define EXIT_WITH(x)                                                         \
    do {                                                                     \
        fprintf(stderr, "%s:%s:%d `%s`\n", __FILE__, __func__, __LINE__, x); \
        _exit(ERROR);                                                        \
    } while (FALSE)

#define EXIT_IF(condition)         \
    do {                           \
        if (condition) {           \
            EXIT_WITH(#condition); \
        }                          \
    } while (FALSE)

#define CAP_CUBES (1 << 6)
#define CAP_LINES (1 << 4)

#define CAP_BUFFER (1 << 12)

#define CAP_PLAYER  1
#define CAP_ENEMIES (1 << 4)

STATIC_ASSERT(CAP_ENEMIES <= CAP_LINES);

#define CAP_WORLD (CAP_CUBES - (CAP_PLAYER + CAP_ENEMIES))

STATIC_ASSERT(CAP_WORLD < CAP_CUBES);

#define CAP_SPRITES (1 << 5)

STATIC_ASSERT((CAP_PLAYER + CAP_ENEMIES) <= CAP_SPRITES);

#endif
