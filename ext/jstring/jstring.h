/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 The PHP Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * PHP4 Japanese String module "jstring"
 *
 * History:
 *   2000.5.19  Release php-4.0RC2_jstring-1.0
 *   2001.4.1   Release php4_jstring-1.0.91
 *   2001.4.30  Release php4-jstring-1.1 (contribute to The PHP Group)
 */

/*
 * PHP3 Internationalization support program.
 *
 * Copyright (c) 1999,2000 by the PHP3 internationalization team.
 * All rights reserved.
 *
 * See README_PHP3-i18n-ja for more detail.
 *
 * Authors:
 *    Hironori Sato <satoh@jpnnet.com>
 *    Shigeru Kanemoto <sgk@happysize.co.jp>
 *    Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>
 */


#ifndef _JSTRING_H
#define _JSTRING_H

#ifdef COMPILE_DL_JSTRING
#undef HAVE_JSTRING
#define HAVE_JSTRING 1
#endif

#if HAVE_JSTRING

#include "mbfilter.h"

extern zend_module_entry jstring_module_entry;
#define jstring_module_ptr &jstring_module_entry

extern PHP_MINIT_FUNCTION(jstring);
extern PHP_MSHUTDOWN_FUNCTION(jstring);
extern PHP_RINIT_FUNCTION(jstring);
extern PHP_RSHUTDOWN_FUNCTION(jstring);
PHP_MINFO_FUNCTION(jstring);

/* php function registration */
PHP_FUNCTION(jstr_language);
PHP_FUNCTION(jstr_internal_encoding);
PHP_FUNCTION(jstr_http_input);
PHP_FUNCTION(jstr_http_output);
PHP_FUNCTION(jstr_detect_order);
PHP_FUNCTION(jstr_substitute_character);
PHP_FUNCTION(jstr_preferred_mime_name);
PHP_FUNCTION(jstr_gpc_handler);
PHP_FUNCTION(jstr_output_handler);
PHP_FUNCTION(jstr_strlen);
PHP_FUNCTION(jstr_strpos);
PHP_FUNCTION(jstr_strrpos);
PHP_FUNCTION(jstr_substr);
PHP_FUNCTION(jstr_strcut);
PHP_FUNCTION(jstr_strwidth);
PHP_FUNCTION(jstr_strimwidth);
PHP_FUNCTION(jstr_convert_encoding);
PHP_FUNCTION(jstr_detect_encoding);
PHP_FUNCTION(jstr_convert_kana);
PHP_FUNCTION(jstr_encode_mimeheader);
PHP_FUNCTION(jstr_decode_mimeheader);
PHP_FUNCTION(jstr_convert_variables);
PHP_FUNCTION(jstr_encode_numericentity);
PHP_FUNCTION(jstr_decode_numericentity);
PHP_FUNCTION(jstr_send_mail);

ZEND_BEGIN_MODULE_GLOBALS(jstring)
	int language;
	int current_language;
	int internal_encoding;
	int current_internal_encoding;
	int http_output_encoding;
	int current_http_output_encoding;
	int http_input_identify;
	int http_input_identify_get;
	int http_input_identify_post;
	int http_input_identify_cookie;
	int *http_input_list;
	int http_input_list_size;
	int *detect_order_list;
	int detect_order_list_size;
	int *current_detect_order_list;
	int current_detect_order_list_size;
	int filter_illegal_mode;
	int filter_illegal_substchar;
	int current_filter_illegal_mode;
	int current_filter_illegal_substchar;
	mbfl_buffer_converter *outconv;
ZEND_END_MODULE_GLOBALS(jstring);


#ifdef ZTS
#define JSTRLS_D zend_jstring_globals *jstring_globals
#define JSTRLS_DC , JSTRLS_D
#define JSTRLS_C jstring_globals
#define JSTRLS_CC , JSTRLS_C
#define JSTRG(v) (jstring_globals->v)
#define JSTRLS_FETCH() zend_jstring_globals *jstring_globals = ts_resource(jstring_globals_id)
#else
#define JSTRLS_D
#define JSTRLS_DC
#define JSTRLS_C
#define JSTRLS_CC
#define JSTRG(v) (jstring_globals.v)
#define JSTRLS_FETCH()
#endif

#else	/* HAVE_JSTRING */

#define jstring_module_ptr NULL

#endif	/* HAVE_JSTRING */

#define phpext_jstring_ptr jstring_module_ptr

#endif		/* _JSTRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
