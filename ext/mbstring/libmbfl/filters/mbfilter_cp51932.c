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

static void mbfl_filt_conv_cp51932_wchar_flush(mbfl_convert_filter *filter);
static void mbfl_filt_ident_cp51932(unsigned char c, mbfl_identify_filter *filter);

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

const struct mbfl_identify_vtbl vtbl_identify_cp51932 = {
	mbfl_no_encoding_cp51932,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_cp51932
};

const mbfl_encoding mbfl_encoding_cp51932 = {
	mbfl_no_encoding_cp51932,
	"CP51932",
	"CP51932",
	mbfl_encoding_cp51932_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_cp51932_wchar,
	&vtbl_wchar_cp51932
};

const struct mbfl_convert_vtbl vtbl_cp51932_wchar = {
	mbfl_no_encoding_cp51932,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp51932_wchar,
	mbfl_filt_conv_cp51932_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp51932 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp51932,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp51932,
	mbfl_filt_conv_common_flush
};

/*
 * cp51932 => wchar
 */
void mbfl_filt_conv_cp51932_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			(*filter->output_function)(c, filter->data);
		} else if (c >= 0xA1 && c <= 0xFE) { /* CP932 first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* kana first char */
			filter->status = 2;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			(*filter->output_function)(w, filter->data);
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
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_WINCP932;
			}
			(*filter->output_function)(w, filter->data);
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			(*filter->output_function)(w, filter->data);
		}
		break;

	case 2:	/* got 0x8e, X0201 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			(*filter->output_function)(w, filter->data);
		} else {
			w = 0x8e00 | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			(*filter->output_function)(w, filter->data);
		}
		break;

	default:
		filter->status = 0;
		break;
	}
}

static void mbfl_filt_conv_cp51932_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		(*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}
}

/*
 * wchar => cp51932
 */
void mbfl_filt_conv_wchar_cp51932(int c, mbfl_convert_filter *filter)
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
		if (c == 0xa5) {		/* YEN SIGN */
			s1 = 0x005c;			/* YEN SIGN */
		} else if (c == 0x203e) {	/* OVER LINE */
			s1 = 0x007e;			/* FULLWIDTH MACRON */
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s1 = 0x2141;
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
			(*filter->output_function)(s1, filter->data);
		} else if (s1 < 0x100) {	/* kana */
			(*filter->output_function)(0x8e, filter->data);
			(*filter->output_function)(s1, filter->data);
		} else if (s1 < 0x8080)  {	/* X 0208 */
			(*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data);
			(*filter->output_function)((s1 & 0xff) | 0x80, filter->data);
		} else {
		    mbfl_filt_conv_illegal_output(c, filter);
		}
	} else {
		mbfl_filt_conv_illegal_output(c, filter);
	}
}

int is_unused_cp932_range(unsigned char byte1, unsigned char byte2)
{
	int kuten = (byte1 - 0xA1) * 94 + byte2 - 0xA1;
	if (kuten <= 137) {
		if (kuten == 31 || kuten == 32 || kuten == 33 || kuten == 60 || kuten == 80 || kuten == 81 || kuten == 137) {
			return 0;
		}
	}

	int wchar = 0;
	if (kuten >= cp932ext1_ucs_table_min && kuten < cp932ext1_ucs_table_max) {
		wchar = cp932ext1_ucs_table[kuten - cp932ext1_ucs_table_min];
	} else if (kuten >= 0 && kuten < jisx0208_ucs_table_size) {
		wchar = jisx0208_ucs_table[kuten];
	} else if (kuten >= cp932ext2_ucs_table_min && kuten < cp932ext2_ucs_table_max) {
		wchar = cp932ext2_ucs_table[kuten - cp932ext2_ucs_table_min];
	}

	return !wchar;
}

static void mbfl_filt_ident_cp51932(unsigned char c, mbfl_identify_filter *filter)
{
	switch (filter->status & 0xF) {
	case 0: /* ASCII */
		if (c < 0x80) { /* ok */
			;
		} else if (c >= 0xA1 && c <= 0xFC) { /* CP932, first byte */
			filter->status = (c << 8) | 1;
		} else if (c == 0x8E) { /* Kana, first char */
			filter->status = 2;
		} else {
			filter->flag = 1; /* bad */
		}
		break;

	case 1: /* Second byte of CP932 character */
		if (c < 0xA1 || c > 0xFE || is_unused_cp932_range(filter->status >> 8, c)) { /* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;

	case 2:	/* JIS X 0201 Kana */
		if (c < 0xA1 || c > 0xDF) { /* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;
	}
}
