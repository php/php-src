/* NeuQuant Neural-Net Quantization Algorithm
 * ------------------------------------------
 *
 * Copyright (c) 1994 Anthony Dekker
 *
 * NEUQUANT Neural-Net quantization algorithm by Anthony Dekker, 1994.
 * See "Kohonen neural networks for optimal colour quantization"
 * in "Network: Computation in Neural Systems" Vol. 5 (1994) pp 351-367.
 * for a discussion of the algorithm.
 * See also  http://members.ozemail.com.au/~dekker/NEUQUANT.HTML
 *
 * Any party obtaining a copy of these files from the author, directly or
 * indirectly, is granted, free of charge, a full and unrestricted irrevocable,
 * world-wide, paid up, royalty-free, nonexclusive right and license to deal
 * in this software and documentation files (the "Software"), including without
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons who receive
 * copies from any such party to do so, with the only requirement being
 * that this copyright notice remain intact.
 *
 *
 * Modified to process 32bit RGBA images.
 * Stuart Coyle 2004-2007
 * From: http://pngnq.sourceforge.net/
 *
 * Ported to libgd by Pierre A. Joye
 * (and make it thread safety by droping static and  global variables)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include "gd.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_nnquant.h"

/* Network Definitions
   ------------------- */

#define maxnetpos	(MAXNETSIZE-1)
#define netbiasshift	4			/* bias for colour values */
#define ncycles		100			/* no. of learning cycles */

/* defs for freq and bias */
#define intbiasshift    16			/* bias for fractions */
#define intbias		(((int) 1)<<intbiasshift)
#define gammashift  	10			/* gamma = 1024 */
#define gamma   	(((int) 1)<<gammashift)
#define betashift  	10
#define beta		(intbias>>betashift)	/* beta = 1/1024 */
#define betagamma	(intbias<<(gammashift-betashift))

/* defs for decreasing radius factor */
#define initrad		(MAXNETSIZE>>3)		/* for 256 cols, radius starts */
#define radiusbiasshift	6			/* at 32.0 biased by 6 bits */
#define radiusbias	(((int) 1)<<radiusbiasshift)
#define initradius	(initrad*radiusbias)	/* and decreases by a */
#define radiusdec	30			/* factor of 1/30 each cycle */

/* defs for decreasing alpha factor */
#define alphabiasshift	10			/* alpha starts at 1.0 */
#define initalpha	(((int) 1)<<alphabiasshift)

/* radbias and alpharadbias used for radpower calculation */
#define radbiasshift	8
#define radbias		(((int) 1)<<radbiasshift)
#define alpharadbshift  (alphabiasshift+radbiasshift)
#define alpharadbias    (((int) 1)<<alpharadbshift)

#define ALPHA 0
#define RED 1
#define BLUE 2
#define GREEN 3

typedef int nq_pixel[5];

typedef struct {
	/* biased by 10 bits */
	int alphadec;

	/* lengthcount = H*W*3 */
	int lengthcount;

	/* sampling factor 1..30 */
	int samplefac;

	/* Number of colours to use. Made a global instead of #define */
	int netsize;

	/* for network lookup - really 256 */
	int netindex[256];

	/* ABGRc */
	/* the network itself */
	nq_pixel network[MAXNETSIZE];

	/* bias and freq arrays for learning */
	int bias[MAXNETSIZE];
	int freq[MAXNETSIZE];

	/* radpower for precomputation */
	int radpower[initrad];

	/* the input image itself */
	unsigned char *thepicture;
} nn_quant;

/* Initialise network in range (0,0,0,0) to (255,255,255,255) and set parameters
   ----------------------------------------------------------------------- */
