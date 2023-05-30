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
 * the source code included in this files was separated from mbfilter_ja.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

/* CP932 is Microsoft's version of Shift-JIS.
 *
 * What we call "SJIS-win" is a variant of CP932 which maps U+00A5
 * and U+203E the same way as eucJP-win; namely, instead of mapping
 * U+00A5 (YEN SIGN) to 0x5C and U+203E (OVERLINE) to 0x7E,
 * these codepoints are mapped to appropriate JIS X 0208 characters.
 *
 * When converting from Shift-JIS to Unicode, there is no difference
 * between CP932 and "SJIS-win".
 *
 * Additional facts:
 *
 * • In the libmbfl library which formed the base for mbstring, "CP932" and
 *   "SJIS-win" were originally aliases. The differing mappings were added in
 *   December 2002. The libmbfl author later stated that this was done so that
 *   "CP932" would comply with a certain specification, while "SJIS-win" would
 *   maintain the existing mappings. He does not remember which specification
 *   it was.
 * • The WHATWG specification for "Shift_JIS" (followed by web browsers)
 *   agrees with our mappings for "CP932".
 * • Microsoft Windows' "best-fit" mappings for CP932 (via the
 *   WideCharToMultiByte API) convert U+00A5 to 0x5C, which also agrees with
 *   our mappings for "CP932".
 * • glibc's iconv converts U+203E to CP932 0x7E, which again agrees with
 *   our mappings for "CP932".
 * • When converting Shift-JIS to CP932, the conversion goes through Unicode.
 *   Shift-JIS 0x7E converts to U+203E, so mapping U+203E to 0x7E means that
 *   0x7E will go to 0x7E when converting Shift-JIS to CP932.
 */

#include "mbfilter.h"
#include "mbfilter_cp932.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

