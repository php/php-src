

/*
 * gd_topal.c 
 * 
 * This code is adapted pretty much entirely from jquant2.c,
 * Copyright (C) 1991-1996, Thomas G. Lane. That file is
 * part of the Independent JPEG Group's software. Conditions of
 * use are compatible with the gd license. See the gd license 
 * statement and README-JPEG.TXT for additional information.
 *
 * This file contains 2-pass color quantization (color mapping) routines.
 * These routines provide selection of a custom color map for an image,
 * followed by mapping of the image to that color map, with optional
 * Floyd-Steinberg dithering.
 *
 * It is also possible to use just the second pass to map to an arbitrary
 * externally-given color map.
 *
 * Note: ordered dithering is not supported, since there isn't any fast
 * way to compute intercolor distances; it's unclear that ordered dither's
 * fundamental assumptions even hold with an irregularly spaced color map.
 *
 * SUPPORT FOR ALPHA CHANNELS WAS HACKED IN BY THOMAS BOUTELL, who also
 * adapted the code to work within gd rather than within libjpeg, and
 * may not have done a great job of either. It's not Thomas G. Lane's fault.
 */

#include "gd.h"
#include "gdhelpers.h"
#include <string.h>
#include <stdlib.h>

/*
 * This module implements the well-known Heckbert paradigm for color
 * quantization.  Most of the ideas used here can be traced back to
 * Heckbert's seminal paper
 *   Heckbert, Paul.  "Color Image Quantization for Frame Buffer Display",
 *   Proc. SIGGRAPH '82, Computer Graphics v.16 #3 (July 1982), pp 297-304.
 *
 * In the first pass over the image, we accumulate a histogram showing the
 * usage count of each possible color.  To keep the histogram to a reasonable
 * size, we reduce the precision of the input; typical practice is to retain
 * 5 or 6 bits per color, so that 8 or 4 different input values are counted
 * in the same histogram cell.
 *
 * Next, the color-selection step begins with a box representing the whole
 * color space, and repeatedly splits the "largest" remaining box until we
 * have as many boxes as desired colors.  Then the mean color in each
 * remaining box becomes one of the possible output colors.
 * 
 * The second pass over the image maps each input pixel to the closest output
 * color (optionally after applying a Floyd-Steinberg dithering correction).
 * This mapping is logically trivial, but making it go fast enough requires
 * considerable care.
 *
 * Heckbert-style quantizers vary a good deal in their policies for choosing
 * the "largest" box and deciding where to cut it.  The particular policies
 * used here have proved out well in experimental comparisons, but better ones
 * may yet be found.
 *
 * In earlier versions of the IJG code, this module quantized in YCbCr color
 * space, processing the raw upsampled data without a color conversion step.
 * This allowed the color conversion math to be done only once per colormap
 * entry, not once per pixel.  However, that optimization precluded other
 * useful optimizations (such as merging color conversion with upsampling)
 * and it also interfered with desired capabilities such as quantizing to an
 * externally-supplied colormap.  We have therefore abandoned that approach.
 * The present code works in the post-conversion color space, typically RGB.
 *
 * To improve the visual quality of the results, we actually work in scaled
 * RGBA space, giving G distances more weight than R, and R in turn more than
 * B.  Alpha is weighted least. To do everything in integer math, we must 
 * use integer scale factors. The 2/3/1 scale factors used here correspond 
 * loosely to the relative weights of the colors in the NTSC grayscale 
 * equation. 
 */

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

#define R_SCALE 2		/* scale R distances by this much */
#define G_SCALE 3		/* scale G distances by this much */
#define B_SCALE 1		/* and B by this much */
#define A_SCALE 4		/* and alpha by this much. This really
				   only scales by 1 because alpha
				   values are 7-bit to begin with. */

/* Channel ordering (fixed in gd) */
#define C0_SCALE R_SCALE
#define C1_SCALE G_SCALE
#define C2_SCALE B_SCALE
#define C3_SCALE A_SCALE

/*
 * First we have the histogram data structure and routines for creating it.
 *
 * The number of bits of precision can be adjusted by changing these symbols.
 * We recommend keeping 6 bits for G and 5 each for R and B.
 * If you have plenty of memory and cycles, 6 bits all around gives marginally
 * better results; if you are short of memory, 5 bits all around will save
 * some space but degrade the results.
 * To maintain a fully accurate histogram, we'd need to allocate a "long"
 * (preferably unsigned long) for each cell.  In practice this is overkill;
 * we can get by with 16 bits per cell.  Few of the cell counts will overflow,
 * and clamping those that do overflow to the maximum value will give close-
 * enough results.  This reduces the recommended histogram size from 256Kb
 * to 128Kb, which is a useful savings on PC-class machines.
 * (In the second pass the histogram space is re-used for pixel mapping data;
 * in that capacity, each cell must be able to store zero to the number of
 * desired colors.  16 bits/cell is plenty for that too.)
 * Since the JPEG code is intended to run in small memory model on 80x86
 * machines, we can't just allocate the histogram in one chunk.  Instead
 * of a true 3-D array, we use a row of pointers to 2-D arrays.  Each
 * pointer corresponds to a C0 value (typically 2^5 = 32 pointers) and
 * each 2-D array has 2^6*2^5 = 2048 or 2^6*2^6 = 4096 entries.  Note that
 * on 80x86 machines, the pointer row is in near memory but the actual
 * arrays are in far memory (same arrangement as we use for image arrays).
 */

#define MAXNUMCOLORS  (gdMaxColors)	/* maximum size of colormap */

#define HIST_C0_BITS  5		/* bits of precision in R histogram */
#define HIST_C1_BITS  6		/* bits of precision in G histogram */
#define HIST_C2_BITS  5		/* bits of precision in B histogram */
#define HIST_C3_BITS  3		/* bits of precision in A histogram */

/* Number of elements along histogram axes. */
#define HIST_C0_ELEMS  (1<<HIST_C0_BITS)
#define HIST_C1_ELEMS  (1<<HIST_C1_BITS)
#define HIST_C2_ELEMS  (1<<HIST_C2_BITS)
#define HIST_C3_ELEMS  (1<<HIST_C3_BITS)

/* These are the amounts to shift an input value to get a histogram index. */
#define C0_SHIFT  (8-HIST_C0_BITS)
#define C1_SHIFT  (8-HIST_C1_BITS)
#define C2_SHIFT  (8-HIST_C2_BITS)
/* Beware! Alpha is 7 bit to begin with */
#define C3_SHIFT  (7-HIST_C3_BITS)


typedef unsigned short histcell;	/* histogram cell; prefer an unsigned type */

typedef histcell *histptr;	/* for pointers to histogram cells */

typedef histcell hist1d[HIST_C3_ELEMS];		/* typedefs for the array */
typedef hist1d *hist2d;		/* type for the 2nd-level pointers */
typedef hist2d *hist3d;		/* type for third-level pointer */
typedef hist3d *hist4d;		/* type for top-level pointer */


/* Declarations for Floyd-Steinberg dithering.

 * Errors are accumulated into the array fserrors[], at a resolution of
 * 1/16th of a pixel count.  The error at a given pixel is propagated
 * to its not-yet-processed neighbors using the standard F-S fractions,
 *              ...     (here)  7/16
 *              3/16    5/16    1/16
 * We work left-to-right on even rows, right-to-left on odd rows.
 *
 * We can get away with a single array (holding one row's worth of errors)
 * by using it to store the current row's errors at pixel columns not yet
 * processed, but the next row's errors at columns already processed.  We
 * need only a few extra variables to hold the errors immediately around the
 * current column.  (If we are lucky, those variables are in registers, but
 * even if not, they're probably cheaper to access than array elements are.)
 *
 * The fserrors[] array has (#columns + 2) entries; the extra entry at
 * each end saves us from special-casing the first and last pixels.
 * Each entry is three values long, one value for each color component.
 *
 */

typedef signed short FSERROR;	/* 16 bits should be enough */
typedef int LOCFSERROR;		/* use 'int' for calculation temps */

