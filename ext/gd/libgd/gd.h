#ifndef GD_H
#define GD_H 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
/* default fontpath for unix systems */
#define DEFAULT_FONTPATH "/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1:."
#define PATHSEPARATOR ":"
#else
/* default fontpath for windows systems */
#define DEFAULT_FONTPATH "c:\\winnt\\fonts;."
#define PATHSEPARATOR ";"
#endif

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied, 
 * including but not limited to implied warranties of merchantability and 
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* stdio is needed for file I/O. */
#include <stdio.h>
#include "gd_io.h"

void php_gd_error_ex(int type, const char *format, ...);

void php_gd_error(const char *format, ...);


/* The maximum number of palette entries in palette-based images.
	In the wonderful new world of gd 2.0, you can of course have
	many more colors when using truecolor mode. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for 
	read-only purposes. */

/* If 'truecolor' is set true, the image is truecolor; 
	pixels are represented by integers, which
	must be 32 bits wide or more. 

	True colors are repsented as follows:

	ARGB

	Where 'A' (alpha channel) occupies only the
	LOWER 7 BITS of the MSB. This very small 
	loss of alpha channel resolution allows gd 2.x
	to keep backwards compatibility by allowing
	signed integers to be used to represent colors,
	and negative numbers to represent special cases,
	just as in gd 1.x. */

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)
#define gdEffectReplace 0
#define gdEffectAlphaBlend 1
#define gdEffectNormal 2
#define gdEffectOverlay 3


/* This function accepts truecolor pixel values only. The 
	source color is composited with the destination color
	based on the alpha channel value of the source color.
	The resulting color is opaque. */

int gdAlphaBlend(int dest, int src);

typedef struct gdImageStruct {
	/* Palette-based image pixels */
	unsigned char ** pixels;
	int sx;
	int sy;
	/* These are valid in palette images only. See also
		'alpha', which appears later in the structure to
		preserve binary backwards compatibility */
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors]; 
	int open[gdMaxColors];
	/* For backwards compatibility, this is set to the
		first palette entry with 100% transparency,
		and is also set and reset by the 
		gdImageColorTransparent function. Newer
		applications can allocate palette entries
		with any desired level of transparency; however,
		bear in mind that many viewers, notably
		many web browsers, fail to implement
		full alpha channel for PNG and provide
		support for full opacity or transparency only. */
	int transparent;
	int *polyInts;
	int polyAllocated;
	struct gdImageStruct *brush;
	struct gdImageStruct *tile;	
	int brushColorMap[gdMaxColors];
	int tileColorMap[gdMaxColors];
	int styleLength;
	int stylePos;
	int *style;
	int interlace;
	/* New in 2.0: thickness of line. Initialized to 1. */
	int thick;
	/* New in 2.0: alpha channel for palettes. Note that only
		Macintosh Internet Explorer and (possibly) Netscape 6
		really support multiple levels of transparency in
		palettes, to my knowledge, as of 2/15/01. Most
		common browsers will display 100% opaque and
		100% transparent correctly, and do something 
		unpredictable and/or undesirable for levels
		in between. TBB */
	int alpha[gdMaxColors]; 
	/* Truecolor flag and pixels. New 2.0 fields appear here at the
		end to minimize breakage of existing object code. */
	int trueColor;
	int ** tpixels;
	/* Should alpha channel be copied, or applied, each time a
		pixel is drawn? This applies to truecolor images only.
		No attempt is made to alpha-blend in palette images,
		even if semitransparent palette entries exist. 
		To do that, build your image as a truecolor image,
		then quantize down to 8 bits. */
	int alphaBlendingFlag;
	/* Should antialias functions be used */
	int antialias;
	/* Should the alpha channel of the image be saved? This affects
		PNG at the moment; other future formats may also
		have that capability. JPEG doesn't. */
	int saveAlphaFlag;
} gdImage;

typedef gdImage * gdImagePtr;

