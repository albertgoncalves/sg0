#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;

typedef struct stat     FileStat;
typedef struct timespec Time;

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

typedef struct {
    void* address;
    u32   len;
} MemMap;

typedef struct {
    const char* buffer;
    u32         len;
} String;

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    u8 x, y, z;
} Vec3u;

#define ATTRIBUTE(x) __attribute__((x))

#define OK    0
#define ERROR 1

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

static MemMap path_to_map(const char* path) {
    EXIT_IF(!path);
    const i32 file = open(path, O_RDONLY);
    EXIT_IF(file < 0);
    FileStat stat;
    EXIT_IF(fstat(file, &stat) < 0);
    const MemMap map = {
        .address =
            mmap(NULL, (u32)stat.st_size, PROT_READ, MAP_SHARED, file, 0),
        .len = (u32)stat.st_size,
    };
    EXIT_IF(map.address == MAP_FAILED);
    close(file);
    return map;
}

#define CAP_BUFFER (1 << 13)
static char BUFFER[CAP_BUFFER];
static u32  LEN_BUFFER = 0;

static const char* string_to_buffer(String string) {
    EXIT_IF(CAP_BUFFER <= (LEN_BUFFER + string.len + 1));
    char* copy = &BUFFER[LEN_BUFFER];
    memcpy(copy, string.buffer, string.len);
    LEN_BUFFER += string.len;
    BUFFER[LEN_BUFFER++] = '\0';
    return copy;
}

static const char* map_to_buffer(MemMap map) {
    const String string = {
        .buffer = (const char*)map.address,
        .len = map.len,
    };
    return string_to_buffer(string);
}

#define MILLI_PER_SECOND 1000llu
#define MICRO_PER_SECOND 1000000llu
#define NANO_PER_SECOND  1000000000llu
#define NANO_PER_MILLI   (NANO_PER_SECOND / MILLI_PER_SECOND)
#define NANO_PER_MICRO   (NANO_PER_SECOND / MICRO_PER_SECOND)

static u64 now_ns(void) {
    Time time;
    EXIT_IF(clock_gettime(CLOCK_MONOTONIC, &time));
    return ((u64)time.tv_sec * NANO_PER_SECOND) + (u64)time.tv_nsec;
}

#endif
