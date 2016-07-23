#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

/* Code drawn from ppmtogif.c, from the pbmplus package
**
** Based on GIFENCOD by David Rowley <mgardi@watdscu.waterloo.edu>. A
** Lempel-Zim compression based on "compress".
**
** Modified by Marcel Wijkstra <wijkstra@fwi.uva.nl>
**
** Copyright (C) 1989 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
**
** The Graphics Interchange Format(c) is the Copyright property of
** CompuServe Incorporated.  GIF(sm) is a Service Mark property of
** CompuServe Incorporated.
*/

/*
 * a code_int must be able to hold 2**GIFBITS values of type int, and also -1
 */
typedef int             code_int;

#ifdef SIGNED_COMPARE_SLOW
typedef unsigned long int count_int;
typedef unsigned short int count_short;
#else /*SIGNED_COMPARE_SLOW*/
typedef long int          count_int;
#endif /*SIGNED_COMPARE_SLOW*/

/* 2.0.28: threadsafe */

#define maxbits GIFBITS

/* should NEVER generate this code */
#define maxmaxcode ((code_int)1 << GIFBITS)

#define HSIZE  5003            /* 80% occupancy */
#define hsize HSIZE            /* Apparently invariant, left over from
					compress */

typedef struct {
	int Width, Height;
	int curx, cury;
	long CountDown;
	int Pass;
	int Interlace;
        int n_bits;                        /* number of bits/code */
        code_int maxcode;                  /* maximum code, given n_bits */
        count_int htab [HSIZE];
        unsigned short codetab [HSIZE];
	code_int free_ent;                  /* first unused entry */
	/*
	 * block compression parameters -- after all codes are used up,
	 * and compression rate changes, start over.
	 */
	int clear_flg;
	int offset;
	long int in_count;            /* length of input */
	long int out_count;           /* # of codes output (for debugging) */

	int g_init_bits;
	gdIOCtx * g_outfile;

	int ClearCode;
	int EOFCode;
	unsigned long cur_accum;
	int cur_bits;
        /*
         * Number of characters so far in this 'packet'
         */
        int a_count;
        /*
         * Define the storage for the packet accumulator
         */
        char accum[ 256 ];
} GifCtx;

static int gifPutWord(int w, gdIOCtx *out);
static int colorstobpp(int colors);
static void BumpPixel (GifCtx *ctx);
static int GIFNextPixel (gdImagePtr im, GifCtx *ctx);
static void GIFEncode (gdIOCtxPtr fp, int GWidth, int GHeight, int GInterlace, int Background, int Transparent, int BitsPerPixel, int *Red, int *Green, int *Blue, gdImagePtr im);
static void GIFAnimEncode(gdIOCtxPtr fp, int IWidth, int IHeight, int LeftOfs, int TopOfs, int GInterlace, int Transparent, int Delay, int Disposal, int BitsPerPixel, int *Red, int *Green, int *Blue, gdImagePtr im);
static void compress (int init_bits, gdIOCtx *outfile, gdImagePtr im, GifCtx *ctx);
static void output (code_int code, GifCtx *ctx);
static void cl_block (GifCtx *ctx);
static void cl_hash (register count_int chsize, GifCtx *ctx);
static void char_init (GifCtx *ctx);
static void char_out (int c, GifCtx *ctx);
static void flush_char (GifCtx *ctx);
void * gdImageGifPtr (gdImagePtr im, int *size)
{
  void *rv;
  gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
  gdImageGifCtx (im, out);
  rv = gdDPExtractData (out, size);
  out->gd_free (out);
  return rv;
}

void gdImageGif (gdImagePtr im, FILE * outFile)
{
  gdIOCtx *out = gdNewFileCtx (outFile);
  gdImageGifCtx (im, out);
  out->gd_free (out);
}

void gdImageGifCtx(gdImagePtr im, gdIOCtxPtr out)
{
	gdImagePtr pim = 0, tim = im;
	int interlace, BitsPerPixel;
	interlace = im->interlace;
	if (im->trueColor) {
		/* Expensive, but the only way that produces an
			acceptable result: mix down to a palette
			based temporary image. */
		pim = gdImageCreatePaletteFromTrueColor(im, 1, 256);
		if (!pim) {
			return;
		}
		tim = pim;
	}
	BitsPerPixel = colorstobpp(tim->colorsTotal);
	/* All set, let's do it. */
	GIFEncode(
		out, tim->sx, tim->sy, tim->interlace, 0, tim->transparent, BitsPerPixel,
		tim->red, tim->green, tim->blue, tim);
	if (pim) {
		/* Destroy palette based temporary image. */
		gdImageDestroy(	pim);
	}
}

/*
  Function: gdImageGifAnimBeginPtr

    Like <gdImageGifAnimBegin> except that it outputs to a memory
    buffer.  See <gdImageGifAnimBegin>.

    The returned memory must be freed by the caller when it is no
    longer needed. **The caller must invoke <gdFree>(), not free()**,
    unless the caller is absolutely certain that the same
    implementations of malloc, free, etc. are used both at library
    build time and at application build time (but don't; it could
    always change).

    The 'size' parameter receives the total size of the block of
    memory.

  Parameters:

    im          - The reference image
    size        - Output: the size in bytes of the result.
    GlobalCM    - Global colormap flag: 1 -> yes, 0 -> no, -1 -> do default
    Loops       - Loop count; 0 -> infinite, -1 means no loop

  Returns:

   A pointer to the resulting data (the contents of the start of the
   GIF) or NULL if an error occurred.

*/

