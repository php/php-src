/**
 * File: Image Filters
 */

#include "gd.h"

#include "gd_intern.h"
#include "gdhelpers.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <time.h>

#undef NDEBUG
/* Comment out this line to enable asserts.
 * TODO: This logic really belongs in cmake and configure.
 */
#define NDEBUG 1
#include <assert.h>

typedef int(BGD_STDCALL *FuncPtr)(gdImagePtr, int, int);

#define GET_PIXEL_FUNCTION(src)                                                \
	(src->trueColor ? gdImageGetTrueColorPixel : gdImageGetPixel)

static int gdClampFloatToByte(float value) {
	if (!isfinite(value)) {
		return value > 0.0f ? 255 : 0;
	}
	if (value > 255.0f) {
		return 255;
	}
	if (value < 0.0f) {
		return 0;
	}
	return (int)value;
}

#ifdef _WIN32
# define GD_SCATTER_SEED() (unsigned int)(time(0) * GetCurrentProcessId())
#else
# define GD_SCATTER_SEED() (unsigned int)(time(0) * getpid())
#endif

/*
	Function: gdImageScatter
 */
BGD_DECLARE(int) gdImageScatter(gdImagePtr im, int sub, int plus) {
	gdScatter s;

	s.sub = sub;
	s.plus = plus;
	s.num_colors = 0;
	s.seed = GD_SCATTER_SEED();
	return gdImageScatterEx(im, &s);
}

/*
	Function: gdImageScatterColor
 */
BGD_DECLARE(int)
gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[],
					unsigned int num_colors) {
	gdScatter s;

	s.sub = sub;
	s.plus = plus;
	s.colors = colors;
	s.num_colors = num_colors;
	s.seed = GD_SCATTER_SEED();
	return gdImageScatterEx(im, &s);
}

/*
	Function: gdImageScatterEx
 */
BGD_DECLARE(int) gdImageScatterEx(gdImagePtr im, gdScatterPtr scatter) {
	register int x, y;
	int dest_x, dest_y;
	int pxl, new_pxl;
	unsigned int n;
	int sub = scatter->sub, plus = scatter->plus;

	if (plus == 0 && sub == 0) {
		return 1;
	} else if (sub >= plus) {
		return 0;
	}

	(void)srand(scatter->seed);

	if (scatter->num_colors) {
		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx; x++) {
				dest_x = (int)(x + ((rand() % (plus - sub)) + sub));
				dest_y = (int)(y + ((rand() % (plus - sub)) + sub));

				if (!gdImageBoundsSafe(im, dest_x, dest_y)) {
					continue;
				}

				pxl = gdImageGetPixel(im, x, y);
				new_pxl = gdImageGetPixel(im, dest_x, dest_y);

				for (n = 0; n < scatter->num_colors; n++) {
					if (pxl == scatter->colors[n]) {
						gdImageSetPixel(im, dest_x, dest_y, pxl);
						gdImageSetPixel(im, x, y, new_pxl);
					}
				}
			}
		}
	} else {
		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx; x++) {
				dest_x = (int)(x + ((rand() % (plus - sub)) + sub));
				dest_y = (int)(y + ((rand() % (plus - sub)) + sub));

				if (!gdImageBoundsSafe(im, dest_x, dest_y)) {
					continue;
				}

				pxl = gdImageGetPixel(im, x, y);
				new_pxl = gdImageGetPixel(im, dest_x, dest_y);

				gdImageSetPixel(im, dest_x, dest_y, pxl);
				gdImageSetPixel(im, x, y, new_pxl);
			}
		}
	}

	return 1;
}

/*
	Function: gdImagePixelate
 */
