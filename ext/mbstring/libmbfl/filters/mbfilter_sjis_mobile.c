
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
#include "mbfilter_sjis_mobile.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

#include "emoji2uni.h"

extern int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n);
extern int mbfl_filt_ident_sjis(int c, mbfl_identify_filter *filter);
extern const unsigned char mblen_table_sjis[];

static const char *mbfl_encoding_sjis_docomo_aliases[] = {"SJIS-DOCOMO", "shift_jis-imode", "x-sjis-emoji-docomo", NULL};
static const char *mbfl_encoding_sjis_kddi_aliases[] = {"SJIS-KDDI", "shift_jis-kddi", "x-sjis-emoji-kddi", NULL};
static const char *mbfl_encoding_sjis_sb_aliases[] = {"SJIS-SOFTBANK", "shift_jis-softbank", "x-sjis-emoji-softbank", NULL};

const mbfl_encoding mbfl_encoding_sjis_docomo = {
 	mbfl_no_encoding_sjis_docomo,
 	"SJIS-Mobile#DOCOMO",
 	"Shift_JIS",
 	(const char *(*)[])&mbfl_encoding_sjis_docomo_aliases,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_kddi = {
 	mbfl_no_encoding_sjis_kddi,
 	"SJIS-Mobile#KDDI",
 	"Shift_JIS",
 	(const char *(*)[])&mbfl_encoding_sjis_kddi_aliases,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_sb = {
 	mbfl_no_encoding_sjis_sb,
 	"SJIS-Mobile#SOFTBANK",
 	"Shift_JIS",
 	(const char *(*)[])&mbfl_encoding_sjis_sb_aliases,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const struct mbfl_identify_vtbl vtbl_identify_sjis_docomo = {
	mbfl_no_encoding_sjis_docomo,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjis
};

const struct mbfl_identify_vtbl vtbl_identify_sjis_kddi = {
	mbfl_no_encoding_sjis_kddi,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjis
};

const struct mbfl_identify_vtbl vtbl_identify_sjis_sb = {
	mbfl_no_encoding_sjis_sb,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjis
};

const struct mbfl_convert_vtbl vtbl_sjis_docomo_wchar = {
 	mbfl_no_encoding_sjis_docomo,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_docomo = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_docomo,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
 	mbfl_filt_conv_sjis_mobile_flush
};

const struct mbfl_convert_vtbl vtbl_sjis_kddi_wchar = {
 	mbfl_no_encoding_sjis_kddi,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_kddi = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_kddi,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush
};

const struct mbfl_convert_vtbl vtbl_sjis_sb_wchar = {
 	mbfl_no_encoding_sjis_sb,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_sb = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_sb,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush
};

static const char nflags_s[10][2] = {"CN","DE","ES","FR","GB","IT","JP","KR","RU","US"};
static const int nflags_code_kddi[10] = {0x2549, 0x2546, 0x24c0, 0x2545, 0x2548, 0x2547, 0x2750, 0x254a, 0x24c1, 0x27f7};
static const int nflags_code_sb[10] = {0x2b0a, 0x2b05, 0x2b08, 0x2b04, 0x2b07, 0x2b06, 0x2b02, 0x2b0b, 0x2b09, 0x2b03};

const unsigned short mbfl_docomo2uni_pua[4][3] = {
	{0x28c2, 0x292f, 0xe63e},
	{0x2930, 0x2934, 0xe6ac},
	{0x2935, 0x2951, 0xe6b1},
	{0x2952, 0x29db, 0xe6ce},	
};

const unsigned short mbfl_kddi2uni_pua[7][3] = {
	{0x26ec, 0x2838, 0xe468},
	{0x284c, 0x2863, 0xe5b5},
	{0x24b8, 0x24ca, 0xe5cd},
	{0x24cb, 0x2545, 0xea80},	
	{0x2839, 0x284b, 0xeafb},	
	{0x2546, 0x25c0, 0xeb0e},	
	{0x25c1, 0x25c6, 0xeb89},
};

const unsigned short mbfl_sb2uni_pua[6][3] = {
	{0x27a9, 0x2802, 0xe101},
	{0x2808, 0x2861, 0xe201},
	{0x2921, 0x297a, 0xe001},
	{0x2980, 0x29cc, 0xe301},
	{0x2a99, 0x2ae4, 0xe401},
	{0x2af8, 0x2b35, 0xe501},
};

const unsigned short mbfl_kddi2uni_pua_b[8][3] = {
	{0x24b8, 0x24f6, 0xec40},
	{0x24f7, 0x2573, 0xec80},
	{0x2574, 0x25b2, 0xed40},
	{0x25b3, 0x25c6, 0xed80},
	{0x26ec, 0x272a, 0xef40},
	{0x272b, 0x27a7, 0xef80},
	{0x27a8, 0x27e6, 0xf040},
	{0x27e7, 0x2863, 0xf080},
};

#define NFLAGS(c) (0x1F1A5+(int)(c))

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

#define CODE2JIS(c1,c2,s1,s2)       \
	c1 = (s1)/94+0x21;				\
	c2 = (s1)-94*((c1)-0x21)+0x21;	\
	s1 = ((c1) << 8) | (c2);		\
	s2 = 1

int
mbfilter_conv_map_tbl(int c, int *w, const unsigned short map[][3], int n)
{
	int i, match = 0;
	
	for (i = 0; i < n; i++) {
		if (map[i][0] <= c && c <= map[i][1]) {
			*w = c - map[i][0] + map[i][2];
			match = 1;
			break;
		}
	}
	return match;
}

int
mbfilter_conv_r_map_tbl(int c, int *w, const unsigned short map[][3], int n)
{
	int i, match = 0;
	
	for (i = 0; i < n; i++) {
		if (map[i][2] <= c && c <= map[i][2] - map[i][0] + map[i][1]) {
			*w = c + map[i][0] - map[i][2];
			match = 1;
			break;
		}
	}
	return match;
}

int
mbfilter_sjis_emoji_docomo2unicode(int s, int *snd)
{
	int w = s;
	if (s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
		if (s >= mb_tbl_code2uni_docomo1_min + 0x00a2 &&
			s <= mb_tbl_code2uni_docomo1_min + 0x00ad &&
			s != mb_tbl_code2uni_docomo1_min + 0x00a3) {
			w =  0x20E3; 	
			*snd = mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min]; 
			if (*snd > 0xf000) {
				*snd += 0x10000;
			}
		} else {
			w = mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min];
			if (w > 0xf000) {
				w += 0x10000;
			} else if (w > 0xe000) { /* unsuported by Unicode 6.0 */
				w += 0xf0000;
			}
			*snd = 0;
			if (!w) {
				w = s;
			}
		}
	}

	return w;
}

int
mbfilter_sjis_emoji_kddi2unicode(int s, int *snd)
{
	int w = s, si, c;
	const int nflags_order_kddi[] = {3, 1, 5, 4, 0, 7};

	*snd = 0;
	if (s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi1_max) {
		si = s - mb_tbl_code2uni_kddi1_min;
		if (si == 0x0008) { /* ES */
			*snd = NFLAGS(nflags_s[2][0]); w = NFLAGS(nflags_s[2][1]);
		} else if (si == 0x0009) { /* RU */
			*snd = NFLAGS(nflags_s[8][0]); w = NFLAGS(nflags_s[8][1]);
		} else if (si >= 0x008d && si <= 0x0092) {
			c = nflags_order_kddi[si-0x008d];
			*snd = NFLAGS(nflags_s[c][0]); w = NFLAGS(nflags_s[c][1]);
		} else if (si == 0x0104) {
			*snd = 0x0023; w = 0x20E3;
		} else {
			w = mb_tbl_code2uni_kddi1[si];
			if (w > 0xf000) {
				w += 0x10000;
			} else if (w > 0xe000) { /* unsuported by Unicode 6.0 */
				w += 0xf0000;
			}			
		}
	} else if (s >= mb_tbl_code2uni_kddi2_min && s <= mb_tbl_code2uni_kddi2_max) {
		si = s - mb_tbl_code2uni_kddi2_min;
		if (si == 100) { /* JP */
			*snd = NFLAGS(nflags_s[6][0]); w = NFLAGS(nflags_s[6][1]);
		} else if (si >= 0x00ba && si <= 0x00c2) {
			*snd = si-0x00ba+0x0031; w = 0x20E3;
		} else if (si == 0x010b) { /* US */
			*snd = NFLAGS(nflags_s[9][0]); w = NFLAGS(nflags_s[9][1]);
		} else if (si == 0x0144) {
			*snd = 0x0030; w = 0x20E3;
		} else {
			w = mb_tbl_code2uni_kddi2[si];
			if (w > 0xf000) {
				w += 0x10000;
			} else if (w > 0xe000) { /* unsuported by Unicode 6.0 */
				w += 0xf0000;
			}			
		}
	}
	return w;
}

int
mbfilter_sjis_emoji_sb2unicode(int s, int *snd)
{
	int w = s, si, c;
	const int nflags_order_sb[10] = {6, 9, 3, 1, 5, 4, 2, 8, 0, 7};

	*snd = 0;
	if (s >= mb_tbl_code2uni_sb1_min &&	s <= mb_tbl_code2uni_sb1_max) {
		si = s - mb_tbl_code2uni_sb1_min;
		if (si == 0x006e || (si >= 0x007a && si <= 0x0083)) {
			*snd =  mb_tbl_code2uni_sb1[si];
			if (*snd > 0xf000) {
				*snd += 0x10000;
			}
			w = 0x20E3;
		} else {
			w = mb_tbl_code2uni_sb1[si];    
			if (w > 0xf000) {
				w += 0x10000;
			} else if (w > 0xe000) { /* unsuported by Unicode 6.0 */
				w += 0xf0000;
			}			
		}
	} else if (s >= mb_tbl_code2uni_sb2_min && s <= mb_tbl_code2uni_sb2_max) {
		si = s - mb_tbl_code2uni_sb2_min;
		w = mb_tbl_code2uni_sb2[si];
		if (w > 0xf000) {
			w += 0x10000;
		} else if (w > 0xe000) { /* unsuported by Unicode 6.0 */
			w += 0xf0000;
		}		
	} else if (s >= mb_tbl_code2uni_sb3_min && s <= mb_tbl_code2uni_sb3_max) {
		si = s - mb_tbl_code2uni_sb3_min;
		if (si >= 0x0069 && si <= 0x0072) {
			c = nflags_order_sb[si-0x0069];
			*snd = NFLAGS(nflags_s[c][0]); w = NFLAGS(nflags_s[c][1]);
		} else {
			w = mb_tbl_code2uni_sb3[si];
			if (w > 0xf000) {
				w += 0x10000;
			} else if (w > 0xe000) { /* unsuported by Unicode 6.0 */
				w += 0xf0000;
			}			
		}
	}
	return w;
}

int
mbfilter_unicode2sjis_emoji_docomo(int c, int *s1, mbfl_convert_filter *filter)
{
	int i, match = 0, c1s;
	
	if (filter->status == 1) {
		c1s = filter->cache;
		filter->cache = 0;
		filter->status = 0;
		if (c == 0x20E3) {
			if (c1s == 0x0023) {
				*s1 = 0x2964;
				match = 1;
			} else if (c1s == 0x0030) { 
				*s1 = 0x296f;
				match = 1;
			} else if (c1s >= 0x0031 && c1s <= 0x0039) {
				*s1 = 0x2966 + (c1s - 0x0031);
				match = 1;
			}
		} else {
			CK((*filter->output_function)(c1s, filter->data));
		}
	} else {
		if (c == 0x0023 || (c >= 0x0030 && c<=0x0039)) {
			filter->status = 1;
			filter->cache = c;
			*s1 = -1;
			return match;
		}		

		if (c == 0x00A9) {
			*s1 = 0x29b5; match = 1;
		} else if (c == 0x00AE) {
			*s1 = 0x29ba; match = 1;
		} else if (c >= mb_tbl_uni_docomo2code2_min && c <= mb_tbl_uni_docomo2code2_max) {
			i = mbfl_bisec_srch2(c, mb_tbl_uni_docomo2code2_key, mb_tbl_uni_docomo2code2_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_docomo2code2_value[i];
				match = 1;
			}
		} else if (c >= mb_tbl_uni_docomo2code3_min && c <= mb_tbl_uni_docomo2code3_max) {
			i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_docomo2code3_key, mb_tbl_uni_docomo2code3_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_docomo2code3_value[i];
				match = 1;
			}			
		} else if (c >= mb_tbl_uni_docomo2code5_min && c <= mb_tbl_uni_docomo2code5_max) {
			i = mbfl_bisec_srch2(c - 0xf0000, mb_tbl_uni_docomo2code5_key, mb_tbl_uni_docomo2code5_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_docomo2code5_val[i];
				match = 1;
			} 			
		}
	}

	return match;
}

