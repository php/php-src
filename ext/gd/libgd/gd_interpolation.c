/*
 * The two pass scaling function is based on:
 * Filtered Image Rescaling
 * Based on Gems III
 *  - Schumacher general filtered image rescaling
 * (pp. 414-424)
 * by Dale Schumacher
 *
 * 	Additional changes by Ray Gardener, Daylon Graphics Ltd.
 * 	December 4, 1999
 *
 * 	Ported to libgd by Pierre Joye. Support for multiple channels
 * 	added (argb for now).
 *
 * 	Initial sources code is avaibable in the Gems Source Code Packages:
 * 	http://www.acm.org/pubs/tog/GraphicsGems/GGemsIII.tar.gz
 *
 */

/*
        Summary:

                - Horizontal filter contributions are calculated on the fly,
                  as each column is mapped from src to dst image. This lets
                  us omit having to allocate a temporary full horizontal stretch
                  of the src image.

                - If none of the src pixels within a sampling region differ,
                  then the output pixel is forced to equal (any of) the source pixel.
                  This ensures that filters do not corrupt areas of constant color.

                - Filter weight contribution results, after summing, are
                  rounded to the nearest pixel color value instead of
                  being casted to ILubyte (usually an int or char). Otherwise,
                  artifacting occurs.
*/

/*
        Additional functions are available for simple rotation or up/downscaling.
        downscaling using the fixed point implementations are usually much faster
        than the existing gdImageCopyResampled while having a similar or better
        quality.

        For image rotations, the optimized versions have a lazy antialiasing for
        the edges of the images. For a much better antialiased result, the affine
        function is recommended.
*/

/*
TODO:
 - Optimize pixel accesses and loops once we have continuous buffer
 - Add scale support for a portion only of an image (equivalent of
copyresized/resampled)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef NDEBUG
/* Comment out this line to enable asserts.
 * TODO: This logic really belongs in cmake and configure.
 */
/* #define NDEBUG 1 */
#include <assert.h>

#include "gd.h"
#include "gd_intern.h"
#include "gdhelpers.h"

static gdImagePtr gdImageScaleBilinear(gdImagePtr im, const unsigned int new_width,
                                       const unsigned int new_height);
static gdImagePtr gdImageScaleBicubicFixed(gdImagePtr src, const unsigned int width,
                                           const unsigned int height);
static gdImagePtr gdImageScaleNearestNeighbour(gdImagePtr im, const unsigned int width,
                                               const unsigned int height);
static gdImagePtr gdImageRotateNearestNeighbour(gdImagePtr src, const float degrees,
                                                const int bgColor);
static gdImagePtr gdImageRotateGeneric(gdImagePtr src, const float degrees, const int bgColor);

#ifdef FUNCTION_NOT_USED_YET
/* only used by the inactive edge helper below */
typedef long gdFixed;
/* Integer to fixed point */
#define gd_itofx(x) ((x) << 8)

/* Float to fixed point */
#define gd_ftofx(x) (long)((x) * 256)

/*  Double to fixed point */
#define gd_dtofx(x) (long)((x) * 256)

/* Fixed point to integer */
#define gd_fxtoi(x) ((x) >> 8)

/* Fixed point to float */
#define gd_fxtof(x) ((float)(x) / 256)

/* Fixed point to double */
#define gd_fxtod(x) ((double)(x) / 256)

/* Multiply a fixed by a fixed */
#define gd_mulfx(x, y) (((x) * (y)) >> 8)

/* Divide a fixed by a fixed */
#define gd_divfx(x, y) (((x) << 8) / (y))
#endif

typedef struct _FilterInfo {
    double (*function)(const double, const double), support;
} FilterInfo;

typedef struct {
    double *Weights; /* Normalized weights of neighboring pixels */
    int Left, Right; /* Bounds of source pixels window */
} ContributionType;  /* Contirbution information for a single pixel */

typedef struct {
    ContributionType *ContribRow; /* Row (or column) of contribution weights */
    unsigned int WindowSize,      /* Filter window size (of affecting source pixels) */
        LineLength;               /* Length of line (no. or rows / cols) */
} LineContribType;

static double KernelBessel_J1(const double x)
{
    double p, q;

    register long i;

    static const double Pone[] = {0.581199354001606143928050809e+21,
                                  -0.6672106568924916298020941484e+20,
                                  0.2316433580634002297931815435e+19,
                                  -0.3588817569910106050743641413e+17,
                                  0.2908795263834775409737601689e+15,
                                  -0.1322983480332126453125473247e+13,
                                  0.3413234182301700539091292655e+10,
                                  -0.4695753530642995859767162166e+7,
                                  0.270112271089232341485679099e+4},
                        Qone[] = {0.11623987080032122878585294e+22,
                                  0.1185770712190320999837113348e+20,
                                  0.6092061398917521746105196863e+17,
                                  0.2081661221307607351240184229e+15,
                                  0.5243710262167649715406728642e+12,
                                  0.1013863514358673989967045588e+10,
                                  0.1501793594998585505921097578e+7,
                                  0.1606931573481487801970916749e+4,
                                  0.1e+1};

    p = Pone[8];
    q = Qone[8];
    for (i = 7; i >= 0; i--) {
        p = p * x * x + Pone[i];
        q = q * x * x + Qone[i];
    }
    return (double)(p / q);
}

static double KernelBessel_P1(const double x)
{
    double p, q;

    register long i;

    static const double Pone[] = {0.352246649133679798341724373e+5, 0.62758845247161281269005675e+5,
                                  0.313539631109159574238669888e+5, 0.49854832060594338434500455e+4,
                                  0.2111529182853962382105718e+3,   0.12571716929145341558495e+1},
                        Qone[] = {
                            0.352246649133679798068390431e+5, 0.626943469593560511888833731e+5,
                            0.312404063819041039923015703e+5, 0.4930396490181088979386097e+4,
                            0.2030775189134759322293574e+3,   0.1e+1};

    p = Pone[5];
    q = Qone[5];
    for (i = 4; i >= 0; i--) {
        p = p * (8.0 / x) * (8.0 / x) + Pone[i];
        q = q * (8.0 / x) * (8.0 / x) + Qone[i];
    }
    return (double)(p / q);
}

static double KernelBessel_Q1(const double x)
{
    double p, q;

    register long i;

    static const double Pone[] = {0.3511751914303552822533318e+3, 0.7210391804904475039280863e+3,
                                  0.4259873011654442389886993e+3, 0.831898957673850827325226e+2,
                                  0.45681716295512267064405e+1,   0.3532840052740123642735e-1},
                        Qone[] = {0.74917374171809127714519505e+4, 0.154141773392650970499848051e+5,
                                  0.91522317015169922705904727e+4, 0.18111867005523513506724158e+4,
                                  0.1038187585462133728776636e+3,  0.1e+1};

    p = Pone[5];
    q = Qone[5];
    for (i = 4; i >= 0; i--) {
        p = p * (8.0 / x) * (8.0 / x) + Pone[i];
        q = q * (8.0 / x) * (8.0 / x) + Qone[i];
    }
    return (double)(p / q);
}

static double KernelBessel_Order1(double x)
{
    double p, q;

    if (x == 0.0)
        return (0.0f);
    p = x;
    if (x < 0.0)
        x = (-x);
    if (x < 8.0)
        return (p * KernelBessel_J1(x));
    q = (double)sqrt(2.0f / (M_PI * x)) *
        (double)(KernelBessel_P1(x) * (1.0f / sqrt(2.0f) * (sin(x) - cos(x))) -
                 8.0f / x * KernelBessel_Q1(x) * (-1.0f / sqrt(2.0f) * (sin(x) + cos(x))));
    if (p < 0.0f)
        q = (-q);
    return (q);
}

static double filter_sinc(const double x, const double support)
{
    ARG_NOT_USED(support);
    /* X-scaled Sinc(x) function. */
    if (x == 0.0)
        return (1.0);
    return (sin(M_PI * (double)x) / (M_PI * (double)x));
}

static double filter_bessel(const double x, const double support)
{
    ARG_NOT_USED(support);
    if (x == 0.0f)
        return (double)(M_PI / 4.0f);
    return (KernelBessel_Order1((double)M_PI * x) / (2.0f * x));
}

static double filter_blackman(const double x, const double support)
{
    ARG_NOT_USED(support);
    return (0.42f + 0.5f * (double)cos(M_PI * x) + 0.08f * (double)cos(2.0f * M_PI * x));
}

static double filter_linear(const double x, const double support)
{
    ARG_NOT_USED(support);
    double ax = fabs(x);
    if (ax < 1.0f) {
        return (1.0f - ax);
    }
    return 0.0f;
}

static double filter_blackman_bessel(const double x, const double support)
{
    ARG_NOT_USED(support);
    return (filter_blackman(x / support, support) * filter_bessel(x, support));
}

static double filter_blackman_sinc(const double x, const double support)
{
    ARG_NOT_USED(support);
    return (filter_blackman(x / support, support) * filter_sinc(x, support));
}

/**
 * Generalized cubic kernel (for a=-1 it is the same as BicubicKernel):
  \verbatim
          /
         | (a+2)|t|**3 - (a+3)|t|**2 + 1     , |t| <= 1
  h(t) = | a|t|**3 - 5a|t|**2 + 8a|t| - 4a   , 1 < |t| <= 2
         | 0                                 , otherwise
          \
  \endverbatim
 * Often used values for a are -1 and -1/2.
 */
