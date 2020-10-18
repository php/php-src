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

#include "mbfilter.h"
#include "mbfilter_cp1251.h"
#include "unicode_table_cp1251.h"

static int mbfl_filt_ident_cp1251(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_cp1251_aliases[] = {"CP1251", "CP-1251", "WINDOWS-1251", NULL};

const mbfl_encoding mbfl_encoding_cp1251 = {
	mbfl_no_encoding_cp1251,
	"Windows-1251",
	"Windows-1251",
	mbfl_encoding_cp1251_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_cp1251_wchar,
	&vtbl_wchar_cp1251
};

const struct mbfl_identify_vtbl vtbl_identify_cp1251 = {
	mbfl_no_encoding_cp1251,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_cp1251
};

const struct mbfl_convert_vtbl vtbl_wchar_cp1251 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp1251,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp1251,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_cp1251_wchar = {
	mbfl_no_encoding_cp1251,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp1251_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_cp1251_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c < cp1251_ucs_table_min) {
		s = c;
	} else {
		s = cp1251_ucs_table[c - cp1251_ucs_table_min];
		if (!s) {
			s = c | MBFL_WCSGROUP_THROUGH;
		}
	}

	CK((*filter->output_function)(s, filter->data));
	return c;
}

int mbfl_filt_conv_wchar_cp1251(int c, mbfl_convert_filter *filter)
{
	if (c < 0x80) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		for (int n = 0; n < cp1251_ucs_table_len; n++) {
			if (c == cp1251_ucs_table[n]) {
				CK((*filter->output_function)(cp1251_ucs_table_min + n, filter->data));
				return c;
			}
		}
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

static int mbfl_filt_ident_cp1251(int c, mbfl_identify_filter *filter)
{
	/* Only one byte in this single-byte encoding is not used */
	if (c == 0x98) {
		filter->flag = 1;
	}
	return c;
}