void * gdImageGifAnimBeginPtr(gdImagePtr im, int *size, int GlobalCM, int Loops)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) return NULL;
	gdImageGifAnimBeginCtx(im, out, GlobalCM, Loops);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}


/*
  Function: gdImageGifAnimBegin

    This function must be called as the first function when creating a
    GIF animation. It writes the correct GIF file headers to selected
    file output, and prepares for frames to be added for the
    animation. The image argument is not used to produce an image
    frame to the file, it is only used to establish the GIF animation
    frame size, interlacing options and the color
    palette. <gdImageGifAnimAdd> is used to add the first and
    subsequent frames to the animation, and the animation must be
    terminated by writing a semicolon character (;) to it or by using
    gdImageGifAnimEnd to do that.

    The GlobalCM flag indicates if a global color map (or palette) is
    used in the GIF89A header. A nonzero value specifies that a global
    color map should be used to reduce the size of the animation. Of
    course, if the color maps of individual frames differ greatly, a
    global color map may not be a good idea. GlobalCM=1 means write
    global color map, GlobalCM=0 means do not, and GlobalCM=-1 means
    to do the default, which currently is to use a global color map.

    If Loops is 0 or greater, the Netscape 2.0 extension for animation
    loop count is written. 0 means infinite loop count. -1 means that
    the extension is not added which results in no looping. -1 is the
    default.

  Variants:

    <gdImageGifAnimBeginCtx> outputs the image via a <gdIOCtx> struct.

    <gdImageGifAnimBeginPtr> stores the image in a large array of bytes.

  Parameters:

    im          - The reference image
    outfile     - The output FILE*.
    GlobalCM    - Global colormap flag: 1 -> yes, 0 -> no, -1 -> do default
    Loops       - Loop count; 0 -> infinite, -1 means no loop

  Returns:

    Nothing.

  Example:

    See <gdImageGifAnimBegin>.

*/

void gdImageGifAnimBegin(gdImagePtr im, FILE *outFile, int GlobalCM, int Loops)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageGifAnimBeginCtx(im, out, GlobalCM, Loops);
	out->gd_free(out);
}



/*
  Function: gdImageGifAnimBeginCtx

    Like <gdImageGifAnimBegin> except that it outputs to <gdIOCtx>.
    See <gdImageGifAnimBegin>.

  Parameters:

    im          - The reference image
    out         - Pointer to the output <gdIOCtx>.
    GlobalCM    - Global colormap flag: 1 -> yes, 0 -> no, -1 -> do default
    Loops       - Loop count; 0 -> infinite, -1 means no loop

  Returns:

    Nothing.

*/
void gdImageGifAnimBeginCtx(gdImagePtr im, gdIOCtxPtr out, int GlobalCM, int Loops)
{
	int B;
	int RWidth, RHeight;
	int Resolution;
	int ColorMapSize;
	int BitsPerPixel;
	int Background = 0;
	int i;

	/* Default is to use global color map */
	if (GlobalCM < 0) {
		GlobalCM = 1;
	}

	BitsPerPixel = colorstobpp(im->colorsTotal);
	ColorMapSize = 1 << BitsPerPixel;

	RWidth = im->sx;
	RHeight = im->sy;

	Resolution = BitsPerPixel;

	/* Write the Magic header */
	gdPutBuf("GIF89a", 6, out);

	/* Write out the screen width and height */
	gifPutWord(RWidth, out);
	gifPutWord(RHeight, out);

	/* Indicate that there is a global colour map */
	B = GlobalCM ? 0x80 : 0;

	/* OR in the resolution */
	B |= (Resolution - 1) << 4;

	/* OR in the Bits per Pixel */
	B |= (BitsPerPixel - 1);

	/* Write it out */
	gdPutC(B, out);

	/* Write out the Background colour */
	gdPutC(Background, out);

	/* Byte of 0's (future expansion) */
	gdPutC(0, out);

	/* Write out the Global Colour Map */
	if(GlobalCM) {
		for(i = 0; i < ColorMapSize; ++i) {
			gdPutC(im->red[i], out);
			gdPutC(im->green[i], out);
			gdPutC(im->blue[i], out);
		}
	}

	if(Loops >= 0) {
		gdPutBuf("!\377\13NETSCAPE2.0\3\1", 16, out);
		gifPutWord(Loops, out);
		gdPutC(0, out);
	}
}



/*
  Function: gdImageGifAnimAddPtr

    Like <gdImageGifAnimAdd> (which contains more information) except
    that it stores the data to write into memory and returns a pointer
    to it.

    This memory must be freed by the caller when it is no longer
    needed. **The caller must invoke <gdFree>(), not free(),** unless
    the caller is absolutely certain that the same implementations of
    malloc, free, etc. are used both at library build time and at
    application build time (but don't; it could always change).

    The 'size' parameter receives the total size of the block of
    memory.

  Parameters:

    im          - The image to add.
    size        - Output: the size of the resulting buffer.
    LocalCM     - Flag.  If 1, use a local color map for this frame.
    LeftOfs     - Left offset of image in frame.
    TopOfs      - Top offset of image in frame.
    Delay       - Delay before next frame (in 1/100 seconds)
    Disposal    - MODE: How to treat this frame when the next one loads.
    previm      - NULL or a pointer to the previous image written.

  Returns:

    Pointer to the resulting data or NULL if an error occurred.

*/
void * gdImageGifAnimAddPtr(gdImagePtr im, int *size, int LocalCM,
                                         int LeftOfs, int TopOfs, int Delay,
                                         int Disposal, gdImagePtr previm)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) return NULL;
	gdImageGifAnimAddCtx(im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}


