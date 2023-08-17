#include "string.h"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef struct stat FileStat;

MemMap string_open(const char* path) {
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

const char* string_copy(MemMap map) {
    const String string = {
        .buffer = (const char*)map.address,
        .len = map.len,
    };
    EXIT_IF(CAP_BUFFER < (LEN_BUFFER + string.len + 1));
    char* copy = &BUFFER[LEN_BUFFER];
    memcpy(copy, string.buffer, string.len);
    LEN_BUFFER += string.len;
    BUFFER[LEN_BUFFER++] = '\0';
    return copy;
}
