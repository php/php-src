/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  +----------------------------------------------------------------------+
  | Author: Sepehr Mahmoudi                                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include "php_intl.h"
#include "grapheme_util.h"
#include "intl_common.h"
#include "intl_error.h"
#include "zend_exceptions.h"

#include <unicode/ubrk.h>
#include <unicode/utext.h>

/* {{{ Validate that mask_char is exactly one grapheme cluster */
static bool grapheme_mask_validate_mask_char(zend_string *mask_char)
{
    if (ZSTR_LEN(mask_char) == 0) {
        return false;
    }

    if (!grapheme_validate_utf8(ZSTR_VAL(mask_char), ZSTR_LEN(mask_char))) {
        return false;
    }

    UErrorCode status = U_ZERO_ERROR;
    UBreakIterator *bi = ubrk_open(UBRK_CHARACTER, intl_locale_get_default(), NULL, 0, &status);
    if (U_FAILURE(status)) {
        return false;
    }

    UText *ut = utext_openUTF8(NULL, ZSTR_VAL(mask_char), ZSTR_LEN(mask_char), &status);
    if (U_FAILURE(status)) {
        ubrk_close(bi);
        return false;
    }

    ubrk_setUText(bi, ut, &status);
    if (U_FAILURE(status)) {
        utext_close(ut);
        ubrk_close(bi);
        return false;
    }

    int32_t start = ubrk_first(bi);
    int32_t end = ubrk_next(bi);
    int32_t next = ubrk_next(bi);

    utext_close(ut);
    ubrk_close(bi);

    /* Must have exactly one complete grapheme cluster */
    return (start != UBRK_DONE && end != UBRK_DONE && next == UBRK_DONE);
}
/* }}} */

/* {{{ proto string|false grapheme_mask(string $string, string $mask_char = "*", int $offset = 0, ?int $length = null)
   Masks a portion of a string respecting grapheme cluster boundaries */
PHP_FUNCTION(grapheme_mask)
{
    zend_string *str, *mask_char;
    zend_long offset = 0;
    zend_long length = 0;
    bool length_is_null = true;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_STR(str)
        Z_PARAM_STR_DEFAULT(mask_char, "*")
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(length, length_is_null)
    ZEND_PARSE_PARAMETERS_END();

    /* Validate mask_char */
    if (!grapheme_mask_validate_mask_char(mask_char)) {
        zend_value_error("grapheme_mask(): Argument #2 ($mask_char) must be exactly one grapheme cluster");
        RETURN_THROWS();
    }

    /* Fast path: Empty input string */
    if (ZSTR_LEN(str) == 0) {
        RETURN_STR_COPY(str);
    }

    /* Validate UTF-8 for input string */
    if (!grapheme_validate_utf8(ZSTR_VAL(str), ZSTR_LEN(str))) {
        RETURN_FALSE;
    }

    UErrorCode status = U_ZERO_ERROR;
    UBreakIterator *bi = ubrk_open(UBRK_CHARACTER, intl_locale_get_default(), NULL, 0, &status);
    if (U_FAILURE(status)) {
        intl_error_set(NULL, status, "grapheme_mask(): Failed to create break iterator", 0);
        RETURN_FALSE;
    }

    UText *ut = utext_openUTF8(NULL, ZSTR_VAL(str), ZSTR_LEN(str), &status);
    if (U_FAILURE(status)) {
        ubrk_close(bi);
        intl_error_set(NULL, status, "grapheme_mask(): Failed to open UText", 0);
        RETURN_FALSE;
    }

    ubrk_setUText(bi, ut, &status);
    if (U_FAILURE(status)) {
        utext_close(ut);
        ubrk_close(bi);
        intl_error_set(NULL, status, "grapheme_mask(): Failed to set UText", 0);
        RETURN_FALSE;
    }

    /* Phase 1: Cache all grapheme boundaries */
    uint32_t alloc_graphemes = 16;
    int32_t *boundaries = emalloc(alloc_graphemes * sizeof(int32_t));
    if (!boundaries) {
        utext_close(ut);
        ubrk_close(bi);
        RETURN_FALSE;
    }

    int32_t idx = 0;
    int32_t pos = ubrk_first(bi);
    boundaries[idx++] = pos;  /* First boundary (0) */

    while ((pos = ubrk_next(bi)) != UBRK_DONE) {
        if (idx >= alloc_graphemes) {
            alloc_graphemes *= 2;
            int32_t *new_boundaries = erealloc(boundaries, alloc_graphemes * sizeof(int32_t));
            if (!new_boundaries) {
                efree(boundaries);
                utext_close(ut);
                ubrk_close(bi);
                RETURN_FALSE;
            }
            boundaries = new_boundaries;
        }
        boundaries[idx++] = pos;
    }

    utext_close(ut);
    ubrk_close(bi);

    int32_t total_graphemes = idx - 1;  /* Number of grapheme clusters */

    /* Phase 2: Calculate start and length in grapheme units */
    zend_long start = offset;
    if (start < 0) {
        start += total_graphemes;
        if (start < 0) {
            start = 0;
        }
    } else if (start > total_graphemes) {
        start = total_graphemes;
    }

    zend_long mask_len;
    if (length_is_null) {
        mask_len = total_graphemes - start;
    } else {
        mask_len = length;
        if (mask_len < 0) {
            mask_len = total_graphemes - start + mask_len;
            if (mask_len < 0) {
                mask_len = 0;
            }
        } else if (start + mask_len > total_graphemes) {
            mask_len = total_graphemes - start;
        }
    }

    /* No-op check */
    if (mask_len <= 0) {
        efree(boundaries);
        RETURN_STR_COPY(str);
    }

    /* Phase 3: Get byte offsets */
    int32_t start_byte_offset = boundaries[start];
    int32_t end_byte_offset;

    if (start + mask_len == total_graphemes) {
        end_byte_offset = ZSTR_LEN(str);
    } else {
        end_byte_offset = boundaries[start + mask_len];
    }

    efree(boundaries);

    /* Phase 4: Build result */
    size_t prefix_len = start_byte_offset;
    size_t suffix_len = ZSTR_LEN(str) - end_byte_offset;
    size_t mask_char_len = ZSTR_LEN(mask_char);
    size_t total_mask_bytes = mask_len * mask_char_len;
    size_t final_len = prefix_len + total_mask_bytes + suffix_len;

    zend_string *result = zend_string_alloc(final_len, 0);
    char *dest = ZSTR_VAL(result);

    /* Copy prefix */
    if (prefix_len > 0) {
        memcpy(dest, ZSTR_VAL(str), prefix_len);
        dest += prefix_len;
    }

    /* Copy mask */
    for (zend_long i = 0; i < mask_len; i++) {
        memcpy(dest, ZSTR_VAL(mask_char), mask_char_len);
        dest += mask_char_len;
    }

    /* Copy suffix */
    if (suffix_len > 0) {
        memcpy(dest, ZSTR_VAL(str) + end_byte_offset, suffix_len);
    }

    ZSTR_VAL(result)[final_len] = '\0';
    RETURN_NEW_STR(result);
}
/* }}} */
