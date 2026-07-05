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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   |          Sara Golemon <pollita@php.net>                              |
   |          Sepehr mahmoodi <sepehrphpr@gmail.com> (grapheme_mask)      |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_intl.h"
#include "intl_convert.h"
#include "intl_data.h"
#include "grapheme.h"
#include "grapheme_util.h"
#include "grapheme_arginfo.h"
#include "grapheme_mask.h"   /* ← تابع جدید grapheme_mask */

/* ************************************ *
 *   توابع عمومی موجود در ماژول
 * ************************************ */

/* {{{ PHP_FUNCTION(grapheme_strlen) */
PHP_FUNCTION(grapheme_strlen)
{
	char *str;
	size_t str_len;
	int32_t count;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	count = grapheme_count_graphemes(str, str_len);
	if (count >= 0) {
		RETURN_LONG(count);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strpos) */
PHP_FUNCTION(grapheme_strpos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long offset = 0;
	int32_t pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	pos = grapheme_strpos_utf8(haystack, haystack_len, needle, needle_len, offset, NULL);
	if (pos >= 0) {
		RETURN_LONG(pos);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_stripos) */
PHP_FUNCTION(grapheme_stripos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long offset = 0;
	int32_t pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	pos = grapheme_stripos_utf8(haystack, haystack_len, needle, needle_len, offset, NULL);
	if (pos >= 0) {
		RETURN_LONG(pos);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strrpos) */
PHP_FUNCTION(grapheme_strrpos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long offset = 0;
	int32_t pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	pos = grapheme_strrpos_utf8(haystack, haystack_len, needle, needle_len, offset, NULL);
	if (pos >= 0) {
		RETURN_LONG(pos);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strripos) */
PHP_FUNCTION(grapheme_strripos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long offset = 0;
	int32_t pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	pos = grapheme_strripos_utf8(haystack, haystack_len, needle, needle_len, offset, NULL);
	if (pos >= 0) {
		RETURN_LONG(pos);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_substr) */
PHP_FUNCTION(grapheme_substr)
{
	char *str;
	size_t str_len;
	zend_long start, length = 0;
	int32_t start_offset, end_offset, grapheme_len;
	UChar *u_str = NULL;
	UErrorCode status = U_ZERO_ERROR;
	int32_t u_str_len;
	UBreakIterator *bi = NULL;
	zend_bool length_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_LONG(start)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(length, length_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (str_len == 0) {
		RETURN_EMPTY_STRING();
	}

	/* محاسبه تعداد grapheme‌ها */
	grapheme_len = grapheme_count_graphemes(str, str_len);
	if (grapheme_len < 0) {
		RETURN_FALSE;
	}

	/* تنظیم start */
	if (start < 0) {
		start = grapheme_len + start;
		if (start < 0) {
			start = 0;
		}
	}
	if (start >= grapheme_len) {
		RETURN_FALSE;
	}

	/* تنظیم length */
	if (length_is_null) {
		length = grapheme_len - start;
	} else if (length < 0) {
		length = grapheme_len - start + length;
		if (length < 0) {
			RETURN_FALSE;
		}
	}

	if (length == 0) {
		RETURN_EMPTY_STRING();
	}

	/* تبدیل به UChar */
	u_str_len = (int32_t)str_len;
	u_str = (UChar *)emalloc(sizeof(UChar) * (u_str_len + 1));
	u_charsToUChars(str, u_str, u_str_len);
	u_str[u_str_len] = 0;

	/* ایجاد iterator */
	bi = ubrk_open(UBRK_CHARACTER, NULL, u_str, u_str_len, &status);
	if (U_FAILURE(status)) {
		efree(u_str);
		RETURN_FALSE;
	}

	/* پیدا کردن start_offset */
	start_offset = 0;
	for (int32_t i = 0; i < start; i++) {
		start_offset = ubrk_following(bi, start_offset);
		if (start_offset == UBRK_DONE) {
			ubrk_close(bi);
			efree(u_str);
			RETURN_FALSE;
		}
	}

	/* پیدا کردن end_offset */
	end_offset = start_offset;
	for (int32_t i = 0; i < length; i++) {
		end_offset = ubrk_following(bi, end_offset);
		if (end_offset == UBRK_DONE) {
			end_offset = u_str_len;
			break;
		}
	}

	ubrk_close(bi);

	/* تبدیل به UTF-8 */
	int32_t out_capacity = (end_offset - start_offset) * 3 + 1;
	char *out = (char *)emalloc(out_capacity);
	int32_t out_len;
	u_strToUTF8(out, out_capacity, &out_len, u_str + start_offset, end_offset - start_offset, &status);

	efree(u_str);

	if (U_FAILURE(status)) {
		efree(out);
		RETURN_FALSE;
	}

	out[out_len] = '\0';
	RETURN_STRINGL(out, out_len);
	efree(out);
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strstr) */
PHP_FUNCTION(grapheme_strstr)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_bool before_needle = 0;
	int32_t pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(before_needle)
	ZEND_PARSE_PARAMETERS_END();

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	pos = grapheme_strpos_utf8(haystack, haystack_len, needle, needle_len, 0, NULL);
	if (pos >= 0) {
		if (before_needle) {
			RETURN_STRINGL(haystack, pos);
		} else {
			RETURN_STRINGL(haystack + pos, haystack_len - pos);
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_stristr) */
PHP_FUNCTION(grapheme_stristr)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_bool before_needle = 0;
	int32_t pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(before_needle)
	ZEND_PARSE_PARAMETERS_END();

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	pos = grapheme_stripos_utf8(haystack, haystack_len, needle, needle_len, 0, NULL);
	if (pos >= 0) {
		if (before_needle) {
			RETURN_STRINGL(haystack, pos);
		} else {
			RETURN_STRINGL(haystack + pos, haystack_len - pos);
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_extract) */
PHP_FUNCTION(grapheme_extract)
{
	char *haystack, *next;
	size_t haystack_len;
	zend_long size, start = 0;
	int32_t extracted_len, next_offset;
	UErrorCode status = U_ZERO_ERROR;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_LONG(size)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(start)
	ZEND_PARSE_PARAMETERS_END();

	if (start < 0) {
		start = 0;
	}
	if ((size_t)start >= haystack_len) {
		RETURN_FALSE;
	}

	extracted_len = grapheme_extract_utf8(haystack + start, haystack_len - start,
										  size, &next_offset, &status);

	if (extracted_len <= 0 || U_FAILURE(status)) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(haystack + start, extracted_len);
}
/* }}} */

/* ************************************ *
 *   جدول توابع ماژول
 * ************************************ */

/* {{{ grapheme_functions[] */
static const zend_function_entry grapheme_functions[] = {
	PHP_FE(grapheme_strlen,	arginfo_grapheme_strlen)
	PHP_FE(grapheme_strpos,	arginfo_grapheme_strpos)
	PHP_FE(grapheme_stripos,	arginfo_grapheme_stripos)
	PHP_FE(grapheme_strrpos,	arginfo_grapheme_strrpos)
	PHP_FE(grapheme_strripos,	arginfo_grapheme_strripos)
	PHP_FE(grapheme_substr,	arginfo_grapheme_substr)
	PHP_FE(grapheme_strstr,	arginfo_grapheme_strstr)
	PHP_FE(grapheme_stristr,	arginfo_grapheme_stristr)
	PHP_FE(grapheme_extract,	arginfo_grapheme_extract)
	PHP_FE(grapheme_mask,		arginfo_grapheme_mask)   /* ← تابع جدید */
	PHP_FE_END
};
/* }}} */

/* ************************************ *
 *   مدیریت ماژول
 * ************************************ */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(grapheme)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(grapheme)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "grapheme support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ grapheme_module_entry */
zend_module_entry grapheme_module_entry = {
	STANDARD_MODULE_HEADER,
	"grapheme",
	grapheme_functions,
	PHP_MINIT(grapheme),
	NULL,	/* MSHUTDOWN */
	NULL,	/* RINIT */
	NULL,	/* RSHUTDOWN */
	PHP_MINFO(grapheme),
	PHP_GRAPHEME_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GRAPHEME
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(grapheme)
#endif
