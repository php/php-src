#include <stdio.h>
#include "gd.h"
#include "gdfontg.h"
#include "gdfonts.h"

int
main (void)
{
  /* Input and output files */
  FILE *in;
  FILE *out;

  /* Input and output images */
  gdImagePtr im_in = 0, im_out = 0;

  /* Brush image */
  gdImagePtr brush;

  /* Color indexes */
  int white;
  int blue;
  int red;
  int green;

  /* Points for polygon */
  gdPoint points[3];

  /* Create output image, 256 by 256 pixels, true color. */
  im_out = gdImageCreateTrueColor (256, 256);
  /* First color allocated is background. */
  white = gdImageColorAllocate (im_out, 255, 255, 255);

  /* Set transparent color. */
  gdImageColorTransparent (im_out, white);

  /* Try to load demoin.png and paste part of it into the
     output image. */
  in = fopen ("demoin.png", "rb");
  if (!in)
    {
      fprintf (stderr, "Can't load source image; this demo\n");
      fprintf (stderr, "is much more impressive if demoin.png\n");
      fprintf (stderr, "is available.\n");
      im_in = 0;
    }
  else
    {
      im_in = gdImageCreateFromPng (in);
      fclose (in);
      /* Now copy, and magnify as we do so */
      gdImageCopyResized (im_out, im_in,
			  32, 32, 0, 0, 192, 192, 255, 255);
    }
  red = gdImageColorAllocate (im_out, 255, 0, 0);
  green = gdImageColorAllocate (im_out, 0, 255, 0);
  blue = gdImageColorAllocate (im_out, 0, 0, 255);
  /* Rectangle */
  gdImageLine (im_out, 16, 16, 240, 16, green);
  gdImageLine (im_out, 240, 16, 240, 240, green);
  gdImageLine (im_out, 240, 240, 16, 240, green);
  gdImageLine (im_out, 16, 240, 16, 16, green);
  /* Circle */
  gdImageArc (im_out, 128, 128, 60, 20, 0, 720, blue);
  /* Arc */
  gdImageArc (im_out, 128, 128, 40, 40, 90, 270, blue);
  /* Flood fill: doesn't do much on a continuously
     variable tone jpeg original. */
  gdImageFill (im_out, 8, 8, blue);
  /* Polygon */
  points[0].x = 64;
  points[0].y = 0;
  points[1].x = 0;
  points[1].y = 128;
  points[2].x = 128;
  points[2].y = 128;
  gdImageFilledPolygon (im_out, points, 3, green);
  /* Brush. A fairly wild example also involving a line style! */
  if (im_in)
    {
      int style[8];
      brush = gdImageCreateTrueColor (16, 16);
      gdImageCopyResized (brush, im_in,
			  0, 0, 0, 0,
			  gdImageSX (brush), gdImageSY (brush),
			  gdImageSX (im_in), gdImageSY (im_in));
      gdImageSetBrush (im_out, brush);
      /* With a style, so they won't overprint each other.
         Normally, they would, yielding a fat-brush effect. */
      style[0] = 0;
      style[1] = 0;
      style[2] = 0;
      style[3] = 0;
      style[4] = 0;
      style[5] = 0;
      style[6] = 0;
      style[7] = 1;
      gdImageSetStyle (im_out, style, 8);
      /* Draw the styled, brushed line */
      gdImageLine (im_out, 0, 255, 255, 0, gdStyledBrushed);
    }
  /* Text */
  gdImageString (im_out, gdFontGiant, 32, 32,
		 (unsigned char *) "hi", red);
  gdImageStringUp (im_out, gdFontSmall, 64, 64,
		   (unsigned char *) "hi", red);
  /* Make output image interlaced (progressive, in the case of JPEG) */
  gdImageInterlace (im_out, 1);
  out = fopen ("demoout.png", "wb");
  /* Write PNG */
  gdImagePng (im_out, out);
  fclose (out);
  gdImageDestroy (im_out);
  if (im_in)
    {
      gdImageDestroy (im_in);
    }
  return 0;
}
