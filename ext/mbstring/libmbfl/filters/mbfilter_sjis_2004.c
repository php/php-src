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
 * The source code included in this files was separated from mbfilter_sjis.c
 * by rui hirokawa <hirokawa@php.net> on 15 aug 2011.
 */

/* Although the specification for Shift-JIS-2004 indicates that 0x5C and
 * 0x7E should (respectively) represent a Yen sign and an overbar, feedback
 * from Japanese PHP users indicates that they prefer 0x5C and 0x7E to be
 * treated as equivalent to U+005C and U+007E. This is the historical
 * behavior of mbstring, and promotes compatibility with other software
 * which handles Shift-JIS and Shift-JIS-2004 text in this way. */

#include "mbfilter.h"
#include "mbfilter_sjis_2004.h"
#include "mbfilter_euc_jp_2004.h"
#include "mbfilter_iso2022jp_2004.h"

#include "unicode_table_jis2004.h"
#include "unicode_table_jis.h"

extern const unsigned char mblen_table_sjis_mobile[];
extern const unsigned char mblen_table_eucjp[];

static size_t mb_sjis2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_sjis2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_eucjp2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_eucjp2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_iso2022jp2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_iso2022jp2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

extern int mbfl_bisec_srch(int w, const unsigned short *tbl, int n);
extern int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n);

static const char *mbfl_encoding_sjis2004_aliases[] = {"SJIS2004","Shift_JIS-2004", NULL};
static const char *mbfl_encoding_eucjp2004_aliases[] = {"EUC_JP-2004", NULL};

const mbfl_encoding mbfl_encoding_sjis2004 = {
	mbfl_no_encoding_sjis2004,
	"SJIS-2004",
	"Shift_JIS",
	mbfl_encoding_sjis2004_aliases,
	mblen_table_sjis_mobile, /* Leading byte values used for SJIS-2004 are the same as mobile SJIS variants */
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis2004_wchar,
	&vtbl_wchar_sjis2004,
	mb_sjis2004_to_wchar,
	mb_wchar_to_sjis2004,
	NULL
};

