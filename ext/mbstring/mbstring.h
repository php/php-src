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
 * PHP4 Multibyte String module "mbstring" (currently only for Japanese)
 *
 * History:
 *   2000.5.19  Release php-4.0RC2_jstring-1.0
 *   2001.4.1   Release php4_jstring-1.0.91
 *   2001.4.30  Release php4-jstring-1.1 (contribute to The PHP Group)
 *   2001.5.1   Renamed from jstring to mbstring (hirokawa@php.net)
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


#ifndef _MBSTRING_H
#define _MBSTRING_H

#ifdef COMPILE_DL_MBSTRING
#undef HAVE_MBSTRING
#define HAVE_MBSTRING 1
#endif

#if HAVE_MBSTRING

#include "mbfilter.h"

extern zend_module_entry mbstring_module_entry;
#define mbstring_module_ptr &mbstring_module_entry

extern PHP_MINIT_FUNCTION(mbstring);
extern PHP_MSHUTDOWN_FUNCTION(mbstring);
extern PHP_RINIT_FUNCTION(mbstring);
extern PHP_RSHUTDOWN_FUNCTION(mbstring);
PHP_MINFO_FUNCTION(mbstring);

/* php function registration */
PHP_FUNCTION(mb_language);
PHP_FUNCTION(mb_internal_encoding);
PHP_FUNCTION(mb_http_input);
PHP_FUNCTION(mb_http_output);
PHP_FUNCTION(mb_detect_order);
PHP_FUNCTION(mb_substitute_character);
PHP_FUNCTION(mb_preferred_mime_name);
PHP_FUNCTION(mb_gpc_handler);
PHP_FUNCTION(mb_output_handler);
PHP_FUNCTION(mb_strlen);
PHP_FUNCTION(mb_strpos);
PHP_FUNCTION(mb_strrpos);
PHP_FUNCTION(mb_substr);
PHP_FUNCTION(mb_strcut);
PHP_FUNCTION(mb_strwidth);
PHP_FUNCTION(mb_strimwidth);
PHP_FUNCTION(mb_convert_encoding);
PHP_FUNCTION(mb_detect_encoding);
PHP_FUNCTION(mb_convert_kana);
PHP_FUNCTION(mb_encode_mimeheader);
PHP_FUNCTION(mb_decode_mimeheader);
PHP_FUNCTION(mb_convert_variables);
PHP_FUNCTION(mb_encode_numericentity);
PHP_FUNCTION(mb_decode_numericentity);
PHP_FUNCTION(mb_send_mail);

ZEND_BEGIN_MODULE_GLOBALS(mbstring)
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
	int http_input_identify_string;
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
ZEND_END_MODULE_GLOBALS(mbstring);


#ifdef ZTS
#define MBSTRLS_D zend_mbstring_globals *mbstring_globals
#define MBSTRLS_DC , MBSTRLS_D
#define MBSTRLS_C mbstring_globals
#define MBSTRLS_CC , MBSTRLS_C
#define MBSTRG(v) (mbstring_globals->v)
#define MBSTRLS_FETCH() zend_mbstring_globals *mbstring_globals = ts_resource(mbstring_globals_id)
#else
#define MBSTRLS_D
#define MBSTRLS_DC
#define MBSTRLS_C
#define MBSTRLS_CC
#define MBSTRG(v) (mbstring_globals.v)
#define MBSTRLS_FETCH()
#endif

#else	/* HAVE_MBSTRING */

#define mbstring_module_ptr NULL

#endif	/* HAVE_MBSTRING */

#define phpext_mbstring_ptr mbstring_module_ptr

#endif		/* _MBSTRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
