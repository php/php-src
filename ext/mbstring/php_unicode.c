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
   | Author: Wez Furlong (wez@thebrainroom.com)                           |
   +----------------------------------------------------------------------+

	Based on code from ucdata-2.5, which has the following Copyright:

	Copyright 2001 Computing Research Labs, New Mexico State University

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
*/

#include "php.h"

/* include case folding data generated from the official UnicodeData.txt file */
#include "mbstring.h"
#include "php_unicode.h"
#include "unicode_data.h"

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

static bool prop_lookup(unsigned long code, unsigned long n)
{
	long l = _ucprop_offsets[n];
	long r = _ucprop_offsets[n + 1] - 1;
	while (l <= r) {
		/*
		 * Determine a "mid" point and adjust to make sure the mid point is at
		 * the beginning of a range pair.
		 */
		long m = (l + r) >> 1;
		m -= (m & 1);
		if (code > _ucprop_ranges[m + 1])
			l = m + 2;
		else if (code < _ucprop_ranges[m])
			r = m - 2;
		else
			return true;
	}
	return false;

}

MBSTRING_API bool php_unicode_is_prop1(unsigned long code, int prop)
{
	return prop_lookup(code, prop);
}

MBSTRING_API bool php_unicode_is_prop(unsigned long code, ...)
{
	bool result = false;
	va_list va;
	va_start(va, code);

	while (1) {
		int prop = va_arg(va, int);
		if (prop < 0) {
			break;
		}

		if (prop_lookup(code, prop)) {
			result = true;
			break;
		}
	}

	va_end(va);
	return result;
}

static inline unsigned mph_hash(unsigned d, unsigned x) {
	x ^= d;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	return x;
}

#define CODE_NOT_FOUND ((unsigned) -1)

static inline unsigned mph_lookup(
		unsigned code,
		const short *g_table, unsigned g_table_size,
		const unsigned *table, unsigned table_size)
{
	short g = g_table[mph_hash(0, code) % g_table_size];

	unsigned idx;
	if (g <= 0) {
		idx = -g;
	} else {
		idx = mph_hash(g, code) % table_size;
	}

	if (table[2*idx] == code) {
		return table[2*idx + 1];
	}
	return CODE_NOT_FOUND;
}

#define CASE_LOOKUP(code, type) \
	mph_lookup(code, _uccase_##type##_g, _uccase_##type##_g_size, \
			_uccase_##type##_table, _uccase_##type##_table_size)

static unsigned php_unicode_toupper_raw(unsigned code, enum mbfl_no_encoding enc)
{
	/* After the ASCII characters, the first codepoint with an uppercase version
	 * is 0xB5 (MICRO SIGN) */
	if (code < 0xB5) {
		/* Fast path for ASCII */
		if (code >= 0x61 && code <= 0x7A) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x69)) {
				return 0x130;
			}
			return code - 0x20;
		}
		return code;
	} else {
		unsigned new_code = CASE_LOOKUP(code, upper);
		if (new_code != CODE_NOT_FOUND) {
			return new_code;
		}
		return code;
	}
}

static unsigned php_unicode_tolower_raw(unsigned code, enum mbfl_no_encoding enc)
{
	/* After the ASCII characters, the first codepoint with a lowercase version
	 * is 0xC0 (LATIN CAPITAL LETTER A WITH GRAVE) */
	if (code < 0xC0) {
		/* Fast path for ASCII */
		if (code >= 0x41 && code <= 0x5A) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x0049L)) {
				return 0x0131L;
			}
			return code + 0x20;
		}
		return code;
	} else {
		unsigned new_code = CASE_LOOKUP(code, lower);
		if (new_code != CODE_NOT_FOUND) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x130)) {
				return 0x69;
			}
			return new_code;
		}
		return code;
	}
}

static unsigned php_unicode_totitle_raw(unsigned code, enum mbfl_no_encoding enc)
{
	unsigned new_code = CASE_LOOKUP(code, title);
	if (new_code != CODE_NOT_FOUND) {
		return new_code;
	}

	/* No dedicated title-case variant, use to-upper instead */
	return php_unicode_toupper_raw(code, enc);
}

static unsigned php_unicode_tofold_raw(unsigned code, enum mbfl_no_encoding enc)
{
	if (code < 0x80) {
		/* Fast path for ASCII */
		if (code >= 0x41 && code <= 0x5A) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x49)) {
				return 0x131;
			}
			return code + 0x20;
		}
		return code;
	} else {
		unsigned new_code = CASE_LOOKUP(code, fold);
		if (new_code != CODE_NOT_FOUND) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x130)) {
				return 0x69;
			}
			return new_code;
		}
		return code;
	}
}