/*
  Function: gdImageGifAnimAdd

    This function writes GIF animation frames to GIF animation, which
    was initialized with <gdImageGifAnimBegin>. With _LeftOfs_ and
    _TopOfs_ you can place this frame in different offset than (0,0)
    inside the image screen as defined in <gdImageGifAnimBegin>. Delay
    between the previous frame and this frame is in 1/100s
    units. _Disposal_ is usually <gdDisposalNone>, meaning that the
    pixels changed by this frame should remain on the display when the
    next frame begins to render, but can also be <gdDisposalUnknown>
    (not recommended), <gdDisposalRestoreBackground> (restores the
    first allocated color of the global palette), or
    <gdDisposalRestorePrevious> (restores the appearance of the
    affected area before the frame was rendered). Only
    <gdDisposalNone> is a sensible choice for the first frame. If
    _previm_ is passed, the built-in GIF optimizer will always use
    <gdDisposalNone> regardless of the Disposal parameter.

    Setting the _LocalCM_ flag to 1 adds a local palette for this
    image to the animation. Otherwise the global palette is assumed
    and the user must make sure the palettes match. Use
    <gdImagePaletteCopy> to do that.

    Automatic optimization is activated by giving the previous image
    as a parameter. This function then compares the images and only
    writes the changed pixels to the new frame in animation. The
    _Disposal_ parameter for optimized animations must be set to 1,
    also for the first frame. _LeftOfs_ and _TopOfs_ parameters are
    ignored for optimized frames. To achieve good optimization, it is
    usually best to use a single global color map. To allow
    <gdImageGifAnimAdd> to compress unchanged pixels via the use of a
    transparent color, the image must include a transparent color.


  Variants:

    <gdImageGifAnimAddCtx> outputs its data via a <gdIOCtx> struct.

    <gdImageGifAnimAddPtr> outputs its data to a memory buffer which
    it returns.

  Parameters:

    im          - The image to add.
    outfile     - The output FILE* being written.
    LocalCM     - Flag.  If 1, use a local color map for this frame.
    LeftOfs     - Left offset of image in frame.
    TopOfs      - Top offset of image in frame.
    Delay       - Delay before next frame (in 1/100 seconds)
    Disposal    - MODE: How to treat this frame when the next one loads.
    previm      - NULL or a pointer to the previous image written.

  Returns:

    Nothing.

  Example:

    > {
    > gdImagePtr im, im2, im3;
    > int black, white, trans;
    > FILE *out;
    >
    > im = gdImageCreate(100, 100);     // Create the image
    > white = gdImageColorAllocate(im, 255, 255, 255); // Allocate background
    > black = gdImageColorAllocate(im, 0, 0, 0); // Allocate drawing color
    > trans = gdImageColorAllocate(im, 1, 1, 1); // trans clr for compression
    > gdImageRectangle(im, 0, 0, 10, 10, black); // Draw rectangle
    >
    > out = fopen("anim.gif", "wb");// Open output file in binary mode
    > gdImageGifAnimBegin(im, out, 1, 3);// Write GIF hdr, global clr map,loops
    > // Write the first frame.  No local color map.  Delay = 1s
    > gdImageGifAnimAdd(im, out, 0, 0, 0, 100, 1, NULL);
    >
    > // construct the second frame
    > im2 = gdImageCreate(100, 100);
    > (void)gdImageColorAllocate(im2, 255, 255, 255); // White background
    > gdImagePaletteCopy (im2, im);  // Make sure the palette is identical
    > gdImageRectangle(im2, 0, 0, 15, 15, black);    // Draw something
    > // Allow animation compression with transparent pixels
    > gdImageColorTransparent (im2, trans);
    > gdImageGifAnimAdd(im2, out, 0, 0, 0, 100, 1, im);  // Add second frame
    >
    > // construct the third frame
    > im3 = gdImageCreate(100, 100);
    > (void)gdImageColorAllocate(im3, 255, 255, 255); // white background
    > gdImagePaletteCopy (im3, im); // Make sure the palette is identical
    > gdImageRectangle(im3, 0, 0, 15, 20, black); // Draw something
    > // Allow animation compression with transparent pixels
    > gdImageColorTransparent (im3, trans);
    > // Add the third frame, compressing against the second one
    > gdImageGifAnimAdd(im3, out, 0, 0, 0, 100, 1, im2);
    > gdImageGifAnimEnd(out);  // End marker, same as putc(';', out);
    > fclose(out); // Close file
    >
    > // Destroy images
    > gdImageDestroy(im);
    > gdImageDestroy(im2);
    > gdImageDestroy(im3);
    > }

*/

void gdImageGifAnimAdd(gdImagePtr im, FILE *outFile, int LocalCM,
                                    int LeftOfs, int TopOfs, int Delay,
                                    int Disposal, gdImagePtr previm)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageGifAnimAddCtx(im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm);
	out->gd_free(out);
}

static int comparewithmap(gdImagePtr im1, gdImagePtr im2, int c1, int c2, int *colorMap)
{
	if(!colorMap) {
		return c1 == c2;
	}

	if(-2 != colorMap[c1]) {
		return colorMap[c1] == c2;
	}

	return (colorMap[c1] = gdImageColorExactAlpha(im2, im1->red[c1], im1->green[c1], im1->blue[c1], im1->alpha[c1])) == c2;
}

