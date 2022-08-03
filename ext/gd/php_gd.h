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
   +----------------------------------------------------------------------+
*/

#ifndef PHP_GD_H
#define PHP_GD_H

#include "zend_string.h"
#include "php_streams.h"

#if defined(HAVE_LIBGD) || defined(HAVE_GD_BUNDLED)

/* open_basedir and safe_mode checks */
#define PHP_GD_CHECK_OPEN_BASEDIR(filename, errormsg)                       \
	if (!filename || php_check_open_basedir(filename)) {      \
		php_error_docref(NULL, E_WARNING, errormsg);      \
		RETURN_FALSE;                                               \
	}

#define PHP_GDIMG_TYPE_GIF      1
#define PHP_GDIMG_TYPE_PNG      2
#define PHP_GDIMG_TYPE_JPG      3
#define PHP_GDIMG_TYPE_WBM      4
#define PHP_GDIMG_TYPE_XBM      5
#define PHP_GDIMG_TYPE_XPM      6
#define PHP_GDIMG_TYPE_GD       8
#define PHP_GDIMG_TYPE_GD2      9
#define PHP_GDIMG_TYPE_GD2PART  10
#define PHP_GDIMG_TYPE_WEBP     11
#define PHP_GDIMG_TYPE_BMP      12
#define PHP_GDIMG_TYPE_TGA      13
#define PHP_GDIMG_TYPE_AVIF     14

#define PHP_IMG_GIF    1
#define PHP_IMG_JPG    2
#define PHP_IMG_JPEG   2
#define PHP_IMG_PNG    4
#define PHP_IMG_WBMP   8
#define PHP_IMG_XPM   16
#define PHP_IMG_WEBP  32
#define PHP_IMG_BMP   64
#define PHP_IMG_TGA  128
#define PHP_IMG_AVIF 256

/* Section Filters Declarations */
/* IMPORTANT NOTE FOR NEW FILTER
 * Do not forget to update:
 * IMAGE_FILTER_MAX: define the last filter index
 * IMAGE_FILTER_MAX_ARGS: define the biggest amount of arguments
 * image_filter array in PHP_FUNCTION(imagefilter)
 * */
#define IMAGE_FILTER_NEGATE         0
#define IMAGE_FILTER_GRAYSCALE      1
#define IMAGE_FILTER_BRIGHTNESS     2
#define IMAGE_FILTER_CONTRAST       3
#define IMAGE_FILTER_COLORIZE       4
#define IMAGE_FILTER_EDGEDETECT     5
#define IMAGE_FILTER_EMBOSS         6
#define IMAGE_FILTER_GAUSSIAN_BLUR  7
#define IMAGE_FILTER_SELECTIVE_BLUR 8
#define IMAGE_FILTER_MEAN_REMOVAL   9
#define IMAGE_FILTER_SMOOTH         10
#define IMAGE_FILTER_PIXELATE       11
#define IMAGE_FILTER_SCATTER		12
#define IMAGE_FILTER_MAX            12
#define IMAGE_FILTER_MAX_ARGS       6

#ifdef HAVE_GD_BUNDLED
#define GD_BUNDLED 1
#else
#define GD_BUNDLED 0
#endif

#ifdef PHP_WIN32
#	ifdef PHP_GD_EXPORTS
#		define PHP_GD_API __declspec(dllexport)
#	else
#		define PHP_GD_API __declspec(dllimport)
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GD_API __attribute__ ((visibility("default")))
#else
#	define PHP_GD_API
#endif

PHPAPI extern const char php_sig_gif[3];
PHPAPI extern const char php_sig_jpg[3];
PHPAPI extern const char php_sig_png[8];
PHPAPI extern const char php_sig_bmp[2];
PHPAPI extern const char php_sig_riff[4];
PHPAPI extern const char php_sig_webp[4];
PHPAPI extern const char php_sig_avif[4];

extern zend_module_entry gd_module_entry;
#define phpext_gd_ptr &gd_module_entry

#include "php_version.h"
#define PHP_GD_VERSION PHP_VERSION

/* gd.c functions */
PHP_MINFO_FUNCTION(gd);
PHP_MINIT_FUNCTION(gd);
PHP_MSHUTDOWN_FUNCTION(gd);
PHP_RSHUTDOWN_FUNCTION(gd);

PHP_GD_API struct gdImageStruct *php_gd_libgdimageptr_from_zval_p(zval* zp);

#else

#define phpext_gd_ptr NULL

#endif

#endif /* PHP_GD_H */
