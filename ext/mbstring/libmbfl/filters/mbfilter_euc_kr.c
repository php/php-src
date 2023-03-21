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

#include "mbfilter.h"
#include "mbfilter_euc_kr.h"
#include "unicode_table_uhc.h"

static int mbfl_filt_conv_euckr_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_euckr_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_euckr(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const unsigned char mblen_table_euckr[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_euc_kr_aliases[] = {"EUC_KR", "eucKR", "x-euc-kr", NULL};

const mbfl_encoding mbfl_encoding_euc_kr = {
	mbfl_no_encoding_euc_kr,
	"EUC-KR",
	"EUC-KR",
	mbfl_encoding_euc_kr_aliases,
	mblen_table_euckr,
	0,
	&vtbl_euckr_wchar,
	&vtbl_wchar_euckr,
	mb_euckr_to_wchar,
	mb_wchar_to_euckr,
	NULL
};

const struct mbfl_convert_vtbl vtbl_euckr_wchar = {
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_euckr_wchar,
	mbfl_filt_conv_euckr_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_euckr = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_kr,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_euckr,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_euckr_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, w, flag;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (((c >= 0xA1 && c <= 0xAC) || (c >= 0xB0 && c <= 0xFD)) && c != 0xC9) { /* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		flag = 0;
		if (c1 >= 0xa1 && c1 <= 0xc6) {
			flag = 1;
		} else if (c1 >= 0xc7 && c1 <= 0xfe && c1 != 0xc9) {
			flag = 2;
		}
		if (flag > 0 && c >= 0xa1 && c <= 0xfe) {
			if (flag == 1) { /* 1st: 0xa1..0xc6, 2nd: 0x41..0x7a, 0x81..0xfe */
				w = (c1 - 0xa1)*190 + c - 0x41;
				ZEND_ASSERT(w < uhc2_ucs_table_size);
				w = uhc2_ucs_table[w];
			} else { /* 1st: 0xc7..0xc8,0xca..0xfe, 2nd: 0xa1..0xfe */
				w = (c1 - 0xc7)*94 + c - 0xa1;
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

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

int mbfl_filt_conv_wchar_euckr(int c, mbfl_convert_filter *filter)
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

	/* exclude UHC extension area (although we are using the UHC conversion tables) */
	if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
		s = 0;
	}

	if (s <= 0) {
		if (c < 0x80) {
			s = c;
		} else {
			s = -1;
		}
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

static int mbfl_filt_conv_euckr_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_euckr_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (((c >= 0xA1 && c <= 0xAC) || (c >= 0xB0 && c <= 0xFD)) && c != 0xC9 && p < e) {
			unsigned char c2 = *p++;

			if (c >= 0xA1 && c <= 0xC6 && c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int w = (c - 0xA1)*190 + c2 - 0x41;
				ZEND_ASSERT(w < uhc2_ucs_table_size);
				w = uhc2_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else if (c >= 0xC7 && c <= 0xFE && c != 0xC9 && c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int w = (c - 0xC7)*94 + c2 - 0xA1;
				ZEND_ASSERT(w < uhc3_ucs_table_size);
				w = uhc3_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_euckr(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_uhc_table_min && w < ucs_a1_uhc_table_max) {
			s = ucs_a1_uhc_table[w - ucs_a1_uhc_table_min];
		} else if (w >= ucs_a2_uhc_table_min && w < ucs_a2_uhc_table_max) {
			s = ucs_a2_uhc_table[w - ucs_a2_uhc_table_min];
		} else if (w >= ucs_a3_uhc_table_min && w < ucs_a3_uhc_table_max) {
			s = ucs_a3_uhc_table[w - ucs_a3_uhc_table_min];
		} else if (w >= ucs_i_uhc_table_min && w < ucs_i_uhc_table_max) {
			s = ucs_i_uhc_table[w - ucs_i_uhc_table_min];
		} else if (w >= ucs_s_uhc_table_min && w < ucs_s_uhc_table_max) {
			s = ucs_s_uhc_table[w - ucs_s_uhc_table_min];
		} else if (w >= ucs_r1_uhc_table_min && w < ucs_r1_uhc_table_max) {
			s = ucs_r1_uhc_table[w - ucs_r1_uhc_table_min];
		} else if (w >= ucs_r2_uhc_table_min && w < ucs_r2_uhc_table_max) {
			s = ucs_r2_uhc_table[w - ucs_r2_uhc_table_min];
		}

		/* Exclude UHC extension area (although we are using the UHC conversion tables) */
		if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
			s = 0;
		}

		if (!s) {
			if (w < 0x80) {
				out = mb_convert_buf_add(out, w);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_euckr);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
