
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd_vector2d_private.h"
#include "gd_intern.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_surface.h"
#include "gd_array.h"
#include "gd_path_matrix.h"
#include "gd_span_rle.h"

#include "gd_fixed.h"
#include "gd_compositor.h"
#include "gd_gradient.h"

#define BILINEAR_INTERPOLATION_BITS 7
#define BILINEAR_INTERPOLATION_RANGE (1 << BILINEAR_INTERPOLATION_BITS)

#define CLIP(v, low, high) ((v) < (low) ? (low) : ((v) > (high) ? (high) : (v)))
#define MOD(a, b) ((a) < 0 ? ((b) - ((-(a) - 1) % (b))) - 1 : (a) % (b))

static inline int _fixed_to_bilinear_weight (gd_fixed_t x)
{
    return (x >> (16 - BILINEAR_INTERPOLATION_BITS)) &
           ((1 << BILINEAR_INTERPOLATION_BITS) - 1);
}

static inline uint32_t fetch_pixel_general (gdSurfacePtr surface, int x, int y, int check_bounds)
{
    if (check_bounds && (x < 0 || x >= surface->width || y < 0 || y >= surface->height))
    {
        return 0;
    }

    uint32_t *src = (uint32_t *)(surface->data);
    return src[y * surface->stride/4  + x];
}

static inline uint32_t bilinear_interpolation (uint32_t tl, uint32_t tr, uint32_t bl, uint32_t br, int distx, int disty)
{
    uint64_t distxy, distxiy, distixy, distixiy;
    uint64_t tl64, tr64, bl64, br64;
    uint64_t f, r;

    if (tl == tr && tl == bl && tl == br)
        return tl;

    distx <<= (8 - BILINEAR_INTERPOLATION_BITS);
    disty <<= (8 - BILINEAR_INTERPOLATION_BITS);

    distxy = (uint64_t)distx * disty;
    distxiy = (uint64_t)distx * (256 - disty);
    distixy = (uint64_t)(256 - distx) * disty;
    distixiy = (uint64_t)(256 - distx) * (256 - disty);

    /* Alpha and Blue */
    tl64 = tl & 0xff0000ff;
    tr64 = tr & 0xff0000ff;
    bl64 = bl & 0xff0000ff;
    br64 = br & 0xff0000ff;

    f = tl64 * distixiy + tr64 * distxiy + bl64 * distixy + br64 * distxy;
    r = f & 0x0000ff0000ff0000ull;

    /* Red and Green */
    tl64 = tl;
    tl64 = ((tl64 << 16) & 0x000000ff00000000ull) | (tl64 & 0x0000ff00ull);

    tr64 = tr;
    tr64 = ((tr64 << 16) & 0x000000ff00000000ull) | (tr64 & 0x0000ff00ull);

    bl64 = bl;
    bl64 = ((bl64 << 16) & 0x000000ff00000000ull) | (bl64 & 0x0000ff00ull);

    br64 = br;
    br64 = ((br64 << 16) & 0x000000ff00000000ull) | (br64 & 0x0000ff00ull);

    f = tl64 * distixiy + tr64 * distxiy + bl64 * distixy + br64 * distxy;
    r |= ((f >> 16) & 0x000000ff00000000ull) | (f & 0xff000000ull);

    return (uint32_t)(r >> 16);
}

static inline int _update_w_repeat (gdExtendMode repeat, int *c, int size)
{
    if (repeat == GD_EXTEND_NONE) {
        if (*c < 0 || *c >= size)
            return 0;
    }
    else if (repeat == GD_EXTEND_REPEAT)
    {
        while (*c >= size)
            *c -= size;
        while (*c < 0)
            *c += size;
    }
    else if (repeat == GD_EXTEND_PAD)
    {
        *c = CLIP (*c, 0, size - 1);
    }
    else /* REFLECT */
    {
    *c = MOD (*c, size * 2);
    if (*c >= size)
        *c = size * 2 - *c - 1;
    }
    return 1;
}