static void initnet(nn_quant *nnq, unsigned char *thepic, int len, int sample, int colours)
{
	register int i;
	register int *p;

	/* Clear out network from previous runs */
	/* thanks to Chen Bin for this fix */
	memset((void*)nnq->network, 0, sizeof(nq_pixel)*MAXNETSIZE);

	nnq->thepicture = thepic;
	nnq->lengthcount = len;
	nnq->samplefac = sample;
	nnq->netsize = colours;

	for (i=0; i < nnq->netsize; i++) {
		p = nnq->network[i];
		p[0] = p[1] = p[2] = p[3] = (i << (netbiasshift+8)) / nnq->netsize;
		nnq->freq[i] = intbias / nnq->netsize;	/* 1/netsize */
		nnq->bias[i] = 0;
	}
}

/* -------------------------- */

/* Unbias network to give byte values 0..255 and record
 * position i to prepare for sort
 */
/* -------------------------- */

static void unbiasnet(nn_quant *nnq)
{
	int i,j,temp;

	for (i=0; i < nnq->netsize; i++) {
		for (j=0; j<4; j++) {
			/* OLD CODE: network[i][j] >>= netbiasshift; */
			/* Fix based on bug report by Juergen Weigert jw@suse.de */
			temp = (nnq->network[i][j] + (1 << (netbiasshift - 1))) >> netbiasshift;
			if (temp > 255) temp = 255;
			nnq->network[i][j] = temp;
		}
		nnq->network[i][4] = i;			/* record colour no */
	}
}

/* Output colormap to unsigned char ptr in RGBA format */
static void getcolormap(nn_quant *nnq, unsigned char *map)
{
	int i,j;
	for(j=0; j < nnq->netsize; j++) {
		for (i=3; i>=0; i--) {
			*map = nnq->network[j][i];
			map++;
		}
	}
}

/* Insertion sort of network and building of netindex[0..255] (to do after unbias)
   ------------------------------------------------------------------------------- */
static void inxbuild(nn_quant *nnq)
{
	register int i,j,smallpos,smallval;
	register int *p,*q;
	int previouscol,startpos;

	previouscol = 0;
	startpos = 0;
	for (i=0; i < nnq->netsize; i++) {
		p = nnq->network[i];
		smallpos = i;
		smallval = p[2];			/* index on g */
		/* find smallest in i..netsize-1 */
		for (j=i+1; j < nnq->netsize; j++) {
			q = nnq->network[j];
			if (q[2] < smallval) {		/* index on g */
				smallpos = j;
				smallval = q[2];	/* index on g */
			}
		}
		q = nnq->network[smallpos];
		/* swap p (i) and q (smallpos) entries */
		if (i != smallpos) {
			j = q[0];
			q[0] = p[0];
			p[0] = j;
			j = q[1];
			q[1] = p[1];
			p[1] = j;
			j = q[2];
			q[2] = p[2];
			p[2] = j;
			j = q[3];
			q[3] = p[3];
			p[3] = j;
			j = q[4];
			q[4] = p[4];
			p[4] = j;
		}
		/* smallval entry is now in position i */
		if (smallval != previouscol) {
			nnq->netindex[previouscol] = (startpos+i)>>1;
			for (j=previouscol+1; j<smallval; j++) nnq->netindex[j] = i;
			previouscol = smallval;
			startpos = i;
		}
	}
	nnq->netindex[previouscol] = (startpos+maxnetpos)>>1;
	for (j=previouscol+1; j<256; j++) nnq->netindex[j] = maxnetpos; /* really 256 */
}


/* Search for ABGR values 0..255 (after net is unbiased) and return colour index
	 ---------------------------------------------------------------------------- */
