/********************************************/
/* gd interface to freetype library         */
/*                                          */
/* John Ellson   ellson@graphviz.org        */
/********************************************/

/**
 * File: FreeType font rendering
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_intern.h"
#include "gdhelpers.h"

/* The bundled PHP font cache can outlive the request which populated it. */
#define GD_FT_CACHE_MALLOC(size) gdPMalloc(size)
#define GD_FT_CACHE_FREE(ptr) gdPFree(ptr)
#define GD_FT_ACCESS(path, mode) VCWD_ACCESS(path, mode)
#define GD_FT_IS_PATH(path)                                                                        \
    (IS_ABSOLUTE_PATH(path, strlen(path)) || strchr(path, '/') || strchr(path, '\\'))

/* 2.0.10: WIN32, not MSWIN32 */
#if !defined(_WIN32) && !defined(_WIN32_WCE)
#include <unistd.h>
#elif defined(_WIN32_WCE)
#include <wce_stdlib.h> /* getenv() */
#include <wce_unistd.h> /* access() */
#define getenv wceex_getenv
#define access wceex_access
#else /* _WIN32_WCE */
#include <io.h>
#ifndef R_OK
#define R_OK 04 /* Needed in Windows */
#endif
#endif

/* number of antialised colors for indexed bitmaps */
#define GD_NUMCOLORS 8

#ifndef HAVE_LIBFREETYPE
BGD_DECLARE(char *)
gdImageStringFTEx(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                  double angle, int x, int y, const char *string, gdFTStringExtraPtr strex)
{
    (void)im;
    (void)brect;
    (void)fg;
    (void)fontlist;
    (void)ptsize;
    (void)angle;
    (void)x;
    (void)y;
    (void)string;
    (void)strex;

    return "libgd was not built with FreeType font support\n";
}

BGD_DECLARE(char *)
gdImageStringFT(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                double angle, int x, int y, const char *string)
{
    (void)im;
    (void)brect;
    (void)fg;
    (void)fontlist;
    (void)ptsize;
    (void)angle;
    (void)x;
    (void)y;
    (void)string;

    return "libgd was not built with FreeType font support\n";
}
#else
#include "gdcache.h"
/* 2.0.16 Christophe Thomas: starting with FreeType 2.1.6, this is
  mandatory, and it has been supported for a long while. */
#ifdef HAVE_FT2BUILD_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_SIZES_H
#else
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftsizes.h>
#endif

#ifdef HAVE_LIBFONTCONFIG
static int fontConfigFlag = 0;

/* translate a fontconfig fontpattern into a fontpath.
        return NULL if OK, else return error string */
static char *font_pattern(char **fontpath, char *fontpattern);
#endif

#ifdef HAVE_LIBFREETYPE
#include "entities.h"
static char *font_path(char **fontpath, char *name_list);
#endif

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
 * fontlist parameter to gdImageStringFT. 2.0.18: space was a
 * poor choice for this.
 */
#define LISTSEPARATOR ";"

/*
 * DEFAULT_FONTPATH and PATHSEPARATOR are host type dependent and
 * are normally set by configure in config.h.  These are just
 * some last resort values that might match some Un*x system
 * if building this version of gd separate from graphviz.
 */

#ifndef DEFAULT_FONTPATH
#if defined(_WIN32)
#define DEFAULT_FONTPATH "C:\\WINDOWS\\FONTS;C:\\WINNT\\FONTS"
#elif defined(__APPLE__) || (defined(__MWERKS__) && defined(macintosh))
#define DEFAULT_FONTPATH "/usr/share/fonts/truetype:/System/Library/Fonts:/Library/Fonts"
#else
/* default fontpath for unix systems  - whatever happened to standards ! */
#define DEFAULT_FONTPATH                                                                           \
    "/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/"                        \
    "usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/"                      \
    "truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/"                       \
    "Type1:/usr/lib/X11/fonts/Type1:/usr/openwin/lib/X11/fonts/Type1"
#endif
#endif

#ifndef PATHSEPARATOR
#if defined(_WIN32)
#define PATHSEPARATOR ";"
#else
#define PATHSEPARATOR ":"
#endif
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE !FALSE
#endif

/**
 * Function: gdImageStringTTF
 *
 * Alias of <gdImageStringFT>.
 */
BGD_DECLARE(char *)
gdImageStringTTF(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                 double angle, int x, int y, const char *string)
{
    /* 2.0.6: valid return */
    return gdImageStringFT(im, brect, fg, fontlist, ptsize, angle, x, y, string);
}

typedef struct {
    char *fontlist; /* key */
    int flags;      /* key */
    char *fontpath;
    FT_Library *library;
    FT_Face face;
} font_t;

typedef struct {
    const char *fontlist; /* key */
    int flags;            /* key */
    FT_Library *library;
} fontkey_t;

typedef struct {
    int pixel;             /* key */
    int bgcolor;           /* key */
    int fgcolor; /* key */ /* -ve means no antialias */
    gdImagePtr im;         /* key */
    int tweencolor;
} tweencolor_t;

