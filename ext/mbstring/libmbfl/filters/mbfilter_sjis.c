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

const unsigned char mblen_table_sjis[] = { /* 0x80-0x9f,0xE0-0xFF */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static const char *mbfl_encoding_sjis_aliases[] = {"x-sjis", "SHIFT-JIS", NULL};

const mbfl_encoding mbfl_encoding_sjis = {
	mbfl_no_encoding_sjis,
	"SJIS",
	"Shift_JIS",
	mbfl_encoding_sjis_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_wchar,
	&vtbl_wchar_sjis
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
		if (c == 0x5C) {
			CK((*filter->output_function)(0xA5, filter->data));
		} else if (c == 0x7E) {
			CK((*filter->output_function)(0x203E, filter->data));
		} else if (c >= 0 && c < 0x80) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else if (c > 0x80 && c < 0xF0 && c != 0xA0) { /* Kanji, first byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(c | MBFL_WCSGROUP_THROUGH, filter->data));
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
			} else {
				w = 0;
			}
			if (w <= 0) {
				if (s1 < 0x7F && s2 < 0x7F) {
					w = (s1 << 8) | s2 | MBFL_WCSPLANE_JIS0208;
				} else {
					w = (c1 << 8) | c | MBFL_WCSGROUP_THROUGH;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			w = (c1 << 8) | c | MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
	}

	return c;
}

static int mbfl_filt_conv_sjis_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		mbfl_filt_conv_illegal_output(filter->cache, filter);
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
	if (c == 0x5C) {
		/* Unicode 0x5C is a backslash; but Shift-JIS uses 0x5C for the
		 * Yen sign. JIS X 0208 kuten 0x2140 is a backslash. */
		s1 = 0x2140;
	} else if (c == 0x7E) {
		/* Unicode 0x7E is a tilde, but Shift-JIS uses 0x7E for overline (or
		 * macron). JIS X 0208 kuten 0x2141 is 'WAVE DASH' */
		s1 = 0x2141;
	} else if (c == 0xAF || c == 0x203E) { /* U+00AF is MACRON, U+203E is OVERLINE */
		s1 = 0x7E; /* Halfwidth overline/macron */
	} else if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
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
			s1 = 0x5C;
		} else if (c == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0xFF5E) { /* FULLWIDTH TILDE */
			s1 = 0x2141;
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

	return c;
}
