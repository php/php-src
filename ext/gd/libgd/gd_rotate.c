#if HAVE_GD_BUNDLED
# include "gd.h"
#else
# include <gd.h>
#endif

#include "gd_intern.h"
#include <math.h>

/*
 * Rotate function Added on 2003/12
 * by Pierre-Alain Joye (pierre@php.net)
 **/
/* Begin rotate function */
#ifdef ROTATE_PI
#undef ROTATE_PI
#endif /* ROTATE_PI */

#define ROTATE_DEG2RAD  3.1415926535897932384626433832795/180
void gdImageSkewX (gdImagePtr dst, gdImagePtr src, int uRow, int iOffset, double dWeight, int clrBack, int ignoretransparent)
{
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	int i, r, g, b, a, clrBackR, clrBackG, clrBackB, clrBackA;
	FuncPtr f;

	int pxlOldLeft, pxlLeft=0, pxlSrc;

	/* Keep clrBack as color index if required */
	if (src->trueColor) {
		pxlOldLeft = clrBack;
		f = gdImageGetTrueColorPixel;
	} else {
		pxlOldLeft = clrBack;
		clrBackR = gdImageRed(src, clrBack);
		clrBackG = gdImageGreen(src, clrBack);
		clrBackB = gdImageBlue(src, clrBack);
		clrBackA = gdImageAlpha(src, clrBack);
		clrBack =  gdTrueColorAlpha(clrBackR, clrBackG, clrBackB, clrBackA);
		f = gdImageGetPixel;
	}

	for (i = 0; i < iOffset; i++) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}

	if (i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}

	for (i = 0; i < src->sx; i++) {
		pxlSrc = f (src,i,uRow);

		r = (int)(gdImageRed(src,pxlSrc) * dWeight);
		g = (int)(gdImageGreen(src,pxlSrc) * dWeight);
		b = (int)(gdImageBlue(src,pxlSrc) * dWeight);
		a = (int)(gdImageAlpha(src,pxlSrc) * dWeight);

		pxlLeft = gdImageColorAllocateAlpha(src, r, g, b, a);

		if (pxlLeft == -1) {
			pxlLeft = gdImageColorClosestAlpha(src, r, g, b, a);
		}

		r = gdImageRed(src,pxlSrc) - (gdImageRed(src,pxlLeft) - gdImageRed(src,pxlOldLeft));
		g = gdImageGreen(src,pxlSrc) - (gdImageGreen(src,pxlLeft) - gdImageGreen(src,pxlOldLeft));
		b = gdImageBlue(src,pxlSrc) - (gdImageBlue(src,pxlLeft) - gdImageBlue(src,pxlOldLeft));
		a = gdImageAlpha(src,pxlSrc) - (gdImageAlpha(src,pxlLeft) - gdImageAlpha(src,pxlOldLeft));

        if (r>255) {
        	r = 255;
        }

		if (g>255) {
			g = 255;
		}

		if (b>255) {
			b = 255;
		}

		if (a>127) {
			a = 127;
		}

		if (ignoretransparent && pxlSrc == dst->transparent) {
			pxlSrc = dst->transparent;
		} else {
			pxlSrc = gdImageColorAllocateAlpha(dst, r, g, b, a);

			if (pxlSrc == -1) {
				pxlSrc = gdImageColorClosestAlpha(dst, r, g, b, a);
			}
		}

		if ((i + iOffset >= 0) && (i + iOffset < dst->sx)) {
			gdImageSetPixel (dst, i+iOffset, uRow,  pxlSrc);
		}

		pxlOldLeft = pxlLeft;
	}

	i += iOffset;

	if (i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, pxlLeft);
	}

	gdImageSetPixel (dst, iOffset, uRow, clrBack);

	i--;

	while (++i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}
}

