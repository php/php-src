/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Bakken <ssb@php.net>                                   |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* gd 1.2 is copyright 1994, 1995, Quest Protein Database Center,
   Cold Spring Harbor Labs. */

/* Note that there is no code from the gd package in this file */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/head.h"
#include <math.h>
#include "SAPI.h"
#include "php_gd.h"
#include "ext/standard/php_image.h"
#include "ext/standard/info.h"
#include "php_open_temporary_file.h"
#include "php_memory_streams.h"
#include "zend_object_handlers.h"

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
# include <windows.h>
# include <Winuser.h>
# include <Wingdi.h>
#endif

#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
# include <X11/xpm.h>
#endif

#include "gd_compat.h"

#ifdef HAVE_GD_BUNDLED
# include "libgd/gd.h"
# include "libgd/gd_errors.h"
# include "libgd/gdfontt.h"  /* 1 Tiny font */
# include "libgd/gdfonts.h"  /* 2 Small font */
# include "libgd/gdfontmb.h" /* 3 Medium bold font */
# include "libgd/gdfontl.h"  /* 4 Large font */
# include "libgd/gdfontg.h"  /* 5 Giant font */
#else
# include <gd.h>
# include <gd_errors.h>
# include <gdfontt.h>  /* 1 Tiny font */
# include <gdfonts.h>  /* 2 Small font */
# include <gdfontmb.h> /* 3 Medium bold font */
# include <gdfontl.h>  /* 4 Large font */
# include <gdfontg.h>  /* 5 Giant font */
#endif

#if defined(HAVE_GD_FREETYPE) && defined(HAVE_GD_BUNDLED)
# include <ft2build.h>
# include FT_FREETYPE_H
#endif

#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
# include "X11/xpm.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* don't used libgd constants, not used, so going to be removed */
#define PHP_GD_FLIP_HORIZONTAL 1
#define PHP_GD_FLIP_VERTICAL   2
#define PHP_GD_FLIP_BOTH       3

#ifdef HAVE_GD_FREETYPE
static void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int);
#endif

#include "gd_arginfo.h"

/* as it is not really public, duplicate declaration here to avoid
   pointless warnings */
int overflow2(int a, int b);

