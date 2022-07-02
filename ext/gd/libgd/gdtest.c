#include <stdio.h>
#ifdef _WIN32
#include <process.h>
int
unlink (const char *filename)
{
  return _unlink (filename);
}
#else
#include <unistd.h>		/* for getpid(), unlink() */
#endif
#include "gd.h"

void CompareImages (char *msg, gdImagePtr im1, gdImagePtr im2);

static int freadWrapper (void *context, char *buf, int len);
static int fwriteWrapper (void *context, const char *buffer, int len);

int
main (int argc, char **argv)
{
  gdImagePtr im, ref, im2, im3;
  FILE *in, *out;
  void *iptr;
  int sz;
  gdIOCtxPtr ctx;
  char of[256];
  int colRed, colBlu;
  gdSource imgsrc;
  gdSink imgsnk;
  int foreground;
  int i;
  if (argc != 2)
    {
      fprintf (stderr, "Usage: gdtest filename.png\n");
      exit (1);
    }
  in = fopen (argv[1], "rb");
  if (!in)
    {
      fprintf (stderr, "Input file does not exist!\n");
      exit (1);
    }
  im = gdImageCreateFromPng (in);

  rewind (in);
  ref = gdImageCreateFromPng (in);

  fclose (in);

  printf ("Reference File has %d Palette entries\n", ref->colorsTotal);

  CompareImages ("Initial Versions", ref, im);


  /* */
  /* Send to PNG File then Ptr */
  /* */
  snprintf (of, sizeof(of), "%s.png", argv[1]);
  out = fopen (of, "wb");
  gdImagePng (im, out);
  fclose (out);

  in = fopen (of, "rb");
  if (!in)
    {
      fprintf (stderr, "PNG Output file does not exist!\n");
      exit (1);
    }
  im2 = gdImageCreateFromPng (in);
  fclose (in);

  CompareImages ("GD->PNG File->GD", ref, im2);

  unlink (of);
  gdImageDestroy (im2);

  iptr = gdImagePngPtr (im, &sz);
  ctx = gdNewDynamicCtx (sz, iptr);
  im2 = gdImageCreateFromPngCtx (ctx);

  CompareImages ("GD->PNG ptr->GD", ref, im2);

  gdImageDestroy (im2);
  ctx->gd_free (ctx);


  /* */
  /* Send to GD2 File then Ptr */
  /* */
  snprintf (of, sizeof(of), "%s.gd2", argv[1]);
  out = fopen (of, "wb");
  gdImageGd2 (im, out, 128, 2);
  fclose (out);

  in = fopen (of, "rb");
  if (!in)
    {
      fprintf (stderr, "GD2 Output file does not exist!\n");
      exit (1);
    }
  im2 = gdImageCreateFromGd2 (in);
  fclose (in);

  CompareImages ("GD->GD2 File->GD", ref, im2);

  unlink (of);
  gdImageDestroy (im2);

  iptr = gdImageGd2Ptr (im, 128, 2, &sz);
  /*printf("Got ptr %d (size %d)\n",iptr, sz); */
  ctx = gdNewDynamicCtx (sz, iptr);
  /*printf("Got ctx %d\n",ctx); */
  im2 = gdImageCreateFromGd2Ctx (ctx);
  /*printf("Got img2 %d\n",im2); */

  CompareImages ("GD->GD2 ptr->GD", ref, im2);

  gdImageDestroy (im2);
  ctx->gd_free (ctx);


  /* */
  /* Send to GD File then Ptr */
  /* */
  snprintf (of, sizeof(of), "%s.gd", argv[1]);
  out = fopen (of, "wb");
  gdImageGd (im, out);
  fclose (out);

  in = fopen (of, "rb");
  if (!in)
    {
      fprintf (stderr, "GD Output file does not exist!\n");
      exit (1);
    }
  im2 = gdImageCreateFromGd (in);
  fclose (in);

  CompareImages ("GD->GD File->GD", ref, im2);

  unlink (of);
  gdImageDestroy (im2);

  iptr = gdImageGdPtr (im, &sz);
  /*printf("Got ptr %d (size %d)\n",iptr, sz); */
  ctx = gdNewDynamicCtx (sz, iptr);
  /*printf("Got ctx %d\n",ctx); */
  im2 = gdImageCreateFromGdCtx (ctx);
  /*printf("Got img2 %d\n",im2); */

  CompareImages ("GD->GD ptr->GD", ref, im2);

  gdImageDestroy (im2);
  ctx->gd_free (ctx);

  /*
     ** Test gdImageCreateFromPngSource'
     * */

  in = fopen (argv[1], "rb");

  imgsrc.source = freadWrapper;
  imgsrc.context = in;
  im2 = gdImageCreateFromPngSource (&imgsrc);
  fclose (in);

  if (im2 == NULL)
    {
      printf ("GD Source: ERROR Null returned by gdImageCreateFromPngSource\n");
    }
  else
    {
      CompareImages ("GD Source", ref, im2);
      gdImageDestroy (im2);
    };


  /*
     ** Test gdImagePngToSink'
     * */

  snprintf (of, sizeof(of), "%s.snk", argv[1]);
  out = fopen (of, "wb");
  imgsnk.sink = fwriteWrapper;
  imgsnk.context = out;
  gdImagePngToSink (im, &imgsnk);
  fclose (out);
  in = fopen (of, "rb");
  if (!in)
    {
      fprintf (stderr, "GD Sink: ERROR - GD Sink Output file does not exist!\n");
    }
  else
    {
      im2 = gdImageCreateFromPng (in);
      fclose (in);

      CompareImages ("GD Sink", ref, im2);
      gdImageDestroy (im2);
    };

  unlink (of);

  /* */
  /*  Test Extraction */
  /* */
  in = fopen ("test/gdtest_200_300_150_100.png", "rb");
  if (!in)
    {
      fprintf (stderr, "gdtest_200_300_150_100.png does not exist!\n");
      exit (1);
    }
  im2 = gdImageCreateFromPng (in);
  fclose (in);


  in = fopen ("test/gdtest.gd2", "rb");
  if (!in)
    {
      fprintf (stderr, "gdtest.gd2 does not exist!\n");
      exit (1);
    }
  im3 = gdImageCreateFromGd2Part (in, 200, 300, 150, 100);
  fclose (in);

  CompareImages ("GD2Part (gdtest_200_300_150_100.png, gdtest.gd2(part))", im2, im3);

  gdImageDestroy (im2);
  gdImageDestroy (im3);

  /* */
  /*  Copy Blend */
  /* */
  in = fopen ("test/gdtest.png", "rb");
  if (!in)
    {
      fprintf (stderr, "gdtest.png does not exist!\n");
      exit (1);
    }
  im2 = gdImageCreateFromPng (in);
  fclose (in);

  im3 = gdImageCreate (100, 60);
  colRed = gdImageColorAllocate (im3, 255, 0, 0);
  colBlu = gdImageColorAllocate (im3, 0, 0, 255);
  gdImageFilledRectangle (im3, 0, 0, 49, 30, colRed);
  gdImageFilledRectangle (im3, 50, 30, 99, 59, colBlu);

  gdImageCopyMerge (im2, im3, 150, 200, 10, 10, 90, 50, 50);
  gdImageCopyMerge (im2, im3, 180, 70, 10, 10, 90, 50, 50);

  gdImageCopyMergeGray (im2, im3, 250, 160, 10, 10, 90, 50, 50);
  gdImageCopyMergeGray (im2, im3, 80, 70, 10, 10, 90, 50, 50);

  gdImageDestroy (im3);

  in = fopen ("test/gdtest_merge.png", "rb");
  if (!in)
    {
      fprintf (stderr, "gdtest_merge.png does not exist!\n");
      exit (1);
    }
  im3 = gdImageCreateFromPng (in);
  fclose (in);

  printf ("[Merged Image has %d colours]\n", im2->colorsTotal);
  CompareImages ("Merged (gdtest.png, gdtest_merge.png)", im2, im3);

  gdImageDestroy (im2);
  gdImageDestroy (im3);

#ifdef HAVE_JPEG
  out = fopen ("test/gdtest.jpg", "wb");
  if (!out)
    {
      fprintf (stderr, "Can't create file test/gdtest.jpg.\n");
      exit (1);
    }
  gdImageJpeg (im, out, -1);
  fclose (out);
  in = fopen ("test/gdtest.jpg", "rb");
  if (!in)
    {
      fprintf (stderr, "Can't open file test/gdtest.jpg.\n");
      exit (1);
    }
  im2 = gdImageCreateFromJpeg (in);
  fclose (in);
  if (!im2)
    {
      fprintf (stderr, "gdImageCreateFromJpeg failed.\n");
      exit (1);
    }
  gdImageDestroy (im2);
  printf ("Created test/gdtest.jpg successfully. Compare this image\n"
	  "to the input image manually. Some difference must be\n"
	  "expected as JPEG is a lossy file format.\n");
#endif /* HAVE_JPEG */
  /* Assume the color closest to black is the foreground
     color for the B&W wbmp image. */
  fprintf (stderr, "NOTE: the WBMP output image will NOT match the original unless the original\n"
	   "is also black and white. This is OK!\n");
  foreground = gdImageColorClosest (im, 0, 0, 0);
  fprintf (stderr, "Foreground index is %d\n", foreground);
  if (foreground == -1)
    {
      fprintf (stderr, "Source image has no colors, skipping wbmp test.\n");
    }
  else
    {
      out = fopen ("test/gdtest.wbmp", "wb");
      if (!out)
	{
	  fprintf (stderr, "Can't create file test/gdtest.wbmp.\n");
	  exit (1);
	}
      gdImageWBMP (im, foreground, out);
      fclose (out);
      in = fopen ("test/gdtest.wbmp", "rb");
      if (!in)
	{
	  fprintf (stderr, "Can't open file test/gdtest.wbmp.\n");
	  exit (1);
	}
      im2 = gdImageCreateFromWBMP (in);
      fprintf (stderr, "WBMP has %d colors\n", gdImageColorsTotal (im2));
      fprintf (stderr, "WBMP colors are:\n");
      for (i = 0; (i < gdImageColorsTotal (im2)); i++)
	{
	  fprintf (stderr, "%02X%02X%02X\n",
		   gdImageRed (im2, i),
		   gdImageGreen (im2, i),
		   gdImageBlue (im2, i));
	}
      fclose (in);
      if (!im2)
	{
	  fprintf (stderr, "gdImageCreateFromWBMP failed.\n");
	  exit (1);
	}
      CompareImages ("WBMP test (gdtest.png, gdtest.wbmp)", ref, im2);
      out = fopen ("test/gdtest_wbmp_to_png.png", "wb");
      if (!out)
	{
	  fprintf (stderr, "Can't create file test/gdtest_wbmp_to_png.png.\n");
	  exit (1);
	}
      gdImagePng (im2, out);
      fclose (out);
      gdImageDestroy (im2);
    }
  gdImageDestroy (im);
  gdImageDestroy (ref);

  return 0;
}