typedef struct {
    int pixel;             /* key */
    int bgcolor;           /* key */
    int fgcolor; /* key */ /* -ve means no antialias */
    gdImagePtr im;         /* key */
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

static int comp_entities(const void *e1, const void *e2)
{
    struct entities_s *en1 = (struct entities_s *)e1;
    struct entities_s *en2 = (struct entities_s *)e2;
    return strcmp(en1->name, en2->name);
}

extern int any2eucjp(char *, const char *, unsigned int);

/* Persistent font cache until explicitly cleared */
/* Fonts can be used across multiple images */

/* 2.0.16: thread safety (the font cache is shared) */
gdMutexDeclare(gdFontCacheMutex);
static gdCache_head_t *fontCache = NULL;
static FT_Library library;

#define Tcl_UniChar int
#define TCL_UTF_MAX 3
static int gdTcl_UtfToUniChar(const char *str, Tcl_UniChar *chPtr)
/* str is the UTF8 next character pointer */
/* chPtr is the int for the result */
{
    int byte;
    char entity_name_buf[ENTITY_NAME_LENGTH_MAX + 1];
    char *p;
    struct entities_s key, *res;

    /* HTML4.0 entities in decimal form, e.g. &#197; */
    /* or in hexadecimal form, e.g. &#x6C34; */
    byte = *((unsigned char *)str);
    if (byte == '&') {
        int i, n = 0;

        byte = *((unsigned char *)(str + 1));
        if (byte == '#') {
            byte = *((unsigned char *)(str + 2));
            if (byte == 'x' || byte == 'X') {
                for (i = 3; i < 8; i++) {
                    byte = *((unsigned char *)(str + i));
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
                    byte = *((unsigned char *)(str + i));
                    if (byte >= '0' && byte <= '9') {
                        n = (n * 10) + (byte - '0');
                    } else {
                        break;
                    }
                }
            }
            if (byte == ';') {
                *chPtr = (Tcl_UniChar)n;
                return ++i;
            }
        } else {
            key.name = p = entity_name_buf;
            for (i = 1; i <= 1 + ENTITY_NAME_LENGTH_MAX; i++) {
                byte = *((unsigned char *)(str + i));
                if (byte == '\0')
                    break;
                if (byte == ';') {
                    *p++ = '\0';
                    res = bsearch(&key, entities, NR_OF_ENTITIES, sizeof(entities[0]),
                                  *comp_entities);
                    if (res) {
                        *chPtr = (Tcl_UniChar)res->value;
                        return ++i;
                    }
                    break;
                }
                *p++ = byte;
            }
        }
    }

    /* Unroll 1 to 3 byte UTF-8 sequences, use loop to handle longer ones. */
    byte = *((unsigned char *)str);
#ifdef JISX0208
    if (0xA1 <= byte && byte <= 0xFE) {
        int ku, ten;

        ku = (byte & 0x7F) - 0x20;
        ten = (str[1] & 0x7F) - 0x20;
        if ((ku < 1 || ku > 92) || (ten < 1 || ten > 94)) {
            *chPtr = (Tcl_UniChar)byte;
            return 1;
        }

        *chPtr = (Tcl_UniChar)UnicodeTbl[ku - 1][ten - 1];
        return 2;
    } else
#endif /* JISX0208 */
        if (byte < 0xC0) {
            /* Handles properly formed UTF-8 characters between
             * 0x01 and 0x7F.  Also treats \0 and naked trail
             * bytes 0x80 to 0xBF as valid characters representing
             * themselves.
             */

            *chPtr = (Tcl_UniChar)byte;
            return 1;
        } else if (byte < 0xE0) {
            if ((str[1] & 0xC0) == 0x80) {
                /* Two-byte-character lead-byte followed by a trail-byte. */
                *chPtr = (Tcl_UniChar)(((byte & 0x1F) << 6) | (str[1] & 0x3F));
                return 2;
            }
            /*
             * A two-byte-character lead-byte not followed by trail-byte
             * represents itself.
             */

            *chPtr = (Tcl_UniChar)byte;
            return 1;
        } else if (byte < 0xF0) {
            if (((str[1] & 0xC0) == 0x80) && ((str[2] & 0xC0) == 0x80)) {
                /* Three-byte-character lead byte followed by two trail bytes. */
                *chPtr =
                    (Tcl_UniChar)(((byte & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F));
                return 3;
            }
            /* A three-byte-character lead-byte not followed by two trail-bytes represents itself.
             */
            *chPtr = (Tcl_UniChar)byte;
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

    *chPtr = (Tcl_UniChar)byte;
    return 1;
}

#ifdef HAVE_LIBRAQM
#include <raqm.h>
#else
#define RAQM_VERSION_ATLEAST(a, b, c) 0
#endif

typedef struct {
    unsigned int index;
    FT_Pos x_advance;
    FT_Pos y_advance;
    FT_Pos x_offset;
    FT_Pos y_offset;
    uint32_t cluster;
} glyphInfo;

static ssize_t textLayout(uint32_t *text, size_t len, FT_Face face, gdFTStringExtraPtr strex,
                          glyphInfo **glyph_info)
{
    size_t count;
    glyphInfo *info;

    if (!len) {
        return 0;
    }

#ifdef HAVE_LIBRAQM
    size_t i;
    raqm_glyph_t *glyphs;
    raqm_t *rq = raqm_create();

    if (!rq || !raqm_set_text(rq, text, len) || !raqm_set_freetype_face(rq, face) ||
        !raqm_set_par_direction(rq, RAQM_DIRECTION_DEFAULT) || !raqm_layout(rq)) {
        raqm_destroy(rq);
        return -1;
    }

    glyphs = raqm_get_glyphs(rq, &count);
    if (!glyphs) {
        raqm_destroy(rq);
        return -1;
    }

    info = (glyphInfo *)gdMalloc(sizeof(glyphInfo) * count);
    if (!info) {
        raqm_destroy(rq);
        return -1;
    }

    for (i = 0; i < count; i++) {
        info[i].index = glyphs[i].index;
        info[i].x_offset = glyphs[i].x_offset;
        info[i].y_offset = glyphs[i].y_offset;
        info[i].x_advance = glyphs[i].x_advance;
        info[i].y_advance = glyphs[i].y_advance;
        info[i].cluster = glyphs[i].cluster;
    }

    raqm_destroy(rq);
#else
    FT_UInt glyph_index = 0, previous = 0;
    FT_Vector delta;
    FT_Error err;
    info = (glyphInfo *)gdMalloc(sizeof(glyphInfo) * len);
    if (!info) {
        return -1;
    }
    for (count = 0; count < len; count++) {
        /* Convert character code to glyph index */
        glyph_index = FT_Get_Char_Index(face, text[count]);

        /* retrieve kerning distance */
        if (!(strex && (strex->flags & gdFTEX_DISABLE_KERNING)) && !FT_IS_FIXED_WIDTH(face) &&
            FT_HAS_KERNING(face) && previous && glyph_index)
            FT_Get_Kerning(face, previous, glyph_index, ft_kerning_default, &delta);
        else
            delta.x = delta.y = 0;

        err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        if (err) {
            gdFree(info);
            return -1;
        }
        info[count].index = glyph_index;
        info[count].x_offset = 0;
        info[count].y_offset = 0;
        if (delta.x != 0)
            info[count - 1].x_advance += delta.x;
        info[count].x_advance = face->glyph->metrics.horiAdvance;
        info[count].y_advance = 0;
        info[count].cluster = count;

        /* carriage returns or newlines */
        if (text[count] == '\r' || text[count] == '\n')
            previous = 0; /* clear kerning flag */
        else
            previous = glyph_index;
    }
#endif

    *glyph_info = info;
    if (count <= (size_t)SSIZE_MAX) {
        return (ssize_t)count;
    }
    return -1;
}

/********************************************************************/
/* font cache functions                                             */

static int fontTest(void *element, void *key)
{
    font_t *a = (font_t *)element;
    fontkey_t *b = (fontkey_t *)key;

    return a->flags == b->flags && strcmp(a->fontlist, b->fontlist) == 0;
}

#ifdef HAVE_LIBFONTCONFIG
static int useFontConfig(int flag)
{
    if (fontConfigFlag) {
        return (!(flag & gdFTEX_FONTPATHNAME));
    }
    return flag & gdFTEX_FONTCONFIG;
}
#endif

static void *fontFetch(char **error, void *key)
{
    font_t *a;
    fontkey_t *b = (fontkey_t *)key;
    char *fontpath = NULL;
    char *suffix;
    FT_Error err;
    const unsigned int b_font_list_len = strlen(b->fontlist);

    *error = NULL;

    a = (font_t *)GD_FT_CACHE_MALLOC(sizeof(font_t));
    if (!a) {
        *error = "could not alloc font cache entry";
        return NULL;
    }

    a->fontlist = (char *)GD_FT_CACHE_MALLOC(b_font_list_len + 1);
    if (a->fontlist == NULL) {
        GD_FT_CACHE_FREE(a);
        *error = "could not alloc full list of fonts";
        return NULL;
    }
    memcpy(a->fontlist, b->fontlist, b_font_list_len);
    a->fontlist[b_font_list_len] = 0;

    a->flags = b->flags;
    a->library = b->library;
    a->fontpath = NULL;

#ifdef HAVE_LIBFONTCONFIG
    if (!useFontConfig(b->flags))
        *error = font_path(&fontpath, a->fontlist);
    else
        *error = font_pattern(&fontpath, a->fontlist);
#else
    *error = font_path(&fontpath, a->fontlist);
#endif /* HAVE_LIBFONTCONFIG */
    if (*error || !fontpath || !fontpath[0]) {
        GD_FT_CACHE_FREE(a->fontlist);
        gdFree(fontpath);
        GD_FT_CACHE_FREE(a);

        if (!*error)
            *error = "font_path() returned an empty font pathname";

        return NULL;
    }
    a->fontpath = (char *)GD_FT_CACHE_MALLOC(strlen(fontpath) + 1);
    if (!a->fontpath) {
        GD_FT_CACHE_FREE(a->fontlist);
        gdFree(fontpath);
        GD_FT_CACHE_FREE(a);
        *error = "could not alloc font path";
        return NULL;
    }
    memcpy(a->fontpath, fontpath, strlen(fontpath) + 1);
    gdFree(fontpath);

    err = FT_New_Face(*b->library, a->fontpath, 0, &a->face);

    /* Read kerning metrics for Postscript fonts. */
    if (!err &&
        ((suffix = strstr(a->fontpath, ".pfa")) || (suffix = strstr(a->fontpath, ".pfb"))) &&
        ((strcpy(suffix, ".afm") && (GD_FT_ACCESS(a->fontpath, R_OK) == 0)) ||
         (strcpy(suffix, ".pfm") && (GD_FT_ACCESS(a->fontpath, R_OK) == 0)))) {
        err = FT_Attach_File(a->face, a->fontpath);
    }

    if (err) {
        GD_FT_CACHE_FREE(a->fontlist);
        GD_FT_CACHE_FREE(a->fontpath);
        GD_FT_CACHE_FREE(a);
        *error = "Could not read font";
        return NULL;
    }

    return (void *)a;
}

static void fontRelease(void *element)
{
    font_t *a = (font_t *)element;

    FT_Done_Face(a->face);
    GD_FT_CACHE_FREE(a->fontlist);
    GD_FT_CACHE_FREE(a->fontpath);
    GD_FT_CACHE_FREE((char *)element);
}

/********************************************************************/
/* tweencolor cache functions                                            */

static int tweenColorTest(void *element, void *key)
{
    tweencolor_t *a = (tweencolor_t *)element;
    tweencolorkey_t *b = (tweencolorkey_t *)key;

    return (a->pixel == b->pixel && a->bgcolor == b->bgcolor && a->fgcolor == b->fgcolor &&
            a->im == b->im);
}

/*
 * Computes a color in im's color table that is part way between
 * the background and foreground colors proportional to the gray
 * pixel value in the range 0-GD_NUMCOLORS. The fg and bg colors must already
 * be in the color table for palette images. For truecolor images the
 * returned value simply has an alpha component and gdImageAlphaBlend
 * does the work so that text can be alpha blended across a complex
 * background (TBB; and for real in 2.0.2).
 */
static void *tweenColorFetch(char **error, void *key)
{
    tweencolor_t *a;
    tweencolorkey_t *b = (tweencolorkey_t *)key;
    int pixel, npixel, bg, fg;
    gdImagePtr im;

    (void)error;

    a = (tweencolor_t *)gdMalloc(sizeof(tweencolor_t));
    if (!a) {
        return NULL;
    }

    pixel = a->pixel = b->pixel;
    bg = a->bgcolor = b->bgcolor;
    fg = a->fgcolor = b->fgcolor;
    im = a->im = b->im;

    /* if fg is specified by a negative color idx, then don't antialias */
    if (fg < 0) {
        if ((pixel + pixel) >= GD_NUMCOLORS)
            a->tweencolor = -fg;
        else
            a->tweencolor = bg;
    } else {
        npixel = GD_NUMCOLORS - pixel;
        if (im->trueColor) {
            /* 2.0.1: use gdImageSetPixel to do the alpha blending work,
               or to just store the alpha level. All we have to do here
               is incorporate our knowledge of the percentage of this
               pixel that is really "lit" by pushing the alpha value
               up toward transparency in edge regions. */
            a->tweencolor = gdTrueColorAlpha(
                gdTrueColorGetRed(fg), gdTrueColorGetGreen(fg), gdTrueColorGetBlue(fg),
                gdAlphaMax - (gdTrueColorGetAlpha(fg) * pixel / GD_NUMCOLORS));
        } else {
            a->tweencolor =
                gdImageColorResolve(im, (pixel * im->red[fg] + npixel * im->red[bg]) / GD_NUMCOLORS,
                                    (pixel * im->green[fg] + npixel * im->green[bg]) / GD_NUMCOLORS,
                                    (pixel * im->blue[fg] + npixel * im->blue[bg]) / GD_NUMCOLORS);
        }
    }
    return (void *)a;
}

static void tweenColorRelease(void *element) { gdFree((char *)element); }

/* draw_bitmap - transfers glyph bitmap to GD image */
static char *gdft_draw_bitmap(gdCache_head_t *tc_cache, gdImagePtr im, int fg, FT_Bitmap bitmap,
                              int pen_x, int pen_y)
{
    unsigned char *pixel = NULL;
    int *tpixel = NULL;
    int opixel;
    int x, y, pc, pcr;
    unsigned int col;
    unsigned int row;

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
            if ((y > im->cy2) || (y < im->cy1))
                continue;
            for (col = 0; col < bitmap.width; col++, pc++) {
                int level;
                if (bitmap.pixel_mode == ft_pixel_mode_grays) {
                    /*
                     * Scale to 128 levels of alpha for gd use.
                     * alpha 0 is opacity, so be sure to invert at the end
                     */
                    level = (bitmap.buffer[pc] * gdAlphaMax / (bitmap.num_grays - 1));
                } else if (bitmap.pixel_mode == ft_pixel_mode_mono) {
                    /* 2.0.5: mode_mono fix from Giuliano Pochini */
                    level = ((bitmap.buffer[(col >> 3) + pcr]) & (1 << (~col & 0x07)))
                                ? gdAlphaTransparent
                                : gdAlphaOpaque;
                } else {
                    return "Unsupported ft_pixel_mode";
                }
                if (level == 0) /* if background */
                    continue;

                if ((fg >= 0) && (im->trueColor)) {
                    /* Consider alpha in the foreground color itself to be an
                       upper bound on how opaque things get, when truecolor is
                       available. Without truecolor this results in far too many
                       color indexes. */
                    level = level * (gdAlphaMax - gdTrueColorGetAlpha(fg)) / gdAlphaMax;
                }
                level = gdAlphaMax - level; /* inverting to get alpha */
                x = pen_x + col;
                /* clip if out of bounds */
                /* 2.0.16: clip to clipping rectangle, Matt McNabb */
                if ((x > im->cx2) || (x < im->cx1))
                    continue;
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
                            *tpixel = gdAlphaBlend(opixel, (level << 24) + (fg & 0xFFFFFF));
                        } else {
                            *tpixel = (level << 24) + (fg & 0xFFFFFF);
                        }
                    } else {
                        *tpixel = (level << 24) + (fg & 0xFFFFFF);
                    }
                }
            }
        }
        return (char *)NULL;
    }
    /* Non-truecolor case, restored to its more or less original form */
    for (row = 0; row < bitmap.rows; row++) {
        int pcr;
        pc = row * bitmap.pitch;
        pcr = pc;
        if (bitmap.pixel_mode == ft_pixel_mode_mono)
            pc *= 8; /* pc is measured in bits for monochrome images */

        y = pen_y + row;

        /* clip if out of bounds */
        if (y > im->cy2 || y < im->cy1)
            continue;

        for (col = 0; col < bitmap.width; col++, pc++) {
            if (bitmap.pixel_mode == ft_pixel_mode_grays) {
                /*
                 * Round to GD_NUMCOLORS levels of antialiasing for
                 * index color images since only 256 colors are
                 * available.
                 */
                tc_key.pixel = ((bitmap.buffer[pc] * GD_NUMCOLORS) + bitmap.num_grays / 2) /
                               (bitmap.num_grays - 1);
            } else if (bitmap.pixel_mode == ft_pixel_mode_mono) {
                /* 2.0.5: mode_mono fix from Giuliano Pochini */
                tc_key.pixel =
                    ((bitmap.buffer[(col >> 3) + pcr]) & (1 << (~col & 0x07))) ? GD_NUMCOLORS : 0;
            } else {
                return "Unsupported ft_pixel_mode";
            }
            if (tc_key.pixel == 0) /* if background */
                continue;

            x = pen_x + col;

            /* clip if out of bounds */
            if (x > im->cx2 || x < im->cx1)
                continue;
            /* get pixel location in gd buffer */
            pixel = &im->pixels[y][x];
            if (tc_key.pixel == GD_NUMCOLORS) {
                /* use fg color directly. gd 2.0.2: watch out for
                   negative indexes (thanks to David Marwood). */
                *pixel = (fg < 0) ? -fg : fg;
            } else {
                /* find antialised color */

                tc_key.bgcolor = *pixel;
                tc_elem = (tweencolor_t *)gdCacheGet(tc_cache, &tc_key);
                if (!tc_elem)
                    return tc_cache->error;
                *pixel = tc_elem->tweencolor;
            }
        }
    }
    return (char *)NULL;
}

