#include <float.h>
#include <math.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd_gradient.h"
#include "gd_path_matrix.h"
#include "gd_vector2d_private.h"
#include "gdhelpers.h"

static int finite_matrix(const gdPathMatrix *m)
{
    return m && isfinite(m->m00) && isfinite(m->m10) && isfinite(m->m01) && isfinite(m->m11) &&
           isfinite(m->m02) && isfinite(m->m12);
}

static int valid_extend(gdExtendMode e) { return e >= GD_EXTEND_NONE && e <= GD_EXTEND_PAD; }

static gdPaintPtr create_gradient(gdGradientKind kind)
{
    gdPaintPtr paint = gdMalloc(sizeof(*paint));
    gdGradient *g;
    if (!paint)
        return NULL;
    g = gdCalloc(1, sizeof(*g));
    if (!g) {
        gdFree(paint);
        return NULL;
    }
    g->kind = kind;
    g->extend = GD_EXTEND_PAD;
    gdPathMatrixInitIdentity(&g->matrix);
    gdPathMatrixInitIdentity(&g->inverse);
    paint->ref = 1;
    paint->type = gdPaintTypeGradient;
    paint->gradient = g;
    return paint;
}

BGD_DECLARE(gdPaintPtr) gdPaintCreateLinear(double x0, double y0, double x1, double y1)
{
    gdPaintPtr p;
    gdGradient *g;
    double dx, dy, ls, s;
    if (!isfinite(x0) || !isfinite(y0) || !isfinite(x1) || !isfinite(y1))
        return NULL;
    p = create_gradient(GD_GRADIENT_LINEAR);
    if (!p)
        return NULL;
    g = p->gradient;
    dx = x1 - x0;
    dy = y1 - y0;
    ls = dx * dx + dy * dy;
    s = fmax(fmax(fabs(x0), fabs(y0)), fmax(fabs(x1), fabs(y1)));
    g->geometry.linear.x0 = x0;
    g->geometry.linear.y0 = y0;
    g->geometry.linear.dx = dx;
    g->geometry.linear.dy = dy;
    g->geometry.linear.scale = s;
    g->geometry.linear.inverse_length_squared =
        (isfinite(ls) && ls > 64.0 * DBL_EPSILON * fmax(1.0, s * s)) ? 1.0 / ls : 0.0;
    return p;
}

BGD_DECLARE(gdPaintPtr)
gdPaintCreateRadial(double x0, double y0, double r0, double x1, double y1, double r1)
{
    gdPaintPtr p;
    gdGradient *g;
    double s;
    if (!isfinite(x0) || !isfinite(y0) || !isfinite(r0) || !isfinite(x1) || !isfinite(y1) ||
        !isfinite(r1) || r0 < 0 || r1 < 0)
        return NULL;
    p = create_gradient(GD_GRADIENT_RADIAL);
    if (!p)
        return NULL;
    g = p->gradient;
    g->geometry.radial.x0 = x0;
    g->geometry.radial.y0 = y0;
    g->geometry.radial.r0 = r0;
    g->geometry.radial.x1 = x1;
    g->geometry.radial.y1 = y1;
    g->geometry.radial.r1 = r1;
    g->geometry.radial.cdx = x1 - x0;
    g->geometry.radial.cdy = y1 - y0;
    g->geometry.radial.dr = r1 - r0;
    g->geometry.radial.quadratic_a = g->geometry.radial.cdx * g->geometry.radial.cdx +
                                     g->geometry.radial.cdy * g->geometry.radial.cdy -
                                     g->geometry.radial.dr * g->geometry.radial.dr;
    s = fmax(fmax(fabs(x0), fabs(y0)), fmax(fabs(x1), fabs(y1)));
    s = fmax(s, fmax(r0, r1));
    g->geometry.radial.scale = s;
    return p;
}

void gdGradientDestroy(gdGradient *g)
{
    if (g) {
        gdFree(g->stops);
        gdFree(g);
    }
}