typedef FSERROR *FSERRPTR;	/* pointer to error array */

/* Private object */

typedef struct
  {
    hist4d histogram;		/* pointer to the histogram */
    int needs_zeroed;		/* TRUE if next pass must zero histogram */

    /* Variables for Floyd-Steinberg dithering */
    FSERRPTR fserrors;		/* accumulated errors */
    int on_odd_row;		/* flag to remember which row we are on */
    int *error_limiter;		/* table for clamping the applied error */
    int *error_limiter_storage;	/* gdMalloc'd storage for the above */
    int transparentIsPresent;	/* TBB: for rescaling to ensure that */
    int opaqueIsPresent;	/* 100% opacity & transparency are preserved */
  }
my_cquantizer;

typedef my_cquantizer *my_cquantize_ptr;

/*
 * Prescan the pixel array. 
 * 
 * The prescan simply updates the histogram, which has been
 * initialized to zeroes by start_pass.
 *
 */

static void
prescan_quantize (gdImagePtr im, my_cquantize_ptr cquantize)
{
  register histptr histp;
  register hist4d histogram = cquantize->histogram;
  int row;
  int col;
  int *ptr;
  int width = im->sx;

  for (row = 0; row < im->sy; row++)
    {
      ptr = im->tpixels[row];
      for (col = width; col > 0; col--)
	{
	  /* get pixel value and index into the histogram */
	  int r, g, b, a;
	  r = gdTrueColorGetRed (*ptr) >> C0_SHIFT;
	  g = gdTrueColorGetGreen (*ptr) >> C1_SHIFT;
	  b = gdTrueColorGetBlue (*ptr) >> C2_SHIFT;
	  a = gdTrueColorGetAlpha (*ptr);
	  /* We must have 100% opacity and transparency available
	     in the color map to do an acceptable job with alpha 
	     channel, if opacity and transparency are present in the
	     original, because of the visual properties of large
	     flat-color border areas (requiring 100% transparency) 
	     and the behavior of poorly implemented browsers 
	     (requiring 100% opacity). Test for the presence of
	     these here, and rescale the most opaque and transparent
	     palette entries at the end if so. This avoids the need
	     to develop a fuller understanding I have not been able
	     to reach so far in my study of this subject. TBB */
	  if (a == gdAlphaTransparent)
	    {
	      cquantize->transparentIsPresent = 1;
	    }
	  if (a == gdAlphaOpaque)
	    {
	      cquantize->opaqueIsPresent = 1;
	    }
	  a >>= C3_SHIFT;
	  histp = &histogram[r][g][b][a];
	  /* increment, check for overflow and undo increment if so. */
	  if (++(*histp) <= 0)
	    (*histp)--;
	  ptr++;
	}
    }
}


/*
 * Next we have the really interesting routines: selection of a colormap
 * given the completed histogram.
 * These routines work with a list of "boxes", each representing a rectangular
 * subset of the input color space (to histogram precision).
 */

typedef struct
{
  /* The bounds of the box (inclusive); expressed as histogram indexes */
  int c0min, c0max;
  int c1min, c1max;
  int c2min, c2max;
  int c3min, c3max;
  /* The volume (actually 2-norm) of the box */
  int volume;
  /* The number of nonzero histogram cells within this box */
  long colorcount;
}
box;

typedef box *boxptr;

static boxptr
find_biggest_color_pop (boxptr boxlist, int numboxes)
/* Find the splittable box with the largest color population */
/* Returns NULL if no splittable boxes remain */
{
  register boxptr boxp;
  register int i;
  register long maxc = 0;
  boxptr which = NULL;

  for (i = 0, boxp = boxlist; i < numboxes; i++, boxp++)
    {
      if (boxp->colorcount > maxc && boxp->volume > 0)
	{
	  which = boxp;
	  maxc = boxp->colorcount;
	}
    }
  return which;
}


static boxptr
find_biggest_volume (boxptr boxlist, int numboxes)
/* Find the splittable box with the largest (scaled) volume */
/* Returns NULL if no splittable boxes remain */
{
  register boxptr boxp;
  register int i;
  register int maxv = 0;
  boxptr which = NULL;

  for (i = 0, boxp = boxlist; i < numboxes; i++, boxp++)
    {
      if (boxp->volume > maxv)
	{
	  which = boxp;
	  maxv = boxp->volume;
	}
    }
  return which;
}


static void
update_box (gdImagePtr im, my_cquantize_ptr cquantize, boxptr boxp)
/* Shrink the min/max bounds of a box to enclose only nonzero elements, */
/* and recompute its volume and population */
{
  hist4d histogram = cquantize->histogram;
  histptr histp;
  int c0, c1, c2, c3;
  int c0min, c0max, c1min, c1max, c2min, c2max, c3min, c3max;
  int dist0, dist1, dist2, dist3;
  long ccount;

  c0min = boxp->c0min;
  c0max = boxp->c0max;
  c1min = boxp->c1min;
  c1max = boxp->c1max;
  c2min = boxp->c2min;
  c2max = boxp->c2max;
  c3min = boxp->c3min;
  c3max = boxp->c3max;

  if (c0max > c0min)
    {
      for (c0 = c0min; c0 <= c0max; c0++)
	{
	  for (c1 = c1min; c1 <= c1max; c1++)
	    {
	      for (c2 = c2min; c2 <= c2max; c2++)
		{
		  histp = &histogram[c0][c1][c2][c3min];
		  for (c3 = c3min; c3 <= c3max; c3++)
		    {
		      if (*histp++ != 0)
			{
			  boxp->c0min = c0min = c0;
			  goto have_c0min;
			}
		    }
		}
	    }
	}
    }
have_c0min:
  if (c0max > c0min)
    {
      for (c0 = c0max; c0 >= c0min; c0--)
	{
	  for (c1 = c1min; c1 <= c1max; c1++)
	    {
	      for (c2 = c2min; c2 <= c2max; c2++)
		{
		  histp = &histogram[c0][c1][c2][c3min];
		  for (c3 = c3min; c3 <= c3max; c3++)
		    {
		      if (*histp++ != 0)
			{
			  boxp->c0max = c0max = c0;
			  goto have_c0max;
			}
		    }
		}
	    }
	}
    }
have_c0max:
  if (c1max > c1min)
    for (c1 = c1min; c1 <= c1max; c1++)
      for (c0 = c0min; c0 <= c0max; c0++)
	{
	  for (c2 = c2min; c2 <= c2max; c2++)
	    {
	      histp = &histogram[c0][c1][c2][c3min];
	      for (c3 = c3min; c3 <= c3max; c3++)
		if (*histp++ != 0)
		  {
		    boxp->c1min = c1min = c1;
		    goto have_c1min;
		  }
	    }
	}
have_c1min:
  if (c1max > c1min)
    for (c1 = c1max; c1 >= c1min; c1--)
      for (c0 = c0min; c0 <= c0max; c0++)
	{
	  for (c2 = c2min; c2 <= c2max; c2++)
	    {
	      histp = &histogram[c0][c1][c2][c3min];
	      for (c3 = c3min; c3 <= c3max; c3++)
		if (*histp++ != 0)
		  {
		    boxp->c1max = c1max = c1;
		    goto have_c1max;
		  }
	    }
	}
have_c1max:
  /* The original version hand-rolled the array lookup a little, but
     with four dimensions, I don't even want to think about it. TBB */
  if (c2max > c2min)
    for (c2 = c2min; c2 <= c2max; c2++)
      for (c0 = c0min; c0 <= c0max; c0++)
	for (c1 = c1min; c1 <= c1max; c1++)
	  for (c3 = c3min; c3 <= c3max; c3++)
	    if (histogram[c0][c1][c2][c3] != 0)
	      {
		boxp->c2min = c2min = c2;
		goto have_c2min;
	      }
have_c2min:
  if (c2max > c2min)
    for (c2 = c2max; c2 >= c2min; c2--)
      for (c0 = c0min; c0 <= c0max; c0++)
	for (c1 = c1min; c1 <= c1max; c1++)
	  for (c3 = c3min; c3 <= c3max; c3++)
	    if (histogram[c0][c1][c2][c3] != 0)
	      {
		boxp->c2max = c2max = c2;
		goto have_c2max;
	      }
have_c2max:
  if (c3max > c3min)
    for (c3 = c3min; c3 <= c3max; c3++)
      for (c0 = c0min; c0 <= c0max; c0++)
	for (c1 = c1min; c1 <= c1max; c1++)
	  for (c2 = c2min; c2 <= c2max; c2++)
	    if (histogram[c0][c1][c2][c3] != 0)
	      {
		boxp->c3min = c3min = c3;
		goto have_c3min;
	      }
have_c3min:
  if (c3max > c3min)
    for (c3 = c3max; c3 >= c3min; c3--)
      for (c0 = c0min; c0 <= c0max; c0++)
	for (c1 = c1min; c1 <= c1max; c1++)
	  for (c2 = c2min; c2 <= c2max; c2++)
	    if (histogram[c0][c1][c2][c3] != 0)
	      {
		boxp->c3max = c3max = c3;
		goto have_c3max;
	      }
have_c3max:
  /* Update box volume.
   * We use 2-norm rather than real volume here; this biases the method
   * against making long narrow boxes, and it has the side benefit that
   * a box is splittable iff norm > 0.
   * Since the differences are expressed in histogram-cell units,
   * we have to shift back to 8-bit units to get consistent distances; 
   * after which, we scale according to the selected distance scale factors.
   * TBB: alpha shifts back to 7 bit units. That was accounted for in the
   * alpha scale factor. 
   */
  dist0 = ((c0max - c0min) << C0_SHIFT) * C0_SCALE;
  dist1 = ((c1max - c1min) << C1_SHIFT) * C1_SCALE;
  dist2 = ((c2max - c2min) << C2_SHIFT) * C2_SCALE;
  dist3 = ((c3max - c3min) << C3_SHIFT) * C3_SCALE;
  boxp->volume = dist0 * dist0 + dist1 * dist1 + dist2 * dist2 + dist3 * dist3;

  /* Now scan remaining volume of box and compute population */
  ccount = 0;
  for (c0 = c0min; c0 <= c0max; c0++)
    for (c1 = c1min; c1 <= c1max; c1++)
      for (c2 = c2min; c2 <= c2max; c2++)
	{
	  histp = &histogram[c0][c1][c2][c3min];
	  for (c3 = c3min; c3 <= c3max; c3++, histp++)
	    if (*histp != 0)
	      {
		ccount++;
	      }
	}
  boxp->colorcount = ccount;
}


