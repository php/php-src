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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_cp5022x.h"
#include "mbfilter_jis.h"
#include "mbfilter_tl_jisx0201_jisx0208.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"

typedef struct _mbfl_filt_conv_wchar_cp50220_ctx {
	mbfl_filt_tl_jisx0201_jisx0208_param tl_param;
	mbfl_convert_filter last;
} mbfl_filt_conv_wchar_cp50220_ctx;

static int mbfl_filt_ident_jis_ms(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_cp50220(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_cp50221(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_cp50222(int c, mbfl_identify_filter *filter);
static void mbfl_filt_conv_wchar_cp50220_ctor(mbfl_convert_filter *filt);
static void mbfl_filt_conv_wchar_cp50220_dtor(mbfl_convert_filter *filt);
static void mbfl_filt_conv_wchar_cp50220_copy(mbfl_convert_filter *src, mbfl_convert_filter *dest);

const mbfl_encoding mbfl_encoding_jis_ms = {
	mbfl_no_encoding_jis_ms,
	"JIS-ms",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_cp50220 = {
	mbfl_no_encoding_cp50220,
	"CP50220",
	"ISO-2022-JP",
	(const char *(*)[])NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_cp50220raw = {
	mbfl_no_encoding_cp50220raw,
	"CP50220raw",
	"ISO-2022-JP",
	(const char *(*)[])NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_cp50221 = {
	mbfl_no_encoding_cp50221,
	"CP50221",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_cp50222 = {
	mbfl_no_encoding_cp50222,
	"CP50222",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE
};

const struct mbfl_identify_vtbl vtbl_identify_jis_ms = {
	mbfl_no_encoding_jis_ms,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_jis_ms
};

const struct mbfl_identify_vtbl vtbl_identify_cp50220 = {
	mbfl_no_encoding_cp50220,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp50220
};

const struct mbfl_identify_vtbl vtbl_identify_cp50220raw = {
	mbfl_no_encoding_cp50220raw,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp50220
};

const struct mbfl_identify_vtbl vtbl_identify_cp50221 = {
	mbfl_no_encoding_cp50221,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp50221
};

const struct mbfl_identify_vtbl vtbl_identify_cp50222 = {
	mbfl_no_encoding_cp50222,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp50222
};

const struct mbfl_convert_vtbl vtbl_jis_ms_wchar = {
	mbfl_no_encoding_jis_ms,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_ms_wchar,
	mbfl_filt_conv_common_flush,
};

const struct mbfl_convert_vtbl vtbl_wchar_jis_ms = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_jis_ms,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_jis_ms,
	mbfl_filt_conv_any_jis_flush
};

const struct mbfl_convert_vtbl vtbl_cp50220_wchar = {
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_ms_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50220 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50220,
	mbfl_filt_conv_wchar_cp50220_ctor,
	mbfl_filt_conv_wchar_cp50220_dtor,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush,
	mbfl_filt_conv_wchar_cp50220_copy
};

const struct mbfl_convert_vtbl vtbl_cp50220raw_wchar = {
	mbfl_no_encoding_cp50220raw,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_ms_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50220raw = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50220raw,
	mbfl_filt_conv_wchar_cp50220_ctor,
	mbfl_filt_conv_wchar_cp50220_dtor,
	mbfl_filt_conv_wchar_cp50220raw,
	mbfl_filt_conv_any_jis_flush,
	mbfl_filt_conv_wchar_cp50220_copy
};

const struct mbfl_convert_vtbl vtbl_cp50221_wchar = {
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_ms_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50221 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50221,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush
};

const struct mbfl_convert_vtbl vtbl_cp50222_wchar = {
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_ms_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50222 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50222,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_cp50222,
	mbfl_filt_conv_wchar_cp50222_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * JIS-ms => wchar
 */
int
mbfl_filt_conv_jis_ms_wchar(int c, mbfl_convert_filter *filter)
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
				if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
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
			CK((*filter->output_function)(w, filter->data));
		} else if (c == 0x1b) {
			filter->status += 2;
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
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
			CK((*filter->output_function)(0x1b, filter->data));
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
			CK((*filter->output_function)(0x1b, filter->data));
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
			CK((*filter->output_function)(0x1b, filter->data));
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
			CK((*filter->output_function)(0x1b, filter->data));
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

/*
 * wchar => JIS
 */
int
mbfl_filt_conv_wchar_jis_ms(int c, mbfl_convert_filter *filter)
{
	int c1, s;

	s = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 10 * 94)) {
		/* PUE => Microsoft extended (pseudo 95ku - 114ku) */
		/* See http://www.opengroup.or.jp/jvc/cde/ucs-conv.html#ch4_2 */
		s = c - 0xe000;
		s = (s / 94 + 0x75) << 8 | (s % 94 + 0x21);
	} else if (c >= (0xe000 + 10 * 94) && c <= (0xe000 + 20 * 94)) {
		/* PUE => JISX0212 user-defined (G3 85ku - 94ku) */
		/* See http://www.opengroup.or.jp/jvc/cde/ucs-conv.html#ch4_2 */
		s = c - (0xe000 + 10 * 94);
		s = (s / 94 + 0xf5) << 8 | (s % 94 + 0xa1);
	}

	/* do some transliteration */
	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_JIS0208) {
			s = c & MBFL_WCSPLANE_MASK;
		} else if (c1 == MBFL_WCSPLANE_JIS0212) {
			s = c & MBFL_WCSPLANE_MASK;
			s |= 0x8080;
		} else if (c == 0xa5) {		/* YEN SIGN */
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
	if (s <= 0 || (s >= 0x8080 && s < 0x10000)) {
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
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x100) { /* kana */
			if ((filter->status & 0xff00) != 0x100) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x49, filter->data));		/* 'I' */
			}
			filter->status = 0x100;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else if (s < 0x8080) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else if (s < 0x10000) { /* X 0212 */
			if ((filter->status & 0xff00) != 0x300) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x44, filter->data));		/* 'D' */
			}
			filter->status = 0x300;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * wchar => CP50220
 */
static void
mbfl_filt_conv_wchar_cp50220_ctor(mbfl_convert_filter *filt)
{
	mbfl_filt_conv_wchar_cp50220_ctx *ctx;

	mbfl_filt_conv_common_ctor(filt);

	ctx = mbfl_malloc(sizeof(mbfl_filt_conv_wchar_cp50220_ctx));
	if (ctx == NULL) {
		mbfl_filt_conv_common_dtor(filt);
		return;
	}

	ctx->tl_param.mode = MBFL_FILT_TL_HAN2ZEN_KATAKANA | MBFL_FILT_TL_HAN2ZEN_GLUE;

	ctx->last = *filt;
	ctx->last.opaque = ctx;
	ctx->last.data = filt->data;
	filt->filter_function = vtbl_tl_jisx0201_jisx0208.filter_function;
	filt->filter_flush = vtbl_tl_jisx0201_jisx0208.filter_flush;
	filt->output_function = (int(*)(int, void *))ctx->last.filter_function;
	filt->flush_function = (int(*)(void *))ctx->last.filter_flush;
	filt->data = &ctx->last;
	filt->opaque = ctx;
	vtbl_tl_jisx0201_jisx0208.filter_ctor(filt);
}

static void
mbfl_filt_conv_wchar_cp50220_copy(mbfl_convert_filter *src, mbfl_convert_filter *dest)
{
	mbfl_filt_conv_wchar_cp50220_ctx *ctx;

	*dest = *src;
	ctx = mbfl_malloc(sizeof(mbfl_filt_conv_wchar_cp50220_ctx));
	if (ctx != NULL) {
		*ctx = *(mbfl_filt_conv_wchar_cp50220_ctx*)src->opaque;
	}

	dest->opaque = ctx;
	dest->data = &ctx->last;
}

static void
mbfl_filt_conv_wchar_cp50220_dtor(mbfl_convert_filter *filt)
{
	vtbl_tl_jisx0201_jisx0208.filter_dtor(filt);

	if (filt->opaque != NULL) {
		mbfl_free(filt->opaque);
	}

	mbfl_filt_conv_common_dtor(filt);
}

/*
 * wchar => cp50220raw
 */
int
mbfl_filt_conv_wchar_cp50220raw(int c, mbfl_convert_filter *filter)
{
	if (c & MBFL_WCSPLANE_JIS0208) {
		const int s = c & MBFL_WCSPLANE_MASK;
	
		if ((filter->status & 0xff00) != 0x200) {
			CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
			CK((*filter->output_function)(0x24, filter->data));		/* '$' */
			CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			filter->status = 0x200;
		}
		CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
		CK((*filter->output_function)(s & 0x7f, filter->data));
		return c;
	} else {
		return mbfl_filt_conv_wchar_cp50221(c, filter);
	}
}

/*
 * wchar => CP50221
 */
int
mbfl_filt_conv_wchar_cp50221(int c, mbfl_convert_filter *filter)
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
	} else if (c >= 0xe000 && c < (0xe000 + 10 * 94)) {
		/* PUE => Microsoft extended */
		/* See http://www.opengroup.or.jp/jvc/cde/ucs-conv.html#ch4_2 */
		s = c - 0xe000;
		s = (s / 94 + 0x75) << 8 | (s % 94 + 0x21);
	} else if (c >= (0xe000 + 10 * 94) && c <= (0xe000 + 20 * 94)) {
		/* PUE => JISX0212 user-defined (G3 85ku - 94ku) */
		/* See http://www.opengroup.or.jp/jvc/cde/ucs-conv.html#ch4_2 */
		s = c - (0xe000 + 10 * 94);
		s = (s / 94 + 0xf5) << 8 | (s % 94 + 0xa1);
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
	if (s <= 0 || (s >= 0x8080 && s < 0x10000)) {
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
		} else if (s < 0x8080) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * wchar => CP50222
 */
int
mbfl_filt_conv_wchar_cp50222(int c, mbfl_convert_filter *filter)
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
	} else if (c >= 0xe000 && c < (0xe000 + 10 * 94)) {
		/* PUE => Microsoft extended */
		/* See http://www.opengroup.or.jp/jvc/cde/ucs-conv.html#ch4_2 */
		s = c - 0xe000;
		s = (s / 94 + 0x75) << 8 | (s % 94 + 0x21);
	} else if (c >= (0xe000 + 10 * 94) && c <= (0xe000 + 20 * 94)) {
		/* PUE => JISX0212 user-defined (G3 85ku - 94ku) */
		/* See http://www.opengroup.or.jp/jvc/cde/ucs-conv.html#ch4_2 */
		s = c - (0xe000 + 10 * 94);
		s = (s / 94 + 0xf5) << 8 | (s % 94 + 0xa1);
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
	if (s <= 0 || (s >= 0x8080 && s < 0x10000)) {
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
		} else if (s < 0x8080) { /* X 0208 */
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
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
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

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}


static int mbfl_filt_ident_jis_ms(int c, mbfl_identify_filter *filter)
{
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
		} else if (c == 0x0e) {			/* "kana in" */
			filter->status = 0x20;
		} else if (c == 0x0f) {			/* "kana out" */
			filter->status = 0;
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
/*	case 0x91:	 X 0212 second char */
	case 1:
		filter->status &= ~0xf;
		if (c == 0x1b) {
			goto retry;
		} else if (c < 0x21 || c > 0x7e) {		/* bad */
			filter->flag = 1;
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {		/* '(' */
			filter->status++;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x44) {		/* 'D' */
			filter->status = 0x90;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42 || c == 0x48) {		/* 'B' or 'H' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int mbfl_filt_ident_cp50220(int c, mbfl_identify_filter *filter)
{
retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x80:	 X 0208 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (filter->status == 0x80 && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
	case 1:
		if (c == 0x1b) {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			if (c < 0x21 || c > 0x7e) {		/* bad */
				filter->flag = 1;
			}
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42) {		/* 'B' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int mbfl_filt_ident_cp50221(int c, mbfl_identify_filter *filter)
{
retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x80:	 X 0208 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (filter->status == 0x80 && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
	case 1:
		if (c == 0x1b) {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			if (c < 0x21 || c > 0x7e) {		/* bad */
				filter->flag = 1;
			}
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42) {		/* 'B' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int mbfl_filt_ident_cp50222(int c, mbfl_identify_filter *filter)
{
retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x80:	 X 0208 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (filter->status == 0x80 && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
	case 1:
		if (c == 0x1b) {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			if (c < 0x21 || c > 0x7e) {		/* bad */
				filter->flag = 1;
			}
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42) {		/* 'B' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}



