/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997 PHP Development Team (See Credits file)           |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@guardian.no>                               |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHP_GD_H
#define _PHP_GD_H

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
PHP_FUNCTION(imagecreate);
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
PHPAPI int phpi_get_le_gd(void);

static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)());
static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, void (*func_p)());
static void _php_image_output_wbmp(gdImagePtr im, FILE *fp);

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

#endif /* _PHP_GD_H */