static int
median_cut (gdImagePtr im, my_cquantize_ptr cquantize,
	    boxptr boxlist, int numboxes,
	    int desired_colors)
/* Repeatedly select and split the largest box until we have enough boxes */
{
  int n, lb;
  int c0, c1, c2, c3, cmax;
  register boxptr b1, b2;

  while (numboxes < desired_colors)
    {
      /* Select box to split.
       * Current algorithm: by population for first half, then by volume.
       */
      if (numboxes * 2 <= desired_colors)
	{
	  b1 = find_biggest_color_pop (boxlist, numboxes);
	}
      else
	{
	  b1 = find_biggest_volume (boxlist, numboxes);
	}
      if (b1 == NULL)		/* no splittable boxes left! */
	break;
      b2 = &boxlist[numboxes];	/* where new box will go */
      /* Copy the color bounds to the new box. */
      b2->c0max = b1->c0max;
      b2->c1max = b1->c1max;
      b2->c2max = b1->c2max;
      b2->c3max = b1->c3max;
      b2->c0min = b1->c0min;
      b2->c1min = b1->c1min;
      b2->c2min = b1->c2min;
      b2->c3min = b1->c3min;
      /* Choose which axis to split the box on.
       * Current algorithm: longest scaled axis.
       * See notes in update_box about scaling distances.
       */
      c0 = ((b1->c0max - b1->c0min) << C0_SHIFT) * C0_SCALE;
      c1 = ((b1->c1max - b1->c1min) << C1_SHIFT) * C1_SCALE;
      c2 = ((b1->c2max - b1->c2min) << C2_SHIFT) * C2_SCALE;
      c3 = ((b1->c3max - b1->c3min) << C3_SHIFT) * C3_SCALE;
      /* We want to break any ties in favor of green, then red, then blue, 
         with alpha last. */
      cmax = c1;
      n = 1;
      if (c0 > cmax)
	{
	  cmax = c0;
	  n = 0;
	}
      if (c2 > cmax)
	{
	  cmax = c2;
	  n = 2;
	}
      if (c3 > cmax)
	{
	  n = 3;
	}
      /* Choose split point along selected axis, and update box bounds.
       * Current algorithm: split at halfway point.
       * (Since the box has been shrunk to minimum volume,
       * any split will produce two nonempty subboxes.)
       * Note that lb value is max for lower box, so must be < old max.
       */
      switch (n)
	{
	case 0:
	  lb = (b1->c0max + b1->c0min) / 2;
	  b1->c0max = lb;
	  b2->c0min = lb + 1;
	  break;
	case 1:
	  lb = (b1->c1max + b1->c1min) / 2;
	  b1->c1max = lb;
	  b2->c1min = lb + 1;
	  break;
	case 2:
	  lb = (b1->c2max + b1->c2min) / 2;
	  b1->c2max = lb;
	  b2->c2min = lb + 1;
	  break;
	case 3:
	  lb = (b1->c3max + b1->c3min) / 2;
	  b1->c3max = lb;
	  b2->c3min = lb + 1;
	  break;
	}
      /* Update stats for boxes */
      update_box (im, cquantize, b1);
      update_box (im, cquantize, b2);
      numboxes++;
    }
  return numboxes;
}


static void
compute_color (gdImagePtr im, my_cquantize_ptr cquantize,
	       boxptr boxp, int icolor)
/* 
   Compute representative color for a box, put it in 
   palette index icolor */
{
  /* Current algorithm: mean weighted by pixels (not colors) */
  /* Note it is important to get the rounding correct! */
  hist4d histogram = cquantize->histogram;
  histptr histp;
  int c0, c1, c2, c3;
  int c0min, c0max, c1min, c1max, c2min, c2max, c3min, c3max;
  long count;
  long total = 0;
  long c0total = 0;
  long c1total = 0;
  long c2total = 0;
  long c3total = 0;

  c0min = boxp->c0min;
  c0max = boxp->c0max;
  c1min = boxp->c1min;
  c1max = boxp->c1max;
  c2min = boxp->c2min;
  c2max = boxp->c2max;
  c3min = boxp->c3min;
  c3max = boxp->c3max;

  for (c0 = c0min; c0 <= c0max; c0++)
    {
      for (c1 = c1min; c1 <= c1max; c1++)
	{
	  for (c2 = c2min; c2 <= c2max; c2++)
	    {
	      histp = &histogram[c0][c1][c2][c3min];
	      for (c3 = c3min; c3 <= c3max; c3++)
		{
		  if ((count = *histp++) != 0)
		    {
		      total += count;
		      c0total += ((c0 << C0_SHIFT) + ((1 << C0_SHIFT) >> 1)) * count;
		      c1total += ((c1 << C1_SHIFT) + ((1 << C1_SHIFT) >> 1)) * count;
		      c2total += ((c2 << C2_SHIFT) + ((1 << C2_SHIFT) >> 1)) * count;
		      c3total += ((c3 << C3_SHIFT) + ((1 << C3_SHIFT) >> 1)) * count;
		    }
		}
	    }
	}
    }
  im->red[icolor] = (int) ((c0total + (total >> 1)) / total);
  im->green[icolor] = (int) ((c1total + (total >> 1)) / total);
  im->blue[icolor] = (int) ((c2total + (total >> 1)) / total);
  im->alpha[icolor] = (int) ((c3total + (total >> 1)) / total);
  im->open[icolor] = 0;
  if (im->colorsTotal <= icolor)
    {
      im->colorsTotal = icolor + 1;
    }
}

