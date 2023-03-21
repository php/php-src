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
#include "mbfilter_sjis_mobile.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

#include "emoji2uni.h"

const unsigned char mblen_table_sjis_mobile[] = { /* 0x81-0x9F,0xE0-0xFC */
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
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1
};

extern int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n);

static int mbfl_filt_conv_sjis_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_sjis_docomo_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_sjis_docomo(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_sjis_kddi_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_sjis_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_sjis_sb_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_sjis_sb(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_sjis_docomo_aliases[] = {"SJIS-DOCOMO", "shift_jis-imode", "x-sjis-emoji-docomo", NULL};
static const char *mbfl_encoding_sjis_kddi_aliases[] = {"SJIS-KDDI", "shift_jis-kddi", "x-sjis-emoji-kddi", NULL};
static const char *mbfl_encoding_sjis_sb_aliases[] = {"SJIS-SOFTBANK", "shift_jis-softbank", "x-sjis-emoji-softbank", NULL};

const mbfl_encoding mbfl_encoding_sjis_docomo = {
	mbfl_no_encoding_sjis_docomo,
	"SJIS-Mobile#DOCOMO",
	"Shift_JIS",
	mbfl_encoding_sjis_docomo_aliases,
	mblen_table_sjis_mobile,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_docomo_wchar,
	&vtbl_wchar_sjis_docomo,
	mb_sjis_docomo_to_wchar,
	mb_wchar_to_sjis_docomo,
	NULL
};

const mbfl_encoding mbfl_encoding_sjis_kddi = {
	mbfl_no_encoding_sjis_kddi,
	"SJIS-Mobile#KDDI",
	"Shift_JIS",
	mbfl_encoding_sjis_kddi_aliases,
	mblen_table_sjis_mobile,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_kddi_wchar,
	&vtbl_wchar_sjis_kddi,
	mb_sjis_kddi_to_wchar,
	mb_wchar_to_sjis_kddi,
	NULL
};

const mbfl_encoding mbfl_encoding_sjis_sb = {
	mbfl_no_encoding_sjis_sb,
	"SJIS-Mobile#SOFTBANK",
	"Shift_JIS",
	mbfl_encoding_sjis_sb_aliases,
	mblen_table_sjis_mobile,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_sjis_sb_wchar,
	&vtbl_wchar_sjis_sb,
	mb_sjis_sb_to_wchar,
	mb_wchar_to_sjis_sb,
	NULL
};

const struct mbfl_convert_vtbl vtbl_sjis_docomo_wchar = {
	mbfl_no_encoding_sjis_docomo,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mobile_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_docomo = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_docomo,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_sjis_kddi_wchar = {
	mbfl_no_encoding_sjis_kddi,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mobile_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_kddi = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_kddi,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_sjis_sb_wchar = {
	mbfl_no_encoding_sjis_sb,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_sjis_mobile_wchar,
	mbfl_filt_conv_sjis_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_sjis_sb = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_sb,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_sjis_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
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

/* Regional Indicator Unicode codepoints are from 0x1F1E6-0x1F1FF
 * These correspond to the letters A-Z
 * To display the flag emoji for a country, two unicode codepoints are combined,
 * which correspond to the two-letter code for that country
 * This macro converts uppercase ASCII values to Regional Indicator codepoints */
#define NFLAGS(c) (0x1F1A5+(int)(c))

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define SJIS_ENCODE(c1,c2,s1,s2) \
		do { \
			s1 = ((c1 - 1) >> 1) + ((c1) < 0x5F ? 0x71 : 0xB1); \
			s2 = c2; \
			if ((c1) & 1) { \
				if ((c2) < 0x60) { \
					s2--; \
				} \
				s2 += 0x20; \
			} else { \
				s2 += 0x7e; \
			} \
		} while (0)

#define SJIS_DECODE(c1,c2,s1,s2) \
		do { \
			if (c1 < 0xa0) { \
				s1 = ((c1 - 0x81) << 1) + 0x21; \
			} else { \
				s1 = ((c1 - 0xc1) << 1) + 0x21; \
			} \
			s2 = c2; \
			if (c2 < 0x9f) { \
				if (c2 < 0x7f) { \
					s2++; \
				} \
				s2 -= 0x20; \
			} else { \
				s1++; \
				s2 -= 0x7e; \
			} \
		} while (0)

int mbfilter_conv_map_tbl(int c, int *w, const unsigned short map[][3], int n)
{
	for (int i = 0; i < n; i++) {
		if (map[i][0] <= c && c <= map[i][1]) {
			*w = c - map[i][0] + map[i][2];
			return 1;
		}
	}
	return 0;
}

int mbfilter_conv_r_map_tbl(int c, int *w, const unsigned short map[][3], int n)
{
	/* Convert in reverse direction */
	for (int i = 0; i < n; i++) {
		if (map[i][2] <= c && c <= map[i][2] - map[i][0] + map[i][1]) {
			*w = c + map[i][0] - map[i][2];
			return 1;
		}
	}
	return 0;
}

/* number -> (ku*94)+ten value for telephone keypad character */
#define DOCOMO_KEYPAD(n) ((n) == 0 ? 0x296F : (0x2965 + (n)))
#define DOCOMO_KEYPAD_HASH 0x2964

#define EMIT_KEYPAD_EMOJI(c) do { *snd = (c); return 0x20E3; } while(0)

/* Unicode codepoints for emoji are above 0x1F000, but we only store 16-bits
 * in our tables. Therefore, add 0x10000 to recover the true values.
 *
 * Again, for some emoji which are not supported by Unicode, we use codepoints
 * in the Private Use Area above 0xFE000. Again, add 0xF0000 to recover the
 * true value. */
static inline int convert_emoji_cp(int cp)
{
	if (cp > 0xF000)
		return cp + 0x10000;
	else if (cp > 0xE000)
		return cp + 0xF0000;
	return cp;
}

int mbfilter_sjis_emoji_docomo2unicode(int s, int *snd)
{
	/* All three mobile vendors had emoji for numbers on a telephone keypad
	 * Unicode doesn't have those, but it has a combining character which puts
	 * a 'keypad button' around the following character, making it look like
	 * a key on a telephone or keyboard. That combining char is codepoint 0x20E3. */
	if (s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
		if ((s >= DOCOMO_KEYPAD(1) && s <= DOCOMO_KEYPAD(9)) || s == DOCOMO_KEYPAD(0) || s == DOCOMO_KEYPAD_HASH) {
			EMIT_KEYPAD_EMOJI(convert_emoji_cp(mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min]));
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_docomo1[s - mb_tbl_code2uni_docomo1_min]);
		}
	}
	return 0;
}

#define EMIT_FLAG_EMOJI(country) do { *snd = NFLAGS((country)[0]); return NFLAGS((country)[1]); } while(0)

static const char nflags_kddi[6][2] = {"FR", "DE", "IT", "GB", "CN", "KR"};

int mbfilter_sjis_emoji_kddi2unicode(int s, int *snd)
{
	if (s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi1_max) {
		if (s == 0x24C0) { /* Spain */
			EMIT_FLAG_EMOJI("ES");
		} else if (s == 0x24C1) { /* Russia */
			EMIT_FLAG_EMOJI("RU");
		} else if (s >= 0x2545 && s <= 0x254A) {
			EMIT_FLAG_EMOJI(nflags_kddi[s - 0x2545]);
		} else if (s == 0x25BC) {
			EMIT_KEYPAD_EMOJI('#');
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_kddi1[s - mb_tbl_code2uni_kddi1_min]);
		}
	} else if (s >= mb_tbl_code2uni_kddi2_min && s <= mb_tbl_code2uni_kddi2_max) {
		if (s == 0x2750) { /* Japan */
			EMIT_FLAG_EMOJI("JP");
		} else if (s >= 0x27A6 && s <= 0x27AE) {
			EMIT_KEYPAD_EMOJI(s - 0x27A6 + '1');
		} else if (s == 0x27F7) { /* United States */
			EMIT_FLAG_EMOJI("US");
		} else if (s == 0x2830) {
			EMIT_KEYPAD_EMOJI('0');
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_kddi2[s - mb_tbl_code2uni_kddi2_min]);
		}
	}
	return 0;
}

static const char nflags_sb[10][2] = {"JP", "US", "FR", "DE", "IT", "GB", "ES", "RU", "CN", "KR"};

int mbfilter_sjis_emoji_sb2unicode(int s, int *snd)
{
	if (s >= mb_tbl_code2uni_sb1_min && s <= mb_tbl_code2uni_sb1_max) {
		if (s == 0x2817 || (s >= 0x2823 && s <= 0x282C)) {
			EMIT_KEYPAD_EMOJI(mb_tbl_code2uni_sb1[s - mb_tbl_code2uni_sb1_min]);
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_sb1[s - mb_tbl_code2uni_sb1_min]);
		}
	} else if (s >= mb_tbl_code2uni_sb2_min && s <= mb_tbl_code2uni_sb2_max) {
		*snd = 0;
		return convert_emoji_cp(mb_tbl_code2uni_sb2[s - mb_tbl_code2uni_sb2_min]);
	} else if (s >= mb_tbl_code2uni_sb3_min && s <= mb_tbl_code2uni_sb3_max) {
		if (s >= 0x2B02 && s <= 0x2B0B) {
			EMIT_FLAG_EMOJI(nflags_sb[s - 0x2B02]);
		} else {
			*snd = 0;
			return convert_emoji_cp(mb_tbl_code2uni_sb3[s - mb_tbl_code2uni_sb3_min]);
		}
	}
	return 0;
}

