/**
 * Title: Crop
 *
 * A couple of functions to crop images, automatically (auto detection of
 * the borders color), using a given color (with or without tolerance)
 * or using a selection.
 *
 * The threshold method works relatively well but it can be improved.
 * Maybe L*a*b* and Delta-E will give better results (and a better
 * granularity).
 *
 * Example:
 * (start code)
 *   im2 = gdImageAutoCrop(im, GD_CROP_SIDES);
 *   if (im2) {

 *   }
 *   gdImageDestroy(im2);
 *  (end code)
 **/

#include <gd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int gdGuessBackgroundColorFromCorners(gdImagePtr im, int *color);
static int gdColorMatch(gdImagePtr im, int col1, int col2, float threshold);

/**
 * Function: gdImageCrop
 *  Crops the src image using the area defined by the <crop> rectangle.
 *  The result is returned as a new image.
 *
 *
 * Parameters:
 * 	src - Source image
 *  crop - Rectangular region to crop
 *
 * Returns:
 *  <gdImagePtr> on success or NULL
 */
gdImagePtr gdImageCrop(gdImagePtr src, const gdRectPtr crop)
{
	gdImagePtr dst;
	int alphaBlendingFlag;

	if (gdImageTrueColor(src)) {
		dst = gdImageCreateTrueColor(crop->width, crop->height);
	} else {
		dst = gdImageCreate(crop->width, crop->height);
	}
	if (!dst) return NULL;
	alphaBlendingFlag = dst->alphaBlendingFlag;
	gdImageAlphaBlending(dst, gdEffectReplace);
	gdImageCopy(dst, src, 0, 0, crop->x, crop->y, crop->width, crop->height);
	gdImageAlphaBlending(dst, alphaBlendingFlag);

	return dst;
}

/**
 * Function: gdImageAutoCrop
 *  Automatic croping of the src image using the given mode
 *  (see <gdCropMode>)
 *
 *
 * Parameters:
 * 	im - Source image
 *  mode - crop mode
 *
 * Returns:
 *  <gdImagePtr> on success or NULL
 *
 * See also:
 *  <gdCropMode>
 */
gdImagePtr gdImageCropAuto(gdImagePtr im, const unsigned int mode)
{
	const int width = gdImageSX(im);
	const int height = gdImageSY(im);

	int x,y;
	int color, match;
	gdRect crop;

	crop.x = 0;
	crop.y = 0;
	crop.width = 0;
	crop.height = 0;

	switch (mode) {
		case GD_CROP_TRANSPARENT:
			color = gdImageGetTransparent(im);
			break;

		case GD_CROP_BLACK:
			color = gdImageColorClosestAlpha(im, 0, 0, 0, 0);
			break;

		case GD_CROP_WHITE:
			color = gdImageColorClosestAlpha(im, 255, 255, 255, 0);
			break;

		case GD_CROP_SIDES:
			gdGuessBackgroundColorFromCorners(im, &color);
			break;

		case GD_CROP_DEFAULT:
		default:
			color = gdImageGetTransparent(im);
			if (color == -1) {
				gdGuessBackgroundColorFromCorners(im, &color);
			}
			break;
	}

	/* TODO: Add gdImageGetRowPtr and works with ptr at the row level
	 * for the true color and palette images
	 * new formats will simply work with ptr
	 */
	match = 1;
	for (y = 0; match && y < height; y++) {
		for (x = 0; match && x < width; x++) {
			int c2 = gdImageGetPixel(im, x, y);
			match = (color == c2);
		}
	}

	/* Whole image would be cropped > bye */
	if (match) {
		return NULL;
	}

	crop.y = y - 1;

	match = 1;
	for (y = height - 1; match && y >= 0; y--) {
		for (x = 0; match && x < width; x++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}
	crop.height = y - crop.y + 2;

	match = 1;
	for (x = 0; match && x < width; x++) {
		for (y = 0; match && y < crop.y + crop.height; y++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}
	crop.x = x - 1;

	match = 1;
	for (x = width - 1; match && x >= 0; x--) {
		for (y = 0; match &&  y < crop.y + crop.height; y++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}
	crop.width = x - crop.x + 2;

	if (crop.x < 0 || crop.y < 0 || crop.width <= 0 || crop.height <= 0) {
		return NULL;
	}
	return gdImageCrop(im, &crop);
}
/*TODOs: Implement DeltaE instead, way better perceptual differences */
/**
 * Function: gdImageThresholdCrop
 *  Crop an image using a given color. The threshold argument defines
 *  the tolerance to be used while comparing the image color and the
 *  color to crop. The method used to calculate the color difference
 *  is based on the color distance in the RGB(a) cube.
 *
 *
 * Parameters:
 * 	im - Source image
 *  color - color to crop
 *  threshold - tolerance (0..100)
 *
 * Returns:
 *  <gdImagePtr> on success or NULL
 *
 * See also:
 *  <gdCropMode>, <gdImageAutoCrop> or <gdImageCrop>
 */
gdImagePtr gdImageCropThreshold(gdImagePtr im, const unsigned int color, const float threshold)
{
	const int width = gdImageSX(im);
	const int height = gdImageSY(im);

	int x,y;
	int match;
	gdRect crop;

	crop.x = 0;
	crop.y = 0;
	crop.width = 0;
	crop.height = 0;

	/* Pierre: crop everything sounds bad */
	if (threshold > 1.0) {
		return NULL;
	}

	if (!gdImageTrueColor(im) && color >= gdImageColorsTotal(im)) {
		return NULL;
	}

	/* TODO: Add gdImageGetRowPtr and works with ptr at the row level
	 * for the true color and palette images
	 * new formats will simply work with ptr
	 */
	match = 1;
	for (y = 0; match && y < height; y++) {
		for (x = 0; match && x < width; x++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x,y), threshold)) > 0;
		}
	}

	/* Whole image would be cropped > bye */
	if (match) {
		return NULL;
	}

	crop.y = y - 1;

	match = 1;
	for (y = height - 1; match && y >= 0; y--) {
		for (x = 0; match && x < width; x++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x, y), threshold)) > 0;
		}
	}
	crop.height = y - crop.y + 2;

	match = 1;
	for (x = 0; match && x < width; x++) {
		for (y = 0; match && y < crop.y + crop.height; y++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x,y), threshold)) > 0;
		}
	}
	crop.x = x - 1;

	match = 1;
	for (x = width - 1; match && x >= 0; x--) {
		for (y = 0; match &&  y < crop.y + crop.height; y++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x,y), threshold)) > 0;
		}
	}
	crop.width = x - crop.x + 2;

	return gdImageCrop(im, &crop);
}