/*
  Function: gdImageGifAnimAddCtx

    Adds an animation frame via a <gdIOCtxPtr>.  See gdImageGifAnimAdd>.

  Parameters:

    im          - The image to add.
    out         - The output <gdIOCtxPtr>.
    LocalCM     - Flag.  If 1, use a local color map for this frame.
    LeftOfs     - Left offset of image in frame.
    TopOfs      - Top offset of image in frame.
    Delay       - Delay before next frame (in 1/100 seconds)
    Disposal    - MODE: How to treat this frame when the next one loads.
    previm      - NULL or a pointer to the previous image written.

  Returns:

    Nothing.

*/
void gdImageGifAnimAddCtx(gdImagePtr im, gdIOCtxPtr out,
                                       int LocalCM, int LeftOfs, int TopOfs,
                                       int Delay, int Disposal,
                                       gdImagePtr previm)
{
	gdImagePtr pim = NULL, tim = im;
	int interlace, transparent, BitsPerPixel;
	interlace = im->interlace;
	transparent = im->transparent;

	/* Default is no local color map */
	if(LocalCM < 0) {
		LocalCM = 0;
	}

	if(im->trueColor) {
		/* Expensive, but the only way that produces an
			acceptable result: mix down to a palette
			based temporary image. */
		pim = gdImageCreatePaletteFromTrueColor(im, 1, 256);
		if (!pim) {
			return;
		}
		tim = pim;
	}

	if (previm) {
		/* create optimized animation.  Compare this image to
		   the previous image and crop the temporary copy of
		   current image to include only changed rectangular
		   area.  Also replace unchanged pixels inside this
		   area with transparent color.  Transparent color
		   needs to be already allocated!
		   Preconditions:
		   TopOfs, LeftOfs are assumed 0

		   Images should be of same size.  If not, a temporary
		   copy is made with the same size as previous image.

		*/
		gdImagePtr prev_pim = 0, prev_tim = previm;
		int x, y;
		int min_x = 0;
		int min_y = tim->sy;
		int max_x = 0;
		int max_y = 0;
		int colorMap[256];

		if (previm->trueColor) {
			prev_pim = gdImageCreatePaletteFromTrueColor(previm, 1, 256);
			if (!prev_pim) {
				goto fail_end;
			}
			prev_tim = prev_pim;
		}

		for (x = 0; x < 256; ++x) {
			colorMap[x] = -2;
		}

		/* First find bounding box of changed areas. */
		/* first find the top changed row */
		for (y = 0; y < tim->sy; ++y) {
			for (x = 0; x < tim->sx; ++x) {
				if (!comparewithmap(prev_tim, tim,
				                    prev_tim->pixels[y][x],
				                    tim->pixels[y][x],
				                    colorMap)) {
					min_y = max_y = y;
					min_x = max_x = x;
					goto break_top;
				}
			}
		}

break_top:
		if (tim->sy == min_y) {
			/* No changes in this frame!! Encode empty image. */
			transparent = 0;
			min_x = min_y = 1;
			max_x = max_y = 0;
		} else {
			/* Then the bottom row */
			for (y = tim->sy - 1; y > min_y; --y) {
				for (x = 0; x < tim->sx; ++x) {
					if (!comparewithmap
					        (prev_tim, tim,
					         prev_tim->pixels[y][x],
					         tim->pixels[y][x],
					         colorMap)) {
						max_y = y;
						if(x < min_x) {
							min_x = x;
						}
						if(x > max_x) {
							max_x = x;
						}
						goto break_bot;
					}
				}
			}

break_bot:
			/* left side */
			for (x = 0; x < min_x; ++x) {
				for (y = min_y; y <= max_y; ++y) {
					if (!comparewithmap
					        (prev_tim, tim,
					         prev_tim->pixels[y][x],
					         tim->pixels[y][x],
					         colorMap)) {
						min_x = x;
						goto break_left;
					}
				}
			}

break_left:
			/* right side */
			for (x = tim->sx - 1; x > max_x; --x) {
				for (y = min_y; y <= max_y; ++y) {
					if (!comparewithmap
					        (prev_tim, tim,
					         prev_tim->pixels[y][x],
					         tim->pixels[y][x],
					         colorMap)) {
						max_x = x;
						goto break_right;
					}
				}
			}

break_right:
			;
		}

		LeftOfs = min_x;
		TopOfs = min_y;
		Disposal = 1;

		/* Make a copy of the image with the new offsets.
		   But only if necessary. */
		if (min_x != 0 || max_x != tim->sx - 1
		        || min_y != 0 || max_y != tim->sy - 1
		        || transparent >= 0) {

			gdImagePtr pim2 = gdImageCreate(max_x-min_x + 1, max_y-min_y + 1);

			if (!pim2) {
				if (prev_pim) {
					gdImageDestroy(prev_pim);
				}
				goto fail_end;
			}

			gdImagePaletteCopy(pim2, LocalCM ? tim : prev_tim);
			gdImageCopy(pim2, tim, 0, 0, min_x, min_y,
			            max_x - min_x + 1, max_y - min_y + 1);

			if (pim) {
				gdImageDestroy(pim);
			}

			tim = pim = pim2;
		}

		/* now let's compare pixels for transparent
		   optimization.  But only if transparent is set. */
		if (transparent >= 0) {
			for(y = 0; y < tim->sy; ++y) {
				for (x = 0; x < tim->sx; ++x) {
					if(comparewithmap
					        (prev_tim, tim,
					         prev_tim->pixels[min_y + y][min_x + x],
					         tim->pixels[y][x], 0)) {
						gdImageSetPixel(tim, x, y, transparent);
						break;
					}
				}
			}
		}

		if(prev_pim) {
			gdImageDestroy(prev_pim);
		}
	}

	BitsPerPixel = colorstobpp(tim->colorsTotal);

	/* All set, let's do it. */
	GIFAnimEncode(
	    out, tim->sx, tim->sy, LeftOfs, TopOfs, interlace, transparent,
	    Delay, Disposal, BitsPerPixel,
	    LocalCM ? tim->red : 0, tim->green, tim->blue, tim);

fail_end:
	if(pim) {
		/* Destroy palette based temporary image. */
		gdImageDestroy(pim);
	}
}



