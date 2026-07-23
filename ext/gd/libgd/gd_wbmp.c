/*
 * WBMP: Wireless Bitmap Type 0: B/W, Uncompressed Bitmap
 * Specification of the WBMP format can be found in the file:
 * SPEC-WAESpec-19990524.pdf
 * You can download the WAP specification on: http://www.wapforum.com/
 *
 * gd_wbmp.c
 *
 * Copyright (C) Johan Van den Brande (johan@vandenbrande.com)
 *
 * Fixed: gdImageWBMPPtr, gdImageWBMP
 *
 * Recoded: gdImageWBMPCtx for use with my wbmp library
 * (wbmp library included, but you can find the latest distribution
 * at http://www.vandenbrande.com/wbmp)
 *
 * Implemented: gdImageCreateFromWBMPCtx, gdImageCreateFromWBMP
 *
 *--------------------------------------------------------------------------
 *
 * Parts of this code are from Maurice Smurlo.
 *
 ** Copyright (C) Maurice Szmurlo --- T-SIT --- January 2000
 ** (Maurice.Szmurlo@info.unicaen.fr)
 **
 ** Permission to use, copy, modify, and distribute this software and its
 ** documentation for any purpose and without fee is hereby granted, provided
 ** that the above copyright notice appear in all copies and that both that
 ** copyright notice and this permission notice appear in supporting
 ** documentation.  This software is provided "as is" without express or
 ** implied warranty.
 *
 *--------------------------------------------------------------------------
 *
 * Parts of this code are inspired by  'pbmtowbmp.c' and 'wbmptopbm.c' by
 * Terje Sannum <terje@looplab.com>.
 *
 ** Permission to use, copy, modify, and distribute this software and its
 ** documentation for any purpose and without fee is hereby granted, provided
 ** that the above copyright notice appear in all copies and that both that
 ** copyright notice and this permission notice appear in supporting
 ** documentation.  This software is provided "as is" without express or
 ** implied warranty.
 *
 *--------------------------------------------------------------------------
 *
 * Todo:
 *
 * gdCreateFromWBMP function for reading WBMP files
 *
 *--------------------------------------------------------------------------
 */

/**
 * File: WBMP IO
 *
 * Read and write WBMP images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_errors.h"
#include "gdfonts.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "wbmp.h"

/* gd_putout
 * ---------
 * Wrapper around gdPutC for use with writewbmp
 */
static void gd_putout(int i, void *out) { gdPutC(i, (gdIOCtx *)out); }

/* gd_getin
 * --------
 * Wrapper around gdGetC for use with readwbmp
 */
static int gd_getin(void *in) { return (gdGetC((gdIOCtx *)in)); }

static int _gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out);

/*
        Function: gdImageWBMPCtx

        Write the image as a wbmp file

        Parameters:
                image - gd image structure
                fg    - the index of the foreground color. any other value will be
                                considered as background and will not be written
                out   - the stream where to write
 */
BGD_DECLARE(void) gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out)
{
    _gdImageWBMPCtx(image, fg, out);
}

/* returns 0 on success, 1 on failure */
static int _gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out)
{
    int x, y, pos;
    Wbmp *wbmp;

    /* create the WBMP */
    if ((wbmp = createwbmp(gdImageSX(image), gdImageSY(image), WBMP_WHITE)) == NULL) {
        gd_error("Could not create WBMP\n");
        return 1;
    }

    /* fill up the WBMP structure */
    pos = 0;
    for (y = 0; y < gdImageSY(image); y++) {
        for (x = 0; x < gdImageSX(image); x++) {
            if (gdImageGetPixel(image, x, y) == fg) {
                wbmp->bitmap[pos] = WBMP_BLACK;
            }
            pos++;
        }
    }

    /* write the WBMP to a gd file descriptor */
    if (writewbmp(wbmp, &gd_putout, out)) {
        freewbmp(wbmp);
        gd_error("Could not save WBMP\n");
        return 1;
    }

    /* des submitted this bugfix: gdFree the memory. */
    freewbmp(wbmp);

    return 0;
}

