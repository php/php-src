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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   |          Rainer Schaaf <rjs@pdflib.com>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* pdflib 2.02 ... 3.0x is subject to the ALADDIN FREE PUBLIC LICENSE.
   Copyright (C) 1997-1999 Thomas Merz. 2000-2001 PDFlib GmbH */
/* Note that there is no code from the pdflib package in this file */

/* {{{ includes
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "zend_list.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "php_streams.h"

#if HAVE_LIBGD13
#include "ext/gd/php_gd.h"
#if HAVE_GD_BUNDLED
#include "ext/gd/libgd/gd.h"
#else
#include "gd.h"
#endif
static int le_gd;
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif
/* }}} */

#if HAVE_PDFLIB

#include "php_pdf.h"

static int le_pdf;

/* {{{ constants
 * to adopt the php way of error handling to PDFlib
 * The image related functions in PDFlib return -1 on error
 * but they may return 0 (FALSE) in normal cases
 * so this offset will repair this
 */
#define PDFLIB_IMAGE_OFFSET	1
#define PDFLIB_FONT_OFFSET	1
#define PDFLIB_PDI_OFFSET	1
#define PDFLIB_PATTERN_OFFSET	1
#define PDFLIB_SPOT_OFFSET	1
/* }}} */

/* {{{ pdf_functions[]
 */
function_entry pdf_functions[] = {
	PHP_FE(pdf_new, NULL)		/* new function */
	PHP_FE(pdf_delete, NULL)	/* new function */
	PHP_FE(pdf_open_file, NULL)	/* new function */
	PHP_FE(pdf_get_buffer, NULL)	/* new function */
	PHP_FE(pdf_close, NULL)
	PHP_FE(pdf_begin_page, NULL)
	PHP_FE(pdf_end_page, NULL)
	PHP_FE(pdf_get_majorversion, NULL)
	PHP_FE(pdf_get_minorversion, NULL)
	PHP_FE(pdf_get_value, NULL)
	PHP_FE(pdf_set_value, NULL)
	PHP_FE(pdf_get_parameter, NULL)
	PHP_FE(pdf_set_parameter, NULL)
	PHP_FE(pdf_findfont, NULL)	/* new function */
	PHP_FE(pdf_setfont, NULL)	/* new function */
	PHP_FE(pdf_show, NULL)
	PHP_FE(pdf_show_xy, NULL)
	PHP_FE(pdf_continue_text, NULL)
	PHP_FE(pdf_show_boxed, NULL)
	PHP_FE(pdf_stringwidth, NULL)	/* new parameters: [int font, float size] */
	PHP_FE(pdf_set_text_pos, NULL)
	PHP_FE(pdf_setdash, NULL)
	PHP_FE(pdf_setpolydash, NULL)	/* new function: not yet finished */
	PHP_FE(pdf_setflat, NULL)
	PHP_FE(pdf_setlinejoin, NULL)
	PHP_FE(pdf_setlinecap, NULL)
	PHP_FE(pdf_setmiterlimit, NULL)
	PHP_FE(pdf_setlinewidth, NULL)
	PHP_FE(pdf_save, NULL)
	PHP_FE(pdf_restore, NULL)
	PHP_FE(pdf_translate, NULL)
	PHP_FE(pdf_scale, NULL)
	PHP_FE(pdf_rotate, NULL)
	PHP_FE(pdf_skew, NULL)
	PHP_FE(pdf_concat, NULL)	/* new function */
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
	PHP_FE(pdf_clip, NULL)
	PHP_FE(pdf_endpath, NULL)
	PHP_FE(pdf_open_image_file, NULL)  /* new parameters: [char *stringpram, int intparam] */
	PHP_FE(pdf_open_ccitt, NULL)	/* new function */
	PHP_FE(pdf_open_image, NULL)	/* new function */
	PHP_FE(pdf_close_image, NULL)
	PHP_FE(pdf_place_image, NULL)
	PHP_FE(pdf_add_bookmark, NULL)
	PHP_FE(pdf_set_info, NULL)
	PHP_FE(pdf_attach_file, NULL)	/* new function */
	PHP_FE(pdf_add_note, NULL)	/* new function */
	PHP_FE(pdf_add_pdflink, NULL)
	PHP_FE(pdf_add_locallink, NULL)	/* new function */
	PHP_FE(pdf_add_launchlink, NULL)/* new function */
	PHP_FE(pdf_add_weblink, NULL)
	PHP_FE(pdf_set_border_style, NULL)
	PHP_FE(pdf_set_border_color, NULL)
	PHP_FE(pdf_set_border_dash, NULL)

	/* End of the official PDFLIB V3.x API */

	/* aliases for compatibility reasons */
	PHP_FALIAS(pdf_add_outline, pdf_add_bookmark, NULL)

	/* old font handling */
	PHP_FE(pdf_set_font, NULL)		/* deprecated */
	PHP_FE(pdf_get_font, NULL)		/* deprecated */
	PHP_FE(pdf_get_fontname, NULL)		/* deprecated */
	PHP_FE(pdf_get_fontsize, NULL)		/* deprecated */

	/* old way of starting a PDF document */
	PHP_FE(pdf_open, NULL)			/* deprecated */

	/* old stuff for setting infos */
	PHP_FE(pdf_set_info_creator, NULL)	/* deprecated */
	PHP_FE(pdf_set_info_title, NULL)	/* deprecated */
	PHP_FE(pdf_set_info_subject, NULL)	/* deprecated */
	PHP_FE(pdf_set_info_author, NULL)	/* deprecated */
	PHP_FE(pdf_set_info_keywords, NULL)	/* deprecated */
	PHP_FE(pdf_set_leading, NULL)   	/* deprecated */
	PHP_FE(pdf_set_text_rendering, NULL)	/* deprecated */
	PHP_FE(pdf_set_horiz_scaling, NULL)	/* deprecated */
	PHP_FE(pdf_set_text_rise, NULL)		/* deprecated */
	PHP_FE(pdf_set_char_spacing, NULL)	/* deprecated */
	PHP_FE(pdf_set_word_spacing, NULL)	/* deprecated */
	PHP_FE(pdf_set_transition, NULL)	/* deprecated */
	PHP_FE(pdf_set_duration, NULL)		/* deprecated */
	PHP_FE(pdf_get_image_height, NULL)	/* deprecated */
	PHP_FE(pdf_get_image_width, NULL)	/* deprecated */

	/* old stuff for opening images */
	PHP_FE(pdf_open_jpeg, NULL)		/* deprecated */
	PHP_FE(pdf_open_tiff, NULL)		/* deprecated */
	PHP_FE(pdf_open_png, NULL)		/* deprecated */
	PHP_FE(pdf_open_gif, NULL)		/* deprecated */

	/* some more stuff for compatibility */
	PHP_FE(pdf_add_annotation, NULL)
#if HAVE_LIBGD13
	PHP_FE(pdf_open_memory_image, NULL)
#endif
	/* depreciatet after V4.0 of PDFlib */
	PHP_FE(pdf_setgray_fill, NULL)
	PHP_FE(pdf_setgray_stroke, NULL)
	PHP_FE(pdf_setgray, NULL)
	PHP_FE(pdf_setrgbcolor_fill, NULL)
	PHP_FE(pdf_setrgbcolor_stroke, NULL)
	PHP_FE(pdf_setrgbcolor, NULL)

#if (PDFLIB_MAJORVERSION >= 4)
/* support for new functions in PDFlib V4.0 */
	PHP_FE(pdf_open_pdi, NULL)
	PHP_FE(pdf_close_pdi, NULL)
	PHP_FE(pdf_open_pdi_page, NULL)
	PHP_FE(pdf_place_pdi_page, NULL)
	PHP_FE(pdf_close_pdi_page, NULL)
	PHP_FE(pdf_get_pdi_parameter, NULL)
	PHP_FE(pdf_get_pdi_value, NULL)
	PHP_FE(pdf_begin_pattern, NULL)
	PHP_FE(pdf_end_pattern, NULL)
	PHP_FE(pdf_begin_template, NULL)
	PHP_FE(pdf_end_template, NULL)
	PHP_FE(pdf_setcolor, NULL)
	PHP_FE(pdf_makespotcolor, NULL)
	PHP_FE(pdf_arcn, NULL)
	PHP_FE(pdf_add_thumbnail, NULL)
	PHP_FE(pdf_initgraphics, NULL)
	PHP_FE(pdf_setmatrix, NULL)
#endif /* PDFlib >= V4 */

	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdf_module_entry
 */
zend_module_entry pdf_module_entry = {
    STANDARD_MODULE_HEADER,
	"pdf", 
	pdf_functions, 
	PHP_MINIT(pdf), 
	PHP_MSHUTDOWN(pdf), 
	NULL, 
	NULL, 
	PHP_MINFO(pdf), 
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES 
};
/* }}} */

#ifdef COMPILE_DL_PDF
ZEND_GET_MODULE(pdf)
#endif

/* {{{ _free_pdf_doc
 */
static void _free_pdf_doc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PDF *pdf = (PDF *)rsrc->ptr;
	PDF_delete(pdf);
}
/* }}} */

