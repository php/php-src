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
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter_kr.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

/* UHC was introduced by MicroSoft in Windows 95, and is also known as CP949.
 * It is the same as EUC-KR, but with 8,822 additional characters added to
 * complete all the characters in the Johab charset. */

#include "mbfilter.h"
#include "mbfilter_uhc.h"
#define UNICODE_TABLE_UHC_DEF
#include "unicode_table_uhc.h"

static int mbfl_filt_conv_uhc_wchar_flush(mbfl_convert_filter *filter);

static const unsigned char mblen_table_uhc[] = { /* 0x81-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_uhc_aliases[] = {"CP949", NULL};

const mbfl_encoding mbfl_encoding_uhc = {
	mbfl_no_encoding_uhc,
	"UHC",
	"UHC",
	mbfl_encoding_uhc_aliases,
	mblen_table_uhc,
	0,
	&vtbl_uhc_wchar,
	&vtbl_wchar_uhc
};

const struct mbfl_convert_vtbl vtbl_uhc_wchar = {
	mbfl_no_encoding_uhc,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_uhc_wchar,
	mbfl_filt_conv_uhc_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_uhc = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_uhc,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_uhc,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_uhc_wchar(int c, mbfl_convert_filter *filter)
{
	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0x80 && c < 0xfe && c != 0xc9) { /* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		int c1 = filter->cache, w = 0;

		if (c1 >= 0x81 && c1 <= 0xa0 && c >= 0x41 && c <= 0xfe) {
			w = (c1 - 0x81)*190 + (c - 0x41);
			if (w >= 0 && w < uhc1_ucs_table_size) {
				w = uhc1_ucs_table[w];
			}
		} else if (c1 >= 0xa1 && c1 <= 0xc6 && c >= 0x41 && c <= 0xfe) {
			w = (c1 - 0xa1)*190 + (c - 0x41);
			if (w >= 0 && w < uhc2_ucs_table_size) {
				w = uhc2_ucs_table[w];
			}
		} else if (c1 >= 0xc7 && c1 < 0xfe && c >= 0xa1 && c <= 0xfe) {
			w = (c1 - 0xc7)*94 + (c - 0xa1);
			if (w >= 0 && w < uhc3_ucs_table_size) {
				w = uhc3_ucs_table[w];
			}
		}

		if (w == 0) {
			w = MBFL_BAD_INPUT;
		}
		CK((*filter->output_function)(w, filter->data));
		break;

	default:
		filter->status = 0;
		break;
	}

	return 0;
}

static int mbfl_filt_conv_uhc_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_uhc(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_uhc_table_min && c < ucs_a1_uhc_table_max) {
		s = ucs_a1_uhc_table[c - ucs_a1_uhc_table_min];
	} else if (c >= ucs_a2_uhc_table_min && c < ucs_a2_uhc_table_max) {
		s = ucs_a2_uhc_table[c - ucs_a2_uhc_table_min];
	} else if (c >= ucs_a3_uhc_table_min && c < ucs_a3_uhc_table_max) {
		s = ucs_a3_uhc_table[c - ucs_a3_uhc_table_min];
	} else if (c >= ucs_i_uhc_table_min && c < ucs_i_uhc_table_max) {
		s = ucs_i_uhc_table[c - ucs_i_uhc_table_min];
	} else if (c >= ucs_s_uhc_table_min && c < ucs_s_uhc_table_max) {
		s = ucs_s_uhc_table[c - ucs_s_uhc_table_min];
	} else if (c >= ucs_r1_uhc_table_min && c < ucs_r1_uhc_table_max) {
		s = ucs_r1_uhc_table[c - ucs_r1_uhc_table_min];
	} else if (c >= ucs_r2_uhc_table_min && c < ucs_r2_uhc_table_max) {
		s = ucs_r2_uhc_table[c - ucs_r2_uhc_table_min];
	}

	if (s == 0 && c != 0) {
		s = -1;
	}

	if (s >= 0) {
		if (s < 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}
