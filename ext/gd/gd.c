/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Bakken <ssb@fast.no>                                   |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* gd 1.2 is copyright 1994, 1995, Quest Protein Database Center, 
   Cold Spring Harbor Labs. */

/* Note that there is no code from the gd package in this file */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/head.h"
#include <math.h>
#include "SAPI.h"
#include "php_gd.h"
#include "ext/standard/info.h"

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_LIBGD

static int le_gd, le_gd_font;
#if HAVE_LIBT1
#include <t1lib.h>
static int le_ps_font, le_ps_enc;
static void php_free_ps_font(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void php_free_ps_enc(zend_rsrc_list_entry *rsrc TSRMLS_DC);
#endif

#include <gd.h>
#include <gdfontt.h>  /* 1 Tiny font */
#include <gdfonts.h>  /* 2 Small font */
#include <gdfontmb.h> /* 3 Medium bold font */
#include <gdfontl.h>  /* 4 Large font */
#include <gdfontg.h>  /* 5 Giant font */
#ifdef HAVE_GD_WBMP
#include "libgd/wbmp.h"
#endif
#ifdef ENABLE_GD_TTF
# include "gdttf.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef ENABLE_GD_TTF
static void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int, int);
#endif

#if HAVE_LIBGD15
/* it's >= 1.5, i.e. has IOCtx */
#define USE_GD_IOCTX 1
#else
#undef USE_GD_IOCTX
#endif

#ifdef USE_GD_IOCTX
#include "gd_ctx.c"
#else
#define gdImageCreateFromGdCtx      NULL
#define gdImageCreateFromGd2Ctx     NULL
#define gdImageCreateFromGd2partCtx NULL
#define gdImageCreateFromGifCtx     NULL
#define gdImageCreateFromJpegCtx    NULL
#define gdImageCreateFromPngCtx     NULL
#define gdImageCreateFromWBMPCtx    NULL
typedef FILE gdIOCtx;
#define CTX_PUTC(c, fp) fputc(c, fp)
#endif

#ifndef HAVE_GDIMAGECOLORRESOLVE
extern int gdImageColorResolve(gdImagePtr, int, int, int);
#endif

#if HAVE_COLORCLOSESTHWB
int gdImageColorClosestHWB(gdImagePtr im, int r, int g, int b);
#endif

static gdImagePtr _php_image_create_from_string (zval **Data, char *tn, gdImagePtr (*ioctx_func_p)() TSRMLS_DC);
static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(), gdImagePtr (*ioctx_func_p)());
static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, void (*func_p)());
static int _php_image_type ( char data[8] );
static void _php_image_convert(INTERNAL_FUNCTION_PARAMETERS, int image_type);
static void _php_image_bw_convert(gdImagePtr im_org, gdIOCtx *out, int threshold);

/* {{{ gd_functions[]
 */
function_entry gd_functions[] = {
	PHP_FE(imagearc,								NULL)
	PHP_FE(imageellipse,							NULL)
	PHP_FE(imagechar,								NULL)
	PHP_FE(imagecharup,								NULL)
	PHP_FE(imagecolorallocate,						NULL)
#if HAVE_LIBGD15
	PHP_FE(imagepalettecopy,						NULL)
	PHP_FE(imagecreatefromstring,					NULL)
#endif
	PHP_FE(imagecolorat,							NULL)
	PHP_FE(imagecolorclosest,						NULL)
#if HAVE_COLORCLOSESTHWB
	PHP_FE(imagecolorclosesthwb,					NULL)
#endif
	PHP_FE(imagecolordeallocate,					NULL)
	PHP_FE(imagecolorresolve,						NULL)
	PHP_FE(imagecolorexact,							NULL)
	PHP_FE(imagecolorset,							NULL)
	PHP_FE(imagecolortransparent,					NULL)
	PHP_FE(imagecolorstotal,						NULL)
	PHP_FE(imagecolorsforindex,						NULL)
	PHP_FE(imagecopy,								NULL)
#if HAVE_LIBGD15
	PHP_FE(imagecopymerge,							NULL)
	PHP_FE(imagecopymergegray,						NULL)
#endif
	PHP_FE(imagecopyresized,						NULL)
	PHP_FE(imagecreate,								NULL)
#if HAVE_LIBGD20
	PHP_FE(imagecreatetruecolor,					NULL)
	PHP_FE(imagetruecolortopalette,					NULL)
	PHP_FE(imagesetthickness,						NULL)
	PHP_FE(imagefilledarc,							NULL)
	PHP_FE(imagefilledellipse,						NULL)
	PHP_FE(imagealphablending,						NULL)
	PHP_FE(imagecolorresolvealpha, 					NULL)
	PHP_FE(imagecolorclosestalpha,					NULL)
	PHP_FE(imagecolorexactalpha,					NULL)
	PHP_FE(imagecopyresampled,						NULL)
#endif

#if HAVE_GD_IMAGESETTILE
	PHP_FE(imagesettile,							NULL)
#endif

#if HAVE_GD_IMAGESETBRUSH
	PHP_FE(imagesetbrush,							NULL)
#endif

	PHP_FE(imagesetstyle,							NULL)

#ifdef HAVE_GD_PNG
	PHP_FE(imagecreatefrompng,						NULL)
#endif
#ifdef HAVE_GD_GIF_READ
	PHP_FE(imagecreatefromgif,						NULL)
#endif
#ifdef HAVE_GD_JPG
	PHP_FE(imagecreatefromjpeg,						NULL)
#endif
#ifdef HAVE_GD_WBMP
	PHP_FE(imagecreatefromwbmp,						NULL)
#endif
#ifdef HAVE_GD_XBM
	PHP_FE(imagecreatefromxbm,						NULL)
#endif
#ifdef HAVE_GD_XPM
	PHP_FE(imagecreatefromxpm,						NULL)
#endif
	PHP_FE(imagecreatefromgd,						NULL)
#ifdef HAVE_GD_GD2
	PHP_FE(imagecreatefromgd2,						NULL)
	PHP_FE(imagecreatefromgd2part,					NULL)
#endif
#ifdef HAVE_GD_PNG
	PHP_FE(imagepng,								NULL)
#endif
#ifdef HAVE_GD_GIF_CREATE
	PHP_FE(imagegif,								NULL)
#endif
#ifdef HAVE_GD_JPG
	PHP_FE(imagejpeg,								NULL)
#endif
#ifdef HAVE_GD_WBMP
	PHP_FE(imagewbmp,                               NULL)
#endif
	PHP_FE(imagegd,									NULL)
#ifdef HAVE_GD_GD2
	PHP_FE(imagegd2,								NULL)
#endif

	PHP_FE(imagedestroy,							NULL)
	PHP_FE(imagegammacorrect,						NULL)
	PHP_FE(imagefill,								NULL)
	PHP_FE(imagefilledpolygon,						NULL)
	PHP_FE(imagefilledrectangle,					NULL)
	PHP_FE(imagefilltoborder,						NULL)
	PHP_FE(imagefontwidth,							NULL)
	PHP_FE(imagefontheight,							NULL)
	PHP_FE(imageinterlace,							NULL)
	PHP_FE(imageline,								NULL)
	PHP_FE(imageloadfont,							NULL)
	PHP_FE(imagepolygon,							NULL)
	PHP_FE(imagerectangle,							NULL)
	PHP_FE(imagesetpixel,							NULL)
	PHP_FE(imagestring,								NULL)
	PHP_FE(imagestringup,							NULL)
	PHP_FE(imagesx,									NULL)
	PHP_FE(imagesy,									NULL)
	PHP_FE(imagedashedline,							NULL)

#ifdef ENABLE_GD_TTF
	PHP_FE(imagettfbbox,							NULL)
	PHP_FE(imagettftext,							NULL)
#if HAVE_LIBGD20 && HAVE_LIBFREETYPE && HAVE_GD_STRINGFTEX
	PHP_FE(imageftbbox,								NULL)
	PHP_FE(imagefttext,								NULL)
#endif
#endif

#ifdef HAVE_LIBT1
	PHP_FE(imagepsloadfont,							NULL)
	/*
	PHP_FE(imagepscopyfont,							NULL)
	*/
	PHP_FE(imagepsfreefont,							NULL)
	PHP_FE(imagepsencodefont,						NULL)
	PHP_FE(imagepsextendfont,						NULL)
	PHP_FE(imagepsslantfont,						NULL)
	PHP_FE(imagepstext,								NULL)
	PHP_FE(imagepsbbox,								NULL)
#endif
	PHP_FE(imagetypes,								NULL)
	
#if defined(HAVE_GD_JPG) && defined(HAVE_GD_WBMP)
	PHP_FE(jpeg2wbmp,								NULL)
#endif
#if defined(HAVE_GD_PNG) && defined(HAVE_GD_WBMP)
	PHP_FE(png2wbmp,								NULL)
#endif
#ifdef HAVE_GD_WBMP
	PHP_FE(image2wbmp,								NULL)
#endif	
#if HAVE_GD_BUNDLED
	PHP_FE(imagelayereffect,						NULL)
	PHP_FE(imagecolormatch,							NULL)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry gd_module_entry = {
	STANDARD_MODULE_HEADER,
	"gd",
	gd_functions,
	PHP_MINIT(gd),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(gd),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GD
ZEND_GET_MODULE(gd)
#endif

/* {{{ php_free_gd_image
 */
static void php_free_gd_image(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	gdImageDestroy((gdImagePtr)rsrc->ptr);
}
/* }}} */

/* {{{ php_free_gd_font
 */
static void php_free_gd_font(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	gdFontPtr fp = (gdFontPtr)rsrc->ptr;

	if (fp->data) {
		efree(fp->data);
	}
	efree(fp);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(gd)
{
	le_gd = zend_register_list_destructors_ex(php_free_gd_image, NULL, "gd", module_number);
	le_gd_font = zend_register_list_destructors_ex(php_free_gd_font, NULL, "gd font", module_number);
#if HAVE_LIBT1
	T1_SetBitmapPad(8);
	T1_InitLib(NO_LOGFILE|IGNORE_CONFIGFILE|IGNORE_FONTDATABASE);
	T1_SetLogLevel(T1LOG_DEBUG);
	le_ps_font = zend_register_list_destructors_ex(php_free_ps_font, NULL, "gd PS font", module_number);
	le_ps_enc = zend_register_list_destructors_ex(php_free_ps_enc, NULL, "gd PS encoding", module_number);
#endif
	REGISTER_LONG_CONSTANT("IMG_GIF", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_JPG", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_JPEG", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_PNG", 4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_WBMP", 8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_XPM", 16, CONST_CS | CONST_PERSISTENT);
#ifdef gdTiled
	/* special colours for gd */
	REGISTER_LONG_CONSTANT("IMG_COLOR_TILED", gdTiled, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_STYLED", gdStyled, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_BRUSHED", gdBrushed, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_STYLEDBRUSHED", gdStyledBrushed, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_COLOR_TRANSPARENT", gdTransparent, CONST_CS | CONST_PERSISTENT);
#endif
#if HAVE_LIBGD20
	/* for imagefilledarc */
	REGISTER_LONG_CONSTANT("IMG_ARC_ROUNDED", gdArc, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_PIE", gdPie, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_CHORD", gdChord, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_NOFILL", gdNoFill, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_ARC_EDGED", gdEdged, CONST_CS | CONST_PERSISTENT);
#endif
#if HAVE_GD_BUNDLED
	REGISTER_LONG_CONSTANT("IMG_EFFECT_REPLACE", gdEffectReplace, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_ALPHABLEND", gdEffectAlphaBlend, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_NORMAL", gdEffectNormal, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_EFFECT_OVERLAY", gdEffectOverlay, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GD_BUNDLED", 1, CONST_CS | CONST_PERSISTENT);
#else
	REGISTER_LONG_CONSTANT("GD_BUNDLED", 0, CONST_CS | CONST_PERSISTENT);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(gd)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "GD Support", "enabled");

	/* need to use a PHPAPI function here because it is external module in windows */

#if HAVE_GD_BUNDLED
	php_info_print_table_row(2, "GD Version", "bundled (2.0 compatible)");
#elif HAVE_LIBGD20
	php_info_print_table_row(2, "GD Version", "2.0 or higher");
#elif HAVE_GDIMAGECOLORRESOLVE
	php_info_print_table_row(2, "GD Version", "1.6.2 or higher");
#elif HAVE_LIBGD13
	php_info_print_table_row(2, "GD Version", "between 1.3 and 1.6.1");
#else
	php_info_print_table_row(2, "GD Version", "1.2");
#endif

#ifdef ENABLE_GD_TTF
	php_info_print_table_row(2, "FreeType Support", "enabled");
#if HAVE_LIBFREETYPE
	php_info_print_table_row(2, "FreeType Linkage", "with freetype");
#elif HAVE_LIBTTF
	php_info_print_table_row(2, "FreeType Linkage", "with TTF library");
#else
	php_info_print_table_row(2, "FreeType Linkage", "with unknown library");
#endif
#endif

#ifdef HAVE_LIBT1
	php_info_print_table_row(2, "T1Lib Support", "enabled");
#endif

/* this next part is stupid ... if I knew better, I'd put them all on one row (cmv) */

#ifdef HAVE_GD_GIF_READ
	php_info_print_table_row(2, "GIF Read Support", "enabled");
#endif
#ifdef HAVE_GD_GIF_CREATE
	php_info_print_table_row(2, "GIF Create Support", "enabled");
#endif
#ifdef HAVE_GD_JPG
	php_info_print_table_row(2, "JPG Support", "enabled");
#endif
#ifdef HAVE_GD_PNG
	php_info_print_table_row(2, "PNG Support", "enabled");
#endif
#ifdef HAVE_GD_WBMP
	php_info_print_table_row(2, "WBMP Support", "enabled");
#endif
#ifdef HAVE_GD_XBM
	php_info_print_table_row(2, "XBM Support", "enabled");
#endif
	php_info_print_table_end();
}
/* }}} */

/* Need this for cpdf. See also comment in file.c php3i_get_le_fp() */
PHP_GD_API int phpi_get_le_gd(void)
{
	return le_gd;
}

#ifndef HAVE_GDIMAGECOLORRESOLVE

/* {{{ gdImageColorResolve
 */
/********************************************************************/
/* gdImageColorResolve is a replacement for the old fragment:       */
/*                                                                  */
/*      if ((color=gdImageColorExact(im,R,G,B)) < 0)                */
/*        if ((color=gdImageColorAllocate(im,R,G,B)) < 0)           */
/*          color=gdImageColorClosest(im,R,G,B);                    */
/*                                                                  */
/* in a single function                                             */

int gdImageColorResolve(gdImagePtr im, int r, int g, int b)
{
	int c;
	int ct = -1;
	int op = -1;
	long rd, gd, bd, dist;
	long mindist = 3*255*255;  /* init to max poss dist */

	for (c = 0; c < im->colorsTotal; c++) {
		if (im->open[c]) {
			op = c;             /* Save open slot */
			continue;           /* Color not in use */
		}
		rd = (long)(im->red  [c] - r);
		gd = (long)(im->green[c] - g);
		bd = (long)(im->blue [c] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if (dist < mindist) {
			if (dist == 0) {
				return c;       /* Return exact match color */
			}
			mindist = dist;
			ct = c;
		}
	}
	/* no exact match.  We now know closest, but first try to allocate exact */
	if (op == -1) {
		op = im->colorsTotal;
		if (op == gdMaxColors) {    /* No room for more colors */
			return ct;          /* Return closest available color */
		}
		im->colorsTotal++;
	}
	im->red  [op] = r;
	im->green[op] = g;
	im->blue [op] = b;
	im->open [op] = 0;
	return op;                  /* Return newly allocated color */
}
/* }}} */

#endif

#define FLIPWORD(a) (((a & 0xff000000) >> 24) | ((a & 0x00ff0000) >> 8) | ((a & 0x0000ff00) << 8) | ((a & 0x000000ff) << 24))

/* {{{ proto int imageloadfont(string filename)
   Load a new font */
PHP_FUNCTION(imageloadfont) 
{
	zval **file;
	int hdr_size = sizeof(gdFont) - sizeof(char *);
	int ind, body_size, n=0, b, i, body_size_check;
	gdFontPtr font;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(file);

	stream = php_stream_open_wrapper(Z_STRVAL_PP(file), "rb", IGNORE_PATH|IGNORE_URL_WIN|REPORT_ERRORS, NULL);
	if (stream == NULL) {
		RETURN_FALSE;
	}

	/* Only supports a architecture-dependent binary dump format
	 * at the moment.
	 * The file format is like this on machines with 32-byte integers:
	 *
	 * byte 0-3:   (int) number of characters in the font
	 * byte 4-7:   (int) value of first character in the font (often 32, space)
	 * byte 8-11:  (int) pixel width of each character
	 * byte 12-15: (int) pixel height of each character
	 * bytes 16-:  (char) array with character data, one byte per pixel
	 *                    in each character, for a total of 
	 *                    (nchars*width*height) bytes.
	 */
	font = (gdFontPtr)emalloc(sizeof(gdFont));
	b = 0;
	while (b < hdr_size && (n = php_stream_read(stream, (char*)&font[b], hdr_size - b)))
		b += n;
	if (!n) {
		efree(font);
		if (php_stream_eof(stream)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "End of file while reading header");
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading header");
		}
		php_stream_close(stream);
		RETURN_FALSE;
	}
	i = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_END);
	body_size_check = php_stream_tell(stream) - hdr_size;
	php_stream_seek(stream, i, SEEK_SET);
	body_size = font->w * font->h * font->nchars;
	if (body_size != body_size_check) {
		font->w = FLIPWORD(font->w);
		font->h = FLIPWORD(font->h);
		font->nchars = FLIPWORD(font->nchars);
		body_size = font->w * font->h * font->nchars;
	}
	if (body_size != body_size_check) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error reading font");
		efree(font);
		RETURN_FALSE;
	}

	font->data = emalloc(body_size);
	b = 0;
	while (b < body_size && (n = php_stream_read(stream, &font->data[b], body_size - b)))
		b += n;
	if (!n) {
		efree(font->data);
		efree(font);
		if (php_stream_eof(stream)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "End of file while reading body");
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading body");
		}
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_close(stream);

	/* Adding 5 to the font index so we will never have font indices
	 * that overlap with the old fonts (with indices 1-5).  The first
	 * list index given out is always 1.
	 */
	ind = 5 + zend_list_insert(font, le_gd_font);

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto void imagesetstyle(resource im, array styles)
   Set the line drawing styles for use with imageline and IMG_COLOR_STYLED. */
PHP_FUNCTION(imagesetstyle)
{
	zval **IM, **styles;
	gdImagePtr im;
	int * stylearr;
	int index;
	HashPosition pos;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM, &styles) == FAILURE)  {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	convert_to_array_ex(styles);

	/* copy the style values in the stylearr */
	stylearr = emalloc(sizeof(int) * zend_hash_num_elements(HASH_OF(*styles)));

	zend_hash_internal_pointer_reset_ex(HASH_OF(*styles), &pos);
		
	for (index=0;; zend_hash_move_forward_ex(HASH_OF(*styles), &pos))	{
		zval ** item;

		if (zend_hash_get_current_data_ex(HASH_OF(*styles), (void**)&item, &pos) == FAILURE)
			break;

		convert_to_long_ex(item);

		stylearr[index++] = Z_LVAL_PP(item);
	}
	gdImageSetStyle(im, stylearr, index);

	efree(stylearr);

	RETURN_TRUE;
}
/* }}} */

#if HAVE_LIBGD20
/* {{{ proto int imagecreatetruecolor(int x_size, int y_size)
   Create a new true color image */
PHP_FUNCTION(imagecreatetruecolor)
{
	zval **x_size, **y_size;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x_size, &y_size) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(x_size);
	convert_to_long_ex(y_size);

	im = gdImageCreateTrueColor(Z_LVAL_PP(x_size), Z_LVAL_PP(y_size));

	ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
}
/* }}} */

/* {{{ proto void imagetruecolortopalette(resource im, bool ditherFlag, int colorsWanted)
   Convert a true colour image to a palette based image with a number of colours, optionally using dithering. */
PHP_FUNCTION(imagetruecolortopalette)
{
	zval **IM, **dither, **ncolors;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &IM, &dither, &ncolors) == FAILURE)  {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	convert_to_boolean_ex(dither);
	convert_to_long_ex(ncolors);
		
	gdImageTrueColorToPalette(im, Z_LVAL_PP(dither), Z_LVAL_PP(ncolors));

	RETURN_TRUE;
}
/* }}} */

#if HAVE_GD_BUNDLED
/* {{{ proto void imagecolormatch(resource im1, resource im2)
   Makes the colors of the palette version of an image more closely match the true color version */
PHP_FUNCTION(imagecolormatch)
{
	zval **IM1, **IM2;
	gdImagePtr im1, im2;
	int result;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM1, &IM2 ) == FAILURE)  {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im1, gdImagePtr, IM1, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im2, gdImagePtr, IM2, -1, "Image", le_gd);

	result = gdImageColorMatch(im1, im2);
	switch( result )
	{
	case -1:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Image1 must be TrueColor" );
		RETURN_FALSE;
		break;
	case -2:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Image2 must be Palette" );
		RETURN_FALSE;
		break;
	case -3:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Image1 and Image2 must be the same size" );
		RETURN_FALSE;
		break;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void imagesetthickness(resource im, int thickness)
   Set line thickness for drawing lines, ellipses, rectangles, polygons etc. */
PHP_FUNCTION(imagesetthickness)
{
	zval **IM, **thick;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM, &thick) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	convert_to_long_ex(thick);
		
	gdImageSetThickness(im, Z_LVAL_PP(thick));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void imagefilledellipse(resource im, int cx, int cy, int w, int h, int color)
   Draw an ellipse */
PHP_FUNCTION(imagefilledellipse)
{
	zval **IM, **cx, **cy, **w, **h, **color;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &IM, &cx, &cy, &w, &h, &color) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(cx);
	convert_to_long_ex(cy);
	convert_to_long_ex(w);
	convert_to_long_ex(h);
	convert_to_long_ex(color);

	gdImageFilledEllipse(im, Z_LVAL_PP(cx), Z_LVAL_PP(cy), Z_LVAL_PP(w), Z_LVAL_PP(h), Z_LVAL_PP(color));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagefilledarc(int im, int cx, int cy, int w, int h, int s, int e, int col, int style)
   Draw a filled partial ellipse */
PHP_FUNCTION(imagefilledarc)
{
	zval **IM, **cx, **cy, **w, **h, **ST, **E, **col, **style;
	gdImagePtr im;
	int e, st;

	if (ZEND_NUM_ARGS() != 9 || zend_get_parameters_ex(9, &IM, &cx, &cy, &w, &h, &ST, &E, &col, &style) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(cx);
	convert_to_long_ex(cy);
	convert_to_long_ex(w);
	convert_to_long_ex(h);
	convert_to_long_ex(ST);
	convert_to_long_ex(E);
	convert_to_long_ex(col);
	convert_to_long_ex(style);

	e = Z_LVAL_PP(E);
	st = Z_LVAL_PP(ST);

	if (e < 0)   e %= 360;
	if (st < 0) st %= 360;

	gdImageFilledArc(im, Z_LVAL_PP(cx), Z_LVAL_PP(cy), Z_LVAL_PP(w), Z_LVAL_PP(h), st, e, Z_LVAL_PP(col), Z_LVAL_PP(style));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto void imagealphablending(resource im, bool on)
   Turn alpha blending mode on or off for the given image */
PHP_FUNCTION(imagealphablending)
{
	zval **IM, **blend;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 ||	zend_get_parameters_ex(2, &IM, &blend) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	convert_to_boolean_ex(blend);
		
	gdImageAlphaBlending(im, Z_LVAL_PP(blend));

	RETURN_TRUE;
}
/* }}} */

#if HAVE_GD_BUNDLED
/* {{{ proto void imagelayereffect(resource im, int effect)
   Set the alpha blending flag to use the bundled libgd layering effects */
PHP_FUNCTION(imagelayereffect)
{
	zval **IM, **effect;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 ||	zend_get_parameters_ex(2, &IM, &effect) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	convert_to_long_ex(effect);

	gdImageAlphaBlending(im, Z_LVAL_PP(effect) );

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int imagecolorresolvealpha(resource im, int red, int green, int blue, int alpha)
   Resolve/Allocate a colour with an alpha level.  Works for true colour and palette based images */
PHP_FUNCTION(imagecolorresolvealpha)
{
	zval **IM, ** red, **green, **blue, **alpha;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &IM, &red, &green, &blue, &alpha) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	convert_to_long_ex(alpha);

	RETURN_LONG(gdImageColorResolveAlpha(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue), Z_LVAL_PP(alpha)));
}
/* }}} */

/* {{{ proto int imagecolorclosestalpha(resource im, int red, int green, int blue, int alpha)
   Find the closest matching colour with alpha transparency */
PHP_FUNCTION(imagecolorclosestalpha)
{
	zval **IM, ** red, **green, **blue, **alpha;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &IM, &red, &green, &blue, &alpha) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	convert_to_long_ex(alpha);

	RETURN_LONG(gdImageColorClosestAlpha(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue), Z_LVAL_PP(alpha)));
}
/* }}} */

