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
 - Add scale support for a portion only of an image (equivalent of copyresized/resampled)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gd.h>
#include "gdhelpers.h"

#ifdef _MSC_VER
# pragma optimize("t", on)
# include <emmintrin.h>
#endif

#ifndef HAVE_FLOORF
# define HAVE_FLOORF 0
#endif
#if HAVE_FLOORF == 0
# ifndef floorf
/* float floorf(float x);*/
#  define floorf(x) ((float)(floor(x)))
# endif
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#define MIN3(a,b,c) ((a)<(b)?(MIN(a,c)):(MIN(b,c)))
#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif
#define MAX3(a,b,c) ((a)<(b)?(MAX(b,c)):(MAX(a,c)))

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/* only used here, let do a generic fixed point integers later if required by other
   part of GD */
typedef long gdFixed;
/* Integer to fixed point */
#define gd_itofx(x) (long)((unsigned long)(x) << 8)

/* Float to fixed point */
#define gd_ftofx(x) (long)((x) * 256)

/*  Double to fixed point */
#define gd_dtofx(x) (long)((x) * 256)

/* Fixed point to integer */
#define gd_fxtoi(x) ((x) >> 8)

/* Fixed point to float */
# define gd_fxtof(x) ((float)(x) / 256)

/* Fixed point to double */
#define gd_fxtod(x) ((double)(x) / 256)

/* Multiply a fixed by a fixed */
#define gd_mulfx(x,y) (((x) * (y)) >> 8)

/* Divide a fixed by a fixed */
#define gd_divfx(x,y) ((long)((unsigned long)(x) << 8) / (y))

typedef struct
{
   double *Weights;  /* Normalized weights of neighboring pixels */
   int Left,Right;   /* Bounds of source pixels window */
} ContributionType;  /* Contirbution information for a single pixel */

typedef struct
{
   ContributionType *ContribRow; /* Row (or column) of contribution weights */
   unsigned int WindowSize,      /* Filter window size (of affecting source pixels) */
				LineLength;      /* Length of line (no. or rows / cols) */
} LineContribType;

/* Each core filter has its own radius */
#define DEFAULT_FILTER_BICUBIC				3.0
#define DEFAULT_FILTER_BOX					0.5
#define DEFAULT_FILTER_GENERALIZED_CUBIC	0.5
#define DEFAULT_FILTER_RADIUS				1.0
#define DEFAULT_LANCZOS8_RADIUS				8.0
#define DEFAULT_LANCZOS3_RADIUS				3.0
#define DEFAULT_HERMITE_RADIUS				1.0
#define DEFAULT_BOX_RADIUS					0.5
#define DEFAULT_TRIANGLE_RADIUS				1.0
#define DEFAULT_BELL_RADIUS					1.5
#define DEFAULT_CUBICSPLINE_RADIUS			2.0
#define DEFAULT_MITCHELL_RADIUS				2.0
#define DEFAULT_COSINE_RADIUS				1.0
#define DEFAULT_CATMULLROM_RADIUS			2.0
#define DEFAULT_QUADRATIC_RADIUS			1.5
#define DEFAULT_QUADRATICBSPLINE_RADIUS		1.5
#define DEFAULT_CUBICCONVOLUTION_RADIUS		3.0
#define DEFAULT_GAUSSIAN_RADIUS				1.0
#define DEFAULT_HANNING_RADIUS				1.0
#define DEFAULT_HAMMING_RADIUS				1.0
#define DEFAULT_SINC_RADIUS					1.0
#define DEFAULT_WELSH_RADIUS				1.0

enum GD_RESIZE_FILTER_TYPE{
	FILTER_DEFAULT          = 0,
	FILTER_BELL,
	FILTER_BESSEL,
	FILTER_BLACKMAN,
	FILTER_BOX,
	FILTER_BSPLINE,
	FILTER_CATMULLROM,
	FILTER_COSINE,
	FILTER_CUBICCONVOLUTION,
	FILTER_CUBICSPLINE,
	FILTER_HERMITE,
	FILTER_LANCZOS3,
	FILTER_LANCZOS8,
	FILTER_MITCHELL,
	FILTER_QUADRATIC,
	FILTER_QUADRATICBSPLINE,
	FILTER_TRIANGLE,
	FILTER_GAUSSIAN,
	FILTER_HANNING,
	FILTER_HAMMING,
	FILTER_SINC,
	FILTER_WELSH,

	FILTER_CALLBACK        = 999
};

typedef enum GD_RESIZE_FILTER_TYPE gdResizeFilterType;

static double KernelBessel_J1(const double x)
{
	double p, q;

	register long i;

	static const double
	Pone[] =
	{
		0.581199354001606143928050809e+21,
		-0.6672106568924916298020941484e+20,
		0.2316433580634002297931815435e+19,
		-0.3588817569910106050743641413e+17,
		0.2908795263834775409737601689e+15,
		-0.1322983480332126453125473247e+13,
		0.3413234182301700539091292655e+10,
		-0.4695753530642995859767162166e+7,
		0.270112271089232341485679099e+4
	},
	Qone[] =
	{
		0.11623987080032122878585294e+22,
		0.1185770712190320999837113348e+20,
		0.6092061398917521746105196863e+17,
		0.2081661221307607351240184229e+15,
		0.5243710262167649715406728642e+12,
		0.1013863514358673989967045588e+10,
		0.1501793594998585505921097578e+7,
		0.1606931573481487801970916749e+4,
		0.1e+1
	};

	p = Pone[8];
	q = Qone[8];
	for (i=7; i >= 0; i--)
	{
		p = p*x*x+Pone[i];
		q = q*x*x+Qone[i];
	}
	return (double)(p/q);
}

static double KernelBessel_P1(const double x)
{
	double p, q;

	register long i;

	static const double
	Pone[] =
	{
		0.352246649133679798341724373e+5,
		0.62758845247161281269005675e+5,
		0.313539631109159574238669888e+5,
		0.49854832060594338434500455e+4,
		0.2111529182853962382105718e+3,
		0.12571716929145341558495e+1
	},
	Qone[] =
	{
		0.352246649133679798068390431e+5,
		0.626943469593560511888833731e+5,
		0.312404063819041039923015703e+5,
		0.4930396490181088979386097e+4,
		0.2030775189134759322293574e+3,
		0.1e+1
	};

	p = Pone[5];
	q = Qone[5];
	for (i=4; i >= 0; i--)
	{
		p = p*(8.0/x)*(8.0/x)+Pone[i];
		q = q*(8.0/x)*(8.0/x)+Qone[i];
	}
	return (double)(p/q);
}

static double KernelBessel_Q1(const double x)
{
	double p, q;

	register long i;

	static const double
	Pone[] =
	{
		0.3511751914303552822533318e+3,
		0.7210391804904475039280863e+3,
		0.4259873011654442389886993e+3,
		0.831898957673850827325226e+2,
		0.45681716295512267064405e+1,
		0.3532840052740123642735e-1
	},
	Qone[] =
	{
		0.74917374171809127714519505e+4,
		0.154141773392650970499848051e+5,
		0.91522317015169922705904727e+4,
		0.18111867005523513506724158e+4,
		0.1038187585462133728776636e+3,
		0.1e+1
	};

	p = Pone[5];
	q = Qone[5];
	for (i=4; i >= 0; i--)
	{
		p = p*(8.0/x)*(8.0/x)+Pone[i];
		q = q*(8.0/x)*(8.0/x)+Qone[i];
	}
	return (double)(p/q);
}

static double KernelBessel_Order1(double x)
{
	double p, q;

	if (x == 0.0)
		return (0.0f);
	p = x;
	if (x < 0.0)
		x=(-x);
	if (x < 8.0)
		return (p*KernelBessel_J1(x));
	q = (double)sqrt(2.0f/(M_PI*x))*(double)(KernelBessel_P1(x)*(1.0f/sqrt(2.0f)*(sin(x)-cos(x)))-8.0f/x*KernelBessel_Q1(x)*
		(-1.0f/sqrt(2.0f)*(sin(x)+cos(x))));
	if (p < 0.0f)
		q = (-q);
	return (q);
}

static double filter_bessel(const double x)
{
	if (x == 0.0f)
		return (double)(M_PI/4.0f);
	return (KernelBessel_Order1((double)M_PI*x)/(2.0f*x));
}


static double filter_blackman(const double x)
{
	return (0.42f+0.5f*(double)cos(M_PI*x)+0.08f*(double)cos(2.0f*M_PI*x));
}

/**
 * Bicubic interpolation kernel (a=-1):
  \verbatim
          /
         | 1-2|t|**2+|t|**3          , if |t| < 1
  h(t) = | 4-8|t|+5|t|**2-|t|**3     , if 1<=|t|<2
         | 0                         , otherwise
          \
  \endverbatim
 * ***bd*** 2.2004
 */
static double filter_bicubic(const double t)
{
  const double abs_t = (double)fabs(t);
  const double abs_t_sq = abs_t * abs_t;
  if (abs_t<1) return 1-2*abs_t_sq+abs_t_sq*abs_t;
  if (abs_t<2) return 4 - 8*abs_t +5*abs_t_sq - abs_t_sq*abs_t;
  return 0;
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
static double filter_generalized_cubic(const double t)
{
	const double a = -DEFAULT_FILTER_GENERALIZED_CUBIC;
	double abs_t = (double)fabs(t);
	double abs_t_sq = abs_t * abs_t;
	if (abs_t < 1) return (a + 2) * abs_t_sq * abs_t - (a + 3) * abs_t_sq + 1;
	if (abs_t < 2) return a * abs_t_sq * abs_t - 5 * a * abs_t_sq + 8 * a * abs_t - 4 * a;
	return 0;
}

#ifdef FUNCTION_NOT_USED_YET
/* CubicSpline filter, default radius 2 */
static double filter_cubic_spline(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x < 1.0 ) {
		const double x2 = x*x;

		return (0.5 * x2 * x - x2 + 2.0 / 3.0);
	}
	if (x < 2.0) {
		return (pow(2.0 - x, 3.0)/6.0);
	}
	return 0;
}
#endif

#ifdef FUNCTION_NOT_USED_YET
/* CubicConvolution filter, default radius 3 */
static double filter_cubic_convolution(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
	const double x2 = x1 * x1;
	const double x2_x = x2 * x;

	if (x <= 1.0) return ((4.0 / 3.0)* x2_x - (7.0 / 3.0) * x2 + 1.0);
	if (x <= 2.0) return (- (7.0 / 12.0) * x2_x + 3 * x2 - (59.0 / 12.0) * x + 2.5);
	if (x <= 3.0) return ( (1.0/12.0) * x2_x - (2.0 / 3.0) * x2 + 1.75 * x - 1.5);
	return 0;
}
#endif

static double filter_box(double x) {
	if (x < - DEFAULT_FILTER_BOX)
		return 0.0f;
	if (x < DEFAULT_FILTER_BOX)
		return 1.0f;
	return 0.0f;
}

