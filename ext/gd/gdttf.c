/* gd interface to freetype library         */
/*                                          */
/* John Ellson   ellson@lucent.com          */

/* $Id$ */

#include "php.h"

#if PHP_WIN32
#include "config.w32.h"
#else
#include "php_config.h"
#endif
#if HAVE_LIBTTF && !defined(USE_GD_IMGSTRTTF)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gd.h>
#include "gdttf.h"
#include "gdcache.h"
#include <freetype.h>

#ifndef HAVE_GDIMAGECOLORRESOLVE
extern int gdImageColorResolve(gdImagePtr, int, int, int);
#endif

/* number of fonts cached before least recently used is replaced */
#define FONTCACHESIZE 6

/* number of character glyphs cached per font before 
	least-recently-used is replaced */
#define GLYPHCACHESIZE 120

/* number of bitmaps cached per glyph before 
	least-recently-used is replaced */
#define BITMAPCACHESIZE 8

/* number of antialias color lookups cached */
#define TWEENCOLORCACHESIZE 32

/* ptsize below which anti-aliasing is ineffective */
#define MINANTIALIASPTSIZE 0

/* display resolution - (Not really.  This has to be 72 or hinting is wrong) */ 
#define RESOLUTION 72

/* Number of colors used for anti-aliasing */
#undef NUMCOLORS
#define NUMCOLORS 4

/* Line separation as a factor of font height.  
      No space between if LINESPACE = 1.00 
      Line separation will be rounded up to next pixel row*/
#define LINESPACE 1.05

#ifndef TRUE
#define FALSE 0
#define TRUE !FALSE
#endif

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

typedef struct {
	char				*fontname;	/* key */
	double				ptsize;		/* key */
	double				angle;		/* key */
	double				sin_a, cos_a;
	TT_Engine           *engine;
	TT_Face				face;
	TT_Face_Properties  properties;
	TT_Instance			instance;
	TT_CharMap			char_map_Unicode;
	TT_CharMap			char_map_Big5;
	TT_CharMap			char_map_Roman;
	int					have_char_map_Unicode;
	int					have_char_map_Big5;
	int					have_char_map_Roman;
	TT_Matrix			matrix;
	TT_Instance_Metrics	imetrics;
	gdCache_head_t		*glyphCache;
} font_t;

typedef struct {
	char				*fontname;	/* key */
	double				ptsize;		/* key */
	double				angle;		/* key */
	TT_Engine			*engine;
} fontkey_t;

typedef struct {
	int					character;	/* key */
	int					hinting;	/* key */
	TT_Glyph			glyph;
	TT_Glyph_Metrics	metrics;
	TT_Outline			outline;
	TT_Pos				oldx, oldy;
	TT_Raster_Map		Bit;
	int					gray_render;
	int					xmin, xmax, ymin, ymax;
	gdCache_head_t		*bitmapCache;
} glyph_t;

typedef struct {
	int					character;	/* key */
	int					hinting;	/* key */
	int					gray_render;
	font_t				*font;
} glyphkey_t;

typedef struct {
	int					xoffset;	/* key */
	int					yoffset;	/* key */
	char				*bitmap;
} bitmap_t;

typedef struct {
	int					xoffset;	/* key */
	int					yoffset;	/* key */
	glyph_t				*glyph;
} bitmapkey_t;

typedef struct { 
    unsigned char       pixel;		/* key */
    unsigned char       bgcolor;	/* key */
    int					fgcolor;	/* key */ /* -ve means no antialias */
    gdImagePtr          im;			/* key */
    unsigned char       tweencolor;
} tweencolor_t;

typedef struct {
    unsigned char       pixel;      /* key */
    unsigned char       bgcolor;    /* key */
    int					fgcolor;    /* key */ /* -ve means no antialias */
    gdImagePtr          im;			/* key */
} tweencolorkey_t;  

/* forward declarations so that glyphCache can be initialized by font code */
static int glyphTest ( void *element, void *key );
static void *glyphFetch ( char **error, void *key );
static void glyphRelease( void *element );

/* forward declarations so that bitmapCache can be initialized by glyph code */
static int bitmapTest ( void *element, void *key );
static void *bitmapFetch ( char **error, void *key );
static void bitmapRelease( void *element );