static unsigned int inxsearch(nn_quant *nnq, int al, int b, int g, int r)
{
	register int i, j, dist, a, bestd;
	register int *p;
	unsigned int best;

	bestd = 1000;		/* biggest possible dist is 256*3 */
	best = 0;
	i = nnq->netindex[g];	/* index on g */
	j = i-1;		/* start at netindex[g] and work outwards */

	while ((i<nnq->netsize) || (j>=0)) {
		if (i< nnq->netsize) {
			p = nnq->network[i];
			dist = p[2] - g;		/* inx key */
			if (dist >= bestd) i = nnq->netsize;	/* stop iter */
			else {
				i++;
				if (dist<0) dist = -dist;
				a = p[1] - b;
				if (a<0) a = -a;
				dist += a;
				if (dist<bestd) {
					a = p[3] - r;
					if (a<0) a = -a;
					dist += a;
				}
				if(dist<bestd) {
					a = p[0] - al;
					if (a<0) a = -a;
					dist += a;
				}
				if (dist<bestd) {
					bestd=dist;
					best=p[4];
				}
			}
		}

		if (j>=0) {
			p = nnq->network[j];
			dist = g - p[2]; /* inx key - reverse dif */
			if (dist >= bestd) j = -1; /* stop iter */
			else {
				j--;
				if (dist<0) dist = -dist;
				a = p[1] - b;
				if (a<0) a = -a;
				dist += a;
				if (dist<bestd) {
					a = p[3] - r;
					if (a<0) a = -a;
					dist += a;
				}
				if(dist<bestd) {
					a = p[0] - al;
					if (a<0) a = -a;
					dist += a;
				}
				if (dist<bestd) {
					bestd=dist;
					best=p[4];
				}
			}
		}
	}

	return(best);
}

/* Search for biased ABGR values
   ---------------------------- */
static int contest(nn_quant *nnq, int al, int b, int g, int r)
{
	/* finds closest neuron (min dist) and updates freq */
	/* finds best neuron (min dist-bias) and returns position */
	/* for frequently chosen neurons, freq[i] is high and bias[i] is negative */
	/* bias[i] = gamma*((1/netsize)-freq[i]) */

	register int i,dist,a,biasdist,betafreq;
	unsigned int bestpos,bestbiaspos;
	double bestd,bestbiasd;
	register int *p,*f, *n;

	bestd = ~(((int) 1)<<31);
	bestbiasd = bestd;
	bestpos = 0;
	bestbiaspos = bestpos;
	p = nnq->bias;
	f = nnq->freq;

	for (i=0; i< nnq->netsize; i++) {
		n = nnq->network[i];
		dist = n[0] - al;
		if (dist<0) dist = -dist;
		a = n[1] - b;
		if (a<0) a = -a;
		dist += a;
		a = n[2] - g;
		if (a<0) a = -a;
		dist += a;
		a = n[3] - r;
		if (a<0) a = -a;
		dist += a;
		if (dist<bestd) {
			bestd=dist;
			bestpos=i;
		}
		biasdist = dist - ((*p)>>(intbiasshift-netbiasshift));
		if (biasdist<bestbiasd) {
			bestbiasd=biasdist;
			bestbiaspos=i;
		}
		betafreq = (*f >> betashift);
		*f++ -= betafreq;
		*p++ += (betafreq<<gammashift);
	}
	nnq->freq[bestpos] += beta;
	nnq->bias[bestpos] -= betagamma;
	return(bestbiaspos);
}


/* Move neuron i towards biased (a,b,g,r) by factor alpha
	 ---------------------------------------------------- */

static void altersingle(nn_quant *nnq, int alpha, int i, int al, int b, int g, int r)
{
	register int *n;

	n = nnq->network[i];	/* alter hit neuron */
	*n -= (alpha*(*n - al)) / initalpha;
	n++;
	*n -= (alpha*(*n - b)) / initalpha;
	n++;
	*n -= (alpha*(*n - g)) / initalpha;
	n++;
	*n -= (alpha*(*n - r)) / initalpha;
}


/* Move adjacent neurons by precomputed alpha*(1-((i-j)^2/[r]^2)) in radpower[|i-j|]
	 --------------------------------------------------------------------------------- */