/**
 * Function: gdFreeFontCache
 *
 * Alias of <gdFontCacheShutdown>.
 */
BGD_DECLARE(void) gdFreeFontCache() { gdFontCacheShutdown(); }

BGD_DECLARE(void) gdFontCacheMutexSetup(void) { gdMutexSetup(gdFontCacheMutex); }

BGD_DECLARE(void) gdFontCacheMutexShutdown(void) { gdMutexShutdown(gdFontCacheMutex); }

/**
 * Function: gdFontCacheShutdown
 *
 * Shut down the font cache and free the allocated resources.
 *
 * Important:
 *  This function has to be called whenever FreeType operations have been
 *  invoked, to avoid resource leaks. It doesn't harm to call this function
 *  multiple times.
 */
BGD_DECLARE(void) gdFontCacheShutdown()
{
    if (fontCache) {
        gdMutexLock(gdFontCacheMutex);
        gdCacheDelete(fontCache);
        /* 2.0.16: Gustavo Scotti: make sure we don't free this twice */
        fontCache = 0;
        gdMutexUnlock(gdFontCacheMutex);
        FT_Done_FreeType(library);
    }
}

/**
 * Function: gdImageStringFT
 *
 * Render an UTF-8 string onto a gd image.
 *
 * Parameters:
 *	im       - The image to draw onto.
 *  brect    - The bounding rectangle as array of 8 integers where each pair
 *             represents the x- and y-coordinate of a point. The points
 *             specify the lower left, lower right, upper right and upper left
 *             corner.
 *	fg       - The font color.
 *	fontlist - The semicolon delimited list of font filenames to look for.
 *	ptsize   - The height of the font in typographical points (pt).
 *	angle    - The angle in radian to rotate the font counter-clockwise.
 *	x        - The x-coordinate of the basepoint (roughly the lower left corner)
 *of the first letter. y        - The y-coordinate of the basepoint (roughly the
 *lower left corner) of the first letter. string   - The string to render.
 *
 * Variant:
 *  - <gdImageStringFTEx>
 *
 * See also:
 *  - <gdImageString>
 */
