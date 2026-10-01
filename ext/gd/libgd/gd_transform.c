/**
 * File: Transformations
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"

/**
 * Function: gdImageFlipVertical
 *
 * Flip an image vertically
 *
 * The image is mirrored upside-down.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageFlipHorizontal>
 *   - <gdImageFlipBoth>
 */
BGD_DECLARE(void) gdImageFlipVertical(gdImagePtr im)
{
    register int x, y;

    if (im->trueColor) {
        for (y = 0; y < im->sy / 2; y++) {
            int *row_dst = im->tpixels[y];
            int *row_src = im->tpixels[im->sy - 1 - y];
            for (x = 0; x < im->sx; x++) {
                register int p;
                p = row_dst[x];
                row_dst[x] = im->tpixels[im->sy - 1 - y][x];
                row_src[x] = p;
            }
        }
    } else {
        unsigned char p;
        for (y = 0; y < im->sy / 2; y++) {
            for (x = 0; x < im->sx; x++) {
                p = im->pixels[y][x];
                im->pixels[y][x] = im->pixels[im->sy - 1 - y][x];
                im->pixels[im->sy - 1 - y][x] = p;
            }
        }
    }
    return;
}

/**
 * Function: gdImageFlipHorizontal
 *
 * Flip an image horizontally
 *
 * The image is mirrored left-right.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageFlipVertical>
 *   - <gdImageFlipBoth>
 */
BGD_DECLARE(void) gdImageFlipHorizontal(gdImagePtr im)
{

    int x, y;

    if (im->trueColor) {
        int *px1, *px2, tmp;

        for (y = 0; y < im->sy; y++) {
            px1 = im->tpixels[y];
            px2 = im->tpixels[y] + im->sx - 1;
            for (x = 0; x < (im->sx >> 1); x++) {
                tmp = *px1;
                *px1 = *px2;
                *px2 = tmp;
                px1++;
                px2--;
            }
        }
    } else {
        unsigned char *px1, *px2, tmp;

        for (y = 0; y < im->sy; y++) {
            px1 = im->pixels[y];
            px2 = im->pixels[y] + im->sx - 1;
            for (x = 0; x < (im->sx >> 1); x++) {
                tmp = *px1;
                *px1 = *px2;
                *px2 = tmp;
                px1++;
                px2--;
            }
        }
    }
}

/**
 * Function: gdImageFlipBoth
 *
 * Flip an image vertically and horizontally
 *
 * The image is mirrored upside-down and left-right.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageFlipVertical>
 *   - <gdImageFlipHorizontal>
 */
BGD_DECLARE(void) gdImageFlipBoth(gdImagePtr im)
{
    gdImageFlipVertical(im);
    gdImageFlipHorizontal(im);
}
