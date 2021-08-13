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
 * The source code included in this files was separated from mbfilter_cn.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_hz.h"

#include "unicode_table_cp936.h"
#include "unicode_table_gb2312.h"

static int mbfl_filt_conv_hz_wchar_flush(mbfl_convert_filter *filter);

const mbfl_encoding mbfl_encoding_hz = {
	mbfl_no_encoding_hz,
	"HZ",
	"HZ-GB-2312",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_hz_wchar,
	&vtbl_wchar_hz
};

const struct mbfl_convert_vtbl vtbl_hz_wchar = {
	mbfl_no_encoding_hz,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_hz_wchar,
	mbfl_filt_conv_hz_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_hz = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_hz,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_hz,
	mbfl_filt_conv_any_hz_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_hz_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status & 0xf) {
	/* case 0x00: ASCII */
	/* case 0x10: GB2312 */
	case 0:
		if (c == '~') {
			filter->status += 2;
		} else if (filter->status == 0x10 && ((c > 0x20 && c <= 0x29) || (c >= 0x30 && c <= 0x77))) {
			/* DBCS first char */
			filter->cache = c;
			filter->status += 1;
		} else if (filter->status == 0 && c >= 0 && c < 0x80) { /* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			CK((*filter->output_function)(c | MBFL_WCSGROUP_THROUGH, filter->data));
		}
		break;

	/* case 0x11: GB2312 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c1 > 0x20 && c1 < 0x7F && c > 0x20 && c < 0x7F) {
			s = (c1 - 1)*192 + c + 0x40; /* GB2312 */
			if (s >= 0 && s < cp936_ucs_table_size) {
				if (s == 0x1864) {
					w = 0x30FB;
				} else if (s == 0x186A) {
					w = 0x2015;
				} else if (s == 0x186C) {
					w = 0x2225;
				} else if ((s >= 0x1920 && s <= 0x192A) || s == 0x1963 || (s >= 0x1C60 && s <= 0x1C7F) || (s >= 0x1DBB && s <= 0x1DC4)) {
					w = 0;
				} else {
					w = cp936_ucs_table[s];
				}
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = (c1 << 8) | c | MBFL_WCSPLANE_GB2312;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			w = (c1 << 8) | c | MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	/* '~' */
	case 2:
		if (c == '}' && filter->status == 0x12) {
			filter->status = 0;
		} else if (c == '{' && filter->status == 2) {
			filter->status = 0x10;
		} else if (c == '~' && filter->status == 2) {
			CK((*filter->output_function)('~', filter->data));
		} else if (c == '\n') {
			/* "~\n" is a line continuation; no output is needed, nor should we shift modes */
			filter->status -= 2;
		} else {
			/* Invalid character after ~ */
			filter->status -= 2;
			CK((*filter->output_function)(c | MBFL_WCSGROUP_THROUGH, filter->data));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int mbfl_filt_conv_hz_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 0x11) {
		/* 2-byte character was truncated */
		CK((*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_hz(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		if (c == 0xB7 || c == 0x144 || c == 0x148 || c == 0x251 || c == 0x261 || c == 0x2CA || c == 0x2CB || c == 0x2D9) {
			s = 0;
		} else {
			s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
		}
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		if (c == 0x2015) {
			s = 0xA1AA;
		} else if (c == 0x2010 || c == 0x2013 || c == 0x2014 || c == 0x2016 || c == 0x2025 || c == 0x2035 ||
				c == 0x2105 || c == 0x2109 || c == 0x2121 || (c >= 0x2170 && c <= 0x2179) || (c >= 0x2196 && c <= 0x2199) ||
				c == 0x2215 || c == 0x221F || c == 0x2223 || c == 0x2252 || c == 0x2266 || c == 0x2267 || c == 0x2295 ||
				(c >= 0x2550 && c <= 0x2573) || c == 0x22BF || c == 0x2609 || (c >= 0x2581 && c <= 0x258F) ||
				(c >= 0x2593 && c <= 0x2595) || c == 0x25BC || c == 0x25BD || (c >= 0x25E2 && c <= 0x25E5)) {
			s = 0;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		if (c == 0x30FB) {
			s = 0xA1A4;
		} else if (c == 0x3006 || c == 0x3007 || c == 0x3012 || c == 0x3231 || c == 0x32A3 || c >= 0x3300 ||
				(c >= 0x3018 && c <= 0x3040) || (c >= 0x309B && c <= 0x309E) || (c >= 0x30FC && c <= 0x30FE)) {
			s = 0;
		} else {
			s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
		}
	} else if (c >= ucs_i_gb2312_table_min && c < ucs_i_gb2312_table_max) {
		s = ucs_i_gb2312_table[c - ucs_i_gb2312_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		if (c == 0xFF04) {
			s = 0xA1E7;
		} else if (c == 0xFF5E) {
			s = 0xA1AB;
		} else if (c >= 0xFF01 && c <= 0xFF5D) {
			s = c - 0xFF01 + 0xA3A1;
		} else if (c == 0xFFE0 || c == 0xFFE1 || c == 0xFFE3 || c == 0xFFE5) {
			s = ucs_hff_s_cp936_table[c - 0xFFE0];
		}
	}

	if (s & 0x8000) {
		s -= 0x8080;
	}

	if (s <= 0) {
		s = (c == 0) ? 0 : -1;
	} else if ((s >= 0x80 && s < 0x2121) || s > 0x8080) {
		s = -1;
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)('~', filter->data));
				CK((*filter->output_function)('}', filter->data));
			}
			filter->status = 0;
			if (s == 0x7E) {
				CK((*filter->output_function)('~', filter->data));
			}
			CK((*filter->output_function)(s, filter->data));
		} else { /* GB 2312-80 */
			if ((filter->status & 0xFF00) != 0x200) {
				CK((*filter->output_function)('~', filter->data));
				CK((*filter->output_function)('{', filter->data));
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7F, filter->data));
			CK((*filter->output_function)(s & 0x7F, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

int mbfl_filt_conv_any_hz_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if (filter->status & 0xFF00) {
		CK((*filter->output_function)('~', filter->data));
		CK((*filter->output_function)('}', filter->data));
	}
	filter->status = 0;
	return 0;
}
