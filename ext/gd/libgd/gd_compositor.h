#ifndef GD_COMPOSITOR_H
#define GD_COMPOSITOR_H

#include <stddef.h>
#include <stdint.h>
#include "gd_vector2d_private.h"

typedef struct {
    float r, g, b, a;
} gdPremulPixelF;

int gdCompositeOperatorIsValid(gdCompositeOperator op);
int gdCompositeOperatorIsUnbounded(gdCompositeOperator op);
gdPremulPixelF gdCompositePixel(gdCompositeOperator op, gdPremulPixelF src,
                                gdPremulPixelF dst, float coverage);
void gdCompositeSpan(gdCompositeOperator op, const gdPremulPixelF *src,
                     ptrdiff_t src_stride, gdPremulPixelF *dst,
                     const float *coverage, size_t n);
gdPremulPixelF gdCompositePixelFromArgb32(uint32_t pixel);
uint32_t gdCompositePixelToArgb32(gdPremulPixelF pixel);
gdPremulPixelF gdCompositePixelFromGd(int pixel);
int gdCompositePixelToGd(gdPremulPixelF pixel);

#endif
