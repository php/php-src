/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@guardian.no>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_GD_H
#define PHP_GD_H

#define HAVE_GDIMAGECREATEFROMPNG 1

#if HAVE_LIBTTF|HAVE_LIBFREETYPE
#define ENABLE_GD_TTF
#endif

#if HAVE_LIBGD

#include <gd.h>

#if HAVE_LIBT1
#include "gdt1.h"
#endif

#define PHP_GDIMG_TYPE_GIF 1
#define PHP_GDIMG_TYPE_PNG 2
#define PHP_GDIMG_TYPE_JPG 3
#define PHP_GDIMG_TYPE_WBM 4
#define PHP_GDIMG_TYPE_XBM 5
#define PHP_GDIMG_TYPE_XPM 6

extern const char php_sig_gif[3];
extern const char php_sig_jpg[3];
extern const char php_sig_png[3];

extern zend_module_entry gd_module_entry;
#define phpext_gd_ptr &gd_module_entry

typedef struct {
	int le_gd;
	int le_gd_font;
#if HAVE_LIBT1
	int le_ps_font;
	int le_ps_enc;
#endif
} php_gd_globals;

/* gd.c functions */
PHP_MINFO_FUNCTION(gd);
extern PHP_MINIT_FUNCTION(gd);
extern PHP_MSHUTDOWN_FUNCTION(gd);

#ifndef HAVE_GDIMAGECOLORRESOLVE
extern int gdImageColorResolve(gdImagePtr, int, int, int);
#endif
PHP_FUNCTION(imagearc);
PHP_FUNCTION(imagechar);
PHP_FUNCTION(imagecharup);
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
PHP_FUNCTION(imagecreatefromstring);
PHP_FUNCTION(imagecreatefromgif);
PHP_FUNCTION(imagecreatefromjpeg);
PHP_FUNCTION(imagecreatefromxbm);
PHP_FUNCTION(imagecreatefromxpm);
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
PHP_FUNCTION(imagecreatefrompng);
PHP_FUNCTION(imagepng);
PHP_FUNCTION(imagecreatefromwbmp);
PHP_FUNCTION(imagewbmp);
void php_gdimagecharup(gdImagePtr, gdFontPtr, int, int, int, int);
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

gdImagePtr _php_image_create_from_string ( zval **Data, char *tn, gdImagePtr (*ioctx_func_p)() );
static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(), gdImagePtr (*ioctx_func_p)());
static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, void (*func_p)());
static void _php_image_output_wbmp(gdImagePtr im, FILE *fp);
static int _php_image_type ( char data[8] );

PHPAPI int phpi_get_le_gd(void);

#ifdef ZTS
#define GDLS_D php_gd_globals *gd_globals
#define GDG(v) (gd_globals->v)
#define GDLS_FETCH() php_gd_globals *gd_globals = ts_resource(gd_globals_id)
#else
#define GDLS_D
#define GDG(v) (gd_globals.v)
#define GDLS_FETCH()
#endif

#else

#define phpext_gd_ptr NULL

#endif

#endif /* PHP_GD_H */