typedef struct {
	/* # of characters in font */
	int nchars;
	/* First character is numbered... (usually 32 = space) */
	int offset;
	/* Character width and height */
	int w;
	int h;
	/* Font data; array of characters, one row after another.
		Easily included in code, also easily loaded from
		data files. */
	char *data;
} gdFont;

/* Text functions take these. */
typedef gdFont *gdFontPtr;

/* For backwards compatibility only. Use gdImageSetStyle()
	for MUCH more flexible line drawing. Also see
	gdImageSetBrush(). */
#define gdDashSize 4

/* Special colors. */

#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
	This is used in line styles only. */
#define gdTransparent (-6)

/* Functions to manipulate images. */

/* Creates a palette-based image (up to 256 colors). */
gdImagePtr gdImageCreate(int sx, int sy);

/* An alternate name for the above (2.0). */
#define gdImageCreatePalette gdImageCreate

/* Creates a truecolor image (millions of colors). */
gdImagePtr gdImageCreateTrueColor(int sx, int sy);

/* Creates an image from various file types. These functions
	return a palette or truecolor image based on the
	nature of the file being loaded. Truecolor PNG
	stays truecolor; palette PNG stays palette-based;
	JPEG is always truecolor. */
gdImagePtr gdImageCreateFromPng(FILE *fd);
gdImagePtr gdImageCreateFromPngCtx(gdIOCtxPtr in);
gdImagePtr gdImageCreateFromWBMP(FILE *inFile);
gdImagePtr gdImageCreateFromWBMPCtx(gdIOCtx *infile); 
gdImagePtr gdImageCreateFromJpeg(FILE *infile);
gdImagePtr gdImageCreateFromJpegCtx(gdIOCtx *infile);

/* A custom data source. */
/* The source function must return -1 on error, otherwise the number
        of bytes fetched. 0 is EOF, not an error! */
/* context will be passed to your source function. */

typedef struct {
        int (*source) (void *context, char *buffer, int len);
        void *context;
} gdSource, *gdSourcePtr;

gdImagePtr gdImageCreateFromPngSource(gdSourcePtr in);

gdImagePtr gdImageCreateFromGd(FILE *in);
gdImagePtr gdImageCreateFromGdCtx(gdIOCtxPtr in);

gdImagePtr gdImageCreateFromGd2(FILE *in);
gdImagePtr gdImageCreateFromGd2Ctx(gdIOCtxPtr in);

gdImagePtr gdImageCreateFromGd2Part(FILE *in, int srcx, int srcy, int w, int h);
gdImagePtr gdImageCreateFromGd2PartCtx(gdIOCtxPtr in, int srcx, int srcy, int w, int h);

gdImagePtr gdImageCreateFromXbm(FILE *fd);

gdImagePtr gdImageCreateFromXpm (char *filename);

void gdImageDestroy(gdImagePtr im);

/* Replaces or blends with the background depending on the
	most recent call to gdImageAlphaBlending and the
	alpha channel value of 'color'; default is to overwrite. 
	Tiling and line styling are also implemented
	here. All other gd drawing functions pass through this call, 
	allowing for many useful effects. */
	
void gdImageSetPixel(gdImagePtr im, int x, int y, int color);

int gdImageGetPixel(gdImagePtr im, int x, int y);

void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageAALine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/* For backwards compatibility only. Use gdImageSetStyle()
	for much more flexible line drawing. */
void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Corners specified (not width and height). Upper left first, lower right
 	second. */
void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Solid bar. Upper left corner first, lower right corner second. */
void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageChar(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);
void gdImageCharUp(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);
void gdImageString(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);
void gdImageStringUp(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);
void gdImageString16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);
void gdImageStringUp16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);

/* clean up after using fonts in gdImageStringFT() */
void gdFreeFontCache();

/* Calls gdImageStringFT. Provided for backwards compatibility only. */
char *gdImageStringTTF(gdImage *im, int *brect, int fg, char *fontlist,
                double ptsize, double angle, int x, int y, char *string);