static void
select_colors (gdImagePtr im, my_cquantize_ptr cquantize, int desired_colors)
/* Master routine for color selection */
{
  boxptr boxlist;
  int numboxes;
  int i;

  /* Allocate workspace for box list */
  boxlist = (boxptr) gdMalloc (desired_colors * sizeof (box));
  /* Initialize one box containing whole space */
  numboxes = 1;
  /* Note maxval for alpha is different */
  boxlist[0].c0min = 0;
  boxlist[0].c0max = 255 >> C0_SHIFT;
  boxlist[0].c1min = 0;
  boxlist[0].c1max = 255 >> C1_SHIFT;
  boxlist[0].c2min = 0;
  boxlist[0].c2max = 255 >> C2_SHIFT;
  boxlist[0].c3min = 0;
  boxlist[0].c3max = gdAlphaMax >> C3_SHIFT;
  /* Shrink it to actually-used volume and set its statistics */
  update_box (im, cquantize, &boxlist[0]);
  /* Perform median-cut to produce final box list */
  numboxes = median_cut (im, cquantize, boxlist, numboxes, desired_colors);
  /* Compute the representative color for each box, fill colormap */
  for (i = 0; i < numboxes; i++)
    compute_color (im, cquantize, &boxlist[i], i);
  /* TBB: if the image contains colors at both scaled ends
     of the alpha range, rescale slightly to make sure alpha 
     covers the full spectrum from 100% transparent to 100% 
     opaque. Even a faint distinct background color is 
     generally considered failure with regard to alpha. */

  im->colorsTotal = numboxes;
  gdFree (boxlist);
}


/*
 * These routines are concerned with the time-critical task of mapping input
 * colors to the nearest color in the selected colormap.
 *
 * We re-use the histogram space as an "inverse color map", essentially a
 * cache for the results of nearest-color searches.  All colors within a
 * histogram cell will be mapped to the same colormap entry, namely the one
 * closest to the cell's center.  This may not be quite the closest entry to
 * the actual input color, but it's almost as good.  A zero in the cache
 * indicates we haven't found the nearest color for that cell yet; the array
 * is cleared to zeroes before starting the mapping pass.  When we find the
 * nearest color for a cell, its colormap index plus one is recorded in the
 * cache for future use.  The pass2 scanning routines call fill_inverse_cmap
 * when they need to use an unfilled entry in the cache.
 *
 * Our method of efficiently finding nearest colors is based on the "locally
 * sorted search" idea described by Heckbert and on the incremental distance
 * calculation described by Spencer W. Thomas in chapter III.1 of Graphics
 * Gems II (James Arvo, ed.  Academic Press, 1991).  Thomas points out that
 * the distances from a given colormap entry to each cell of the histogram can
 * be computed quickly using an incremental method: the differences between
 * distances to adjacent cells themselves differ by a constant.  This allows a
 * fairly fast implementation of the "brute force" approach of computing the
 * distance from every colormap entry to every histogram cell.  Unfortunately,
 * it needs a work array to hold the best-distance-so-far for each histogram
 * cell (because the inner loop has to be over cells, not colormap entries).
 * The work array elements have to be INT32s, so the work array would need
 * 256Kb at our recommended precision.  This is not feasible in DOS machines.
 *
 * To get around these problems, we apply Thomas' method to compute the
 * nearest colors for only the cells within a small subbox of the histogram.
 * The work array need be only as big as the subbox, so the memory usage
 * problem is solved.  Furthermore, we need not fill subboxes that are never
 * referenced in pass2; many images use only part of the color gamut, so a
 * fair amount of work is saved.  An additional advantage of this
 * approach is that we can apply Heckbert's locality criterion to quickly
 * eliminate colormap entries that are far away from the subbox; typically
 * three-fourths of the colormap entries are rejected by Heckbert's criterion,
 * and we need not compute their distances to individual cells in the subbox.
 * The speed of this approach is heavily influenced by the subbox size: too
 * small means too much overhead, too big loses because Heckbert's criterion
 * can't eliminate as many colormap entries.  Empirically the best subbox
 * size seems to be about 1/512th of the histogram (1/8th in each direction).
 *
 * Thomas' article also describes a refined method which is asymptotically
 * faster than the brute-force method, but it is also far more complex and
 * cannot efficiently be applied to small subboxes.  It is therefore not
 * useful for programs intended to be portable to DOS machines.  On machines
 * with plenty of memory, filling the whole histogram in one shot with Thomas'
 * refined method might be faster than the present code --- but then again,
 * it might not be any faster, and it's certainly more complicated.
 */


/* log2(histogram cells in update box) for each axis; this can be adjusted */
#define BOX_C0_LOG  (HIST_C0_BITS-3)
#define BOX_C1_LOG  (HIST_C1_BITS-3)
#define BOX_C2_LOG  (HIST_C2_BITS-3)
#define BOX_C3_LOG  (HIST_C3_BITS-3)

#define BOX_C0_ELEMS  (1<<BOX_C0_LOG)	/* # of hist cells in update box */
#define BOX_C1_ELEMS  (1<<BOX_C1_LOG)
#define BOX_C2_ELEMS  (1<<BOX_C2_LOG)
#define BOX_C3_ELEMS  (1<<BOX_C3_LOG)

#define BOX_C0_SHIFT  (C0_SHIFT + BOX_C0_LOG)
#define BOX_C1_SHIFT  (C1_SHIFT + BOX_C1_LOG)
#define BOX_C2_SHIFT  (C2_SHIFT + BOX_C2_LOG)
#define BOX_C3_SHIFT  (C3_SHIFT + BOX_C3_LOG)


/*
 * The next three routines implement inverse colormap filling.  They could
 * all be folded into one big routine, but splitting them up this way saves
 * some stack space (the mindist[] and bestdist[] arrays need not coexist)
 * and may allow some compilers to produce better code by registerizing more
 * inner-loop variables.
 */

static int
find_nearby_colors (gdImagePtr im, my_cquantize_ptr cquantize,
		int minc0, int minc1, int minc2, int minc3, int colorlist[])