/* {{{ custom_errorhandler
 */
static void custom_errorhandler(PDF *p, int type, const char *shortmsg)
{
	TSRMLS_FETCH();
	switch (type){
		case PDF_NonfatalError:
			/*
			 * PDFlib warnings should be visible to the user.
			 * If he decides to live with PDFlib warnings
			 * he may use the PDFlib function
			 * pdf_set_parameter($p, "warning" 0) to switch off
			 * the warnings inside PDFlib.
			 */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal PDFlib warning: %s", shortmsg);
			return;
		case PDF_MemoryError: /* give up in all other cases */
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
			php_error_docref(NULL TSRMLS_CC, E_ERROR,"PDFlib error: %s", shortmsg);
		}
}
/* }}} */

/* {{{ pdf_emalloc
 */
static void *pdf_emalloc(PDF *p, size_t size, const char *caller)
{
	return(emalloc(size));
}
/* }}} */

/* {{{ pdf_realloc
 */
static void *pdf_realloc(PDF *p, void *mem, size_t size, const char *caller)
{
	return(erealloc(mem, size));
}
/* }}} */

/* {{{ pdf_efree
 */
static void pdf_efree(PDF *p, void *mem)
{
	efree(mem);
}
/* }}} */

/* {{{ pdf_flushwrite
 */
static size_t pdf_flushwrite(PDF *p, void *data, size_t size)
{
	TSRMLS_FETCH();

	return(php_write(data, size TSRMLS_CC));
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdf)
{
	char tmp[32];

	snprintf(tmp, 31, "%d.%02d", PDF_get_majorversion(), PDF_get_minorversion() );
	tmp[31]=0;

	php_info_print_table_start();
	php_info_print_table_row(2, "PDF Support", "enabled" );
#if (PDFLIB_MAJORVERSION >= 4)
	php_info_print_table_row(2, "PDFlib GmbH Version", PDFLIB_VERSIONSTRING );
#else
	php_info_print_table_row(2, "PDFlib GmbH Version", tmp );
#endif
	php_info_print_table_row(2, "Revision", "$Revision$" );
	php_info_print_table_end();

}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pdf)
{
	if ((PDF_get_majorversion() != PDFLIB_MAJORVERSION) ||
			(PDF_get_minorversion() != PDFLIB_MINORVERSION)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR,"PDFlib error: Version mismatch in wrapper code");
	}
	le_pdf = zend_register_list_destructors_ex(_free_pdf_doc, NULL, "pdf object", module_number);

	/* this does something like setlocale("C", ...) in PDFlib 3.x */
	PDF_boot();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdf)
{
	PDF_shutdown();
	return SUCCESS;
}
/* }}} */

/* {{{ _php_pdf_set_info
 */
static void _php_pdf_set_info(INTERNAL_FUNCTION_PARAMETERS, char *field) 
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	PDF_set_info(pdf, field, Z_STRVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pdf_set_info(int pdfdoc, string fieldname, string value)
   Fills an info field of the document */
PHP_FUNCTION(pdf_set_info)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	PDF_set_info(pdf, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pdf_set_info_creator(int pdfdoc, string creator)
   Fills the creator field of the document */
PHP_FUNCTION(pdf_set_info_creator)
{
	_php_pdf_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Creator");
}
/* }}} */

/* {{{ proto bool pdf_set_info_title(int pdfdoc, string title)
   Fills the title field of the document */
PHP_FUNCTION(pdf_set_info_title) 
{
	_php_pdf_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Title");
}
/* }}} */

/* {{{ proto bool pdf_set_info_subject(int pdfdoc, string subject)
   Fills the subject field of the document */
PHP_FUNCTION(pdf_set_info_subject) 
{
	_php_pdf_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Subject");
}
/* }}} */

/* {{{ proto bool pdf_set_info_author(int pdfdoc, string author)
   Fills the author field of the document */
PHP_FUNCTION(pdf_set_info_author) 
{
	_php_pdf_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Author");
}
/* }}} */

/* {{{ proto bool pdf_set_info_keywords(int pdfdoc, string keywords)
   Fills the keywords field of the document */
PHP_FUNCTION(pdf_set_info_keywords)
{
	_php_pdf_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Keywords");
}
/* }}} */

/* {{{ proto int pdf_open([int filedesc])
   Opens a new pdf document. If filedesc is NULL, document is created in memory. This is the old interface, only for compatibility use pdf_new + pdf_open_file instead */
PHP_FUNCTION(pdf_open)
{
	zval **file;
	FILE *fp = NULL;
	PDF *pdf;
	int argc = ZEND_NUM_ARGS();

	if(argc > 1)  {
		WRONG_PARAM_COUNT;
	} else if (argc != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		fp = NULL;
	} else {
		php_stream *stream;

		php_stream_from_zval(stream, file);
		
		if (php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void*)&fp, 1) == FAILURE)	{
			RETURN_FALSE;
		}
	}

	pdf = PDF_new2(custom_errorhandler, pdf_emalloc, pdf_realloc, pdf_efree, NULL);

	if(fp) {
		if (PDF_open_fp(pdf, fp) < 0) {
			RETURN_FALSE;
		}
	} else {
		PDF_open_mem(pdf, pdf_flushwrite);
	}
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_set_parameter(pdf, "imagewarning", "true");
#endif
	PDF_set_parameter(pdf, "binding", "PHP");

	ZEND_REGISTER_RESOURCE(return_value, pdf, le_pdf);
}

/* }}} */

/* {{{ proto void pdf_close(int pdfdoc)
   Closes the pdf document */
PHP_FUNCTION(pdf_close)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_close(pdf);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto void pdf_begin_page(int pdfdoc, float width, float height)
   Starts page */