const struct mbfl_convert_vtbl vtbl_sjis2004_wchar = {
	mbfl_no_encoding_sjis2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_eucjp2004 = {
	mbfl_no_encoding_eucjp2004,
	"EUC-JP-2004",
	"EUC-JP",
	mbfl_encoding_eucjp2004_aliases,
	mblen_table_eucjp,
	0,
	&vtbl_eucjp2004_wchar,
	&vtbl_wchar_eucjp2004,
	mb_eucjp2004_to_wchar,
	mb_wchar_to_eucjp2004,
	NULL
};

const struct mbfl_convert_vtbl vtbl_eucjp2004_wchar = {
	mbfl_no_encoding_eucjp2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_eucjp2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_eucjp2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	"ISO-2022-JP-2004",
	"ISO-2022-JP-2004",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_2004_wchar,
	&vtbl_wchar_2022jp_2004,
	mb_iso2022jp2004_to_wchar,
	mb_wchar_to_iso2022jp2004,
	NULL
};

const struct mbfl_convert_vtbl vtbl_2022jp_2004_wchar = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jp_2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
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

int mbfl_filt_conv_jis2004_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, s, s1 = 0, s2 = 0, w = 0, w1;

	switch (filter->status & 0xf) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
				CK((*filter->output_function)(c, filter->data));
			} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
				if (c == 0x5c) {
					CK((*filter->output_function)(0x00a5, filter->data));
				} else if (c == 0x7e) {
					CK((*filter->output_function)(0x203e, filter->data));
				} else {
					CK((*filter->output_function)(c, filter->data));
				}
			} else { /* ISO-2022-JP-2004 */
				if (c == 0x1b) {
					filter->status += 6;
				} else if ((filter->status == 0x80 || filter->status == 0x90 || filter->status == 0xa0)
				   && c > 0x20 && c < 0x7f) { /* kanji first char */
					filter->cache = c;
					if (filter->status == 0x90) {
						filter->status += 1; /* JIS X 0213 plane 1 */
					} else if (filter->status == 0xa0) {
						filter->status += 4; /* JIS X 0213 plane 2 */
					} else {
						filter->status += 5; /* JIS X 0208 */
					}
				} else {
					CK((*filter->output_function)(c, filter->data));
				}
			}
		} else {
			if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
				if (c > 0xa0 && c < 0xff) { /* X 0213 plane 1 first char */
					filter->status = 1;
					filter->cache = c;
				} else if (c == 0x8e) { /* kana first char */
					filter->cache = 0x8E; /* So error will be reported if input is truncated right here */
					filter->status = 2;
				} else if (c == 0x8f) { /* X 0213 plane 2 first char */
					filter->status = 3;
				} else {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				}
			} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
				if (c > 0xa0 && c < 0xe0) { /* kana */
					CK((*filter->output_function)(0xfec0 + c, filter->data));
				} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
					filter->status = 1;
					filter->cache = c;
				} else {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				}
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

	case 1: /* kanji second char */
		filter->status &= ~0xf;
		c1 = filter->cache;

		if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
			if (c > 0xa0 && c < 0xff) {
				s1 = c1 - 0x80;
				s2 = c - 0x80;
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				break;
			}
		} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
			if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
				SJIS_DECODE(c1, c, s1, s2);
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				break;
			}
		} else { /* ISO-2022-JP-2004 */
			if (c >= 0x21 && c <= 0x7E) {
				s1 = c1;
				s2 = c;
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				break;
			}
		}
		w1 = (s1 << 8) | s2;

		/* conversion for combining characters */
		if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) ||
			(w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 ||
			(w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
			k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
			if (k >= 0) {
				w = jisx0213_u2_tbl[2*k];
				CK((*filter->output_function)(w, filter->data));
				w = jisx0213_u2_tbl[2*k+1];
			}
		}

		/* conversion for BMP  */
		if (w <= 0) {
			w1 = (s1 - 0x21)*94 + s2 - 0x21;
			if (w1 >= 0 && w1 < jisx0213_ucs_table_size) {
				w = jisx0213_ucs_table[w1];
			}
		}

		/* conversion for CJK Unified Ideographs ext.B (U+2XXXX) */
		if (w <= 0) {
			w1 = (s1 << 8) | s2;
			k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				w = jisx0213_jis_u5_tbl[k] + 0x20000;
			}
		}

		if (w <= 0) {
			w = MBFL_BAD_INPUT;
		}
		CK((*filter->output_function)(w, filter->data));
		break;

	case 2: /* got 0x8e: EUC-JP-2004 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 3: /* X 0213 plane 2 first char: EUC-JP-2004 (0x8f) */
		if (c == 0xA1 || (c >= 0xA3 && c <= 0xA5) || c == 0xA8 || (c >= 0xAC && c <= 0xAF) || (c >= 0xEE && c <= 0xFE)) {
			filter->cache = c - 0x80;
			filter->status++;
		} else {
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 4: /* X 0213 plane 2 second char: EUC-JP-2004, ISO-2022-JP-2004 */
		filter->status &= ~0xF;
		c1 = filter->cache;
		if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
			c2 = c - 0x80;
		} else {
			c2 = c;
		}

		if (c2 < 0x21 || c2 > 0x7E) {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			break;
		}

		s1 = c1 - 0x21;
		s2 = c2 - 0x21;

		if (((s1 >= 0 && s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) ||
			(s1 >= 77 && s1 < 94)) && s2 >= 0 && s2 < 94) {
			/* calc offset from ku */
			for (k = 0; k < jisx0213_p2_ofst_len; k++) {
				if (s1 == jisx0213_p2_ofst[k]) {
					break;
				}
			}
			k -= jisx0213_p2_ofst[k];

			/* check for japanese chars in BMP */
			s = (s1 + 94 + k)*94 + s2;
			ZEND_ASSERT(s < jisx0213_ucs_table_size);
			w = jisx0213_ucs_table[s];

			/* check for japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
			if (w <= 0) {
				w1 = ((c1 + k + 94) << 8) | c2;
				k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
				if (k >= 0) {
					w = jisx0213_jis_u5_tbl[k] + 0x20000;
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

	case 5: /* X 0208: ISO-2022-JP-2004 */
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (s >= 0 && s < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[s];
			}
		}

		if (w <= 0) {
			w = MBFL_BAD_INPUT;
		}

		CK((*filter->output_function)(w, filter->data));
		break;

	/* ESC: ISO-2022-JP-2004 */
