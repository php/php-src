/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
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
   | Author: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>              |
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
#undef PHPAPI
#define PHPAPI
#endif

#if HAVE_MBSTRING

#include "mbfilter.h"
#include "SAPI.h"

#define PHP_MBSTRING_API 20020405

PHPAPI char * php_mb_convert_encoding(char *input, size_t length, char *_to_encoding, char *_from_encodings, size_t *output_len TSRMLS_DC);
PHPAPI int php_mb_check_encoding_list(const char *encoding_list TSRMLS_DC);

#if HAVE_MBREGEX
#include "mbregex.h"
#endif

extern zend_module_entry mbstring_module_entry;
#define mbstring_module_ptr &mbstring_module_entry

PHP_MINIT_FUNCTION(mbstring);
PHP_MSHUTDOWN_FUNCTION(mbstring);
PHP_RINIT_FUNCTION(mbstring);
PHP_RSHUTDOWN_FUNCTION(mbstring);
PHP_MINFO_FUNCTION(mbstring);

/* functions in php_unicode.c */
PHP_FUNCTION(mb_convert_case);

/* php function registration */
PHP_FUNCTION(mb_language);
PHP_FUNCTION(mb_internal_encoding);
PHP_FUNCTION(mb_http_input);
PHP_FUNCTION(mb_http_output);
PHP_FUNCTION(mb_detect_order);
PHP_FUNCTION(mb_substitute_character);
PHP_FUNCTION(mb_preferred_mime_name);
PHP_FUNCTION(mb_parse_str);
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
PHP_FUNCTION(mb_get_info);
#if HAVE_MBREGEX
PHP_FUNCTION(mb_regex_encoding);
PHP_FUNCTION(mb_ereg);
PHP_FUNCTION(mb_eregi);
PHP_FUNCTION(mb_ereg_replace);
PHP_FUNCTION(mb_eregi_replace);
PHP_FUNCTION(mb_split);
PHP_FUNCTION(mb_ereg_match);
PHP_FUNCTION(mb_ereg_search);
PHP_FUNCTION(mb_ereg_search_pos);
PHP_FUNCTION(mb_ereg_search_regs);
PHP_FUNCTION(mb_ereg_search_init);
PHP_FUNCTION(mb_ereg_search_getregs);
PHP_FUNCTION(mb_ereg_search_getpos);
PHP_FUNCTION(mb_ereg_search_setpos);
#endif

#if HAVE_MBREGEX
#define PHP_MBREGEX_MAXCACHE 50
int php_mbregex_name2mbctype(const char *pname);
#endif

char *mbstr_strrchr(const char *s, char c TSRMLS_DC);
int mbstr_is_mb_leadbyte(const char *s TSRMLS_DC);
PHPAPI int mbstr_encoding_translation(TSRMLS_D);

ZEND_BEGIN_MODULE_GLOBALS(mbstring)
	int language;
	int current_language;
	int internal_encoding;
	int current_internal_encoding;
#ifdef ZEND_MULTIBYTE
	int *script_encoding_list;
	int script_encoding_list_size;
#endif /* ZEND_MULTIBYTE */
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
	long func_overload;
	int encoding_translation;
	mbfl_buffer_converter *outconv;
#if HAVE_MBREGEX
	int default_mbctype;
	int current_mbctype;
	HashTable ht_rc;
	zval **search_str;
	zval *search_str_val;
	unsigned int search_pos;
	mb_regex_t *search_re;
	struct mbre_registers *search_regs;
#endif
ZEND_END_MODULE_GLOBALS(mbstring)

#define MB_OVERLOAD_MAIL 1
#define MB_OVERLOAD_STRING 2
#define MB_OVERLOAD_REGEX 4

struct mb_overload_def {
	int type;
	char *orig_func;
	char *ovld_func;
	char *save_func;
};

#ifdef ZTS
#define MBSTRG(v) TSRMG(mbstring_globals_id, zend_mbstring_globals *, v)
#else
#define MBSTRG(v) (mbstring_globals.v)
#endif

#ifdef ZEND_MULTIBYTE
PHPAPI int php_mbstring_set_zend_encoding(TSRMLS_D);
char* php_mbstring_encoding_detector(char *string, int length, char *list
		TSRMLS_DC);
int php_mbstring_encoding_converter(char **to, int *to_length, char *from,
		int from_length, const char *encoding_to, const char *encoding_from
		TSRMLS_DC);
int php_mbstring_oddlen(char *string, int length, const char *encoding TSRMLS_DC);
#endif /* ZEND_MULTIBYTE */

SAPI_POST_HANDLER_FUNC(php_mbstr_post_handler);
SAPI_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data);

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