/* {{{ proto int imagecolorexactalpha(resource im, int red, int green, int blue, int alpha)
   Find exact match for colour with transparency */
PHP_FUNCTION(imagecolorexactalpha)
{
	zval **IM, **red, **green, **blue, **alpha;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &IM, &red, &green, &blue, &alpha) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	convert_to_long_ex(alpha);
	
	RETURN_LONG(gdImageColorExactAlpha(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue), Z_LVAL_PP(alpha)));
}
/* }}} */

/* {{{ proto int imagecopyresampled(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int dst_w, int dst_h, int src_w, int src_h)
   Copy and resize part of an image using resampling to help ensure clarity */
PHP_FUNCTION(imagecopyresampled)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY, **DW, **DH;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;

	if (ZEND_NUM_ARGS() != 10 ||
		zend_get_parameters_ex(10, &DIM, &SIM, &DX, &DY, &SX, &SY, &DW, &DH, &SW, &SH) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);
	convert_to_long_ex(DW);
	convert_to_long_ex(DH);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);
	dstH = Z_LVAL_PP(DH);
	dstW = Z_LVAL_PP(DW);

	gdImageCopyResampled(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */
#endif

#if HAVE_GD_IMAGESETTILE
/* {{{ proto int imagesettile(resource image, resource tile)
   Set the tile image to $tile when filling $image with the "IMG_COLOR_TILED" color */
PHP_FUNCTION(imagesettile)
{
	zval **IM, **TILE;
	gdImagePtr im, tile;

	if (ZEND_NUM_ARGS() != 2 ||	zend_get_parameters_ex(2, &IM, &TILE) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(tile, gdImagePtr, TILE, -1, "Image", le_gd);

	gdImageSetTile(im, tile);

	RETURN_TRUE;
}
/* }}} */
#endif

#if HAVE_GD_IMAGESETBRUSH
/* {{{ proto int imagesetbrush(resource image, resource brush)
   Set the brush image to $brush when filling $image with the "IMG_COLOR_BRUSHED" color */
PHP_FUNCTION(imagesetbrush)
{
	zval **IM, **TILE;
	gdImagePtr im, tile;

	if (ZEND_NUM_ARGS() != 2 ||	zend_get_parameters_ex(2, &IM, &TILE) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(tile, gdImagePtr, TILE, -1, "Image", le_gd);

	gdImageSetBrush(im, tile);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int imagecreate(int x_size, int y_size)
   Create a new image */
PHP_FUNCTION(imagecreate)
{
	zval **x_size, **y_size;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x_size, &y_size) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(x_size);
	convert_to_long_ex(y_size);

	im = gdImageCreate(Z_LVAL_PP(x_size), Z_LVAL_PP(y_size));

	ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
}
/* }}} */

/* {{{ proto int imagetypes(void)
   Return the types of images supported in a bitfield - 1=GIF, 2=JPEG, 4=PNG, 8=WBMP, 16=XPM */
PHP_FUNCTION(imagetypes)
{
	int ret=0;	
#ifdef HAVE_GD_GIF_CREATE
	ret = 1;
#endif
#ifdef HAVE_GD_JPG
	ret |= 2;
#endif
#ifdef HAVE_GD_PNG
	ret |= 4;
#endif
#ifdef HAVE_GD_WBMP
	ret |= 8;
#endif
#ifdef HAVE_GD_XPM
	ret |= 16;
#endif
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ _php_image_type
 */
static const char php_sig_gd2[3] = {'g', 'd', '2'};

static int _php_image_type (char data[8])
{
#ifdef HAVE_LIBGD15
	/* Based on ext/standard/images.c */

	if (data == NULL || strlen(data) <= 0)
		return -1;

	if (!memcmp(data, php_sig_gd2, 3))
		return PHP_GDIMG_TYPE_GD2;
	else if (!memcmp(data, php_sig_jpg, 3))
		return PHP_GDIMG_TYPE_JPG;
	else if (!memcmp(data, php_sig_png, 3)) {
		if (!memcmp(data, php_sig_png, 8))
			return PHP_GDIMG_TYPE_PNG;
	}
	else if (!memcmp(data, php_sig_gif, 3))
		return PHP_GDIMG_TYPE_GIF;
#ifdef HAVE_GD_WBMP
	else {
		gdIOCtx *io_ctx;
		io_ctx = gdNewDynamicCtx (8, data);
		if (io_ctx) {
			if (getmbi((int(*)(void*))gdGetC, io_ctx) == 0 && skipheader((int(*)(void*))gdGetC, io_ctx) == 0 ) {
				io_ctx->free(io_ctx);
				return PHP_GDIMG_TYPE_WBM;
			} else
				io_ctx->free(io_ctx);
		}
	}
#endif
	return -1;
#endif
}
/* }}} */

#ifdef HAVE_LIBGD15 
/* {{{ _php_image_create_from_string
 */
gdImagePtr _php_image_create_from_string(zval **data, char *tn, gdImagePtr (*ioctx_func_p)() TSRMLS_DC)
{
	gdImagePtr im;
	gdIOCtx *io_ctx;

	io_ctx = gdNewDynamicCtx (Z_STRLEN_PP(data), Z_STRVAL_PP(data));

	if (!io_ctx) {
		return NULL;
	}

	im = (*ioctx_func_p)(io_ctx);
	if (!im) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Passed data is not in '%s' format", tn);
		return NULL;
	}

	return im;
}
/* }}} */

/* {{{ proto int imagecreatefromstring(string image)
   Create a new image from the image stream in the string */
PHP_FUNCTION(imagecreatefromstring)
{
	zval **data;
	gdImagePtr im;
	int imtype;
	char sig[8];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_string_ex(data);
	memcpy(sig, Z_STRVAL_PP(data), 8);

	imtype = _php_image_type (sig);

	switch (imtype) {
		case PHP_GDIMG_TYPE_JPG:
#ifdef HAVE_GD_JPG
			im = _php_image_create_from_string (data, "JPEG", gdImageCreateFromJpegCtx TSRMLS_CC);
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No JPEG support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_PNG:
#ifdef HAVE_GD_PNG
			im = _php_image_create_from_string (data, "PNG", gdImageCreateFromPngCtx TSRMLS_CC);
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No PNG support in this PHP build");
			RETURN_FALSE;
#endif
			break;
			
		case PHP_GDIMG_TYPE_GIF:
#ifdef HAVE_GD_GIF_READ
			im = _php_image_create_from_string (data, "GIF", gdImageCreateFromGifCtx TSRMLS_CC);
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No GIF support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_WBM:
#ifdef HAVE_GD_WBMP
			im = _php_image_create_from_string (data, "WBMP", gdImageCreateFromWBMPCtx TSRMLS_CC);
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No WBMP support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_GD2:
#ifdef HAVE_GD_GD2
			im = _php_image_create_from_string (data, "GD2", gdImageCreateFromGd2Ctx TSRMLS_CC);
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No GD2 support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Data is not in a recognized format.");
			RETURN_FALSE;
	}

	if (!im) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't create GD Image Stream out of Data");
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
}
/* }}} */
#endif

/* {{{ _php_image_create_from
 */
static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(), gdImagePtr (*ioctx_func_p)()) 
{
	zval **file, **srcx, **srcy, **width, **height;
	gdImagePtr im = NULL;
	char *fn=NULL;
	php_stream *stream;
	FILE * fp = NULL;
	int argc=ZEND_NUM_ARGS();
	
	if ((image_type == PHP_GDIMG_TYPE_GD2PART && argc != 5) ||
		(image_type != PHP_GDIMG_TYPE_GD2PART && argc != 1) || 
		zend_get_parameters_ex(argc, &file, &srcx, &srcy, &width, &height) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_string_ex(file);

	if (argc == 5 && image_type == PHP_GDIMG_TYPE_GD2PART) {
		multi_convert_to_long_ex(4, srcx, srcy, width, height);
	}
	
	fn = Z_STRVAL_PP(file);

	stream = php_stream_open_wrapper(fn, "rb", REPORT_ERRORS|IGNORE_PATH|IGNORE_URL_WIN, NULL);
	if (stream == NULL)	{
		RETURN_FALSE;
	}

#ifndef USE_GD_IOCTX
	ioctx_func_p = NULL; /* don't allow sockets without IOCtx */
#endif

	/* try and avoid allocating a FILE* if the stream is not naturally a FILE* */
	if (php_stream_is(stream, PHP_STREAM_IS_STDIO))	{
		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)&fp, REPORT_ERRORS)) {
			goto out_err;
		}
	}
	else if (ioctx_func_p)	{
#ifdef USE_GD_IOCTX
		/* we can create an io context */
		gdIOCtx* io_ctx;
		size_t buff_size;
		char *buff;

		/* needs to be malloc (persistent) - GD will free() it later */
		buff_size = php_stream_copy_to_mem(stream, &buff, PHP_STREAM_COPY_ALL, 1);

		if(!buff_size) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Cannot read image data");
			goto out_err;
		}
		
		io_ctx = gdNewDynamicCtx(buff_size, buff);
		if(!io_ctx) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Cannot allocate GD IO context");
			goto out_err;
		}
		if (image_type == PHP_GDIMG_TYPE_GD2PART) {
			im = (*ioctx_func_p)(io_ctx, Z_LVAL_PP(srcx), Z_LVAL_PP(srcy), Z_LVAL_PP(width), Z_LVAL_PP(height));
		} else {
			im = (*ioctx_func_p)(io_ctx);
		}
		io_ctx->free(io_ctx);
