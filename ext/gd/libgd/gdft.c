
/********************************************/
/* gd interface to freetype library         */
/*                                          */
/* John Ellson   ellson@graphviz.org        */
/********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gd.h"
#include "gdhelpers.h"

#ifndef MSWIN32
#include <unistd.h>
#else
#include <io.h>
#ifndef R_OK
# define R_OK 04			/* Needed in Windows */
#endif
#endif

#ifdef WIN32
#define access _access
#ifndef R_OK
#define R_OK 2
#endif
#endif

/* number of antialised colors for indexed bitmaps */
/* overwrite Windows GDI define in case of windows build */
#ifdef NUMCOLORS
#undef NUMCOLORS
#endif
#define NUMCOLORS 8

char *
gdImageStringTTF (gdImage * im, int *brect, int fg, char *fontlist,
		  double ptsize, double angle, int x, int y, char *string)
{
	/* 2.0.6: valid return */
	return gdImageStringFT (im, brect, fg, fontlist, ptsize, angle, x, y, string);
}

#ifndef HAVE_LIBFREETYPE
char *
gdImageStringFTEx (gdImage * im, int *brect, int fg, char *fontlist,
		 double ptsize, double angle, int x, int y, char *string,
		 gdFTStringExtraPtr strex)
{
	return "libgd was not built with FreeType font support\n";
}

char *
gdImageStringFT (gdImage * im, int *brect, int fg, char *fontlist,
		 double ptsize, double angle, int x, int y, char *string)
{
	return "libgd was not built with FreeType font support\n";
}
#else

#include "gdcache.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

/* number of fonts cached before least recently used is replaced */
#define FONTCACHESIZE 6

/* number of antialias color lookups cached */
#define TWEENCOLORCACHESIZE 32

/*
 * Line separation as a factor of font height.
 *      No space between if LINESPACE = 1.00
 *      Line separation will be rounded up to next pixel row.
 */
#define LINESPACE 1.05

/*
 * The character (space) used to separate alternate fonts in the
 * fontlist parameter to gdImageStringFT. 2.0.18: space was a oor choice for this.
 */
#define LISTSEPARATOR ";"

/*
 * DEFAULT_FONTPATH and PATHSEPARATOR are host type dependent and
 * are normally set by configure in config.h.  These are just
 * some last resort values that might match some Un*x system
 * if building this version of gd separate from graphviz.
 */
#ifndef DEFAULT_FONTPATH
#if defined(__APPLE__) || (defined(__MWERKS__) && defined(macintosh))
#define DEFAULT_FONTPATH "/usr/share/fonts/truetype:/System/Library/Fonts:/Library/Fonts"
#else
#define DEFAULT_FONTPATH "/usr/share/fonts/truetype"
#endif
#endif
#ifndef PATHSEPARATOR
#define PATHSEPARATOR ":"
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE !FALSE
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

typedef struct
{
	char *fontlist;		/* key */
	FT_Library *library;
	FT_Face face;
	FT_Bool have_char_map_unicode, have_char_map_big5, have_char_map_sjis, have_char_map_apple_roman;
	gdCache_head_t *glyphCache;
} font_t;

typedef struct
{
	char *fontlist;		/* key */
	int preferred_map;
	FT_Library *library;
} fontkey_t;

typedef struct
{
	int pixel;		/* key */
	int bgcolor;		/* key */
	int fgcolor;		/* key *//* -ve means no antialias */
	gdImagePtr im;		/* key */
	int tweencolor;
} tweencolor_t;

typedef struct
{
	int pixel;		/* key */
	int bgcolor;		/* key */
	int fgcolor;		/* key *//* -ve means no antialias */
	gdImagePtr im;		/* key */
} tweencolorkey_t;

/********************************************************************
 * gdTcl_UtfToUniChar is borrowed from Tcl ...
 */
/*
 * tclUtf.c --
 *
 *      Routines for manipulating UTF-8 strings.
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
 *      Extract the Tcl_UniChar represented by the UTF-8 string.  Bad
 *      UTF-8 sequences are converted to valid Tcl_UniChars and processing
 *      continues.  Equivalent to Plan 9 chartorune().
 *
 *      The caller must ensure that the source buffer is long enough that
 *      this routine does not run off the end and dereference non-existent
 *      memory looking for trail bytes.  If the source buffer is known to
 *      be '\0' terminated, this cannot happen.  Otherwise, the caller
 *      should call Tcl_UtfCharComplete() before calling this routine to
 *      ensure that enough bytes remain in the string.
 *
 * Results:
 *      *chPtr is filled with the Tcl_UniChar, and the return value is the
 *      number of bytes from the UTF-8 string that were consumed.
 *
 * Side effects:
 *      None.
 *
 *---------------------------------------------------------------------------
 */

#ifdef JISX0208
#include "jisx0208.h"
#endif

extern int any2eucjp (char *, char *, unsigned int);

/* Persistent font cache until explicitly cleared */
/* Fonts can be used across multiple images */

/* 2.0.16: thread safety (the font cache is shared) */
gdMutexDeclare(gdFontCacheMutex);
static gdCache_head_t *fontCache = NULL;
static FT_Library library;

