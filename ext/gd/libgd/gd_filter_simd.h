#ifndef GD_FILTER_SIMD_H
#define GD_FILTER_SIMD_H

#include "gd.h"

int gdImageSmooth_avx(gdImagePtr im, float weight);
int gdImageSmooth_sse2(gdImagePtr im, float weight);

#endif /* GD_FILTER_SIMD_H */