#endif		
	}
	else	{
		/* try and force the stream to be FILE* */
		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO | PHP_STREAM_CAST_TRY_HARD, (void**)&fp, REPORT_ERRORS))
			goto out_err;
	}
	
	if (!im && fp)	{
		if (image_type == PHP_GDIMG_TYPE_GD2PART) {
			im = (*func_p)(fp, Z_LVAL_PP(srcx), Z_LVAL_PP(srcy), Z_LVAL_PP(width), Z_LVAL_PP(height));
		} else {
			im = (*func_p)(fp);
		}

		fflush(fp);
	}

	if (im) {
		ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
		php_stream_close(stream);
		return;
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid %s file", fn, tn);
out_err:
	php_stream_close(stream);
	RETURN_FALSE;

}
/* }}} */

#ifdef HAVE_GD_GIF_READ
/* {{{ proto int imagecreatefromgif(string filename)
   Create a new image from GIF file or URL */
PHP_FUNCTION(imagecreatefromgif)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF", gdImageCreateFromGif, gdImageCreateFromGifCtx);
}
/* }}} */
#endif /* HAVE_GD_GIF_READ */

#ifdef HAVE_GD_JPG
/* {{{ proto int imagecreatefromjpeg(string filename)
   Create a new image from JPEG file or URL */
PHP_FUNCTION(imagecreatefromjpeg)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageCreateFromJpeg, gdImageCreateFromJpegCtx);
}
/* }}} */
#endif /* HAVE_GD_JPG */

#ifdef HAVE_GD_PNG
/* {{{ proto int imagecreatefrompng(string filename)
   Create a new image from PNG file or URL */
PHP_FUNCTION(imagecreatefrompng)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImageCreateFromPng, gdImageCreateFromPngCtx);
}
/* }}} */
#endif /* HAVE_GD_PNG */

#ifdef HAVE_GD_XBM
/* {{{ proto int imagecreatefromxbm(string filename)
   Create a new image from XBM file or URL */
PHP_FUNCTION(imagecreatefromxbm)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XBM, "XBM", gdImageCreateFromXbm, NULL);
}
/* }}} */
#endif /* HAVE_GD_XBM */

/* {{{ proto int imagecreatefromxpm(string filename)
   Create a new image from XPM file or URL */
PHP_FUNCTION(imagecreatefromxpm)
{
	/*
#ifdef HAVE_GD_XPM
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XPM, "XPM", gdImageCreateFromXpm, NULL);
#else
	*/
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "No XPM support in this PHP build");
	RETURN_FALSE;
	/*
#endif
	*/
}
/* }}} */

#ifdef HAVE_GD_WBMP
/* {{{ proto int imagecreatefromwbmp(string filename)
   Create a new image from WBMP file or URL */
PHP_FUNCTION(imagecreatefromwbmp)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WBM, "WBMP", gdImageCreateFromWBMP, gdImageCreateFromWBMPCtx);
}
/* }}} */
#endif /* HAVE_GD_WBMP */

/* {{{ proto int imagecreatefromgd(string filename)
   Create a new image from GD file or URL */
PHP_FUNCTION(imagecreatefromgd)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD, "GD", gdImageCreateFromGd, gdImageCreateFromGdCtx);
}
/* }}} */

#ifdef HAVE_GD_GD2
/* {{{ proto int imagecreatefromgd2(string filename)
   Create a new image from GD2 file or URL */
PHP_FUNCTION(imagecreatefromgd2)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2, "GD2", gdImageCreateFromGd2, gdImageCreateFromGd2Ctx);
}
/* }}} */

/* {{{ proto int imagecreatefromgd2part(string filename, int srcX, int srcY, int width, int height)
   Create a new image from a given part of GD2 file or URL */
PHP_FUNCTION(imagecreatefromgd2part)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2PART, "GD2", gdImageCreateFromGd2Part, gdImageCreateFromGd2PartCtx);
}
/* }}} */
#endif /* HAVE_GD_GD2 */

/* {{{ _php_image_output
 */
static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, void (*func_p)()) 
{
	zval **imgind, **file, **quality;
	gdImagePtr im;
	char *fn = NULL;
	FILE *fp;
	int argc = ZEND_NUM_ARGS();
	int q = -1, i;

	/* The quality parameter for Wbmp stands for the threshold when called from image2wbmp() */
	/* When called from imagewbmp() the quality parameter stands for the foreground color. Default: black. */

	if (argc < 1 || argc > 3 || zend_get_parameters_ex(argc, &imgind, &file, &quality) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, imgind, -1, "Image", le_gd);

	if (argc > 1) {
		convert_to_string_ex(file);
		fn = Z_STRVAL_PP(file);
		if (argc == 3) {
			convert_to_long_ex(quality);
			q = Z_LVAL_PP(quality);
		}
	}

	if ((argc == 2) || (argc == 3 && Z_STRLEN_PP(file))) {
		if (!fn || fn == empty_string || php_check_open_basedir(fn TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid filename '%s'", fn);
			RETURN_FALSE;
		}

		fp = VCWD_FOPEN(fn, "wb");
		if (!fp) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' for writing", fn);
			RETURN_FALSE;
		}
		
		switch(image_type) {
			case PHP_GDIMG_CONVERT_WBM:
				if(q<0||q>255) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid threshold value '%d'. It must be between 0 and 255", q);
				}
			case PHP_GDIMG_TYPE_JPG:
				(*func_p)(im, fp, q);
				break;
			case PHP_GDIMG_TYPE_WBM:
				for(i=0; i < gdImageColorsTotal(im); i++) {
					if(gdImageRed(im, i) == 0) break;
				}
				(*func_p)(im, i, fp);
				break;
			default:
				(*func_p)(im, fp);
				break;
		}
		fflush(fp);
		fclose(fp);
	} else {
		int   b;
		FILE *tmp;
		char  buf[4096];

		tmp = tmpfile();
		if (tmp == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open temporary file");
			RETURN_FALSE;
		}

		switch(image_type) {
			case PHP_GDIMG_CONVERT_WBM:
				if(q<0||q>255) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid threshold value '%d'. It must be between 0 and 255", q);
				}
			case PHP_GDIMG_TYPE_JPG:
				(*func_p)(im, tmp, q);
				break;
			case PHP_GDIMG_TYPE_WBM:
				for(i=0; i < gdImageColorsTotal(im); i++) {
					if(gdImageRed(im, i) == 0) break;
				}
				(*func_p)(im, q, tmp);
				break;
			default:
				(*func_p)(im, tmp);
				break;
		}

		fseek(tmp, 0, SEEK_SET);

#if APACHE && defined(CHARSET_EBCDIC)
		/* XXX this is unlikely to work any more thies@thieso.net */

		/* This is a binary file already: avoid EBCDIC->ASCII conversion */
		ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
		while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
			php_write(buf, b TSRMLS_CC);
		}

		fclose(tmp);
		/* the temporary file is automatically deleted */
	}
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_GD_GIF_CREATE
/* {{{ proto int imagegif(int im [, string filename])
   Output GIF image to browser or file */
PHP_FUNCTION(imagegif)
{
#ifdef HAVE_GD_GIF_CTX
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF", gdImageGifCtx);
#else
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF", gdImageGif);
#endif
}
/* }}} */
#endif /* HAVE_GD_GIF_CREATE */

#ifdef HAVE_GD_PNG
/* {{{ proto int imagepng(int im [, string filename])
   Output PNG image to browser or file */
PHP_FUNCTION(imagepng)
{
#ifdef USE_GD_IOCTX
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImagePngCtx);
#else
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImagePng);
#endif
}
/* }}} */
#endif /* HAVE_GD_PNG */

#ifdef HAVE_GD_JPG
/* {{{ proto int imagejpeg(int im [, string filename [, int quality]])
   Output JPEG image to browser or file */
PHP_FUNCTION(imagejpeg)
{
#ifdef USE_GD_IOCTX
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageJpegCtx);
#else
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageJpeg);
#endif
}
/* }}} */
#endif /* HAVE_GD_JPG */

#ifdef HAVE_GD_WBMP
/* {{{ proto int imagewbmp(int im [, string filename, [, int foreground]])
   Output WBMP image to browser or file */
PHP_FUNCTION(imagewbmp)
{
#ifdef USE_GD_IOCTX
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WBM, "WBMP", gdImageWBMPCtx);
#else
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WBM, "WBMP", gdImageWBMP);
#endif
}
/* }}} */
#endif /* HAVE_GD_WBMP */

/* {{{ proto int imagegd(int im [, string filename])
   Output GD image to browser or file */
PHP_FUNCTION(imagegd)
{
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD, "GD", gdImageGd);
}
/* }}} */

#ifdef HAVE_GD_GD2
/* {{{ proto int imagegd2(int im [, string filename])
   Output GD2 image to browser or file */
PHP_FUNCTION(imagegd2)
{
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2, "GD2", gdImageGd2);
}
/* }}} */
#endif /* HAVE_GD_GD2 */

/* {{{ proto int imagedestroy(int im)
   Destroy an image */
PHP_FUNCTION(imagedestroy)
{
	zval **IM;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	zend_list_delete(Z_LVAL_PP(IM));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecolorallocate(int im, int red, int green, int blue)
   Allocate a color for an image */
PHP_FUNCTION(imagecolorallocate)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorAllocate(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

#if HAVE_LIBGD15
/* {{{ proto int imagepalettecopy(int dst, int src)
   Copy the palette from the src image onto the dst image */
PHP_FUNCTION(imagepalettecopy)
{
	zval **dstim, **srcim;
	gdImagePtr dst, src;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &dstim, &srcim) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(dst, gdImagePtr, dstim, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(src, gdImagePtr, srcim, -1, "Image", le_gd);

	gdImagePaletteCopy(dst, src);
}
/* }}} */
#endif

/* {{{ proto int imagecolorat(int im, int x, int y)
   Get the index of the color of a pixel */
PHP_FUNCTION(imagecolorat)
{
	zval **IM, **x, **y;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &IM, &x, &y) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);

#if HAVE_LIBGD20
	if(gdImageTrueColor(im)) {
		if (im->tpixels && gdImageBoundsSafe(im, Z_LVAL_PP(x), Z_LVAL_PP(y))) {
			RETURN_LONG(gdImageTrueColorPixel(im, Z_LVAL_PP(x), Z_LVAL_PP(y)));
		} else {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%ld,%ld is out of bounds", Z_LVAL_PP(x), Z_LVAL_PP(y));
			RETURN_FALSE;
		}
	} else {
#endif
		if (im->pixels && gdImageBoundsSafe(im, Z_LVAL_PP(x), Z_LVAL_PP(y))) {
#if HAVE_LIBGD13
			RETURN_LONG(im->pixels[Z_LVAL_PP(y)][Z_LVAL_PP(x)]);
#else
			RETURN_LONG(im->pixels[Z_LVAL_PP(x)][Z_LVAL_PP(y)]);
#endif
		} else {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%ld,%ld is out of bounds", Z_LVAL_PP(x), Z_LVAL_PP(y));
			RETURN_FALSE;
		}
#if HAVE_LIBGD20
	}
#endif
}
/* }}} */

/* {{{ proto int imagecolorclosest(int im, int red, int green, int blue)
   Get the index of the closest color to the specified color */
