#include "time.h"

#include <time.h>

typedef struct timespec Time;

u64 time_nanoseconds(void) {
    Time time;
    EXIT_IF(clock_gettime(CLOCK_MONOTONIC, &time));
    return ((u64)time.tv_sec * NANO_PER_SECOND) + (u64)time.tv_nsec;
}