/* local prototype */
char *gdttfchar(gdImage *im, int fg, font_t *font, int x, int y, TT_F26Dot6 x1,  TT_F26Dot6 y1, TT_F26Dot6 *advance, TT_BBox **bbox, char **next);



/********************************************************************
 * gdTcl_UtfToUniChar is borrowed from ...
 */
/*
 * tclUtf.c --
 *
 *	Routines for manipulating UTF-8 strings.
 *
 * Copyright (c) 1997-1998 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclUtf.c 1.25 98/01/28 18:02:43
 */

/*
 *---------------------------------------------------------------------------
 *
 * gdTcl_UtfToUniChar --
 *
 *	Extract the Tcl_UniChar represented by the UTF-8 string.  Bad
 *	UTF-8 sequences are converted to valid Tcl_UniChars and processing
 *	continues.  Equivalent to Plan 9 chartorune().
 *
 *	The caller must ensure that the source buffer is long enough that
 *	this routine does not run off the end and dereference non-existent
 *	memory looking for trail bytes.  If the source buffer is known to
 *	be '\0' terminated, this cannot happen.  Otherwise, the caller
 *	should call Tcl_UtfCharComplete() before calling this routine to
 *	ensure that enough bytes remain in the string.
 *
 * Results:
 *	*chPtr is filled with the Tcl_UniChar, and the return value is the
 *	number of bytes from the UTF-8 string that were consumed.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */
 
#ifndef CHARSET_EBCDIC
#define ASC(ch) (ch)
#else /*CHARSET_EBCDIC*/
#define ASC(ch) os_toascii[(unsigned char) (ch)]
#endif /*CHARSET_EBCDIC*/

#define Tcl_UniChar int
#define TCL_UTF_MAX 3
static int
gdTcl_UtfToUniChar(char *str, Tcl_UniChar *chPtr)
/* str is the UTF8 next character pointer */
/* chPtr is the int for the result */
{
    int byte;
    
	/* HTML4.0 entities in decimal form, e.g. &#197; */
    byte = *((unsigned char *) str);
	if (byte == '&') {
		int i, n=0;

		byte = *((unsigned char *) (str+1));
		if (byte == '#') {
			for (i = 2; i < 8; i++) {
				byte = *((unsigned char *) (str+i));
				if (byte >= '0' && byte <= '9') {
					n = (n * 10) + (byte - '0');
				} 
				else
					break;
			}
			if (byte == ';') {
				*chPtr = (Tcl_UniChar) n;
				return ++i;
			}
		}
	}
	 
    /*
     * Unroll 1 to 3 byte UTF-8 sequences, use loop to handle longer ones.
     */

    byte = ASC(*((unsigned char *) str));
    if (byte < 0xC0) {
	/*
	 * Handles properly formed UTF-8 characters between 0x01 and 0x7F.
	 * Also treats \0 and naked trail bytes 0x80 to 0xBF as valid
	 * characters representing themselves.
	 */

	*chPtr = (Tcl_UniChar) byte;
	return 1;
    } else if (byte < 0xE0) {
	if ((ASC(str[1]) & 0xC0) == 0x80) {
	    /*
	     * Two-byte-character lead-byte followed by a trail-byte.
	     */
	     
	    *chPtr = (Tcl_UniChar) (((byte & 0x1F) << 6) | (ASC(str[1]) & 0x3F));
	    return 2;
	}
	/*
	 * A two-byte-character lead-byte not followed by trail-byte
	 * represents itself.
	 */
	 
	*chPtr = (Tcl_UniChar) byte;
	return 1;
    } else if (byte < 0xF0) {
	if (((ASC(str[1]) & 0xC0) == 0x80) && ((ASC(str[2]) & 0xC0) == 0x80)) {
	    /*
	     * Three-byte-character lead byte followed by two trail bytes.
	     */

	    *chPtr = (Tcl_UniChar) (((byte & 0x0F) << 12) 
		    | ((ASC(str[1]) & 0x3F) << 6) | (ASC(str[2]) & 0x3F));
	    return 3;
	}
	/*
	 * A three-byte-character lead-byte not followed by two trail-bytes
	 * represents itself.
	 */

	*chPtr = (Tcl_UniChar) byte;
	return 1;
    }
#if TCL_UTF_MAX > 3
    else {
	int ch, total, trail;

	total = totalBytes[byte];
	trail = total - 1;
	if (trail > 0) {
	    ch = byte & (0x3F >> trail);
	    do {
		str++;
		if ((ASC(*str) & 0xC0) != 0x80) {
		    *chPtr = byte;
		    return 1;
		}
		ch <<= 6;
		ch |= (ASC(*str) & 0x3F);
		trail--;
	    } while (trail > 0);
	    *chPtr = ch;
	    return total;
	}
    }
#endif

    *chPtr = (Tcl_UniChar) byte;
    return 1;
}