BGD_DECLARE(char *)
gdImageStringFT(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                double angle, int x, int y, const char *string)
{
    return gdImageStringFTEx(im, brect, fg, fontlist, ptsize, angle, x, y, string, 0);
}

/**
 * Function: gdFontCacheSetup
 *
 * Set up the font cache.
 *
 * This is called automatically from the string rendering functions, if it
 * has not already been called. So there's no need to call this function
 * explicitly.
 */
BGD_DECLARE(int) gdFontCacheSetup(void)
{
    if (fontCache) {
        /* Already set up */
        return 0;
    }
    if (FT_Init_FreeType(&library)) {
        return -1;
    }
    fontCache = gdCacheCreate(FONTCACHESIZE, fontTest, fontFetch, fontRelease);
    if (!fontCache) {
        return -2;
    }
    return 0;
}

/*
  Function: gdImageStringFTEx

  gdImageStringFTEx extends the capabilities of gdImageStringFT by
  providing a way to pass additional parameters.

  If the strex parameter is not null, it must point to a
  gdFTStringExtra structure. As of gd 2.0.5, this structure is defined
  as follows:
  (start code)

  typedef struct {
          // logical OR of gdFTEX_ values
          int flags;

          // fine tune line spacing for '\n'
          double linespacing;

          // Preferred character mapping
          int charmap;

          // Rendering resolution
          int hdpi;
          int vdpi;
          char *xshow;
          char *fontpath;
  } gdFTStringExtra, *gdFTStringExtraPtr;

  (end code)

  To output multiline text with a specific line spacing, include
  gdFTEX_LINESPACE in the setting of flags:

        > flags |= gdFTEX_LINESPACE;

  And also set linespacing to the desired spacing, expressed as a
  multiple of the font height. Thus a line spacing of 1.0 is the
  minimum to guarantee that lines of text do not collide.

  If gdFTEX_LINESPACE is not present, or strex is null, or
  gdImageStringFT is called, linespacing defaults to 1.05.

  To specify a preference for Unicode, Shift_JIS Big5 character
  encoding, set or To output multiline text with a specific line
  spacing, include gdFTEX_CHARMAP in the setting of flags:

        > flags |= gdFTEX_CHARMAP;

  And set charmap to the desired value, which can be any of
  gdFTEX_Unicode, gdFTEX_Shift_JIS, gdFTEX_Big5, or
  gdFTEX_Adobe_Custom. If you do not specify a preference, Unicode
  will be tried first. If the preferred character mapping is not found
  in the font, other character mappings are attempted.

  GD operates on the assumption that the output image will be rendered
  to a computer screen. By default, gd passes a resolution of 96 dpi
  to the freetype text rendering engine. This influences the "hinting"
  decisions made by the renderer. To specify a different resolution,
  set hdpi and vdpi accordingly (in dots per inch) and add
  gdFTEX_RESOLUTION to flags:

        > flags | gdFTEX_RESOLUTION;

  GD 2.0.29 and later will normally attempt to apply kerning tables,
  if fontconfig is available, to adjust the relative positions of
  consecutive characters more ideally for that pair of
  characters. This can be turn off by specifying the
  gdFTEX_DISABLE_KERNING flag:

        > flags | gdFTEX_DISABLE_KERNING;

  GD 2.0.29 and later can return a vector of individual character
  position advances, occasionally useful in applications that must
  know exactly where each character begins. This is returned in the
  xshow element of the gdFTStringExtra structure if the gdFTEX_XSHOW
  flag is set:

        > flags | gdFTEX_XSHOW;

  The caller is responsible for calling gdFree() on the xshow element
  after the call if gdFTEX_XSHOW is set.

  GD 2.0.29 and later can also return the path to the actual font file
  used if the gdFTEX_RETURNFONTPATHNAME flag is set. This is useful
  because GD 2.0.29 and above are capable of selecting a font
  automatically based on a fontconfig font pattern when fontconfig is
  available. This information is returned in the fontpath element of
  the gdFTStringExtra structure.

        > flags | gdFTEX_RETURNFONTPATHNAME;

  The caller is responsible for calling gdFree() on the fontpath
  element after the call if gdFTEX_RETURNFONTPATHNAME is set.

  GD 2.0.29 and later can use fontconfig to resolve font names,
  including fontconfig patterns, if the gdFTEX_FONTCONFIG flag is
  set. As a convenience, this behavior can be made the default by
  calling <gdFTUseFontConfig> with a nonzero value. In that situation it
  is not necessary to set the gdFTEX_FONTCONFIG flag on every call;
  however explicit font path names can still be used if the
  gdFTEX_FONTPATHNAME flag is set:

        > flags | gdFTEX_FONTPATHNAME;

  Unless <gdFTUseFontConfig> has been called with a nonzero value, GD
  2.0.29 and later will still expect the fontlist argument to the
  freetype text output functions to be a font file name or list
  thereof as in previous versions. If you do not wish to make
  fontconfig the default, it is still possible to force the use of
  fontconfig for a single call to the freetype text output functions
  by setting the gdFTEX_FONTCONFIG flag:

        > flags | gdFTEX_FONTCONFIG;

  GD 2.0.29 and above can use fontconfig to resolve font names,
  including fontconfig patterns, if the gdFTEX_FONTCONFIG flag is
  set. As a convenience, this behavior can be made the default by
  calling <gdFTUseFontConfig> with a nonzero value. In that situation it
  is not necessary to set the gdFTEX_FONTCONFIG flag on every call;
  however explicit font path names can still be used if the
  gdFTEX_FONTPATHNAME flag is set:

        > flags | gdFTEX_FONTPATHNAME;

  For more information, see <gdImageStringFT>.
*/

