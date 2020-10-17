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

static void mbfl_filt_ident_cp5022x(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_conv_wchar_cp50220_ctor(mbfl_convert_filter *filt);
static void mbfl_filt_conv_wchar_cp50220_dtor(mbfl_convert_filter *filt);

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
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50220_wchar,
	&vtbl_wchar_cp50220
};

const mbfl_encoding mbfl_encoding_cp50221 = {
	mbfl_no_encoding_cp50221,
	"CP50221",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50221_wchar,
	&vtbl_wchar_cp50221
};

const mbfl_encoding mbfl_encoding_cp50222 = {
	mbfl_no_encoding_cp50222,
	"CP50222",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50222_wchar,
	&vtbl_wchar_cp50222
};

const struct mbfl_identify_vtbl vtbl_identify_cp50220 = {
	mbfl_no_encoding_cp50220,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_cp5022x
};

const struct mbfl_identify_vtbl vtbl_identify_cp50221 = {
	mbfl_no_encoding_cp50221,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_cp5022x
};

const struct mbfl_identify_vtbl vtbl_identify_cp50222 = {
	mbfl_no_encoding_cp50222,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_cp5022x
};

const struct mbfl_convert_vtbl vtbl_cp50220_wchar = {
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50220 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50220,
	mbfl_filt_conv_wchar_cp50220_ctor,
	mbfl_filt_conv_wchar_cp50220_dtor,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush
};

const struct mbfl_convert_vtbl vtbl_cp50221_wchar = {
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50221 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50221,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush
};

const struct mbfl_convert_vtbl vtbl_cp50222_wchar = {
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50222 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50222,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50222,
	mbfl_filt_conv_wchar_cp50222_flush
};

void mbfl_filt_conv_cp5022x_wchar(int c, mbfl_convert_filter *filter)
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
			(*filter->output_function)(0xa5, filter->data);
		} else if (filter->status == 0x10 && c == 0x7e) {	/* OVER LINE */
			(*filter->output_function)(0x203e, filter->data);
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x60) {		/* kana */
			(*filter->output_function)(0xff40 + c, filter->data);
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x93) {		/* kanji first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			(*filter->output_function)(c, filter->data);
		} else if (c > 0xa0 && c < 0xe0) {	/* GR kana */
			(*filter->output_function)(0xfec0 + c, filter->data);
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			(*filter->output_function)(w, filter->data);
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
					w = (c1 << 8) | c;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0212;
				}
			}
			(*filter->output_function)(w, filter->data);
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			(*filter->output_function)(w, filter->data);
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
			(*filter->output_function)(0x1b, filter->data);
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
			(*filter->output_function)(0x1b, filter->data);
			(*filter->output_function)(0x24, filter->data);
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
			(*filter->output_function)(0x1b, filter->data);
			(*filter->output_function)(0x24, filter->data);
			(*filter->output_function)(0x28, filter->data);
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
			(*filter->output_function)(0x1b, filter->data);
			(*filter->output_function)(0x28, filter->data);
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}
}

/*
 * wchar => CP50220
 */
static void
mbfl_filt_conv_wchar_cp50220_ctor(mbfl_convert_filter *filt)
{
	mbfl_filt_conv_common_ctor(filt);

	mbfl_convert_filter *last = emalloc(sizeof(mbfl_convert_filter));
	*last = *filt;
	filt->filter_function = vtbl_tl_jisx0201_jisx0208.filter_function;
	filt->filter_flush = (filter_flush_t)vtbl_tl_jisx0201_jisx0208.filter_flush;
	filt->output_function = (output_function_t)last->filter_function;
	filt->flush_function = (flush_function_t)last->filter_flush;
	filt->data = last;
	filt->opaque = (void*)(MBFL_FILT_TL_HAN2ZEN_KATAKANA | MBFL_FILT_TL_HAN2ZEN_GLUE);
}

static void
mbfl_filt_conv_wchar_cp50220_dtor(mbfl_convert_filter *filt)
{
	if (filt->data) {
		efree(filt->data);
	}
}

/*
 * wchar => CP50221
 */
