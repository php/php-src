#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"

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