#define Tcl_UniChar int
#define TCL_UTF_MAX 3
static int gdTcl_UtfToUniChar (char *str, Tcl_UniChar * chPtr)
/* str is the UTF8 next character pointer */
/* chPtr is the int for the result */
{
	int byte;

	/* HTML4.0 entities in decimal form, e.g. &#197; */
	byte = *((unsigned char *) str);
	if (byte == '&') {
		int i, n = 0;

		byte = *((unsigned char *) (str + 1));
		if (byte == '#') {
			byte = *((unsigned char *) (str + 2));
			if (byte == 'x' || byte == 'X') {
				for (i = 3; i < 8; i++) {
					byte = *((unsigned char *) (str + i));
					if (byte >= 'A' && byte <= 'F')
						byte = byte - 'A' + 10;
					else if (byte >= 'a' && byte <= 'f')
						byte = byte - 'a' + 10;
					else if (byte >= '0' && byte <= '9')
						byte = byte - '0';
					else
						break;
					n = (n * 16) + byte;
				}
			} else {
				for (i = 2; i < 8; i++) {
					byte = *((unsigned char *) (str + i));
					if (byte >= '0' && byte <= '9') {
						n = (n * 10) + (byte - '0');
					} else {
						break;
					}
				}
			}
			if (byte == ';') {
				*chPtr = (Tcl_UniChar) n;
				return ++i;
			}
		}
	}

	/* Unroll 1 to 3 byte UTF-8 sequences, use loop to handle longer ones. */

	byte = *((unsigned char *) str);
#ifdef JISX0208
	if (0xA1 <= byte && byte <= 0xFE) {
		int ku, ten;

		ku = (byte & 0x7F) - 0x20;
		ten = (str[1] & 0x7F) - 0x20;
		if ((ku < 1 || ku > 92) || (ten < 1 || ten > 94)) {
			*chPtr = (Tcl_UniChar) byte;
			return 1;
		}

		*chPtr = (Tcl_UniChar) UnicodeTbl[ku - 1][ten - 1];
		return 2;
	} else
#endif /* JISX0208 */
	if (byte < 0xC0) {
		/* Handles properly formed UTF-8 characters between
		 * 0x01 and 0x7F.  Also treats \0 and naked trail
		 * bytes 0x80 to 0xBF as valid characters representing
		 * themselves.
		 */

		*chPtr = (Tcl_UniChar) byte;
		return 1;
	} else if (byte < 0xE0) {
		if ((str[1] & 0xC0) == 0x80) {
			/* Two-byte-character lead-byte followed by a trail-byte. */

			*chPtr = (Tcl_UniChar) (((byte & 0x1F) << 6) | (str[1] & 0x3F));
			return 2;
		}
		/*
		 * A two-byte-character lead-byte not followed by trail-byte
		 * represents itself.
		 */

		*chPtr = (Tcl_UniChar) byte;
		return 1;
	} else if (byte < 0xF0) {
		if (((str[1] & 0xC0) == 0x80) && ((str[2] & 0xC0) == 0x80)) {
			/* Three-byte-character lead byte followed by two trail bytes. */

			*chPtr = (Tcl_UniChar) (((byte & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F));
			return 3;
		}
		/* A three-byte-character lead-byte not followed by two trail-bytes represents itself. */

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
				if ((*str & 0xC0) != 0x80) {
					*chPtr = byte;
					return 1;
				}
				ch <<= 6;
				ch |= (*str & 0x3F);
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

static int fontTest (void *element, void *key)
{
	font_t *a = (font_t *) element;
	fontkey_t *b = (fontkey_t *) key;

	if (strcmp (a->fontlist, b->fontlist) == 0) {
		switch (b->preferred_map) {
			case gdFTEX_Unicode:
				if (a->have_char_map_unicode) {
					return 1;
				}
				break;
			case gdFTEX_Shift_JIS:
				if (a->have_char_map_sjis) {
					return 1;
				}
				break;
			case gdFTEX_Big5:
				if (a->have_char_map_sjis) {
					return 1;
				}
				break;
		}
	}
	return 0;
}

static void *fontFetch (char **error, void *key)
{
	font_t *a;
	fontkey_t *b = (fontkey_t *) key;
	int n;
	int font_found = 0;
	unsigned short platform, encoding;
	char *fontsearchpath, *fontlist;
	char fullname[MAXPATHLEN], cur_dir[MAXPATHLEN];
	char *name, *path=NULL, *dir;
	char *strtok_ptr;
	FT_Error err;
	FT_CharMap found = 0;
	FT_CharMap charmap;

	a = (font_t *) gdPMalloc(sizeof(font_t));
	a->fontlist = gdPEstrdup(b->fontlist);
	a->library = b->library;

	/*
	 * Search the pathlist for any of a list of font names.
	 */
	fontsearchpath = getenv ("GDFONTPATH");
	if (!fontsearchpath) {
		fontsearchpath = DEFAULT_FONTPATH;
	}
	fontlist = gdEstrdup(a->fontlist);

	/*
	 * Must use gd_strtok_r becasuse strtok() isn't thread safe
	 */
	for (name = gd_strtok_r (fontlist, LISTSEPARATOR, &strtok_ptr); name; name = gd_strtok_r (0, LISTSEPARATOR, &strtok_ptr)) {
		char *strtok_ptr_path;
		/* make a fresh copy each time - strtok corrupts it. */
		path = gdEstrdup (fontsearchpath);

		/* if name is an absolute filename then test directly */
#ifdef NETWARE
		if (*name == '/' || (name[0] != 0 && strstr(name, ":/"))) {
#else
		if (*name == '/' || (name[0] != 0 && name[1] == ':' && (name[2] == '/' || name[2] == '\\'))) {
#endif
			snprintf(fullname, sizeof(fullname) - 1, "%s", name);
			if (access(fullname, R_OK) == 0) {
				font_found++;
				break;
			}
		}
		for (dir = gd_strtok_r (path, PATHSEPARATOR, &strtok_ptr_path); dir;
		     dir = gd_strtok_r (0, PATHSEPARATOR, &strtok_ptr_path)) {
			if (!strcmp(dir, ".")) {
			#if HAVE_GETCWD
				dir = VCWD_GETCWD(cur_dir, MAXPATHLEN);
#elif HAVE_GETWD
				dir = VCWD_GETWD(cur_dir);
#endif
				if (!dir) {
					continue;
				}
			}

#define GD_CHECK_FONT_PATH(ext)	\
	snprintf(fullname, sizeof(fullname) - 1, "%s/%s%s", dir, name, ext);	\
	if (access(fullname, R_OK) == 0) {	\
		font_found++;	\
		break;	\
	}	\

			GD_CHECK_FONT_PATH("");
			GD_CHECK_FONT_PATH(".ttf");
			GD_CHECK_FONT_PATH(".pfa");
			GD_CHECK_FONT_PATH(".pfb");
			GD_CHECK_FONT_PATH(".dfont");
		}
		gdFree(path);
		path = NULL;
		if (font_found) {
			break;
		}
	}

	if (path) {
		gdFree(path);
	}

	gdFree(fontlist);

	if (!font_found) {
		gdPFree(a->fontlist);
		gdPFree(a);
		*error = "Could not find/open font";
		return NULL;
	}

	err = FT_New_Face (*b->library, fullname, 0, &a->face);
	if (err) {
		gdPFree(a->fontlist);
		gdPFree(a);
		*error = "Could not read font";
		return NULL;
	}

	/* FIXME - This mapping stuff is incomplete - where is the spec? */
	/* EAM   - It's worse than that. It's pointless to match character encodings here.
	 *         As currently written, the stored a->face->charmap only matches one of
	 *         the actual charmaps and we cannot know at this stage if it is the right
	 *         one. We should just skip all this stuff, and check in gdImageStringFTEx
	 *         if some particular charmap is preferred and if so whether it is held in
	 *         one of the a->face->charmaps[0..num_charmaps].
	 *         And why is it so bad not to find any recognized charmap?  The user may
	 *         still know what mapping to use, even if we do not.  In that case we can
	 *         just use the map in a->face->charmaps[num_charmaps] and be done with it.
	 */

	for (n = 0; n < a->face->num_charmaps; n++) {
		charmap = a->face->charmaps[n];
		platform = charmap->platform_id;
		encoding = charmap->encoding_id;

		/* Whatever is the last value is what should be set */
		a->have_char_map_unicode = 0;
		a->have_char_map_big5 = 0;
		a->have_char_map_sjis = 0;
		a->have_char_map_apple_roman = 0;

/* EAM DEBUG - Newer versions of libfree2 make it easier by defining encodings */
#if (defined(FREETYPE_MAJOR) && ((FREETYPE_MAJOR == 2 && ((FREETYPE_MINOR == 1 && FREETYPE_PATCH >= 3) || FREETYPE_MINOR > 1) || FREETYPE_MAJOR > 2)))
	if (charmap->encoding == FT_ENCODING_MS_SYMBOL
		|| charmap->encoding == FT_ENCODING_ADOBE_CUSTOM
		|| charmap->encoding == FT_ENCODING_ADOBE_STANDARD) {
		a->have_char_map_unicode = 1;
		found = charmap;
		a->face->charmap = charmap;
		return (void *)a;
	}
#endif /* Freetype 2.1.3 or better */
/* EAM DEBUG */

		if ((platform == 3 && encoding == 1)		/* Windows Unicode */
			|| (platform == 3 && encoding == 0)	/* Windows Symbol */
			|| (platform == 2 && encoding == 1)	/* ISO Unicode */
			|| (platform == 0))
		{						/* Apple Unicode */
			a->have_char_map_unicode = 1;
			found = charmap;
			if (b->preferred_map == gdFTEX_Unicode) {
				break;
			}
		} else if (platform == 3 && encoding == 4) {	/* Windows Big5 */
			a->have_char_map_big5 = 1;
			found = charmap;
			if (b->preferred_map == gdFTEX_Big5) {
				break;
			}
		} else if (platform == 3 && encoding == 2) {	/* Windows Sjis */
			a->have_char_map_sjis = 1;
			found = charmap;
			if (b->preferred_map == gdFTEX_Shift_JIS) {
				break;
			}
		} else if ((platform == 1 && encoding == 0)	/* Apple Roman */
			|| (platform == 2 && encoding == 0))
		{						/* ISO ASCII */
			a->have_char_map_apple_roman = 1;
			found = charmap;
			if (b->preferred_map == gdFTEX_MacRoman) {
				break;
			}
		}
	}
	if (!found) {
		gdPFree(a->fontlist);
		gdPFree(a);
		*error = "Unable to find a CharMap that I can handle";
		return NULL;
	}

	/* 2.0.5: we should actually return this */
	a->face->charmap = found;
	return (void *) a;
}

static void fontRelease (void *element)
{
	font_t *a = (font_t *) element;

	FT_Done_Face (a->face);
	gdPFree(a->fontlist);
	gdPFree((char *) element);
}

/********************************************************************/
/* tweencolor cache functions                                            */

static int tweenColorTest (void *element, void *key)
{
	tweencolor_t *a = (tweencolor_t *) element;
	tweencolorkey_t *b = (tweencolorkey_t *) key;

	return (a->pixel == b->pixel && a->bgcolor == b->bgcolor && a->fgcolor == b->fgcolor && a->im == b->im);
}

/*
 * Computes a color in im's color table that is part way between
 * the background and foreground colors proportional to the gray
 * pixel value in the range 0-NUMCOLORS. The fg and bg colors must already
 * be in the color table for palette images. For truecolor images the
 * returned value simply has an alpha component and gdImageAlphaBlend
 * does the work so that text can be alpha blended across a complex
 * background (TBB; and for real in 2.0.2).
 */
static void * tweenColorFetch (char **error, void *key)
{
	tweencolor_t *a;
	tweencolorkey_t *b = (tweencolorkey_t *) key;
	int pixel, npixel, bg, fg;
	gdImagePtr im;

	a = (tweencolor_t *) gdMalloc (sizeof (tweencolor_t));
	pixel = a->pixel = b->pixel;
	bg = a->bgcolor = b->bgcolor;
	fg = a->fgcolor = b->fgcolor;
	im = a->im = b->im;

	/* if fg is specified by a negative color idx, then don't antialias */
	if (fg < 0) {
		if ((pixel + pixel) >= NUMCOLORS) {
			a->tweencolor = -fg;
		} else {
			a->tweencolor = bg;
		}
	} else {
		npixel = NUMCOLORS - pixel;
		if (im->trueColor) {
			/* 2.0.1: use gdImageSetPixel to do the alpha blending work,
			 * or to just store the alpha level. All we have to do here
			 * is incorporate our knowledge of the percentage of this
			 * pixel that is really "lit" by pushing the alpha value
			 * up toward transparency in edge regions.
			 */
			a->tweencolor = gdTrueColorAlpha(
						gdTrueColorGetRed(fg),
						gdTrueColorGetGreen(fg),
                                        	gdTrueColorGetBlue(fg),
						gdAlphaMax - (gdTrueColorGetAlpha (fg) * pixel / NUMCOLORS));
		} else {
			a->tweencolor = gdImageColorResolve(im,
						(pixel * im->red[fg] + npixel * im->red[bg]) / NUMCOLORS,
						(pixel * im->green[fg] + npixel * im->green[bg]) / NUMCOLORS,
						(pixel * im->blue[fg] + npixel * im->blue[bg]) / NUMCOLORS);
		}
	}
	return (void *) a;
}

static void tweenColorRelease (void *element)
{
	gdFree((char *) element);
}

/* draw_bitmap - transfers glyph bitmap to GD image */
static char * gdft_draw_bitmap (gdCache_head_t *tc_cache, gdImage * im, int fg, FT_Bitmap bitmap, int pen_x, int pen_y)
{
	unsigned char *pixel = NULL;
	int *tpixel = NULL;
	int opixel;
	int x, y, row, col, pc, pcr;

	tweencolor_t *tc_elem;
	tweencolorkey_t tc_key;

	/* copy to image, mapping colors */
	tc_key.fgcolor = fg;
	tc_key.im = im;
	/* Truecolor version; does not require the cache */
	if (im->trueColor) {
		for (row = 0; row < bitmap.rows; row++) {
			pc = row * bitmap.pitch;
			pcr = pc;
			y = pen_y + row;
			/* clip if out of bounds */
			/* 2.0.16: clipping rectangle, not image bounds */
			if ((y > im->cy2) || (y < im->cy1)) {
				continue;
			}
			for (col = 0; col < bitmap.width; col++, pc++) {
				int level;
				if (bitmap.pixel_mode == ft_pixel_mode_grays) {
					/* Scale to 128 levels of alpha for gd use.
					 * alpha 0 is opacity, so be sure to invert at the end
					 */
					level = (bitmap.buffer[pc] * gdAlphaMax / (bitmap.num_grays - 1));
				} else if (bitmap.pixel_mode == ft_pixel_mode_mono) {
					/* 2.0.5: mode_mono fix from Giuliano Pochini */
					level = ((bitmap.buffer[(col>>3)+pcr]) & (1<<(~col&0x07))) ? gdAlphaTransparent : gdAlphaOpaque;
				} else {
					return "Unsupported ft_pixel_mode";
				}
				if (level == 0)  /* if background */
					continue;
				if ((fg >= 0) && (im->trueColor)) {
					/* Consider alpha in the foreground color itself to be an
					 * upper bound on how opaque things get, when truecolor is
					 * available. Without truecolor this results in far too many
					 * color indexes.
					 */
					level = level * (gdAlphaMax - gdTrueColorGetAlpha(fg)) / gdAlphaMax;
				}
				level = gdAlphaMax - level;
				x = pen_x + col;
				/* clip if out of bounds */
				/* 2.0.16: clip to clipping rectangle, Matt McNabb */
				if ((x > im->cx2) || (x < im->cx1)) {
					continue;
				}
				/* get pixel location in gd buffer */
				tpixel = &im->tpixels[y][x];
				if (fg < 0) {
					if (level < (gdAlphaMax / 2)) {
						*tpixel = -fg;
					}
				} else {
					if (im->alphaBlendingFlag) {
						opixel = *tpixel;
						if (gdTrueColorGetAlpha(opixel) != gdAlphaTransparent) {
							*tpixel = gdAlphaBlend (opixel,
							                        (level << 24) + (fg & 0xFFFFFF));
						} else {
							*tpixel = (level << 24) + (fg & 0xFFFFFF);
						}
					} else {
						*tpixel = (level << 24) + (fg & 0xFFFFFF);
					}
				}
			}
		}
		return (char *) NULL;
	}
	/* Non-truecolor case, restored to its more or less original form */
	for (row = 0; row < bitmap.rows; row++) {
		int pcr;
		pc = row * bitmap.pitch;
		pcr = pc;
		if (bitmap.pixel_mode==ft_pixel_mode_mono) {
			pc *= 8;    /* pc is measured in bits for monochrome images */
		}
		y = pen_y + row;

		/* clip if out of bounds */
		if (y >= im->sy || y < 0) {
			continue;
		}

		for (col = 0; col < bitmap.width; col++, pc++) {
			if (bitmap.pixel_mode == ft_pixel_mode_grays) {
				/*
				 * Round to NUMCOLORS levels of antialiasing for
				 * index color images since only 256 colors are
				 * available.
				 */
				tc_key.pixel = ((bitmap.buffer[pc] * NUMCOLORS) + bitmap.num_grays / 2) / (bitmap.num_grays - 1);
			} else if (bitmap.pixel_mode == ft_pixel_mode_mono) {
				tc_key.pixel = ((bitmap.buffer[pc / 8] << (pc % 8)) & 128) ? NUMCOLORS : 0;
				/* 2.0.5: mode_mono fix from Giuliano Pochini */
				tc_key.pixel = ((bitmap.buffer[(col>>3)+pcr]) & (1<<(~col&0x07))) ? NUMCOLORS : 0;
			} else {
				return "Unsupported ft_pixel_mode";
			}
			if (tc_key.pixel > 0) { /* if not background */
				x = pen_x + col;

				/* clip if out of bounds */
				if (x >= im->sx || x < 0) {
					continue;
				}
				/* get pixel location in gd buffer */
				pixel = &im->pixels[y][x];
				if (tc_key.pixel == NUMCOLORS) {
					/* use fg color directly. gd 2.0.2: watch out for
					 * negative indexes (thanks to David Marwood).
					 */
					*pixel = (fg < 0) ? -fg : fg;
				} else {
					/* find antialised color */
					tc_key.bgcolor = *pixel;
					tc_elem = (tweencolor_t *) gdCacheGet(tc_cache, &tc_key);
					*pixel = tc_elem->tweencolor;
				}
			}
		}
	}
	return (char *) NULL;
}

static int
gdroundupdown (FT_F26Dot6 v1, int roundup)
{
	return (!roundup) ? v1 >> 6 : (v1 + 63) >> 6;
}

void gdFontCacheShutdown()
{
	gdMutexLock(gdFontCacheMutex);

	if (fontCache) {
		gdCacheDelete(fontCache);
		fontCache = NULL;
		FT_Done_FreeType(library);
	}

	gdMutexUnlock(gdFontCacheMutex);
}

void gdFreeFontCache()
{
	gdFontCacheShutdown();
}

void gdFontCacheMutexSetup()
{
	gdMutexSetup(gdFontCacheMutex);
}

void gdFontCacheMutexShutdown()
{
	gdMutexShutdown(gdFontCacheMutex);
}

int gdFontCacheSetup(void)
{
	if (fontCache) {
		/* Already set up */
		return 0;
	}
	if (FT_Init_FreeType(&library)) {
		return -1;
	}
	fontCache = gdCacheCreate (FONTCACHESIZE, fontTest, fontFetch, fontRelease);
	return 0;
}


/********************************************************************/
/* gdImageStringFT -  render a utf8 string onto a gd image          */

char *
gdImageStringFT (gdImage * im, int *brect, int fg, char *fontlist,
		 double ptsize, double angle, int x, int y, char *string)
{
	return gdImageStringFTEx(im, brect, fg, fontlist, ptsize, angle, x, y, string, 0);
}

char *
gdImageStringFTEx (gdImage * im, int *brect, int fg, char *fontlist, double ptsize, double angle, int x, int y, char *string, gdFTStringExtraPtr strex)
{
	FT_BBox bbox, glyph_bbox;
	FT_Matrix matrix;
	FT_Vector pen, delta, penf;
	FT_Face face;
	FT_Glyph image;
	FT_GlyphSlot slot;
	FT_Bool use_kerning;
	FT_UInt glyph_index, previous;
	double sin_a = sin (angle);
	double cos_a = cos (angle);
	int len, i = 0, ch;
	int x1 = 0, y1 = 0;
	font_t *font;
	fontkey_t fontkey;
	char *next;
	char *tmpstr = NULL;
	int render = (im && (im->trueColor || (fg <= 255 && fg >= -255)));
	FT_BitmapGlyph bm;
	/* 2.0.13: Bob Ostermann: don't force autohint, that's just for testing freetype and doesn't look as good */
	int render_mode = FT_LOAD_DEFAULT;
	int m, mfound;
	/* Now tuneable thanks to Wez Furlong */
	double linespace = LINESPACE;
	/* 2.0.6: put this declaration with the other declarations! */
	/*
	*   make a new tweenColorCache on every call
	*   because caching colormappings between calls
	*   is not safe. If the im-pointer points to a
	*   brand new image, the cache gives out bogus
	*   colorindexes.          -- 27.06.2001 <krisku@arrak.fi>
	*/
	gdCache_head_t  *tc_cache;
	/* Tuneable horizontal and vertical resolution in dots per inch */
	int hdpi, vdpi;

	if (strex && ((strex->flags & gdFTEX_LINESPACE) == gdFTEX_LINESPACE)) {
		linespace = strex->linespacing;
	}
	tc_cache = gdCacheCreate(TWEENCOLORCACHESIZE, tweenColorTest, tweenColorFetch, tweenColorRelease);

	/***** initialize font library and font cache on first call ******/

	gdMutexLock(gdFontCacheMutex);
	if (!fontCache) {
		if (gdFontCacheSetup() != 0) {
			gdCacheDelete(tc_cache);
			gdMutexUnlock(gdFontCacheMutex);
			return "Failure to initialize font library";
		}
	}
	/*****/

	/* 2.0.12: allow explicit specification of the preferred map;
	 * but we still fall back if it is not available.
	 */
	m = gdFTEX_Unicode;
	if (strex && (strex->flags & gdFTEX_CHARMAP)) {
		m = strex->charmap;
	}

	/* get the font (via font cache) */
	fontkey.fontlist = fontlist;
	fontkey.library = &library;
	fontkey.preferred_map = m;
	font = (font_t *) gdCacheGet (fontCache, &fontkey);
	if (!font) {
		gdCacheDelete(tc_cache);
		gdMutexUnlock(gdFontCacheMutex);
		return fontCache->error;
	}
	face = font->face;		/* shortcut */
	slot = face->glyph;		/* shortcut */

	/*
	 * Added hdpi and vdpi to support images at non-screen resolutions, i.e. 300 dpi TIFF,
	 * or 100h x 50v dpi FAX format. 2.0.23.
	 * 2004/02/27 Mark Shackelford, mark.shackelford@acs-inc.com
	 */
	hdpi = GD_RESOLUTION;
	vdpi = GD_RESOLUTION;
	if (strex && (strex->flags & gdFTEX_RESOLUTION)) {
		hdpi = strex->hdpi;
		vdpi = strex->vdpi;
	}

	if (FT_Set_Char_Size(face, 0, (FT_F26Dot6) (ptsize * 64), hdpi, vdpi)) {
		gdCacheDelete(tc_cache);
		gdMutexUnlock(gdFontCacheMutex);
		return "Could not set character size";
	}

	matrix.xx = (FT_Fixed) (cos_a * (1 << 16));
	matrix.yx = (FT_Fixed) (sin_a * (1 << 16));
	matrix.xy = -matrix.yx;
	matrix.yy = matrix.xx;

	penf.x = penf.y = 0;		/* running position of non-rotated string */
	pen.x = pen.y = 0;		/* running position of rotated string */
	bbox.xMin = bbox.xMax = bbox.yMin = bbox.yMax = 0;

	use_kerning = FT_HAS_KERNING (face);
	previous = 0;
	if (fg < 0) {
		render_mode |= FT_LOAD_MONOCHROME;
	}

	/* Try all three types of maps, but start with the specified one */
	mfound = 0;
	for (i = 0; i < 3; i++) {
		switch (m) {
			case gdFTEX_Unicode:
				if (font->have_char_map_unicode) {
					mfound = 1;
				}
				break;
			case gdFTEX_Shift_JIS:
				if (font->have_char_map_sjis) {
					mfound = 1;
				}
				break;
			case gdFTEX_Big5:
				/* This was the 'else' case, we can't really 'detect' it */
				mfound = 1;
				break;
		}
		if (mfound) {
			break;
		}
		m++;
		m %= 3;
	}
	if (!mfound) {
		/* No character set found! */
		gdMutexUnlock(gdFontCacheMutex);
		return "No character set found";
	}

#ifndef JISX0208
	if (font->have_char_map_sjis) {
#endif
		tmpstr = (char *) gdMalloc(BUFSIZ);
		any2eucjp(tmpstr, string, BUFSIZ);
		next = tmpstr;
#ifndef JISX0208
	} else {
		next = string;
	}
#endif

	i = 0;
	while (*next) {
		ch = *next;

		/* carriage returns */
		if (ch == '\r') {
			penf.x = 0;
			x1 = (int)(- penf.y * sin_a + 32) / 64;
			y1 = (int)(- penf.y * cos_a + 32) / 64;
			pen.x = pen.y = 0;
			previous = 0;		/* clear kerning flag */
			next++;
			continue;
		}
		/* newlines */
		if (ch == '\n') {
			if (!*(++next)) break;
			/* 2.0.13: reset penf.x. Christopher J. Grayce */
			penf.x = 0;
			  penf.y -= (long)(face->size->metrics.height * linespace);
			  penf.y = (penf.y - 32) & -64;		/* round to next pixel row */
			  x1 = (int)(- penf.y * sin_a + 32) / 64;
			  y1 = (int)(- penf.y * cos_a + 32) / 64;
			  pen.x = pen.y = 0;
			  previous = 0;		/* clear kerning flag */
			  continue;
		}

/* EAM DEBUG */
#if (defined(FREETYPE_MAJOR) && ((FREETYPE_MAJOR == 2 && ((FREETYPE_MINOR == 1 && FREETYPE_PATCH >= 3) || FREETYPE_MINOR > 1) || FREETYPE_MAJOR > 2)))
		if (font->face->family_name && font->face->charmap->encoding &&
			font->face->charmap->encoding == FT_ENCODING_MS_SYMBOL && strcmp(font->face->family_name, "Symbol") == 0) {
			/* I do not know the significance of the constant 0xf000.
			 * It was determined by inspection of the character codes
			 * stored in Microsoft font symbol.
			 * Added by Pierre (pajoye@php.net):
			 * Convert to the Symbol glyph range only for a Symbol family member
			 */
			len = gdTcl_UtfToUniChar (next, &ch);
			ch |= 0xf000;
			next += len;
		} else
#endif /* Freetype 2.1 or better */
/* EAM DEBUG */

		switch (m) {
			case gdFTEX_Unicode:
				if (font->have_char_map_unicode) {
					/* use UTF-8 mapping from ASCII */
					len = gdTcl_UtfToUniChar(next, &ch);
					next += len;
				}
				break;
			case gdFTEX_Shift_JIS:
				if (font->have_char_map_sjis) {
					unsigned char c;
					int jiscode;
					c = *next;
					if (0xA1 <= c && c <= 0xFE) {
						next++;
						jiscode = 0x100 * (c & 0x7F) + ((*next) & 0x7F);

						ch = (jiscode >> 8) & 0xFF;
						jiscode &= 0xFF;

						if (ch & 1) {
							jiscode += 0x40 - 0x21;
						} else {
							jiscode += 0x9E - 0x21;
						}

						if (jiscode >= 0x7F) {
							jiscode++;
						}
						ch = (ch - 0x21) / 2 + 0x81;
						if (ch >= 0xA0) {
							ch += 0x40;
						}

						ch = (ch << 8) + jiscode;
					} else {
						ch = c & 0xFF;	/* don't extend sign */
					}
					if (*next) next++;
				}
				break;
			case gdFTEX_Big5: {
				/*
				 * Big 5 mapping:
				 * use "JIS-8 half-width katakana" coding from 8-bit characters. Ref:
				 * ftp://ftp.ora.com/pub/examples/nutshell/ujip/doc/japan.inf-032092.sjs
				 */
				ch = (*next) & 0xFF;	/* don't extend sign */
				next++;
				if (ch >= 161	/* first code of JIS-8 pair */
					&& *next) { /* don't advance past '\0' */
					/* TBB: Fix from Kwok Wah On: & 255 needed */
					ch = (ch * 256) + ((*next) & 255);
					next++;
				}
			}
			break;
		}

		/* set rotation transform */
		FT_Set_Transform(face, &matrix, NULL);
		/* Convert character code to glyph index */
		glyph_index = FT_Get_Char_Index(face, ch);

		/* retrieve kerning distance and move pen position */
		if (use_kerning && previous && glyph_index) {
			FT_Get_Kerning(face, previous, glyph_index, ft_kerning_default, &delta);
			pen.x += (int)(delta.x * cos_a);
			pen.y -= (int)(delta.x * sin_a);
			penf.x += delta.x;
		}

		if (brect) { /* only if need brect */
			/* load glyph image into the slot (erase previous one) */
			if (FT_Load_Glyph(face, glyph_index, render_mode | FT_LOAD_IGNORE_TRANSFORM)) {
				if (tmpstr) {
					gdFree(tmpstr);
				}
				gdCacheDelete(tc_cache);
				gdMutexUnlock(gdFontCacheMutex);
				return "Problem loading glyph";
			}

			/* transform glyph image */
			if (FT_Get_Glyph(slot, &image)) {
				if (tmpstr) {
					gdFree(tmpstr);
				}
				gdCacheDelete(tc_cache);
				gdMutexUnlock(gdFontCacheMutex);
				return "Problem loading glyph";
			}

			FT_Glyph_Get_CBox(image, ft_glyph_bbox_gridfit, &glyph_bbox);
			glyph_bbox.xMin += penf.x;
			glyph_bbox.yMin += penf.y;
			glyph_bbox.xMax += penf.x;
			glyph_bbox.yMax += penf.y;
			if (ch == ' ') { /* special case for trailing space */
				glyph_bbox.xMax += slot->metrics.horiAdvance;
			}
			if (!i) { /* if first character, init BB corner values */
				bbox.xMin = glyph_bbox.xMin;
				bbox.yMin = glyph_bbox.yMin;
				bbox.xMax = glyph_bbox.xMax;
				bbox.yMax = glyph_bbox.yMax;
			} else {
				if (bbox.xMin > glyph_bbox.xMin) {
					bbox.xMin = glyph_bbox.xMin;
				}
				if (bbox.yMin > glyph_bbox.yMin) {
					bbox.yMin = glyph_bbox.yMin;
				}
				if (bbox.xMax < glyph_bbox.xMax) {
					bbox.xMax = glyph_bbox.xMax;
				}
				if (bbox.yMax < glyph_bbox.yMax) {
					bbox.yMax = glyph_bbox.yMax;
				}
			}
			i++;
		}

		/* increment (unrotated) pen position */
		penf.x += slot->metrics.horiAdvance;

		if (render) {
			if (!brect || angle != 0) {
				/* reload the rotated glyph (for bbox we needed FT_LOAD_IGNORE_TRANSFORM - bbox is rotated later) */
				FT_Done_Glyph(image);

				/* load glyph image into the slot (erase previous one) */
				if (FT_Load_Glyph(face, glyph_index, render_mode)) {
					if (tmpstr) {
						gdFree(tmpstr);
					}
					gdCacheDelete(tc_cache);
					gdMutexUnlock(gdFontCacheMutex);
					return "Problem loading glyph";
				}

				/* transform glyph image */
				if (FT_Get_Glyph(slot, &image)) {
					if (tmpstr) {
						gdFree(tmpstr);
					}
					gdCacheDelete(tc_cache);
					gdMutexUnlock(gdFontCacheMutex);
					return "Problem loading glyph";
				}
			}

			if (image->format != ft_glyph_format_bitmap && FT_Glyph_To_Bitmap(&image, ft_render_mode_normal, 0, 1)) {
				FT_Done_Glyph(image);
				if (tmpstr) {
					gdFree(tmpstr);
				}
				gdCacheDelete(tc_cache);
				gdMutexUnlock(gdFontCacheMutex);
				return "Problem rendering glyph";
			}

			/* now, draw to our target surface */
			bm = (FT_BitmapGlyph) image;
			gdft_draw_bitmap(tc_cache, im, fg, bm->bitmap, x + x1 + ((pen.x + 31) >> 6) + bm->left, y + y1 + ((pen.y + 31) >> 6) - bm->top);
		}

		/* record current glyph index for kerning */
		previous = glyph_index;

		/* increment pen position */
		pen.x += image->advance.x >> 10;
		pen.y -= image->advance.y >> 10;

		FT_Done_Glyph(image);
	}

	if (brect) { /* only if need brect */
		/* For perfect rounding, must get sin(a + pi/4) and sin(a - pi/4). */
		double d1 = sin (angle + 0.78539816339744830962);
		double d2 = sin (angle - 0.78539816339744830962);

		/* rotate bounding rectangle (at 0, 0) */
		brect[0] = (int) (bbox.xMin * cos_a - bbox.yMin * sin_a);
		brect[1] = (int) (bbox.xMin * sin_a + bbox.yMin * cos_a);
		brect[2] = (int) (bbox.xMax * cos_a - bbox.yMin * sin_a);
		brect[3] = (int) (bbox.xMax * sin_a + bbox.yMin * cos_a);
		brect[4] = (int) (bbox.xMax * cos_a - bbox.yMax * sin_a);
		brect[5] = (int) (bbox.xMax * sin_a + bbox.yMax * cos_a);
		brect[6] = (int) (bbox.xMin * cos_a - bbox.yMax * sin_a);
		brect[7] = (int) (bbox.xMin * sin_a + bbox.yMax * cos_a);

		/* scale, round and offset brect */
		brect[0] = x + gdroundupdown(brect[0], d2 > 0);
		brect[1] = y - gdroundupdown(brect[1], d1 < 0);
		brect[2] = x + gdroundupdown(brect[2], d1 > 0);
		brect[3] = y - gdroundupdown(brect[3], d2 > 0);
		brect[4] = x + gdroundupdown(brect[4], d2 < 0);
		brect[5] = y - gdroundupdown(brect[5], d1 > 0);
		brect[6] = x + gdroundupdown(brect[6], d1 < 0);
		brect[7] = y - gdroundupdown(brect[7], d2 < 0);
	}

	if (tmpstr) {
		gdFree(tmpstr);
	}
	gdCacheDelete(tc_cache);
	gdMutexUnlock(gdFontCacheMutex);
	return (char *) NULL;
}

#endif /* HAVE_LIBFREETYPE */
