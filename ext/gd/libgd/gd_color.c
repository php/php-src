#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_color.h"
#define GD_MAX_COLOR_DIST_SQ 260100

/**
 * The threshold method works relatively well for border cropping purposes.
 * A perceptually uniform metric (L*a*b* + Delta-E) would give better
 * color discrimination but at significant computational cost per pixel,
 * which is hard to justify for this use case.
 */
int gdColorMatch(gdImagePtr im, int col1, int col2, float threshold) {
	const int dr = gdImageRed(im, col1) - gdImageRed(im, col2);
	const int dg = gdImageGreen(im, col1) - gdImageGreen(im, col2);
	const int db = gdImageBlue(im, col1) - gdImageBlue(im, col2);
	const int da = (gdImageAlpha(im, col1) - gdImageAlpha(im, col2)) * 2;
	const int dist = dr * dr + dg * dg + db * db + da * da;

	const float t = threshold / 100.0f;
	return dist <= (int)(t * t * GD_MAX_COLOR_DIST_SQ);
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