static void php_image_filter_negate(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_grayscale(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_brightness(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_contrast(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_colorize(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_edgedetect(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_emboss(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_gaussian_blur(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_selective_blur(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_mean_removal(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_smooth(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_pixelate(INTERNAL_FUNCTION_PARAMETERS);
static void php_image_filter_scatter(INTERNAL_FUNCTION_PARAMETERS);

/* End Section filters declarations */
static gdImagePtr _php_image_create_from_string(zend_string *Data, char *tn, gdImagePtr (*ioctx_func_p)(gdIOCtxPtr));
static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(FILE *), gdImagePtr (*ioctx_func_p)(gdIOCtxPtr));
static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn);
static gdIOCtx *create_stream_context_from_zval(zval *to_zval);
static gdIOCtx *create_stream_context(php_stream *stream, int close_stream);
static gdIOCtx *create_output_context(void);
static int _php_image_type(zend_string *data);

/* output streaming (formerly gd_ctx.c) */
static void _php_image_output_ctx(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn);

/*********************************************************
 *
 * GD Object Representation
 *
 ********************************************************/

zend_class_entry *gd_image_ce;

typedef struct _gd_ext_image_object {
	gdImagePtr image;
	zend_object std;
} php_gd_image_object;

static zend_object_handlers php_gd_image_object_handlers;

static zend_function *php_gd_image_object_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "You cannot initialize a GdImage object except through helper functions");
	return NULL;
}

/**
 * Returns the underlying php_gd_image_object from a zend_object
 */

static zend_always_inline php_gd_image_object* php_gd_exgdimage_from_zobj_p(zend_object* obj)
{
	return (php_gd_image_object *) ((char *) (obj) - XtOffsetOf(php_gd_image_object, std));
}

/**
 * Converts an extension GdImage instance contained within a zval into the gdImagePtr
 * for use with library APIs
 */
PHP_GD_API gdImagePtr php_gd_libgdimageptr_from_zval_p(zval* zp)
{
	return php_gd_exgdimage_from_zobj_p(Z_OBJ_P(zp))->image;
}


zend_object *php_gd_image_object_create(zend_class_entry *class_type)
{
	size_t block_len = sizeof(php_gd_image_object) + zend_object_properties_size(class_type);
	php_gd_image_object *intern = emalloc(block_len);
	memset(intern, 0, block_len);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &php_gd_image_object_handlers;

	return &intern->std;
}

static void php_gd_image_object_free(zend_object *intern)
{
	php_gd_image_object *img_obj_ptr = php_gd_exgdimage_from_zobj_p(intern);
	if (img_obj_ptr->image) {
		gdImageDestroy(img_obj_ptr->image);
	}
	zend_object_std_dtor(intern);
}

/**
 * Creates a new GdImage object wrapping the gdImagePtr and attaches it
 * to the zval (usually return_value).
 *
 * This function must only be called once per valid gdImagePtr
 */
void php_gd_assign_libgdimageptr_as_extgdimage(zval *val, gdImagePtr image)
{
	object_init_ex(val, gd_image_ce);
	php_gd_exgdimage_from_zobj_p(Z_OBJ_P(val))->image = image;
}

static void php_gd_object_minit_helper(void)
{
	gd_image_ce = register_class_GdImage();
	gd_image_ce->create_object = php_gd_image_object_create;

	/* setting up the object handlers for the GdImage class */
	memcpy(&php_gd_image_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_gd_image_object_handlers.clone_obj = NULL;
	php_gd_image_object_handlers.free_obj = php_gd_image_object_free;
	php_gd_image_object_handlers.get_constructor = php_gd_image_object_get_constructor;
	php_gd_image_object_handlers.compare = zend_objects_not_comparable;
	php_gd_image_object_handlers.offset = XtOffsetOf(php_gd_image_object, std);
}

static zend_class_entry *gd_font_ce = NULL;
static zend_object_handlers php_gd_font_object_handlers;

typedef struct _php_gd_font_object {
	gdFontPtr font;
	zend_object std;
} php_gd_font_object;

static php_gd_font_object *php_gd_font_object_from_zend_object(zend_object *zobj)
{
	return ((php_gd_font_object*)(zobj + 1)) - 1;
}

static zend_object *php_gd_font_object_to_zend_object(php_gd_font_object *obj)
{
	return ((zend_object*)(obj + 1)) - 1;
}

static zend_object *php_gd_font_object_create(zend_class_entry *ce)
{
	php_gd_font_object *obj = zend_object_alloc(sizeof(php_gd_font_object), ce);
	zend_object *zobj = php_gd_font_object_to_zend_object(obj);

	obj->font = NULL;
	zend_object_std_init(zobj, ce);
	object_properties_init(zobj, ce);
	zobj->handlers = &php_gd_font_object_handlers;

	return zobj;
}

static void php_gd_font_object_free(zend_object *zobj)
{
	php_gd_font_object *obj = php_gd_font_object_from_zend_object(zobj);

	if (obj->font) {
		if (obj->font->data) {
			efree(obj->font->data);
		}
		efree(obj->font);
		obj->font = NULL;
	}

	zend_object_std_dtor(zobj);
}

static zend_function *php_gd_font_object_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "You cannot initialize a GdFont object except through helper functions");
	return NULL;
}

static void php_gd_font_minit_helper(void)
{
	gd_font_ce = register_class_GdFont();
	gd_font_ce->create_object = php_gd_font_object_create;

	/* setting up the object handlers for the GdFont class */
	memcpy(&php_gd_font_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_gd_font_object_handlers.clone_obj = NULL;
	php_gd_font_object_handlers.free_obj = php_gd_font_object_free;
	php_gd_font_object_handlers.get_constructor = php_gd_font_object_get_constructor;
	php_gd_font_object_handlers.offset = XtOffsetOf(php_gd_font_object, std);
}

/*********************************************************
 *
 * Extension Implementation
 *
 ********************************************************/

zend_module_entry gd_module_entry = {
	STANDARD_MODULE_HEADER,
	"gd",
	ext_functions,
	PHP_MINIT(gd),
	PHP_MSHUTDOWN(gd),
	NULL,
	PHP_RSHUTDOWN(gd),
	PHP_MINFO(gd),
	PHP_GD_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GD
ZEND_GET_MODULE(gd)
#endif

/* {{{ PHP_INI_BEGIN */
PHP_INI_BEGIN()
	PHP_INI_ENTRY_EX("gd.jpeg_ignore_warning", "1", PHP_INI_ALL, NULL, zend_ini_boolean_displayer_cb)
PHP_INI_END()
/* }}} */

/* {{{ php_gd_error_method */
void php_gd_error_method(int type, const char *format, va_list args)
{
	switch (type) {
#ifndef PHP_WIN32
		case GD_DEBUG:
		case GD_INFO:
#endif
		case GD_NOTICE:
			type = E_NOTICE;
			break;
		case GD_WARNING:
			type = E_WARNING;
			break;
		default:
			type = E_ERROR;
	}
	php_verror(NULL, "", type, format, args);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(gd)
{
	php_gd_object_minit_helper();
	php_gd_font_minit_helper();

#if defined(HAVE_GD_FREETYPE) && defined(HAVE_GD_BUNDLED)
	gdFontCacheMutexSetup();
#endif
	gdSetErrorMethod(php_gd_error_method);

	REGISTER_INI_ENTRIES();

	register_gd_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(gd)
{
#if defined(HAVE_GD_FREETYPE) && defined(HAVE_GD_BUNDLED)
	gdFontCacheMutexShutdown();
#endif
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(gd)
{
#ifdef HAVE_GD_FREETYPE
	gdFontCacheShutdown();
#endif
	return SUCCESS;
}
/* }}} */

#ifdef HAVE_GD_BUNDLED
#define PHP_GD_VERSION_STRING "bundled (2.1.0 compatible)"
#else
# define PHP_GD_VERSION_STRING GD_VERSION_STRING
#endif

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(gd)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "GD Support", "enabled");

	/* need to use a PHPAPI function here because it is external module in windows */

#ifdef HAVE_GD_BUNDLED
	php_info_print_table_row(2, "GD Version", PHP_GD_VERSION_STRING);
#else
	php_info_print_table_row(2, "GD headers Version", PHP_GD_VERSION_STRING);
#ifdef HAVE_GD_LIBVERSION
	php_info_print_table_row(2, "GD library Version", gdVersionString());
#endif
#endif

#ifdef HAVE_GD_FREETYPE
	php_info_print_table_row(2, "FreeType Support", "enabled");
	php_info_print_table_row(2, "FreeType Linkage", "with freetype");
#ifdef HAVE_GD_BUNDLED
	{
		char tmp[256];

#ifdef FREETYPE_PATCH
		snprintf(tmp, sizeof(tmp), "%d.%d.%d", FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
#elif defined(FREETYPE_MAJOR)
		snprintf(tmp, sizeof(tmp), "%d.%d", FREETYPE_MAJOR, FREETYPE_MINOR);
#else
		snprintf(tmp, sizeof(tmp), "1.x");
#endif
		php_info_print_table_row(2, "FreeType Version", tmp);
	}
#endif
#endif

	php_info_print_table_row(2, "GIF Read Support", "enabled");
	php_info_print_table_row(2, "GIF Create Support", "enabled");

#ifdef HAVE_GD_JPG
	{
		php_info_print_table_row(2, "JPEG Support", "enabled");
#ifdef HAVE_GD_BUNDLED
		php_info_print_table_row(2, "libJPEG Version", gdJpegGetVersionString());
#endif
	}
#endif

#ifdef HAVE_GD_PNG
	php_info_print_table_row(2, "PNG Support", "enabled");
#ifdef HAVE_GD_BUNDLED
	php_info_print_table_row(2, "libPNG Version", gdPngGetVersionString());
#endif
#endif
	php_info_print_table_row(2, "WBMP Support", "enabled");
#ifdef HAVE_GD_XPM
	php_info_print_table_row(2, "XPM Support", "enabled");
#ifdef HAVE_GD_BUNDLED
	{
		char tmp[12];
		snprintf(tmp, sizeof(tmp), "%d", XpmLibraryVersion());
		php_info_print_table_row(2, "libXpm Version", tmp);
	}
#endif
#endif
	php_info_print_table_row(2, "XBM Support", "enabled");
#ifdef USE_GD_JISX0208
	php_info_print_table_row(2, "JIS-mapped Japanese Font Support", "enabled");
#endif
#ifdef HAVE_GD_WEBP
	php_info_print_table_row(2, "WebP Support", "enabled");
#endif
#ifdef HAVE_GD_BMP
	php_info_print_table_row(2, "BMP Support", "enabled");
#endif
#ifdef HAVE_GD_AVIF
	php_info_print_table_row(2, "AVIF Support", "enabled");
#endif
#ifdef HAVE_GD_TGA
	php_info_print_table_row(2, "TGA Read Support", "enabled");
#endif
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ */
PHP_FUNCTION(gd_info)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	add_assoc_string(return_value, "GD Version", PHP_GD_VERSION_STRING);

#ifdef HAVE_GD_FREETYPE
	add_assoc_bool(return_value, "FreeType Support", 1);
	add_assoc_string(return_value, "FreeType Linkage", "with freetype");
#else
	add_assoc_bool(return_value, "FreeType Support", 0);
#endif
	add_assoc_bool(return_value, "GIF Read Support", 1);
	add_assoc_bool(return_value, "GIF Create Support", 1);
#ifdef HAVE_GD_JPG
	add_assoc_bool(return_value, "JPEG Support", 1);
#else
	add_assoc_bool(return_value, "JPEG Support", 0);
#endif
#ifdef HAVE_GD_PNG
	add_assoc_bool(return_value, "PNG Support", 1);
#else
	add_assoc_bool(return_value, "PNG Support", 0);
#endif
	add_assoc_bool(return_value, "WBMP Support", 1);
#ifdef HAVE_GD_XPM
	add_assoc_bool(return_value, "XPM Support", 1);
#else
	add_assoc_bool(return_value, "XPM Support", 0);
#endif
	add_assoc_bool(return_value, "XBM Support", 1);
#ifdef HAVE_GD_WEBP
	add_assoc_bool(return_value, "WebP Support", 1);
#else
	add_assoc_bool(return_value, "WebP Support", 0);
#endif
#ifdef HAVE_GD_BMP
	add_assoc_bool(return_value, "BMP Support", 1);
#else
	add_assoc_bool(return_value, "BMP Support", 0);
#endif
#ifdef HAVE_GD_AVIF
	add_assoc_bool(return_value, "AVIF Support", 1);
#else
	add_assoc_bool(return_value, "AVIF Support", 0);
#endif
#ifdef HAVE_GD_TGA
	add_assoc_bool(return_value, "TGA Read Support", 1);
#else
	add_assoc_bool(return_value, "TGA Read Support", 0);
#endif
#ifdef USE_GD_JISX0208
	add_assoc_bool(return_value, "JIS-mapped Japanese Font Support", 1);
#else
	add_assoc_bool(return_value, "JIS-mapped Japanese Font Support", 0);
#endif
}
/* }}} */

#define FLIPWORD(a) (((a & 0xff000000) >> 24) | ((a & 0x00ff0000) >> 8) | ((a & 0x0000ff00) << 8) | ((a & 0x000000ff) << 24))

/* {{{ Load a new font */
PHP_FUNCTION(imageloadfont)
{
	zend_string *file;
	int hdr_size = sizeof(gdFont) - sizeof(char *);
	int body_size, n = 0, b, i, body_size_check;
	gdFontPtr font;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P", &file) == FAILURE) {
		RETURN_THROWS();
	}

	stream = php_stream_open_wrapper(ZSTR_VAL(file), "rb", IGNORE_PATH | REPORT_ERRORS, NULL);
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
	font = (gdFontPtr) emalloc(sizeof(gdFont));
	b = 0;
	while (b < hdr_size && (n = php_stream_read(stream, (char*)&font[b], hdr_size - b)) > 0) {
		b += n;
	}

	if (n <= 0) {
		efree(font);
		if (php_stream_eof(stream)) {
			php_error_docref(NULL, E_WARNING, "End of file while reading header");
		} else {
			php_error_docref(NULL, E_WARNING, "Error while reading header");
		}
		php_stream_close(stream);
		RETURN_FALSE;
	}
	i = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_END);
	body_size_check = php_stream_tell(stream) - hdr_size;
	php_stream_seek(stream, i, SEEK_SET);

	if (overflow2(font->nchars, font->h) || overflow2(font->nchars * font->h, font->w )) {
		php_error_docref(NULL, E_WARNING, "Error reading font, invalid font header");
		efree(font);
		php_stream_close(stream);
		RETURN_FALSE;
	}

	body_size = font->w * font->h * font->nchars;
	if (body_size != body_size_check) {
		font->w = FLIPWORD(font->w);
		font->h = FLIPWORD(font->h);
		font->nchars = FLIPWORD(font->nchars);
		if (overflow2(font->nchars, font->h) || overflow2(font->nchars * font->h, font->w )) {
			php_error_docref(NULL, E_WARNING, "Error reading font, invalid font header");
			efree(font);
			php_stream_close(stream);
			RETURN_FALSE;
		}
		body_size = font->w * font->h * font->nchars;
	}

	if (body_size != body_size_check) {
		php_error_docref(NULL, E_WARNING, "Error reading font");
		efree(font);
		php_stream_close(stream);
		RETURN_FALSE;
	}

	ZEND_ASSERT(body_size > 0);
	font->data = emalloc(body_size);
	b = 0;
	while (b < body_size && (n = php_stream_read(stream, &font->data[b], body_size - b)) > 0) {
		b += n;
	}

	if (n <= 0) {
		efree(font->data);
		efree(font);
		if (php_stream_eof(stream)) {
			php_error_docref(NULL, E_WARNING, "End of file while reading body");
		} else {
			php_error_docref(NULL, E_WARNING, "Error while reading body");
		}
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_close(stream);

	object_init_ex(return_value, gd_font_ce);
	php_gd_font_object_from_zend_object(Z_OBJ_P(return_value))->font = font;
}
/* }}} */

/* {{{ Set the line drawing styles for use with imageline and IMG_COLOR_STYLED. */
PHP_FUNCTION(imagesetstyle)
{
	zval *IM, *styles, *item;
	gdImagePtr im;
	int *stylearr;
	int index = 0;
	uint32_t num_styles;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oa", &IM, gd_image_ce, &styles) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	num_styles = zend_hash_num_elements(Z_ARRVAL_P(styles));
	if (num_styles == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	/* copy the style values in the stylearr */
	stylearr = safe_emalloc(sizeof(int), num_styles, 0);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(styles), item) {
		stylearr[index++] = zval_get_long(item);
	} ZEND_HASH_FOREACH_END();

	gdImageSetStyle(im, stylearr, index);

	efree(stylearr);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Create a new true color image */
PHP_FUNCTION(imagecreatetruecolor)
{
	zend_long x_size, y_size;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &x_size, &y_size) == FAILURE) {
		RETURN_THROWS();
	}

	if (x_size <= 0 || x_size >= INT_MAX) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	if (y_size <= 0 || y_size >= INT_MAX) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	im = gdImageCreateTrueColor(x_size, y_size);

	if (!im) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im);
}
/* }}} */

/* {{{ return true if the image uses truecolor */
PHP_FUNCTION(imageistruecolor)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	RETURN_BOOL(im->trueColor);
}
/* }}} */

/* {{{ Convert a true color image to a palette based image with a number of colors, optionally using dithering. */
PHP_FUNCTION(imagetruecolortopalette)
{
	zval *IM;
	bool dither;
	zend_long ncolors;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Obl", &IM, gd_image_ce, &dither, &ncolors) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (ncolors <= 0 || ZEND_LONG_INT_OVFL(ncolors)) {
		zend_argument_value_error(3, "must be greater than 0 and less than %d", INT_MAX);
		RETURN_THROWS();
	}

	if (gdImageTrueColorToPalette(im, dither, (int)ncolors)) {
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "Couldn't convert to palette");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Convert a palette based image to a true color image. */
PHP_FUNCTION(imagepalettetotruecolor)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (gdImagePaletteToTrueColor(im) == 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Makes the colors of the palette version of an image more closely match the true color version */
PHP_FUNCTION(imagecolormatch)
{
	zval *IM1, *IM2;
	gdImagePtr im1, im2;
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &IM1, gd_image_ce, &IM2, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im1 = php_gd_libgdimageptr_from_zval_p(IM1);
	im2 = php_gd_libgdimageptr_from_zval_p(IM2);

	result = gdImageColorMatch(im1, im2);
	switch (result) {
		case -1:
			zend_argument_value_error(1, "must be TrueColor");
			RETURN_THROWS();
			break;
		case -2:
			zend_argument_value_error(2, "must be Palette");
			RETURN_THROWS();
			break;
		case -3:
			zend_argument_value_error(2, "must be the same size as argument #1 ($im1)");
			RETURN_THROWS();
			break;
		case -4:
			zend_argument_value_error(2, "must have at least one color");
			RETURN_THROWS();
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Set line thickness for drawing lines, ellipses, rectangles, polygons etc. */
PHP_FUNCTION(imagesetthickness)
{
	zval *IM;
	zend_long thick;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &IM, gd_image_ce, &thick) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageSetThickness(im, thick);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw an ellipse */
PHP_FUNCTION(imagefilledellipse)
{
	zval *IM;
	zend_long cx, cy, w, h, color;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllll", &IM, gd_image_ce, &cx, &cy, &w, &h, &color) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageFilledEllipse(im, cx, cy, w, h, color);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a filled partial ellipse */
PHP_FUNCTION(imagefilledarc)
{
	zval *IM;
	zend_long cx, cy, w, h, ST, E, col, style;
	gdImagePtr im;
	int e, st;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollllllll", &IM, gd_image_ce, &cx, &cy, &w, &h, &ST, &E, &col, &style) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	e = E;
	if (e < 0) {
		e %= 360;
	}

	st = ST;
	if (st < 0) {
		st %= 360;
	}

	gdImageFilledArc(im, cx, cy, w, h, st, e, col, style);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Turn alpha blending mode on or off for the given image */
PHP_FUNCTION(imagealphablending)
{
	zval *IM;
	bool blend;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ob", &IM, gd_image_ce, &blend) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageAlphaBlending(im, blend);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Include alpha channel to a saved image */
PHP_FUNCTION(imagesavealpha)
{
	zval *IM;
	bool save;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ob", &IM, gd_image_ce, &save) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageSaveAlpha(im, save);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Set the alpha blending flag to use the bundled libgd layering effects */
PHP_FUNCTION(imagelayereffect)
{
	zval *IM;
	zend_long effect;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &IM, gd_image_ce, &effect) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageAlphaBlending(im, effect);

	RETURN_TRUE;
}
/* }}} */

#define CHECK_RGBA_RANGE(component, name, argument_number) \
	if (component < 0 || component > gd##name##Max) { \
		zend_argument_value_error(argument_number, "must be between 0 and %d (inclusive)", gd##name##Max); \
		RETURN_THROWS(); \
	}

/* {{{ Allocate a color with an alpha level.  Works for true color and palette based images */
PHP_FUNCTION(imagecolorallocatealpha)
{
	zval *IM;
	zend_long red, green, blue, alpha;
	gdImagePtr im;
	int ct = (-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll", &IM, gd_image_ce, &red, &green, &blue, &alpha) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);
	CHECK_RGBA_RANGE(alpha, Alpha, 5);

	ct = gdImageColorAllocateAlpha(im, red, green, blue, alpha);
	if (ct < 0) {
		RETURN_FALSE;
	}
	RETURN_LONG((zend_long)ct);
}
/* }}} */

/* {{{ Resolve/Allocate a colour with an alpha level.  Works for true colour and palette based images */
PHP_FUNCTION(imagecolorresolvealpha)
{
	zval *IM;
	zend_long red, green, blue, alpha;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll", &IM, gd_image_ce, &red, &green, &blue, &alpha) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);
	CHECK_RGBA_RANGE(alpha, Alpha, 5);

	RETURN_LONG(gdImageColorResolveAlpha(im, red, green, blue, alpha));
}
/* }}} */

/* {{{ Find the closest matching colour with alpha transparency */
PHP_FUNCTION(imagecolorclosestalpha)
{
	zval *IM;
	zend_long red, green, blue, alpha;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll", &IM, gd_image_ce, &red, &green, &blue, &alpha) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);
	CHECK_RGBA_RANGE(alpha, Alpha, 5);

	RETURN_LONG(gdImageColorClosestAlpha(im, red, green, blue, alpha));
}
/* }}} */

/* {{{ Find exact match for colour with transparency */
PHP_FUNCTION(imagecolorexactalpha)
{
	zval *IM;
	zend_long red, green, blue, alpha;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll", &IM, gd_image_ce, &red, &green, &blue, &alpha) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);
	CHECK_RGBA_RANGE(alpha, Alpha, 5);

	RETURN_LONG(gdImageColorExactAlpha(im, red, green, blue, alpha));
}
/* }}} */

/* {{{ Copy and resize part of an image using resampling to help ensure clarity */
PHP_FUNCTION(imagecopyresampled)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY, DW, DH;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOllllllll", &DIM, gd_image_ce, &SIM, gd_image_ce, &DX, &DY, &SX, &SY, &DW, &DH, &SW, &SH) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);
	im_dst = php_gd_libgdimageptr_from_zval_p(DIM);

	srcX = SX;
	srcY = SY;
	srcH = SH;
	srcW = SW;
	dstX = DX;
	dstY = DY;
	dstH = DH;
	dstW = DW;

	gdImageCopyResampled(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);

	RETURN_TRUE;
}
/* }}} */

#ifdef PHP_WIN32
/* {{{ Grab a window or its client area using a windows handle (HWND property in COM instance) */
PHP_FUNCTION(imagegrabwindow)
{
	HWND window;
	bool client_area = 0;
	RECT rc = {0};
	int Width, Height;
	HDC		hdc;
	HDC memDC;
	HBITMAP memBM;
	HBITMAP hOld;
	zend_long lwindow_handle;
	gdImagePtr im = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|b", &lwindow_handle, &client_area) == FAILURE) {
		RETURN_THROWS();
	}

	window = (HWND) lwindow_handle;

	if (!IsWindow(window)) {
		php_error_docref(NULL, E_NOTICE, "Invalid window handle");
		RETURN_FALSE;
	}

	hdc		= GetDC(0);

	if (client_area) {
		GetClientRect(window, &rc);
		Width = rc.right;
		Height = rc.bottom;
	} else {
		GetWindowRect(window, &rc);
		Width	= rc.right - rc.left;
		Height	= rc.bottom - rc.top;
	}

	Width		= (Width/4)*4;

	memDC	= CreateCompatibleDC(hdc);
	memBM	= CreateCompatibleBitmap(hdc, Width, Height);
	hOld	= (HBITMAP) SelectObject (memDC, memBM);

	PrintWindow(window, memDC, (UINT) client_area);

	im = gdImageCreateTrueColor(Width, Height);
	if (im) {
		int x,y;
		for (y=0; y <= Height; y++) {
			for (x=0; x <= Width; x++) {
				int c = GetPixel(memDC, x,y);
				gdImageSetPixel(im, x, y, gdTrueColor(GetRValue(c), GetGValue(c), GetBValue(c)));
			}
		}
	}

	SelectObject(memDC,hOld);
	DeleteObject(memBM);
	DeleteDC(memDC);
	ReleaseDC( 0, hdc );

	if (!im) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im);
}
/* }}} */

/* {{{ Grab a screenshot */
PHP_FUNCTION(imagegrabscreen)
{
	HWND window = GetDesktopWindow();
	RECT rc = {0};
	int Width, Height;
	HDC		hdc;
	HDC memDC;
	HBITMAP memBM;
	HBITMAP hOld;
	gdImagePtr im;
	hdc		= GetDC(0);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!hdc) {
		RETURN_FALSE;
	}

	GetWindowRect(window, &rc);
	Width	= rc.right - rc.left;
	Height	= rc.bottom - rc.top;

	Width		= (Width/4)*4;

	memDC	= CreateCompatibleDC(hdc);
	memBM	= CreateCompatibleBitmap(hdc, Width, Height);
	hOld	= (HBITMAP) SelectObject (memDC, memBM);
	BitBlt( memDC, 0, 0, Width, Height , hdc, rc.left, rc.top , SRCCOPY );

	im = gdImageCreateTrueColor(Width, Height);
	if (im) {
		int x,y;
		for (y=0; y <= Height; y++) {
			for (x=0; x <= Width; x++) {
				int c = GetPixel(memDC, x,y);
				gdImageSetPixel(im, x, y, gdTrueColor(GetRValue(c), GetGValue(c), GetBValue(c)));
			}
		}
	}

	SelectObject(memDC,hOld);
	DeleteObject(memBM);
	DeleteDC(memDC);
	ReleaseDC( 0, hdc );

	if (!im) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im);
}
/* }}} */
#endif /* PHP_WIN32 */

/* {{{ Rotate an image using a custom angle */
PHP_FUNCTION(imagerotate)
{
	zval *SIM;
	gdImagePtr im_dst, im_src;
	double degrees;
	zend_long color;
	bool ignoretransparent = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Odl|b", &SIM, gd_image_ce,  &degrees, &color, &ignoretransparent) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);
	im_dst = gdImageRotateInterpolated(im_src, (const float)degrees, color);

	if (im_dst == NULL) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im_dst);
}
/* }}} */

/* {{{ Set the tile image to $tile when filling $image with the "IMG_COLOR_TILED" color */
PHP_FUNCTION(imagesettile)
{
	zval *IM, *TILE;
	gdImagePtr im, tile;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &IM, gd_image_ce, &TILE, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);
	tile = php_gd_libgdimageptr_from_zval_p(TILE);

	gdImageSetTile(im, tile);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Set the brush image to $brush when filling $image with the "IMG_COLOR_BRUSHED" color */
PHP_FUNCTION(imagesetbrush)
{
	zval *IM, *TILE;
	gdImagePtr im, tile;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &IM, gd_image_ce, &TILE, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);
	tile = php_gd_libgdimageptr_from_zval_p(TILE);

	gdImageSetBrush(im, tile);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Create a new image */
PHP_FUNCTION(imagecreate)
{
	zend_long x_size, y_size;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &x_size, &y_size) == FAILURE) {
		RETURN_THROWS();
	}

	if (x_size <= 0 || x_size >= INT_MAX) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	if (y_size <= 0 || y_size >= INT_MAX) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	im = gdImageCreate(x_size, y_size);

	if (!im) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im);
}
/* }}} */

/* {{{ Return the types of images supported in a bitfield - 1=GIF, 2=JPEG, 4=PNG, 8=WBMP, 16=XPM, etc */
PHP_FUNCTION(imagetypes)
{
	int ret = 0;
	ret = PHP_IMG_GIF;
#ifdef HAVE_GD_JPG
	ret |= PHP_IMG_JPG;
#endif
#ifdef HAVE_GD_PNG
	ret |= PHP_IMG_PNG;
#endif
	ret |= PHP_IMG_WBMP;
#ifdef HAVE_GD_XPM
	ret |= PHP_IMG_XPM;
#endif
#ifdef HAVE_GD_WEBP
	ret |= PHP_IMG_WEBP;
#endif
#ifdef HAVE_GD_BMP
	ret |= PHP_IMG_BMP;
#endif
#ifdef HAVE_GD_TGA
	ret |= PHP_IMG_TGA;
#endif
#ifdef HAVE_GD_AVIF
	ret |= PHP_IMG_AVIF;
#endif

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ _php_ctx_getmbi */

static int _php_ctx_getmbi(gdIOCtx *ctx)
{
	int i, mbi = 0;

	do {
		i = (ctx->getC)(ctx);
		if (i < 0) {
			return -1;
		}
		mbi = (mbi << 7) | (i & 0x7f);
	} while (i & 0x80);

	return mbi;
}
/* }}} */

/* {{{ _php_image_type
 * Based on ext/standard/image.c
 */
static const char php_sig_gd2[3] = {'g', 'd', '2'};

static int _php_image_type(zend_string *data)
{
	if (ZSTR_LEN(data) < 12) {
		/* Handle this the same way as an unknown image type. */
		return -1;
	}

	if (!memcmp(ZSTR_VAL(data), php_sig_gd2, sizeof(php_sig_gd2))) {
		return PHP_GDIMG_TYPE_GD2;
	} else if (!memcmp(ZSTR_VAL(data), php_sig_jpg, sizeof(php_sig_jpg))) {
		return PHP_GDIMG_TYPE_JPG;
	} else if (!memcmp(ZSTR_VAL(data), php_sig_png, sizeof(php_sig_png))) {
		return PHP_GDIMG_TYPE_PNG;
	} else if (!memcmp(ZSTR_VAL(data), php_sig_gif, sizeof(php_sig_gif))) {
		return PHP_GDIMG_TYPE_GIF;
	} else if (!memcmp(ZSTR_VAL(data), php_sig_bmp, sizeof(php_sig_bmp))) {
		return PHP_GDIMG_TYPE_BMP;
	} else if(!memcmp(ZSTR_VAL(data), php_sig_riff, sizeof(php_sig_riff)) && !memcmp(ZSTR_VAL(data) + sizeof(php_sig_riff) + sizeof(uint32_t), php_sig_webp, sizeof(php_sig_webp))) {
		return PHP_GDIMG_TYPE_WEBP;
	}

	php_stream *image_stream = php_stream_memory_open(TEMP_STREAM_READONLY, data);

	if (image_stream != NULL) {
		bool is_avif = php_is_image_avif(image_stream);
		php_stream_close(image_stream);

		if (is_avif) {
			return PHP_GDIMG_TYPE_AVIF;
		}
	}

	gdIOCtx *io_ctx;
	io_ctx = gdNewDynamicCtxEx(8, ZSTR_VAL(data), 0);
	if (io_ctx) {
		if (_php_ctx_getmbi(io_ctx) == 0 && _php_ctx_getmbi(io_ctx) >= 0) {
			io_ctx->gd_free(io_ctx);
			return PHP_GDIMG_TYPE_WBM;
		} else {
			io_ctx->gd_free(io_ctx);
		}
	}

	return -1;
}
/* }}} */

/* {{{ _php_image_create_from_string */
gdImagePtr _php_image_create_from_string(zend_string *data, char *tn, gdImagePtr (*ioctx_func_p)(gdIOCtxPtr))
{
	gdImagePtr im;
	gdIOCtx *io_ctx;

	io_ctx = gdNewDynamicCtxEx(ZSTR_LEN(data), ZSTR_VAL(data), 0);

	if (!io_ctx) {
		return NULL;
	}

	im = (*ioctx_func_p)(io_ctx);
	if (!im) {
		php_error_docref(NULL, E_WARNING, "Passed data is not in \"%s\" format", tn);
		io_ctx->gd_free(io_ctx);
		return NULL;
	}

	io_ctx->gd_free(io_ctx);

	return im;
}
/* }}} */

/* {{{ Create a new image from the image stream in the string */
PHP_FUNCTION(imagecreatefromstring)
{
	zend_string *data;
	gdImagePtr im;
	int imtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &data) == FAILURE) {
		RETURN_THROWS();
	}

	imtype = _php_image_type(data);

	switch (imtype) {
		case PHP_GDIMG_TYPE_JPG:
#ifdef HAVE_GD_JPG
			im = _php_image_create_from_string(data, "JPEG", gdImageCreateFromJpegCtx);
#else
			php_error_docref(NULL, E_WARNING, "No JPEG support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_PNG:
#ifdef HAVE_GD_PNG
			im = _php_image_create_from_string(data, "PNG", gdImageCreateFromPngCtx);
#else
			php_error_docref(NULL, E_WARNING, "No PNG support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_GIF:
			im = _php_image_create_from_string(data, "GIF", gdImageCreateFromGifCtx);
			break;

		case PHP_GDIMG_TYPE_WBM:
			im = _php_image_create_from_string(data, "WBMP", gdImageCreateFromWBMPCtx);
			break;

		case PHP_GDIMG_TYPE_GD2:
			im = _php_image_create_from_string(data, "GD2", gdImageCreateFromGd2Ctx);
			break;

		case PHP_GDIMG_TYPE_BMP:
			im = _php_image_create_from_string(data, "BMP", gdImageCreateFromBmpCtx);
			break;

		case PHP_GDIMG_TYPE_WEBP:
#ifdef HAVE_GD_WEBP
			im = _php_image_create_from_string(data, "WEBP", gdImageCreateFromWebpCtx);
			break;
#else
			php_error_docref(NULL, E_WARNING, "No WEBP support in this PHP build");
			RETURN_FALSE;
#endif

		case PHP_GDIMG_TYPE_AVIF:
#ifdef HAVE_GD_AVIF
			im = _php_image_create_from_string(data, "AVIF", gdImageCreateFromAvifCtx);
			break;
#else
			php_error_docref(NULL, E_WARNING, "No AVIF support in this PHP build");
			RETURN_FALSE;
#endif

		default:
			php_error_docref(NULL, E_WARNING, "Data is not in a recognized format");
			RETURN_FALSE;
	}

	if (!im) {
		php_error_docref(NULL, E_WARNING, "Couldn't create GD Image Stream out of Data");
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im);
}
/* }}} */

/* {{{ _php_image_create_from */
static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(FILE *), gdImagePtr (*ioctx_func_p)(gdIOCtxPtr))
{
	char *file;
	size_t file_len;
	zend_long srcx, srcy, width, height;
	gdImagePtr im = NULL;
	php_stream *stream;
	FILE * fp = NULL;
#ifdef HAVE_GD_JPG
	long ignore_warning;
#endif

	if (image_type == PHP_GDIMG_TYPE_GD2PART) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "pllll", &file, &file_len, &srcx, &srcy, &width, &height) == FAILURE) {
			RETURN_THROWS();
		}

		if (width < 1) {
			zend_argument_value_error(4, "must be greater than or equal to 1");
			RETURN_THROWS();
		}

		if (height < 1) {
			zend_argument_value_error(5, "must be greater than or equal to 1");
			RETURN_THROWS();
		}

	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &file, &file_len) == FAILURE) {
			RETURN_THROWS();
		}
	}


	stream = php_stream_open_wrapper(file, "rb", REPORT_ERRORS|IGNORE_PATH, NULL);
	if (stream == NULL)	{
		RETURN_FALSE;
	}

	/* try and avoid allocating a FILE* if the stream is not naturally a FILE* */
	if (php_stream_is(stream, PHP_STREAM_IS_STDIO))	{
		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)&fp, REPORT_ERRORS)) {
			goto out_err;
		}
	} else if (ioctx_func_p || image_type == PHP_GDIMG_TYPE_GD2PART) {
		/* we can create an io context */
		gdIOCtx* io_ctx;
		zend_string *buff;
		char *pstr;

		buff = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);

		if (!buff) {
			php_error_docref(NULL, E_WARNING,"Cannot read image data");
			goto out_err;
		}

		/* needs to be malloc (persistent) - GD will free() it later */
		pstr = pestrndup(ZSTR_VAL(buff), ZSTR_LEN(buff), 1);
		io_ctx = gdNewDynamicCtxEx(ZSTR_LEN(buff), pstr, 0);
		if (!io_ctx) {
			pefree(pstr, 1);
			zend_string_release_ex(buff, 0);
			php_error_docref(NULL, E_WARNING,"Cannot allocate GD IO context");
			goto out_err;
		}

		if (image_type == PHP_GDIMG_TYPE_GD2PART) {
			im = gdImageCreateFromGd2PartCtx(io_ctx, srcx, srcy, width, height);
		} else {
			im = (*ioctx_func_p)(io_ctx);
		}
		io_ctx->gd_free(io_ctx);
		pefree(pstr, 1);
		zend_string_release_ex(buff, 0);
	}
	else if (php_stream_can_cast(stream, PHP_STREAM_AS_STDIO)) {
		/* try and force the stream to be FILE* */
		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO | PHP_STREAM_CAST_TRY_HARD, (void **) &fp, REPORT_ERRORS)) {
			goto out_err;
		}
	}

	if (!im && fp) {
		switch (image_type) {
			case PHP_GDIMG_TYPE_GD2PART:
				im = gdImageCreateFromGd2Part(fp, srcx, srcy, width, height);
				break;
#ifdef HAVE_GD_XPM
			case PHP_GDIMG_TYPE_XPM:
				im = gdImageCreateFromXpm(file);
				break;
#endif

#ifdef HAVE_GD_JPG
			case PHP_GDIMG_TYPE_JPG:
				ignore_warning = INI_INT("gd.jpeg_ignore_warning");
				im = gdImageCreateFromJpegEx(fp, ignore_warning);
			break;
#endif

			default:
				im = (*func_p)(fp);
				break;
		}

		fflush(fp);
	}

