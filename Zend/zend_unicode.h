/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2006 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at                              |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_UNICODE_H
#define ZEND_UNICODE_H

#include "zend.h"
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/ustdio.h>
#include <unicode/uchar.h>
#include <unicode/uloc.h>
#include <unicode/ucol.h>

#define ZEND_FROM_U_ERROR_STOP   0
#define ZEND_FROM_U_ERROR_SKIP   1
#define ZEND_FROM_U_ERROR_SUBST  2
#define ZEND_FROM_U_ERROR_ESCAPE 3

/* internal functions */

int zend_set_converter_encoding(UConverter **converter, const char *encoding);
void zend_set_converter_subst_char(UConverter *conv, UChar *subst_char, int8_t subst_char_len);
void zend_set_converter_error_mode(UConverter *conv, uint8_t error_mode);


/* API functions */

ZEND_API void zend_convert_to_unicode(UConverter *conv, UChar **target, int32_t *target_len, const char *source, int32_t source_len, UErrorCode *status);
ZEND_API void zend_convert_from_unicode(UConverter *conv, char **target, int32_t *target_len, const UChar *source, int32_t source_len, UErrorCode *status);
ZEND_API void zend_convert_encodings(UConverter *target_conv, UConverter *source_conv, char **target, int32_t *target_len, const char *source, int32_t source_len, UErrorCode *status);
ZEND_API int zval_string_to_unicode_ex(zval *string, UConverter *conv);
ZEND_API int zval_string_to_unicode(zval *string TSRMLS_DC);
ZEND_API int zval_unicode_to_string(zval *string, UConverter *conv TSRMLS_DC);

ZEND_API int zend_cmp_unicode_and_string(UChar *ustr, char* str, uint len);
ZEND_API int zend_cmp_unicode_and_literal(UChar *ustr, int32_t ulen, char* str, int32_t slen);

ZEND_API void zend_case_fold_string(UChar **dest, int32_t *dest_len, UChar *src, int32_t src_len, uint32_t options, UErrorCode *status);

ZEND_API int zend_is_valid_identifier(UChar *ident, int32_t ident_len);
ZEND_API int zend_normalize_identifier(UChar **dest, int32_t *dest_len, UChar *ident, int32_t ident_len, zend_bool fold_case);

/*
 * Function to get a codepoint at position n. Iterates over codepoints starting from the
 * beginning of the string. Does not check for n > length, this is left up to the caller.
 */
static inline UChar32 zend_get_codepoint_at(UChar *str, int32_t length, int32_t n)
{
	int32_t offset = 0;
	UChar32 c = 0;

	U16_FWD_N(str, offset, length, n);
	U16_GET(str, 0, offset, length, c);

	return c;
}

/*
 * Convert a single codepoint to UChar sequence (1 or 2).
 * The UChar buffer is assumed to be large enough.
 */
static inline int zend_codepoint_to_uchar(UChar32 codepoint, UChar *buf)
{
	if (U_IS_BMP(codepoint)) {
		*buf++ = (UChar) codepoint;
		return 1;
	} else if (codepoint <= UCHAR_MAX_VALUE) {
		*buf++ = (UChar) U16_LEAD(codepoint);
		*buf++ = (UChar) U16_TRAIL(codepoint);
		return 2;
	} else {
		return 0;
	}
}

#define ZEND_U_CONVERTER(c) ((c)?(c):UG(fallback_encoding_conv))

#define USTR_FREE(ustr) do { if (ustr) { efree(ustr); } } while (0);
#define UBYTES(len) ((len) * sizeof(UChar))
#define USTR_LEN(str) (UG(unicode)?u_strlen((UChar*)(str)):strlen((char*)(str)))

#define USTR_MAKE(cs) zend_ascii_to_unicode(cs, sizeof(cs) ZEND_FILE_LINE_CC)
#define USTR_MAKE_REL(cs) zend_ascii_to_unicode(cs, sizeof(cs) ZEND_FILE_LINE_RELAY_CC)
static inline UChar* zend_ascii_to_unicode(const char *cs, size_t cs_size ZEND_FILE_LINE_DC)
{
	/* u_charsToUChars() takes care of the terminating NULL */
	UChar *us = eumalloc_rel(cs_size);
	u_charsToUChars(cs, us, cs_size);
	return us;
}

#endif /* ZEND_UNICODE_H */
