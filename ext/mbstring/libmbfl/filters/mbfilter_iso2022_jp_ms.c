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
#include "mbfilter_iso2022_jp_ms.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"

static const char *mbfl_encoding_2022jpms_aliases[] = {"ISO2022JPMS", NULL};

const mbfl_encoding mbfl_encoding_2022jpms = {
	mbfl_no_encoding_2022jpms,
	"ISO-2022-JP-MS",
	"ISO-2022-JP",
	mbfl_encoding_2022jpms_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jpms_wchar,
	&vtbl_wchar_2022jpms
};

const struct mbfl_convert_vtbl vtbl_2022jpms_wchar = {
	mbfl_no_encoding_2022jpms,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_2022jpms_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jpms = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jpms,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022jpms,
	mbfl_filt_conv_any_2022jpms_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define sjistoidx(c1, c2) \
        (((c1) > 0x9f) \
        ? (((c1) - 0xc1) * 188 + (c2) - (((c2) > 0x7e) ? 0x41 : 0x40)) \
        : (((c1) - 0x81) * 188 + (c2) - (((c2) > 0x7e) ? 0x41 : 0x40)))
#define idxtojis1(c) (((c) / 94) + 0x21)
#define idxtojis2(c) (((c) % 94) + 0x21)

#define JISX0201_KANA  0x20
#define JISX0208_KANJI 0x80
#define UDC            0xA0

int mbfl_filt_conv_2022jpms_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status & 0xF) {
	case 0:
		if (c == 0x1B) {
			filter->status += 2;
		} else if (filter->status == JISX0201_KANA && c > 0x20 && c < 0x60) {
			CK((*filter->output_function)(0xFF40 + c, filter->data));
		} else if ((filter->status == JISX0208_KANJI || filter->status == UDC) && c > 0x20 && c < 0x80) {
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

	/* Kanji, second byte */
	case 1:
		w = 0;
		filter->status &= ~0xF;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7F) {
			s = ((c1 - 0x21) * 94) + c - 0x21;
			if (filter->status == JISX0208_KANJI) {
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
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s >= 0 && s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}
				}

				if (w <= 0) {
					w = (c1 << 8) | c | MBFL_WCSPLANE_JIS0208;
				}
			} else {
				if (c1 > 0x20 && c1 < 0x35) {
					w = 0xE000 + ((c1 - 0x21) * 94) + c - 0x21;
				}
				if (w <= 0) {
					w = (((c1 - 0x21) + 0x7f) << 8) | c | MBFL_WCSPLANE_JIS0208;
				}
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
		} else if (c == '?') {
			filter->status = UDC;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(0x1B242800 | c | MBFL_WCSGROUP_THROUGH, filter->data));
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 'B' || c == 'J') {
			filter->status = 0;
		} else if (c == 'I') {
			filter->status = JISX0201_KANA;
		} else {
			filter->status &= ~0xF;
			CK((*filter->output_function)(0x1B2800 | c | MBFL_WCSGROUP_THROUGH, filter->data));
		}
	}

	return c;
}

static int cp932ext3_cp932ext2_jis(int c)
{
	int idx;

	idx = sjistoidx(0xfa, 0x40) + c;
	if (idx >= sjistoidx(0xfa, 0x5c))
		idx -=  sjistoidx(0xfa, 0x5c) - sjistoidx(0xed, 0x40);
	else if (idx >= sjistoidx(0xfa, 0x55))
		idx -=  sjistoidx(0xfa, 0x55) - sjistoidx(0xee, 0xfa);
	else if (idx >= sjistoidx(0xfa, 0x40))
		idx -=  sjistoidx(0xfa, 0x40) - sjistoidx(0xee, 0xef);
	return idxtojis1(idx) << 8 | idxtojis2(idx);
}

int mbfl_filt_conv_wchar_2022jpms(int c, mbfl_convert_filter *filter)
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
		/* Private User Area (95ku - 114ku) */
		s1 = c - 0xE000;
		c1 = (s1 / 94) + 0x7f;
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
			s1 = 0x224C;
		}
	}

	if ((s1 <= 0) || (s1 >= 0xa1a1 && s2 == 0)) { /* not found or X 0212 */
		s1 = -1;
		for (c1 = 0; c1 < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; c1++) {
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = (((c1 / 94) + 0x2D) << 8) + (c1 % 94) + 0x21;
				break;
			}
		}

		if (s1 <= 0) {
			for (c1 = 0; c1 < cp932ext3_ucs_table_max - cp932ext3_ucs_table_min; c1++) {
				if (c == cp932ext3_ucs_table[c1]) {
					s1 = cp932ext3_cp932ext2_jis(c1);
					break;
				}
			}
		}

		if (c == 0) {
			s1 = 0;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* latin */
			if (filter->status & 0xFF00) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			CK((*filter->output_function)(s1, filter->data));
			filter->status = 0;
		} else if (s1 > 0xA0 && s1 < 0xE0) { /* kana */
			if ((filter->status & 0xFF00) != 0x100) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('I', filter->data));
			}
			filter->status = 0x100;
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		} else if (s1 < 0x7E7F) { /* X 0208 */
			if ((filter->status & 0xFF00) != 0x200) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s1 >> 8) & 0xFF, filter->data));
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		} else if (s1 < 0x927F) { /* UDC */
			if ((filter->status & 0xFF00) != 0x800) {
				CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('?', filter->data));
			}
			filter->status = 0x800;
			CK((*filter->output_function)(((s1 >> 8) - 0x5E) & 0x7F, filter->data));
			CK((*filter->output_function)(s1 & 0x7F, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

int mbfl_filt_conv_any_2022jpms_flush(mbfl_convert_filter *filter)
{
	/* Go back to ASCII (so strings can be safely concatenated) */
	if ((filter->status & 0xFF00) != 0) {
		CK((*filter->output_function)(0x1B, filter->data)); /* ESC */
		CK((*filter->output_function)('(', filter->data));
		CK((*filter->output_function)('B', filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}