static double filter_generalized_cubic(const double t, const double support)
{
    const double a = -support;
    double abs_t = (double)fabs(t);
    double abs_t_sq = abs_t * abs_t;
    if (abs_t < 1)
        return (a + 2) * abs_t_sq * abs_t - (a + 3) * abs_t_sq + 1;
    if (abs_t < 2)
        return a * abs_t_sq * abs_t - 5 * a * abs_t_sq + 8 * a * abs_t - 4 * a;
    return 0;
}

/* CubicSpline filter, default radius 2 */
static double filter_cubic_spline(const double x1, const double support)
{
    ARG_NOT_USED(support);
    const double x = x1 < 0.0 ? -x1 : x1;

    if (x < 1.0) {
        const double x2 = x * x;

        return (0.5 * x2 * x - x2 + 2.0 / 3.0);
    }
    if (x < 2.0) {
        return (pow(2.0 - x, 3.0) / 6.0);
    }
    return 0;
}

#ifdef FUNCTION_NOT_USED_YET
/* CubicConvolution filter, default radius 3 */
static double filter_cubic_convolution(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;
    const double x2 = x1 * x1;
    const double x2_x = x2 * x;
    ARG_NOT_USED(support);
    if (x <= 1.0)
        return ((4.0 / 3.0) * x2_x - (7.0 / 3.0) * x2 + 1.0);
    if (x <= 2.0)
        return (-(7.0 / 12.0) * x2_x + 3 * x2 - (59.0 / 12.0) * x + 2.5);
    if (x <= 3.0)
        return ((1.0 / 12.0) * x2_x - (2.0 / 3.0) * x2 + 1.75 * x - 1.5);
    return 0;
}
#endif

static double filter_box(double x, const double support)
{
    if (x < -support)
        return 0.0f;
    if (x < support)
        return 1.0f;
    return 0.0f;
}

static double filter_catmullrom(const double x, const double support)
{
    ARG_NOT_USED(support);
    if (x < -2.0)
        return (0.0f);
    if (x < -1.0)
        return (0.5f * (4.0f + x * (8.0f + x * (5.0f + x))));
    if (x < 0.0)
        return (0.5f * (2.0f + x * x * (-5.0f - 3.0f * x)));
    if (x < 1.0)
        return (0.5f * (2.0f + x * x * (-5.0f + 3.0f * x)));
    if (x < 2.0)
        return (0.5f * (4.0f + x * (-8.0f + x * (5.0f - x))));
    return (0.0f);
}

/* Lanczos8 filter, default radius 8 */
static double filter_lanczos8(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;

    if (x == 0.0)
        return 1;

    if (x < support) {
        return support * sin(x * M_PI) * sin(x * M_PI / support) / (x * M_PI * x * M_PI);
    }
    return 0.0;
}

static double filter_lanczos3(const double x1, const double support)
{
    if (x1 < -3.0)
        return (0.0);
    if (x1 < 0.0)
        return (filter_sinc(-x1, support) * filter_sinc(-x1 / 3.0, support));
    if (x1 < 3.0)
        return (filter_sinc(x1, support) * filter_sinc(x1 / 3.0, support));
    return (0.0);
}

/* Hermite filter, default radius 1 */
static double filter_hermite(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;
    ARG_NOT_USED(support);

    if (x < 1.0)
        return ((2.0 * x - 3) * x * x + 1.0);

    return 0.0;
}

/* Trangle filter, default radius 1 */
static double filter_triangle(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;
    ARG_NOT_USED(support);

    if (x < 1.0)
        return (1.0 - x);
    return 0.0;
}

/* Bell filter, default radius 1.5 */
static double filter_bell(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;
    ARG_NOT_USED(support);

    if (x < 0.5)
        return (0.75 - x * x);
    if (x < 1.5)
        return (0.5 * pow(x - 1.5, 2.0));
    return 0.0;
}

/* Mitchell filter, default radius 2.0 */
static double filter_mitchell(const double x, const double support)
{
    ARG_NOT_USED(support);
#define KM_B (1.0f / 3.0f)
#define KM_C (1.0f / 3.0f)
#define KM_P0 ((6.0f - 2.0f * KM_B) / 6.0f)
#define KM_P2 ((-18.0f + 12.0f * KM_B + 6.0f * KM_C) / 6.0f)
#define KM_P3 ((12.0f - 9.0f * KM_B - 6.0f * KM_C) / 6.0f)
#define KM_Q0 ((8.0f * KM_B + 24.0f * KM_C) / 6.0f)
#define KM_Q1 ((-12.0f * KM_B - 48.0f * KM_C) / 6.0f)
#define KM_Q2 ((6.0f * KM_B + 30.0f * KM_C) / 6.0f)
#define KM_Q3 ((-1.0f * KM_B - 6.0f * KM_C) / 6.0f)

    if (x < -2.0)
        return (0.0f);
    if (x < -1.0)
        return (KM_Q0 - x * (KM_Q1 - x * (KM_Q2 - x * KM_Q3)));
    if (x < 0.0f)
        return (KM_P0 + x * x * (KM_P2 - x * KM_P3));
    if (x < 1.0f)
        return (KM_P0 + x * x * (KM_P2 + x * KM_P3));
    if (x < 2.0f)
        return (KM_Q0 + x * (KM_Q1 + x * (KM_Q2 + x * KM_Q3)));

    return (0.0f);
}

/* Cosine filter, default radius 1 */
static double filter_cosine(const double x, const double support)
{
    ARG_NOT_USED(support);
    if ((x >= -1.0) && (x <= 1.0))
        return ((cos(x * M_PI) + 1.0) / 2.0);

    return 0;
}

/* Quadratic filter, default radius 1.5 */
static double filter_quadratic(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;
    ARG_NOT_USED(support);
    if (x <= 0.5)
        return (-2.0 * x * x + 1);
    if (x <= 1.5)
        return (x * x - 2.5 * x + 1.5);
    return 0.0;
}

static double filter_bspline(const double x, const double support)
{
    ARG_NOT_USED(support);
    if (x > 2.0f) {
        return 0.0f;
    } else {
        double a, b, c, d;
        /* Was calculated anyway cause the "if((x-1.0f) < 0)" */
        const double xm1 = x - 1.0f;
        const double xp1 = x + 1.0f;
        const double xp2 = x + 2.0f;

        if ((xp2) <= 0.0f)
            a = 0.0f;
        else
            a = xp2 * xp2 * xp2;
        if ((xp1) <= 0.0f)
            b = 0.0f;
        else
            b = xp1 * xp1 * xp1;
        if (x <= 0)
            c = 0.0f;
        else
            c = x * x * x;
        if ((xm1) <= 0.0f)
            d = 0.0f;
        else
            d = xm1 * xm1 * xm1;

        return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));
    }
}

/* QuadraticBSpline filter, default radius 1.5 */
static double filter_quadratic_bspline(const double x1, const double support)
{
    const double x = x1 < 0.0 ? -x1 : x1;
    ARG_NOT_USED(support);
    if (x <= 0.5)
        return (-x * x + 0.75);
    if (x <= 1.5)
        return (0.5 * x * x - 1.5 * x + 1.125);
    return 0.0;
}

static double filter_gaussian(const double x, const double support)
{
    ARG_NOT_USED(support);
    /* return(exp((double) (-2.0 * x * x)) * sqrt(2.0 / M_PI)); */
    return (double)(exp(-2.0f * x * x) * 0.79788456080287f);
}

static double filter_hanning(const double x, const double support)
{
    ARG_NOT_USED(support);
    /* A Cosine windowing function */
    return (0.5 + 0.5 * cos(M_PI * x));
}

static double filter_hamming(const double x, const double support)
{
    ARG_NOT_USED(support);
    /* should be
    (0.54+0.46*cos(M_PI*(double) x));
    but this approximation is sufficient */
    if (x < -1.0f)
        return 0.0f;
    if (x < 0.0f)
        return 0.92f * (-2.0f * x - 3.0f) * x * x + 1.0f;
    if (x < 1.0f)
        return 0.92f * (2.0f * x - 3.0f) * x * x + 1.0f;
    return 0.0f;
}

static double filter_power(const double x, const double support)
{
    ARG_NOT_USED(support);
    const double a = 2.0f;
    if (fabs(x) > 1)
        return 0.0f;
    return (1.0f - (double)fabs(pow(x, a)));
}

static double filter_welsh(const double x, const double support)
{
    ARG_NOT_USED(support);
    /* Welsh parabolic windowing filter */
    if (x < 1.0)
        return (1 - x * x);
    return (0.0);
}

#if defined(_MSC_VER) && !defined(inline)
#define inline __inline
#endif

/* keep it for future usage for affine copy over an existing image, targetting
 * fix for 2.2.2 */
#ifdef FUNCTION_NOT_USED_YET
/* Copied from upstream's libgd */
static inline int _color_blend(const int dst, const int src)
{
    const int src_alpha = gdTrueColorGetAlpha(src);

    if (src_alpha == gdAlphaOpaque) {
        return src;
    } else {
        const int dst_alpha = gdTrueColorGetAlpha(dst);

        if (src_alpha == gdAlphaTransparent)
            return dst;
        if (dst_alpha == gdAlphaTransparent) {
            return src;
        } else {
            register int alpha, red, green, blue;
            const int src_weight = gdAlphaTransparent - src_alpha;
            const int dst_weight = (gdAlphaTransparent - dst_alpha) * src_alpha / gdAlphaMax;
            const int tot_weight = src_weight + dst_weight;

            alpha = src_alpha * dst_alpha / gdAlphaMax;

            red = (gdTrueColorGetRed(src) * src_weight + gdTrueColorGetRed(dst) * dst_weight) /
                  tot_weight;
            green =
                (gdTrueColorGetGreen(src) * src_weight + gdTrueColorGetGreen(dst) * dst_weight) /
                tot_weight;
            blue = (gdTrueColorGetBlue(src) * src_weight + gdTrueColorGetBlue(dst) * dst_weight) /
                   tot_weight;

            return ((alpha << 24) + (red << 16) + (green << 8) + blue);
        }
    }
}