/* register_im: */
	if (im) {
		php_stream_close(stream);
		php_gd_assign_libgdimageptr_as_extgdimage(return_value, im);
		return;
	}

	php_error_docref(NULL, E_WARNING, "\"%s\" is not a valid %s file", file, tn);
out_err:
	php_stream_close(stream);
	RETURN_FALSE;

}
/* }}} */

/* {{{ Create a new image from GIF file or URL */
PHP_FUNCTION(imagecreatefromgif)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF", gdImageCreateFromGif, gdImageCreateFromGifCtx);
}
/* }}} */

#ifdef HAVE_GD_JPG
/* {{{ Create a new image from JPEG file or URL */
PHP_FUNCTION(imagecreatefromjpeg)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageCreateFromJpeg, gdImageCreateFromJpegCtx);
}
/* }}} */
#endif /* HAVE_GD_JPG */

#ifdef HAVE_GD_PNG
/* {{{ Create a new image from PNG file or URL */
PHP_FUNCTION(imagecreatefrompng)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImageCreateFromPng, gdImageCreateFromPngCtx);
}
/* }}} */
#endif /* HAVE_GD_PNG */

#ifdef HAVE_GD_WEBP
/* {{{ Create a new image from WEBP file or URL */
PHP_FUNCTION(imagecreatefromwebp)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WEBP, "WEBP", gdImageCreateFromWebp, gdImageCreateFromWebpCtx);
}
/* }}} */
#endif /* HAVE_GD_WEBP */