static double filter_catmullrom(const double x)
{
	if (x < -2.0)
		return(0.0f);
	if (x < -1.0)
		return(0.5f*(4.0f+x*(8.0f+x*(5.0f+x))));
	if (x < 0.0)
		return(0.5f*(2.0f+x*x*(-5.0f-3.0f*x)));
	if (x < 1.0)
		return(0.5f*(2.0f+x*x*(-5.0f+3.0f*x)));
	if (x < 2.0)
		return(0.5f*(4.0f+x*(-8.0f+x*(5.0f-x))));
	return(0.0f);
}

#ifdef FUNCTION_NOT_USED_YET
static double filter_filter(double t)
{
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}
#endif

#ifdef FUNCTION_NOT_USED_YET
/* Lanczos8 filter, default radius 8 */
static double filter_lanczos8(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
#define R DEFAULT_LANCZOS8_RADIUS

	if ( x == 0.0) return 1;

	if ( x < R) {
		return R * sin(x*M_PI) * sin(x * M_PI/ R) / (x * M_PI * x * M_PI);
	}
	return 0.0;
#undef R
}
#endif

#ifdef FUNCTION_NOT_USED_YET
/* Lanczos3 filter, default radius 3 */
static double filter_lanczos3(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
#define R DEFAULT_LANCZOS3_RADIUS

	if ( x == 0.0) return 1;

	if ( x < R)
	{
		return R * sin(x*M_PI) * sin(x * M_PI / R) / (x * M_PI * x * M_PI);
	}
	return 0.0;
#undef R
}
#endif

/* Hermite filter, default radius 1 */
static double filter_hermite(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x < 1.0) return ((2.0 * x - 3) * x * x + 1.0 );

	return 0.0;
}

/* Trangle filter, default radius 1 */
static double filter_triangle(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
	if (x < 1.0) return (1.0 - x);
	return 0.0;
}

/* Bell filter, default radius 1.5 */
static double filter_bell(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x < 0.5) return (0.75 - x*x);
	if (x < 1.5) return (0.5 * pow(x - 1.5, 2.0));
	return 0.0;
}

/* Mitchell filter, default radius 2.0 */
static double filter_mitchell(const double x)
{
#define KM_B (1.0f/3.0f)
#define KM_C (1.0f/3.0f)
#define KM_P0 ((  6.0f - 2.0f * KM_B ) / 6.0f)
#define KM_P2 ((-18.0f + 12.0f * KM_B + 6.0f * KM_C) / 6.0f)
#define KM_P3 (( 12.0f - 9.0f  * KM_B - 6.0f * KM_C) / 6.0f)
#define KM_Q0 ((  8.0f * KM_B + 24.0f * KM_C) / 6.0f)
#define KM_Q1 ((-12.0f * KM_B - 48.0f * KM_C) / 6.0f)
#define KM_Q2 ((  6.0f * KM_B + 30.0f * KM_C) / 6.0f)
#define KM_Q3 (( -1.0f * KM_B -  6.0f * KM_C) / 6.0f)

	if (x < -2.0)
		return(0.0f);
	if (x < -1.0)
		return(KM_Q0-x*(KM_Q1-x*(KM_Q2-x*KM_Q3)));
	if (x < 0.0f)
		return(KM_P0+x*x*(KM_P2-x*KM_P3));
	if (x < 1.0f)
		return(KM_P0+x*x*(KM_P2+x*KM_P3));
	if (x < 2.0f)
		return(KM_Q0+x*(KM_Q1+x*(KM_Q2+x*KM_Q3)));
	return(0.0f);
}



#ifdef FUNCTION_NOT_USED_YET
/* Cosine filter, default radius 1 */
static double filter_cosine(const double x)
{
	if ((x >= -1.0) && (x <= 1.0)) return ((cos(x * M_PI) + 1.0)/2.0);

	return 0;
}
#endif

/* Quadratic filter, default radius 1.5 */
static double filter_quadratic(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x <= 0.5) return (- 2.0 * x * x + 1);
	if (x <= 1.5) return (x * x - 2.5* x + 1.5);
	return 0.0;
}

static double filter_bspline(const double x)
{
	if (x>2.0f) {
		return 0.0f;
	} else {
		double a, b, c, d;
		/* Was calculated anyway cause the "if((x-1.0f) < 0)" */
		const double xm1 = x - 1.0f;
		const double xp1 = x + 1.0f;
		const double xp2 = x + 2.0f;

		if ((xp2) <= 0.0f) a = 0.0f; else a = xp2*xp2*xp2;
		if ((xp1) <= 0.0f) b = 0.0f; else b = xp1*xp1*xp1;
		if (x <= 0) c = 0.0f; else c = x*x*x;
		if ((xm1) <= 0.0f) d = 0.0f; else d = xm1*xm1*xm1;

		return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));
	}
}

#ifdef FUNCTION_NOT_USED_YET
/* QuadraticBSpline filter, default radius 1.5 */
static double filter_quadratic_bspline(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x <= 0.5) return (- x * x + 0.75);
	if (x <= 1.5) return (0.5 * x * x - 1.5 * x + 1.125);
	return 0.0;
}
#endif

static double filter_gaussian(const double x)
{
	/* return(exp((double) (-2.0 * x * x)) * sqrt(2.0 / M_PI)); */
	return (double)(exp(-2.0f * x * x) * 0.79788456080287f);
}

static double filter_hanning(const double x)
{
	/* A Cosine windowing function */
	return(0.5 + 0.5 * cos(M_PI * x));
}

static double filter_hamming(const double x)
{
	/* should be
	(0.54+0.46*cos(M_PI*(double) x));
	but this approximation is sufficient */
	if (x < -1.0f)
		return 0.0f;
	if (x < 0.0f)
		return 0.92f*(-2.0f*x-3.0f)*x*x+1.0f;
	if (x < 1.0f)
		return 0.92f*(2.0f*x-3.0f)*x*x+1.0f;
	return 0.0f;
}

static double filter_power(const double x)
{
	const double a = 2.0f;
	if (fabs(x)>1) return 0.0f;
	return (1.0f - (double)fabs(pow(x,a)));
}

static double filter_sinc(const double x)
{
	/* X-scaled Sinc(x) function. */
	if (x == 0.0) return(1.0);
	return (sin(M_PI * (double) x) / (M_PI * (double) x));
}

#ifdef FUNCTION_NOT_USED_YET
static double filter_welsh(const double x)
{
	/* Welsh parabolic windowing filter */
	if (x <  1.0)
		return(1 - x*x);
	return(0.0);
}
#endif

/* Copied from upstream's libgd */
static inline int _color_blend (const int dst, const int src)
{
    const int src_alpha = gdTrueColorGetAlpha(src);

    if( src_alpha == gdAlphaOpaque ) {
		return src;
	} else {
		const int dst_alpha = gdTrueColorGetAlpha(dst);

		if( src_alpha == gdAlphaTransparent ) return dst;
		if( dst_alpha == gdAlphaTransparent ) {
			return src;
		} else {
			register int alpha, red, green, blue;
			const int src_weight = gdAlphaTransparent - src_alpha;
			const int dst_weight = (gdAlphaTransparent - dst_alpha) * src_alpha / gdAlphaMax;
			const int tot_weight = src_weight + dst_weight;

			alpha = src_alpha * dst_alpha / gdAlphaMax;

			red = (gdTrueColorGetRed(src) * src_weight
				   + gdTrueColorGetRed(dst) * dst_weight) / tot_weight;
			green = (gdTrueColorGetGreen(src) * src_weight
				   + gdTrueColorGetGreen(dst) * dst_weight) / tot_weight;
			blue = (gdTrueColorGetBlue(src) * src_weight
				   + gdTrueColorGetBlue(dst) * dst_weight) / tot_weight;

			return ((alpha << 24) + (red << 16) + (green << 8) + blue);
		}
	}
}

static inline int getPixelOverflowTC(gdImagePtr im, const int x, const int y, const int bgColor)
{
	if (gdImageBoundsSafe(im, x, y)) {
		const int c = im->tpixels[y][x];
		if (c == im->transparent) {
			return bgColor == -1 ? gdTrueColorAlpha(0, 0, 0, 127) : bgColor;
		}
		return c;
	} else {
		return bgColor;
	}
}

#define colorIndex2RGBA(c) gdTrueColorAlpha(im->red[(c)], im->green[(c)], im->blue[(c)], im->alpha[(c)])
#define colorIndex2RGBcustomA(c, a) gdTrueColorAlpha(im->red[(c)], im->green[(c)], im->blue[(c)], im->alpha[(a)])
static inline int getPixelOverflowPalette(gdImagePtr im, const int x, const int y, const int bgColor)
{
	if (gdImageBoundsSafe(im, x, y)) {
		const int c = im->pixels[y][x];
		if (c == im->transparent) {
			return bgColor == -1 ? gdTrueColorAlpha(0, 0, 0, 127) : bgColor;
		}
		return colorIndex2RGBA(c);
	} else {
		return bgColor;
	}
}

static int getPixelInterpolateWeight(gdImagePtr im, const double x, const double y, const int bgColor)
{
	/* Closest pixel <= (xf,yf) */
	int sx = (int)(x);
	int sy = (int)(y);
	const double xf = x - (double)sx;
	const double yf = y - (double)sy;
	const double nxf = (double) 1.0 - xf;
	const double nyf = (double) 1.0 - yf;
	const double m1 = xf * yf;
	const double m2 = nxf * yf;
	const double m3 = xf * nyf;
	const double m4 = nxf * nyf;

	/* get color values of neighbouring pixels */
	const int c1 = im->trueColor == 1 ? getPixelOverflowTC(im, sx, sy, bgColor)         : getPixelOverflowPalette(im, sx, sy, bgColor);
	const int c2 = im->trueColor == 1 ? getPixelOverflowTC(im, sx - 1, sy, bgColor)     : getPixelOverflowPalette(im, sx - 1, sy, bgColor);
	const int c3 = im->trueColor == 1 ? getPixelOverflowTC(im, sx, sy - 1, bgColor)     : getPixelOverflowPalette(im, sx, sy - 1, bgColor);
	const int c4 = im->trueColor == 1 ? getPixelOverflowTC(im, sx - 1, sy - 1, bgColor) : getPixelOverflowPalette(im, sx, sy - 1, bgColor);
	int r, g, b, a;

	if (x < 0) sx--;
	if (y < 0) sy--;

	/* component-wise summing-up of color values */
	if (im->trueColor) {
		r = (int)(m1*gdTrueColorGetRed(c1)   + m2*gdTrueColorGetRed(c2)   + m3*gdTrueColorGetRed(c3)   + m4*gdTrueColorGetRed(c4));
		g = (int)(m1*gdTrueColorGetGreen(c1) + m2*gdTrueColorGetGreen(c2) + m3*gdTrueColorGetGreen(c3) + m4*gdTrueColorGetGreen(c4));
		b = (int)(m1*gdTrueColorGetBlue(c1)  + m2*gdTrueColorGetBlue(c2)  + m3*gdTrueColorGetBlue(c3)  + m4*gdTrueColorGetBlue(c4));
		a = (int)(m1*gdTrueColorGetAlpha(c1) + m2*gdTrueColorGetAlpha(c2) + m3*gdTrueColorGetAlpha(c3) + m4*gdTrueColorGetAlpha(c4));
	} else {
		r = (int)(m1*im->red[(c1)]   + m2*im->red[(c2)]   + m3*im->red[(c3)]   + m4*im->red[(c4)]);
		g = (int)(m1*im->green[(c1)] + m2*im->green[(c2)] + m3*im->green[(c3)] + m4*im->green[(c4)]);
		b = (int)(m1*im->blue[(c1)]  + m2*im->blue[(c2)]  + m3*im->blue[(c3)]  + m4*im->blue[(c4)]);
		a = (int)(m1*im->alpha[(c1)] + m2*im->alpha[(c2)] + m3*im->alpha[(c3)] + m4*im->alpha[(c4)]);
	}

	r = CLAMP(r, 0, 255);
	g = CLAMP(g, 0, 255);
	b = CLAMP(b, 0, 255);
	a = CLAMP(a, 0, gdAlphaMax);
	return gdTrueColorAlpha(r, g, b, a);
}

