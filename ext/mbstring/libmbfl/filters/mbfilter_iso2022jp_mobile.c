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
 * The source code included in this files was separated from mbfilter_iso2022_jp_ms.c
 * by Rui Hirokawa <hirokawa@php.net> on 25 July 2011.
 *
 */

#include "mbfilter.h"
#include "mbfilter_iso2022jp_mobile.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"
#include "emoji2uni.h"

static size_t mb_iso2022jp_kddi_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_iso2022jp_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static int mbfl_filt_conv_2022jp_mobile_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_2022jp_mobile(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_2022jp_mobile_wchar_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_2022jp_mobile_flush(mbfl_convert_filter *filter);

extern int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n);

/* Regional Indicator Unicode codepoints are from 0x1F1E6-0x1F1FF
 * These correspond to the letters A-Z
 * To display the flag emoji for a country, two unicode codepoints are combined,
 * which correspond to the two-letter code for that country
 * This macro converts uppercase ASCII values to Regional Indicator codepoints */
#define NFLAGS(c) (0x1F1A5+((unsigned int)(c)))

static const char nflags_s[10][2] = {
	"CN","DE","ES","FR","GB","IT","JP","KR","RU","US"
};
static const int nflags_code_kddi[10] = {
	0x2549, 0x2546, 0x24C0, 0x2545, 0x2548, 0x2547, 0x2750, 0x254A, 0x24C1, 0x27F7
};

static const char *mbfl_encoding_2022jp_kddi_aliases[] = {"ISO-2022-JP-KDDI", NULL};

const mbfl_encoding mbfl_encoding_2022jp_kddi = {
	mbfl_no_encoding_2022jp_kddi,
	"ISO-2022-JP-MOBILE#KDDI",
	"ISO-2022-JP",
	mbfl_encoding_2022jp_kddi_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_kddi_wchar,
	&vtbl_wchar_2022jp_kddi,
	mb_iso2022jp_kddi_to_wchar,
	mb_wchar_to_iso2022jp_kddi,
	NULL
};

const struct mbfl_convert_vtbl vtbl_2022jp_kddi_wchar = {
	mbfl_no_encoding_2022jp_kddi,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_2022jp_mobile_wchar,
	mbfl_filt_conv_2022jp_mobile_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jp_kddi = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_kddi,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022jp_mobile,
	mbfl_filt_conv_wchar_2022jp_mobile_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define SJIS_ENCODE(c1,c2,s1,s2) \
		do { \
			s1 = ((c1 - 1) >> 1) + ((c1) < 0x5F ? 0x71 : 0xB1); \
			s2 = c2; \
			if ((c1) & 1) { \
				if ((c2) < 0x60) { \
					s2--; \
				} \
				s2 += 0x20; \
			} else { \
				s2 += 0x7e; \
			} \
		} while (0)

#define SJIS_DECODE(c1,c2,s1,s2) \
		do { \
			if (c1 < 0xa0) { \
				s1 = ((c1 - 0x81) << 1) + 0x21; \
			} else { \
				s1 = ((c1 - 0xc1) << 1) + 0x21; \
			} \
			s2 = c2; \
			if (c2 < 0x9f) { \
				if (c2 < 0x7f) { \
					s2++; \
				} \
				s2 -= 0x20; \
			} else { \
				s1++; \
				s2 -= 0x7e; \
			} \
		} while (0)

/* (ku*94)+ten value -> Shift-JIS byte sequence */
#define CODE2JIS(c1,c2,s1,s2) \
	c1 = (s1)/94+0x21; \
	c2 = (s1)-94*((c1)-0x21)+0x21; \
	s1 = ((c1) << 8) | (c2); \
	s2 = 1

#define ASCII          0
#define JISX0201_KANA  0x20
#define JISX0208_KANJI 0x80

#define EMIT_KEYPAD_EMOJI(c) do { *snd = (c); return 0x20E3; } while(0)
#define EMIT_FLAG_EMOJI(country) do { *snd = NFLAGS((country)[0]); return NFLAGS((country)[1]); } while(0)

static const char nflags_kddi[6][2] = {"FR", "DE", "IT", "GB", "CN", "KR"};

static inline int convert_emoji_cp(int cp)
{
	if (cp > 0xF000)
		return cp + 0x10000;
	if (cp > 0xE000)
		return cp + 0xF0000;
	return cp;
}

static int mbfilter_sjis_emoji_kddi2unicode(int s, int *snd)
{
	if (s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi1_max) {
		if (s == 0x24C0) { /* Spain */
			EMIT_FLAG_EMOJI("ES");
		} else if (s == 0x24C1) { /* Russia */
			EMIT_FLAG_EMOJI("RU");
		} else if (s >= 0x2545 && s <= 0x254A) {
			EMIT_FLAG_EMOJI(nflags_kddi[s - 0x2545]);
		} else if (s == 0x25BC) {
			EMIT_KEYPAD_EMOJI('#');
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_kddi1[s - mb_tbl_code2uni_kddi1_min]);
		}
	} else if (s >= mb_tbl_code2uni_kddi2_min && s <= mb_tbl_code2uni_kddi2_max) {
		if (s == 0x2750) { /* Japan */
			EMIT_FLAG_EMOJI("JP");
		} else if (s >= 0x27A6 && s <= 0x27AE) {
			EMIT_KEYPAD_EMOJI(s - 0x27A6 + '1');
		} else if (s == 0x27F7) { /* United States */
			EMIT_FLAG_EMOJI("US");
		} else if (s == 0x2830) {
			EMIT_KEYPAD_EMOJI('0');
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_kddi2[s - mb_tbl_code2uni_kddi2_min]);
		}
	}
	return 0;
}

