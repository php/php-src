/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
   |         Hironori Sato <satoh@jpnnet.com>                             |
   |         Shigeru Kanemoto <sgk@happysize.co.jp>                       |
   +----------------------------------------------------------------------+
 */

#ifndef _MBSTRING_H
#define _MBSTRING_H

#ifdef COMPILE_DL_MBSTRING
#undef HAVE_MBSTRING
#define HAVE_MBSTRING 1
#endif

#include "php_version.h"
#define PHP_MBSTRING_VERSION PHP_VERSION

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
PHP_FUNCTION(mb_str_split);
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
PHP_FUNCTION(mb_ord);
PHP_FUNCTION(mb_chr);
PHP_FUNCTION(mb_scrub);


MBSTRING_API char *php_mb_safe_strrchr_ex(const char *s, unsigned int c,
                                    size_t nbytes, const mbfl_encoding *enc);
MBSTRING_API char *php_mb_safe_strrchr(const char *s, unsigned int c,
                                 size_t nbytes);

MBSTRING_API char *php_mb_convert_encoding_ex(
		const char *input, size_t length,
		const mbfl_encoding *to_encoding, const mbfl_encoding *from_encoding, size_t *output_len);
MBSTRING_API char * php_mb_convert_encoding(const char *input, size_t length,
                                      const mbfl_encoding *to_encoding,
                                      const char *_from_encodings,
                                      size_t *output_len);

MBSTRING_API size_t php_mb_mbchar_bytes_ex(const char *s, const mbfl_encoding *enc);
MBSTRING_API size_t php_mb_mbchar_bytes(const char *s);

MBSTRING_API size_t php_mb_stripos(int mode, const char *old_haystack, size_t old_haystack_len, const char *old_needle, size_t old_needle_len, zend_long offset, zend_string *from_encoding);
MBSTRING_API int php_mb_check_encoding(const char *input, size_t length, const char *enc);

ZEND_BEGIN_MODULE_GLOBALS(mbstring)
	char *internal_encoding_name;
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
	enum mbfl_no_language language;
	zend_bool encoding_translation;
	zend_bool strict_detection;
	size_t illegalchars;
	mbfl_buffer_converter *outconv;
    void *http_output_conv_mimetypes;
#if HAVE_MBREGEX
    struct _zend_mb_regex_globals *mb_regex_globals;
    zend_long regex_stack_limit;
#endif
	zend_string *last_used_encoding_name;
	const mbfl_encoding *last_used_encoding;
	/* Whether an explicit internal_encoding / http_output / http_input encoding was set. */
	zend_bool internal_encoding_set;
	zend_bool http_output_set;
	zend_bool http_input_set;
#if HAVE_MBREGEX
    zend_long regex_retry_limit;
#endif
ZEND_END_MODULE_GLOBALS(mbstring)

#define MBSTRG(v) ZEND_MODULE_GLOBALS_ACCESSOR(mbstring, v)

#if defined(ZTS) && defined(COMPILE_DL_MBSTRING)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#else	/* HAVE_MBSTRING */

#define mbstring_module_ptr NULL

#endif	/* HAVE_MBSTRING */

#define phpext_mbstring_ptr mbstring_module_ptr

#endif		/* _MBSTRING_H */