PHP_FUNCTION(imagecolorclosest)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorClosest(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

#if HAVE_COLORCLOSESTHWB
/* {{{ proto int imagecolorclosesthwb(int im, int red, int green, int blue)
   Get the index of the color which has the hue, white and blackness nearest to the given color */
PHP_FUNCTION(imagecolorclosesthwb)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;
	
	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	
	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);

	RETURN_LONG(gdImageColorClosestHWB(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */
#endif

/* {{{ proto int imagecolordeallocate(int im, int index)
   De-allocate a color for an image */
PHP_FUNCTION(imagecolordeallocate)
{
	zval **IM, **index;
	int col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM, &index) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(index);
	col = Z_LVAL_PP(index);

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		gdImageColorDeallocate(im, col);
		RETURN_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Color index %d out of range",	col);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagecolorresolve(int im, int red, int green, int blue)
   Get the index of the specified color or its closest possible alternative */
PHP_FUNCTION(imagecolorresolve)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorResolve(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

/* {{{ proto int imagecolorexact(int im, int red, int green, int blue)
   Get the index of the specified color */
PHP_FUNCTION(imagecolorexact)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorExact(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

/* {{{ proto int imagecolorset(int im, int col, int red, int green, int blue)
   Set the color for the specified palette index */
PHP_FUNCTION(imagecolorset)
{
	zval **IM, **color, **red, **green, **blue;
	int col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &IM, &color, &red, &green, &blue) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(color);
	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	col = Z_LVAL_PP(color);
	
	if (col >= 0 && col < gdImageColorsTotal(im)) {
		im->red[col]   = Z_LVAL_PP(red);
		im->green[col] = Z_LVAL_PP(green);
		im->blue[col]  = Z_LVAL_PP(blue);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imagecolorsforindex(int im, int col)
   Get the colors for an index */
PHP_FUNCTION(imagecolorsforindex)
{
	zval **IM, **index;
	int col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM, &index) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(index);
	col = Z_LVAL_PP(index);
#if HAVE_LIBGD20
	if ((col >= 0 && gdImageTrueColor(im)) || (!gdImageTrueColor(im) && col >= 0 && col < gdImageColorsTotal(im))) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value,"red",  gdImageRed(im,col));
		add_assoc_long(return_value,"green", gdImageGreen(im,col));
		add_assoc_long(return_value,"blue", gdImageBlue(im,col));
		add_assoc_long(return_value,"alpha", gdImageAlpha(im,col));
	} 
#else
	if (col >= 0 && col < gdImageColorsTotal(im)) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value,"red",  im->red[col]);
		add_assoc_long(return_value,"green", im->green[col]);
		add_assoc_long(return_value,"blue", im->blue[col]);
	}
#endif
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Color index %d out of range", col);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagegammacorrect(int im, float inputgamma, float outputgamma)
   Apply a gamma correction to a GD image */
PHP_FUNCTION(imagegammacorrect)
{
	zval **IM, **inputgamma, **outputgamma;
	gdImagePtr im;
	int i;
	double input, output;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &IM, &inputgamma, &outputgamma) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_double_ex(inputgamma);
	convert_to_double_ex(outputgamma);

	input = Z_DVAL_PP(inputgamma);
	output = Z_DVAL_PP(outputgamma);

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

#if HAVE_LIBGD20
	if (gdImageTrueColor(im))	{
		int x, y, c;

		for (y = 0; y < gdImageSY(im); y++)	{
			for (x = 0; x < gdImageSX(im); x++)	{
				c = gdImageGetPixel(im, x, y);
				gdImageSetPixel(im, x, y, 
						gdTrueColor(
							(int)((pow((pow((gdTrueColorGetRed(c) / 255.0), input)), 1.0 / output) * 255)+.5),
							(int)((pow((pow((gdTrueColorGetGreen(c) / 255.0), input)), 1.0 / output) * 255)+.5),
							(int)((pow((pow((gdTrueColorGetBlue(c) / 255.0), input)), 1.0 / output) * 255)+.5)
							)
						);
			}
		}
		RETURN_TRUE;
	}
#endif
	for (i = 0; i < gdImageColorsTotal(im); i++) {
		im->red[i]   = (int)((pow((pow((im->red[i]   / 255.0), input)), 1.0 / output) * 255)+.5);
		im->green[i] = (int)((pow((pow((im->green[i] / 255.0), input)), 1.0 / output) * 255)+.5);
		im->blue[i]  = (int)((pow((pow((im->blue[i]  / 255.0), input)), 1.0 / output) * 255)+.5);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagesetpixel(int im, int x, int y, int col)
   Set a single pixel */
PHP_FUNCTION(imagesetpixel)
{
	zval **IM, **x, **y, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &x, &y, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(col);

	gdImageSetPixel(im, Z_LVAL_PP(x), Z_LVAL_PP(y), Z_LVAL_PP(col));

	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imageline(int im, int x1, int y1, int x2, int y2, int col)
   Draw a line */
PHP_FUNCTION(imageline)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageLine(im, Z_LVAL_PP(x1), Z_LVAL_PP(y1), Z_LVAL_PP(x2), Z_LVAL_PP(y2), Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagedashedline(int im, int x1, int y1, int x2, int y2, int col)
   Draw a dashed line */
PHP_FUNCTION(imagedashedline)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageDashedLine(im, Z_LVAL_PP(x1), Z_LVAL_PP(y1), Z_LVAL_PP(x2), Z_LVAL_PP(y2), Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagerectangle(int im, int x1, int y1, int x2, int y2, int col)
   Draw a rectangle */
PHP_FUNCTION(imagerectangle)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageRectangle(im, Z_LVAL_PP(x1), Z_LVAL_PP(y1), Z_LVAL_PP(x2), Z_LVAL_PP(y2), Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagefilledrectangle(int im, int x1, int y1, int x2, int y2, int col)
   Draw a filled rectangle */
PHP_FUNCTION(imagefilledrectangle)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageFilledRectangle(im, Z_LVAL_PP(x1), Z_LVAL_PP(y1), Z_LVAL_PP(x2), Z_LVAL_PP(y2), Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagearc(int im, int cx, int cy, int w, int h, int s, int e, int col)
   Draw a partial ellipse */
PHP_FUNCTION(imagearc)
{
	zval **IM, **cx, **cy, **w, **h, **ST, **E, **col;
	gdImagePtr im;
	int e, st;

	if (ZEND_NUM_ARGS() != 8 ||	zend_get_parameters_ex(8, &IM, &cx, &cy, &w, &h, &ST, &E, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(cx);
	convert_to_long_ex(cy);
	convert_to_long_ex(w);
	convert_to_long_ex(h);
	convert_to_long_ex(ST);
	convert_to_long_ex(E);
	convert_to_long_ex(col);

	e = Z_LVAL_PP(E);
	st = Z_LVAL_PP(ST);

	if (e < 0)   e %= 360;
	if (st < 0) st %= 360;

	gdImageArc(im, Z_LVAL_PP(cx), Z_LVAL_PP(cy), Z_LVAL_PP(w), Z_LVAL_PP(h), st, e, Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto void imageellipse(resource im, int cx, int cy, int w, int h, int color)
   Draw an ellipse */
PHP_FUNCTION(imageellipse)
{
	zval **IM, **cx, **cy, **w, **h, **color;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &IM, &cx, &cy, &w, &h, &color) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(cx);
	convert_to_long_ex(cy);
	convert_to_long_ex(w);
	convert_to_long_ex(h);
	convert_to_long_ex(color);

#ifdef HAVE_GD_IMAGEELLIPSE  /* this function is missing from GD 2.0.1 */
	gdImageEllipse(im, Z_LVAL_PP(cx), Z_LVAL_PP(cy), Z_LVAL_PP(w), Z_LVAL_PP(h), Z_LVAL_PP(color));
#else
	gdImageArc(im, Z_LVAL_PP(cx), Z_LVAL_PP(cy), Z_LVAL_PP(w), Z_LVAL_PP(h), 0, 360, Z_LVAL_PP(color));
#endif

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagefilltoborder(int im, int x, int y, int border, int col)
   Flood fill to specific color */
PHP_FUNCTION(imagefilltoborder)
{
	zval **IM, **x, **y, **border, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 5 ||	zend_get_parameters_ex(5, &IM, &x, &y, &border, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(border);
	convert_to_long_ex(col);

	gdImageFillToBorder(im, Z_LVAL_PP(x), Z_LVAL_PP(y), Z_LVAL_PP(border), Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagefill(int im, int x, int y, int col)
   Flood fill */
PHP_FUNCTION(imagefill)
{
	zval **IM, **x, **y, **col;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &x, &y, &col) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(col);

	gdImageFill(im, Z_LVAL_PP(x), Z_LVAL_PP(y), Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagecolorstotal(int im)
   Find out the number of colors in an image's palette */
PHP_FUNCTION(imagecolorstotal)
{
	zval **IM;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	RETURN_LONG(gdImageColorsTotal(im));
}
/* }}} */	

/* {{{ proto int imagecolortransparent(int im [, int col])
   Define a color as transparent */
PHP_FUNCTION(imagecolortransparent)
{
	zval **IM, **COL;
	gdImagePtr im;

	switch(ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &IM) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &IM, &COL) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
		}
		convert_to_long_ex(COL);
		break;
	default:
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	if (ZEND_NUM_ARGS() > 1) {
		gdImageColorTransparent(im, Z_LVAL_PP(COL));
	}

	RETURN_LONG(gdImageGetTransparent(im));
}
/* }}} */	

/* {{{ proto int imageinterlace(int im [, int interlace])
   Enable or disable interlace */
PHP_FUNCTION(imageinterlace)
{
	zval **IM, **INT;
	gdImagePtr im;

	switch(ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &IM) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &IM, &INT) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
		}
		convert_to_long_ex(INT);
		break;
	default:
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	if (ZEND_NUM_ARGS() > 1) {
		gdImageInterlace(im, Z_LVAL_PP(INT));
	}

	RETURN_LONG(gdImageGetInterlaced(im));
}
/* }}} */	

/* {{{ php_imagepolygon
   arg = 0  normal polygon
   arg = 1  filled polygon */
/* im, points, num_points, col */
static void php_imagepolygon(INTERNAL_FUNCTION_PARAMETERS, int filled) 
{
	zval **IM, **POINTS, **NPOINTS, **COL;
	pval **var = NULL;
	gdImagePtr im;
	gdPointPtr points;	
	int npoints, col, nelem, i;

	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &POINTS, &NPOINTS, &COL) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(NPOINTS);
	convert_to_long_ex(COL);

	npoints = Z_LVAL_PP(NPOINTS);
	col = Z_LVAL_PP(COL);

	if (Z_TYPE_PP(POINTS) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "2nd argument to imagepolygon not an array");
		RETURN_FALSE;
	}

	nelem = zend_hash_num_elements(Z_ARRVAL_PP(POINTS));
	if (nelem < 6) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must have at least 3 points in your array");
		RETURN_FALSE;
	}

	if (nelem < npoints * 2) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Trying to use %d points in array with only %d points", npoints, nelem/2);
		RETURN_FALSE;
	}

	points = (gdPointPtr) emalloc(npoints * sizeof(gdPoint));

	for (i = 0; i < npoints; i++) {
		if (zend_hash_index_find(Z_ARRVAL_PP(POINTS), (i * 2), (void **) &var) == SUCCESS) {
			SEPARATE_ZVAL((var));
			convert_to_long(*var);
			points[i].x = Z_LVAL_PP(var);
		}
		if (zend_hash_index_find(Z_ARRVAL_PP(POINTS), (i * 2) + 1, (void **) &var) == SUCCESS) {
			SEPARATE_ZVAL(var);
			convert_to_long(*var);
			points[i].y = Z_LVAL_PP(var);
		}
	}

	if (filled) {
		gdImageFilledPolygon(im, points, npoints, col);
	} else {
		gdImagePolygon(im, points, npoints, col);
	}

	efree(points);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagepolygon(int im, array point, int num_points, int col)
   Draw a polygon */
PHP_FUNCTION(imagepolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefilledpolygon(int im, array point, int num_points, int col)
   Draw a filled polygon */
PHP_FUNCTION(imagefilledpolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ php_find_gd_font
 */
static gdFontPtr php_find_gd_font(int size)
{
	gdFontPtr font;
	int ind_type;
	TSRMLS_FETCH();
	
	switch (size) {
		case 1:
			 font = gdFontTiny;
			 break;
		case 2:
			 font = gdFontSmall;
			 break;
		case 3:
			 font = gdFontMediumBold;
			 break;
		case 4:
			 font = gdFontLarge;
			 break;
		case 5:
			 font = gdFontGiant;
			 break;
		default:
			font = zend_list_find(size - 5, &ind_type);
			 if (!font || ind_type != le_gd_font) {
				  if (size < 1) {
					   font = gdFontTiny;
				  } else {
					   font = gdFontGiant;
				  }
			 }
			 break;
	}

	return font;
}
/* }}} */

/* {{{ php_imagefontsize
 * arg = 0  ImageFontWidth
 * arg = 1  ImageFontHeight
 */
static void php_imagefontsize(INTERNAL_FUNCTION_PARAMETERS, int arg)
{
	zval **SIZE;
	gdFontPtr font;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &SIZE) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_long_ex(SIZE);

	font = php_find_gd_font(Z_LVAL_PP(SIZE));
	RETURN_LONG(arg ? font->h : font->w);
}
/* }}} */

/* {{{ proto int imagefontwidth(int font)
   Get font width */
PHP_FUNCTION(imagefontwidth)
{
	php_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefontheight(int font)
   Get font height */
PHP_FUNCTION(imagefontheight)
{
	php_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ php_gdimagecharup
 * workaround for a bug in gd 1.2 */
static void php_gdimagecharup(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color)
{
	int cx, cy, px, py, fline;
	cx = 0;
	cy = 0;
	if ((c < f->offset) || (c >= (f->offset + f->nchars))) {
		return;
	}
	fline = (c - f->offset) * f->h * f->w;
	for (py = y; (py > (y - f->w)); py--) {
		for (px = x; (px < (x + f->h)); px++) {
			if (f->data[fline + cy * f->w + cx]) {
				gdImageSetPixel(im, px, py, color);	
			}
			cy++;
		}
		cy = 0;
		cx++;
	}
}
/* }}} */

/* {{{ php_imagechar
 * arg = 0  ImageChar
 * arg = 1  ImageCharUp
 * arg = 2  ImageString
 * arg = 3  ImageStringUp
 */
static void php_imagechar(INTERNAL_FUNCTION_PARAMETERS, int mode) 
{
	zval **IM, **SIZE, **X, **Y, **C, **COL;
	gdImagePtr im;
	int ch = 0, col, x, y, size, i, l = 0;
	unsigned char *str = NULL;
	gdFontPtr font;

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &SIZE, &X, &Y, &C, &COL) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(SIZE);
	convert_to_long_ex(X);
	convert_to_long_ex(Y);
	convert_to_string_ex(C);
	convert_to_long_ex(COL);

	col = Z_LVAL_PP(COL);

	if (mode < 2) {
		ch = (int)((unsigned char)*(Z_STRVAL_PP(C)));
	} else {
		str = (unsigned char *) estrndup(Z_STRVAL_PP(C), Z_STRLEN_PP(C));
		l = strlen(str);
	}

	y = Z_LVAL_PP(Y);
	x = Z_LVAL_PP(X);
	size = Z_LVAL_PP(SIZE);

	font = php_find_gd_font(size);

	switch(mode) {
		case 0:
			gdImageChar(im, font, x, y, ch, col);
			break;
		case 1:
			php_gdimagecharup(im, font, x, y, ch, col);
			break;
		case 2:
			for (i = 0; (i < l); i++) {
				gdImageChar(im, font, x, y, (int)((unsigned char)str[i]),
							col);
				x += font->w;
			}
			break;
		case 3: {
			for (i = 0; (i < l); i++) {
				/* php_gdimagecharup(im, font, x, y, (int)str[i], col); */
				gdImageCharUp(im, font, x, y, (int)str[i], col);
				y -= font->w;
			}
			break;
		}
	}
	if (str) {
		efree(str);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagechar(int im, int font, int x, int y, string c, int col)
   Draw a character */ 
PHP_FUNCTION(imagechar) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagecharup(int im, int font, int x, int y, string c, int col)
   Draw a character rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagecharup) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int imagestring(int im, int font, int x, int y, string str, int col)
   Draw a string horizontally */
PHP_FUNCTION(imagestring) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto int imagestringup(int im, int font, int x, int y, string str, int col)
   Draw a string vertically - rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagestringup) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto int imagecopy(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h)
   Copy part of an image */ 
PHP_FUNCTION(imagecopy)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX;

	if (ZEND_NUM_ARGS() != 8 ||	
		zend_get_parameters_ex(8, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);

	gdImageCopy(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */

#if HAVE_LIBGD15
/* {{{ proto int imagecopymerge(int src_im, int dst_im, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h, int pct)
   Merge one part of an image with another */
PHP_FUNCTION(imagecopymerge)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY, **PCT;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX, pct;

	if (ZEND_NUM_ARGS() != 9 ||
		zend_get_parameters_ex(9, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH, &PCT) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);
	convert_to_long_ex(PCT);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);
	pct  = Z_LVAL_PP(PCT);

	gdImageCopyMerge(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH, pct);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecopymergegray(int src_im, int dst_im, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h, int pct)
   Merge one part of an image with another */
PHP_FUNCTION(imagecopymergegray)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY, **PCT;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX, pct;

	if (ZEND_NUM_ARGS() != 9 ||
		zend_get_parameters_ex(9, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH, &PCT) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);
	convert_to_long_ex(PCT);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);
	pct  = Z_LVAL_PP(PCT);

	gdImageCopyMergeGray(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH, pct);
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int imagecopyresized(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int dst_w, int dst_h, int src_w, int src_h)
   Copy and resize part of an image */
PHP_FUNCTION(imagecopyresized)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY, **DW, **DH;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;

	if (ZEND_NUM_ARGS() != 10 ||
		zend_get_parameters_ex(10, &DIM, &SIM, &DX, &DY, &SX, &SY, &DW, &DH, &SW, &SH) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);
	convert_to_long_ex(DW);
	convert_to_long_ex(DH);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);
	dstH = Z_LVAL_PP(DH);
	dstW = Z_LVAL_PP(DW);

	gdImageCopyResized(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagesx(int im)
   Get image width */
PHP_FUNCTION(imagesx)
{
	zval **IM;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	RETURN_LONG(gdImageSX(im));
}
/* }}} */

/* {{{ proto int imagesy(int im)
   Get image height */
PHP_FUNCTION(imagesy)
{
	zval **IM;
	gdImagePtr im;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	RETURN_LONG(gdImageSY(im));
}
/* }}} */

#ifdef ENABLE_GD_TTF
#define TTFTEXT_DRAW 0
#define TTFTEXT_BBOX 1
#endif

#ifdef ENABLE_GD_TTF

#if HAVE_LIBGD20 && HAVE_LIBFREETYPE && HAVE_GD_STRINGFTEX
/* {{{ proto array imageftbbox(int size, int angle, string font_file, string text[, array extrainfo])
   Give the bounding box of a text using fonts via freetype2 */
PHP_FUNCTION(imageftbbox)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX, 1);
}
/* }}} */

/* {{{ proto array imagefttext(int im, int size, int angle, int x, int y, int col, string font_file, string text, [array extrainfo])
   Write text to the image using fonts via freetype2 */
PHP_FUNCTION(imagefttext)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW, 1);
}
/* }}} */
#endif 


/* {{{ proto array imagettfbbox(int size, int angle, string font_file, string text)
   Give the bounding box of a text using TrueType fonts */
PHP_FUNCTION(imagettfbbox)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX, 0);
}
/* }}} */

/* {{{ proto array imagettftext(int im, int size, int angle, int x, int y, int col, string font_file, string text)
   Write text to the image using a TrueType font */
PHP_FUNCTION(imagettftext)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW, 0);
}
/* }}} */

/* {{{ php_imagettftext_common
 */
static void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int mode, int extended)
{
	zval **IM, **PTSIZE, **ANGLE, **X, **Y, **C, **FONTNAME, **COL, **EXT = NULL;
	gdImagePtr im=NULL;
	int col, x, y, l=0, i, brect[8];
	double ptsize, angle;
	unsigned char *str = NULL, *fontname = NULL;
	char *error;
	int argc;
#if HAVE_GD_STRINGFTEX
	gdFTStringExtra strex;
#endif

#if !HAVE_GD_STRINGFTEX
	assert(!extended);
#endif
	
	argc = ZEND_NUM_ARGS();

	if (mode == TTFTEXT_BBOX) {
		if ((extended && argc != 5) || (!extended && argc != 4) ||
			  	zend_get_parameters_ex(argc, &PTSIZE, &ANGLE, &FONTNAME, &C, &EXT) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
		}
	} else {
		if ((extended && argc != 9) || (!extended && argc != 8) ||
			  	zend_get_parameters_ex(argc, &IM, &PTSIZE, &ANGLE, &X, &Y, &COL, &FONTNAME, &C, &EXT) == FAILURE) {
			ZEND_WRONG_PARAM_COUNT();
		}
		ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	}

	convert_to_double_ex(PTSIZE);
	convert_to_double_ex(ANGLE);
	convert_to_string_ex(FONTNAME);
	convert_to_string_ex(C);

	if (mode == TTFTEXT_BBOX) {
		im = NULL;
		col = x = y = -1;
	} else {
		convert_to_long_ex(X);
		convert_to_long_ex(Y);
		convert_to_long_ex(COL);
		col = Z_LVAL_PP(COL);
		y = Z_LVAL_PP(Y);
		x = Z_LVAL_PP(X);

#if HAVE_GD_STRINGFTEX
		if (EXT)	{
			/* parse extended info */
			
			HashPosition pos;
			
			convert_to_array_ex(EXT);
			memset(&strex, 0, sizeof(strex));
			
			/* walk the assoc array */
			zend_hash_internal_pointer_reset_ex(HASH_OF(*EXT), &pos);
			do {
			  zval ** item;
			  char * key;

			  if (zend_hash_get_current_key_ex(HASH_OF(*EXT), &key, NULL, NULL, 0, &pos) == FAILURE)
				  continue;

			  if (zend_hash_get_current_data_ex(HASH_OF(*EXT), (void**)&item, &pos) == FAILURE)
				  continue;
				
			  if (strcmp("linespacing", key) == 0)	{
					convert_to_double_ex(item);
					strex.flags |= gdFTEX_LINESPACE;
					strex.linespacing = Z_DVAL_PP(item);
			  }
			  
			} while(zend_hash_move_forward_ex(HASH_OF(*EXT), &pos) == SUCCESS);
		}
#endif
		
	}

	ptsize = Z_DVAL_PP(PTSIZE);
	angle = Z_DVAL_PP(ANGLE) * (M_PI/180); /* convert to radians */

	str = (unsigned char *) Z_STRVAL_PP(C);
	l = strlen(str);

#ifdef VIRTUAL_DIR
	if(virtual_filepath(Z_STRVAL_PP(FONTNAME), &fontname TSRMLS_CC)) {
		fontname = (unsigned char*)Z_STRVAL_PP(FONTNAME);
	}
#else
	fontname = (unsigned char*)Z_STRVAL_PP(FONTNAME);
#endif


#ifdef USE_GD_IMGSTRTTF
# if HAVE_GD_STRINGFTEX
	if (extended)	{
		error = gdImageStringFTEx(im, brect, col, fontname, ptsize, angle, x, y, str, &strex);
	}
	else
# endif

# if HAVE_GD_STRINGFT
	error = gdImageStringFT(im, brect, col, fontname, ptsize, angle, x, y, str);
# elif HAVE_GD_STRINGTTF
	error = gdImageStringTTF(im, brect, col, fontname, ptsize, angle, x, y, str);
# endif

#else /* !USE_GD_IMGSTRTTF */
	error = gdttf(im, brect, col, fontname, ptsize, angle, x, y, str);
#endif


	if (error) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", error);
		RETURN_FALSE;
	}
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* return array with the text's bounding box */
	for (i = 0; i < 8; i++) {
		add_next_index_long(return_value, brect[i]);
	}
}
/* }}} */
#endif	/* ENABLE_GD_TTF */