static inline int _setEdgePixel(const gdImagePtr src, unsigned int x, unsigned int y,
                                gdFixed coverage, const int bgColor)
{
    const gdFixed f_127 = gd_itofx(127);
    register int c = src->tpixels[y][x];
    c = c | (((int)(gd_fxtof(gd_mulfx(coverage, f_127)) + 50.5f)) << 24);
    return _color_blend(bgColor, c);
}
#endif

static inline int getPixelOverflowTC(gdImagePtr im, const int x, const int y,
                                     const int bgColor /* 31bit ARGB TC */)
{
    if (gdImageBoundsSafe(im, x, y)) {
        const int c = im->tpixels[y][x];
        if (c == im->transparent) {
            return bgColor == -1 ? gdTrueColorAlpha(0, 0, 0, 127) : bgColor;
        }
        return c; /* 31bit ARGB TC */
    } else {
        return bgColor; /* 31bit ARGB TC */
    }
}

static inline int getPixelOverflowTCClipped(gdImagePtr im, const int x, const int y,
                                            const int bgColor, const gdRectPtr clip)
{
    if (clip != NULL &&
        (x < clip->x || y < clip->y || x >= clip->x + clip->width || y >= clip->y + clip->height)) {
        return bgColor;
    }
    return getPixelOverflowTC(im, x, y, bgColor);
}

#define colorIndex2RGBA(c)                                                                         \
    gdTrueColorAlpha(im->red[(c)], im->green[(c)], im->blue[(c)], im->alpha[(c)])
#define colorIndex2RGBcustomA(c, a)                                                                \
    gdTrueColorAlpha(im->red[(c)], im->green[(c)], im->blue[(c)], im->alpha[(a)])
static inline int getPixelOverflowPalette(gdImagePtr im, const int x, const int y,
                                          const int bgColor /* 31bit ARGB TC */)
{
    if (gdImageBoundsSafe(im, x, y)) {
        const int c = im->pixels[y][x];
        if (c == im->transparent) {
            return bgColor == -1 ? gdTrueColorAlpha(0, 0, 0, 127) : bgColor;
        }
        return colorIndex2RGBA(c);
    } else {
        return bgColor; /* 31bit ARGB TC */
    }
}

static inline int getPixelOverflowPaletteClipped(gdImagePtr im, const int x, const int y,
                                                 const int bgColor, const gdRectPtr clip)
{
    if (clip != NULL &&
        (x < clip->x || y < clip->y || x >= clip->x + clip->width || y >= clip->y + clip->height)) {
        return bgColor;
    }
    return getPixelOverflowPalette(im, x, y, bgColor);
}

static inline int gdClampInt(const int value, const int min, const int max)
{
    return value < min ? min : (value > max ? max : value);
}

static inline interpolation_method gdImageGetEffectiveInterpolation(const gdImagePtr im)
{
    switch (im->interpolation_id) {
    case GD_DEFAULT:
    case GD_BILINEAR_FIXED:
    case GD_LINEAR:
        return filter_linear;

    case GD_BICUBIC:
    case GD_BICUBIC_FIXED:
        return filter_cubic_spline;

    default:
        return im->interpolation;
    }
}

static int getPixelInterpolateWeightClipped(gdImagePtr im, const double x, const double y,
                                            const int bgColor, const gdRectPtr clip)
{
    /* Closest pixel <= (xf,yf) */
    int sx = (int)(x);
    int sy = (int)(y);
    const double xf = x - (double)sx;
    const double yf = y - (double)sy;
    const double nxf = (double)1.0 - xf;
    const double nyf = (double)1.0 - yf;
    const double m1 = xf * yf;
    const double m2 = nxf * yf;
    const double m3 = xf * nyf;
    const double m4 = nxf * nyf;

    /* get color values of neighbouring pixels */
    const int c1 = im->trueColor == 1 ? getPixelOverflowTCClipped(im, sx, sy, bgColor, clip)
                                      : getPixelOverflowPaletteClipped(im, sx, sy, bgColor, clip);
    const int c2 = im->trueColor == 1
                       ? getPixelOverflowTCClipped(im, sx - 1, sy, bgColor, clip)
                       : getPixelOverflowPaletteClipped(im, sx - 1, sy, bgColor, clip);
    const int c3 = im->trueColor == 1
                       ? getPixelOverflowTCClipped(im, sx, sy - 1, bgColor, clip)
                       : getPixelOverflowPaletteClipped(im, sx, sy - 1, bgColor, clip);
    const int c4 = im->trueColor == 1
                       ? getPixelOverflowTCClipped(im, sx - 1, sy - 1, bgColor, clip)
                       : getPixelOverflowPaletteClipped(im, sx, sy - 1, bgColor, clip);
    int r, g, b, a;

    if (x < 0)
        sx--;
    if (y < 0)
        sy--;

    /* component-wise summing-up of color values */
    r = (int)(m1 * gdTrueColorGetRed(c1) + m2 * gdTrueColorGetRed(c2) + m3 * gdTrueColorGetRed(c3) +
              m4 * gdTrueColorGetRed(c4));
    g = (int)(m1 * gdTrueColorGetGreen(c1) + m2 * gdTrueColorGetGreen(c2) +
              m3 * gdTrueColorGetGreen(c3) + m4 * gdTrueColorGetGreen(c4));
    b = (int)(m1 * gdTrueColorGetBlue(c1) + m2 * gdTrueColorGetBlue(c2) +
              m3 * gdTrueColorGetBlue(c3) + m4 * gdTrueColorGetBlue(c4));
    a = (int)(m1 * gdTrueColorGetAlpha(c1) + m2 * gdTrueColorGetAlpha(c2) +
              m3 * gdTrueColorGetAlpha(c3) + m4 * gdTrueColorGetAlpha(c4));

    r = CLAMP(r, 0, 255);
    g = CLAMP(g, 0, 255);
    b = CLAMP(b, 0, 255);
    a = CLAMP(a, 0, gdAlphaMax);
    return gdTrueColorAlpha(r, g, b, a);
}

/**
 * InternalFunction: getPixelInterpolated
 *  Returns the interpolated color value using the default interpolation
 *  method. The returned color is always in the ARGB format (truecolor).
 *
 * Parameters:
 * 	im - Image to set the default interpolation method
 *  y - X value of the ideal position
 *  y - Y value of the ideal position
 *  method - Interpolation method <gdInterpolationMethod>
 *
 * Returns:
 *  the interpolated color or -1 on error
 *
 * See also:
 *  <gdSetInterpolationMethod>
 */
static int getPixelInterpolatedClipped(gdImagePtr im, const double x, const double y,
                                       const int bgColor, const gdRectPtr clip)
{
    const int xi = (int)(x);
    const int yi = (int)(y);
    int yii;
    int i;
    double kernel, kernel_cache_y;
    double kernel_x[12], kernel_y[4];
    double new_r = 0.0f, new_g = 0.0f, new_b = 0.0f, new_a = 0.0f;
    const interpolation_method interpolation = gdImageGetEffectiveInterpolation(im);

    /* These methods use special implementations */
    if (im->interpolation_id == GD_NEAREST_NEIGHBOUR) {
        const int sx = gdClampInt((int)floor(x + 0.5), 0, gdImageSX(im) - 1);
        const int sy = gdClampInt((int)floor(y + 0.5), 0, gdImageSY(im) - 1);

        return im->trueColor == 1 ? getPixelOverflowTCClipped(im, sx, sy, bgColor, clip)
                                  : getPixelOverflowPaletteClipped(im, sx, sy, bgColor, clip);
    }

    if (im->interpolation_id == GD_WEIGHTED4) {
        return getPixelInterpolateWeightClipped(im, x, y, bgColor, clip);
    }

    if (interpolation) {
        for (i = 0; i < 4; i++) {
            kernel_x[i] = (double)interpolation((double)(xi + i - 1 - x), 1.0);
            kernel_y[i] = (double)interpolation((double)(yi + i - 1 - y), 1.0);
        }
    } else {
        return -1;
    }

    /*
     * TODO: use the known fast rgba multiplication implementation once
     * the new formats are in place
     */
    for (yii = yi - 1; yii < yi + 3; yii++) {
        int xii;
        kernel_cache_y = kernel_y[yii - (yi - 1)];
        if (im->trueColor) {
            for (xii = xi - 1; xii < xi + 3; xii++) {
                const int rgbs = getPixelOverflowTCClipped(im, xii, yii, bgColor, clip);

                kernel = kernel_cache_y * kernel_x[xii - (xi - 1)];
                new_r += kernel * gdTrueColorGetRed(rgbs);
                new_g += kernel * gdTrueColorGetGreen(rgbs);
                new_b += kernel * gdTrueColorGetBlue(rgbs);
                new_a += kernel * gdTrueColorGetAlpha(rgbs);
            }
        } else {
            for (xii = xi - 1; xii < xi + 3; xii++) {
                const int rgbs = getPixelOverflowPaletteClipped(im, xii, yii, bgColor, clip);

                kernel = kernel_cache_y * kernel_x[xii - (xi - 1)];
                new_r += kernel * gdTrueColorGetRed(rgbs);
                new_g += kernel * gdTrueColorGetGreen(rgbs);
                new_b += kernel * gdTrueColorGetBlue(rgbs);
                new_a += kernel * gdTrueColorGetAlpha(rgbs);
            }
        }
    }

    new_r = CLAMP(new_r, 0, 255);
    new_g = CLAMP(new_g, 0, 255);
    new_b = CLAMP(new_b, 0, 255);
    new_a = CLAMP(new_a, 0, gdAlphaMax);

    return gdTrueColorAlpha((int)(new_r + 0.5), (int)(new_g + 0.5), (int)(new_b + 0.5),
                            (int)(new_a + 0.5));
}

