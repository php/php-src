/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDF_H
#define PHP_PDF_H

#if HAVE_PDFLIB

#include <pdflib.h>

extern zend_module_entry pdf_module_entry;
#define pdf_module_ptr &pdf_module_entry

PHP_MINFO_FUNCTION(pdf);
PHP_MINIT_FUNCTION(pdf);
PHP_MSHUTDOWN_FUNCTION(pdf);
PHP_FUNCTION(pdf_new);		/* new function */
PHP_FUNCTION(pdf_delete);	/* new function */
PHP_FUNCTION(pdf_open_file);
PHP_FUNCTION(pdf_get_buffer);	/* new function */
PHP_FUNCTION(pdf_close);
PHP_FUNCTION(pdf_begin_page);
PHP_FUNCTION(pdf_end_page);
PHP_FUNCTION(pdf_get_majorversion);
PHP_FUNCTION(pdf_get_minorversion);
PHP_FUNCTION(pdf_get_value);
PHP_FUNCTION(pdf_set_value);
PHP_FUNCTION(pdf_get_parameter);
PHP_FUNCTION(pdf_set_parameter);
PHP_FUNCTION(pdf_findfont);	/* new function */
PHP_FUNCTION(pdf_setfont);	/* new function */
PHP_FUNCTION(pdf_show);
PHP_FUNCTION(pdf_show_xy);
PHP_FUNCTION(pdf_continue_text);
PHP_FUNCTION(pdf_show_boxed);
PHP_FUNCTION(pdf_stringwidth);	/* new parameters: [int font, float size] */
PHP_FUNCTION(pdf_set_text_pos);
PHP_FUNCTION(pdf_setdash);
PHP_FUNCTION(pdf_setpolydash);	/* new function: not yet finished */
PHP_FUNCTION(pdf_setflat);
PHP_FUNCTION(pdf_setlinejoin);
PHP_FUNCTION(pdf_setlinecap);
PHP_FUNCTION(pdf_setmiterlimit);
PHP_FUNCTION(pdf_setlinewidth);
PHP_FUNCTION(pdf_save);
PHP_FUNCTION(pdf_restore);
PHP_FUNCTION(pdf_translate);
PHP_FUNCTION(pdf_scale);
PHP_FUNCTION(pdf_rotate);
PHP_FUNCTION(pdf_skew);
PHP_FUNCTION(pdf_concat);	/* new function */
PHP_FUNCTION(pdf_moveto);
PHP_FUNCTION(pdf_lineto);
PHP_FUNCTION(pdf_curveto);
PHP_FUNCTION(pdf_circle);
PHP_FUNCTION(pdf_arc);
PHP_FUNCTION(pdf_rect);
PHP_FUNCTION(pdf_closepath);
PHP_FUNCTION(pdf_stroke);
PHP_FUNCTION(pdf_closepath_stroke);
PHP_FUNCTION(pdf_fill);
PHP_FUNCTION(pdf_fill_stroke);
PHP_FUNCTION(pdf_closepath_fill_stroke);
PHP_FUNCTION(pdf_clip);
PHP_FUNCTION(pdf_endpath);
PHP_FUNCTION(pdf_setgray_fill);
PHP_FUNCTION(pdf_setgray_stroke);
PHP_FUNCTION(pdf_setgray);
PHP_FUNCTION(pdf_setrgbcolor_fill);
PHP_FUNCTION(pdf_setrgbcolor_stroke);
PHP_FUNCTION(pdf_setrgbcolor);
PHP_FUNCTION(pdf_open_image_file);  /* new parameters: [char *stringpram, int intparam] */
PHP_FUNCTION(pdf_open_ccitt);	/* new function */
PHP_FUNCTION(pdf_open_image);	/* new function: checkit not yet completeted :( */
PHP_FUNCTION(pdf_close_image);
PHP_FUNCTION(pdf_place_image);
PHP_FUNCTION(pdf_add_bookmark);
PHP_FUNCTION(pdf_set_info);
PHP_FUNCTION(pdf_attach_file);	/* new function */
PHP_FUNCTION(pdf_add_note);	/* new function */
PHP_FUNCTION(pdf_add_pdflink);
PHP_FUNCTION(pdf_add_locallink);	/* new function */
PHP_FUNCTION(pdf_add_launchlink);	/* new function */
PHP_FUNCTION(pdf_add_weblink);
PHP_FUNCTION(pdf_set_border_style);
PHP_FUNCTION(pdf_set_border_color);
PHP_FUNCTION(pdf_set_border_dash);

