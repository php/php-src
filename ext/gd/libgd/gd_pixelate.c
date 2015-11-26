#include "gd.h"

int gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode)
{
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
					gdImageFilledRectangle(im, x, y, x + block_size - 1, y + block_size - 1, c);
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
					c = gdImageColorResolveAlpha(im, r / total, g / total, b / total, a / total);
					gdImageFilledRectangle(im, x, y, x + block_size - 1, y + block_size - 1, c);
				}
			}
		}
		break;
	default:
		return 0;
	}
	return 1;
}