static int getPixelInterpolated(gdImagePtr im, const double x, const double y, const int bgColor)
{
    return getPixelInterpolatedClipped(im, x, y, bgColor, NULL);
}

static inline LineContribType *_gdContributionsAlloc(unsigned int line_length,
                                                     unsigned int windows_size)
{
    unsigned int u = 0;
    LineContribType *res;
    size_t weights_size;

    if (overflow2(windows_size, sizeof(double))) {
        return NULL;
    } else {
        weights_size = windows_size * sizeof(double);
    }
    res = (LineContribType *)gdMalloc(sizeof(LineContribType));
    if (!res) {
        return NULL;
    }
    res->WindowSize = windows_size;
    res->LineLength = line_length;
    if (overflow2(line_length, sizeof(ContributionType))) {
        gdFree(res);
        return NULL;
    }
    res->ContribRow = (ContributionType *)gdMalloc(line_length * sizeof(ContributionType));
    if (res->ContribRow == NULL) {
        gdFree(res);
        return NULL;
    }
    for (u = 0; u < line_length; u++) {
        res->ContribRow[u].Weights = (double *)gdMalloc(weights_size);
        if (res->ContribRow[u].Weights == NULL) {
            unsigned int i;

            for (i = 0; i < u; i++) {
                gdFree(res->ContribRow[i].Weights);
            }
            gdFree(res->ContribRow);
            gdFree(res);
            return NULL;
        }
    }
    return res;
}

static inline void _gdContributionsFree(LineContribType *p)
{
    unsigned int u;
    for (u = 0; u < p->LineLength; u++) {
        gdFree(p->ContribRow[u].Weights);
    }
    gdFree(p->ContribRow);
    gdFree(p);
}

static inline LineContribType *_gdContributionsCalc(unsigned int line_size, unsigned int src_size,
                                                    double scale_d, const double support,
                                                    const interpolation_method pFilter)
{
    double width_d;
    double scale_f_d = 1.0;
    const double filter_width_d = support;
    int windows_size;
    unsigned int u;
    LineContribType *res;

    if (scale_d < 1.0) {
        width_d = filter_width_d / scale_d;
        scale_f_d = scale_d;
    } else {
        width_d = filter_width_d;
    }

    windows_size = 2 * (int)ceil(width_d) + 1;
    res = _gdContributionsAlloc(line_size, windows_size);
    if (res == NULL) {
        return NULL;
    }
    for (u = 0; u < line_size; u++) {
        const double dCenter = (double)u / scale_d;
        /* get the significant edge points affecting the pixel */
        register int iLeft = MAX(0, (int)floor(dCenter - width_d));
        int iRight = MIN((int)ceil(dCenter + width_d), (int)src_size - 1);
        double dTotalWeight = 0.0;
        int iSrc;

        /* Cut edge points to fit in filter window in case of spill-off */
        if (iRight - iLeft + 1 > windows_size) {
            if (iLeft < ((int)src_size - 1 / 2)) {
                iLeft++;
            } else {
                iRight--;
            }
        }

        res->ContribRow[u].Left = iLeft;
        res->ContribRow[u].Right = iRight;

        for (iSrc = iLeft; iSrc <= iRight; iSrc++) {
            dTotalWeight +=
                (res->ContribRow[u].Weights[iSrc - iLeft] =
                     scale_f_d * (*pFilter)(scale_f_d * (dCenter - (double)iSrc), support));
        }

        if (dTotalWeight < 0.0) {
            _gdContributionsFree(res);
            return NULL;
        }

        if (dTotalWeight > 0.0) {
            for (iSrc = iLeft; iSrc <= iRight; iSrc++) {
                res->ContribRow[u].Weights[iSrc - iLeft] /= dTotalWeight;
            }
        }
    }
    return res;
}

static inline void _gdScaleOneAxis(gdImagePtr pSrc, gdImagePtr dst, unsigned int dst_len,
                                   unsigned int row, LineContribType *contrib, gdAxis axis)
{
    unsigned int ndx;

    for (ndx = 0; ndx < dst_len; ndx++) {
        double r = 0, g = 0, b = 0, a = 0;
        const int left = contrib->ContribRow[ndx].Left;
        const int right = contrib->ContribRow[ndx].Right;
        int *dest = (axis == HORIZONTAL) ? &dst->tpixels[row][ndx] : &dst->tpixels[ndx][row];

        int i;

        /* Accumulate each channel */
        for (i = left; i <= right; i++) {
            const int left_channel = i - left;
            const int srcpx = (axis == HORIZONTAL) ? pSrc->tpixels[row][i] : pSrc->tpixels[i][row];

            r +=
                contrib->ContribRow[ndx].Weights[left_channel] * (double)(gdTrueColorGetRed(srcpx));
            g += contrib->ContribRow[ndx].Weights[left_channel] *
                 (double)(gdTrueColorGetGreen(srcpx));
            b += contrib->ContribRow[ndx].Weights[left_channel] *
                 (double)(gdTrueColorGetBlue(srcpx));
            a += contrib->ContribRow[ndx].Weights[left_channel] *
                 (double)(gdTrueColorGetAlpha(srcpx));
        } /* for */

        *dest = gdTrueColorAlpha(uchar_clamp(r, 0xFF), uchar_clamp(g, 0xFF), uchar_clamp(b, 0xFF),
                                 uchar_clamp(a, 0x7F)); /* alpha is 0..127 */
    } /* for */
} /* _gdScaleOneAxis*/

static inline int _gdScalePass(const gdImagePtr pSrc, const unsigned int src_len,
                               const gdImagePtr pDst, const unsigned int dst_len,
                               const unsigned int num_lines, const gdAxis axis,
                               const FilterInfo *filter)
{
    unsigned int line_ndx;
    LineContribType *contrib;

    /* Same dim, just copy it. */
    assert(dst_len != src_len); // TODO: caller should handle this.

    contrib = _gdContributionsCalc(dst_len, src_len, (double)dst_len / (double)src_len,
                                   filter->support, filter->function);
    if (contrib == NULL) {
        return 0;
    }

    /* Scale each line */
    for (line_ndx = 0; line_ndx < num_lines; line_ndx++) {
        _gdScaleOneAxis(pSrc, pDst, dst_len, line_ndx, contrib, axis);
    }
    _gdContributionsFree(contrib);
    return 1;
} /* _gdScalePass*/

static const FilterInfo filters[GD_METHOD_COUNT + 1] = {
    {filter_box, 0.0},
    {filter_bell, 1.5},
    {filter_bessel, 0.0},
    {NULL, 0.0}, /* NA bilenear/bilinear fixed */
    {NULL, 0.0}, /* NA bicubic */
    {NULL, 0.0}, /* NA bicubic fixed */
    {filter_blackman, 1.0},
    {filter_box, 0.5},
    {filter_bspline, 1.5},
    {filter_catmullrom, 2.0},
    {filter_gaussian, 1.25},
    {filter_generalized_cubic, 0.5},
    {filter_hermite, 1.0},
    {filter_hamming, 1.0},
    {filter_hanning, 1.0},
    {filter_mitchell, 2.0},
    {NULL, 0.0}, /* NA Nearest */
    {filter_power, 0.0},
    {filter_quadratic, 1.5},
    {filter_sinc, 1.0},
    {filter_triangle, 1.0},
    {NULL, 1.0}, /* NA weighted4 */
    {filter_linear, 1.0},
    {filter_lanczos3, 3.0},
    {filter_lanczos8, 8.0},
    {filter_blackman_bessel, 3.2383},
    {filter_blackman_sinc, 4.0},
    {filter_quadratic_bspline, 1.5},
    {filter_cubic_spline, 0.0},
    {filter_cosine, 0.0},
    {filter_welsh, 0.0},
};

static const FilterInfo *_get_filterinfo_for_id(gdInterpolationMethod id)
{

    if (id >= GD_METHOD_COUNT) {
        id = GD_DEFAULT;
    }
    return &filters[id];
}

static gdImagePtr gdImageScaleTwoPass(const gdImagePtr src, const unsigned int new_width,
                                      const unsigned int new_height)
{
    const unsigned int src_width = src->sx;
    const unsigned int src_height = src->sy;
    gdImagePtr tmp_im = NULL;
    gdImagePtr dst = NULL;
    int scale_pass_res;
    const FilterInfo *filter = _get_filterinfo_for_id(src->interpolation_id);

    /* First, handle the trivial case. */
    if (src_width == new_width && src_height == new_height) {
        return gdImageClone(src);
    }

    /* Convert to truecolor if it isn't; this code requires it. */
    if (!src->trueColor) {
        gdImagePaletteToTrueColor(src);
    }

    /* Scale horizontally unless sizes are the same. */
    if (src_width == new_width) {
        tmp_im = src;
    } else {
        tmp_im = gdImageCreateTrueColor(new_width, src_height);
        if (tmp_im == NULL) {
            return NULL;
        }
        gdImageSetInterpolationMethod(tmp_im, src->interpolation_id);

        scale_pass_res =
            _gdScalePass(src, src_width, tmp_im, new_width, src_height, HORIZONTAL, filter);
        if (scale_pass_res != 1) {
            gdImageDestroy(tmp_im);
            return NULL;
        }
    }

    /* If vertical sizes match, we're done. */
    if (src_height == new_height) {
        assert(tmp_im != src);
        return tmp_im;
    }

    /* Otherwise, we need to scale vertically. */
    dst = gdImageCreateTrueColor(new_width, new_height);
    if (dst != NULL) {
        gdImageSetInterpolationMethod(dst, src->interpolation_id);
        scale_pass_res =
            _gdScalePass(tmp_im, src_height, dst, new_height, new_width, VERTICAL, filter);
        if (scale_pass_res != 1) {
            gdImageDestroy(dst);
            if (src != tmp_im) {
                gdImageDestroy(tmp_im);
            }
            return NULL;
        }
    }

    if (src != tmp_im) {
        gdImageDestroy(tmp_im);
    }

    return dst;
} /* gdImageScaleTwoPass*/

