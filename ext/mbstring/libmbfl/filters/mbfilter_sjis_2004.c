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
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_sjis_2004.h"

#include "unicode_table_jis2004.h"
#include "unicode_table_jis.h"

extern const unsigned char mblen_table_sjis[];

static int mbfl_filt_ident_sjis2004(int c, mbfl_identify_filter *filter);

extern int mbfl_filt_ident_sjis(int c, mbfl_identify_filter *filter);
extern int mbfl_bisec_srch(int w, const unsigned short *tbl, int n);
extern int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n);

static const char *mbfl_encoding_sjis2004_aliases[] = {"SJIS2004","Shift_JIS-2004", NULL};

const mbfl_encoding mbfl_encoding_sjis2004 = {
	mbfl_no_encoding_sjis2004,
	"SJIS-2004",
	"Shift_JIS",
	(const char *(*)[])&mbfl_encoding_sjis2004_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const struct mbfl_identify_vtbl vtbl_identify_sjis2004 = {
	mbfl_no_encoding_sjis2004,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjis
};

const struct mbfl_convert_vtbl vtbl_sjis2004_wchar = {
	mbfl_no_encoding_sjis2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis2004,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_jis2004_flush
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


/*
 * JIS-2004 => wchar
 */
int
mbfl_filt_conv_jis2004_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, s, s1 = 0, s2 = 0, w = 0, w1;

retry:
	switch (filter->status & 0xf) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
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
				   && c > 0x20 && c < 0x7f) {		/* kanji first char */
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
				if (c > 0xa0 && c < 0xff) {	/* X 0213 plane 1 first char */
					filter->status = 1;
					filter->cache = c;
				} else if (c == 0x8e) {	/* kana first char */
					filter->status = 2;
				} else if (c == 0x8f) {	/* X 0213 plane 2 first char */
					filter->status = 3;
				} else {
					w = c & MBFL_WCSGROUP_MASK;
					w |= MBFL_WCSGROUP_THROUGH;
					CK((*filter->output_function)(w, filter->data));
				}
			} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
				if (c > 0xa0 && c < 0xe0) {	/* kana */
					CK((*filter->output_function)(0xfec0 + c, filter->data));
				} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
					filter->status = 1;
					filter->cache = c;
				} else {
					w = c & MBFL_WCSGROUP_MASK;
					w |= MBFL_WCSGROUP_THROUGH;
					CK((*filter->output_function)(w, filter->data));
				}
			} else {
				w = c & MBFL_WCSGROUP_MASK;
				w |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(w, filter->data));
			}
		}
		break;

	case 1:		/* kanji second char */
		filter->status &= ~0xf;
		c1 = filter->cache;

		if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
			if (c > 0xa0 && c < 0xff) {
				s1 = c1 - 0x80;
				s2 = c - 0x80;
			}
		} else if (filter->from->no_encoding == mbfl_no_encoding_sjis2004) {
			if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
				SJIS_DECODE(c1, c, s1, s2);
			}
		} else {
			s1 = c1;
			s2 = c;
		}
		w1 = (s1 << 8) | s2;

		if (w1 >= 0x2121) {
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
				if (s1 < 0x7f && s2 < 0x7f) {
					w = (s1 << 8) | s2;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0213;
				} else {
					w = (c1 << 8) | c;
					w &= MBFL_WCSGROUP_MASK;
					w |= MBFL_WCSGROUP_THROUGH;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 2:	/* got 0x8e : EUC-JP-2004 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = 0x8e00 | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 3:	/* X 0213 plane 2 first char : EUC-JP-2004 (0x8f), ISO-2022-JP-2004 */
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
				s1 = c - 0x80;
			} else {
				s1 = c;
			}
			if (s1 > 0x20 && s1 < 0x80) {
				filter->cache = s1;
				filter->status++;
			} else {
				if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
					w = c | 0x8f00;
					w &= MBFL_WCSGROUP_MASK;
					w |= MBFL_WCSGROUP_THROUGH;
				} else {
					w = c & 0x7f;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0213;
				}
				CK((*filter->output_function)(w, filter->data));
			}
		}
		break;

	case 4:	/* X 0213 plane 2 second char : EUC-JP-2004, ISO-2022-JP-2004 */

		filter->status &= ~0xf;
		c1 = filter->cache;
		if (filter->from->no_encoding == mbfl_no_encoding_eucjp2004) {
			c2 = c - 0x80;
		} else {
			c2 = c;
		}
		s1 = c1 - 0x21;
		s2 = c2 - 0x21;

		if (((s1 >= 0 && s1 <= 4 && s1 != 1) || s1 == 7 || (s1 >= 11 && s1 <= 14) ||
			(s1 >= 77 && s1 < 94)) && s2 >= 0 && s2 < 94) {
			/* calc offset from ku */
			for (k = 0; k < jisx0213_p2_ofst_len; k++) {
				if (s1 == jisx0213_p2_ofst[k]-1) {
					break;
				}
			}
			k = k - (jisx0213_p2_ofst[k]-1);

			/* check for japanese chars in BMP */
			s = (s1 + 94 + k)*94 + s2;
			if (s >= 0 && s < jisx0213_ucs_table_size) {
				w = jisx0213_ucs_table[s];
			} else {
				w = 0;
			}

			/* check for japanese chars in CJK Unified Ideographs ext.B (U+2XXXX) */
			if (w <= 0) {
				w1 = ((c1 + k + 94) << 8) | c2;
				k = mbfl_bisec_srch2(w1, jisx0213_jis_u5_key, jisx0213_u5_tbl_len);
				if (k >= 0) {
					w = jisx0213_jis_u5_tbl[k] + 0x20000;
				}
			}

			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c2 & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_JIS0213;
			}

			CK((*filter->output_function)(w, filter->data));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			if (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				w = (c1 << 8) | c | 0x8f0000;
				w &= MBFL_WCSGROUP_MASK;
				w |= MBFL_WCSGROUP_THROUGH;
			} else {
				w = ((c1 & 0x7f) << 8) | (c2 & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_JIS0213;
			}
			CK((*filter->output_function)(w, filter->data));
		}

		break;

	case 5:	/* X 0208 : ISO-2022-JP-2004 */
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (s >= 0 && s < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[s];
			}
		}
		if (w <= 0) {
			w = (c1 << 8) | c;
			w &= MBFL_WCSPLANE_MASK;
			w |= MBFL_WCSPLANE_JIS0208;
		}
		CK((*filter->output_function)(w, filter->data));
		break;

	/* ESC : ISO-2022-JP-2004 */