/* the platform-independent resolution used for size and position calculations
 */
/*    the size of the error introduced by rounding is affected by this number */
#define METRIC_RES 300

BGD_DECLARE(char *)
gdImageStringFTEx(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                  double angle, int x, int y, const char *string, gdFTStringExtraPtr strex)
{
    FT_Matrix matrix;
    FT_Vector penf, oldpenf, total_min = {0, 0}, total_max = {0, 0}, glyph_min, glyph_max;
    FT_Face face;
    FT_CharMap charmap = NULL;
    FT_CharMap fallback_charmap = NULL;
    FT_Glyph image;
    FT_GlyphSlot slot;
    FT_Error err;
    FT_UInt glyph_index;
    double sin_a = sin(angle);
    double cos_a = cos(angle);
    int i, ch;
    font_t *font;
    fontkey_t fontkey;
    const char *next;
    char *tmpstr = 0;
    uint32_t *text;
    glyphInfo *info = NULL;
    ssize_t count;
    int render = (im && (im->trueColor || (fg <= 255 && fg >= -255)));
    FT_BitmapGlyph bm;
    /* 2.0.13: Bob Ostermann: don't force autohint, that's just for testing
       freetype and doesn't look as good */
    int render_mode = FT_LOAD_DEFAULT;
    int encoding, encodingfound;
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
    gdCache_head_t *tc_cache;
    /* Tuneable horizontal and vertical resolution in dots per inch */
    int hdpi, vdpi, horiAdvance, vertAdvance, xshow_alloc = 0, xshow_pos = 0;
    FT_Size platform_specific = NULL, platform_independent;

    if (strex) {
        if ((strex->flags & gdFTEX_LINESPACE) == gdFTEX_LINESPACE) {
            linespace = strex->linespacing;
        }
    }
    tc_cache =
        gdCacheCreate(TWEENCOLORCACHESIZE, tweenColorTest, tweenColorFetch, tweenColorRelease);

    /***** initialize font library and font cache on first call ******/
    if (!fontCache) {
        if (gdFontCacheSetup() != 0) {
            gdCacheDelete(tc_cache);
            return "Failure to initialize font library";
        }
    }
    /*****/
    gdMutexLock(gdFontCacheMutex);
    /* get the font (via font cache) */
    fontkey.fontlist = fontlist;
    if (strex)
        fontkey.flags = strex->flags & (gdFTEX_FONTPATHNAME | gdFTEX_FONTCONFIG);
    else
        fontkey.flags = 0;
    fontkey.library = &library;
    font = (font_t *)gdCacheGet(fontCache, &fontkey);
    if (!font) {
        gdCacheDelete(tc_cache);
        gdMutexUnlock(gdFontCacheMutex);
        return fontCache->error;
    }
    face = font->face;  /* shortcut */
    slot = face->glyph; /* shortcut */

    if (brect) {
        total_min.x = total_min.y = 0;
        total_max.x = total_max.y = 0;
    }

    /*
     * Added hdpi and vdpi to support images at non-screen resolutions, i.e. 300
     * dpi TIFF, or 100h x 50v dpi FAX format. 2.0.23. 2004/02/27 Mark
     * Shackelford, mark.shackelford@acs-inc.com
     */
    hdpi = GD_RESOLUTION;
    vdpi = GD_RESOLUTION;
    encoding = gdFTEX_Unicode;
    if (strex) {
        if (strex->flags & gdFTEX_RESOLUTION) {
            hdpi = strex->hdpi;
            vdpi = strex->vdpi;
        }
        if (strex->flags & gdFTEX_XSHOW) {
            strex->xshow = NULL;
        }
        /* 2.0.12: allow explicit specification of the preferred map;
           but we still fall back if it is not available. */
        if (strex->flags & gdFTEX_CHARMAP) {
            encoding = strex->charmap;
        }
        /* 2.0.29: we can return the font path if desired */
        if (strex->flags & gdFTEX_RETURNFONTPATHNAME) {
            const unsigned int fontpath_len = strlen(font->fontpath);

            strex->fontpath = (char *)gdMalloc(fontpath_len + 1);
            if (strex->fontpath == NULL) {
                gdCacheDelete(tc_cache);
                gdMutexUnlock(gdFontCacheMutex);
                return "could not alloc full list of fonts";
            }
            memcpy(strex->fontpath, font->fontpath, fontpath_len);
            strex->fontpath[fontpath_len] = 0;
        }
    }

    matrix.xx = (FT_Fixed)(cos_a * (1 << 16));
    matrix.yx = (FT_Fixed)(sin_a * (1 << 16));
    matrix.xy = -matrix.yx;
    matrix.yy = matrix.xx;

    /* Keep RAQM layout in text space; apply transform later only for
     * the render glyph load. */
#ifdef HAVE_LIBRAQM
    FT_Set_Transform(face, NULL, NULL);
#else
    /* set rotation transform */
    FT_Set_Transform(face, &matrix, NULL);
#endif

    FT_New_Size(face, &platform_independent);
    FT_Activate_Size(platform_independent);
    if (FT_Set_Char_Size(face, 0, (FT_F26Dot6)(ptsize * 64), METRIC_RES, METRIC_RES)) {
        gdCacheDelete(tc_cache);
        gdMutexUnlock(gdFontCacheMutex);
        return "Could not set character size";
    }

    if (render) {
        FT_New_Size(face, &platform_specific);
        FT_Activate_Size(platform_specific);
        if (FT_Set_Char_Size(face, 0, (FT_F26Dot6)(ptsize * 64), hdpi, vdpi)) {
            gdCacheDelete(tc_cache);
            gdMutexUnlock(gdFontCacheMutex);
            return "Could not set character size";
        }
    }

    if (fg < 0)
        render_mode |= FT_LOAD_MONOCHROME;

    /* find requested charmap */
    encodingfound = 0;
    for (i = 0; i < face->num_charmaps; i++) {
        charmap = face->charmaps[i];

        if (encoding == gdFTEX_Unicode) {
            if (charmap->encoding == FT_ENCODING_UNICODE) {
                encodingfound++;
                break;
            }
            if (!fallback_charmap && (charmap->encoding == FT_ENCODING_MS_SYMBOL ||
                                      charmap->encoding == FT_ENCODING_ADOBE_CUSTOM ||
                                      charmap->encoding == FT_ENCODING_ADOBE_STANDARD)) {
                fallback_charmap = charmap;
            }
        } else if (encoding == gdFTEX_Adobe_Custom) {
            if (charmap->encoding == FT_ENCODING_ADOBE_CUSTOM) {
                encodingfound++;
                break;
            }
            if (!fallback_charmap && charmap->encoding == FT_ENCODING_APPLE_ROMAN) {
                fallback_charmap = charmap;
            }
        } else if (encoding == gdFTEX_Big5) {
            /* renamed sometime after freetype-2.1.4 */
#ifndef FT_ENCODING_BIG5
#define FT_ENCODING_BIG5 FT_ENCODING_MS_BIG5
#endif
            if (charmap->encoding == FT_ENCODING_BIG5) {
                encodingfound++;
                break;
            }
        } else if (encoding == gdFTEX_Shift_JIS) {
            /* renamed sometime after freetype-2.1.4 */
#ifndef FT_ENCODING_SJIS
#define FT_ENCODING_SJIS FT_ENCODING_MS_SJIS
#endif
            if (charmap->encoding == FT_ENCODING_SJIS) {
                encodingfound++;
                break;
            }
        }
    }
    if (!encodingfound && fallback_charmap) {
        charmap = fallback_charmap;
        encodingfound = 1;
    }
    if (encodingfound) {
        FT_Set_Charmap(face, charmap);
    } else {
        /* No character set found! */
        gdCacheDelete(tc_cache);
        gdMutexUnlock(gdFontCacheMutex);
        return "No character set found";
    }

#ifndef JISX0208
    if (encoding == gdFTEX_Shift_JIS) {
#endif
        if ((tmpstr = (char *)gdMalloc(BUFSIZ))) {
            any2eucjp(tmpstr, string, BUFSIZ);
            next = tmpstr;
        } else {
            next = string;
        }
#ifndef JISX0208
    } else {
        next = string;
    }
#endif

    oldpenf.x = oldpenf.y = 0; /* for postscript xshow operator */
    penf.x = penf.y = 0;       /* running position of non-rotated glyphs */
    text = (uint32_t *)gdCalloc(sizeof(uint32_t), strlen(next));
    i = 0;
    while (*next) {
        int len;
        ch = *next;
        switch (encoding) {
        case gdFTEX_Unicode: {
            /* use UTF-8 mapping from ASCII */
            len = gdTcl_UtfToUniChar(next, &ch);
            /* EAM DEBUG */
            /* TBB: get this exactly right: 2.1.3 *or better*, all possible
             * cases. */
            /* 2.0.24: David R. Morrison: use the more complete ifdef here. */
            if (charmap->encoding == FT_ENCODING_MS_SYMBOL) {
                /* I do not know the significance of the constant 0xf000. */
                /* It was determined by inspection of the character codes */
                /* stored in Microsoft font symbol.ttf                    */
                ch |= 0xf000;
            }
            /* EAM DEBUG */
            next += len;
        } break;
        case gdFTEX_Shift_JIS: {
            unsigned char c;
            int jiscode;
            c = *next;
            if (0xA1 <= c && c <= 0xFE) {
                next++;
                jiscode = 0x100 * (c & 0x7F) + ((*next) & 0x7F);

                ch = (jiscode >> 8) & 0xFF;
                jiscode &= 0xFF;

                if (ch & 1)
                    jiscode += 0x40 - 0x21;
                else
                    jiscode += 0x9E - 0x21;

                if (jiscode >= 0x7F)
                    jiscode++;
                ch = (ch - 0x21) / 2 + 0x81;
                if (ch >= 0xA0)
                    ch += 0x40;

                ch = (ch << 8) + jiscode;
            } else {
                ch = c & 0xFF; /* don't extend sign */
            }
            if (*next)
                next++;
        } break;
        case gdFTEX_Big5: {
            /*
             * Big 5 mapping:
             * use "JIS-8 half-width katakana" coding from 8-bit characters.
             * Ref:
             * ftp://ftp.ora.com/pub/examples/nutshell/ujip/doc/japan.inf-032092.sjs
             */
            ch = (*next) & 0xFF; /* don't extend sign */
            next++;
            if (ch >= 161 /* first code of JIS-8 pair */
                && *next) {
                /* don't advance past '\0' */
                /* TBB: Fix from Kwok Wah On: & 255 needed */
                ch = (ch * 256) + ((*next) & 255);
                next++;
            }
        } break;

        case gdFTEX_Adobe_Custom:
        default:
            ch &= 0xFF;
            next++;
            break;
        }
        text[i] = ch;
        i++;
    }

    FT_Activate_Size(platform_independent);

    count = textLayout(text, i, face, strex, &info);

    if (count < 0) {
        gdFree(text);
        gdFree(tmpstr);
        gdCacheDelete(tc_cache);
        gdMutexUnlock(gdFontCacheMutex);
        if (info) {
            gdFree(info);
        }
        return "Problem doing text layout";
    }

    for (i = 0; i < count; i++) {
        FT_Activate_Size(platform_independent);

        ch = text[info[i].cluster];

        /* carriage returns */
        if (ch == '\r') {
            penf.x = 0;
            continue;
        }

        /* newlines */
        if (ch == '\n') {
            /* 2.0.13: reset penf.x. Christopher J. Grayce */
            penf.x = 0;
            penf.y += linespace * ptsize * 64 * METRIC_RES / 72;
            penf.y &= ~63; /* round down to 1/METRIC_RES */
            continue;
        }

        glyph_index = info[i].index;
        /* When we know the position of the second or subsequent character,
        save the (kerned) advance from the preceeding character in the
        xshow vector */
        if (i && strex && (strex->flags & gdFTEX_XSHOW)) {
            /* make sure we have enough allocation for two numbers
            so we don't have to recheck for the terminating number */
            if (!xshow_alloc) {
                xshow_alloc = 100;
                strex->xshow = gdMalloc(xshow_alloc);
                if (!strex->xshow) {
                    if (tmpstr)
                        gdFree(tmpstr);
                    gdFree(text);
                    gdCacheDelete(tc_cache);
                    gdMutexUnlock(gdFontCacheMutex);
                    gdFree(info);
                    return "Problem allocating memory";
                }
                xshow_pos = 0;
            } else if (xshow_pos + 20 > xshow_alloc) {
                xshow_alloc += 100;
                strex->xshow = gdReallocEx(strex->xshow, xshow_alloc);
                if (!strex->xshow) {
                    if (tmpstr)
                        gdFree(tmpstr);
                    gdFree(text);
                    gdCacheDelete(tc_cache);
                    gdMutexUnlock(gdFontCacheMutex);
                    gdFree(info);
                    return "Problem allocating memory";
                }
            }
            xshow_pos += sprintf(strex->xshow + xshow_pos, "%g ",
                                 (double)(penf.x - oldpenf.x) * hdpi / (64 * METRIC_RES));
        }
        oldpenf.x = penf.x;

#ifdef HAVE_LIBRAQM
        /* Keep RAQM brect/layout metrics in unrotated text space. */
        FT_Set_Transform(face, NULL, NULL);
#endif

        /* load glyph image into the slot (erase previous one) */
        err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        if (err) {
            if (tmpstr)
                gdFree(tmpstr);
            gdFree(text);
            gdCacheDelete(tc_cache);
            gdMutexUnlock(gdFontCacheMutex);
            gdFree(info);
            return "Problem loading glyph";
        }

        horiAdvance = info[i].x_advance;
        vertAdvance = info[i].y_advance;

        if (brect) {
            /* only if need brect */

            glyph_min.x = penf.x + slot->metrics.horiBearingX;
            glyph_min.y = penf.y - slot->metrics.horiBearingY;

#if 0
			if (ch == ' ') {      /* special case for trailing space */
				glyph_max.x = penf.x + horiAdvance;
			} else {
				glyph_max.x = glyph_min.x + slot->metrics.width;
			}
#else
            glyph_max.x = penf.x + horiAdvance;
#endif
            glyph_max.y = glyph_min.y + slot->metrics.height;

            if (i == 0) {
                total_min = glyph_min;
                total_max = glyph_max;
            } else {
                if (glyph_min.x < total_min.x)
                    total_min.x = glyph_min.x;
                if (glyph_min.y < total_min.y)
                    total_min.y = glyph_min.y;
                if (glyph_max.x > total_max.x)
                    total_max.x = glyph_max.x;
                if (glyph_max.y > total_max.y)
                    total_max.y = glyph_max.y;
            }
        }

        if (render) {
            FT_Activate_Size(platform_specific);

#ifdef HAVE_LIBRAQM
            /* Render rotated glyph bitmaps while RAQM pen advances remain in
             * layout space. */
            FT_Set_Transform(face, &matrix, NULL);
#endif

            /* load glyph again into the slot (erase previous one)  - this time
             * with scaling */
            err = FT_Load_Glyph(face, glyph_index, render_mode);
            if (err) {
                if (tmpstr)
                    gdFree(tmpstr);
                gdFree(text);
                gdCacheDelete(tc_cache);
                gdMutexUnlock(gdFontCacheMutex);
                gdFree(info);
                return "Problem loading glyph";
            }

            /* load and transform glyph image */
            FT_Get_Glyph(slot, &image);

            if (image->format != ft_glyph_format_bitmap) {
                err = FT_Glyph_To_Bitmap(&image, ft_render_mode_normal, 0, 1);
                if (err) {
                    FT_Done_Glyph(image);
                    if (tmpstr)
                        gdFree(tmpstr);
                    gdFree(text);
                    gdCacheDelete(tc_cache);
                    gdMutexUnlock(gdFontCacheMutex);
                    gdFree(info);
                    return "Problem rendering glyph";
                }
            }

            /* now, draw to our target surface */
            bm = (FT_BitmapGlyph)image;
            /* position rounded down to nearest pixel at current dpi
            (the estimate was rounded up to next 1/METRIC_RES, so this should
            fit) */
            FT_Pos pen_x = penf.x + info[i].x_offset;
            FT_Pos pen_y = penf.y - info[i].y_offset;
            gdft_draw_bitmap(
                tc_cache, im, fg, bm->bitmap,
                (int)(x + (pen_x * cos_a + pen_y * sin_a) * hdpi / (METRIC_RES * 64) + bm->left),
                (int)(y - (pen_x * sin_a - pen_y * cos_a) * vdpi / (METRIC_RES * 64) - bm->top));

            FT_Done_Glyph(image);
        }

        penf.x += horiAdvance;
        penf.y += vertAdvance;
    }

    gdFree(text);
    if (info) {
        gdFree(info);
    }

    /* Save the (unkerned) advance from the last character in the xshow vector
     */
    if (strex && (strex->flags & gdFTEX_XSHOW) && strex->xshow) {
        sprintf(strex->xshow + xshow_pos, "%g",
                (double)(penf.x - oldpenf.x) * hdpi / (64 * METRIC_RES));
    }

    if (brect) {
        /* only if need brect */
        double scalex = (double)hdpi / (64 * METRIC_RES);
        double scaley = (double)vdpi / (64 * METRIC_RES);

        /* rotate bounding rectangle, scale and round to int pixels, and
         * translate */
        brect[0] = x + (total_min.x * cos_a + total_max.y * sin_a) * scalex;
        brect[1] = y - (total_min.x * sin_a - total_max.y * cos_a) * scaley;
        brect[2] = x + (total_max.x * cos_a + total_max.y * sin_a) * scalex;
        brect[3] = y - (total_max.x * sin_a - total_max.y * cos_a) * scaley;
        brect[4] = x + (total_max.x * cos_a + total_min.y * sin_a) * scalex;
        brect[5] = y - (total_max.x * sin_a - total_min.y * cos_a) * scaley;
        brect[6] = x + (total_min.x * cos_a + total_min.y * sin_a) * scalex;
        brect[7] = y - (total_min.x * sin_a - total_min.y * cos_a) * scaley;
    }

    FT_Done_Size(platform_independent);
    if (render)
        FT_Done_Size(platform_specific);

    if (tmpstr)
        gdFree(tmpstr);
    gdCacheDelete(tc_cache);
    gdMutexUnlock(gdFontCacheMutex);
    return (char *)NULL;
}