/*
  Function: gdImageCreateFromWBMPCtx

  Reads in a WBMP image via a <gdIOCtx> struct.  See
  <gdImageCreateFromWBMP>.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPCtx(gdIOCtx *infile)
{
    Wbmp *wbmp;
    gdImagePtr im = NULL;
    int black, white;
    int col, row, pos;

    if (readwbmp(&gd_getin, infile, &wbmp)) {
        return (NULL);
    }

    if (!(im = gdImageCreate(wbmp->width, wbmp->height))) {
        freewbmp(wbmp);
        return NULL;
    }

    /* create the background color */
    white = gdImageColorAllocate(im, 255, 255, 255);
    /* create foreground color */
    black = gdImageColorAllocate(im, 0, 0, 0);

    /* fill in image (in a wbmp 1 = white/ 0 = black) */
    pos = 0;
    for (row = 0; row < wbmp->height; row++) {
        for (col = 0; col < wbmp->width; col++) {
            if (wbmp->bitmap[pos++] == WBMP_WHITE) {
                gdImageSetPixel(im, col, row, white);
            } else {
                gdImageSetPixel(im, col, row, black);
            }
        }
    }

    freewbmp(wbmp);

    return im;
}

/*
  Function: gdImageCreateFromWBMP

        <gdImageCreateFromWBMP> is called to load images from WBMP format
        files. Invoke <gdImageCreateFromWBMP> with an already opened
        pointer to a file containing the desired
        image. <gdImageCreateFromWBMP> returns a gdImagePtr to the new
        image, or NULL if unable to load the image (most often because the
        file is corrupt or does not contain a WBMP
        image). <gdImageCreateFromWBMP> does not close the file. You can
        inspect the sx and sy members of the image to determine its
        size. The image must eventually be destroyed using
        <gdImageDestroy>.

  Variants:

        <gdImageCreateFromWBMPPtr> creates an image from WBMP data (i.e. the
        contents of a WBMP file) already in memory.

        <gdImageCreateFromWBMPCtx> reads in an image using the functions in
        a <gdIOCtx> struct.

  Parameters:

        infile - The input FILE pointer

  Returns:

        A pointer to the new image or NULL if an error occurred.

  Example:
        (start code)

        gdImagePtr im;
        FILE *in;
        in = fopen("mywbmp.wbmp", "rb");
        im = gdImageCreateFromWBMP(in);
        fclose(in);
        // ... Use the image ...
        gdImageDestroy(im);

        (end code)
 */

BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMP(FILE *inFile)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewFileCtx(inFile);
    if (in == NULL)
        return NULL;
    im = gdImageCreateFromWBMPCtx(in);
    in->gd_free(in);
    return im;
}

/*
  Function: gdImageCreateFromWBMPPtr

  Parameters:

        size - size of WBMP data in bytes.
        data - WBMP data (i.e. contents of a WBMP file).

  See <gdImageCreateFromWBMP>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
    if (!in) {
        return 0;
    }
    im = gdImageCreateFromWBMPCtx(in);
    in->gd_free(in);
    return im;
}

/*
        Function: gdImageWBMP
 */
BGD_DECLARE(void) gdImageWBMP(gdImagePtr im, int fg, FILE *outFile)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL)
        return;
    gdImageWBMPCtx(im, fg, out);
    out->gd_free(out);
}

/*
        Function: gdImageWBMPPtr
 */
BGD_DECLARE(void *) gdImageWBMPPtr(gdImagePtr im, int *size, int fg)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL)
        return NULL;
    if (!_gdImageWBMPCtx(im, fg, out)) {
        rv = gdDPExtractData(out, size);
    } else {
        rv = NULL;
    }
    out->gd_free(out);
    return rv;
}
