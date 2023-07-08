#ifndef PCG_RNG_H
#define PCG_RNG_H

#include "prelude.h"

#include <math.h>

typedef struct {
    u64 state;
    u64 increment;
} PcgRng;

static PcgRng RNG = {
    .state = 9600629759793949339llu,
    .increment = 15726070495360670683llu,
};

static u32 get_random_uniform_u32(void) {
    const u64 state = RNG.state;
    RNG.state = (state * 6364136223846793005llu) + (RNG.increment | 1);
    const u32 xor_shift = (u32)(((state >> 18u) ^ state) >> 27u);
    const u32 rotate = (u32)(state >> 59u);
    return (xor_shift >> rotate) | (xor_shift << ((-rotate) & 31));
}

static void set_seed(u64 state, u64 increment) {
    RNG.state = 0;
    RNG.increment = (increment << 1u) | 1u;
    get_random_uniform_u32();
    RNG.state += state;
    get_random_uniform_u32();
}

// NOTE: See `https://www.pcg-random.org/using-pcg-c-basic.html#generating-doubles`.
static f32 get_random_uniform_f32() {
    const u32 r = get_random_uniform_u32();
    return ldexpf((f32)r, -32);
}

// NOTE: See `https://github.com/imneme/pcg-c-basic/blob/master/pcg_basic.c#L75-L109`.
static u32 get_random_bounded_u32(u32 bound) {
    const u32 threshold = (-bound) % bound;
    for (;;) {
        const u32 r = get_random_uniform_u32();
        if (threshold <= r) {
            return r % bound;
        }
    }
}

#endif