static int mbfl_filt_conv_cp932_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_cp932_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_cp932(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static void mb_wchar_to_sjiswin(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const unsigned char mblen_table_sjis[] = { /* 0x81-0x9f,0xE0-0xFF */
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
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static const char *mbfl_encoding_cp932_aliases[] = {"MS932", "Windows-31J", "MS_Kanji", NULL};
static const char *mbfl_encoding_sjiswin_aliases[] = {"SJIS-ms", "SJIS-open", NULL};

const mbfl_encoding mbfl_encoding_cp932 = {
	mbfl_no_encoding_cp932,
	"CP932",
	"Shift_JIS",
	mbfl_encoding_cp932_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp932_wchar,
	&vtbl_wchar_cp932,
	mb_cp932_to_wchar,
	mb_wchar_to_cp932,
	NULL
};

const struct mbfl_convert_vtbl vtbl_cp932_wchar = {
	mbfl_no_encoding_cp932,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp932_wchar,
	mbfl_filt_conv_cp932_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp932 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp932,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp932,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_sjiswin = {
	mbfl_no_encoding_sjiswin,
	"SJIS-win",
	"Shift_JIS",
	mbfl_encoding_sjiswin_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjiswin_wchar,
	&vtbl_wchar_sjiswin,
	mb_cp932_to_wchar,
	mb_wchar_to_sjiswin,
	NULL
};

const struct mbfl_convert_vtbl vtbl_sjiswin_wchar = {
	mbfl_no_encoding_sjiswin,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp932_wchar,
	mbfl_filt_conv_cp932_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_sjiswin = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjiswin,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjiswin,
	mbfl_filt_conv_common_flush,
	NULL,
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

int mbfl_filt_conv_cp932_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1:		/* kanji second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = (s1 - 0x21)*94 + s2 - 0x21;
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
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {		/* vendor ext3 (115ku - 119ku) */
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= (94*94) && s < (114*94)) {		/* user (95ku - 114ku) */
					w = s - (94*94) + 0xe000;
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

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp932_wchar_flush(mbfl_convert_filter *filter)
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

int mbfl_filt_conv_wchar_cp932(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1, s2;

	s1 = 0;
	s2 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) {
		s1 = 0x7E;
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 20*94)) {	/* user  (95ku - 114ku) */
		s1 = c - 0xe000;
		c1 = s1/94 + 0x7f;
		c2 = s1%94 + 0x21;
		s1 = (c1 << 8) | c2;
		s2 = 1;
	}
	if (s1 <= 0) {
		if (c == 0xa5) { /* YEN SIGN */
			s1 = 0x5C;
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
		}
	}
	if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
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
		if (s1 <= 0) {
			c1 = 0;
			c2 = cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
				if (c == cp932ext3_ucs_table[c1]) {
					s1 = ((c1/94 + 0x93) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}
	if (s1 >= 0) {
		if (s1 < 0x100) { /* latin or kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* kanji */
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_wchar_sjiswin(int c, mbfl_convert_filter *filter)
{
	if (c == 0xA5) {
		CK((*filter->output_function)(0x81, filter->data));
		CK((*filter->output_function)(0x8F, filter->data));
	} else if (c == 0x203E) {
		CK((*filter->output_function)(0x81, filter->data));
		CK((*filter->output_function)(0x50, filter->data));
	} else {
		return mbfl_filt_conv_wchar_cp932(c, filter);
	}
	return 0;
}

static size_t mb_cp932_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c > 0xA0 && c < 0xE0) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c < 0xFD && c != 0xA0 && p < e) {
			unsigned char c2 = *p++;

			if (c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F) {
				unsigned int s1, s2, w = 0;
				SJIS_DECODE(c, c2, s1, s2);
				unsigned int s = (s1 - 0x21)*94 + s2 - 0x21;

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
					} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
						w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
					} else if (s >= (94*94) && s < (114*94)) {
						w = s - (94*94) + 0xE000;
					}
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

static void mb_wchar_to_cp932(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s1 = 0, s2 = 0, c1, c2;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s1 = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w == 0x203E) {
			s1 = 0x7E;
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s1 = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s1 = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s1 = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			s1 = w - 0xE000;
			c1 = s1/94 + 0x7F;
			c2 = s1%94 + 0x21;
			s1 = (c1 << 8) | c2;
			s2 = 1;
		}

		if (w == 0xA5) { /* YEN SIGN */
			s1 = 0x5C;
		} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (w == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		} else if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		}

		if (!s1 || (s1 >= 0x8080 && !s2)) { /* not found or X 0212 */
			for (unsigned int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (cp932ext1_ucs_table[i] == w) {
					s1 = ((i/94 + 0x2D) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			for (unsigned int i = 0; i < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; i++) {
				if (cp932ext3_ucs_table[i] == w) {
					s1 = ((i/94 + 0x93) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp932);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

emit_output:
		if (s1 < 0x100) {
			out = mb_convert_buf_add(out, s1);
		} else {
			c1 = (s1 >> 8) & 0xFF;
			c2 = s1 & 0xFF;
			SJIS_ENCODE(c1, c2, s1, s2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_sjiswin(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s1 = 0, s2 = 0, c1, c2;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s1 = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s1 = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s1 = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s1 = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			s1 = w - 0xE000;
			c1 = s1/94 + 0x7F;
			c2 = s1%94 + 0x21;
			s1 = (c1 << 8) | c2;
			s2 = 1;
		}

		if (w == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (w == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		} else if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		}

		if (!s1 || (s1 >= 0x8080 && !s2)) { /* not found or X 0212 */
			for (unsigned int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (cp932ext1_ucs_table[i] == w) {
					s1 = ((i/94 + 0x2D) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			for (unsigned int i = 0; i < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; i++) {
				if (cp932ext3_ucs_table[i] == w) {
					s1 = ((i/94 + 0x93) << 8) + (i%94 + 0x21);
					goto emit_output;
				}
			}

			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp932);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

emit_output:
		if (s1 < 0x100) {
			out = mb_convert_buf_add(out, s1);
		} else {
			c1 = (s1 >> 8) & 0xFF;
			c2 = s1 & 0xFF;
			SJIS_ENCODE(c1, c2, s1, s2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