#endif /* HAVE_LIBFREETYPE */

#ifdef HAVE_LIBFONTCONFIG
/* Code to find font path, with special mapping for Postscript font names.
 *
 * Dag Lem <dag@nimrod.no>
 */

#include <fontconfig/fontconfig.h>

/* #define NO_POSTSCRIPT_ALIAS 1 */
#ifndef NO_POSTSCRIPT_ALIAS
typedef struct _PostscriptAlias {
    char *name;
    char *family;
    char *style;
} PostscriptAlias;

/* This table maps standard Postscript font names to URW Type 1 fonts.
   The mapping is converted from Ghostscript (Fontmap.GS)
   for use with fontconfig. */
static PostscriptAlias postscript_alias[] = {
    {"AvantGarde-Book", "URW Gothic L", "Book"},
    {"AvantGarde-BookOblique", "URW Gothic L", "Book Oblique"},
    {"AvantGarde-Demi", "URW Gothic L", "Demi"},
    {"AvantGarde-DemiOblique", "URW Gothic L", "Demi Oblique"},

    {"Bookman-Demi", "URW Bookman L", "Demi Bold"},
    {"Bookman-DemiItalic", "URW Bookman L", "Demi Bold Italic"},
    {"Bookman-Light", "URW Bookman L", "Light"},
    {"Bookman-LightItalic", "URW Bookman L", "Light Italic"},

    {"Courier", "Nimbus Mono L", "Regular"},
    {"Courier-Oblique", "Nimbus Mono L", "Regular Oblique"},
    {"Courier-Bold", "Nimbus Mono L", "Bold"},
    {"Courier-BoldOblique", "Nimbus Mono L", "Bold Oblique"},

    {"Helvetica", "Nimbus Sans L", "Regular"},
    {"Helvetica-Oblique", "Nimbus Sans L", "Regular Italic"},
    {"Helvetica-Bold", "Nimbus Sans L", "Bold"},
    {"Helvetica-BoldOblique", "Nimbus Sans L", "Bold Italic"},

    {"Helvetica-Narrow", "Nimbus Sans L", "Regular Condensed"},
    {"Helvetica-Narrow-Oblique", "Nimbus Sans L", "Regular Condensed Italic"},
    {"Helvetica-Narrow-Bold", "Nimbus Sans L", "Bold Condensed"},
    {"Helvetica-Narrow-BoldOblique", "Nimbus Sans L", "Bold Condensed Italic"},

    {"NewCenturySchlbk-Roman", "Century Schoolbook L", "Roman"},
    {"NewCenturySchlbk-Italic", "Century Schoolbook L", "Italic"},
    {"NewCenturySchlbk-Bold", "Century Schoolbook L", "Bold"},
    {"NewCenturySchlbk-BoldItalic", "Century Schoolbook L", "Bold Italic"},

    {"Palatino-Roman", "URW Palladio L", "Roman"},
    {"Palatino-Italic", "URW Palladio L", "Italic"},
    {"Palatino-Bold", "URW Palladio L", "Bold"},
    {"Palatino-BoldItalic", "URW Palladio L", "Bold Italic"},

    {"Symbol", "Standard Symbols L", "Regular"},

    {"Times-Roman", "Nimbus Roman No9 L", "Regular"},
    {"Times-Italic", "Nimbus Roman No9 L", "Regular Italic"},
    {"Times-Bold", "Nimbus Roman No9 L", "Medium"},
    {"Times-BoldItalic", "Nimbus Roman No9 L", "Medium Italic"},

    {"ZapfChancery-MediumItalic", "URW Chancery L", "Medium Italic"},

    {"ZapfDingbats", "Dingbats", ""},
};
#endif