/********************************************************************/
/* font cache functions                                             */

static int
fontTest ( void *element, void *key )
{
	font_t *a=(font_t *)element;
	fontkey_t *b=(fontkey_t *)key;

	return ( strcmp(a->fontname, b->fontname) == 0
			&&	a->ptsize == b->ptsize
			&&  a->angle == b->angle);
}

static void *
fontFetch ( char **error, void *key )
{
	TT_Error		err;
	font_t			*a;
	fontkey_t		*b=(fontkey_t *)key;
	int				i, n, map_found;
	short			platform, encoding;
	TSRMLS_FETCH();

	a = (font_t *)pemalloc(sizeof(font_t), 1);
#ifdef VIRTUAL_DIR
	/* a->fontname will be freed in fontRelease() later on */
	if (virtual_filepath(b->fontname, &a->fontname TSRMLS_CC)) {
		*error = "Could not find/open font";
		pefree(a, 1);
		return NULL;
	}
#else
	a->fontname = (char *)pemalloc(strlen(b->fontname) + 1, 1);
	strcpy(a->fontname, b->fontname);
#endif
	a->ptsize = b->ptsize;
	a->angle = b->angle;
	a->sin_a = sin(a->angle);
	a->cos_a = cos(a->angle);
	a->engine = b->engine;
	if ((err = TT_Open_Face(*b->engine, a->fontname, &a->face))) {
		if (err == TT_Err_Could_Not_Open_File) {
			*error = "Could not find/open font";
		}
		else {
			*error = "Could not read font";
		}
		pefree(a, 1);
		return NULL;
	}
	/* get face properties and allocate preload arrays */
	TT_Get_Face_Properties(a->face, &a->properties);

	/* create instance */
	if (TT_New_Instance(a->face, &a->instance)) {
		*error = "Could not create face instance";
		pefree(a, 1);
		return NULL;
	}
	
	if (TT_Set_Instance_Resolutions(a->instance, RESOLUTION, RESOLUTION)) {
		*error = "Could not set device resolutions";
		pefree(a, 1);
		return NULL;
	}

	if (TT_Set_Instance_CharSize(a->instance, (TT_F26Dot6)(a->ptsize*64))) {
		*error = "Could not set character size";
		pefree(a, 1);
		return NULL;
	}

	TT_Get_Instance_Metrics(a->instance, &a->imetrics);
	
	/* First, look for a Unicode charmap */
	n = TT_Get_CharMap_Count(a->face);

	for (i = 0; i < n; i++) {
        TT_Get_CharMap_ID(a->face, i, &platform, &encoding);
		if ((platform == 3 && encoding == 1)           /* Windows Unicode */
			|| (platform == 2 && encoding == 1)
			|| (platform == 0)) {        /* ?? Unicode */
			TT_Get_CharMap(a->face, i, &a->char_map_Unicode);
			a->have_char_map_Unicode = 1;
			map_found++;
		} else if (platform == 3 && encoding == 4) {   /* Windows Big5 */
			TT_Get_CharMap(a->face, i, &a->char_map_Big5);
			a->have_char_map_Big5 = 1;
			map_found++;
		} else if (platform == 1 && encoding == 0) {   /* Apple Roman */
			TT_Get_CharMap(a->face, i, &a->char_map_Roman);
			a->have_char_map_Roman = 1;
			map_found++;
        	}
	}

	if (! map_found) {
		*error = "Unable to find a CharMap that I can handle";
		pefree(a, 1);
	        return NULL;
	}

	a->matrix.xx = (TT_Fixed) (a->cos_a * (1<<16));
	a->matrix.yx = (TT_Fixed) (a->sin_a * (1<<16));
	a->matrix.xy = - a->matrix.yx;
	a->matrix.yy = a->matrix.xx;

	a->glyphCache = gdCacheCreate( GLYPHCACHESIZE,
					glyphTest, glyphFetch, glyphRelease);

	return (void *)a;
}