void mbfl_filt_conv_wchar_cp50221(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
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
		} else if (c == 0x203e) {	/* OVER LINE */
			s = 0x1007e;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s = 0x2141;
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
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x28, filter->data);		/* '(' */
				(*filter->output_function)(0x42, filter->data);		/* 'B' */
				filter->status = 0;
			}
			(*filter->output_function)(s, filter->data);
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x28, filter->data);		/* '(' */
				(*filter->output_function)(0x49, filter->data);		/* 'I' */
				filter->status = 0x500;
			}
			(*filter->output_function)(s - 0x80, filter->data);
		} else if (s <= 0x927E) { /* X 0208 + extensions */
			if ((filter->status & 0xff00) != 0x200) {
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x24, filter->data);		/* '$' */
				(*filter->output_function)(0x42, filter->data);		/* 'B' */
				filter->status = 0x200;
			}
			(*filter->output_function)((s >> 8) & 0xFF, filter->data);
			(*filter->output_function)(s & 0xFF, filter->data);
		} else if (s < 0x10000) { /* X0212 */
			mbfl_filt_conv_illegal_output(c, filter);
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x28, filter->data);		/* '(' */
				(*filter->output_function)(0x4a, filter->data);		/* 'J' */
			}
			filter->status = 0x400;
			(*filter->output_function)(s & 0x7f, filter->data);
		}
	} else {
		mbfl_filt_conv_illegal_output(c, filter);
	}
}

/*
 * wchar => CP50222
 */
void mbfl_filt_conv_wchar_cp50222(int c, mbfl_convert_filter *filter)
{
	int s;

	s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
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
		} else if (c == 0x203e) {	/* OVER LINE */
			s = 0x1007e;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s = 0x2141;
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
				(*filter->output_function)(0x0f, filter->data);		/* SO */
				filter->status = 0;
			} else if ((filter->status & 0xff00) != 0) {
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x28, filter->data);		/* '(' */
				(*filter->output_function)(0x42, filter->data);		/* 'B' */
				filter->status = 0;
			}
			(*filter->output_function)(s, filter->data);
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				(*filter->output_function)(0x0e, filter->data);		/* SI */
				filter->status = 0x500;
			}
			(*filter->output_function)(s - 0x80, filter->data);
		} else if (s <= 0x927E) { /* X 0208 */
			if ((filter->status & 0xff00) == 0x500) {
				(*filter->output_function)(0x0f, filter->data);		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x200) {
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x24, filter->data);		/* '$' */
				(*filter->output_function)(0x42, filter->data);		/* 'B' */
				filter->status = 0x200;
			}
			(*filter->output_function)((s >> 8) & 0xFF, filter->data);
			(*filter->output_function)(s & 0xFF, filter->data);
		} else if (s < 0x10000) { /* X0212 */
			mbfl_filt_conv_illegal_output(c, filter);
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) == 0x500) {
				(*filter->output_function)(0x0f, filter->data);		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x400) {
				(*filter->output_function)(0x1b, filter->data);		/* ESC */
				(*filter->output_function)(0x28, filter->data);		/* '(' */
				(*filter->output_function)(0x4a, filter->data);		/* 'J' */
			}
			filter->status = 0x400;
			(*filter->output_function)(s & 0x7f, filter->data);
		}
	} else {
		mbfl_filt_conv_illegal_output(c, filter);
	}
}

void mbfl_filt_conv_wchar_cp50222_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) == 0x500) {
		(*filter->output_function)(0x0f, filter->data);		/* SO */
	} else if ((filter->status & 0xff00) != 0) {
		(*filter->output_function)(0x1b, filter->data);		/* ESC */
		(*filter->output_function)(0x28, filter->data);		/* '(' */
		(*filter->output_function)(0x42, filter->data);		/* 'B' */
	}
	filter->status &= 0xff;

	if (filter->flush_function != NULL) {
		(*filter->flush_function)(filter->data);
	}
}