int
mbfilter_unicode2sjis_emoji_kddi(int c, int *s1, mbfl_convert_filter *filter)
{
	int i, match = 0, c1s;
	
	if (filter->status == 1) {
		c1s = filter->cache;
		filter->cache = 0;
		filter->status = 0;
		if (c == 0x20E3) {
			if (c1s == 0x0023) {
				*s1 = 0x25bc;
				match = 1;
			} else if (c1s == 0x0030) { 
				*s1 = 0x2830;
				match = 1;
			} else if (c1s >= 0x0031 && c1s <= 0x0039) {
				*s1 = 0x27a6 + (c1s - 0x0031);
				match = 1;
			}
		} else if ((c >= NFLAGS(0x41) && c <= NFLAGS(0x5A)) && (c1s >= NFLAGS(0x41) && c1s <= NFLAGS(0x5A))) {
			for (i=0; i<10; i++) {
				if (c1s == NFLAGS(nflags_s[i][0]) && c == NFLAGS(nflags_s[i][1])) {
					*s1 = nflags_code_kddi[i]; 
					match = 1;
					break;
				}
			}
		} else {
			if (c1s >= ucs_a1_jis_table_min && c1s < ucs_a1_jis_table_max) {
				c1s = ucs_a1_jis_table[c1s - ucs_a1_jis_table_min];
				CK((*filter->output_function)(c1s, filter->data));
			}
		}
	} else {
		if (c == 0x0023 || ( c >= 0x0030 && c<=0x0039) ||
			(c >= NFLAGS(0x41) && c<= NFLAGS(0x5A))) {
			filter->status = 1;
			filter->cache = c;
			*s1 = -1;
			return match;
		}		

		if (c == 0x00A9) {
			*s1 = 0x27dc; match = 1;
		} else if (c == 0x00AE) {
			*s1 = 0x27dd; match = 1;
		} else if (c >= mb_tbl_uni_kddi2code2_min && c <= mb_tbl_uni_kddi2code2_max) {
			i = mbfl_bisec_srch2(c, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_kddi2code2_value[i];
				match = 1;
			}
		} else if (c >= mb_tbl_uni_kddi2code3_min && c <= mb_tbl_uni_kddi2code3_max) {
			i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_kddi2code3_value[i];
				match = 1;
			}
		} else if (c >= mb_tbl_uni_kddi2code5_min && c <= mb_tbl_uni_kddi2code5_max) {
			i = mbfl_bisec_srch2(c - 0xf0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_kddi2code5_val[i];
				match = 1;
			} 			
		}		
	}
	
	return match;
}

