/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#if HAVE_MBSTRING

/* include case folding data generated from the official UnicodeData.txt file */
#include "mbstring.h"
#include "php_unicode.h"
#include "unicode_data.h"
#include "libmbfl/mbfl/mbfilter_wchar.h"

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

static int prop_lookup(unsigned long code, unsigned long n)
{
	long l, r, m;

	/*
	 * There is an extra node on the end of the offsets to allow this routine
	 * to work right.  If the index is 0xffff, then there are no nodes for the
	 * property.
	 */
	if ((l = _ucprop_offsets[n]) == 0xffff)
		return 0;

	/*
	 * Locate the next offset that is not 0xffff.  The sentinel at the end of
	 * the array is the max index value.
	 */
	for (m = 1; n + m < _ucprop_size && _ucprop_offsets[n + m] == 0xffff; m++)
		;

	r = _ucprop_offsets[n + m] - 1;

	while (l <= r) {
		/*
		 * Determine a "mid" point and adjust to make sure the mid point is at
		 * the beginning of a range pair.
		 */
		m = (l + r) >> 1;
		m -= (m & 1);
		if (code > _ucprop_ranges[m + 1])
			l = m + 2;
		else if (code < _ucprop_ranges[m])
			r = m - 2;
		else if (code >= _ucprop_ranges[m] && code <= _ucprop_ranges[m + 1])
			return 1;
	}
	return 0;

}

MBSTRING_API int php_unicode_is_prop1(unsigned long code, int prop)
{
	return prop_lookup(code, prop);
}

MBSTRING_API int php_unicode_is_prop(unsigned long code, ...)
{
	int result = 0;
	va_list va;
	va_start(va, code);

	while (1) {
		int prop = va_arg(va, int);
		if (prop < 0) {
			break;
		}

		if (prop_lookup(code, prop)) {
			result = 1;
			break;
		}
	}

	va_end(va);
	return result;
}

#define CODE_NOT_FOUND ((unsigned long) -1)

static unsigned long case_lookup(unsigned long code, long l, long r)
{
	long m;
	const unsigned int *tmp;

	/*
	 * Do the binary search.
	 */
	while (l <= r) {
		/*
		 * Determine a "mid" point and adjust to make sure the mid point is at
		 * the beginning of a case mapping triple.
		 */
		m = (l + r) >> 1;
		tmp = &_uccase_map[m*2];
		if (code > *tmp)
			l = m + 1;
		else if (code < *tmp)
			r = m - 1;
		else if (code == *tmp)
			return tmp[1];
	}

	return CODE_NOT_FOUND;
}

MBSTRING_API unsigned long php_unicode_toupper(unsigned long code, enum mbfl_no_encoding enc)
{
	if (code < 0x80) {
		/* Fast path for ASCII */
		if (code >= 0x61 && code <= 0x7A) {
			if (enc == mbfl_no_encoding_8859_9 && code == 0x0069L) {
				return 0x0130L;
			}
			return code - 0x20;
		}
		return code;
	} else {
		long l = 0;
		long r = _uccase_len[0] - 1;
		unsigned long new_code = case_lookup(code, l, r);
		if (new_code != CODE_NOT_FOUND) {
			return new_code;
		}
		return code;
	}
}

MBSTRING_API unsigned long php_unicode_tolower(unsigned long code, enum mbfl_no_encoding enc)
{
	if (code < 0x80) {
		/* Fast path for ASCII */
		if (code >= 0x41 && code <= 0x5A) {
			if (enc == mbfl_no_encoding_8859_9 && code == 0x0049L) {
				return 0x0131L;
			}
			return code + 0x20;
		}
		return code;
	} else {
		long l = _uccase_len[0];
		long r = _uccase_len[0] + _uccase_len[1] - 1;
		unsigned long new_code = case_lookup(code, l, r);
		if (new_code != CODE_NOT_FOUND) {
			return new_code;
		}
		return code;
	}
}

MBSTRING_API unsigned long php_unicode_totitle(unsigned long code, enum mbfl_no_encoding enc)
{
	long l = _uccase_len[0] + _uccase_len[1];
	long r = _uccase_size - 1;
	unsigned long new_code = case_lookup(code, l, r);
	if (new_code != CODE_NOT_FOUND) {
		return new_code;
	}

	/* No dedicated title-case variant, use to-upper instead */
	return php_unicode_toupper(code, enc);
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
	switch (data->case_mode) {
		case PHP_UNICODE_CASE_UPPER:
			c = php_unicode_toupper(c, data->no_encoding);
			break;

		case PHP_UNICODE_CASE_LOWER:
			c = php_unicode_tolower(c, data->no_encoding);
			break;

		case PHP_UNICODE_CASE_TITLE:
		{
			int res = php_unicode_is_prop(c,
				UC_MN, UC_ME, UC_CF, UC_LM, UC_SK, UC_LU, UC_LL, UC_LT, UC_PO, UC_OS, -1);
			if (data->title_mode) {
				if (res) {
					c = php_unicode_tolower(c, data->no_encoding);
				} else {
					data->title_mode = 0;
				}
			} else {
				if (res) {
					data->title_mode = 1;
					c = php_unicode_totitle(c, data->no_encoding);
				}
			}
			break;
		}
	}
	return (*data->next_filter->filter_function)(c, data->next_filter);
}

MBSTRING_API char *php_unicode_convert_case(
		int case_mode, const char *srcstr, size_t srclen, size_t *ret_len,
		const mbfl_encoding *src_encoding)
{
	struct convert_case_data data;
	mbfl_convert_filter *from_wchar, *to_wchar;
	mbfl_string result, *result_ptr;

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
	result_ptr = mbfl_memory_device_result(&device, &result);
	mbfl_convert_filter_delete(to_wchar);
	mbfl_convert_filter_delete(from_wchar);

	if (!result_ptr) {
		return NULL;
	}

	*ret_len = result.len;
	return (char *) result.val;
}


#endif /* HAVE_MBSTRING */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