static void mbfl_filt_ident_cp5022x_x0201(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_cp5022x_x0208(unsigned char c, mbfl_identify_filter *filter);

static int handle_cp5022x_escape(unsigned char c, mbfl_identify_filter *filter)
{
	switch (filter->status) {
	case 0:
		if (c == 0x1B) {
			filter->status = 2;
			return 1;
		}
		break;

	/* 2nd byte of Kanji */
	case 1:
		if (c == 0x1B) {
			filter->flag = 1;
			filter->status = 2;
			return 1;
		}
		break;

	/* ESC */
	case 2:
		if (c == '$') {
			filter->status = 3;
			return 1;
		} else if (c == '(') {
			filter->status = 4;
			return 1;
		} else {
			filter->flag = 1; /* bad */
			filter->status = 0;
		}
		break;

	/* ESC $ */
	case 3:
		filter->status = 0;
		if (c == '@' || c == 'B') {
			filter->filter_function = mbfl_filt_ident_cp5022x_x0208;
			return 1;
		} else {
			filter->flag = 1; /* bad */
		}
		break;

	/* ESC ( */
	case 4:
		filter->status = 0;
		if (c == 'B' || c == 'J') {
			/* 'J' is actually for X 0201 Latin, but the acceptable bytes are the same */
			filter->filter_function = mbfl_filt_ident_cp5022x;
			return 1;
		} else if (c == 'I') { /* ESC ( I escape added by MicroSoft */
			filter->filter_function = mbfl_filt_ident_cp5022x_x0201;
			return 1;
		} else {
			filter->flag = 1; /* bad */
		}
	}

	return 0;
}

#undef IN
#define IN(c,lo,hi) ((c) >= lo && (c) <= hi)

static void mbfl_filt_ident_cp5022x(unsigned char c, mbfl_identify_filter *filter)
{
	/* We convert single bytes from 0xA1-0xDF to JIS X 0201 kana, even if
	 * no escape to shift to JIS X 0201 has been seen */
	if (!handle_cp5022x_escape(c, filter) && (IN(c,0x80,0xA0) || c >= 0xE0)) {
		filter->flag = 1;
	}
}

static void mbfl_filt_ident_cp5022x_x0201(unsigned char c, mbfl_identify_filter *filter)
{
	/* JIS X 0201 kana use single bytes from 0xA1-0xDF; when in X 0201 Kana mode,
	 * we map these to corresponding bytes with high bit cleared */
	if (!handle_cp5022x_escape(c, filter) && !IN(c,0x21,0x5F)) {
		filter->flag = 1;
	}
}

/* CP50220 actually uses the CP932 character set rather than "vanilla" JIS X 0208
 * Note we are working with kuten codes here, not the Shift-JIS-like representation
 * used for CP932 encoding
 * Although the number of characters in CP932 is significantly expanded from
 * JIS X 0208, there are still lots of unmapped kuten codes */
static int is_unused_cp932(unsigned char ku, unsigned char ten)
{
	if (IN(ku,0x29,0x2C) || ku == 0x2E || ku == 0x2F || IN(ku,0x75,0x78) || ku == 0x7D || ku == 0x7E) {
		return 1;
	} else if (IN(ku,0x22,0x28)) {
		if (ku == 0x22) {
			return IN(ten,0x2F,0x39) || IN(ten,0x42,0x49) || IN(ten,0x51,0x5B) || IN(ten,0x6B,0x71) || IN(ten,0x7A,0x7D);
		} else if (ku == 0x23) {
			return (ten <= 0x2F) || IN(ten,0x3A,0x40) || IN(ten,0x5B,0x60) || (ten >= 0x7B);
		} else if (ku == 0x24) {
			return (ten >= 0x74);
		} else if (ku == 0x25) {
			return (ten >= 0x77);
		} else if (ku == 0x26) {
			return IN(ten,0x39,0x40) || (ten >= 0x59);
		} else if (ku == 0x27) {
			return IN(ten,0x42,0x50) || (ten >= 0x72);
		} else { /* ku == 0x28 */
			return (ten >= 0x41);
		}
	} else if (ku == 0x2D) {
		return (ten == 0x3F) || IN(ten,0x57,0x5E) || (ten >= 0x7D);
	} else if (ku == 0x4F) {
		return (ten >= 0x54);
	} else if (ku == 0x74) {
		return (ten >= 0x27);
	} else if (ku == 0x7C) {
		return (ten == 0x6F) || (ten == 0x70);
	}
	return 0;
}

static void mbfl_filt_ident_cp5022x_x0208(unsigned char c, mbfl_identify_filter *filter)
{
	if (!handle_cp5022x_escape(c, filter)) {
		if (filter->status == 0) { /* First byte of Kanji */
			if (c < 0x21 || c > 0x92) {
				filter->flag = 1;
			}
			filter->status = c;
		} else { /* Second byte of Kanji */
			if (c < 0x21 || c > 0x7E || is_unused_cp932(filter->status, c)) {
				filter->flag = 1;
			}
			filter->status = 0;
		}
	}
}
