#if HAVE_GD_BUNDLED
# include "gd.h"
# include "gdhelpers.h"
#else
# include <gd.h>
# include "libgd/gdhelpers.h"
#endif

#include "gd_intern.h"
#include "php.h"

/* bring the palette colors in im2 to be closer to im1
 *
 */
int gdImageColorMatch (gdImagePtr im1, gdImagePtr im2)
{
	unsigned long *buf; /* stores our calculations */
	unsigned long *bp; /* buf ptr */
	int color, rgb;
	int x,y;
	int count;

	if( !im1->trueColor ) {
		return -1; /* im1 must be True Color */
	}
	if( im2->trueColor ) {
		return -2; /* im2 must be indexed */
	}
	if( (im1->sx != im2->sx) || (im1->sy != im2->sy) ) {
		return -3; /* the images are meant to be the same dimensions */
	}
	if (im2->colorsTotal<1) {
		return -4; /* At least 1 color must be allocated */
	}

	buf = (unsigned long *)safe_emalloc(sizeof(unsigned long), 5 * gdMaxColors, 0);
	memset( buf, 0, sizeof(unsigned long) * 5 * gdMaxColors );

	for (x=0; x<im1->sx; x++) {
		for( y=0; y<im1->sy; y++ ) {
			color = im2->pixels[y][x];
			rgb = im1->tpixels[y][x];
			bp = buf + (color * 5);
			(*(bp++))++;
			*(bp++) += gdTrueColorGetRed(rgb);
			*(bp++) += gdTrueColorGetGreen(rgb);
			*(bp++) += gdTrueColorGetBlue(rgb);
			*(bp++) += gdTrueColorGetAlpha(rgb);
		}
	}
	bp = buf;
	for (color=0; color<im2->colorsTotal; color++) {
		count = *(bp++);
		if( count > 0 ) {
			im2->red[color]		= *(bp++) / count;
			im2->green[color]	= *(bp++) / count;
			im2->blue[color]	= *(bp++) / count;
			im2->alpha[color]	= *(bp++) / count;
		} else {
			bp += 4;
		}
	}
	gdFree(buf);
	return 0;
}