static inline uint32_t
_surface_fetch_pixel_bilinear(gdSurfacePtr image, gd_fixed_t x, gd_fixed_t y, gdExtendMode repeat_mode)
{
    int width = image->width;
    int height = image->height;
    int x1, y1, x2, y2;
    uint32_t tl, tr, bl, br;
    int32_t distx, disty;

    x1 = x - gd_fixed_1 / 2;
    y1 = y - gd_fixed_1 / 2;

    distx = _fixed_to_bilinear_weight(x1);
    disty = _fixed_to_bilinear_weight(y1);
    x1 = gd_fixed_to_int(x1);
    y1 = gd_fixed_to_int(y1);
    x2 = x1 + 1;
    y2 = y1 + 1;
    if (repeat_mode != GD_EXTEND_NONE) {
        _update_w_repeat(repeat_mode, &x1, width);
        _update_w_repeat(repeat_mode, &y1, height);
        _update_w_repeat(repeat_mode, &x2, width);
        _update_w_repeat(repeat_mode, &y2, height);

        tl = fetch_pixel_general(image, x1, y1, 0);
        bl = fetch_pixel_general(image, x1, y2, 0);
        tr = fetch_pixel_general(image, x2, y1, 0);
        br = fetch_pixel_general(image, x2, y2, 0);
    } else {
        tl = fetch_pixel_general(image, x1, y1, 1);
        tr = fetch_pixel_general(image, x2, y1, 1);
        bl = fetch_pixel_general(image, x1, y2, 1);
        br = fetch_pixel_general(image, x2, y2, 1);
    }
    return bilinear_interpolation(tl, tr, bl, br, distx, disty);
}

#define ALPHA(c) ((c) >> 24)
static void operator_argb_color_source(uint32_t* dest, int length, uint32_t color, uint32_t alpha)
{
    gdPremulPixelF src = gdCompositePixelFromArgb32(color);
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_SOURCE, src, gdCompositePixelFromArgb32(dest[i]), alpha / 255.0f));
}