/* RJS:
   End of the official PDFLIB V3.x API */

/* old font handling */
PHP_FUNCTION(pdf_set_font);		/* deprecated */
PHP_FUNCTION(pdf_get_font);		/* deprecated */
PHP_FUNCTION(pdf_get_fontname);		/* deprecated */
PHP_FUNCTION(pdf_get_fontsize);		/* deprecated */

/* old way of starting a PDF document */
PHP_FUNCTION(pdf_open);			/* deprecated */

/* old stuff for setting infos */
PHP_FUNCTION(pdf_set_info_creator);	/* deprecated */
PHP_FUNCTION(pdf_set_info_title);	/* deprecated */
PHP_FUNCTION(pdf_set_info_subject);	/* deprecated */
PHP_FUNCTION(pdf_set_info_author);	/* deprecated */
PHP_FUNCTION(pdf_set_info_keywords);	/* deprecated */
PHP_FUNCTION(pdf_set_leading);   	/* deprecated */
PHP_FUNCTION(pdf_set_text_rendering);	/* deprecated */
PHP_FUNCTION(pdf_set_horiz_scaling);	/* deprecated */
PHP_FUNCTION(pdf_set_text_rise);		/* deprecated */
PHP_FUNCTION(pdf_set_char_spacing);	/* deprecated */
PHP_FUNCTION(pdf_set_word_spacing);	/* deprecated */
PHP_FUNCTION(pdf_set_transition);	/* deprecated */
PHP_FUNCTION(pdf_set_duration);		/* deprecated */
PHP_FUNCTION(pdf_get_image_height);	/* deprecated */
PHP_FUNCTION(pdf_get_image_width);	/* deprecated */

/* old stuff for opening images */
PHP_FUNCTION(pdf_open_jpeg);		/* deprecated */
PHP_FUNCTION(pdf_open_tiff);		/* deprecated */
PHP_FUNCTION(pdf_open_png);		/* deprecated */
PHP_FUNCTION(pdf_open_gif);		/* deprecated */

/* some more stuff for compatibility */
PHP_FUNCTION(pdf_add_annotation);
#if HAVE_LIBGD13
PHP_FUNCTION(pdf_open_memory_image);
#endif

#if (PDFLIB_MAJORVERSION >= 4)
/* support for new functions in PDFlib V4.0 */
PHP_FUNCTION(pdf_open_pdi);
PHP_FUNCTION(pdf_close_pdi);
PHP_FUNCTION(pdf_open_pdi_page);
PHP_FUNCTION(pdf_place_pdi_page);
PHP_FUNCTION(pdf_close_pdi_page);
PHP_FUNCTION(pdf_get_pdi_parameter);
PHP_FUNCTION(pdf_get_pdi_value);
PHP_FUNCTION(pdf_begin_pattern);
PHP_FUNCTION(pdf_end_pattern);
PHP_FUNCTION(pdf_setcolor);
PHP_FUNCTION(pdf_makespotcolor);
PHP_FUNCTION(pdf_begin_template);
PHP_FUNCTION(pdf_end_template);
PHP_FUNCTION(pdf_arcn);
PHP_FUNCTION(pdf_add_thumbnail);
PHP_FUNCTION(pdf_initgraphics);
PHP_FUNCTION(pdf_setmatrix);
#endif /* PDFlib >= V4 */

#ifdef ZTS
#define PDFG(v) TSRMG(pdf_globals_id, php_pdf_globals *, v)
#else
#define PDFG(v) (pdf_globals.v)
#endif


#else
#define pdf_module_ptr NULL
#endif
#define phpext_pdf_ptr pdf_module_ptr
#endif /* PHP_PDF_H */