/*
  Function: gdImageGifAnimEnd

    Terminates the GIF file properly.

    (Previous versions of this function's documentation suggested just
    manually writing a semicolon (';') instead since that is all this
    function does.  While that has no longer changed, we now suggest
    that you do not do this and instead always call
    <gdImageGifAnimEnd> (or equivalent) since later versions could
    possibly do more or different things.)

  Variants:

    <gdImageGifAnimEndCtx> outputs its data via a <gdIOCtx> struct.

    <gdImageGifAnimEndPtr> outputs its data to a memory buffer which
    it returns.

  Parameters:

    outfile     - the destination FILE*.

  Returns:

    Nothing.

*/

void gdImageGifAnimEnd(FILE *outFile)
{
#if 1
	putc(';', outFile);
#else
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageGifAnimEndCtx(out);
	out->gd_free(out);
#endif
}

/*
  Function: gdImageGifAnimEndPtr

    Like <gdImageGifAnimEnd> (which contains more information) except
    that it stores the data to write into memory and returns a pointer
    to it.

    This memory must be freed by the caller when it is no longer
    needed. **The caller must invoke <gdFree>(), not free(),** unless
    the caller is absolutely certain that the same implementations of
    malloc, free, etc. are used both at library build time and at
    application build time (but don't; it could always change).

    The 'size' parameter receives the total size of the block of
    memory.

  Parameters:

    size        - Output: the size of the resulting buffer.

  Returns:

    Pointer to the resulting data or NULL if an error occurred.

*/

void * gdImageGifAnimEndPtr(int *size)
{
	char *rv = (char *) gdMalloc(1);
	if(!rv) {
		return 0;
	}
	*rv = ';';
	*size = 1;
	return (void *)rv;
}

/*
  Function: gdImageGifAnimEndCtx

    Like <gdImageGifAnimEnd>, but writes its data via a <gdIOCtx>.

  Parameters:

    out         - the destination <gdIOCtx>.

  Returns:

    Nothing.

*/

void gdImageGifAnimEndCtx(gdIOCtx *out)
{
	/*
	 * Write the GIF file terminator
	 */
	gdPutC(';', out);
}

static int
colorstobpp(int colors)
{
    int bpp = 0;

    if ( colors <= 2 )
        bpp = 1;
    else if ( colors <= 4 )
        bpp = 2;
    else if ( colors <= 8 )
        bpp = 3;
    else if ( colors <= 16 )
        bpp = 4;
    else if ( colors <= 32 )
        bpp = 5;
    else if ( colors <= 64 )
        bpp = 6;
    else if ( colors <= 128 )
        bpp = 7;
    else if ( colors <= 256 )
        bpp = 8;
    return bpp;
    }

/*****************************************************************************
 *
 * GIFENCODE.C    - GIF Image compression interface
 *
 * GIFEncode( FName, GHeight, GWidth, GInterlace, Background, Transparent,
 *            BitsPerPixel, Red, Green, Blue, gdImagePtr )
 *
 *****************************************************************************/

#define TRUE 1
#define FALSE 0
/*
 * Bump the 'curx' and 'cury' to point to the next pixel
 */
static void
BumpPixel(GifCtx *ctx)
{
        /*
         * Bump the current X position
         */
        ++(ctx->curx);

        /*
         * If we are at the end of a scan line, set curx back to the beginning
         * If we are interlaced, bump the cury to the appropriate spot,
         * otherwise, just increment it.
         */
        if( ctx->curx == ctx->Width ) {
                ctx->curx = 0;

                if( !ctx->Interlace )
                        ++(ctx->cury);
                else {
                     switch( ctx->Pass ) {

                       case 0:
                          ctx->cury += 8;
                          if( ctx->cury >= ctx->Height ) {
                                ++(ctx->Pass);
                                ctx->cury = 4;
                          }
                          break;

                       case 1:
                          ctx->cury += 8;
                          if( ctx->cury >= ctx->Height ) {
                                ++(ctx->Pass);
                                ctx->cury = 2;
                          }
                          break;

                       case 2:
                          ctx->cury += 4;
                          if( ctx->cury >= ctx->Height ) {
                             ++(ctx->Pass);
                             ctx->cury = 1;
                          }
                          break;

                       case 3:
                          ctx->cury += 2;
                          break;
                        }
                }
        }
}

/*
 * Return the next pixel from the image
 */
static int
GIFNextPixel(gdImagePtr im, GifCtx *ctx)
{
        int r;

        if( ctx->CountDown == 0 )
                return EOF;

        --(ctx->CountDown);

        r = gdImageGetPixel(im, ctx->curx, ctx->cury);

        BumpPixel(ctx);

        return r;
}

/* public */