/* Locate the colormap entries close enough to an update box to be candidates
 * for the nearest entry to some cell(s) in the update box.  The update box
 * is specified by the center coordinates of its first cell.  The number of
 * candidate colormap entries is returned, and their colormap indexes are
 * placed in colorlist[].
 * This routine uses Heckbert's "locally sorted search" criterion to select
 * the colors that need further consideration.
 */
{
  int numcolors = im->colorsTotal;
  int maxc0, maxc1, maxc2, maxc3;
  int centerc0, centerc1, centerc2, centerc3;
  int i, x, ncolors;
  int minmaxdist, min_dist, max_dist, tdist;
  int mindist[MAXNUMCOLORS];	/* min distance to colormap entry i */

  /* Compute true coordinates of update box's upper corner and center.
   * Actually we compute the coordinates of the center of the upper-corner
   * histogram cell, which are the upper bounds of the volume we care about.
   * Note that since ">>" rounds down, the "center" values may be closer to
   * min than to max; hence comparisons to them must be "<=", not "<".
   */
  maxc0 = minc0 + ((1 << BOX_C0_SHIFT) - (1 << C0_SHIFT));
  centerc0 = (minc0 + maxc0) >> 1;
  maxc1 = minc1 + ((1 << BOX_C1_SHIFT) - (1 << C1_SHIFT));
  centerc1 = (minc1 + maxc1) >> 1;
  maxc2 = minc2 + ((1 << BOX_C2_SHIFT) - (1 << C2_SHIFT));
  centerc2 = (minc2 + maxc2) >> 1;
  maxc3 = minc3 + ((1 << BOX_C3_SHIFT) - (1 << C3_SHIFT));
  centerc3 = (minc3 + maxc3) >> 1;

  /* For each color in colormap, find:
   *  1. its minimum squared-distance to any point in the update box
   *     (zero if color is within update box);
   *  2. its maximum squared-distance to any point in the update box.
   * Both of these can be found by considering only the corners of the box.
   * We save the minimum distance for each color in mindist[];
   * only the smallest maximum distance is of interest.
   */
  minmaxdist = 0x7FFFFFFFL;

  for (i = 0; i < numcolors; i++)
    {
      /* We compute the squared-c0-distance term, then add in the other three. */
      x = im->red[i];
      if (x < minc0)
	{
	  tdist = (x - minc0) * C0_SCALE;
	  min_dist = tdist * tdist;
	  tdist = (x - maxc0) * C0_SCALE;
	  max_dist = tdist * tdist;
	}
      else if (x > maxc0)
	{
	  tdist = (x - maxc0) * C0_SCALE;
	  min_dist = tdist * tdist;
	  tdist = (x - minc0) * C0_SCALE;
	  max_dist = tdist * tdist;
	}
      else
	{
	  /* within cell range so no contribution to min_dist */
	  min_dist = 0;
	  if (x <= centerc0)
	    {
	      tdist = (x - maxc0) * C0_SCALE;
	      max_dist = tdist * tdist;
	    }
	  else
	    {
	      tdist = (x - minc0) * C0_SCALE;
	      max_dist = tdist * tdist;
	    }
	}

      x = im->green[i];
      if (x < minc1)
	{
	  tdist = (x - minc1) * C1_SCALE;
	  min_dist += tdist * tdist;
	  tdist = (x - maxc1) * C1_SCALE;
	  max_dist += tdist * tdist;
	}
      else if (x > maxc1)
	{
	  tdist = (x - maxc1) * C1_SCALE;
	  min_dist += tdist * tdist;
	  tdist = (x - minc1) * C1_SCALE;
	  max_dist += tdist * tdist;
	}
      else
	{
	  /* within cell range so no contribution to min_dist */
	  if (x <= centerc1)
	    {
	      tdist = (x - maxc1) * C1_SCALE;
	      max_dist += tdist * tdist;
	    }
	  else
	    {
	      tdist = (x - minc1) * C1_SCALE;
	      max_dist += tdist * tdist;
	    }
	}

      x = im->blue[i];
      if (x < minc2)
	{
	  tdist = (x - minc2) * C2_SCALE;
	  min_dist += tdist * tdist;
	  tdist = (x - maxc2) * C2_SCALE;
	  max_dist += tdist * tdist;
	}
      else if (x > maxc2)
	{
	  tdist = (x - maxc2) * C2_SCALE;
	  min_dist += tdist * tdist;
	  tdist = (x - minc2) * C2_SCALE;
	  max_dist += tdist * tdist;
	}
      else
	{
	  /* within cell range so no contribution to min_dist */
	  if (x <= centerc2)
	    {
	      tdist = (x - maxc2) * C2_SCALE;
	      max_dist += tdist * tdist;
	    }
	  else
	    {
	      tdist = (x - minc2) * C2_SCALE;
	      max_dist += tdist * tdist;
	    }
	}

      x = im->alpha[i];
      if (x < minc3)
	{
	  tdist = (x - minc3) * C3_SCALE;
	  min_dist += tdist * tdist;
	  tdist = (x - maxc3) * C3_SCALE;
	  max_dist += tdist * tdist;
	}
      else if (x > maxc3)
	{
	  tdist = (x - maxc3) * C3_SCALE;
	  min_dist += tdist * tdist;
	  tdist = (x - minc3) * C3_SCALE;
	  max_dist += tdist * tdist;
	}
      else
	{
	  /* within cell range so no contribution to min_dist */
	  if (x <= centerc3)
	    {
	      tdist = (x - maxc3) * C3_SCALE;
	      max_dist += tdist * tdist;
	    }
	  else
	    {
	      tdist = (x - minc3) * C3_SCALE;
	      max_dist += tdist * tdist;
	    }
	}

      mindist[i] = min_dist;	/* save away the results */
      if (max_dist < minmaxdist)
	minmaxdist = max_dist;
    }

  /* Now we know that no cell in the update box is more than minmaxdist
   * away from some colormap entry.  Therefore, only colors that are
   * within minmaxdist of some part of the box need be considered.
   */
  ncolors = 0;
  for (i = 0; i < numcolors; i++)
    {
      if (mindist[i] <= minmaxdist)
	colorlist[ncolors++] = i;
    }
  return ncolors;
}


static void
find_best_colors (gdImagePtr im, my_cquantize_ptr cquantize,
		  int minc0, int minc1, int minc2, int minc3,
		  int numcolors, int colorlist[], int bestcolor[])
/* Find the closest colormap entry for each cell in the update box,
 * given the list of candidate colors prepared by find_nearby_colors.
 * Return the indexes of the closest entries in the bestcolor[] array.
 * This routine uses Thomas' incremental distance calculation method to
 * find the distance from a colormap entry to successive cells in the box.
 */
{
  int ic0, ic1, ic2;
  int i, icolor;
  register int *bptr;		/* pointer into bestdist[] array */
  int *cptr;			/* pointer into bestcolor[] array */
  int dist0, dist1, dist2;	/* initial distance values */
  int xx0, xx1, xx2;		/* distance increments */
  int inc0, inc1, inc2, inc3;	/* initial values for increments */
  /* This array holds the distance to the nearest-so-far color for each cell */
  int bestdist[BOX_C0_ELEMS * BOX_C1_ELEMS * BOX_C2_ELEMS * BOX_C3_ELEMS];

  /* Initialize best-distance for each cell of the update box */
  bptr = bestdist;
  for (i = BOX_C0_ELEMS * BOX_C1_ELEMS * BOX_C2_ELEMS * BOX_C3_ELEMS - 1; i >= 0; i--)
    *bptr++ = 0x7FFFFFFFL;

  /* For each color selected by find_nearby_colors,
   * compute its distance to the center of each cell in the box.
   * If that's less than best-so-far, update best distance and color number.
   */

  /* Nominal steps between cell centers ("x" in Thomas article) */
#define STEP_C0  ((1 << C0_SHIFT) * C0_SCALE)
#define STEP_C1  ((1 << C1_SHIFT) * C1_SCALE)
#define STEP_C2  ((1 << C2_SHIFT) * C2_SCALE)
#define STEP_C3  ((1 << C3_SHIFT) * C3_SCALE)

	for (i = 0; i < numcolors; i++)	{
		icolor = colorlist[i];
		/* Compute (square of) distance from minc0/c1/c2 to this color */
		inc0 = (minc0 - (im->red[icolor])) * C0_SCALE;
		dist0 = inc0 * inc0;
		inc1 = (minc1 - (im->green[icolor])) * C1_SCALE;
		dist0 += inc1 * inc1;
		inc2 = (minc2 - (im->blue[icolor])) * C2_SCALE;
		dist0 += inc2 * inc2;
		inc3 = (minc3 - (im->alpha[icolor])) * C3_SCALE;
		dist0 += inc3 * inc3;
		/* Form the initial difference increments */
		inc0 = inc0 * (2 * STEP_C0) + STEP_C0 * STEP_C0;
		inc1 = inc1 * (2 * STEP_C1) + STEP_C1 * STEP_C1;
		inc2 = inc2 * (2 * STEP_C2) + STEP_C2 * STEP_C2;
		inc3 = inc3 * (2 * STEP_C3) + STEP_C3 * STEP_C3;
		/* Now loop over all cells in box, updating distance per Thomas method */
		bptr = bestdist;
		cptr = bestcolor;
		xx0 = inc0;
		for (ic0 = BOX_C0_ELEMS - 1; ic0 >= 0; ic0--) {
			dist1 = dist0;
			xx1 = inc1;
			for (ic1 = BOX_C1_ELEMS - 1; ic1 >= 0; ic1--) {
				dist2 = dist1;
				xx2 = inc2;
				for (ic2 = BOX_C2_ELEMS - 1; ic2 >= 0; ic2--) {
					register int dist3 = dist2;		/* current distance in inner loop */
					register int xx3 = inc3;
					register int ic3;
					for (ic3 = BOX_C3_ELEMS - 1; ic3 >= 0; ic3--) {
						if (dist3 < *bptr) {
							*bptr = dist3;
							*cptr = icolor;
						}
						dist3 += xx3;
						xx3 += 2 * STEP_C3 * STEP_C3;
						bptr++;
						cptr++;
					}
					dist2 += xx2;
					xx2 += 2 * STEP_C2 * STEP_C2;
				}
				dist1 += xx1;
				xx1 += 2 * STEP_C1 * STEP_C1;
			}
			dist0 += xx0;
			xx0 += 2 * STEP_C0 * STEP_C0;
		}
	}
}