void gdImageSkewY (gdImagePtr dst, gdImagePtr src, int uCol, int iOffset, double dWeight, int clrBack, int ignoretransparent)
{
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	int i, iYPos=0, r, g, b, a;
	FuncPtr f;
	int pxlOldLeft, pxlLeft=0, pxlSrc;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}

	for (i = 0; i<=iOffset; i++) {
		gdImageSetPixel (dst, uCol, i, clrBack);
	}
	r = (int)((double)gdImageRed(src,clrBack) * dWeight);
	g = (int)((double)gdImageGreen(src,clrBack) * dWeight);
	b = (int)((double)gdImageBlue(src,clrBack) * dWeight);
	a = (int)((double)gdImageAlpha(src,clrBack) * dWeight);

	pxlOldLeft = gdImageColorAllocateAlpha(dst, r, g, b, a);

	for (i = 0; i < src->sy; i++) {
		pxlSrc = f (src, uCol, i);
		iYPos = i + iOffset;

		r = (int)((double)gdImageRed(src,pxlSrc) * dWeight);
		g = (int)((double)gdImageGreen(src,pxlSrc) * dWeight);
		b = (int)((double)gdImageBlue(src,pxlSrc) * dWeight);
		a = (int)((double)gdImageAlpha(src,pxlSrc) * dWeight);

		pxlLeft = gdImageColorAllocateAlpha(src, r, g, b, a);

		if (pxlLeft == -1) {
			pxlLeft = gdImageColorClosestAlpha(src, r, g, b, a);
		}

		r = gdImageRed(src,pxlSrc) - (gdImageRed(src,pxlLeft) - gdImageRed(src,pxlOldLeft));
		g = gdImageGreen(src,pxlSrc) - (gdImageGreen(src,pxlLeft) - gdImageGreen(src,pxlOldLeft));
		b = gdImageBlue(src,pxlSrc) - (gdImageBlue(src,pxlLeft) - gdImageBlue(src,pxlOldLeft));
		a = gdImageAlpha(src,pxlSrc) - (gdImageAlpha(src,pxlLeft) - gdImageAlpha(src,pxlOldLeft));

		if (r>255) {
        		r = 255;
		}

		if (g>255) {
			g = 255;
		}

		if (b>255) {
    			b = 255;
		}

		if (a>127) {
			a = 127;
		}

		if (ignoretransparent && pxlSrc == dst->transparent) {
			pxlSrc = dst->transparent;
		} else {
			pxlSrc = gdImageColorAllocateAlpha(dst, r, g, b, a);

			if (pxlSrc == -1) {
				pxlSrc = gdImageColorClosestAlpha(dst, r, g, b, a);
			}
		}

		if ((iYPos >= 0) && (iYPos < dst->sy)) {
			gdImageSetPixel (dst, uCol, iYPos, pxlSrc);
		}

		pxlOldLeft = pxlLeft;
	}

	i = iYPos;
	if (i < dst->sy) {
		gdImageSetPixel (dst, uCol, i, pxlLeft);
	}

	i--;
	while (++i < dst->sy) {
		gdImageSetPixel (dst, uCol, i, clrBack);
	}
}

/* Rotates an image by 90 degrees (counter clockwise) */
gdImagePtr gdImageRotate90 (gdImagePtr src, int ignoretransparent)
{
	int uY, uX;
	int c,r,g,b,a;
	gdImagePtr dst;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}
	dst = gdImageCreateTrueColor(src->sy, src->sx);

	if (dst != NULL) {
		int old_blendmode = dst->alphaBlendingFlag;
		dst->alphaBlendingFlag = 0;

		dst->transparent = src->transparent;

		gdImagePaletteCopy (dst, src);

		for (uY = 0; uY<src->sy; uY++) {
			for (uX = 0; uX<src->sx; uX++) {
				c = f (src, uX, uY);
				if (!src->trueColor) {
					r = gdImageRed(src,c);
					g = gdImageGreen(src,c);
					b = gdImageBlue(src,c);
					a = gdImageAlpha(src,c);
					c = gdTrueColorAlpha(r, g, b, a);
				}
				if (ignoretransparent && c == dst->transparent) {
					gdImageSetPixel(dst, uY, (dst->sy - uX - 1), dst->transparent);
				} else {
					gdImageSetPixel(dst, uY, (dst->sy - uX - 1), c);
				}
			}
		}
		dst->alphaBlendingFlag = old_blendmode;
	}

	return dst;
}