static inline unsigned php_unicode_tolower_simple(unsigned code, enum mbfl_no_encoding enc) {
	code = php_unicode_tolower_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		return _uccase_extra_table[code & 0xffffff];
	}
	return code;
}
static inline unsigned php_unicode_toupper_simple(unsigned code, enum mbfl_no_encoding enc) {
	code = php_unicode_toupper_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		return _uccase_extra_table[code & 0xffffff];
	}
	return code;
}
static inline unsigned php_unicode_totitle_simple(unsigned code, enum mbfl_no_encoding enc) {
	code = php_unicode_totitle_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		return _uccase_extra_table[code & 0xffffff];
	}
	return code;
}
static inline unsigned php_unicode_tofold_simple(unsigned code, enum mbfl_no_encoding enc) {
	code = php_unicode_tofold_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		return _uccase_extra_table[code & 0xffffff];
	}
	return code;
}

static uint32_t *emit_special_casing_sequence(uint32_t w, uint32_t *out)
{
	unsigned int len = w >> 24;
	const unsigned int *p = &_uccase_extra_table[w & 0xFFFFFF];
	while (len--) {
		*out++ = *++p;
	}
	return out;
}

MBSTRING_API zend_string *php_unicode_convert_case(php_case_mode case_mode, const char *srcstr, size_t in_len, const mbfl_encoding *src_encoding, int illegal_mode, uint32_t illegal_substchar)
{
	/* A Unicode codepoint can expand out to up to 3 codepoints when uppercased, lowercased, or title cased
	 * See http://www.unicode.org/Public/UNIDATA/SpecialCasing.txt */
	uint32_t wchar_buf[64], converted_buf[192];
	unsigned int state = 0, title_mode = 0;
	unsigned char *in = (unsigned char*)srcstr;
	enum mbfl_no_encoding enc = src_encoding->no_encoding;

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, in_len + 1, illegal_substchar, illegal_mode);

	while (in_len) {
		size_t out_len = src_encoding->to_wchar(&in, &in_len, wchar_buf, 64, &state);
		ZEND_ASSERT(out_len <= 64);
		uint32_t *p = converted_buf;

		/* In all cases, handle invalid characters early, as we assign special meaning to codepoints > 0xFFFFFF */
		switch (case_mode) {
		case PHP_UNICODE_CASE_UPPER_SIMPLE:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				*p++ = (UNEXPECTED(w > 0xFFFFFF)) ? w : php_unicode_toupper_simple(w, enc);
			}
			break;

		case PHP_UNICODE_CASE_LOWER_SIMPLE:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				*p++ = (UNEXPECTED(w > 0xFFFFFF)) ? w : php_unicode_tolower_simple(w, enc);
			}
			break;

		case PHP_UNICODE_CASE_FOLD_SIMPLE:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				*p++ = (UNEXPECTED(w > 0xFFFFFF)) ? w : php_unicode_tofold_simple(w, enc);
			}
			break;

		case PHP_UNICODE_CASE_TITLE_SIMPLE:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				if (UNEXPECTED(w > 0xFFFFFF)) {
					*p++ = w;
					continue;
				}
				*p++ = title_mode ? php_unicode_tolower_simple(w, enc) : php_unicode_totitle_simple(w, enc);
				if (!php_unicode_is_case_ignorable(w)) {
					title_mode = php_unicode_is_cased(w);
				}
			}
			break;

		case PHP_UNICODE_CASE_UPPER:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				if (UNEXPECTED(w > 0xFFFFFF)) {
					*p++ = w;
					continue;
				}
				w = php_unicode_toupper_raw(w, enc);
				if (UNEXPECTED(w > 0xFFFFFF)) {
					p = emit_special_casing_sequence(w, p);
				} else {
					*p++ = w;
				}
			}
			break;

		case PHP_UNICODE_CASE_LOWER:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				if (UNEXPECTED(w > 0xFFFFFF)) {
					*p++ = w;
					continue;
				}
				w = php_unicode_tolower_raw(w, enc);
				if (UNEXPECTED(w > 0xFFFFFF)) {
					p = emit_special_casing_sequence(w, p);
				} else {
					*p++ = w;
				}
			}
			break;

		case PHP_UNICODE_CASE_FOLD:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				if (UNEXPECTED(w > 0xFFFFFF)) {
					*p++ = w;
					continue;
				}
				w = php_unicode_tofold_raw(w, enc);
				if (UNEXPECTED(w > 0xFFFFFF)) {
					p = emit_special_casing_sequence(w, p);
				} else {
					*p++ = w;
				}
			}
			break;

		case PHP_UNICODE_CASE_TITLE:
			for (int i = 0; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				if (UNEXPECTED(w > 0xFFFFFF)) {
					*p++ = w;
					continue;
				}
				uint32_t w2 = title_mode ? php_unicode_tolower_raw(w, enc) : php_unicode_totitle_raw(w, enc);
				if (UNEXPECTED(w2 > 0xFFFFFF)) {
					p = emit_special_casing_sequence(w2, p);
				} else {
					*p++ = w2;
				}
				if (!php_unicode_is_case_ignorable(w)) {
					title_mode = php_unicode_is_cased(w);
				}
			}
			break;

			EMPTY_SWITCH_DEFAULT_CASE()
		}

		ZEND_ASSERT(p - converted_buf <= 192);
		src_encoding->from_wchar(converted_buf, p - converted_buf, &buf, !in_len);
	}

	return mb_convert_buf_result(&buf);
}