/*
        Bilinear, bicubic and nearest implementations are
        rewamped versions of the implementation in CBitmapEx

        http://www.codeproject.com/Articles/29121/CBitmapEx-Free-C-Bitmap-Manipulation-Class

        The GD_BILINEAR_FIXED and GD_BICUBIC_FIXED public names are kept for
        compatibility, but these implementations use floating point arithmetic.
*/
static gdImagePtr gdImageScaleNearestNeighbour(gdImagePtr im, const unsigned int width,
                                               const unsigned int height)
{
    const unsigned long new_width = MAX(1, width);
    const unsigned long new_height = MAX(1, height);
    const double dx = (double)im->sx / (double)new_width;
    const double dy = (double)im->sy / (double)new_height;

    gdImagePtr dst_img;
    unsigned long dst_offset_x;
    unsigned long dst_offset_y = 0;
    unsigned int i;

    dst_img = gdImageCreateTrueColor(new_width, new_height);

    if (dst_img == NULL) {
        return NULL;
    }

    for (i = 0; i < new_height; i++) {
        unsigned int j;
        dst_offset_x = 0;
        if (im->trueColor) {
            for (j = 0; j < new_width; j++) {
                const long m = gdClampInt((int)((double)i * dy), 0, gdImageSY(im) - 1);
                const long n = gdClampInt((int)((double)j * dx), 0, gdImageSX(im) - 1);

                dst_img->tpixels[dst_offset_y][dst_offset_x++] = im->tpixels[m][n];
            }
        } else {
            for (j = 0; j < new_width; j++) {
                const long m = gdClampInt((int)((double)i * dy), 0, gdImageSY(im) - 1);
                const long n = gdClampInt((int)((double)j * dx), 0, gdImageSX(im) - 1);

                dst_img->tpixels[dst_offset_y][dst_offset_x++] = colorIndex2RGBA(im->pixels[m][n]);
            }
        }
        dst_offset_y++;
    }
    return dst_img;
}

static gdImagePtr gdImageScaleBilinearPalette(gdImagePtr im, const unsigned int new_width,
                                              const unsigned int new_height)
{
    long _width = MAX(1, new_width);
    long _height = MAX(1, new_height);
    double dx = (double)gdImageSX(im) / (double)_width;
    double dy = (double)gdImageSY(im) / (double)_height;

    int dst_offset_h;
    int dst_offset_v = 0;
    long i;
    gdImagePtr new_img;
    const int transparent = im->transparent;

    new_img = gdImageCreateTrueColor(new_width, new_height);
    if (new_img == NULL) {
        return NULL;
    }

    if (transparent < 0) {
        /* uninitialized */
        new_img->transparent = -1;
    } else {
        new_img->transparent = gdTrueColorAlpha(im->red[transparent], im->green[transparent],
                                                im->blue[transparent], im->alpha[transparent]);
    }

    for (i = 0; i < _height; i++) {
        long j;
        const double src_y = (double)i * dy;
        register long m = gdClampInt((int)src_y, 0, gdImageSY(im) - 1);

        dst_offset_h = 0;

        for (j = 0; j < _width; j++) {
            /* Update bitmap */
            const double src_x = (double)j * dx;
            const long n = gdClampInt((int)src_x, 0, gdImageSX(im) - 1);
            const double f_f = src_y - (double)m;
            const double f_g = src_x - (double)n;

            const double f_w1 = (1.0 - f_f) * (1.0 - f_g);
            const double f_w2 = (1.0 - f_f) * f_g;
            const double f_w3 = f_f * (1.0 - f_g);
            const double f_w4 = f_f * f_g;
            unsigned int pixel1;
            unsigned int pixel2;
            unsigned int pixel3;
            unsigned int pixel4;

            /* 0 for bgColor; (n,m) is supposed to be valid anyway */
            pixel1 = getPixelOverflowPalette(im, n, m, 0);
            pixel2 = getPixelOverflowPalette(im, n + 1, m, pixel1);
            pixel3 = getPixelOverflowPalette(im, n, m + 1, pixel1);
            pixel4 = getPixelOverflowPalette(im, n + 1, m + 1, pixel1);

            {
                const unsigned char red = uchar_clamp(
                    f_w1 * gdTrueColorGetRed(pixel1) + f_w2 * gdTrueColorGetRed(pixel2) +
                        f_w3 * gdTrueColorGetRed(pixel3) + f_w4 * gdTrueColorGetRed(pixel4),
                    0xFF);
                const unsigned char green = uchar_clamp(
                    f_w1 * gdTrueColorGetGreen(pixel1) + f_w2 * gdTrueColorGetGreen(pixel2) +
                        f_w3 * gdTrueColorGetGreen(pixel3) + f_w4 * gdTrueColorGetGreen(pixel4),
                    0xFF);
                const unsigned char blue = uchar_clamp(
                    f_w1 * gdTrueColorGetBlue(pixel1) + f_w2 * gdTrueColorGetBlue(pixel2) +
                        f_w3 * gdTrueColorGetBlue(pixel3) + f_w4 * gdTrueColorGetBlue(pixel4),
                    0xFF);
                const unsigned char alpha = uchar_clamp(
                    f_w1 * gdTrueColorGetAlpha(pixel1) + f_w2 * gdTrueColorGetAlpha(pixel2) +
                        f_w3 * gdTrueColorGetAlpha(pixel3) + f_w4 * gdTrueColorGetAlpha(pixel4),
                    0x7F);

                new_img->tpixels[dst_offset_v][dst_offset_h] =
                    gdTrueColorAlpha(red, green, blue, alpha);
            }

            dst_offset_h++;
        }

        dst_offset_v++;
    }
    return new_img;
}

static gdImagePtr gdImageScaleBilinearTC(gdImagePtr im, const unsigned int new_width,
                                         const unsigned int new_height)
{
    long dst_w = MAX(1, new_width);
    long dst_h = MAX(1, new_height);
    double dx = (double)gdImageSX(im) / (double)dst_w;
    double dy = (double)gdImageSY(im) / (double)dst_h;

    int dst_offset_h;
    int dst_offset_v = 0;
    long i;
    gdImagePtr new_img;

    new_img = gdImageCreateTrueColor(new_width, new_height);
    if (!new_img) {
        return NULL;
    }

    for (i = 0; i < dst_h; i++) {
        long j;
        dst_offset_h = 0;
        for (j = 0; j < dst_w; j++) {
            /* Update bitmap */
            const double src_y = (double)i * dy;
            const double src_x = (double)j * dx;
            const long m = gdClampInt((int)src_y, 0, gdImageSY(im) - 1);
            const long n = gdClampInt((int)src_x, 0, gdImageSX(im) - 1);
            const double f_f = src_y - (double)m;
            const double f_g = src_x - (double)n;

            const double f_w1 = (1.0 - f_f) * (1.0 - f_g);
            const double f_w2 = (1.0 - f_f) * f_g;
            const double f_w3 = f_f * (1.0 - f_g);
            const double f_w4 = f_f * f_g;
            unsigned int pixel1;
            unsigned int pixel2;
            unsigned int pixel3;
            unsigned int pixel4;
            /* 0 for bgColor; (n,m) is supposed to be valid anyway */
            pixel1 = getPixelOverflowTC(im, n, m, 0);
            pixel2 = getPixelOverflowTC(im, n + 1, m, pixel1);
            pixel3 = getPixelOverflowTC(im, n, m + 1, pixel1);
            pixel4 = getPixelOverflowTC(im, n + 1, m + 1, pixel1);

            {
                const unsigned char red = uchar_clamp(
                    f_w1 * gdTrueColorGetRed(pixel1) + f_w2 * gdTrueColorGetRed(pixel2) +
                        f_w3 * gdTrueColorGetRed(pixel3) + f_w4 * gdTrueColorGetRed(pixel4),
                    0xFF);
                const unsigned char green = uchar_clamp(
                    f_w1 * gdTrueColorGetGreen(pixel1) + f_w2 * gdTrueColorGetGreen(pixel2) +
                        f_w3 * gdTrueColorGetGreen(pixel3) + f_w4 * gdTrueColorGetGreen(pixel4),
                    0xFF);
                const unsigned char blue = uchar_clamp(
                    f_w1 * gdTrueColorGetBlue(pixel1) + f_w2 * gdTrueColorGetBlue(pixel2) +
                        f_w3 * gdTrueColorGetBlue(pixel3) + f_w4 * gdTrueColorGetBlue(pixel4),
                    0xFF);
                const unsigned char alpha = uchar_clamp(
                    f_w1 * gdTrueColorGetAlpha(pixel1) + f_w2 * gdTrueColorGetAlpha(pixel2) +
                        f_w3 * gdTrueColorGetAlpha(pixel3) + f_w4 * gdTrueColorGetAlpha(pixel4),
                    0x7F);

                new_img->tpixels[dst_offset_v][dst_offset_h] =
                    gdTrueColorAlpha(red, green, blue, alpha);
            }

            dst_offset_h++;
        }

        dst_offset_v++;
    }
    return new_img;
}

