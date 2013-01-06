/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_GD_H
#define PHP_GD_H

#define HAVE_GDIMAGECREATEFROMPNG 1

#if HAVE_LIBFREETYPE
# ifndef ENABLE_GD_TTF
#  define ENABLE_GD_TTF
# endif
#endif

#if HAVE_LIBGD

/* open_basedir and safe_mode checks */
#define PHP_GD_CHECK_OPEN_BASEDIR(filename, errormsg)                       \
	if (!filename || php_check_open_basedir(filename TSRMLS_CC)) {      \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, errormsg);      \
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

#ifdef PHP_WIN32
#	define PHP_GD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GD_API __attribute__ ((visibility("default")))
#else
#	define PHP_GD_API
#endif

PHPAPI extern const char php_sig_gif[3];
PHPAPI extern const char php_sig_jpg[3];
PHPAPI extern const char php_sig_png[3];

extern zend_module_entry gd_module_entry;
#define phpext_gd_ptr &gd_module_entry

/* gd.c functions */
PHP_MINFO_FUNCTION(gd);
PHP_MINIT_FUNCTION(gd);
#if HAVE_LIBT1 || HAVE_GD_FONTMUTEX
PHP_MSHUTDOWN_FUNCTION(gd);
#endif
#if HAVE_GD_STRINGFT
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

#ifdef HAVE_GD_BUNDLED
PHP_FUNCTION(imageantialias);
#endif

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
#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
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

PHP_FUNCTION(imageinterlace);
PHP_FUNCTION(imageline);
PHP_FUNCTION(imageloadfont);
PHP_FUNCTION(imagepolygon);
PHP_FUNCTION(imagerectangle);
PHP_FUNCTION(imagesetpixel);
PHP_FUNCTION(imagestring);
PHP_FUNCTION(imagestringup);
PHP_FUNCTION(imagesx);
PHP_FUNCTION(imagesy);
PHP_FUNCTION(imagedashedline);
PHP_FUNCTION(imagettfbbox);
PHP_FUNCTION(imagettftext);
PHP_FUNCTION(imagepsloadfont);
/*
PHP_FUNCTION(imagepscopyfont);
*/
PHP_FUNCTION(imagepsfreefont);
PHP_FUNCTION(imagepsencodefont);
PHP_FUNCTION(imagepsextendfont);
PHP_FUNCTION(imagepsslantfont);
PHP_FUNCTION(imagepstext);
PHP_FUNCTION(imagepsbbox);

PHP_FUNCTION(jpeg2wbmp);
PHP_FUNCTION(png2wbmp);
PHP_FUNCTION(image2wbmp);

PHP_FUNCTION(imagecolormatch);

#if HAVE_GD_BUNDLED
PHP_FUNCTION(imagelayereffect);
PHP_FUNCTION(imagexbm);
#endif

PHP_FUNCTION(imagefilter);
PHP_FUNCTION(imageconvolution);

PHP_GD_API int phpi_get_le_gd(void);

#else

#define phpext_gd_ptr NULL

#endif

#endif /* PHP_GD_H */