/* {{{ Create a new image from XBM file or URL */
PHP_FUNCTION(imagecreatefromxbm)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XBM, "XBM", gdImageCreateFromXbm, NULL);
}
/* }}} */

#ifdef HAVE_GD_AVIF
/* {{{ Create a new image from AVIF file or URL */
PHP_FUNCTION(imagecreatefromavif)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_AVIF, "AVIF", gdImageCreateFromAvif, gdImageCreateFromAvifCtx);
}
/* }}} */
#endif /* HAVE_GD_AVIF */

#ifdef HAVE_GD_XPM
/* {{{ Create a new image from XPM file or URL */
PHP_FUNCTION(imagecreatefromxpm)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XPM, "XPM", NULL, NULL);
}
/* }}} */
#endif

/* {{{ Create a new image from WBMP file or URL */
PHP_FUNCTION(imagecreatefromwbmp)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WBM, "WBMP", gdImageCreateFromWBMP, gdImageCreateFromWBMPCtx);
}
/* }}} */

/* {{{ Create a new image from GD file or URL */
PHP_FUNCTION(imagecreatefromgd)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD, "GD", gdImageCreateFromGd, gdImageCreateFromGdCtx);
}
/* }}} */

/* {{{ Create a new image from GD2 file or URL */
PHP_FUNCTION(imagecreatefromgd2)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2, "GD2", gdImageCreateFromGd2, gdImageCreateFromGd2Ctx);
}
/* }}} */

/* {{{ Create a new image from a given part of GD2 file or URL */
PHP_FUNCTION(imagecreatefromgd2part)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2PART, "GD2", NULL, NULL);
}
/* }}} */

#ifdef HAVE_GD_BMP
/* {{{ Create a new image from BMP file or URL */
PHP_FUNCTION(imagecreatefrombmp)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_BMP, "BMP", gdImageCreateFromBmp, gdImageCreateFromBmpCtx);
}
/* }}} */
#endif

#ifdef HAVE_GD_TGA
/* {{{ Create a new image from TGA file or URL */
PHP_FUNCTION(imagecreatefromtga)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_TGA, "TGA", gdImageCreateFromTga, gdImageCreateFromTgaCtx);
}
/* }}} */
#endif

/* {{{ _php_image_output */
static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn)
{
	zval *imgind;
	char *file = NULL;
	zend_long quality = 0, type = 0;
	gdImagePtr im;
	FILE *fp;
	size_t file_len = 0;
	int argc = ZEND_NUM_ARGS();
	int q = -1, t = 1;

	/* The quality parameter for gd2 stands for chunk size */

	switch (image_type) {
		case PHP_GDIMG_TYPE_GD:
			if (zend_parse_parameters(argc, "O|p!", &imgind, gd_image_ce, &file, &file_len) == FAILURE) {
				RETURN_THROWS();
			}
			break;
		case PHP_GDIMG_TYPE_GD2:
			if (zend_parse_parameters(argc, "O|p!ll", &imgind, gd_image_ce, &file, &file_len, &quality, &type) == FAILURE) {
				RETURN_THROWS();
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	im = php_gd_libgdimageptr_from_zval_p(imgind);

	if (argc >= 3) {
		q = quality;
		if (argc == 4) {
			t = type;
		}
	}

	if (file_len) {
		PHP_GD_CHECK_OPEN_BASEDIR(file, "Invalid filename");

		fp = VCWD_FOPEN(file, "wb");
		if (!fp) {
			php_error_docref(NULL, E_WARNING, "Unable to open \"%s\" for writing", file);
			RETURN_FALSE;
		}

		switch (image_type) {
			case PHP_GDIMG_TYPE_GD:
				gdImageGd(im, fp);
				break;
			case PHP_GDIMG_TYPE_GD2:
				if (q == -1) {
					q = 128;
				}
				gdImageGd2(im, fp, q, t);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
		fflush(fp);
		fclose(fp);
	} else {
		int   b;
		FILE *tmp;
		char  buf[4096];
		zend_string *path;

		tmp = php_open_temporary_file(NULL, NULL, &path);
		if (tmp == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to open temporary file");
			RETURN_FALSE;
		}

		switch (image_type) {
			case PHP_GDIMG_TYPE_GD:
				gdImageGd(im, tmp);
				break;
			case PHP_GDIMG_TYPE_GD2:
				if (q == -1) {
					q = 128;
				}
				gdImageGd2(im, tmp, q, t);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		fseek(tmp, 0, SEEK_SET);

		while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
			php_write(buf, b);
		}

		fclose(tmp);
		VCWD_UNLINK((const char *)ZSTR_VAL(path)); /* make sure that the temporary file is removed */
		zend_string_release_ex(path, 0);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Output XBM image to browser or file */
PHP_FUNCTION(imagexbm)
{
	zval *imgind;
	char *file = NULL;
	size_t file_len = 0;
	zend_long foreground_color;
	bool foreground_color_is_null = 1;
	gdImagePtr im;
	int i;
	gdIOCtx *ctx = NULL;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Op!|l!", &imgind, gd_image_ce, &file, &file_len, &foreground_color, &foreground_color_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(imgind);

	if (file != NULL) {
		stream = php_stream_open_wrapper(file, "wb", REPORT_ERRORS|IGNORE_PATH, NULL);
		if (stream == NULL) {
			RETURN_FALSE;
		}

		ctx = create_stream_context(stream, 1);
	} else {
		ctx = create_output_context();
	}

	if (foreground_color_is_null) {
		for (i=0; i < gdImageColorsTotal(im); i++) {
			if (!gdImageRed(im, i) && !gdImageGreen(im, i) && !gdImageBlue(im, i)) {
				break;
			}
		}

		foreground_color = i;
	}

	gdImageXbmCtx(im, file ? file : "", (int) foreground_color, ctx);

	ctx->gd_free(ctx);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Output GIF image to browser or file */
PHP_FUNCTION(imagegif)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF");
}
/* }}} */

#ifdef HAVE_GD_PNG
/* {{{ Output PNG image to browser or file */
PHP_FUNCTION(imagepng)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG");
}
/* }}} */
#endif /* HAVE_GD_PNG */

#ifdef HAVE_GD_WEBP
/* {{{ Output WEBP image to browser or file */
PHP_FUNCTION(imagewebp)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WEBP, "WEBP");
}
/* }}} */
#endif /* HAVE_GD_WEBP */

#ifdef HAVE_GD_AVIF
/* {{{ Output AVIF image to browser or file */
PHP_FUNCTION(imageavif)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_AVIF, "AVIF");
}
/* }}} */
#endif /* HAVE_GD_AVIF */

#ifdef HAVE_GD_JPG
/* {{{ Output JPEG image to browser or file */
PHP_FUNCTION(imagejpeg)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG");
}
/* }}} */
#endif /* HAVE_GD_JPG */

/* {{{ Output WBMP image to browser or file */
PHP_FUNCTION(imagewbmp)
{
	zval *imgind;
	zend_long foreground_color;
	zend_long foreground_color_is_null = 1;
	gdImagePtr im;
	int i;
	gdIOCtx *ctx = NULL;
	zval *to_zval = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!l!", &imgind, gd_image_ce, &to_zval, &foreground_color, &foreground_color_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(imgind);

	if (to_zval != NULL) {
		ctx = create_stream_context_from_zval(to_zval);
		if (!ctx) {
			RETURN_FALSE;
		}
	} else {
		ctx = create_output_context();
	}

	if (foreground_color_is_null) {
		for (i=0; i < gdImageColorsTotal(im); i++) {
			if (!gdImageRed(im, i) && !gdImageGreen(im, i) && !gdImageBlue(im, i)) {
				break;
			}
		}

		foreground_color = i;
	}

	gdImageWBMPCtx(im, foreground_color, ctx);

	ctx->gd_free(ctx);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Output GD image to browser or file */
PHP_FUNCTION(imagegd)
{
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD, "GD");
}
/* }}} */