/**
 * Function: getPixelInterpolated
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
 *  GD_TRUE if the affine is rectilinear or GD_FALSE
 *
 * See also:
 *  <gdSetInterpolationMethod>
 */
int getPixelInterpolated(gdImagePtr im, const double x, const double y, const int bgColor)
{
	const int xi=(int)((x) < 0 ? x - 1: x);
	const int yi=(int)((y) < 0 ? y - 1: y);
	int yii;
	int i;
	double kernel, kernel_cache_y;
	double kernel_x[12], kernel_y[4];
	double new_r = 0.0f, new_g = 0.0f, new_b = 0.0f, new_a = 0.0f;

	/* These methods use special implementations */
	if (im->interpolation_id == GD_BILINEAR_FIXED || im->interpolation_id == GD_BICUBIC_FIXED || im->interpolation_id == GD_NEAREST_NEIGHBOUR) {
		return -1;
	}

	if (im->interpolation_id == GD_WEIGHTED4) {
		return getPixelInterpolateWeight(im, x, y, bgColor);
	}

	if (im->interpolation_id == GD_NEAREST_NEIGHBOUR) {
		if (im->trueColor == 1) {
			return getPixelOverflowTC(im, xi, yi, bgColor);
		} else {
			return getPixelOverflowPalette(im, xi, yi, bgColor);
		}
	}
	if (im->interpolation) {
		for (i=0; i<4; i++) {
			kernel_x[i] = (double) im->interpolation((double)(xi+i-1-x));
			kernel_y[i] = (double) im->interpolation((double)(yi+i-1-y));
		}
	} else {
		return -1;
	}

	/*
	 * TODO: use the known fast rgba multiplication implementation once
	 * the new formats are in place
	 */
	for (yii = yi-1; yii < yi+3; yii++) {
		int xii;
		kernel_cache_y = kernel_y[yii-(yi-1)];
		if (im->trueColor) {
			for (xii=xi-1; xii<xi+3; xii++) {
				const int rgbs = getPixelOverflowTC(im, xii, yii, bgColor);

				kernel = kernel_cache_y * kernel_x[xii-(xi-1)];
				new_r += kernel * gdTrueColorGetRed(rgbs);
				new_g += kernel * gdTrueColorGetGreen(rgbs);
				new_b += kernel * gdTrueColorGetBlue(rgbs);
				new_a += kernel * gdTrueColorGetAlpha(rgbs);
			}
		} else {
			for (xii=xi-1; xii<xi+3; xii++) {
				const int rgbs = getPixelOverflowPalette(im, xii, yii, bgColor);

				kernel = kernel_cache_y * kernel_x[xii-(xi-1)];
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

	return gdTrueColorAlpha(((int)new_r), ((int)new_g), ((int)new_b), ((int)new_a));
}

static inline LineContribType * _gdContributionsAlloc(unsigned int line_length, unsigned int windows_size)
{
	unsigned int u = 0;
	LineContribType *res;
	size_t weights_size;

	if (overflow2(windows_size, sizeof(double))) {
		return NULL;
	} else {
		weights_size = windows_size * sizeof(double);
	}
	res = (LineContribType *) gdMalloc(sizeof(LineContribType));
	if (!res) {
		return NULL;
	}
	res->WindowSize = windows_size;
	res->LineLength = line_length;
	if (overflow2(line_length, sizeof(ContributionType))) {
		gdFree(res);
		return NULL;
	}
	res->ContribRow = (ContributionType *) gdMalloc(line_length * sizeof(ContributionType));
	if (res->ContribRow == NULL) {
		gdFree(res);
		return NULL;
	}
	for (u = 0 ; u < line_length ; u++) {
		res->ContribRow[u].Weights = (double *) gdMalloc(weights_size);
		if (res->ContribRow[u].Weights == NULL) {
			unsigned int i;
			for (i=0;i<u;i++) {
				gdFree(res->ContribRow[i].Weights);
			}
			gdFree(res->ContribRow);
			gdFree(res);
			return NULL;
		}
	}
	return res;
}

static inline void _gdContributionsFree(LineContribType * p)
{
	unsigned int u;
	for (u = 0; u < p->LineLength; u++)  {
		gdFree(p->ContribRow[u].Weights);
	}
	gdFree(p->ContribRow);
	gdFree(p);
}

static inline LineContribType *_gdContributionsCalc(unsigned int line_size, unsigned int src_size, double scale_d,  const interpolation_method pFilter)
{
	double width_d;
	double scale_f_d = 1.0;
	const double filter_width_d = DEFAULT_BOX_RADIUS;
	int windows_size;
	unsigned int u;
	LineContribType *res;

	if (scale_d < 1.0) {
		width_d = filter_width_d / scale_d;
		scale_f_d = scale_d;
	}  else {
		width_d= filter_width_d;
	}

	windows_size = 2 * (int)ceil(width_d) + 1;
	res = _gdContributionsAlloc(line_size, windows_size);
	if (res == NULL) {
		return NULL;
	}
	for (u = 0; u < line_size; u++) {
	const double dCenter = (double)u / scale_d;
	/* get the significant edge points affecting the pixel */
	register int iLeft = MAX(0, (int)floor (dCenter - width_d));
	int iRight = MIN((int)ceil(dCenter + width_d), (int)src_size - 1);
	double dTotalWeight = 0.0;
		int iSrc;

	/* Cut edge points to fit in filter window in case of spill-off */
	if (iRight - iLeft + 1 > windows_size)  {
		if (iLeft < ((int)src_size - 1 / 2))  {
			iLeft++;
		} else {
			iRight--;
		}
	}

	res->ContribRow[u].Left = iLeft;
	res->ContribRow[u].Right = iRight;

	for (iSrc = iLeft; iSrc <= iRight; iSrc++) {
		dTotalWeight += (res->ContribRow[u].Weights[iSrc-iLeft] =  scale_f_d * (*pFilter)(scale_f_d * (dCenter - (double)iSrc)));
	}

		if (dTotalWeight < 0.0) {
			_gdContributionsFree(res);
			return NULL;
		}

	if (dTotalWeight > 0.0) {
		for (iSrc = iLeft; iSrc <= iRight; iSrc++) {
			res->ContribRow[u].Weights[iSrc-iLeft] /= dTotalWeight;
		}
	}
	}
	return res;
}

static inline void _gdScaleRow(gdImagePtr pSrc,  unsigned int src_width, gdImagePtr dst, unsigned int dst_width, unsigned int row, LineContribType *contrib)
{
    int *p_src_row = pSrc->tpixels[row];
    int *p_dst_row = dst->tpixels[row];
	unsigned int x;

    for (x = 0; x < dst_width; x++) {
		register unsigned char r = 0, g = 0, b = 0, a = 0;
        const int left = contrib->ContribRow[x].Left;
        const int right = contrib->ContribRow[x].Right;
		int i;

		/* Accumulate each channel */
        for (i = left; i <= right; i++) {
			const int left_channel = i - left;
            r += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetRed(p_src_row[i])));
            g += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetGreen(p_src_row[i])));
            b += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetBlue(p_src_row[i])));
			a += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetAlpha(p_src_row[i])));
        }
        p_dst_row[x] = gdTrueColorAlpha(r, g, b, a);
    }
}

static inline int _gdScaleHoriz(gdImagePtr pSrc, unsigned int src_width, unsigned int src_height, gdImagePtr pDst,  unsigned int dst_width, unsigned int dst_height)
{
	unsigned int u;
	LineContribType * contrib;

	/* same width, just copy it */
	if (dst_width == src_width) {
		unsigned int y;
		for (y = 0; y < src_height - 1; ++y) {
			memcpy(pDst->tpixels[y], pSrc->tpixels[y], src_width);
		}
	}

	contrib = _gdContributionsCalc(dst_width, src_width, (double)dst_width / (double)src_width, pSrc->interpolation);
	if (contrib == NULL) {
		return 0;
	}
	/* Scale each row */
	for (u = 0; u < dst_height; u++) {
		_gdScaleRow(pSrc, src_width, pDst, dst_width, u, contrib);
	}
	_gdContributionsFree (contrib);
	return 1;
}

static inline void _gdScaleCol (gdImagePtr pSrc,  unsigned int src_width, gdImagePtr pRes, unsigned int dst_width, unsigned int dst_height, unsigned int uCol, LineContribType *contrib)
{
	unsigned int y;
	for (y = 0; y < dst_height; y++) {
		register unsigned char r = 0, g = 0, b = 0, a = 0;
		const int iLeft = contrib->ContribRow[y].Left;
		const int iRight = contrib->ContribRow[y].Right;
		int i;

		/* Accumulate each channel */
		for (i = iLeft; i <= iRight; i++) {
			const int pCurSrc = pSrc->tpixels[i][uCol];
			const int i_iLeft = i - iLeft;
			r += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetRed(pCurSrc)));
			g += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetGreen(pCurSrc)));
			b += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetBlue(pCurSrc)));
			a += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetAlpha(pCurSrc)));
		}
		pRes->tpixels[y][uCol] = gdTrueColorAlpha(r, g, b, a);
	}
}

static inline int _gdScaleVert (const gdImagePtr pSrc, const unsigned int src_width, const unsigned int src_height, const gdImagePtr pDst, const unsigned int dst_width, const unsigned int dst_height)
{
	unsigned int u;
	LineContribType * contrib;

	/* same height, copy it */
	if (src_height == dst_height) {
		unsigned int y;
		for (y = 0; y < src_height - 1; ++y) {
			memcpy(pDst->tpixels[y], pSrc->tpixels[y], src_width);
		}
	}

	contrib = _gdContributionsCalc(dst_height, src_height, (double)(dst_height) / (double)(src_height), pSrc->interpolation);
	if (contrib == NULL) {
		return 0;
	}
	/* scale each column */
	for (u = 0; u < dst_width; u++) {
		_gdScaleCol(pSrc, src_width, pDst, dst_width, dst_height, u, contrib);
	}
	_gdContributionsFree(contrib);
	return 1;
}

