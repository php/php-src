#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include <math.h>

/* In tests this is sufficient to prevent obvious artifacts */
#define MAG 4

#define PI 3.141592
#define DEG2RAD(x) ((x) * PI / 180.)

#define MAX4(x, y, z, w)                                                       \
	((MAX((x), (y))) > (MAX((z), (w))) ? (MAX((x), (y))) : (MAX((z), (w))))
#define MIN4(x, y, z, w)                                                       \
	((MIN((x), (y))) < (MIN((z), (w))) ? (MIN((x), (y))) : (MIN((z), (w))))

#define MAXX(x) MAX4(x[0], x[2], x[4], x[6])
#define MINX(x) MIN4(x[0], x[2], x[4], x[6])
#define MAXY(x) MAX4(x[1], x[3], x[5], x[7])
#define MINY(x) MIN4(x[1], x[3], x[5], x[7])

/**
 * Function: gdImageStringFTCircle
 *
 * Draw text curved along the top and bottom of a circular area of an image.
 *
 * Parameters:
 *  im          - The image to draw onto.
 *  cx          - The x-coordinate of the center of the circular area.
 *  cy          - The y-coordinate of the center of the circular area.
 *  radius      - The radius of the circular area.
 *  textRadius  - The height of each character; if textRadius is 1/2 of radius,
 *	              characters extend halfway from the edge to the center.
 *  fillPortion - The percentage of the 180 degrees of the circular area
 *                assigned to each section of text, that is actually occupied
 *                by text. The value has to be in range 0.0 to 1.0, with useful
 *                values from about 0.4 to 0.9; 0.9 looks better than 1.0 which
 *                is rather crowded.
 *  font        - The fontlist that is passed to <gdImageStringFT>.
 *  points      - The point size, which functions as a hint. Although the size
 *                of the text is determined by radius, textRadius and
 *                fillPortion, a point size that 'hints' appropriately should be
 *                passed. If it's known that the text will be large, a large
 *                point size such as 24.0 should be passed to get the best
 *                results.
 *  top         - The text to draw clockwise at the top of the circular area.
 *  bottom      - The text to draw counterclockwise at the bottom of the
 *                circular area.
 *  fgcolor     - The font color.
 *
 * Returns:
 *  NULL on success, or an error string on failure.
 */
