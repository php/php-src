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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* pdflib 0.6 is subject to the ALADDIN FREE PUBLIC LICENSE.
   Copyright (C) 1997 Thomas Merz. */

/* Note that there is no code from the pdflib package in this file */

#if !PHP_31 && defined(THREAD_SAFE)
#undef THREAD_SAFE
#endif

#include "php.h"
#include "ext/standard/head.h"
#include <math.h>
#include "php3_pdf.h"

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if WIN32|WINNT
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_PDFLIB

#ifdef THREAD_SAFE
DWORD PDFlibTls;
static int numthreads=0;

typedef struct pdflib_global_struct{
	int le_pdf_info;
	int le_pdf;
} pdflib_global_struct;

# define PDF_GLOBAL(a) pdflib_globals->a
# define PDF_TLS_VARS pdflib_global_struct *pdflib_globals=TlsGetValue(PDFlibTls)

#else
#  define PDF_GLOBAL(a) a
#  define PDF_TLS_VARS
int le_pdf_info;
int le_pdf;
#endif

function_entry pdf_functions[] = {
	{"pdf_get_info",			php3_pdf_get_info,			NULL},
	{"pdf_set_info_creator",	php3_pdf_set_info_creator,	NULL},
	{"pdf_set_info_title",		php3_pdf_set_info_title,	NULL},
	{"pdf_set_info_subject",	php3_pdf_set_info_subject,	NULL},
	{"pdf_set_info_author",		php3_pdf_set_info_author,	NULL},
	{"pdf_set_info_keywords",	php3_pdf_set_info_keywords,	NULL},
	{"pdf_open",				php3_pdf_open,				NULL},
	{"pdf_close",				php3_pdf_close,				NULL},
	{"pdf_begin_page",			php3_pdf_begin_page,		NULL},
	{"pdf_end_page",			php3_pdf_end_page,			NULL},
	{"pdf_show",				php3_pdf_show,				NULL},
	{"pdf_show_xy",				php3_pdf_show_xy,			NULL},
	{"pdf_set_font",			php3_pdf_set_font,			NULL},
	{"pdf_set_leading",			php3_pdf_set_leading,		NULL},
	{"pdf_set_text_rendering",	php3_pdf_set_text_rendering,NULL},
	{"pdf_set_horiz_scaling",	php3_pdf_set_horiz_scaling,	NULL},
	{"pdf_set_text_rise",		php3_pdf_set_text_rise,		NULL},
	{"pdf_set_text_matrix",		php3_pdf_set_text_matrix,	NULL},
	{"pdf_set_text_pos",		php3_pdf_set_text_pos,		NULL},
	{"pdf_set_char_spacing",		php3_pdf_set_char_spacing,		NULL},
	{"pdf_set_word_spacing",		php3_pdf_set_word_spacing,		NULL},
	{"pdf_continue_text",		php3_pdf_continue_text,		NULL},
	{"pdf_stringwidth",		php3_pdf_stringwidth,		NULL},
	{"pdf_save",				php3_pdf_save,				NULL},
	{"pdf_restore",				php3_pdf_restore,			NULL},
	{"pdf_translate",			php3_pdf_translate,			NULL},
	{"pdf_scale",				php3_pdf_scale,				NULL},
	{"pdf_rotate",				php3_pdf_rotate,			NULL},
	{"pdf_setflat",				php3_pdf_setflat,			NULL},
	{"pdf_setlinejoin",			php3_pdf_setlinejoin,		NULL},
	{"pdf_setlinecap",			php3_pdf_setlinecap,		NULL},
	{"pdf_setmiterlimit",		php3_pdf_setmiterlimit,		NULL},
	{"pdf_setlinewidth",		php3_pdf_setlinewidth,		NULL},
	{"pdf_setdash",				php3_pdf_setdash,			NULL},
	{"pdf_moveto",				php3_pdf_moveto,			NULL},
	{"pdf_lineto",				php3_pdf_lineto,			NULL},
	{"pdf_curveto",				php3_pdf_curveto,			NULL},
	{"pdf_circle",				php3_pdf_circle,			NULL},
	{"pdf_arc",					php3_pdf_arc,				NULL},
	{"pdf_rect",				php3_pdf_rect,				NULL},
	{"pdf_closepath",			php3_pdf_closepath,			NULL},
	{"pdf_stroke",				php3_pdf_stroke,			NULL},
	{"pdf_closepath_stroke",	php3_pdf_closepath_stroke,	NULL},
	{"pdf_fill",				php3_pdf_fill,  			NULL},
	{"pdf_fill_stroke",			php3_pdf_fill_stroke,  		NULL},
	{"pdf_closepath_fill_stroke",	php3_pdf_closepath_fill_stroke, NULL},
	{"pdf_endpath",				php3_pdf_endpath,			NULL},
	{"pdf_clip",				php3_pdf_clip,				NULL},
	{"pdf_setgray_fill",		php3_pdf_setgray_fill,		NULL},
	{"pdf_setgray_stroke",		php3_pdf_setgray_stroke,	NULL},
	{"pdf_setgray",				php3_pdf_setgray,			NULL},
	{"pdf_setrgbcolor_fill",	php3_pdf_setrgbcolor_fill,	NULL},
	{"pdf_setrgbcolor_stroke",	php3_pdf_setrgbcolor_stroke,NULL},
	{"pdf_setrgbcolor",			php3_pdf_setrgbcolor,		NULL},
	{"pdf_add_outline",			php3_pdf_add_outline,		NULL},
	{"pdf_set_transition",			php3_pdf_set_transition,		NULL},
	{"pdf_set_duration",			php3_pdf_set_duration,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry pdf_module_entry = {
	"pdf", pdf_functions, php3_minit_pdf, php3_mend_pdf, NULL, NULL, php3_info_pdf, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
#include "dl/phpdl.h"
DLEXPORT php3_module_entry *get_module(void) { return &pdf_module_entry; }
#endif

static void _free_pdf_info(PDF_info *info)
{
	if(info->Title) efree(info->Title);
	if(info->Subject) efree(info->Subject);
	if(info->Author) efree(info->Author);
	if(info->Keywords) efree(info->Keywords);
	if(info->Creator) efree(info->Creator);
}

int php3_minit_pdf(INIT_FUNC_ARGS)
{
	PDF_GLOBAL(le_pdf_info) = register_list_destructors(_free_pdf_info, NULL);
	PDF_GLOBAL(le_pdf) = register_list_destructors(php3_pdf_close, NULL);
	return SUCCESS;
}

void php3_info_pdf(ZEND_MODULE_INFO_FUNC_ARGS) {
	/* need to use a PHPAPI function here because it is external module in windows */
	php3_printf("%s. AFM files in %s", PDFLIB_VERSION, PDF_DEFAULT_FONT_PATH);
}

int php3_mend_pdf(void){
	return SUCCESS;
}

/* {{{ proto int pdf_get_info(void)
   Returns a default info structure for a pdf document */
void php3_pdf_get_info(INTERNAL_FUNCTION_PARAMETERS) {
	PDF_info *pdf_info;
	int id;
	PDF_TLS_VARS;

	pdf_info = PDF_get_info();

	if(!pdf_info) {
		php3_error(E_WARNING, "Could not get PDF info");
		RETURN_FALSE;
	}

	id = php3_list_insert(pdf_info,PDF_GLOBAL(le_pdf_info));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto pdf_set_info_creator(int info, string creator)
   Fills the creator field of the info structure */
void php3_pdf_set_info_creator(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF_info *pdf_info;
	PDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf_info = php3_list_find(id,&type);
	if (!pdf_info || type!=PDF_GLOBAL(le_pdf_info)) {
		php3_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	pdf_info->Creator = estrdup(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto pdf_set_info_title(int info, string title)
   Fills the title field of the info structure */
void php3_pdf_set_info_title(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF_info *pdf_info;
	PDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf_info = php3_list_find(id,&type);
	if (!pdf_info || type!=PDF_GLOBAL(le_pdf_info)) {
		php3_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	pdf_info->Title = estrdup(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto pdf_set_info_subject(int info, string subject)
   Fills the subject field of the info structure */
void php3_pdf_set_info_subject(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF_info *pdf_info;
	PDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf_info = php3_list_find(id,&type);
	if (!pdf_info || type!=PDF_GLOBAL(le_pdf_info)) {
		php3_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	pdf_info->Subject = estrdup(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto pdf_set_info_author(int info, string author)
   Fills the author field of the info structure */
void php3_pdf_set_info_author(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF_info *pdf_info;
	PDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf_info = php3_list_find(id,&type);
	if (!pdf_info || type!=PDF_GLOBAL(le_pdf_info)) {
		php3_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	pdf_info->Author = estrdup(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto pdf_set_info_keywords(int info, string keywords)
   Fills the keywords field of the info structure */
void php3_pdf_set_info_keywords(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF_info *pdf_info;
	PDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf_info = php3_list_find(id,&type);
	if (!pdf_info || type!=PDF_GLOBAL(le_pdf_info)) {
		php3_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	pdf_info->Keywords = estrdup(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_open(int filedesc, int info)
   Opens a new pdf document */
void php3_pdf_open(INTERNAL_FUNCTION_PARAMETERS) {
	pval *file;
	pval *info;
	int id, type;
	FILE *fp;
	PDF_info *pdf_info;
	PDF *pdf;
	PDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &file, &info) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(file);
	convert_to_long(info);
	id=file->value.lval;
	fp = php3_list_find(id,&type);
	if (!fp || type!=php3i_get_le_fp()) {
		php3_error(E_WARNING,"Unable to find file identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	id=info->value.lval;
	pdf_info = php3_list_find(id,&type);
	if (!pdf_info || type!=PDF_GLOBAL(le_pdf_info)) {
		php3_error(E_WARNING,"Unable to find pdf info identifier %d (%d!=%d)",id, type, PDF_GLOBAL(le_pdf_info));
		RETURN_FALSE;
	}

	pdf = PDF_open(fp, pdf_info);
	if(!pdf)
		RETURN_FALSE;

	id = php3_list_insert(pdf,PDF_GLOBAL(le_pdf));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto void pdf_close(int pdfdoc)
   Closes the pdf document */
void php3_pdf_close(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_close(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_begin_page(int pdfdoc, double height, double width)
   Starts page */
void php3_pdf_begin_page(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	double height, width;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	height = arg2->value.dval;
	width = arg3->value.dval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_begin_page(pdf, (float) height, (float) width);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_end_page(int pdfdoc)
   Ends page */
void php3_pdf_end_page(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_end_page(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_show(int pdfdoc, string text)
   Output text at current position */
void php3_pdf_show(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_show(pdf, arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_show_xy(int pdfdoc, string text)
   Output text at position */
void php3_pdf_show_xy(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_show_xy(pdf, arg2->value.str.val, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_font(int pdfdoc, string font, double size, string encoding)
   Select the current font face and size */
void php3_pdf_set_font(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	convert_to_string(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_font(pdf, arg2->value.str.val, (float) arg3->value.dval, builtin);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_leading(int pdfdoc, double distance)
   Sets distance between text lines */
void php3_pdf_set_leading(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_leading(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_text_rendering(int pdfdoc, int mode)
   Determines how text is rendered */
void php3_pdf_set_text_rendering(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_text_rendering(pdf, (byte) arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_horiz_scaling(int pdfdoc, double scale)
   Sets horizontal scaling of text */
void php3_pdf_set_horiz_scaling(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_horiz_scaling(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_text_rise(int pdfdoc, double value)
   Sets the text rise */
void php3_pdf_set_text_rise(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	PDF_set_text_rise(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_text_matrix(int pdfdoc, arry matrix)
   Sets the text matrix */
void php3_pdf_set_text_matrix(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *data;
	int id, type, i;
	HashTable *matrix;
	PDF *pdf;
	PDF_matrix pdfmatrix;
	float *pdfmatrixptr;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_array(arg2);
	id=arg1->value.lval;
	matrix=arg2->value.ht;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	
	if(_php3_hash_num_elements(matrix) != 6) {
		 php3_error(E_WARNING,"Text matrix must have 6 elements");
		RETURN_FALSE;
	}

	pdfmatrixptr = (float *) &pdfmatrix;
	_php3_hash_internal_pointer_reset(matrix);
	for(i=0; i<_php3_hash_num_elements(matrix); i++) {
		_php3_hash_get_current_data(matrix, (void *) &data);
		switch(data->type) {
			case IS_DOUBLE:
				*pdfmatrixptr++ = (float) data->value.dval;
			default:
				*pdfmatrixptr++ = 0.0;
		}
		_php3_hash_move_forward(matrix);
	}

	PDF_set_text_matrix(pdf, pdfmatrix);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_text_pos(int pdfdoc, double x, double y)
   */
void php3_pdf_set_text_pos(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_text_pos(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_char_spacing(int pdfdoc, double space)
   Sets character spacing */
void php3_pdf_set_char_spacing(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_char_spacing(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_word_spacing(int pdfdoc, double space)
   Sets spacing between words */
void php3_pdf_set_word_spacing(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_word_spacing(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_continue_text(int pdfdoc, string text)
   Output text in next line */
void php3_pdf_continue_text(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_continue_text(pdf, arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double pdf_stringwidth(int pdfdoc, string text)
   Returns width of text in current font*/
void php3_pdf_stringwidth(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	double width;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	width = (double) PDF_stringwidth(pdf, arg2->value.str.val);

	RETURN_DOUBLE((double)width);
}
/* }}} */

/* {{{ proto void pdf_save(int pdfdoc)
   Saves current enviroment */
void php3_pdf_save(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_save(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_restore(int pdfdoc)
   Restores formerly saved enviroment */
void php3_pdf_restore(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_restore(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_translate(int pdfdoc, double x, double y)
   Sets origin of coordinate system */
void php3_pdf_translate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_translate(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_scale(int pdfdoc, double x-scale, double y-scale)
   Sets scaling */
void php3_pdf_scale(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_scale(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_rotate(int pdfdoc, double angle)
   Sets rotation */
void php3_pdf_rotate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_rotate(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setflat(int pdfdoc, double value)
   Sets flatness */
void php3_pdf_setflat(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 100) && (arg2->value.lval < 0)) {
		php3_error(E_WARNING,"Parameter of pdf_setflat() has to between 0 and 100");
		RETURN_FALSE;
	}

	PDF_setflat(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinejoin(int pdfdoc, int value)
   Sets linejoin parameter */
void php3_pdf_setlinejoin(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 2) && (arg2->value.lval < 0)) {
		php3_error(E_WARNING,"Parameter of pdf_setlinejoin() has to between 0 and 2");
		RETURN_FALSE;
	}

	PDF_setlinejoin(pdf, (byte) arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinecap(int pdfdoc, int value)
   Sets linecap parameter */
void php3_pdf_setlinecap(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 2) && (arg2->value.lval < 0)) {
		php3_error(E_WARNING,"Parameter of pdf_setlinecap() has to be > 0 and =< 2");
		RETURN_FALSE;
	}

	PDF_setlinecap(pdf, (byte) arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setmiterlimit(int pdfdoc, double value)
   Sets miter limit */
void php3_pdf_setmiterlimit(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if(arg2->value.dval < 1) {
		php3_error(E_WARNING,"Parameter of pdf_setmiterlimit() has to be >= 1");
		RETURN_FALSE;
	}

	PDF_setmiterlimit(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinewidth(int pdfdoc, double width)
   Sets line width */
void php3_pdf_setlinewidth(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setlinewidth(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setdash(int pdfdoc, double white, double black)
   Sets dash pattern */
void php3_pdf_setdash(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setdash(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_moveto(int pdfdoc, double x, double y)
   Sets current point */
void php3_pdf_moveto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_moveto(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_curveto(int pdfdoc, double x1, double y1, double x2, double y2, double x3, double y3)
   Draws a curve */
void php3_pdf_curveto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 7 || getParameters(ht, 7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
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
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
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
void php3_pdf_lineto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_lineto(pdf, (float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_circle(int pdfdoc, double x, double y, double radius)
   Draws a circle */
void php3_pdf_circle(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_circle(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_arc(int pdfdoc, double x, double y, double radius, double start, double end)
   Draws an arc */
void php3_pdf_arc(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 6 || getParameters(ht, 6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	convert_to_double(arg6);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_arc(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval, (float) arg5->value.dval, (float) arg6->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_rect(int pdfdoc, double x, double y, double width, double height)
   Draws a rectangle */
void php3_pdf_rect(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 5 || getParameters(ht, 5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_double(arg5);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
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
void php3_pdf_closepath(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_closepath(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_stroke(int pdfdoc)
   Close path and draw line along path */
void php3_pdf_closepath_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_closepath_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_stroke(int pdfdoc)
   Draw line along path path */
void php3_pdf_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_fill(int pdfdoc)
   Fill current path */
void php3_pdf_fill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_fill(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_fill_stroke(int pdfdoc)
   Fill and stroke current path */
void php3_pdf_fill_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_fill_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_fill_stroke(int pdfdoc)
   Close, fill and stroke current path */
void php3_pdf_closepath_fill_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_closepath_fill_stroke(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_endpath(int pdfdoc)
   Ends current path */
void php3_pdf_endpath(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_endpath(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_clip(int pdfdoc)
   Clips to current path */
void php3_pdf_clip(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_clip(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray_fill(int pdfdoc, double value)
   Sets filling color to gray value */
void php3_pdf_setgray_fill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setgray_fill(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray_stroke(int pdfdoc, double value)
   Sets drawing color to gray value */
void php3_pdf_setgray_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setgray_stroke(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray(int pdfdoc, double value)
   Sets drawing and filling color to gray value */
void php3_pdf_setgray(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setgray(pdf, (float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor_fill(int pdfdoc, double red, double green, double blue)
   Sets filling color to rgb color value */
void php3_pdf_setrgbcolor_fill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setrgbcolor_fill(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor_stroke(int pdfdoc, double red, double green, double blue)
   Sets drawing color to rgb color value */
void php3_pdf_setrgbcolor_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setrgbcolor_stroke(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor(int pdfdoc, double red, double green, double blue)
   Sets drawing and filling color to rgb color value */
void php3_pdf_setrgbcolor(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_setrgbcolor(pdf, (float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_outline(int pdfdoc, string text);
   Add bookmark for current page */
void php3_pdf_add_outline(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_add_outline(pdf, arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_transition(int pdfdoc, int transition)
   Sets transition between pages */
void php3_pdf_set_transition(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_transition(pdf, arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_duration(int pdfdoc, double duration)
   Sets duration between pages */
void php3_pdf_set_duration(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	PDF *pdf;
	PDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=PDF_GLOBAL(le_pdf)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	PDF_set_duration(pdf, (float) arg2->value.dval);

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