static void
GIFEncode(gdIOCtxPtr fp, int GWidth, int GHeight, int GInterlace, int Background, int Transparent, int BitsPerPixel, int *Red, int *Green, int *Blue, gdImagePtr im)
{
        int B;
        int RWidth, RHeight;
        int LeftOfs, TopOfs;
        int Resolution;
        int ColorMapSize;
        int InitCodeSize;
        int i;
		GifCtx ctx;

		memset(&ctx, 0, sizeof(ctx));
        ctx.Interlace = GInterlace;
		ctx.in_count = 1;

        ColorMapSize = 1 << BitsPerPixel;

        RWidth = ctx.Width = GWidth;
        RHeight = ctx.Height = GHeight;
        LeftOfs = TopOfs = 0;

        Resolution = BitsPerPixel;

        /*
         * Calculate number of bits we are expecting
         */
        ctx.CountDown = (long)ctx.Width * (long)ctx.Height;

        /*
         * Indicate which pass we are on (if interlace)
         */
        ctx.Pass = 0;

        /*
         * The initial code size
         */
        if( BitsPerPixel <= 1 )
                InitCodeSize = 2;
        else
                InitCodeSize = BitsPerPixel;

        /*
         * Set up the current x and y position
         */
        ctx.curx = ctx.cury = 0;

        /*
         * Write the Magic header
         */
        gdPutBuf(Transparent < 0 ? "GIF87a" : "GIF89a", 6, fp );

        /*
         * Write out the screen width and height
         */
        gifPutWord( RWidth, fp );
        gifPutWord( RHeight, fp );

        /*
         * Indicate that there is a global colour map
         */
        B = 0x80;       /* Yes, there is a color map */

        /*
         * OR in the resolution
         */
        B |= (Resolution - 1) << 5;

        /*
         * OR in the Bits per Pixel
         */
        B |= (BitsPerPixel - 1);

        /*
         * Write it out
         */
        gdPutC( B, fp );

        /*
         * Write out the Background colour
         */
        gdPutC( Background, fp );

        /*
         * Byte of 0's (future expansion)
         */
        gdPutC( 0, fp );

        /*
         * Write out the Global Colour Map
         */
        for( i=0; i<ColorMapSize; ++i ) {
                gdPutC( Red[i], fp );
                gdPutC( Green[i], fp );
                gdPutC( Blue[i], fp );
        }

	/*
	 * Write out extension for transparent colour index, if necessary.
	 */
	if ( Transparent >= 0 ) {
	    gdPutC( '!', fp );
	    gdPutC( 0xf9, fp );
	    gdPutC( 4, fp );
	    gdPutC( 1, fp );
	    gdPutC( 0, fp );
	    gdPutC( 0, fp );
	    gdPutC( (unsigned char) Transparent, fp );
	    gdPutC( 0, fp );
	}

        /*
         * Write an Image separator
         */
        gdPutC( ',', fp );

        /*
         * Write the Image header
         */

        gifPutWord( LeftOfs, fp );
        gifPutWord( TopOfs, fp );
        gifPutWord( ctx.Width, fp );
        gifPutWord( ctx.Height, fp );

        /*
         * Write out whether or not the image is interlaced
         */
        if( ctx.Interlace )
                gdPutC( 0x40, fp );
        else
                gdPutC( 0x00, fp );

        /*
         * Write out the initial code size
         */
        gdPutC( InitCodeSize, fp );

        /*
         * Go and actually compress the data
         */
        compress( InitCodeSize+1, fp, im, &ctx );

        /*
         * Write out a Zero-length packet (to end the series)
         */
        gdPutC( 0, fp );

        /*
         * Write the GIF file terminator
         */
        gdPutC( ';', fp );
}

static void GIFAnimEncode(gdIOCtxPtr fp, int IWidth, int IHeight, int LeftOfs, int TopOfs, int GInterlace, int Transparent, int Delay, int Disposal, int BitsPerPixel, int *Red, int *Green, int *Blue, gdImagePtr im)
{
	int B;
	int ColorMapSize;
	int InitCodeSize;
	int i;
	GifCtx ctx;

	memset(&ctx, 0, sizeof(ctx));

	ctx.Interlace = GInterlace;
	ctx.in_count = 1;

	ColorMapSize = 1 << BitsPerPixel;

	if(LeftOfs < 0) {
		LeftOfs = 0;
	}
	if(TopOfs < 0) {
		TopOfs = 0;
	}
	if(Delay < 0) {
		Delay = 100;
	}
	if(Disposal < 0) {
		Disposal = 1;
	}

	ctx.Width = IWidth;
	ctx.Height = IHeight;

	/* Calculate number of bits we are expecting */
	ctx.CountDown = (long)ctx.Width * (long)ctx.Height;

	/* Indicate which pass we are on (if interlace) */
	ctx.Pass = 0;

	/* The initial code size */
	if(BitsPerPixel <= 1) {
		InitCodeSize = 2;
	} else {
		InitCodeSize = BitsPerPixel;
	}

	/* Set up the current x and y position */
	ctx.curx = ctx.cury = 0;

	/* Write out extension for image animation and looping */
	gdPutC('!', fp);
	gdPutC(0xf9, fp);
	gdPutC(4, fp);
	gdPutC((Transparent >= 0 ? 1 : 0) | (Disposal << 2), fp);
	gdPutC((unsigned char)(Delay & 255), fp);
	gdPutC((unsigned char)((Delay >> 8) & 255), fp);
	gdPutC((unsigned char) Transparent, fp);
	gdPutC(0, fp);

	/* Write an Image separator */
	gdPutC(',', fp);

	/* Write out the Image header */
	gifPutWord(LeftOfs, fp);
	gifPutWord(TopOfs, fp);
	gifPutWord(ctx.Width, fp);
	gifPutWord(ctx.Height, fp);

	/* Indicate that there is a local colour map */
	B = (Red && Green && Blue) ? 0x80 : 0;

	/* OR in the interlacing */
	B |= ctx.Interlace ? 0x40 : 0;

	/* OR in the Bits per Pixel */
	B |= (Red && Green && Blue) ? (BitsPerPixel - 1) : 0;

	/* Write it out */
	gdPutC(B, fp);

	/* Write out the Local Colour Map */
	if(Red && Green && Blue) {
		for(i = 0; i < ColorMapSize; ++i) {
			gdPutC(Red[i], fp);
			gdPutC(Green[i], fp);
			gdPutC(Blue[i], fp);
		}
	}

	/* Write out the initial code size */
	gdPutC(InitCodeSize, fp);

	/* Go and actually compress the data */
	compress(InitCodeSize + 1, fp, im, &ctx);

	/* Write out a Zero-length packet (to end the series) */
	gdPutC(0, fp);
}