/*	case 0x06:	*/
/*	case 0x16:	*/
/*	case 0x26:	*/
/*	case 0x86:	*/
/*	case 0x96:	*/
/*	case 0xa6:	*/
	case 6:
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

	/* ESC $ : ISO-2022-JP-2004 */
/*	case 0x07:	*/
/*	case 0x17:	*/
/*	case 0x27:	*/
/*	case 0x87:	*/
/*	case 0x97:	*/
/*	case 0xa7:	*/
	case 7:
		if (c == 0x42) {	/* 'B' -> JIS X 0208-1983 */
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

		break;

	/* ESC $ ( : ISO-2022-JP-2004 */
/*	case 0x08:	*/
/*	case 0x18:	*/
/*	case 0x28:	*/
/*	case 0x88:	*/
/*	case 0x98:	*/
/*	case 0xa8:	*/
	case 8:
		if (c == 0x51) {	/* JIS X 0213 plane 1 */
			filter->status = 0x90;
		} else if (c == 0x50) {			/* JIS X 0213 plane 2 */
			filter->status = 0xa0;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

	/* ESC ( : ISO-2022-JP-2004 */
/*	case 0x09:	*/
/*	case 0x19:	*/
/*	case 0x29:	*/
/*	case 0x89:	*/
/*	case 0x99:	*/
	case 9:
		if (c == 0x42) {		/* 'B' : ASCII */
			filter->status = 0;
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

int
mbfl_filt_conv_wchar_jis2004(int c, mbfl_convert_filter *filter) {
	int k;
	int c1, c2, s1 = 0, s2;

retry:

	/* check for 1st char of combining characters */
	if ((filter->status & 0xf)== 0 && (
			c == 0x00E6 ||
			(c >= 0x0254 && c <= 0x02E9) ||
			(c >= 0x304B && c <= 0x3053) ||
			(c >= 0x30AB && c <= 0x30C8) ||
			c == 0x31F7)) {
		for (k=0;k<jisx0213_u2_tbl_len;k++) {
			if (c == jisx0213_u2_tbl[2*k]) {
				filter->status++;
				filter->cache = k;
				return c;
			}
		}
	}

	/* check for 2nd char of combining characters */
	if ((filter->status & 0xf) == 1 &&
		filter->cache >= 0 && filter->cache <= jisx0213_u2_tbl_len) {
		k = filter->cache;
		filter->status &= ~0xf;
		filter->cache = 0;

		c1 = jisx0213_u2_tbl[2*k];
		if ((c1 == 0x0254 || c1 == 0x028C || c1 == 0x0259 || c1 == 0x025A)
			&& c == 0x0301) {
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
					CK((*filter->output_function)(0x24, filter->data));
					CK((*filter->output_function)(0x28, filter->data));
					CK((*filter->output_function)(0x51, filter->data));
				}
				filter->status = 0x200;

				s2 = s1 & 0x7f;
				s1 = (s1 >> 8) & 0x7f;
			}

			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
			goto retry;
		}
	}

	/* check for major japanese chars: U+4E00 - U+9FFF */
	if (s1 <= 0) {
		for (k=0; k < uni2jis_tbl_len ;k++) {
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
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_JIS0213) {
			s1 = c & MBFL_WCSPLANE_MASK;
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	} else if (s1 >= 0x9980) {
		s1 = -1;
	}

	if (s1 >= 0) {
		if (s1 < 0x80) { /* ASCII */
			if (filter->to->no_encoding == mbfl_no_encoding_2022jp_2004 &&
				(filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s1, filter->data));
		} else if (s1 < 0x100) { /* latin or kana */
			if  (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
				CK((*filter->output_function)(0x8e, filter->data));
			}
			CK((*filter->output_function)(s1, filter->data));
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
					CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
					CK((*filter->output_function)(0x24, filter->data));		/* '$' */
					CK((*filter->output_function)(0x28, filter->data));		/* '(' */
					CK((*filter->output_function)(0x51, filter->data));		/* 'Q' */
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
					s1  = jisx0213_p2_ofst[k] - 1 + 0x21;
				}
				if  (filter->to->no_encoding == mbfl_no_encoding_eucjp2004) {
					s2 |= 0x80;
					s1 |= 0x80;
					CK((*filter->output_function)(0x8f, filter->data));
				} else {
					if ((filter->status & 0xff00) != 0x200) {
						CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
						CK((*filter->output_function)(0x24, filter->data));		/* '$' */
						CK((*filter->output_function)(0x28, filter->data));		/* '(' */
						CK((*filter->output_function)(0x50, filter->data));		/* 'P' */
					}
					filter->status = 0x200;
				}
			}

			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

int
mbfl_filt_conv_jis2004_flush(mbfl_convert_filter *filter)
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
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x51, filter->data));		/* 'Q' */
			}
			filter->status = 0x200;
		}

		CK((*filter->output_function)(s1, filter->data));
		CK((*filter->output_function)(s2, filter->data));
	}

	/* back to latin */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
	}

	filter->status = 0;

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}
