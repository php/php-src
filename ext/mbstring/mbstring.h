/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * PHP 4 Multibyte String module "mbstring" (currently only for Japanese)
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

#ifdef PHP_WIN32
#	undef MBSTRING_API
#	ifdef MBSTRING_EXPORTS
#		define MBSTRING_API __declspec(dllexport)
#	elif defined(COMPILE_DL_MBSTRING)
#		define MBSTRING_API __declspec(dllimport)
#	else
#		define MBSTRING_API /* nothing special */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	undef MBSTRING_API
#	define MBSTRING_API __attribute__ ((visibility("default")))
#else
#	undef MBSTRING_API
#	define MBSTRING_API /* nothing special */
#endif


#if HAVE_MBSTRING

#include "libmbfl/mbfl/mbfilter.h"
#include "SAPI.h"

#define PHP_MBSTRING_API 20021024

extern zend_module_entry mbstring_module_entry;
#define mbstring_module_ptr &mbstring_module_entry

PHP_MINIT_FUNCTION(mbstring);
PHP_MSHUTDOWN_FUNCTION(mbstring);
PHP_RINIT_FUNCTION(mbstring);
PHP_RSHUTDOWN_FUNCTION(mbstring);
PHP_MINFO_FUNCTION(mbstring);

/* functions in php_unicode.c */
PHP_FUNCTION(mb_convert_case);
PHP_FUNCTION(mb_strtoupper);
PHP_FUNCTION(mb_strtolower);

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
PHP_FUNCTION(mb_stripos);
PHP_FUNCTION(mb_strripos);
PHP_FUNCTION(mb_strstr);
PHP_FUNCTION(mb_strrchr);
PHP_FUNCTION(mb_stristr);
PHP_FUNCTION(mb_strrichr);
PHP_FUNCTION(mb_substr_count);
PHP_FUNCTION(mb_substr);
PHP_FUNCTION(mb_strcut);
PHP_FUNCTION(mb_strwidth);
PHP_FUNCTION(mb_strimwidth);
PHP_FUNCTION(mb_convert_encoding);
PHP_FUNCTION(mb_detect_encoding);
PHP_FUNCTION(mb_list_encodings);
PHP_FUNCTION(mb_encoding_aliases);
PHP_FUNCTION(mb_convert_kana);
PHP_FUNCTION(mb_encode_mimeheader);
PHP_FUNCTION(mb_decode_mimeheader);
PHP_FUNCTION(mb_convert_variables);
PHP_FUNCTION(mb_encode_numericentity);
PHP_FUNCTION(mb_decode_numericentity);
PHP_FUNCTION(mb_send_mail);
PHP_FUNCTION(mb_get_info);
PHP_FUNCTION(mb_check_encoding);

MBSTRING_API char *php_mb_safe_strrchr_ex(const char *s, unsigned int c,
                                    size_t nbytes, const mbfl_encoding *enc);
MBSTRING_API char *php_mb_safe_strrchr(const char *s, unsigned int c,
                                 size_t nbytes);

MBSTRING_API char * php_mb_convert_encoding(const char *input, size_t length,
                                      const char *_to_encoding,
                                      const char *_from_encodings,
                                      size_t *output_len);

MBSTRING_API int php_mb_check_encoding_list(const char *encoding_list);

MBSTRING_API size_t php_mb_mbchar_bytes_ex(const char *s, const mbfl_encoding *enc);
MBSTRING_API size_t php_mb_mbchar_bytes(const char *s);

MBSTRING_API int php_mb_encoding_detector_ex(const char *arg_string, int arg_length,
											 char *arg_list);

MBSTRING_API int php_mb_encoding_converter_ex(char **str, int *len, const char *encoding_to,
											  const char *encoding_from);
MBSTRING_API int php_mb_stripos(int mode, const char *old_haystack, unsigned int old_haystack_len, const char *old_needle, unsigned int old_needle_len, long offset, const char *from_encoding);

/* internal use only */
int _php_mb_ini_mbstring_internal_encoding_set(const char *new_value, uint new_value_length);

ZEND_BEGIN_MODULE_GLOBALS(mbstring)
	char *internal_encoding_name;
	enum mbfl_no_language language;
	const mbfl_encoding *internal_encoding;
	const mbfl_encoding *current_internal_encoding;
	const mbfl_encoding *http_output_encoding;
	const mbfl_encoding *current_http_output_encoding;
	const mbfl_encoding *http_input_identify;
	const mbfl_encoding *http_input_identify_get;
	const mbfl_encoding *http_input_identify_post;
	const mbfl_encoding *http_input_identify_cookie;
	const mbfl_encoding *http_input_identify_string;
	const mbfl_encoding **http_input_list;
	size_t http_input_list_size;
	const mbfl_encoding **detect_order_list;
	size_t detect_order_list_size;
	const mbfl_encoding **current_detect_order_list;
	size_t current_detect_order_list_size;
	enum mbfl_no_encoding *default_detect_order_list;
	size_t default_detect_order_list_size;
	int filter_illegal_mode;
	int filter_illegal_substchar;
	int current_filter_illegal_mode;
	int current_filter_illegal_substchar;
	long func_overload;
	zend_bool encoding_translation;
	long strict_detection;
	long illegalchars;
	mbfl_buffer_converter *outconv;
    void *http_output_conv_mimetypes;
#if HAVE_MBREGEX
    struct _zend_mb_regex_globals *mb_regex_globals;
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
#define MBSTRG(v) ZEND_TSRMG(mbstring_globals_id, zend_mbstring_globals *, v)
ZEND_TSRMLS_CACHE_EXTERN;
#else
#define MBSTRG(v) (mbstring_globals.v)
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