/***************************************************************************
 *
 *  GIFCOMPR.C       - GIF Image compression routines
 *
 *  Lempel-Ziv compression based on 'compress'.  GIF modifications by
 *  David Rowley (mgardi@watdcsu.waterloo.edu)
 *
 ***************************************************************************/

/*
 * General DEFINEs
 */

#define GIFBITS    12

#ifdef NO_UCHAR
 typedef char   char_type;
#else /*NO_UCHAR*/
 typedef        unsigned char   char_type;
#endif /*NO_UCHAR*/

/*
 *
 * GIF Image compression - modified 'compress'
 *
 * Based on: compress.c - File compression ala IEEE Computer, June 1984.
 *
 * By Authors:  Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *              Jim McKie               (decvax!mcvax!jim)
 *              Steve Davies            (decvax!vax135!petsd!peora!srd)
 *              Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *              James A. Woods          (decvax!ihnp4!ames!jaw)
 *              Joe Orost               (decvax!vax135!petsd!joe)
 *
 */
#include <ctype.h>

#define ARGVAL() (*++(*argv) || (--argc && *++argv))

#ifdef COMPATIBLE               /* But wrong! */
# define MAXCODE(n_bits)        ((code_int) 1 << (n_bits) - 1)
#else /*COMPATIBLE*/
# define MAXCODE(n_bits)        (((code_int) 1 << (n_bits)) - 1)
#endif /*COMPATIBLE*/

#define HashTabOf(i)       ctx->htab[i]
#define CodeTabOf(i)    ctx->codetab[i]


/*
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**GIFBITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */

#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i)        ((char_type*)(htab))[i]
#define de_stack               ((char_type*)&tab_suffixof((code_int)1<<GIFBITS))

/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */

static void
output(code_int code, GifCtx *ctx);

