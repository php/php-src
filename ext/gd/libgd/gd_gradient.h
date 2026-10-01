#ifndef GD_GRADIENT_H
#define GD_GRADIENT_H

#include "gd_compositor.h"
#include "gd_vector2d_private.h"
#include <stddef.h>
#include <stdint.h>

#define GD_GRADIENT_LUT_SIZE 1024

typedef enum { GD_GRADIENT_LINEAR, GD_GRADIENT_RADIAL } gdGradientKind;

typedef struct {
    double offset;
    gdPremulPixelF color;
    uint64_t sequence;
} gdColorStop;

struct gdGradientStruct {
    gdGradientKind kind;
    gdExtendMode extend;
    gdPathMatrix matrix;
    gdPathMatrix inverse;
    gdColorStop *stops;
    size_t stop_count, stop_capacity;
    uint64_t next_sequence;
    gdPremulPixelF lut[GD_GRADIENT_LUT_SIZE];
    union {
        struct {
            double x0, y0, dx, dy, inverse_length_squared, scale;
        } linear;
        struct {
            double x0, y0, r0, x1, y1, r1, cdx, cdy, dr, quadratic_a, scale;
        } radial;
    } geometry;
};

void gdGradientDestroy(gdGradient *gradient);
int gdExtendFold(double raw, gdExtendMode mode, double *folded);
gdPremulPixelF gdGradientSample(const gdGradient *gradient, const gdPathMatrix *device_to_pattern,
                                double device_x, double device_y);

#endif