BGD_DECLARE(char *)
gdImageStringFTCircle(gdImagePtr im, int cx, int cy, double radius,
					  double textRadius, double fillPortion, char *font,
					  double points, char *top, char *bottom, int fgcolor) {
	char *err;
	int w;
	int brect[8];
	int sx1, sx2, sy1, sy2, sx, sy;
	int x, y;
	int fr, fg, fb, fa;
	int ox, oy;
	double prop;
	gdImagePtr im1;
	gdImagePtr im2;
	gdImagePtr im3;
	/* obtain brect so that we can size the image */
	err = gdImageStringFT((gdImagePtr)NULL, &brect[0], 0, font, points * MAG, 0,
						  0, 0, bottom);
	if (err) {
		return err;
	}
	sx1 = MAXX(brect) - MINX(brect) + 6;
	sy1 = MAXY(brect) - MINY(brect) + 6;
	err = gdImageStringFT((gdImagePtr)NULL, &brect[0], 0, font, points * MAG, 0,
						  0, 0, top);
	if (err) {
		return err;
	}
	sx2 = MAXX(brect) - MINX(brect) + 6;
	sy2 = MAXY(brect) - MINY(brect) + 6;
	/* Pad by 4 pixels to allow for slight errors
	   observed in the bounding box returned by freetype */
	if (sx1 > sx2) {
		sx = sx1 * 2 + 4;
	} else {
		sx = sx2 * 2 + 4;
	}
	if (sy1 > sy2) {
		sy = sy1;
	} else {
		sy = sy2;
	}
	im1 = gdImageCreateTrueColor(sx, sy);
	if (!im1) {
		return "could not create first image";
	}
	err =
		gdImageStringFT(im1, 0, gdTrueColor(255, 255, 255), font, points * MAG,
						0, ((sx / 2) - sx1) / 2, points * MAG, bottom);
	if (err) {
		gdImageDestroy(im1);
		return err;
	}
	/* We don't know the descent, which would be needed to do this
	   with the angle parameter. Instead, implement a simple
	   flip operation ourselves. */
	err =
		gdImageStringFT(im1, 0, gdTrueColor(255, 255, 255), font, points * MAG,
						0, sx / 2 + ((sx / 2) - sx2) / 2, points * MAG, top);
	if (err) {
		gdImageDestroy(im1);
		return err;
	}
	/* Flip in place is tricky, be careful not to double-swap things */
	if (sy & 1) {
		for (y = 0; (y <= (sy / 2)); y++) {
			int xlimit = sx - 2;
			if (y == (sy / 2)) {
				/* If there is a "middle" row, be careful
				   not to swap twice! */
				xlimit -= (sx / 4);
			}
			for (x = (sx / 2) + 2; (x < xlimit); x++) {
				int t;
				int ox = sx - x + (sx / 2) - 1;	
				int oy = sy - y - 1;
				t = im1->tpixels[oy][ox];
				im1->tpixels[oy][ox] = im1->tpixels[y][x];
				im1->tpixels[y][x] = t;
			}
		}
	} else {
		for (y = 0; (y < (sy / 2)); y++) {
			int xlimit = sx - 2;
			for (x = (sx / 2) + 2; (x < xlimit); x++) {
				int t;
				int ox = sx - x + (sx / 2) - 1;
				int oy = sy - y - 1;
				t = im1->tpixels[oy][ox];
				im1->tpixels[oy][ox] = im1->tpixels[y][x];
				im1->tpixels[y][x] = t;
			}
		}
	}
#if STEP_PNGS
	{
		FILE *out = fopen("gdfx1.png", "wb");
		gdImagePng(im1, out);
		fclose(out);
	}
#endif /* STEP_PNGS */
	/* Resample taller; the exact proportions of the text depend on the
	   ratio of textRadius to radius, and the value of fillPortion */
	if (sx > sy * 10) {
		w = sx;
	} else {
		w = sy * 10;
	}
	im2 = gdImageCreateTrueColor(w, w);
	if (!im2) {
		gdImageDestroy(im1);
		return "could not create resampled image";
	}
	prop = textRadius / radius;
	gdImageCopyResampled(im2, im1, gdImageSX(im2) * (1.0 - fillPortion) / 4,
						 sy * 10 * (1.0 - prop), 0, 0,
						 gdImageSX(im2) * fillPortion / 2, sy * 10 * prop,
						 gdImageSX(im1) / 2, gdImageSY(im1));
	gdImageCopyResampled(im2, im1,
						 (gdImageSX(im2) / 2) +
							 gdImageSX(im2) * (1.0 - fillPortion) / 4,
						 sy * 10 * (1.0 - prop), gdImageSX(im1) / 2, 0,
						 gdImageSX(im2) * fillPortion / 2, sy * 10 * prop,
						 gdImageSX(im1) / 2, gdImageSY(im1));
#if STEP_PNGS
	{
		FILE *out = fopen("gdfx2.png", "wb");
		gdImagePng(im2, out);
		fclose(out);
	}
#endif /* STEP_PNGS */

	gdImageDestroy(im1);

	/* Ready to produce a circle */
	im3 = gdImageSquareToCircle(im2, radius);
	if (im3 == NULL) {
		gdImageDestroy(im2);
		return 0;
	}
	gdImageDestroy(im2);
	/* Now blend im3 with the destination. Cheat a little. The
	   source (im3) is white-on-black, so we can use the
	   red component as a basis for alpha as long as we're
	   careful to shift off the extra bit and invert
	   (alpha ranges from 0 to 127 where 0 is OPAQUE).
	   Also be careful to allow for an alpha component
	   in the fgcolor parameter itself (gug!) */
	fr = gdTrueColorGetRed(fgcolor);
	fg = gdTrueColorGetGreen(fgcolor);
	fb = gdTrueColorGetBlue(fgcolor);
	fa = gdTrueColorGetAlpha(fgcolor);
	ox = cx - (im3->sx / 2);
	oy = cy - (im3->sy / 2);
	for (y = 0; (y < im3->sy); y++) {
		for (x = 0; (x < im3->sx); x++) {
			int a = gdTrueColorGetRed(im3->tpixels[y][x]) >> 1;
			a *= (127 - fa);
			a /= 127;
			a = 127 - a;
			gdImageSetPixel(im, x + ox, y + oy,
							gdTrueColorAlpha(fr, fg, fb, a));
		}
	}
	gdImageDestroy(im3);
	return 0;
}

#if GDFX_MAIN

