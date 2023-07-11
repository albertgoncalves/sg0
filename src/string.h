#ifndef STRING_H
#define STRING_H

#include "prelude.h"

typedef struct {
    const char* buffer;
    u32         len;
} String;

typedef struct {
    void* address;
    u32   len;
} MemMap;

MemMap      string_open(const char*);
const char* string_copy(MemMap);

extern char BUFFER[CAP_BUFFER];
extern u32  LEN_BUFFER;

#endif
