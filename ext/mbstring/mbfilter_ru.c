/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 The PHP Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Den V. Tsopa <tdv@edisoft.ru>                                |
   +----------------------------------------------------------------------+
 */

/*
 * "russian code filter and converter"
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"

#if defined(HAVE_MBSTR_RU)
#include "mbfilter.h"
#include "unicode_table_ru.h"

/*
 * encoding filter
 */
#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * cp1251 => wchar
 */
int
mbfl_filt_conv_cp1251_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int s;

	if (c >= 0 && c < cp1251_ucs_table_min) {
		s = c;
	} else if (c >= cp1251_ucs_table_min && c < 0x100) {
		s = cp1251_ucs_table[c - cp1251_ucs_table_min];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_CP1251;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data TSRMLS_CC));

	return c;
}

/*
 * wchar => cp1251
 */
int
mbfl_filt_conv_wchar_cp1251(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int s, n;

	s = -1;
	n = cp1251_ucs_table_len-1;
	while (n >= 0) {
		if (c == cp1251_ucs_table[n]) {
			s = cp1251_ucs_table_min + n;
			break;
		}
		n--;
	}
	if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_CP1251) {
		s = c & MBFL_WCSPLANE_MASK;
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data TSRMLS_CC));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}

/*
 * cp866 => wchar
 */
int
mbfl_filt_conv_cp866_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int s;

	if (c >= 0 && c < cp866_ucs_table_min) {
		s = c;
	} else if (c >= cp866_ucs_table_min && c < 0x100) {
		s = cp866_ucs_table[c - cp866_ucs_table_min];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_CP866;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data TSRMLS_CC));

	return c;
}

/*
 * wchar => cp866
 */
int
mbfl_filt_conv_wchar_cp866(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int s, n;

	s = -1;
	n = cp866_ucs_table_len-1;
	while (n >= 0) {
		if (c == cp866_ucs_table[n]) {
			s = cp866_ucs_table_min + n;
			break;
		}
		n--;
	}
	if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_CP866) {
		s = c & MBFL_WCSPLANE_MASK;
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data TSRMLS_CC));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}

/*
 * koi8r => wchar
 */
int
mbfl_filt_conv_koi8r_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int s;

	if (c >= 0 && c < koi8r_ucs_table_min) {
		s = c;
	} else if (c >= koi8r_ucs_table_min && c < 0x100) {
		s = koi8r_ucs_table[c - koi8r_ucs_table_min];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_KOI8R;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data TSRMLS_CC));

	return c;
}

/*
 * wchar => koi8r
 */
int
mbfl_filt_conv_wchar_koi8r(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int s, n;

	s = -1;
	n = koi8r_ucs_table_len-1;
	while (n >= 0) {
		if (c == koi8r_ucs_table[n]) {
			s = koi8r_ucs_table_min + n;
			break;
		}
		n--;
	}
	if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_KOI8R) {
		s = c & MBFL_WCSPLANE_MASK;
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data TSRMLS_CC));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}

#endif /* HAVE_MBSTR_RU */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
