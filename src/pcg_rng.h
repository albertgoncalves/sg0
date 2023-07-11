#ifndef PCG_RNG_H
#define PCG_RNG_H

#include "prelude.h"

void pcg_rng_seed(u64, u64);
u32  pcg_rng_random_uniform_u32(void);
u32  pcg_rng_random_bounded_u32(u32);
f32  pcg_rng_random_uniform_f32(void);

#endif