int
mbfilter_unicode2sjis_emoji_docomo(int c, int *s1, mbfl_convert_filter *filter)
{
	/* When converting SJIS-Mobile to Unicode, we convert keypad symbol emoji
	 * to a sequence of 2 codepoints, one of which is a combining character which
	 * adds the 'key' image around the other
	 *
	 * In the other direction, look for such sequences and convert them to a
	 * single emoji */
	if (filter->status == 1) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x2964;
			} else if (c1 == '0') {
				*s1 = 0x296F;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x2966 + (c1 - '1');
			}
			return 1;
		} else {
			/* This character wasn't combining character to make keypad symbol,
			 * so pass the previous character through... and proceed to process the
			 * current character as usual
			 * (Single-byte ASCII characters are valid in Shift-JIS...) */
			CK((*filter->output_function)(c1, filter->data));
		}
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status = 1;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x29B5;
		return 1;
	} else if (c == 0x00AE) { /* Registered sign */
		*s1 = 0x29BA;
		return 1;
	} else if (c >= mb_tbl_uni_docomo2code2_min && c <= mb_tbl_uni_docomo2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_docomo2code2_key, mb_tbl_uni_docomo2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_docomo2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_docomo2code3_min && c <= mb_tbl_uni_docomo2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_docomo2code3_key, mb_tbl_uni_docomo2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_docomo2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_docomo2code5_min && c <= mb_tbl_uni_docomo2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_docomo2code5_key, mb_tbl_uni_docomo2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_docomo2code5_val[i];
			return 1;
		}
	}
	return 0;
}