gdImagePtr gdImageScaleTwoPass(const gdImagePtr src, const unsigned int src_width, const unsigned int src_height, const unsigned int new_width, const unsigned int new_height)
{
	gdImagePtr tmp_im;
	gdImagePtr dst;
	int scale_pass_res;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	/* Convert to truecolor if it isn't; this code requires it. */
	if (!src->trueColor) {
		gdImagePaletteToTrueColor(src);
	}

	tmp_im = gdImageCreateTrueColor(new_width, src_height);
	if (tmp_im == NULL) {
		return NULL;
	}
	gdImageSetInterpolationMethod(tmp_im, src->interpolation_id);
	scale_pass_res = _gdScaleHoriz(src, src_width, src_height, tmp_im, new_width, src_height);
	if (scale_pass_res != 1) {
		gdImageDestroy(tmp_im);
		return NULL;
	}

	dst = gdImageCreateTrueColor(new_width, new_height);
	if (dst == NULL) {
		gdImageDestroy(tmp_im);
		return NULL;
	}
	gdImageSetInterpolationMethod(dst, src->interpolation_id);
	scale_pass_res = _gdScaleVert(tmp_im, new_width, src_height, dst, new_width, new_height);
	if (scale_pass_res != 1) {
		gdImageDestroy(dst);
		gdImageDestroy(tmp_im);
		return NULL;
	}
	gdImageDestroy(tmp_im);

	return dst;
}

/*
	BilinearFixed, BicubicFixed and nearest implementations are rewamped versions of the implementation in CBitmapEx
	http://www.codeproject.com/Articles/29121/CBitmapEx-Free-C-Bitmap-Manipulation-Class
	Integer only implementation, good to have for common usages like pre scale very large
	images before using another interpolation methods for the last step.
*/
gdImagePtr gdImageScaleNearestNeighbour(gdImagePtr im, const unsigned int width, const unsigned int height)
{
	const unsigned long new_width = MAX(1, width);
	const unsigned long new_height = MAX(1, height);
	const float dx = (float)im->sx / (float)new_width;
	const float dy = (float)im->sy / (float)new_height;
	const gdFixed f_dx = gd_ftofx(dx);
	const gdFixed f_dy = gd_ftofx(dy);

	gdImagePtr dst_img;
	unsigned long  dst_offset_x;
	unsigned long  dst_offset_y = 0;
	unsigned int i;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	dst_img = gdImageCreateTrueColor(new_width, new_height);

	if (dst_img == NULL) {
		return NULL;
	}

	for (i=0; i<new_height; i++) {
		unsigned int j;
		dst_offset_x = 0;
		if (im->trueColor) {
			for (j=0; j<new_width; j++) {
				const gdFixed f_i = gd_itofx(i);
				const gdFixed f_j = gd_itofx(j);
				const gdFixed f_a = gd_mulfx(f_i, f_dy);
				const gdFixed f_b = gd_mulfx(f_j, f_dx);
				const long m = gd_fxtoi(f_a);
				const long n = gd_fxtoi(f_b);

				dst_img->tpixels[dst_offset_y][dst_offset_x++] = im->tpixels[m][n];
			}
		} else {
			for (j=0; j<new_width; j++) {
				const gdFixed f_i = gd_itofx(i);
				const gdFixed f_j = gd_itofx(j);
				const gdFixed f_a = gd_mulfx(f_i, f_dy);
				const gdFixed f_b = gd_mulfx(f_j, f_dx);
				const long m = gd_fxtoi(f_a);
				const long n = gd_fxtoi(f_b);

				dst_img->tpixels[dst_offset_y][dst_offset_x++] = colorIndex2RGBA(im->pixels[m][n]);
			}
		}
		dst_offset_y++;
	}
	return dst_img;
}

static gdImagePtr gdImageScaleBilinearPalette(gdImagePtr im, const unsigned int new_width, const unsigned int new_height)
{
	long _width = MAX(1, new_width);
	long _height = MAX(1, new_height);
	float dx = (float)gdImageSX(im) / (float)_width;
	float dy = (float)gdImageSY(im) / (float)_height;
	gdFixed f_dx = gd_ftofx(dx);
	gdFixed f_dy = gd_ftofx(dy);
	gdFixed f_1 = gd_itofx(1);

	int dst_offset_h;
	int dst_offset_v = 0;
	long i;
	gdImagePtr new_img;
	const int transparent = im->transparent;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	new_img = gdImageCreateTrueColor(new_width, new_height);
	if (new_img == NULL) {
		return NULL;
	}

	if (transparent < 0) {
		/* uninitialized */
		new_img->transparent = -1;
	} else {
		new_img->transparent = gdTrueColorAlpha(im->red[transparent], im->green[transparent], im->blue[transparent], im->alpha[transparent]);
	}

	for (i=0; i < _height; i++) {
		long j;
		const gdFixed f_i = gd_itofx(i);
		const gdFixed f_a = gd_mulfx(f_i, f_dy);
		register long m = gd_fxtoi(f_a);

		dst_offset_h = 0;

		for (j=0; j < _width; j++) {
			/* Update bitmap */
			gdFixed f_j = gd_itofx(j);
			gdFixed f_b = gd_mulfx(f_j, f_dx);

			const long n = gd_fxtoi(f_b);
			gdFixed f_f = f_a - gd_itofx(m);
			gdFixed f_g = f_b - gd_itofx(n);

			const gdFixed f_w1 = gd_mulfx(f_1-f_f, f_1-f_g);
			const gdFixed f_w2 = gd_mulfx(f_1-f_f, f_g);
			const gdFixed f_w3 = gd_mulfx(f_f, f_1-f_g);
			const gdFixed f_w4 = gd_mulfx(f_f, f_g);
			unsigned int pixel1;
			unsigned int pixel2;
			unsigned int pixel3;
			unsigned int pixel4;
			register gdFixed f_r1, f_r2, f_r3, f_r4,
					f_g1, f_g2, f_g3, f_g4,
					f_b1, f_b2, f_b3, f_b4,
					f_a1, f_a2, f_a3, f_a4;

			/* 0 for bgColor; (n,m) is supposed to be valid anyway */
			pixel1 = getPixelOverflowPalette(im, n, m, 0);
			pixel2 = getPixelOverflowPalette(im, n + 1, m, pixel1);
			pixel3 = getPixelOverflowPalette(im, n, m + 1, pixel1);
			pixel4 = getPixelOverflowPalette(im, n + 1, m + 1, pixel1);

			f_r1 = gd_itofx(gdTrueColorGetRed(pixel1));
			f_r2 = gd_itofx(gdTrueColorGetRed(pixel2));
			f_r3 = gd_itofx(gdTrueColorGetRed(pixel3));
			f_r4 = gd_itofx(gdTrueColorGetRed(pixel4));
			f_g1 = gd_itofx(gdTrueColorGetGreen(pixel1));
			f_g2 = gd_itofx(gdTrueColorGetGreen(pixel2));
			f_g3 = gd_itofx(gdTrueColorGetGreen(pixel3));
			f_g4 = gd_itofx(gdTrueColorGetGreen(pixel4));
			f_b1 = gd_itofx(gdTrueColorGetBlue(pixel1));
			f_b2 = gd_itofx(gdTrueColorGetBlue(pixel2));
			f_b3 = gd_itofx(gdTrueColorGetBlue(pixel3));
			f_b4 = gd_itofx(gdTrueColorGetBlue(pixel4));
			f_a1 = gd_itofx(gdTrueColorGetAlpha(pixel1));
			f_a2 = gd_itofx(gdTrueColorGetAlpha(pixel2));
			f_a3 = gd_itofx(gdTrueColorGetAlpha(pixel3));
			f_a4 = gd_itofx(gdTrueColorGetAlpha(pixel4));

			{
				const unsigned char red = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_r1) + gd_mulfx(f_w2, f_r2) + gd_mulfx(f_w3, f_r3) + gd_mulfx(f_w4, f_r4));
				const unsigned char green = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_g1) + gd_mulfx(f_w2, f_g2) + gd_mulfx(f_w3, f_g3) + gd_mulfx(f_w4, f_g4));
				const unsigned char blue = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_b1) + gd_mulfx(f_w2, f_b2) + gd_mulfx(f_w3, f_b3) + gd_mulfx(f_w4, f_b4));
				const unsigned char alpha = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_a1) + gd_mulfx(f_w2, f_a2) + gd_mulfx(f_w3, f_a3) + gd_mulfx(f_w4, f_a4));

				new_img->tpixels[dst_offset_v][dst_offset_h] = gdTrueColorAlpha(red, green, blue, alpha);
			}

			dst_offset_h++;
		}

		dst_offset_v++;
	}
	return new_img;
}

