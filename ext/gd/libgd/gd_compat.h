#ifndef GD_COMPAT_H
#define GD_COMPAT_H 1

#if HAVE_GD_BUNDLED
# include "gd.h"
#else
# include <gd.h>
#endif

const char * gdPngGetVersionString();
const char * gdJpegGetVersionString();
int gdJpegGetVersionInt();
int overflow2(int a, int b);

/* filters section
 *
 * Negate the imag src, white becomes black,
 * The red, green, and blue intensities of an image are negated.
 * White becomes black, yellow becomes blue, etc.
 */
int gdImageNegate(gdImagePtr src);

/* Convert the image src to a grayscale image */
int gdImageGrayScale(gdImagePtr src);

/* Set the brightness level <brightness> for the image src */
int gdImageBrightness(gdImagePtr src, int brightness);

/* Set the contrast level <contrast> for the image <src> */
int gdImageContrast(gdImagePtr src, double contrast);

/* Simply adds or substracts respectively red, green or blue to a pixel */
int gdImageColor(gdImagePtr src, const int red, const int green, const int blue, const int alpha);

/* Image convolution by a 3x3 custom matrix */
int gdImageConvolution(gdImagePtr src, float ft[3][3], float filter_div, float offset);
int gdImageEdgeDetectQuick(gdImagePtr src);
int gdImageGaussianBlur(gdImagePtr im);
int gdImageSelectiveBlur( gdImagePtr src);
int gdImageEmboss(gdImagePtr im);
int gdImageMeanRemoval(gdImagePtr im);
int gdImageSmooth(gdImagePtr im, float weight);
enum gdPixelateMode {
	GD_PIXELATE_UPPERLEFT,
	GD_PIXELATE_AVERAGE
};

int gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode);

#if !HAVE_GD_IMAGEELLIPSE
void gdImageEllipse(gdImagePtr im, int cx, int cy, int w, int h, int c);
#endif

gdImagePtr gdImageRotate (gdImagePtr src, double dAngle, int clrBack, int ignoretransparent);

int gdImageColorMatch (gdImagePtr im1, gdImagePtr im2);

#endif