int
mbfilter_unicode2sjis_emoji_sb(int c, int *s1, mbfl_convert_filter *filter)
{
	int i, match = 0, c1s;
	
	if (filter->status == 1) {
		filter->status = 0;
		c1s = filter->cache;
		filter->cache = 0;
		if (c == 0x20E3) {
			if (c1s == 0x0023) {
				*s1 = 0x2817;
				match = 1;
			} else if (c1s == 0x0030) { 
				*s1 = 0x282c;
				match = 1;
			} else if (c1s >= 0x0031 && c1s <= 0x0039) {
				*s1 = 0x2823 + (c1s - 0x0031);
				match = 1;
			}
		} else if ((c >= NFLAGS(0x41) && c <= NFLAGS(0x5A)) && (c1s >= NFLAGS(0x41) && c1s <= NFLAGS(0x5A))) {
			for (i=0; i<10; i++) {
				if (c1s == NFLAGS(nflags_s[i][0]) && c == NFLAGS(nflags_s[i][1])) {
					*s1 = nflags_code_sb[i]; 
					match = 1;
					break;
				}
			}
		} else {
			if (c1s >= ucs_a1_jis_table_min && c1s < ucs_a1_jis_table_max) {
				c1s = ucs_a1_jis_table[c1s - ucs_a1_jis_table_min];
				CK((*filter->output_function)(c1s, filter->data));
			}
		}
	} else {
		if (c == 0x0023 || ( c >= 0x0030 && c<=0x0039) || (c >= NFLAGS(0x41) && c<= NFLAGS(0x5A))) {
			filter->status = 1;
			filter->cache = c;
			*s1 = -1;
			return match;
		}		

		if (c == 0x00A9) {
			*s1 = 0x2855; match = 1;
		} else if (c == 0x00AE) {
			*s1 = 0x2856; match = 1;
		} else if (c >= mb_tbl_uni_sb2code2_min && c <= mb_tbl_uni_sb2code2_max) {
			i = mbfl_bisec_srch2(c, mb_tbl_uni_sb2code2_key, mb_tbl_uni_sb2code2_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_sb2code2_value[i];
				match = 1;
			}
		} else if (c >= mb_tbl_uni_sb2code3_min && c <= mb_tbl_uni_sb2code3_max) {
			i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_sb2code3_key, mb_tbl_uni_sb2code3_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_sb2code3_value[i];
				match = 1;
			}
		} else if (c >= mb_tbl_uni_sb2code5_min && c <= mb_tbl_uni_sb2code5_max) {
			i = mbfl_bisec_srch2(c - 0xf0000, mb_tbl_uni_sb2code5_key, mb_tbl_uni_sb2code5_len);
			if (i >= 0) {
				*s1 = mb_tbl_uni_sb2code5_val[i];
				match = 1;
			} 			
		}		
	}
	return match;
}

