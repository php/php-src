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
 * The author of this file: Moriyoshi Koizumi <koizumi@gree.co.jp>
 *
 */

#include "mbfilter.h"
#include "mbfilter_cp5022x.h"
#include "mbfilter_jis.h"
#include "mbfilter_tl_jisx0201_jisx0208.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"

static int mbfl_filt_conv_cp5022x_wchar_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_cp50220_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_cp50220(int c, mbfl_convert_filter *filter);

/* Previously, a dubious 'encoding' called 'cp50220raw' was supported
 * This was just CP50220, but the implementation was less strict regarding
 * invalid characters; it would silently pass some through
 * This 'encoding' only existed in mbstring. In case some poor, lost soul is
 * still using it, retain minimal support by aliasing it to CP50220
 *
 * Further, mbstring also had a made-up encoding called "JIS-ms"
 * This was the same as CP5022{0,1,2}, but without their special ways of
 * handling conversion of Unicode half-width katakana */
static const char *cp50220_aliases[] = {"cp50220raw", "cp50220-raw", "JIS-ms", NULL};

const mbfl_encoding mbfl_encoding_cp50220 = {
	mbfl_no_encoding_cp50220,
	"CP50220",
	"ISO-2022-JP",
	cp50220_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50220_wchar,
	&vtbl_wchar_cp50220
};

const mbfl_encoding mbfl_encoding_cp50221 = {
	mbfl_no_encoding_cp50221,
	"CP50221",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50221_wchar,
	&vtbl_wchar_cp50221
};

const mbfl_encoding mbfl_encoding_cp50222 = {
	mbfl_no_encoding_cp50222,
	"CP50222",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50222_wchar,
	&vtbl_wchar_cp50222
};

const struct mbfl_convert_vtbl vtbl_cp50220_wchar = {
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50220 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50220,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50220,
	mbfl_filt_conv_wchar_cp50220_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_cp50221_wchar = {
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50221 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50221,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_cp50222_wchar = {
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50222 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50222,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50222,
	mbfl_filt_conv_wchar_cp50222_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_cp5022x_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x20:	 X 0201 kana */
/*	case 0x80:	 X 0208 */
/*	case 0x90:	 X 0212 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (c == 0x0e) {		/* "kana in" */
			filter->status = 0x20;
		} else if (c == 0x0f) {		/* "kana out" */
			filter->status = 0;
		} else if (filter->status == 0x10 && c == 0x5c) {	/* YEN SIGN */
			CK((*filter->output_function)(0xa5, filter->data));
		} else if (filter->status == 0x10 && c == 0x7e) {	/* OVER LINE */
			CK((*filter->output_function)(0x203e, filter->data));
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x60) {		/* kana */
			CK((*filter->output_function)(0xff40 + c, filter->data));
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x93) {		/* kanji first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* GR kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

/*	case 0x81:	 X 0208 second char */
/*	case 0x91:	 X 0212 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (filter->status == 0x80) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= 94 * 94 && s < 114 * 94) {
					/* user-defined => PUA (Microsoft extended) */
					w = s - 94*94 + 0xe000;
				} else {
					w = 0;
				}
				if (w <= 0) {
					w = (c1 << 8) | c;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0208;
				}
			} else {
				if (s >= 0 && s < jisx0212_ucs_table_size) {
					w = jisx0212_ucs_table[s];
				} else {
					w = 0;
				}
				if (w <= 0) {
					w = (c1 << 8) | c | MBFL_WCSPLANE_JIS0212;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			w = (c1 << 8) | c | MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	/* ESC */
/*	case 0x02:	*/
/*	case 0x12:	*/
/*	case 0x22:	*/
/*	case 0x82:	*/
/*	case 0x92:	*/
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b | MBFL_WCSGROUP_THROUGH, filter->data));
			goto retry;
		}
		break;

	/* ESC $ */
/*	case 0x03:	*/
/*	case 0x13:	*/
/*	case 0x23:	*/
/*	case 0x83:	*/
/*	case 0x93:	*/
	case 3:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {			/* '(' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b | MBFL_WCSGROUP_THROUGH, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			goto retry;
		}
		break;

	/* ESC $ ( */
/*	case 0x04:	*/
/*	case 0x14:	*/
/*	case 0x24:	*/
/*	case 0x84:	*/
/*	case 0x94:	*/
	case 4:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x44) {			/* 'D' */
			filter->status = 0x90;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b | MBFL_WCSGROUP_THROUGH, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

	/* ESC ( */
/*	case 0x05:	*/
/*	case 0x15:	*/
/*	case 0x25:	*/
/*	case 0x85:	*/
/*	case 0x95:	*/
	case 5:
		if (c == 0x42 || c == 0x48) {		/* 'B' or 'H' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b | MBFL_WCSGROUP_THROUGH, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int mbfl_filt_conv_cp5022x_wchar_flush(mbfl_convert_filter *filter)
{
	if ((filter->status & 0xF) == 1) {
		/* 2-byte (JIS X 0208 or 0212) character was truncated */
		CK((*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data));
	}
	return 0;
}