static void
fontRelease( void *element )
{
	font_t *a=(font_t *)element;

	gdCacheDelete(a->glyphCache);
	TT_Done_Instance(a->instance);
	TT_Close_Face(a->face);
	pefree(a->fontname, 1);
	pefree((char *)element, 1);
}

/********************************************************************/
/* glyph cache functions                                            */

static int
glyphTest ( void *element, void *key )
{
	glyph_t *a=(glyph_t *)element;
	glyphkey_t *b=(glyphkey_t *)key;

	return (a->character == b->character
		&& a->hinting == b->hinting
		&& a->gray_render == b->gray_render);
}

static void *
glyphFetch ( char **error, void *key )
{
	glyph_t				*a;
	glyphkey_t			*b=(glyphkey_t *)key;
	short				glyph_code;
	int					flags, err;
	int					crect[8], xmin, xmax, ymin, ymax;
	double				cos_a, sin_a;

	a = (glyph_t *)pemalloc(sizeof(glyph_t), 1);
	a->character = b->character;
	a->hinting = b->hinting;
	a->gray_render = b->gray_render;
	a->oldx = a->oldy = 0;

	/* create glyph container */
	if ((TT_New_Glyph(b->font->face, &a->glyph))) {
		*error = "Could not create glyph container";
		pefree(a, 1);
		return NULL;
	}

	flags = TTLOAD_SCALE_GLYPH;
	if (a->hinting && b->font->angle == 0.0) {
		flags |= TTLOAD_HINT_GLYPH;
	}
	if (b->font->have_char_map_Unicode) {
		glyph_code = TT_Char_Index(b->font->char_map_Unicode, a->character);
	} else if (a->character < 161 && b->font->have_char_map_Roman) {
		glyph_code = TT_Char_Index(b->font->char_map_Roman, a->character);
	} else if ( b->font->have_char_map_Big5) {
		glyph_code = TT_Char_Index(b->font->char_map_Big5, a->character);
	}
	if ((err=TT_Load_Glyph(b->font->instance, a->glyph, glyph_code, flags))) {
		*error = "TT_Load_Glyph problem";
		pefree(a, 1);
		return NULL;
	}

	TT_Get_Glyph_Metrics(a->glyph, &a->metrics);
	if (b->font->angle != 0.0) {
		TT_Get_Glyph_Outline(a->glyph, &a->outline);
		TT_Transform_Outline(&a->outline, &b->font->matrix);
	}

	/* calculate bitmap size */
	xmin = a->metrics.bbox.xMin -64;
	ymin = a->metrics.bbox.yMin -64;
	xmax = a->metrics.bbox.xMax +64;
	ymax = a->metrics.bbox.yMax +64;

	cos_a = b->font->cos_a;
	sin_a = b->font->sin_a;
	crect[0] = (int)(xmin * cos_a - ymin * sin_a);
	crect[1] = (int)(xmin * sin_a + ymin * cos_a);
	crect[2] = (int)(xmax * cos_a - ymin * sin_a);
	crect[3] = (int)(xmax * sin_a + ymin * cos_a);
	crect[4] = (int)(xmax * cos_a - ymax * sin_a);
	crect[5] = (int)(xmax * sin_a + ymax * cos_a);
	crect[6] = (int)(xmin * cos_a - ymax * sin_a);
	crect[7] = (int)(xmin * sin_a + ymax * cos_a);
	a->xmin = MIN(MIN(crect[0], crect[2]), MIN(crect[4], crect[6]));
	a->xmax = MAX(MAX(crect[0], crect[2]), MAX(crect[4], crect[6]));
	a->ymin = MIN(MIN(crect[1], crect[3]), MIN(crect[5], crect[7]));
	a->ymax = MAX(MAX(crect[1], crect[3]), MAX(crect[5], crect[7]));

	/* allocate bitmap large enough for character */
	a->Bit.rows = (a->ymax - a->ymin + 32 + 64) / 64;
	a->Bit.width = (a->xmax - a->xmin + 32 + 64) / 64;
	a->Bit.flow = TT_Flow_Up;
	if (a->gray_render) {
		a->Bit.cols = a->Bit.width;               /* 1 byte per pixel */
	}
	else {
		a->Bit.cols = (a->Bit.width + 7) / 8;     /* 1 bit per pixel */
	}
	a->Bit.cols = (a->Bit.cols + 3) & ~3;         /* pad to 32 bits */
	a->Bit.size = a->Bit.rows * a->Bit.cols;      /* # of bytes in buffer */
	a->Bit.bitmap = NULL;

	a->bitmapCache = gdCacheCreate( BITMAPCACHESIZE,
					bitmapTest, bitmapFetch, bitmapRelease);

	return (void *)a;
}