/*	case 0x06:	*/
/*	case 0x16:	*/
/*	case 0x26:	*/
/*	case 0x86:	*/
/*	case 0x96:	*/
/*	case 0xa6:	*/
	case 6:
		if (c == '$') {
			filter->status++;
		} else if (c == '(') {
			filter->status += 3;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $: ISO-2022-JP-2004 */
/*	case 0x07:	*/
/*	case 0x17:	*/
/*	case 0x27:	*/
/*	case 0x87:	*/
/*	case 0x97:	*/
/*	case 0xa7:	*/
	case 7:
		if (c == 'B') { /* JIS X 0208-1983 */
			filter->status = 0x80;
		} else if (c == '(') {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC $ (: ISO-2022-JP-2004 */
/*	case 0x08:	*/
/*	case 0x18:	*/
/*	case 0x28:	*/
/*	case 0x88:	*/
/*	case 0x98:	*/
/*	case 0xa8:	*/
	case 8:
		if (c == 'Q') { /* JIS X 0213 plane 1 */
			filter->status = 0x90;
		} else if (c == 'P') { /* JIS X 0213 plane 2 */
			filter->status = 0xa0;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC (: ISO-2022-JP-2004 */
/*	case 0x09:	*/
/*	case 0x19:	*/
/*	case 0x29:	*/
/*	case 0x89:	*/
/*	case 0x99:	*/
	case 9:
		if (c == 'B') {
			filter->status = 0;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

int mbfl_filt_conv_jis2004_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_jis2004(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, s1, s2;

retry:
	s1 = 0;
	/* check for 1st char of combining characters */
	if ((filter->status & 0xf) == 0 && (
			c == 0x00E6 ||
			(c >= 0x0254 && c <= 0x02E9) ||
			(c >= 0x304B && c <= 0x3053) ||
			(c >= 0x30AB && c <= 0x30C8) ||
			c == 0x31F7)) {
		for (k = 0; k < jisx0213_u2_tbl_len; k++) {
			if (c == jisx0213_u2_tbl[2*k]) {
				filter->status++;
				filter->cache = k;
				return 0;
			}
		}
	}

	/* check for 2nd char of combining characters */
	if ((filter->status & 0xf) == 1 && filter->cache >= 0 && filter->cache < jisx0213_u2_tbl_len) {
		k = filter->cache;
		filter->status &= ~0xf;
		filter->cache = 0;

		c1 = jisx0213_u2_tbl[2*k];
		if ((c1 == 0x0254 || c1 == 0x028C || c1 == 0x0259 || c1 == 0x025A) && c == 0x0301) {
			k++;
		}
		if (c == jisx0213_u2_tbl[2*k+1]) {
			s1 = jisx0213_u2_key[k];
		} else { /* fallback */
			s1 = jisx0213_u2_fb_tbl[k];

			if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
			} else if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				s2 = (s1 & 0xff) + 0x80;
				s1 = ((s1 >> 8) & 0xff) + 0x80;
			} else {
				if (filter->status != 0x200) {
					CK((*filter->output_function)(0x1b, filter->data));
					CK((*filter->output_function)('$', filter->data));
					CK((*filter->output_function)('(', filter->data));
					CK((*filter->output_function)('Q', filter->data));
				}
				filter->status = 0x200;

				s2 = s1 & 0x7f;
				s1 = (s1 >> 8) & 0x7f;
			}

			/* Flush out cached data */
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
			goto retry;
		}
	}

	/* check for major japanese chars: U+4E00 - U+9FFF */
	if (s1 <= 0) {
		for (k = 0; k < uni2jis_tbl_len; k++) {
			if (c >= uni2jis_tbl_range[k][0] && c <= uni2jis_tbl_range[k][1]) {
				s1 = uni2jis_tbl[k][c-uni2jis_tbl_range[k][0]];
				break;
			}
		}
	}

	/* check for japanese chars in compressed mapping area: U+1E00 - U+4DBF */
	if (s1 <= 0 && c >= ucs_c1_jisx0213_min && c <= ucs_c1_jisx0213_max) {
		k = mbfl_bisec_srch(c, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
		if (k >= 0) {
			s1 = ucs_c1_jisx0213_ofst[k] + c - ucs_c1_jisx0213_tbl[2*k];
		}
	}

	/* check for japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
	if (s1 <= 0 && c >= jisx0213_u5_tbl_min && c <= jisx0213_u5_tbl_max) {
		k = mbfl_bisec_srch2(c - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
		if (k >= 0) {
			s1 = jisx0213_u5_jis_tbl[k];
		}
	}

	if (s1 <= 0) {
		/* CJK Compatibility Forms: U+FE30 - U+FE4F */
		if (c == 0xfe45) {
			s1 = 0x233e;
		} else if (c == 0xfe46) {
			s1 = 0x233d;
		} else if (c >= 0xf91d && c <= 0xf9dc) {
			/* CJK Compatibility Ideographs: U+F900 - U+F92A */
			k = mbfl_bisec_srch2(c, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
			if (k >= 0) {
				s1 = ucs_r2b_jisx0213_cmap_val[k];
			}
		}
	}

	if (s1 <= 0) {
		if (c == 0) {
			s1 = 0;
		} else {
			s1 = -1;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* ASCII */
			if (filter->to->no_encoding == mbfl_no_encoding_2022jp_2004 && (filter->status & 0xff00)) {
				CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('B', filter->data));
			}
			filter->status = 0;
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x100) { /* latin or kana */
			if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				CK((*filter->output_function)(0x8e, filter->data));
				CK((*filter->output_function)(s1, filter->data));
			} else if (filter->to->no_encoding == mbfl_no_encoding_sjis2004 && (s1 >= 0xA1 && s1 <= 0xDF)) {
				CK((*filter->output_function)(s1, filter->data));
			} else {
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		} else if (s1 < 0x7f00) { /* X 0213 plane 1 */
			if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
			} else if  (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				s2 = (s1 & 0xff) + 0x80;
				s1 = ((s1 >> 8) & 0xff) + 0x80;
			} else {
				if ((filter->status & 0xff00) != 0x200) {
					CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
					CK((*filter->output_function)('$', filter->data));
					CK((*filter->output_function)('(', filter->data));
					CK((*filter->output_function)('Q', filter->data));
				}
				filter->status = 0x200;
				s2 = s1 & 0xff;
				s1 = (s1 >> 8) & 0xff;
			}
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		} else { /* X 0213 plane 2 */
			if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
			} else {
				s2 = s1 & 0xff;
				k = ((s1 >> 8) & 0xff) - 0x7f;
				if (k >= 0 && k < jisx0213_p2_ofst_len) {
					s1 = jisx0213_p2_ofst[k] + 0x21;
				}
				if  (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
					s2 |= 0x80;
					s1 |= 0x80;
					CK((*filter->output_function)(0x8f, filter->data));
				} else {
					if ((filter->status & 0xff00) != 0x200) {
						CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
						CK((*filter->output_function)('$', filter->data));
						CK((*filter->output_function)('(', filter->data));
						CK((*filter->output_function)('P', filter->data));
					}
					filter->status = 0x200;
				}
			}

			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_wchar_jis2004_flush(mbfl_convert_filter *filter)
{
	int k, c1, c2, s1, s2;

	k = filter->cache;
	filter->cache = 0;

	if (filter->status == 1 && k >= 0 && k <= jisx0213_u2_tbl_len) {
		s1 = jisx0213_u2_fb_tbl[k];

		if (filter->to->no_encoding == mbfl_no_encoding_sjis2004) {
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
		} else if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
			s2 = (s1 & 0xff) | 0x80;
			s1 = ((s1 >> 8) & 0xff) | 0x80;
		} else {
			s2 = s1 & 0x7f;
			s1 = (s1 >> 8) & 0x7f;
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
				CK((*filter->output_function)('$', filter->data));
				CK((*filter->output_function)('(', filter->data));
				CK((*filter->output_function)('Q', filter->data));
			}
			filter->status = 0x200;
		}

		CK((*filter->output_function)(s1, filter->data));
		CK((*filter->output_function)(s2, filter->data));
	}

	/* If we had switched to a different charset, go back to ASCII mode
	 * This makes it possible to concatenate arbitrary valid strings
	 * together and get a valid string */
	if (filter->status & 0xff00) {
		CK((*filter->output_function)(0x1b, filter->data)); /* ESC */
		CK((*filter->output_function)('(', filter->data));
		CK((*filter->output_function)('B', filter->data));
	}

	filter->status = 0;

	if (filter->flush_function) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_sjis2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			if (c == 0x5C) {
				*out++ = 0xA5;
			} else if (c == 0x7E) {
				*out++ = 0x203E;
			} else {
				*out++ = c;
			}
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c < 0xFD && c != 0xA0) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 < 0x40 || c2 > 0xFC || c2 == 0x7F) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			unsigned int s1, s2;
			SJIS_DECODE(c, c2, s1, s2);
			unsigned int w1 = (s1 << 8) | s2, w = 0;

			/* Conversion for combining characters */
			if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) || (w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 || (w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
				int k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
				if (k >= 0) {
					*out++ = jisx0213_u2_tbl[2*k];
					*out++ = jisx0213_u2_tbl[2*k+1];
					continue;
				}
			}

			/* Conversion for BMP */
			w1 = (s1 - 0x21)*94 + s2 - 0x21;
			if (w1 < jisx0213_ucs_table_size) {
				w = jisx0213_ucs_table[w1];
			}

			/* Conversion for CJK Unified Ideographs extension B (U+2XXXX) */
			if (!w) {
				w1 = (s1 << 8) | s2;
				int k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
				if (k >= 0) {
					w = jisx0213_jis_u5_tbl[k] + 0x20000;
				}
			}

			*out++ = w ? w : MBFL_BAD_INPUT;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	 *in_len = e - p;
	 *in = p;
	 return out - buf;
}

static void mb_wchar_to_sjis2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;
	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		goto process_codepoint;
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		if (w == 0xE6 || (w >= 0x254 && w <= 0x2E9) || (w >= 0x304B && w <= 0x3053) || (w >= 0x30AB && w <= 0x30C8) || w == 0x31F7) {
			for (int k = 0; k < jisx0213_u2_tbl_len; k++) {
				if (w == jisx0213_u2_tbl[2*k]) {
					if (!len) {
						if (!end) {
							buf->state = w;
							MB_CONVERT_BUF_STORE(buf, out, limit);
							return;
						}
					} else {
						uint32_t w2 = *in++; len--;
						if ((w == 0x254 || w == 0x28C || w == 0x259 || w == 0x25A) && w2 == 0x301) {
							k++;
						}
						if (w2 == jisx0213_u2_tbl[2*k+1]) {
							s = jisx0213_u2_key[k];
							break;
						}
						in--; len++;
					}

					/* Fallback */
					s = jisx0213_u2_fb_tbl[k];
					break;
				}
			}
		}

		/* Check for major Japanese chars: U+4E00-U+9FFF */
		if (!s) {
			for (int k = 0; k < uni2jis_tbl_len; k++) {
				if (w >= uni2jis_tbl_range[k][0] && w <= uni2jis_tbl_range[k][1]) {
					s = uni2jis_tbl[k][w - uni2jis_tbl_range[k][0]];
					break;
				}
			}
		}

		/* Check for Japanese chars in compressed mapping area: U+1E00-U+4DBF */
		if (!s && w >= ucs_c1_jisx0213_min && w <= ucs_c1_jisx0213_max) {
			int k = mbfl_bisec_srch(w, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
			if (k >= 0) {
				s = ucs_c1_jisx0213_ofst[k] + w - ucs_c1_jisx0213_tbl[2*k];
			}
		}

		/* Check for Japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
		if (!s && w >= jisx0213_u5_tbl_min && w <= jisx0213_u5_tbl_max) {
			int k = mbfl_bisec_srch2(w - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				s = jisx0213_u5_jis_tbl[k];
			}
		}

		if (!s) {
			/* CJK Compatibility Forms: U+FE30-U+FE4F */
			if (w == 0xFE45) {
				s = 0x233E;
			} else if (w == 0xFE46) {
				s = 0x233D;
			} else if (w >= 0xF91D && w <= 0xF9DC) {
				/* CJK Compatibility Ideographs: U+F900-U+F92A */
				int k = mbfl_bisec_srch2(w, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
				if (k >= 0) {
					s = ucs_r2b_jisx0213_cmap_val[k];
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_eucjp2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xFE) {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			if (c2 <= 0xA0 || c2 == 0xFF) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			unsigned int s1 = c - 0x80, s2 = c2 - 0x80;
			unsigned int w1 = (s1 << 8) | s2, w = 0;

			/* Conversion for combining characters */
			if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) || (w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 || (w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
				int k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
				if (k >= 0) {
					*out++ = jisx0213_u2_tbl[2*k];
					*out++ = jisx0213_u2_tbl[2*k+1];
					continue;
				}
			}

			/* Conversion for BMP  */
			w1 = (s1 - 0x21)*94 + s2 - 0x21;
			if (w1 < jisx0213_ucs_table_size) {
				w = jisx0213_ucs_table[w1];
			}

			/* Conversion for CJK Unified Ideographs ext.B (U+2XXXX) */
			if (!w) {
				w1 = (s1 << 8) | s2;
				int k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
				if (k >= 0) {
					w = jisx0213_jis_u5_tbl[k] + 0x20000;
				}
			}

			*out++ = w ? w : MBFL_BAD_INPUT;
		} else if (c == 0x8E && p < e) {
			/* Kana */
			unsigned char c2 = *p++;
			if (c2 >= 0xA1 && c2 <= 0xDF) {
				*out++ = 0xFEC0 + c2;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x8F && p < e) {
			unsigned char c2 = *p++;
			if ((c2 == 0xA1 || (c2 >= 0xA3 && c2 <= 0xA5) || c2 == 0xA8 || (c2 >= 0xAC && c2 <= 0xAF) || (c2 >= 0xEE && c2 <= 0xFE)) && p < e) {
				unsigned char c3 = *p++;

				if (c3 < 0xA1 || c3 == 0xFF) {
					*out++ = MBFL_BAD_INPUT;
					continue;
				}

				unsigned int s1 = c2 - 0xA1, s2 = c3 - 0xA1;

				if (((s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) || (s1 >= 77 && s1 < 94)) && s2 < 94) {
					int k;
					for (k = 0; k < jisx0213_p2_ofst_len; k++) {
						if (s1 == jisx0213_p2_ofst[k]) {
							break;
						}
					}
					k -= jisx0213_p2_ofst[k];

					/* Check for Japanese chars in BMP */
					unsigned int s = (s1 + 94 + k)*94 + s2;
					ZEND_ASSERT(s < jisx0213_ucs_table_size);
					unsigned int w = jisx0213_ucs_table[s];

					/* Check for Japanese chars in CJK Unified Ideographs ext B (U+2XXXX) */
					if (!w) {
						k = mbfl_bisec_srch2(((c2 - 0x80 + k + 94) << 8) | (c3 - 0x80), jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
						if (k >= 0) {
							w = jisx0213_jis_u5_tbl[k] + 0x20000;
						}
					}

					*out++ = w ? w : MBFL_BAD_INPUT;
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
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

static void mb_wchar_to_eucjp2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;
	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		goto process_codepoint;
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		/* Check for 1st char of combining characters */
		if (w == 0xE6 || (w >= 0x254 && w <= 0x2E9) || (w >= 0x304B && w <= 0x3053) || (w >= 0x30AB && w <= 0x30C8) || w == 0x31F7) {
			for (int k = 0; k < jisx0213_u2_tbl_len; k++) {
				if (w == jisx0213_u2_tbl[2*k]) {
					if (!len) {
						if (!end) {
							buf->state = w;
							MB_CONVERT_BUF_STORE(buf, out, limit);
							return;
						}
					} else {
						uint32_t w2 = *in++; len--;
						if ((w == 0x254 || w == 0x28C || w == 0x259 || w == 0x25A) && w2 == 0x301) {
							k++;
						}
						if (w2 == jisx0213_u2_tbl[2*k+1]) {
							s = jisx0213_u2_key[k];
							break;
						}
						in--; len++;
					}

					/* Fallback */
					s = jisx0213_u2_fb_tbl[k];
					break;
				}
			}
		}

		/* Check for major Japanese chars: U+4E00-U+9FFF */
		if (!s) {
			for (int k = 0; k < uni2jis_tbl_len; k++) {
				if (w >= uni2jis_tbl_range[k][0] && w <= uni2jis_tbl_range[k][1]) {
					s = uni2jis_tbl[k][w - uni2jis_tbl_range[k][0]];
					break;
				}
			}
		}

		/* Check for Japanese chars in compressed mapping area: U+1E00-U+4DBF */
		if (!s && w >= ucs_c1_jisx0213_min && w <= ucs_c1_jisx0213_max) {
			int k = mbfl_bisec_srch(w, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
			if (k >= 0) {
				s = ucs_c1_jisx0213_ofst[k] + w - ucs_c1_jisx0213_tbl[2*k];
			}
		}

		/* Check for Japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
		if (!s && w >= jisx0213_u5_tbl_min && w <= jisx0213_u5_tbl_max) {
			int k = mbfl_bisec_srch2(w - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				s = jisx0213_u5_jis_tbl[k];
			}
		}

		if (!s) {
			/* CJK Compatibility Forms: U+FE30-U+FE4F */
			if (w == 0xFE45) {
				s = 0x233E;
			} else if (w == 0xFE46) {
				s = 0x233D;
			} else if (w >= 0xF91D && w <= 0xF9DC) {
				/* CJK Compatibility Ideographs: U+F900-U+F92A */
				int k = mbfl_bisec_srch2(w, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
				if (k >= 0) {
					s = ucs_r2b_jisx0213_cmap_val[k];
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_eucjp2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			out = mb_convert_buf_add(out, s);
		} else if (s <= 0xFF) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, 0x8E, s);
		} else if (s <= 0x7EFF) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, ((s >> 8) & 0xFF) + 0x80, (s & 0xFF) + 0x80);
		} else {
			unsigned int s2 = s & 0xFF;
			int k = ((s >> 8) & 0xFF) - 0x7F;
			ZEND_ASSERT(k < jisx0213_p2_ofst_len);
			s = jisx0213_p2_ofst[k] + 0x21;
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
			out = mb_convert_buf_add3(out, 0x8F, s | 0x80, s2 | 0x80);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

#define ASCII 0
#define JISX0208 1
#define JISX0213_PLANE1 2
#define JISX0213_PLANE2 3

static size_t mb_iso2022jp2004_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			if (c == 0x1B) {
				if ((e - p) < 2) {
					*out++ = MBFL_BAD_INPUT;
					p = e;
					break;
				}
				unsigned char c2 = *p++;
				unsigned char c3 = *p++;
				if (c2 == '$') {
					if (c3 == 'B') {
						*state = JISX0208;
					} else if (c3 == '(') {
						if (p == e) {
							*out++ = MBFL_BAD_INPUT;
							break;
						}
						unsigned char c4 = *p++;
						if (c4 == 'Q') {
							*state = JISX0213_PLANE1;
						} else if (c4 == 'P') {
							*state = JISX0213_PLANE2;
						} else {
							*out++ = MBFL_BAD_INPUT;
						}
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else if (c2 == '(') {
					if (c3 == 'B') {
						*state = ASCII;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					p--;
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (*state >= JISX0208 && c > 0x20 && c < 0x7F) {
				if (p == e) {
					*out++ = MBFL_BAD_INPUT;
					break;
				}
				unsigned char c2 = *p++;
				if (c2 < 0x21 || c2 > 0x7E) {
					*out++ = MBFL_BAD_INPUT;
					continue;
				}

				if (*state == JISX0213_PLANE1) {
					unsigned int w1 = (c << 8) | c2;

					/* Conversion for combining characters */
					if ((w1 >= 0x2477 && w1 <= 0x2479) || (w1 >= 0x2479 && w1 <= 0x247B) || (w1 >= 0x2577 && w1 <= 0x257E) || w1 == 0x2678 || w1 == 0x2B44 || (w1 >= 0x2B48 && w1 <= 0x2B4F) || (w1 >= 0x2B65 && w1 <= 0x2B66)) {
						int k = mbfl_bisec_srch2(w1, jisx0213_u2_key, jisx0213_u2_tbl_len);
						if (k >= 0) {
							*out++ = jisx0213_u2_tbl[2*k];
							*out++ = jisx0213_u2_tbl[2*k+1];
							continue;
						}
					}

					/* Conversion for BMP */
					uint32_t w = 0;
					w1 = (c - 0x21)*94 + c2 - 0x21;
					if (w1 < jisx0213_ucs_table_size) {
						w = jisx0213_ucs_table[w1];
					}

					/* Conversion for CJK Unified Ideographs ext.B (U+2XXXX) */
					if (!w) {
						int k = mbfl_bisec_srch2((c << 8) | c2, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
						if (k >= 0) {
							w = jisx0213_jis_u5_tbl[k] + 0x20000;
						}
					}

					*out++ = w ? w : MBFL_BAD_INPUT;
				} else if (*state == JISX0213_PLANE2) {

					unsigned int s1 = c - 0x21, s2 = c2 - 0x21;

					if (((s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) || (s1 >= 77 && s1 < 94)) && s2 < 94) {
						int k;
						for (k = 0; k < jisx0213_p2_ofst_len; k++) {
							if (s1 == jisx0213_p2_ofst[k]) {
								break;
							}
						}
						k -= jisx0213_p2_ofst[k];

						/* Check for Japanese chars in BMP */
						unsigned int s = (s1 + 94 + k)*94 + s2;
						ZEND_ASSERT(s < jisx0213_ucs_table_size);
						uint32_t w = jisx0213_ucs_table[s];

						/* Check for Japanese chars in CJK Unified Ideographs ext B (U+2XXXX) */
						if (!w) {
							k = mbfl_bisec_srch2(((c + k + 94) << 8) | c2, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
							if (k >= 0) {
								w = jisx0213_jis_u5_tbl[k] + 0x20000;
							}
						}

						*out++ = w ? w : MBFL_BAD_INPUT;
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else { /* state == JISX0208 */
					unsigned int s = (c - 0x21)*94 + c2 - 0x21;
					uint32_t w = 0;
					if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					}
					*out++ = w ? w : MBFL_BAD_INPUT;
				}
			} else {
				*out++ = c;
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jp2004(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;
	if (buf->state & 0xFF00) {
		int k = (buf->state >> 8) - 1;
		w = jisx0213_u2_tbl[2*k];
		buf->state &= 0xFF;
		goto process_codepoint;
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		if (w == 0xE6 || (w >= 0x254 && w <= 0x2E9) || (w >= 0x304B && w <= 0x3053) || (w >= 0x30AB && w <= 0x30C8) || w == 0x31F7) {
			for (int k = 0; k < jisx0213_u2_tbl_len; k++) {
				if (w == jisx0213_u2_tbl[2*k]) {
					if (!len) {
						if (!end) {
							buf->state |= (k+1) << 8;
							MB_CONVERT_BUF_STORE(buf, out, limit);
							return;
						}
					}	else {
						uint32_t w2 = *in++; len--;
						if ((w == 0x254 || w == 0x28C || w == 0x259 || w == 0x25A) && w2 == 0x301) {
							k++;
						}
						if (w2 == jisx0213_u2_tbl[2*k+1]) {
							s = jisx0213_u2_key[k];
							break;
						}
						in--; len++;
					}

					s = jisx0213_u2_fb_tbl[k];
					break;
				}
			}
		}

		/* Check for major Japanese chars: U+4E00-U+9FFF */
		if (!s) {
			for (int k = 0; k < uni2jis_tbl_len; k++) {
				if (w >= uni2jis_tbl_range[k][0] && w <= uni2jis_tbl_range[k][1]) {
					s = uni2jis_tbl[k][w - uni2jis_tbl_range[k][0]];
					break;
				}
			}
		}

		/* Check for Japanese chars in compressed mapping area: U+1E00-U+4DBF */
		if (!s && w >= ucs_c1_jisx0213_min && w <= ucs_c1_jisx0213_max) {
			int k = mbfl_bisec_srch(w, ucs_c1_jisx0213_tbl, ucs_c1_jisx0213_tbl_len);
			if (k >= 0) {
				s = ucs_c1_jisx0213_ofst[k] + w - ucs_c1_jisx0213_tbl[2*k];
			}
		}

		/* Check for Japanese chars in CJK Unified Ideographs ext B (U+2XXXX) */
		if (!s && w >= jisx0213_u5_tbl_min && w <= jisx0213_u5_tbl_max) {
			int k = mbfl_bisec_srch2(w - 0x20000, jisx0213_u5_jis_key, jisx0213_u5_tbl_len);
			if (k >= 0) {
				s = jisx0213_u5_jis_tbl[k];
			}
		}

		if (!s) {
			/* CJK Compatibility Forms: U+FE30-U+FE4F */
			if (w == 0xFE45) {
				s = 0x233E;
			} else if (w == 0xFE46) {
				s = 0x233D;
			} else if (w >= 0xF91D && w <= 0xF9DC) {
				/* CJK Compatibility Ideographs: U+F900-U+F92A */
				int k = mbfl_bisec_srch2(w, ucs_r2b_jisx0213_cmap_key, ucs_r2b_jisx0213_cmap_len);
				if (k >= 0) {
					s = ucs_r2b_jisx0213_cmap_val[k];
				}
			}
		}

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7F) {
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s <= 0xFF) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp2004);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0x7EFF) {
			if (buf->state != JISX0213_PLANE1) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'Q');
				buf->state = JISX0213_PLANE1;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else {
			if (buf->state != JISX0213_PLANE2) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'P');
				buf->state = JISX0213_PLANE2;
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			}
			unsigned int s2 = s & 0xFF;
			int k = ((s >> 8) & 0xFF) - 0x7F;
			ZEND_ASSERT(k < jisx0213_p2_ofst_len);
			s = jisx0213_p2_ofst[k] + 0x21;
			out = mb_convert_buf_add2(out, s, s2);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
