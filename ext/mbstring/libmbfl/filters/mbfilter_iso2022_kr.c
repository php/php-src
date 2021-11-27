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

/* ISO-2022-KR is defined in RFC 1557
 * The RFC says that _each_ line which uses KS X 1001 characters must start
 * with an escape sequence of ESC $ ) C
 * We don't enforce that for ISO-2022-KR input */

#include "mbfilter.h"
#include "mbfilter_iso2022_kr.h"
#include "unicode_table_uhc.h"

static int mbfl_filt_conv_2022kr_wchar_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_any_2022kr_flush(mbfl_convert_filter *filter);

const mbfl_encoding mbfl_encoding_2022kr = {
	mbfl_no_encoding_2022kr,
	"ISO-2022-KR",
	"ISO-2022-KR",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022kr_wchar,
	&vtbl_wchar_2022kr,
	NULL,
	NULL
};

const struct mbfl_convert_vtbl vtbl_wchar_2022kr = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022kr,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022kr,
	mbfl_filt_conv_any_2022kr_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_2022kr_wchar = {
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_2022kr_wchar,
	mbfl_filt_conv_2022kr_wchar_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_2022kr_wchar(int c, mbfl_convert_filter *filter)
{
	int w = 0;

	switch (filter->status & 0xf) {
	/* case 0x00: ASCII */
	/* case 0x10: KSC5601 */
	case 0:
		if (c == 0x1b) { /* ESC */
			filter->status += 2;
		} else if (c == 0x0f) { /* shift in (ASCII) */
			filter->status = 0;
		} else if (c == 0x0e) { /* shift out (KSC5601) */
			filter->status = 0x10;
		} else if ((filter->status & 0x10) && c > 0x20 && c < 0x7f) {
			/* KSC5601 lead byte */
			filter->cache = c;
			filter->status = 0x11;
		} else if ((filter->status & 0x10) == 0 && c >= 0 && c < 0x80) {
			/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0x10;
		int c1 = filter->cache;
		int flag = 0;

		if (c1 > 0x20 && c1 < 0x47) {
			flag = 1;
		} else if (c1 >= 0x47 && c1 <= 0x7e && c1 != 0x49) {
			flag = 2;
		}

		if (flag > 0 && c > 0x20 && c < 0x7f) {
			if (flag == 1) {
				if (c1 != 0x22 || c <= 0x65) {
					w = (c1 - 0x21)*190 + (c - 0x41) + 0x80;
					ZEND_ASSERT(w < uhc2_ucs_table_size);
					w = uhc2_ucs_table[w];
				}
			} else {
				w = (c1 - 0x47)*94 + c - 0x21;
				ZEND_ASSERT(w < uhc3_ucs_table_size);
				w = uhc3_ucs_table[w];
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2: /* ESC */
		if (c == '$') {
			filter->status++;
		} else {
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* ESC $ */
		if (c == ')') {
			filter->status++;
		} else {
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 4: /* ESC $ ) */
		filter->status = 0;
		if (c != 'C') {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_2022kr_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* 2-byte character was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_2022kr(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s = 0;

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

	c1 = (s >> 8) & 0xff;
	c2 = s & 0xff;
	/* exclude UHC extension area */
	if (c1 < 0xa1 || c2 < 0xa1) {
		s = c;
	}

	if (s & 0x8000) {
		s -= 0x8080;
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else {
			s = -1;
		}
	} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
		s = -1;
	}

	if (s >= 0) {
		if (s < 0x80 && s >= 0) { /* ASCII */
			if (filter->status & 0x10) {
				CK((*filter->output_function)(0x0f, filter->data)); /* shift in */
				filter->status &= ~0x10;
			}
			CK((*filter->output_function)(s, filter->data));
		} else {
			if ((filter->status & 0x100) == 0) {
				CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)(')', filter->data));
				CK((*filter->output_function)('C', filter->data));
				filter->status |= 0x100;
			}
			if ((filter->status & 0x10) == 0) {
				CK((*filter->output_function)(0x0e, filter->data)); /* shift out */
				filter->status |= 0x10;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_any_2022kr_flush(mbfl_convert_filter *filter)
{
	/* back to ascii */
	if (filter->status & 0xff00) {
		CK((*filter->output_function)(0x0f, filter->data)); /* shift in */
	}

	filter->status = filter->cache = 0;

	if (filter->flush_function) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}