static void
glyphRelease( void *element )
{
	glyph_t *a=(glyph_t *)element;

	gdCacheDelete(a->bitmapCache);
	TT_Done_Glyph( a->glyph );
	pefree ((char *)element, 1);
}

/********************************************************************/
/* bitmap cache functions                                            */

static int
bitmapTest ( void *element, void *key )
{
	bitmap_t *a=(bitmap_t *)element;
	bitmapkey_t *b=(bitmapkey_t *)key;

	if (a->xoffset == b->xoffset && a->yoffset == b->yoffset) {
		b->glyph->Bit.bitmap = a->bitmap;
		return TRUE;
	}
	return FALSE;
}

static void *
bitmapFetch ( char **error, void *key )
{
	bitmap_t			*a;
	bitmapkey_t			*b=(bitmapkey_t *)key;

	a = (bitmap_t *)pemalloc(sizeof(bitmap_t), 1);
	a->xoffset = b->xoffset;
	a->yoffset = b->yoffset;

	b->glyph->Bit.bitmap = a->bitmap = (char *)pemalloc(b->glyph->Bit.size, 1);
	memset(a->bitmap, 0, b->glyph->Bit.size);
	/* render glyph */
	if (b->glyph->gray_render) {
		TT_Get_Glyph_Pixmap(b->glyph->glyph, &b->glyph->Bit,
			a->xoffset, a->yoffset);
	}
	else {
		TT_Get_Glyph_Bitmap(b->glyph->glyph, &b->glyph->Bit,
			a->xoffset, a->yoffset);
	}
	return (void *)a;
}

static void
bitmapRelease( void *element )
{
	bitmap_t *a=(bitmap_t *)element;

	pefree (a->bitmap, 1);
	pefree ((char *)element, 1);
}

/********************************************************************/
/* tweencolor cache functions                                            */

static int
tweenColorTest (void *element, void *key)
{ 
    tweencolor_t *a=(tweencolor_t *)element;
    tweencolorkey_t *b=(tweencolorkey_t *)key;
    
    return (a->pixel == b->pixel    
         && a->bgcolor == b->bgcolor
         && a->fgcolor == b->fgcolor
         && a->im == b->im);
} 

static void *
tweenColorFetch (char **error, void *key)
{
    tweencolor_t *a;
    tweencolorkey_t *b=(tweencolorkey_t *)key;
	int pixel, npixel, bg, fg;
	gdImagePtr im;
   
    a = (tweencolor_t *)pemalloc(sizeof(tweencolor_t), 1);
	pixel = a->pixel = b->pixel;
	bg = a->bgcolor = b->bgcolor;
	fg = a->fgcolor = b->fgcolor;
	im = b->im;

	/* if fg is specified by a negative color idx, then don't antialias */
	if (fg <0) {
		a->tweencolor = -fg;
	} else {
		npixel = NUMCOLORS - pixel;
		a->tweencolor = gdImageColorResolve(im,
			(pixel * im->red  [fg] + npixel * im->red  [bg]) / NUMCOLORS,
			(pixel * im->green[fg] + npixel * im->green[bg]) / NUMCOLORS,
			(pixel * im->blue [fg] + npixel * im->blue [bg]) / NUMCOLORS);
	}
    *error = NULL;
    return (void *)a;
}   
        
static void
tweenColorRelease(void *element)
{   
    pefree((char *)element, 1);
}   