/* This algorithm comes from pnmcrop (http://netpbm.sourceforge.net/)
 * Three steps:
 *  - if 3 corners are equal.
 *  - if two are equal.
 *  - Last solution: average the colors
 */
static int gdGuessBackgroundColorFromCorners(gdImagePtr im, int *color)
{
	const int tl = gdImageGetPixel(im, 0, 0);
	const int tr = gdImageGetPixel(im, gdImageSX(im) - 1, 0);
	const int bl = gdImageGetPixel(im, 0, gdImageSY(im) -1);
	const int br = gdImageGetPixel(im, gdImageSX(im) - 1, gdImageSY(im) -1);

	if (tr == bl && tr == br) {
		*color = tr;
		return 3;
	} else if (tl == bl && tl == br) {
		*color = tl;
		return 3;
	} else if (tl == tr &&  tl == br) {
		*color = tl;
		return 3;
	} else if (tl == tr &&  tl == bl) {
		*color = tl;
		return 3;
	} else if (tl == tr  || tl == bl || tl == br) {
		*color = tl;
		return 2;
	} else if (tr == bl || tr == br) {
		*color = tr;
		return 2;
	} else if (br == bl) {
		*color = bl;
		return 2;
	} else {
		register int r,b,g,a;

		r = (int)(0.5f + (gdImageRed(im, tl) + gdImageRed(im, tr) + gdImageRed(im, bl) + gdImageRed(im, br)) / 4);
		g = (int)(0.5f + (gdImageGreen(im, tl) + gdImageGreen(im, tr) + gdImageGreen(im, bl) + gdImageGreen(im, br)) / 4);
		b = (int)(0.5f + (gdImageBlue(im, tl) + gdImageBlue(im, tr) + gdImageBlue(im, bl) + gdImageBlue(im, br)) / 4);
		a = (int)(0.5f + (gdImageAlpha(im, tl) + gdImageAlpha(im, tr) + gdImageAlpha(im, bl) + gdImageAlpha(im, br)) / 4);
		*color = gdImageColorClosestAlpha(im, r, g, b, a);
		return 0;
	}
}

static int gdColorMatch(gdImagePtr im, int col1, int col2, float threshold)
{
	const int dr = gdImageRed(im, col1) - gdImageRed(im, col2);
	const int dg = gdImageGreen(im, col1) - gdImageGreen(im, col2);
	const int db = gdImageBlue(im, col1) - gdImageBlue(im, col2);
	const int da = gdImageAlpha(im, col1) - gdImageAlpha(im, col2);
	const double dist = sqrt(dr * dr + dg * dg + db * db + da * da);
	const double dist_perc = sqrt(dist / (255^2 + 255^2 + 255^2));
	return (dist_perc <= threshold);
	//return (100.0 * dist / 195075) < threshold;
}

/*
 * To be implemented when we have more image formats.
 * Buffer like gray8 gray16 or rgb8 will require some tweak
 * and can be done in this function (called from the autocrop
 * function. (Pierre)
 */
#if 0
static int colors_equal (const int col1, const in col2)
{

}
#endif
