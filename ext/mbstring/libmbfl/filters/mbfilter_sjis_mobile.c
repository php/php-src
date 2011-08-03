
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

static int mbfl_filt_ident_sjis_mobile(int c, mbfl_identify_filter *filter);

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

const mbfl_encoding mbfl_encoding_sjis_docomo = {
 	mbfl_no_encoding_sjis_docomo,
 	"SJIS-Mobile#DOCOMO",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_kddi = {
 	mbfl_no_encoding_sjis_kddi,
 	"SJIS-Mobile#KDDI",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_sb = {
 	mbfl_no_encoding_sjis_sb,
 	"SJIS-Mobile#SOFTBANK",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_docomo_pua = {
 	mbfl_no_encoding_sjis_docomo_pua,
 	"SJIS-Mobile#DOCOMO-PUA",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_kddi_pua = {
 	mbfl_no_encoding_sjis_kddi_pua,
 	"SJIS-Mobile#KDDI-PUA",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_kddi_pua_b = {
 	mbfl_no_encoding_sjis_kddi_pua_b,
 	"SJIS-Mobile#KDDI-PUA-B",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
};

const mbfl_encoding mbfl_encoding_sjis_sb_pua = {
 	mbfl_no_encoding_sjis_sb_pua,
 	"SJIS-Mobile#SOFTBANK-PUA",
 	"Shift_JIS",
 	NULL,
 	mblen_table_sjis,
 	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE
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
 	mbfl_filt_conv_common_flush
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
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_sjis_docomo_pua_wchar = {
 	mbfl_no_encoding_sjis_docomo_pua,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_docomo_pua = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_docomo_pua,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_sjis_kddi_pua_wchar = {
 	mbfl_no_encoding_sjis_kddi_pua,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_kddi_pua = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_kddi_pua,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_sjis_kddi_pua_b_wchar = {
 	mbfl_no_encoding_sjis_kddi_pua_b,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_kddi_pua_b = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_kddi_pua_b,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_sjis_sb_pua_wchar = {
 	mbfl_no_encoding_sjis_sb_pua,
 	mbfl_no_encoding_wchar,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_sjis_mobile_wchar,
 	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_sb_pua = {
 	mbfl_no_encoding_wchar,
 	mbfl_no_encoding_sjis_sb_pua,
 	mbfl_filt_conv_common_ctor,
 	mbfl_filt_conv_common_dtor,
 	mbfl_filt_conv_wchar_sjis_mobile,
 	mbfl_filt_conv_common_flush
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

int
mbfiler_sjis_emoji_docomo2unicode(int s, int *snd)
{
	int w = s;
	if (s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
		if (s >= mb_tbl_code2uni_docomo1_min + 0x00a2 &&
			s <= mb_tbl_code2uni_docomo1_min + 0x00ad &&
			s != mb_tbl_code2uni_docomo1_min + 0x00a3) {
			w =  0x20E3; 	
			*snd = mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min]; 
		} else {
			w = mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min];
			*snd = 0;
			if (!w) {
				w = s;
			}
		}
	}
	return w;
}

int
mbfiler_sjis_emoji_kddi2unicode(int s, int *snd)
{
	int w = s, si;
	const char flags[][2] = {"FR","DE","IT","GB","CN","KR","ES","RU","JP","US"};
	
	*snd = 0;
	if (s >= mb_tbl_code2uni_kddi1_min &&
		s <= mb_tbl_code2uni_kddi1_max) {
		si = s - mb_tbl_code2uni_kddi1_min;
		if (si == 0x0008) {
			*snd = 0x1F1A5 + flags[6][0]; w = 0x1F1A5 + flags[6][1];
		} else if (si == 0x0009) {
			*snd = 0x1F1A5 + flags[7][0]; w = 0x1F1A5 + flags[7][1];
		} else if (si >= 0x008d && si <= 0x0092) {
			*snd = 0x1F1A5 + flags[si - 0x8d][0];
			w = 0x1F1A5 + flags[si - 0x8d][1];
		} else if (si == 0x0104) {
			*snd = 0x0023; w = 0x20E3;
		} else {
			w = mb_tbl_code2uni_kddi1[si];
		}
	} else if (s >= mb_tbl_code2uni_kddi2_min &&
			   s <= mb_tbl_code2uni_kddi2_max) {
		si = s - mb_tbl_code2uni_kddi2_min;
		if (si == 100) {
			*snd = 0x1F1A5 + flags[8][0]; w = 0x1F1A5 + flags[8][1];
		} else if (si >= 0x00ba && si <= 0x00c2) {
			*snd = si-0x00ba+0x0031; w = 0x20E3;
		} else if (si == 0x010b) {
			*snd = 0x1F1A5 + flags[9][0]; w = 0x1F1A5 + flags[9][1];
		} else if (si == 0x0144) {
			*snd = 0x0030; w = 0x20E3;
		} else {
			w = mb_tbl_code2uni_kddi2[si];
		}
	}
	return w;
}

int
mbfiler_sjis_emoji_sb2unicode(int s, int *snd)
{
	int w = s, si;
	const char flags[][2] = {"JP","US","FR","DE","IT","GB","ES","RU","CN","KR"};
	
	*snd = 0;
	if (s >= mb_tbl_code2uni_sb1_min &&
		s <= mb_tbl_code2uni_sb1_max) {
		si = s - mb_tbl_code2uni_sb1_min;
		if (si == 0x006e || (si >= 0x007a && si <= 0x0083)) {
			*snd =  mb_tbl_code2uni_sb1[si];
			w = 0x20E3;
		} else {
			w = mb_tbl_code2uni_sb1[si];    
		}
	} else if (s >= mb_tbl_code2uni_sb2_min &&
			   s <= mb_tbl_code2uni_sb2_max) {
		si = s - mb_tbl_code2uni_sb2_min;
		w = mb_tbl_code2uni_sb2[si];
	} else if (s >= mb_tbl_code2uni_sb3_min &&
			   s <= mb_tbl_code2uni_sb3_max) {
		si = s - mb_tbl_code2uni_sb3_min;
		if (si >= 0x0069 && si <= 0x0072) {
			*snd = 0x1F1A5 + flags[si - 0x0069][0];
			w = 0x1F1A5 + flags[si - 0x0069][1];
		} else {
			w = mb_tbl_code2uni_sb3[si];
		}
	}
	return w;
}

int
mbfiler_sjis_emoji_kddi2unicode_pua(int s, int *w)
{

	if (s >= 0x26ec && s <= 0x2838) {
		*w = s - 0x26ec + 0xe468;
	} else if (s >= 0x284c && s <= 0x2863) {
		*w = s - 0x284c + 0xe5b5;
	} else if (s >= 0x24b8 && s <= 0x24ca) {
		*w = s - 0x24b8 + 0xe5cd;
	} else if (s >= 0x24cb && s <= 0x2545) {
		*w = s - 0x24cb + 0xea80;
	} else if (s >= 0x2839 && s <= 0x284b) {
		*w = s - 0x2839 + 0xeafb;
	} else if (s >= 0x2546 && s <= 0x25c0) {
		*w = s - 0x2546 + 0xeb0e;
	}

	return *w;
}

int
mbfiler_sjis_emoji_kddi2unicode_pua_b(int s, int c1, int c, int *w)
{
	if ((c1 == 0xf3 && (c >= 0x40 && c<=0xfc && c != 0x7f)) ||
		(c1 == 0xf4 && (c >= 0x40 && c<=0x93 && c != 0x7f)) ||
		((c1 == 0xf6 || c1 == 0xf7) && (c >= 0x40 && c<=0xfc && c != 0x7f))) {
		*w = (c1 << 8) + c - 0x0700; /* KDDI unofficial UTF-8 encoding */
	}
	return *w;
}

int
mbfiler_sjis_emoji2unicode_pua(int s, int *w)
{
	if (s >= 0x27a9 && s <= 0x2802) {
		*w = s - 0x27a9 + 0xe101;
	} else if (s >= 0x2808 && s <= 0x285a) {
		*w = s - 0x2808 + 0xe201;
	} else if (s >= 0x2921 && s <= 0x297a) {
		*w = s - 0x2921 + 0xe001;
	} else if (s >= 0x2980 && s <= 0x29cc) {
		*w = s - 0x2980 + 0xe301;
	} else if (s >= 0x2a99 && s <= 0x2ae4) {
		*w = s - 0x2a99 + 0xe401;
	} else if (s >= 0x2af8 && s <= 0x2b2e) {
		*w = s - 0x2af8 + 0xe501;
	}
	return *w;
}

int
mbfiler_unicode_pua2sjis_emoji_kddi(int c, int *s1, int *s2, int *c1, int *c2)
{
	int match = 1;

	if (c >= 0xe468 && c <= 0xe5b4) {
		*s1 = c - 0xe468 + 0x26ec;
	} else if (c >= 0xe5b5 && c <= 0xe5cc) {
		*s1 = c - 0xe5b5 + 0x284c;
	} else if (c >= 0xe5cd && c <= 0xe5df) {
		*s1 = c - 0xe5cd + 0x24b8;
	} else if (c >= 0xea80 && c <= 0xeafa) {
		*s1 = c - 0xea80 + 0x24cb;
	} else if (c >= 0xeafb && c <= 0xeb0d) {
		*s1 = c - 0xeafb + 0x2839;
	} else if (c >= 0xeb0e && c <= 0xeb88) {
		*s1 = c - 0xeb0e + 0x2546;
	} else {
		match = 0;
	}
	if (match && *s1 >0) {
		*c1 = *s1/94+0x21;
		*c2 = *s1-94*(*c1-0x21)+0x21;
		*s1 = (*c1 << 8) | *c2;
		*s2 = 1;
	}

	return 1;
}



int
mbfiler_unicode_pua2sjis_emoji(int c, int *s1, int *s2, int *c1, int *c2, int *sjis_encoded)
{
	int match = 1;

	if (c >= 0xe001 && c <= 0xe05a) {
		*s1 = c - 0xe001 + 0x2921;
	} else if (c >= 0xe101 && c <= 0xe15a) {
		*s1 = c - 0xe101 + 0x27a9;
	} else if (c >= 0xe201 && c <= 0xe253) {
		*s1 = c - 0xe201 + 0x2808;
	} else if (c >= 0xe301 && c <= 0xe34d) {
		*s1 = c - 0xe301 + 0x2980;
	} else if (c >= 0xe401 && c <= 0xe44c) {
		*s1 = c - 0xe401 + 0x2a99;
	} else if (c >= 0xe501 && c <= 0xe537) {
		*s1 = c - 0xe501 + 0x2af8;
	} else if (
		((c >= 0xec40 && c<=0xecfc)  && c != 0xec7f) ||
		((c >= 0xed40 && c<=0xed93)  && c != 0xed7f) ||	
		((c >= 0xef40 && c<=0xeffc)  && c != 0xef7f) ||	
		((c >= 0xf040 && c<=0xf0fc)  && c != 0xf07f)) {
		/* KDDI unofficial UTF-8 encoding */
		*s1 = c + 0x0700;
		*sjis_encoded = 1;
	} else {
		match = 0;
	}
	if (match && *sjis_encoded == 0 && *s1 >0) {
		*c1 = *s1/94+0x21;
		*c2 = *s1-94*(*c1-0x21)+0x21;
		*s1 = (*c1 << 8) | *c2;
		*s2 = 1;
	}

	return 1;
}

int
mbfiler_unicode2sjis_emoji_docomo(int c, int *s1, int *s2, int *c1, int *c2, mbfl_convert_filter *filter)
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
			if (c1s >= ucs_a1_jis_table_min && c1s < ucs_a1_jis_table_max) {
				c1s = ucs_a1_jis_table[c1s - ucs_a1_jis_table_min];
				CK((*filter->output_function)(c1s, filter->data));
			}			
		}
	} else {
		if (c == 0x0023 || (c >= 0x0030 && c<=0x0039)) {
			filter->status = 1;
			filter->cache = c;
			*s1 = -1;
			return 0;
		}		

		if (c == 0x00A9) {
			*s1 = 0x29b5; match = 1;
		} else if (c == 0x00AE) {
			*s1 = 0x29ba; match = 1;
		} else if (c >= mb_tbl_uni_docomo2code2_min && c <= mb_tbl_uni_docomo2code2_max) {
			for (i=0; i < sizeof(mb_tbl_uni_docomo2code2_key)/sizeof(int); i++) {
				if (c >0 && mb_tbl_uni_docomo2code2_key[i] == c) {
					*s1 = mb_tbl_uni_docomo2code2_value[i];
					match = 1;
					break;
				}
			}
		} else if (c >= mb_tbl_uni_docomo2code3_min && c <= mb_tbl_uni_docomo2code3_max) {
			for (i=0; i < sizeof(mb_tbl_uni_docomo2code3_key)/sizeof(int); i++) {
				if (c >0 && mb_tbl_uni_docomo2code3_key[i] == c) {
					*s1 = mb_tbl_uni_docomo2code3_value[i];
					match = 1;
					break;
				}
			}
		}
	}

	if (match && *s1 >0) {
		*c1 = *s1/94+0x21;
		*c2 = *s1-94*(*c1-0x21)+0x21;
		*s1 = (*c1 << 8) | *c2;
		*s2 = 1;
	}

	return 1;
}

const char nflags_s[][2] = {"CN","DE","ES","FR","GB","IT","JP","KR","RU","US"};
const int nflags_code_kddi[] = {0x2549, 0x2546, 0x24c0, 0x2545, 0x2548, 0x2547, 0x2750, 0x254a, 0x24c1, 0x27f7};
const int nflags_code_sb[] = {0x2b0a, 0x2b05, 0x2b08, 0x2b04, 0x2b07, 0x2b06, 0x2b02, 0x2b0b, 0x2b09, 0x2b03};

int
mbfiler_unicode2sjis_emoji_kddi(int c, int *s1, int *s2, int *c1, int *c2, mbfl_convert_filter *filter)
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
		} else if ((c >= 0x1F1A5 + 0x41 && c <= 0x1F1A5 + 0x5A) &&
				   (c1s >= 0x1F1A5 + 0x41 && c1s <= 0x1F1A5 + 0x5A)) {
			for (i=0;i<10;i++) {
				if (c1s == 0x1F1A5 + nflags_s[i][0] && c == 0x1F1A5 + nflags_s[i][1]) {
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
			(c >= 0x1F1A5 + 0x41 && c<= 0x1F1A5 + 0x5A)) {
			filter->status = 1;
			filter->cache = c;
			*s1 = -1;
			return 0;
		}		

		if (c == 0x00A9) {
			*s1 = 0x27dc; match = 1;
		} else if (c == 0x00AE) {
			*s1 = 0x27dd; match = 1;
		} else if (c >= mb_tbl_uni_kddi2code2_min && c <= mb_tbl_uni_kddi2code2_max) {
			for (i=0; i < sizeof(mb_tbl_uni_kddi2code2_key)/sizeof(int); i++) {
				if (c >0 && mb_tbl_uni_kddi2code2_key[i] == c) {
					*s1 = mb_tbl_uni_kddi2code2_value[i];
					match = 1;
					break;
				}
			}
		} else if (c >= mb_tbl_uni_kddi2code3_min && c <= mb_tbl_uni_kddi2code3_max) {
			for (i=0; i < sizeof(mb_tbl_uni_kddi2code3_key)/sizeof(int); i++) {
				if (c >0 && mb_tbl_uni_kddi2code3_key[i] == c) {
					*s1 = mb_tbl_uni_kddi2code3_value[i];
					match = 1;
					break;
				}
			}
		}
	}
	
	if (match && *s1 >0) {
		*c1 = *s1/94+0x21;
		*c2 = *s1-94*(*c1-0x21)+0x21;
		*s1 = (*c1 << 8) | *c2;
		*s2 = 1;
	}
	
	return 1;
}

int
mbfiler_unicode2sjis_emoji_sb(int c, int *s1, int *s2, int *c1, int *c2, mbfl_convert_filter *filter)
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
		} else if ((c >= 0x1F1A5 + 0x41 && c <= 0x1F1A5 + 0x5A) &&
				   (c1s >= 0x1F1A5 + 0x41 && c1s <= 0x1F1A5 + 0x5A)) {
			for (i=0;i<10;i++) {
				if (c1s == 0x1F1A5 + nflags_s[i][0] && c == 0x1F1A5 + nflags_s[i][1]) {
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
		if (c == 0x0023 || ( c >= 0x0030 && c<=0x0039) ||
			(c >= 0x1F1A5 + 0x41 && c<= 0x1F1A5 + 0x5A)) {
			filter->status = 1;
			filter->cache = c;
			*s1 = -1;
			return 0;
		}		

		if (c == 0x00A9) {
			*s1 = 0x2855; match = 1;
		} else if (c == 0x00AE) {
			*s1 = 0x2856; match = 1;
		} else if (c >= mb_tbl_uni_sb2code2_min && c <= mb_tbl_uni_sb2code2_max) {
			for (i=0; i < sizeof(mb_tbl_uni_sb2code2_key)/sizeof(int); i++) {
				if (c >0 && mb_tbl_uni_sb2code2_key[i] == c) {
					*s1 = mb_tbl_uni_sb2code2_value[i];
					match = 1;
					break;
				}
			}
		} else if (c >= mb_tbl_uni_sb2code3_min && c <= mb_tbl_uni_sb2code3_max) {
			for (i=0; i < sizeof(mb_tbl_uni_sb2code3_key)/sizeof(int); i++) {
				if (c >0 && mb_tbl_uni_sb2code3_key[i] == c) {
					*s1 = mb_tbl_uni_sb2code3_value[i];
					match = 1;
					break;
				}
			}
		}
	}
	
	if (match && *s1 >0) {
		*c1 = *s1/94+0x21;
		*c2 = *s1-94*(*c1-0x21)+0x21;
		*s1 = (*c1 << 8) | *c2;
		*s2 = 1;
	}

	return 1;
}

/*
 * SJIS-win => wchar
 */
int
mbfl_filt_conv_sjis_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, s1, s2, w;
	int snd = 0;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			if ((filter->from->no_encoding == mbfl_no_encoding_sjis_sb_pua ||
				 filter->from->no_encoding == mbfl_no_encoding_sjis_sb) && c == 0x1b) {
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
						w = mbfiler_sjis_emoji_docomo2unicode(s, &snd);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_kddi) {
						w = mbfiler_sjis_emoji_kddi2unicode(s, &snd);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb) {
						w = mbfiler_sjis_emoji_sb2unicode(s, &snd);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_kddi_pua) {
						mbfiler_sjis_emoji_kddi2unicode_pua(s, &w);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_kddi_pua_b) {
						mbfiler_sjis_emoji_kddi2unicode_pua_b(s, c1, c, &w);
					} else if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb_pua) {
						mbfiler_sjis_emoji2unicode_pua(s, &w);
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
	case 2:
		if ((filter->from->no_encoding == mbfl_no_encoding_sjis_sb_pua ||
			 filter->from->no_encoding == mbfl_no_encoding_sjis_sb) && 
			c == 0x24) {
				filter->cache = c;
				filter->status = 3;
		} else {
			filter->cache = 0;
			filter->status = 0;
			CK((*filter->output_function)(0x1b, filter->data));
		}
		break;

	case 3:
		/* Softbank Emoji: ESC $ [GEFOPQ] X 0x0f */
		if ((filter->from->no_encoding == mbfl_no_encoding_sjis_sb_pua ||
			 filter->from->no_encoding == mbfl_no_encoding_sjis_sb) && 
			((c >= 0x45 && c <= 0x47) || (c >= 0x4f && c <= 0x51))) {
				filter->cache = c;
				filter->status = 4;
		} else {
			filter->cache = 0;
			filter->status = 0;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
		}
		break;

	case 4:
		/* Softbank Emoji Web code: ESC $ [GEFOPQ] X 0x0f */
		w = 0;
		if (filter->from->no_encoding == mbfl_no_encoding_sjis_sb_pua ||
			filter->from->no_encoding == mbfl_no_encoding_sjis_sb) {
			c1 = filter->cache;

			if (c == 0x0f) {
				w = c;
				filter->cache = 0;
				filter->status = 0;				
			} else {
				if  (filter->from->no_encoding == mbfl_no_encoding_sjis_sb_pua) {
					if (c1 == 0x47 && c >= 0x21 && c <= 0x7a) {
						w = c - 0x0021 + 0xe001;
					} else if (c1 == 0x45 && c >= 0x21 && c <= 0x7a) {
						w = c - 0x0021 + 0xe101;
					} else if (c1 == 0x46 && c >= 0x21 && c <= 0x7a) {
						w = c - 0x0021 + 0xe201;
					} else if (c1 == 0x4f && c >= 0x21 && c <= 0x6d) {
						w = c - 0x0021 + 0xe301;
					} else if (c1 == 0x50 && c >= 0x21 && c <= 0x6c) {
						w = c - 0x0021 + 0xe401;
					} else if (c1 == 0x51 && c >= 0x21 && c <= 0x5e) {
						w = c - 0x0021 + 0xe501;
					}
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
					w = mbfiler_sjis_emoji_sb2unicode(s, &snd);
					if (w > 0  && snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}					
				}
				if (w > 0) {
					CK((*filter->output_function)(w, filter->data));
				}
			}
		}

		if (w <= 0) {
			c1 = filter->cache;
			filter->cache = 0;
			filter->status = 0;
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(c1 & 0xff, filter->data));
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
 * wchar => SJIS-win
 */
int
mbfl_filt_conv_wchar_sjis_mobile(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1, s2;
	int sjis_encoded = 0;

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

 	if (filter->to->no_encoding == mbfl_no_encoding_sjis_docomo) {
 		mbfiler_unicode2sjis_emoji_docomo(c, &s1, &s2, &c1, &c2, filter);
 	} else if (filter->to->no_encoding == mbfl_no_encoding_sjis_kddi) {
 		mbfiler_unicode2sjis_emoji_kddi(c, &s1, &s2, &c1, &c2, filter);
 	} else if (filter->to->no_encoding == mbfl_no_encoding_sjis_sb) {
 		mbfiler_unicode2sjis_emoji_sb(c, &s1, &s2, &c1, &c2, filter);
 	} else if (filter->to->no_encoding == mbfl_no_encoding_sjis_kddi_pua) {
 		mbfiler_unicode_pua2sjis_emoji_kddi(c, &s1, &s2, &c1, &c2);
 	} else if (filter->to->no_encoding == mbfl_no_encoding_sjis_docomo_pua ||
			   filter->to->no_encoding == mbfl_no_encoding_sjis_kddi_pua_b ||
			   filter->to->no_encoding == mbfl_no_encoding_sjis_sb_pua) {
 		mbfiler_unicode_pua2sjis_emoji(c, &s1, &s2, &c1, &c2, &sjis_encoded);
 	}

	if (filter->status == 1 && filter->cache > 0) {
		return c;
	}

	if (s1 >= 0) {
		if (s1 < 0x100) { /* latin or kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* kanji */
			if (sjis_encoded) {
				s2 = s1 & 0xff;
				s1 = (s1 >> 8) & 0xff;
			} else {
				c1 = (s1 >> 8) & 0xff;
				c2 = s1 & 0xff;
				SJIS_ENCODE(c1, c2, s1, s2);
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

static int mbfl_filt_ident_sjis_mobile(int c, mbfl_identify_filter *filter)
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


