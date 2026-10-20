//
// Created by pierr on 7/21/2021.
//

#ifndef GD_GD_FIXED_H
#define GD_GD_FIXED_H
/* Add *BSD and co for inttypes.h */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "msinttypes/stdint.h"
#else
#include <stdint.h>
#endif
/* only used here, let do a generic fixed point integers later if required by other
   part of GD */
typedef uint32_t gdFixed;
typedef int32_t gd_fixed_16_16;
typedef gd_fixed_16_16 gd_fixed_t;
#define gd_fixed_e ((gd_fixed_t)1)
#define gd_fixed_1 (gd_int_to_fixed(1))
#define gd_fixed_1_minus_e (gd_fixed_1 - gd_fixed_e)
#define gd_fixed_minus_1 (pixman_int_to_fixed(-1))
#define gd_fixed_to_int(f) ((int)((f) >> 16))
#define gd_int_to_fixed(i) ((gd_fixed_t)(i) * (gd_fixed_t)65536)
#define gd_fixed_to_double(f) (double)((f) / (double)gd_fixed_1)
#define gd_double_to_fixed(d) ((gd_fixed_t)((d) * 65536.0))
#define gd_fixed_frac(f) ((f) & gd_fixed_1_minus_e)
#define gd_fixed_floor(f) ((f) & ~gd_fixed_1_minus_e)
#define gd_fixed_ceil(f) gd_fixed_floor((f) + gd_fixed_1_minus_e)
#define gd_fixed_fraction(f) ((f) & gd_fixed_1_minus_e)
#define gd_fixed_mod_2(f) ((f) & (gd_fixed1 | gd_fixed_1_minus_e))

#endif // GD_GD_FIXED_H
