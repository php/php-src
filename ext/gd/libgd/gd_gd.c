/**
 * File: GD IO
 *
 * Read and write GD images.
 *
 * The GD image format is a proprietary image format of libgd. *It has to be*
 * *regarded as being obsolete, and should only be used for development and*
 * *testing purposes.*
 *
 * Structure of a GD image file:
 *  - file header
 *  - color header (either truecolor or palette)
 *  - image data
 *
 * All numbers are stored in big-endian format. Note that all GD output is done
 * in the GD 2.x format (not to be confused with the GD2 format), but input may
 * also be in the GD 1.x format.
 *
 * GD 1.x file header structure:
 *  width  - 1 word
 *  height - 1 word
 *
 * GD 1.x color header (palette only):
 *  count       - 1 byte (the number of used palette colors)
 *  transparent - 1 word (257 signals no transparency)
 *  palette     - 256×3 bytes (RGB triplets)
 *
 * GD 2.x file header structure:
 *  signature     - 1 word ("\xFF\xFE" for truecolor, "\xFF\xFF" for palette)
 *  width         - 1 word
 *  height        - 1 word
 *
 * GD 2.x truecolor image color header:
 *  truecolor   - 1 byte (always "\001")
 *  transparent - 1 dword (ARGB color); "\377\377\377\377" means that no
 *				  transparent color is set
 *
 * GD 2.x palette image color header:
 *  truecolor   - 1 byte (always "\0")
 *  count       - 1 word (the number of used palette colors)
 *  transparent - 1 dword (palette index); "\377\377\377\377" means that no
 *				  transparent color is set
 *  palette     - 256 dwords (RGBA colors)
 *
 * Image data:
 *  Sequential pixel data; row-major from top to bottom, left to right:
 *   - 1 byte per pixel for palette images
 *   - 1 dword (ARGB) per pixel for truecolor images
 */


#include "gd.h"
#include "gd_errors.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/* Exported functions: */
extern void gdImageGd (gdImagePtr im, FILE * out);


/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/*#define GD2_DBG(s) (s) */
#define GD2_DBG(s)

/*
 * Shared code to read color tables from gd file.
 */
int _gdGetColors(gdIOCtx *in, gdImagePtr im, int gd2xFlag) {
	int i;
	if (gd2xFlag) {
		int trueColorFlag;
		if (!gdGetByte(&trueColorFlag, in)) {
			goto fail1;
		}
		/* 2.0.12: detect bad truecolor .gd files created by pre-2.0.12.
		   Beginning in 2.0.12 truecolor is indicated by the initial 2-byte
		   signature. */
		if (trueColorFlag != im->trueColor) {
			goto fail1;
		}
		/* This should have been a word all along */
		if (!im->trueColor) {
			if (!gdGetWord(&im->colorsTotal, in)) {
				goto fail1;
			}
			if (im->colorsTotal > gdMaxColors) {
				goto fail1;
			}
		}
		/* Int to accommodate truecolor single-color transparency */
		if (!gdGetInt(&im->transparent, in)) {
			goto fail1;
		}
	} else {
		if (!gdGetByte(&im->colorsTotal, in)) {
			goto fail1;
		}
		if (!gdGetWord(&im->transparent, in)) {
			goto fail1;
		}
	}
	/* Make sure transparent index is within bounds of the palette. */
	if (!(im->trueColor) && (im->transparent >= im->colorsTotal || im->transparent < 0)) {
		im->transparent = (-1);
	}
	GD2_DBG(printf("Palette had %d colours (T=%d)\n", im->colorsTotal, im->transparent));
	if (im->trueColor) {
		return TRUE;
	}
	for (i = 0; (i < gdMaxColors); i++) {
		if (!gdGetByte(&im->red[i], in)) {
			goto fail1;
		}
		if (!gdGetByte(&im->green[i], in)) {
			goto fail1;
		}
		if (!gdGetByte(&im->blue[i], in)) {
			goto fail1;
		}
		if (gd2xFlag) {
			if (!gdGetByte(&im->alpha[i], in)) {
				goto fail1;
			}
		}
	}

	for (i = 0; (i < im->colorsTotal); i++) {
		im->open[i] = 0;
	}

	return TRUE;
fail1:
	return FALSE;
}

/* */
/* Use the common basic header info to make the image object. */
/* */
static gdImagePtr _gdCreateFromFile(gdIOCtx *in, int *sx, int *sy) {
	gdImagePtr im;
	int gd2xFlag = 0;
	int trueColorFlag = 0;

	if (!gdGetWord(sx, in)) {
		goto fail1;
	}
	if ((*sx == 65535) || (*sx == 65534)) {
		/* This is a gd 2.0 .gd file */
		gd2xFlag = 1;
		/* 2.0.12: 65534 signals a truecolor .gd file. There is a slight redundancy here but we can live with it. */
		if (*sx == 65534) {
			trueColorFlag = 1;
		}
		if (!gdGetWord(sx, in)) {
			goto fail1;
		}
	}
	if (!gdGetWord(sy, in)) {
		goto fail1;
	}

	GD2_DBG(printf("Image is %dx%d\n", *sx, *sy));

	if (trueColorFlag) {
		im = gdImageCreateTrueColor(*sx, *sy);
	} else {
		im = gdImageCreate(*sx, *sy);
	}
	if(!im) {
		goto fail1;
	}
	if (!_gdGetColors(in, im, gd2xFlag)) {
		goto fail2;
	}

	return im;
fail2:
	gdImageDestroy(im);
fail1:
	return 0;
}

