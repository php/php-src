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

#include "mbfilter.h"
#include "mbfilter_sjis_mac.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

#include "sjis_mac2uni.h"

const unsigned char mblen_table_sjismac[] = { /* 0x81-0x9F,0xE0-0xED */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static int mbfl_filt_conv_wchar_sjis_mac_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_sjis_mac_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_sjismac_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_sjismac(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_sjis_mac_aliases[] = {"MacJapanese", "x-Mac-Japanese", NULL};

const mbfl_encoding mbfl_encoding_sjis_mac = {
	mbfl_no_encoding_sjis_mac,
	"SJIS-mac",
	"Shift_JIS",
	mbfl_encoding_sjis_mac_aliases,
	mblen_table_sjismac,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_mac_wchar,
	&vtbl_wchar_sjis_mac,
	mb_sjismac_to_wchar,
	mb_wchar_to_sjismac,
	NULL
};

const struct mbfl_convert_vtbl vtbl_sjis_mac_wchar = {
	mbfl_no_encoding_sjis_mac,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mac_wchar,
	mbfl_filt_conv_sjis_mac_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_mac = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_mac,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mac,
	mbfl_filt_conv_wchar_sjis_mac_flush,
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
		} else if (c > 0x80 && c <= 0xed && c != 0xa0) {	/* kanji first char */
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
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
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

				for (i=0; i<code_tbl_m_len; i++) {
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
						if (w == 0) {
							CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
							return 0;
						}
						s2 = 0;
						if (s >= 0x043e && s <= 0x0441) {
							s2 = 0xf87a;
						} else if (s == 0x03b1 || s == 0x03b7) {
							s2 = 0xf87f;
						} else if (s == 0x04b8 || s == 0x04b9 || s == 0x04c4) {
							s2 = 0x20dd;
						} else if (s == 0x1ed9 || s == 0x1eda || s == 0x1ee8 || s == 0x1ef3 ||
								   (s >= 0x1ef5 && s <= 0x1efb) || s == 0x1f05 || s == 0x1f06 ||
								   s == 0x1f18 || (s >= 0x1ff2 && s <= 0x20a5)) {
							s2 = 0xf87e;
						}
						if (s2 > 0) {
							CK((*filter->output_function)(w, filter->data));
							w = s2;
						}
						break;
					}
				}
			}

			if (w == 0 && s >= 0 && s < jisx0208_ucs_table_size) {	/* X 0208 */
				w = jisx0208_ucs_table[s];
			}

			if (w <= 0) {
				w = MBFL_BAD_INPUT;
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_sjis_mac_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_sjis_mac(int c, mbfl_convert_filter *filter)
{
	int i, c1, c2, s1 = 0, s2 = 0, mode;

	// a1: U+0000 -> U+046F
	// a2: U+2000 -> U+30FF
	//  i: U+4E00 -> U+9FFF
	//  r: U+FF00 -> U+FFFF

	switch (filter->status) {
	case 1:
		c1 = filter->cache;
		filter->cache = filter->status = 0;

		if (c == 0xf87a) {
			for (i = 0; i < 4; i++) {
				if (c1 == s_form_tbl[i+34+3+3]) {
					s1 = s_form_sjis_tbl[i+34+3+3];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
			}
		} else if (c == 0x20dd) {
			for (i = 0; i < 3; i++) {
				if (c1 == s_form_tbl[i+34+3]) {
					s1 = s_form_sjis_tbl[i+34+3];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
			}
		} else if (c == 0xf87f) {
			for (i = 0; i < 3; i++) {
				if (c1 == s_form_tbl[i+34]) {
					s1 = s_form_sjis_tbl[i+34];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
				s1 = -1;
			}
		} else if (c == 0xf87e) {
			for (i = 0; i < 34; i++) {
				if (c1 == s_form_tbl[i]) {
					s1 = s_form_sjis_tbl[i];
					break;
				}
			}
			if (s1 <= 0) {
				s2 = c1;
				s1 = -1;
			}
		} else {
			s2 = c1;
			s1 = c;
		}

		if (s2 > 0) {
			for (i = 0; i < s_form_tbl_len; i++) {
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
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}

		if (s2 <= 0 || s1 == -1) {
			break;
		}
		s1 = s2 = 0;
		ZEND_FALLTHROUGH;

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
			for (i = 0; i < s_form_tbl_len; i++) {
				if (c == s_form_tbl[i]) {
					filter->status = 1;
					filter->cache = c;
					return 0;
				}
			}

			if (c == 0xf860 || c == 0xf861 || c == 0xf862) {
				/* Apple 'transcoding hint' codepoints (from private use area) */
				filter->status = 2;
				filter->cache = c;
				return 0;
			}
		}

		if (s1 <= 0) {
			if (c == 0xa0) {
				s1 = 0x00a0;
			} else if (c == 0xa5) { /* YEN SIGN */
				/* Unicode has codepoint 0xFFE5 for a fullwidth Yen sign;
				 * convert codepoint 0xA5 to halfwidth Yen sign */
				s1 = 0x5c; /* HALFWIDTH YEN SIGN */
			} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
				s1 = 0x2140;
			}
		}

		if (s1 <= 0) {
			for (i=0; i<wchar2sjis_mac_r_tbl_len; i++) {
				if (c >= wchar2sjis_mac_r_tbl[i][0] && c <= wchar2sjis_mac_r_tbl[i][1]) {
					s1 = c - wchar2sjis_mac_r_tbl[i][0] + wchar2sjis_mac_r_tbl[i][2];
					break;
				}
			}

			if (s1 <= 0) {
				for (i=0; i<wchar2sjis_mac_r_map_len; i++) {
					if (c >= wchar2sjis_mac_r_map[i][0] && c <= wchar2sjis_mac_r_map[i][1]) {
						s1 = wchar2sjis_mac_code_map[i][c-wchar2sjis_mac_r_map[i][0]];
						break;
					}
				}
			}

			if (s1 <= 0) {
				for (i=0; i<wchar2sjis_mac_wchar_tbl_len ; i++) {
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
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
		break;

	case 2:
		c1 = filter->cache;
		filter->cache = 0;
		filter->status = 0;
		if (c1 == 0xf860) {
			for (i = 0; i < 5; i++) {
				if (c == code_tbl_m[i][2]) {
					filter->cache = c | 0x10000;
					filter->status = 3;
					break;
				}
			}
		} else if (c1 == 0xf861) {
			for (i = 0; i < 3; i++) {
				if (c == code_tbl_m[i+5][2]) {
					filter->cache = c | 0x20000;
					filter->status = 3;
					break;
				}
			}
		} else if (c1 == 0xf862) {
			for (i = 0; i < 4; i++) {
				if (c == code_tbl_m[i+5+3][2]) {
					filter->cache = c | 0x40000;
					filter->status = 3;
					break;
				}
			}
		}

		if (filter->status == 0) {
			/* Didn't find any of expected codepoints after Apple transcoding hint */
			CK(mbfl_filt_conv_illegal_output(c1, filter));
			return mbfl_filt_conv_wchar_sjis_mac(c, filter);
		}
		break;

	case 3:
		s1 = 0;
		c1 = filter->cache & 0xffff;
		mode = (filter->cache & 0xf0000) >> 16;

		filter->cache = filter->status = 0;

		if (mode == 0x1) {
			for (i = 0; i < 5; i++) {
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
			} else {
				CK(mbfl_filt_conv_illegal_output(0xf860, filter));
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		} else if (mode == 0x2) {
			for (i = 0; i < 3; i++) {
				if (c1 == code_tbl_m[i+5][2] && c == code_tbl_m[i+5][3]) {
					filter->cache = c | 0x20000;
					filter->status = 4;
					break;
				}
			}
		} else if (mode == 0x4) {
			for (i = 0; i < 4; i++) {
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
			for (i = 0; i < 3; i++) {
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
			} else {
				CK(mbfl_filt_conv_illegal_output(0xf861, filter));
				for (i = 0; i < 3; i++) {
					if (c1 == code_tbl_m[i+5][3]) {
						CK(mbfl_filt_conv_illegal_output(code_tbl_m[i+5][2], filter));
						break;
					}
				}
				CK(mbfl_filt_conv_illegal_output(c1, filter));
				CK(mbfl_filt_conv_illegal_output(c, filter));
			}
		} else if (mode == 0x4) {
			for (i = 0; i < 4; i++) {
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

		filter->cache = filter->status = 0;

		if (mode == 0x4) {
			for (i = 0; i < 4; i++) {
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
			} else {
				CK(mbfl_filt_conv_illegal_output(0xf862, filter));
				for (i = 0; i < 4; i++) {
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

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int
mbfl_filt_conv_wchar_sjis_mac_flush(mbfl_convert_filter *filter)
{
	int i, c1, s1 = 0;
	if (filter->status == 1 && filter->cache > 0) {
		c1 = filter->cache;
		for (i=0;i<s_form_tbl_len;i++) {
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

static size_t mb_sjismac_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	/* A single SJIS-Mac kuten code can convert to up to 5 Unicode codepoints, oh my! */
	ZEND_ASSERT(bufsize >= 5);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80 && c != 0x5C) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c <= 0xED && c != 0xA0) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F) {
				unsigned int w = 0, s1 = 0, s2 = 0;
				SJIS_DECODE(c, c2, s1, s2);
				unsigned int s = (s1 - 0x21)*94 + s2 - 0x21;

				if (s <= 0x89) {
					if (s == 0x1C) {
						w = 0x2014; /* EM DASH */
					} else if (s == 0x1F) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 0x20) {
						w = 0x301C; /* FULLWIDTH TILDE */
					} else if (s == 0x21) {
						w = 0x2016; /* PARALLEL TO */
					} else if (s == 0x3C) {
						w = 0x2212; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 0x50) {
						w = 0xA2; /* FULLWIDTH CENT SIGN */
					} else if (s == 0x51) {
						w = 0xA3; /* FULLWIDTH POUND SIGN */
					} else if (s == 0x89) {
						w = 0xAC; /* FULLWIDTH NOT SIGN */
					}
					if (w) {
						*out++ = w;
						continue;
					}
				}

				for (int i = 0; i < 7; i++) {
					if (s >= code_tbl[i][0] && s <= code_tbl[i][1]) {
						*out++ = s - code_tbl[i][0] + code_tbl[i][2];
						goto next_iteration;
					}
				}

				for (int i = 0; i < code_tbl_m_len; i++) {
					if (s == code_tbl_m[i][0]) {
						int n = 5;
						if (code_tbl_m[i][1] == 0xF860) {
							n = 3;
						} else if (code_tbl_m[i][1] == 0xF861) {
							n = 4;
						}
						if ((limit - out) < n) {
							p -= 2;
							goto finished;
						}
						for (int j = 1; j <= n; j++) {
							*out++ = code_tbl_m[i][j];
						}
						goto next_iteration;
					}
				}

				for (int i = 0; i < 8; i++) {
					if (s >= code_ofst_tbl[i][0] && s <= code_ofst_tbl[i][1]) {
						w = code_map[i][s - code_ofst_tbl[i][0]];
						if (!w) {
							*out++ = MBFL_BAD_INPUT;
							goto next_iteration;
						}
						if ((limit - out) < 2) {
							p -= 2;
							goto finished;
						}
						*out++ = w;
						if (s >= 0x43E && s <= 0x441) {
							*out++ = 0xF87A;
						} else if (s == 0x3B1 || s == 0x3B7) {
							*out++ = 0xF87F;
						} else if (s == 0x4B8 || s == 0x4B9 || s == 0x4C4) {
							*out++ = 0x20DD;
						} else if (s == 0x1ED9 || s == 0x1EDA || s == 0x1EE8 || s == 0x1EF3 || (s >= 0x1EF5 && s <= 0x1EFB) || s == 0x1F05 || s == 0x1F06 || s == 0x1F18 || (s >= 0x1FF2 && s <= 0x20A5)) {
							*out++ = 0xF87E;
						}
						goto next_iteration;
					}
				}

				if (s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c == 0x5C) {
			*out++ = 0xA5;
		} else if (c == 0x80) {
			*out++ = 0x5C;
		} else if (c == 0xA0) {
			*out++ = 0xA0;
		} else if (c == 0xFD) {
			*out++ = 0xA9;
		} else if (c == 0xFE) {
			*out++ = 0x2122;
		} else if (c == 0xFF) {
			if ((limit - out) < 2) {
				p--;
				break;
			}
			*out++ = 0x2026;
			*out++ = 0xF87F;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
next_iteration: ;
	}

finished:
	*in_len = e - p;
	*in = p;
	return out - buf;
}

static bool process_s_form(uint32_t w, uint32_t w2, unsigned int *s)
{
	if (w2 == 0xF87A) {
		for (int i = 0; i < 4; i++) {
			if (w == s_form_tbl[i+34+3+3]) {
				*s = s_form_sjis_tbl[i+34+3+3];
				return true;
			}
		}
	} else if (w2 == 0x20DD) {
		for (int i = 0; i < 3; i++) {
			if (w == s_form_tbl[i+34+3]) {
				*s = s_form_sjis_tbl[i+34+3];
				return true;
			}
		}
	} else if (w2 == 0xF87F) {
		for (int i = 0; i < 3; i++) {
			if (w == s_form_tbl[i+34]) {
				*s = s_form_sjis_tbl[i+34];
				return true;
			}
		}
	} else if (w2 == 0xF87E) {
		for (int i = 0; i < 34; i++) {
			if (w == s_form_tbl[i]) {
				*s = s_form_sjis_tbl[i];
				return true;
			}
		}
	}

	return false;
}

/* For codepoints F860-F862, which are treated specially in MacJapanese */
static int transcoding_hint_cp_width[3] = { 3, 4, 5 };

static void mb_wchar_to_sjismac(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;

	if (buf->state) {
		w = buf->state & 0xFFFF;
		if (buf->state & 0xFF000000L) {
			goto resume_transcoding_hint;
		} else {
			buf->state = 0;
			goto process_codepoint;
		}
	}

	while (len--) {
		w = *in++;
process_codepoint: ;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			if (w == 0x5C) {
				s = 0x80;
			} else if (w == 0xA9) {
				s = 0xFD;
			} else {
				s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
			}
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			if (w == 0x2122) {
				s = 0xFE;
			} else if (w == 0x2014) {
				s = 0x213D;
			} else if (w == 0x2116) {
				s = 0x2C1D;
			} else {
				s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
			}
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (w >= 0x2000) {
			for (int i = 0; i < s_form_tbl_len; i++) {
				if (w == s_form_tbl[i]) {
					if (!len) {
						if (end) {
							s = s_form_sjis_fallback_tbl[i];
							if (s) {
								MB_CONVERT_BUF_ENSURE(buf, out, limit, 2);
								out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
							} else {
								MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
							}
						} else {
							buf->state = w;
						}
						MB_CONVERT_BUF_STORE(buf, out, limit);
						return;
					}
					uint32_t w2 = *in++;
					len--;

					if (!process_s_form(w, w2, &s)) {
						in--; len++;

						for (int i = 0; i < s_form_tbl_len; i++) {
							if (w == s_form_tbl[i]) {
								s = s_form_sjis_fallback_tbl[i];
								break;
							}
						}
					}

					if (s <= 0xFF) {
						out = mb_convert_buf_add(out, s);
					} else {
						MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
						out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
					}

					goto next_iteration;
				}
			}

			if (w == 0xF860 || w == 0xF861 || w == 0xF862) {
				/* Apple 'transcoding hint' codepoints (from private use area) */
				if (!len) {
					if (end) {
						MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
					} else {
						buf->state = w;
					}
					MB_CONVERT_BUF_STORE(buf, out, limit);
					return;
				}

				uint32_t w2 = *in++;
				len--;

				for (int i = 0; i < code_tbl_m_len; i++) {
					if (w == code_tbl_m[i][1] && w2 == code_tbl_m[i][2]) {
						/* This might be a valid transcoding hint sequence */
						int index = 3;

resume_transcoding_hint:
						if (buf->state) {
							i = buf->state >> 24;
							index = (buf->state >> 16) & 0xFF;
							buf->state = 0;
						}

						int expected = transcoding_hint_cp_width[w - 0xF860];

						while (index <= expected) {
							if (!len) {
								if (end) {
									for (int j = 1; j < index; j++) {
										MB_CONVERT_ERROR(buf, out, limit, code_tbl_m[i][j], mb_wchar_to_sjismac);
									}
								} else {
									buf->state = (i << 24) | (index << 16) | (w & 0xFFFF);
								}
								MB_CONVERT_BUF_STORE(buf, out, limit);
								return;
							}

							w2 = *in++;
							len--;

							if (w2 != code_tbl_m[i][index]) {
								/* Didn't match */
								for (int j = 1; j < index; j++) {
									MB_CONVERT_ERROR(buf, out, limit, code_tbl_m[i][j], mb_wchar_to_sjismac);
								}
								MB_CONVERT_ERROR(buf, out, limit, w2, mb_wchar_to_sjismac);
								MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
								goto next_iteration;
							}

							index++;
						}

						/* Successful match, emit SJIS-mac bytes */
						s = code_tbl_m[i][0];
						unsigned int c1 = (s / 94) + 0x21, c2 = (s % 94) + 0x21, s1, s2;
						SJIS_ENCODE(c1, c2, s1, s2);
						MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
						out = mb_convert_buf_add2(out, s1, s2);
						goto next_iteration;
					}
				}

				/* No valid transcoding hint sequence found */
				in--; len++;
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				continue;
			}
		}

		if (!s) {
			if (w == 0xA0) {
				s = 0xA0;
			} else if (w == 0xA5) { /* YEN SIGN */
				/* Unicode has codepoint 0xFFE5 for a fullwidth Yen sign;
				 * convert codepoint 0xA5 to halfwidth Yen sign */
				s = 0x5C; /* HALFWIDTH YEN SIGN */
			} else if (w == 0xFF3C) {	/* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else {
				for (int i = 0; i < wchar2sjis_mac_r_tbl_len; i++) {
					if (w >= wchar2sjis_mac_r_tbl[i][0] && w <= wchar2sjis_mac_r_tbl[i][1]) {
						s = w - wchar2sjis_mac_r_tbl[i][0] + wchar2sjis_mac_r_tbl[i][2];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto found_kuten_code;
					}
				}

				for (int i = 0; i < wchar2sjis_mac_r_map_len; i++) {
					if (w >= wchar2sjis_mac_r_map[i][0] && w <= wchar2sjis_mac_r_map[i][1]) {
						s = wchar2sjis_mac_code_map[i][w - wchar2sjis_mac_r_map[i][0]];
						if (s) {
							s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
							goto found_kuten_code;
						}
					}
				}

				for (int i = 0; i < wchar2sjis_mac_wchar_tbl_len; i++) {
					if (w == wchar2sjis_mac_wchar_tbl[i][0]) {
						s = wchar2sjis_mac_wchar_tbl[i][1];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto found_kuten_code;
					}
				}
			}
		}

found_kuten_code:
		if ((!s && w) || s >= 0x8080) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjismac);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s <= 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			unsigned int c1 = (s >> 8) & 0xFF, c2 = s & 0xFF, s1, s2;
			SJIS_ENCODE(c1, c2, s1, s2);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, s1, s2);
		}

next_iteration: ;
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
