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

#ifndef _PHP3_GD_H
#define _PHP3_GD_H

#if COMPILE_DL
#undef HAVE_LIBGD
#define HAVE_LIBGD 1
#endif

#if HAVE_LIBGD

#include <gd.h>

extern php3_module_entry gd_module_entry;
#define phpext_gd_ptr &gd_module_entry

/* gd.c functions */
extern void php3_info_gd(void);
extern int php3_minit_gd(INIT_FUNC_ARGS);
extern int php3_mend_gd(SHUTDOWN_FUNC_ARGS);
extern int gdImageColorResolve(gdImagePtr, int, int, int);
extern void php3_imagearc(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagechar(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecharup(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorallocate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorat(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorclosest(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolordeallocate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorresolve(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorexact(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorset(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorstotal(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolorsforindex(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecolortransparent(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecopy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecopyresized(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecreate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagecreatefromgif (INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagedestroy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagefill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagefilledpolygon(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagefilledrectangle(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagefilltoborder(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagefontwidth(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagefontheight(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagegif (INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imageinterlace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imageline(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imageloadfont(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagepolygon(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagerectangle(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagesetpixel(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagestring(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagestringup(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagesxfn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagesyfn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_free_gd_font(gdFontPtr);
extern void _php3_gdimagecharup(gdImagePtr, gdFontPtr, int, int, int, int);
extern void php3_imagedashedline(INTERNAL_FUNCTION_PARAMETERS);
#ifdef HAVE_LIBTTF
extern void php3_imagettfbbox(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_imagettftext(INTERNAL_FUNCTION_PARAMETERS);
#endif
#else

#define phpext_gd_ptr NULL

#endif

#endif /* _PHP3_GD_H */