static gdPremulPixelF ramp_at(const gdColorStop *s, size_t n, double t)
{
    size_t i, last;
    gdPremulPixelF z = {0, 0, 0, 0}, a, b, out;
    double u;
    if (!n)
        return z;
    if (n == 1)
        return s[0].color;
    if (t < s[0].offset)
        return s[0].color;
    if (t >= s[n - 1].offset)
        return s[n - 1].color;
    for (i = 0; i < n && s[i].offset < t; i++) {
    }
    if (s[i].offset == t) {
        while (i + 1 < n && s[i + 1].offset == t)
            i++;
        return s[i].color;
    }
    last = i;
    a = s[i - 1].color;
    b = s[last].color;
    u = (t - s[i - 1].offset) / (s[last].offset - s[i - 1].offset);
    out.r = (float)(a.r + (b.r - a.r) * u);
    out.g = (float)(a.g + (b.g - a.g) * u);
    out.b = (float)(a.b + (b.b - a.b) * u);
    out.a = (float)(a.a + (b.a - a.a) * u);
    return out;
}

BGD_DECLARE(int)
gdPaintAddColorStopRgba(gdPaintPtr p, double off, double r, double g, double b, double a)
{
    gdGradient *gradient;
    gdColorStop *candidate;
    gdPremulPixelF lut[GD_GRADIENT_LUT_SIZE];
    size_t n, i, j;
    if (!p || p->type != gdPaintTypeGradient || !isfinite(off) || !isfinite(r) || !isfinite(g) ||
        !isfinite(b) || !isfinite(a) || off < 0 || off > 1 || r < 0 || r > 1 || g < 0 || g > 1 ||
        b < 0 || b > 1 || a < 0 || a > 1)
        return 0;
    gradient = p->gradient;
    n = gradient->stop_count + 1;
    candidate = gdMalloc(n * sizeof(*candidate));
    if (!candidate)
        return 0;
    if (gradient->stop_count)
        memcpy(candidate, gradient->stops, gradient->stop_count * sizeof(*candidate));
    candidate[n - 1].offset = off;
    candidate[n - 1].sequence = gradient->next_sequence;
    candidate[n - 1].color.r = (float)(r * a);
    candidate[n - 1].color.g = (float)(g * a);
    candidate[n - 1].color.b = (float)(b * a);
    candidate[n - 1].color.a = (float)a;
    for (i = 1; i < n; i++) {
        gdColorStop v = candidate[i];
        j = i;
        while (j &&
               (candidate[j - 1].offset > v.offset ||
                (candidate[j - 1].offset == v.offset && candidate[j - 1].sequence > v.sequence))) {
            candidate[j] = candidate[j - 1];
            j--;
        }
        candidate[j] = v;
    }
    for (i = 0; i < GD_GRADIENT_LUT_SIZE; i++)
        lut[i] = ramp_at(candidate, n, i / (double)(GD_GRADIENT_LUT_SIZE - 1));
    gdFree(gradient->stops);
    gradient->stops = candidate;
    gradient->stop_count = n;
    gradient->stop_capacity = n;
    gradient->next_sequence++;
    memcpy(gradient->lut, lut, sizeof(lut));
    return 1;
}

BGD_DECLARE(int) gdPaintAddColorStopRgb(gdPaintPtr p, double o, double r, double g, double b)
{
    return gdPaintAddColorStopRgba(p, o, r, g, b, 1.0);
}

BGD_DECLARE(int) gdPaintSetExtend(gdPaintPtr p, gdExtendMode e)
{
    if (!p || !valid_extend(e))
        return 0;
    if (p->type == gdPaintTypeGradient) {
        p->gradient->extend = e;
        return 1;
    }
    if (p->type == gdPaintTypePattern) {
        p->pattern->extend = e;
        return 1;
    }
    return 0;
}

BGD_DECLARE(int) gdPaintSetMatrix(gdPaintPtr p, const gdPathMatrixPtr m)
{
    gdPathMatrix inv;
    if (!p || !finite_matrix(m))
        return 0;
    inv = *m;
    if (!gdPathMatrixInvert(&inv))
        return 0;
    if (p->type == gdPaintTypeGradient) {
        p->gradient->matrix = *m;
        p->gradient->inverse = inv;
        return 1;
    }
    if (p->type == gdPaintTypePattern) {
        p->pattern->matrix = *m;
        return 1;
    }
    return 0;
}

