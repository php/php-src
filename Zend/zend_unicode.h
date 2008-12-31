/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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
#include <unicode/uclean.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/ustdio.h>
#include <unicode/uchar.h>
#include <unicode/uloc.h>
#include <unicode/ucol.h>

enum {
  ZEND_CONV_ERROR_STOP,
  ZEND_CONV_ERROR_SKIP,
  ZEND_CONV_ERROR_SUBST,
  ZEND_CONV_ERROR_ESCAPE_UNICODE,
  ZEND_CONV_ERROR_ESCAPE_ICU,
  ZEND_CONV_ERROR_ESCAPE_JAVA,
  ZEND_CONV_ERROR_ESCAPE_XML_DEC,
  ZEND_CONV_ERROR_ESCAPE_XML_HEX,
  ZEND_CONV_ERROR_LAST_ENUM,

  ZEND_CONV_ERROR_EXCEPTION	= 0x100
};

typedef enum {
	ZEND_FROM_UNICODE,
	ZEND_TO_UNICODE,
} zend_conv_direction;


typedef struct _zend_collator {
	UCollator    *coll;
	int       refcount;
} zend_collator;


extern ZEND_API zend_class_entry *unicodeConversionException;


/* internal functions */

int zend_set_converter_encoding(UConverter **converter, const char *encoding);
void zend_set_converter_error_mode(UConverter *conv, zend_conv_direction dir, uint16_t error_mode);
void zend_set_converter_subst_char(UConverter *conv, UChar *subst_char);
void zend_register_unicode_exceptions(TSRMLS_D);
void zend_update_converters_error_behavior(TSRMLS_D);
zend_collator* zend_collator_create(UCollator *coll);
void zend_collator_destroy(zend_collator *zcoll);


/* API functions */

ZEND_API void zend_convert_encodings(UConverter *target_conv, UConverter *source_conv, char **target, int *target_len, const char *source, int source_len, UErrorCode *status);
ZEND_API char* zend_unicode_to_ascii(const UChar *us, int us_len TSRMLS_DC);

ZEND_API int zend_string_to_unicode_ex(UConverter *conv, UChar **target, int *target_len, const char *source, int source_len, UErrorCode *status);
ZEND_API int zend_string_to_unicode(UConverter *conv, UChar **u, int *u_len, char *s, int s_len TSRMLS_DC);
ZEND_API int zend_unicode_to_string_ex(UConverter *conv, char **s, int *s_len, const UChar *u, int u_len, UErrorCode *status);
ZEND_API int zend_unicode_to_string(UConverter *conv, char **s, int *s_len, const UChar *u, int u_len TSRMLS_DC);

ZEND_API int zval_string_to_unicode_ex(zval *string, UConverter *conv TSRMLS_DC);
ZEND_API int zval_string_to_unicode(zval *string TSRMLS_DC);
ZEND_API int zval_unicode_to_string_ex(zval *string, UConverter *conv TSRMLS_DC);
ZEND_API int zval_unicode_to_string(zval *string TSRMLS_DC);

ZEND_API int zend_cmp_unicode_and_string(UChar *ustr, char* str, uint len);
ZEND_API int zend_cmp_unicode_and_literal(UChar *ustr, int ulen, char* str, int slen);

ZEND_API void zend_case_fold_string(UChar **dest, int *dest_len, UChar *src, int src_len, uint32_t options, UErrorCode *status);

ZEND_API int zend_is_valid_identifier(UChar *ident, int ident_len);
ZEND_API int zend_normalize_identifier(UChar **dest, int *dest_len, UChar *ident, int ident_len, zend_bool fold_case);

ZEND_API void zend_raise_conversion_error_ex(char *message, UConverter *conv, zend_conv_direction dir, int error_char_offset TSRMLS_DC);

/*
 * Function to get a codepoint at position n. Iterates over codepoints starting from the
 * beginning of the string. Does not check for n > length, this is left up to the caller.
 */
static inline UChar32 zend_get_codepoint_at(UChar *str, int length, int n)
{
	int32_t offset = 0;
	UChar32 c = 0;

	if (n > 0) {
		U16_FWD_N(str, offset, length, n);
	}
	U16_NEXT(str, offset, length, c);

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

#define ZSTR_LEN(__type, __str) ((__type==IS_UNICODE)?u_strlen(__str.u):strlen(__str.s))

#define ZEND_U_CONVERTER(c) ((c)?(c):UG(fallback_encoding_conv))

#define USTR_FREE(ustr) do { if (ustr) { efree(ustr); } } while (0);
#define UBYTES(len) ((len) * sizeof(UChar))
#define USTR_LEN(str) (UG(unicode)?u_strlen((str).u):strlen((str).s))
#define USTR_VAL(str) (UG(unicode)?(str).u:(str).s)

#define USTR_BYTES(__type, __length) \
	((__type == IS_UNICODE)?UBYTES(__length):__length)

#define TEXT_BYTES(__chars_len) \
	(UG(unicode) ? UBYTES(__chars_len) : __chars_len)

#define TEXT_CHARS(__bytes_len) \
	(UG(unicode) ? (__bytes_len / sizeof(UChar)) : __bytes_len)

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
