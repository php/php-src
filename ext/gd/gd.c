/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@guardian.no>                               |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* gd 1.2 is copyright 1994, 1995, Quest Protein Database Center, 
   Cold Spring Harbor Labs. */

/* Note that there is no code from the gd package in this file */

#include "php.h"
#include "ext/standard/head.h"
#include <math.h>
#include "SAPI.h"
#include "php3_gd.h"

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if WIN32|WINNT
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_LIBGD
#include <gd.h>
#include <gdfontt.h>  /* 1 Tiny font */
#include <gdfonts.h>  /* 2 Small font */
#include <gdfontmb.h> /* 3 Medium bold font */
#include <gdfontl.h>  /* 4 Large font */
#include <gdfontg.h>  /* 5 Giant font */
#ifdef ENABLE_GD_TTF
# include "gdttf.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef ENABLE_GD_TTF
static void php3_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int);
#endif

#ifdef THREAD_SAFE
DWORD GDlibTls;
static int numthreads=0;
void *gdlib_mutex=NULL;

typedef struct gdlib_global_struct{
	int le_gd;
	int le_gd_font;
} gdlib_global_struct;

# define GD_GLOBAL(a) gdlib_globals->a
# define GD_TLS_VARS gdlib_global_struct *gdlib_globals = TlsGetValue(GDlibTls);

#else
#  define GD_GLOBAL(a) a
#  define GD_TLS_VARS
int le_gd;
int le_gd_font;
#endif

function_entry gd_functions[] = {
	{"imagearc",				php3_imagearc,				NULL},
	{"imagechar",				php3_imagechar,				NULL},
	{"imagecharup",				php3_imagecharup,			NULL},
	{"imagecolorallocate",		php3_imagecolorallocate,	NULL},
	{"imagecolorat",		php3_imagecolorat,		NULL},
	{"imagecolorclosest",		php3_imagecolorclosest,		NULL},
	{"imagecolordeallocate",	php3_imagecolordeallocate,	NULL},
	{"imagecolorresolve",		php3_imagecolorresolve,		NULL},
	{"imagecolorexact",			php3_imagecolorexact,		NULL},
	{"imagecolorset",		php3_imagecolorset,		NULL},
	{"imagecolortransparent",	php3_imagecolortransparent,	NULL},
	{"imagecolorstotal",		php3_imagecolorstotal,		NULL},
	{"imagecolorsforindex",		php3_imagecolorsforindex,	NULL},
	{"imagecopy",			php3_imagecopy,			NULL},
	{"imagecopyresized",		php3_imagecopyresized,		NULL},
	{"imagecreate",				php3_imagecreate,			NULL},
	{"imagecreatefromgif",		php3_imagecreatefromgif,	NULL},
	{"imagedestroy",			php3_imagedestroy,			NULL},
	{"imagefill",				php3_imagefill,				NULL},
	{"imagefilledpolygon",		php3_imagefilledpolygon,	NULL},
	{"imagefilledrectangle",	php3_imagefilledrectangle,	NULL},
	{"imagefilltoborder",		php3_imagefilltoborder,		NULL},
	{"imagefontwidth",			php3_imagefontwidth,		NULL},
	{"imagefontheight",			php3_imagefontheight,		NULL},
	{"imagegif",				php3_imagegif,				NULL},
	{"imageinterlace",			php3_imageinterlace,		NULL},
	{"imageline",				php3_imageline,				NULL},
	{"imageloadfont",			php3_imageloadfont,			NULL},
	{"imagepolygon",			php3_imagepolygon,			NULL},
	{"imagerectangle",			php3_imagerectangle,		NULL},
	{"imagesetpixel",			php3_imagesetpixel,			NULL},
	{"imagestring",				php3_imagestring,			NULL},
	{"imagestringup",			php3_imagestringup,			NULL},
	{"imagesx",					php3_imagesxfn,				NULL},
	{"imagesy",					php3_imagesyfn,				NULL},
	{"imagedashedline",			php3_imagedashedline,  		NULL},
#ifdef ENABLE_GD_TTF
	{"imagettfbbox",			php3_imagettfbbox,			NULL},
	{"imagettftext",			php3_imagettftext,			NULL},
#endif
	{NULL, NULL, NULL}
};

php3_module_entry gd_module_entry = {
	"gd", gd_functions, php3_minit_gd, php3_mend_gd, NULL, NULL, php3_info_gd, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
# if PHP_31
#  include "../phpdl.h"
# else
#  include "dl/phpdl.h"
# endif
DLEXPORT php3_module_entry *get_module(void) { return &gd_module_entry; }
#endif


#define PolyMaxPoints 256


int php3_minit_gd(INIT_FUNC_ARGS)
{
#if defined(THREAD_SAFE)
	gdlib_global_struct *gdlib_globals;
	PHP3_MUTEX_ALLOC(gdlib_mutex);
	PHP3_MUTEX_LOCK(gdlib_mutex);
	numthreads++;
	if (numthreads==1){
		if (!PHP3_TLS_PROC_STARTUP(GDlibTls)){
			PHP3_MUTEX_UNLOCK(gdlib_mutex);
			PHP3_MUTEX_FREE(gdlib_mutex);
			return FAILURE;
		}
	}
	PHP3_MUTEX_UNLOCK(gdlib_mutex);
	if(!PHP3_TLS_THREAD_INIT(GDlibTls,gdlib_globals,gdlib_global_struct)){
		PHP3_MUTEX_FREE(gdlib_mutex);
		return FAILURE;
	}
#endif
	GD_GLOBAL(le_gd) = register_list_destructors(gdImageDestroy, NULL);
	GD_GLOBAL(le_gd_font) = register_list_destructors(php3_free_gd_font, NULL);
	return SUCCESS;
}

void php3_info_gd(ZEND_MODULE_INFO_FUNC_ARGS)
{
	/* need to use a PHPAPI function here because it is external module in windows */
#if HAVE_LIBGD13
	php3_printf("Version 1.3");
#else
	php3_printf("Version 1.2");
#endif
#ifdef ENABLE_GD_TTF
	php3_printf(" with FreeType support");
#endif
}

int php3_mend_gd(SHUTDOWN_FUNC_ARGS)
{
	GD_TLS_VARS;
#ifdef THREAD_SAFE
	PHP3_TLS_THREAD_FREE(gdlib_globals);
	PHP3_MUTEX_LOCK(gdlib_mutex);
	numthreads--;
	if (numthreads<1) {
		PHP3_TLS_PROC_SHUTDOWN(GDlibTls);
		PHP3_MUTEX_UNLOCK(gdlib_mutex);
		PHP3_MUTEX_FREE(gdlib_mutex);
		return SUCCESS;
	}
	PHP3_MUTEX_UNLOCK(gdlib_mutex);
#endif
	return SUCCESS;
}

/********************************************************************/
/* gdImageColorResolve is a replacement for the old fragment:       */
/*                                                                  */
/*      if ((color=gdImageColorExact(im,R,G,B)) < 0)                */
/*        if ((color=gdImageColorAllocate(im,R,G,B)) < 0)           */
/*          color=gdImageColorClosest(im,R,G,B);                    */
/*                                                                  */
/* in a single function                                             */

int
gdImageColorResolve(gdImagePtr im, int r, int g, int b)
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

void php3_free_gd_font(gdFontPtr fp)
{
	if (fp->data) {
		efree(fp->data);
	}
	efree(fp);
}

/* {{{ proto int imageloadfont(string filename)
Load a new font */
PHP_FUNCTION(imageloadfont) {
	pval *file;
	int hdr_size = sizeof(gdFont) - sizeof(char *);
	int ind, body_size, n=0, b;
	gdFontPtr font;
	FILE *fp;
	int issock=0, socketd=0;
	GD_TLS_VARS;


	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

#if WIN32|WINNT
	fp = fopen(file->value.str.val, "rb");
#else
	fp = php3_fopen_wrapper(file->value.str.val, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd);
#endif
	if (fp == NULL) {
		php3_error(E_WARNING, "ImageFontLoad: unable to open file");
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
	while (b < hdr_size && (n = fread(&font[b], 1, hdr_size - b, fp)))
		b += n;
	if (!n) {
		fclose(fp);
		efree(font);
		if (feof(fp)) {
			php3_error(E_WARNING, "ImageFontLoad: end of file while reading header");
		} else {
			php3_error(E_WARNING, "ImageFontLoad: error while reading header");
		}
		RETURN_FALSE;
	}
	body_size = font->w * font->h * font->nchars;
	font->data = emalloc(body_size);
	b = 0;
	while (b < body_size && (n = fread(&font->data[b], 1, body_size - b, fp)))
		b += n;
	if (!n) {
		fclose(fp);
		efree(font->data);
		efree(font);
		if (feof(fp)) {
			php3_error(E_WARNING, "ImageFontLoad: end of file while reading body");
		} else {
			php3_error(E_WARNING, "ImageFontLoad: error while reading body");
		}
		RETURN_FALSE;
	}
	fclose(fp);

	/* Adding 5 to the font index so we will never have font indices
	 * that overlap with the old fonts (with indices 1-5).  The first
	 * list index given out is always 1.
	 */
	ind = 5 + php3_list_insert(font, GD_GLOBAL(le_gd_font));

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imagecreate(int x_size, int y_size)
Create a new image */
PHP_FUNCTION(imagecreate)
{
	pval *x_size, *y_size;
	int ind;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &x_size, &y_size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(x_size);
	convert_to_long(y_size);

	im = gdImageCreate(x_size->value.lval, y_size->value.lval);
	ind = php3_list_insert(im, GD_GLOBAL(le_gd));		

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imagecreatefromgif(string filename)
Create a new image from file or URL */
PHP_FUNCTION(imagecreatefromgif )
{
	pval *file;
	int ind;
	gdImagePtr im;
	char *fn=NULL;
	FILE *fp;
	int issock=0, socketd=0;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

	fn = file->value.str.val;

#if WIN32|WINNT
	fp = fopen(file->value.str.val, "rb");
#else
	fp = php3_fopen_wrapper(file->value.str.val, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd);
#endif
	if (!fp) {
		php3_strip_url_passwd(fn);
		php3_error(E_WARNING,
					"ImageCreateFromGif: Unable to open %s for reading", fn);
		RETURN_FALSE;
	}

	im = gdImageCreateFromGif (fp);

	fflush(fp);
	fclose(fp);

	ind = php3_list_insert(im, GD_GLOBAL(le_gd));

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imagedestroy(int im)
Destroy an image */
PHP_FUNCTION(imagedestroy)
{
	pval *imgind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &imgind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(imgind);

	php3_list_delete(imgind->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecolorallocate(int im, int red, int green, int blue)
Allocate a color for an image */
PHP_FUNCTION(imagecolorallocate)
{
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorAllocate: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorAllocate(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* im, x, y */
/* {{{ proto int imagecolorat(int im, int x, int y)
Get the index of the color of a pixel */
PHP_FUNCTION(imagecolorat)
{
	pval *imgind, *x, *y;
	int ind, ind_type;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &imgind, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(x);
	convert_to_long(y);
	
	ind = imgind->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorAt: Unable to find image pointer");
		RETURN_FALSE;
	}
	if (gdImageBoundsSafe(im, x->value.lval, y->value.lval)) {
#if HAVE_LIBGD13
		RETURN_LONG(im->pixels[y->value.lval][x->value.lval]);
#else
		RETURN_LONG(im->pixels[x->value.lval][y->value.lval]);
#endif
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagecolorclosest(int im, int red, int green, int blue)
Get the index of the closest color to the specified color */
PHP_FUNCTION(imagecolorclosest)
{
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorClosest: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorClosest(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* {{{ proto int imagecolordeallocate(int im, int index)
De-allocate a color for an image */
PHP_FUNCTION(imagecolordeallocate)
{
	pval *imgind, *index;
	int ind, ind_type, col;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &imgind, &index) == FAILURE) {
	WRONG_PARAM_COUNT;
	}

	convert_to_long(imgind);
	convert_to_long(index);
	ind = imgind->value.lval;
	col = index->value.lval;

	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorDeallocate: Unable to find image pointer");
		RETURN_FALSE;
	}

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		gdImageColorDeallocate(im, col);
		RETURN_TRUE;
        }
        else {
                php3_error(E_WARNING, "Color index out of range");
                RETURN_FALSE;
        }
}
/* }}} */

/* {{{ proto int imagecolorresolve(int im, int red, int green, int blue)
Get the index of the specified color or its closest possible alternative */
PHP_FUNCTION(imagecolorresolve)
{
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorResolve: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorResolve(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* {{{ proto int imagecolorexact(int im, int red, int green, int blue)
Get the index of the specified color */
PHP_FUNCTION(imagecolorexact)
{
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorExact: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorExact(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* {{{ proto int imagecolorset(int im, int col, int red, int green, int blue)
Set the color for the specified palette index */
PHP_FUNCTION(imagecolorset)
{
	pval *imgind, *color, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 5 || getParameters(ht, 5, &imgind, &color, &red, &green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(color);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	col = color->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorSet: Unable to find image pointer");
		RETURN_FALSE;
	}
	if (col >= 0 && col < gdImageColorsTotal(im)) {
		im->red[col] = r;
		im->green[col] = g;
		im->blue[col] = b;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imagecolorsforindex(int im, int col)
Get the colors for an index */
PHP_FUNCTION(imagecolorsforindex)
{
	pval *imgind, *index;
	int col, ind, ind_type;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &imgind, &index) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(index);
	ind = imgind->value.lval;
	col = index->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorsForIndex: Unable to find image pointer");
		RETURN_FALSE;
	}

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value,"red",im->red[col]);
		add_assoc_long(return_value,"green",im->green[col]);
		add_assoc_long(return_value,"blue",im->blue[col]);
	}
	else {
		php3_error(E_WARNING, "Color index out of range");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagegif(int im, string filename)
Output image to browser or file */
PHP_FUNCTION(imagegif )
{
	pval *imgind, *file;
	gdImagePtr im;
	char *fn=NULL;
	FILE *fp;
	int argc;
	int ind_type;
	int output=1;
	GD_TLS_VARS;

	argc = ARG_COUNT(ht);
	if (argc < 1 || argc > 2 || getParameters(ht, argc, &imgind, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(imgind);

	if (argc == 2) {
		convert_to_string(file);
		fn = file->value.str.val;
		if (!fn || fn == empty_string || _php3_check_open_basedir(fn)) {
			php3_error(E_WARNING, "ImageGif: Invalid filename");
			RETURN_FALSE;
		}
	}

	im = php3_list_find(imgind->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageGif: unable to find image pointer");
		RETURN_FALSE;
	}

	if (argc == 2) {
		fp = fopen(fn, "wb");
		if (!fp) {
			php3_error(E_WARNING, "ImageGif: unable to open %s for writing", fn);
			RETURN_FALSE;
		}
		gdImageGif (im,fp);
		fflush(fp);
		fclose(fp);
	}
	else {
		int   b;
		FILE *tmp;
		char  buf[4096];

		tmp = tmpfile();
		if (tmp == NULL) {
			php3_error(E_WARNING, "Unable to open temporary file");
			RETURN_FALSE;
		}

		output = php3_header();

		if (output) {
			SLS_FETCH();
			
			gdImageGif (im, tmp);
			fseek(tmp, 0, SEEK_SET);
#if APACHE && defined(CHARSET_EBCDIC)
			/* This is a binary file already: avoid EBCDIC->ASCII conversion */
			ap_bsetflag(((request_rec *) SG(server_context))->connection->client, B_EBCDIC2ASCII, 0);
#endif
			while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
				php3_write(buf, b);
			}
		}

		fclose(tmp);
		/* the temporary file is automatically deleted */
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagesetpixel(int im, int x, int y, int col)
Set a single pixel */
PHP_FUNCTION(imagesetpixel)
{
	pval *imarg, *xarg, *yarg, *colarg;
	gdImagePtr im;
	int col, y, x;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &imarg, &xarg, &yarg, &colarg) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(imarg);
	convert_to_long(xarg);
	convert_to_long(yarg);
	convert_to_long(colarg);

	col = colarg->value.lval;
	y = yarg->value.lval;
	x = xarg->value.lval;

	im = php3_list_find(imarg->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageSetPixel(im,x,y,col);

	RETURN_TRUE;
}
/* }}} */	

/* im, x1, y1, x2, y2, col */
/* {{{ proto int imageline(int im, int x1, int y1, int x2, int y2, int col)
Draw a line */
PHP_FUNCTION(imageline)
{
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageLine(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagedashedline(int im, int x1, int y1, int x2, int y2, int col)
Draw a dashed line */
PHP_FUNCTION(imagedashedline)
{
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 || getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageDashedLine(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */

/* im, x1, y1, x2, y2, col */
/* {{{ proto int imagerectangle(int im, int x1, int y1, int x2, int y2, int col)
Draw a rectangle */
PHP_FUNCTION(imagerectangle)
{
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageRectangle(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */	

/* im, x1, y1, x2, y2, col */
/* {{{ proto int imagefilledrectangle(int im, int x1, int y1, int x2, int y2, int col)
Draw a filled rectangle */
PHP_FUNCTION(imagefilledrectangle)
{
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageFilledRectangle(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagearc(int im, int cx, int cy, int w, int h, int s, int e, int col)
Draw a partial ellipse */
PHP_FUNCTION(imagearc)
{
	pval *COL, *E, *ST, *H, *W, *CY, *CX, *IM;
	gdImagePtr im;
	int col, e, st, h, w, cy, cx;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 8 ||
		getParameters(ht, 8, &IM, &CX, &CY, &W, &H, &ST, &E, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(CX);
	convert_to_long(CY);
	convert_to_long(W);
	convert_to_long(H);
	convert_to_long(ST);
	convert_to_long(E);
	convert_to_long(COL);

	col = COL->value.lval;
	e = E->value.lval;
	st = ST->value.lval;
	h = H->value.lval;
	w = W->value.lval;
	cy = CY->value.lval;
	cx = CX->value.lval;

	if (e < 0) {
		e %= 360;
	}
	if (st < 0) {
		st %= 360;
	}

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageArc(im,cx,cy,w,h,st,e,col);
	RETURN_TRUE;
}
/* }}} */	

/* im, x, y, border, col */
/* {{{ proto int imagefilltoborder(int im, int x, int y, int border, int col)
Flood fill to specific color */
PHP_FUNCTION(imagefilltoborder)
{
	pval *IM, *X, *Y, *BORDER, *COL;
	gdImagePtr im;
	int col, border, y, x;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 5 ||
		getParameters(ht, 5, &IM, &X, &Y, &BORDER, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X);
	convert_to_long(Y);
	convert_to_long(BORDER);
	convert_to_long(COL);

	col = COL->value.lval;
	border = BORDER->value.lval;
	y = Y->value.lval;
	x = X->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageFillToBorder(im,x,y,border,col);
	RETURN_TRUE;
}
/* }}} */	

/* im, x, y, col */
/* {{{ proto int imagefill(int im, int x, int y, int col)
Flood fill */
PHP_FUNCTION(imagefill)
{
	pval *IM, *X, *Y, *COL;
	gdImagePtr im;
	int col, y, x;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &IM, &X, &Y, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X);
	convert_to_long(Y);
	convert_to_long(COL);

	col = COL->value.lval;
	y = Y->value.lval;
	x = X->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageFill(im,x,y,col);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagecolorstotal(int im)
Find out the number of colors in an image's palette */
PHP_FUNCTION(imagecolorstotal)
{
	pval *IM;
	gdImagePtr im;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(IM);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageColorsTotal(im));
}
/* }}} */	

/* im, col */
/* {{{ proto int imagecolortransparent(int im [, int col])
Define a color as transparent */
PHP_FUNCTION(imagecolortransparent)
{
	pval *IM, *COL = NULL;
	gdImagePtr im;
	int col;
	int ind_type;
	GD_TLS_VARS;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &IM) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht, 2, &IM, &COL) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(COL);
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_long(IM);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	if (COL != NULL) {
		col = COL->value.lval;
		gdImageColorTransparent(im,col);
	}
	col = gdImageGetTransparent(im);
	RETURN_LONG(col);
}
/* }}} */	

/* im, interlace */
/* {{{ proto int imageinterlace(int im [, int interlace])
Enable or disable interlace */
PHP_FUNCTION(imageinterlace)
{
	pval *IM, *INT = NULL;
	gdImagePtr im;
	int interlace;
	int ind_type;
	GD_TLS_VARS;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &IM) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht, 2, &IM, &INT) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(INT);
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_long(IM);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	if (INT != NULL) {
		interlace = INT->value.lval;
		gdImageInterlace(im,interlace);
	}
	interlace = gdImageGetInterlaced(im);
	RETURN_LONG(interlace);
}
/* }}} */	

/* arg = 0  normal polygon
   arg = 1  filled polygon */
/* im, points, num_points, col */
static void _php3_imagepolygon(INTERNAL_FUNCTION_PARAMETERS, int filled) {
	pval *IM, *POINTS, *NPOINTS, *COL, **var;
	gdImagePtr im;
	gdPoint points[PolyMaxPoints];	
	int npoints, col, nelem, i;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &IM, &POINTS, &NPOINTS, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(NPOINTS);
	convert_to_long(COL);

	npoints = NPOINTS->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	if (POINTS->type != IS_ARRAY) {
		php3_error(E_WARNING, "2nd argument to imagepolygon not an array");
		RETURN_FALSE;
	}

/*
    ** we shouldn't need this check, should we? **

    if (!ParameterPassedByReference(ht, 2)) {
        php3_error(E_WARNING, "2nd argument to imagepolygon not passed by reference");
		RETURN_FALSE;
    }
*/

	nelem = _php3_hash_num_elements(POINTS->value.ht);
	if (nelem < 6) {
		php3_error(E_WARNING,
					"you must have at least 3 points in your array");
		RETURN_FALSE;
	}

	if (nelem < npoints * 2) {
		php3_error(E_WARNING,
					"trying to use %d points in array with only %d points",
					npoints, nelem/2);
		RETURN_FALSE;
	}

	if (npoints > PolyMaxPoints) {
		php3_error(E_WARNING, "maximum %d points", PolyMaxPoints);
		RETURN_FALSE;
	}

	for (i = 0; i < npoints; i++) {
		if (_php3_hash_index_find(POINTS->value.ht, (i * 2), (void **)&var) == SUCCESS) {
			SEPARATE_ZVAL(var);
			convert_to_long(*var);
			points[i].x = (*var)->value.lval;
		}
		if (_php3_hash_index_find(POINTS->value.ht, (i * 2) + 1, (void **)&var) == SUCCESS) {
			SEPARATE_ZVAL(var);
			convert_to_long(*var);
			points[i].y = (*var)->value.lval;
		}
	}

	if (filled) {
		gdImageFilledPolygon(im, points, npoints, col);
	}
	else {
		gdImagePolygon(im, points, npoints, col);
	}

	RETURN_TRUE;
}


/* {{{ proto int imagepolygon(int im, array point, int num_points, int col)
Draw a polygon */
PHP_FUNCTION(imagepolygon)
{
	_php3_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefilledpolygon(int im, array point, int num_points, int col)
Draw a filled polygon */
PHP_FUNCTION(imagefilledpolygon)
{
	_php3_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


static gdFontPtr _php3_find_gd_font(int size)
{
	gdFontPtr font;
	int ind_type;
	GD_TLS_VARS;

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
			font = php3_list_find(size - 5, &ind_type);
			 if (!font || ind_type != GD_GLOBAL(le_gd_font)) {
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


/*
 * arg = 0  ImageFontWidth
 * arg = 1  ImageFontHeight
 */
static void _php3_imagefontsize(INTERNAL_FUNCTION_PARAMETERS, int arg)
{
	pval *SIZE;
	gdFontPtr font;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &SIZE) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(SIZE);
	font = _php3_find_gd_font(SIZE->value.lval);
	RETURN_LONG(arg ? font->h : font->w);
}


/* {{{ proto int imagefontwidth(int font)
Get font width */
PHP_FUNCTION(imagefontwidth)
{
	_php3_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefontheight(int font)
Get font height */
PHP_FUNCTION(imagefontheight)
{
	_php3_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* workaround for a bug in gd 1.2 */
void _php3_gdimagecharup(gdImagePtr im, gdFontPtr f, int x, int y, int c,
						 int color)
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

/*
 * arg = 0  ImageChar
 * arg = 1  ImageCharUp
 * arg = 2  ImageString
 * arg = 3  ImageStringUp
 */
static void _php3_imagechar(INTERNAL_FUNCTION_PARAMETERS, int mode) {
	pval *IM, *SIZE, *X, *Y, *C, *COL;
	gdImagePtr im;
	int ch = 0, col, x, y, size, i, l = 0;
	unsigned char *string = NULL;
	int ind_type;
	gdFontPtr font;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &SIZE, &X, &Y, &C, &COL) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(SIZE);
	convert_to_long(X);
	convert_to_long(Y);
	convert_to_string(C);
	convert_to_long(COL);

	col = COL->value.lval;

	if (mode < 2) {
		ch = (int)((unsigned char)*(C->value.str.val));
	} else {
		string = (unsigned char *) estrndup(C->value.str.val,C->value.str.len);
		l = strlen(string);
	}

	y = Y->value.lval;
	x = X->value.lval;
	size = SIZE->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		if (string) {
			efree(string);
		}
		RETURN_FALSE;
	}
	
	font = _php3_find_gd_font(size);

	switch(mode) {
    	case 0:
			gdImageChar(im, font, x, y, ch, col);
			break;
    	case 1:
			_php3_gdimagecharup(im, font, x, y, ch, col);
			break;
    	case 2:
			for (i = 0; (i < l); i++) {
				gdImageChar(im, font, x, y, (int)((unsigned char)string[i]),
							col);
				x += font->w;
			}
			break;
    	case 3: {
			for (i = 0; (i < l); i++) {
				/* _php3_gdimagecharup(im, font, x, y, (int)string[i], col); */
				gdImageCharUp(im, font, x, y, (int)string[i], col);
				y -= font->w;
			}
			break;
		}
	}
	if (string) {
		efree(string);
	}
	RETURN_TRUE;
}	

/* {{{ proto int imagechar(int im, int font, int x, int y, string c, int col)
Draw a character */ 
PHP_FUNCTION(imagechar) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagecharup(int im, int font, int x, int y, string c, int col)
Draw a character rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagecharup) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int imagestring(int im, int font, int x, int y, string str, int col)
Draw a string horizontally */
PHP_FUNCTION(imagestring) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto int imagestringup(int im, int font, int x, int y, string str, int col)
Draw a string vertically - rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagestringup) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto int imagecopy(int dst_im, int src_im, int dstX, int dstY, int srcX, int srcY, int srcW, int srcH)
Copy part of an image */ 
PHP_FUNCTION(imagecopy)
{
	pval *SIM, *DIM, *SX, *SY, *SW, *SH, *DX, *DY;
	gdImagePtr im_dst;
	gdImagePtr im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 8 ||
		getParameters(ht, 8, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH)
						 == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(SIM);
	convert_to_long(DIM);
	convert_to_long(SX);
	convert_to_long(SY);
	convert_to_long(SW);
	convert_to_long(SH);
	convert_to_long(DX);
	convert_to_long(DY);

	srcX = SX->value.lval;
	srcY = SY->value.lval;
	srcH = SH->value.lval;
	srcW = SW->value.lval;
	dstX = DX->value.lval;
	dstY = DY->value.lval;

	im_src = php3_list_find(SIM->value.lval, &ind_type);
	if (!im_src || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	im_dst = php3_list_find(DIM->value.lval, &ind_type);
	if (!im_dst || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageCopy(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecopyresized(int dst_im, int src_im, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);
Copy and resize part of an image */
PHP_FUNCTION(imagecopyresized)
{
	pval *SIM, *DIM, *SX, *SY, *SW, *SH, *DX, *DY, *DW, *DH;
	gdImagePtr im_dst;
	gdImagePtr im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 10 ||
		getParameters(ht, 10, &DIM, &SIM, &DX, &DY, &SX, &SY, &DW, &DH,
					  &SW, &SH) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(SIM);
	convert_to_long(DIM);
	convert_to_long(SX);
	convert_to_long(SY);
	convert_to_long(SW);
	convert_to_long(SH);
	convert_to_long(DX);
	convert_to_long(DY);
	convert_to_long(DW);
	convert_to_long(DH);

	srcX = SX->value.lval;
	srcY = SY->value.lval;
	srcH = SH->value.lval;
	srcW = SW->value.lval;
	dstX = DX->value.lval;
	dstY = DY->value.lval;
	dstH = DH->value.lval;
	dstW = DW->value.lval;

	im_src = php3_list_find(SIM->value.lval, &ind_type);
	if (!im_src || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	im_dst = php3_list_find(DIM->value.lval, &ind_type);
	if (!im_dst || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageCopyResized(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH,
					   srcW, srcH);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagesx(int im)
Get image width */
PHP_FUNCTION(imagesxfn)
{
	pval *IM;
	gdImagePtr im;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageSX(im));
}
/* }}} */

/* {{{ proto int imagesy(int im)
Get image height */
PHP_FUNCTION(imagesyfn)
{
	pval *IM;
	gdImagePtr im;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageSY(im));
}
/* }}} */

#ifdef ENABLE_GD_TTF

#define TTFTEXT_DRAW 0
#define TTFTEXT_BBOX 1

/* {{{ proto array imagettfbbox(int size, int angle, string font_file, string text)
Give the bounding box of a text using TrueType fonts */
PHP_FUNCTION(imagettfbbox)
{
	php3_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX);
}
/* }}} */

/* {{{ proto array imagettftext(int im, int size, int angle, int x, int y, int col, string font_file, string text)
Write text to the image using a TrueType font */
PHP_FUNCTION(imagettftext)
{
	php3_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW);
}
/* }}} */

static
void php3_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	pval *IM, *PTSIZE, *ANGLE, *X, *Y, *C, *FONTNAME, *COL;
	gdImagePtr im;
	int  col, x, y, l=0, i;
	int brect[8];
	double ptsize, angle;
	unsigned char *string = NULL, *fontname = NULL;
	int ind_type;
	char				*error;

	GD_TLS_VARS;

	if (mode == TTFTEXT_BBOX) {
		if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &PTSIZE, &ANGLE, &FONTNAME, &C) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		if (ARG_COUNT(ht) != 8 || getParameters(ht, 8, &IM, &PTSIZE, &ANGLE, &X, &Y, &COL, &FONTNAME, &C) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	}

	convert_to_double(PTSIZE);
	convert_to_double(ANGLE);
	convert_to_string(FONTNAME);
	convert_to_string(C);
	if (mode == TTFTEXT_BBOX) {
              im = NULL;
		col = x = y = -1;
	} else {
		convert_to_long(X);
		convert_to_long(Y);
		convert_to_long(IM);
		convert_to_long(COL);
		col = COL->value.lval;
		y = Y->value.lval;
		x = X->value.lval;
              im = php3_list_find(IM->value.lval, &ind_type);
              if (!im || ind_type != GD_GLOBAL(le_gd)) {
                      php3_error(E_WARNING, "Unable to find image pointer");
                      RETURN_FALSE;
              }
	}

	ptsize = PTSIZE->value.dval;
	angle = ANGLE->value.dval * (M_PI/180); /* convert to radians */

	string = (unsigned char *) C->value.str.val;
	l = strlen(string);
	fontname = (unsigned char *) FONTNAME->value.str.val;

	error = gdttf(im, brect, col, fontname, ptsize, angle, x, y, string);

	if (error) {
		php3_error(E_WARNING, error);
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
#endif
#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
