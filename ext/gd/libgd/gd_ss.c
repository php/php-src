#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"

#define TRUE 1
#define FALSE 0

/* Exported functions: */
extern void gdImagePngToSink (gdImagePtr im, gdSinkPtr out);
extern gdImagePtr gdImageCreateFromPngSource (gdSourcePtr inSource);

/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/*#define GD_SS_DBG(s) (s) */
#define GD_SS_DBG(s)

void
gdImagePngToSink (gdImagePtr im, gdSinkPtr outSink)
{
  gdIOCtx *out = gdNewSSCtx (NULL, outSink);
  gdImagePngCtx (im, out);
  out->free (out);
}

gdImagePtr
gdImageCreateFromPngSource (gdSourcePtr inSource)
{
  gdIOCtx *in = gdNewSSCtx (inSource, NULL);
  gdImagePtr im;

  im = gdImageCreateFromPngCtx (in);

  in->free (in);

  return im;
}