int mbfilter_unicode2sjis_emoji_kddi(int c, int *s1, mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x25BC;
			} else if (c1 == '0') {
				*s1 = 0x2830;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x27a6 + (c1 - '1');
			}
			return 1;
		} else {
			CK((*filter->output_function)(c1, filter->data));
		}
	} else if (filter->status == 2) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c >= NFLAGS('B') && c <= NFLAGS('U')) { /* B for GB, U for RU */
			for (int i = 0; i < 10; i++) {
				if (c1 == NFLAGS(nflags_s[i][0]) && c == NFLAGS(nflags_s[i][1])) {
					*s1 = nflags_code_kddi[i];
					return 1;
				}
			}
		}

		/* If none of the KDDI national flag emoji matched, then we have no way
		 * to convert the previous codepoint... */
		CK(mbfl_filt_conv_illegal_output(c1, filter));
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status = 1;
		filter->cache = c;
		return 0;
	} else if (c >= NFLAGS('C') && c <= NFLAGS('U')) { /* C for CN, U for US */
		filter->status = 2;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x27DC;
		return 1;
	} else if (c == 0xAE) { /* Registered sign */
		*s1 = 0x27DD;
		return 1;
	} else if (c >= mb_tbl_uni_kddi2code2_min && c <= mb_tbl_uni_kddi2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code3_min && c <= mb_tbl_uni_kddi2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_kddi2code5_min && c <= mb_tbl_uni_kddi2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_kddi2code5_val[i];
			return 1;
		}
	}
	return 0;
}

