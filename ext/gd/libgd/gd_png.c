#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"

/* JCE: Arrange HAVE_LIBPNG so that it can be set in gd.h */
#ifdef HAVE_LIBPNG

#include "png.h"		/* includes zlib.h and setjmp.h */
#include "gdhelpers.h"

#define TRUE 1
#define FALSE 0

/*---------------------------------------------------------------------------

    gd_png.c                 Copyright 1999 Greg Roelofs and Thomas Boutell

    The routines in this file, gdImagePng*() and gdImageCreateFromPng*(),
    are drop-in replacements for gdImageGif*() and gdImageCreateFromGif*(),
    except that these functions are noisier in the case of errors (comment
    out all fprintf() statements to disable that).

    GD 2.0 supports RGBA truecolor and will read and write truecolor PNGs.
    GD 2.0 supports 8 bits of color resolution per channel and 
    7 bits of alpha channel resolution. Images with more than 8 bits
    per channel are reduced to 8 bits. Images with an alpha channel are
    only able to resolve down to '1/128th opaque' instead of '1/256th',
    and this conversion is also automatic. I very much doubt you can see it.
    Both tRNS and true alpha are supported.

    Gamma is ignored, and there is no support for text annotations.

    Last updated:  9 February 2001

  ---------------------------------------------------------------------------*/

#ifndef PNG_SETJMP_NOT_SUPPORTED
typedef struct _jmpbuf_wrapper
  {
    jmp_buf jmpbuf;
  }
jmpbuf_wrapper;

static jmpbuf_wrapper gdPngJmpbufStruct;

static void
gdPngErrorHandler (png_structp png_ptr, png_const_charp msg)
{
  jmpbuf_wrapper *jmpbuf_ptr;

  /* This function, aside from the extra step of retrieving the "error
   * pointer" (below) and the fact that it exists within the application
   * rather than within libpng, is essentially identical to libpng's
   * default error handler.  The second point is critical:  since both
   * setjmp() and longjmp() are called from the same code, they are
   * guaranteed to have compatible notions of how big a jmp_buf is,
   * regardless of whether _BSD_SOURCE or anything else has (or has not)
   * been defined. */

  fprintf (stderr, "gd-png:  fatal libpng error: %s\n", msg);
  fflush (stderr);

  jmpbuf_ptr = png_get_error_ptr (png_ptr);
  if (jmpbuf_ptr == NULL)
    {				/* we are completely hosed now */
      fprintf (stderr,
	       "gd-png:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n");
      fflush (stderr);
      exit (99);
    }

  longjmp (jmpbuf_ptr->jmpbuf, 1);
}
#endif

static void
gdPngReadData (png_structp png_ptr,
	       png_bytep data, png_size_t length)
{
  gdGetBuf (data, length, (gdIOCtx *)
	    png_get_io_ptr (png_ptr));
}

static void
gdPngWriteData (png_structp png_ptr,
		png_bytep data, png_size_t length)
{
  gdPutBuf (data, length, (gdIOCtx *)
	    png_get_io_ptr (png_ptr));
}

static void
gdPngFlushData (png_structp png_ptr)
{
}

gdImagePtr
gdImageCreateFromPng (FILE * inFile)
{
  gdImagePtr im;
  gdIOCtx *in = gdNewFileCtx (inFile);
  im = gdImageCreateFromPngCtx (in);
  in->gd_free (in);
  return im;
}


/* This routine is based in part on the Chapter 13 demo code in "PNG: The
 *  Definitive Guide" (http://www.cdrom.com/pub/png/pngbook.html).
 */
