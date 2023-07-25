#include "time.h"

u64 time_now(void) {
    Time time;
    EXIT_IF(clock_gettime(CLOCK_MONOTONIC, &time));
    return ((u64)time.tv_sec * NANOS_PER_SECOND) + (u64)time.tv_nsec;
}

void time_sleep(u64 nanoseconds) {
    const Time time = (Time){
        .tv_sec = nanoseconds / NANOS_PER_SECOND,
        .tv_nsec = nanoseconds % NANOS_PER_SECOND,
    };
    EXIT_IF(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &time, NULL));
}