static void alterneigh(nn_quant *nnq, int rad, int i, int al, int b,int g, int r)
{
	register int j,k,lo,hi,a;
	register int *p, *q;

	lo = i-rad;
	if (lo<-1) lo=-1;
	hi = i+rad;
	if (hi>nnq->netsize) hi=nnq->netsize;

	j = i+1;
	k = i-1;
	q = nnq->radpower;
	while ((j<hi) || (k>lo)) {
		a = (*(++q));
		if (j<hi) {
			p = nnq->network[j];
			*p -= (a*(*p - al)) / alpharadbias;
			p++;
			*p -= (a*(*p - b)) / alpharadbias;
			p++;
			*p -= (a*(*p - g)) / alpharadbias;
			p++;
			*p -= (a*(*p - r)) / alpharadbias;
			j++;
		}
		if (k>lo) {
			p = nnq->network[k];
			*p -= (a*(*p - al)) / alpharadbias;
			p++;
			*p -= (a*(*p - b)) / alpharadbias;
			p++;
			*p -= (a*(*p - g)) / alpharadbias;
			p++;
			*p -= (a*(*p - r)) / alpharadbias;
			k--;
		}
	}
}


/* Main Learning Loop
   ------------------ */

static void learn(nn_quant *nnq, int verbose) /* Stu: N.B. added parameter so that main() could control verbosity. */
{
	register int i,j,al,b,g,r;
	int radius,rad,alpha,step,delta,samplepixels;
	register unsigned char *p;
	unsigned char *lim;

	nnq->alphadec = 30 + ((nnq->samplefac-1)/3);
	p = nnq->thepicture;
	lim = nnq->thepicture + nnq->lengthcount;
	samplepixels = nnq->lengthcount/(4 * nnq->samplefac);
	/* here's a problem with small images: samplepixels < ncycles => delta = 0 */
	delta = samplepixels/ncycles;
	/* kludge to fix */
	if(delta==0) delta = 1;
	alpha = initalpha;
	radius = initradius;

	rad = radius >> radiusbiasshift;

	for (i=0; i<rad; i++)
		nnq->radpower[i] = alpha*(((rad*rad - i*i)*radbias)/(rad*rad));

	if (verbose) gd_error_ex(GD_NOTICE, "beginning 1D learning: initial radius=%d\n", rad);

	if ((nnq->lengthcount%prime1) != 0) step = 4*prime1;
	else {
		if ((nnq->lengthcount%prime2) !=0) step = 4*prime2;
		else {
			if ((nnq->lengthcount%prime3) !=0) step = 4*prime3;
			else step = 4*prime4;
		}
	}

	i = 0;
	while (i < samplepixels) {
		al = p[ALPHA] << netbiasshift;
		b = p[BLUE] << netbiasshift;
		g = p[GREEN] << netbiasshift;
		r = p[RED] << netbiasshift;
		j = contest(nnq, al,b,g,r);

		altersingle(nnq, alpha,j,al,b,g,r);
		if (rad) alterneigh(nnq, rad,j,al,b,g,r);   /* alter neighbours */

		p += step;
		while (p >= lim) p -= nnq->lengthcount;

		i++;
		if (i%delta == 0) {                    /* FPE here if delta=0*/
			alpha -= alpha / nnq->alphadec;
			radius -= radius / radiusdec;
			rad = radius >> radiusbiasshift;
			if (rad <= 1) rad = 0;
			for (j=0; j<rad; j++)
				nnq->radpower[j] = alpha*(((rad*rad - j*j)*radbias)/(rad*rad));
		}
	}
	if (verbose) gd_error_ex(GD_NOTICE, "finished 1D learning: final alpha=%f !\n",((float)alpha)/initalpha);
}

/**
 * Function: gdImageNeuQuant
 *
 * Creates a new palette image from a truecolor image
 *
 * This is the same as calling <gdImageCreatePaletteFromTrueColor> with the
 * quantization method <GD_QUANT_NEUQUANT>.
 *
 * Parameters:
 *   im            - The image.
 *   max_color     - The number of desired palette entries.
 *   sample_factor - The quantization precision between 1 (highest quality) and
 *                   10 (fastest).
 *
 * Returns:
 *   A newly create palette image; NULL on failure.
 */