/* {{{ Output GD2 image to browser or file */
PHP_FUNCTION(imagegd2)
{
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2, "GD2");
}
/* }}} */

#ifdef HAVE_GD_BMP
/* {{{ Output BMP image to browser or file */
PHP_FUNCTION(imagebmp)
{
	zval *imgind;
	bool compressed = 1;
	gdImagePtr im;
	gdIOCtx *ctx = NULL;
	zval *to_zval = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!b", &imgind, gd_image_ce, &to_zval, &compressed) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(imgind);

	if (to_zval != NULL) {
		ctx = create_stream_context_from_zval(to_zval);
		if (!ctx) {
			RETURN_FALSE;
		}
	} else {
		ctx = create_output_context();
	}

	gdImageBmpCtx(im, ctx, (int) compressed);

	ctx->gd_free(ctx);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Destroy an image - No effect as of PHP 8.0 */
PHP_FUNCTION(imagedestroy)
{
	/* This function used to free the resource, as resources are no longer used, it does nothing */
	zval *IM;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Allocate a color for an image */
PHP_FUNCTION(imagecolorallocate)
{
	zval *IM;
	zend_long red, green, blue;
	gdImagePtr im;
	int ct = (-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll", &IM, gd_image_ce, &red, &green, &blue) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);

	ct = gdImageColorAllocate(im, red, green, blue);
	if (ct < 0) {
		RETURN_FALSE;
	}
	RETURN_LONG(ct);
}
/* }}} */

/* {{{ Copy the palette from the src image onto the dst image */
PHP_FUNCTION(imagepalettecopy)
{
	zval *dstim, *srcim;
	gdImagePtr dst, src;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &dstim, gd_image_ce, &srcim, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	src = php_gd_libgdimageptr_from_zval_p(srcim);
	dst = php_gd_libgdimageptr_from_zval_p(dstim);

	gdImagePaletteCopy(dst, src);
}
/* }}} */

/* {{{ Get the index of the color of a pixel */
PHP_FUNCTION(imagecolorat)
{
	zval *IM;
	zend_long x, y;
	gdImagePtr im;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(IM, gd_image_ce)
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
	ZEND_PARSE_PARAMETERS_END();

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (gdImageTrueColor(im)) {
		if (im->tpixels && gdImageBoundsSafe(im, x, y)) {
			RETURN_LONG(gdImageTrueColorPixel(im, x, y));
		} else {
			php_error_docref(NULL, E_NOTICE, "" ZEND_LONG_FMT "," ZEND_LONG_FMT " is out of bounds", x, y);
			RETURN_FALSE;
		}
	} else {
		if (im->pixels && gdImageBoundsSafe(im, x, y)) {
			RETURN_LONG(im->pixels[y][x]);
		} else {
			php_error_docref(NULL, E_NOTICE, "" ZEND_LONG_FMT "," ZEND_LONG_FMT " is out of bounds", x, y);
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ Get the index of the closest color to the specified color */
PHP_FUNCTION(imagecolorclosest)
{
	zval *IM;
	zend_long red, green, blue;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll", &IM, gd_image_ce, &red, &green, &blue) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);

	RETURN_LONG(gdImageColorClosest(im, red, green, blue));
}
/* }}} */

/* {{{ Get the index of the color which has the hue, white and blackness nearest to the given color */
PHP_FUNCTION(imagecolorclosesthwb)
{
	zval *IM;
	zend_long red, green, blue;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll", &IM, gd_image_ce, &red, &green, &blue) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);

	RETURN_LONG(gdImageColorClosestHWB(im, red, green, blue));
}
/* }}} */

/* {{{ De-allocate a color for an image */
PHP_FUNCTION(imagecolordeallocate)
{
	zval *IM;
	zend_long index;
	int col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &IM, gd_image_ce, &index) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	/* We can return right away for a truecolor image as deallocating colours is meaningless here */
	if (gdImageTrueColor(im)) {
		RETURN_TRUE;
	}

	col = index;

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		gdImageColorDeallocate(im, col);
		RETURN_TRUE;
	} else {
		zend_argument_value_error(2, "must be between 0 and %d", gdImageColorsTotal(im));
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Get the index of the specified color or its closest possible alternative */
PHP_FUNCTION(imagecolorresolve)
{
	zval *IM;
	zend_long red, green, blue;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll", &IM, gd_image_ce, &red, &green, &blue) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);

	RETURN_LONG(gdImageColorResolve(im, red, green, blue));
}
/* }}} */

/* {{{ Get the index of the specified color */
PHP_FUNCTION(imagecolorexact)
{
	zval *IM;
	zend_long red, green, blue;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll", &IM, gd_image_ce, &red, &green, &blue) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);

	RETURN_LONG(gdImageColorExact(im, red, green, blue));
}
/* }}} */

/* {{{ Set the color for the specified palette index */
PHP_FUNCTION(imagecolorset)
{
	zval *IM;
	zend_long color, red, green, blue, alpha = 0;
	int col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll|l", &IM, gd_image_ce, &color, &red, &green, &blue, &alpha) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	CHECK_RGBA_RANGE(red, Red, 2);
	CHECK_RGBA_RANGE(green, Green, 3);
	CHECK_RGBA_RANGE(blue, Blue, 4);

	col = color;

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		im->red[col]   = red;
		im->green[col] = green;
		im->blue[col]  = blue;
		im->alpha[col]  = alpha;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Get the colors for an index */
PHP_FUNCTION(imagecolorsforindex)
{
	zval *IM;
	zend_long index;
	int col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &IM, gd_image_ce, &index) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	col = index;

	if ((col >= 0 && gdImageTrueColor(im)) || (!gdImageTrueColor(im) && col >= 0 && col < gdImageColorsTotal(im))) {
		array_init(return_value);

		add_assoc_long(return_value,"red",  gdImageRed(im,col));
		add_assoc_long(return_value,"green", gdImageGreen(im,col));
		add_assoc_long(return_value,"blue", gdImageBlue(im,col));
		add_assoc_long(return_value,"alpha", gdImageAlpha(im,col));
	} else {
		zend_argument_value_error(2, "is out of range");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Apply a gamma correction to a GD image */
PHP_FUNCTION(imagegammacorrect)
{
	zval *IM;
	gdImagePtr im;
	int i;
	double input, output, gamma;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Odd", &IM, gd_image_ce, &input, &output) == FAILURE) {
		RETURN_THROWS();
	}

	if (input <= 0.0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	if (output <= 0.0) {
		zend_argument_value_error(3, "must be greater than 0");
		RETURN_THROWS();
	}

	gamma = input / output;

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (gdImageTrueColor(im))	{
		int x, y, c;

		for (y = 0; y < gdImageSY(im); y++)	{
			for (x = 0; x < gdImageSX(im); x++)	{
				c = gdImageGetPixel(im, x, y);
				gdImageSetPixel(im, x, y,
					gdTrueColorAlpha(
						(int) ((pow((gdTrueColorGetRed(c)   / 255.0), gamma) * 255) + .5),
						(int) ((pow((gdTrueColorGetGreen(c) / 255.0), gamma) * 255) + .5),
						(int) ((pow((gdTrueColorGetBlue(c)  / 255.0), gamma) * 255) + .5),
						gdTrueColorGetAlpha(c)
					)
				);
			}
		}
		RETURN_TRUE;
	}

	for (i = 0; i < gdImageColorsTotal(im); i++) {
		im->red[i]   = (int)((pow((im->red[i]   / 255.0), gamma) * 255) + .5);
		im->green[i] = (int)((pow((im->green[i] / 255.0), gamma) * 255) + .5);
		im->blue[i]  = (int)((pow((im->blue[i]  / 255.0), gamma) * 255) + .5);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Set a single pixel */
PHP_FUNCTION(imagesetpixel)
{
	zval *IM;
	zend_long x, y, col;
	gdImagePtr im;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_OBJECT_OF_CLASS(IM, gd_image_ce)
		Z_PARAM_LONG(x)
		Z_PARAM_LONG(y)
		Z_PARAM_LONG(col)
	ZEND_PARSE_PARAMETERS_END();

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageSetPixel(im, x, y, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a line */
PHP_FUNCTION(imageline)
{
	zval *IM;
	zend_long x1, y1, x2, y2, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllll", &IM, gd_image_ce,  &x1, &y1, &x2, &y2, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (im->AA) {
		gdImageSetAntiAliased(im, col);
		col = gdAntiAliased;
	}
	gdImageLine(im, x1, y1, x2, y2, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a dashed line */
PHP_FUNCTION(imagedashedline)
{
	zval *IM;
	zend_long x1, y1, x2, y2, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllll", &IM, gd_image_ce, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageDashedLine(im, x1, y1, x2, y2, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a rectangle */
PHP_FUNCTION(imagerectangle)
{
	zval *IM;
	zend_long x1, y1, x2, y2, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllll", &IM, gd_image_ce, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageRectangle(im, x1, y1, x2, y2, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a filled rectangle */
PHP_FUNCTION(imagefilledrectangle)
{
	zval *IM;
	zend_long x1, y1, x2, y2, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllll", &IM, gd_image_ce, &x1, &y1, &x2, &y2, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);
	gdImageFilledRectangle(im, x1, y1, x2, y2, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a partial ellipse */
PHP_FUNCTION(imagearc)
{
	zval *IM;
	zend_long cx, cy, w, h, ST, E, col;
	gdImagePtr im;
	int e, st;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllllll", &IM, gd_image_ce, &cx, &cy, &w, &h, &ST, &E, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	e = E;
	if (e < 0) {
		e %= 360;
	}

	st = ST;
	if (st < 0) {
		st %= 360;
	}

	gdImageArc(im, cx, cy, w, h, st, e, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw an ellipse */
PHP_FUNCTION(imageellipse)
{
	zval *IM;
	zend_long cx, cy, w, h, color;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olllll", &IM, gd_image_ce,  &cx, &cy, &w, &h, &color) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageEllipse(im, cx, cy, w, h, color);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Flood fill to specific color */
PHP_FUNCTION(imagefilltoborder)
{
	zval *IM;
	zend_long x, y, border, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll", &IM, gd_image_ce,  &x, &y, &border, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageFillToBorder(im, x, y, border, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Flood fill */
PHP_FUNCTION(imagefill)
{
	zval *IM;
	zend_long x, y, col;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll", &IM, gd_image_ce, &x, &y, &col) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	gdImageFill(im, x, y, col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Find out the number of colors in an image's palette */
PHP_FUNCTION(imagecolorstotal)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	RETURN_LONG(gdImageColorsTotal(im));
}
/* }}} */

/* {{{ Define a color as transparent */
PHP_FUNCTION(imagecolortransparent)
{
	zval *IM;
	zend_long COL = 0;
	bool COL_IS_NULL = 1;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l!", &IM, gd_image_ce, &COL, &COL_IS_NULL) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (!COL_IS_NULL) {
		gdImageColorTransparent(im, COL);
	}

	RETURN_LONG(gdImageGetTransparent(im));
}
/* }}} */

/* {{{ Enable or disable interlace */
PHP_FUNCTION(imageinterlace)
{
	zval *IM;
	bool INT = 0;
	bool INT_IS_NULL = 1;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b!", &IM, gd_image_ce, &INT, &INT_IS_NULL) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (!INT_IS_NULL) {
		gdImageInterlace(im, INT);
	}

	RETURN_BOOL(gdImageGetInterlaced(im));
}
/* }}} */

/* {{{ php_imagepolygon
   arg = -1 open polygon
   arg = 0  normal polygon
   arg = 1  filled polygon */
/* im, points, num_points, col */
static void php_imagepolygon(INTERNAL_FUNCTION_PARAMETERS, int filled)
{
	zval *IM, *POINTS;
	zend_long NPOINTS, COL;
	bool COL_IS_NULL = 1;
	zval *var = NULL;
	gdImagePtr im;
	gdPointPtr points;
	int npoints, col, nelem, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oal|l!", &IM, gd_image_ce, &POINTS, &NPOINTS, &COL, &COL_IS_NULL) == FAILURE) {
		RETURN_THROWS();
	}
	if (COL_IS_NULL) {
		COL = NPOINTS;
		NPOINTS = zend_hash_num_elements(Z_ARRVAL_P(POINTS));
		if (NPOINTS % 2 != 0) {
			zend_argument_value_error(2, "must have an even number of elements");
			RETURN_THROWS();
		}
		NPOINTS /= 2;
	} else {
		php_error_docref(NULL, E_DEPRECATED, "Using the $num_points parameter is deprecated");
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	npoints = NPOINTS;
	col = COL;

	nelem = zend_hash_num_elements(Z_ARRVAL_P(POINTS));
	if (npoints < 3) {
		zend_argument_value_error(3, "must be greater than or equal to 3");
		RETURN_THROWS();
	}

	if (nelem < npoints * 2) {
		zend_value_error("Trying to use %d points in array with only %d points", npoints, nelem/2);
		RETURN_THROWS();
	}

	points = (gdPointPtr) safe_emalloc(npoints, sizeof(gdPoint), 0);

	for (i = 0; i < npoints; i++) {
		if ((var = zend_hash_index_find(Z_ARRVAL_P(POINTS), (i * 2))) != NULL) {
			points[i].x = zval_get_long(var);
		}
		if ((var = zend_hash_index_find(Z_ARRVAL_P(POINTS), (i * 2) + 1)) != NULL) {
			points[i].y = zval_get_long(var);
		}
	}

	if (im->AA) {
		gdImageSetAntiAliased(im, col);
		col = gdAntiAliased;
	}
	switch (filled) {
		case -1:
			gdImageOpenPolygon(im, points, npoints, col);
			break;
		case 0:
			gdImagePolygon(im, points, npoints, col);
			break;
		case 1:
			gdImageFilledPolygon(im, points, npoints, col);
			break;
	}

	efree(points);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Draw a polygon */
PHP_FUNCTION(imagepolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Draw a polygon */
PHP_FUNCTION(imageopenpolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, -1);
}
/* }}} */

/* {{{ Draw a filled polygon */
PHP_FUNCTION(imagefilledpolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ php_find_gd_font */
static gdFontPtr php_find_gd_font(zend_object *font_obj, zend_long font_int)
{
	if (font_obj) {
		return php_gd_font_object_from_zend_object(font_obj)->font;
	}

	switch (font_int) {
		case 1: return gdFontTiny;
		case 2: return gdFontSmall;
		case 3: return gdFontMediumBold;
		case 4: return gdFontLarge;
		case 5: return gdFontGiant;
	}

	return font_int < 1 ? gdFontTiny : gdFontGiant;
}
/* }}} */

/* {{{ php_imagefontsize
 * arg = 0  ImageFontWidth
 * arg = 1  ImageFontHeight
 */
static void php_imagefontsize(INTERNAL_FUNCTION_PARAMETERS, int arg)
{
	zend_object *font_obj = NULL;
	zend_long font_int = 0;
	gdFontPtr font;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_LONG(font_obj, gd_font_ce, font_int)
	ZEND_PARSE_PARAMETERS_END();

	font = php_find_gd_font(font_obj, font_int);
	RETURN_LONG(arg ? font->h : font->w);
}
/* }}} */

/* {{{ Get font width */
PHP_FUNCTION(imagefontwidth)
{
	php_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Get font height */
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
	zval *IM;
	zend_long X, Y, COL;
	char *C;
	size_t C_len;
	gdImagePtr im;
	int ch = 0, col, x, y, i, l = 0;
	unsigned char *str = NULL;
	zend_object *font_obj;
	zend_long font_int = 0;
	gdFontPtr font = NULL;

	ZEND_PARSE_PARAMETERS_START(6, 6)
		Z_PARAM_OBJECT_OF_CLASS(IM, gd_image_ce)
		Z_PARAM_OBJ_OF_CLASS_OR_LONG(font_obj, gd_font_ce, font_int)
		Z_PARAM_LONG(X)
		Z_PARAM_LONG(Y)
		Z_PARAM_STRING(C, C_len)
		Z_PARAM_LONG(COL)
	ZEND_PARSE_PARAMETERS_END();

	im = php_gd_libgdimageptr_from_zval_p(IM);

	col = COL;

	if (mode < 2) {
		ch = (int)((unsigned char)*C);
	} else {
		str = (unsigned char *) estrndup(C, C_len);
		l = strlen((char *)str);
	}

	y = Y;
	x = X;

	font = php_find_gd_font(font_obj, font_int);

	switch (mode) {
		case 0:
			gdImageChar(im, font, x, y, ch, col);
			break;
		case 1:
			php_gdimagecharup(im, font, x, y, ch, col);
			break;
		case 2:
			for (i = 0; (i < l); i++) {
				gdImageChar(im, font, x, y, (int) ((unsigned char) str[i]), col);
				x += font->w;
			}
			break;
		case 3: {
			for (i = 0; (i < l); i++) {
				/* php_gdimagecharup(im, font, x, y, (int) str[i], col); */
				gdImageCharUp(im, font, x, y, (int) str[i], col);
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

/* {{{ Draw a character */
PHP_FUNCTION(imagechar)
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Draw a character rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagecharup)
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Draw a string horizontally */
PHP_FUNCTION(imagestring)
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ Draw a string vertically - rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagestringup)
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ Copy part of an image */
PHP_FUNCTION(imagecopy)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOllllll", &DIM, gd_image_ce, &SIM, gd_image_ce, &DX, &DY, &SX, &SY, &SW, &SH) == FAILURE) {
		RETURN_THROWS();
	}

	im_dst = php_gd_libgdimageptr_from_zval_p(DIM);
	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

	srcX = SX;
	srcY = SY;
	srcH = SH;
	srcW = SW;
	dstX = DX;
	dstY = DY;

	gdImageCopy(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Merge one part of an image with another */
PHP_FUNCTION(imagecopymerge)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY, PCT;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX, pct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOlllllll", &DIM, gd_image_ce, &SIM, gd_image_ce, &DX, &DY, &SX, &SY, &SW, &SH, &PCT) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);
	im_dst = php_gd_libgdimageptr_from_zval_p(DIM);

	srcX = SX;
	srcY = SY;
	srcH = SH;
	srcW = SW;
	dstX = DX;
	dstY = DY;
	pct  = PCT;

	gdImageCopyMerge(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH, pct);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Merge one part of an image with another */
PHP_FUNCTION(imagecopymergegray)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY, PCT;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX, pct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOlllllll", &DIM, gd_image_ce, &SIM, gd_image_ce, &DX, &DY, &SX, &SY, &SW, &SH, &PCT) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);
	im_dst = php_gd_libgdimageptr_from_zval_p(DIM);

	srcX = SX;
	srcY = SY;
	srcH = SH;
	srcW = SW;
	dstX = DX;
	dstY = DY;
	pct  = PCT;

	gdImageCopyMergeGray(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH, pct);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Copy and resize part of an image */
PHP_FUNCTION(imagecopyresized)
{
	zval *SIM, *DIM;
	zend_long SX, SY, SW, SH, DX, DY, DW, DH;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOllllllll", &DIM, gd_image_ce, &SIM, gd_image_ce, &DX, &DY, &SX, &SY, &DW, &DH, &SW, &SH) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);
	im_dst = php_gd_libgdimageptr_from_zval_p(DIM);

	srcX = SX;
	srcY = SY;
	srcH = SH;
	srcW = SW;
	dstX = DX;
	dstY = DY;
	dstH = DH;
	dstW = DW;

	if (dstW <= 0) {
		zend_argument_value_error(7, "must be greater than 0");
		RETURN_THROWS();
	}

	if (dstH <= 0) {
		zend_argument_value_error(8, "must be greater than 0");
		RETURN_THROWS();
	}

	if (srcW <= 0) {
		zend_argument_value_error(9, "must be greater than 0");
		RETURN_THROWS();
	}

	if (srcH <= 0) {
		zend_argument_value_error(10, "must be greater than 0");
		RETURN_THROWS();
	}

	gdImageCopyResized(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get image width */
PHP_FUNCTION(imagesx)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	RETURN_LONG(gdImageSX(im));
}
/* }}} */

/* {{{ Get image height */
PHP_FUNCTION(imagesy)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	RETURN_LONG(gdImageSY(im));
}
/* }}} */

/* {{{ Set the clipping rectangle. */
PHP_FUNCTION(imagesetclip)
{
	zval *im_zval;
	gdImagePtr im;
	zend_long x1, y1, x2, y2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll", &im_zval, gd_image_ce, &x1, &y1, &x2, &y2) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(im_zval);

	gdImageSetClip(im, x1, y1, x2, y2);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get the clipping rectangle. */
PHP_FUNCTION(imagegetclip)
{
	zval *im_zval;
	gdImagePtr im;
	int x1, y1, x2, y2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &im_zval, gd_image_ce) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(im_zval);

	gdImageGetClip(im, &x1, &y1, &x2, &y2);

	array_init(return_value);
	add_next_index_long(return_value, x1);
	add_next_index_long(return_value, y1);
	add_next_index_long(return_value, x2);
	add_next_index_long(return_value, y2);
}
/* }}} */

#define TTFTEXT_DRAW 0
#define TTFTEXT_BBOX 1

#ifdef HAVE_GD_FREETYPE
/* {{{ Give the bounding box of a text using fonts via freetype2 */
PHP_FUNCTION(imageftbbox)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX);
}
/* }}} */

/* {{{ Write text to the image using fonts via freetype2 */
PHP_FUNCTION(imagefttext)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW);
}
/* }}} */

/* {{{ php_imagettftext_common */
static void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *IM, *EXT = NULL;
	gdImagePtr im=NULL;
	zend_long col = -1, x = 0, y = 0;
	size_t str_len, fontname_len;
	int i, brect[8];
	double ptsize, angle;
	char *str = NULL, *fontname = NULL;
	char *error = NULL;
	gdFTStringExtra strex = {0};

	if (mode == TTFTEXT_BBOX) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ddss|a", &ptsize, &angle, &fontname, &fontname_len, &str, &str_len, &EXT) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oddlllss|a", &IM, gd_image_ce, &ptsize, &angle, &x, &y, &col, &fontname, &fontname_len, &str, &str_len, &EXT) == FAILURE) {
			RETURN_THROWS();
		}
		im = php_gd_libgdimageptr_from_zval_p(IM);
	}

	/* convert angle to radians */
	angle = angle * (M_PI/180);

	if (EXT) {	/* parse extended info */
		zval *item;
		zend_string *key;

		/* walk the assoc array */
		if (!HT_IS_PACKED(Z_ARRVAL_P(EXT))) {
			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(EXT), key, item) {
				if (key == NULL) {
					continue;
				}
				if (zend_string_equals_literal(key, "linespacing")) {
					strex.flags |= gdFTEX_LINESPACE;
					strex.linespacing = zval_get_double(item);
				}
			} ZEND_HASH_FOREACH_END();
		}
	}

#ifdef VIRTUAL_DIR
	{
		char tmp_font_path[MAXPATHLEN];

		if (!VCWD_REALPATH(fontname, tmp_font_path)) {
			fontname = NULL;
		}
	}
#endif /* VIRTUAL_DIR */

	PHP_GD_CHECK_OPEN_BASEDIR(fontname, "Invalid font filename");

	if (EXT) {
		error = gdImageStringFTEx(im, brect, col, fontname, ptsize, angle, x, y, str, &strex);
	} else {
		error = gdImageStringFT(im, brect, col, fontname, ptsize, angle, x, y, str);
	}

	if (error) {
		php_error_docref(NULL, E_WARNING, "%s", error);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* return array with the text's bounding box */
	for (i = 0; i < 8; i++) {
		add_next_index_long(return_value, brect[i]);
	}
}
/* }}} */
#endif /* HAVE_GD_FREETYPE */

/* Section Filters */
#define PHP_GD_SINGLE_RES	\
	zval *SIM;	\
	gdImagePtr im_src;	\
	if (zend_parse_parameters(1, "O", &SIM, gd_image_ce) == FAILURE) {	\
		RETURN_THROWS();	\
	}	\
	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

static void php_image_filter_negate(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageNegate(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_grayscale(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageGrayScale(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_brightness(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	gdImagePtr im_src;
	zend_long brightness, tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oll", &SIM, gd_image_ce, &tmp, &brightness) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

	if (gdImageBrightness(im_src, (int)brightness) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_contrast(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	gdImagePtr im_src;
	zend_long contrast, tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oll", &SIM, gd_image_ce, &tmp, &contrast) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

	if (gdImageContrast(im_src, (int)contrast) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_colorize(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	gdImagePtr im_src;
	zend_long r,g,b,tmp;
	zend_long a = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollll|l", &SIM, gd_image_ce, &tmp, &r, &g, &b, &a) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

	if (gdImageColor(im_src, (int) r, (int) g, (int) b, (int) a) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_edgedetect(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageEdgeDetectQuick(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_emboss(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageEmboss(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_gaussian_blur(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageGaussianBlur(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_selective_blur(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageSelectiveBlur(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_mean_removal(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageMeanRemoval(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_smooth(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *SIM;
	zend_long tmp;
	gdImagePtr im_src;
	double weight;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Old", &SIM, gd_image_ce, &tmp, &weight) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

	if (gdImageSmooth(im_src, (float)weight)==1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_pixelate(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *IM;
	gdImagePtr im;
	zend_long tmp, blocksize;
	bool mode = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oll|b", &IM, gd_image_ce, &tmp, &blocksize, &mode) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (gdImagePixelate(im, (int) blocksize, (const unsigned int) mode)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

static void php_image_filter_scatter(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *IM;
	zval *hash_colors = NULL;
	gdImagePtr im;
	zend_long tmp;
	zend_long scatter_sub, scatter_plus;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Olll|a", &IM, gd_image_ce, &tmp, &scatter_sub, &scatter_plus, &hash_colors) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (hash_colors) {
		uint32_t i = 0;
		uint32_t num_colors = zend_hash_num_elements(Z_ARRVAL_P(hash_colors));
		zval *color;
		int *colors;

		if (num_colors == 0) {
			RETURN_BOOL(gdImageScatter(im, (int)scatter_sub, (int)scatter_plus));
		}

		colors = emalloc(num_colors * sizeof(int));

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(hash_colors), color) {
			*(colors + i++) = (int) zval_get_long(color);
		} ZEND_HASH_FOREACH_END();

		RETVAL_BOOL(gdImageScatterColor(im, (int)scatter_sub, (int)scatter_plus, colors, num_colors));

		efree(colors);
	} else {
		RETURN_BOOL(gdImageScatter(im, (int) scatter_sub, (int) scatter_plus));
	}
}

/* {{{ Applies Filter an image using a custom angle */
PHP_FUNCTION(imagefilter)
{
	zval *tmp;

	typedef void (*image_filter)(INTERNAL_FUNCTION_PARAMETERS);
	zend_long filtertype;
	image_filter filters[] =
	{
		php_image_filter_negate ,
		php_image_filter_grayscale,
		php_image_filter_brightness,
		php_image_filter_contrast,
		php_image_filter_colorize,
		php_image_filter_edgedetect,
		php_image_filter_emboss,
		php_image_filter_gaussian_blur,
		php_image_filter_selective_blur,
		php_image_filter_mean_removal,
		php_image_filter_smooth,
		php_image_filter_pixelate,
		php_image_filter_scatter
	};

	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > IMAGE_FILTER_MAX_ARGS) {
		WRONG_PARAM_COUNT;
	} else if (zend_parse_parameters(2, "Ol", &tmp, gd_image_ce, &filtertype) == FAILURE) {
		RETURN_THROWS();
	}

	if (filtertype >= 0 && filtertype <= IMAGE_FILTER_MAX) {
		filters[filtertype](INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}
/* }}} */

/* {{{ Apply a 3x3 convolution matrix, using coefficient div and offset */
PHP_FUNCTION(imageconvolution)
{
	zval *SIM, *hash_matrix;
	zval *var = NULL, *var2 = NULL;
	gdImagePtr im_src = NULL;
	double div, offset;
	int nelem, i, j, res;
	float matrix[3][3] = {{0,0,0}, {0,0,0}, {0,0,0}};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oadd", &SIM, gd_image_ce, &hash_matrix, &div, &offset) == FAILURE) {
		RETURN_THROWS();
	}

	im_src = php_gd_libgdimageptr_from_zval_p(SIM);

	nelem = zend_hash_num_elements(Z_ARRVAL_P(hash_matrix));
	if (nelem != 3) {
		zend_argument_value_error(2, "must be a 3x3 array");
		RETURN_THROWS();
	}

	for (i=0; i<3; i++) {
		if ((var = zend_hash_index_find(Z_ARRVAL_P(hash_matrix), (i))) != NULL && Z_TYPE_P(var) == IS_ARRAY) {
			if (zend_hash_num_elements(Z_ARRVAL_P(var)) != 3 ) {
				zend_argument_value_error(2, "must be a 3x3 array, matrix[%d] only has %d elements", i, zend_hash_num_elements(Z_ARRVAL_P(var)));
				RETURN_THROWS();
			}

			for (j=0; j<3; j++) {
				if ((var2 = zend_hash_index_find(Z_ARRVAL_P(var), j)) != NULL) {
					matrix[i][j] = (float) zval_get_double(var2);
				} else {
					zend_argument_value_error(2, "must be a 3x3 array, matrix[%d][%d] cannot be found (missing integer key)", i, j);
					RETURN_THROWS();
				}
			}
		}
	}
	res = gdImageConvolution(im_src, matrix, (float)div, (float)offset);

	if (res) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
/* End section: Filters */

/* {{{ Flip an image (in place) horizontally, vertically or both directions. */
PHP_FUNCTION(imageflip)
{
	zval *IM;
	zend_long mode;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &IM, gd_image_ce, &mode) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	switch (mode) {
		case PHP_GD_FLIP_VERTICAL:
			gdImageFlipVertical(im);
			break;

		case PHP_GD_FLIP_HORIZONTAL:
			gdImageFlipHorizontal(im);
			break;

		case PHP_GD_FLIP_BOTH:
			gdImageFlipBoth(im);
			break;

		default:
			zend_argument_value_error(2, "must be one of IMG_FLIP_VERTICAL, IMG_FLIP_HORIZONTAL, or IMG_FLIP_BOTH");
			RETURN_THROWS();
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Should antialiased functions used or not*/
PHP_FUNCTION(imageantialias)
{
	zval *IM;
	bool alias;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ob", &IM, gd_image_ce, &alias) == FAILURE) {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);
	if (im->trueColor) {
		im->AA = alias;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Crop an image using the given coordinates and size, x, y, width and height. */
PHP_FUNCTION(imagecrop)
{
	zval *IM;
	gdImagePtr im;
	gdImagePtr im_crop;
	gdRect rect;
	zval *z_rect;
	zval *tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oa", &IM, gd_image_ce, &z_rect) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "x", sizeof("x") -1)) != NULL) {
		rect.x = zval_get_long(tmp);
	} else {
		zend_argument_value_error(2, "must have an \"x\" key");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "y", sizeof("y") - 1)) != NULL) {
		rect.y = zval_get_long(tmp);
	} else {
		zend_argument_value_error(2, "must have a \"y\" key");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "width", sizeof("width") - 1)) != NULL) {
		rect.width = zval_get_long(tmp);
	} else {
		zend_argument_value_error(2, "must have a \"width\" key");
		RETURN_THROWS();
	}

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "height", sizeof("height") - 1)) != NULL) {
		rect.height = zval_get_long(tmp);
	} else {
		zend_argument_value_error(2, "must have a \"height\" key");
		RETURN_THROWS();
	}

	im_crop = gdImageCrop(im, &rect);

	if (im_crop == NULL) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im_crop);
}
/* }}} */

/* {{{ Crop an image automatically using one of the available modes. */
PHP_FUNCTION(imagecropauto)
{
	zval *IM;
	zend_long mode = GD_CROP_DEFAULT;
	zend_long color = -1;
	double threshold = 0.5f;
	gdImagePtr im;
	gdImagePtr im_crop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|ldl", &IM, gd_image_ce, &mode, &threshold, &color) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	switch (mode) {
		case GD_CROP_DEFAULT:
		case GD_CROP_TRANSPARENT:
		case GD_CROP_BLACK:
		case GD_CROP_WHITE:
		case GD_CROP_SIDES:
			im_crop = gdImageCropAuto(im, mode);
			break;

		case GD_CROP_THRESHOLD:
			if (color < 0 || (!gdImageTrueColor(im) && color >= gdImageColorsTotal(im))) {
				zend_argument_value_error(4, "must be greater than or equal to 0 when using the threshold mode");
				RETURN_THROWS();
			}
			im_crop = gdImageCropThreshold(im, color, (float) threshold);
			break;

		default:
			zend_argument_value_error(2, "must be a valid mode");
			RETURN_THROWS();
	}

	if (im_crop == NULL) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im_crop);
}
/* }}} */

/* {{{ Scale an image using the given new width and height. */
PHP_FUNCTION(imagescale)
{
	zval *IM;
	gdImagePtr im;
	gdImagePtr im_scaled = NULL;
	int new_width, new_height;
	zend_long tmp_w, tmp_h=-1, tmp_m = GD_BILINEAR_FIXED;
	gdInterpolationMethod method, old_method;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|ll", &IM, gd_image_ce, &tmp_w, &tmp_h, &tmp_m) == FAILURE)  {
		RETURN_THROWS();
	}
	method = tmp_m;

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (tmp_h < 0 || tmp_w < 0) {
		/* preserve ratio */
		long src_x, src_y;

		src_x = gdImageSX(im);
		src_y = gdImageSY(im);

		if (src_x && tmp_h < 0) {
			tmp_h = tmp_w * src_y / src_x;
		}
		if (src_y && tmp_w < 0) {
			tmp_w = tmp_h * src_x / src_y;
		}
	}

	if (tmp_h <= 0 || tmp_h > INT_MAX || tmp_w <= 0 || tmp_w > INT_MAX) {
		RETURN_FALSE;
	}

	new_width = tmp_w;
	new_height = tmp_h;

	/* gdImageGetInterpolationMethod() is only available as of GD 2.1.1 */
	old_method = im->interpolation_id;
	if (gdImageSetInterpolationMethod(im, method)) {
		im_scaled = gdImageScale(im, new_width, new_height);
	}
	gdImageSetInterpolationMethod(im, old_method);

	if (im_scaled == NULL) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, im_scaled);
}
/* }}} */

/* {{{ Return an image containing the affine tramsformed src image, using an optional clipping area */
PHP_FUNCTION(imageaffine)
{
	zval *IM;
	gdImagePtr src;
	gdImagePtr dst;
	gdRect rect;
	gdRectPtr pRect = NULL;
	zval *z_rect = NULL;
	zval *z_affine;
	zval *tmp;
	double affine[6];
	int i, nelems;
	zval *zval_affine_elem = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oa|a!", &IM, gd_image_ce, &z_affine, &z_rect) == FAILURE)  {
		RETURN_THROWS();
	}

	src = php_gd_libgdimageptr_from_zval_p(IM);

	if ((nelems = zend_hash_num_elements(Z_ARRVAL_P(z_affine))) != 6) {
		zend_argument_value_error(2, "must have 6 elements");
		RETURN_THROWS();
	}

	for (i = 0; i < nelems; i++) {
		if ((zval_affine_elem = zend_hash_index_find(Z_ARRVAL_P(z_affine), i)) != NULL) {
			switch (Z_TYPE_P(zval_affine_elem)) {
				case IS_LONG:
					affine[i]  = Z_LVAL_P(zval_affine_elem);
					break;
				case IS_DOUBLE:
					affine[i] = Z_DVAL_P(zval_affine_elem);
					break;
				case IS_STRING:
					affine[i] = zval_get_double(zval_affine_elem);
					break;
				default:
					zend_argument_type_error(3, "contains invalid type for element %i", i);
					RETURN_THROWS();
			}
		}
	}

	if (z_rect != NULL) {
		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "x", sizeof("x") - 1)) != NULL) {
			rect.x = zval_get_long(tmp);
		} else {
			zend_argument_value_error(3, "must have an \"x\" key");
			RETURN_THROWS();
		}

		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "y", sizeof("y") - 1)) != NULL) {
			rect.y = zval_get_long(tmp);
		} else {
			zend_argument_value_error(3, "must have a \"y\" key");
			RETURN_THROWS();
		}

		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "width", sizeof("width") - 1)) != NULL) {
			rect.width = zval_get_long(tmp);
		} else {
			zend_argument_value_error(3, "must have a \"width\" key");
			RETURN_THROWS();
		}

		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "height", sizeof("height") - 1)) != NULL) {
			rect.height = zval_get_long(tmp);
		} else {
			zend_argument_value_error(3, "must have a \"height\" key");
			RETURN_THROWS();
		}
		pRect = &rect;
	}

	if (gdTransformAffineGetImage(&dst, src, pRect, affine) != GD_TRUE) {
		RETURN_FALSE;
	}

	if (dst == NULL) {
		RETURN_FALSE;
	}

	php_gd_assign_libgdimageptr_as_extgdimage(return_value, dst);
}
/* }}} */

/* {{{ Return an image containing the affine tramsformed src image, using an optional clipping area */
PHP_FUNCTION(imageaffinematrixget)
{
	double affine[6];
	zend_long type;
	zval *options = NULL;
	zval *tmp;
	int res = GD_FALSE, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &type, &options) == FAILURE)  {
		RETURN_THROWS();
	}

	switch((gdAffineStandardMatrix)type) {
		case GD_AFFINE_TRANSLATE:
		case GD_AFFINE_SCALE: {
			double x, y;
			if (Z_TYPE_P(options) != IS_ARRAY) {
				zend_argument_type_error(1, "must be of type array when using translate or scale");
				RETURN_THROWS();
			}

			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(options), "x", sizeof("x") - 1)) != NULL) {
				x = zval_get_double(tmp);
			} else {
				zend_argument_value_error(2, "must have an \"x\" key");
				RETURN_THROWS();
			}

			if ((tmp = zend_hash_str_find(Z_ARRVAL_P(options), "y", sizeof("y") - 1)) != NULL) {
				y = zval_get_double(tmp);
			} else {
				zend_argument_value_error(2, "must have a \"y\" key");
				RETURN_THROWS();
			}

			if (type == GD_AFFINE_TRANSLATE) {
				res = gdAffineTranslate(affine, x, y);
			} else {
				res = gdAffineScale(affine, x, y);
			}
			break;
		}

		case GD_AFFINE_ROTATE:
		case GD_AFFINE_SHEAR_HORIZONTAL:
		case GD_AFFINE_SHEAR_VERTICAL: {
			double angle;

			angle = zval_get_double(options);

			if (type == GD_AFFINE_SHEAR_HORIZONTAL) {
				res = gdAffineShearHorizontal(affine, angle);
			} else if (type == GD_AFFINE_SHEAR_VERTICAL) {
				res = gdAffineShearVertical(affine, angle);
			} else {
				res = gdAffineRotate(affine, angle);
			}
			break;
		}

		default:
			zend_argument_value_error(1, "must be a valid element type");
			RETURN_THROWS();
	}

	if (res == GD_FALSE) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		for (i = 0; i < 6; i++) {
			add_index_double(return_value, i, affine[i]);
		}
	}
} /* }}} */

/* {{{ Concat two matrices (as in doing many ops in one go) */
PHP_FUNCTION(imageaffinematrixconcat)
{
	double m1[6];
	double m2[6];
	double mr[6];

	zval *tmp;
	zval *z_m1;
	zval *z_m2;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "aa", &z_m1, &z_m2) == FAILURE)  {
		RETURN_THROWS();
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(z_m1)) != 6) {
		zend_argument_value_error(1, "must have 6 elements");
		RETURN_THROWS();
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(z_m2)) != 6) {
		zend_argument_value_error(1, "must have 6 elements");
		RETURN_THROWS();
	}

	for (i = 0; i < 6; i++) {
		if ((tmp = zend_hash_index_find(Z_ARRVAL_P(z_m1), i)) != NULL) {
			switch (Z_TYPE_P(tmp)) {
				case IS_LONG:
					m1[i]  = Z_LVAL_P(tmp);
					break;
				case IS_DOUBLE:
					m1[i] = Z_DVAL_P(tmp);
					break;
				case IS_STRING:
					m1[i] = zval_get_double(tmp);
					break;
				default:
					zend_argument_type_error(1, "contains invalid type for element %i", i);
					RETURN_THROWS();
			}
		}

		if ((tmp = zend_hash_index_find(Z_ARRVAL_P(z_m2), i)) != NULL) {
			switch (Z_TYPE_P(tmp)) {
				case IS_LONG:
					m2[i]  = Z_LVAL_P(tmp);
					break;
				case IS_DOUBLE:
					m2[i] = Z_DVAL_P(tmp);
					break;
				case IS_STRING:
					m2[i] = zval_get_double(tmp);
					break;
				default:
					zend_argument_type_error(2, "contains invalid type for element %i", i);
					RETURN_THROWS();
			}
		}
	}

	if (gdAffineConcat (mr, m1, m2) != GD_TRUE) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i = 0; i < 6; i++) {
		add_index_double(return_value, i, mr[i]);
	}
} /* }}} */

/* {{{ Get the default interpolation method. */
PHP_FUNCTION(imagegetinterpolation)
{
	zval *IM;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &IM, gd_image_ce) == FAILURE)  {
		RETURN_THROWS();
	}
	im = php_gd_libgdimageptr_from_zval_p(IM);

#ifdef HAVE_GD_GET_INTERPOLATION
	RETURN_LONG(gdImageGetInterpolationMethod(im));
#else
	RETURN_LONG(im->interpolation_id);
#endif
}
/* }}} */

/* {{{ Set the default interpolation method, passing -1 or 0 sets it to the libgd default (bilinear). */
PHP_FUNCTION(imagesetinterpolation)
{
	zval *IM;
	gdImagePtr im;
	zend_long method = GD_BILINEAR_FIXED;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &IM, gd_image_ce, &method) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (method == -1) {
		 method = GD_BILINEAR_FIXED;
	}
	RETURN_BOOL(gdImageSetInterpolationMethod(im, (gdInterpolationMethod) method));
}
/* }}} */

