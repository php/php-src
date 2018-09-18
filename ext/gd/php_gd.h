/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if HAVE_LIBFREETYPE
# ifndef ENABLE_GD_TTF
#  define ENABLE_GD_TTF
# endif
#endif

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
#define PHP_GDIMG_CONVERT_WBM   7
#define PHP_GDIMG_TYPE_GD       8
#define PHP_GDIMG_TYPE_GD2      9
#define PHP_GDIMG_TYPE_GD2PART  10
#define PHP_GDIMG_TYPE_WEBP     11
#define PHP_GDIMG_TYPE_BMP      12

#define PHP_IMG_GIF    1
#define PHP_IMG_JPG    2
#define PHP_IMG_JPEG   2
#define PHP_IMG_PNG    4
#define PHP_IMG_WBMP   8
#define PHP_IMG_XPM   16
#define PHP_IMG_WEBP  32
#define PHP_IMG_BMP   64

#ifdef PHP_WIN32
#	define PHP_GD_API __declspec(dllexport)
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

extern zend_module_entry gd_module_entry;
#define phpext_gd_ptr &gd_module_entry

#include "php_version.h"
#define PHP_GD_VERSION PHP_VERSION

/* gd.c functions */
PHP_MINFO_FUNCTION(gd);
PHP_MINIT_FUNCTION(gd);
#if HAVE_GD_FREETYPE && HAVE_LIBFREETYPE
PHP_RSHUTDOWN_FUNCTION(gd);
#endif

PHP_FUNCTION(gd_info);
PHP_FUNCTION(imagearc);
PHP_FUNCTION(imageellipse);
PHP_FUNCTION(imagechar);
PHP_FUNCTION(imagecharup);
PHP_FUNCTION(imageistruecolor);
PHP_FUNCTION(imagecolorallocate);
PHP_FUNCTION(imagepalettecopy);
PHP_FUNCTION(imagecolorat);
PHP_FUNCTION(imagecolorclosest);
PHP_FUNCTION(imagecolorclosesthwb);
PHP_FUNCTION(imagecolordeallocate);
PHP_FUNCTION(imagecolorresolve);
PHP_FUNCTION(imagecolorexact);
PHP_FUNCTION(imagecolorset);
PHP_FUNCTION(imagecolorstotal);
PHP_FUNCTION(imagecolorsforindex);
PHP_FUNCTION(imagecolortransparent);
PHP_FUNCTION(imagecopy);
PHP_FUNCTION(imagecopymerge);
PHP_FUNCTION(imagecopyresized);
PHP_FUNCTION(imagetypes);
PHP_FUNCTION(imagecreate);
PHP_FUNCTION(imageftbbox);
PHP_FUNCTION(imagefttext);

PHP_FUNCTION(imagecreatetruecolor);
PHP_FUNCTION(imagetruecolortopalette);
PHP_FUNCTION(imagepalettetotruecolor);
PHP_FUNCTION(imagesetthickness);
PHP_FUNCTION(imagefilledellipse);
PHP_FUNCTION(imagefilledarc);
PHP_FUNCTION(imagealphablending);
PHP_FUNCTION(imagesavealpha);
PHP_FUNCTION(imagecolorallocatealpha);
PHP_FUNCTION(imagecolorresolvealpha);
PHP_FUNCTION(imagecolorclosestalpha);
PHP_FUNCTION(imagecolorexactalpha);
PHP_FUNCTION(imagecopyresampled);

#ifdef PHP_WIN32
PHP_FUNCTION(imagegrabwindow);
PHP_FUNCTION(imagegrabscreen);
#endif

PHP_FUNCTION(imagerotate);

PHP_FUNCTION(imageflip);

PHP_FUNCTION(imageantialias);

PHP_FUNCTION(imagecrop);
PHP_FUNCTION(imagecropauto);
PHP_FUNCTION(imagescale);
PHP_FUNCTION(imageaffine);
PHP_FUNCTION(imageaffinematrixget);
PHP_FUNCTION(imageaffinematrixconcat);
PHP_FUNCTION(imagesetinterpolation);

PHP_FUNCTION(imagesetthickness);
PHP_FUNCTION(imagecopymergegray);
PHP_FUNCTION(imagesetbrush);
PHP_FUNCTION(imagesettile);
PHP_FUNCTION(imagesetstyle);

PHP_FUNCTION(imagecreatefromstring);
PHP_FUNCTION(imagecreatefromgif);
PHP_FUNCTION(imagecreatefromjpeg);
PHP_FUNCTION(imagecreatefromxbm);
PHP_FUNCTION(imagecreatefromwebp);
PHP_FUNCTION(imagecreatefrompng);
PHP_FUNCTION(imagecreatefromwbmp);
PHP_FUNCTION(imagecreatefromgd);
PHP_FUNCTION(imagecreatefromgd2);
PHP_FUNCTION(imagecreatefromgd2part);
#if defined(HAVE_GD_BMP)
PHP_FUNCTION(imagecreatefrombmp);
#endif
#if defined(HAVE_GD_XPM)
PHP_FUNCTION(imagecreatefromxpm);
#endif

PHP_FUNCTION(imagegammacorrect);
PHP_FUNCTION(imagedestroy);
PHP_FUNCTION(imagefill);
PHP_FUNCTION(imagefilledpolygon);
PHP_FUNCTION(imagefilledrectangle);
PHP_FUNCTION(imagefilltoborder);
PHP_FUNCTION(imagefontwidth);
PHP_FUNCTION(imagefontheight);

PHP_FUNCTION(imagegif );
PHP_FUNCTION(imagejpeg );
PHP_FUNCTION(imagepng);
PHP_FUNCTION(imagewebp);
PHP_FUNCTION(imagewbmp);
PHP_FUNCTION(imagegd);
PHP_FUNCTION(imagegd2);
#if defined(HAVE_GD_BMP)
PHP_FUNCTION(imagebmp);
#endif

PHP_FUNCTION(imageinterlace);
PHP_FUNCTION(imageline);
PHP_FUNCTION(imageloadfont);
PHP_FUNCTION(imagepolygon);
PHP_FUNCTION(imageopenpolygon);
PHP_FUNCTION(imagerectangle);
PHP_FUNCTION(imagesetpixel);
PHP_FUNCTION(imagestring);
PHP_FUNCTION(imagestringup);
PHP_FUNCTION(imagesx);
PHP_FUNCTION(imagesy);
PHP_FUNCTION(imagesetclip);
PHP_FUNCTION(imagegetclip);
PHP_FUNCTION(imagedashedline);
PHP_FUNCTION(imagettfbbox);
PHP_FUNCTION(imagettftext);

PHP_FUNCTION(jpeg2wbmp);
PHP_FUNCTION(png2wbmp);
PHP_FUNCTION(image2wbmp);

PHP_FUNCTION(imagecolormatch);

PHP_FUNCTION(imagelayereffect);
PHP_FUNCTION(imagexbm);

PHP_FUNCTION(imagefilter);
PHP_FUNCTION(imageconvolution);

PHP_FUNCTION(imageresolution);

PHP_GD_API int phpi_get_le_gd(void);

#else

#define phpext_gd_ptr NULL

#endif

#endif /* PHP_GD_H */