#if HAVE_LIBT1

/* {{{ php_free_ps_font
 */
static void php_free_ps_font(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	int *font = (int *)rsrc->ptr;

	T1_DeleteFont(*font);
	efree(font);
}
/* }}} */

/* {{{ php_free_ps_enc
 */
static void php_free_ps_enc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	char **enc = (char **)rsrc->ptr;

	T1_DeleteEncoding(enc);
}
/* }}} */

/* {{{ proto int imagepsloadfont(string pathname)
   Load a new font from specified file */
PHP_FUNCTION(imagepsloadfont)
{
	zval **file;
	int f_ind, *font;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(file);

	f_ind = T1_AddFont(Z_STRVAL_PP(file));

	if (f_ind < 0) {
		switch (f_ind) {
		case -1:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't find the font file");
			RETURN_FALSE;
			break;
		case -2:
		case -3:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	if (T1_LoadFont(f_ind)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't load the font");
		RETURN_FALSE;
	}

	font = (int *) emalloc(sizeof(int));
	*font = f_ind;
	ZEND_REGISTER_RESOURCE(return_value, font, le_ps_font);
}
/* }}} */

/* {{{ proto int imagepscopyfont(int font_index)
   Make a copy of a font for purposes like extending or reenconding */
/* The function in t1lib which this function uses seem to be buggy...
PHP_FUNCTION(imagepscopyfont)
{
	zval **fnt;
	int l_ind, type;
	gd_ps_font *nf_ind, *of_ind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fnt) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_long_ex(fnt);

	of_ind = zend_list_find(Z_LVAL_PP(fnt), &type);

	if (type != le_ps_font) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d is not a Type 1 font index", Z_LVAL_PP(fnt));
		RETURN_FALSE;
	}

	nf_ind = emalloc(sizeof(gd_ps_font));
	nf_ind->font_id = T1_CopyFont(of_ind->font_id);

	if (nf_ind->font_id < 0) {
		l_ind = nf_ind->font_id;
		efree(nf_ind);
		switch (l_ind) {
		case -1:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "FontID %d is not loaded in memory", l_ind);
			RETURN_FALSE;
			break;
		case -2:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Tried to copy a logical font");
			RETURN_FALSE;
			break;
		case -3:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	nf_ind->extend = 1;
	l_ind = zend_list_insert(nf_ind, le_ps_font);
	RETURN_LONG(l_ind);
}
*/
/* }}} */

/* {{{ proto bool imagepsfreefont(int font_index)
   Free memory used by a font */
PHP_FUNCTION(imagepsfreefont)
{
	zval **fnt;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fnt) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	zend_list_delete(Z_LVAL_PP(fnt));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imagepsencodefont(int font_index, string filename)
   To change a fonts character encoding vector */
PHP_FUNCTION(imagepsencodefont)
{
	zval **fnt, **enc;
	char **enc_vector;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &enc) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(enc);

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	if ((enc_vector = T1_LoadEncoding(Z_STRVAL_PP(enc))) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't load encoding vector from %s", Z_STRVAL_PP(enc));
		RETURN_FALSE;
	}

	T1_DeleteAllSizes(*f_ind);
	if (T1_ReencodeFont(*f_ind, enc_vector)) {
		T1_DeleteEncoding(enc_vector);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't reencode font");
		RETURN_FALSE;
	}
	zend_list_insert(enc_vector, le_ps_enc);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imagepsextendfont(int font_index, float extend)
   Extend or or condense (if extend < 1) a font */
PHP_FUNCTION(imagepsextendfont)
{
	zval **fnt, **ext;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &ext) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_double_ex(ext);

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	T1_DeleteAllSizes(*f_ind);
	if (T1_ExtendFont(*f_ind, Z_DVAL_PP(ext)) != 0) RETURN_FALSE;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool imagepsslantfont(int font_index, float slant)
   Slant a font */
PHP_FUNCTION(imagepsslantfont)
{
	zval **fnt, **slt;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &slt) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_double_ex(slt);

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	if (T1_SlantFont(*f_ind, Z_DVAL_PP(slt)) != 0) RETURN_FALSE;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imagepstext(int image, string text, int font, int size, int xcoord, int ycoord [, int space, int tightness, float angle, int antialias])
   Rasterize a string over an image */
PHP_FUNCTION(imagepstext)
{
	zval **img, **str, **fnt, **sz, **fg, **bg, **sp, **px, **py, **aas, **wd, **ang;
	int i, j, x, y;
	int space;
	int *f_ind;
	int h_lines, v_lines, c_ind;
	int rd, gr, bl, fg_rd, fg_gr, fg_bl, bg_rd, bg_gr, bg_bl, _fg, _bg;
#if HAVE_LIBGD20
	int fg_al, bg_al, al;
#endif
	int aa[16], aa_steps;
	int width, amount_kern, add_width;
	double angle, extend;
	unsigned long aa_greys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	gdImagePtr bg_img;
	GLYPH *str_img;
	T1_OUTLINE *char_path, *str_path;
	T1_TMATRIX *transform = NULL;
	char *_str;
	
	switch(ZEND_NUM_ARGS()) {
	case 8:
		if (zend_get_parameters_ex(8, &img, &str, &fnt, &sz, &fg, &bg, &px, &py) == FAILURE) {
			RETURN_FALSE;
		}
		space = 0;
		aa_steps = 4;
		width = 0;
		angle = 0;
		break;
	case 12:
		if (zend_get_parameters_ex(12, &img, &str, &fnt, &sz, &fg, &bg, &px, &py, &sp, &wd, &ang, &aas) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_long_ex(sp);
		convert_to_long_ex(wd);
		convert_to_double_ex(ang);
		convert_to_long_ex(aas);
		space = Z_LVAL_PP(sp);
		width = Z_LVAL_PP(wd);
		angle = Z_DVAL_PP(ang);
		aa_steps = Z_LVAL_PP(aas);
		break;
	default:
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(str);
	convert_to_long_ex(sz);

	ZEND_FETCH_RESOURCE(bg_img, gdImagePtr, img, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	x = Z_LVAL_PP(px);
	y = Z_LVAL_PP(py);
	_fg = Z_LVAL_PP(fg);
	_bg = Z_LVAL_PP(bg);
	
	fg_rd = gdImageRed  (bg_img, _fg);
	fg_gr = gdImageGreen(bg_img, _fg);
	fg_bl = gdImageBlue (bg_img, _fg);
#if HAVE_LIBGD20
	fg_al = gdImageAlpha(bg_img, _fg);
#endif

	bg_rd = gdImageRed  (bg_img, _bg);
	bg_gr = gdImageGreen(bg_img, _bg);
	bg_bl = gdImageBlue (bg_img, _bg);
#if HAVE_LIBGD20
	bg_al = gdImageAlpha(bg_img, _bg);
#endif

	for (i = 0; i < aa_steps; i++) {
		rd = bg_rd+(double)(fg_rd-bg_rd)/aa_steps*(i+1);
		gr = bg_gr+(double)(fg_gr-bg_gr)/aa_steps*(i+1);
		bl = bg_bl+(double)(fg_bl-bg_bl)/aa_steps*(i+1);
#if HAVE_LIBGD20
		al = bg_al+(double)(fg_al-bg_al)/aa_steps*(i+1);
		aa[i] = gdImageColorResolveAlpha(bg_img, rd, gr, bl, al);
#else
		aa[i] = gdImageColorResolve(bg_img, rd, gr, bl);
#endif

	}

	T1_AASetBitsPerPixel(8);

	switch (aa_steps) {
	case 4:
		T1_AASetGrayValues(0, 1, 2, 3, 4);
		T1_AASetLevel(T1_AA_LOW);
		break;
	case 16:
		T1_AAHSetGrayValues(aa_greys);
		T1_AASetLevel(T1_AA_HIGH);
		break;
	default:
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid value %d as number of steps for antialiasing", aa_steps);
		RETURN_FALSE;
	}

	if (angle) {
		transform = T1_RotateMatrix(NULL, angle);
	}

	_str = Z_STRVAL_PP(str);

	{
		extend = T1_GetExtend(*f_ind);
		str_path = T1_GetCharOutline(*f_ind, _str[0], Z_LVAL_PP(sz), transform);

		for (i = 1; i < Z_STRLEN_PP(str); i++) {
			amount_kern = (int) T1_GetKerning(*f_ind, _str[i-1], _str[i]);
			amount_kern += _str[i-1] == ' ' ? space : 0;
			add_width = (int) (amount_kern+width)/extend;

			char_path = T1_GetMoveOutline(*f_ind, add_width, 0, 0, Z_LVAL_PP(sz), transform);
			str_path = T1_ConcatOutlines(str_path, char_path);

			char_path = T1_GetCharOutline(*f_ind, _str[i], Z_LVAL_PP(sz), transform);
			str_path = T1_ConcatOutlines(str_path, char_path);
		}
		str_img = T1_AAFillOutline(str_path, 0);
	}

	if (T1_errno) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "libt1 returned error %d", T1_errno);
		RETURN_FALSE;
	}

	h_lines = str_img->metrics.ascent -  str_img->metrics.descent;
	v_lines = str_img->metrics.rightSideBearing - str_img->metrics.leftSideBearing;

	for (i = 0; i < v_lines; i++) {
		for (j = 0; j < h_lines; j++) {
			switch (str_img->bits[j*v_lines+i]) {
			case 0:
				break;
			default:
				c_ind = aa[str_img->bits[j*v_lines+i]-1];
				gdImageSetPixel(bg_img, x+str_img->metrics.leftSideBearing+i, y-str_img->metrics.ascent+j, c_ind);
			}
		}
	}

	if (array_init(return_value) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't initialize array for returning bounding box");
		RETURN_FALSE;
	}

	add_next_index_long(return_value, str_img->metrics.leftSideBearing);
	add_next_index_long(return_value, str_img->metrics.descent);
	add_next_index_long(return_value, str_img->metrics.rightSideBearing);
	add_next_index_long(return_value, str_img->metrics.ascent);
}
/* }}} */

/* {{{ proto array imagepsbbox(string text, int font, int size [, int space, int tightness, int angle])
   Return the bounding box needed by a string if rasterized */
PHP_FUNCTION(imagepsbbox)
{
	zval **str, **fnt, **sz, **sp, **wd, **ang;
	int i, space, add_width = 0, char_width, amount_kern;
	int cur_x, cur_y, dx, dy;
	int x1, y1, x2, y2, x3, y3, x4, y4;
	int *f_ind;
	int per_char = 0;
	double angle, sin_a = 0, cos_a = 0;
	BBox char_bbox, str_bbox = {0, 0, 0, 0};

	switch(ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &str, &fnt, &sz) == FAILURE) {
			RETURN_FALSE;
		}
		space = 0;
		break;
	case 6:
		if (zend_get_parameters_ex(6, &str, &fnt, &sz, &sp, &wd, &ang) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_long_ex(sp);
		convert_to_long_ex(wd);
		convert_to_double_ex(ang);
		space = Z_LVAL_PP(sp);
		add_width = Z_LVAL_PP(wd);
		angle = Z_DVAL_PP(ang) * M_PI / 180;
		sin_a = sin(angle);
		cos_a = cos(angle);
		per_char =  add_width || angle ? 1 : 0;
		break;
	default:
		ZEND_WRONG_PARAM_COUNT();
	}

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	convert_to_string_ex(str);
	convert_to_long_ex(sz);

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
#define new_x(a, b) (int) ((a) * cos_a - (b) * sin_a)
#define new_y(a, b) (int) ((a) * sin_a + (b) * cos_a)

	if (per_char) {
		space += T1_GetCharWidth(*f_ind, ' ');
		cur_x = cur_y = 0;

		for (i = 0; i < Z_STRLEN_PP(str); i++) {
			if (Z_STRVAL_PP(str)[i] == ' ') {
				char_bbox.llx = char_bbox.lly = char_bbox.ury = 0;
				char_bbox.urx = char_width = space;
			} else {
				char_bbox = T1_GetCharBBox(*f_ind, Z_STRVAL_PP(str)[i]);
				char_width = T1_GetCharWidth(*f_ind, Z_STRVAL_PP(str)[i]);
			}
			amount_kern = i ? T1_GetKerning(*f_ind, Z_STRVAL_PP(str)[i-1], Z_STRVAL_PP(str)[i]) : 0;

			/* Transfer character bounding box to right place */
			x1 = new_x(char_bbox.llx, char_bbox.lly) + cur_x;
			y1 = new_y(char_bbox.llx, char_bbox.lly) + cur_y;
			x2 = new_x(char_bbox.llx, char_bbox.ury) + cur_x;
			y2 = new_y(char_bbox.llx, char_bbox.ury) + cur_y;
			x3 = new_x(char_bbox.urx, char_bbox.ury) + cur_x;
			y3 = new_y(char_bbox.urx, char_bbox.ury) + cur_y;
			x4 = new_x(char_bbox.urx, char_bbox.lly) + cur_x;
			y4 = new_y(char_bbox.urx, char_bbox.lly) + cur_y;

			/* Find min & max values and compare them with current bounding box */
			str_bbox.llx = min(str_bbox.llx, min(x1, min(x2, min(x3, x4))));
			str_bbox.lly = min(str_bbox.lly, min(y1, min(y2, min(y3, y4))));
			str_bbox.urx = max(str_bbox.urx, max(x1, max(x2, max(x3, x4))));
			str_bbox.ury = max(str_bbox.ury, max(y1, max(y2, max(y3, y4))));

			/* Move to the next base point */
			dx = new_x(char_width + add_width + amount_kern, 0);
			dy = new_y(char_width + add_width + amount_kern, 0);
			cur_x += dx;
			cur_y += dy;
			/*
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", x1, y1, x2, y2, x3, y3, x4, y4, char_bbox.llx, char_bbox.lly, char_bbox.urx, char_bbox.ury, char_width, amount_kern, cur_x, cur_y, dx, dy);
			*/
		}

	} else {
		str_bbox = T1_GetStringBBox(*f_ind, Z_STRVAL_PP(str), Z_STRLEN_PP(str), space, T1_KERNING);
	}
	if (T1_errno) RETURN_FALSE;
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	/*
	printf("%d %d %d %d\n", str_bbox.llx, str_bbox.lly, str_bbox.urx, str_bbox.ury);
	*/
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.llx)*Z_LVAL_PP(sz)/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.lly)*Z_LVAL_PP(sz)/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.urx)*Z_LVAL_PP(sz)/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.ury)*Z_LVAL_PP(sz)/1000));
}
/* }}} */
#endif 

