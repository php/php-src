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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDF_H
#define PHP_PDF_H

#if HAVE_PDFLIB

/* with version 2.20 of pdflib PDF_open_mem isn't available unless
   the following define isn't there.
*/
#define PDF_OPEN_MEM_SUPPORTED
#include <pdflib.h>

extern zend_module_entry pdf_module_entry;
#define pdf_module_ptr &pdf_module_entry

extern PHP_MINFO_FUNCTION(pdf);
extern PHP_MINIT_FUNCTION(pdf);
extern PHP_MSHUTDOWN_FUNCTION(pdf);
PHP_FUNCTION(pdf_set_info);
PHP_FUNCTION(pdf_set_info_creator);
PHP_FUNCTION(pdf_set_info_title);
PHP_FUNCTION(pdf_set_info_subject);
PHP_FUNCTION(pdf_set_info_author);
PHP_FUNCTION(pdf_set_info_keywords);
PHP_FUNCTION(pdf_open);
PHP_FUNCTION(pdf_close);
PHP_FUNCTION(pdf_begin_page);
PHP_FUNCTION(pdf_end_page);
PHP_FUNCTION(pdf_show);
PHP_FUNCTION(pdf_show_xy);
#if PDFLIB_MAJORVERSION >= 3 | (PDFLIB_MAJORVERSION >= 2 & PDFLIB_MINORVERSION >= 20)
PHP_FUNCTION(pdf_show_boxed);
PHP_FUNCTION(pdf_skew);
#endif
PHP_FUNCTION(pdf_set_font);
PHP_FUNCTION(pdf_get_font);
PHP_FUNCTION(pdf_get_fontname);
PHP_FUNCTION(pdf_get_fontsize);
PHP_FUNCTION(pdf_set_leading);
PHP_FUNCTION(pdf_set_text_rendering);
PHP_FUNCTION(pdf_set_horiz_scaling);
PHP_FUNCTION(pdf_set_text_rise);
#if PDFLIB_MAJORVERSION < 3 & PDFLIB_MINORVERSION < 30
PHP_FUNCTION(pdf_set_text_matrix);
#endif
PHP_FUNCTION(pdf_set_text_pos);
PHP_FUNCTION(pdf_set_char_spacing);
PHP_FUNCTION(pdf_set_word_spacing);
PHP_FUNCTION(pdf_continue_text);
PHP_FUNCTION(pdf_stringwidth);
PHP_FUNCTION(pdf_save);
PHP_FUNCTION(pdf_restore);
PHP_FUNCTION(pdf_translate);
PHP_FUNCTION(pdf_scale);
PHP_FUNCTION(pdf_rotate);
PHP_FUNCTION(pdf_setflat);
PHP_FUNCTION(pdf_setlinejoin);
PHP_FUNCTION(pdf_setlinecap);
PHP_FUNCTION(pdf_setmiterlimit);
PHP_FUNCTION(pdf_setlinewidth);
PHP_FUNCTION(pdf_setdash);
PHP_FUNCTION(pdf_moveto);
PHP_FUNCTION(pdf_curveto);
PHP_FUNCTION(pdf_lineto);
PHP_FUNCTION(pdf_circle);
PHP_FUNCTION(pdf_arc);
PHP_FUNCTION(pdf_rect);
PHP_FUNCTION(pdf_closepath);
PHP_FUNCTION(pdf_closepath_stroke);
PHP_FUNCTION(pdf_stroke);
PHP_FUNCTION(pdf_fill);
PHP_FUNCTION(pdf_fill_stroke);
PHP_FUNCTION(pdf_closepath_fill_stroke);
PHP_FUNCTION(pdf_endpath);
PHP_FUNCTION(pdf_clip);
PHP_FUNCTION(pdf_set_parameter);
PHP_FUNCTION(pdf_get_parameter);
PHP_FUNCTION(pdf_set_value);
PHP_FUNCTION(pdf_get_value);
PHP_FUNCTION(pdf_setgray_fill);
PHP_FUNCTION(pdf_setgray_stroke);
PHP_FUNCTION(pdf_setgray);
PHP_FUNCTION(pdf_setrgbcolor_fill);
PHP_FUNCTION(pdf_setrgbcolor_stroke);
PHP_FUNCTION(pdf_setrgbcolor);
PHP_FUNCTION(pdf_add_outline);
PHP_FUNCTION(pdf_set_transition);
PHP_FUNCTION(pdf_set_duration);
PHP_FUNCTION(pdf_open_jpeg);
PHP_FUNCTION(pdf_open_tiff);
PHP_FUNCTION(pdf_open_png);
PHP_FUNCTION(pdf_open_gif);
PHP_FUNCTION(pdf_open_image_file);
#if HAVE_LIBGD13
PHP_FUNCTION(pdf_open_memory_image);
#endif
PHP_FUNCTION(pdf_close_image);
PHP_FUNCTION(pdf_place_image);
PHP_FUNCTION(pdf_put_image);
PHP_FUNCTION(pdf_execute_image);
PHP_FUNCTION(pdf_add_weblink);
PHP_FUNCTION(pdf_add_pdflink);
PHP_FUNCTION(pdf_add_annotation);
PHP_FUNCTION(pdf_set_border_style);
PHP_FUNCTION(pdf_set_border_color);
PHP_FUNCTION(pdf_set_border_dash);
PHP_FUNCTION(pdf_get_image_width);
PHP_FUNCTION(pdf_get_image_height);


#ifdef ZTS
#define PDFG(v) (pdf_globals->v)
#define PDFLS_FETCH() php_pdf_globals *pdf_globals = ts_resource(pdf_globals_id)
#else
#define PDFG(v) (pdf_globals.v)
#define PDFLS_FETCH()
#endif


#else
#define pdf_module_ptr NULL
#endif
#define phpext_pdf_ptr pdf_module_ptr
#endif /* PHP_PDF_H */