int mbfilter_unicode2sjis_emoji_sb(int c, int *s1, mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c == 0x20E3) {
			if (c1 == '#') {
				*s1 = 0x2817;
			} else if (c1 == '0') {
				*s1 = 0x282c;
			} else { /* Previous character was '1'-'9' */
				*s1 = 0x2823 + (c1 - '1');
			}
			return 1;
		} else {
			CK((*filter->output_function)(c1, filter->data));
		}
	} else if (filter->status == 2) {
		int c1 = filter->cache;
		filter->cache = filter->status = 0;
		if (c >= NFLAGS('B') && c <= NFLAGS('U')) { /* B for GB, U for RU */
			for (int i = 0; i < 10; i++) {
				if (c1 == NFLAGS(nflags_s[i][0]) && c == NFLAGS(nflags_s[i][1])) {
					*s1 = nflags_code_sb[i];
					return 1;
				}
			}
		}

		/* If none of the SoftBank national flag emoji matched, then we have no way
		 * to convert the previous codepoint... */
		CK(mbfl_filt_conv_illegal_output(c1, filter));
	}

	if (c == '#' || (c >= '0' && c <= '9')) {
		filter->status = 1;
		filter->cache = c;
		return 0;
	} else if (c >= NFLAGS('C') && c <= NFLAGS('U')) { /* C for CN, U for US */
		filter->status = 2;
		filter->cache = c;
		return 0;
	}

	if (c == 0xA9) { /* Copyright sign */
		*s1 = 0x2855;
		return 1;
	} else if (c == 0xAE) { /* Registered sign */
		*s1 = 0x2856;
		return 1;
	} else if (c >= mb_tbl_uni_sb2code2_min && c <= mb_tbl_uni_sb2code2_max) {
		int i = mbfl_bisec_srch2(c, mb_tbl_uni_sb2code2_key, mb_tbl_uni_sb2code2_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_sb2code2_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_sb2code3_min && c <= mb_tbl_uni_sb2code3_max) {
		int i = mbfl_bisec_srch2(c - 0x10000, mb_tbl_uni_sb2code3_key, mb_tbl_uni_sb2code3_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_sb2code3_value[i];
			return 1;
		}
	} else if (c >= mb_tbl_uni_sb2code5_min && c <= mb_tbl_uni_sb2code5_max) {
		int i = mbfl_bisec_srch2(c - 0xF0000, mb_tbl_uni_sb2code5_key, mb_tbl_uni_sb2code5_len);
		if (i >= 0) {
			*s1 = mb_tbl_uni_sb2code5_val[i];
			return 1;
		}
	}
	return 0;
}

