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
   |          Sepehr Lajevardi <sepehrphpr@gmail.com> (grapheme_mask)     |
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
#include "grapheme_mask.h"

#include <unicode/ubrk.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>
#include <unicode/utf8.h>

/* {{{ Defines */
#define GRAPHEME_MAX_BYTES 4
/* }}} */

/* {{{ Internal function prototypes */
static int32_t grapheme_count_graphemes(const char *str, size_t str_len);
static int32_t grapheme_find_boundary(const char *str, size_t str_len, int32_t offset, int direction);
/* }}} */

/* {{{ grapheme_count_graphemes */
static int32_t grapheme_count_graphemes(const char *str, size_t str_len)
{
	UErrorCode status = U_ZERO_ERROR;
	UBreakIterator *bi;
	int32_t count = 0;
	UChar *u_str;
	int32_t u_str_len;

	if (str_len == 0) {
		return 0;
	}

	u_str = (UChar *)emalloc(sizeof(UChar) * (str_len + 1));
	u_charsToUChars(str, u_str, (int32_t)str_len);
	u_str[str_len] = 0;
	u_str_len = (int32_t)str_len;

	bi = ubrk_open(UBRK_CHARACTER, NULL, u_str, u_str_len, &status);
	if (U_FAILURE(status)) {
		efree(u_str);
		return -1;
	}

	while (ubrk_next(bi) != UBRK_DONE) {
		count++;
	}

	ubrk_close(bi);
	efree(u_str);
	return count;
}
/* }}} */

/* {{{ grapheme_find_boundary */
static int32_t grapheme_find_boundary(const char *str, size_t str_len, int32_t offset, int direction)
{
	UErrorCode status = U_ZERO_ERROR;
	UBreakIterator *bi;
	UChar *u_str;
	int32_t u_str_len, pos = 0, grapheme_count = 0, target = offset;

	if (str_len == 0) {
		return -1;
	}

	u_str = (UChar *)emalloc(sizeof(UChar) * (str_len + 1));
	u_charsToUChars(str, u_str, (int32_t)str_len);
	u_str[str_len] = 0;
	u_str_len = (int32_t)str_len;

	bi = ubrk_open(UBRK_CHARACTER, NULL, u_str, u_str_len, &status);
	if (U_FAILURE(status)) {
		efree(u_str);
		return -1;
	}

	if (direction > 0) {
		while (grapheme_count < target && (pos = ubrk_next(bi)) != UBRK_DONE) {
			grapheme_count++;
		}
	} else {
		pos = u_str_len;
		ubrk_last(bi);
		while (grapheme_count < (-target) && (pos = ubrk_previous(bi)) != UBRK_DONE) {
			grapheme_count++;
		}
	}

	ubrk_close(bi);
	efree(u_str);
	if (grapheme_count != target) {
		return -1;
	}
	return pos;
}
/* }}} */

/* {{{ grapheme_mask_validate_mask_char */
UBool grapheme_mask_validate_mask_char(const char *mask_char, size_t mask_char_len, UErrorCode *status)
{
	if (mask_char_len == 0) {
		return false;
	}
	UChar* uchars = NULL;
	int32_t capacity = (int32_t)(mask_char_len + 1);
	uchars = (UChar*) emalloc(capacity * sizeof(UChar));
	u_charsToUChars(mask_char, uchars, (int32_t)mask_char_len);
	uchars[mask_char_len] = 0;

	UBreakIterator* bi = ubrk_open(UBRK_CHARACTER, NULL, uchars, mask_char_len, status);
	if (U_FAILURE(*status)) {
		efree(uchars);
		return false;
	}
	int32_t firstBoundary = ubrk_following(bi, 0);
	ubrk_close(bi);
	efree(uchars);

	if (firstBoundary == UBRK_DONE || (size_t)firstBoundary != mask_char_len) {
		return false;
	}
	return true;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_mask) */
