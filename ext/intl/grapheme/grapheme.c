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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   |          Sepehr mahmoodi <sepehrphpr@gmail.com>                     |
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
#include <unicode/utext.h>
#include <unicode/utypes.h>
#include <unicode/utf8.h>

/* {{{ grapheme_count_graphemes */
static int32_t grapheme_count_graphemes(const char *str, size_t str_len)
{
    UErrorCode status = U_ZERO_ERROR;
    UText *ut = NULL;
    UBreakIterator *bi = NULL;
    int32_t count = 0;

    if (str_len == 0) {
        return 0;
    }

    ut = utext_openUTF8(ut, str, str_len, &status);
    if (U_FAILURE(status)) {
        return -1;
    }

    bi = ubrk_open(UBRK_CHARACTER, NULL, NULL, 0, &status);
    if (U_FAILURE(status)) {
        utext_close(ut);
        return -1;
    }

    ubrk_setUText(bi, ut, &status);
    if (U_FAILURE(status)) {
        ubrk_close(bi);
        utext_close(ut);
        return -1;
    }

    int32_t pos = ubrk_first(bi);
    while (pos != UBRK_DONE) {
        count++;
        pos = ubrk_next(bi);
    }
    /* count includes both start and end, so actual graphemes = count - 1 */
    if (count > 0) count--;

    ubrk_close(bi);
    utext_close(ut);

    return count;
}
/* }}} */

/* {{{ grapheme_find_boundary */
static int32_t grapheme_find_boundary(const char *str, size_t str_len, int32_t grapheme_offset)
{
    UErrorCode status = U_ZERO_ERROR;
    UText *ut = NULL;
    UBreakIterator *bi = NULL;
    int32_t offset = -1;

    if (grapheme_offset < 0 || str_len == 0) {
        return -1;
    }

    ut = utext_openUTF8(ut, str, str_len, &status);
    if (U_FAILURE(status)) {
        return -1;
    }

    bi = ubrk_open(UBRK_CHARACTER, NULL, NULL, 0, &status);
    if (U_FAILURE(status)) {
        utext_close(ut);
        return -1;
    }

    ubrk_setUText(bi, ut, &status);
    if (U_FAILURE(status)) {
        ubrk_close(bi);
        utext_close(ut);
        return -1;
    }

    /* move to the nth grapheme boundary */
    int32_t pos = ubrk_first(bi);
    for (int32_t i = 0; i < grapheme_offset && pos != UBRK_DONE; i++) {
        pos = ubrk_next(bi);
    }

    if (pos != UBRK_DONE) {
        offset = pos;
    }

    ubrk_close(bi);
    utext_close(ut);

    return offset;
}
/* }}} */

/* The mask functions are now in grapheme_mask.c – just include the header */

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
    REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_COUNT", GRAPHEME_EXTR_COUNT, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_MAXBYTES", GRAPHEME_EXTR_MAXBYTES, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_MAXCHARS", GRAPHEME_EXTR_MAXCHARS, CONST_CS | CONST_PERSISTENT);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(grapheme)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "grapheme support", "enabled");
    php_info_print_table_row(2, "ICU version", U_ICU_VERSION);
    php_info_print_table_end();
}
/* }}} */

/* {{{ grapheme_module_entry */
zend_module_entry grapheme_module_entry = {
    STANDARD_MODULE_HEADER,
    "grapheme",
    grapheme_functions,
    PHP_MINIT(grapheme),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(grapheme),
    PHP_GRAPHEME_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */
