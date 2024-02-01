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
#include "mbfilter_sjis.h"

#define UNICODE_TABLE_CP932_DEF
#define UNICODE_TABLE_JIS_DEF

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

static int mbfl_filt_conv_sjis_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_sjis_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_sjis(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

const unsigned char mblen_table_sjis[] = { /* 0x81-0x9F,0xE0-0xEF */
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
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static const char *mbfl_encoding_sjis_aliases[] = {"x-sjis", "SHIFT-JIS", NULL};

const mbfl_encoding mbfl_encoding_sjis = {
	mbfl_no_encoding_sjis,
	"SJIS",
	"Shift_JIS",
	mbfl_encoding_sjis_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_wchar,
	&vtbl_wchar_sjis,
	mb_sjis_to_wchar,
	mb_wchar_to_sjis,
	NULL
};

const struct mbfl_convert_vtbl vtbl_sjis_wchar = {
	mbfl_no_encoding_sjis,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis,
	mbfl_filt_conv_common_flush,
	NULL
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define SJIS_ENCODE(c1,c2,s1,s2)	\
		do {						\
			s1 = c1;				\
			s1--;					\
			s1 >>= 1;				\
			if ((c1) < 0x5f) {		\
				s1 += 0x71;			\
			} else {				\
				s1 += 0xb1;			\
			}						\
			s2 = c2;				\
			if ((c1) & 1) {			\
				if ((c2) < 0x60) {	\
					s2--;			\
				}					\
				s2 += 0x20;			\
			} else {				\
				s2 += 0x7e;			\
			}						\
		} while (0)

#define SJIS_DECODE(c1,c2,s1,s2)	\
		do {						\
			s1 = c1;				\
			if (s1 < 0xa0) {		\
				s1 -= 0x81;			\
			} else {				\
				s1 -= 0xc1;			\
			}						\
			s1 <<= 1;				\
			s1 += 0x21;				\
			s2 = c2;				\
			if (s2 < 0x9f) {		\
				if (s2 < 0x7f) {	\
					s2++;			\
				}					\
				s2 -= 0x20;			\
			} else {				\
				s1++;				\
				s2 -= 0x7e;			\
			}						\
		} while (0)

int mbfl_filt_conv_sjis_wchar(int c, mbfl_convert_filter *filter)
{
	int s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else if (c > 0x80 && c < 0xF0 && c != 0xA0) { /* Kanji, first byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* Kanji, second byte */
		filter->status = 0;
		int c1 = filter->cache;
		if (c >= 0x40 && c <= 0xFC && c != 0x7F) {
			SJIS_DECODE(c1, c, s1, s2);
			w = (s1 - 0x21)*94 + s2 - 0x21;
			if (w >= 0 && w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
				if (!w)
					w = MBFL_BAD_INPUT;
			} else {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
	}

	return 0;
}

static int mbfl_filt_conv_sjis_wchar_flush(mbfl_convert_filter *filter)
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

int mbfl_filt_conv_wchar_sjis(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1, s2;

	s1 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xAF || c == 0x203E) { /* U+00AF is MACRON, U+203E is OVERLINE */
			s1 = 0x2131; /* FULLWIDTH MACRON */
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		} else if (c == 0) {
			s1 = 0;
		} else {
			s1 = -1;
		}
	} else if (s1 >= 0x8080) { /* JIS X 0212; not supported */
		s1 = -1;
	}

	if (s1 >= 0) {
		if (s1 < 0x100) { /* Latin/Kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* Kanji */
			c1 = (s1 >> 8) & 0xFF;
			c2 = s1 & 0xFF;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_sjis_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) { /* Kana */
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c <= 0xEF && c != 0xA0 && p < e) {
			unsigned char c2 = *p++;
			if (c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F) {
				unsigned int s1, s2;
				SJIS_DECODE(c, c2, s1, s2);
				uint32_t w = (s1 - 0x21)*94 + s2 - 0x21;
				if (w < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[w];
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
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_sjis(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xAF || w == 0x203E) {
				s = 0x2131; /* FULLWIDTH MACRON */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
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
			} else if (w != 0) {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		} else if (s >= 0x8080) { /* JIS X 0212; not supported */
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

		if (s <= 0xFF) {
			/* Latin/Kana */
			out = mb_convert_buf_add(out, s);
		} else {
			/* Kanji */
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s2;
			SJIS_ENCODE(c1, c2, s, s2);
			out = mb_convert_buf_add2(out, s, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
