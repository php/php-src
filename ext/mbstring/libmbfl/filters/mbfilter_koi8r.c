/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this part: Den V. Tsopa <tdv@edisoft.ru>
 *
 */
/*
 * The source code included in this files was separated from mbfilter_ru.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_koi8r.h"
#include "unicode_table_koi8r.h"

static int mbfl_filt_ident_koi8r(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_koi8r_aliases[] = {"KOI8-R", "KOI8R", NULL};

const mbfl_encoding mbfl_encoding_koi8r = {
	mbfl_no_encoding_koi8r,
	"KOI8-R",
	"KOI8-R",
	(const char *(*)[])&mbfl_encoding_koi8r_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_koi8r_wchar,
	&vtbl_wchar_koi8r
};

const struct mbfl_identify_vtbl vtbl_identify_koi8r = {
	mbfl_no_encoding_koi8r,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_koi8r
};

const struct mbfl_convert_vtbl vtbl_wchar_koi8r = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_koi8r,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_koi8r,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_koi8r_wchar = {
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_koi8r_wchar,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * koi8r => wchar
 */
int
mbfl_filt_conv_koi8r_wchar(int c, mbfl_convert_filter *filter)
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

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => koi8r
 */
int
mbfl_filt_conv_wchar_koi8r(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c < 0x80) {
		s = c;
	} else {
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
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

static int mbfl_filt_ident_koi8r(int c, mbfl_identify_filter *filter)
{
	if (c >= 0x80 && c < 0xff)
		filter->flag = 0;
	else
		filter->flag = 1; /* not it */
	return c;
}