/********************************************************************/
/* gdttfchar -  render one character onto a gd image                */

static int OneTime=0;
static gdCache_head_t   *tweenColorCache;

char *
gdttfchar(gdImage *im, int fg, font_t *font,
	int x, int y,					/* string start pos in pixels */
	TT_F26Dot6 x1,	TT_F26Dot6 y1,	/* char start offset (*64) from x,y */
	TT_F26Dot6 *advance,
	TT_BBox **bbox, 
	char **next)
{
    int pc, ch, len;
	int row, col;
	int x2, y2;     /* char start pos in pixels */ 
	int x3, y3;     /* current pixel pos */
	unsigned char *pixel;

    glyph_t *glyph;
    glyphkey_t glyphkey;
    bitmapkey_t bitmapkey;
	tweencolor_t *tweencolor;
	tweencolorkey_t tweencolorkey;

	/****** set up tweenColorCache on first call ************/
	if (! OneTime) {
		tweenColorCache = gdCacheCreate(TWEENCOLORCACHESIZE,
			tweenColorTest, tweenColorFetch, tweenColorRelease);
		OneTime++;
	}
	/**************/

	if (font->have_char_map_Unicode) { /* use UTF-8 mapping from ASCII */
        len = gdTcl_UtfToUniChar(*next, &ch);
        *next += len;
	} else {
	/*
	 * Big 5 mapping:
	 * use "JIS-8 half-width katakana" coding from 8-bit characters.  Ref:
	 * ftp://ftp.ora.com/pub/examples/nutshell/ujip/doc/japan.inf-032092.sjs
	 */
		ch = (**next) & 255;         /* don't extend sign */
		(*next)++;
		if (ch >= 161                /* first code of JIS-8 pair */
			&& **next) {                /* don't advance past '\0' */
			ch = (ch * 256) + **next;
			(*next)++;
		}
	}

	glyphkey.character = ch;
	glyphkey.hinting = 1;
	/* if fg is specified by a negative color idx, then don't antialias */
	glyphkey.gray_render = ((font->ptsize < MINANTIALIASPTSIZE) || (fg <0))?FALSE:TRUE;
    glyphkey.font = font;
    glyph = (glyph_t *)gdCacheGet(font->glyphCache, &glyphkey);
    if (! glyph)
		return font->glyphCache->error;

	*bbox = &glyph->metrics.bbox;
	*advance = glyph->metrics.advance;

	/* if null *im, or invalid color,  then assume user just wants brect */
	if (!im || fg > 255 || fg < -255)
		return (char *)NULL;

	/* render (via cache) a bitmap for the current fractional offset */
	bitmapkey.xoffset = ((x1+32) & 63) - 32 - ((glyph->xmin+32) & -64);
	bitmapkey.yoffset = ((y1+32) & 63) - 32 - ((glyph->ymin+32) & -64);
	bitmapkey.glyph = glyph;
	gdCacheGet(glyph->bitmapCache, &bitmapkey);

	/* copy to gif, mapping colors */
      x2 = x + (((glyph->xmin+32) & -64) + ((x1+32) & -64)) / 64;
      y2 = y - (((glyph->ymin+32) & -64) + ((y1+32) & -64)) / 64;
	tweencolorkey.fgcolor = fg;
	tweencolorkey.im = im;
	for (row = 0; row < glyph->Bit.rows; row++) {
		if (glyph->gray_render)
			pc = row * glyph->Bit.cols;
		else
			pc = row * glyph->Bit.cols * 8;
		y3 = y2 - row;
		if (y3 >= im->sy || y3 < 0) continue;
		for (col = 0; col < glyph->Bit.width; col++, pc++) {
			if (glyph->gray_render) {
				tweencolorkey.pixel = 
					*((unsigned char *)(glyph->Bit.bitmap) + pc);
			} else {
				tweencolorkey.pixel = 
					(((*((unsigned char *)(glyph->Bit.bitmap) + pc/8))
						<<(pc%8))&128)?4:0;
			}
			/* if not background */
			if (tweencolorkey.pixel > 0) {
				x3 = x2 + col;
				if (x3 >= im->sx || x3 < 0) continue;
#if HAVE_LIBGD20
				if (im->trueColor) {
					pixel = &im->tpixels[y3][x3];
				} else
#endif
				{
#if HAVE_LIBGD13
					pixel = &im->pixels[y3][x3];
#else
					pixel = &im->pixels[x3][y3];
#endif
				}
				tweencolorkey.bgcolor = *pixel;
				tweencolor = (tweencolor_t *)gdCacheGet(
					tweenColorCache, &tweencolorkey);
				*pixel = tweencolor->tweencolor;
			}
		}
	}
	return (char *)NULL;
}

