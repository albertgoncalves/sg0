#include "pcg_rng.h"

#include <math.h>

typedef struct {
    u64 state;
    u64 increment;
} PcgRng;

static PcgRng RNG = {
    .state = 9600629759793949339llu,
    .increment = 15726070495360670683llu,
};

void pcg_rng_seed(u64 state, u64 increment) {
    RNG.state = 0;
    RNG.increment = (increment << 1u) | 1u;
    pcg_rng_random_uniform_u32();
    RNG.state += state;
    pcg_rng_random_uniform_u32();
}

u32 pcg_rng_random_uniform_u32(void) {
    const u64 state = RNG.state;
    RNG.state = (state * 6364136223846793005llu) + (RNG.increment | 1);
    const u32 xor_shift = (u32)(((state >> 18u) ^ state) >> 27u);
    const u32 rotate = (u32)(state >> 59u);
    return (xor_shift >> rotate) | (xor_shift << ((-rotate) & 31));
}

// NOTE: See `https://www.pcg-random.org/using-pcg-c-basic.html#generating-doubles`.
f32 pcg_rng_random_uniform_f32(void) {
    const u32 x = pcg_rng_random_uniform_u32();
    return ldexpf((f32)x, -32);
}

// NOTE: See `https://github.com/imneme/pcg-c-basic/blob/master/pcg_basic.c#L75-L109`.
u32 pcg_rng_random_bounded_u32(u32 bound) {
    const u32 threshold = (-bound) % bound;
    for (;;) {
        const u32 x = pcg_rng_random_uniform_u32();
        if (threshold <= x) {
            return x % bound;
        }
    }
}