static gdImagePtr gdImageScaleBilinearTC(gdImagePtr im, const unsigned int new_width, const unsigned int new_height)
{
	long dst_w = MAX(1, new_width);
	long dst_h = MAX(1, new_height);
	float dx = (float)gdImageSX(im) / (float)dst_w;
	float dy = (float)gdImageSY(im) / (float)dst_h;
	gdFixed f_dx = gd_ftofx(dx);
	gdFixed f_dy = gd_ftofx(dy);
	gdFixed f_1 = gd_itofx(1);

	int dst_offset_h;
	int dst_offset_v = 0;
	long i;
	gdImagePtr new_img;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	new_img = gdImageCreateTrueColor(new_width, new_height);
	if (!new_img){
		return NULL;
	}

	for (i=0; i < dst_h; i++) {
		long j;
		dst_offset_h = 0;
		for (j=0; j < dst_w; j++) {
			/* Update bitmap */
			gdFixed f_i = gd_itofx(i);
			gdFixed f_j = gd_itofx(j);
			gdFixed f_a = gd_mulfx(f_i, f_dy);
			gdFixed f_b = gd_mulfx(f_j, f_dx);
			const long m = gd_fxtoi(f_a);
			const long n = gd_fxtoi(f_b);
			gdFixed f_f = f_a - gd_itofx(m);
			gdFixed f_g = f_b - gd_itofx(n);

			const gdFixed f_w1 = gd_mulfx(f_1-f_f, f_1-f_g);
			const gdFixed f_w2 = gd_mulfx(f_1-f_f, f_g);
			const gdFixed f_w3 = gd_mulfx(f_f, f_1-f_g);
			const gdFixed f_w4 = gd_mulfx(f_f, f_g);
			unsigned int pixel1;
			unsigned int pixel2;
			unsigned int pixel3;
			unsigned int pixel4;
			register gdFixed f_r1, f_r2, f_r3, f_r4,
					f_g1, f_g2, f_g3, f_g4,
					f_b1, f_b2, f_b3, f_b4,
					f_a1, f_a2, f_a3, f_a4;
			/* 0 for bgColor; (n,m) is supposed to be valid anyway */
			pixel1 = getPixelOverflowTC(im, n, m, 0);
			pixel2 = getPixelOverflowTC(im, n + 1, m, pixel1);
			pixel3 = getPixelOverflowTC(im, n, m + 1, pixel1);
			pixel4 = getPixelOverflowTC(im, n + 1, m + 1, pixel1);

			f_r1 = gd_itofx(gdTrueColorGetRed(pixel1));
			f_r2 = gd_itofx(gdTrueColorGetRed(pixel2));
			f_r3 = gd_itofx(gdTrueColorGetRed(pixel3));
			f_r4 = gd_itofx(gdTrueColorGetRed(pixel4));
			f_g1 = gd_itofx(gdTrueColorGetGreen(pixel1));
			f_g2 = gd_itofx(gdTrueColorGetGreen(pixel2));
			f_g3 = gd_itofx(gdTrueColorGetGreen(pixel3));
			f_g4 = gd_itofx(gdTrueColorGetGreen(pixel4));
			f_b1 = gd_itofx(gdTrueColorGetBlue(pixel1));
			f_b2 = gd_itofx(gdTrueColorGetBlue(pixel2));
			f_b3 = gd_itofx(gdTrueColorGetBlue(pixel3));
			f_b4 = gd_itofx(gdTrueColorGetBlue(pixel4));
			f_a1 = gd_itofx(gdTrueColorGetAlpha(pixel1));
			f_a2 = gd_itofx(gdTrueColorGetAlpha(pixel2));
			f_a3 = gd_itofx(gdTrueColorGetAlpha(pixel3));
			f_a4 = gd_itofx(gdTrueColorGetAlpha(pixel4));
			{
				const unsigned char red   = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_r1) + gd_mulfx(f_w2, f_r2) + gd_mulfx(f_w3, f_r3) + gd_mulfx(f_w4, f_r4));
				const unsigned char green = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_g1) + gd_mulfx(f_w2, f_g2) + gd_mulfx(f_w3, f_g3) + gd_mulfx(f_w4, f_g4));
				const unsigned char blue  = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_b1) + gd_mulfx(f_w2, f_b2) + gd_mulfx(f_w3, f_b3) + gd_mulfx(f_w4, f_b4));
				const unsigned char alpha = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_a1) + gd_mulfx(f_w2, f_a2) + gd_mulfx(f_w3, f_a3) + gd_mulfx(f_w4, f_a4));

				new_img->tpixels[dst_offset_v][dst_offset_h] = gdTrueColorAlpha(red, green, blue, alpha);
			}

			dst_offset_h++;
		}

		dst_offset_v++;
	}
	return new_img;
}

gdImagePtr gdImageScaleBilinear(gdImagePtr im, const unsigned int new_width, const unsigned int new_height)
{
	if (im->trueColor) {
		return gdImageScaleBilinearTC(im, new_width, new_height);
	} else {
		return gdImageScaleBilinearPalette(im, new_width, new_height);
	}
}

gdImagePtr gdImageScaleBicubicFixed(gdImagePtr src, const unsigned int width, const unsigned int height)
{
	const long new_width = MAX(1, width);
	const long new_height = MAX(1, height);
	const int src_w = gdImageSX(src);
	const int src_h = gdImageSY(src);
	const gdFixed f_dx = gd_ftofx((float)src_w / (float)new_width);
	const gdFixed f_dy = gd_ftofx((float)src_h / (float)new_height);
	const gdFixed f_1 = gd_itofx(1);
	const gdFixed f_2 = gd_itofx(2);
	const gdFixed f_4 = gd_itofx(4);
	const gdFixed f_6 = gd_itofx(6);
	const gdFixed f_gamma = gd_ftofx(1.04f);
	gdImagePtr dst;

	unsigned int dst_offset_x;
	unsigned int dst_offset_y = 0;
	long i;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

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

	for (i=0; i < new_height; i++) {
		long j;
		dst_offset_x = 0;

		for (j=0; j < new_width; j++) {
			const gdFixed f_a = gd_mulfx(gd_itofx(i), f_dy);
			const gdFixed f_b = gd_mulfx(gd_itofx(j), f_dx);
			const long m = gd_fxtoi(f_a);
			const long n = gd_fxtoi(f_b);
			const gdFixed f_f = f_a - gd_itofx(m);
			const gdFixed f_g = f_b - gd_itofx(n);
			unsigned int src_offset_x[16], src_offset_y[16];
			long k;
			register gdFixed f_red = 0, f_green = 0, f_blue = 0, f_alpha = 0;
			unsigned char red, green, blue, alpha = 0;
			int *dst_row = dst->tpixels[dst_offset_y];

			if ((m < 1) || (n < 1)) {
				src_offset_x[0] = n;
				src_offset_y[0] = m;
			} else {
				src_offset_x[0] = n - 1;
				src_offset_y[0] = m;
			}

			src_offset_x[1] = n;
			src_offset_y[1] = m;

			if ((m < 1) || (n >= src_w - 1)) {
				src_offset_x[2] = n;
				src_offset_y[2] = m;
			} else {
				src_offset_x[2] = n + 1;
				src_offset_y[2] = m;
			}

			if ((m < 1) || (n >= src_w - 2)) {
				src_offset_x[3] = n;
				src_offset_y[3] = m;
			} else {
				src_offset_x[3] = n + 1 + 1;
				src_offset_y[3] = m;
			}

			if (n < 1) {
				src_offset_x[4] = n;
				src_offset_y[4] = m;
			} else {
				src_offset_x[4] = n - 1;
				src_offset_y[4] = m;
			}

			src_offset_x[5] = n;
			src_offset_y[5] = m;
			if (n >= src_w-1) {
				src_offset_x[6] = n;
				src_offset_y[6] = m;
			} else {
				src_offset_x[6] = n + 1;
				src_offset_y[6] = m;
			}

			if (n >= src_w - 2) {
				src_offset_x[7] = n;
				src_offset_y[7] = m;
			} else {
				src_offset_x[7] = n + 1 + 1;
				src_offset_y[7] = m;
			}

			if ((m >= src_h - 1) || (n < 1)) {
				src_offset_x[8] = n;
				src_offset_y[8] = m;
			} else {
				src_offset_x[8] = n - 1;
				src_offset_y[8] = m;
			}

			src_offset_x[9] = n;
			src_offset_y[9] = m;

			if ((m >= src_h-1) || (n >= src_w-1)) {
				src_offset_x[10] = n;
				src_offset_y[10] = m;
			} else {
				src_offset_x[10] = n + 1;
				src_offset_y[10] = m;
			}

			if ((m >= src_h - 1) || (n >= src_w - 2)) {
				src_offset_x[11] = n;
				src_offset_y[11] = m;
			} else {
				src_offset_x[11] = n + 1 + 1;
				src_offset_y[11] = m;
			}

			if ((m >= src_h - 2) || (n < 1)) {
				src_offset_x[12] = n;
				src_offset_y[12] = m;
			} else {
				src_offset_x[12] = n - 1;
				src_offset_y[12] = m;
			}

			src_offset_x[13] = n;
			src_offset_y[13] = m;

			if ((m >= src_h - 2) || (n >= src_w - 1)) {
				src_offset_x[14] = n;
				src_offset_y[14] = m;
			} else {
				src_offset_x[14] = n + 1;
				src_offset_y[14] = m;
			}

			if ((m >= src_h - 2) || (n >= src_w - 2)) {
				src_offset_x[15] = n;
				src_offset_y[15] = m;
			} else {
				src_offset_x[15] = n  + 1 + 1;
				src_offset_y[15] = m;
			}

			for (k = -1; k < 3; k++) {
				const gdFixed f = gd_itofx(k)-f_f;
				const gdFixed f_fm1 = f - f_1;
				const gdFixed f_fp1 = f + f_1;
				const gdFixed f_fp2 = f + f_2;
				register gdFixed f_a = 0, f_b = 0, f_d = 0, f_c = 0;
				register gdFixed f_RY;
				int l;

				if (f_fp2 > 0) f_a = gd_mulfx(f_fp2, gd_mulfx(f_fp2,f_fp2));
				if (f_fp1 > 0) f_b = gd_mulfx(f_fp1, gd_mulfx(f_fp1,f_fp1));
				if (f > 0)     f_c = gd_mulfx(f, gd_mulfx(f,f));
				if (f_fm1 > 0) f_d = gd_mulfx(f_fm1, gd_mulfx(f_fm1,f_fm1));

				f_RY = gd_divfx((f_a - gd_mulfx(f_4,f_b) + gd_mulfx(f_6,f_c) - gd_mulfx(f_4,f_d)),f_6);

				for (l = -1; l < 3; l++) {
					const gdFixed f = gd_itofx(l) - f_g;
					const gdFixed f_fm1 = f - f_1;
					const gdFixed f_fp1 = f + f_1;
					const gdFixed f_fp2 = f + f_2;
					register gdFixed f_a = 0, f_b = 0, f_c = 0, f_d = 0;
					register gdFixed f_RX, f_R, f_rs, f_gs, f_bs, f_ba;
					register int c;
					const int _k = ((k+1)*4) + (l+1);

					if (f_fp2 > 0) f_a = gd_mulfx(f_fp2,gd_mulfx(f_fp2,f_fp2));

					if (f_fp1 > 0) f_b = gd_mulfx(f_fp1,gd_mulfx(f_fp1,f_fp1));

					if (f > 0) f_c = gd_mulfx(f,gd_mulfx(f,f));

					if (f_fm1 > 0) f_d = gd_mulfx(f_fm1,gd_mulfx(f_fm1,f_fm1));

					f_RX = gd_divfx((f_a-gd_mulfx(f_4,f_b)+gd_mulfx(f_6,f_c)-gd_mulfx(f_4,f_d)),f_6);
					f_R = gd_mulfx(f_RY,f_RX);

					c = src->tpixels[*(src_offset_y + _k)][*(src_offset_x + _k)];
					f_rs = gd_itofx(gdTrueColorGetRed(c));
					f_gs = gd_itofx(gdTrueColorGetGreen(c));
					f_bs = gd_itofx(gdTrueColorGetBlue(c));
					f_ba = gd_itofx(gdTrueColorGetAlpha(c));

					f_red += gd_mulfx(f_rs,f_R);
					f_green += gd_mulfx(f_gs,f_R);
					f_blue += gd_mulfx(f_bs,f_R);
					f_alpha += gd_mulfx(f_ba,f_R);
				}
			}

			red    = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_red,   f_gamma)),  0, 255);
			green  = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_green, f_gamma)),  0, 255);
			blue   = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_blue,  f_gamma)),  0, 255);
			alpha  = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_alpha,  f_gamma)), 0, 127);

			*(dst_row + dst_offset_x) = gdTrueColorAlpha(red, green, blue, alpha);

			dst_offset_x++;
		}
		dst_offset_y++;
	}
	return dst;
}