static gdImagePtr gdImageScaleBilinear(gdImagePtr im, const unsigned int new_width,
                                       const unsigned int new_height)
{
    if (im->trueColor) {
        return gdImageScaleBilinearTC(im, new_width, new_height);
    } else {
        return gdImageScaleBilinearPalette(im, new_width, new_height);
    }
}

static gdImagePtr gdImageScaleBicubicFixed(gdImagePtr src, const unsigned int width,
                                           const unsigned int height)
{
    const long new_width = MAX(1, width);
    const long new_height = MAX(1, height);
    const int src_w = gdImageSX(src);
    const int src_h = gdImageSY(src);
    const double dx = (double)src_w / (double)new_width;
    const double dy = (double)src_h / (double)new_height;
    const double gamma = 1.04;
    gdImagePtr dst;

    unsigned int dst_offset_x;
    unsigned int dst_offset_y = 0;
    long i;

    /* impact perf a bit, but not that much. Implementation for palette
       images can be done at a later point.
    */
    if (src->trueColor == 0) {
        gdImagePaletteToTrueColor(src);
    }

    dst = gdImageCreateTrueColor(new_width, new_height);
    if (!dst) {
        return NULL;
    }

    dst->saveAlphaFlag = 1;

    for (i = 0; i < new_height; i++) {
        long j;
        const double src_y = (double)i * dy;
        const long m = gdClampInt((int)src_y, 0, src_h - 1);
        const double f_f = src_y - (double)m;
        dst_offset_x = 0;

        for (j = 0; j < new_width; j++) {
            const double src_x = (double)j * dx;
            const long n = gdClampInt((int)src_x, 0, src_w - 1);
            const double f_g = src_x - (double)n;
            long k;
            double red_acc = 0.0, green_acc = 0.0, blue_acc = 0.0, alpha_acc = 0.0;
            unsigned char red, green, blue, alpha = 0;
            int *dst_row = dst->tpixels[dst_offset_y];

            for (k = -1; k < 3; k++) {
                int l;
                const int src_y_sample = gdClampInt((int)m + (int)k, 0, src_h - 1);
                const double weight_y = filter_cubic_spline((double)k - f_f, 0.0);

                for (l = -1; l < 3; l++) {
                    const int src_x_sample = gdClampInt((int)n + (int)l, 0, src_w - 1);
                    const double weight = weight_y * filter_cubic_spline((double)l - f_g, 0.0);
                    const int c = src->tpixels[src_y_sample][src_x_sample];

                    red_acc += weight * (double)gdTrueColorGetRed(c);
                    green_acc += weight * (double)gdTrueColorGetGreen(c);
                    blue_acc += weight * (double)gdTrueColorGetBlue(c);
                    alpha_acc += weight * (double)gdTrueColorGetAlpha(c);
                }
            }

            red = uchar_clamp(red_acc * gamma, 0xFF);
            green = uchar_clamp(green_acc * gamma, 0xFF);
            blue = uchar_clamp(blue_acc * gamma, 0xFF);
            alpha = uchar_clamp(alpha_acc * gamma, 0x7F);

            *(dst_row + dst_offset_x) = gdTrueColorAlpha(red, green, blue, alpha);

            dst_offset_x++;
        }
        dst_offset_y++;
    }
    return dst;
}

/**
 * Function: gdImageScale
 *
 * Scale an image
 *
 * Creates a new image, scaled to the requested size using the current
 * <gdInterpolationMethod>.
 *
 * Note that GD_WEIGHTED4 is not yet supported by this function.
 *
 * Parameters:
 *   src        - The source image.
 *   new_width  - The new width.
 *   new_height - The new height.
 *
 * Returns:
 *   The scaled image on success, NULL on failure.
 *
 * See also:
 *   - <gdImageCopyResized>
 *   - <gdImageCopyResampled>
 */
BGD_DECLARE(gdImagePtr)
gdImageScale(const gdImagePtr src, const unsigned int new_width, const unsigned int new_height)
{
    gdImagePtr im_scaled = NULL;

    if (src == NULL || (uintmax_t)src->interpolation_id >= GD_METHOD_COUNT) {
        return NULL;
    }

    if (new_width == 0 || new_height == 0) {
        return NULL;
    }
    if ((int)new_width == gdImageSX(src) && (int)new_height == gdImageSY(src)) {
        return gdImageClone(src);
    }
    switch (src->interpolation_id) {
    /*Special cases, optimized implementations */
    case GD_NEAREST_NEIGHBOUR:
        im_scaled = gdImageScaleNearestNeighbour(src, new_width, new_height);
        break;

    case GD_BILINEAR_FIXED:
    case GD_LINEAR:
        im_scaled = gdImageScaleBilinear(src, new_width, new_height);
        break;

    case GD_BICUBIC_FIXED:
    case GD_BICUBIC:
        im_scaled = gdImageScaleBicubicFixed(src, new_width, new_height);
        break;

    /* generic */
    default:
        if (src->interpolation == NULL) {
            return NULL;
        }
        im_scaled = gdImageScaleTwoPass(src, new_width, new_height);
        break;
    }

    return im_scaled;
}

static int gdRotatedImageSize(gdImagePtr src, const float angle, gdRectPtr bbox)
{
    gdRect src_area;
    double m[6];

    gdAffineRotate(m, angle);
    src_area.x = 0;
    src_area.y = 0;
    src_area.width = gdImageSX(src);
    src_area.height = gdImageSY(src);
    if (gdTransformAffineBoundingBox(&src_area, m, bbox) != GD_TRUE) {
        return GD_FALSE;
    }

    return GD_TRUE;
}

static gdImagePtr gdImageRotateNearestNeighbour(gdImagePtr src, const float degrees,
                                                const int bgColor)
{
    double _angle = ((double)(-degrees) / 180.0) * M_PI;
    const int src_w = gdImageSX(src);
    const int src_h = gdImageSY(src);
    const double H = (double)(src_h / 2);
    const double W = (double)(src_w / 2);
    const double cos_angle = cos(-_angle);
    const double sin_angle = sin(-_angle);

    unsigned int dst_offset_x;
    unsigned int dst_offset_y = 0;
    unsigned int i;
    gdImagePtr dst;
    gdRect bbox;
    unsigned int new_height, new_width;

    gdRotatedImageSize(src, degrees, &bbox);
    new_width = bbox.width;
    new_height = bbox.height;

    dst = gdImageCreateTrueColor(new_width, new_height);
    if (!dst) {
        return NULL;
    }
    dst->saveAlphaFlag = 1;
    for (i = 0; i < new_height; i++) {
        unsigned int j;
        dst_offset_x = 0;
        for (j = 0; j < new_width; j++) {
            const double dst_y = (double)((int)i - (int)new_height / 2);
            const double dst_x = (double)((int)j - (int)new_width / 2);
            const double src_y = dst_x * sin_angle + dst_y * cos_angle + H;
            const double src_x = dst_x * cos_angle - dst_y * sin_angle + W;
            long m = (long)floor(src_y + 0.5);
            long n = (long)floor(src_x + 0.5);

            if ((m > 0) && (m < src_h - 1) && (n > 0) && (n < src_w - 1)) {
                if (dst_offset_y < new_height) {
                    dst->tpixels[dst_offset_y][dst_offset_x++] = src->tpixels[m][n];
                }
            } else {
                if (dst_offset_y < new_height) {
                    dst->tpixels[dst_offset_y][dst_offset_x++] = bgColor;
                }
            }
        }
        dst_offset_y++;
    }
    return dst;
}

static gdImagePtr gdImageRotateGeneric(gdImagePtr src, const float degrees, const int bgColor)
{
    double _angle = ((double)(-degrees) / 180.0) * M_PI;
    const int src_w = gdImageSX(src);
    const int src_h = gdImageSY(src);
    const double H = (double)(src_h / 2);
    const double W = (double)(src_w / 2);
    const double cos_angle = cos(-_angle);
    const double sin_angle = sin(-_angle);

    unsigned int dst_offset_x;
    unsigned int dst_offset_y = 0;
    unsigned int i;
    gdImagePtr dst;
    unsigned int new_width, new_height;
    gdRect bbox;

    if (bgColor < 0) {
        return NULL;
    }

    gdRotatedImageSize(src, degrees, &bbox);
    new_width = bbox.width;
    new_height = bbox.height;

    dst = gdImageCreateTrueColor(new_width, new_height);
    if (!dst) {
        return NULL;
    }
    dst->saveAlphaFlag = 1;

    for (i = 0; i < new_height; i++) {
        unsigned int j;
        dst_offset_x = 0;
        for (j = 0; j < new_width; j++) {
            const double dst_y = (double)((int)i - (int)new_height / 2);
            const double dst_x = (double)((int)j - (int)new_width / 2);
            const double src_y = dst_x * sin_angle + dst_y * cos_angle + H;
            const double src_x = dst_x * cos_angle - dst_y * sin_angle + W;
            long m = (long)floor(src_y);
            long n = (long)floor(src_x);

            if (m < -1 || n < -1 || m >= src_h || n >= src_w) {
                dst->tpixels[dst_offset_y][dst_offset_x++] = bgColor;
            } else {
                dst->tpixels[dst_offset_y][dst_offset_x++] =
                    getPixelInterpolated(src, src_x, src_y, bgColor);
            }
        }
        dst_offset_y++;
    }
    return dst;
}

/**
 * Function: gdImageRotateInterpolated
 *
 * Rotate an image
 *
 * Creates a new image, counter-clockwise rotated by the requested angle
 * using the current <gdInterpolationMethod>. Non-square angles will add a
 * border with bgcolor.
 *
 * Parameters:
 *   src     - The source image.
 *   angle   - The angle in degrees.
 *   bgcolor - The color to fill the added background with.
 *
 * Returns:
 *   The rotated image on success, NULL on failure.
 *
 * See also:
 *   - <gdImageCopyRotated>
 */