int mbfl_filt_conv_sjis_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, s1, s2, w, snd = 0;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* ASCII */
			if (filter->from == &mbfl_encoding_sjis_sb && c == 0x1B) {
				/* ESC; escape sequences were used on older SoftBank phones for emoji */
				filter->cache = c;
				filter->status = 2;
			} else {
				CK((*filter->output_function)(c, filter->data));
			}
		} else if (c > 0xA0 && c < 0xE0) { /* Kana */
			CK((*filter->output_function)(0xFEC0 + c, filter->data));
		} else if (c > 0x80 && c < 0xFD && c != 0xA0) { /* Kanji, first byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* Kanji, second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c >= 0x40 && c <= 0xFC && c != 0x7F) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = ((s1 - 0x21) * 94) + s2 - 0x21;
			if (s <= 137) {
				if (s == 31) {
					w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xFF5E; /* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225; /* PARALLEL TO */
				} else if (s == 60) {
					w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xFFE0; /* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xFFE1; /* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xFFE2; /* FULLWIDTH NOT SIGN */
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

				/* Emoji */
				if (filter->from == &mbfl_encoding_sjis_docomo && s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
					w = mbfilter_sjis_emoji_docomo2unicode(s, &snd);
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				} else if (filter->from == &mbfl_encoding_sjis_kddi && s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi2_max) {
					w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				} else if (filter->from == &mbfl_encoding_sjis_sb && s >= mb_tbl_code2uni_sb1_min && s <= mb_tbl_code2uni_sb3_max) {
					w = mbfilter_sjis_emoji_sb2unicode(s, &snd);
					if (snd > 0) {
						CK((*filter->output_function)(snd, filter->data));
					}
				}

				if (w == 0) {
					if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) { /* vendor ext3 (115ku - 119ku) */
						w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
					} else if (s >= (94*94) && s < (114*94)) { /* user (95ku - 114ku) */
						w = s - (94*94) + 0xe000;
					}
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

	/* ESC: Softbank Emoji */
	case 2:
		if (c == '$') {
			filter->cache = c;
			filter->status++;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			filter->status = filter->cache = 0;
		}
		break;

	/* ESC $: Softbank Emoji */
	case 3:
		if ((c >= 'E' && c <= 'G') || (c >= 'O' && c <= 'Q')) {
			filter->cache = c;
			filter->status++;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			filter->status = filter->cache = 0;
		}
		break;

	/* ESC $ [GEFOPQ]: Softbank Emoji */
	case 4:
		c1 = filter->cache;
		if (c == 0xF) { /* Terminate sequence of emoji */
			filter->status = filter->cache = 0;
			return 0;
		} else {
			if (c1 == 'G' && c >= 0x21 && c <= 0x7a) {
				s1 = (0x91 - 0x21) * 94;
			} else if (c1 == 'E' && c >= 0x21 && c <= 0x7A) {
				s1 = (0x8D - 0x21) * 94;
			} else if (c1 == 'F' && c >= 0x21 && c <= 0x7A) {
				s1 = (0x8E - 0x21) * 94;
			} else if (c1 == 'O' && c >= 0x21 && c <= 0x6D) {
				s1 = (0x92 - 0x21) * 94;
			} else if (c1 == 'P' && c >= 0x21 && c <= 0x6C) {
				s1 = (0x95 - 0x21) * 94;
			} else if (c1 == 'Q' && c >= 0x21 && c <= 0x5E) {
				s1 = (0x96 - 0x21) * 94;
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->status = filter->cache = 0;
				return 0;
			}

			w = mbfilter_sjis_emoji_sb2unicode(s1 + c - 0x21, &snd);
			if (w > 0) {
				if (snd > 0) {
					CK((*filter->output_function)(snd, filter->data));
				}
				CK((*filter->output_function)(w, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->status = filter->cache = 0;
			}
		}
	}

	return 0;
}

static int mbfl_filt_conv_sjis_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status && filter->status != 4) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_sjis_mobile(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s1 = 0, s2 = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xE000 && c < (0xE000 + 20*94)) {
		/* Private User Area (95ku - 114ku) */
		s1 = c - 0xE000;
		c1 = (s1 / 94) + 0x7F;
		c2 = (s1 % 94) + 0x21;
		s1 = (c1 << 8) | c2;
		s2 = 1;
	}

	if (s1 <= 0) {
		if (c == 0xA5) { /* YEN SIGN */
			s1 = 0x216F; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xFF3c) { /* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0x2225) { /* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215D;
		} else if (c == 0xFFE0) { /* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xFFE1) { /* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xFFE2) { /* FULLWIDTH NOT SIGN */
			s1 = 0x224C;
		}
	}

	if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
		s1 = -1;

		/* CP932 vendor ext1 (13ku) */
		for (c1 = 0; c1 < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; c1++) {
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = (((c1 / 94) + 0x2D) << 8) + (c1 % 94) + 0x21;
				break;
			}
		}

		if (s1 <= 0) {
			/* CP932 vendor ext2 (115ku - 119ku) */
			for (c1 = 0; c1 < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; c1++) {
				if (c == cp932ext2_ucs_table[c1]) {
					s1 = (((c1 / 94) + 0x79) << 8) + (c1 % 94) + 0x21;
					break;
				}
			}
		}

		if (c == 0) {
			s1 = 0;
		}
	}

	if ((filter->to == &mbfl_encoding_sjis_docomo && mbfilter_unicode2sjis_emoji_docomo(c, &s1, filter) > 0) ||
		  (filter->to == &mbfl_encoding_sjis_kddi   && mbfilter_unicode2sjis_emoji_kddi(c, &s1, filter) > 0) ||
		  (filter->to == &mbfl_encoding_sjis_sb     && mbfilter_unicode2sjis_emoji_sb(c, &s1, filter) > 0)) {
		s1 = (((s1 / 94) + 0x21) << 8) | ((s1 % 94) + 0x21);
 	}

	if (filter->status) {
		return 0;
	}

	if (s1 >= 0) {
		if (s1 < 0x100) { /* Latin/Kana */
			CK((*filter->output_function)(s1, filter->data));
		} else { /* Kanji */
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data));
			CK((*filter->output_function)(s2, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_sjis_mobile_flush(mbfl_convert_filter *filter)
{
	int c1 = filter->cache;
	if (filter->status == 1 && (c1 == '#' || (c1 >= '0' && c1 <= '9'))) {
		filter->cache = filter->status = 0;
		CK((*filter->output_function)(c1, filter->data));
	} else if (filter->status == 2) {
		/* First of a pair of Regional Indicator codepoints came at the end of a string */
		filter->cache = filter->status = 0;
		CK(mbfl_filt_conv_illegal_output(c1, filter));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_sjis_docomo_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	/* Leave one extra space available in output buffer, since some iterations of
	 * main loop (below) may emit two wchars */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c < 0xFD && c != 0xA0) {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F) {
				uint32_t w = 0;
				unsigned int s1, s2;
				SJIS_DECODE(c, c2, s1, s2);
				unsigned int s = ((s1 - 0x21) * 94) + s2 - 0x21;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (!w) {
					if (s >= mb_tbl_code2uni_docomo1_min && s <= mb_tbl_code2uni_docomo1_max) {
						int snd = 0;
						w = mbfilter_sjis_emoji_docomo2unicode(s, &snd);
						if (snd) {
							*out++ = snd;
						}
					} else if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}

					if (!w) {
						if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
							w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
						} else if (s >= (94*94) && s < (114*94)) {
							w = s - (94*94) + 0xE000;
						}
					}
				}

				*out++ = w ? w : MBFL_BAD_INPUT;
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

static void mb_wchar_to_sjis_docomo(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len + (buf->state ? 1 : 0));

	uint32_t w;
	unsigned int s = 0;

	if (buf->state) {
		/* Continue what we were doing on the previous call */
		w = buf->state;
		buf->state = 0;
		if (len) {
			goto reprocess_wchar;
		} else {
			goto emit_output;
		}
	}

	while (len--) {
		w = *in++;
reprocess_wchar:
		s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = w - 0xE000;
			s = (((s / 94) + 0x7F) << 8) | ((s % 94) + 0x21);
			goto process_emoji;
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (w && (!s || s >= 0x8080)) {
			s = 0;

			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}

			for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
				if (w == cp932ext2_ucs_table[i]) {
					s = (((i / 94) + 0x79) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}
		}

process_emoji:
		/* When converting SJIS-Mobile to Unicode, we convert keypad symbol emoji
		 * to a sequence of 2 codepoints, one of which is a combining character which
		 * adds the 'key' image around the other
		 *
		 * In the other direction, look for such sequences and convert them to a
		 * single emoji */
		if (w == '#' || (w >= '0' && w <= '9')) {
			if (!len) {
				if (end) {
					goto emit_output;
				} else {
					/* If we are at the end of the current buffer of codepoints, but another
					 * buffer is coming, then remember that we have to reprocess `w` */
					buf->state = w;
					break;
				}
			}
			uint32_t w2 = *in++; len--;
			if (w2 == 0x20E3) {
				if (w == '#') {
					s = 0x2964;
				} else if (w == '0') {
					s = 0x296F;
				} else { /* Previous character was '1'-'9' */
					s = 0x2966 + (w - '1');
				}
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			} else {
				in--; len++;
			}
		} else if (w == 0xA9) { /* Copyright sign */
			s = (((0x29B5 / 94) + 0x21) << 8) | ((0x29B5 % 94) + 0x21);
		} else if (w == 0xAE) { /* Registered sign */
			s = (((0x29BA / 94) + 0x21) << 8) | ((0x29BA % 94) + 0x21);
		} else if (w >= mb_tbl_uni_docomo2code2_min && w <= mb_tbl_uni_docomo2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_docomo2code2_key, mb_tbl_uni_docomo2code2_len);
			if (i >= 0) {
				s = mb_tbl_uni_docomo2code2_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_docomo2code3_min && w <= mb_tbl_uni_docomo2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_docomo2code3_key, mb_tbl_uni_docomo2code3_len);
			if (i >= 0) {
				s = mb_tbl_uni_docomo2code3_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_docomo2code5_min && w <= mb_tbl_uni_docomo2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_docomo2code5_key, mb_tbl_uni_docomo2code5_len);
			if (i >= 0) {
				s = mb_tbl_uni_docomo2code5_val[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		}

emit_output:
		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_docomo);
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

static size_t mb_sjis_kddi_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c < 0xFD && c != 0xA0) {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F) {
				uint32_t w = 0;
				unsigned int s1, s2;
				SJIS_DECODE(c, c2, s1, s2);
				unsigned int s = ((s1 - 0x21) * 94) + s2 - 0x21;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (!w) {
					if (s >= mb_tbl_code2uni_kddi1_min && s <= mb_tbl_code2uni_kddi2_max) {
						int snd = 0;
						w = mbfilter_sjis_emoji_kddi2unicode(s, &snd);
						if (snd) {
							*out++ = snd;
						}
					} else if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}

					if (!w) {
						if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
							w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
						} else if (s >= (94*94) && s < (114*94)) {
							w = s - (94*94) + 0xE000;
						}
					}
				}

				*out++ = w ? w : MBFL_BAD_INPUT;
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

static void mb_wchar_to_sjis_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len + (buf->state ? 1 : 0));

	uint32_t w;
	unsigned int s = 0;

	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		if (len) {
			goto reprocess_wchar;
		} else {
			goto emit_output;
		}
	}

	while (len--) {
		w = *in++;
reprocess_wchar:
		s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = w - 0xE000;
			s = (((s / 94) + 0x7F) << 8) | ((s % 94) + 0x21);
			goto process_emoji;
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3c) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (w && (!s || s >= 0x8080)) {
			s = 0;

			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}

			for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
				if (w == cp932ext2_ucs_table[i]) {
					s = (((i / 94) + 0x79) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}
		}