static void
fill_inverse_cmap (gdImagePtr im, my_cquantize_ptr cquantize,
		   int c0, int c1, int c2, int c3)
/* Fill the inverse-colormap entries in the update box that contains */
/* histogram cell c0/c1/c2/c3.  (Only that one cell MUST be filled, but */
/* we can fill as many others as we wish.) */
{
  hist4d histogram = cquantize->histogram;
  int minc0, minc1, minc2, minc3;	/* lower left corner of update box */
  int ic0, ic1, ic2, ic3;
  register int *cptr;		/* pointer into bestcolor[] array */
  register histptr cachep;	/* pointer into main cache array */
  /* This array lists the candidate colormap indexes. */
  int colorlist[MAXNUMCOLORS];
  int numcolors;		/* number of candidate colors */
  /* This array holds the actually closest colormap index for each cell. */
  int bestcolor[BOX_C0_ELEMS * BOX_C1_ELEMS * BOX_C2_ELEMS * BOX_C3_ELEMS];

  /* Convert cell coordinates to update box ID */
  c0 >>= BOX_C0_LOG;
  c1 >>= BOX_C1_LOG;
  c2 >>= BOX_C2_LOG;
  c3 >>= BOX_C3_LOG;

  /* Compute true coordinates of update box's origin corner.
   * Actually we compute the coordinates of the center of the corner
   * histogram cell, which are the lower bounds of the volume we care about.
   */
  minc0 = (c0 << BOX_C0_SHIFT) + ((1 << C0_SHIFT) >> 1);
  minc1 = (c1 << BOX_C1_SHIFT) + ((1 << C1_SHIFT) >> 1);
  minc2 = (c2 << BOX_C2_SHIFT) + ((1 << C2_SHIFT) >> 1);
  minc3 = (c3 << BOX_C3_SHIFT) + ((1 << C3_SHIFT) >> 1);
  /* Determine which colormap entries are close enough to be candidates
   * for the nearest entry to some cell in the update box.
   */
  numcolors = find_nearby_colors (im, cquantize, minc0, minc1, minc2, minc3, colorlist);

  /* Determine the actually nearest colors. */
  find_best_colors (im, cquantize, minc0, minc1, minc2, minc3, numcolors, colorlist,
		    bestcolor);

  /* Save the best color numbers (plus 1) in the main cache array */
  c0 <<= BOX_C0_LOG;		/* convert ID back to base cell indexes */
  c1 <<= BOX_C1_LOG;
  c2 <<= BOX_C2_LOG;
  c3 <<= BOX_C3_LOG;
  cptr = bestcolor;
  for (ic0 = 0; ic0 < BOX_C0_ELEMS; ic0++)
    {
      for (ic1 = 0; ic1 < BOX_C1_ELEMS; ic1++)
	{
	  for (ic2 = 0; ic2 < BOX_C2_ELEMS; ic2++)
	    {
	      cachep = &histogram[c0 + ic0][c1 + ic1][c2 + ic2][c3];
	      for (ic3 = 0; ic3 < BOX_C3_ELEMS; ic3++)
		{
		  *cachep++ = (histcell) ((*cptr++) + 1);
		}
	    }
	}
    }
}


/*
 * Map some rows of pixels to the output colormapped representation.
 */

void
pass2_no_dither (gdImagePtr im, my_cquantize_ptr cquantize)
/* This version performs no dithering */
{
  hist4d histogram = cquantize->histogram;
  register int *inptr;
  register unsigned char *outptr;
  register histptr cachep;
  register int c0, c1, c2, c3;
  int row;
  int col;
  int width = im->sx;
  int num_rows = im->sy;
  for (row = 0; row < num_rows; row++)
    {
      inptr = im->tpixels[row];
      outptr = im->pixels[row];
      for (col = 0; col < width; col++)
	{
	  int r, g, b, a;
	  /* get pixel value and index into the cache */
	  r = gdTrueColorGetRed (*inptr);
	  g = gdTrueColorGetGreen (*inptr);
	  b = gdTrueColorGetBlue (*inptr);
	  a = gdTrueColorGetAlpha (*inptr++);
	  c0 = r >> C0_SHIFT;
	  c1 = g >> C1_SHIFT;
	  c2 = b >> C2_SHIFT;
	  c3 = a >> C3_SHIFT;
	  cachep = &histogram[c0][c1][c2][c3];
	  /* If we have not seen this color before, find nearest colormap entry */
	  /* and update the cache */
	  if (*cachep == 0)
	    {
#if 0
	      /* TBB: quick and dirty approach for use when testing
	         fill_inverse_cmap for errors */
	      int i;
	      int best = -1;
	      int mindist = 0x7FFFFFFF;
	      for (i = 0; (i < im->colorsTotal); i++)
		{
		  int rdist = (im->red[i] >> C0_SHIFT) - c0;
		  int gdist = (im->green[i] >> C1_SHIFT) - c1;
		  int bdist = (im->blue[i] >> C2_SHIFT) - c2;
		  int adist = (im->alpha[i] >> C3_SHIFT) - c3;
		  int dist = (rdist * rdist) * R_SCALE +
		  (gdist * gdist) * G_SCALE +
		  (bdist * bdist) * B_SCALE +
		  (adist * adist) * A_SCALE;
		  if (dist < mindist)
		    {
		      best = i;
		      mindist = dist;
		    }
		}
	      *cachep = best + 1;
#endif
	      fill_inverse_cmap (im, cquantize, c0, c1, c2, c3);
	    }
	  /* Now emit the colormap index for this cell */
	  *outptr++ = (*cachep - 1);
	}
    }
}

/* We assume that right shift corresponds to signed division by 2 with
 * rounding towards minus infinity.  This is correct for typical "arithmetic
 * shift" instructions that shift in copies of the sign bit.  But some
 * C compilers implement >> with an unsigned shift.  For these machines you
 * must define RIGHT_SHIFT_IS_UNSIGNED.
 * RIGHT_SHIFT provides a proper signed right shift of an INT32 quantity.
 * It is only applied with constant shift counts.  SHIFT_TEMPS must be
 * included in the variables of any routine using RIGHT_SHIFT.
 */

#ifdef RIGHT_SHIFT_IS_UNSIGNED
#define SHIFT_TEMPS	INT32 shift_temp;
#define RIGHT_SHIFT(x,shft)  \
	((shift_temp = (x)) < 0 ? \
	 (shift_temp >> (shft)) | ((~((INT32) 0)) << (32-(shft))) : \
	 (shift_temp >> (shft)))
#else
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,shft)	((x) >> (shft))
#endif


void
pass2_fs_dither (gdImagePtr im, my_cquantize_ptr cquantize)