/* FreeType 2 text output */
char *gdImageStringFT(gdImage *im, int *brect, int fg, char *fontlist,
                double ptsize, double angle, int x, int y, char *string);

typedef struct {
	int flags; /* for future expansion logical OR of gdFTEX_ values */
	double linespacing; /* fine tune line spacing for '\n' */
} gdFTStringExtra, *gdFTStringExtraPtr;
#define gdFTEX_LINESPACE 1

/* FreeType 2 text output with fine tuning */
char *
gdImageStringFTEx(gdImage * im, int *brect, int fg, char * fontlist,
		double ptsize, double angle, int x, int y, char * string,
		gdFTStringExtraPtr strex);


/* Point type for use in polygon drawing. */
typedef struct {
	int x, y;
} gdPoint, *gdPointPtr;

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);

/* These functions still work with truecolor images, 
	for which they never return error. */
int gdImageColorAllocate(gdImagePtr im, int r, int g, int b);
/* gd 2.0: palette entries with non-opaque transparency are permitted. */
int gdImageColorAllocateAlpha(gdImagePtr im, int r, int g, int b, int a);
/* Assumes opaque is the preferred alpha channel value */
int gdImageColorClosest(gdImagePtr im, int r, int g, int b);
/* Closest match taking all four parameters into account.
	A slightly different color with the same transparency
	beats the exact same color with radically different
	transparency */
int gdImageColorClosestAlpha(gdImagePtr im, int r, int g, int b, int a);
/* An alternate method */
int gdImageColorClosestHWB(gdImagePtr im, int r, int g, int b);
/* Returns exact, 100% opaque matches only */
int gdImageColorExact(gdImagePtr im, int r, int g, int b);
/* Returns an exact match only, including alpha */
int gdImageColorExactAlpha(gdImagePtr im, int r, int g, int b, int a);
/* Opaque only */
int gdImageColorResolve(gdImagePtr im, int r, int g, int b);
/* Based on gdImageColorExactAlpha and gdImageColorClosestAlpha */
int gdImageColorResolveAlpha(gdImagePtr im, int r, int g, int b, int a);

/* A simpler way to obtain an opaque truecolor value for drawing on a
	truecolor image. Not for use with palette images! */

#define gdTrueColor(r, g, b) (((r) << 16) + \
	((g) << 8) + \
	(b))

/* Returns a truecolor value with an alpha channel component.
	gdAlphaMax (127, **NOT 255**) is transparent, 0 is completely
	opaque. */

#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + \
	((r) << 16) + \
	((g) << 8) + \
	(b))

void gdImageColorDeallocate(gdImagePtr im, int color);

/* Converts a truecolor image to a palette-based image,
	using a high-quality two-pass quantization routine
	which attempts to preserve alpha channel information
	as well as R/G/B color information when creating
	a palette. If ditherFlag is set, the image will be
	dithered to approximate colors better, at the expense
	of some obvious "speckling." colorsWanted can be
	anything up to 256. If the original source image
	includes photographic information or anything that
	came out of a JPEG, 256 is strongly recommended.

	Better yet, don't use this function -- write real
	truecolor PNGs and JPEGs. The disk space gain of
        conversion to palette is not great (for small images
        it can be negative) and the quality loss is ugly. */

void gdImageTrueColorToPalette(gdImagePtr im, int ditherFlag, int colorsWanted);

/* An attempt at getting the results of gdImageTrueColorToPalette
	to look a bit more like the original (im1 is the original
	and im2 is the palette version */
int gdImageColorMatch(gdImagePtr im1, gdImagePtr im2);

/* Specifies a color index (if a palette image) or an
	RGB color (if a truecolor image) which should be
	considered 100% transparent. FOR TRUECOLOR IMAGES,
	THIS IS IGNORED IF AN ALPHA CHANNEL IS BEING
	SAVED. Use gdImageSaveAlpha(im, 0); to
	turn off the saving of a full alpha channel in
	a truecolor image. Note that gdImageColorTransparent
	is usually compatible with older browsers that
	do not understand full alpha channels well. TBB */