process_emoji:
		if (w == '#' || (w >= '0' && w <= '9')) {
			if (!len) {
				if (end) {
					goto emit_output;
				} else {
					/* If we are at the end of the current buffer of codepoints, but another
					 * buffer is coming, then remember that we have to reprocess `w` */
					buf->state = w;
					break;
				}
			}
			uint32_t w2 = *in++; len--;
			if (w2 == 0x20E3) {
				if (w == '#') {
					s = 0x25BC;
				} else if (w == '0') {
					s = 0x2830;
				} else { /* Previous character was '1'-'9' */
					s = 0x27A6 + (w - '1');
				}
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			} else {
				in--; len++;
			}
		} else if (w >= NFLAGS('C') && w <= NFLAGS('U')) { /* C for CN, U for US */
			if (!len) {
				if (end) {
					MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_kddi);
				} else {
					/* Reprocess `w` when this function is called again with another buffer
					 * of wchars */
					buf->state = w;
				}
				break;
			}
			uint32_t w2 = *in++; len--;
			if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
				for (int i = 0; i < 10; i++) {
					if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
						s = nflags_code_kddi[i];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto emit_output;
					}
				}
			}
			in--; len++;
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_kddi);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			continue;
		} else if (w == 0xA9) { /* Copyright sign */
			s = (((0x27DC / 94) + 0x21) << 8) | ((0x27DC % 94) + 0x21);
		} else if (w == 0xAE) { /* Registered sign */
			s = (((0x27DD / 94) + 0x21) << 8) | ((0x27DD % 94) + 0x21);
		} else if (w >= mb_tbl_uni_kddi2code2_min && w <= mb_tbl_uni_kddi2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
			if (i >= 0) {
				s = mb_tbl_uni_kddi2code2_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_kddi2code3_min && w <= mb_tbl_uni_kddi2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
			if (i >= 0) {
				s = mb_tbl_uni_kddi2code3_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_kddi2code5_min && w <= mb_tbl_uni_kddi2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
			if (i >= 0) {
				s = mb_tbl_uni_kddi2code5_val[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		}

emit_output:
		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_kddi);
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

static size_t mb_sjis_sb_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	if (*state) {
		goto softbank_emoji_escapes;
	}

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			/* Escape sequence */
			if (p == e || *p++ != '$' || p == e) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}
			unsigned char c2 = *p++;
			if ((c2 < 'E' || c2 > 'G') && (c2 < 'O' || c2 > 'Q')) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}
			/* Escape sequence was valid, next should be a series of specially
			 * encoded Softbank emoji */
			*state = c2;

softbank_emoji_escapes:
			while (p < e && out < limit) {
				c = *p++;
				if (c == 0xF) {
					*state = 0;
					break;
				}
				unsigned int s = 0;
				if (*state == 'G' && c >= 0x21 && c <= 0x7A) {
					s = (0x91 - 0x21) * 94;
				} else if (*state == 'E' && c >= 0x21 && c <= 0x7A) {
					s = (0x8D - 0x21) * 94;
				} else if (*state == 'F' && c >= 0x21 && c <= 0x7A) {
					s = (0x8E - 0x21) * 94;
				} else if (*state == 'O' && c >= 0x21 && c <= 0x6D) {
					s = (0x92 - 0x21) * 94;
				} else if (*state == 'P' && c >= 0x21 && c <= 0x6C) {
					s = (0x95 - 0x21) * 94;
				} else if (*state == 'Q' && c >= 0x21 && c <= 0x5E) {
					s = (0x96 - 0x21) * 94;
				} else {
					*out++ = MBFL_BAD_INPUT;
					*state = 0;
					break;
				}

				int snd = 0;
				uint32_t w = mbfilter_sjis_emoji_sb2unicode(s + c - 0x21, &snd);
				if (w) {
					if (snd) {
						*out++ = snd;
					}
					*out++ = w;
				} else {
					*out++ = MBFL_BAD_INPUT;
					*state = 0;
					break;
				}
			}
		} else if (c <= 0x7F) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* Kana */
			*out++ = 0xFEC0 + c;
		} else if (c > 0x80 && c < 0xFD && c != 0xA0) {
			/* Kanji */
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;

			if (c2 >= 0x40 && c2 <= 0xFC && c2 != 0x7F) {
				uint32_t w = 0;
				unsigned int s1, s2;
				SJIS_DECODE(c, c2, s1, s2);
				unsigned int s = ((s1 - 0x21) * 94) + s2 - 0x21;

				if (s <= 137) {
					if (s == 31) {
						w = 0xFF3C; /* FULLWIDTH REVERSE SOLIDUS */
					} else if (s == 32) {
						w = 0xFF5E; /* FULLWIDTH TILDE */
					} else if (s == 33) {
						w = 0x2225; /* PARALLEL TO */
					} else if (s == 60) {
						w = 0xFF0D; /* FULLWIDTH HYPHEN-MINUS */
					} else if (s == 80) {
						w = 0xFFE0; /* FULLWIDTH CENT SIGN */
					} else if (s == 81) {
						w = 0xFFE1; /* FULLWIDTH POUND SIGN */
					} else if (s == 137) {
						w = 0xFFE2; /* FULLWIDTH NOT SIGN */
					}
				}

				if (!w) {
					if (s >= mb_tbl_code2uni_sb1_min && s <= mb_tbl_code2uni_sb3_max) {
						int snd = 0;
						w = mbfilter_sjis_emoji_sb2unicode(s, &snd);
						if (snd) {
							*out++ = snd;
						}
					} else if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					}

					if (!w) {
						if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
							w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
						} else if (s >= (94*94) && s < (114*94)) {
							w = s - (94*94) + 0xE000;
						}
					}
				}

				*out++ = w ? w : MBFL_BAD_INPUT;
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

