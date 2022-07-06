
#include "gd.h"
#include <string.h>

#define PI 3.141592
#define DEG2RAD(x) ((x)*PI/180.)

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define MAX4(x,y,z,w) \
	((MAX((x),(y))) > (MAX((z),(w))) ? (MAX((x),(y))) : (MAX((z),(w))))
#define MIN4(x,y,z,w) \
	((MIN((x),(y))) < (MIN((z),(w))) ? (MIN((x),(y))) : (MIN((z),(w))))

#define MAXX(x) MAX4(x[0],x[2],x[4],x[6])
#define MINX(x) MIN4(x[0],x[2],x[4],x[6])
#define MAXY(x) MAX4(x[1],x[3],x[5],x[7])
#define MINY(x) MIN4(x[1],x[3],x[5],x[7])

int
main (int argc, char *argv[])
{
#ifndef HAVE_LIBFREETYPE
  fprintf (stderr, "gd was not compiled with HAVE_LIBFREETYPE defined.\n");
  fprintf (stderr, "Install the FreeType library, including the\n");
  fprintf (stderr, "header files. Then edit the gd Makefile, type\n");
  fprintf (stderr, "make clean, and type make again.\n");
  return 1;
#else
  gdImagePtr im;
  int black;
  int white;
  int brect[8];
  int x, y;
  char *err;
  FILE *out;
#ifdef JISX0208
  char *s = "Hello. ‚±‚ñ‚É‚¿‚Í Qyjpqg,";	/* String to draw. */
#else
  char *s = "Hello. Qyjpqg,";	/* String to draw. */
#endif

  double sz = 40.;

#if 0
  double angle = 0.;
#else
  double angle = DEG2RAD (-90);
#endif

#ifdef JISX0208
  char *f = "/usr/openwin/lib/locale/ja/X11/fonts/TT/HG-MinchoL.ttf";	/* UNICODE */
  /* char *f = "/usr/local/lib/fonts/truetype/DynaFont/dfpop1.ttf"; *//* SJIS */
#else
  char *f = "times";		/* TrueType font */
#endif

  /* obtain brect so that we can size the image */
  err = gdImageStringFT ((gdImagePtr) NULL, &brect[0], 0, f, sz, angle, 0, 0, s);
  if (err)
    {
      fprintf (stderr, "%s", err);
      return 1;
    }

  /* create an image just big enough for the string */
  x = MAXX (brect) - MINX (brect) + 6;
  y = MAXY (brect) - MINY (brect) + 6;
#if 0
  im = gdImageCreate (500, 500);
#else
  /* gd 2.0: true color images can use freetype too */
  im = gdImageCreateTrueColor (x, y);
#endif

  /* Background color. gd 2.0: fill the image with it; truecolor
     images have a black background otherwise. */
  white = gdImageColorResolve (im, 255, 255, 255);
  gdImageFilledRectangle (im, 0, 0, x, y, white);
  black = gdImageColorResolve (im, 0, 0, 0);

  /* render the string, offset origin to center string */
  x = 0 - MINX (brect) + 3;
  y = 0 - MINY (brect) + 3;

  err = gdImageStringFT (im, NULL, black, f, sz, angle, x, y, s);
  if (err)
    {
      fprintf (stderr, "%s", err);
      return 1;
    }
  /* TBB: Write img to test/fttest.png */
  out = fopen ("test/fttest.png", "wb");
  if (!out)
    {
      fprintf (stderr, "Can't create test/fttest.png\n");
      exit (1);
    }
  gdImagePng (im, out);
  fclose (out);
  fprintf (stderr, "Test image written to test/fttest.png\n");
  /* Destroy it */
  gdImageDestroy (im);

  return 0;
#endif /* HAVE_FREETYPE */
}