static FcPattern *find_font(FcPattern *pattern)
{
    FcResult result;

    FcConfigSubstitute(0, pattern, FcMatchPattern);
    FcConfigSubstitute(0, pattern, FcMatchFont);
    FcDefaultSubstitute(pattern);

    return FcFontMatch(0, pattern, &result);
}

#ifndef NO_POSTSCRIPT_ALIAS
static char *find_postscript_font(FcPattern **fontpattern, char *fontname)
{
    FcPattern *font = NULL;
    size_t i;

    *fontpattern = NULL;
    for (i = 0; i < sizeof(postscript_alias) / sizeof(*postscript_alias); i++) {
        if (strcmp(fontname, postscript_alias[i].name) == 0) {
            FcChar8 *family;

            FcPattern *pattern =
                FcPatternBuild(0, FC_FAMILY, FcTypeString, postscript_alias[i].family, FC_STYLE,
                               FcTypeString, postscript_alias[i].style, (char *)0);
            font = find_font(pattern);
            FcPatternDestroy(pattern);

            if (!font)
                return "fontconfig: Couldn't find font.";
            if (FcPatternGetString(font, FC_FAMILY, 0, &family) != FcResultMatch) {
                FcPatternDestroy(font);
                return "fontconfig: Couldn't retrieve font family name.";
            }

            /* Check whether we got the font family we wanted. */
            if (strcmp((const char *)family, postscript_alias[i].family) != 0) {
                FcPatternDestroy(font);
                return "fontconfig: Didn't find expected font family. Perhaps "
                       "URW Type 1 fonts need installing?";
            }
            break;
        }
    }

    *fontpattern = font;
    return NULL;
}
#endif