/* {{{ Get or set the resolution of the image in DPI. */
PHP_FUNCTION(imageresolution)
{
	zval *IM;
	gdImagePtr im;
	zend_long res_x, res_y;
	bool res_x_is_null = 1, res_y_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l!l!", &IM, gd_image_ce, &res_x, &res_x_is_null, &res_y, &res_y_is_null) == FAILURE)  {
		RETURN_THROWS();
	}

	im = php_gd_libgdimageptr_from_zval_p(IM);

	if (!res_x_is_null && !res_y_is_null) {
		gdImageSetResolution(im, res_x, res_y);
		RETURN_TRUE;
	} else if (!res_x_is_null && res_y_is_null) {
		gdImageSetResolution(im, res_x, res_x);
		RETURN_TRUE;
	} else if (res_x_is_null && !res_y_is_null) {
		gdImageSetResolution(im, res_y, res_y);
		RETURN_TRUE;
	}

	array_init(return_value);
	add_next_index_long(return_value, gdImageResolutionX(im));
	add_next_index_long(return_value, gdImageResolutionY(im));
}
/* }}} */


/*********************************************************
 *
 * Stream Handling
 * Formerly contained within ext/gd/gd_ctx.c and included
 * at the the top of this file
 *
 ********************************************************/

#define CTX_PUTC(c,ctx) ctx->putC(ctx, c)

