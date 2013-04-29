/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
UBreakIterator* grapheme_get_break_iterator(void *stack_buffer, UErrorCode *status TSRMLS_DC );

void
grapheme_substr_ascii(char *str, int32_t str_len, int32_t f, int32_t l, int argc, char **sub_str, int *sub_str_len);

int
grapheme_strrpos_utf16(unsigned char *haystack, int32_t haystack_len, unsigned char*needle, int32_t needle_len, int32_t offset, int f_ignore_case TSRMLS_DC);

int
grapheme_strpos_utf16(unsigned char *haystack, int32_t haystack_len, unsigned char*needle, int32_t needle_len, int32_t offset, int *puchar_pos, int f_ignore_case TSRMLS_DC);

int grapheme_ascii_check(const unsigned char *day, int32_t len);

int grapheme_split_string(const UChar *text, int32_t text_length, int boundary_array[], int boundary_array_len TSRMLS_DC );

int32_t
grapheme_count_graphemes(UBreakIterator *bi, UChar *string, int32_t string_len);

int32_t
grapheme_memnstr_grapheme(UBreakIterator *bi, UChar *haystack, UChar *needle, int32_t needle_len, UChar *end);

inline void *grapheme_memrchr_grapheme(const void *s, int c, int32_t n);

UChar *
grapheme_get_haystack_offset(UBreakIterator* bi, UChar *uhaystack, int32_t uhaystack_len, int32_t offset);

int32_t
grapheme_strrpos_ascii(unsigned char *haystack, int32_t haystack_len, unsigned char *needle, int32_t needle_len, int32_t offset);

UBreakIterator* 
grapheme_get_break_iterator(void *stack_buffer, UErrorCode *status TSRMLS_DC );

/* OUTSIDE_STRING: check if (possibly negative) long offset is outside the string with int32_t length */
#define OUTSIDE_STRING(offset, max_len) ( offset <= INT32_MIN || offset > INT32_MAX || (offset < 0 ? -offset > (long) max_len : offset >= (long) max_len) )

#endif // GRAPHEME_GRAPHEME_UTIL_H