gdImagePtr
gdImageCreateFromPngCtx (gdIOCtx * infile)
{
  png_byte sig[8];
  png_structp png_ptr;
  png_infop info_ptr;
  png_uint_32 width, height, rowbytes, w, h;
  int bit_depth, color_type, interlace_type;
  int num_palette, num_trans;
  png_colorp palette;
  png_color_16p trans_gray_rgb;
  png_color_16p trans_color_rgb;
  png_bytep trans;
  png_bytep image_data = NULL;
  png_bytepp row_pointers = NULL;
  gdImagePtr im = NULL;
  int i, j, *open = NULL;
  volatile int transparent = -1;
  volatile int palette_allocated = FALSE;

  /* Make sure the signature can't match by dumb luck -- TBB */
  /* GRR: isn't sizeof(infile) equal to the size of the pointer? */
  memset (infile, 0, sizeof (infile));

  /* first do a quick check that the file really is a PNG image; could
   * have used slightly more general png_sig_cmp() function instead */
  gdGetBuf (sig, 8, infile);
  if (!png_check_sig (sig, 8))
    return NULL;		/* bad signature */

#ifndef PNG_SETJMP_NOT_SUPPORTED
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, &gdPngJmpbufStruct,
				    gdPngErrorHandler, NULL);
#else
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
  if (png_ptr == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate libpng main struct\n");
      return NULL;
    }

  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate libpng info struct\n");
      png_destroy_read_struct (&png_ptr, NULL, NULL);
      return NULL;
    }

  /* we could create a second info struct here (end_info), but it's only
   * useful if we want to keep pre- and post-IDAT chunk info separated
   * (mainly for PNG-aware image editors and converters) */

  /* setjmp() must be called in every non-callback function that calls a
   * PNG-reading libpng function */
#ifndef PNG_SETJMP_NOT_SUPPORTED
  if (setjmp (gdPngJmpbufStruct.jmpbuf))
    {
      fprintf (stderr, "gd-png error: setjmp returns error condition\n");
      png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
      return NULL;
    }
