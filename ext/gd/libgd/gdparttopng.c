#include <stdio.h>
#include <stdlib.h>		/* For atoi */
#include "gd.h"

/* A short program which converts a .png file into a .gd file, for
   your convenience in creating images on the fly from a
   basis image that must be loaded quickly. The .gd format
   is not intended to be a general-purpose format. */

int
main (int argc, char **argv)
{
  gdImagePtr im;
  FILE *in, *out;
  int x, y, w, h;

  if (argc != 7)
    {
      fprintf (stderr, "Usage: gdparttopng filename.gd filename.png x y w h\n");
      exit (1);
    }
  in = fopen (argv[1], "rb");
  if (!in)
    {
      fprintf (stderr, "Input file does not exist!\n");
      exit (1);
    }

  x = atoi (argv[3]);
  y = atoi (argv[4]);
  w = atoi (argv[5]);
  h = atoi (argv[6]);

  printf ("Extracting from (%d, %d), size is %dx%d\n", x, y, w, h);

  im = gdImageCreateFromGd2Part (in, x, y, w, h);
  fclose (in);
  if (!im)
    {
      fprintf (stderr, "Input is not in PNG format!\n");
      exit (1);
    }
  out = fopen (argv[2], "wb");
  if (!out)
    {
      fprintf (stderr, "Output file cannot be written to!\n");
      gdImageDestroy (im);
      exit (1);
    }
  gdImagePng (im, out);
  fclose (out);
  gdImageDestroy (im);

  return 0;
}