/* This version performs Floyd-Steinberg dithering */
{
  hist4d histogram = cquantize->histogram;
  register LOCFSERROR cur0, cur1, cur2, cur3;	/* current error or pixel value */
  LOCFSERROR belowerr0, belowerr1, belowerr2, belowerr3;	/* error for pixel below cur */
  LOCFSERROR bpreverr0, bpreverr1, bpreverr2, bpreverr3;	/* error for below/prev col */
  register FSERRPTR errorptr;	/* => fserrors[] at column before current */
  int *inptr;			/* => current input pixel */
  unsigned char *outptr;	/* => current output pixel */
  histptr cachep;
  int dir;			/* +1 or -1 depending on direction */
  int dir4;			/* 4*dir, for advancing errorptr */
  int row;
  int col;
  int width = im->sx;
  int num_rows = im->sy;
  int *error_limit = cquantize->error_limiter;
  int *colormap0 = im->red;
  int *colormap1 = im->green;
  int *colormap2 = im->blue;
  int *colormap3 = im->alpha;
  SHIFT_TEMPS

    for (row = 0; row < num_rows; row++)
    {
      inptr = im->tpixels[row];
      outptr = im->pixels[row];
      if (cquantize->on_odd_row)
	{
	  /* work right to left in this row */
	  inptr += (width - 1);	/* so point to rightmost pixel */
	  outptr += width - 1;
	  dir = -1;
	  dir4 = -4;
	  errorptr = cquantize->fserrors + (width + 1) * 4;	/* => entry after last column */
	  cquantize->on_odd_row = FALSE;	/* flip for next time */
	}
      else
	{
	  /* work left to right in this row */
	  dir = 1;
	  dir4 = 4;
	  errorptr = cquantize->fserrors;	/* => entry before first real column */
	  cquantize->on_odd_row = TRUE;		/* flip for next time */
	}
      /* Preset error values: no error propagated to first pixel from left */
      cur0 = cur1 = cur2 = cur3 = 0;
      /* and no error propagated to row below yet */
      belowerr0 = belowerr1 = belowerr2 = belowerr3 = 0;
      bpreverr0 = bpreverr1 = bpreverr2 = bpreverr3 = 0;

      for (col = width; col > 0; col--)
	{
	  int a;
	  /* curN holds the error propagated from the previous pixel on the
	   * current line.  Add the error propagated from the previous line
	   * to form the complete error correction term for this pixel, and
	   * round the error term (which is expressed * 16) to an integer.
	   * RIGHT_SHIFT rounds towards minus infinity, so adding 8 is correct
	   * for either sign of the error value.
	   * Note: errorptr points to *previous* column's array entry.
	   */
	  cur0 = RIGHT_SHIFT (cur0 + errorptr[dir4 + 0] + 8, 4);
	  cur1 = RIGHT_SHIFT (cur1 + errorptr[dir4 + 1] + 8, 4);
	  cur2 = RIGHT_SHIFT (cur2 + errorptr[dir4 + 2] + 8, 4);
	  cur3 = RIGHT_SHIFT (cur3 + errorptr[dir4 + 3] + 8, 4);
	  /* Limit the error using transfer function set by init_error_limit.
	   * See comments with init_error_limit for rationale.
	   */
	  cur0 = error_limit[cur0];
	  cur1 = error_limit[cur1];
	  cur2 = error_limit[cur2];
	  cur3 = error_limit[cur3];
	  /* Form pixel value + error, and range-limit to 0..MAXJSAMPLE.
	   * The maximum error is +- MAXJSAMPLE (or less with error limiting);
	   * but we'll be lazy and just clamp this with an if test (TBB).
	   */
	  cur0 += gdTrueColorGetRed (*inptr);
	  cur1 += gdTrueColorGetGreen (*inptr);
	  cur2 += gdTrueColorGetBlue (*inptr);
	  /* Expand to 8 bits for consistency with dithering algorithm -- TBB */
	  a = gdTrueColorGetAlpha (*inptr);
	  cur3 += (a << 1) + (a >> 6);
	  if (cur0 < 0)
	    {
	      cur0 = 0;
	    }
	  if (cur0 > 255)
	    {
	      cur0 = 255;
	    }
	  if (cur1 < 0)
	    {
	      cur1 = 0;
	    }
	  if (cur1 > 255)
	    {
	      cur1 = 255;
	    }
	  if (cur2 < 0)
	    {
	      cur2 = 0;
	    }
	  if (cur2 > 255)
	    {
	      cur2 = 255;
	    }
	  if (cur3 < 0)
	    {
	      cur3 = 0;
	    }
	  if (cur3 > 255)
	    {
	      cur3 = 255;
	    }
	  /* Index into the cache with adjusted pixel value */
	  cachep = &histogram
	    [cur0 >> C0_SHIFT]
	    [cur1 >> C1_SHIFT]
	    [cur2 >> C2_SHIFT]
	    [cur3 >> (C3_SHIFT + 1)];
	  /* If we have not seen this color before, find nearest colormap */
	  /* entry and update the cache */
	  if (*cachep == 0)
	    fill_inverse_cmap (im, cquantize,
		       cur0 >> C0_SHIFT, cur1 >> C1_SHIFT, cur2 >> C2_SHIFT,
			       cur3 >> (C3_SHIFT + 1));
	  /* Now emit the colormap index for this cell */
	  {
	    register int pixcode = *cachep - 1;
	    *outptr = pixcode;
	    /* Compute representation error for this pixel */
	    cur0 -= colormap0[pixcode];
	    cur1 -= colormap1[pixcode];
	    cur2 -= colormap2[pixcode];
	    cur3 -= ((colormap3[pixcode] << 1) + (colormap3[pixcode] >> 6));
	  }
	  /* Compute error fractions to be propagated to adjacent pixels.
	   * Add these into the running sums, and simultaneously shift the
	   * next-line error sums left by 1 column.
	   */
	  {
	    register LOCFSERROR bnexterr, delta;

	    bnexterr = cur0;	/* Process component 0 */
	    delta = cur0 * 2;
	    cur0 += delta;	/* form error * 3 */
	    errorptr[0] = (FSERROR) (bpreverr0 + cur0);
	    cur0 += delta;	/* form error * 5 */
	    bpreverr0 = belowerr0 + cur0;
	    belowerr0 = bnexterr;
	    cur0 += delta;	/* form error * 7 */
	    bnexterr = cur1;	/* Process component 1 */
	    delta = cur1 * 2;
	    cur1 += delta;	/* form error * 3 */
	    errorptr[1] = (FSERROR) (bpreverr1 + cur1);
	    cur1 += delta;	/* form error * 5 */
	    bpreverr1 = belowerr1 + cur1;
	    belowerr1 = bnexterr;
	    cur1 += delta;	/* form error * 7 */
	    bnexterr = cur2;	/* Process component 2 */
	    delta = cur2 * 2;
	    cur2 += delta;	/* form error * 3 */
	    errorptr[2] = (FSERROR) (bpreverr2 + cur2);
	    cur2 += delta;	/* form error * 5 */
	    bpreverr2 = belowerr2 + cur2;
	    belowerr2 = bnexterr;
	    cur2 += delta;	/* form error * 7 */
	    bnexterr = cur3;	/* Process component 3 */
	    delta = cur3 * 2;
	    cur3 += delta;	/* form error * 3 */
	    errorptr[3] = (FSERROR) (bpreverr3 + cur3);
	    cur3 += delta;	/* form error * 5 */
	    bpreverr3 = belowerr3 + cur3;
	    belowerr3 = bnexterr;
	    cur3 += delta;	/* form error * 7 */
	  }
	  /* At this point curN contains the 7/16 error value to be propagated
	   * to the next pixel on the current line, and all the errors for the
	   * next line have been shifted over.  We are therefore ready to move on.
	   */
	  inptr += dir;		/* Advance pixel pointers to next column */
	  outptr += dir;
	  errorptr += dir4;	/* advance errorptr to current column */
	}
      /* Post-loop cleanup: we must unload the final error values into the
       * final fserrors[] entry.  Note we need not unload belowerrN because
       * it is for the dummy column before or after the actual array.
       */
      errorptr[0] = (FSERROR) bpreverr0;	/* unload prev errs into array */
      errorptr[1] = (FSERROR) bpreverr1;
      errorptr[2] = (FSERROR) bpreverr2;
      errorptr[3] = (FSERROR) bpreverr3;
    }
}


/*
 * Initialize the error-limiting transfer function (lookup table).
 * The raw F-S error computation can potentially compute error values of up to
 * +- MAXJSAMPLE.  But we want the maximum correction applied to a pixel to be
 * much less, otherwise obviously wrong pixels will be created.  (Typical
 * effects include weird fringes at color-area boundaries, isolated bright
 * pixels in a dark area, etc.)  The standard advice for avoiding this problem
 * is to ensure that the "corners" of the color cube are allocated as output
 * colors; then repeated errors in the same direction cannot cause cascading
 * error buildup.  However, that only prevents the error from getting
 * completely out of hand; Aaron Giles reports that error limiting improves
 * the results even with corner colors allocated.
 * A simple clamping of the error values to about +- MAXJSAMPLE/8 works pretty
 * well, but the smoother transfer function used below is even better.  Thanks
 * to Aaron Giles for this idea.
 */

