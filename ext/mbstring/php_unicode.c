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
#include "libmbfl/mbfl/mbfilter_wchar.h"

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

unsigned php_unicode_tofold_raw(unsigned code, enum mbfl_no_encoding enc)
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

static inline void php_unicode_tolower_full(unsigned code, enum mbfl_no_encoding enc,
	mbfl_convert_filter* next_filter) {
	code = php_unicode_tolower_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		unsigned len = code >> 24;
		const unsigned *p = &_uccase_extra_table[code & 0xffffff];
		while (len--) {
			(next_filter->filter_function)(*++p, next_filter);
		}
	} else {
		(next_filter->filter_function)(code, next_filter);
	}
}

static inline void php_unicode_toupper_full(unsigned code, enum mbfl_no_encoding enc,
	mbfl_convert_filter* next_filter) {
	code = php_unicode_toupper_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		unsigned len = code >> 24;
		const unsigned *p = &_uccase_extra_table[code & 0xffffff];
		while (len--) {
			(next_filter->filter_function)(*++p, next_filter);
		}
	} else {
		(next_filter->filter_function)(code, next_filter);
	}
}

static inline void php_unicode_totitle_full(unsigned code, enum mbfl_no_encoding enc,
	mbfl_convert_filter* next_filter) {
	code = php_unicode_totitle_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		unsigned len = code >> 24;
		const unsigned *p = &_uccase_extra_table[code & 0xffffff];
		while (len--) {
			(next_filter->filter_function)(*++p, next_filter);
		}
	} else {
		(next_filter->filter_function)(code, next_filter);
	}
}

static inline void php_unicode_tofold_full(unsigned code, enum mbfl_no_encoding enc,
	mbfl_convert_filter* next_filter) {
	code = php_unicode_tofold_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		unsigned len = code >> 24;
		const unsigned *p = &_uccase_extra_table[code & 0xffffff];
		while (len--) {
			(next_filter->filter_function)(*++p, next_filter);
		}
	} else {
		(next_filter->filter_function)(code, next_filter);
	}
}

struct convert_case_data {
	mbfl_convert_filter *next_filter;
	enum mbfl_no_encoding no_encoding;
	int case_mode;
	int title_mode;
};

static int convert_case_filter(int c, void *void_data)
{
	struct convert_case_data *data = (struct convert_case_data *) void_data;
	unsigned code;

	/* Handle invalid characters early, as we assign special meaning to
	 * codepoints above 0xffffff. */
	if (UNEXPECTED((unsigned) c > 0xffffff)) {
		(*data->next_filter->filter_function)(c, data->next_filter);
		return 0;
	}

	switch (data->case_mode) {
		case PHP_UNICODE_CASE_UPPER_SIMPLE:
			code = php_unicode_toupper_simple(c, data->no_encoding);
			(data->next_filter->filter_function)(code, data->next_filter);
			break;

		case PHP_UNICODE_CASE_UPPER:
			php_unicode_toupper_full(c, data->no_encoding, data->next_filter);
			break;

		case PHP_UNICODE_CASE_LOWER_SIMPLE:
			code = php_unicode_tolower_simple(c, data->no_encoding);
			(data->next_filter->filter_function)(code, data->next_filter);
			break;

		case PHP_UNICODE_CASE_LOWER:
			php_unicode_tolower_full(c, data->no_encoding, data->next_filter);
			break;

		case PHP_UNICODE_CASE_FOLD:
			php_unicode_tofold_full(c, data->no_encoding, data->next_filter);
			break;

		case PHP_UNICODE_CASE_FOLD_SIMPLE:
			code = php_unicode_tofold_simple(c, data->no_encoding);
			(data->next_filter->filter_function)(code, data->next_filter);
			break;

		case PHP_UNICODE_CASE_TITLE_SIMPLE:
		case PHP_UNICODE_CASE_TITLE:
		{
			if (data->title_mode) {
				if (data->case_mode == PHP_UNICODE_CASE_TITLE_SIMPLE) {
					code = php_unicode_tolower_simple(c, data->no_encoding);
					(data->next_filter->filter_function)(code, data->next_filter);
				} else {
					php_unicode_tolower_full(c, data->no_encoding, data->next_filter);
				}
			} else {
				if (data->case_mode == PHP_UNICODE_CASE_TITLE_SIMPLE) {
					code = php_unicode_totitle_simple(c, data->no_encoding);
					(data->next_filter->filter_function)(code, data->next_filter);
				} else {
					php_unicode_totitle_full(c, data->no_encoding, data->next_filter);
				}
			}
			if (!php_unicode_is_case_ignorable(c)) {
				data->title_mode = php_unicode_is_cased(c);
			}
			break;
		}
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return 0;
}

MBSTRING_API char *php_unicode_convert_case(
		int case_mode, const char *srcstr, size_t srclen, size_t *ret_len,
		const mbfl_encoding *src_encoding, int illegal_mode, int illegal_substchar)
{
	struct convert_case_data data;
	mbfl_convert_filter *from_wchar, *to_wchar;
	mbfl_string result;

	mbfl_memory_device device;
	mbfl_memory_device_init(&device, srclen + 1, 0);

	/* encoding -> wchar filter */
	to_wchar = mbfl_convert_filter_new(src_encoding,
			&mbfl_encoding_wchar, convert_case_filter, NULL, &data);
	if (to_wchar == NULL) {
		mbfl_memory_device_clear(&device);
		return NULL;
	}

	/* wchar -> encoding filter */
	from_wchar = mbfl_convert_filter_new(
			&mbfl_encoding_wchar, src_encoding,
			mbfl_memory_device_output, NULL, &device);
	if (from_wchar == NULL) {
		mbfl_convert_filter_delete(to_wchar);
		mbfl_memory_device_clear(&device);
		return NULL;
	}

	to_wchar->illegal_mode = illegal_mode;
	to_wchar->illegal_substchar = illegal_substchar;
	from_wchar->illegal_mode = illegal_mode;
	from_wchar->illegal_substchar = illegal_substchar;

	data.next_filter = from_wchar;
	data.no_encoding = src_encoding->no_encoding;
	data.case_mode = case_mode;
	data.title_mode = 0;

	{
		/* feed data */
		const unsigned char *p = (const unsigned char *) srcstr;
		size_t n = srclen;
		while (n > 0) {
			if ((*to_wchar->filter_function)(*p++, to_wchar) < 0) {
				break;
			}
			n--;
		}
	}

	mbfl_convert_filter_flush(to_wchar);
	mbfl_convert_filter_flush(from_wchar);
	mbfl_memory_device_result(&device, &result);
	mbfl_convert_filter_delete(to_wchar);
	mbfl_convert_filter_delete(from_wchar);

	*ret_len = result.len;
	return (char *) result.val;
}