int gdExtendFold(double raw, gdExtendMode mode, double *f)
{
    double m;
    if (!f || !isfinite(raw) || !valid_extend(mode))
        return 0;
    if (raw == 0.0)
        raw = 0.0;
    switch (mode) {
    case GD_EXTEND_NONE:
        if (raw < 0 || raw > 1)
            return 0;
        *f = raw;
        return 1;
    case GD_EXTEND_PAD:
        *f = raw < 0 ? 0 : (raw > 1 ? 1 : raw);
        return 1;
    case GD_EXTEND_REPEAT:
        *f = raw - floor(raw);
        return 1;
    case GD_EXTEND_REFLECT:
        m = fmod(raw, 2.0);
        if (m < 0)
            m += 2;
        *f = m <= 1 ? m : 2 - m;
        return 1;
    default:
        return 0;
    }
}

static int radial_t(const gdGradient *g, double x, double y, double *out)
{
    const double cx = g->geometry.radial.cdx, cy = g->geometry.radial.cdy,
                 dr = g->geometry.radial.dr;
    double px = x - g->geometry.radial.x0, py = y - g->geometry.radial.y0,
           r0 = g->geometry.radial.r0;
    double A = g->geometry.radial.quadratic_a, B = px * cx + py * cy + r0 * dr,
           C = px * px + py * py - r0 * r0;
    double scale = fmax(1.0, g->geometry.radial.scale + fabs(px) + fabs(py));
    double coord_tol = 64 * DBL_EPSILON * scale;
    double tol = coord_tol * scale, roots[2], disc;
    int n = 0, valid = 0;
    if (fabs(cx) <= coord_tol && fabs(cy) <= coord_tol && fabs(dr) <= coord_tol)
        return 0;
    if (fabs(A) <= tol) {
        if (fabs(B) <= tol)
            return 0;
        roots[n++] = C / (2 * B);
    } else {
        disc = B * B - A * C;
        if (disc < 0) {
            if (disc >= -tol * scale * scale)
                disc = 0;
            else
                return 0;
        }
        disc = sqrt(disc);
        roots[n++] = (B + disc) / A;
        roots[n++] = (B - disc) / A;
    }
    for (int i = 0; i < n; i++)
        if (isfinite(roots[i]) && r0 + roots[i] * dr >= -coord_tol &&
            (g->extend != GD_EXTEND_NONE || (roots[i] >= 0 && roots[i] <= 1))) {
            if (!valid || roots[i] > *out)
                *out = roots[i];
            valid = 1;
        }
    return valid;
}

gdPremulPixelF gdGradientSample(const gdGradient *g, const gdPathMatrix *m, double x, double y)
{
    gdPremulPixelF z = {0, 0, 0, 0};
    double px, py, t, f;
    long i;
    if (!g || !m || !g->stop_count)
        return z;
    px = m->m01 * y + m->m00 * x + m->m02;
    py = m->m11 * y + m->m10 * x + m->m12;
    if (g->kind == GD_GRADIENT_LINEAR) {
        if (g->geometry.linear.inverse_length_squared == 0)
            return z;
        t = ((px - g->geometry.linear.x0) * g->geometry.linear.dx +
             (py - g->geometry.linear.y0) * g->geometry.linear.dy) *
            g->geometry.linear.inverse_length_squared;
    } else if (!radial_t(g, px, py, &t))
        return z;
    if (!gdExtendFold(t, g->extend, &f))
        return z;
    i = (long)floor(f * (GD_GRADIENT_LUT_SIZE - 1) + 0.5);
    if (i < 0)
        i = 0;
    if (i >= GD_GRADIENT_LUT_SIZE)
        i = GD_GRADIENT_LUT_SIZE - 1;
    return g->lut[i];
}
