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
   | Authors: Sepehr mahmoodi <sepehrphpr@gmail.com>                     |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_intl.h"
#include "intl_error.h"
#include "grapheme.h"
#include "grapheme_util.h"
#include "grapheme_mask.h"

#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/utypes.h>
#include <unicode/utf8.h>

/* {{{ grapheme_mask_validate_mask_char */
static UBool grapheme_mask_validate_mask_char(const char *mask_char, size_t mask_char_len)
{
    if (mask_char_len == 0) {
        return 0;
    }

    UErrorCode status = U_ZERO_ERROR;
    UText *ut = utext_openUTF8(NULL, mask_char, mask_char_len, &status);
    if (U_FAILURE(status)) {
        return 0;
    }

    /* Use the existing grapheme_count_graphemes function */
    int32_t count = grapheme_count_graphemes(ut);
    utext_close(ut);
    
    return (count == 1);
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_mask) */
PHP_FUNCTION(grapheme_mask)
{
    zend_string *str;
    zend_string *mask_char = NULL;
    zend_long offset = 0;
    zend_long length = 0;
    zend_bool length_is_null = 1;
    int32_t total_graphemes = 0;
    int32_t start = 0, mask_len = 0;
    int32_t *boundaries = NULL;
    int32_t boundaries_capacity = 16;
    int32_t start_byte_offset, end_byte_offset;
    zend_string *result;
    char *p;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_STR(str)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR_DEFAULT(mask_char, "*")
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(length, length_is_null)
    ZEND_PARSE_PARAMETERS_END();

    /* empty string -> return as is */
    if (ZSTR_LEN(str) == 0) {
        RETURN_STR_COPY(str);
    }

    /* validate mask_char: must be exactly one grapheme cluster */
    if (!grapheme_mask_validate_mask_char(ZSTR_VAL(mask_char), ZSTR_LEN(mask_char))) {
        zend_argument_value_error(2, "must be exactly one grapheme cluster");
        RETURN_THROWS();
    }

    /* validate UTF-8 input - using existing helper */
    if (!grapheme_ascii_check(ZSTR_VAL(str), ZSTR_LEN(str))) {
        /* Not ASCII, need to validate UTF-8 */
        UErrorCode status = U_ZERO_ERROR;
        UText *ut = utext_openUTF8(NULL, ZSTR_VAL(str), ZSTR_LEN(str), &status);
        if (U_FAILURE(status)) {
            intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "Invalid UTF-8 string", 1);
            RETURN_FALSE;
        }
        utext_close(ut);
    }

    /* build cache of all grapheme boundaries */
    {
        UErrorCode status = U_ZERO_ERROR;
        UText *ut = NULL;
        UBreakIterator *bi = NULL;

        ut = utext_openUTF8(NULL, ZSTR_VAL(str), ZSTR_LEN(str), &status);
        if (U_FAILURE(status)) {
            intl_error_set(NULL, status, "utext_openUTF8 failed", 1);
            RETURN_FALSE;
        }

        bi = grapheme_get_break_iterator(&status);
        if (U_FAILURE(status)) {
            utext_close(ut);
            intl_error_set(NULL, status, "grapheme_get_break_iterator failed", 1);
            RETURN_FALSE;
        }

        ubrk_setUText(bi, ut, &status);
        if (U_FAILURE(status)) {
            ubrk_close(bi);
            utext_close(ut);
            intl_error_set(NULL, status, "ubrk_setUText failed", 1);
            RETURN_FALSE;
        }

        boundaries = emalloc(boundaries_capacity * sizeof(int32_t));

        /* collect all boundaries */
        int32_t pos = ubrk_first(bi);
        while (pos != UBRK_DONE) {
            if (total_graphemes >= boundaries_capacity) {
                boundaries_capacity *= 2;
                int32_t *new_boundaries = erealloc(boundaries, boundaries_capacity * sizeof(int32_t));
                if (!new_boundaries) {
                    ubrk_close(bi);
                    utext_close(ut);
                    efree(boundaries);
                    zend_error(E_ERROR, "Memory allocation failed");
                }
                boundaries = new_boundaries;
            }
            boundaries[total_graphemes] = pos;
            total_graphemes++;
            pos = ubrk_next(bi);
        }

        ubrk_close(bi);
        utext_close(ut);
    }

    /* Adjust offset */
    if (offset < 0) {
        offset = total_graphemes + offset;
        if (offset < 0) offset = 0;
    } else if (offset > total_graphemes) {
        offset = total_graphemes;
    }
    start = (int32_t)offset;

    /* Determine mask_len */
    if (length_is_null) {
        mask_len = total_graphemes - start;
    } else {
        if (length < 0) {
            /* negative length from end */
            length = total_graphemes - start + length;
            if (length < 0) length = 0;
        }
        mask_len = (int32_t)length;
        if (start + mask_len > total_graphemes) {
            mask_len = total_graphemes - start;
        }
    }

    /* No-op if nothing to mask */
    if (mask_len <= 0 || start >= total_graphemes) {
        efree(boundaries);
        RETURN_STR_COPY(str);
    }

    /* Compute byte offsets */
    start_byte_offset = boundaries[start];
    if (start + mask_len == total_graphemes) {
        end_byte_offset = (int32_t)ZSTR_LEN(str);
    } else {
        end_byte_offset = boundaries[start + mask_len];
    }

    /* Build result string: prefix + mask_char repeated per grapheme + suffix */
    {
        size_t mask_char_len = ZSTR_LEN(mask_char);
        size_t prefix_len = start_byte_offset;
        size_t masked_len = mask_len * mask_char_len;
        size_t suffix_len = ZSTR_LEN(str) - end_byte_offset;
        size_t final_len = prefix_len + masked_len + suffix_len;

        result = zend_string_alloc(final_len, 0);
        p = ZSTR_VAL(result);

        /* Copy prefix (bytes before masking) */
        if (prefix_len > 0) {
            memcpy(p, ZSTR_VAL(str), prefix_len);
            p += prefix_len;
        }

        /* Write mask_char for each grapheme to be masked */
        for (int32_t i = 0; i < mask_len; i++) {
            memcpy(p, ZSTR_VAL(mask_char), mask_char_len);
            p += mask_char_len;
        }

        /* Copy suffix (bytes after masking) */
        if (suffix_len > 0) {
            memcpy(p, ZSTR_VAL(str) + end_byte_offset, suffix_len);
        }

        ZSTR_VAL(result)[final_len] = '\0';
    }

    efree(boundaries);
    RETURN_NEW_STR(result);
}
/* }}} */