/********************************************************************/
/* gdttf -  render a utf8 string onto a gd image					*/

char *
gdttf(gdImage *im, int *brect, int fg, char *fontname,
	double ptsize, double angle, int x, int y, char *str)
{
	TT_F26Dot6 ur_x=0, ur_y=0, ll_x=0, ll_y=0;
	TT_F26Dot6 advance_x, advance_y, advance, x1, y1;
	TT_BBox *bbox;
	double sin_a, cos_a;
    int i=0, ch;
	font_t *font;
	fontkey_t fontkey;
	char *error, *next;

	/****** initialize font engine on first call ************/
    static gdCache_head_t	*fontCache;
	static TT_Engine 	engine;

	if (! fontCache) {
		if (TT_Init_FreeType(&engine)) {
			return "Failure to initialize font engine";
		}
		fontCache = gdCacheCreate( FONTCACHESIZE,
			fontTest, fontFetch, fontRelease);
	}
	/**************/

	/* get the font (via font cache) */
	fontkey.fontname = fontname;
	fontkey.ptsize = ptsize;
	fontkey.angle = angle;
	fontkey.engine = &engine;
	font = (font_t *)gdCacheGet(fontCache, &fontkey);
	if (! font) {
		return fontCache->error;
	}
	sin_a = font->sin_a;
	cos_a = font->cos_a;
	advance_x = advance_y = 0;

	next=str;
	while (*next) {	  
		ch = *next;

		/* carriage returns */
		if (ch == '\r') {
			advance_x = 0;
			next++;
			continue;
		}
		/* newlines */
		if (ch == '\n') {
                      advance_y -= (TT_F26Dot6)(font->imetrics.y_ppem * LINESPACE * 64);
                      advance_y = (advance_y-32) & -64; /* round to next pixel row */
			next++;
			continue;
		}

		x1 = (TT_F26Dot6)(advance_x * cos_a - advance_y * sin_a);
		y1 = (TT_F26Dot6)(advance_x * sin_a + advance_y * cos_a);

		if ((error=gdttfchar(im, fg, font, x, y, x1, y1, &advance, &bbox, &next)))
			return error;

		if (! i++) { /* if first character, init BB corner values */
			ll_x = bbox->xMin;
			ll_y = bbox->yMin;
			ur_x = bbox->xMax;
			ur_y = bbox->yMax;
		}
		else {
			if (! advance_x) ll_x = MIN(bbox->xMin, ll_x);
			ll_y = MIN(advance_y + bbox->yMin, ll_y);
			ur_x = MAX(advance_x + bbox->xMax, ur_x);
			if (! advance_y) ur_y = MAX(bbox->yMax, ur_y);
		}
		advance_x += advance;
	}

	/* rotate bounding rectangle */
	brect[0] = (int)(ll_x * cos_a - ll_y * sin_a);
	brect[1] = (int)(ll_x * sin_a + ll_y * cos_a);
	brect[2] = (int)(ur_x * cos_a - ll_y * sin_a);
	brect[3] = (int)(ur_x * sin_a + ll_y * cos_a);
	brect[4] = (int)(ur_x * cos_a - ur_y * sin_a);
	brect[5] = (int)(ur_x * sin_a + ur_y * cos_a);
	brect[6] = (int)(ll_x * cos_a - ur_y * sin_a);
	brect[7] = (int)(ll_x * sin_a + ur_y * cos_a);

	/* scale, round and offset brect */
	i = 0;
	while (i<8) {
		brect[i] = x + (brect[i] + 32) / 64;
		i++;
		brect[i] = y - (brect[i] + 32) / 64;
		i++;
	}

    return (char *)NULL;
}
   
#endif /* HAVE_LIBTTF */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
