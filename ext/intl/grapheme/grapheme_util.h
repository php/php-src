/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   +----------------------------------------------------------------------+
 */

#ifndef GRAPHEME_GRAPHEME_UTIL_H
#define GRAPHEME_GRAPHEME_UTIL_H

#include "php_intl.h"
#include "intl_convert.h"

/* get_break_interator: get a break iterator from the global structure */
UBreakIterator* grapheme_get_break_iterator(void *stack_buffer, UErrorCode *status );

zend_long grapheme_ascii_check(const unsigned char *day, size_t len);
void grapheme_substr_ascii(char *str, size_t str_len, int32_t f, int32_t l, char **sub_str, int32_t *sub_str_len);
zend_long grapheme_strrpos_ascii(char *haystack, size_t haystack_len, char *needle, size_t needle_len, int32_t offset);

int32_t grapheme_strrpos_utf16(char *haystack, size_t haystack_len, char *needle, size_t needle_len, int32_t offset, int f_ignore_case);
int32_t grapheme_strpos_utf16(char *haystack, size_t haystack_len, char *needle, size_t needle_len, int32_t offset, int *puchar_pos, int f_ignore_case, int last);

int32_t grapheme_split_string(const UChar *text, int32_t text_length, int boundary_array[], int boundary_array_len );

int32_t grapheme_count_graphemes(UBreakIterator *bi, UChar *string, int32_t string_len);

inline void *grapheme_memrchr_grapheme(const void *s, int c, int32_t n);

int32_t grapheme_get_haystack_offset(UBreakIterator* bi, int32_t offset);

UBreakIterator* grapheme_get_break_iterator(void *stack_buffer, UErrorCode *status );

/* OUTSIDE_STRING: check if (possibly negative) long offset is outside the string with int32_t length */
#define OUTSIDE_STRING(offset, max_len) ( offset <= INT32_MIN || offset > INT32_MAX || (offset < 0 ? -offset > (zend_long) max_len : offset >= (zend_long) max_len) )

#endif // GRAPHEME_GRAPHEME_UTIL_H