static void _php_image_output_putc(struct gdIOCtx *ctx, int c) /* {{{ */
{
	/* without the following downcast, the write will fail
	 * (i.e., will write a zero byte) for all
	 * big endian architectures:
	 */
	unsigned char ch = (unsigned char) c;
	php_write(&ch, 1);
} /* }}} */

static int _php_image_output_putbuf(struct gdIOCtx *ctx, const void* buf, int l) /* {{{ */
{
	return php_write((void *)buf, l);
} /* }}} */

static void _php_image_output_ctxfree(struct gdIOCtx *ctx) /* {{{ */
{
	if(ctx) {
		efree(ctx);
	}
} /* }}} */

static void _php_image_stream_putc(struct gdIOCtx *ctx, int c) /* {{{ */ {
	char ch = (char) c;
	php_stream * stream = (php_stream *)ctx->data;
	php_stream_write(stream, &ch, 1);
} /* }}} */

static int _php_image_stream_putbuf(struct gdIOCtx *ctx, const void* buf, int l) /* {{{ */
{
	php_stream * stream = (php_stream *)ctx->data;
	return php_stream_write(stream, (void *)buf, l);
} /* }}} */

static void _php_image_stream_ctxfree(struct gdIOCtx *ctx) /* {{{ */
{
	if(ctx->data) {
		ctx->data = NULL;
	}
	if(ctx) {
		efree(ctx);
	}
} /* }}} */