void gdImageColorTransparent(gdImagePtr im, int color);

void gdImagePaletteCopy(gdImagePtr dst, gdImagePtr src);
void gdImagePng(gdImagePtr im, FILE *out);
void gdImagePngCtx(gdImagePtr im, gdIOCtx *out);

void gdImageWBMP(gdImagePtr image, int fg, FILE *out);
void gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out);

/* Guaranteed to correctly free memory returned
	by the gdImage*Ptr functions */
void gdFree(void *m);

/* Best to free this memory with gdFree(), not free() */
void *gdImageWBMPPtr(gdImagePtr im, int *size, int fg);

/* 100 is highest quality (there is always a little loss with JPEG).
       0 is lowest. 10 is about the lowest useful setting. */
void gdImageJpeg(gdImagePtr im, FILE *out, int quality);
void gdImageJpegCtx(gdImagePtr im, gdIOCtx *out, int quality);

/* Best to free this memory with gdFree(), not free() */
void *gdImageJpegPtr(gdImagePtr im, int *size, int quality);

gdImagePtr gdImageCreateFromGif(FILE *fd);
gdImagePtr gdImageCreateFromGifCtx(gdIOCtxPtr in);
gdImagePtr gdImageCreateFromGifSource(gdSourcePtr in);

/* A custom data sink. For backwards compatibility. Use
	gdIOCtx instead. */
/* The sink function must return -1 on error, otherwise the number
        of bytes written, which must be equal to len. */
/* context will be passed to your sink function. */
typedef struct {
        int (*sink) (void *context, const char *buffer, int len);
        void *context;
} gdSink, *gdSinkPtr;

void gdImagePngToSink(gdImagePtr im, gdSinkPtr out);

void gdImageGd(gdImagePtr im, FILE *out);
void gdImageGd2(gdImagePtr im, FILE *out, int cs, int fmt);

/* Best to free this memory with gdFree(), not free() */
void* gdImagePngPtr(gdImagePtr im, int *size);

/* Best to free this memory with gdFree(), not free() */
void* gdImageGdPtr(gdImagePtr im, int *size);

/* Best to free this memory with gdFree(), not free() */
void* gdImageGd2Ptr(gdImagePtr im, int cs, int fmt, int *size);

void gdImageEllipse(gdImagePtr im, int cx, int cy, int w, int h, int c);

/* Style is a bitwise OR ( | operator ) of these.
	gdArc and gdChord are mutually exclusive;
	gdChord just connects the starting and ending
	angles with a straight line, while gdArc produces
	a rounded edge. gdPie is a synonym for gdArc. 
	gdNoFill indicates that the arc or chord should be
	outlined, not filled. gdEdged, used together with
	gdNoFill, indicates that the beginning and ending
	angles should be connected to the center; this is
	a good way to outline (rather than fill) a
	'pie slice'. */
#define gdArc   0
#define gdPie   gdArc
#define gdChord 1
#define gdNoFill 2
#define gdEdged 4

void gdImageFilledArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color, int style);
void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);
void gdImageFilledEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);
void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color);
void gdImageFill(gdImagePtr im, int x, int y, int color);
void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h);
void gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, 
			int srcX, int srcY, int w, int h, int pct);
void gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
                        int srcX, int srcY, int w, int h, int pct);

/* Stretches or shrinks to fit, as needed. Does NOT attempt
	to average the entire set of source pixels that scale down onto the
	destination pixel. */
void gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);

/* gd 2.0: stretches or shrinks to fit, as needed. When called with a
	truecolor destination image, this function averages the
	entire set of source pixels that scale down onto the
	destination pixel, taking into account what portion of the
	destination pixel each source pixel represents. This is a
	floating point operation, but this is not a performance issue
	on modern hardware, except for some embedded devices. If the 
	destination is a palette image, gdImageCopyResized is 
	substituted automatically. */
void gdImageCopyResampled(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);