#endif

  png_set_sig_bytes (png_ptr, 8);	/* we already read the 8 signature bytes */

  png_set_read_fn (png_ptr, (void *) infile, gdPngReadData);
  png_read_info (png_ptr, info_ptr);	/* read all PNG info up to image data */

  png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);
  if ((color_type == PNG_COLOR_TYPE_RGB) ||
      (color_type == PNG_COLOR_TYPE_RGB_ALPHA))
    {
      im = gdImageCreateTrueColor ((int) width, (int) height);
    }
  else
    {
      im = gdImageCreate ((int) width, (int) height);
    }
  if (im == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate gdImage struct\n");
      png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
      gdFree (image_data);
      gdFree (row_pointers);
      return NULL;
    }

  if (bit_depth == 16)
    png_set_strip_16 (png_ptr);
  else if (bit_depth < 8)
    png_set_packing (png_ptr);	/* expand to 1 byte per pixel */
  switch (color_type)
    {
    case PNG_COLOR_TYPE_PALETTE:
      png_get_PLTE (png_ptr, info_ptr, &palette, &num_palette);
#ifdef DEBUG
      fprintf (stderr, "gd-png color_type is palette, colors: %d\n",
	       num_palette);
#endif /* DEBUG */
      if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
	{
	  /* gd 2.0: we support this rather thoroughly now. Grab the
	     first fully transparent entry, if any, as the value of 
	     the simple-transparency index, mostly for backwards
	     binary compatibility. The alpha channel is where it's
	     really at these days. */
	  int firstZero = 1;
	  png_get_tRNS (png_ptr, info_ptr, &trans, &num_trans, NULL);
	  for (i = 0; i < num_trans; ++i)
	    {
	      im->alpha[i] = gdAlphaMax - (trans[i] >> 1);
	      if ((trans[i] == 0) && (firstZero))
		{
		  transparent = i;
		  firstZero = 0;
		}
	    }
	}
      break;

    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      /* create a fake palette and check for single-shade transparency */
      if ((palette = (png_colorp) gdMalloc (256 * sizeof (png_color))) == NULL)
	{
	  fprintf (stderr, "gd-png error: cannot allocate gray palette\n");
	  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
	  return NULL;
	}
      palette_allocated = TRUE;
      if (bit_depth < 8)
	{
	  num_palette = 1 << bit_depth;
	  for (i = 0; i < 256; ++i)
	    {
	      j = (255 * i) / (num_palette - 1);
	      palette[i].red = palette[i].green = palette[i].blue = j;
	    }
	}
      else
	{
	  num_palette = 256;
	  for (i = 0; i < 256; ++i)
	    {
	      palette[i].red = palette[i].green = palette[i].blue = i;
	    }
	}
      if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
	{
	  png_get_tRNS (png_ptr, info_ptr, NULL, NULL, &trans_gray_rgb);
	  if (bit_depth == 16)	/* png_set_strip_16() not yet in effect */
	    transparent = trans_gray_rgb->gray >> 8;
	  else
	    transparent = trans_gray_rgb->gray;
	  /* Note slight error in 16-bit case:  up to 256 16-bit shades
	   * may get mapped to a single 8-bit shade, and only one of them
	   * is supposed to be transparent.  IOW, both opaque pixels and
	   * transparent pixels will be mapped into the transparent entry.
	   * There is no particularly good way around this in the case
	   * that all 256 8-bit shades are used, but one could write some
	   * custom 16-bit code to handle the case where there are gdFree
	   * palette entries.  This error will be extremely rare in
	   * general, though.  (Quite possibly there is only one such
	   * image in existence.) */
	}
      break;

    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
      /* gd 2.0: we now support truecolor. See the comment above
         for a rare situation in which the transparent pixel may not
         work properly with 16-bit channels. */
      if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
	{
	  png_get_tRNS (png_ptr, info_ptr, NULL, NULL, &trans_color_rgb);
	  if (bit_depth == 16)	/* png_set_strip_16() not yet in effect */
	    transparent = gdTrueColor (trans_color_rgb->red >> 8,
				       trans_color_rgb->green >> 8,
				       trans_color_rgb->blue >> 8);
	  else
	    transparent = gdTrueColor (trans_color_rgb->red,
				       trans_color_rgb->green,
				       trans_color_rgb->blue);
	}
      break;
    }

  png_read_update_info (png_ptr, info_ptr);

  /* allocate space for the PNG image data */
  rowbytes = png_get_rowbytes (png_ptr, info_ptr);
  if ((image_data = (png_bytep) gdMalloc (rowbytes * height)) == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate image data\n");
      png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
      return NULL;
    }
  if ((row_pointers = (png_bytepp) gdMalloc (height * sizeof (png_bytep))) == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate row pointers\n");
      png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
      gdFree (image_data);
      return NULL;
    }

  /* set the individual row_pointers to point at the correct offsets */
  for (h = 0; h < height; ++h)
    {
      row_pointers[h] = image_data + h * rowbytes;
    }

  png_read_image (png_ptr, row_pointers);	/* read whole image... */
  png_read_end (png_ptr, NULL);	/* ...done! */

  if (!im->trueColor)
    {
      im->colorsTotal = num_palette;
      im->transparent = transparent;
      /* load the palette and mark all entries "open" (unused) for now */
      open = im->open;
      for (i = 0; i < num_palette; ++i)
	{
	  im->red[i] = palette[i].red;
	  im->green[i] = palette[i].green;
	  im->blue[i] = palette[i].blue;
	  open[i] = 1;
	}
      for (i = num_palette; i < gdMaxColors; ++i)
	{
	  open[i] = 1;
	}
    }
  im->interlace = (interlace_type == PNG_INTERLACE_ADAM7);

  /* can't nuke structs until done with palette */
  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
  switch (color_type)
    {
    case PNG_COLOR_TYPE_RGB:
      for (h = 0; h < height; h++)
	{
	  int boffset = 0;
	  for (w = 0; w < width; w++)
	    {
	      register png_byte r = row_pointers[h][boffset++];
	      register png_byte g = row_pointers[h][boffset++];
	      register png_byte b = row_pointers[h][boffset++];
	      im->tpixels[h][w] = gdTrueColor (r, g, b);
	    }
	}
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      for (h = 0; h < height; h++)
	{
	  int boffset = 0;
	  for (w = 0; w < width; w++)
	    {
	      register png_byte r = row_pointers[h][boffset++];
	      register png_byte g = row_pointers[h][boffset++];
	      register png_byte b = row_pointers[h][boffset++];
	      /* gd has only 7 bits of alpha channel resolution, and
	         127 is transparent, 0 opaque. A moment of convenience, 
	         a lifetime of compatibility. */
	      register png_byte a = gdAlphaMax -
	      (row_pointers[h][boffset++] >> 1);
	      im->tpixels[h][w] = gdTrueColorAlpha (r, g, b, a);
	    }
	}
      break;
    default:
      /* Palette image, or something coerced to be one */
      for (h = 0; h < height; ++h)
	{
	  for (w = 0; w < width; ++w)
	    {
	      register png_byte idx = row_pointers[h][w];
	      im->pixels[h][w] = idx;
	      open[idx] = 0;
	    }
	}
    }