/* Rotates an image by 180 degrees (counter clockwise) */
gdImagePtr gdImageRotate180 (gdImagePtr src, int ignoretransparent)
{
	int uY, uX;
	int c,r,g,b,a;
	gdImagePtr dst;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}
	dst = gdImageCreateTrueColor(src->sx, src->sy);

	if (dst != NULL) {
		int old_blendmode = dst->alphaBlendingFlag;
		dst->alphaBlendingFlag = 0;

		dst->transparent = src->transparent;

		gdImagePaletteCopy (dst, src);

		for (uY = 0; uY<src->sy; uY++) {
			for (uX = 0; uX<src->sx; uX++) {
				c = f (src, uX, uY);
				if (!src->trueColor) {
					r = gdImageRed(src,c);
					g = gdImageGreen(src,c);
					b = gdImageBlue(src,c);
					a = gdImageAlpha(src,c);
					c = gdTrueColorAlpha(r, g, b, a);
				}

				if (ignoretransparent && c == dst->transparent) {
					gdImageSetPixel(dst, (dst->sx - uX - 1), (dst->sy - uY - 1), dst->transparent);
				} else {
					gdImageSetPixel(dst, (dst->sx - uX - 1), (dst->sy - uY - 1), c);
				}
			}
		}
		dst->alphaBlendingFlag = old_blendmode;
	}

	return dst;
}

/* Rotates an image by 270 degrees (counter clockwise) */
gdImagePtr gdImageRotate270 (gdImagePtr src, int ignoretransparent)
{
	int uY, uX;
	int c,r,g,b,a;
	gdImagePtr dst;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}
	dst = gdImageCreateTrueColor (src->sy, src->sx);

	if (dst != NULL) {
		int old_blendmode = dst->alphaBlendingFlag;
		dst->alphaBlendingFlag = 0;

		dst->transparent = src->transparent;

		gdImagePaletteCopy (dst, src);

		for (uY = 0; uY<src->sy; uY++) {
			for (uX = 0; uX<src->sx; uX++) {
				c = f (src, uX, uY);
				if (!src->trueColor) {
					r = gdImageRed(src,c);
					g = gdImageGreen(src,c);
					b = gdImageBlue(src,c);
					a = gdImageAlpha(src,c);
					c = gdTrueColorAlpha(r, g, b, a);
				}

				if (ignoretransparent && c == dst->transparent) {
					gdImageSetPixel(dst, (dst->sx - uY - 1), uX, dst->transparent);
				} else {
					gdImageSetPixel(dst, (dst->sx - uY - 1), uX, c);
				}
			}
		}
		dst->alphaBlendingFlag = old_blendmode;
	}

	return dst;
}

gdImagePtr gdImageRotate45 (gdImagePtr src, double dAngle, int clrBack, int ignoretransparent)
{
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	gdImagePtr dst1,dst2,dst3;
	FuncPtr f;
	double dRadAngle, dSinE, dTan, dShear;
	double dOffset;     /* Variable skew offset */
	int u, iShear, newx, newy;
	int clrBackR, clrBackG, clrBackB, clrBackA;

	/* See GEMS I for the algorithm details */
	dRadAngle = dAngle * ROTATE_DEG2RAD; /* Angle in radians */
	dSinE = sin (dRadAngle);
	dTan = tan (dRadAngle / 2.0);

	newx = (int)(src->sx + src->sy * fabs(dTan));
	newy = src->sy;

	/* 1st shear */
	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}

	dst1 = gdImageCreateTrueColor(newx, newy);
	/******* Perform 1st shear (horizontal) ******/
	if (dst1 == NULL) {
		return NULL;
	}
#ifdef HAVE_GD_BUNDLED
	dst1->alphaBlendingFlag = gdEffectReplace;
#else
	gdImageAlphaBlending(dst1, 0);
#endif
	if (dAngle == 0.0) {
		/* Returns copy of src */
		gdImageCopy (dst1, src,0,0,0,0,src->sx,src->sy);
		return dst1;
	}

	gdImagePaletteCopy (dst1, src);

	if (ignoretransparent) {
		if (gdImageTrueColor(src)) {
			dst1->transparent = src->transparent;
		} else {

			dst1->transparent = gdTrueColorAlpha(gdImageRed(src, src->transparent), gdImageBlue(src, src->transparent), gdImageGreen(src, src->transparent), 127);
		}
	}

	dRadAngle = dAngle * ROTATE_DEG2RAD; /* Angle in radians */
	dSinE = sin (dRadAngle);
	dTan = tan (dRadAngle / 2.0);

	for (u = 0; u < dst1->sy; u++) {
		if (dTan >= 0.0) {
			dShear = ((double)(u + 0.5)) * dTan;
		} else {
			dShear = ((double)(u - dst1->sy) + 0.5) * dTan;
		}

		iShear = (int)floor(dShear);
		gdImageSkewX(dst1, src, u, iShear, (dShear - iShear), clrBack, ignoretransparent);
	}

	/*
	The 1st shear may use the original clrBack as color index
	Convert it once here
	*/
	if(!src->trueColor) {
		clrBackR = gdImageRed(src, clrBack);
		clrBackG = gdImageGreen(src, clrBack);
		clrBackB = gdImageBlue(src, clrBack);
		clrBackA = gdImageAlpha(src, clrBack);
		clrBack =  gdTrueColorAlpha(clrBackR, clrBackG, clrBackB, clrBackA);
	}
	/* 2nd shear */
	newx = dst1->sx;

	if (dSinE > 0.0) {
		dOffset = (src->sx-1) * dSinE;
	} else {
		dOffset = -dSinE *  (src->sx - newx);
	}

	newy = (int) ((double) src->sx * fabs( dSinE ) + (double) src->sy * cos (dRadAngle))+1;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}
	dst2 = gdImageCreateTrueColor(newx, newy);
	if (dst2 == NULL) {
		gdImageDestroy(dst1);
		return NULL;
	}