gdImagePtr gdImageRotate90(gdImagePtr src);
gdImagePtr gdImageRotate180(gdImagePtr src);
gdImagePtr gdImageRotate270(gdImagePtr src);
gdImagePtr gdImageRotate45(gdImagePtr src, double dAngle, int clrBack);
gdImagePtr gdImageRotate (gdImagePtr src, double dAngle, int clrBack);

void gdImageSetBrush(gdImagePtr im, gdImagePtr brush);
void gdImageSetTile(gdImagePtr im, gdImagePtr tile);
void gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels);
/* Line thickness (defaults to 1). Affects lines, ellipses, 
	rectangles, polygons and so forth. */
void gdImageSetThickness(gdImagePtr im, int thickness);
/* On or off (1 or 0) for all three of these. */
void gdImageInterlace(gdImagePtr im, int interlaceArg);
void gdImageAlphaBlending(gdImagePtr im, int alphaBlendingArg);
void gdImageAntialias(gdImagePtr im, int antialias);
void gdImageSaveAlpha(gdImagePtr im, int saveAlphaArg);

/* Macros to access information about images. */

/* Returns nonzero if the image is a truecolor image,
	zero for a palette image. */

#define gdImageTrueColor(im) ((im)->trueColor)

#define gdImageSX(im) ((im)->sx)
#define gdImageSY(im) ((im)->sy)
#define gdImageColorsTotal(im) ((im)->colorsTotal)
#define gdImageRed(im, c) ((im)->trueColor ? gdTrueColorGetRed(c) : \
	(im)->red[(c)])
#define gdImageGreen(im, c) ((im)->trueColor ? gdTrueColorGetGreen(c) : \
	(im)->green[(c)])
#define gdImageBlue(im, c) ((im)->trueColor ? gdTrueColorGetBlue(c) : \
	(im)->blue[(c)])
#define gdImageAlpha(im, c) ((im)->trueColor ? gdTrueColorGetAlpha(c) : \
	(im)->alpha[(c)])
#define gdImageGetTransparent(im) ((im)->transparent)
#define gdImageGetInterlaced(im) ((im)->interlace)

/* These macros provide direct access to pixels in
	palette-based and truecolor images, respectively.
	If you use these macros, you must perform your own
	bounds checking. Use of the macro for the correct type
	of image is also your responsibility. */
#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]

/* I/O Support routines. */

gdIOCtx* gdNewFileCtx(FILE*);
gdIOCtx* gdNewDynamicCtx(int, void*);
gdIOCtx* gdNewSSCtx(gdSourcePtr in, gdSinkPtr out);
void* gdDPExtractData(struct gdIOCtx* ctx, int *size);

#define GD2_CHUNKSIZE           128 
#define GD2_CHUNKSIZE_MIN	64
#define GD2_CHUNKSIZE_MAX       4096

#define GD2_VERS                2
#define GD2_ID                  "gd2"
#define GD2_FMT_RAW             1
#define GD2_FMT_COMPRESSED      2

/* Image comparison definitions */
int gdImageCompare(gdImagePtr im1, gdImagePtr im2);

#define GD_CMP_IMAGE		1	/* Actual image IS different */
#define GD_CMP_NUM_COLORS	2	/* Number of Colours in pallette differ */
#define GD_CMP_COLOR		4	/* Image colours differ */
#define GD_CMP_SIZE_X		8	/* Image width differs */
#define GD_CMP_SIZE_Y		16	/* Image heights differ */
#define GD_CMP_TRANSPARENT	32	/* Transparent colour */
#define GD_CMP_BACKGROUND	64	/* Background colour */
#define GD_CMP_INTERLACE	128	/* Interlaced setting */
#define GD_CMP_TRUECOLOR	256	/* Truecolor vs palette differs */

/* resolution affects ttf font rendering, particularly hinting */
#define GD_RESOLUTION           96      /* pixels per inch */

#ifdef __cplusplus
}
#endif

#define gdImageBoundsSafe(im, x, y) (!(y < 0 || y >= (im)->sy || x < 0 || x >= (im)->sx))

#endif /* GD_H */
