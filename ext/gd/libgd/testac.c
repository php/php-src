
#include <stdio.h>
#include "gd.h"

/* If palette is true, we convert from truecolor to palette at the end,
   to test gdImageTrueColorToPalette and see file size/
   quality tradeoffs. */

void testDrawing (
		   gdImagePtr im_in,
		   double scale,
		   int blending,
		   int palette,
		   char *filename);

int
main (int argc, char *argv[])
{
  /* Input and output files */
  FILE *in;
  FILE *out;

  /* Input image */
  gdImagePtr im_in = 0;

  /* Colors */
  int lightBlue;

  if (argc != 2)
    {
      fprintf (stderr, "Usage: testac filename.png\n");
      exit (1);
    }
  /* Load original PNG, which should contain alpha channel
     information. We will use it in two ways: preserving it
     literally, for use with compatible browsers, and
     compositing it ourselves against a background of our
     choosing (alpha blending). We'll change its size
     and try creating palette versions of it. */
  in = fopen (argv[1], "rb");
  if (!in)
    {
      fprintf (stderr, "Can't load %s.\n", argv[1]);
      exit (1);
    }
  else
    {
      im_in = gdImageCreateFromPng (in);
      fclose (in);
    }
  testDrawing (im_in, 1.0, 0, 0, "noblending-fullsize-truecolor.png");
  testDrawing (im_in, 1.0, 1, 0, "blending-fullsize-truecolor.png");
  testDrawing (im_in, 0.5, 0, 0, "noblending-halfsize-truecolor.png");
  testDrawing (im_in, 0.5, 1, 0, "blending-halfsize-truecolor.png");
  testDrawing (im_in, 2.0, 0, 0, "noblending-doublesize-truecolor.png");
  testDrawing (im_in, 2.0, 1, 0, "blending-doublesize-truecolor.png");
  testDrawing (im_in, 1.0, 0, 1, "noblending-fullsize-palette.png");
  testDrawing (im_in, 1.0, 1, 1, "blending-fullsize-palette.png");
  testDrawing (im_in, 0.5, 0, 1, "noblending-halfsize-palette.png");
  testDrawing (im_in, 0.5, 1, 1, "blending-halfsize-palette.png");
  testDrawing (im_in, 2.0, 0, 1, "noblending-doublesize-palette.png");
  testDrawing (im_in, 2.0, 1, 1, "blending-doublesize-palette.png");
  gdImageDestroy (im_in);
  return 0;
}

/* If palette is true, we convert from truecolor to palette at the end,
   to test gdImageTrueColorToPalette and see file size/
   quality tradeoffs. */
void
testDrawing (
	      gdImagePtr im_in,
	      double scale,
	      int blending,
	      int palette,
	      char *filename)
{
  gdImagePtr im_out;
  FILE *out;
  /* Create output image. */
  im_out = gdImageCreateTrueColor ((int) (gdImageSX (im_in) * scale),
				   (int) (gdImageSY (im_in) * scale));
  /*
     Request alpha blending. This causes future
     drawing operations to perform alpha channel blending
     with the background, resulting in an opaque image.
     Without this call, pixels in the foreground color are
     copied literally, *including* the alpha channel value,
     resulting in an output image which is potentially
     not opaque. This flag can be set and cleared as often
     as desired. */
  gdImageAlphaBlending (im_out, blending);

  /* Flood with light blue. */
  gdImageFill (im_out, (int) (gdImageSX (im_in) * scale / 2),
	       (int) (gdImageSY (im_in) * scale / 2),
	       gdTrueColor (192, 192, 255));
  /* Copy the source image. Alpha blending should result in
     compositing against red. With blending turned off, the
     browser or viewer will composite against its preferred
     background, or, if it does not support an alpha channel,
     we will see the original colors for the pixels that
     ought to be transparent or semitransparent. */
  gdImageCopyResampled (im_out, im_in,
			0, 0,
			0, 0,
       (int) (gdImageSX (im_in) * scale), (int) (gdImageSY (im_in) * scale),
			gdImageSX (im_in), gdImageSY (im_in));
  /* Write PNG */
  out = fopen (filename, "wb");

  /* If this image is the result of alpha channel blending,
     it will not contain an interesting alpha channel itself.
     Save a little file size by not saving the alpha channel.
     Otherwise the file would typically be slightly larger. */
  gdImageSaveAlpha (im_out, !blending);

  /* If requested, convert from truecolor to palette. */
  if (palette)
    {
      /* Dithering, 256 colors. */
      gdImageTrueColorToPalette (im_out, 1, 256);
    }

  gdImagePng (im_out, out);
  fclose (out);

  gdImageDestroy (im_out);
}
