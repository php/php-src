/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* pdflib 2.02 is subject to the ALADDIN FREE PUBLIC LICENSE.
   Copyright (C) 1997 Thomas Merz. */

/* Note that there is no code from the pdflib package in this file */

#if !PHP_31 && defined(THREAD_SAFE)
#undef THREAD_SAFE
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"

#include <math.h>

#if HAVE_LIBGD13
#include "gd.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_PDFLIB

#include "php_pdf.h"

#ifdef THREAD_SAFE
DWORD PDFlibTls;
static int numthreads=0;

typedef struct pdflib_global_struct{
	int le_pdf_image;
	int le_outline;
	int le_pdf;
} pdflib_global_struct;

# define PDF_GLOBAL(a) pdflib_globals->a
# define PDF_TLS_VARS pdflib_global_struct *pdflib_globals=TlsGetValue(PDFlibTls)

#else
#  define PDF_GLOBAL(a) a
#  define PDF_TLS_VARS
static int le_pdf_image;
static int le_outline;
static int le_pdf;
#endif

function_entry pdf_functions[] = {
	PHP_FE(pdf_set_info, NULL)
	PHP_FE(pdf_set_info_creator, NULL)
	PHP_FE(pdf_set_info_title, NULL)
	PHP_FE(pdf_set_info_subject, NULL)
	PHP_FE(pdf_set_info_author, NULL)
	PHP_FE(pdf_set_info_keywords, NULL)
	PHP_FE(pdf_open, NULL)
	PHP_FE(pdf_close, NULL)
	PHP_FE(pdf_begin_page, NULL)
	PHP_FE(pdf_end_page, NULL)
	PHP_FE(pdf_show, NULL)
	PHP_FE(pdf_show_xy, NULL)
	PHP_FE(pdf_show_boxed, NULL)
	PHP_FE(pdf_skew, NULL)
	PHP_FE(pdf_set_font, NULL)
	PHP_FE(pdf_set_leading, NULL)
	PHP_FE(pdf_set_text_rendering, NULL)
	PHP_FE(pdf_set_horiz_scaling, NULL)
	PHP_FE(pdf_set_text_rise, NULL)
	PHP_FE(pdf_set_text_pos, NULL)
	PHP_FE(pdf_set_char_spacing, NULL)
	PHP_FE(pdf_set_word_spacing, NULL)
	PHP_FE(pdf_get_font, NULL)
	PHP_FE(pdf_get_fontname, NULL)
	PHP_FE(pdf_get_fontsize, NULL)
	PHP_FE(pdf_continue_text, NULL)
	PHP_FE(pdf_stringwidth, NULL)
	PHP_FE(pdf_save, NULL)
	PHP_FE(pdf_restore, NULL)
	PHP_FE(pdf_translate, NULL)
	PHP_FE(pdf_scale, NULL)
	PHP_FE(pdf_rotate, NULL)
	PHP_FE(pdf_setflat, NULL)
	PHP_FE(pdf_setlinejoin, NULL)
	PHP_FE(pdf_setlinecap, NULL)
	PHP_FE(pdf_setmiterlimit, NULL)
	PHP_FE(pdf_setlinewidth, NULL)
	PHP_FE(pdf_setdash, NULL)
	PHP_FE(pdf_moveto, NULL)
	PHP_FE(pdf_lineto, NULL)
	PHP_FE(pdf_curveto, NULL)
	PHP_FE(pdf_circle, NULL)
	PHP_FE(pdf_arc, NULL)
	PHP_FE(pdf_rect, NULL)
	PHP_FE(pdf_closepath, NULL)
	PHP_FE(pdf_stroke, NULL)
	PHP_FE(pdf_closepath_stroke, NULL)
	PHP_FE(pdf_fill, NULL)
	PHP_FE(pdf_fill_stroke, NULL)
	PHP_FE(pdf_closepath_fill_stroke, NULL)
	PHP_FE(pdf_endpath, NULL)
	PHP_FE(pdf_clip, NULL)
	PHP_FE(pdf_set_parameter, NULL)
	PHP_FE(pdf_get_parameter, NULL)
	PHP_FE(pdf_set_value, NULL)
	PHP_FE(pdf_get_value, NULL)
	PHP_FE(pdf_setgray_fill, NULL)
	PHP_FE(pdf_setgray_stroke, NULL)
	PHP_FE(pdf_setgray, NULL)
	PHP_FE(pdf_setrgbcolor_fill, NULL)
	PHP_FE(pdf_setrgbcolor_stroke, NULL)
	PHP_FE(pdf_setrgbcolor, NULL)
	PHP_FE(pdf_add_outline, NULL)
	PHP_FALIAS(pdf_add_bookmark, pdf_add_outline, NULL)
	PHP_FE(pdf_set_transition, NULL)
	PHP_FE(pdf_set_duration, NULL)
	PHP_FE(pdf_open_jpeg, NULL)
	PHP_FE(pdf_open_tiff, NULL)
	PHP_FE(pdf_open_png, NULL)
#if HAVE_LIBGD13
	PHP_FE(pdf_open_memory_image, NULL)
#endif
	PHP_FE(pdf_open_gif, NULL)
	PHP_FE(pdf_open_image_file, NULL)
	PHP_FE(pdf_close_image, NULL)
	PHP_FE(pdf_place_image, NULL)
	PHP_FE(pdf_add_weblink, NULL)
	PHP_FE(pdf_add_pdflink, NULL)
	PHP_FE(pdf_add_annotation, NULL)
	PHP_FE(pdf_set_border_style, NULL)
	PHP_FE(pdf_set_border_color, NULL)
	PHP_FE(pdf_set_border_dash, NULL)
	PHP_FE(pdf_get_image_height, NULL)
	PHP_FE(pdf_get_image_width, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry pdf_module_entry = {
	"pdf", pdf_functions, PHP_MINIT(pdf), PHP_MSHUTDOWN(pdf), NULL, NULL, PHP_MINFO(pdf), STANDARD_MODULE_PROPERTIES 
};

#ifdef COMPILE_DL_PDF
#include "dl/phpdl.h"
ZEND_GET_MODULE(pdf)
#endif

static void _free_pdf_image(int image)
{
}

static void _free_pdf_doc(PDF *pdf)
{
	PDF_close(pdf);
	PDF_delete(pdf);
}

static void _free_outline(int *outline)
{
	if(outline) efree(outline);
}

static void custom_errorhandler(PDF *p, int type, const char*shortmsg) {
	switch (type){
		case PDF_NonfatalError:
			return;
		case PDF_MemoryError:/*give up in all other cases */
		case PDF_IOError:
		case PDF_RuntimeError:
		case PDF_IndexError:
		case PDF_TypeError:
		case PDF_DivisionByZero:
		case PDF_OverflowError:
		case PDF_SyntaxError:
		case PDF_ValueError:
		case PDF_SystemError:
		case PDF_UnknownError:
		default:
			if (p !=NULL)
				PDF_delete(p);/*clean up PDFlib */
			php3_error(E_ERROR,"Internal pdflib error: %s", shortmsg);
		}
}

static void *pdf_emalloc(PDF *p, size_t size, const char *caller) {
	return(emalloc(size));
}

static void *pdf_realloc(PDF *p, void *mem, size_t size, const char *caller) {
	return(erealloc(mem, size));
}

static void pdf_efree(PDF *p, void *mem) {
	return(efree(mem));
}

static size_t pdf_flushwrite(PDF *p, void *data, size_t size){
	if(php_header())
		return(php_write(data, size));
	return 0;
}

PHP_MINIT_FUNCTION(pdf)
{
	PDF_GLOBAL(le_pdf_image) = register_list_destructors(_free_pdf_image, NULL);
	PDF_GLOBAL(le_outline) = register_list_destructors(_free_outline, NULL);
	PDF_GLOBAL(le_pdf) = register_list_destructors(_free_pdf_doc, NULL);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(pdf)
{
	char tmp[32];

	snprintf(tmp, 31, "%d.%02d", PDF_get_majorversion(), PDF_get_minorversion() );
	tmp[31]=0;

	php_info_print_table_start();
	php_info_print_table_row(2, "PDF Support", "enabled" );
	php_info_print_table_row(2, "PDFLib Version", tmp );
	php_info_print_table_row(2, "CJK Font Support", "yes" );
#ifdef PDF_OPEN_MEM_SUPPORTED
	php_info_print_table_row(2, "In-memory PDF Creation Support", "yes" );
#else
	php_info_print_table_row(2, "In-memory PDF Creation Support", "no" );
#endif
	php_info_print_table_end();

}

PHP_MSHUTDOWN_FUNCTION(pdf){
	return SUCCESS;
}

/* {{{ proto bool pdf_set_info(int pdfdoc, string fieldname, string value)
   Fills an info field of the document */
PHP_FUNCTION(pdf_set_info) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if (!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	PDF_set_info(pdf, arg2->value.str.val, arg3->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pdf_set_info_creator(int pdfdoc, string creator)
   Fills the creator field of the document */
PHP_FUNCTION(pdf_set_info_creator) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if (!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	PDF_set_info(pdf, "Creator", arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pdf_set_info_title(int pdfdoc, string title)
   Fills the title field of the document */
PHP_FUNCTION(pdf_set_info_title) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if (!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	PDF_set_info(pdf, "Title", arg2->value.str.val);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool pdf_set_info_subject(int pdfdoc, string subject)
   Fills the subject field of the document */
PHP_FUNCTION(pdf_set_info_subject) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if (!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	PDF_set_info(pdf, "Subject", arg2->value.str.val);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool pdf_set_info_author(int pdfdoc, string author)
   Fills the author field of the document */
PHP_FUNCTION(pdf_set_info_author) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if (!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	PDF_set_info(pdf, "Author", arg2->value.str.val);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool pdf_set_info_keywords(int pdfdoc, string keywords)
   Fills the keywords field of the document */
PHP_FUNCTION(pdf_set_info_keywords) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if (!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	PDF_set_info(pdf, "Keywords", arg2->value.str.val);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int pdf_open(int filedesc)
   Opens a new pdf document */
PHP_FUNCTION(pdf_open) {
	pval **file;
	int id;
	FILE *fp;
	PDF *pdf;
	int argc;
	PDF_TLS_VARS;

	argc = ZEND_NUM_ARGS();
	if(argc > 1)
		WRONG_PARAM_COUNT;
	if (argc != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
#if defined PDF_OPEN_MEM_SUPPORTED
		fp = NULL;
#else
		php3_error(E_WARNING, "Your version of pdflib does not support in memory creation of PDF documents. You have to pass a file handle to pdf_open()");
		WRONG_PARAM_COUNT;
#endif
	} else {
		ZEND_FETCH_RESOURCE(fp, FILE *, file, -1, "File-Handle", php_file_le_fopen());
		/* XXX should do anzend_list_addref for <fp> here! */
	}

	pdf = PDF_new2(custom_errorhandler, pdf_emalloc, pdf_realloc, pdf_efree, NULL);

	if(fp) {
		if (0 > PDF_open_fp(pdf, fp))
			RETURN_FALSE;
	} else {
		PDF_open_mem(pdf, pdf_flushwrite);
	}

	id = zend_list_insert(pdf,PDF_GLOBAL(le_pdf));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto void pdf_close(int pdfdoc)
   Closes the pdf document */
PHP_FUNCTION(pdf_close) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	zend_list_delete(id);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_begin_page(int pdfdoc, double width, double height)
   Starts page */
PHP_FUNCTION(pdf_begin_page) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	double height, width;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	width = arg2->value.dval;
	height = arg3->value.dval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_begin_page(pdf, (float) width, (float) height);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_end_page(int pdfdoc)
   Ends page */
PHP_FUNCTION(pdf_end_page) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_end_page(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_show(int pdfdoc, string text)
   Output text at current position */
PHP_FUNCTION(pdf_show) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_show(pdf, arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_show_xy(int pdfdoc, string text, double x-koor, double y-koor)
   Output text at position */
PHP_FUNCTION(pdf_show_xy) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_show_xy(pdf, arg2->value.str.val, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_show_boxed(int pdfdoc, string text, double x-koor, double y-koor, double width, double height, string mode)
   Output text formated in a boxed */
PHP_FUNCTION(pdf_show_boxed) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7;
	int id, type;
	int nr;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 7 || getParameters(ht, 7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	convert_to_double(arg6);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	nr = PDF_show_boxed(pdf, arg2->value.str.val, (float) arg3->value.dval, (float) arg4->value.dval, (float) arg5->value.dval, (float) arg6->value.dval, arg7->value.str.val, NULL);

	RETURN_LONG(nr);
}
/* }}} */


/* {{{ proto void pdf_set_font(int pdfdoc, string font, double size, string encoding [, int embed])
   Select the current font face, size and encoding */
PHP_FUNCTION(pdf_set_font) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5;
	int id, type, font, embed;
	PDF *pdf;
	PDF_TLS_VARS;

	switch (ZEND_NUM_ARGS()) {
	case 4:
		if (getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		embed = 0;
		break;
	case 5:
		if (getParameters(ht, 5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg5);
		embed = arg5->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	convert_to_string(arg4);

	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	font = PDF_findfont(pdf, arg2->value.str.val, arg4->value.str.val, embed);
	if (font < 0) {
		php_error(E_WARNING,"Font %s not found", arg2->value.str.val);
		RETURN_FALSE;
	}

	PDF_setfont(pdf, font, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_get_font(int pdfdoc)
   Gets the current font */
PHP_FUNCTION(pdf_get_font) {
	pval *arg1;
	int id, type, font;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	font = (int) PDF_get_value(pdf, "font", 0);

	RETURN_LONG(font);
}
/* }}} */

/* {{{ proto string pdf_get_fontname(int pdfdoc)
   Gets the current font name */
PHP_FUNCTION(pdf_get_fontname) {
	pval *arg1;
	int id, type;
	char *fontname;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	fontname = (char *) PDF_get_parameter(pdf, "fontname", 0);

	RETURN_STRING(fontname, 1);
}
/* }}} */

/* {{{ proto double pdf_get_fontsize(int pdfdoc)
   Gets the current font size */
PHP_FUNCTION(pdf_get_fontsize) {
	pval *arg1;
	int id, type;
	float fontsize;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	fontsize = PDF_get_value(pdf, "fontsize", 0);

	RETURN_DOUBLE(fontsize);
}
/* }}} */

/* {{{ proto void pdf_set_leading(int pdfdoc, double distance)
   Sets distance between text lines */
PHP_FUNCTION(pdf_set_leading) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_leading(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_text_rendering(int pdfdoc, int mode)
   Determines how text is rendered */
PHP_FUNCTION(pdf_set_text_rendering) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_text_rendering(pdf, arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_horiz_scaling(int pdfdoc, double scale)
   Sets horizontal scaling of text */
PHP_FUNCTION(pdf_set_horiz_scaling) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_horiz_scaling(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_text_rise(int pdfdoc, double value)
   Sets the text rise */
PHP_FUNCTION(pdf_set_text_rise) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_text_rise(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

#if 0
/* {{{ proto void pdf_set_text_matrix(int pdfdoc, arry matrix)
   Sets the text matrix */
PHP_FUNCTION(pdf_set_text_matrix) {
	pval *arg1, *arg2, *data;
	int id, type, i;
	HashTable *matrix;
	PDF *pdf;
	float pdfmatrix[6];
	float *pdfmatrixptr;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_array(arg2);
	id=arg1->value.lval;
	matrix=arg2->value.ht;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	if(zend_hash_num_elements(matrix) != 6) {
		 php_error(E_WARNING,"Text matrix must have 6 elements");
		RETURN_FALSE;
	}

	pdfmatrixptr = (float *) &pdfmatrix;
	zend_hash_internal_pointer_reset(matrix);
	for(i=0; i<zend_hash_num_elements(matrix); i++) {
		zend_hash_get_current_data(matrix, (void *) &data);
		switch(data->type) {
			case IS_DOUBLE:
				*pdfmatrixptr++ = (float) data->value.dval;
				break;
			default:
				*pdfmatrixptr++ = 0.0;
				break;
		}
		zend_hash_move_forward(matrix);
	}

	PDF_set_text_matrix(pdf, pdfmatrix[0], pdfmatrix[1], pdfmatrix[2],
	                         pdfmatrix[3], pdfmatrix[4], pdfmatrix[5]);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void pdf_set_text_pos(int pdfdoc, double x, double y)
   Set the position of text for the next pdf_show call */
PHP_FUNCTION(pdf_set_text_pos) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_text_pos(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_char_spacing(int pdfdoc, double space)
   Sets character spacing */
PHP_FUNCTION(pdf_set_char_spacing) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_char_spacing(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_word_spacing(int pdfdoc, double space)
   Sets spacing between words */
PHP_FUNCTION(pdf_set_word_spacing) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_word_spacing(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_continue_text(int pdfdoc, string text)
   Output text in next line */
PHP_FUNCTION(pdf_continue_text) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_continue_text(pdf, arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double pdf_stringwidth(int pdfdoc, string text)
   Returns width of text in current font*/
PHP_FUNCTION(pdf_stringwidth) {
	pval *arg1, *arg2;
	int id, type;
	double width;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	width = (double) PDF_stringwidth(pdf, arg2->value.str.val, PDF_get_font(pdf), PDF_get_fontsize(pdf));

	RETURN_DOUBLE((double)width);
}
/* }}} */

/* {{{ proto void pdf_save(int pdfdoc)
   Saves current enviroment */
PHP_FUNCTION(pdf_save) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_save(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_restore(int pdfdoc)
   Restores formerly saved enviroment */
PHP_FUNCTION(pdf_restore) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_restore(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_translate(int pdfdoc, double x, double y)
   Sets origin of coordinate system */
PHP_FUNCTION(pdf_translate) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_translate(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_scale(int pdfdoc, double x-scale, double y-scale)
   Sets scaling */
PHP_FUNCTION(pdf_scale) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_scale(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_rotate(int pdfdoc, double angle)
   Sets rotation */
PHP_FUNCTION(pdf_rotate) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_rotate(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_skew(int pdfdoc, double xangle, double yangle)
   Skew the coordinate system */
PHP_FUNCTION(pdf_skew) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_skew(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setflat(int pdfdoc, double value)
   Sets flatness */
PHP_FUNCTION(pdf_setflat) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 100) && (arg2->value.lval < 0)) {
		php_error(E_WARNING,"Parameter of pdf_setflat() has to between 0 and 100");
		RETURN_FALSE;
	}

	PDF_setflat(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinejoin(int pdfdoc, int value)
   Sets linejoin parameter */
PHP_FUNCTION(pdf_setlinejoin) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 2) && (arg2->value.lval < 0)) {
		php_error(E_WARNING,"Parameter of pdf_setlinejoin() has to between 0 and 2");
		RETURN_FALSE;
	}

	PDF_setlinejoin(pdf, arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinecap(int pdfdoc, int value)
   Sets linecap parameter */
PHP_FUNCTION(pdf_setlinecap) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 2) && (arg2->value.lval < 0)) {
		php_error(E_WARNING,"Parameter of pdf_setlinecap() has to be > 0 and =< 2");
		RETURN_FALSE;
	}

	PDF_setlinecap(pdf, arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setmiterlimit(int pdfdoc, double value)
   Sets miter limit */
PHP_FUNCTION(pdf_setmiterlimit) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if(arg2->value.dval < 1) {
		php_error(E_WARNING,"Parameter of pdf_setmiterlimit() has to be >= 1");
		RETURN_FALSE;
	}

	PDF_setmiterlimit(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinewidth(int pdfdoc, double width)
   Sets line width */
PHP_FUNCTION(pdf_setlinewidth) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setlinewidth(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setdash(int pdfdoc, double white, double black)
   Sets dash pattern */
PHP_FUNCTION(pdf_setdash) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setdash(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_moveto(int pdfdoc, double x, double y)
   Sets current point */
PHP_FUNCTION(pdf_moveto) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_moveto(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_curveto(int pdfdoc, double x1, double y1, double x2, double y2, double x3, double y3)
   Draws a curve */
PHP_FUNCTION(pdf_curveto) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 7 || getParameters(ht, 7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	convert_to_double(arg6);
	convert_to_double(arg7);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_curveto(pdf, (float) arg2->value.dval,
	                (float) arg3->value.dval,
	                (float) arg4->value.dval,
	                (float) arg5->value.dval,
	                (float) arg6->value.dval,
	                (float) arg7->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_lineto(int pdfdoc, double x, double y)
   Draws a line */
PHP_FUNCTION(pdf_lineto) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_lineto(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_circle(int pdfdoc, double x, double y, double radius)
   Draws a circle */
PHP_FUNCTION(pdf_circle) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_circle(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_arc(int pdfdoc, double x, double y, double radius, double start, double end)
   Draws an arc */
PHP_FUNCTION(pdf_arc) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 6 || getParameters(ht, 6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	convert_to_double(arg6);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_arc(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval, (float) arg5->value.dval, (float) arg6->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_rect(int pdfdoc, double x, double y, double width, double height)
   Draws a rectangle */
PHP_FUNCTION(pdf_rect) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || getParameters(ht, 5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_rect(pdf, (float) arg2->value.dval,
	                (float) arg3->value.dval,
	                (float) arg4->value.dval,
	                (float) arg5->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath(int pdfdoc)
   Close path */
PHP_FUNCTION(pdf_closepath) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_closepath(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_stroke(int pdfdoc)
   Close path and draw line along path */
PHP_FUNCTION(pdf_closepath_stroke) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_closepath_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_stroke(int pdfdoc)
   Draw line along path path */
PHP_FUNCTION(pdf_stroke) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_fill(int pdfdoc)
   Fill current path */
PHP_FUNCTION(pdf_fill) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_fill(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_fill_stroke(int pdfdoc)
   Fill and stroke current path */
PHP_FUNCTION(pdf_fill_stroke) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_fill_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_fill_stroke(int pdfdoc)
   Close, fill and stroke current path */
PHP_FUNCTION(pdf_closepath_fill_stroke) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_closepath_fill_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_endpath(int pdfdoc)
   Ends current path */
PHP_FUNCTION(pdf_endpath) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_endpath(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_clip(int pdfdoc)
   Clips to current path */
PHP_FUNCTION(pdf_clip) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_clip(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_parameter(int pdfdoc, string key, string value)
   Sets arbitrary parameters */
PHP_FUNCTION(pdf_set_parameter) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_parameter(pdf, arg2->value.str.val, arg3->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string pdf_get_parameter(int pdfdoc, string key, double modifier)
   Gets arbitrary parameters */
PHP_FUNCTION(pdf_get_parameter) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	char *value;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	value = PDF_get_parameter(pdf, arg2->value.str.val, (float) (arg3->value.dval));

	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto void pdf_set_value(int pdfdoc, string key, double value)
   Sets arbitrary value */
PHP_FUNCTION(pdf_set_value) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_value(pdf, arg2->value.str.val, arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double pdf_get_value(int pdfdoc, string key, double modifier)
   Gets arbitrary value */
PHP_FUNCTION(pdf_get_value) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	double value;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	value = PDF_get_value(pdf, arg2->value.str.val, arg3->value.dval);

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ proto void pdf_setgray_fill(int pdfdoc, double value)
   Sets filling color to gray value */
PHP_FUNCTION(pdf_setgray_fill) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setgray_fill(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray_stroke(int pdfdoc, double value)
   Sets drawing color to gray value */
PHP_FUNCTION(pdf_setgray_stroke) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setgray_stroke(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray(int pdfdoc, double value)
   Sets drawing and filling color to gray value */
PHP_FUNCTION(pdf_setgray) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setgray(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor_fill(int pdfdoc, double red, double green, double blue)
   Sets filling color to rgb color value */
PHP_FUNCTION(pdf_setrgbcolor_fill) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setrgbcolor_fill(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor_stroke(int pdfdoc, double red, double green, double blue)
   Sets drawing color to rgb color value */
PHP_FUNCTION(pdf_setrgbcolor_stroke) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setrgbcolor_stroke(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor(int pdfdoc, double red, double green, double blue)
   Sets drawing and filling color to rgb color value */
PHP_FUNCTION(pdf_setrgbcolor) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setrgbcolor(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_add_outline(int pdfdoc, string text [, int parent, int open]);
   Add bookmark for current page */
PHP_FUNCTION(pdf_add_outline) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	int *outline, *parent, parentid, open;
	PDF *pdf;
	PDF_TLS_VARS;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);

	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find document identifier %d",id);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() > 2) {
		convert_to_long(arg3);
		id = arg3->value.lval;

		if (id > 0) {
			parent = zend_list_find(id, &type);
			if (!parent || (type != PDF_GLOBAL(le_outline))) {
				php_error(E_WARNING,"Unable to find identifier %d",id);
				RETURN_FALSE;
			} else {
				parentid = *parent;
			}
		} else {
			parentid = 0;
		}

		if (ZEND_NUM_ARGS() > 3) {
			convert_to_long(arg4);
			open = arg4->value.lval;
		} else {
			open = 0;
		}
	} else {
		parentid = 0;
		open = 0;
	}

	outline=emalloc(sizeof(int));
	*outline = PDF_add_bookmark(pdf, arg2->value.str.val, parentid, open);
	id = zend_list_insert(outline,PDF_GLOBAL(le_outline));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto void pdf_set_transition(int pdfdoc, int transition)
   Sets transition between pages */
PHP_FUNCTION(pdf_set_transition) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	switch(arg2->value.lval) {
		case 0:
			PDF_set_transition(pdf, "none");
			break;
		case 1:
			PDF_set_transition(pdf, "split");
			break;
		case 2:
			PDF_set_transition(pdf, "blinds");
			break;
		case 3:
			PDF_set_transition(pdf, "box");
			break;
		case 4:
			PDF_set_transition(pdf, "wipe");
			break;
		case 5:
			PDF_set_transition(pdf, "dissolve");
			break;
		case 6:
			PDF_set_transition(pdf, "glitter");
			break;
		case 7:
			PDF_set_transition(pdf, "replace");
			break;
		default:
			PDF_set_transition(pdf, "none");
			
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_duration(int pdfdoc, double duration)
   Sets duration between pages */
PHP_FUNCTION(pdf_set_duration) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_duration(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_open_gif(int pdf, string giffile)
   Opens a gif file and returns an image for placement in a pdf document */
PHP_FUNCTION(pdf_open_gif) {
	pval *arg1, *arg2;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	pdf_image = PDF_open_image_file(pdf, "gif", arg2->value.str.val, "", 0);

	if(pdf_image < 0) {
		php_error(E_WARNING, "Could not open image");
		RETURN_FALSE;
	}

	id = zend_list_insert((void *) pdf_image,PDF_GLOBAL(le_pdf_image));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int pdf_open_jpeg(int pdf, string jpegfile)
   Opens a jpeg file and returns an image for placement in a pdf document */
PHP_FUNCTION(pdf_open_jpeg) {
	pval *arg1, *arg2;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	pdf_image = PDF_open_image_file(pdf, "jpeg", arg2->value.str.val, "", 0);

	if(pdf_image < 0) {
		php_error(E_WARNING, "Could not open image");
		RETURN_FALSE;
	}

	id = zend_list_insert((void *) pdf_image,PDF_GLOBAL(le_pdf_image));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int pdf_open_png(int pdf, string pngfile)
   Opens a png file and returns an image for placement in a pdf document */
PHP_FUNCTION(pdf_open_png) {
	pval *arg1, *arg2;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	pdf_image = PDF_open_image_file(pdf, "png", arg2->value.str.val, "", 0);

	if(pdf_image < 0) {
		php_error(E_WARNING, "Could not open image");
		RETURN_FALSE;
	}

	id = zend_list_insert((void *) pdf_image,PDF_GLOBAL(le_pdf_image));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int pdf_open_tiff(int pdf, string tifffile)
   Opens a tiff file and returns an image for placement in a pdf document */
PHP_FUNCTION(pdf_open_tiff) {
	pval *arg1, *arg2;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	pdf_image = PDF_open_image_file(pdf, "tiff", arg2->value.str.val, "", 0);

	if(pdf_image < 0) {
		php_error(E_WARNING, "Could not open image");
		RETURN_FALSE;
	}

	id = zend_list_insert((void *) pdf_image,PDF_GLOBAL(le_pdf_image));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int pdf_open_image_file(int pdf, string type, string file)
   Opens an image file of the given type and returns an image for placement in a pdf document */
PHP_FUNCTION(pdf_open_image_file) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	pdf_image = PDF_open_image_file(pdf, arg2->value.str.val, arg3->value.str.val, "", 0);

	if(pdf_image < 0) {
		php_error(E_WARNING, "Could not open image");
		RETURN_FALSE;
	}

	id = zend_list_insert((void *) pdf_image,PDF_GLOBAL(le_pdf_image));
	RETURN_LONG(id);
}
/* }}} */

#if HAVE_LIBGD13
/* {{{ proto int pdf_open_memory_image(int pdf, int image)
   Takes an gd image and returns an image for placement in a pdf document */
PHP_FUNCTION(pdf_open_memory_image) {
	pval *argv[2];
	int argc;
	int i, j, id, gid, type, color, count;
	int pdf_image;
	gdImagePtr im;
	unsigned char *buffer, *ptr;
	PDF *pdf;
	PDF_TLS_VARS;

	argc = ZEND_NUM_ARGS();
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	id=argv[0]->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	convert_to_long(argv[1]);
	gid=argv[1]->value.lval;
	im = zend_list_find(gid, &type);
	if (!im || type != phpi_get_le_gd()) {
		php_error(E_WARNING, "pdf: Unable to find image pointer");
		RETURN_FALSE;
	}

	count = 3 * im->sx * im->sy;
	if(NULL == (buffer = (unsigned char *) emalloc(count)))
		RETURN_FALSE;

	ptr = buffer;
	for(i=0; i<im->sy; i++) {
		for(j=0; j<im->sx; j++) {
			color = im->pixels[i][j];
			*ptr++ = im->red[color];
			*ptr++ = im->green[color];
			*ptr++ = im->blue[color];
		}
	}

	pdf_image = PDF_open_image(pdf, "raw", "memory", buffer, im->sx*im->sy*3, im->sx, im->sy, 3, 8, NULL);
	efree(buffer);

	if(-1 == pdf_image) {
		php_error(E_WARNING, "Could not open image");
		efree(buffer);
		RETURN_FALSE;
	}

	id = zend_list_insert((void *) pdf_image,PDF_GLOBAL(le_pdf_image));
	RETURN_LONG(id);
}
/* }}} */
#endif /* HAVE_LIBGD13 */

/* {{{ proto void pdf_close_image(int pdfimage)
   Closes the pdf image */
PHP_FUNCTION(pdf_close_image) {
	pval *arg1, *arg2;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	convert_to_long(arg2);
	id=arg2->value.lval;
	pdf_image = (int) zend_list_find(id,&type);
	if(pdf_image < 0 || type!=PDF_GLOBAL(le_pdf_image)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_close_image(pdf, pdf_image);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_place_image(int pdf, int pdfimage, double x, double y, double scale)
   Places image in the pdf document */
PHP_FUNCTION(pdf_place_image) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5;
	int id, type;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 5 || getParameters(ht, 5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	convert_to_long(arg2);
	id=arg2->value.lval;
	pdf_image = (int) zend_list_find(id,&type);
	if(pdf_image < 0 || type!=PDF_GLOBAL(le_pdf_image)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);

	PDF_place_image(pdf, pdf_image, (float) arg3->value.dval, (float) arg4->value.dval, arg5->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_get_image_width(int pdf, int pdfimage)
   Returns the width of an image */
PHP_FUNCTION(pdf_get_image_width) {
	pval *arg1, *arg2;
	int id, type;
	int width;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	convert_to_long(arg2);
	id=arg2->value.lval;
	pdf_image = (int) zend_list_find(id,&type);
	if(pdf_image < 0 || type!=PDF_GLOBAL(le_pdf_image)) {
		php_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	width = PDF_get_image_width(pdf, pdf_image);

	RETURN_LONG(width);
}
/* }}} */

/* {{{ proto void pdf_get_image_height(int pdf, int pdfimage)
   Returns the height of an image */
PHP_FUNCTION(pdf_get_image_height) {
	pval *arg1, *arg2;
	int id, type;
	int height;
	int pdf_image;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	convert_to_long(arg2);
	id=arg2->value.lval;
	pdf_image = (int) zend_list_find(id,&type);
	if(pdf_image < 0 || type!=PDF_GLOBAL(le_pdf_image)) {
		php_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	height = PDF_get_image_height(pdf, pdf_image);

	RETURN_LONG(height);
}
/* }}} */

/* {{{ proto void pdf_add_weblink(int pdfdoc, double llx, double lly, double urx, double ury, string url)
   Adds link to web resource */
PHP_FUNCTION(pdf_add_weblink) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 6 || getParameters(ht, 6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	convert_to_string(arg6);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_add_weblink(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval, (float) arg5->value.dval, arg6->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_pdflink(int pdfdoc, double llx, double lly, double urx, double ury, string filename, int page, string dest)
   Adds link to pdf document */
PHP_FUNCTION(pdf_add_pdflink) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 8 || getParameters(ht, 8, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	convert_to_string(arg6);
	convert_to_long(arg7);
	convert_to_string(arg8);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_add_pdflink(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval, (float) arg5->value.dval, arg6->value.str.val, arg7->value.lval, arg8->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_border_style(int pdfdoc, string style, double width)
   Set style of box surounding all kinds of annotations and link */
PHP_FUNCTION(pdf_set_border_style) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_border_style(pdf, arg2->value.str.val, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_border_color(int pdfdoc, double red, double green, double blue)
   Set color of box surounded all kinds of annotations and links */
PHP_FUNCTION(pdf_set_border_color) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_border_color(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_border_dash(int pdfdoc, double black, double white)
   Set the border dash style of all kinds of annotations and links */
PHP_FUNCTION(pdf_set_border_dash) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_border_dash(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_annotation(int pdfdoc, double xll, double yll, double xur, double xur, string title, string text)
   Sets annotation */
PHP_FUNCTION(pdf_add_annotation) {
	pval *argv[11];
	int id, type, argc;
	PDF *pdf;
	PDF_TLS_VARS;

	argc = ZEND_NUM_ARGS();
	if(argc != 7)
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_string(argv[5]);
	convert_to_string(argv[6]);
	id=argv[0]->value.lval;
	pdf = zend_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	PDF_add_note(pdf,
	             (float) argv[1]->value.dval,
	             (float) argv[2]->value.dval,
	             (float) argv[3]->value.dval,
	             (float) argv[4]->value.dval,
	             argv[6]->value.str.val,
	             argv[5]->value.str.val,
	             "note", 1);

	RETURN_TRUE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