PHP_FUNCTION(pdf_begin_page)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_begin_page(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_end_page(int pdfdoc)
   Ends page */
PHP_FUNCTION(pdf_end_page)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_end_page(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_show(int pdfdoc, string text)
   Output text at current position */
PHP_FUNCTION(pdf_show)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	PDF_show2(pdf, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_show_xy(int pdfdoc, string text, float x_koor, float y_koor)
   Output text at position */
PHP_FUNCTION(pdf_show_xy)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	PDF_show_xy2(pdf, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_show_boxed(int pdfdoc, string text, float x_koor, float y_koor, float width, float height, string mode [, string feature])
   Output text formated in a boxed */
PHP_FUNCTION(pdf_show_boxed)
{
	zval **argv[8];
	int argc = ZEND_NUM_ARGS();
	int nr;
	char *feature;
	PDF *pdf;

	if (((argc < 7) || (argc > 8)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, argv[0], -1, "pdf object", le_pdf);

	convert_to_string_ex(argv[1]);
	convert_to_double_ex(argv[2]);
	convert_to_double_ex(argv[3]);
	convert_to_double_ex(argv[4]);
	convert_to_double_ex(argv[5]);
	convert_to_string_ex(argv[6]);

	if(argc == 8) {
		convert_to_string_ex(argv[7]);
		feature = Z_STRVAL_PP(argv[7]);
	} else {
		feature = NULL;
	}

	nr = PDF_show_boxed(pdf, Z_STRVAL_PP(argv[1]),
							(float) Z_DVAL_PP(argv[2]),
							(float) Z_DVAL_PP(argv[3]),
							(float) Z_DVAL_PP(argv[4]),
							(float) Z_DVAL_PP(argv[5]),
							Z_STRVAL_PP(argv[6]),
							feature);

	RETURN_LONG(nr);
}
/* }}} */

/* {{{ proto void pdf_set_font(int pdfdoc, string font, float size, string encoding [, int embed])
   Select the current font face, size and encoding */
PHP_FUNCTION(pdf_set_font)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	int font, embed;
	PDF *pdf;

	switch (ZEND_NUM_ARGS()) {
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		embed = 0;
		break;
	case 5:
		if (zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg5);
		embed = Z_LVAL_PP(arg5);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_string_ex(arg4);

	font = PDF_findfont(pdf, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg4), embed);
	if (font == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Font %s not found", Z_STRVAL_PP(arg2));
		RETURN_FALSE;
	}

	PDF_setfont(pdf, font, (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php_pdf_set_value
 */
static void _php_pdf_set_value(INTERNAL_FUNCTION_PARAMETERS, char *field) 
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	PDF_set_value(pdf, field, (float)Z_DVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_value(int pdfdoc, string key, float value)
   Sets arbitrary value */
PHP_FUNCTION(pdf_set_value)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_set_value(pdf, Z_STRVAL_PP(arg2), (float)Z_DVAL_PP(arg3));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto float pdf_get_value(int pdfdoc, string key, float modifier)
   Gets arbitrary value */
PHP_FUNCTION(pdf_get_value)
{
	zval **argv[3];
	int argc = ZEND_NUM_ARGS();
	PDF *pdf;
	double value;

	if(((argc < 2) || (argc > 3)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, argv[0], -1, "pdf object", le_pdf);

	convert_to_string_ex(argv[1]);
	if(argc == 3)
	    convert_to_double_ex(argv[2]);

	if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "imagewidth"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "imageheight"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "resx"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "resy"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "capheight"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_FONT_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "ascender"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_FONT_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "descender"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PDFLIB_FONT_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "font"))) {
		value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), 0.0)+PDFLIB_FONT_OFFSET;
	} else {
		if(argc < 3) {
		    value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), 0.0);
		} else {
			value = PDF_get_value(pdf, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2]));
		}
	}

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ proto int pdf_get_font(int pdfdoc)
   Gets the current font */
PHP_FUNCTION(pdf_get_font) 
{
	zval **arg1;
	int font;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);
	
	font = (int) PDF_get_value(pdf, "font", 0);
	RETURN_LONG(font+PDFLIB_FONT_OFFSET);
}
/* }}} */

/* {{{ proto string pdf_get_fontname(int pdfdoc)
   Gets the current font name */
PHP_FUNCTION(pdf_get_fontname) 
{
	zval **arg1;
	char *fontname;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	fontname = (char *) PDF_get_parameter(pdf, "fontname", 0);
	RETURN_STRING(fontname, 1);
}
/* }}} */

/* {{{ proto float pdf_get_fontsize(int pdfdoc)
   Gets the current font size */
PHP_FUNCTION(pdf_get_fontsize) 
{
	zval **arg1;
	float fontsize;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	fontsize = PDF_get_value(pdf, "fontsize", 0);
	RETURN_DOUBLE(fontsize);
}
/* }}} */

/* {{{ proto void pdf_set_leading(int pdfdoc, float distance)
   Sets distance between text lines */
PHP_FUNCTION(pdf_set_leading) 
{
	_php_pdf_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "leading");
}
/* }}} */

/* {{{ proto void pdf_set_text_rendering(int pdfdoc, int mode)
   Determines how text is rendered */
PHP_FUNCTION(pdf_set_text_rendering) 
{
	_php_pdf_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "textrendering");
}
/* }}} */

/* {{{ proto void pdf_set_horiz_scaling(int pdfdoc, float scale)
   Sets horizontal scaling of text */
PHP_FUNCTION(pdf_set_horiz_scaling) 
{
	_php_pdf_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "horizscaling");
}
/* }}} */

/* {{{ proto void pdf_set_text_rise(int pdfdoc, float value)
   Sets the text rise */
PHP_FUNCTION(pdf_set_text_rise) 
{
	_php_pdf_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "textrise");
}
/* }}} */

/* {{{ proto void pdf_set_char_spacing(int pdfdoc, float space)
   Sets character spacing */
PHP_FUNCTION(pdf_set_char_spacing)
{
	_php_pdf_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "charspacing");
}
/* }}} */

/* {{{ proto void pdf_set_word_spacing(int pdfdoc, float space)
   Sets spacing between words */
PHP_FUNCTION(pdf_set_word_spacing)
{
	_php_pdf_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "wordspacing");
}
/* }}} */

/* {{{ proto void pdf_set_text_pos(int pdfdoc, float x, float y)
   Sets the position of text for the next pdf_show call */