/*
 * SJIS-win => wchar
 */
int
mbfl_filt_conv_sjis_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, s1, s2, w;
	int snd = 0;

retry:
	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb && c == 0x1b) {
				filter->cache = c;
				filter->status = 2;
			} else {
				CK((*filter->output_function)(c, filter->data));
			}
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
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
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {		/* vendor ext3 (115ku - 119ku) */
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= (94*94) && s < (114*94)) {		/* user (95ku - 114ku) */
					w = s - (94*94) + 0xe000;
				}
				
 				if (s >= (94*94) && s < 119*94) {
					if (filter->from->no_encoding == mbfl_no_encoding_sjis_docomo) {
						w = mbfilter_sjis_emoji_docomo2unicode(s, &snd);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_kddi) {
						w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb) {
						w = mbfilter_sjis_emoji_sb2unicode(s, &snd);
					}

					if (w > 0  && snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				}		
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
	/* ESC : Softbank Emoji */
	case 2:
		if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb && 
			c == 0x24) {
				filter->cache = c;
				filter->status++;
		} else {
			filter->cache = 0;
			filter->status = 0;
			CK((*filter->output_function)(0x1b, filter->data));
			goto retry;
		}
		break;

	/* ESC $ : Softbank Emoji */
	case 3:
		if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb && 
			((c >= 0x45 && c <= 0x47) || (c >= 0x4f && c <= 0x51))) {
				filter->cache = c;
				filter->status++;
		} else {
			filter->cache = 0;
			filter->status = 0;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			goto retry;
		}
		break;

	/* ESC [GEFOPQ] : Softbank Emoji */
	case 4:
		w = 0;
		if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb) {
			c1 = filter->cache;

			if (c == 0x0f) {
				w = c;
				filter->cache = 0;
				filter->status = 0;				
			} else {
				if (c1 == 0x47 && c >= 0x21 && c <= 0x7a) {
					s1 = 0x91; s2 = c;	
				} else if (c1 == 0x45 && c >= 0x21 && c <= 0x7a) {
					s1 = 0x8d; s2 = c;	
				} else if (c1 == 0x46 && c >= 0x21 && c <= 0x7a) {
					s1 = 0x8e; s2 = c;	
				} else if (c1 == 0x4f && c >= 0x21 && c <= 0x6d) {
					s1 = 0x92; s2 = c;	
				} else if (c1 == 0x50 && c >= 0x21 && c <= 0x6c) {
					s1 = 0x95; s2 = c;	
				} else if (c1 == 0x51 && c >= 0x21 && c <= 0x5e) {
					s1 = 0x96; s2 = c;	
				}
				s  = (s1 - 0x21)*94 + s2 - 0x21;
				w = mbfilter_sjis_emoji_sb2unicode(s, &snd);
				if (w > 0) {
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}					
					CK((*filter->output_function)(w, filter->data));
				}
			}
		}

		if (w <= 0) {
			c1 = filter->cache;
			filter->cache = 0;
			filter->status = 0;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(c1 & 0xff, filter->data));
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
 * wchar => SJIS-win
 */
