

/*
   WBMP: Wireless Bitmap Type 0: B/W, Uncompressed Bitmap
   Specification of the WBMP format can be found in the file: 
   SPEC-WAESpec-19990524.pdf
   You can download the WAP specification on: http://www.wapforum.com/ 

   gd_wbmp.c

   Copyright (C) Johan Van den Brande (johan@vandenbrande.com)

   Fixed: gdImageWBMPPtr, gdImageWBMP

   Recoded: gdImageWBMPCtx for use with my wbmp library
   (wbmp library included, but you can find the latest distribution
   at http://www.vandenbrande.com/wbmp)

   Implemented: gdImageCreateFromWBMPCtx, gdImageCreateFromWBMP 

   ---------------------------------------------------------------------------

   Parts of this code are from Maurice Smurlo.


   ** Copyright (C) Maurice Szmurlo --- T-SIT --- January 2000
   ** (Maurice.Szmurlo@info.unicaen.fr)

   ** Permission to use, copy, modify, and distribute this software and its
   ** documentation for any purpose and without fee is hereby granted, provided
   ** that the above copyright notice appear in all copies and that both that
   ** copyright notice and this permission notice appear in supporting
   ** documentation.  This software is provided "as is" without express or
   ** implied warranty.

   ---------------------------------------------------------------------------
   Parts od this code are inspired by  'pbmtowbmp.c' and 'wbmptopbm.c' by 
   Terje Sannum <terje@looplab.com>.
   **
   ** Permission to use, copy, modify, and distribute this software and its
   ** documentation for any purpose and without fee is hereby granted, provided
   ** that the above copyright notice appear in all copies and that both that
   ** copyright notice and this permission notice appear in supporting
   ** documentation.  This software is provided "as is" without express or
   ** implied warranty.
   **
   ---------------------------------------------------------------------------

   Todo:

   gdCreateFromWBMP function for reading WBMP files

   ----------------------------------------------------------------------------
 */

#include <gd.h>
#include <gdfonts.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "wbmp.h"


/* gd_putout
   ** ---------
   ** Wrapper around gdPutC for use with writewbmp
   **
 */
void
gd_putout (int i, void *out)
{
  gdPutC ((unsigned char)i, (gdIOCtx *) out);
}


/* gd_getin
   ** --------
   ** Wrapper around gdGetC for use with readwbmp
   **
 */
int
gd_getin (void *in)
{
  return (gdGetC ((gdIOCtx *) in));
}


/*      gdImageWBMPCtx
   **  --------------
   **  Write the image as a wbmp file
   **  Parameters are:
   **  image:  gd image structure;
   **  fg:     the index of the foreground color. any other value will be 
   **          considered as background and will not be written
   **  out:    the stream where to write
 */
void
gdImageWBMPCtx (gdImagePtr image, int fg, gdIOCtx * out)
{

  int x, y, pos;
  Wbmp *wbmp;


  /* create the WBMP */
  if ((wbmp = createwbmp (gdImageSX (image), gdImageSY (image), WBMP_WHITE)) == NULL)
    fprintf (stderr, "Could not create WBMP\n");

  /* fill up the WBMP structure */
  pos = 0;
  for (y = 0; y < gdImageSY (image); y++)
    {
      for (x = 0; x < gdImageSX (image); x++)
	{
	  if (gdImageGetPixel (image, x, y) == fg)
	    {
	      wbmp->bitmap[pos] = WBMP_BLACK;
	    }
	  pos++;
	}
    }

  /* write the WBMP to a gd file descriptor */
  if (writewbmp (wbmp, &gd_putout, out))
    fprintf (stderr, "Could not save WBMP\n");
  /* des submitted this bugfix: gdFree the memory. */
  freewbmp (wbmp);
}


/* gdImageCreateFromWBMPCtx
   ** ------------------------
   ** Create a gdImage from a WBMP file input from an gdIOCtx
 */
gdImagePtr
gdImageCreateFromWBMPCtx (gdIOCtx * infile)
{
  /* FILE *wbmp_file; */
  Wbmp *wbmp;
  gdImagePtr im = NULL;
  int black, white;
  int col, row, pos;

  if (readwbmp (&gd_getin, infile, &wbmp))
    return (NULL);

  if (!(im = gdImageCreate (wbmp->width, wbmp->height)))
    {
      freewbmp (wbmp);
      return (NULL);
    }

  /* create the background color */
  white = gdImageColorAllocate (im, 255, 255, 255);
  /* create foreground color */
  black = gdImageColorAllocate (im, 0, 0, 0);

  /* fill in image (in a wbmp 1 = white/ 0 = black) */
  pos = 0;
  for (row = 0; row < wbmp->height; row++)
    {
      for (col = 0; col < wbmp->width; col++)
	{
	  if (wbmp->bitmap[pos++] == WBMP_WHITE)
	    {
	      gdImageSetPixel (im, col, row, white);
	    }
	  else
	    {
	      gdImageSetPixel (im, col, row, black);
	    }
	}
    }

  freewbmp (wbmp);

  return (im);
}


/* gdImageCreateFromWBMP
   ** ---------------------
 */
gdImagePtr
gdImageCreateFromWBMP (FILE * inFile)
{
  gdImagePtr im;
  gdIOCtx *in = gdNewFileCtx (inFile);
  im = gdImageCreateFromWBMPCtx (in);
  in->gd_free (in);
  return (im);
}

/* gdImageWBMP
   ** -----------
 */
void
gdImageWBMP (gdImagePtr im, int fg, FILE * outFile)
{
  gdIOCtx *out = gdNewFileCtx (outFile);
  gdImageWBMPCtx (im, fg, out);
  out->gd_free (out);
}

/* gdImageWBMPPtr
   ** --------------
 */
void *
gdImageWBMPPtr (gdImagePtr im, int *size, int fg)
{
  void *rv;
  gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
  gdImageWBMPCtx (im, fg, out);
  rv = gdDPExtractData (out, size);
  out->gd_free (out);
  return rv;
}
