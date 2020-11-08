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
 * The author of this part: Haluk AKIN <halukakin@gmail.com>
 *
 */
/*
 * The source code included in this files was separated from mbfilter_ru.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_cp1254.h"
#include "unicode_table_cp1254.h"

static int mbfl_filt_ident_cp1254(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_cp1254_aliases[] = {"CP1254", "CP-1254", "WINDOWS-1254", NULL};

const mbfl_encoding mbfl_encoding_cp1254 = {
	mbfl_no_encoding_cp1254,
	"Windows-1254",
	"Windows-1254",
	mbfl_encoding_cp1254_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_cp1254_wchar,
	&vtbl_wchar_cp1254
};

const struct mbfl_identify_vtbl vtbl_identify_cp1254 = {
	mbfl_no_encoding_cp1254,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_cp1254
};

const struct mbfl_convert_vtbl vtbl_cp1254_wchar = {
	mbfl_no_encoding_cp1254,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp1254_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp1254 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp1254,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp1254,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_wchar_cp1254(int c, mbfl_convert_filter *filter)
{
	if (c < 0x80) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		for (int n = 0; n < cp1254_ucs_table_len; n++) {
			if (c == cp1254_ucs_table[n]) {
				CK((*filter->output_function)(cp1254_ucs_table_min + n, filter->data));
				return c;
			}
		}
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

int mbfl_filt_conv_cp1254_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c < cp1254_ucs_table_min) {
		s = c;
	} else {
		s = cp1254_ucs_table[c - cp1254_ucs_table_min];
		if (!s) {
			s = c | MBFL_WCSGROUP_THROUGH;
		}
	}

	CK((*filter->output_function)(s, filter->data));
	return c;
}

static int mbfl_filt_ident_cp1254(int c, mbfl_identify_filter *filter)
{
	if (c >= 0x81 && c <= 0x9E && !cp1254_ucs_table[c - cp1254_ucs_table_min]) {
		filter->flag = 1;
	}
	return c;
}