static void _php_image_stream_ctxfreeandclose(struct gdIOCtx *ctx) /* {{{ */
{

	if(ctx->data) {
		php_stream_close((php_stream *) ctx->data);
		ctx->data = NULL;
	}
	if(ctx) {
		efree(ctx);
	}
} /* }}} */

static gdIOCtx *create_stream_context_from_zval(zval *to_zval) {
	php_stream *stream;
	int close_stream = 1;

	if (Z_TYPE_P(to_zval) == IS_RESOURCE) {
		php_stream_from_zval_no_verify(stream, to_zval);
		if (stream == NULL) {
			return NULL;
		}
		close_stream = 0;
	} else if (Z_TYPE_P(to_zval) == IS_STRING) {
		if (CHECK_ZVAL_NULL_PATH(to_zval)) {
			zend_argument_type_error(2, "must not contain null bytes");
			return NULL;
		}

		stream = php_stream_open_wrapper(Z_STRVAL_P(to_zval), "wb", REPORT_ERRORS|IGNORE_PATH, NULL);
		if (stream == NULL) {
			return NULL;
		}
	} else {
		zend_argument_type_error(2, "must be a file name or a stream resource, %s given", zend_zval_type_name(to_zval));
		return NULL;
	}

	return create_stream_context(stream, close_stream);
}

static gdIOCtx *create_stream_context(php_stream *stream, int close_stream) {
	gdIOCtx *ctx = ecalloc(1, sizeof(gdIOCtx));

	ctx->putC = _php_image_stream_putc;
	ctx->putBuf = _php_image_stream_putbuf;
	if (close_stream) {
		ctx->gd_free = _php_image_stream_ctxfreeandclose;
	} else {
		ctx->gd_free = _php_image_stream_ctxfree;
	}
	ctx->data = (void *)stream;

	return ctx;
}

static gdIOCtx *create_output_context(void) {
	gdIOCtx *ctx = ecalloc(1, sizeof(gdIOCtx));

	ctx->putC = _php_image_output_putc;
	ctx->putBuf = _php_image_output_putbuf;
	ctx->gd_free = _php_image_output_ctxfree;

	return ctx;
}

static void _php_image_output_ctx(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn)
{
	zval *imgind;
	zend_long quality = -1, basefilter = -1, speed = -1;
	gdImagePtr im;
	gdIOCtx *ctx = NULL;
	zval *to_zval = NULL;

	if (image_type == PHP_GDIMG_TYPE_GIF) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!", &imgind, gd_image_ce, &to_zval) == FAILURE) {
			RETURN_THROWS();
		}
	} else if (image_type == PHP_GDIMG_TYPE_PNG) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!ll", &imgind, gd_image_ce, &to_zval, &quality, &basefilter) == FAILURE) {
			RETURN_THROWS();
		}
	} else if (image_type == PHP_GDIMG_TYPE_AVIF) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!ll", &imgind, gd_image_ce, &to_zval, &quality, &speed) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!l", &imgind, gd_image_ce, &to_zval, &quality) == FAILURE) {
			RETURN_THROWS();
		}
	}

	im = php_gd_libgdimageptr_from_zval_p(imgind);

	if (to_zval != NULL) {
		ctx = create_stream_context_from_zval(to_zval);
		if (!ctx) {
			RETURN_FALSE;
		}
	} else {
		ctx = create_output_context();
	}

	switch (image_type) {
#ifdef HAVE_GD_JPG
		case PHP_GDIMG_TYPE_JPG:
			gdImageJpegCtx(im, ctx, (int) quality);
			break;
#endif
#ifdef HAVE_GD_WEBP
		case PHP_GDIMG_TYPE_WEBP:
			if (quality == -1) {
				quality = 80;
			}
			gdImageWebpCtx(im, ctx, (int) quality);
			break;
#endif
#ifdef HAVE_GD_AVIF
		case PHP_GDIMG_TYPE_AVIF:
			if (speed == -1) {
				speed = 6;
			}
			gdImageAvifCtx(im, ctx, (int) quality, (int) speed);
			break;
#endif
#ifdef HAVE_GD_PNG
		case PHP_GDIMG_TYPE_PNG:
#ifdef HAVE_GD_BUNDLED
			gdImagePngCtxEx(im, ctx, (int) quality, (int) basefilter);
#else
			gdImagePngCtxEx(im, ctx, (int) quality);
#endif
			break;
#endif
		case PHP_GDIMG_TYPE_GIF:
			gdImageGifCtx(im, ctx);
			break;
		 EMPTY_SWITCH_DEFAULT_CASE()
	}

	ctx->gd_free(ctx);

	RETURN_TRUE;
}

/* }}} */