static int mbfl_filt_conv_wchar_cp50220(int c, mbfl_convert_filter *filter)
{
	int mode = MBFL_FILT_TL_HAN2ZEN_KATAKANA | MBFL_FILT_TL_HAN2ZEN_GLUE, second = 0;
	bool consumed = false;

	if (filter->cache) {
		int s = mbfl_convert_kana(filter->cache, c, &consumed, &second, mode);
		filter->cache = consumed ? 0 : c;
		mbfl_filt_conv_wchar_cp50221(s, filter);
		if (second) {
			mbfl_filt_conv_wchar_cp50221(second, filter);
		}
	} else if (c == 0) {
		/* This case has to be handled separately, since `filter->cache == 0` means
		 * no codepoint is cached */
		(*filter->output_function)(0, filter->data);
	} else {
		filter->cache = c;
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp50220_flush(mbfl_convert_filter *filter)
{
	int mode = MBFL_FILT_TL_HAN2ZEN_KATAKANA | MBFL_FILT_TL_HAN2ZEN_GLUE, second = 0;

	if (filter->cache) {
		int s = mbfl_convert_kana(filter->cache, 0, NULL, &second, mode);
		mbfl_filt_conv_wchar_cp50221(s, filter);
		if (second) {
			mbfl_filt_conv_wchar_cp50221(s, filter);
		}
		filter->cache = 0;
	}

	return mbfl_filt_conv_any_jis_flush(filter);
}

int mbfl_filt_conv_wchar_cp50221(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c <= 0xE757) {
		/* 'private'/'user' codepoints */
		s = c - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
	}

	if (s <= 0) {
		if (c == 0xa5) {			/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
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
		}
	}

	/* Above, we do a series of lookups in `ucs_*_jis_table` to find a
	 * corresponding kuten code for this Unicode codepoint
	 * If we get zero, that means the codepoint is not in JIS X 0208
	 * On the other hand, if we get a result with the high bits set on both
	 * upper and lower bytes, that is not a code in JIS X 0208 but rather
	 * in JIS X 0213
	 * In either case, check if this codepoint is one of the extensions added
	 * to JIS X 0208 by MicroSoft (to make CP932) */
	if (s == 0 || ((s & 0x8000) && (s & 0x80))) {
		int i;
		s = -1;

		for (i = 0;
				i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
				i++) {
			const int oh = cp932ext1_ucs_table_min / 94;

			if (c == cp932ext1_ucs_table[i]) {
				s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
				break;
			}
		}

		if (s < 0) {
			const int oh = cp932ext2_ucs_table_min / 94;
			const int cp932ext2_ucs_table_size =
					cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			for (i = 0; i < cp932ext2_ucs_table_size; i++) {
				if (c == cp932ext2_ucs_table[i]) {
					s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
					break;
				}
			}
		}

		if (s < 0) {
			const int cp932ext3_ucs_table_size =
					cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
			const int limit = cp932ext3_ucs_table_size >
					cp932ext3_eucjp_table_size ?
						cp932ext3_eucjp_table_size:
						cp932ext3_ucs_table_size;
			for (i = 0; i < limit; i++) {
				if (c == cp932ext3_ucs_table[i]) {
					s = cp932ext3_eucjp_table[i];
					break;
				}
			}
		}

		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0;
			}
			CK((*filter->output_function)(s, filter->data));
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x49, filter->data));		/* 'I' */
				filter->status = 0x500;
			}
			CK((*filter->output_function)(s - 0x80, filter->data));
		} else if (s <= 0x927E) { /* X 0208 + extensions */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			CK(mbfl_filt_conv_illegal_output(c, filter));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

/*
 * wchar => CP50222
 */
int mbfl_filt_conv_wchar_cp50222(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c <= 0xE757) {
		/* 'private'/'user' codepoints */
		s = c - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
	}

	if (s <= 0) {
		if (c == 0xa5) {			/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
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
		}
	}
	if (s == 0 || ((s & 0x8000) && (s & 0x80))) {
		int i;
		s = -1;

		for (i = 0;
				i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
			const int oh = cp932ext1_ucs_table_min / 94;

			if (c == cp932ext1_ucs_table[i]) {
				s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
				break;
			}
		}

		if (s <= 0) {
			const int oh = cp932ext2_ucs_table_min / 94;
			const int cp932ext2_ucs_table_size =
					cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			for (i = 0; i < cp932ext2_ucs_table_size; i++) {
				if (c == cp932ext2_ucs_table[i]) {
					s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
					break;
				}
			}
		}

		if (s <= 0) {
			const int cp932ext3_ucs_table_size =
					cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
			const int limit = cp932ext3_ucs_table_size >
					cp932ext3_eucjp_table_size ?
						cp932ext3_eucjp_table_size:
						cp932ext3_ucs_table_size;
			for (i = 0; i < limit; i++) {
				if (c == cp932ext3_ucs_table[i]) {
					s = cp932ext3_eucjp_table[i];
					break;
				}
			}
		}

		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			} else if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0;
			}
			CK((*filter->output_function)(s, filter->data));
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				CK((*filter->output_function)(0x0e, filter->data));		/* SI */
				filter->status = 0x500;
			}
			CK((*filter->output_function)(s - 0x80, filter->data));
		} else if (s <= 0x927E) { /* X 0208 */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			CK(mbfl_filt_conv_illegal_output(c, filter));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

int
mbfl_filt_conv_wchar_cp50222_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) == 0x500) {
		CK((*filter->output_function)(0x0f, filter->data));		/* SO */
	} else if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
	}
	filter->status &= 0xff;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}