static void operator_argb_color_source_over(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    gdPremulPixelF src = gdCompositePixelFromArgb32(color);
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_OVER, src, gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void operator_argb_color_destination_in(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    gdPremulPixelF src = gdCompositePixelFromArgb32(color);
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_DEST_IN, src, gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void operator_argb_color_destination_out(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    gdPremulPixelF src = gdCompositePixelFromArgb32(color);
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_DEST_OUT, src, gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void operator_argb_color(gdCompositeOperator op, uint32_t *dest,
                                int length, uint32_t color, uint32_t alpha)
{
    gdPremulPixelF src = gdCompositePixelFromArgb32(color);
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            op, src, gdCompositePixelFromArgb32(dest[i]), alpha / 255.0f));
}

#define spanBlendLoop(func) \
    while(count--) \
    { \
        uint32_t* target = (uint32_t*)(surface->data + currentSpan->y * surface->stride) + currentSpan->x; \
        func(target, currentSpan->len, color, currentSpan->coverage); \
        ++currentSpan; \
    };

static void argb32_blend_color(gdSurfacePtr surface, gdImageOp op, const gdSpanRlePtr rle, uint32_t color)
{
    int count = rle->spans.size;
    gdSpanPtr currentSpan = rle->spans.data;
    switch (op) {
      case gdImageOpsSrc:
            spanBlendLoop(operator_argb_color_source);
            break;
      case gdImageOpsSrcOver:
            spanBlendLoop(operator_argb_color_source_over);
            break;
      case gdImageOpsDstIn:
            spanBlendLoop(operator_argb_color_destination_in);
            break;
      case gdImageOpsDstOut:
            spanBlendLoop(operator_argb_color_destination_out);
            break;
        default:
            while (count--) {
                uint32_t *target = (uint32_t *)(surface->data + currentSpan->y * surface->stride) + currentSpan->x;
                operator_argb_color(op, target, currentSpan->len, color, currentSpan->coverage);
                ++currentSpan;
            }
            break;
    }
}

static inline uint32_t premultiply_color(const gdColorPtr color, double opacity)
{
    const double a = color->a * opacity;
    const uint32_t alpha = (uint8_t)floor(a * 255.0 + 0.5);
    const uint32_t pr = (uint8_t)floor(color->r * a * 255.0 + 0.5);
    const uint32_t pg = (uint8_t)floor(color->g * a * 255.0 + 0.5);
    const uint32_t pb = (uint8_t)floor(color->b * a * 255.0 + 0.5);

    return (alpha << 24) | (pr << 16) | (pg << 8) | (pb);
}

#define _getVarName(var)  #var
void gdBlendColor(gdContextPtr context, const gdSpanRlePtr rle, const gdColorPtr color)
{
    if(color==NULL)
        return;

    gdStatePtr state = context->state;
    // replace once we have more than
    switch (context->surface->type) {
        case GD_SURFACE_ARGB32: {
                uint32_t pm_color = premultiply_color(color, state->opacity);
                argb32_blend_color(context->surface, state->op, rle, pm_color);
                break;
            }
      case GD_SURFACE_XRGB32:
      case GD_SURFACE_A8:
            gd_error("gdDraw does not implement %s yet.\n", _getVarName(GD_SURFACE_XRGB32));
            break;
      default:
            gd_error("gdDraw: provided surface has an unknown type.\n");
        return;
    }
}

typedef struct {
    gdPathMatrix matrix;
    gdExtendMode extend;
    uint8_t* data;
    gdSurfacePtr surface;
    int width;
    int height;
    int stride;
    int alpha;
} _spans_pattern;

// TODO: Once the rest is a tat bit faster, use func ptr for all but *_compose_source
// Macros are not an option, unreadable and painful to debug.
static void argb32_compose_source(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_SOURCE, gdCompositePixelFromArgb32(src[i]),
            gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void argb32_compose_source_over(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_OVER, gdCompositePixelFromArgb32(src[i]),
            gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void argb32_compose_dst_out(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_DEST_OUT, gdCompositePixelFromArgb32(src[i]),
            gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void argb32_compose_dst_in(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            GD_OP_DEST_IN, gdCompositePixelFromArgb32(src[i]),
            gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

static void argb32_compose(gdCompositeOperator op, uint32_t *dest, int length,
                           const uint32_t *src, uint32_t const_alpha)
{
    for (int i = 0; i < length; i++)
        dest[i] = gdCompositePixelToArgb32(gdCompositePixel(
            op, gdCompositePixelFromArgb32(src[i]),
            gdCompositePixelFromArgb32(dest[i]), const_alpha / 255.0f));
}

#define BUFFER_SIZE 1024
static void render_spans_compose_source(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                        const gdExtendMode extend, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha + 127) / 255;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            while(b < end) {
                *b = _surface_fetch_pixel_bilinear(pattern->surface, x, y, extend);
                x += fdx;
                y += fdy;
                ++b;
            }
            argb32_compose_source(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose_source_over(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                             const gdExtendMode extend, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha + 127) / 255;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            while(b < end) {
                *b = _surface_fetch_pixel_bilinear(pattern->surface, x, y, extend);
                x += fdx;
                y += fdy;
                ++b;
            }
            argb32_compose_source_over(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose_dst_in(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                        const gdExtendMode extend, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha + 127) / 255;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            while(b < end) {
                *b = _surface_fetch_pixel_bilinear(pattern->surface, x, y, extend);
                x += fdx;
                y += fdy;
                ++b;
            }
            argb32_compose_dst_in(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose_dst_out(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                         const gdExtendMode extend, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha + 127) / 255;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            while(b < end) {
                *b = _surface_fetch_pixel_bilinear(pattern->surface, x, y, extend);
                x += fdx;
                y += fdy;
                ++b;
            }
            argb32_compose_dst_out(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose(const gdSurface *surface, gdCompositeOperator op,
                                 const _spans_pattern *pattern, uint32_t *buffer,
                                 gdExtendMode extend, int fdx, int fdy,
                                 int count, gdSpanPtr spans)
{
    while (count--) {
        uint32_t *target = (uint32_t *)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        int coverage = (spans->coverage * pattern->alpha + 127) / 255;
        int length = spans->len;
        while (length) {
            int l = MIN(length, BUFFER_SIZE);
            for (int i = 0; i < l; i++) {
                buffer[i] = _surface_fetch_pixel_bilinear(pattern->surface, x, y, extend);
                x += fdx;
                y += fdy;
            }
            argb32_compose(op, target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void argb32_pattern_tiled_blend_transformed(gdSurfacePtr surface, gdImageOp op, const gdSpanRlePtr rle, const _spans_pattern * pattern)
{
    uint32_t buffer[BUFFER_SIZE];
    const gdExtendMode extend = pattern->extend;
    int fdx = gd_double_to_fixed(pattern->matrix.m00);
    int fdy = gd_double_to_fixed(pattern->matrix.m10);
    int count = rle->spans.size;
    gdSpanPtr spans = rle->spans.data;
    switch (op) {
        case gdImageOpsSrc:
            render_spans_compose_source(surface, pattern, buffer, extend, fdx, fdy, count, spans);
            break;
        case gdImageOpsSrcOver:
            render_spans_compose_source_over(surface, pattern, buffer, extend, fdx, fdy, count, spans);
            break;
        case gdImageOpsDstIn:
            render_spans_compose_dst_in(surface, pattern, buffer, extend, fdx, fdy, count, spans);
            break;
        case gdImageOpsDstOut:
            render_spans_compose_dst_out(surface, pattern, buffer, extend, fdx, fdy, count, spans);
            break;
        default:
            render_spans_compose(surface, op, pattern, buffer, extend,
                                 fdx, fdy, count, spans);
            break;
    }
}

#define spans_untransformed_blend_loop(composition) \
while(count--) { \
    int x = spans->x; \
    int length = spans->len; \
    int sx = xoff + x; \
    int sy = yoff + spans->y; \
    if(sy >= 0 && sy < image_height && sx < image_width) { \
        if(sx < 0) { \
            x -= sx; \
            length += sx; \
            sx = 0; \
        } \
        if(sx + length > image_width) length = image_width - sx; \
        if(length > 0) { \
            const int coverage = (spans->coverage * pattern->alpha + 127) / 255; \
            const uint32_t* src = (const uint32_t*)(pattern->data + sy * pattern->stride) + sx; \
            uint32_t* dest = (uint32_t*)(surface->data + spans->y * surface->stride) + x; \
            composition(dest, length, src, coverage); \
        } \
    } \
    ++spans; \
}

static void argb32_pattern_blend_untransformed(gdSurfacePtr surface, gdImageOp op, const gdSpanRlePtr rle, const _spans_pattern * pattern)
{
    const int image_width = pattern->width;
    const int image_height = pattern->height;
    int xoff = (int)(pattern->matrix.m02);
    int yoff = (int)(pattern->matrix.m12);

    int count = rle->spans.size;
    gdSpanPtr spans = rle->spans.data;

    switch (op) {
        case gdImageOpsSrc:
            spans_untransformed_blend_loop(argb32_compose_source);
            break;
        case gdImageOpsSrcOver:
            spans_untransformed_blend_loop(argb32_compose_source_over);
            break;
        case gdImageOpsDstIn:
            spans_untransformed_blend_loop(argb32_compose_dst_in);
            break;
        case gdImageOpsDstOut:
            spans_untransformed_blend_loop(argb32_compose_dst_out);
            break;
        default:
            while (count--) {
                int x = spans->x;
                int length = spans->len;
                int sx = xoff + x;
                int sy = yoff + spans->y;
                if (sy >= 0 && sy < image_height && sx < image_width) {
                    if (sx < 0) { x -= sx; length += sx; sx = 0; }
                    if (sx + length > image_width) length = image_width - sx;
                    if (length > 0) {
                        int coverage = (spans->coverage * pattern->alpha + 127) / 255;
                        const uint32_t *src = (const uint32_t *)(pattern->data + sy * pattern->stride) + sx;
                        uint32_t *dest = (uint32_t *)(surface->data + spans->y * surface->stride) + x;
                        argb32_compose(op, dest, length, src, coverage);
                    }
                }
                ++spans;
            }
    }
}

void gdDrawBlendPattern(gdContextPtr context, const gdSpanRlePtr rle, const gdPathPatternPtr pattern)
{
    if(pattern == NULL)
        return;

    gdStatePtr state = context->state;
    _spans_pattern pattern_impl;
    pattern_impl.extend = pattern->extend;
    pattern_impl.data = pattern->surface->data;
    pattern_impl.surface = pattern->surface;
    pattern_impl.width = pattern->surface->width;
    pattern_impl.height = pattern->surface->height;
    pattern_impl.stride = pattern->surface->stride;
    pattern_impl.alpha = (int)(state->opacity * pattern->opacity * 255.0 + 0.5);

    pattern_impl.matrix = pattern->matrix;
    gdPathMatrixMultiply(&pattern_impl.matrix, &pattern_impl.matrix, &state->matrix);
    gdPathMatrixInvert(&pattern_impl.matrix);

    const gdPathMatrixPtr matrix = &pattern_impl.matrix;
    int translating = (matrix->m00==1.0 && matrix->m10==0.0 && matrix->m01==0.0 && matrix->m11==1.0);
    if(translating) {
        if(pattern->extend == GD_EXTEND_NONE)
            argb32_pattern_blend_untransformed(context->surface, state->op, rle, &pattern_impl);
        else
            argb32_pattern_tiled_blend_transformed(context->surface, state->op, rle, &pattern_impl);
    } else {
        if(pattern->extend == GD_EXTEND_NONE)
            //argb32_pattern_blend_untransformed(context->surface, state->op, rle, &pattern_impl);
            argb32_pattern_tiled_blend_transformed(context->surface, state->op, rle, &pattern_impl);
        else
            argb32_pattern_tiled_blend_transformed(context->surface, state->op, rle, &pattern_impl);
    }
}

static unsigned int rle_coverage_at(const gdSpanRlePtr rle, int x, int y,
                                    int *cursor)
{
    if (!rle)
        return 255;
    while (*cursor < rle->spans.size) {
        const gdSpanPtr s = &rle->spans.data[*cursor];
        if (s->y < y || (s->y == y && s->x + s->len <= x)) {
            (*cursor)++;
            continue;
        }
        if (s->y == y && x >= s->x && x < s->x + s->len)
            return s->coverage;
        break;
    }
    return 0;
}

static gdPremulPixelF pattern_pixel_at(const _spans_pattern *pattern, int px, int py)
{
    double cx = px + 0.5;
    double cy = py + 0.5;
    gd_fixed_t x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
    gd_fixed_t y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
    return gdCompositePixelFromArgb32(
        _surface_fetch_pixel_bilinear(pattern->surface, x, y, pattern->extend));
}

typedef struct {
    const gdGradient *gradient;
    gdPathMatrix device_to_pattern;
    int valid;
} gdGradientSampler;

static void gradient_sampler_init(gdGradientSampler *s, const gdGradient *g,
                                  const gdStatePtr state)
{
    s->gradient = g;
    s->device_to_pattern = g->matrix;
    gdPathMatrixMultiply(&s->device_to_pattern, &s->device_to_pattern,
                         &state->matrix);
    s->valid = gdPathMatrixInvert(&s->device_to_pattern);
}

static gdPremulPixelF gradient_pixel_at(const gdGradientSampler *s, int x, int y)
{
    gdPremulPixelF z = {0, 0, 0, 0};
    return s->valid ? gdGradientSample(s->gradient, &s->device_to_pattern,
                                      x + 0.5, y + 0.5) : z;
}

static void gdDrawBlendGradient(gdContextPtr context, const gdSpanRlePtr rle,
                                const gdGradient *gradient)
{
    gdGradientSampler sampler;
    gdStatePtr state = context->state;
    int count = rle->spans.size;
    gdSpanPtr span = rle->spans.data;
    gradient_sampler_init(&sampler, gradient, state);
    while (count--) {
        uint32_t *dst = (uint32_t *)(context->surface->data +
                        span->y * context->surface->stride) + span->x;
        float coverage = (float)(span->coverage / 255.0 * state->opacity);
        for (int i = 0; i < span->len; i++) {
            gdPremulPixelF src = gradient_pixel_at(&sampler, span->x + i, span->y);
            gdPremulPixelF old = gdCompositePixelFromArgb32(dst[i]);
            dst[i] = gdCompositePixelToArgb32(gdCompositePixel(state->op, src, old, coverage));
        }
        span++;
    }
}

static gdPremulPixelF lerp_clip(gdPremulPixelF dst, gdPremulPixelF result,
                                float coverage)
{
    gdPremulPixelF p;
    p.r = dst.r + (result.r - dst.r) * coverage;
    p.g = dst.g + (result.g - dst.g) * coverage;
    p.b = dst.b + (result.b - dst.b) * coverage;
    p.a = dst.a + (result.a - dst.a) * coverage;
    return p;
}

static void blend_unbounded(gdContextPtr context, const gdSpanRlePtr shape)
{
    gdStatePtr state = context->state;
    gdPaintPtr source = state->source;
    gdSpanRlePtr clip_path = state->clippath;
    gdPremulPixelF solid = {0, 0, 0, 0};
    _spans_pattern pattern;
    gdGradientSampler gradient;
    float paint_opacity = (float)state->opacity;
    int shape_cursor = 0, clip_cursor = 0;
    int x0 = MAX(0, (int)floor(context->clip.x));
    int y0 = MAX(0, (int)floor(context->clip.y));
    int x1 = MIN(context->surface->width, (int)ceil(context->clip.x + context->clip.w));
    int y1 = MIN(context->surface->height, (int)ceil(context->clip.y + context->clip.h));

    if (source->type == gdPaintTypeColor) {
        solid = gdCompositePixelFromArgb32(premultiply_color(source->color, 1.0));
    } else if (source->type == gdPaintTypePattern) {
        gdPathPatternPtr p = source->pattern;
        pattern.extend = p->extend;
        pattern.surface = p->surface;
        pattern.matrix = p->matrix;
        gdPathMatrixMultiply(&pattern.matrix, &pattern.matrix, &state->matrix);
        gdPathMatrixInvert(&pattern.matrix);
        paint_opacity *= (float)p->opacity;
    } else if (source->type == gdPaintTypeGradient) {
        gradient_sampler_init(&gradient, source->gradient, state);
    } else {
        gd_error("Paint method not implemented or does not exist.");
        return;
    }

    for (int y = y0; y < y1; y++) {
        uint32_t *row = (uint32_t *)(context->surface->data + y * context->surface->stride);
        for (int x = x0; x < x1; x++) {
            float mask = rle_coverage_at(shape, x, y, &shape_cursor) / 255.0f;
            float clip = clip_path ? rle_coverage_at(clip_path, x, y, &clip_cursor) / 255.0f : 1.0f;
            gdPremulPixelF src = source->type == gdPaintTypeColor ? solid :
                (source->type == gdPaintTypePattern ? pattern_pixel_at(&pattern, x, y) :
                 gradient_pixel_at(&gradient, x, y));
            gdPremulPixelF dst = gdCompositePixelFromArgb32(row[x]);
            gdPremulPixelF result;
            if (clip == 0.0f)
                continue;
            result = gdCompositePixel(state->op, src, dst, mask * paint_opacity);
            if (clip < 1.0f)
                result = lerp_clip(dst, result, clip);
            row[x] = gdCompositePixelToArgb32(result);
        }
    }
}

void gdPathBlend(gdContextPtr context, const gdSpanRlePtr rle)
{
    if (rle == NULL)
        return;

    if (gdCompositeOperatorIsUnbounded(context->state->op) &&
        rle == context->rle) {
        blend_unbounded(context, rle);
        return;
    }
    if (rle->spans.size == 0 || context->state->opacity == 0.0)
        return;

    gdPaintPtr source = context->state->source;
    switch (source->type) {
        case gdPaintTypeColor:
            gdBlendColor(context, rle, source->color);
            break;
        case gdPaintTypePattern:
            gdDrawBlendPattern(context, rle, source->pattern);
            break;
        case gdPaintTypeGradient:
            gdDrawBlendGradient(context, rle, source->gradient);
            break;
        case gdPaintTypeSurface:
        default:
            gd_error("Paint method not implemented or does not exist.");
            break;
    }
}
