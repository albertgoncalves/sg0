#ifndef TIME_H
#define TIME_H

#include "prelude.h"

#include <time.h>

typedef struct timespec Time;

#define MILLI_PER_SECOND 1000llu
#define MICRO_PER_SECOND 1000000llu
#define NANO_PER_SECOND  1000000000llu
#define NANO_PER_MILLI   (NANO_PER_SECOND / MILLI_PER_SECOND)
#define NANO_PER_MICRO   (NANO_PER_SECOND / MICRO_PER_SECOND)

#define FRAME_UPDATE_COUNT 6
#define FRAME_DURATION     (NANO_PER_SECOND / (60 + 1))
#define FRAME_UPDATE_STEP  (FRAME_DURATION / FRAME_UPDATE_COUNT)

static u64 now_ns(void) {
    Time time;
    EXIT_IF(clock_gettime(CLOCK_MONOTONIC, &time));
    return ((u64)time.tv_sec * NANO_PER_SECOND) + (u64)time.tv_nsec;
}

static void wait(u64 now) {
    const u64 elapsed = now_ns() - now;
    if (elapsed < FRAME_DURATION) {
        EXIT_IF(usleep((u32)((FRAME_DURATION - elapsed) / NANO_PER_MICRO)));
    }
}

#endif
