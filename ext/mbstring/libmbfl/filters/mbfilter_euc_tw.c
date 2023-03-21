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
 * The author of this file: Rui Hirokawa <hirokawa@php.net>
 *
 */
/*
 * The source code included in this files was separated from mbfilter_tw.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_euc_tw.h"

#include "unicode_table_cns11643.h"

static int mbfl_filt_conv_euctw_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_euctw_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_euctw(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const unsigned char mblen_table_euctw[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};


static const char *mbfl_encoding_euc_tw_aliases[] = {"EUC_TW", "eucTW", "x-euc-tw", NULL};

const mbfl_encoding mbfl_encoding_euc_tw = {
	mbfl_no_encoding_euc_tw,
	"EUC-TW",
	"EUC-TW",
	mbfl_encoding_euc_tw_aliases,
	mblen_table_euctw,
	0,
	&vtbl_euctw_wchar,
	&vtbl_wchar_euctw,
	mb_euctw_to_wchar,
	mb_wchar_to_euctw,
	NULL
};

const struct mbfl_convert_vtbl vtbl_euctw_wchar = {
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_euctw_wchar,
	mbfl_filt_conv_euctw_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_euctw = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_tw,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_euctw,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_euctw_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (((c >= 0xA1 && c <= 0xA6) || (c >= 0xC2 && c <= 0xFD)) && c != 0xC3) { /* 2-byte character, first byte */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8E) { /* 4-byte character, first byte */
			filter->status = 2;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* 2-byte character, second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xA0 && c < 0xFF) {
			w = (c1 - 0xA1)*94 + (c - 0xA1);
			if (w >= 0 && w < cns11643_1_ucs_table_size) {
				w = cns11643_1_ucs_table[w];
			} else {
				w = 0;
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2: /* got 0x8e, second byte */
		if (c == 0xA1 || c == 0xA2 || c == 0xAE) {
			filter->status = 3;
			filter->cache = c - 0xA1;
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* got 0x8e, third byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0xA1 && ((c1 == 0 && ((c >= 0xA1 && c <= 0xA6) || (c >= 0xC2 && c <= 0xFD)) && c != 0xC3) ||
				(c1 == 1 && c <= 0xF2) || (c1 == 13 && c <= 0xE7))) {
			filter->status = 4;
			filter->cache = (c1 << 8) + c - 0xA1;
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 4:	/* multi-byte character, fourth byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c1 <= 0xDFF && c > 0xA0 && c < 0xFF) {
			int plane = (c1 & 0xF00) >> 8; /* This is actually the CNS-11643 plane minus one */
			s = (c1 & 0xFF)*94 + c - 0xA1;
			w = 0;
			if (s >= 0) {
				/* A later version of CNS-11643 moved all the characters in "plane 14" to "plane 3",
				 * and added tens of thousands more characters in planes 4, 5, 6, and 7
				 * We only support the older version of CNS-11643
				 * This is the same as iconv from glibc 2.2 */
				if (plane == 0 && s < cns11643_1_ucs_table_size) {
					w = cns11643_1_ucs_table[s];
				} else if (plane == 1 && s < cns11643_2_ucs_table_size) {
					w = cns11643_2_ucs_table[s];
				} else if (plane == 13 && s < cns11643_14_ucs_table_size) {
					w = cns11643_14_ucs_table[s];
				}
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

int mbfl_filt_conv_wchar_euctw(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_cns11643_table_min && c < ucs_a1_cns11643_table_max) {
		s = ucs_a1_cns11643_table[c - ucs_a1_cns11643_table_min];
	} else if (c >= ucs_a2_cns11643_table_min && c < ucs_a2_cns11643_table_max) {
		s = ucs_a2_cns11643_table[c - ucs_a2_cns11643_table_min];
	} else if (c >= ucs_a3_cns11643_table_min && c < ucs_a3_cns11643_table_max) {
		s = ucs_a3_cns11643_table[c - ucs_a3_cns11643_table_min];
	} else if (c >= ucs_i_cns11643_table_min && c < ucs_i_cns11643_table_max) {
		s = ucs_i_cns11643_table[c - ucs_i_cns11643_table_min];
	} else if (c >= ucs_r_cns11643_table_min && c < ucs_r_cns11643_table_max) {
		s = ucs_r_cns11643_table[c - ucs_r_cns11643_table_min];
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		int plane = (s & 0x1F0000) >> 16;
		if (plane <= 1) {
			if (s < 0x80) { /* latin */
				CK((*filter->output_function)(s, filter->data));
			} else {
				s = (s & 0xFFFF) | 0x8080;
				CK((*filter->output_function)((s >> 8) & 0xFF, filter->data));
				CK((*filter->output_function)(s & 0xFF, filter->data));
			}
		} else {
			s = (0x8EA00000 + (plane << 16)) | ((s & 0xFFFF) | 0x8080);
			CK((*filter->output_function)(0x8e , filter->data));
			CK((*filter->output_function)((s >> 16) & 0xFF, filter->data));
			CK((*filter->output_function)((s >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s & 0xFF, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return 0;
}

static int mbfl_filt_conv_euctw_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* 2-byte or 4-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_euctw_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (((c >= 0xA1 && c <= 0xA6) || (c >= 0xC2 && c <= 0xFD)) && c != 0xC3 && p < e) {
			unsigned char c2 = *p++;

			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int w = (c - 0xA1)*94 + (c2 - 0xA1);
				if (w < cns11643_1_ucs_table_size) {
					w = cns11643_1_ucs_table[w];
				} else {
					w = 0;
				}
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			unsigned char c2 = *p++;

			if ((c2 == 0xA1 || c2 == 0xA2 || c2 == 0xAE) && p < e) {
				unsigned int plane = c2 - 0xA1; /* This is actually the CNS-11643 plane minus one */
				unsigned char c3 = *p++;

				if (c3 >= 0xA1 && ((plane == 0 && ((c3 >= 0xA1 && c3 <= 0xA6) || (c3 >= 0xC2 && c3 <= 0xFD)) && c3 != 0xC3) || (plane == 1 && c3 <= 0xF2) || (plane == 13 && c3 <= 0xE7)) && p < e) {
					unsigned char c4 = *p++;

					if (c2 <= 0xAE && c4 > 0xA0 && c4 < 0xFF) {
						unsigned int s = (c3 - 0xA1)*94 + c4 - 0xA1, w = 0;

						/* A later version of CNS-11643 moved all the characters in "plane 14" to "plane 3",
						 * and added tens of thousands more characters in planes 4, 5, 6, and 7
						 * We only support the older version of CNS-11643
						 * This is the same as iconv from glibc 2.2 */
						if (plane == 0 && s < cns11643_1_ucs_table_size) {
							w = cns11643_1_ucs_table[s];
						} else if (plane == 1 && s < cns11643_2_ucs_table_size) {
							w = cns11643_2_ucs_table[s];
						} else if (plane == 13 && s < cns11643_14_ucs_table_size) {
							w = cns11643_14_ucs_table[s];
						}

						if (!w)
							w = MBFL_BAD_INPUT;
						*out++ = w;
						continue;
					}
				}
			}

			*out++ = MBFL_BAD_INPUT;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_euctw(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cns11643_table_min && w < ucs_a1_cns11643_table_max) {
			s = ucs_a1_cns11643_table[w - ucs_a1_cns11643_table_min];
		} else if (w >= ucs_a2_cns11643_table_min && w < ucs_a2_cns11643_table_max) {
			s = ucs_a2_cns11643_table[w - ucs_a2_cns11643_table_min];
		} else if (w >= ucs_a3_cns11643_table_min && w < ucs_a3_cns11643_table_max) {
			s = ucs_a3_cns11643_table[w - ucs_a3_cns11643_table_min];
		} else if (w >= ucs_i_cns11643_table_min && w < ucs_i_cns11643_table_max) {
			s = ucs_i_cns11643_table[w - ucs_i_cns11643_table_min];
		} else if (w >= ucs_r_cns11643_table_min && w < ucs_r_cns11643_table_max) {
			s = ucs_r_cns11643_table[w - ucs_r_cns11643_table_min];
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_euctw);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			}
		} else {
			unsigned int plane = s >> 16;
			if (plane <= 1) {
				if (s < 0x80) {
					out = mb_convert_buf_add(out, s);
				} else {
					out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
				}
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add4(out, 0x8E, 0xA0 + plane, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
			}
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
