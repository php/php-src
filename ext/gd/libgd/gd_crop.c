/**
 * File: Cropping
 *
 * Crop an image
 *
 * Some functions to crop images, automatically (auto detection of the border
 * color), using a given color (with or without tolerance) or using a given
 * rectangle.
 *
 * Example:
 * (start code)
 *   im2 = gdImageAutoCrop(im, GD_CROP_SIDES);
 *   if (im2) {
 *       gdImageDestroy(im); // unless you need the original image subsequently
 *       // do something with the cropped image
 *   }
 *   gdImageDestroy(im2);
 *  (end code)
 **/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gd.h"
#include "gd_color.h"
#include <stdlib.h>

static int gdGuessBackgroundColorFromCorners(gdImagePtr im, int *color);

/**
 * Function: gdImageCrop
 *
 * Crop an image to a given rectangle
 *
 * Parameters:
 *   src  - The image.
 *   crop - The cropping rectangle, see <gdRect>.
 *
 * Returns:
 *   The newly created cropped image, or NULL on failure.
 *
 * See also:
 *   - <gdImageCropAuto>
 *   - <gdImageCropThreshold>
 */
BGD_DECLARE(gdImagePtr) gdImageCrop(gdImagePtr src, const gdRect *crop) {
	gdImagePtr dst;
	int alphaBlendingFlag;

	if (gdImageTrueColor(src)) {
		dst = gdImageCreateTrueColor(crop->width, crop->height);
	} else {
		dst = gdImageCreate(crop->width, crop->height);
	}
	if (!dst)
		return NULL;
	alphaBlendingFlag = dst->alphaBlendingFlag;
	gdImageAlphaBlending(dst, gdEffectReplace);
	gdImageCopy(dst, src, 0, 0, crop->x, crop->y, crop->width, crop->height);
	gdImageAlphaBlending(dst, alphaBlendingFlag);

	return dst;
}

/**
 * Function: gdImageCropAuto
 *
 * Crop an image automatically
 *
 * This function detects the cropping area according to the given _mode_.
 *
 * Parameters:
 *   im   - The image.
 *   mode - The cropping mode, see <gdCropMode>.
 *
 * Returns:
 *   The newly created cropped image, or NULL on failure.
 *
 * See also:
 *   - <gdImageCrop>
 *   - <gdImageCropThreshold>
 */
BGD_DECLARE(gdImagePtr)
gdImageCropAuto(gdImagePtr im, const unsigned int mode) {
	const int width = gdImageSX(im);
	const int height = gdImageSY(im);

	int x,y;
	int color;
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
			break;
	}

	for (x = 0, y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (color != gdImageGetPixel(im, x, y)) {
				goto break1;
		}
	}
	}
break1:

	/* Whole image would be cropped > bye */
	if (y == height && x == width) {
		return NULL;
	}

	crop.y = y;

	for (y = height - 1; y >= 0; y--) {
		for (x = 0; x < width; x++) {
			if (color != gdImageGetPixel(im, x, y)) {
				goto break2;
		}
	}
	}
break2:

	crop.height = y - crop.y + 1;

	for (x = 0; x < width; x++) {
		for (y = crop.y; y < crop.y + crop.height; y++) {
			if (color != gdImageGetPixel(im, x, y)) {
				goto break3;
		}
	}
	}
break3:

	crop.x = x;

	for (x = width - 1; x >= 0; x--) {
		for (y = crop.y; y < crop.y + crop.height; y++) {
			if (color != gdImageGetPixel(im, x, y)) {
				goto break4;
		}
	}
	}
break4:

	crop.width = x - crop.x + 1;

	return gdImageCrop(im, &crop);
}

/**
 * Function: gdImageCropThreshold
 *
 * Crop an image using a given color
 *
 * The _threshold_ defines the tolerance to be used while comparing the image
 * color and the color to crop. The method used to calculate the color
 * difference is based on the color distance in the RGB(A) cube.
 *
 * Parameters:
 *   im        - The image.
 *   color     - The crop color.
 *   threshold - The crop threshold.
 *
 * Returns:
 *   The newly created cropped image, or NULL on failure.
 *
 * See also:
 *   - <gdImageCrop>
 *   - <gdImageCropAuto>
 */
BGD_DECLARE(gdImagePtr)
gdImageCropThreshold(gdImagePtr im, const unsigned int color,
					 const float threshold) {
	const int width = gdImageSX(im);
	const int height = gdImageSY(im);

	int x,y;
	gdRect crop;

	crop.x = 0;
	crop.y = 0;
	crop.width = 0;
	crop.height = 0;

	/* To crop everything sounds bad */
	if (threshold > 100.0) {
		return NULL;
	}

	if (!gdImageTrueColor(im) &&
		color >= (unsigned int)gdImageColorsTotal(im)) {
		return NULL;
	}

	for (x = 0, y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (!gdColorMatch(im, color, gdImageGetPixel(im, x, y),
							  threshold)) {
				goto break1;
		}
	}
	}
break1:

	/* Whole image would be cropped > bye */
	if (y == height && x == width) {
		return NULL;
	}

	crop.y = y;

	for (y = height - 1; y >= 0; y--) {
		for (x = 0; x < width; x++) {
			if (!gdColorMatch(im, color, gdImageGetPixel(im, x, y),
							  threshold)) {
				goto break2;
		}
	}
	}
break2:

	crop.height = y - crop.y + 1;

	for (x = 0; x < width; x++) {
		for (y = crop.y; y < crop.y + crop.height; y++) {
			if (!gdColorMatch(im, color, gdImageGetPixel(im, x, y),
							  threshold)) {
				goto break3;
		}
	}
	}
break3:

	crop.x = x;

	for (x = width - 1; x >= 0; x--) {
		for (y = crop.y; y < crop.y + crop.height; y++) {
			if (!gdColorMatch(im, color, gdImageGetPixel(im, x, y),
							  threshold)) {
				goto break4;
		}
	}
	}
break4:

	crop.width = x - crop.x + 1;

	return gdImageCrop(im, &crop);
}

/* This algorithm comes from pnmcrop (http://netpbm.sourceforge.net/)
 * Three steps:
 *  - if 3 corners are equal.
 *  - if two are equal.
 *  - Last solution: average the colors
 */
static int gdGuessBackgroundColorFromCorners(gdImagePtr im, int *color) {
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

		r = (2 + gdImageRed(im, tl) + gdImageRed(im, tr) + gdImageRed(im, bl) +
			 gdImageRed(im, br)) /
			4;
		g = (2 + gdImageGreen(im, tl) + gdImageGreen(im, tr) +
			 gdImageGreen(im, bl) + gdImageGreen(im, br)) /
			4;
		b = (2 + gdImageBlue(im, tl) + gdImageBlue(im, tr) +
			 gdImageBlue(im, bl) + gdImageBlue(im, br)) /
			4;
		a = (2 + gdImageAlpha(im, tl) + gdImageAlpha(im, tr) +
			 gdImageAlpha(im, bl) + gdImageAlpha(im, br)) /
			4;
		*color = gdImageColorClosestAlpha(im, r, g, b, a);
		return 0;
	}
}
