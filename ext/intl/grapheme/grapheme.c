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
   | Authors: Rui Hirokawa <hirokawa@php.net>                             |
   |          Stanislav Malyshev <stas@php.net>                           |
   |          Kirti Velankar <kirtig@yahoo-inc.com>                       |
   |          sepehr mahmoodi <sepehrphpr@gmail.com>                           |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include "grapheme_util.h"
#include "grapheme_arginfo.h"
#include "grapheme_mask.h"

/* {{{ grapheme_functions[] */
static const zend_function_entry grapheme_functions[] = {
    PHP_FE(grapheme_strlen,                arginfo_grapheme_strlen)
    PHP_FE(grapheme_strpos,                arginfo_grapheme_strpos)
    PHP_FE(grapheme_stripos,               arginfo_grapheme_stripos)
    PHP_FE(grapheme_strrpos,               arginfo_grapheme_strrpos)
    PHP_FE(grapheme_strripos,              arginfo_grapheme_strripos)
    PHP_FE(grapheme_substr,                arginfo_grapheme_substr)
    PHP_FE(grapheme_strstr,                arginfo_grapheme_strstr)
    PHP_FE(grapheme_stristr,               arginfo_grapheme_stristr)
    PHP_FE(grapheme_extract,               arginfo_grapheme_extract)
    PHP_FE(grapheme_mask,                  arginfo_grapheme_mask)
    PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(grapheme)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(grapheme)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(grapheme)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Grapheme Support", "enabled");
    php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(grapheme)
{
    return SUCCESS;
}
/* }}} */

/* {{{ grapheme_module_entry */
zend_module_entry grapheme_module_entry = {
    STANDARD_MODULE_HEADER,
    "grapheme",
    grapheme_functions,
    PHP_MINIT(grapheme),
    PHP_MSHUTDOWN(grapheme),
    NULL,
    PHP_RSHUTDOWN(grapheme),
    PHP_MINFO(grapheme),
    PHP_GRAPHEME_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GRAPHEME
ZEND_GET_MODULE(grapheme)
#endif

/* {{{ Returns the number of grapheme clusters in a string */
PHP_FUNCTION(grapheme_strlen)
{
    const char *str;
    size_t str_len;
    UText ut = UTEXT_INITIALIZER;
    int32_t grapheme_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(str, str_len)
    ZEND_PARSE_PARAMETERS_END();

    UText *utxt = grapheme_open_utext(&ut, str, str_len);
    if (utxt == NULL) {
        RETURN_FALSE;
    }

    grapheme_len = grapheme_count_graphemes(utxt);
    utext_close(utxt);

    if (grapheme_len < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(grapheme_len);
}
/* }}} */

/* {{{ Find position (in grapheme units) of first occurrence of a string */
PHP_FUNCTION(grapheme_strpos)
{
    const char *haystack, *needle;
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
    if (pos < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(pos);
}
/* }}} */

/* {{{ Finds position (in grapheme units) of first occurrence of a string (case-insensitive) */
PHP_FUNCTION(grapheme_stripos)
{
    const char *haystack, *needle;
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
    if (pos < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(pos);
}
/* }}} */

/* {{{ Find position (in grapheme units) of last occurrence of a string */
PHP_FUNCTION(grapheme_strrpos)
{
    const char *haystack, *needle;
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
    if (pos < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(pos);
}
/* }}} */

/* {{{ Finds position (in grapheme units) of last occurrence of a string (case-insensitive) */
PHP_FUNCTION(grapheme_strripos)
{
    const char *haystack, *needle;
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
    if (pos < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(pos);
}
/* }}} */

/* {{{ Returns part of a string */
PHP_FUNCTION(grapheme_substr)
{
    const char *str;
    size_t str_len;
    zend_long offset = 0, length = 0;
    zend_bool length_is_null = 1;
    const char *p;
    int32_t (*iter)(UText *);
    UText ut = UTEXT_INITIALIZER;
    int32_t start_pos, end_pos, grapheme_len, i;
    char *out;
    size_t out_len;

    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_STRING(str, str_len)
        Z_PARAM_LONG(offset)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG_OR_NULL(length, length_is_null)
    ZEND_PARSE_PARAMETERS_END();

    if (str_len == 0) {
        RETURN_FALSE;
    }

    if (length_is_null) {
        length = GRAPHEME_STRPOS_UTF8_MAXLEN;
    }

    UText *utxt = grapheme_open_utext(&ut, str, str_len);
    if (utxt == NULL) {
        RETURN_FALSE;
    }

    grapheme_len = grapheme_count_graphemes(utxt);
    if (grapheme_len <= 0) {
        utext_close(utxt);
        RETURN_FALSE;
    }

    /* normalize offset */
    if (offset < 0) {
        offset += grapheme_len;
    }

    if (offset < 0 || offset >= grapheme_len) {
        utext_close(utxt);
        RETURN_FALSE;
    }

    /* calculate end position */
    if (length == 0) {
        end_pos = offset;
    } else if (length > 0) {
        end_pos = offset + length;
        if (end_pos > grapheme_len) {
            end_pos = grapheme_len;
        }
    } else {
        end_pos = grapheme_len + length;
        if (end_pos <= offset) {
            utext_close(utxt);
            RETURN_FALSE;
        }
    }

    /* find start byte position */
    start_pos = 0;
    if (offset > 0) {
        start_pos = grapheme_strpos_utf8(str, str_len, offset, NULL);
        if (start_pos < 0) {
            utext_close(utxt);
            RETURN_FALSE;
        }
    }

    /* find end byte position */
    if (end_pos == offset) {
        out_len = 0;
    } else {
        int32_t byte_len = grapheme_strpos_utf8(str, str_len, end_pos - offset, str + start_pos);
        if (byte_len < 0) {
            utext_close(utxt);
            RETURN_FALSE;
        }
        out_len = byte_len;
    }

    utext_close(utxt);

    if (out_len == 0) {
        RETURN_EMPTY_STRING();
    }

    out = estrndup(str + start_pos, out_len);
    out[out_len] = '\0';
    RETURN_STRINGL(out, out_len);
}
/* }}} */

/* {{{ Returns part of haystack string from the first occurrence of needle to the end */
PHP_FUNCTION(grapheme_strstr)
{
    const char *haystack, *needle;
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
    if (pos < 0) {
        RETURN_FALSE;
    }

    if (before_needle) {
        RETURN_STRINGL(haystack, pos);
    } else {
        RETURN_STRINGL(haystack + pos, haystack_len - pos);
    }
}
/* }}} */

/* {{{ Returns part of haystack string from the first occurrence of needle (case-insensitive) to the end */
PHP_FUNCTION(grapheme_stristr)
{
    const char *haystack, *needle;
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
    if (pos < 0) {
        RETURN_FALSE;
    }

    if (before_needle) {
        RETURN_STRINGL(haystack, pos);
    } else {
        RETURN_STRINGL(haystack + pos, haystack_len - pos);
    }
}
/* }}} */

/* {{{ Function to extract a sequence of grapheme clusters from a text buffer */
PHP_FUNCTION(grapheme_extract)
{
    const char *str;
    size_t str_len;
    zend_long size, extract_type = GRAPHEME_EXTRACT_TYPE_COUNT;
    zend_long start = 0;
    zend_long *next = NULL;
    zend_long next_pos;
    int32_t extracted_len;
    char *out;

    ZEND_PARSE_PARAMETERS_START(2, 5)
        Z_PARAM_STRING(str, str_len)
        Z_PARAM_LONG(size)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(extract_type)
        Z_PARAM_LONG(start)
        Z_PARAM_LONG_OR_NULL(next, 1)
    ZEND_PARSE_PARAMETERS_END();

    if (size <= 0) {
        php_error_docref(NULL, E_WARNING, "Size must be positive");
        RETURN_FALSE;
    }

    if (start < 0 || (size_t)start >= str_len) {
        php_error_docref(NULL, E_WARNING, "Start not contained in string");
        RETURN_FALSE;
    }

    extracted_len = grapheme_extract_utf8(str, str_len, size, extract_type, start, &next_pos);
    if (extracted_len < 0) {
        RETURN_FALSE;
    }

    if (next) {
        *next = next_pos;
    }

    out = estrndup(str + start, extracted_len);
    out[extracted_len] = '\0';
    RETURN_STRINGL(out, extracted_len);
}
/* }}} */
