/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  |                                                                      |
  | Redistribution and use in source and binary forms, with or without   |
  | modification, are permitted provided that the following conditions   |
  | are met:                                                             |
  |                                                                      |
  | 1. Redistributions of source code must retain the above copyright    |
  |    notice, this list of conditions and the following disclaimer.     |
  |                                                                      |
  | 2. Redistributions in binary form must reproduce the above copyright |
  |    notice, this list of conditions and the following disclaimer in   |
  |    the documentation and/or other materials provided with the        |
  |    distribution.                                                     |
  |                                                                      |
  | 3. Neither the name of The PHP Group nor the names of its            |
  |    contributors may be used to endorse or promote products derived   |
  |    from this software without specific prior written permission.     |
  |                                                                      |
  | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS  |
  | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT    |
  | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS    |
  | FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE       |
  | COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, |
  | INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, |
  | BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     |
  | LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER     |
  | CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   |
  | LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    |
  | ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE      |
  | POSSIBILITY OF SUCH DAMAGE.                                          |
  +----------------------------------------------------------------------+
  | Author: Sepehr <sepehrphpr@gmail.com>                                |
  +----------------------------------------------------------------------+
*/

/* grapheme_mask.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/ustring.h>

#include "php_intl.h"
#include "grapheme_util.h"

/* validate mask_char: must be exactly one grapheme cluster */
static int grapheme_mask_validate_mask_char(zend_string *mask_char)
{
    UErrorCode status = U_ZERO_ERROR;
    UText ut = UTEXT_INITIALIZER;
    int32_t count;

    utext_openUTF8(&ut, ZSTR_VAL(mask_char), ZSTR_LEN(mask_char), &status);
    if (U_FAILURE(status)) {
        return FAILURE;
    }

    count = grapheme_count_graphemes(&ut);
    utext_close(&ut);

    return (count == 1) ? SUCCESS : FAILURE;
}

/* {{{ Return a string with parts masked by a mask character */
PHP_FUNCTION(grapheme_mask)
{
    zend_string *string;
    zend_string *mask_char = NULL;
    zend_long offset = 0;
    zend_long length = ZEND_LONG_MAX;
    UErrorCode status = U_ZERO_ERROR;
    UText ut_string = UTEXT_INITIALIZER;
    UBreakIterator *bi;
    int32_t total_graphemes;
    int32_t *boundaries = NULL;
    int32_t start_idx, end_idx;
    int32_t mask_start, mask_end;
    zend_long offset_adj, length_adj;
    int32_t prefix_len, masked_len, suffix_len;
    zend_string *result;
    int32_t result_len;
    char *result_p;

    /* Parse arguments */
    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_STR(string)
        Z_PARAM_STR_OR_NULL(mask_char)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG(length)
    ZEND_PARSE_PARAMETERS_END();

    if (mask_char == NULL) {
        mask_char = ZSTR_INIT_LITERAL("*", 1); /* already UTF-8 */
    }

    if (grapheme_mask_validate_mask_char(mask_char) == FAILURE) {
        zend_argument_value_error(2, "must be exactly one grapheme cluster");
        RETURN_THROWS();
    }

    /* validate string as UTF-8 */
    if (!grapheme_ascii_check(ZSTR_VAL(string), ZSTR_LEN(string))) {
        /* If not pure ASCII, check UTF-8 validity */
        UChar32 ch;
        int32_t i = 0;
        U8_NEXT(ZSTR_VAL(string), i, ZSTR_LEN(string), ch);
        if (i < 0) {
            zend_argument_value_error(1, "must be a valid UTF-8 string");
            RETURN_THROWS();
        }
        /* More thorough check */
        u_strFromUTF8(NULL, 0, NULL, ZSTR_VAL(string), ZSTR_LEN(string), &status);
        if (U_FAILURE(status)) {
            zend_argument_value_error(1, "must be a valid UTF-8 string");
            RETURN_THROWS();
        }
    }

    /* Open UText and BreakIterator */
    utext_openUTF8(&ut_string, ZSTR_VAL(string), ZSTR_LEN(string), &status);
    if (U_FAILURE(status)) {
        RETURN_FALSE;
    }

    bi = grapheme_get_break_iterator(&status);
    if (U_FAILURE(status)) {
        RETURN_FALSE;
    }
    ubrk_setUText(bi, &ut_string, &status);
    if (U_FAILURE(status)) {
        utext_close(&ut_string);
        ubrk_close(bi);
        RETURN_FALSE;
    }

    /* Collect grapheme boundaries */
    {
        int32_t count = 0;
        int32_t *tmp;
        boundaries = (int32_t*) emalloc(sizeof(int32_t) * 16);
        int32_t cap = 16;
        int32_t pos = ubrk_first(bi);
        if (pos != UBRK_DONE) {
            boundaries[count++] = pos;
            while ((pos = ubrk_next(bi)) != UBRK_DONE) {
                if (count >= cap) {
                    cap *= 2;
                    tmp = (int32_t*) erealloc(boundaries, sizeof(int32_t) * cap);
                    boundaries = tmp;
                }
                boundaries[count++] = pos;
            }
        }
        ubrk_close(bi);
        utext_close(&ut_string);
        total_graphemes = count - 1; /* number of graphemes = (number of boundaries - 1) */
    }

    if (total_graphemes <= 0) {
        efree(boundaries);
        RETURN_EMPTY_STRING();
    }

    offset_adj = (offset >= 0) ? offset : offset + total_graphemes;
    length_adj = (length == ZEND_LONG_MAX) ? total_graphemes : length;

    grapheme_get_haystack_offset(&offset_adj, &length_adj, 0, total_graphemes, OFFSET_GR, OFFSET_LEN);
    if (offset_adj < 0 || length_adj <= 0) {
        efree(boundaries);
        RETURN_EMPTY_STRING();
    }

    mask_start = boundaries[offset_adj];
    mask_end = boundaries[offset_adj + length_adj];

    prefix_len = mask_start;
    masked_len = mask_end - mask_start;
    suffix_len = boundaries[total_graphemes] - mask_end;

    result_len = prefix_len + ZSTR_LEN(mask_char) * (masked_len > 0 ? 1 : 0) + suffix_len;
    result = zend_string_alloc(result_len, 0);
    result_p = ZSTR_VAL(result);

    if (prefix_len > 0) {
        memcpy(result_p, ZSTR_VAL(string), prefix_len);
        result_p += prefix_len;
    }

    if (masked_len > 0) {
        memcpy(result_p, ZSTR_VAL(mask_char), ZSTR_LEN(mask_char));
        result_p += ZSTR_LEN(mask_char);
    }

    if (suffix_len > 0) {
        memcpy(result_p, ZSTR_VAL(string) + mask_end, suffix_len);
    }

    ZSTR_VAL(result)[result_len] = '\0';

    efree(boundaries);
    RETURN_STR(result);
}
/* }}} */