BGD_DECLARE(int)
gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode) {
	int x, y;

	if (block_size <= 0) {
		return 0;
	} else if (block_size == 1) {
		return 1;
	}
	switch (mode) {
	case GD_PIXELATE_UPPERLEFT:
		for (y = 0; y < im->sy; y += block_size) {
			for (x = 0; x < im->sx; x += block_size) {
				if (gdImageBoundsSafe(im, x, y)) {
					int c = gdImageGetPixel(im, x, y);
					gdImageFilledRectangle(im, x, y, x + block_size - 1,
										   y + block_size - 1, c);
				}
			}
		}
		break;
	case GD_PIXELATE_AVERAGE:
		for (y = 0; y < im->sy; y += block_size) {
			for (x = 0; x < im->sx; x += block_size) {
				int a, r, g, b, c;
				int total;
				int cx, cy;

				a = r = g = b = c = total = 0;
				/* sampling */
				for (cy = 0; cy < block_size; cy++) {
					for (cx = 0; cx < block_size; cx++) {
						if (!gdImageBoundsSafe(im, x + cx, y + cy)) {
							continue;
						}
						c = gdImageGetPixel(im, x + cx, y + cy);
						a += gdImageAlpha(im, c);
						r += gdImageRed(im, c);
						g += gdImageGreen(im, c);
						b += gdImageBlue(im, c);
						total++;
					}
				}
				/* drawing */
				if (total > 0) {
					c = gdImageColorResolveAlpha(im, r / total, g / total,
												 b / total, a / total);
					gdImageFilledRectangle(im, x, y, x + block_size - 1,
										   y + block_size - 1, c);
				}
			}
		}
		break;
	default:
		return 0;
	}
	return 1;
}

/**
 * Function: gdImageNegate
 *
 * Invert an image
 *
 * Parameters:
 *   src - The image.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageNegate(gdImagePtr src) {
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			new_pxl =
				gdImageColorAllocateAlpha(src, 255 - r, 255 - g, 255 - b, a);
			if (new_pxl == -1) {
				new_pxl =
					gdImageColorClosestAlpha(src, 255 - r, 255 - g, 255 - b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

/**
 * Function: gdImageGrayScale
 *
 * Convert an image to grayscale
 *
 * The red, green and blue components of each pixel are replaced by their
 * weighted sum using the same coefficients as the REC.601 luma (Y')
 * calculation. The alpha components are retained.
 *
 * For palette images the result may differ due to palette limitations.
 *
 * Parameters:
 *   src - The image.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageGrayScale(gdImagePtr src) {
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	FuncPtr f;
	int alpha_blending;

	if (src==NULL) {
		return 0;
	}

	alpha_blending = src->alphaBlendingFlag;
	gdImageAlphaBlending(src, gdEffectReplace);

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);
			r = g = b = (int) (.299 * r + .587 * g + .114 * b);

			new_pxl = gdImageColorAllocateAlpha(src, r, g, b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, r, g, b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	gdImageAlphaBlending(src, alpha_blending);

	return 1;
}

/**
 * Function: gdImageBrightness
 *
 * Change the brightness of an image
 *
 * Parameters:
 *   src        - The image.
 *   brightness - The value to add to the color channels of all pixels.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageContrast>
 *   - <gdImageColor>
 */
BGD_DECLARE(int) gdImageBrightness(gdImagePtr src, int brightness) {
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	FuncPtr f;

	if (src==NULL || (brightness < -255 || brightness>255)) {
		return 0;
	}

	if (brightness==0) {
		return 1;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);

			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			r = r + brightness;
			g = g + brightness;
			b = b + brightness;

			r = (r > 255)? 255 : ((r < 0)? 0:r);
			g = (g > 255)? 255 : ((g < 0)? 0:g);
			b = (b > 255)? 255 : ((b < 0)? 0:b);

			new_pxl = gdImageColorAllocateAlpha(src, (int)r, (int)g, (int)b, a);
			if (new_pxl == -1) {
				new_pxl =
					gdImageColorClosestAlpha(src, (int)r, (int)g, (int)b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

/**
 * Function: gdImageContrast
 *
 * Change the contrast of an image
 *
 * Parameters:
 *   src      - The image.
 *   contrast - The contrast adjustment value. Negative values increase, postive
 *              values decrease the contrast. The larger the absolute value, the
 *              stronger the effect.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageBrightness>
 */
BGD_DECLARE(int) gdImageContrast(gdImagePtr src, double contrast) {
	int x, y;
	int r,g,b,a;
	double rf,gf,bf;
	int new_pxl, pxl;

	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	contrast = (double)(100.0-contrast)/100.0;
	contrast = contrast*contrast;

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f(src, x, y);

			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			rf = (double)r/255.0;
			rf = rf-0.5;
			rf = rf*contrast;
			rf = rf+0.5;
			rf = rf*255.0;

			bf = (double)b/255.0;
			bf = bf-0.5;
			bf = bf*contrast;
			bf = bf+0.5;
			bf = bf*255.0;

			gf = (double)g/255.0;
			gf = gf-0.5;
			gf = gf*contrast;
			gf = gf+0.5;
			gf = gf*255.0;

			rf = (rf > 255.0)? 255.0 : ((rf < 0.0)? 0.0:rf);
			gf = (gf > 255.0)? 255.0 : ((gf < 0.0)? 0.0:gf);
			bf = (bf > 255.0)? 255.0 : ((bf < 0.0)? 0.0:bf);

			new_pxl =
				gdImageColorAllocateAlpha(src, (int)rf, (int)gf, (int)bf, a);
			if (new_pxl == -1) {
				new_pxl =
					gdImageColorClosestAlpha(src, (int)rf, (int)gf, (int)bf, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

/**
 * Function: gdImageColor
 *
 * Change channel values of an image
 *
 * Parameters:
 *   src   - The image.
 *   red   - The value to add to the red channel of all pixels.
 *   green - The value to add to the green channel of all pixels.
 *   blue  - The value to add to the blue channel of all pixels.
 *   alpha - The value to add to the alpha channel of all pixels.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageBrightness>
 */
BGD_DECLARE(int)
gdImageColor(gdImagePtr src, const int red, const int green, const int blue,
			 const int alpha) {
	int x, y;
	int new_pxl, pxl;
	FuncPtr f;

	if (src == NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			int r,g,b,a;

			pxl = f(src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			r = r + red;
			g = g + green;
			b = b + blue;
			a = a + alpha;

			r = (r > 255)? 255 : ((r < 0)? 0 : r);
			g = (g > 255)? 255 : ((g < 0)? 0 : g);
			b = (b > 255)? 255 : ((b < 0)? 0 : b);
			a = (a > 127)? 127 : ((a < 0)? 0 : a);

			new_pxl = gdImageColorAllocateAlpha(src, r, g, b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, r, g, b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

/**
 * Function: gdImageConvolution
 *
 * Apply a convolution matrix to an image
 *
 * Depending on the matrix a wide range of effects can be accomplished, e.g.
 * blurring, sharpening, embossing and edge detection.
 *
 * Parameters:
 *   src        - The image.
 *   filter     - The 3x3 convolution matrix.
 *   filter_div - The value to divide the convoluted channel values by.
 *   offset     - The value to add to the convoluted channel values.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageEdgeDetectQuick>
 *   - <gdImageGaussianBlur>
 *   - <gdImageEmboss>
 *   - <gdImageMeanRemoval>
 *   - <gdImageSmooth>
 */
BGD_DECLARE(int)
gdImageConvolution(gdImagePtr src, float filter[3][3], float filter_div,
				   float offset) {
	int         x, y, i, j, new_a;
	float       new_r, new_g, new_b;
	int         new_pxl, pxl=0;
	gdImagePtr  srcback;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	/* We need the orinal image with each safe neoghb. pixel */
	srcback = gdImageCreateTrueColor (src->sx, src->sy);
	if (srcback==NULL) {
		return 0;
	}

	gdImageSaveAlpha(srcback, 1);
	new_pxl = gdImageColorAllocateAlpha(srcback, 0, 0, 0, 127);
	gdImageFill(srcback, 0, 0, new_pxl);

	gdImageCopy(srcback, src,0,0,0,0,src->sx,src->sy);

	f = GET_PIXEL_FUNCTION(src);

	for ( y=0; y<src->sy; y++) {
		for(x=0; x<src->sx; x++) {
			int new_ri, new_gi, new_bi;
			new_r = new_g = new_b = 0;
			pxl = f(srcback, x, y);
			new_a = gdImageAlpha(srcback, pxl);

			for (j=0; j<3; j++) {
				int yv = MIN(MAX(y - 1 + j, 0), src->sy - 1);
				for (i=0; i<3; i++) {
				        pxl = f(srcback, MIN(MAX(x - 1 + i, 0), src->sx - 1), yv);
					new_r += (float)gdImageRed(srcback, pxl) * filter[j][i];
					new_g += (float)gdImageGreen(srcback, pxl) * filter[j][i];
					new_b += (float)gdImageBlue(srcback, pxl) * filter[j][i];
				}
			}

			new_r = (new_r/filter_div)+offset;
			new_g = (new_g/filter_div)+offset;
			new_b = (new_b/filter_div)+offset;

			new_ri = gdClampFloatToByte(new_r);
			new_gi = gdClampFloatToByte(new_g);
			new_bi = gdClampFloatToByte(new_b);

			new_pxl =
				gdImageColorAllocateAlpha(src, new_ri, new_gi, new_bi, new_a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, new_ri, new_gi, new_bi,
												   new_a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	gdImageDestroy(srcback);
	return 1;
}

/*
	Function: gdImageSelectiveBlur
 */
BGD_DECLARE(int) gdImageSelectiveBlur(gdImagePtr src) {
	int         x, y, i, j;
	float       new_r, new_g, new_b;
	int         new_pxl, cpxl, pxl, new_a=0;
	float flt_r [3][3];
	float flt_g [3][3];
	float flt_b [3][3];
	float flt_r_sum, flt_g_sum, flt_b_sum;

	gdImagePtr srcback;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	/* We need the orinal image with each safe neoghb. pixel */
	srcback = gdImageCreateTrueColor (src->sx, src->sy);
	if (srcback==NULL) {
		return 0;
	}
	gdImageCopy(srcback, src,0,0,0,0,src->sx,src->sy);

	f = GET_PIXEL_FUNCTION(src);

	for(y = 0; y<src->sy; y++) {
		for (x=0; x<src->sx; x++) {
		      flt_r_sum = flt_g_sum = flt_b_sum = 0.0;
			cpxl = f(src, x, y);

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					if ((j == 1) && (i == 1)) {
						flt_r[1][1] = flt_g[1][1] = flt_b[1][1] = 0.5;
					} else {
						pxl = f(src, x-(3>>1)+i, y-(3>>1)+j);
						new_a = gdImageAlpha(srcback, pxl);

						new_r = ((float)gdImageRed(srcback, cpxl)) -
								((float)gdImageRed(srcback, pxl));

						if (new_r < 0.0f) {
							new_r = -new_r;
						}
						if (new_r != 0) {
							flt_r[j][i] = 1.0f/new_r;
						} else {
							flt_r[j][i] = 1.0f;
						}

						new_g = ((float)gdImageGreen(srcback, cpxl)) -
								((float)gdImageGreen(srcback, pxl));

						if (new_g < 0.0f) {
							new_g = -new_g;
						}
						if (new_g != 0) {
							flt_g[j][i] = 1.0f/new_g;
						} else {
							flt_g[j][i] = 1.0f;
						}

						new_b = ((float)gdImageBlue(srcback, cpxl)) -
								((float)gdImageBlue(srcback, pxl));

						if (new_b < 0.0f) {
							new_b = -new_b;
						}
						if (new_b != 0) {
							flt_b[j][i] = 1.0f/new_b;
						} else {
							flt_b[j][i] = 1.0f;
						}
					}

					flt_r_sum += flt_r[j][i];
					flt_g_sum += flt_g[j][i];
					flt_b_sum += flt_b [j][i];
				}
			}

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					if (flt_r_sum != 0.0) {
						flt_r[j][i] /= flt_r_sum;
					}
					if (flt_g_sum != 0.0) {
						flt_g[j][i] /= flt_g_sum;
					}
					if (flt_b_sum != 0.0) {
						flt_b [j][i] /= flt_b_sum;
					}
				}
			}

			new_r = new_g = new_b = 0.0;

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					pxl = f(src, x-(3>>1)+i, y-(3>>1)+j);
					new_r += (float)gdImageRed(srcback, pxl) * flt_r[j][i];
					new_g += (float)gdImageGreen(srcback, pxl) * flt_g[j][i];
					new_b += (float)gdImageBlue(srcback, pxl) * flt_b[j][i];
				}
			}

			new_r = (new_r > 255.0f)? 255.0f : ((new_r < 0.0f)? 0.0f:new_r);
			new_g = (new_g > 255.0f)? 255.0f : ((new_g < 0.0f)? 0.0f:new_g);
			new_b = (new_b > 255.0f)? 255.0f : ((new_b < 0.0f)? 0.0f:new_b);
			new_pxl = gdImageColorAllocateAlpha(src, (int)new_r, (int)new_g,
												(int)new_b, new_a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)new_r, (int)new_g,
												   (int)new_b, new_a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	gdImageDestroy(srcback);
	return 1;
}

/**
 * Function: gdImageEdgeDetectQuick
 *
 * Edge detection of an image
 *
 * (see edge_detect_quick.jpg)
 *
 * Parameters:
 *   src - The image.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageMeanRemoval>
 *   - <gdImageConvolution>
 */
BGD_DECLARE(int) gdImageEdgeDetectQuick(gdImagePtr src) {
	float filter[3][3] = {
		{-1.0, 0.0, -1.0}, {0.0, 4.0, 0.0}, {-1.0, 0.0, -1.0}};

	return gdImageConvolution(src, filter, 1, 127);
}

/*
  Function: gdImageGaussianBlur

	<gdImageGaussianBlur> performs a Gaussian blur of radius 1 on the
	image.  The image is modified in place.

	*NOTE:* You will almost certain want to use
	<gdImageCopyGaussianBlurred> instead, as it allows you to change
	your kernel size and sigma value.  Future versions of this
	function may fall back to calling it instead of
	<gdImageConvolution>, causing subtle changes so be warned.

  Parameters:
	im  - The image to blur

  Returns:
	GD_TRUE (1) on success, GD_FALSE (0) on failure.

*/

BGD_DECLARE(int) gdImageGaussianBlur(gdImagePtr im) {
	float filter[3][3] = {{1.0, 2.0, 1.0}, {2.0, 4.0, 2.0}, {1.0, 2.0, 1.0}};

	return gdImageConvolution(im, filter, 16, 0);
}

/**
 * Function: gdImageEmboss
 *
 * Emboss an image
 *
 * (see emboss.jpg)
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageConvolution>
 */
BGD_DECLARE(int) gdImageEmboss(gdImagePtr im) {
/*
	float filter[3][3] =	{{1.0,1.0,1.0},
				{0.0,0.0,0.0},
				{-1.0,-1.0,-1.0}};
*/
	float filter[3][3] = {{1.5, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, -1.5}};

	return gdImageConvolution(im, filter, 1, 127);
}

/**
 * Function: gdImageMeanRemoval
 *
 * Mean removal of an image
 *
 * (see mean_removal.jpg)
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageEdgeDetectQuick>
 *   - <gdImageConvolution>
 */
BGD_DECLARE(int) gdImageMeanRemoval(gdImagePtr im) {
	float filter[3][3] = {
		{-1.0, -1.0, -1.0}, {-1.0, 9.0, -1.0}, {-1.0, -1.0, -1.0}};

	return gdImageConvolution(im, filter, 1, 0);
}

/**
 * Function: gdImageSmooth
 *
 * Smooth an image
 *
 * (see smooth.jpg)
 *
 * Parameters:
 *   im     - The image.
 *   weight - The strength of the smoothing.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - <gdImageConvolution>
 */
BGD_DECLARE(int) gdImageSmooth(gdImagePtr im, float weight) {
	float filter[3][3] = {{1.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}};

	filter[1][1] = weight;

	return gdImageConvolution(im, filter, weight+8, 0);
}

/* Return an array of coefficients for 'radius' and 'sigma' (sigma >=
 * 0 means compute it).  Result length is 2*radius+1. */
static double *gaussian_coeffs(int radius, double sigmaArg) {
	const double sigma = (sigmaArg <= 0.0) ? (2.0 / 3.0) * radius : sigmaArg;
	const double s = 2.0 * sigma * sigma;
	double *result;
	double sum = 0;
	int x, n, count;

	count = 2 * radius + 1;

	result = gdMalloc(sizeof(double) * count);
	if (!result) {
		return NULL;
	} /* if */

	for (x = -radius; x <= radius; x++) {
		double coeff = exp(-(x * x) / s);

		sum += coeff;
		result[x + radius] = coeff;
	} /* for */

	for (n = 0; n < count; n++) {
		result[n] /= sum;
	} /* for */

	return result;
} /* gaussian_coeffs*/

static inline int reflect(int max, int x) {
	assert(x > -max && x < 2 * max);

	if (x < 0)
		return -x;
	if (x >= max)
		return max - (x - max) - 1;
	return x;
} /* reflect*/

static inline void applyCoeffsLine(gdImagePtr src, gdImagePtr dst, int line,
								   int linelen, double *coeffs, int radius,
								   gdAxis axis) {
	int ndx;

	for (ndx = 0; ndx < linelen; ndx++) {
		double r = 0, g = 0, b = 0, a = 0;
		int cndx;
		int *dest = (axis == HORIZONTAL) ? &dst->tpixels[line][ndx]
										 : &dst->tpixels[ndx][line];

		for (cndx = -radius; cndx <= radius; cndx++) {
			const double coeff = coeffs[cndx + radius];
			const int rndx = reflect(linelen, ndx + cndx);

			const int srcpx = (axis == HORIZONTAL) ? src->tpixels[line][rndx]
												   : src->tpixels[rndx][line];

			r += coeff * (double)gdTrueColorGetRed(srcpx);
			g += coeff * (double)gdTrueColorGetGreen(srcpx);
			b += coeff * (double)gdTrueColorGetBlue(srcpx);
			a += coeff * (double)gdTrueColorGetAlpha(srcpx);
		} /* for */

		*dest = gdTrueColorAlpha(uchar_clamp(r, 0xFF), uchar_clamp(g, 0xFF),
								 uchar_clamp(b, 0xFF), uchar_clamp(a, 0x7F));
	} /* for */
} /* applyCoeffsLine*/

static void applyCoeffs(gdImagePtr src, gdImagePtr dst, double *coeffs,
						int radius, gdAxis axis) {
	int line, numlines, linelen;

	if (axis == HORIZONTAL) {
		numlines = src->sy;
		linelen = src->sx;
	} else {
		numlines = src->sx;
		linelen = src->sy;
	} /* if .. else*/

	for (line = 0; line < numlines; line++) {
		applyCoeffsLine(src, dst, line, linelen, coeffs, radius, axis);
	} /* for */
} /* applyCoeffs*/

/*
  Function: gdImageCopyGaussianBlurred

	Return a copy of the source image _src_ blurred according to the
	parameters using the Gaussian Blur algorithm.

	_radius_ is a radius, not a diameter so a radius of 2 (for
	example) will blur across a region 5 pixels across (2 to the
	center, 1 for the center itself and another 2 to the other edge).

	_sigma_ represents the "fatness" of the curve (lower == fatter).
	If _sigma_ is less than or equal to 0,
	<gdImageCopyGaussianBlurred> ignores it and instead computes an
	"optimal" value.  Be warned that future versions of this function
	may compute sigma differently.

	The resulting image is always truecolor.

  More Details:

	A Gaussian Blur is generated by replacing each pixel's color
	values with the average of the surrounding pixels' colors.  This
	region is a circle whose radius is given by argument _radius_.
	Thus, a larger radius will yield a blurrier image.

	This average is not a simple mean of the values.  Instead, values
	are weighted using the Gaussian function (roughly a bell curve
	centered around the destination pixel) giving it much more
	influence on the result than its neighbours.  Thus, a fatter curve
	will give the center pixel more weight and make the image less
	blurry; lower _sigma_ values will yield flatter curves.

	Currently, <gdImageCopyGaussianBlurred> computes the default sigma
	as

		(2/3)*radius

	Note, however that we reserve the right to change this if we find
	a better ratio.  If you absolutely need the current sigma value,
	you should set it yourself.

  Parameters:

	src     - the source image
	radius  - the blur radius (*not* diameter--range is 2*radius + 1)
	sigma   - the sigma value or a value <= 0.0 to use the computed default

  Returns:

	The new image or NULL if an error occurred.  The result is always
	truecolor.

  Example:
	(start code)

	FILE *in;
	gdImagePtr result, src;

	in = fopen("foo.png", "rb");
	src = gdImageCreateFromPng(in);

	result = gdImageCopyGaussianBlurred(im, src->sx / 10, -1.0);

	(end code)
*/

/* TODO: Look into turning this into a generic seperable filter
 * function with Gaussian Blur being one special case.  (At the
 * moment, I can't find any other useful separable filter so for not,
 * it's just blur.) */
BGD_DECLARE(gdImagePtr)
gdImageCopyGaussianBlurred(gdImagePtr src, int radius, double sigma) {
	gdImagePtr tmp = NULL, result = NULL;
	double *coeffs;
	int freeSrc = 0;

	if (radius < 1) {
		return NULL;
	} /* if */

	/* Compute the coefficients. */
	coeffs = gaussian_coeffs(radius, sigma);
	if (!coeffs) {
		return NULL;
	} /* if */

	/* If the image is not truecolor, we first make a truecolor
	 * scratch copy. */
	if (!src->trueColor) {
		int tcstat;

		src = gdImageClone(src);
		if (!src) {
			gdFree(coeffs);
			return NULL;
		}

		tcstat = gdImagePaletteToTrueColor(src);
		if (!tcstat) {
			gdImageDestroy(src);
			gdFree(coeffs);
			return NULL;
		} /* if */

		freeSrc = 1;
	} /* if */

	/* Apply the filter horizontally. */
	tmp = gdImageCreateTrueColor(src->sx, src->sy);
	if (!tmp) {
		if (freeSrc) {
			gdImageDestroy(src);
		}
		gdFree(coeffs);
		return NULL;
	}
	applyCoeffs(src, tmp, coeffs, radius, HORIZONTAL);

	/* Apply the filter vertically. */
	result = gdImageCreateTrueColor(src->sx, src->sy);
	if (result) {
		applyCoeffs(tmp, result, coeffs, radius, VERTICAL);
	} /* if */

	gdImageDestroy(tmp);
	gdFree(coeffs);

	if (freeSrc)
		gdImageDestroy(src);

	return result;
} /* gdImageCopyGaussianBlurred*/
/* End filters function */
