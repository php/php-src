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
 * The source code included in this files was separated from mbfilter_ja.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_euc_jp.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

static int mbfl_filt_conv_eucjp_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_eucjp_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_eucjp(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

const unsigned char mblen_table_eucjp[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_euc_jp_aliases[] = {"EUC", "EUC_JP", "eucJP", "x-euc-jp", NULL};

const mbfl_encoding mbfl_encoding_euc_jp = {
	mbfl_no_encoding_euc_jp,
	"EUC-JP",
	"EUC-JP",
	mbfl_encoding_euc_jp_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_eucjp_wchar,
	&vtbl_wchar_eucjp,
	mb_eucjp_to_wchar,
	mb_wchar_to_eucjp,
	NULL
};

const struct mbfl_convert_vtbl vtbl_eucjp_wchar = {
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_eucjp_wchar,
	mbfl_filt_conv_eucjp_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_eucjp = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_jp,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_eucjp,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * EUC-JP => wchar
 */
int
mbfl_filt_conv_eucjp_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w = 0;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xff) {	/* X 0208 first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* kana first char */
			filter->status = 2;
		} else if (c == 0x8f) {	/* X 0212 first char */
			filter->status = 3;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[s];
				if (!w)
					w = MBFL_BAD_INPUT;
			} else {
				w = MBFL_BAD_INPUT;
			}

			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2:	/* got 0x8e */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* got 0x8f, JIS X 0212 first byte */
		filter->status++;
		filter->cache = c;
		break;

	case 4: /* got 0x8f, JIS X 0212 second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xA0 && c < 0xFF && c1 > 0xA0 && c1 < 0xFF) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0212_ucs_table_size) {
				w = jisx0212_ucs_table[s];
				if (!w)
					w = MBFL_BAD_INPUT;
			} else {
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

static int mbfl_filt_conv_eucjp_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

/*
 * wchar => EUC-JP
 */
int
mbfl_filt_conv_wchar_eucjp(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c == 0xAF) { /* U+00AF is MACRON */
		s = 0xA2B4; /* Use JIS X 0212 overline */
	} else if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s <= 0) {
		if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		} else if (c == 0) {
			s = 0;
		} else {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s < 0x80) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data));
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s & 0xff) | 0x80, filter->data));
		} else {	/* X 0212 */
			CK((*filter->output_function)(0x8f, filter->data));
			CK((*filter->output_function)(((s >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s & 0xff) | 0x80, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_eucjp_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE && p < e) {
			/* JISX 0208 */
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int s = (c - 0xA1)*94 + c2 - 0xA1;
				if (s < jisx0208_ucs_table_size) {
					uint32_t w = jisx0208_ucs_table[s];
					if (!w)
						w = MBFL_BAD_INPUT;
					*out++ = w;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			/* Kana */
			unsigned char c2 = *p++;
			*out++ = (c2 >= 0xA1 && c2 <= 0xDF) ? 0xFEC0 + c2 : MBFL_BAD_INPUT;
		} else if (c == 0x8F) {
			/* JISX 0212 */
			if ((e - p) >= 2) {
				unsigned char c2 = *p++;
				unsigned char c3 = *p++;
				if (c3 >= 0xA1 && c3 <= 0xFE && c2 >= 0xA1 && c2 <= 0xFE) {
					unsigned int s = (c2 - 0xA1)*94 + c3 - 0xA1;
					if (s < jisx0212_ucs_table_size) {
						uint32_t w = jisx0212_ucs_table[s];
						if (!w)
							w = MBFL_BAD_INPUT;
						*out++ = w;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				p = e; /* Jump to end of string */
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_eucjp(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0xAF) { /* U+00AF is MACRON */
			s = 0xA2B4; /* Use JIS X 0212 overline */
		} else if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w == 0) {
				out = mb_convert_buf_add(out, 0);
				continue;
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_eucjp);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		}

		if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x100) {
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else if (s < 0x8080)  {
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 3);
			out = mb_convert_buf_add3(out, 0x8F, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