int main(int argc, char *argv[]) {
	FILE *in;
	FILE *out;
	gdImagePtr im;
	int radius;
	/* Create an image of text on a circle, with an
	   alpha channel so that we can copy it onto a
	   background */
	in = fopen("eleanor.jpg", "rb");
	if (!in) {
		im = gdImageCreateTrueColor(300, 300);
	} else {
		im = gdImageCreateFromJpeg(in);
		fclose(in);
	}
	if (gdImageSX(im) < gdImageSY(im)) {
		radius = gdImageSX(im) / 2;
	} else {
		radius = gdImageSY(im) / 2;
	}
	gdImageStringFTCircle(im, gdImageSX(im) / 2, gdImageSY(im) / 2, radius,
						  radius / 2, 0.8, "arial", 24, "top text",
						  "bottom text", gdTrueColorAlpha(240, 240, 255, 32));
	out = fopen("gdfx.png", "wb");
	if (!out) {
		gd_error("Can't create gdfx.png\n");
		return 1;
	}
	gdImagePng(im, out);
	fclose(out);
	gdImageDestroy(im);
	return 0;
}

#endif /* GDFX_MAIN */

/* Note: don't change these */
#define SUPER 2
#define SUPERBITS1 1
#define SUPERBITS2 2

/**
 * Function: gdImageSquareToCircle
 *
 * Apply polar coordinate transformation to an image.
 *
 * The X axis of the original will be remapped to theta (angle) and the Y axis
 * of the original will be remapped to rho (distance from center).
 *
 * Parameters:
 *  im     - The image, which must be square, i.e. width == height.
 *  radius - The radius of the new image, i.e. width == height == radius * 2.
 *
 * Returns:
 *  The transformed image, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageSquareToCircle(gdImagePtr im, int radius) {
	int x, y;
	double c;
	gdImagePtr im2;
	if (im->sx != im->sy) {
		/* Source image must be square */
		return 0;
	}
	im2 = gdImageCreateTrueColor(radius * 2, radius * 2);
	if (!im2) {
		return 0;
	}
	/* Supersampling for a nicer result */
	c = (im2->sx / 2) * SUPER;
	for (y = 0; (y < im2->sy * SUPER); y++) {
		for (x = 0; (x < im2->sx * SUPER); x++) {
			double rho = sqrt((x - c) * (x - c) + (y - c) * (y - c));
			int pix;
			int cpix;
			double theta;
			double ox;
			double oy;
			int red, green, blue, alpha;
			if (rho > c) {
				continue;
			}
			theta = atan2(x - c, y - c) + PI / 2;
			if (theta < 0) {
				theta += 2 * PI;
			}
			/* Undo supersampling */
			oy = (rho * im->sx) / (im2->sx * SUPER / 2);
			ox = theta * im->sx / (3.141592653 * 2);
			pix = gdImageGetPixel(im, ox, oy);
			cpix = im2->tpixels[y >> SUPERBITS1][x >> SUPERBITS1];
			red = (gdImageRed(im, pix) >> SUPERBITS2) + gdTrueColorGetRed(cpix);
			green = (gdImageGreen(im, pix) >> SUPERBITS2) +
					gdTrueColorGetGreen(cpix);
			blue =
				(gdImageBlue(im, pix) >> SUPERBITS2) + gdTrueColorGetBlue(cpix);
			alpha = (gdImageAlpha(im, pix) >> SUPERBITS2) +
					gdTrueColorGetAlpha(cpix);
			im2->tpixels[y >> SUPERBITS1][x >> SUPERBITS1] =
				gdTrueColorAlpha(red, green, blue, alpha);
		}
	}
	/* Restore full dynamic range, 0-63 yields 0-252. Replication of
	   first 2 bits in last 2 bits has the desired effect. Note
	   slightly different arithmetic for alpha which is 7-bit.
	   NOTE: only correct for SUPER == 2 */
	for (y = 0; (y < im2->sy); y++) {
		for (x = 0; (x < im2->sx); x++) {
			/* Copy first 2 bits to last 2 bits, matching the
			   dynamic range of the original cheaply */
			int cpix = im2->tpixels[y][x];

			im2->tpixels[y][x] =
				gdTrueColorAlpha((gdTrueColorGetRed(cpix) & 0xFC) +
									 ((gdTrueColorGetRed(cpix) & 0xC0) >> 6),
								 (gdTrueColorGetGreen(cpix) & 0xFC) +
									 ((gdTrueColorGetGreen(cpix) & 0xC0) >> 6),
								 (gdTrueColorGetBlue(cpix) & 0xFC) +
									 ((gdTrueColorGetBlue(cpix) & 0xC0) >> 6),
								 (gdTrueColorGetAlpha(cpix) & 0x7C) +
									 ((gdTrueColorGetAlpha(cpix) & 0x60) >> 6));
		}
	}
	return im2;
}

/* 2.0.16: Called by gdImageSharpen to avoid excessive code repetition
	Added on 2003-11-19 by
	Paul Troughton (paul<dot>troughton<at>ieee<dot>org)
	Given filter coefficents and colours of three adjacent pixels,
returns new colour for centre pixel
*/

