
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"

#define TRUE 1
#define FALSE 0

/* Exported functions: */
extern void gdImageGd (gdImagePtr im, FILE * out);


/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/*#define GD2_DBG(s) (s) */
#define GD2_DBG(s)

/* */
/* Shared code to read color tables from gd file. */
/* */
int
_gdGetColors (gdIOCtx * in, gdImagePtr im, int gd2xFlag)
{
  int i;
  if (gd2xFlag)
    {
      if (!gdGetByte (&im->trueColor, in))
	{
	  goto fail1;
	}
      /* This should have been a word all along */
      if (!im->trueColor)
	{
	  if (!gdGetWord (&im->colorsTotal, in))
	    {
	      goto fail1;
	    }
	}
      /* Int to accommodate truecolor single-color transparency */
      if (!gdGetInt (&im->transparent, in))
	{
	  goto fail1;
	}
    }
  else
    {
      if (!gdGetByte (&im->colorsTotal, in))
	{
	  goto fail1;
	}
      if (!gdGetWord (&im->transparent, in))
	{
	  goto fail1;
	}
      if (im->transparent == 257)
	{
	  im->transparent = (-1);
	}
    }
  GD2_DBG (printf ("Pallette had %d colours (T=%d)\n", im->colorsTotal, im->transparent));

  for (i = 0; (i < gdMaxColors); i++)
    {
      if (!gdGetByte (&im->red[i], in))
	{
	  goto fail1;
	}
      if (!gdGetByte (&im->green[i], in))
	{
	  goto fail1;
	}
      if (!gdGetByte (&im->blue[i], in))
	{
	  goto fail1;
	}
      if (gd2xFlag)
	{
	  if (!gdGetByte (&im->alpha[i], in))
	    {
	      goto fail1;
	    }
	}
    }

  for (i = 0; (i < im->colorsTotal); i++)
    {
      im->open[i] = 0;
    };

  return TRUE;
fail1:
  return FALSE;
}

/* */
/* Use the common basic header info to make the image object. */
/* This is also called from _gd2CreateFromFile */
/* */
static
  gdImagePtr
_gdCreateFromFile (gdIOCtx * in, int *sx, int *sy)
{
  gdImagePtr im;
  int gd2xFlag = 0;
  if (!gdGetWord (sx, in))
    {
      goto fail1;
    }
  if (*sx == 65535)
    {
      /* This is a gd 2.0 .gd file */
      gd2xFlag = 1;
      if (!gdGetWord (sx, in))
	{
	  goto fail1;
	}
    }
  if (!gdGetWord (sy, in))
    {
      goto fail1;
    }

  GD2_DBG (printf ("Image is %dx%d\n", *sx, *sy));

  im = gdImageCreate (*sx, *sy);

  if (!_gdGetColors (in, im, gd2xFlag))
    {
      goto fail2;
    }

  return im;
fail2:
  gdImageDestroy (im);
fail1:
  return 0;
}

gdImagePtr
gdImageCreateFromGd (FILE * inFile)
{
  gdImagePtr im;
  gdIOCtx *in;

  in = gdNewFileCtx (inFile);
  im = gdImageCreateFromGdCtx (in);

  in->free (in);

  return im;
}

gdImagePtr
gdImageCreateFromGdCtx (gdIOCtxPtr in)
{
  int sx, sy;
  int x, y;
  gdImagePtr im;

  /* Read the header */
  im = _gdCreateFromFile (in, &sx, &sy);

  if (im == NULL)
    {
      goto fail1;
    };

  /* Then the data... */
  for (y = 0; (y < sy); y++)
    {
      for (x = 0; (x < sx); x++)
	{
	  int ch;
	  ch = gdGetC (in);
	  if (ch == EOF)
	    {
	      goto fail2;
	    }
	  /* ROW-MAJOR IN GD 1.3 */
	  im->pixels[y][x] = ch;
	}
    }

  return im;

fail2:
  gdImageDestroy (im);
fail1:
  return 0;
}

void
_gdPutColors (gdImagePtr im, gdIOCtx * out)
{
  int i;

  gdPutC ((unsigned char)im->trueColor, out);
  if (!im->trueColor)
    {
      gdPutWord (im->colorsTotal, out);
    }
  gdPutInt (im->transparent, out);
  if (!im->trueColor)
    {
      for (i = 0; (i < gdMaxColors); i++)
	{
	  gdPutC ((unsigned char) im->red[i], out);
	  gdPutC ((unsigned char) im->green[i], out);
	  gdPutC ((unsigned char) im->blue[i], out);
	  gdPutC ((unsigned char) im->alpha[i], out);
	}
    }
}

static
void
_gdPutHeader (gdImagePtr im, gdIOCtx * out)
{
  /* 65535 indicates this is a gd 2.x .gd file. */
  gdPutWord (65535, out);
  gdPutWord (im->sx, out);
  gdPutWord (im->sy, out);

  _gdPutColors (im, out);

}

static void
_gdImageGd (gdImagePtr im, gdIOCtx * out)
{
  int x, y;

  _gdPutHeader (im, out);

  for (y = 0; (y < im->sy); y++)
    {
      for (x = 0; (x < im->sx); x++)
	{
	  /* ROW-MAJOR IN GD 1.3 */
	  if (im->trueColor)
	    {
	      gdPutInt (im->tpixels[y][x], out);
	    }
	  else
	    {
	      gdPutC ((unsigned char) im->pixels[y][x], out);
	    }
	}
    }
}

void
gdImageGd (gdImagePtr im, FILE * outFile)
{
  gdIOCtx *out = gdNewFileCtx (outFile);
  _gdImageGd (im, out);
  out->free (out);
}

void *
gdImageGdPtr (gdImagePtr im, int *size)
{
  void *rv;
  gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
  _gdImageGd (im, out);
  rv = gdDPExtractData (out, size);
  out->free (out);
  return rv;
}
