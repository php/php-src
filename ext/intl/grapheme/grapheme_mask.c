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
   | Authors: Sepehr Mahmoodi <sepehrphpr@gmail.com>                     |
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

/* {{{ grapheme_mask_validate_mask_char
   بررسی می‌کند که mask_char دقیقاً یک grapheme cluster باشد */
static UBool grapheme_mask_validate_mask_char(const char *mask_char, size_t mask_char_len)
{
    if (mask_char_len == 0) {
        return 0;
    }

    UErrorCode status = U_ZERO_ERROR;
    UText *ut = NULL;
    UBreakIterator *bi = NULL;
    UBool retval = 0;

    ut = utext_openUTF8(ut, mask_char, mask_char_len, &status);
    if (U_FAILURE(status)) {
        return 0;
    }

    bi = ubrk_open(UBRK_CHARACTER, NULL, NULL, 0, &status);
    if (U_FAILURE(status)) {
        utext_close(ut);
        return 0;
    }

    ubrk_setUText(bi, ut, &status);
    if (U_FAILURE(status)) {
        ubrk_close(bi);
        utext_close(ut);
        return 0;
    }

    int32_t first = ubrk_first(bi);
    int32_t second = ubrk_next(bi);
    int32_t third = ubrk_next(bi);

    retval = (first == 0 && second == (int32_t)mask_char_len && third == UBRK_DONE);

    ubrk_close(bi);
    utext_close(ut);
    return retval;
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_mask)
   string grapheme_mask(string $string, string $mask_char = "*", int $offset = 0, ?int $length = null): string|false */
PHP_FUNCTION(grapheme_mask)
{
    zend_string *str;
    zend_string *mask_char = NULL;
    zend_long offset = 0;
    zend_long length = 0;
    zend_bool length_is_null = 1;

    int32_t total_boundaries = 0;   /* تعداد کل مرزها (گرافیم‌ها + ۱) */
    int32_t num_graphemes;          /* تعداد واقعی گرافیم‌ها */
    int32_t start_idx = 0;
    int32_t mask_len = 0;
    int32_t *boundaries = NULL;
    int32_t boundaries_capacity = 16;

    int32_t start_byte_offset, end_byte_offset;
    zend_string *result;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_STR(str)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR_DEFAULT(mask_char, "*")
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(length, length_is_null)
    ZEND_PARSE_PARAMETERS_END();

    /* رشته‌ی خالی را دست‌نخورده برگردان */
    if (ZSTR_LEN(str) == 0) {
        RETURN_STR_COPY(str);
    }

    /* اعتبارسنجی mask_char: دقیقاً یک خوشه grapheme */
    if (!grapheme_mask_validate_mask_char(ZSTR_VAL(mask_char), ZSTR_LEN(mask_char))) {
        zend_argument_value_error(2, "must be exactly one grapheme cluster");
        RETURN_THROWS();
    }

    /* اعتبارسنجی UTF-8 برای رشته‌ی اصلی */
    if (!grapheme_validate_utf8(ZSTR_VAL(str), ZSTR_LEN(str))) {
        intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "Invalid UTF-8 string", 1);
        RETURN_FALSE;
    }

    /* ۱) کش کردن همه‌ی مرزهای grapheme */
    {
        UErrorCode status = U_ZERO_ERROR;
        UText *ut = NULL;
        UBreakIterator *bi = NULL;

        ut = utext_openUTF8(ut, ZSTR_VAL(str), ZSTR_LEN(str), &status);
        if (U_FAILURE(status)) {
            intl_error_set(NULL, status, "utext_openUTF8 failed", 1);
            RETURN_FALSE;
        }

        bi = ubrk_open(UBRK_CHARACTER, NULL, NULL, 0, &status);
        if (U_FAILURE(status)) {
            utext_close(ut);
            intl_error_set(NULL, status, "ubrk_open failed", 1);
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

        int32_t pos = ubrk_first(bi);
        while (pos != UBRK_DONE) {
            if (total_boundaries >= boundaries_capacity) {
                boundaries_capacity *= 2;
                boundaries = erealloc(boundaries, boundaries_capacity * sizeof(int32_t));
                if (!boundaries) {
                    ubrk_close(bi);
                    utext_close(ut);
                    zend_error(E_ERROR, "Memory allocation failed");
                }
            }
            boundaries[total_boundaries] = pos;
            total_boundaries++;
            pos = ubrk_next(bi);
        }

        ubrk_close(bi);
        utext_close(ut);
    }

    num_graphemes = total_boundaries - 1;  /* مثلاً "سلام" ۵ مرز و ۴ گرافیم */

    /* ۲) تنظیم offset بر اساس تعداد گرافیم‌ها */
    if (offset < 0) {
        offset = num_graphemes + offset;
        if (offset < 0) offset = 0;
    } else if (offset > num_graphemes) {
        offset = num_graphemes;
    }
    start_idx = (int32_t)offset;

    /* ۳) تعیین طول ناحیه‌ی ماسک */
    if (length_is_null) {
        mask_len = num_graphemes - start_idx;
    } else {
        if (length < 0) {
            /* طول منفی: از انتها کم می‌شود */
            length = num_graphemes - start_idx + length;
            if (length < 0) length = 0;
        }
        mask_len = (int32_t)length;
        if (start_idx + mask_len > num_graphemes) {
            mask_len = num_graphemes - start_idx;
        }
    }

    /* اگر چیزی برای ماسک کردن نیست، خود رشته را برگردان */
    if (mask_len <= 0 || start_idx >= num_graphemes) {
        efree(boundaries);
        RETURN_STR_COPY(str);
    }

    /* ۴) محاسبه‌ی آفست‌های بایتی */
    start_byte_offset = boundaries[start_idx];
    if (start_idx + mask_len == num_graphemes) {
        end_byte_offset = (int32_t)ZSTR_LEN(str);
    } else {
        end_byte_offset = boundaries[start_idx + mask_len];
    }

    /* ۵) ساختن رشته‌ی نتیجه: پیشوند + (mask_char تکرارشده به تعداد گرافیم) + پسوند */
    {
        size_t mask_char_len = ZSTR_LEN(mask_char);
        size_t prefix_len = start_byte_offset;
        size_t masked_len = mask_len * mask_char_len;
        size_t suffix_len = ZSTR_LEN(str) - end_byte_offset;
        size_t final_len = prefix_len + masked_len + suffix_len;

        result = zend_string_alloc(final_len, 0);
        if (ZSTR_VAL(result) == NULL) {
            efree(boundaries);
            RETURN_FALSE;
        }

        char *p = ZSTR_VAL(result);

        /* پیشوند */
        memcpy(p, ZSTR_VAL(str), prefix_len);
        p += prefix_len;

        /* کاراکتر ماسک به تعداد گرافیم‌های ناحیه */
        for (int32_t i = 0; i < mask_len; i++) {
            memcpy(p, ZSTR_VAL(mask_char), mask_char_len);
            p += mask_char_len;
        }

        /* پسوند */
        if (suffix_len > 0) {
            memcpy(p, ZSTR_VAL(str) + end_byte_offset, suffix_len);
        }

        ZSTR_VAL(result)[final_len] = '\0';
    }

    efree(boundaries);
    RETURN_NEW_STR(result);
}
/* }}} */