/*
  Function: gdImageCreateFromGd

	<gdImageCreateFromGd> is called to load images from gd format
	files. Invoke <gdImageCreateFromGd> with an already opened pointer
	to a file containing the desired image in the gd file format,
	which is specific to gd and intended for very fast loading. (It is
	not intended for compression; for compression, use PNG or JPEG.)

	<gdImageCreateFromGd> returns a <gdImagePtr> to the new image, or
	NULL if unable to load the image (most often because the file is
	corrupt or does not contain a gd format
	image). <gdImageCreateFromGd> does not close the file. You can
	inspect the sx and sy members of the image to determine its
	size. The image must eventually be destroyed using
	<gdImageDestroy>.

  Variants:

	<gdImageCreateFromGdPtr> creates an image from GD data (i.e. the
	contents of a GD file) already in memory.

	<gdImageCreateFromGdCtx> reads in an image using the functions in
	a <gdIOCtx> struct.

  Parameters:

	infile - The input FILE pointer

  Returns:

	A pointer to the new image or NULL if an error occurred.

  Example:

	> gdImagePtr im;
	> FILE *in;
	> in = fopen("mygd.gd", "rb");
	> im = gdImageCreateFromGd(in);
	> fclose(in);
	> // ... Use the image ...
	> gdImageDestroy(im);
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd(FILE *inFile) {
	gdImagePtr im;
	gdIOCtx *in;

	in = gdNewFileCtx(inFile);
	if (in == NULL)
		return NULL;
	im = gdImageCreateFromGdCtx(in);

	in->gd_free(in);

	return im;
}

/*
  Function: gdImageCreateFromGdPtr

  Parameters:

	size - size of GD data in bytes.
	data - GD data (i.e. contents of a GIF file).

  Reads in GD data from memory. See <gdImageCreateFromGd>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdPtr(int size, void *data) {
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromGdCtx(in);
	in->gd_free(in);

	return im;
}

/*
  Function: gdImageCreateFromGdCtx

  Reads in a GD image via a <gdIOCtx> struct.  See
  <gdImageCreateFromGd>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdCtx(gdIOCtxPtr in) {
	int sx, sy;
	int x, y;
	gdImagePtr im;

	/* Read the header */
	im = _gdCreateFromFile(in, &sx, &sy);

	if (im == NULL) {
		goto fail1;
	}

	/* Then the data... */
	/* 2.0.12: support truecolor properly in .gd as well as in .gd2. Problem reported by Andreas Pfaller. */
	if (im->trueColor) {
		for (y = 0; (y < sy); y++) {
			for (x = 0; (x < sx); x++) {
				int pix;
				if (!gdGetInt(&pix, in)) {
					goto fail2;
				}
				im->tpixels[y][x] = pix;
			}
		}
	} else {
		for (y = 0; (y < sy); y++) {
			for (x = 0; (x < sx); x++) {
				int ch;
				ch = gdGetC(in);
				if (ch == EOF) {
					goto fail2;
				}
				/* ROW-MAJOR IN GD 1.3 */
				im->pixels[y][x] = ch;
			}
		}
	}

	return im;

fail2:
	gdImageDestroy (im);
fail1:
	return 0;
}

void _gdPutColors(gdImagePtr im, gdIOCtx *out) {
	int i;

	gdPutC(im->trueColor, out);
	if (!im->trueColor) {
		gdPutWord(im->colorsTotal, out);
	}
	gdPutInt(im->transparent, out);
	if (!im->trueColor) {
		for (i = 0; (i < gdMaxColors); i++) {
			gdPutC((unsigned char) im->red[i], out);
			gdPutC((unsigned char) im->green[i], out);
			gdPutC((unsigned char) im->blue[i], out);
			gdPutC((unsigned char) im->alpha[i], out);
		}
	}
}

static void _gdPutHeader(gdImagePtr im, gdIOCtx *out) {
	/* 65535 indicates this is a gd 2.x .gd file.
	 * 2.0.12: 65534 indicates truecolor.
	 */
	if (im->trueColor) {
		gdPutWord(65534, out);
	} else {
		gdPutWord(65535, out);
	}
	gdPutWord(im->sx, out);
	gdPutWord(im->sy, out);

	_gdPutColors(im, out);
}

static void _gdImageGd(gdImagePtr im, gdIOCtx *out) {
	int x, y;

	_gdPutHeader(im, out);

	for (y = 0; (y < im->sy); y++) {
		for (x = 0; (x < im->sx); x++) {
			/* ROW-MAJOR IN GD 1.3 */
			if (im->trueColor) {
				gdPutInt(im->tpixels[y][x], out);
			} else {
				gdPutC((unsigned char) im->pixels[y][x], out);
			}
		}
	}
}

/*
	Function: gdImageGd
 */
BGD_DECLARE(void) gdImageGd(gdImagePtr im, FILE *outFile) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL)
		return;
	_gdImageGd(im, out);
	out->gd_free(out);
}

/*
	Function: gdImageGdPtr
 */
BGD_DECLARE(void *) gdImageGdPtr(gdImagePtr im, int *size) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL)
		return NULL;
	_gdImageGd(im, out);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}
