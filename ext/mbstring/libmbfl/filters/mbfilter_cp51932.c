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
#include "mbfilter_cp51932.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"

static int mbfl_filt_conv_cp51932_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_cp51932_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_cp51932(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const unsigned char mblen_table_eucjp[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_cp51932_aliases[] = {"cp51932", NULL};

const mbfl_encoding mbfl_encoding_cp51932 = {
	mbfl_no_encoding_cp51932,
	"CP51932",
	"CP51932",
	mbfl_encoding_cp51932_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_cp51932_wchar,
	&vtbl_wchar_cp51932,
	mb_cp51932_to_wchar,
	mb_wchar_to_cp51932,
	NULL
};

const struct mbfl_convert_vtbl vtbl_cp51932_wchar = {
	mbfl_no_encoding_cp51932,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp51932_wchar,
	mbfl_filt_conv_cp51932_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp51932 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp51932,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp51932,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * cp51932 => wchar
 */
int
mbfl_filt_conv_cp51932_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c >= 0xA1 && c <= 0xFE) { /* CP932, first byte */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) { /* kana first char */
			filter->status = 2;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			w = 0;
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s <= 137) {
				if (s == 31) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xff5e;			/* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225;			/* PARALLEL TO */
				} else if (s == 60) {
					w = 0xff0d;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xffe0;			/* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xffe1;			/* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xffe2;			/* FULLWIDTH NOT SIGN */
				}
			}
			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				}
			}
			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 2:	/* got 0x8e, X0201 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp51932_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

/*
 * wchar => cp51932
 */
int
mbfl_filt_conv_wchar_cp51932(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1;

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
	if (s1 >= 0x8080) s1 = -1; /* we don't support JIS X0213 */
	if (s1 <= 0) {
		if (c == 0xa5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		} else {
			s1 = -1;
			c1 = 0;
			c2 = cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext1 (13ku) */
				if (c == cp932ext1_ucs_table[c1]) {
					s1 = ((c1/94 + 0x2d) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
			if (s1 < 0) {
				c1 = 0;
				c2 = cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
				while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
					if (c == cp932ext2_ucs_table[c1]) {
					  s1 = ((c1/94 + 0x79) << 8) +(c1%94 + 0x21);
					  break;
					}
					c1++;
				}
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) {	/* latin */
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data));
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data));
			CK((*filter->output_function)((s1 & 0xff) | 0x80, filter->data));
		} else {
		    CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_cp51932_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE && p < e) {
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xFE) {
				unsigned int s = (c - 0xA1)*94 + c2 - 0xA1, w = 0;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (w == 0) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8E && p < e) {
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xDF) {
				*out++ = 0xFEC0 + c2;
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

static void mb_wchar_to_cp51932(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		} else if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s >= 0x8080) s = 0; /* We don't support JIS X0213 */

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
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
			} else {
				for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
					if (cp932ext1_ucs_table[i] == w) {
						s = ((i/94 + 0x2D) << 8) + (i%94) + 0x21;
						goto found_it;
					}
				}

				for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
					if (cp932ext2_ucs_table[i] == w) {
						s = ((i/94 + 0x79) << 8) + (i%94) + 0x21;
						goto found_it;
					}
				}
			}
found_it: ;
		}

		if (!s || s >= 0x8080) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp51932);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x100) {
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else {
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) | 0x80, (s & 0xFF) | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