static void mb_wchar_to_sjis_sb(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len + (buf->state ? 1 : 0));

	uint32_t w;
	unsigned int s = 0;

	if (buf->state) {
		w = buf->state;
		buf->state = 0;
		if (len) {
			goto reprocess_wchar;
		} else {
			goto emit_output;
		}
	}

	while (len--) {
		w = *in++;
reprocess_wchar:
		s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		} else if (w >= 0xE000 && w < (0xE000 + 20*94)) {
			/* Private User Area (95ku - 114ku) */
			s = w - 0xE000;
			s = (((s / 94) + 0x7F) << 8) | ((s % 94) + 0x21);
			goto process_emoji;
		}

		if (!s) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x216F; /* FULLWIDTH YEN SIGN */
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			}
		}

		if (w && (!s || s >= 0x8080)) {
			s = 0;

			for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
				if (w == cp932ext1_ucs_table[i]) {
					s = (((i / 94) + 0x2D) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}

			for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
				if (w == cp932ext2_ucs_table[i]) {
					s = (((i / 94) + 0x79) << 8) + (i % 94) + 0x21;
					goto process_emoji;
				}
			}
		}

process_emoji:
		if (w == '#' || (w >= '0' && w <= '9')) {
			if (!len) {
				if (end) {
					goto emit_output;
				} else {
					/* If we are at the end of the current buffer of codepoints, but another
					 * buffer is coming, then remember that we have to reprocess `w` */
					buf->state = w;
					break;
				}
			}
			uint32_t w2 = *in++; len--;
			if (w2 == 0x20E3) {
				if (w == '#') {
					s = 0x2817;
				} else if (w == '0') {
					s = 0x282c;
				} else { /* Previous character was '1'-'9' */
					s = 0x2823 + (w - '1');
				}
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			} else {
				in--; len++;
			}
		} else if (w >= NFLAGS('C') && w <= NFLAGS('U')) { /* C for CN, U for US */
			if (!len) {
				if (end) {
					MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_sb);
				} else {
					/* Reprocess `w` when this function is called again with
					 * another buffer of wchars */
					buf->state = w;
				}
				break;
			}
			uint32_t w2 = *in++; len--;
			if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
				for (int i = 0; i < 10; i++) {
					if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
						s = nflags_code_sb[i];
						s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
						goto emit_output;
					}
				}
			}
			in--; len++;
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_sb);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			continue;
		} else if (w == 0xA9) { /* Copyright sign */
			s = (((0x2855 / 94) + 0x21) << 8) | ((0x2855 % 94) + 0x21);
		} else if (w == 0xAE) { /* Registered sign */
			s = (((0x2856 / 94) + 0x21) << 8) | ((0x2856 % 94) + 0x21);
		} else if (w >= mb_tbl_uni_sb2code2_min && w <= mb_tbl_uni_sb2code2_max) {
			int i = mbfl_bisec_srch2(w, mb_tbl_uni_sb2code2_key, mb_tbl_uni_sb2code2_len);
			if (i >= 0) {
				s = mb_tbl_uni_sb2code2_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_sb2code3_min && w <= mb_tbl_uni_sb2code3_max) {
			int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_sb2code3_key, mb_tbl_uni_sb2code3_len);
			if (i >= 0) {
				s = mb_tbl_uni_sb2code3_value[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		} else if (w >= mb_tbl_uni_sb2code5_min && w <= mb_tbl_uni_sb2code5_max) {
			int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_sb2code5_key, mb_tbl_uni_sb2code5_len);
			if (i >= 0) {
				s = mb_tbl_uni_sb2code5_val[i];
				s = (((s / 94) + 0x21) << 8) | ((s % 94) + 0x21);
			}
		}

emit_output:
		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_sjis_sb);
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