static int mbfl_filt_conv_2022jp_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w, snd = 0;

	switch (filter->status & 0xF) {
	case 0:
		if (c == 0x1B) {
			filter->status += 2;
		} else if (filter->status == JISX0201_KANA && c > 0x20 && c < 0x60) {
			CK((*filter->output_function)(0xFF40 + c, filter->data));
		} else if (filter->status == JISX0208_KANJI && c > 0x20 && c < 0x80) {
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* JISX 0208, second byte */
	case 1:
		w = 0;
		filter->status &= ~0xF;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7F) {
			s = ((c1 - 0x21) * 94) + c - 0x21;

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

			if (s >= (84 * 94) && s < (91 * 94)) {
				s += 22 * 94;
				w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
				if (w > 0 && snd > 0) {
					(*filter->output_function)(snd, filter->data);
				}
			}

			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
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

	/* ESC */
	case 2:
		if (c == '$') {
			filter->status++;
		} else if (c == '(') {
			filter->status += 3;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ */
	case 3:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else if (c == '(') {
			filter->status++;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 'B' || c == 'J') {
			filter->status = 0; /* ASCII mode */
		} else if (c == 'I') {
			filter->status = JISX0201_KANA;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
	}

	return 0;
}

static int mbfl_filt_conv_2022jp_mobile_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfilter_unicode2sjis_emoji_kddi(int c, int *s1, mbfl_convert_filter *filter)
{
	if ((filter->status & 0xF) == 1) {
		int c1 = filter->cache;
		filter->cache = 0;
		filter->status &= ~0xFF;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x25BC;
			} else if (c1 == '0') {
				*s1 = 0x2830;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x27A6 + (c1 - '1');
			}
			return 1;
		} else {
			if (filter->status & 0xFF00) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			CK((*filter->output_function)(c1, filter->data));
			filter->status = 0;
		}
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status |= 1;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x27DC;
		return 1;
	} else if (c == 0xAE) { /* Registered sign */
		*s1 = 0x27DD;
		return 1;
	} else if (c >= mb_tbl_uni_kddi2code2_min && c <= mb_tbl_uni_kddi2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code3_min && c <= mb_tbl_uni_kddi2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code5_min && c <= mb_tbl_uni_kddi2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code5_val[i];
			return 1;
		}
	}
	return 0;
}

