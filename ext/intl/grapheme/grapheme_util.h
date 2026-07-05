#ifndef GRAPHEME_GRAPHEME_UTIL_H
#define GRAPHEME_GRAPHEME_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "php_intl.h"
#include "intl_convert.h"

/* get_break_interator: get a break iterator from the global structure */
UBreakIterator* grapheme_get_break_iterator(UErrorCode *status );

zend_long grapheme_ascii_check(const unsigned char *day, size_t len);
void grapheme_substr_ascii(char *str, size_t str_len, int32_t f, int32_t l, char **sub_str, int32_t *sub_str_len);
zend_long grapheme_strrpos_ascii(char *haystack, size_t haystack_len, char *needle, size_t needle_len, int32_t offset);

int32_t grapheme_strrpos_utf16(char *haystack, size_t haystack_len, char *needle, size_t needle_len, int32_t offset, int f_ignore_case, const char *locale);
int32_t grapheme_strpos_utf16(char *haystack, size_t haystack_len, char *needle, size_t needle_len, int32_t offset, int *puchar_pos, int f_ignore_case, int last, const char *locale);

int32_t grapheme_split_string(const UChar *text, int32_t text_length, int boundary_array[], int boundary_array_len );

int32_t grapheme_count_graphemes(UText *ut);  /* تغییر: UBreakIterator* → UText* */

int32_t grapheme_get_haystack_offset(UBreakIterator* bi, int32_t offset);

/* توابع UTF-8 که در grapheme_string.c تعریف شدن */
int32_t grapheme_strpos_utf8(const char *haystack, size_t haystack_len, 
                             const char *needle, size_t needle_len,
                             int32_t offset, int32_t *puchar_pos);

int32_t grapheme_stripos_utf8(const char *haystack, size_t haystack_len,
                              const char *needle, size_t needle_len,
                              int32_t offset, int32_t *puchar_pos);

int32_t grapheme_strrpos_utf8(const char *haystack, size_t haystack_len,
                              const char *needle, size_t needle_len,
                              int32_t offset, int32_t *puchar_pos);

int32_t grapheme_strripos_utf8(const char *haystack, size_t haystack_len,
                               const char *needle, size_t needle_len,
                               int32_t offset, int32_t *puchar_pos);

int32_t grapheme_extract_utf8(const char *str, size_t str_len,
                              int32_t size, int32_t extract_type,
                              int32_t start, int32_t *next_pos);

UText *grapheme_open_utext(UText *ut, const char *str, size_t str_len);

#define GRAPHEME_STRPOS_UTF8_MAXLEN INT32_MAX

#ifdef __cplusplus
}
#endif

/* OUTSIDE_STRING: check if (possibly negative) long offset is outside the string with int32_t length */
#define OUTSIDE_STRING(offset, max_len) ( offset <= INT32_MIN || offset > INT32_MAX || (offset < 0 ? -offset > (zend_long) max_len : offset > (zend_long) max_len) )

#endif // GRAPHEME_GRAPHEME_UTIL_H