#ifdef DEBUG
  if (!im->trueColor)
    {
      for (i = num_palette; i < gdMaxColors; ++i)
	{
	  if (!open[i])
	    {
	      fprintf (stderr, "gd-png warning: image data references out-of-range"
		       " color index (%d)\n", i);
	    }
	}
    }
#endif

  if (palette_allocated)
    gdFree (palette);
  gdFree (image_data);
  gdFree (row_pointers);

  return im;
}


void
gdImagePng (gdImagePtr im, FILE * outFile)
{
  gdIOCtx *out = gdNewFileCtx (outFile);
  gdImagePngCtx (im, out);
  out->gd_free (out);
}

void *
gdImagePngPtr (gdImagePtr im, int *size)
{
  void *rv;
  gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
  gdImagePngCtx (im, out);
  rv = gdDPExtractData (out, size);
  out->gd_free (out);
  return rv;
}

/* This routine is based in part on code from Dale Lutz (Safe Software Inc.)
 *  and in part on demo code from Chapter 15 of "PNG: The Definitive Guide"
 *  (http://www.cdrom.com/pub/png/pngbook.html).
 */
void
gdImagePngCtx (gdImagePtr im, gdIOCtx * outfile)
{
  int i, j, bit_depth = 0, interlace_type;
  int width = im->sx;
  int height = im->sy;
  int colors = im->colorsTotal;
  int *open = im->open;
  int mapping[gdMaxColors];	/* mapping[gd_index] == png_index */
  png_byte trans_values[256];
  png_color_16 trans_rgb_value;
  png_color palette[gdMaxColors];
  png_structp png_ptr;
  png_infop info_ptr;
  volatile int transparent = im->transparent;
  volatile int remap = FALSE;


#ifndef PNG_SETJMP_NOT_SUPPORTED
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING,
			       &gdPngJmpbufStruct, gdPngErrorHandler, NULL);
#else
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
  if (png_ptr == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate libpng main struct\n");
      return;
    }

  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL)
    {
      fprintf (stderr, "gd-png error: cannot allocate libpng info struct\n");
      png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
      return;
    }

#ifndef PNG_SETJMP_NOT_SUPPORTED
  if (setjmp (gdPngJmpbufStruct.jmpbuf))
    {
      fprintf (stderr, "gd-png error: setjmp returns error condition\n");
      png_destroy_write_struct (&png_ptr, &info_ptr);
      return;
    }
#endif

  png_set_write_fn (png_ptr, (void *) outfile, gdPngWriteData, gdPngFlushData);

  /* This is best for palette images, and libpng defaults to it for
     palette images anyway, so we don't need to do it explicitly.
     What to ideally do for truecolor images depends, alas, on the image.
     gd is intentionally imperfect and doesn't spend a lot of time
     fussing with such things. */
/*  png_set_filter(png_ptr, 0, PNG_FILTER_NONE);  */

  /* may want to force maximum compression, but time penalty is large */
/*  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);  */

  /* can set this to a smaller value without compromising compression if all
   * image data is 16K or less; will save some decoder memory [min == 8] */
/*  png_set_compression_window_bits(png_ptr, 15);  */

  if (!im->trueColor)
    {
      if (transparent >= im->colorsTotal ||
	  (transparent >= 0 && open[transparent]))
	transparent = -1;
    }
  if (!im->trueColor)
    {
      for (i = 0; i < gdMaxColors; ++i)
	mapping[i] = -1;
    }
  if (!im->trueColor)
    {
      /* count actual number of colors used (colorsTotal == high-water mark) */
      colors = 0;
      for (i = 0; i < im->colorsTotal; ++i)
	{
	  if (!open[i])
	    {
	      mapping[i] = colors;
	      ++colors;
	    }
	}
      if (colors < im->colorsTotal)
	{
	  remap = TRUE;
	}
      if (colors <= 2)
	bit_depth = 1;
      else if (colors <= 4)
	bit_depth = 2;
      else if (colors <= 16)
	bit_depth = 4;
      else
	bit_depth = 8;
    }
  interlace_type = im->interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  if (im->trueColor)
    {
      if (im->saveAlphaFlag)
	{
	  png_set_IHDR (png_ptr, info_ptr, width, height, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, interlace_type,
		     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	}
      else
	{
	  png_set_IHDR (png_ptr, info_ptr, width, height, 8,
			PNG_COLOR_TYPE_RGB, interlace_type,
		     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	}
    }
  else
    {
      png_set_IHDR (png_ptr, info_ptr, width, height, bit_depth,
		    PNG_COLOR_TYPE_PALETTE, interlace_type,
		    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }
  if (im->trueColor && (!im->saveAlphaFlag) && (transparent >= 0))
    {
      trans_rgb_value.red = gdTrueColorGetRed (im->trueColor);
      trans_rgb_value.green = gdTrueColorGetGreen (im->trueColor);
      trans_rgb_value.blue = gdTrueColorGetBlue (im->trueColor);
      png_set_tRNS (png_ptr, info_ptr, 0, 0, &trans_rgb_value);
    }
  if (!im->trueColor)
    {
      /* Oy veh. Remap the PNG palette to put the 
         entries with interesting alpha channel
         values first. This minimizes the size
         of the tRNS chunk and thus the size
         of the PNG file as a whole. */
      int tc = 0;
      int i;
      int j;
      int k;
      for (i = 0; (i < im->colorsTotal); i++)
	{
	  if ((!im->open[i]) &&
	      (im->alpha[i] != gdAlphaOpaque))
	    {
	      tc++;
	    }
	}
      if (tc)
	{
#if 0
	  for (i = 0; (i < im->colorsTotal); i++)
	    {
	      trans_values[i] = 255 -
		((im->alpha[i] << 1) +
		 (im->alpha[i] >> 6));
	    }
	  png_set_tRNS (png_ptr, info_ptr, trans_values, 256, NULL);
#endif
	  if (!remap)
	    {
	      remap = TRUE;
	    }
	  /* (Semi-)transparent indexes come up from the bottom
	     of the list of real colors; opaque
	     indexes come down from the top */
	  j = 0;
	  k = colors - 1;
	  for (i = 0; (i < im->colorsTotal); i++)
	    {
	      if (!im->open[i])
		{
		  if (im->alpha[i] != gdAlphaOpaque)
		    {
		       /* Andrew Hull: >> 6, not >> 7! (gd 2.0.5) */ 
		      trans_values[j] = 255 -
			((im->alpha[i] << 1) +
			 (im->alpha[i] >> 6));
		      mapping[i] = j++;
		    }
		  else
		    {
		      mapping[i] = k--;
		    }
		}
	    }
	  png_set_tRNS (png_ptr, info_ptr, trans_values, tc, NULL);
	}
    }

  /* convert palette to libpng layout */
  if (!im->trueColor)
    {
      if (remap)
	for (i = 0; i < im->colorsTotal; ++i)
	  {
	    if (mapping[i] < 0)
	      continue;
	    palette[mapping[i]].red = im->red[i];
	    palette[mapping[i]].green = im->green[i];
	    palette[mapping[i]].blue = im->blue[i];
	  }
      else
	for (i = 0; i < colors; ++i)
	  {
	    palette[i].red = im->red[i];
	    palette[i].green = im->green[i];
	    palette[i].blue = im->blue[i];
	  }
      png_set_PLTE (png_ptr, info_ptr, palette, colors);
    }

  /* write out the PNG header info (everything up to first IDAT) */
  png_write_info (png_ptr, info_ptr);

  /* make sure < 8-bit images are packed into pixels as tightly as possible */
  png_set_packing (png_ptr);

  /* This code allocates a set of row buffers and copies the gd image data
   * into them only in the case that remapping is necessary; in gd 1.3 and
   * later, the im->pixels array is laid out identically to libpng's row
   * pointers and can be passed to png_write_image() function directly.
   * The remapping case could be accomplished with less memory for non-
   * interlaced images, but interlacing causes some serious complications. */
  if (im->trueColor)
    {
      int channels = im->saveAlphaFlag ? 4 : 3;
      /* Our little 7-bit alpha channel trick costs us a bit here. */
      png_bytep *row_pointers;
      row_pointers = gdMalloc (sizeof (png_bytep) * height);
      if (row_pointers == NULL)
	{
	  fprintf (stderr, "gd-png error: unable to allocate row_pointers\n");
	}
      for (j = 0; j < height; ++j)
	{
	  int bo = 0;
	  if ((row_pointers[j] = (png_bytep) gdMalloc (width * channels)) == NULL)
	    {
	      fprintf (stderr, "gd-png error: unable to allocate rows\n");
	      for (i = 0; i < j; ++i)
		gdFree (row_pointers[i]);
	      return;
	    }
	  for (i = 0; i < width; ++i)
	    {
	      unsigned char a;
	      row_pointers[j][bo++] = gdTrueColorGetRed (im->tpixels[j][i]);
	      row_pointers[j][bo++] = gdTrueColorGetGreen (im->tpixels[j][i]);
	      row_pointers[j][bo++] = gdTrueColorGetBlue (im->tpixels[j][i]);
	      if (im->saveAlphaFlag)
		{
		  /* convert the 7-bit alpha channel to an 8-bit alpha channel.
		     We do a little bit-flipping magic, repeating the MSB
		     as the LSB, to ensure that 0 maps to 0 and
		     127 maps to 255. We also have to invert to match
		     PNG's convention in which 255 is opaque. */
		  a = gdTrueColorGetAlpha (im->tpixels[j][i]);
		   /* Andrew Hull: >> 6, not >> 7! (gd 2.0.5) */ 
		  row_pointers[j][bo++] = 255 - ((a << 1) + (a >> 6));
		}
	    }
	}

      png_write_image (png_ptr, row_pointers);
      png_write_end (png_ptr, info_ptr);

      for (j = 0; j < height; ++j)
	gdFree (row_pointers[j]);
      gdFree (row_pointers);
    }
  else
    {
      if (remap)
	{
	  png_bytep *row_pointers;
	  row_pointers = gdMalloc (sizeof (png_bytep) * height);
	  if (row_pointers == NULL)
	    {
	      fprintf (stderr, "gd-png error: unable to allocate row_pointers\n");
	    }
	  for (j = 0; j < height; ++j)
	    {
	      if ((row_pointers[j] = (png_bytep) gdMalloc (width)) == NULL)
		{
		  fprintf (stderr, "gd-png error: unable to allocate rows\n");
		  for (i = 0; i < j; ++i)
		    gdFree (row_pointers[i]);
		  return;
		}
	      for (i = 0; i < width; ++i)
		row_pointers[j][i] = mapping[im->pixels[j][i]];
	    }

	  png_write_image (png_ptr, row_pointers);
	  png_write_end (png_ptr, info_ptr);

	  for (j = 0; j < height; ++j)
	    gdFree (row_pointers[j]);
	  gdFree (row_pointers);
	}
      else
	{
	  png_write_image (png_ptr, im->pixels);
	  png_write_end (png_ptr, info_ptr);
	}
    }
  /* 1.6.3: maybe we should give that memory BACK! TBB */
  png_destroy_write_struct (&png_ptr, &info_ptr);
}


#endif /* HAVE_LIBPNG */
