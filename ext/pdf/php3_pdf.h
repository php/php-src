/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Uwe Steinmann                                               |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef _PHP3_PDF_H
#define _PHP3_PDF_H

#if HAVE_PDFLIB

#include <pdf.h>

extern int le_fp;

extern php3_module_entry pdf_module_entry;
#define pdf_module_ptr &pdf_module_entry

extern void php3_info_pdf(ZEND_MODULE_INFO_FUNC_ARGS);
extern int php3_minit_pdf(INIT_FUNC_ARGS);
extern int php3_mend_pdf(void);
extern void php3_pdf_get_info(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_info_creator(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_info_title(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_info_subject(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_info_author(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_info_keywords(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_begin_page(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_end_page(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_show(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_show_xy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_font(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_leading(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_text_rendering(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_horiz_scaling(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_text_rise(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_text_matrix(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_text_pos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_char_spacing(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_word_spacing(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_continue_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_stringwidth(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_save(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_restore(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_translate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_scale(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_rotate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setflat(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setlinejoin(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setlinecap(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setmiterlimit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setlinewidth(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setdash(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_moveto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_curveto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_lineto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_circle(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_arc(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_rect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_closepath(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_closepath_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_fill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_fill_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_closepath_fill_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_endpath(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_clip(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setgray_fill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setgray_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setgray(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setrgbcolor_fill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setrgbcolor_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_setrgbcolor(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_add_outline(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_transition(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pdf_set_duration(INTERNAL_FUNCTION_PARAMETERS);
#else
#define pdf_module_ptr NULL
#endif
#endif /* _PHP3_PDF_H */
