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
 * the source code included in this files was separated from mbfilter_sjis_open.c
 * by Rui Hirokawa <hirokawa@php.net> on 25 July 2011.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_sjis_mac.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

static int mbfl_filt_ident_sjis_mac(int c, mbfl_identify_filter *filter);

static const unsigned char mblen_table_sjis[] = { /* 0x80-0x9f,0xE0-0xFF */
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

static const char *mbfl_encoding_sjis_mac_aliases[] = {"MacJapanese", "x-Mac-Japanese", NULL};

const mbfl_encoding mbfl_encoding_sjis_mac = {
	mbfl_no_encoding_sjis_mac,
	"SJIS-mac",
	"Shift_JIS",
	(const char *(*)[])&mbfl_encoding_sjis_mac_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const struct mbfl_identify_vtbl vtbl_identify_sjis_mac = {
	mbfl_no_encoding_sjis_mac,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjis_mac
};

const struct mbfl_convert_vtbl vtbl_sjis_mac_wchar = {
	mbfl_no_encoding_sjis_mac,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_sjis_mac_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_mac = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_mac,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_sjis_mac,
	mbfl_filt_conv_sjis_mac_flush
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

#include "sjis_mac2uni.h"

const int code_tbl[][3] = {
	{0x02f0, 0x0303, 0x2460},
	{0x030e, 0x0321, 0x2474},
	{0x032c, 0x0334, 0x2776},
	{0x0341, 0x0349, 0x2488},
	{0x034e, 0x0359, 0x2160},
	{0x0362, 0x036d, 0x2170},
	{0x038a, 0x03a3, 0x249c},
};

const int code_ofst_tbl[] [2]= {
	{0x03ac, 0x03c9},
	{0x0406, 0x0420},
	{0x0432, 0x0441},
	{0x0468, 0x0480},
	{0x04b8, 0x04e8},
	{0x050c, 0x0551},
	{0x1ed9, 0x1f18},
	{0x1ff2, 0x20a5},
};

const int *code_map[] = {
	sjis_mac2wchar1, sjis_mac2wchar2, sjis_mac2wchar3, sjis_mac2wchar4,
	sjis_mac2wchar5, sjis_mac2wchar6, sjis_mac2wchar7, sjis_mac2wchar8};

const int code_tbl_m[][6] = {
	{0x0340, 0xf860, 0x0030, 0x002e, 0x0000, 0x0000},
	{0x03c9, 0xf860, 0x0054, 0x0042, 0x0000, 0x0000},
	{0x035c, 0xf860, 0x0058, 0x0056, 0x0000, 0x0000},
	{0x0370, 0xf860, 0x0078, 0x0076, 0x0000, 0x0000},
	{0x0439, 0xf860, 0x2193, 0x2191, 0x0000, 0x0000},
	{0x0409, 0xf861, 0x0046, 0x0041, 0x0058, 0x0000},
	{0x035b, 0xf861, 0x0058, 0x0049, 0x0056, 0x0000},
	{0x036f, 0xf861, 0x0078, 0x0069, 0x0076, 0x0000},
	{0x035a, 0xf862, 0x0058, 0x0049, 0x0049, 0x0049},
	{0x036e, 0xf862, 0x0078, 0x0069, 0x0069, 0x0069},
	{0x0522, 0xf862, 0x6709, 0x9650, 0x4f1a, 0x793e},
	{0x0523, 0xf862, 0x8ca1, 0x56e3, 0x6cd5, 0x4eba},
};

const int s_form_tbl[] = {
	0x2010,0x2016,0x2026,
	0x3001,0x3002,0x301c,0x3041,0x3043,0x3045,0x3047,0x3049,
	0x3063,0x3083,0x3085,0x3087,0x308e,0x30a1,0x30a3,0x30a5,
	0x30a7,0x30a9,0x30c3,0x30e3,0x30e5,0x30e7,0x30ee,0x30f5,
	0x30f6,0x30fc,0xff1d,0xff3b,0xff3d,0xff5c,0xffe3, // vertical f87e (34)
	0x2026,0xff47,0xff4d, // halfwidth f87f (3)
	0x5927,0x5c0f,0x63a7, // enclosing circle 20dd (3)
	0x21e6,0x21e7,0x21e8,0x21e9, // black arrow f87a (4)
};

const int s_form_sjis_tbl[] = {
	0xeb5d,0xeb61,0xeb63,
	0xeb41,0xeb42,0xeb60,0xec9f,0xeca1,0xeca3,0xeca5,0xeca7,
	0xecc1,0xece1,0xece3,0xece5,0xecec,0xed40,0xed42,0xed44,
	0xed46,0xed48,0xed62,0xed83,0xed85,0xed87,0xed8e,0xed95,
	0xed96,0xeb5b,0xeb81,0xeb6d,0xeb6e,0xeb62,0xeb50, // vertical
	0x00ff,0x864b,0x8645, // halfwidth
	0x8791,0x8792,0x879d, // enclosing circle
	0x86d4,0x86d5,0x86d3,0x86d6, // black arrow
};

const int s_form_sjis_fallback_tbl[] = {
	0x815d,0x8161,0x8163,
	0x8141,0x8142,0x8160,0x829f,0x82a1,0x82a3,0x82a5,0x82a7,
	0x82c1,0x82e1,0x82e3,0x82e5,0x82ec,0x8340,0x8342,0x8344,
	0x8346,0x8348,0x8362,0x8383,0x8385,0x8387,0x838e,0x8395,
	0x8396,0x815b,0x8181,0x816d,0x816e,0x8162,0x8150, // vertical
	0x815d,0x8287,0x828d, // halfwidth
	0x91e5,0x8fac,0x8d54, // enclosing circle
	0x86d0,0x86d1,0x86cf,0x86d2, // arrow
};

const int wchar2sjis_mac_r_tbl[][3] = {
	{0x2160, 0x216b, 0x034e},
	{0x2170, 0x217b, 0x0362},
	{0x2460, 0x2473, 0x02f0},
	{0x2474, 0x2487, 0x030e},
	{0x2488, 0x2490, 0x0341},
	{0x249c, 0x24b5, 0x038a},
	{0x2776, 0x277e, 0x032c},
	{0x30f7, 0x30fa, 0x054e},
	{0x32a4, 0x32a9, 0x04ba},
};

const int wchar2sjis_mac_r_map[][2] = {
	{0x2660, 0x2667},
	{0x322a, 0x3243},
	{0x3296, 0x329e},
	{0x3300, 0x33d4},
	{0xfe30, 0xfe44},
};

const int *wchar2sjis_mac_code_map[] = {
	wchar2sjis_mac4, wchar2sjis_mac7, wchar2sjis_mac8, wchar2sjis_mac9, wchar2sjis_mac10};

const int wchar2sjis_mac_wchar_tbl[][2] = {
	{0x2109, 0x03c2},
	{0x2110, 0x21ef5},
	{0x2113, 0x03bc},
	{0x2116, 0x0406},
	{0x2121, 0x0408},
	{0x21c4, 0x0437},
	{0x21c5, 0x0438},
	{0x21c6, 0x0436},
	{0x21e6, 0x043b},
	{0x21e7, 0x043c},
	{0x21e8, 0x043a},
	{0x21e9, 0x043d},
	{0x221f, 0x0525},
	{0x222e, 0x0524},
	{0x22bf, 0x0526},
	{0x260e, 0x041f},
	{0x261c, 0x0433},
	{0x261d, 0x0434},
	{0x261e, 0x0432},
	{0x261f, 0x0435},
	{0x3004, 0x0420},
	{0x301d, 0x0538},
	{0x301f, 0x0539},
	{0x3020, 0x041e},
	{0x3094, 0x054c},
};



/*
 * SJIS-mac => wchar
 */
int
mbfl_filt_conv_sjis_mac_wchar(int c, mbfl_convert_filter *filter)
{
	int i, j, n;
	int c1, s, s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80 && c != 0x5c) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x5c) {
			CK((*filter->output_function)(0x00a5, filter->data));			
		} else if (c == 0x80) {
			CK((*filter->output_function)(0x005c, filter->data));			
		} else if (c == 0xa0) {
			CK((*filter->output_function)(0x00a0, filter->data));			
		} else if (c == 0xfd) {
			CK((*filter->output_function)(0x00a9, filter->data));			
		} else if (c == 0xfe) {
			CK((*filter->output_function)(0x2122, filter->data));			
		} else if (c == 0xff) {
			CK((*filter->output_function)(0x2026, filter->data));
			CK((*filter->output_function)(0xf87f, filter->data));
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 1:		/* kanji second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = (s1 - 0x21)*94 + s2 - 0x21;
			if (s <= 0x89) {
				if (s == 0x1c) {
					w = 0x2014;		    /* EM DASH */	
				} else if (s == 0x1f) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 0x20) {
					w = 0x301c;			/* FULLWIDTH TILDE */
				} else if (s == 0x21) {
					w = 0x2016;			/* PARALLEL TO */
				} else if (s == 0x3c) {
					w = 0x2212;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 0x50) {
					w = 0x00a2;			/* FULLWIDTH CENT SIGN */
				} else if (s == 0x51) {
					w = 0x00a3;			/* FULLWIDTH POUND SIGN */
				} else if (s == 0x89) {
					w = 0x00ac;			/* FULLWIDTH NOT SIGN */
				}
			}

			/* apple gaiji area 0x8540 - 0x886d */
			if (w == 0) {
				for (i=0; i<7; i++) {
					if (s >= code_tbl[i][0] && s <= code_tbl[i][1]) {
						w = s - code_tbl[i][0] + code_tbl[i][2];
						break;
					} 
				}
			}

			if (w == 0) {

				for (i=0; i<12; i++) {
					if (s == code_tbl_m[i][0]) {
						if (code_tbl_m[i][1] == 0xf860) {
							n = 4;
						} else if (code_tbl_m[i][1] == 0xf861) {
							n = 5;
						} else {
							n = 6;
						}
						for (j=1; j<n-1; j++) {
							CK((*filter->output_function)(code_tbl_m[i][j], filter->data));							
						}
						w = code_tbl_m[i][n-1];
						break;
					}
				}
			}

			if (w == 0) {
				for (i=0; i<8; i++) {
					if (s >= code_ofst_tbl[i][0] && s <= code_ofst_tbl[i][1]) {
						w = code_map[i][s - code_ofst_tbl[i][0]];
						if (w >= 0x10000) {
							CK((*filter->output_function)(w & 0xffff, filter->data));
							if (w & 0x10000) {
								w = 0xf87a;
							} else if (w & 0x20000) {
								w = 0xf87e;
							} else if (w & 0x40000) {
								w = 0xf87f;
							} else if (w & 0x80000) {
								w = 0x20dd;
							} else if (w & 0xF0000) {
								// TBD
								w = 0;
							} else {
								w = 0;
							}
						}
						
						break;
					}
				}
			}
			
			if (w == 0 && s >= 0 && s < jisx0208_ucs_table_size) {	/* X 0208 */
				w = jisx0208_ucs_table[s];
			}
			
			if (w <= 0) {
				w = (s1 << 8) | s2;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_WINCP932;
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

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => SJIS-mac
 */
int
mbfl_filt_conv_wchar_sjis_mac(int c, mbfl_convert_filter *filter)
{
	int i;
	int c1, c2, s1, s2, mode;

	s1 = 0;
	s2 = 0;

	// a1: U+0000 -> U+046F
	// a2: U+2000 -> U+30FF
	//  i: U+4E00 -> U+9FFF
	//  r: U+FF00 -> U+FFFF

	switch (filter->status) {

	case 1:
		c1 = filter->cache;
		filter->cache = 0;
		filter->status = 0;

		s1 = 0;
		s2 = 0;

		if (c == 0xf87a) {
			for (i=0;i<4;i++) {
				if (c1 == s_form_tbl[i+34+3+3]) {
					s1 = s_form_sjis_tbl[i+34+3+3];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
			}
		} else if (c == 0x20dd) {
			for (i=0;i<3;i++) {
				if (c1 == s_form_tbl[i+34+3]) {
					s1 = s_form_sjis_tbl[i+34+3];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
			}
		} else if (c == 0xf87f) {
			for (i=0;i<3;i++) {
				if (c1 == s_form_tbl[i+34]) {
					s1 = s_form_sjis_tbl[i+34];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1; s1 = -1;
			}
		} else if (c == 0xf87e) {
			for (i=0;i<34;i++) {
				if (c1 == s_form_tbl[i]) {
					s1 = s_form_sjis_tbl[i];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1; s1 = -1;
			}
		} else {
			s2 = c1;
			s1 = c;
		}

		if (s2 > 0) {
			for (i=0;i<sizeof(s_form_tbl)/sizeof(int);i++) {
				if (c1 == s_form_tbl[i]) {
					s1 = s_form_sjis_fallback_tbl[i];
					break;
				}
			}
		}

		if (s1 >= 0) {
			if (s1 < 0x100) {
				CK((*filter->output_function)(s1, filter->data));
			} else {
				CK((*filter->output_function)((s1 >> 8) & 0xff, filter->data));
				CK((*filter->output_function)(s1 & 0xff, filter->data));
			}
		} else {
			if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}			
		}

		if (s2 <= 0 || s1 == -1) {
			break;
		}

	case 0:

		if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
			s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
			if (c == 0x5c) {
				s1 = 0x80;
			} else if (c == 0xa9) {
				s1 = 0xfd;
			}
		} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
			s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
			if (c == 0x2122) {
				s1 = 0xfe;
			} else if (c == 0x2014) {
				s1 = 0x213d;
			} else if (c == 0x2116) {
				s1 = 0x2c1d;
			}
		} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
			s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
		} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
			s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
		}
		
		if (c >= 0x2000) {
			for (i=0;i<sizeof(s_form_tbl)/sizeof(int);i++) {
				if (c == s_form_tbl[i]) {
					filter->status = 1;
					filter->cache = c;
					return c;
				}
			}

			if (c == 0xf860 || c == 0xf861 || c == 0xf862) {
				filter->status = 2;
				filter->cache = c;
				return c;				
			}
		}

		if (s1 <= 0) {
			c1 = c & ~MBFL_WCSPLANE_MASK;
			if (c1 == MBFL_WCSPLANE_WINCP932) {
				s1 = c & MBFL_WCSPLANE_MASK;
				s2 = 1;
			} else if (c1 == MBFL_WCSPLANE_JIS0208) {
				s1 = c & MBFL_WCSPLANE_MASK;
			} else if (c1 == MBFL_WCSPLANE_JIS0212) {
				s1 = c & MBFL_WCSPLANE_MASK;
				s1 |= 0x8080;
			} else if (c == 0xa0) {
				s1 = 0x00a0;
			} else if (c == 0xa5) {		/* YEN SIGN */
				s1 = 0x216f;	/* FULLWIDTH YEN SIGN */
			} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
				s1 = 0x2140;
			}
		}
		
		if (s1 <= 0) {
			for (i=0; i<sizeof(wchar2sjis_mac_r_tbl)/(3*sizeof(int));i++) {
				if (c >= wchar2sjis_mac_r_tbl[i][0] && c <= wchar2sjis_mac_r_tbl[i][1]) {
					s1 = c - wchar2sjis_mac_r_tbl[i][0] + wchar2sjis_mac_r_tbl[i][2];
					break;
				}
			}

			if (s1 <= 0) {
				for (i=0; i<sizeof(wchar2sjis_mac_r_map)/(2*sizeof(int));i++) {
					if (c >= wchar2sjis_mac_r_map[i][0] && c <= wchar2sjis_mac_r_map[i][1]) {
						s1 = wchar2sjis_mac_code_map[i][c-wchar2sjis_mac_r_map[i][0]]; 
						break;
					}					
				}
			}

			if (s1 <= 0) {
				for (i=0; i<sizeof(wchar2sjis_mac_wchar_tbl)/(2*sizeof(int));i++) {
					if ( c == wchar2sjis_mac_wchar_tbl[i][0]) {
						s1 = wchar2sjis_mac_wchar_tbl[i][1] & 0xffff;
						break;
					}
				}
			}
			
			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				s1 = (c1 << 8) | c2;
				s2 = 1;
			}
		}
		
		if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
			s1 = -1;
			c1 = 0;
			
			if (c == 0) {
				s1 = 0;
			} else if (s1 <= 0) {
				s1 = -1;
			}
		}
		
		if (s1 >= 0) {
			if (s1 < 0x100) { /* latin or kana */
				CK((*filter->output_function)(s1, filter->data));
			} else { /* kanji */
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			}
		} else {
			if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		}
		break;


	case 2:
		c1 = filter->cache; 
		filter->cache = 0;
		filter->status = 0;
		if (c1 == 0xf860) {
			for (i=0; i<5; i++) {
				if (c == code_tbl_m[i][2]) {
					filter->cache = c | 0x10000;
					filter->status = 3;
					break;
				}
			}
		} else if (c1 == 0xf861) {
			for (i=0; i<3; i++) {
				if (c == code_tbl_m[i+5][2]) {
					filter->cache = c | 0x20000;
					filter->status = 3;
					break;
				}
			}
		} else if (c1 == 0xf862) {
			for (i=0; i<4; i++) {
				if (c == code_tbl_m[i+5+3][2]) {
					filter->cache = c | 0x40000;
					filter->status = 3;
					break;
				}
			}
		}

		if (filter->status == 0 && filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c1, filter));
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
		
		break;

	case 3:
		s1 = 0;
		c1 = filter->cache & 0xffff; 
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = 0;
		filter->status = 0;

		if (mode == 0x1) {
			for (i=0; i<5; i++) {
				if (c1 == code_tbl_m[i][2] && c == code_tbl_m[i][3]) {
					s1 = code_tbl_m[i][0];
					break;
				}
			}

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			}

			if (s1 <= 0 && filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(0xf860, filter));
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}			

		} else if (mode == 0x2) {
			for (i=0; i<3; i++) {
				if (c1 == code_tbl_m[i+5][2] && c == code_tbl_m[i+5][3]) {
					filter->cache = c | 0x20000;
					filter->status = 4;
					break;
				}
			}
		} else if (mode == 0x4) {
			for (i=0; i<4; i++) {
				if (c1 == code_tbl_m[i+8][2] && c == code_tbl_m[i+8][3]) {
					filter->cache = c | 0x40000;
					filter->status = 4;
					break;
				}
			}
		}
		break;

	case 4:
		s1 = 0;
		c1 = filter->cache & 0xffff; 
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = 0;
		filter->status = 0;

		if (mode == 0x2) {
			for (i=0; i<3; i++) {
				if (c1 == code_tbl_m[i+5][3] && c == code_tbl_m[i+5][4]) {
					s1 = code_tbl_m[i+5][0];
					break;
				}
			}			

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			}

			if (s1 <= 0 && filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(0xf861, filter));
				for (i=0; i<3; i++) {
					if (c1 == code_tbl_m[i+5][3]) {
						CK(mbfl_filt_conv_illegal_output(code_tbl_m[i+5][2], filter));
						break;
					}
				}
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}			
		} else if (mode == 0x4) {
			for (i=0; i<4; i++) {
				if (c1 == code_tbl_m[i+8][3] && c == code_tbl_m[i+8][4]) {
					filter->cache = c | 0x40000;
					filter->status = 5;
					break;
				}
			}		
		}
		break;

	case 5:
		s1 = 0;
		c1 = filter->cache & 0xffff; 
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = 0;
		filter->status = 0;

		if (mode == 0x4) {
			for (i=0; i<4; i++) {
				if (c1 == code_tbl_m[i+8][4] && c == code_tbl_m[i+8][5]) {
					s1 = code_tbl_m[i+8][0];
					break;
				}
			}	

			if (s1 > 0) {
				c1 = s1/94+0x21;
				c2 = s1-94*(c1-0x21)+0x21;
				SJIS_ENCODE(c1, c2, s1, s2);
				CK((*filter->output_function)(s1, filter->data));
				CK((*filter->output_function)(s2, filter->data));
			}

			if (s1 <= 0 && filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
				CK(mbfl_filt_conv_illegal_output(0xf862, filter));
				for (i=0; i<4; i++) {
					if (c1 == code_tbl_m[i+8][4]) {
						CK(mbfl_filt_conv_illegal_output( code_tbl_m[i+8][2], filter));
						CK(mbfl_filt_conv_illegal_output( code_tbl_m[i+8][3], filter));
						break;
					}
				}
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		}
		break;

	default:
		filter->status = 0;
		break;
	}
	return c;
}

int
mbfl_filt_conv_sjis_mac_flush(mbfl_convert_filter *filter)
{
	int i, c1, s1 = 0;
	if (filter->status == 1 && filter->cache > 0) {
		c1 = filter->cache;
		for (i=0;i<sizeof(s_form_tbl)/sizeof(int);i++) {
			if (c1 == s_form_tbl[i]) {
				s1 = s_form_sjis_fallback_tbl[i];
				break;
			}
		}
		if (s1 > 0) {
			CK((*filter->output_function)((s1 >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s1 & 0xff, filter->data));
		}
	}
	filter->cache = 0;
	filter->status = 0;

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_ident_sjis_mac(int c, mbfl_identify_filter *filter)
{
	if (filter->status) {		/* kanji second char */
		if (c < 0x40 || c > 0xfc || c == 0x7f) {	/* bad */
		    filter->flag = 1;
		}
		filter->status = 0;
	} else if (c >= 0 && c < 0x80) {	/* latin  ok */
		;
	} else if (c > 0xa0 && c < 0xe0) {	/* kana  ok */
		;
	} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
		filter->status = 1;
	} else {							/* bad */
		filter->flag = 1;
	}

	return c;
}