static int
init_error_limit (gdImagePtr im, my_cquantize_ptr cquantize)
/* Allocate and fill in the error_limiter table */
{
  int *table;
  int in, out;

  cquantize->error_limiter_storage = (int *) gdMalloc ((255 * 2 + 1) * sizeof (int));
  if (!cquantize->error_limiter_storage)
    {
      return 0;
    }
  /* so can index -MAXJSAMPLE .. +MAXJSAMPLE */
  cquantize->error_limiter = cquantize->error_limiter_storage + 255;
  table = cquantize->error_limiter;
#define STEPSIZE ((255+1)/16)
  /* Map errors 1:1 up to +- MAXJSAMPLE/16 */
  out = 0;
  for (in = 0; in < STEPSIZE; in++, out++)
    {
      table[in] = out;
      table[-in] = -out;
    }
  /* Map errors 1:2 up to +- 3*MAXJSAMPLE/16 */
  for (; in < STEPSIZE * 3; in++, out += (in & 1) ? 0 : 1)
    {
      table[in] = out;
      table[-in] = -out;
    }
  /* Clamp the rest to final out value (which is (MAXJSAMPLE+1)/8) */
  for (; in <= 255; in++)
    {
      table[in] = out;
      table[-in] = -out;
    }
#undef STEPSIZE
  return 1;
}

static void
zeroHistogram (hist4d histogram)
{
  int i;
  int j;
  /* Zero the histogram or inverse color map */
  for (i = 0; i < HIST_C0_ELEMS; i++)
    {
      for (j = 0; j < HIST_C1_ELEMS; j++)
	{
	  memset (histogram[i][j],
		  0,
		  HIST_C2_ELEMS * HIST_C3_ELEMS * sizeof (histcell));
	}
    }
}

/* Here we go at last. */
void
gdImageTrueColorToPalette (gdImagePtr im, int dither, int colorsWanted)
{
	my_cquantize_ptr cquantize = 0;
	int i;
	size_t arraysize;
	if (!im->trueColor || colorsWanted <= 0) {
		/* Nothing to do! */
		return;
	}
	
	if (colorsWanted > gdMaxColors) {
		colorsWanted = gdMaxColors;
	}

	im->pixels = gdCalloc (sizeof (unsigned char *), im->sy);

	for (i = 0; i < im->sy; i++) {
		im->pixels[i] = gdCalloc (sizeof (unsigned char *), im->sx);
	}
	cquantize = (my_cquantize_ptr) gdCalloc (sizeof (my_cquantizer), 1);

	/* Allocate the histogram/inverse colormap storage */
	cquantize->histogram = (hist4d) gdMalloc (HIST_C0_ELEMS * sizeof (hist3d));
	for (i = 0; i < HIST_C0_ELEMS; i++) {
		int j;
		cquantize->histogram[i] = (hist3d) gdCalloc (HIST_C1_ELEMS, sizeof (hist2d));
		for (j = 0; j < HIST_C1_ELEMS; j++) {
			cquantize->histogram[i][j] = (hist2d) gdCalloc (HIST_C2_ELEMS * HIST_C3_ELEMS, sizeof (histcell));
		}
	}

	cquantize->fserrors = (FSERRPTR) gdMalloc (4 * sizeof (FSERROR));
	init_error_limit (im, cquantize);
	arraysize = (size_t) ((im->sx + 2) * (4 * sizeof (FSERROR)));
	gdFree(cquantize->fserrors);
	/* Allocate Floyd-Steinberg workspace. */
	cquantize->fserrors = gdCalloc (arraysize, 1);
	cquantize->on_odd_row = FALSE;

	/* Do the work! */
	zeroHistogram (cquantize->histogram);
	prescan_quantize (im, cquantize);
	select_colors (im, cquantize, colorsWanted);

	zeroHistogram (cquantize->histogram);
	if (dither) {
		pass2_fs_dither (im, cquantize);
	} else {
		pass2_no_dither (im, cquantize);
	}
	if (cquantize->transparentIsPresent) {
		int mt = -1;
		int mtIndex = -1;
		for (i = 0; i < im->colorsTotal; i++) {
			if (im->alpha[i] > mt) {
				mtIndex = i;
				mt = im->alpha[i];
			}
		}
		for (i = 0; i < im->colorsTotal; i++) {
			if (im->alpha[i] == mt) {
				im->alpha[i] = gdAlphaTransparent;
			}
		}
	}
	if (cquantize->opaqueIsPresent) {
		int mo = 128;
		int moIndex = -1;
		for (i = 0; i < im->colorsTotal; i++) {
			if (im->alpha[i] < mo) {
				moIndex = i;
				mo = im->alpha[i];
			}
		}
		for (i = 0; i < im->colorsTotal; i++) {
			if (im->alpha[i] == mo) {
				im->alpha[i] = gdAlphaOpaque;
			}
		}
	}
	
	/* Success! Get rid of the truecolor image data. */
	im->trueColor = 0;
	/* Junk the truecolor pixels */
	for (i = 0; i < im->sy; i++) {
		gdFree(im->tpixels[i]);
	}
	gdFree (im->tpixels);
	im->tpixels = 0;
	/* Tediously free stuff. */

	for (i = 0; i < HIST_C0_ELEMS; i++) {
		if (cquantize->histogram[i]) {
			int j;
			for (j = 0; j < HIST_C1_ELEMS; j++) {
				if (cquantize->histogram[i][j]) {
					gdFree(cquantize->histogram[i][j]);
				}
			}
			gdFree(cquantize->histogram[i]);
		}
	}
	if (cquantize->histogram) {
		gdFree(cquantize->histogram);
	}
	if (cquantize->fserrors) {
		gdFree(cquantize->fserrors);
	}
	if (cquantize->error_limiter_storage) {
		gdFree(cquantize->error_limiter_storage);
	}
	if (cquantize) {
		gdFree(cquantize);
	}
}

/* bring the palette colors in im2 to be closer to im1
 *
 */
int
gdImageColorMatch (gdImagePtr im1, gdImagePtr im2)
{
	unsigned long *buf; /* stores our calculations */
	unsigned long *bp; /* buf ptr */
	int color, rgb;
	int x,y;
	int count;

	if( !im1->trueColor ) {
		return -1; /* im1 must be True Color */
	}
	if( im2->trueColor ) {
		return -2; /* im2 must be indexed */
	}
	if( (im1->sx != im2->sx) || (im1->sy != im2->sy) ) {
		return -3; /* the images are meant to be the same dimensions */
	}

	buf = (unsigned long *)gdMalloc( sizeof(unsigned long) * 5 * im2->colorsTotal );
	memset( buf, 0, sizeof(unsigned long) * 5 * im2->colorsTotal );

	for( x=0; x<im1->sx; x++ ) {
		for( y=0; y<im1->sy; y++ ) {
			color = im2->pixels[y][x];
			rgb = im1->tpixels[y][x];
			bp = buf + (color * 5);
			(*(bp++))++;
			*(bp++) += gdTrueColorGetRed(rgb);
			*(bp++) += gdTrueColorGetGreen(rgb);
			*(bp++) += gdTrueColorGetBlue(rgb);
			*(bp++) += gdTrueColorGetAlpha(rgb);
		}
	}
	bp = buf;
	for( color=0; color<im2->colorsTotal; color++ ) {
		count = *(bp++);
		if( count > 0 ) {
			im2->red[color]		= *(bp++) / count;
			im2->green[color]	= *(bp++) / count;
			im2->blue[color]	= *(bp++) / count;
			im2->alpha[color]	= *(bp++) / count;
		} else {
			bp += 4;
		}
	}
	gdFree(buf);
	return 0;
}