#ifdef HAVE_GD_WBMP
/* {{{ proto int image2wbmp(int im [, string filename [, int threshold]])
   Output WBMP image to browser or file */
PHP_FUNCTION(image2wbmp)
{
	_php_image_output (INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_CONVERT_WBM, "WBMP", _php_image_bw_convert);
}
/* }}} */
#endif /* HAVE_GD_WBMP */

#if defined(HAVE_GD_JPG) && defined(HAVE_GD_WBMP)
/* {{{ proto void jpeg2wbmp (string f_org, string f_dest, int d_height, int d_width, int threshold)
   Convert JPEG image to WBMP image */
PHP_FUNCTION(jpeg2wbmp)
{
	_php_image_convert (INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG);
}
/* }}} */
#endif

#if defined(HAVE_GD_PNG) && defined(HAVE_GD_WBMP)
/* {{{ proto void png2wbmp (string f_org, string f_dest, int d_height, int d_width, int threshold)
   Convert PNG image to WBMP image */
PHP_FUNCTION(png2wbmp)
{
	_php_image_convert (INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG);
}
/* }}} */
#endif

#ifdef HAVE_GD_WBMP
/* {{{ _php_image_bw_convert
 * It converts a gd Image to bw using a threshold value */
static void _php_image_bw_convert( gdImagePtr im_org, gdIOCtx *out, int threshold) 
{
	gdImagePtr im_dest;
	int white, black;
	int color, color_org, median;
	int dest_height = gdImageSY (im_org);
	int dest_width = gdImageSX (im_org);
	int x, y;
	TSRMLS_FETCH();
	
	im_dest = gdImageCreate (dest_width, dest_height);
	if (im_dest == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate temporary buffer");
		return;
	}
	white = gdImageColorAllocate (im_dest, 255, 255, 255);
	if( white == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate the colors for the destination buffer");
		return;
	}

	black = gdImageColorAllocate (im_dest, 0, 0, 0);
	if (black == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate the colors for the destination buffer");
		return;
	}

	for (y = 0; y < dest_height; y++) {
		for (x = 0; x < dest_width; x++) {
			color_org = gdImageGetPixel (im_org, x, y);
			median = (im_org->red[color_org] + im_org->green[color_org] + im_org->blue[color_org]) / 3;
			if (median < threshold) {
				color = black;
			}
			else {
				color = white;
			}
			gdImageSetPixel (im_dest, x, y, color);
		}
	}
#ifdef USE_GD_IOCTX
	gdImageWBMPCtx (im_dest, black, out);
#else
	gdImageWBMP (im_dest, black, out);
#endif

}
/* }}} */

