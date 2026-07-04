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
   |          Sepehr mahmoodi <sepehrphpr@gmail.com> (grapheme_mask)     |
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
#include "grapheme_mask.h"   /* ← تابع grapheme_mask در grapheme_mask.c تعریف شده */

#include <unicode/ubrk.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>
#include <unicode/utf8.h>

/* ************************************ *
 *   توابع داخلی که قبلاً در grapheme.c
 *   پیاده‌سازی شدن (بدون تغییر)
 * ************************************ */

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

/* ************************************ *
 *   تابع grapheme_mask حذف شده و
 *   در grapheme_mask.c قرار دارد.
 * ************************************ */

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
    /* Placeholder */
    zend_throw_error(NULL, "grapheme_stripos not implemented yet");
}
/* }}} */

/* {{{ PHP_FUNCTION(grapheme_strrpos) */
PHP_FUNCTION(grapheme_strrpos)
{
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
    RETURN_FALSE;
}
/* }}} */

/* ************************************ *
 *   جدول توابع ماژول
 * ************************************ */

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
    PHP_FE(grapheme_mask, arginfo_grapheme_mask)   /* ← تابع جدید */
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