BGD_DECLARE(gdImagePtr)
gdImageRotateInterpolated(const gdImagePtr src, const float angle, int bgcolor)
{
    /* round to two decimals and keep the 100x multiplication to use it in the
       common square angles case later. Keep the two decimal precisions so
       smaller rotation steps can be done, useful for slow animations, f.e. */
    const int angle_rounded = fmod((int)floorf(angle * 100), 360 * 100);
    gdImagePtr src_tc = src;
    int src_cloned = 0;
    if (src == NULL || bgcolor < 0) {
        return NULL;
    }

    if (!gdImageTrueColor(src)) {
        if (bgcolor < gdMaxColors) {
            bgcolor = gdTrueColorAlpha(src->red[bgcolor], src->green[bgcolor], src->blue[bgcolor],
                                       src->alpha[bgcolor]);
        }
        src_tc = gdImageClone(src);
        gdImagePaletteToTrueColor(src_tc);
        src_cloned = 1;
    }

    /* 0 && 90 degrees multiple rotation, 0 rotation simply clones the return
       image and convert it to truecolor, as we must return truecolor image. */
    switch (angle_rounded) {
    case 0:
        return src_cloned ? src_tc : gdImageClone(src);

    case -27000:
    case 9000:
        if (src_cloned)
            gdImageDestroy(src_tc);
        return gdImageRotate90(src, 0);

    case -18000:
    case 18000:
        if (src_cloned)
            gdImageDestroy(src_tc);
        return gdImageRotate180(src, 0);

    case -9000:
    case 27000:
        if (src_cloned)
            gdImageDestroy(src_tc);
        return gdImageRotate270(src, 0);
    }

    if (src->interpolation_id < 1 || src->interpolation_id > GD_METHOD_COUNT) {
        if (src_cloned)
            gdImageDestroy(src_tc);
        return NULL;
    }

    switch (src->interpolation_id) {
    case GD_NEAREST_NEIGHBOUR: {
        gdImagePtr res = gdImageRotateNearestNeighbour(src_tc, angle, bgcolor);
        if (src_cloned)
            gdImageDestroy(src_tc);
        return res;
    }

    case GD_BILINEAR_FIXED:
    case GD_BICUBIC_FIXED:
    default: {
        gdImagePtr res = gdImageRotateGeneric(src_tc, angle, bgcolor);
        if (src_cloned)
            gdImageDestroy(src_tc);
        return res;
    }
    }
    return NULL;
}

/**
 * Group: Affine Transformation
 **/

static void gdImageClipRectangle(gdImagePtr im, gdRectPtr r)
{
    int c1x, c1y, c2x, c2y;
    int x1, y1;

    gdImageGetClip(im, &c1x, &c1y, &c2x, &c2y);
    x1 = r->x + r->width - 1;
    y1 = r->y + r->height - 1;
    r->x = CLAMP(r->x, c1x, c2x);
    r->y = CLAMP(r->y, c1y, c2y);
    r->width = CLAMP(x1, c1x, c2x) - r->x + 1;
    r->height = CLAMP(y1, c1y, c2y) - r->y + 1;
}

/**
 * Function: gdTransformAffineGetImage
 *  Applies an affine transformation to a region and return an image
 *  containing the complete transformation.
 *
 * Parameters:
 * 	dst - Pointer to a gdImagePtr to store the created image, NULL when
 *        the creation or the transformation failed
 *  src - Source image
 *  src_area - rectangle defining the source region to transform
 *  dstY - Y position in the destination image
 *  affine - The desired affine transformation
 *
 * Returns:
 *  GD_TRUE if the affine is rectilinear or GD_FALSE
 */
BGD_DECLARE(int)
gdTransformAffineGetImage(gdImagePtr *dst, const gdImagePtr src, gdRectPtr src_area,
                          const double affine[6])
{
    int res;
    double m[6];
    gdRect bbox;
    gdRect area_full;

    if (src_area == NULL) {
        area_full.x = 0;
        area_full.y = 0;
        area_full.width = gdImageSX(src);
        area_full.height = gdImageSY(src);
        src_area = &area_full;
    }

    if (gdTransformAffineBoundingBox(src_area, affine, &bbox) != GD_TRUE) {
        *dst = NULL;
        return GD_FALSE;
    }

    *dst = gdImageCreateTrueColor(bbox.width, bbox.height);
    if (*dst == NULL) {
        return GD_FALSE;
    }
    (*dst)->saveAlphaFlag = 1;
    gdImageAlphaBlending(*dst, 0);
    /* The API has no background-color parameter, so uncovered output starts
     * transparent. */
    gdImageFilledRectangle(*dst, 0, 0, bbox.width - 1, bbox.height - 1,
                           gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent));

    if (!src->trueColor) {
        gdImagePaletteToTrueColor(src);
    }

    /* Translate to dst origin (0,0) */
    gdAffineTranslate(m, -bbox.x, -bbox.y);
    gdAffineConcat(m, affine, m);

    res = gdTransformAffineCopy(*dst, 0, 0, src, src_area, m);

    if (res != GD_TRUE) {
        gdImageDestroy(*dst);
        *dst = NULL;
        return GD_FALSE;
    } else {
        return GD_TRUE;
    }
}

/** Function: getPixelRgbInterpolated
 *   get the index of the image's colors
 *
 * Parameters:
 *  im - Image to draw the transformed image
 *  tcolor - TrueColor
 *
 * Return:
 *  index of colors
 */
static int getPixelRgbInterpolated(gdImagePtr im, const int tcolor)
{
    unsigned char r, g, b, a;
    int ct;
    int i;

    b = (unsigned char)(tcolor);
    g = (unsigned char)(tcolor >> 8);
    r = (unsigned char)(tcolor >> 16);
    a = (unsigned char)(tcolor >> 24);

    for (i = 0; i < im->colorsTotal; i++) {
        if (im->red[i] == r && im->green[i] == g && im->blue[i] == b && im->alpha[i] == a) {
            return i;
        }
    }

    ct = im->colorsTotal;
    if (ct == gdMaxColors) {
        return -1;
    }

    im->colorsTotal++;
    im->red[ct] = r;
    im->green[ct] = g;
    im->blue[ct] = b;
    im->alpha[ct] = a;
    im->open[ct] = 0;

    return ct;
}

#define GD_AFFINE_KERNEL_SUPPORT 2.0

typedef struct {
    gdImagePtr src;
    gdRectPtr clip;
    int bgColor;
} gdAffineSampleContext;

static inline int gdAffineSampleIntersectsRegion(const double x, const double y,
                                                 const gdRectPtr region)
{
    const double left = (double)region->x - GD_AFFINE_KERNEL_SUPPORT;
    const double top = (double)region->y - GD_AFFINE_KERNEL_SUPPORT;
    const double right = (double)(region->x + region->width - 1) + GD_AFFINE_KERNEL_SUPPORT;
    const double bottom = (double)(region->y + region->height - 1) + GD_AFFINE_KERNEL_SUPPORT;

    return x >= left && x <= right && y >= top && y <= bottom;
}

static inline int gdAffineSample(const gdAffineSampleContext *ctx, const double x, const double y,
                                 int *color)
{
    int c;

    if (!gdAffineSampleIntersectsRegion(x, y, ctx->clip)) {
        return GD_FALSE;
    }

    c = getPixelInterpolatedClipped(ctx->src, x, y, ctx->bgColor, ctx->clip);
    if (gdTrueColorGetAlpha(c) == gdAlphaTransparent) {
        return GD_FALSE;
    }

    *color = c;
    return GD_TRUE;
}

/**
 * Function: gdTransformAffineCopy
 *  Applies an affine transformation to a region and copy the result
 *  in a destination to the given position.
 *
 * Parameters:
 * 	dst - Image to draw the transformed image
 *  src - Source image
 *  dstX - X position in the destination image
 *  dstY - Y position in the destination image
 *  src_area - Rectangular region to rotate in the src image
 *
 * Returns:
 *  GD_TRUE on success or GD_FALSE on failure
 */