gdImagePtr gdImageNeuQuant(gdImagePtr im, const int max_color, int sample_factor)
{
	const int newcolors = max_color;
	const int verbose = 1;

	int bot_idx, top_idx; /* for remapping of indices */
	int remap[MAXNETSIZE];
	int i,x;

	unsigned char map[MAXNETSIZE][4];
	unsigned char *d;

	nn_quant *nnq = NULL;

	int row;
	unsigned char *rgba = NULL;
	gdImagePtr dst = NULL;

	/* Default it to 3 */
	if (sample_factor < 1) {
		sample_factor = 3;
	}
	/* Start neuquant */
	/* Pierre:
	 * This implementation works with aligned contiguous buffer only
	 * Upcoming new buffers are contiguous and will be much faster.
	 * let don't bloat this code to support our good "old" 31bit format.
	 * It also lets us convert palette image, if one likes to reduce
	 * a palette
	 */
	if (overflow2(gdImageSX(im), gdImageSY(im))
	        || overflow2(gdImageSX(im) * gdImageSY(im), 4)) {
		goto done;
	}
	rgba = (unsigned char *) gdMalloc(gdImageSX(im) * gdImageSY(im) * 4);
	if (!rgba) {
		goto done;
	}

	d = rgba;
	for (row = 0; row < gdImageSY(im); row++) {
		int *p = im->tpixels[row];
		register int c;

		for (i = 0; i < gdImageSX(im); i++) {
			c = *p;
			*d++ = gdImageAlpha(im, c);
			*d++ = gdImageRed(im, c);
			*d++ = gdImageBlue(im, c);
			*d++ = gdImageGreen(im, c);
			p++;
		}
	}

	nnq = (nn_quant *) gdMalloc(sizeof(nn_quant));
	if (!nnq) {
		goto done;
	}

	initnet(nnq, rgba, gdImageSY(im) * gdImageSX(im) * 4, sample_factor, newcolors);

	learn(nnq, verbose);
	unbiasnet(nnq);
	getcolormap(nnq, (unsigned char*)map);
	inxbuild(nnq);
	/* remapping colormap to eliminate opaque tRNS-chunk entries... */
	for (top_idx = newcolors-1, bot_idx = x = 0;  x < newcolors;  ++x) {
		if (map[x][3] == 255) { /* maxval */
			remap[x] = top_idx--;
		} else {
			remap[x] = bot_idx++;
		}
	}
	if (bot_idx != top_idx + 1) {
		gd_error("  internal logic error: remapped bot_idx = %d, top_idx = %d\n",
			 bot_idx, top_idx);
		goto done;
	}

	dst = gdImageCreate(gdImageSX(im), gdImageSY(im));
	if (!dst) {
		goto done;
	}

	for (x = 0; x < newcolors; ++x) {
		dst->red[remap[x]] = map[x][0];
		dst->green[remap[x]] = map[x][1];
		dst->blue[remap[x]] = map[x][2];
		dst->alpha[remap[x]] = map[x][3];
		dst->open[remap[x]] = 0;
		dst->colorsTotal++;
	}

	/* Do each image row */
	for ( row = 0; row < gdImageSY(im); ++row ) {
		int offset;
		unsigned char *p = dst->pixels[row];

		/* Assign the new colors */
		offset = row * gdImageSX(im) * 4;
		for(i=0; i < gdImageSX(im); i++) {
			p[i] = remap[
			           inxsearch(nnq, rgba[i * 4 + offset + ALPHA],
			                     rgba[i * 4 + offset + BLUE],
			                     rgba[i * 4 + offset + GREEN],
			                     rgba[i * 4 + offset + RED])
			       ];
		}
	}

done:
	if (rgba) {
		gdFree(rgba);
	}

	if (nnq) {
		gdFree(nnq);
	}
	return dst;
}
