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

#include "libmbfl/mbfl/mbfilter.h"
#include "SAPI.h"

#define PHP_MBSTRING_API 20021024

extern zend_module_entry mbstring_module_entry;
#define phpext_mbstring_ptr &mbstring_module_entry

PHP_MINIT_FUNCTION(mbstring);
PHP_MSHUTDOWN_FUNCTION(mbstring);
PHP_RINIT_FUNCTION(mbstring);
PHP_RSHUTDOWN_FUNCTION(mbstring);
PHP_MINFO_FUNCTION(mbstring);

MBSTRING_API char *php_mb_safe_strrchr_ex(const char *s, unsigned int c,
                                    size_t nbytes, const mbfl_encoding *enc);
MBSTRING_API char *php_mb_safe_strrchr(const char *s, unsigned int c,
                                 size_t nbytes);

MBSTRING_API char *php_mb_convert_encoding_ex(
		const char *input, size_t length,
		const mbfl_encoding *to_encoding, const mbfl_encoding *from_encoding, size_t *output_len);
MBSTRING_API char * php_mb_convert_encoding(
		const char *input, size_t length, const mbfl_encoding *to_encoding,
		const mbfl_encoding **from_encodings, size_t num_from_encodings, size_t *output_len);

MBSTRING_API size_t php_mb_mbchar_bytes_ex(const char *s, const mbfl_encoding *enc);
MBSTRING_API size_t php_mb_mbchar_bytes(const char *s);

MBSTRING_API size_t php_mb_stripos(int mode, const char *old_haystack, size_t old_haystack_len, const char *old_needle, size_t old_needle_len, zend_long offset, const mbfl_encoding *encoding);
MBSTRING_API int php_mb_check_encoding(const char *input, size_t length, const mbfl_encoding *encoding);

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
#ifdef HAVE_MBREGEX
    struct _zend_mb_regex_globals *mb_regex_globals;
    zend_long regex_stack_limit;
#endif
	zend_string *last_used_encoding_name;
	const mbfl_encoding *last_used_encoding;
	/* Whether an explicit internal_encoding / http_output / http_input encoding was set. */
	zend_bool internal_encoding_set;
	zend_bool http_output_set;
	zend_bool http_input_set;
#ifdef HAVE_MBREGEX
    zend_long regex_retry_limit;
#endif
ZEND_END_MODULE_GLOBALS(mbstring)

#define MBSTRG(v) ZEND_MODULE_GLOBALS_ACCESSOR(mbstring, v)

#if defined(ZTS) && defined(COMPILE_DL_MBSTRING)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* _MBSTRING_H */