/* {{{ _php_image_convert
 * _php_image_convert converts jpeg/png images to wbmp and resizes them as needed  */
static void _php_image_convert(INTERNAL_FUNCTION_PARAMETERS, int image_type ) 
{
	zval **f_org, **f_dest, **height, **width, **threshold;
	gdImagePtr im_org, im_dest, im_tmp;
	char *fn_org = NULL;
	char *fn_dest = NULL;
	FILE *org, *dest;
	int argc;
	int dest_height = -1;
	int dest_width = -1;
	int org_height, org_width;
	int white, black;
	int color, color_org, median;
	int int_threshold;
	int x, y;
	float x_ratio, y_ratio;

	argc = ZEND_NUM_ARGS();
	if (argc != 5 || zend_get_parameters_ex(argc, &f_org, &f_dest, &height, &width, &threshold) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex (f_org);
	convert_to_string_ex (f_dest);
	fn_org  = Z_STRVAL_PP(f_org);
	fn_dest = Z_STRVAL_PP(f_dest);
	convert_to_long_ex(height);
	dest_height = Z_LVAL_PP(height);
	convert_to_long_ex(width);
	dest_width = Z_LVAL_PP(width);
	convert_to_long_ex(threshold);
	int_threshold = Z_LVAL_PP(threshold);

	/* Check threshold value */
	if( int_threshold < 0 || int_threshold > 8 ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid threshold value '%d'", int_threshold);
		RETURN_FALSE;
	}

	/* Check origin file */
	if (!fn_org || fn_org == empty_string || php_check_open_basedir(fn_org TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid origin filename '%s'", fn_org);
		RETURN_FALSE;
	}

	/* Check destination file */
	if (!fn_dest || fn_dest == empty_string || php_check_open_basedir(fn_dest TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid destination filename '%s'", fn_dest);
		RETURN_FALSE;
	}

	/* Open origin file */
	org = VCWD_FOPEN(fn_org, "rb");
	if (!org) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' for reading", fn_org);
		RETURN_FALSE;
	}

	/* Open destination file */
	dest = VCWD_FOPEN(fn_dest, "wb");
	if (!dest) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' for writing", fn_dest);
		RETURN_FALSE;
	}

	switch (image_type) {
#ifdef HAVE_GD_GIF_READ
		case PHP_GDIMG_TYPE_GIF:
			im_org = gdImageCreateFromGif (org);
			if (im_org == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' Not a valid GIF file", fn_dest);
				RETURN_FALSE;
			}
			break;
#endif /* HAVE_GD_GIF_READ */

#ifdef HAVE_GD_JPG
		case PHP_GDIMG_TYPE_JPG:
			im_org = gdImageCreateFromJpeg (org);
			if (im_org == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' Not a valid JPEG file", fn_dest);
				RETURN_FALSE;
			}
			break;
#endif /* HAVE_GD_JPG */


#ifdef HAVE_GD_PNG
		case PHP_GDIMG_TYPE_PNG:
			im_org = gdImageCreateFromPng(org);
			if (im_org == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' Not a valid PNG file", fn_dest);
				RETURN_FALSE;
			}
			break;
#endif /* HAVE_GD_PNG */

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Format not supported");
			RETURN_FALSE;
			break;
	}

	org_width  = gdImageSX (im_org);
	org_height = gdImageSY (im_org);

	x_ratio = (float) org_width / (float) dest_width;
	y_ratio = (float) org_height / (float) dest_height;

	if (x_ratio > 1 && y_ratio > 1) {
		if (y_ratio > x_ratio) {
			x_ratio = y_ratio;
		}
		else {
			y_ratio = x_ratio;
		}
		dest_width = (int)(org_width / x_ratio);
		dest_height = (int)(org_height / y_ratio);
	}
	else {
		x_ratio = (float) dest_width / (float) org_width;
		y_ratio = (float) dest_height / (float) org_height;

		if (y_ratio < x_ratio) {
			x_ratio = y_ratio;
		}
		else {
			y_ratio = x_ratio;
		}
		dest_width = (int)(org_width * x_ratio);
		dest_height = (int)(org_height * y_ratio);
	}

	im_tmp = gdImageCreate (dest_width, dest_height);
	if (im_tmp == NULL ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate temporary buffer");
		RETURN_FALSE;
	}

	gdImageCopyResized (im_tmp, im_org, 0, 0, 0, 0, dest_width, dest_height, org_width, org_height);

	gdImageDestroy(im_org);

	fclose(org);

	im_dest = gdImageCreate(dest_width, dest_height);
	if (im_dest == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate destination buffer");
		RETURN_FALSE;
	}
	white = gdImageColorAllocate(im_dest, 255, 255, 255);
	if (white == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate the colors for the destination buffer");
		RETURN_FALSE;
	}

	black = gdImageColorAllocate(im_dest, 0, 0, 0);
	if (black == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate the colors for the destination buffer");
		RETURN_FALSE;
	}

	int_threshold = int_threshold * 32;

	for (y = 0; y < dest_height; y++) {
		for(x = 0; x < dest_width; x++) {
			color_org = gdImageGetPixel (im_tmp, x, y);
			median = (im_tmp->red[color_org] + im_tmp->green[color_org] + im_tmp->blue[color_org]) / 3;
			if (median < int_threshold) {
				color = black;
			}
			else {
				color = white;
			}
			gdImageSetPixel (im_dest, x, y, color);
		}
	}

	gdImageDestroy (im_tmp );

	gdImageWBMP (im_dest, black , dest);

	fflush(dest);
	fclose(dest);

	gdImageDestroy( im_dest );
	
	RETURN_TRUE;
}
/* }}} */
#endif /* HAVE_GD_WBMP */

#endif	/* HAVE_LIBGD */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
