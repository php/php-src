/*
 * Add ability to load xpm files to gd, requires the xpm
 * library.
 * Caolan.McNamara@ul.ie
 * http://www.csn.ul.ie/~caolan
 */

/**
 * File: XPM Input
 *
 * Read XPM images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_color_map.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HAVE_LIBXPM
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename) {
	(void)filename;
	gd_error_ex(GD_ERROR, "libgd was not built with xpm support\n");
	return NULL;
}
#else

#include <X11/xpm.h>

/*
  Function: gdImageCreateFromXpm

	<gdImageCreateFromXbm> is called to load images from XPM X Window
	System color bitmap format files. This function is available only
	if HAVE_XPM is selected in the Makefile and the Xpm library is
	linked with the application. Unlike most gd file functions, the
	Xpm functions *require filenames*, not file
	pointers. <gdImageCreateFromXpm> returns a <gdImagePtr> to the new
	image, or NULL if unable to load the image (most often because the
	file is corrupt or does not contain an XPM bitmap format
	image). You can inspect the sx and sy members of the image to
	determine its size. The image must eventually be destroyed using
	<gdImageDestroy>.

  Parameters:

	filename - The input filename (*not* FILE pointer)

  Returns:

	A pointer to the new image or NULL if an error occurred.

  Example:
	(start code)

	gdImagePtr im;
	FILE *in;
	in = fopen("myxpm.xpm", "rb");
	im = gdImageCreateFromXpm(in);
	fclose(in);
	// ... Use the image ...
	gdImageDestroy(im);

	(end code)

*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename) {
	XpmInfo info = {0};
	XpmImage image;
	unsigned int i, j, k, number, len;
	char buf[5];
	gdImagePtr im = 0;
	int *pointer;
	int red = 0, green = 0, blue = 0;
	int *colors;
	int ret;

	ret = XpmReadFileToXpmImage(filename, &image, &info);
	if (ret != XpmSuccess) {
		return 0;
	}

	number = image.ncolors;
	if (overflow2(sizeof(int), number)) {
		goto done;
	}
	for (i = 0; i < number; i++) {
		/*
		   avoid NULL pointer dereference
		   TODO better fix need to manage monochrome/monovisual
		   see m_color or g4_color or g_color
		*/
		if (!image.colorTable[i].c_color) {
			goto done;
		}
	}

	colors = (int *)gdMalloc(sizeof(int) * number);
	if (colors == NULL) {
		goto done;
	}

	if (!(im = gdImageCreate(image.width, image.height))) {
		gdFree(colors);
		goto done;
	}

	for (i = 0; i < number; i++) {
		char *c_color = image.colorTable[i].c_color;
		int valid_color = 1;
		if (strcmp(c_color, "None") == 0) {
			colors[i] = gdImageGetTransparent(im);
			if (colors[i] == -1)
				colors[i] = gdImageColorAllocate(im, 0, 0, 0);
			if (colors[i] != -1)
				gdImageColorTransparent(im, colors[i]);
			continue;
		}
		len = strlen(c_color);
		if (len < 1) {
			valid_color = 0;
		}
		if (c_color[0] == '#') {
			switch (len) {
			case 4:
				buf[2] = '\0';
				buf[0] = buf[1] = c_color[1];
				red = strtol(buf, NULL, 16);

				buf[0] = buf[1] = c_color[2];
				green = strtol(buf, NULL, 16);

				buf[0] = buf[1] = c_color[3];
				blue = strtol(buf, NULL, 16);
				break;

			case 7:
				buf[2] = '\0';
				buf[0] = c_color[1];
				buf[1] = c_color[2];
				red = strtol(buf, NULL, 16);

				buf[0] = c_color[3];
				buf[1] = c_color[4];
				green = strtol(buf, NULL, 16);

				buf[0] = c_color[5];
				buf[1] = c_color[6];
				blue = strtol(buf, NULL, 16);
				break;

			case 10:
				buf[3] = '\0';
				buf[0] = c_color[1];
				buf[1] = c_color[2];
				buf[2] = c_color[3];
				red = strtol(buf, NULL, 16);
				red /= 64;

				buf[0] = c_color[4];
				buf[1] = c_color[5];
				buf[2] = c_color[6];
				green = strtol(buf, NULL, 16);
				green /= 64;

				buf[0] = c_color[7];
				buf[1] = c_color[8];
				buf[2] = c_color[9];
				blue = strtol(buf, NULL, 16);
				blue /= 64;
				break;

			case 13:
				buf[4] = '\0';
				buf[0] = c_color[1];
				buf[1] = c_color[2];
				buf[2] = c_color[3];
				buf[3] = c_color[4];
				red = strtol(buf, NULL, 16);
				red /= 256;

				buf[0] = c_color[5];
				buf[1] = c_color[6];
				buf[2] = c_color[7];
				buf[3] = c_color[8];
				green = strtol(buf, NULL, 16);
				green /= 256;

				buf[0] = c_color[9];
				buf[1] = c_color[10];
				buf[2] = c_color[11];
				buf[3] = c_color[12];
				blue = strtol(buf, NULL, 16);
				blue /= 256;
				break;
			default:
				valid_color = 0;
				break;
			}
		} else if (!gdColorMapLookup(GD_COLOR_MAP_X11, c_color, &red, &green,
									 &blue)) {
			valid_color = 0;
		}

		if (!valid_color) {
			gdFree(colors);
			gdImageDestroy(im);
			im = NULL;
			goto done;
		}

		colors[i] = gdImageColorResolve(im, red, green, blue);
	}

	pointer = (int *)image.data;

	for (i = 0; i < image.height; i++) {
		for (j = 0; j < image.width; j++) {
			k = *pointer++;
			if (k >= number) {
				gdFree(colors);
				gdImageDestroy(im);
				im = NULL;
				goto done;
			}
			gdImageSetPixel(im, j, i, colors[k]);
		}
	}

	gdFree(colors);

done:
	XpmFreeXpmImage(&image);
	XpmFreeXpmInfo(&info);
	return im;
}
#endif /* HAVE_LIBXPM */
