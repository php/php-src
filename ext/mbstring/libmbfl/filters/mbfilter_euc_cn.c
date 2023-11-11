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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 4 Dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_euc_cn.h"

#include "unicode_table_cp936.h"

static int mbfl_filt_conv_euccn_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_euccn_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_euccn(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const unsigned char mblen_table_euccn[] = { /* 0xA1-0xFE */
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

static const char *mbfl_encoding_euc_cn_aliases[] = {"CN-GB", "EUC_CN", "eucCN", "x-euc-cn", "gb2312", NULL};

const mbfl_encoding mbfl_encoding_euc_cn = {
	mbfl_no_encoding_euc_cn,
	"EUC-CN",
	"CN-GB",
	mbfl_encoding_euc_cn_aliases,
	mblen_table_euccn,
	0,
	&vtbl_euccn_wchar,
	&vtbl_wchar_euccn,
	mb_euccn_to_wchar,
	mb_wchar_to_euccn,
	NULL
};

const struct mbfl_convert_vtbl vtbl_euccn_wchar = {
	mbfl_no_encoding_euc_cn,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_euccn_wchar,
	mbfl_filt_conv_euccn_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_euccn = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_cn,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_euccn,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_euccn_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if ((c >= 0xA1 && c <= 0xA9) || (c >= 0xB0 && c <= 0xF7)) { /* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xA0 && c < 0xFF) {
			w = (c1 - 0x81)*192 + c - 0x40;
			ZEND_ASSERT(w < cp936_ucs_table_size);
			if (w == 0x1864) {
				w = 0x30FB;
			} else if (w == 0x186A) {
				w = 0x2015;
			} else if ((w >= 0x1921 && w <= 0x192A) || w == 0x1963 || (w >= 0x1C59 && w <= 0x1C7E) || (w >= 0x1DBB && w <= 0x1DC4)) {
				w = 0;
			} else {
				w = cp936_ucs_table[w];
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

int mbfl_filt_conv_wchar_euccn(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		if (c == 0xB7 || c == 0x144 || c == 0x148 || c == 0x251 || c == 0x261) {
			s = 0;
		} else {
			s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
		}
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		if (c == 0x2015) {
			s = 0xA1AA;
		} else if (c == 0x2014 || (c >= 0x2170 && c <= 0x2179)) {
			s = 0;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		if (c == 0x30FB) {
			s = 0xA1A4;
		} else {
			s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
		}
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		if (c == 0xFF04) {
			s = 0xA1E7;
		} else if (c == 0xFF5E) {
			s = 0xA1AB;
		} else if (c >= 0xFF01 && c <= 0xFF5D) {
			s = c - 0xFF01 + 0xA3A1;
		} else if (c >= 0xFFE0 && c <= 0xFFE5) {
			s = ucs_hff_s_cp936_table[c - 0xFFE0];
		}
	}

	/* exclude CP936 extensions */
	if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
		s = 0;
	}

	if (s <= 0) {
		if (c < 0x80) {
			s = c;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s & 0xFF, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_euccn_wchar_flush(mbfl_convert_filter *filter)
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

static size_t mb_euccn_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (((c >= 0xA1 && c <= 0xA9) || (c >= 0xB0 && c <= 0xF7)) && p < e) {
			unsigned char c2 = *p++;

			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int w = (c - 0x81)*192 + c2 - 0x40;
				ZEND_ASSERT(w < cp936_ucs_table_size);
				if (w == 0x1864) {
					w = 0x30FB;
				} else if (w == 0x186A) {
					w = 0x2015;
				} else if ((w >= 0x1921 && w <= 0x192A) || w == 0x1963 || (w >= 0x1C59 && w <= 0x1C7E) || (w >= 0x1DBB && w <= 0x1DC4)) {
					w = 0;
				} else {
					w = cp936_ucs_table[w];
				}

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

static void mb_wchar_to_euccn(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			if (w != 0xB7 && w != 0x144 && w != 0x148 && w != 0x251 && w != 0x261) {
				s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
			}
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			if (w == 0x2015) {
				s = 0xA1AA;
			} else if (w != 0x2014 && (w < 0x2170 || w > 0x2179)) {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			if (w == 0x30FB) {
				s = 0xA1A4;
			} else {
				s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
			}
		} else if (w >= ucs_i_cp936_table_min && w < ucs_i_cp936_table_max) {
			s = ucs_i_cp936_table[w - ucs_i_cp936_table_min];
		} else if (w >= ucs_hff_cp936_table_min && w < ucs_hff_cp936_table_max) {
			if (w == 0xFF04) {
				s = 0xA1E7;
			} else if (w == 0xFF5E) {
				s = 0xA1AB;
			} else if (w >= 0xFF01 && w <= 0xFF5D) {
				s = w - 0xFF01 + 0xA3A1;
			} else if (w >= 0xFFE0 && w <= 0xFFE5) {
				s = ucs_hff_s_cp936_table[w - 0xFFE0];
			}
		}

		/* Exclude CP936 extensions */
		if (((s >> 8) & 0xFF) < 0xA1 || (s & 0xFF) < 0xA1) {
			s = 0;
		}

		if (!s) {
			if (w < 0x80) {
				out = mb_convert_buf_add(out, w);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_euccn);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			}
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
