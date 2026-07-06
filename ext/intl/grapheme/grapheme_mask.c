/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sepehr <sepehrphpr@gmail.com>                                |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_intl.h"
#include "intl_common.h"
#include "intl_error.h"
#include "grapheme/grapheme.h"
#include "grapheme/grapheme_util.h"

#include <unicode/utypes.h>
#include <unicode/utf8.h>
#include <unicode/utext.h>
#include <unicode/ubrk.h>
#include <unicode/ustring.h>

/* {{{ Checks if a string is valid UTF-8 */
static zend_bool grapheme_mask_is_valid_utf8(const char *str, size_t str_len)
{
	UChar32 c;
	int32_t i = 0;

	while (i < (int32_t)str_len) {
		U8_NEXT(str, i, (int32_t)str_len, c);
		if (c < 0) {
			return 0;
		}
	}
	return 1;
}
/* }}} */

/* {{{ Masks grapheme clusters in a UTF-8 string */
PHP_FUNCTION(grapheme_mask)
{
	char *str, *mask_str;
	size_t str_len, mask_len;
	zend_long start = 0, length = 0;
	zend_bool start_is_null = 1, length_is_null = 1;

	/* Parse parameters: string, mask_string, [start, [length]] */
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_STRING(mask_str, mask_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_LONG(start, start_is_null)
		Z_PARAM_ZVAL_OR_LONG(length, length_is_null)
	ZEND_PARSE_PARAMETERS_END();

	/* Validate UTF-8 for the input string */
	if (str_len > 0 && !grapheme_mask_is_valid_utf8(str, str_len)) {
		php_error_docref(NULL, E_WARNING, "Input string is not valid UTF-8");
		RETURN_FALSE;
	}

	/* If start is beyond string, return empty string */
	if (start >= (zend_long)str_len) {
		RETURN_EMPTY_STRING();
	}

	if (mask_len == 0) {
		php_error_docref(NULL, E_WARNING, "Mask string must not be empty");
		RETURN_FALSE;
	}

	/* Validate UTF-8 for the mask string */
	if (!grapheme_mask_is_valid_utf8(mask_str, mask_len)) {
		php_error_docref(NULL, E_WARNING, "Mask string is not valid UTF-8");
		RETURN_FALSE;
	}

	/* Open UText for the input string */
	UErrorCode status = U_ZERO_ERROR;
	UText *ut = utext_openUTF8(NULL, str, (int64_t)str_len, &status);
	if (U_FAILURE(status)) {
		php_error_docref(NULL, E_WARNING, "Failed to open UText for input string");
		RETURN_FALSE;
	}

	/* Get grapheme break iterator */
	UBreakIterator *bi = grapheme_get_break_iterator(NULL, &status);
	if (U_FAILURE(status)) {
		utext_close(ut);
		php_error_docref(NULL, E_WARNING, "Failed to create grapheme break iterator");
		RETURN_FALSE;
	}

	/* Set the break iterator text */
	ubrk_setUText(bi, ut, &status);
	if (U_FAILURE(status)) {
		ubrk_close(bi);
		utext_close(ut);
		php_error_docref(NULL, E_WARNING, "Failed to set text for break iterator");
		RETURN_FALSE;
	}

	/* Collect grapheme boundaries */
	int32_t *boundaries;
	int32_t boundary_count;
	int32_t buflen = 128;
	int32_t pos;

	boundaries = (int32_t *)emalloc(sizeof(int32_t) * (size_t)buflen);
	boundary_count = 0;
	boundaries[boundary_count++] = 0;

	pos = ubrk_first(bi);
	while (pos != UBRK_DONE) {
		pos = ubrk_next(bi);
		if (pos != UBRK_DONE) {
			if (boundary_count >= buflen) {
				buflen *= 2;
				boundaries = (int32_t *)erealloc(boundaries, sizeof(int32_t) * (size_t)buflen);
			}
			boundaries[boundary_count++] = pos;
		}
	}

	/* Determine range to mask */
	int32_t range_start, range_end;
	if (start_is_null) {
		range_start = 0;
	} else {
		if (start < 0) {
			range_start = boundary_count - 1 + (int32_t)start;
			if (range_start < 0) range_start = 0;
		} else {
			range_start = (int32_t)start;
			if (range_start >= boundary_count) range_start = boundary_count - 1;
		}
	}

	if (length_is_null) {
		range_end = boundary_count - 1;
	} else {
		if (length < 0) {
			range_end = boundary_count - 1 + (int32_t)length;
			if (range_end < 0) range_end = 0;
		} else {
			range_end = range_start + (int32_t)length;
			if (range_end > boundary_count - 1) range_end = boundary_count - 1;
		}
	}

	/* Build result */
	zend_string *result;

	/* If no graphemes to mask, return original string */
	if (range_start >= range_end || range_start >= boundary_count - 1) {
		efree(boundaries);
		ubrk_close(bi);
		utext_close(ut);
		RETURN_STRINGL(str, str_len);
	}

	/* Prefix: from byte 0 to the start of the range */
	int32_t prefix_len = boundaries[range_start];
	/* Suffix: from the end of the range to the end of the string */
	int32_t suffix_start = boundaries[range_end];
	int32_t suffix_len = (int32_t)str_len - suffix_start;

	/* Calculate number of graphemes to mask */
	int32_t graphemes_to_mask = range_end - range_start;

	/* Calculate total result length */
	size_t result_len = (size_t)prefix_len + ((size_t)graphemes_to_mask * mask_len) + (size_t)suffix_len;

	result = zend_string_alloc(result_len, 0);

	/* Copy prefix */
	if (prefix_len > 0) {
		memcpy(ZSTR_VAL(result), str, (size_t)prefix_len);
	}

	/* Copy mask for each grapheme */
	char *dest = ZSTR_VAL(result) + prefix_len;
	for (int32_t i = 0; i < graphemes_to_mask; i++) {
		memcpy(dest, mask_str, mask_len);
		dest += mask_len;
	}

	/* Copy suffix */
	if (suffix_len > 0) {
		memcpy(dest, str + suffix_start, (size_t)suffix_len);
	}

	ZSTR_VAL(result)[result_len] = '\0';

	efree(boundaries);
	ubrk_close(bi);
	utext_close(ut);

	RETURN_STR(result);
}
/* }}} */