static char *font_pattern(char **fontpath, char *fontpattern)
{
    FcPattern *font = NULL;
    FcChar8 *file;
    FcPattern *pattern;
#ifndef NO_POSTSCRIPT_ALIAS
    char *error;
#endif

    *fontpath = NULL;
#ifndef NO_POSTSCRIPT_ALIAS
    error = find_postscript_font(&font, fontpattern);

    if (!font) {
        if (error)
            return error;
#endif
        pattern = FcNameParse((const FcChar8 *)fontpattern);
        font = find_font(pattern);
        FcPatternDestroy(pattern);
#ifndef NO_POSTSCRIPT_ALIAS
    }
#endif

    if (!font)
        return "fontconfig: Couldn't find font.";
    if (FcPatternGetString(font, FC_FILE, 0, &file) != FcResultMatch) {
        FcPatternDestroy(font);
        return "fontconfig: Couldn't retrieve font file name.";
    } else {
        const unsigned int file_len = strlen((const char *)file);

        *fontpath = (char *)gdMalloc(file_len + 1);
        if (*fontpath == NULL) {
            return "could not alloc font path";
        }
        memcpy(*fontpath, (const char *)file, file_len);
        (*fontpath)[file_len] = 0;
    }
    FcPatternDestroy(font);

    return NULL;
}

#endif /* HAVE_LIBFONTCONFIG */

#ifdef HAVE_LIBFREETYPE
/* Look up font using font names as file names. */
static char *font_path(char **fontpath, char *name_list)
{
    int font_found = 0;
    char *fontsearchpath, *fontlist;
    char *fullname = NULL;
    char *name, *dir;
    char *path;
    char *strtok_ptr = NULL;
    const unsigned int name_list_len = strlen(name_list);

    /*
     * Search the pathlist for any of a list of font names.
     */
    *fontpath = NULL;
    fontsearchpath = getenv("GDFONTPATH");
    if (!fontsearchpath)
        fontsearchpath = DEFAULT_FONTPATH;
    path = (char *)gdMalloc(sizeof(char) * strlen(fontsearchpath) + 1);
    if (path == NULL) {
        return "could not alloc full list of fonts";
    }
    path[0] = 0;

    fontlist = (char *)gdMalloc(name_list_len + 1);
    if (fontlist == NULL) {
        gdFree(path);
        return "could not alloc full list of fonts";
    }
    memcpy(fontlist, name_list, name_list_len);
    fontlist[name_list_len] = 0;

    /*
     * Must use gd_strtok_r else pointer corrupted by strtok in nested loop.
     */
    for (name = gd_strtok_r(fontlist, LISTSEPARATOR, &strtok_ptr); name;
         name = gd_strtok_r(0, LISTSEPARATOR, &strtok_ptr)) {
        char *path_ptr = NULL;

        /* make a fresh copy each time - strtok corrupts it. */
        sprintf(path, "%s", fontsearchpath);
        /*
         * Allocate an oversized buffer that is guaranteed to be
         * big enough for all paths to be tested.
         */
        /* 2.0.22: Thorben Kundinger: +8 is needed, not +6. */
        fullname = gdReallocEx(fullname, strlen(fontsearchpath) + strlen(name) + 8);
        if (!fullname) {
            gdFree(fontlist);
            gdFree(path);
            return "could not alloc full path of font";
        }
        /* if name is an absolute or relative pathname then test directly */
        if (GD_FT_IS_PATH(name)) {
            sprintf(fullname, "%s", name);
            if (GD_FT_ACCESS(fullname, R_OK) == 0) {
                font_found++;
                break;
            }
        }
        for (dir = gd_strtok_r(path, PATHSEPARATOR, &path_ptr); dir;
             dir = gd_strtok_r(0, PATHSEPARATOR, &path_ptr)) {
            if (strchr(name, '.')) {
                sprintf(fullname, "%s/%s", dir, name);
                if (GD_FT_ACCESS(fullname, R_OK) == 0) {
                    font_found++;
                    break;
                } else {
                    continue;
                }
            }
            sprintf(fullname, "%s/%s.ttf", dir, name);
            if (GD_FT_ACCESS(fullname, R_OK) == 0) {
                font_found++;
                break;
            }
            sprintf(fullname, "%s/%s.pfa", dir, name);
            if (GD_FT_ACCESS(fullname, R_OK) == 0) {
                font_found++;
                break;
            }
            sprintf(fullname, "%s/%s.pfb", dir, name);
            if (GD_FT_ACCESS(fullname, R_OK) == 0) {
                font_found++;
                break;
            }
            sprintf(fullname, "%s/%s.dfont", dir, name);
            if (GD_FT_ACCESS(fullname, R_OK) == 0) {
                font_found++;
                break;
            }
        }

        if (font_found)
            break;
    }
    gdFree(path);
    if (fontlist != NULL) {
        gdFree(fontlist);
        fontlist = NULL;
    }
    if (!font_found) {
        gdFree(fullname);
        return "Could not find/open font";
    }

    *fontpath = fullname;
    return NULL;
}
#endif

/**
 * Function: gdFTUseFontConfig
 *
 * Enable or disable fontconfig by default.
 *
 * If GD is built without libfontconfig support, this function is a NOP.
 *
 * Parameters:
 *  flag - Zero to disable, nonzero to enable.
 *
 * See also:
 *  - <gdImageStringFTEx>
 */
BGD_DECLARE(int) gdFTUseFontConfig(int flag)
{
#ifdef HAVE_LIBFONTCONFIG
    fontConfigFlag = flag;
    return 1;
#else
    (void)flag;
    return 0;
#endif /* HAVE_LIBFONTCONFIG */
}
