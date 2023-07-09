#ifndef TIME_H
#define TIME_H

#include "prelude.h"

#define MILLI_PER_SECOND 1000llu
#define MICRO_PER_SECOND 1000000llu
#define NANO_PER_SECOND  1000000000llu
#define NANO_PER_MILLI   (NANO_PER_SECOND / MILLI_PER_SECOND)
#define NANO_PER_MICRO   (NANO_PER_SECOND / MICRO_PER_SECOND)

u64 time_nanoseconds(void);

#endif
