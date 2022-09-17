#ifndef __STRING_H__
#define __STRING_H__

#include "prelude.h"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define CAP_BUFFER (1 << 12)
static char BUFFER[CAP_BUFFER];
static u32  LEN_BUFFER = 0;

typedef struct stat FileStat;

typedef struct {
    const char* buffer;
    u32         len;
} String;

typedef struct {
    void* address;
    u32   len;
} MemMap;

static const char* string_to_buffer(String string) {
    EXIT_IF(CAP_BUFFER <= (LEN_BUFFER + string.len + 1));
    char* copy = &BUFFER[LEN_BUFFER];
    memcpy(copy, string.buffer, string.len);
    LEN_BUFFER += string.len;
    BUFFER[LEN_BUFFER++] = '\0';
    return copy;
}

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

static const char* map_to_buffer(MemMap map) {
    const String string = {
        .buffer = (const char*)map.address,
        .len = map.len,
    };
    return string_to_buffer(string);
}

#endif