gdImagePtr gdImageScale(const gdImagePtr src, const unsigned int new_width, const unsigned int new_height)
{
	gdImagePtr im_scaled = NULL;

	if (src == NULL || src->interpolation_id < 0 || src->interpolation_id > GD_METHOD_COUNT) {
		return NULL;
	}

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	switch (src->interpolation_id) {
		/*Special cases, optimized implementations */
		case GD_NEAREST_NEIGHBOUR:
			im_scaled = gdImageScaleNearestNeighbour(src, new_width, new_height);
			break;

		case GD_BILINEAR_FIXED:
			im_scaled = gdImageScaleBilinear(src, new_width, new_height);
			break;

		case GD_BICUBIC_FIXED:
			im_scaled = gdImageScaleBicubicFixed(src, new_width, new_height);
			break;

		/* generic */
		default:
			if (src->interpolation == NULL) {
				return NULL;
			}
			im_scaled = gdImageScaleTwoPass(src, src->sx, src->sy, new_width, new_height);
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

gdImagePtr gdImageRotateNearestNeighbour(gdImagePtr src, const float degrees, const int bgColor)
{
	float _angle = ((float) (-degrees / 180.0f) * (float)M_PI);
	const int src_w  = gdImageSX(src);
	const int src_h = gdImageSY(src);
	const gdFixed f_0_5 = gd_ftofx(0.5f);
	const gdFixed f_H = gd_itofx(src_h/2);
	const gdFixed f_W = gd_itofx(src_w/2);
	const gdFixed f_cos = gd_ftofx(cos(-_angle));
	const gdFixed f_sin = gd_ftofx(sin(-_angle));

	unsigned int dst_offset_x;
	unsigned int dst_offset_y = 0;
	unsigned int i;
	gdImagePtr dst;
	gdRect bbox;
	int new_height, new_width;

    gdRotatedImageSize(src, degrees, &bbox);
    new_width = bbox.width;
    new_height = bbox.height;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	dst = gdImageCreateTrueColor(new_width, new_height);
	if (!dst) {
		return NULL;
	}
	dst->saveAlphaFlag = 1;
	for (i = 0; i < new_height; i++) {
		unsigned int j;
		dst_offset_x = 0;
		for (j = 0; j < new_width; j++) {
			gdFixed f_i = gd_itofx((int)i - (int)new_height/2);
			gdFixed f_j = gd_itofx((int)j - (int)new_width/2);
			gdFixed f_m = gd_mulfx(f_j,f_sin) + gd_mulfx(f_i,f_cos) + f_0_5 + f_H;
			gdFixed f_n = gd_mulfx(f_j,f_cos) - gd_mulfx(f_i,f_sin) + f_0_5 + f_W;
			long m = gd_fxtoi(f_m);
			long n = gd_fxtoi(f_n);

			if ((m > 0) && (m < src_h-1) && (n > 0) && (n < src_w-1)) {
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

gdImagePtr gdImageRotateGeneric(gdImagePtr src, const float degrees, const int bgColor)
{
	float _angle = ((float) (-degrees / 180.0f) * (float)M_PI);
	const int src_w  = gdImageSX(src);
	const int src_h = gdImageSY(src);
	const gdFixed f_0_5 = gd_ftofx(0.5f);
	const gdFixed f_H = gd_itofx(src_h/2);
	const gdFixed f_W = gd_itofx(src_w/2);
	const gdFixed f_cos = gd_ftofx(cos(-_angle));
	const gdFixed f_sin = gd_ftofx(sin(-_angle));

	unsigned int dst_offset_x;
	unsigned int dst_offset_y = 0;
	unsigned int i;
	gdImagePtr dst;
	int new_width, new_height;
	gdRect bbox;

	const gdFixed f_slop_y = f_sin;
	const gdFixed f_slop_x = f_cos;
	const gdFixed f_slop = f_slop_x > 0 && f_slop_y > 0 ?
							(f_slop_x > f_slop_y ? gd_divfx(f_slop_y, f_slop_x) : gd_divfx(f_slop_x, f_slop_y))
						: 0;


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
			gdFixed f_i = gd_itofx((int)i - (int)new_height/ 2);
			gdFixed f_j = gd_itofx((int)j - (int)new_width / 2);
			gdFixed f_m = gd_mulfx(f_j,f_sin) + gd_mulfx(f_i,f_cos) + f_0_5 + f_H;
			gdFixed f_n = gd_mulfx(f_j,f_cos) - gd_mulfx(f_i,f_sin) + f_0_5 + f_W;
			long m = gd_fxtoi(f_m);
			long n = gd_fxtoi(f_n);

			if ((n <= 0) || (m <= 0) || (m >= src_h) || (n >= src_w)) {
				dst->tpixels[dst_offset_y][dst_offset_x++] = bgColor;
			} else if ((n <= 1) || (m <= 1) || (m >= src_h - 1) || (n >= src_w - 1)) {
				register int c = getPixelInterpolated(src, n, m, bgColor);
				c = c | (( gdTrueColorGetAlpha(c) + ((int)(127* gd_fxtof(f_slop)))) << 24);

				dst->tpixels[dst_offset_y][dst_offset_x++] = _color_blend(bgColor, c);
			} else {
				dst->tpixels[dst_offset_y][dst_offset_x++] = getPixelInterpolated(src, n, m, bgColor);
			}
		}
		dst_offset_y++;
	}
	return dst;
}

gdImagePtr gdImageRotateBilinear(gdImagePtr src, const float degrees, const int bgColor)
{
	float _angle = (float)((- degrees / 180.0f) * M_PI);
	const unsigned int src_w = gdImageSX(src);
	const unsigned int src_h = gdImageSY(src);
	unsigned int new_width, new_height;
	const gdFixed f_0_5 = gd_ftofx(0.5f);
	const gdFixed f_H = gd_itofx(src_h/2);
	const gdFixed f_W = gd_itofx(src_w/2);
	const gdFixed f_cos = gd_ftofx(cos(-_angle));
	const gdFixed f_sin = gd_ftofx(sin(-_angle));
	const gdFixed f_1 = gd_itofx(1);
	unsigned int i;
	unsigned int dst_offset_x;
	unsigned int dst_offset_y = 0;
	unsigned int src_offset_x, src_offset_y;
	gdImagePtr dst;
	gdRect bbox;

	gdRotatedImageSize(src, degrees, &bbox);

	new_width = bbox.width;
	new_height = bbox.height;

	dst = gdImageCreateTrueColor(new_width, new_height);
	if (dst == NULL) {
		return NULL;
	}
	dst->saveAlphaFlag = 1;

	for (i = 0; i < new_height; i++) {
		unsigned int j;
		dst_offset_x = 0;

		for (j=0; j < new_width; j++) {
			const gdFixed f_i = gd_itofx((int)i - (int)new_height/2);
			const gdFixed f_j = gd_itofx((int)j - (int)new_width/2);
			const gdFixed f_m = gd_mulfx(f_j,f_sin) + gd_mulfx(f_i,f_cos) + f_0_5 + f_H;
			const gdFixed f_n = gd_mulfx(f_j,f_cos) - gd_mulfx(f_i,f_sin) + f_0_5 + f_W;
			const unsigned int m = gd_fxtoi(f_m);
			const unsigned int n = gd_fxtoi(f_n);

			if ((m >= 0) && (m < src_h - 1) && (n >= 0) && (n < src_w - 1)) {
				const gdFixed f_f = f_m - gd_itofx(m);
				const gdFixed f_g = f_n - gd_itofx(n);
				const gdFixed f_w1 = gd_mulfx(f_1-f_f, f_1-f_g);
				const gdFixed f_w2 = gd_mulfx(f_1-f_f, f_g);
				const gdFixed f_w3 = gd_mulfx(f_f, f_1-f_g);
				const gdFixed f_w4 = gd_mulfx(f_f, f_g);

				if (m < src_h-1) {
					src_offset_x = n;
					src_offset_y = m + 1;
				}

				if (!((n >= src_w-1) || (m >= src_h-1))) {
					src_offset_x = n + 1;
					src_offset_y = m + 1;
				}
				{
					const int pixel1 = src->tpixels[src_offset_y][src_offset_x];
					register int pixel2, pixel3, pixel4;

					if (src_offset_y + 1 >= src_h) {
						pixel2 = pixel1;
						pixel3 = pixel1;
						pixel4 = pixel1;
					} else if (src_offset_x + 1 >= src_w) {
						pixel2 = pixel1;
						pixel3 = pixel1;
						pixel4 = pixel1;
					} else {
					    pixel2 = src->tpixels[src_offset_y][src_offset_x + 1];
						pixel3 = src->tpixels[src_offset_y + 1][src_offset_x];
						pixel4 = src->tpixels[src_offset_y + 1][src_offset_x + 1];
					}
					{
						const gdFixed f_r1 = gd_itofx(gdTrueColorGetRed(pixel1));
						const gdFixed f_r2 = gd_itofx(gdTrueColorGetRed(pixel2));
						const gdFixed f_r3 = gd_itofx(gdTrueColorGetRed(pixel3));
						const gdFixed f_r4 = gd_itofx(gdTrueColorGetRed(pixel4));
						const gdFixed f_g1 = gd_itofx(gdTrueColorGetGreen(pixel1));
						const gdFixed f_g2 = gd_itofx(gdTrueColorGetGreen(pixel2));
						const gdFixed f_g3 = gd_itofx(gdTrueColorGetGreen(pixel3));
						const gdFixed f_g4 = gd_itofx(gdTrueColorGetGreen(pixel4));
						const gdFixed f_b1 = gd_itofx(gdTrueColorGetBlue(pixel1));
						const gdFixed f_b2 = gd_itofx(gdTrueColorGetBlue(pixel2));
						const gdFixed f_b3 = gd_itofx(gdTrueColorGetBlue(pixel3));
						const gdFixed f_b4 = gd_itofx(gdTrueColorGetBlue(pixel4));
						const gdFixed f_a1 = gd_itofx(gdTrueColorGetAlpha(pixel1));
						const gdFixed f_a2 = gd_itofx(gdTrueColorGetAlpha(pixel2));
						const gdFixed f_a3 = gd_itofx(gdTrueColorGetAlpha(pixel3));
						const gdFixed f_a4 = gd_itofx(gdTrueColorGetAlpha(pixel4));
						const gdFixed f_red = gd_mulfx(f_w1, f_r1) + gd_mulfx(f_w2, f_r2) + gd_mulfx(f_w3, f_r3) + gd_mulfx(f_w4, f_r4);
						const gdFixed f_green = gd_mulfx(f_w1, f_g1) + gd_mulfx(f_w2, f_g2) + gd_mulfx(f_w3, f_g3) + gd_mulfx(f_w4, f_g4);
						const gdFixed f_blue = gd_mulfx(f_w1, f_b1) + gd_mulfx(f_w2, f_b2) + gd_mulfx(f_w3, f_b3) + gd_mulfx(f_w4, f_b4);
						const gdFixed f_alpha = gd_mulfx(f_w1, f_a1) + gd_mulfx(f_w2, f_a2) + gd_mulfx(f_w3, f_a3) + gd_mulfx(f_w4, f_a4);

						const unsigned char red   = (unsigned char) CLAMP(gd_fxtoi(f_red),   0, 255);
						const unsigned char green = (unsigned char) CLAMP(gd_fxtoi(f_green), 0, 255);
						const unsigned char blue  = (unsigned char) CLAMP(gd_fxtoi(f_blue),  0, 255);
						const unsigned char alpha = (unsigned char) CLAMP(gd_fxtoi(f_alpha), 0, 127);

						dst->tpixels[dst_offset_y][dst_offset_x++] = gdTrueColorAlpha(red, green, blue, alpha);
					}
				}
			} else {
				dst->tpixels[dst_offset_y][dst_offset_x++] = bgColor;
			}
		}
		dst_offset_y++;
	}
	return dst;
}

gdImagePtr gdImageRotateBicubicFixed(gdImagePtr src, const float degrees, const int bgColor)
{
	const float _angle = (float)((- degrees / 180.0f) * M_PI);
	const int src_w = gdImageSX(src);
	const int src_h = gdImageSY(src);
	unsigned int new_width, new_height;
	const gdFixed f_0_5 = gd_ftofx(0.5f);
	const gdFixed f_H = gd_itofx(src_h/2);
	const gdFixed f_W = gd_itofx(src_w/2);
	const gdFixed f_cos = gd_ftofx(cos(-_angle));
	const gdFixed f_sin = gd_ftofx(sin(-_angle));
	const gdFixed f_1 = gd_itofx(1);
	const gdFixed f_2 = gd_itofx(2);
	const gdFixed f_4 = gd_itofx(4);
	const gdFixed f_6 = gd_itofx(6);
	const gdFixed f_gama = gd_ftofx(1.04f);

	unsigned int dst_offset_x;
	unsigned int dst_offset_y = 0;
	unsigned int i;
	gdImagePtr dst;
	gdRect bbox;

	gdRotatedImageSize(src, degrees, &bbox);
	new_width = bbox.width;
	new_height = bbox.height;
	dst = gdImageCreateTrueColor(new_width, new_height);

	if (dst == NULL) {
		return NULL;
	}
	dst->saveAlphaFlag = 1;

	for (i=0; i < new_height; i++) {
		unsigned int j;
		dst_offset_x = 0;

		for (j=0; j < new_width; j++) {
			const gdFixed f_i = gd_itofx((int)i - (int)new_height/2);
			const gdFixed f_j = gd_itofx((int)j - (int)new_width/2);
			const gdFixed f_m = gd_mulfx(f_j,f_sin) + gd_mulfx(f_i,f_cos) + f_0_5 + f_H;
			const gdFixed f_n = gd_mulfx(f_j,f_cos) - gd_mulfx(f_i,f_sin) + f_0_5 + f_W;
			const int m = gd_fxtoi(f_m);
			const int n = gd_fxtoi(f_n);

			if ((m > 0) && (m < src_h - 1) && (n > 0) && (n < src_w-1)) {
				const gdFixed f_f = f_m - gd_itofx(m);
				const gdFixed f_g = f_n - gd_itofx(n);
				unsigned int src_offset_x[16], src_offset_y[16];
				unsigned char red, green, blue, alpha;
				gdFixed f_red=0, f_green=0, f_blue=0, f_alpha=0;
				int k;

				if ((m < 1) || (n < 1)) {
					src_offset_x[0] = n;
					src_offset_y[0] = m;
				} else {
					src_offset_x[0] = n - 1;
					src_offset_y[0] = m;
				}

				src_offset_x[1] = n;
				src_offset_y[1] = m;

				if ((m < 1) || (n >= src_w-1)) {
					src_offset_x[2] = - 1;
					src_offset_y[2] = - 1;
				} else {
					src_offset_x[2] = n + 1;
					src_offset_y[2] = m ;
				}

				if ((m < 1) || (n >= src_w-2)) {
					src_offset_x[3] = - 1;
					src_offset_y[3] = - 1;
				} else {
					src_offset_x[3] = n + 1 + 1;
					src_offset_y[3] = m ;
				}

				if (n < 1) {
					src_offset_x[4] = - 1;
					src_offset_y[4] = - 1;
				} else {
					src_offset_x[4] = n - 1;
					src_offset_y[4] = m;
				}

				src_offset_x[5] = n;
				src_offset_y[5] = m;
				if (n >= src_w-1) {
					src_offset_x[6] = - 1;
					src_offset_y[6] = - 1;
				} else {
					src_offset_x[6] = n + 1;
					src_offset_y[6] = m;
				}

				if (n >= src_w-2) {
					src_offset_x[7] = - 1;
					src_offset_y[7] = - 1;
				} else {
					src_offset_x[7] = n + 1 + 1;
					src_offset_y[7] = m;
				}

				if ((m >= src_h-1) || (n < 1)) {
					src_offset_x[8] = - 1;
					src_offset_y[8] = - 1;
				} else {
					src_offset_x[8] = n - 1;
					src_offset_y[8] = m;
				}

				if (m >= src_h-1) {
					src_offset_x[9] = - 1;
					src_offset_y[9] = - 1;
				} else {
					src_offset_x[9] = n;
					src_offset_y[9] = m;
				}

				if ((m >= src_h-1) || (n >= src_w-1)) {
					src_offset_x[10] = - 1;
					src_offset_y[10] = - 1;
				} else {
					src_offset_x[10] = n + 1;
					src_offset_y[10] = m;
				}

				if ((m >= src_h-1) || (n >= src_w-2)) {
					src_offset_x[11] = - 1;
					src_offset_y[11] = - 1;
				} else {
					src_offset_x[11] = n + 1 + 1;
					src_offset_y[11] = m;
				}

				if ((m >= src_h-2) || (n < 1)) {
					src_offset_x[12] = - 1;
					src_offset_y[12] = - 1;
				} else {
					src_offset_x[12] = n - 1;
					src_offset_y[12] = m;
				}

				if (m >= src_h-2) {
					src_offset_x[13] = - 1;
					src_offset_y[13] = - 1;
				} else {
					src_offset_x[13] = n;
					src_offset_y[13] = m;
				}

				if ((m >= src_h-2) || (n >= src_w - 1)) {
					src_offset_x[14] = - 1;
					src_offset_y[14] = - 1;
				} else {
					src_offset_x[14] = n + 1;
					src_offset_y[14] = m;
				}

				if ((m >= src_h-2) || (n >= src_w-2)) {
					src_offset_x[15] = - 1;
					src_offset_y[15] = - 1;
				} else {
					src_offset_x[15] = n  + 1 + 1;
					src_offset_y[15] = m;
				}

				for (k=-1; k<3; k++) {
					const gdFixed f = gd_itofx(k)-f_f;
					const gdFixed f_fm1 = f - f_1;
					const gdFixed f_fp1 = f + f_1;
					const gdFixed f_fp2 = f + f_2;
					gdFixed f_a = 0, f_b = 0,f_c = 0, f_d = 0;
					gdFixed f_RY;
					int l;

					if (f_fp2 > 0) {
						f_a = gd_mulfx(f_fp2,gd_mulfx(f_fp2,f_fp2));
					}

					if (f_fp1 > 0) {
						f_b = gd_mulfx(f_fp1,gd_mulfx(f_fp1,f_fp1));
					}

					if (f > 0) {
						f_c = gd_mulfx(f,gd_mulfx(f,f));
					}

					if (f_fm1 > 0) {
						f_d = gd_mulfx(f_fm1,gd_mulfx(f_fm1,f_fm1));
					}
					f_RY = gd_divfx((f_a-gd_mulfx(f_4,f_b)+gd_mulfx(f_6,f_c)-gd_mulfx(f_4,f_d)),f_6);

					for (l=-1;  l< 3; l++) {
						const gdFixed f = gd_itofx(l) - f_g;
						const gdFixed f_fm1 = f - f_1;
						const gdFixed f_fp1 = f + f_1;
						const gdFixed f_fp2 = f + f_2;
						gdFixed f_a = 0, f_b = 0, f_c = 0, f_d = 0;
						gdFixed f_RX, f_R;
						const int _k = ((k + 1) * 4) + (l + 1);
						register gdFixed f_rs, f_gs, f_bs, f_as;
						register int c;

						if (f_fp2 > 0) {
							f_a = gd_mulfx(f_fp2,gd_mulfx(f_fp2,f_fp2));
						}

						if (f_fp1 > 0) {
							f_b = gd_mulfx(f_fp1,gd_mulfx(f_fp1,f_fp1));
						}

						if (f > 0) {
							f_c = gd_mulfx(f,gd_mulfx(f,f));
						}

						if (f_fm1 > 0) {
							f_d = gd_mulfx(f_fm1,gd_mulfx(f_fm1,f_fm1));
						}

						f_RX = gd_divfx((f_a - gd_mulfx(f_4, f_b) + gd_mulfx(f_6, f_c) - gd_mulfx(f_4, f_d)), f_6);
						f_R = gd_mulfx(f_RY, f_RX);

						if ((src_offset_x[_k] <= 0) || (src_offset_y[_k] <= 0) || (src_offset_y[_k] >= src_h) || (src_offset_x[_k] >= src_w)) {
							c = bgColor;
						} else if ((src_offset_x[_k] <= 1) || (src_offset_y[_k] <= 1) || (src_offset_y[_k] >= (int)src_h - 1) || (src_offset_x[_k] >= (int)src_w - 1)) {
							gdFixed f_127 = gd_itofx(127);
							c = src->tpixels[src_offset_y[_k]][src_offset_x[_k]];
							c = c | (( (int) (gd_fxtof(gd_mulfx(f_R, f_127)) + 50.5f)) << 24);
							c = _color_blend(bgColor, c);
						} else {
							c = src->tpixels[src_offset_y[_k]][src_offset_x[_k]];
						}

						f_rs = gd_itofx(gdTrueColorGetRed(c));
						f_gs = gd_itofx(gdTrueColorGetGreen(c));
						f_bs = gd_itofx(gdTrueColorGetBlue(c));
						f_as = gd_itofx(gdTrueColorGetAlpha(c));

						f_red   += gd_mulfx(f_rs, f_R);
						f_green += gd_mulfx(f_gs, f_R);
						f_blue  += gd_mulfx(f_bs, f_R);
						f_alpha += gd_mulfx(f_as, f_R);
					}
				}

				red   = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_red, f_gama)),   0, 255);
				green = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_green, f_gama)), 0, 255);
				blue  = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_blue, f_gama)),  0, 255);
				alpha = (unsigned char) CLAMP(gd_fxtoi(gd_mulfx(f_alpha, f_gama)), 0, 127);

				dst->tpixels[dst_offset_y][dst_offset_x] =  gdTrueColorAlpha(red, green, blue, alpha);
			} else {
				dst->tpixels[dst_offset_y][dst_offset_x] =  bgColor;
			}
			dst_offset_x++;
		}

		dst_offset_y++;
	}
	return dst;
}

