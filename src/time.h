#ifndef TIME_H
#define TIME_H

#include "prelude.h"

#include <time.h>

typedef struct timespec Time;

#define MILLIS_PER_SECOND 1000
#define MICROS_PER_SECOND 1000000
#define NANOS_PER_SECOND  1000000000
#define NANOS_PER_MILLI   (NANOS_PER_SECOND / MILLIS_PER_SECOND)
#define NANOS_PER_MICRO   (NANOS_PER_SECOND / MICROS_PER_SECOND)

u64  time_now(void);
void time_sleep(u64);

#endif