static int mbfl_filt_conv_wchar_2022jp_mobile(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0, s2 = 0;

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
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		}
	}

	if (mbfilter_unicode2sjis_emoji_kddi(c, &s1, filter) > 0) {
		/* A KDDI emoji was detected and stored in s1 */
		CODE2JIS(c1,c2,s1,s2);
		s1 -= 0x1600;
	} else if ((filter->status & 0xFF) == 1 && filter->cache) {
		/* We are just processing one of KDDI's special emoji for a phone keypad button */
		return 0;
	}

	if ((s1 <= 0) || (s1 >= 0xa1a1 && s2 == 0)) { /* not found or X 0212 */
		s1 = -1;
		for (c1 = 0; c1 < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; c1++) {
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = (((c1 / 94) + 0x2D) << 8) + (c1 % 94) + 0x21;
				break;
			}
		}

		if (c == 0) {
			s1 = 0;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* ASCII */
			if (filter->status & 0xFF00) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			CK((*filter->output_function)(s1, filter->data));
			filter->status = 0;
		} else if (s1 > 0xA0 && s1 < 0xE0) { /* Kana */
			if ((filter->status & 0xFF00) != 0x100) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('I', filter->data));
			}
			filter->status = 0x100;
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		} else if (s1 < 0x7E7F) { /* JIS X 0208 */
			if ((filter->status & 0xFF00) != 0x200) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s1 >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_wchar_2022jp_mobile_flush(mbfl_convert_filter *filter)
{
	/* Go back to ASCII mode (so strings can be safely concatenated) */
	if (filter->status & 0xFF00) {
		(*filter->output_function)(0x1B, filter->data); /* ESC */
		(*filter->output_function)('(', filter->data);
		(*filter->output_function)('B', filter->data);
	}

	int c1 = filter->cache;
	if ((filter->status & 0xFF) == 1 && (c1 == '#' || (c1 >= '0' && c1 <= '9'))) {
		(*filter->output_function)(c1, filter->data);
	}
	filter->status = filter->cache = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_iso2022jp_kddi_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			if ((e - p) < 2) {
				p = e;
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			unsigned char c3 = *p++;

			if (c2 == '$') {
				if (c3 == '@' || c3 == 'B') {
					*state = JISX0208_KANJI;
				} else if (c3 == '(') {
					if (p == e) {
						*out++ = MBFL_BAD_INPUT;
						break;
					}
					unsigned char c4 = *p++;

					if (c4 == '@' || c4 == 'B') {
						*state = JISX0208_KANJI;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (c2 == '(') {
				if (c3 == 'B' || c3 == 'J') {
					*state = ASCII;
				} else if (c3 == 'I') {
					*state = JISX0201_KANA;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else {
				p--;
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (*state == JISX0201_KANA && c >= 0x21 && c <= 0x5F) {
			*out++ = 0xFF40 + c;
		} else if (*state == JISX0208_KANJI && c >= 0x21 && c <= 0x7F) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 >= 0x21 && c2 <= 0x7E) {
				unsigned int s = ((c - 0x21) * 94) + c2 - 0x21;
				uint32_t w = 0;

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

				if (s >= (84 * 94) && s < (91 * 94)) {
					int snd = 0;
					s += 22 * 94;
					w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
					if (w && snd) {
						*out++ = snd;
					}
				}

				if (!w) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					}
				}

				*out++ = w ? w : MBFL_BAD_INPUT;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jp_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

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

		if (!s) {
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
			}
		}

		if ((w == '#' || (w >= '0' && w <= '9')) && len) {
			uint32_t w2 = *in++; len--;

			if (w2 == 0x20E3) {
				unsigned int s1 = 0;
				if (w == '#') {
					s1 = 0x25BC;
				} else if (w == '0') {
					s1 = 0x2830;
				} else { /* Previous character was '1'-'9' */
					s1 = 0x27A6 + (w - '1');
				}
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			} else {
				in--; len++;
			}
		} else if (w >= NFLAGS('C') && w <= NFLAGS('U') && len) { /* C for CN, U for US */
			uint32_t w2 = *in++; len--;

			if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
				for (int i = 0; i < 10; i++) {
					if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
						unsigned int s1 = nflags_code_kddi[i];
						s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
						goto found_flag_emoji;
					}
				}
			}

			in--; len++;
found_flag_emoji: ;
		}

		if (w == 0xA9) { /* Copyright sign */
			unsigned int s1 = 0x27DC;
			s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
		} else if (w == 0xAE) { /* Registered sign */
			unsigned int s1 = 0x27DD;
			s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
		} else if (w >= mb_tbl_uni_kddi2code2_min && w <= mb_tbl_uni_kddi2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
			if (i >= 0) {
				unsigned int s1 = mb_tbl_uni_kddi2code2_value[i];
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			}
		} else if (w >= mb_tbl_uni_kddi2code3_min && w <= mb_tbl_uni_kddi2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
			if (i >= 0) {
				unsigned int s1 = mb_tbl_uni_kddi2code3_value[i];
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			}
		} else if (w >= mb_tbl_uni_kddi2code5_min && w <= mb_tbl_uni_kddi2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
			if (i >= 0) {
				unsigned int s1 = mb_tbl_uni_kddi2code5_val[i];
				s = (((s1 / 94) + 0x21) << 8) + ((s1 % 94) + 0x21) - 0x1600;
			}
		}

		if (!s || s >= 0xA1A1) {
			s = 0;
			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					break;
				}
			}
			if (w == 0)
				s = 0;
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA1 && s <= 0xDF) {
			if (buf->state != JISX0201_KANA) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX0201_KANA;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else if (s <= 0x7E7E) {
			if (buf->state != JISX0208_KANJI) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX0208_KANJI;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