#ifdef HAVE_GD_BUNDLED
	dst2->alphaBlendingFlag = gdEffectReplace;
#else
	gdImageAlphaBlending(dst2, 0);
#endif

	if (ignoretransparent) {
		dst2->transparent = dst1->transparent;
	}

	for (u = 0; u < dst2->sx; u++, dOffset -= dSinE) {
		iShear = (int)floor (dOffset);
		gdImageSkewY(dst2, dst1, u, iShear, (dOffset - (double)iShear), clrBack, ignoretransparent);
	}

	/* 3rd shear */
	gdImageDestroy(dst1);

	newx = (int) ((double)src->sy * fabs (dSinE) + (double)src->sx * cos (dRadAngle)) + 1;
	newy = dst2->sy;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}
	dst3 = gdImageCreateTrueColor(newx, newy);
	if (dst3 == NULL) {
		gdImageDestroy(dst2);
		return NULL;
	}

#ifdef HAVE_GD_BUNDLED
	dst3->alphaBlendingFlag = gdEffectReplace;
#else
	gdImageAlphaBlending(dst3, 0);
#endif

	if (ignoretransparent) {
		dst3->transparent = dst2->transparent;
	}

	if (dSinE >= 0.0) {
		dOffset = (double)(src->sx - 1) * dSinE * -dTan;
	} else {
		dOffset = dTan * ((double)(src->sx - 1) * -dSinE + (double)(1 - newy));
	}

	for (u = 0; u < dst3->sy; u++, dOffset += dTan) {
		int iShear = (int)floor(dOffset);
		gdImageSkewX(dst3, dst2, u, iShear, (dOffset - iShear), clrBack, ignoretransparent);
	}

	gdImageDestroy(dst2);

	return dst3;
}

gdImagePtr gdImageRotate (gdImagePtr src, double dAngle, int clrBack, int ignoretransparent)
{
	gdImagePtr pMidImg;
	gdImagePtr rotatedImg;

	if (src == NULL) {
		return NULL;
	}

	if (!gdImageTrueColor(src) && (clrBack < 0 || clrBack>=gdImageColorsTotal(src))) {
		return NULL;
	}

	while (dAngle >= 360.0) {
		dAngle -= 360.0;
	}

	while (dAngle < 0) {
		dAngle += 360.0;
	}

	if (dAngle == 90.00) {
		return gdImageRotate90(src, ignoretransparent);
	}
	if (dAngle == 180.00) {
		return gdImageRotate180(src, ignoretransparent);
	}
	if(dAngle == 270.00) {
		return gdImageRotate270 (src, ignoretransparent);
	}

	if ((dAngle > 45.0) && (dAngle <= 135.0)) {
		pMidImg = gdImageRotate90 (src, ignoretransparent);
		dAngle -= 90.0;
	} else if ((dAngle > 135.0) && (dAngle <= 225.0)) {
		pMidImg = gdImageRotate180 (src, ignoretransparent);
		dAngle -= 180.0;
	} else if ((dAngle > 225.0) && (dAngle <= 315.0)) {
		pMidImg = gdImageRotate270 (src, ignoretransparent);
		dAngle -= 270.0;
	} else {
		return gdImageRotate45 (src, dAngle, clrBack, ignoretransparent);
	}

	if (pMidImg == NULL) {
		return NULL;
	}

	rotatedImg = gdImageRotate45 (pMidImg, dAngle, clrBack, ignoretransparent);
	gdImageDestroy(pMidImg);

	return rotatedImg;
}
/* End Rotate function */