PHP_FUNCTION(pdf_set_text_pos) 
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_set_text_pos(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_continue_text(int pdfdoc, string text)
   Output text in next line */
PHP_FUNCTION(pdf_continue_text)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	PDF_continue_text2(pdf, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto float pdf_stringwidth(int pdfdoc, string text [, int font, float size])
   Returns width of text in current font */
PHP_FUNCTION(pdf_stringwidth)
{
	zval **arg1, **arg2, **arg3, **arg4;
	int font;
	double width, size;
	PDF *pdf;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(arg3);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	if (ZEND_NUM_ARGS() == 2) {
	    font = (int)PDF_get_value(pdf, "font", 0)+PDFLIB_FONT_OFFSET;
	    size = PDF_get_value(pdf, "fontsize", 0);
	} else {
	    convert_to_long_ex(arg3);
	    font = Z_LVAL_PP(arg3);
	    convert_to_double_ex(arg4);
	    size = Z_DVAL_PP(arg4);
	}
	width = (double) PDF_stringwidth2(pdf,
		Z_STRVAL_PP(arg2),
		Z_STRLEN_PP(arg2),
		font-PDFLIB_FONT_OFFSET,
		(float)size);

	RETURN_DOUBLE((double) width);
}
/* }}} */

/* {{{ proto void pdf_save(int pdfdoc)
   Saves current enviroment */
PHP_FUNCTION(pdf_save)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_save(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_restore(int pdfdoc)
   Restores formerly saved enviroment */
PHP_FUNCTION(pdf_restore)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_restore(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_translate(int pdfdoc, float x, float y)
   Sets origin of coordinate system */
PHP_FUNCTION(pdf_translate) 
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_translate(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_scale(int pdfdoc, float x_scale, float y_scale)
   Sets scaling */
PHP_FUNCTION(pdf_scale)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_scale(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_rotate(int pdfdoc, float angle)
   Sets rotation */
PHP_FUNCTION(pdf_rotate)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	PDF_rotate(pdf, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_skew(int pdfdoc, float xangle, float yangle)
   Skew the coordinate system */
PHP_FUNCTION(pdf_skew)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_skew(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setflat(int pdfdoc, float value)
   Sets flatness */
PHP_FUNCTION(pdf_setflat) 
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	/* pdflib will do this for you, will throw some exception
	if((Z_LVAL_PP(arg2) > 100) && (Z_LVAL_PP(arg2) < 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter of pdf_setflat() has to between 0 and 100");
		RETURN_FALSE;
	}
	*/

	PDF_setflat(pdf, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinejoin(int pdfdoc, int value)
   Sets linejoin parameter */
PHP_FUNCTION(pdf_setlinejoin) 
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	/* pdflib will do this for you, will throw some exception
	if((Z_LVAL_PP(arg2) > 2) && (Z_LVAL_PP(arg2) < 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter of pdf_setlinejoin() must be between 0 and 2");
		RETURN_FALSE;
	}
	*/

	PDF_setlinejoin(pdf, Z_LVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinecap(int pdfdoc, int value)
   Sets linecap parameter */
PHP_FUNCTION(pdf_setlinecap) 
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	/* pdflib will do this for you, will throw some exception
	if((Z_LVAL_PP(arg2) > 2) && (Z_LVAL_PP(arg2) < 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter of pdf_setlinecap() must be > 0 and <= 2");
		RETURN_FALSE;
	}
	*/

	PDF_setlinecap(pdf, Z_LVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setmiterlimit(int pdfdoc, float value)
   Sets miter limit */
PHP_FUNCTION(pdf_setmiterlimit)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	/* pdflib will do this for you, will throw some exception
	if(Z_DVAL_PP(arg2) < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter of pdf_setmiterlimit() must be >= 1");
		RETURN_FALSE;
	}
	*/

	PDF_setmiterlimit(pdf, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setlinewidth(int pdfdoc, float width)
   Sets line width */
PHP_FUNCTION(pdf_setlinewidth)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	PDF_setlinewidth(pdf, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setdash(int pdfdoc, float black, float white)
   Sets dash pattern */
PHP_FUNCTION(pdf_setdash)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_setdash(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_moveto(int pdfdoc, float x, float y)
   Sets current point */
PHP_FUNCTION(pdf_moveto)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_moveto(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_curveto(int pdfdoc, float x1, float y1, float x2, float y2, float x3, float y3)
   Draws a curve */
PHP_FUNCTION(pdf_curveto)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	PDF_curveto(pdf, (float) Z_DVAL_PP(arg2),
					 (float) Z_DVAL_PP(arg3),
					 (float) Z_DVAL_PP(arg4),
					 (float) Z_DVAL_PP(arg5),
					 (float) Z_DVAL_PP(arg6),
					 (float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_lineto(int pdfdoc, float x, float y)
   Draws a line */
PHP_FUNCTION(pdf_lineto)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_lineto(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_circle(int pdfdoc, float x, float y, float radius)
   Draws a circle */
PHP_FUNCTION(pdf_circle)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	PDF_circle(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_arc(int pdfdoc, float x, float y, float radius, float start, float end)
   Draws an arc */
PHP_FUNCTION(pdf_arc)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);

	PDF_arc(pdf, (float) Z_DVAL_PP(arg2),
				 (float) Z_DVAL_PP(arg3),
				 (float) Z_DVAL_PP(arg4),
				 (float) Z_DVAL_PP(arg5),
				 (float) Z_DVAL_PP(arg6));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_rect(int pdfdoc, float x, float y, float width, float height)
   Draws a rectangle */
PHP_FUNCTION(pdf_rect)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);

	PDF_rect(pdf, (float) Z_DVAL_PP(arg2),
				  (float) Z_DVAL_PP(arg3),
				  (float) Z_DVAL_PP(arg4),
				  (float) Z_DVAL_PP(arg5));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath(int pdfdoc)
   Close path */
PHP_FUNCTION(pdf_closepath)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_closepath(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_stroke(int pdfdoc)
   Close path and draw line along path */
PHP_FUNCTION(pdf_closepath_stroke)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_closepath_stroke(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_stroke(int pdfdoc)
   Draw line along path path */
PHP_FUNCTION(pdf_stroke)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_stroke(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_fill(int pdfdoc)
   Fill current path */
PHP_FUNCTION(pdf_fill) 
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_fill(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_fill_stroke(int pdfdoc)
   Fill and stroke current path */
PHP_FUNCTION(pdf_fill_stroke)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_fill_stroke(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_closepath_fill_stroke(int pdfdoc)
   Close, fill and stroke current path */
PHP_FUNCTION(pdf_closepath_fill_stroke)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_closepath_fill_stroke(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_endpath(int pdfdoc)
   Ends current path */
PHP_FUNCTION(pdf_endpath) 
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_endpath(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_clip(int pdfdoc)
   Clips to current path */
PHP_FUNCTION(pdf_clip)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_clip(pdf);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_parameter(int pdfdoc, string key, string value)
   Sets arbitrary parameters */
PHP_FUNCTION(pdf_set_parameter)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	PDF_set_parameter(pdf, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string pdf_get_parameter(int pdfdoc, string key, mixed modifier)
   Gets arbitrary parameters */
PHP_FUNCTION(pdf_get_parameter)
{
	zval **argv[3];
	int argc = ZEND_NUM_ARGS();
	PDF *pdf;
	char *value;

	if(((argc < 2) || (argc > 3)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pdf, PDF *, argv[0], -1, "pdf object", le_pdf);

	convert_to_string_ex(argv[1]);
	if(argc == 3) {
		convert_to_double_ex(argv[2]);
		value = (char *) PDF_get_parameter(pdf, Z_STRVAL_PP(argv[1]), (float) Z_DVAL_PP(argv[2]));
	} else {
		value = (char *) PDF_get_parameter(pdf, Z_STRVAL_PP(argv[1]), 0.0);
	}

	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto void pdf_setgray_fill(int pdfdoc, float value)
   Sets filling color to gray value */
PHP_FUNCTION(pdf_setgray_fill)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_setcolor(pdf, "fill", "gray", (float) Z_DVAL_PP(arg2), 0, 0, 0);
#else
	PDF_setgray_fill(pdf, (float) Z_DVAL_PP(arg2));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray_stroke(int pdfdoc, float value)
   Sets drawing color to gray value */
PHP_FUNCTION(pdf_setgray_stroke) 
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_setcolor(pdf, "stroke", "gray", (float) Z_DVAL_PP(arg2), 0, 0, 0);
#else
	PDF_setgray_stroke(pdf, (float) Z_DVAL_PP(arg2));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setgray(int pdfdoc, float value)
   Sets drawing and filling color to gray value */
PHP_FUNCTION(pdf_setgray)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_setcolor(pdf, "both", "gray", (float) Z_DVAL_PP(arg2), 0, 0, 0);
#else
	PDF_setgray(pdf, (float) Z_DVAL_PP(arg2));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor_fill(int pdfdoc, float red, float green, float blue)
   Sets filling color to RGB color value */
PHP_FUNCTION(pdf_setrgbcolor_fill)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_setcolor(pdf, "fill", "rgb", (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), 0);
#else
	PDF_setrgbcolor_fill(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor_stroke(int pdfdoc, float red, float green, float blue)
   Sets drawing color to RGB color value */
PHP_FUNCTION(pdf_setrgbcolor_stroke)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_setcolor(pdf, "stroke", "rgb", (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), 0);
#else
	PDF_setrgbcolor_stroke(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setrgbcolor(int pdfdoc, float red, float green, float blue)
   Sets drawing and filling color to RGB color value */
PHP_FUNCTION(pdf_setrgbcolor)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_setcolor(pdf, "both", "rgb", (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), 0);
#else
	PDF_setrgbcolor(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_add_bookmark(int pdfdoc, string text [, int parent, int open])
   Adds bookmark for current page */
PHP_FUNCTION(pdf_add_bookmark)
{
	zval **arg1, **arg2, **arg3, **arg4;
	int parentid, open, id;
	PDF *pdf;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);

	if (ZEND_NUM_ARGS() > 2) {
		convert_to_long_ex(arg3);
		parentid = Z_LVAL_PP(arg3);

		if (ZEND_NUM_ARGS() > 3) {
			convert_to_long_ex(arg4);
			open = Z_LVAL_PP(arg4);
		} else {
			open = 0;
		}
	} else {
		parentid = 0;
		open = 0;
	}

	/* will never return 0 */
	id = PDF_add_bookmark(pdf, Z_STRVAL_PP(arg2), parentid, open);

	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto void pdf_set_transition(int pdfdoc, int transition)
   Sets transition between pages */
PHP_FUNCTION(pdf_set_transition)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);

	switch(Z_LVAL_PP(arg2)) {
		case 0:
			PDF_set_parameter(pdf, "transition", "none");
			break;
		case 1:
			PDF_set_parameter(pdf, "transition", "split");
			break;
		case 2:
			PDF_set_parameter(pdf, "transition", "blinds");
			break;
		case 3:
			PDF_set_parameter(pdf, "transition", "box");
			break;
		case 4:
			PDF_set_parameter(pdf, "transition", "wipe");
			break;
		case 5:
			PDF_set_parameter(pdf, "transition", "dissolve");
			break;
		case 6:
			PDF_set_parameter(pdf, "transition", "glitter");
			break;
		case 7:
			PDF_set_parameter(pdf, "transition", "replace");
			break;
		default:
			PDF_set_parameter(pdf, "transition", "none");
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_duration(int pdfdoc, float duration)
   Sets duration between pages */
PHP_FUNCTION(pdf_set_duration)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	PDF_set_value(pdf, "duration", (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php_pdf_open_image
 */
static void _php_pdf_open_image(INTERNAL_FUNCTION_PARAMETERS, char *type) 
{
	zval **arg1, **arg2;
	PDF *pdf;
	int pdf_image;
	char *image;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg2), &image TSRMLS_CC);
#else
	image = Z_STRVAL_PP(arg2);
#endif  
        
	pdf_image = PDF_open_image_file(pdf, type, image, "", 0);

	RETURN_LONG(pdf_image+PDFLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto int pdf_open_gif(int pdf, string giffile)
   Opens a GIF file and returns an image for placement in a pdf object */
PHP_FUNCTION(pdf_open_gif)
{
	_php_pdf_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"gif");
}
/* }}} */

/* {{{ proto int pdf_open_jpeg(int pdf, string jpegfile)
   Opens a JPEG file and returns an image for placement in a PDF document */
PHP_FUNCTION(pdf_open_jpeg)
{
	_php_pdf_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"jpeg");
}
/* }}} */

/* {{{ proto int pdf_open_png(int pdf, string pngfile)
   Opens a PNG file and returns an image for placement in a PDF document */
PHP_FUNCTION(pdf_open_png)
{
	_php_pdf_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"png");
}
/* }}} */

/* {{{ proto int pdf_open_tiff(int pdf, string tifffile)
   Opens a TIFF file and returns an image for placement in a PDF document */
PHP_FUNCTION(pdf_open_tiff)
{
	_php_pdf_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"tiff");
}
/* }}} */

/* {{{ proto int pdf_open_image_file(int pdf, string type, string file, string stringparam, int intparam)
   Opens an image file of the given type and returns an image for placement in a PDF document */
PHP_FUNCTION(pdf_open_image_file)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PDF *pdf;
	int pdf_image, argc;
	char *image;
	char *stringparam;
	int intparam;

	switch ((argc = ZEND_NUM_ARGS())) {
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 5:
		if (zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg3), &image TSRMLS_CC);
#else
	image = Z_STRVAL_PP(arg3);
#endif  

	if (argc == 3) {
		pdf_image = PDF_open_image_file(pdf, Z_STRVAL_PP(arg2), image, "", 0);
	} else {
	    convert_to_string_ex(arg4);
	    convert_to_long_ex(arg5);

	    stringparam = Z_STRVAL_PP(arg4);
	    intparam = Z_LVAL_PP(arg5);

	    /* adjust the image handle */
	    if (!strcmp(stringparam, "masked"))
		intparam -= PDFLIB_IMAGE_OFFSET;

	    pdf_image = PDF_open_image_file(pdf, Z_STRVAL_PP(arg2), image, stringparam, intparam);
	}

	if (pdf_image == -1) {
	    /* pdflib will do this for you, will throw some exception
	    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open image: %s", image);
	    */
	    RETURN_FALSE;
	}
	RETURN_LONG(pdf_image+PDFLIB_IMAGE_OFFSET);

}
/* }}} */

#if HAVE_LIBGD13
/* {{{ proto int pdf_open_memory_image(int pdf, int image)
   Takes an GD image and returns an image for placement in a PDF document */
PHP_FUNCTION(pdf_open_memory_image)
{
	zval **arg1, **arg2;
	int i, j, color, count;
	int pdf_image;
	gdImagePtr im;
	unsigned char *buffer, *ptr;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);
	ZEND_GET_RESOURCE_TYPE_ID(le_gd,"gd");
	if(!le_gd)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find handle for GD image stream. Please check the GD extension is loaded.");
	}
	ZEND_FETCH_RESOURCE(im, gdImagePtr, arg2, -1, "Image", le_gd);

	count = 3 * im->sx * im->sy;
	buffer = (unsigned char *) emalloc(count);

	ptr = buffer;
	for(i=0; i<im->sy; i++) {
		for(j=0; j<im->sx; j++) {
#if HAVE_LIBGD20
			if(gdImageTrueColor(im)) {
				if (im->tpixels && gdImageBoundsSafe(im, j, i)) {
					color = gdImageTrueColorPixel(im, j, i);
					*ptr++ = (color >> 16) & 0xFF;
					*ptr++ = (color >> 8) & 0xFF;
					*ptr++ = color & 0xFF;
				}
			} else {
#endif
				if (im->pixels && gdImageBoundsSafe(im, j, i)) {
					color = im->pixels[i][j];
					*ptr++ = im->red[color];
					*ptr++ = im->green[color];
					*ptr++ = im->blue[color];
				}
#if HAVE_LIBGD20
			}
#endif		
		}
	}

	pdf_image = PDF_open_image(pdf, "raw", "memory", buffer, im->sx*im->sy*3, im->sx, im->sy, 3, 8, NULL);
	efree(buffer);

	if(pdf_image == -1) {
		/* pdflib will do this for you, will throw some exception
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open image");
		*/
		efree(buffer);
		RETURN_FALSE;
	}

	RETURN_LONG(pdf_image+PDFLIB_IMAGE_OFFSET);
}
/* }}} */
#endif /* HAVE_LIBGD13 */

/* {{{ proto void pdf_close_image(int pdf, int pdfimage)
   Closes the PDF image */
PHP_FUNCTION(pdf_close_image)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);
	convert_to_long_ex(arg2);

	PDF_close_image(pdf, Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_place_image(int pdf, int pdfimage, float x, float y, float scale)
   Places image in the PDF document */
PHP_FUNCTION(pdf_place_image)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);

	PDF_place_image(pdf, Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET, (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), (float) Z_DVAL_PP(arg5));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_get_image_width(int pdf, int pdfimage)
   Returns the width of an image */
PHP_FUNCTION(pdf_get_image_width)
{
	zval **arg1, **arg2;
	PDF *pdf;
	int width;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);
	convert_to_long_ex(arg2);

	width = (int) PDF_get_value(pdf, "imagewidth", (float)Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET);
	RETURN_LONG(width);
}
/* }}} */

/* {{{ proto int pdf_get_image_height(int pdf, int pdfimage)
   Returns the height of an image */
PHP_FUNCTION(pdf_get_image_height)
{
	zval **arg1, **arg2;
	PDF *pdf;
	int height;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);
	convert_to_long_ex(arg2);

	height = (int) PDF_get_value(pdf, "imageheight", (float)Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET);
	RETURN_LONG(height);
}
/* }}} */

/* {{{ proto void pdf_add_weblink(int pdfdoc, float llx, float lly, float urx, float ury, string url)
   Adds link to web resource */
PHP_FUNCTION(pdf_add_weblink)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);
	PDF_add_weblink(pdf, (float) Z_DVAL_PP(arg2), 
						 (float) Z_DVAL_PP(arg3), 
						 (float) Z_DVAL_PP(arg4), 
						 (float) Z_DVAL_PP(arg5), 
						 Z_STRVAL_PP(arg6));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_pdflink(int pdfdoc, float llx, float lly, float urx, float ury, string filename, int page, string dest)
   Adds link to PDF document */
PHP_FUNCTION(pdf_add_pdflink)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 8 || zend_get_parameters_ex(8, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_string_ex(arg8);
	PDF_add_pdflink(pdf, (float) Z_DVAL_PP(arg2), 
						 (float) Z_DVAL_PP(arg3), 
						 (float) Z_DVAL_PP(arg4), 
						 (float) Z_DVAL_PP(arg5),
						 Z_STRVAL_PP(arg6), 
						 Z_LVAL_PP(arg7),
						 Z_STRVAL_PP(arg8));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_border_style(int pdfdoc, string style, float width)
   Sets style of box surounding all kinds of annotations and link */
PHP_FUNCTION(pdf_set_border_style)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_set_border_style(pdf, Z_STRVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_border_color(int pdfdoc, float red, float green, float blue)
   Sets color of box surounded all kinds of annotations and links */
PHP_FUNCTION(pdf_set_border_color)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	PDF_set_border_color(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_set_border_dash(int pdfdoc, float black, float white)
   Sets the border dash style of all kinds of annotations and links */
PHP_FUNCTION(pdf_set_border_dash)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PDF_set_border_dash(pdf, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_annotation(int pdfdoc, float xll, float yll, float xur, float xur, string title, string text)
   Sets annotation (depreciated use pdf_add_note instead) */
PHP_FUNCTION(pdf_add_annotation)
{
	zval **argv[7];
	PDF *pdf;

	if(ZEND_NUM_ARGS() != 7 || zend_get_parameters_array_ex(7, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pdf, PDF *, argv[0], -1, "pdf object", le_pdf);

	convert_to_double_ex(argv[1]);
	convert_to_double_ex(argv[2]);
	convert_to_double_ex(argv[3]);
	convert_to_double_ex(argv[4]);
	convert_to_string_ex(argv[5]);
	convert_to_string_ex(argv[6]);

	PDF_add_note(pdf, (float) Z_DVAL_PP(argv[1]),
					  (float) Z_DVAL_PP(argv[2]),
					  (float) Z_DVAL_PP(argv[3]),
					  (float) Z_DVAL_PP(argv[4]),
					  Z_STRVAL_PP(argv[6]),
					  Z_STRVAL_PP(argv[5]),
					  "note", 1);

	RETURN_TRUE;
}
/* }}} */

/* RJS: START OF NEW CODE */

/* {{{ proto int pdf_new()
   Creates a new PDF object */
PHP_FUNCTION(pdf_new)
{
	PDF *pdf;

	pdf = PDF_new2(custom_errorhandler, pdf_emalloc, pdf_realloc, pdf_efree, NULL);
#if (PDFLIB_MAJORVERSION >= 4)
	PDF_set_parameter(pdf, "imagewarning", "true");
#endif
	PDF_set_parameter(pdf, "binding", "PHP");

	ZEND_REGISTER_RESOURCE(return_value, pdf, le_pdf);
}

/* }}} */

/* {{{ proto int pdf_get_majorversion()
   Returns the major version number of the PDFlib */
PHP_FUNCTION(pdf_get_majorversion)
{
        if (ZEND_NUM_ARGS() != 0) {  
                WRONG_PARAM_COUNT;
        }

        RETURN_LONG(PDF_get_majorversion());
}

/* {{{ proto int pdf_get_minorversion()
   Returns the minor version number of the PDFlib */
PHP_FUNCTION(pdf_get_minorversion)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(PDF_get_minorversion());
}

/* }}} */
/* {{{ proto bool pdf_delete(int pdfdoc)
   Deletes the PDF object */
PHP_FUNCTION(pdf_delete)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);
	zend_list_delete(Z_RESVAL_PP(arg1));

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int pdf_open_file(int pdfdoc [, char filename])
   Opens a new PDF document. If filename is NULL, document is created in memory. This is not yet fully supported */

PHP_FUNCTION(pdf_open_file)
{
	zval **arg1, **arg2;
	int pdf_file;
	char *filename;
	int argc;
	PDF *pdf;

	if((argc = ZEND_NUM_ARGS()) > 2)
		WRONG_PARAM_COUNT;

	if (argc == 1) {
		if (zend_get_parameters_ex(1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	if (argc == 2) {
		convert_to_string_ex(arg2);
		filename = Z_STRVAL_PP(arg2);
		pdf_file = PDF_open_file(pdf, filename);
	} else {
		/* open in memory */
		pdf_file = PDF_open_file(pdf, "");
	}

	if (pdf_file == -1)
	    RETURN_FALSE;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto int pdf_get_buffer(int pdfdoc)
   Fetches the full buffer containig the generated PDF data */
PHP_FUNCTION(pdf_get_buffer)
{
	zval **arg1;
	long size;
	PDF *pdf;
	const char *buffer;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	buffer = PDF_get_buffer(pdf, &size);

	RETURN_STRINGL((char *)buffer, size, 1);
}

/* }}} */

/* {{{ proto int pdf_findfont(int pdfdoc, string fontname, string encoding [, int embed])
   Prepares the font fontname for later use with pdf_setfont() */
PHP_FUNCTION(pdf_findfont)
{
	zval **arg1, **arg2, **arg3, **arg4;
	int embed, font;
	const char *fontname, *encoding;
	PDF *pdf;

	switch (ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		embed = 0;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg4);
		embed = Z_LVAL_PP(arg4);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	fontname = Z_STRVAL_PP(arg2);

	convert_to_string_ex(arg3);
	encoding = Z_STRVAL_PP(arg3);

	font = PDF_findfont(pdf, fontname, encoding, embed);
	if (font == -1) {
		/* pdflib will do this for you, will throw some exception
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Font %s not found", fontname);
		*/
		RETURN_FALSE;
	}

	RETURN_LONG(font+PDFLIB_FONT_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_setfont(int pdfdoc, int font, float fontsize)
   Sets the current font in the fiven fontsize */
PHP_FUNCTION(pdf_setfont)
{
	zval **arg1, **arg2, **arg3;
	int font;
	float fontsize;
	PDF *pdf;

	if(ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	font = Z_LVAL_PP(arg2);

	convert_to_double_ex(arg3);
	fontsize = (float)Z_DVAL_PP(arg3);

	PDF_setfont(pdf, font-PDFLIB_FONT_OFFSET, fontsize);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setpolydash(int pdfdoc, float darray)
   Sets more complicated dash pattern */ 

PHP_FUNCTION(pdf_setpolydash)
{
	zval **arg1, **arg2;
	HashTable *array;
	int len, i;
	float *darray;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_array_ex(arg2);
	array = Z_ARRVAL_PP(arg2);
	len = zend_hash_num_elements(array);

	darray = emalloc(len * sizeof(double));
	zend_hash_internal_pointer_reset(array);
	for (i=0; i<len; i++) {
	    zval *keydata, **keydataptr;

	    zend_hash_get_current_data(array, (void **) &keydataptr);
	    keydata = *keydataptr;
	    if (Z_TYPE_P(keydata) == IS_DOUBLE) {
		darray[i] = (float) Z_DVAL_P(keydata);
	    } else if (Z_TYPE_P(keydata) == IS_LONG) {
		darray[i] = (float) Z_LVAL_P(keydata);
	    } else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PDFlib set_polydash: illegal darray value");
	    }
	    zend_hash_move_forward(array);
	}

	PDF_setpolydash(pdf, darray, len);

	efree(darray);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_concat(int pdf, float a, float b, float c, float d, float e, float f)
   Concatenates a matrix to the current transformation matrix for text and graphics */
PHP_FUNCTION(pdf_concat)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	PDF_concat(pdf,
	    (float) Z_DVAL_PP(arg2),
	    (float) Z_DVAL_PP(arg3),
	    (float) Z_DVAL_PP(arg4),
	    (float) Z_DVAL_PP(arg5),
	    (float) Z_DVAL_PP(arg6),
	    (float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_open_ccitt(int pdf, string filename, int width, int height, int bitreverse, int k, int blackls1)
   Opens an image file with raw CCITT G3 or G4 compresed bitmap data */
PHP_FUNCTION(pdf_open_ccitt)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PDF *pdf;
	int pdf_image;
	char *image;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg2), &image TSRMLS_CC);
#else
	image = Z_STRVAL_PP(arg2);
#endif  

	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);

	pdf_image = PDF_open_CCITT(pdf,
	    image,
	    Z_LVAL_PP(arg3),
	    Z_LVAL_PP(arg4),
	    Z_LVAL_PP(arg5),
	    Z_LVAL_PP(arg6),
	    Z_LVAL_PP(arg7));

	RETURN_LONG(pdf_image+PDFLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto int pdf_open_image(int pdf, string type, string source, string data, long length, int width, int height, int components, int bpc, string params)
   Opens an image of the given type and returns an image for placement in a PDF document */
PHP_FUNCTION(pdf_open_image)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10;
	PDF *pdf;
	int pdf_image;
	char *image;

	if (ZEND_NUM_ARGS() != 10 || zend_get_parameters_ex(10, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_long_ex(arg8);
	convert_to_long_ex(arg9);
	convert_to_string_ex(arg10);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg4), &image TSRMLS_CC);
#else
	image = Z_STRVAL_PP(arg4);
#endif  

	pdf_image = PDF_open_image(pdf,
		Z_STRVAL_PP(arg2),
		Z_STRVAL_PP(arg3),
		image,
		Z_LVAL_PP(arg5),
		Z_LVAL_PP(arg6),
		Z_LVAL_PP(arg7),
		Z_LVAL_PP(arg8),
		Z_LVAL_PP(arg9),
		Z_STRVAL_PP(arg10));

	RETURN_LONG(pdf_image+PDFLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_attach_file(int pdf, float lly, float lly, float urx, float ury, string filename, string description, string author, string mimetype, string icon)
   Adds a file attachment annotation at the rectangle specified by his lower left and upper right corners */
PHP_FUNCTION(pdf_attach_file)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 10 || zend_get_parameters_ex(10, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);

	PDF_attach_file(pdf,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_STRVAL_PP(arg6),
		Z_STRVAL_PP(arg7),
		Z_STRVAL_PP(arg8),
		Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_note(int pdfdoc, float llx, float lly, float urx, float ury, string contents, string title, string icon, int open)
   Sets annotation */
PHP_FUNCTION(pdf_add_note)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 9 || zend_get_parameters_ex(9, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_long_ex(arg9);

	PDF_add_note(pdf,
		 (float) Z_DVAL_PP(arg2),
		 (float) Z_DVAL_PP(arg3),
		 (float) Z_DVAL_PP(arg4),
		 (float) Z_DVAL_PP(arg5),
		 Z_STRVAL_PP(arg6),
		 Z_STRVAL_PP(arg7),
		 Z_STRVAL_PP(arg8),
		 Z_LVAL_PP(arg9));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_locallink(int pdfdoc, float llx, float lly, float urx, float ury, int page, string dest)
   Adds link to web resource */
PHP_FUNCTION(pdf_add_locallink)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_string_ex(arg7);

	PDF_add_locallink(pdf,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_LVAL_PP(arg6),
		Z_STRVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_launchlink(int pdfdoc, float llx, float lly, float urx, float ury, string filename)
   Adds link to web resource */
PHP_FUNCTION(pdf_add_launchlink)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);

	PDF_add_launchlink(pdf,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_STRVAL_PP(arg6));

	RETURN_TRUE;
}
/* }}} */

#if (PDFLIB_MAJORVERSION >= 4)

/* {{{ proto int pdf_open_pdi(int pdf, string filename, string stringparam, int intparam);
 * Open an existing PDF document and prepare it for later use. */
PHP_FUNCTION(pdf_open_pdi)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;
	int pdi_handle;
	char *file;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg2), &file TSRMLS_CC);
#else
	file = Z_STRVAL_PP(arg2);
#endif  

	pdi_handle = PDF_open_pdi(pdf,
		file,
		Z_STRVAL_PP(arg3),
		Z_LVAL_PP(arg4));

	RETURN_LONG(pdi_handle+PDFLIB_PDI_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_close_pdi(int pdf, int doc);
 * Close all open page handles, and close the input PDF document. */
PHP_FUNCTION(pdf_close_pdi)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);

	PDF_close_pdi(pdf,
		Z_LVAL_PP(arg2)-PDFLIB_PDI_OFFSET);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_open_pdi_page(int pdf, int doc, int page, string label);
 * Prepare a page for later use with PDF_place_image(). */
PHP_FUNCTION(pdf_open_pdi_page)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PDF *pdf;
	int pdi_image;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_string_ex(arg4);

	pdi_image = PDF_open_pdi_page(pdf,
		Z_LVAL_PP(arg2)-PDFLIB_PDI_OFFSET,
		Z_LVAL_PP(arg3),
		Z_STRVAL_PP(arg4));

	RETURN_LONG(pdi_image+PDFLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_place_pdi_page(int pdf, int page, float x, float y, float sx, float sy)
 * Place a PDF page with the lower left corner at (x, y), and scale it. */
PHP_FUNCTION(pdf_place_pdi_page)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);

	PDF_place_pdi_page(pdf,
		Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET,
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		(float) Z_DVAL_PP(arg6));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_close_pdi_page(int pdf, int page);
 * Close the page handle, and free all page-related resources. */
PHP_FUNCTION(pdf_close_pdi_page)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);

	PDF_close_pdi_page(pdf,
		Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string pdf_get_pdi_parameter(int pdf, string key, int doc, int page, int index);
 * Get the contents of some PDI document parameter with string type. */
PHP_FUNCTION(pdf_get_pdi_parameter)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PDF *pdf;
	const char *buffer;
	int size;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	buffer = PDF_get_pdi_parameter(pdf,
		Z_STRVAL_PP(arg2),
		Z_LVAL_PP(arg3)-PDFLIB_PDI_OFFSET,
		Z_LVAL_PP(arg4)-PDFLIB_IMAGE_OFFSET,
		Z_LVAL_PP(arg5),
		&size);

	RETURN_STRINGL((char *)buffer, size, 1);
}
/* }}} */

/* {{{ proto float pdf_get_pdi_value(int pdf, string key, int doc, int page, int index);
 * Get the contents of some PDI document parameter with numerical type. */
PHP_FUNCTION(pdf_get_pdi_value)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PDF *pdf;
	double value;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	value = (double)PDF_get_pdi_value(pdf,
		Z_STRVAL_PP(arg2),
		Z_LVAL_PP(arg3)-PDFLIB_PDI_OFFSET,
		Z_LVAL_PP(arg4)-PDFLIB_IMAGE_OFFSET,
		Z_LVAL_PP(arg5));

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ proto int pdf_begin_pattern(int pdf, float width, float height, float xstep, float ystep, int painttype);
 * Start a new pattern definition. */
PHP_FUNCTION(pdf_begin_pattern)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PDF *pdf;
	int pattern_image;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_long_ex(arg6);

	pattern_image = PDF_begin_pattern(pdf,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_LVAL_PP(arg6));

	RETURN_LONG(pattern_image+PDFLIB_PATTERN_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_end_pattern(int pdf);
 * Finish the pattern definition. */
PHP_FUNCTION(pdf_end_pattern)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_end_pattern(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_begin_template(int pdf, float width, float height);
 * Start a new template definition. */
PHP_FUNCTION(pdf_begin_template)
{
	zval **arg1, **arg2, **arg3;
	PDF *pdf;
	int tmpl_image;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);

	tmpl_image = PDF_begin_template(pdf,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3));

	RETURN_LONG(tmpl_image+PDFLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_end_template(int pdf);
 * Finish the template definition. */
PHP_FUNCTION(pdf_end_template)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);


	PDF_end_template(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setcolor(int pdf, string type, string colorspace, float c1 [, float c2 [, float c3 [, float c4]]]);
 * Set the current color space and color. */
PHP_FUNCTION(pdf_setcolor)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PDF *pdf;
	double c1;
	int argc = ZEND_NUM_ARGS();

	if(argc < 4 || argc > 7) {
		WRONG_PARAM_COUNT;
	}
	switch(argc) {
		case 4:
			if(zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 5:
			if(zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 6:
			if(zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 7:
			if(zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_double_ex(arg4);
	if(argc > 4) convert_to_double_ex(arg5);
	if(argc > 5) convert_to_double_ex(arg6);
	if(argc > 6) convert_to_double_ex(arg7);

	if (0 == (strcmp(Z_STRVAL_PP(arg3), "spot"))) {
	    c1 = Z_DVAL_PP(arg4)-PDFLIB_SPOT_OFFSET;
	} else if(0 == (strcmp(Z_STRVAL_PP(arg3), "pattern"))) {
	    c1 = Z_DVAL_PP(arg4)-PDFLIB_PATTERN_OFFSET;
	} else {
	    c1 = Z_DVAL_PP(arg4);
	}

	PDF_setcolor(pdf,
		Z_STRVAL_PP(arg2),
		Z_STRVAL_PP(arg3),
		(float) c1,
		(float) ((argc>4) ? Z_DVAL_PP(arg5):0),
		(float) ((argc>5) ? Z_DVAL_PP(arg6):0),
		(float) ((argc>6) ? Z_DVAL_PP(arg7):0));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pdf_makespotcolor(int pdf, string spotname);
 * Make a named spot color from the current color. */
PHP_FUNCTION(pdf_makespotcolor)
{
	zval **arg1, **arg2;
	PDF *pdf;
	int spotcolor;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_string_ex(arg2);

	spotcolor = PDF_makespotcolor(pdf,
		Z_STRVAL_PP(arg2),
		Z_STRLEN_PP(arg2));

	RETURN_LONG(spotcolor+PDFLIB_SPOT_OFFSET);
}
/* }}} */

/* {{{ proto void pdf_arcn(int pdf, float x, float y, float r, float alpha, float beta);
 * Draw a clockwise circular arc from alpha to beta degrees. */
PHP_FUNCTION(pdf_arcn)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);

	PDF_arcn(pdf,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		(float) Z_DVAL_PP(arg6));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_initgraphics(int pdf);
 * Reset all implicit color and graphics state parameters to their defaults. */
PHP_FUNCTION(pdf_initgraphics)
{
	zval **arg1;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	PDF_initgraphics(pdf);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_add_thumbnail(int pdf, int image);
 * Add an existing image as thumbnail for the current page. */
PHP_FUNCTION(pdf_add_thumbnail)
{
	zval **arg1, **arg2;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_long_ex(arg2);

	PDF_add_thumbnail(pdf,
		Z_LVAL_PP(arg2)-PDFLIB_IMAGE_OFFSET);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void pdf_setmatrix(int pdf, float a, float b, float c, float d, float e, float f)
   Explicitly set the current transformation matrix. */
PHP_FUNCTION(pdf_setmatrix)
{ 
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PDF *pdf;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pdf, PDF *, arg1, -1, "pdf object", le_pdf);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	PDF_setmatrix(pdf,
	    (float) Z_DVAL_PP(arg2),
	    (float) Z_DVAL_PP(arg3),
	    (float) Z_DVAL_PP(arg4),
	    (float) Z_DVAL_PP(arg5),
	    (float) Z_DVAL_PP(arg6),
	    (float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */
#endif /* PDFlib >= V4 */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