static void
compress(int init_bits, gdIOCtxPtr outfile, gdImagePtr im, GifCtx *ctx)
{
    register long fcode;
    register code_int i /* = 0 */;
    register int c;
    register code_int ent;
    register code_int disp;
    register code_int hsize_reg;
    register int hshift;

    /*
     * Set up the globals:  g_init_bits - initial number of bits
     *                      g_outfile   - pointer to output file
     */
    ctx->g_init_bits = init_bits;
    ctx->g_outfile = outfile;

    /*
     * Set up the necessary values
     */
    ctx->offset = 0;
    ctx->out_count = 0;
    ctx->clear_flg = 0;
    ctx->in_count = 1;
    ctx->maxcode = MAXCODE(ctx->n_bits = ctx->g_init_bits);

    ctx->ClearCode = (1 << (init_bits - 1));
    ctx->EOFCode = ctx->ClearCode + 1;
    ctx->free_ent = ctx->ClearCode + 2;

    char_init(ctx);

    ent = GIFNextPixel( im, ctx );

    hshift = 0;
    for ( fcode = (long) hsize;  fcode < 65536L; fcode *= 2L )
        ++hshift;
    hshift = 8 - hshift;                /* set hash code range bound */

    hsize_reg = hsize;
    cl_hash( (count_int) hsize_reg, ctx );            /* clear hash table */

    output( (code_int)ctx->ClearCode, ctx );

#ifdef SIGNED_COMPARE_SLOW
    while ( (c = GIFNextPixel( im )) != (unsigned) EOF ) {
#else /*SIGNED_COMPARE_SLOW*/
    while ( (c = GIFNextPixel( im, ctx )) != EOF ) {  /* } */
#endif /*SIGNED_COMPARE_SLOW*/

        ++(ctx->in_count);

        fcode = (long) (((long) c << maxbits) + ent);
        i = (((code_int)c << hshift) ^ ent);    /* xor hashing */

        if ( HashTabOf (i) == fcode ) {
            ent = CodeTabOf (i);
            continue;
        } else if ( (long)HashTabOf (i) < 0 )      /* empty slot */
            goto nomatch;
        disp = hsize_reg - i;           /* secondary hash (after G. Knott) */
        if ( i == 0 )
            disp = 1;
probe:
        if ( (i -= disp) < 0 )
            i += hsize_reg;

        if ( HashTabOf (i) == fcode ) {
            ent = CodeTabOf (i);
            continue;
        }
        if ( (long)HashTabOf (i) > 0 )
            goto probe;
nomatch:
        output ( (code_int) ent, ctx );
        ++(ctx->out_count);
        ent = c;
#ifdef SIGNED_COMPARE_SLOW
        if ( (unsigned) ctx->free_ent < (unsigned) maxmaxcode) {
#else /*SIGNED_COMPARE_SLOW*/
        if ( ctx->free_ent < maxmaxcode ) {  /* } */
#endif /*SIGNED_COMPARE_SLOW*/
            CodeTabOf (i) = ctx->free_ent++; /* code -> hashtable */
            HashTabOf (i) = fcode;
        } else
                cl_block(ctx);
    }
    /*
     * Put out the final code.
     */
    output( (code_int)ent, ctx );
    ++(ctx->out_count);
    output( (code_int) ctx->EOFCode, ctx );
}

/*****************************************************************
 * TAG( output )
 *
 * Output the given code.
 * Inputs:
 *      code:   A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *              that n_bits =< (long)wordsize - 1.
 * Outputs:
 *      Outputs code to the file.
 * Assumptions:
 *      Chars are 8 bits long.
 * Algorithm:
 *      Maintain a GIFBITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */

static const unsigned long masks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
                                  0x001F, 0x003F, 0x007F, 0x00FF,
                                  0x01FF, 0x03FF, 0x07FF, 0x0FFF,
                                  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };


/* Arbitrary value to mark output is done.  When we see EOFCode, then we don't
 * expect to see any more data.  If we do (e.g. corrupt image inputs), cur_bits
 * might be negative, so flag it to return early.
 */
#define CUR_BITS_FINISHED -1000


static void
output(code_int code, GifCtx *ctx)
{
	if (ctx->cur_bits == CUR_BITS_FINISHED) {
		return;
	}

    ctx->cur_accum &= masks[ ctx->cur_bits ];

    if( ctx->cur_bits > 0 )
        ctx->cur_accum |= ((long)code << ctx->cur_bits);
    else
        ctx->cur_accum = code;

    ctx->cur_bits += ctx->n_bits;

    while( ctx->cur_bits >= 8 ) {
        char_out( (unsigned int)(ctx->cur_accum & 0xff), ctx );
        ctx->cur_accum >>= 8;
        ctx->cur_bits -= 8;
    }

    /*
     * If the next entry is going to be too big for the code size,
     * then increase it, if possible.
     */
   if ( ctx->free_ent > ctx->maxcode || ctx->clear_flg ) {

            if( ctx->clear_flg ) {

                ctx->maxcode = MAXCODE (ctx->n_bits = ctx->g_init_bits);
                ctx->clear_flg = 0;

            } else {

                ++(ctx->n_bits);
                if ( ctx->n_bits == maxbits )
                    ctx->maxcode = maxmaxcode;
                else
                    ctx->maxcode = MAXCODE(ctx->n_bits);
            }
        }

    if( code == ctx->EOFCode ) {
        /*
         * At EOF, write the rest of the buffer.
         */
        while( ctx->cur_bits > 0 ) {
                char_out( (unsigned int)(ctx->cur_accum & 0xff), ctx);
                ctx->cur_accum >>= 8;
                ctx->cur_bits -= 8;
        }

		/* Flag that it's done to prevent re-entry. */
		ctx->cur_bits = CUR_BITS_FINISHED;

        flush_char(ctx);
    }
}

/*
 * Clear out the hash table
 */
static void
cl_block (GifCtx *ctx)             /* table clear for block compress */
{

        cl_hash ( (count_int) hsize, ctx );
        ctx->free_ent = ctx->ClearCode + 2;
        ctx->clear_flg = 1;

        output( (code_int)ctx->ClearCode, ctx);
}

static void
cl_hash(register count_int chsize, GifCtx *ctx)          /* reset code table */

{

        register count_int *htab_p = ctx->htab+chsize;

        register long i;
        register long m1 = -1;

        i = chsize - 16;
        do {                            /* might use Sys V memset(3) here */
                *(htab_p-16) = m1;
                *(htab_p-15) = m1;
                *(htab_p-14) = m1;
                *(htab_p-13) = m1;
                *(htab_p-12) = m1;
                *(htab_p-11) = m1;
                *(htab_p-10) = m1;
                *(htab_p-9) = m1;
                *(htab_p-8) = m1;
                *(htab_p-7) = m1;
                *(htab_p-6) = m1;
                *(htab_p-5) = m1;
                *(htab_p-4) = m1;
                *(htab_p-3) = m1;
                *(htab_p-2) = m1;
                *(htab_p-1) = m1;
                htab_p -= 16;
        } while ((i -= 16) >= 0);

        for ( i += 16; i > 0; --i )
                *--htab_p = m1;
}

/******************************************************************************
 *
 * GIF Specific routines
 *
 ******************************************************************************/

/*
 * Set up the 'byte output' routine
 */
static void
char_init(GifCtx *ctx)
{
        ctx->a_count = 0;
}

/*
 * Add a character to the end of the current packet, and if it is 254
 * characters, flush the packet to disk.
 */
static void
char_out(int c, GifCtx *ctx)
{
        ctx->accum[ ctx->a_count++ ] = c;
        if( ctx->a_count >= 254 )
                flush_char(ctx);
}

/*
 * Flush the packet to disk, and reset the accumulator
 */
static void
flush_char(GifCtx *ctx)
{
        if( ctx->a_count > 0 ) {
                gdPutC( ctx->a_count, ctx->g_outfile );
                gdPutBuf( ctx->accum, ctx->a_count, ctx->g_outfile );
                ctx->a_count = 0;
        }
}

static int gifPutWord(int w, gdIOCtx *out)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	return 0;
}