int
mbfl_filt_conv_wchar_sjis_mobile(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1, s2;

	s1 = 0;
	s2 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 20*94)) {	/* user  (95ku - 114ku) */
		s1 = c - 0xe000;
		c1 = s1/94 + 0x7f;
		c2 = s1%94 + 0x21;
		s1 = (c1 << 8) | c2;
		s2 = 1;
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
		} else if (c == 0xa5) {		/* YEN SIGN */
			s1 = 0x216f;	/* FULLWIDTH YEN SIGN */
		} else if (c == 0x203e) {	/* OVER LINE */
			s1 = 0x2131;	/* FULLWIDTH MACRON */
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
		}
	}

	if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
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
		if (s1 <= 0) {
			c1 = 0;
			c2 = cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext2 (115ku - 119ku) */
				if (c == cp932ext2_ucs_table[c1]) {
					s1 = ((c1/94 + 0x93) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
		}

		if (s1 <= 0) {
			c1 = 0;
			c2 = cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
				if (c == cp932ext3_ucs_table[c1]) {
					s1 = ((c1/94 + 0x93) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}

 	if ((filter->to->no_encoding == mbfl_no_encoding_sjis_docomo &&
		 mbfilter_unicode2sjis_emoji_docomo(c, &s1, filter) > 0) ||
		(filter->to->no_encoding == mbfl_no_encoding_sjis_kddi &&
		 mbfilter_unicode2sjis_emoji_kddi(c, &s1, filter) > 0) ||
		(filter->to->no_encoding == mbfl_no_encoding_sjis_sb &&
		 mbfilter_unicode2sjis_emoji_sb(c, &s1, filter) > 0 )) {
		CODE2JIS(c1,c2,s1,s2);
 	}

	if (filter->status == 1 && filter->cache > 0) {
		return c;
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

	return c;
}

int
mbfl_filt_conv_sjis_mobile_flush(mbfl_convert_filter *filter)
{
	int c1 = filter->cache;
	if (filter->status == 1 && (c1 == 0x0023 || (c1 >= 0x0030 && c1<=0x0039))) {
		CK((*filter->output_function)(c1, filter->data));
	}
	filter->status = 0;
	filter->cache = 0;

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

