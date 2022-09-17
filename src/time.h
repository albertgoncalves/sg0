#ifndef __TIME_H__
#define __TIME_H__

#include "prelude.h"

#include <time.h>

typedef struct timespec Time;

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