gdImagePtr gdImageRotateInterpolated(const gdImagePtr src, const float angle, int bgcolor)
{
	/* round to two decimals and keep the 100x multiplication to use it in the common square angles
	   case later. Keep the two decimal precisions so smaller rotation steps can be done, useful for
	   slow animations. */
	const int angle_rounded = fmod((int) floorf(angle * 100), 360 * 100);

	if (bgcolor < 0) {
		return NULL;
	}

	/* impact perf a bit, but not that much. Implementation for palette
	   images can be done at a later point.
	*/
	if (src->trueColor == 0) {
		if (bgcolor < gdMaxColors) {
			bgcolor =  gdTrueColorAlpha(src->red[bgcolor], src->green[bgcolor], src->blue[bgcolor], src->alpha[bgcolor]);
		}
		gdImagePaletteToTrueColor(src);
	}

	/* no interpolation needed here */
	switch (angle_rounded) {
		case    0: {
			gdImagePtr dst = gdImageCreateTrueColor(src->sx, src->sy);
			if (dst == NULL) {
				return NULL;
			}
			dst->transparent = src->transparent;
			dst->saveAlphaFlag = 1;
			dst->alphaBlendingFlag = gdEffectReplace;

			gdImageCopy(dst, src, 0,0,0,0,src->sx,src->sy);
			return dst;
		}
		case -27000:
		case   9000:
			return gdImageRotate90(src, 0);
		case -18000:
		case  18000:
			return gdImageRotate180(src, 0);
		case -9000:
		case 27000:
			return gdImageRotate270(src, 0);
	}

	if (src == NULL || src->interpolation_id < 1 || src->interpolation_id > GD_METHOD_COUNT) {
		return NULL;
	}

	switch (src->interpolation_id) {
		case GD_NEAREST_NEIGHBOUR:
			return gdImageRotateNearestNeighbour(src, angle, bgcolor);
			break;

		case GD_BILINEAR_FIXED:
			return gdImageRotateBilinear(src, angle, bgcolor);
			break;

		case GD_BICUBIC_FIXED:
			return gdImageRotateBicubicFixed(src, angle, bgcolor);
			break;

		default:
			return gdImageRotateGeneric(src, angle, bgcolor);
	}
	return NULL;
}

