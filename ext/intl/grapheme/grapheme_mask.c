/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  +----------------------------------------------------------------------+
  | Author: Sepehr Mahmoudi <sepehrphpr@gmail.com>                      |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php_intl.h"
#include "grapheme_util.h"
#include "zend_exceptions.h"
#include <unicode/utypes.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>

/* {{{ proto string|false grapheme_mask(string $string, string $mask_char, int $offset = 0, ?int $length = null)
   Masks a portion of a string respecting grapheme cluster boundaries */
PHP_FUNCTION(grapheme_mask)
{
    zend_string *str, *mask_char;
    zend_long offset = 0;
    zend_long length = 0;
    bool length_is_null = 1;

    /* 1. Parse Parameters */
    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_STR(str)
        Z_PARAM_STR(mask_char)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(length, length_is_null)
    ZEND_PARSE_PARAMETERS_END();

    /* 2. Validate Empty Mask -> Throw ValueError (PHP 8 style) */
    if (ZSTR_LEN(mask_char) == 0) {
        zend_value_error("grapheme_mask(): Argument #2 ($mask_char) must not be empty");
        RETURN_THROWS();
    }

    if (ZSTR_LEN(str) == 0) {
        RETURN_STR_COPY(str);
    }

    /* 3. Initialize ICU Break Iterator & UText */
    UErrorCode status = U_ZERO_ERROR;
    UBreakIterator *bi = ubrk_open(UBRK_CHARACTER, intl_locale_get_default(), NULL, 0, &status);
    if (U_FAILURE(status)) {
        php_error_docref(NULL, E_WARNING, "Failed to create break iterator");
        RETURN_FALSE;
    }

    UText *ut = utext_openUTF8(NULL, ZSTR_VAL(str), ZSTR_LEN(str), &status);
    if (U_FAILURE(status)) {
        ubrk_close(bi);
        php_error_docref(NULL, E_WARNING, "Failed to open UText");
        RETURN_FALSE;
    }

    ubrk_setUText(bi, ut, &status);
    if (U_FAILURE(status)) {
        utext_close(ut);
        ubrk_close(bi);
        php_error_docref(NULL, E_WARNING, "Failed to set UText");
        RETURN_FALSE;
    }

    /* 4. Count total graphemes to handle negative offsets/lengths */
    int32_t total_graphemes = 0;
    while (ubrk_next(bi) != UBRK_DONE) {
        total_graphemes++;
    }

    /* 5. Calculate absolute start and mask length */
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

    /* If nothing to mask, return early */
    if (mask_len <= 0) {
        utext_close(ut);
        ubrk_close(bi);
        RETURN_STR_COPY(str);
    }

    /* 6. Find Byte Offsets (Fix: handle start == 0 correctly) */
    int32_t start_byte_offset = 0;
    int32_t end_byte_offset = ZSTR_LEN(str);
    int32_t current_grapheme = 0;
    int32_t pos = ubrk_first(bi);

    while (pos != UBRK_DONE) {
        if (current_grapheme == start) {
            start_byte_offset = pos;
        }
        if (current_grapheme == start + mask_len) {
            end_byte_offset = pos;
            break;
        }
        pos = ubrk_next(bi);
        current_grapheme++;
    }

    /* Clean up ICU objects as we have the byte offsets now */
    utext_close(ut);
    ubrk_close(bi);

    /* 7. Allocate Exact Final Memory and Copy */
    size_t prefix_len = start_byte_offset;
    size_t suffix_len = ZSTR_LEN(str) - end_byte_offset;
    size_t total_mask_bytes = mask_len * ZSTR_LEN(mask_char);
    size_t final_len = prefix_len + total_mask_bytes + suffix_len;

    zend_string *result = zend_string_alloc(final_len, 0);
    char *dest = ZSTR_VAL(result);

    /* Copy Prefix */
    if (prefix_len > 0) {
        memcpy(dest, ZSTR_VAL(str), prefix_len);
        dest += prefix_len;
    }

    /* Copy Mask (repeat mask_char for each grapheme) */
    for (zend_long i = 0; i < mask_len; i++) {
        memcpy(dest, ZSTR_VAL(mask_char), ZSTR_LEN(mask_char));
        dest += ZSTR_LEN(mask_char);
    }

    /* Copy Suffix */
    if (suffix_len > 0) {
        memcpy(dest, ZSTR_VAL(str) + end_byte_offset, suffix_len);
    }

    /* Null-terminate */
    ZSTR_VAL(result)[final_len] = '\0';

    RETURN_NEW_STR(result);
}
/* }}} */