PHP_FUNCTION(grapheme_mask)
{
	char *str, *mask, *mask_char = "*";
	size_t str_len, mask_len, mask_char_len = 1;
	zend_long start = 0, length = 0;
	int32_t start_offset, end_offset, grapheme_count;
	int32_t result_len, result_pos, str_pos, mask_itr;
	UBreakIterator *bi, *str_bi;
	UChar *u_str, *u_mask, *u_result, *u_mask_char;
	UErrorCode status = U_ZERO_ERROR;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(str)
		Z_PARAM_STR(mask)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(start)
		Z_PARAM_LONG(length)
		Z_PARAM_STR_DEFAULT(mask_char, "*")
	ZEND_PARSE_PARAMETERS_END();

	/* Validate mask_char */
	if (!grapheme_mask_validate_mask_char(mask_char->val, mask_char->len, &status)) {
		zend_argument_value_error(5, "must be a single grapheme cluster, i.e. an individual character or a valid multi-byte sequence");
		RETURN_THROWS();
	}

	/* Convert strings to UChar */
	int32_t u_str_len = (int32_t)str_len;
	int32_t u_mask_len = (int32_t)mask_len;
	u_str = (UChar *)emalloc(sizeof(UChar) * (u_str_len + 1));
	u_mask = (UChar *)emalloc(sizeof(UChar) * (u_mask_len + 1));
	u_charsToUChars(str, u_str, u_str_len);
	u_str[u_str_len] = 0;
	u_charsToUChars(mask, u_mask, u_mask_len);
	u_mask[u_mask_len] = 0;

	/* Convert mask_char */
	u_mask_char = (UChar *)emalloc(sizeof(UChar) * (mask_char_len + 1));
	u_charsToUChars(mask_char, u_mask_char, (int32_t)mask_char_len);
	u_mask_char[mask_char_len] = 0;

	/* Break iterator for mask */
	bi = ubrk_open(UBRK_CHARACTER, NULL, u_mask, u_mask_len, &status);
	if (U_FAILURE(status)) {
		efree(u_str); efree(u_mask); efree(u_mask_char);
		zend_throw_error(NULL, "grapheme_mask: Error creating ICU break iterator");
		return;
	}

	/* Determine grapheme start/end indices */
	grapheme_count = grapheme_count_graphemes(str, str_len);
	if (start < 0) {
		start = grapheme_count + start;
		if (start < 0) start = 0;
	}
	if (ZEND_NUM_ARGS() < 4) {
		end_offset = grapheme_count;
	} else {
		if (length < 0) {
			length = grapheme_count - start + length;
			if (length < 0) length = 0;
		}
		end_offset = (int32_t)(start + length);
		if (end_offset > grapheme_count) end_offset = grapheme_count;
	}

	/* Create str_bi for string */
	str_bi = ubrk_open(UBRK_CHARACTER, NULL, u_str, u_str_len, &status);
	if (U_FAILURE(status)) {
		efree(u_str); efree(u_mask); efree(u_mask_char);
		ubrk_close(bi);
		zend_throw_error(NULL, "grapheme_mask: Error creating ICU break iterator for string");
		return;
	}

	start_offset = 0;
	for (int i = 0; i < start; i++) {
		start_offset = ubrk_following(str_bi, start_offset);
	}
	end_offset = start_offset;
	for (int i = start; i < end_offset; i++) {
		end_offset = ubrk_following(str_bi, end_offset);
	}

	/* Phase 1: allocate result buffer */
	result_len = u_str_len * (int32_t)mask_char_len + 1;
	u_result = (UChar *)emalloc(sizeof(UChar) * result_len);
	result_pos = 0;
	str_pos = 0;
	mask_itr = 0;
	UBool mask_done = 0;

	/* Phase 2: prefix */
	while (str_pos < start_offset) {
		u_result[result_pos++] = u_str[str_pos++];
	}

	/* Phase 3: masked region */
	while (str_pos < end_offset && !mask_done) {
		int32_t next_mask_boundary = ubrk_following(bi, mask_itr);
		if (next_mask_boundary == UBRK_DONE) {
			mask_done = 1;
			break;
		}
		int32_t next_str_boundary = ubrk_following(str_bi, str_pos);
		if (next_str_boundary == UBRK_DONE) break;

		/* Overwrite with mask_char */
		for (int c = 0; c < (int32_t)mask_char_len; c++) {
			u_result[result_pos++] = u_mask_char[c];
		}
		str_pos = next_str_boundary;
		mask_itr = next_mask_boundary;
	}

	/* Phase 4: suffix */
	while (str_pos < u_str_len) {
		u_result[result_pos++] = u_str[str_pos++];
	}
	u_result[result_pos] = 0;

	/* Convert back to UTF-8 */
	int32_t out_len = 0;
	char *out = (char *)emalloc(result_pos * 4 + 1);
	u_strToUTF8(out, result_pos * 4, &out_len, u_result, result_pos, &status);
	if (U_FAILURE(status)) {
		efree(u_str); efree(u_mask); efree(u_mask_char); efree(u_result); efree(out);
		ubrk_close(bi); ubrk_close(str_bi);
		zend_throw_error(NULL, "grapheme_mask: UTF-8 conversion error");
		return;
	}
	out[out_len] = '\0';

	/* Cleanup */
	efree(u_str);
	efree(u_mask);
	efree(u_mask_char);
	efree(u_result);
	ubrk_close(bi);
	ubrk_close(str_bi);

	RETURN_STRINGL(out, out_len);
	efree(out);
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strlen) */
PHP_FUNCTION(grapheme_strlen)
{
	char *str;
	size_t str_len;
	int32_t count;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
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
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	pos = grapheme_find_boundary(haystack, haystack_len, (int32_t)offset, 1);
	if (pos >= 0) {
		/* Simplistic search; actual PHP implementation uses u_strstr etc. */
		char *found = zend_memnstr(haystack + pos, needle, needle_len, haystack + haystack_len);
		if (found) {
			RETURN_LONG(found - haystack);
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_stripos) */
PHP_FUNCTION(grapheme_stripos)
{
	/* Placeholder - would need ICU case-insensitive search */
	zend_throw_error(NULL, "grapheme_stripos not implemented yet");
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strrpos) */
PHP_FUNCTION(grapheme_strrpos)
{
	/* Placeholder */
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strripos) */
PHP_FUNCTION(grapheme_strripos)
{
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_substr) */
PHP_FUNCTION(grapheme_substr)
{
	char *str;
	size_t str_len;
	zend_long start, length = 0;
	int32_t start_offset, end_offset;
	UBreakIterator *bi;
	UChar *u_str;
	UErrorCode status = U_ZERO_ERROR;
	int32_t u_str_len, pos;
	zend_bool length_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(str)
		Z_PARAM_LONG(start)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(length, length_is_null)
	ZEND_PARSE_PARAMETERS_END();

	u_str_len = (int32_t)str_len;
	u_str = (UChar *)emalloc(sizeof(UChar) * (u_str_len + 1));
	u_charsToUChars(str, u_str, u_str_len);
	u_str[u_str_len] = 0;

	bi = ubrk_open(UBRK_CHARACTER, NULL, u_str, u_str_len, &status);
	if (U_FAILURE(status)) {
		efree(u_str);
		RETURN_FALSE;
	}

	int32_t count = 0;
	start_offset = 0;
	if (start < 0) {
		start = grapheme_count_graphemes(str, str_len) + start;
		if (start < 0) start = 0;
	}
	for (count = 0; count < start; count++) {
		start_offset = ubrk_following(bi, start_offset);
	}

	if (length_is_null) {
		end_offset = u_str_len;
	} else {
		if (length < 0) {
			length = grapheme_count_graphemes(str, str_len) - start + length;
			if (length < 0) length = 0;
		}
		end_offset = start_offset;
		for (count = 0; count < length && end_offset < u_str_len; count++) {
			end_offset = ubrk_following(bi, end_offset);
		}
	}

	pos = 0;
	ubrk_close(bi);

	int32_t out_len = end_offset - start_offset;
	char *out = (char *)emalloc(out_len + 1);
	u_strToUTF8(out, out_len + 1, &out_len, u_str + start_offset, end_offset - start_offset, &status);
	if (U_FAILURE(status)) {
		efree(u_str);
		efree(out);
		RETURN_FALSE;
	}
	out[out_len] = '\0';
	efree(u_str);
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
	char *found;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(before_needle)
	ZEND_PARSE_PARAMETERS_END();

	found = zend_memnstr(haystack, needle, needle_len, haystack + haystack_len);
	if (found) {
		if (before_needle) {
			RETURN_STRINGL(haystack, found - haystack);
		} else {
			RETURN_STRINGL(found, haystack_len - (found - haystack));
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_stristr) */
PHP_FUNCTION(grapheme_stristr)
{
	zend_throw_error(NULL, "grapheme_stristr not implemented yet");
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_extract) */
PHP_FUNCTION(grapheme_extract)
{
	/* Stub */
	RETURN_FALSE;
}
/* }}} */

/* {{{ grapheme_functions[] */
static const zend_function_entry grapheme_functions[] = {
	PHP_FE(grapheme_strlen, arginfo_grapheme_strlen)
	PHP_FE(grapheme_strpos, arginfo_grapheme_strpos)
	PHP_FE(grapheme_stripos, arginfo_grapheme_stripos)
	PHP_FE(grapheme_strrpos, arginfo_grapheme_strrpos)
	PHP_FE(grapheme_strripos, arginfo_grapheme_strripos)
	PHP_FE(grapheme_substr, arginfo_grapheme_substr)
	PHP_FE(grapheme_strstr, arginfo_grapheme_strstr)
	PHP_FE(grapheme_stristr, arginfo_grapheme_stristr)
	PHP_FE(grapheme_extract, arginfo_grapheme_extract)
	PHP_FE(grapheme_mask, arginfo_grapheme_mask)
	PHP_FE_END
};
/* }}} */

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
	NULL, /* MSHUTDOWN */
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
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