/**
 * Title: Affine transformation
 **/

/**
 * Group: Transform
 **/

 static void gdImageClipRectangle(gdImagePtr im, gdRectPtr r)
{
	int c1x, c1y, c2x, c2y;
	int x1,y1;

	gdImageGetClip(im, &c1x, &c1y, &c2x, &c2y);
	x1 = r->x + r->width - 1;
	y1 = r->y + r->height - 1;
	r->x = CLAMP(r->x, c1x, c2x);
	r->y = CLAMP(r->y, c1y, c2y);
	r->width = CLAMP(x1, c1x, c2x) - r->x + 1;
	r->height = CLAMP(y1, c1y, c2y) - r->y + 1;
}

void gdDumpRect(const char *msg, gdRectPtr r)
{
	printf("%s (%i, %i) (%i, %i)\n", msg, r->x, r->y, r->width, r->height);
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
int gdTransformAffineGetImage(gdImagePtr *dst,
		  const gdImagePtr src,
		  gdRectPtr src_area,
		  const double affine[6])
{
	int res;
	double m[6];
	gdRect bbox;
	gdRect area_full;

	if (src_area == NULL) {
		area_full.x = 0;
		area_full.y = 0;
		area_full.width  = gdImageSX(src);
		area_full.height = gdImageSY(src);
		src_area = &area_full;
	}

	gdTransformAffineBoundingBox(src_area, affine, &bbox);

	*dst = gdImageCreateTrueColor(bbox.width, bbox.height);
	if (*dst == NULL) {
		return GD_FALSE;
	}
	(*dst)->saveAlphaFlag = 1;

	if (!src->trueColor) {
		gdImagePaletteToTrueColor(src);
	}

	/* Translate to dst origin (0,0) */
	gdAffineTranslate(m, -bbox.x, -bbox.y);
	gdAffineConcat(m, affine, m);

	gdImageAlphaBlending(*dst, 0);

	res = gdTransformAffineCopy(*dst,
		  0,0,
		  src,
		  src_area,
		  m);

	if (res != GD_TRUE) {
		gdImageDestroy(*dst);
		dst = NULL;
		return GD_FALSE;
	} else {
		return GD_TRUE;
	}
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
int gdTransformAffineCopy(gdImagePtr dst,
		  int dst_x, int dst_y,
		  const gdImagePtr src,
		  gdRectPtr src_region,
		  const double affine[6])
{
	int c1x,c1y,c2x,c2y;
	int backclip = 0;
	int backup_clipx1, backup_clipy1, backup_clipx2, backup_clipy2;
	register int x, y, src_offset_x, src_offset_y;
	double inv[6];
	int *dst_p;
	gdPointF pt, src_pt;
	gdRect bbox;
	int end_x, end_y;
	gdInterpolationMethod interpolation_id_bak = src->interpolation_id;

	/* These methods use special implementations */
	if (src->interpolation_id == GD_BILINEAR_FIXED || src->interpolation_id == GD_BICUBIC_FIXED || src->interpolation_id == GD_NEAREST_NEIGHBOUR) {
		interpolation_id_bak = src->interpolation_id;

		gdImageSetInterpolationMethod(src, GD_BICUBIC);
	}


	gdImageClipRectangle(src, src_region);

	if (src_region->x > 0 || src_region->y > 0
		|| src_region->width < gdImageSX(src)
		|| src_region->height < gdImageSY(src)) {
		backclip = 1;

		gdImageGetClip(src, &backup_clipx1, &backup_clipy1,
		&backup_clipx2, &backup_clipy2);

		gdImageSetClip(src, src_region->x, src_region->y,
			src_region->x + src_region->width - 1,
			src_region->y + src_region->height - 1);
	}

	if (!gdTransformAffineBoundingBox(src_region, affine, &bbox)) {
		if (backclip) {
			gdImageSetClip(src, backup_clipx1, backup_clipy1,
					backup_clipx2, backup_clipy2);
		}
		gdImageSetInterpolationMethod(src, interpolation_id_bak);
		return GD_FALSE;
	}

	gdImageGetClip(dst, &c1x, &c1y, &c2x, &c2y);

	end_x = bbox.width  + abs(bbox.x);
	end_y = bbox.height + abs(bbox.y);

	/* Get inverse affine to let us work with destination -> source */
	if (gdAffineInvert(inv, affine) == GD_FALSE) {
		gdImageSetInterpolationMethod(src, interpolation_id_bak);
		return GD_FALSE;
	}

	src_offset_x =  src_region->x;
	src_offset_y =  src_region->y;

	if (dst->alphaBlendingFlag) {
		for (y = bbox.y; y <= end_y; y++) {
			pt.y = y + 0.5;
			for (x = 0; x <= end_x; x++) {
				pt.x = x + 0.5;
				gdAffineApplyToPointF(&src_pt, &pt, inv);
				gdImageSetPixel(dst, dst_x + x, dst_y + y, getPixelInterpolated(src, src_offset_x + src_pt.x, src_offset_y + src_pt.y, 0));
			}
		}
	} else {
		for (y = 0; y <= end_y; y++) {
			pt.y = y + 0.5 + bbox.y;
			if ((dst_y + y) < 0 || ((dst_y + y) > gdImageSY(dst) -1)) {
				continue;
			}
			dst_p = dst->tpixels[dst_y + y] + dst_x;

			for (x = 0; x <= end_x; x++) {
				pt.x = x + 0.5 + bbox.x;
				gdAffineApplyToPointF(&src_pt, &pt, inv);

				if ((dst_x + x) < 0 || (dst_x + x) > (gdImageSX(dst) - 1)) {
					break;
				}
				*(dst_p++) = getPixelInterpolated(src, src_offset_x + src_pt.x, src_offset_y + src_pt.y, -1);
			}
		}
	}

	/* Restore clip if required */
	if (backclip) {
		gdImageSetClip(src, backup_clipx1, backup_clipy1,
				backup_clipx2, backup_clipy2);
	}

	gdImageSetInterpolationMethod(src, interpolation_id_bak);
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
int gdTransformAffineBoundingBox(gdRectPtr src, const double affine[6], gdRectPtr bbox)
{
	gdPointF extent[4], min, max, point;
	int i;

	extent[0].x=0.0;
	extent[0].y=0.0;
	extent[1].x=(double) src->width;
	extent[1].y=0.0;
	extent[2].x=(double) src->width;
	extent[2].y=(double) src->height;
	extent[3].x=0.0;
	extent[3].y=(double) src->height;

	for (i=0; i < 4; i++) {
		point=extent[i];
		if (gdAffineApplyToPointF(&extent[i], &point, affine) != GD_TRUE) {
			return GD_FALSE;
		}
	}
	min=extent[0];
	max=extent[0];

	for (i=1; i < 4; i++) {
		if (min.x > extent[i].x)
			min.x=extent[i].x;
		if (min.y > extent[i].y)
			min.y=extent[i].y;
		if (max.x < extent[i].x)
			max.x=extent[i].x;
		if (max.y < extent[i].y)
			max.y=extent[i].y;
	}
	bbox->x = (int) min.x;
	bbox->y = (int) min.y;
	bbox->width  = (int) floor(max.x - min.x) - 1;
	bbox->height = (int) floor(max.y - min.y);
	return GD_TRUE;
}

int gdImageSetInterpolationMethod(gdImagePtr im, gdInterpolationMethod id)
{
	if (im == NULL || id < 0 || id > GD_METHOD_COUNT) {
		return 0;
	}

	switch (id) {
		case GD_DEFAULT:
			id = GD_BILINEAR_FIXED;
		/* Optimized versions */
		case GD_BILINEAR_FIXED:
		case GD_BICUBIC_FIXED:
		case GD_NEAREST_NEIGHBOUR:
		case GD_WEIGHTED4:
			im->interpolation = NULL;
			break;

		/* generic versions*/
		case GD_BELL:
			im->interpolation = filter_bell;
			break;
		case GD_BESSEL:
			im->interpolation = filter_bessel;
			break;
		case GD_BICUBIC:
			im->interpolation = filter_bicubic;
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

		default:
			return 0;
			break;
	}
	im->interpolation_id = id;
	return 1;
}

#ifdef _MSC_VER
# pragma optimize("", on)
#endif