static int gdImageSubSharpen(int pc, int c, int nc, float inner_coeff,
							 float outer_coeff) {
	float red, green, blue, alpha;

	red = inner_coeff * gdTrueColorGetRed(c) +
		  outer_coeff * (gdTrueColorGetRed(pc) + gdTrueColorGetRed(nc));
	green = inner_coeff * gdTrueColorGetGreen(c) +
			outer_coeff * (gdTrueColorGetGreen(pc) + gdTrueColorGetGreen(nc));
	blue = inner_coeff * gdTrueColorGetBlue(c) +
		   outer_coeff * (gdTrueColorGetBlue(pc) + gdTrueColorGetBlue(nc));
	alpha = gdTrueColorGetAlpha(c);

	/* Clamping, as can overshoot bounds in either direction */
	if (red > 255.0f) {
		red = 255.0f;
	}
	if (green > 255.0f) {
		green = 255.0f;
	}
	if (blue > 255.0f) {
		blue = 255.0f;
	}
	if (red < 0.0f) {
		red = 0.0f;
	}
	if (green < 0.0f) {
		green = 0.0f;
	}
	if (blue < 0.0f) {
		blue = 0.0f;
	}

	return gdTrueColorAlpha((int)red, (int)green, (int)blue, (int)alpha);
}

/**
 * Function: gdImageSharpen
 *
 * Sharpen an image.
 *
 * Uses a simple 3x3 convolution kernel and makes use of separability.
 * It's faster, but less flexible, than full-blown unsharp masking.
 * Silently does nothing to non-truecolor images and for pct<0, as it's not a
 * useful blurring function.
 *
 * Parameters:
 *  pct - The sharpening percentage, which can be greater than 100.
 *
 * Author:
 *  Paul Troughton (paul<dot>troughton<at>ieee<dot>org)
 */
BGD_DECLARE(void)
gdImageSharpen(gdImagePtr im, int pct) {
	int x, y;
	int sx, sy;
	float inner_coeff, outer_coeff;

	sx = im->sx;
	sy = im->sy;

	/* Must sum to 1 to avoid overall change in brightness.
	 * Scaling chosen so that pct=100 gives 1-D filter [-1 6 -1]/4,
	 * resulting in a 2-D filter [1 -6 1; -6 36 -6; 1 -6 1]/16,
	 * which gives noticeable, but not excessive, sharpening
	 */

	outer_coeff = -pct / 400.0;
	inner_coeff = 1 - 2 * outer_coeff;

	/* Don't try to do anything with non-truecolor images, as
	   pointless,
	   * nor for pct<=0, as small kernel size leads to nasty
	   artefacts when blurring
	 */
	if ((im->trueColor) && (pct > 0)) {

		/* First pass, 1-D convolution column-wise */
		for (x = 0; x < sx; x++) {

			/* pc is colour of previous pixel; c of the
			   current pixel and nc of the next */
			int pc, c, nc;

			/* Replicate edge pixel at image boundary */
			pc = gdImageGetPixel(im, x, 0);

			/* Stop looping before last pixel to avoid
			   conditional within loop */
			for (y = 0; y < sy - 1; y++) {

				c = gdImageGetPixel(im, x, y);

				nc = gdImageGetTrueColorPixel(im, x, y + 1);

				/* Update centre pixel to new colour */
				gdImageSetPixel(
					im, x, y,
					gdImageSubSharpen(pc, c, nc, inner_coeff, outer_coeff));

				/* Save original colour of current
				   pixel for next time round */
				pc = c;
			}

			/* Deal with last pixel, replicating current
			   pixel at image boundary */
			c = gdImageGetPixel(im, x, y);
			gdImageSetPixel(
				im, x, y,
				gdImageSubSharpen(pc, c, c, inner_coeff, outer_coeff));
		}

		/* Second pass, 1-D convolution row-wise */
		for (y = 0; y < sy; y++) {
			int pc, c;
			pc = gdImageGetPixel(im, 0, y);
			for (x = 0; x < sx - 1; x++) {
				int c, nc;
				c = gdImageGetPixel(im, x, y);
				nc = gdImageGetTrueColorPixel(im, x + 1, y);
				gdImageSetPixel(
					im, x, y,
					gdImageSubSharpen(pc, c, nc, inner_coeff, outer_coeff));
				pc = c;
			}
			c = gdImageGetPixel(im, x, y);
			gdImageSetPixel(
				im, x, y,
				gdImageSubSharpen(pc, c, c, inner_coeff, outer_coeff));
		}
	}
}