BGD_DECLARE(int)
gdTransformAffineCopy(gdImagePtr dst, int dst_x, int dst_y, const gdImagePtr src,
                      gdRectPtr src_region, const double affine[6])
{
    int backclip = 0;
    int backup_clipx1, backup_clipy1, backup_clipx2, backup_clipy2;
    register int x, y, src_offset_x, src_offset_y;
    double inv[6];
    gdPointF pt, src_pt;
    gdRect bbox;
    int end_x, end_y;
    const int transparent = gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
    gdAffineSampleContext sample_ctx;

    gdImageClipRectangle(src, src_region);

    if (src_region->x > 0 || src_region->y > 0 || src_region->width < gdImageSX(src) ||
        src_region->height < gdImageSY(src)) {
        backclip = 1;

        gdImageGetClip(src, &backup_clipx1, &backup_clipy1, &backup_clipx2, &backup_clipy2);

        gdImageSetClip(src, src_region->x, src_region->y, src_region->x + src_region->width - 1,
                       src_region->y + src_region->height - 1);
    }

    if (!gdTransformAffineBoundingBox(src_region, affine, &bbox)) {
        if (backclip) {
            gdImageSetClip(src, backup_clipx1, backup_clipy1, backup_clipx2, backup_clipy2);
        }
        return GD_FALSE;
    }

    end_x = bbox.width + abs(bbox.x);
    end_y = bbox.height + abs(bbox.y);

    /* Get inverse affine to let us work with destination -> source */
    if (gdAffineInvert(inv, affine) == GD_FALSE) {
        if (backclip) {
            gdImageSetClip(src, backup_clipx1, backup_clipy1, backup_clipx2, backup_clipy2);
        }
        return GD_FALSE;
    }

    src_offset_x = src_region->x;
    src_offset_y = src_region->y;
    sample_ctx.src = src;
    sample_ctx.clip = src_region;
    sample_ctx.bgColor = transparent;

    if (dst->alphaBlendingFlag) {
        for (y = bbox.y; y <= end_y; y++) {
            pt.y = y + 0.5;
            for (x = bbox.x; x <= end_x; x++) {
                pt.x = x + 0.5;
                int c;
                gdAffineApplyToPointF(&src_pt, &pt, inv);
                const double sample_x = src_offset_x + src_pt.x;
                const double sample_y = src_offset_y + src_pt.y;
                if (!gdAffineSample(&sample_ctx, sample_x, sample_y, &c)) {
                    continue;
                }
                gdImageSetPixel(dst, dst_x + x, dst_y + y, c);
            }
        }
    } else {
        for (y = bbox.y; y <= end_y; y++) {
            unsigned char *dst_p = NULL;
            int *tdst_p = NULL;

            pt.y = y + 0.5;
            if ((dst_y + y) < 0 || ((dst_y + y) > gdImageSY(dst) - 1)) {
                continue;
            }
            if (dst->trueColor) {
                tdst_p = dst->tpixels[dst_y + y];
            } else {
                dst_p = dst->pixels[dst_y + y];
            }

            for (x = bbox.x; x <= end_x; x++) {
                pt.x = x + 0.5;
                gdAffineApplyToPointF(&src_pt, &pt, inv);

                if ((dst_x + x) < 0 || (dst_x + x) > (gdImageSX(dst) - 1)) {
                    break;
                }
                {
                    const double sample_x = src_offset_x + src_pt.x;
                    const double sample_y = src_offset_y + src_pt.y;
                    int c;

                    if (!gdAffineSample(&sample_ctx, sample_x, sample_y, &c)) {
                        continue;
                    }
                    if (dst->trueColor) {
                        *(tdst_p + dst_x + x) = c;
                    } else {
                        const int pc = getPixelRgbInterpolated(dst, c);
                        if (pc >= 0) {
                            *(dst_p + dst_x + x) = pc;
                        }
                    }
                }
            }
        }
    }

    /* Restore clip if required */
    if (backclip) {
        gdImageSetClip(src, backup_clipx1, backup_clipy1, backup_clipx2, backup_clipy2);
    }

    return GD_TRUE;
}

/**
 * Function: gdTransformAffineBoundingBox
 *  Returns the bounding box of an affine transformation applied to a
 *  rectangular area <gdRect>
 *
 * Parameters:
 * 	src - Rectangular source area for the affine transformation
 *  affine - the affine transformation
 *  bbox - the resulting bounding box
 *
 * Returns:
 *  GD_TRUE if the affine is rectilinear or GD_FALSE
 */
BGD_DECLARE(int)
gdTransformAffineBoundingBox(gdRectPtr src, const double affine[6], gdRectPtr bbox)
{
    gdPointF extent[4], min, max, point;
    double bbox_x_d, bbox_y_d, bbox_width_d, bbox_height_d;
    int bbox_x, bbox_y, bbox_width, bbox_height;
    int i;

    extent[0].x = 0.0;
    extent[0].y = 0.0;
    extent[1].x = (double)src->width;
    extent[1].y = 0.0;
    extent[2].x = (double)src->width;
    extent[2].y = (double)src->height;
    extent[3].x = 0.0;
    extent[3].y = (double)src->height;

    for (i = 0; i < 4; i++) {
        point = extent[i];
        if (gdAffineApplyToPointF(&extent[i], &point, affine) != GD_TRUE) {
            return GD_FALSE;
        }
    }
    min = extent[0];
    max = extent[0];

    for (i = 1; i < 4; i++) {
        if (min.x > extent[i].x)
            min.x = extent[i].x;
        if (min.y > extent[i].y)
            min.y = extent[i].y;
        if (max.x < extent[i].x)
            max.x = extent[i].x;
        if (max.y < extent[i].y)
            max.y = extent[i].y;
    }
    bbox_x_d = floor(min.x);
    bbox_y_d = floor(min.y);
    bbox_width_d = ceil(max.x) - bbox_x_d;
    bbox_height_d = ceil(max.y) - bbox_y_d;
    if (!isfinite(bbox_x_d) || !isfinite(bbox_y_d) || !isfinite(bbox_width_d) ||
        !isfinite(bbox_height_d) || bbox_x_d < INT_MIN || bbox_x_d > INT_MAX ||
        bbox_y_d < INT_MIN || bbox_y_d > INT_MAX || bbox_width_d < 0.0 || bbox_width_d > INT_MAX ||
        bbox_height_d < 0.0 || bbox_height_d > INT_MAX) {
        return GD_FALSE;
    }
    bbox_x = (int)bbox_x_d;
    bbox_y = (int)bbox_y_d;
    bbox_width = (int)bbox_width_d;
    bbox_height = (int)bbox_height_d;
    if ((bbox_x < 0 && bbox_width > INT_MAX + bbox_x) ||
        (bbox_x > 0 && bbox_width > INT_MAX - bbox_x) ||
        (bbox_y < 0 && bbox_height > INT_MAX + bbox_y) ||
        (bbox_y > 0 && bbox_height > INT_MAX - bbox_y)) {
        return GD_FALSE;
    }
    bbox->x = bbox_x;
    bbox->y = bbox_y;
    bbox->width = bbox_width;
    bbox->height = bbox_height;

    return GD_TRUE;
}

/**
 * Group: Interpolation Method
 */

/**
 * Function: gdImageSetInterpolationMethod
 *
 * Set the interpolation method for subsequent operations
 *
 * Parameters:
 *   im - The image.
 *   id - The interpolation method.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdInterpolationMethod>
 *   - <gdImageGetInterpolationMethod>
 */
BGD_DECLARE(int)
gdImageSetInterpolationMethod(gdImagePtr im, gdInterpolationMethod id)
{
    if (im == NULL || (uintmax_t)id > GD_METHOD_COUNT) {
        return 0;
    }

    switch (id) {
    case GD_NEAREST_NEIGHBOUR:
    case GD_WEIGHTED4:
        im->interpolation = NULL;
        break;

    /* generic versions*/
    /* GD_BILINEAR_FIXED and GD_BICUBIC_FIXED are kept for BC reasons */
    case GD_BILINEAR_FIXED:
    case GD_LINEAR:
        im->interpolation = filter_linear;
        break;
    case GD_BELL:
        im->interpolation = filter_bell;
        break;
    case GD_BESSEL:
        im->interpolation = filter_bessel;
        break;
    case GD_BICUBIC_FIXED:
    case GD_BICUBIC:
        /* no interpolation as gdImageScale calls a dedicated function */
        im->interpolation = NULL;
        break;
    case GD_BLACKMAN:
        im->interpolation = filter_blackman;
        break;
    case GD_BOX:
        im->interpolation = filter_box;
        break;
    case GD_BSPLINE:
        im->interpolation = filter_bspline;
        break;
    case GD_CATMULLROM:
        im->interpolation = filter_catmullrom;
        break;
    case GD_GAUSSIAN:
        im->interpolation = filter_gaussian;
        break;
    case GD_GENERALIZED_CUBIC:
        im->interpolation = filter_generalized_cubic;
        break;
    case GD_HERMITE:
        im->interpolation = filter_hermite;
        break;
    case GD_HAMMING:
        im->interpolation = filter_hamming;
        break;
    case GD_HANNING:
        im->interpolation = filter_hanning;
        break;
    case GD_MITCHELL:
        im->interpolation = filter_mitchell;
        break;
    case GD_POWER:
        im->interpolation = filter_power;
        break;
    case GD_QUADRATIC:
        im->interpolation = filter_quadratic;
        break;
    case GD_SINC:
        im->interpolation = filter_sinc;
        break;
    case GD_TRIANGLE:
        im->interpolation = filter_triangle;
        break;
    case GD_LANCZOS3:
        im->interpolation = filter_lanczos3;
        break;
    case GD_LANCZOS8:
        im->interpolation = filter_lanczos8;
        break;
    case GD_BLACKMAN_BESSEL:
        im->interpolation = filter_blackman_bessel;
        break;
    case GD_BLACKMAN_SINC:
        im->interpolation = filter_blackman_sinc;
        break;
    case GD_QUADRATIC_BSPLINE:
        im->interpolation = filter_quadratic_bspline;
        break;
    case GD_CUBIC_SPLINE:
        im->interpolation = filter_cubic_spline;
        break;
    case GD_COSINE:
        im->interpolation = filter_cosine;
        break;
    case GD_WELSH:
        im->interpolation = filter_welsh;
        break;
    case GD_DEFAULT:
        id = GD_LINEAR;
        im->interpolation = filter_linear;
        break;
    default:
        return 0;
    }
    im->interpolation_id = id;
    return 1;
}

/**
 * Function: gdImageGetInterpolationMethod
 *
 * Get the current interpolation method
 *
 * This is here so that the value can be read via a language or VM with an FFI
 * but no (portable) way to extract the value from the struct.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   The current interpolation method.
 *
 * See also:
 *   - <gdInterpolationMethod>
 *   - <gdImageSetInterpolationMethod>
 */
BGD_DECLARE(gdInterpolationMethod)
gdImageGetInterpolationMethod(gdImagePtr im) { return im->interpolation_id; }

#ifdef _MSC_VER
#pragma optimize("", on)
#endif
