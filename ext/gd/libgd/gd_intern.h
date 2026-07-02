#ifndef GD_INTERN_H
#define GD_INTERN_H
#include <limits.h>
#include "gd.h"
#ifdef _MSC_VER
#ifndef SSIZE_MAX
#ifdef _WIN64
#define SSIZE_MAX _I64_MAX
#else
#define SSIZE_MAX INT_MAX
#endif
#endif
#endif
#if defined(_MSC_VER)
#define UNUSED_PARAM(x) x
#elif defined(__GNUC__) || defined(__clang__)
#define UNUSED_PARAM(x) x __attribute__((unused))
#else
#define UNUSED_PARAM(x) x
#endif
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#define MIN3(a,b,c) ((a)<(b)?(MIN(a,c)):(MIN(b,c)))
#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif
#define MAX3(a,b,c) ((a)<(b)?(MAX(b,c)):(MAX(a,c)))
#define CLAMP(x, low, high)                                                    \
	(((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifdef _MSC_VER
#define gd_strcasecmp _stricmp
#else
#define gd_strcasecmp strcasecmp
#endif

typedef enum {
	HORIZONTAL,
	VERTICAL,
} gdAxis;

/* Convert a double to an unsigned char, rounding to the nearest
 * integer and clamping the result between 0 and max.  The absolute
 * value of clr must be less than the maximum value of an unsigned
 * short. */
static inline unsigned char uchar_clamp(double clr, unsigned char max) {
	unsigned short result;

	// assert(fabs(clr) <= SHRT_MAX);

	/* Casting a negative float to an unsigned short is undefined.
	 * However, casting a float to a signed truncates toward zero and
	 * casting a negative signed value to an unsigned of the same size
	 * results in a bit-identical value (assuming twos-complement
	 * arithmetic).	 This is what we want: all legal negative values
	 * for clr will be greater than 255. */

	/* Convert and clamp. */
	result = (unsigned short)(short)(clr + 0.5);
	if (result > max) {
		result = (clr < 0) ? 0 : max;
	} /* if */

	return result;
} /* uchar_clamp*/

/* Internal prototypes: */

/* gd_jpeg.c */
void *gdImageJpegPtrWithMetadataNoSubsampling(gdImagePtr im, int *size,
											  int quality,
											  const gdImageMetadata *metadata);

/* gd_rotate.c */
gdImagePtr gdImageRotate90(gdImagePtr src, int ignoretransparent);
gdImagePtr gdImageRotate180(gdImagePtr src, int ignoretransparent);
gdImagePtr gdImageRotate270(gdImagePtr src, int ignoretransparent);

/**
 * Clip a copy operation to the destination image bounds.
 * Adjusts srcX/srcY to match any clipping applied to dstX/dstY.
 * Returns 0 if the operation is entirely outside dst (nothing to do),
 * 1 if there is work to do.
 */
static inline int gdImageClipCopy(gdImagePtr dst, int *dstX, int *dstY,
								  int *srcX, int *srcY, int *w, int *h) {
	int x1, y1, x2, y2;

	/* overflow-safe dst rect: [dstX, dstY] to [dstX+w, dstY+h] */
	x1 = *dstX;
	y1 = *dstY;

	/* check w/h are positive */
	if (*w <= 0 || *h <= 0) {
		return 0;
	}

	/* overflow check for dstX+w and dstY+h */
	if (*dstX > 0 && *w > INT_MAX - *dstX) {
		x2 = INT_MAX;
	} else {
		x2 = *dstX + *w;
	}
	if (*dstY > 0 && *h > INT_MAX - *dstY) {
		y2 = INT_MAX;
	} else {
		y2 = *dstY + *h;
	}

	/* entirely outside dst? */
	if (x1 >= gdImageSX(dst) || y1 >= gdImageSY(dst) || x2 <= 0 || y2 <= 0) {
		return 0;
	}

	/* clip left */
	if (x1 < 0) {
		*srcX -= x1; /* advance srcX by the same amount */
		*w += x1;	 /* reduce width */
		*dstX = 0;
	}

	/* clip top */
	if (y1 < 0) {
		*srcY -= y1;
		*h += y1;
		*dstY = 0;
	}

	/* clip right */
	if (*dstX + *w > gdImageSX(dst)) {
		*w = gdImageSX(dst) - *dstX;
	}

	/* clip bottom */
	if (*dstY + *h > gdImageSY(dst)) {
		*h = gdImageSY(dst) - *dstY;
	}

	/* sanity: clipping may have reduced w/h to zero */
	if (*w <= 0 || *h <= 0) {
		return 0;
	}

	return 1;
}
static inline int gdImageClipCopyResized(gdImagePtr dst, int *dstX, int *dstY,
										 int *dstW, int *dstH, int *srcX,
										 int *srcY, int *srcW, int *srcH) {
	int orig_dstW = *dstW;
	int orig_dstH = *dstH;

	if (*dstW <= 0 || *dstH <= 0 || *srcW <= 0 || *srcH <= 0) {
		return 0;
	}
	if (*dstX >= gdImageSX(dst) || *dstY >= gdImageSY(dst) ||
		*dstX + *dstW <= 0 || *dstY + *dstH <= 0) {
		return 0;
	}

	/* clip left — adjust srcX proportionally */
	if (*dstX < 0) {
		*srcX += (-*dstX) * *srcW / orig_dstW;
		*srcW -= (-*dstX) * *srcW / orig_dstW;
		*dstW += *dstX;
		*dstX = 0;
	}
	/* clip top */
	if (*dstY < 0) {
		*srcY += (-*dstY) * *srcH / orig_dstH;
		*srcH -= (-*dstY) * *srcH / orig_dstH;
		*dstH += *dstY;
		*dstY = 0;
	}
	/* clip right */
	if (*dstX + *dstW > gdImageSX(dst)) {
		int clip = *dstX + *dstW - gdImageSX(dst);
		*srcW -= clip * *srcW / orig_dstW;
		*dstW = gdImageSX(dst) - *dstX;
	}
	/* clip bottom */
	if (*dstY + *dstH > gdImageSY(dst)) {
		int clip = *dstY + *dstH - gdImageSY(dst);
		*srcH -= clip * *srcH / orig_dstH;
		*dstH = gdImageSY(dst) - *dstY;
	}

	if (*dstW <= 0 || *dstH <= 0)
		return 0;
	return 1;
}
#endif /* GD_INTERN_H */