void
CompareImages (char *msg, gdImagePtr im1, gdImagePtr im2)
{
  int cmpRes;

  cmpRes = gdImageCompare (im1, im2);

  if (cmpRes & GD_CMP_IMAGE)
    {
      printf ("%%%s: ERROR images differ: BAD\n", msg);
    }
  else if (cmpRes != 0)
    {
      printf ("%%%s: WARNING images differ: WARNING - Probably OK\n", msg);
    }
  else
    {
      printf ("%%%s: OK\n", msg);
      return;
    }

  if (cmpRes & (GD_CMP_SIZE_X + GD_CMP_SIZE_Y))
    {
      printf ("-%s: INFO image sizes differ\n", msg);
    }

  if (cmpRes & GD_CMP_NUM_COLORS)
    {
      printf ("-%s: INFO number of palette entries differ %d Vs. %d\n", msg,
	      im1->colorsTotal, im2->colorsTotal);
    }

  if (cmpRes & GD_CMP_COLOR)
    {
      printf ("-%s: INFO actual colours of pixels differ\n", msg);
    }
}


static int
freadWrapper (void *context, char *buf, int len)
{
  int got = fread (buf, 1, len, (FILE *) context);
  return got;
}

static int
fwriteWrapper (void *context, const char *buffer, int len)
{
  return fwrite (buffer, 1, len, (FILE *) context);
}
