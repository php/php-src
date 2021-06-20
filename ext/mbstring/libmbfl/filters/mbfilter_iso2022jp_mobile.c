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
#include "mbfilter_sjis_mobile.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"

static int mbfl_filt_conv_2022jp_mobile_wchar_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_2022jp_mobile_flush(mbfl_convert_filter *filter);

static const char *mbfl_encoding_2022jp_kddi_aliases[] = {"ISO-2022-JP-KDDI", NULL};

const mbfl_encoding mbfl_encoding_2022jp_kddi = {
	mbfl_no_encoding_2022jp_kddi,
	"ISO-2022-JP-MOBILE#KDDI",
	"ISO-2022-JP",
	mbfl_encoding_2022jp_kddi_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_kddi_wchar,
	&vtbl_wchar_2022jp_kddi
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

#define JISX0201_KANA  0x20
#define JISX0208_KANJI 0x80

int mbfl_filt_conv_2022jp_mobile_wchar(int c, mbfl_convert_filter *filter)
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
			CK((*filter->output_function)(c | MBFL_WCSGROUP_THROUGH, filter->data));
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
				w = (c1 << 8) | c | MBFL_WCSPLANE_JIS0208;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)((c1 << 8) | c | MBFL_WCSGROUP_THROUGH, filter->data));
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
			CK((*filter->output_function)(0x1B00 | c | MBFL_WCSGROUP_THROUGH, filter->data));
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
			CK((*filter->output_function)(0x1B2400 | c | MBFL_WCSGROUP_THROUGH, filter->data));
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == '@' || c == 'B') {
			filter->status = JISX0208_KANJI;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(0x1B242800 | c | MBFL_WCSGROUP_THROUGH, filter->data));
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
			CK((*filter->output_function)(0x1B2800 | c | MBFL_WCSGROUP_THROUGH, filter->data));
		}
	}

	return c;
}

static int mbfl_filt_conv_2022jp_mobile_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		mbfl_filt_conv_illegal_output(filter->cache, filter);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_2022jp_mobile(int c, mbfl_convert_filter *filter)
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
	} else if (c >= 0xE000 && c < (0xE000 + 20*94)) {
		/* Private Use Area (95ku - 114ku) */
		s1 = c - 0xE000;
		c1 = (s1 / 94) + 0x7F;
		c2 = (s1 % 94) + 0x21;
		s1 = (c1 << 8) | c2;
	}

	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0x203E) { /* OVER LINE */
			s1 = 0x2131; /* FULLWIDTH MACRON */
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0xFF5E) { /* FULLWIDTH TILDE */
			s1 = 0x2141;
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

	if (mbfilter_unicode2sjis_emoji_kddi(c, &s1, filter)) {
		CODE2JIS(c1,c2,s1,s2);
		s1 -= 0x1600;
	}

	if (filter->status == 1 && filter->cache) {
		/* We are just processing one of KDDI's special emoji for a phone keypad button */
		return c;
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

	return c;
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
	if (filter->status == 1 && (c1 == '#' || (c1 >= '0' && c1 <= '9'))) {
		(*filter->output_function)(c1, filter->data);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}
