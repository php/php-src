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
 * the source code included in this files was separated from mbfilter_cp936.c
 * by rui hirokawa <hirokawa@php.net> on 11 Aug 2011.
 *
 */

#include "mbfilter.h"
#include "mbfilter_gb18030.h"

#include "unicode_table_cp936.h"
#include "unicode_table_gb18030.h"

static int mbfl_filt_conv_gb18030_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_gb18030_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_gb18030(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_gb18030_aliases[] = {"gb-18030", "gb-18030-2000", NULL};

const mbfl_encoding mbfl_encoding_gb18030 = {
	mbfl_no_encoding_gb18030,
	"GB18030",
	"GB18030",
	mbfl_encoding_gb18030_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_gb18030_wchar,
	&vtbl_wchar_gb18030,
	mb_gb18030_to_wchar,
	mb_wchar_to_gb18030,
	NULL
};

const struct mbfl_convert_vtbl vtbl_gb18030_wchar = {
	mbfl_no_encoding_gb18030,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_gb18030_wchar,
	mbfl_filt_conv_gb18030_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_gb18030 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_gb18030,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_gb18030,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/* `tbl` contains inclusive ranges, each represented by a pair of unsigned shorts */
int mbfl_bisec_srch(int w, const unsigned short *tbl, int n)
{
	int l = 0, r = n-1;
	while (l <= r) {
		int probe = (l + r) >> 1;
		unsigned short lo = tbl[2 * probe], hi = tbl[(2 * probe) + 1];
		if (w < lo) {
			r = probe - 1;
		} else if (w > hi) {
			l = probe + 1;
		} else {
			return probe;
		}
	}
	return -1;
}

/* `tbl` contains single values, not ranges */
int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n)
{
	int l = 0, r = n-1;
	while (l <= r) {
		int probe = (l + r) >> 1;
		unsigned short val = tbl[probe];
		if (w < val) {
			r = probe - 1;
		} else if (w > val) {
			l = probe + 1;
		} else {
			return probe;
		}
	}
	return -1;
}

int mbfl_filt_conv_gb18030_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, c3, w = -1;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0x80 && c < 0xff) { /* dbcs/qbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs/qbcs second byte */
		c1 = filter->cache;
		filter->status = 0;

		if (c1 >= 0x81 && c1 <= 0x84 && c >= 0x30 && c <= 0x39) {
			/* 4 byte range: Unicode BMP */
			filter->status = 2;
			filter->cache = (c1 << 8) | c;
			return 0;
		} else if (c1 >= 0x90 && c1 <= 0xe3 && c >= 0x30 && c <= 0x39) {
			/* 4 byte range: Unicode 16 planes */
			filter->status = 2;
			filter->cache = (c1 << 8) | c;
			return 0;
		} else if (((c1 >= 0xaa && c1 <= 0xaf) || (c1 >= 0xf8 && c1 <= 0xfe)) && (c >= 0xa1 && c <= 0xfe)) {
			/* UDA part 1,2: U+E000-U+E4C5 */
			w = 94*(c1 >= 0xf8 ? c1 - 0xf2 : c1 - 0xaa) + (c - 0xa1) + 0xe000;
			CK((*filter->output_function)(w, filter->data));
		} else if (c1 >= 0xa1 && c1 <= 0xa7 && c >= 0x40 && c < 0xa1 && c != 0x7f) {
			/* UDA part3 : U+E4C6-U+E765*/
			w = 96*(c1 - 0xa1) + c - (c >= 0x80 ? 0x41 : 0x40) + 0xe4c6;
			CK((*filter->output_function)(w, filter->data));
		}

		c2 = (c1 << 8) | c;

		if (w <= 0 &&
			((c2 >= 0xa2ab && c2 <= 0xa9f0 + (0xe80f-0xe801)) ||
			 (c2 >= 0xd7fa && c2 <= 0xd7fa + (0xe814-0xe810)) ||
			 (c2 >= 0xfe50 && c2 <= 0xfe80 + (0xe864-0xe844)))) {
			for (k = 0; k < mbfl_gb18030_pua_tbl_max; k++) {
				if (c2 >= mbfl_gb18030_pua_tbl[k][2] && c2 <= mbfl_gb18030_pua_tbl[k][2] + mbfl_gb18030_pua_tbl[k][1] - mbfl_gb18030_pua_tbl[k][0]) {
					w = c2 - mbfl_gb18030_pua_tbl[k][2] + mbfl_gb18030_pua_tbl[k][0];
					CK((*filter->output_function)(w, filter->data));
					break;
				}
			}
		}

		if (w <= 0) {
			if ((c1 >= 0xa1 && c1 <= 0xa9 && c >= 0xa1 && c <= 0xfe) ||
				(c1 >= 0xb0 && c1 <= 0xf7 && c >= 0xa1 && c <= 0xfe) ||
				(c1 >= 0x81 && c1 <= 0xa0 && c >= 0x40 && c <= 0xfe && c != 0x7f) ||
				(c1 >= 0xaa && c1 <= 0xfe && c >= 0x40 && c <= 0xa0 && c != 0x7f) ||
				(c1 >= 0xa8 && c1 <= 0xa9 && c >= 0x40 && c <= 0xa0 && c != 0x7f)) {
				w = (c1 - 0x81)*192 + c - 0x40;
				ZEND_ASSERT(w < cp936_ucs_table_size);
				CK((*filter->output_function)(cp936_ucs_table[w], filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

	case 2: /* qbcs third byte */
		c1 = (filter->cache >> 8) & 0xff;
		c2 = filter->cache & 0xff;
		filter->status = filter->cache = 0;
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) && c2 >= 0x30 && c2 <= 0x39 && c >= 0x81 && c <= 0xfe) {
			filter->cache = (c1 << 16) | (c2 << 8) | c;
			filter->status = 3;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
 		break;

	case 3: /* qbcs fourth byte */
		c1 = (filter->cache >> 16) & 0xff;
		c2 = (filter->cache >> 8) & 0xff;
		c3 = filter->cache & 0xff;
		filter->status = filter->cache = 0;
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) && c2 >= 0x30 && c2 <= 0x39 && c3 >= 0x81 && c3 <= 0xfe && c >= 0x30 && c <= 0x39) {
			if (c1 >= 0x90 && c1 <= 0xe3) {
				w = ((((c1 - 0x90)*10 + (c2 - 0x30))*126 + (c3 - 0x81)))*10 + (c - 0x30) + 0x10000;
				if (w > 0x10FFFF) {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
					return 0;
				}
			} else { /* Unicode BMP */
				w = (((c1 - 0x81)*10 + (c2 - 0x30))*126 + (c3 - 0x81))*10 + (c - 0x30);
				if (w >= 0 && w <= 39419) {
					k = mbfl_bisec_srch(w, mbfl_gb2uni_tbl, mbfl_gb_uni_max);
					w += mbfl_gb_uni_ofst[k];
				} else {
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
					return 0;
				}
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

static int mbfl_filt_conv_gb18030_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* multi-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_gb18030(int c, mbfl_convert_filter *filter)
{
	int k, k1, k2;
	int c1, s = 0, s1 = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		if (c == 0x01f9) {
			s = 0xa8bf;
		} else {
			s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
		}
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		if (c == 0x20ac) { /* euro-sign */
			s = 0xa2e3;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= ucs_ci_cp936_table_min && c < ucs_ci_cp936_table_max) {
		/* U+F900-FA2F CJK Compatibility Ideographs */
		if (c == 0xf92c) {
			s = 0xfd9c;
		} else if (c == 0xf979) {
			s = 0xfd9d;
		} else if (c == 0xf995) {
			s = 0xfd9e;
		} else if (c == 0xf9e7) {
			s = 0xfd9f;
		} else if (c == 0xf9f1) {
			s = 0xfda0;
		} else if (c >= 0xfa0c && c <= 0xfa29) {
			s = ucs_ci_s_cp936_table[c - 0xfa0c];
		}
	} else if (c >= ucs_cf_cp936_table_min && c < ucs_cf_cp936_table_max) {
		/* FE30h CJK Compatibility Forms  */
		s = ucs_cf_cp936_table[c - ucs_cf_cp936_table_min];
	} else if (c >= ucs_sfv_cp936_table_min && c < ucs_sfv_cp936_table_max) {
		/* U+FE50-FE6F Small Form Variants */
		s = ucs_sfv_cp936_table[c - ucs_sfv_cp936_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		/* U+FF00-FFFF HW/FW Forms */
		if (c == 0xff04) {
			s = 0xa1e7;
		} else if (c == 0xff5e) {
			s = 0xa1ab;
		} else if (c >= 0xff01 && c <= 0xff5d) {
			s = c - 0xff01 + 0xa3a1;
		} else if (c >= 0xffe0 && c <= 0xffe5) {
			s = ucs_hff_s_cp936_table[c-0xffe0];
		}
	}

	/* While GB18030 and CP936 are very similar, some mappings are different between these encodings;
	 * do a binary search in a table of differing codepoints to see if we have one */
	if (s <= 0 && c >= mbfl_gb18030_c_tbl_key[0] && c <= mbfl_gb18030_c_tbl_key[mbfl_gb18030_c_tbl_max-1]) {
		k1 = mbfl_bisec_srch2(c, mbfl_gb18030_c_tbl_key, mbfl_gb18030_c_tbl_max);
		if (k1 >= 0) {
			s = mbfl_gb18030_c_tbl_val[k1];
		}
	}

	if (c >= 0xe000 && c <= 0xe864) { /* PUA */
		if (c < 0xe766) {
			if (c < 0xe4c6) {
				c1 = c - 0xe000;
				s = (c1 % 94) + 0xa1;
				c1 /= 94;
				s |= (c1 < 0x06 ? c1 + 0xaa : c1 + 0xf2) << 8;
			} else {
				c1 = c - 0xe4c6;
				s = ((c1 / 96) + 0xa1) << 8;
				c1 %= 96;
				s |= c1 + (c1 >= 0x3f ? 0x41 : 0x40);
			}
		} else {
			/* U+E766..U+E864 */
			k1 = 0;
			k2 = mbfl_gb18030_pua_tbl_max;
			while (k1 < k2) {
				k = (k1 + k2) >> 1;
				if (c < mbfl_gb18030_pua_tbl[k][0]) {
					k2 = k;
				} else if (c > mbfl_gb18030_pua_tbl[k][1]) {
					k1 = k + 1;
				} else {
					s = c - mbfl_gb18030_pua_tbl[k][0] + mbfl_gb18030_pua_tbl[k][2];
					break;
				}
			}
		}
	}

	/* If we have not yet found a suitable mapping for this codepoint, it requires a 4-byte code */
	if (s <= 0 && c >= 0x0080 && c <= 0xffff) {
		/* BMP */
		s = mbfl_bisec_srch(c, mbfl_uni2gb_tbl, mbfl_gb_uni_max);
		if (s >= 0) {
			c1 = c - mbfl_gb_uni_ofst[s];
			s = (c1 % 10) + 0x30;
			c1 /= 10;
			s |= ((c1 % 126) + 0x81) << 8;
			c1 /= 126;
			s |= ((c1 % 10) + 0x30) << 16;
			c1 /= 10;
			s1 = c1 + 0x81;
		}
	} else if (c >= 0x10000 && c <= 0x10ffff) {
		/* Code set 3: Unicode U+10000..U+10FFFF */
		c1 = c - 0x10000;
		s = (c1 % 10) + 0x30;
		c1 /= 10;
		s |= ((c1 % 126) + 0x81) << 8;
		c1 /= 126;
		s |= ((c1 % 10) + 0x30) << 16;
		c1 /= 10;
		s1 = c1 + 0x90;
	}

	if (c == 0) {
		s = 0;
	} else if (s == 0) {
		s = -1;
	}

	if (s >= 0) {
		if (s <= 0x80) { /* latin */
			CK((*filter->output_function)(s, filter->data));
		} else if (s1 > 0) { /* qbcs */
			CK((*filter->output_function)(s1 & 0xff, filter->data));
			CK((*filter->output_function)((s >> 16) & 0xff, filter->data));
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else { /* dbcs */
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_gb18030_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c > 0x80 && c < 0xFF && p < e) {
			unsigned char c2 = *p++;
			unsigned int s = (c << 8) | c2;

			if (((c >= 0x81 && c <= 0x84) || (c >= 0x90 && c <= 0xE3)) && c2 >= 0x30 && c2 <= 0x39) {
				if (p >= e) {
					*out++ = MBFL_BAD_INPUT;
					break;
				}
				unsigned char c3 = *p++;

				if (c3 >= 0x81 && c3 <= 0xFE && p < e) {
					unsigned char c4 = *p++;

					if (c4 >= 0x30 && c4 <= 0x39) {
						if (c >= 0x90 && c <= 0xE3) {
							unsigned int w = ((((c - 0x90)*10 + (c2 - 0x30))*126 + (c3 - 0x81)))*10 + (c4 - 0x30) + 0x10000;
							*out++ = (w > 0x10FFFF) ? MBFL_BAD_INPUT : w;
						} else {
							/* Unicode BMP */
							unsigned int w = (((c - 0x81)*10 + (c2 - 0x30))*126 + (c3 - 0x81))*10 + (c4 - 0x30);
							if (w <= 39419) {
								*out++ = w + mbfl_gb_uni_ofst[mbfl_bisec_srch(w, mbfl_gb2uni_tbl, mbfl_gb_uni_max)];
							} else {
								*out++ = MBFL_BAD_INPUT;
							}
						}
					} else {
						*out++ = MBFL_BAD_INPUT;
					}
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			} else if (((c >= 0xAA && c <= 0xAF) || (c >= 0xF8 && c <= 0xFE)) && (c2 >= 0xA1 && c2 <= 0xFE)) {
				/* UDA part 1, 2: U+E000-U+E4C5 */
				*out++ = 94*(c >= 0xF8 ? c - 0xF2 : c - 0xAA) + (c2 - 0xA1) + 0xE000;
			} else if (c >= 0xA1 && c <= 0xA7 && c2 >= 0x40 && c2 < 0xA1 && c2 != 0x7F) {
				/* UDA part 3: U+E4C6-U+E765 */
				*out++ = 96*(c - 0xA1) + c2 - (c2 >= 0x80 ? 0x41 : 0x40) + 0xE4C6;
			} else {
				if ((s >= 0xA2AB && s <= 0xA9FE) || (s >= 0xD7FA && s <= 0xD7FE) || (s >= 0xFE50 && s <= 0xFEA0)) {
					for (int i = 0; i < mbfl_gb18030_pua_tbl_max; i++) {
						if (s >= mbfl_gb18030_pua_tbl[i][2] && s <= mbfl_gb18030_pua_tbl[i][2] + mbfl_gb18030_pua_tbl[i][1] - mbfl_gb18030_pua_tbl[i][0]) {
							*out++ = s - mbfl_gb18030_pua_tbl[i][2] + mbfl_gb18030_pua_tbl[i][0];
							goto next_iteration;
						}
					}
				}

				if ((c >= 0xA1 && c <= 0xA9 && c2 >= 0xA1 && c2 <= 0xFE) ||
					(c >= 0xB0 && c <= 0xf7 && c2 >= 0xa1 && c2 <= 0xfe) ||
					(c >= 0x81 && c <= 0xa0 && c2 >= 0x40 && c2 <= 0xfe && c2 != 0x7f) ||
					(c >= 0xAA && c <= 0xfe && c2 >= 0x40 && c2 <= 0xa0 && c2 != 0x7f) ||
					(c >= 0xA8 && c <= 0xa9 && c2 >= 0x40 && c2 <= 0xa0 && c2 != 0x7F)) {
					unsigned int w = (c - 0x81)*192 + c2 - 0x40;
					ZEND_ASSERT(w < cp936_ucs_table_size);
					*out++ = cp936_ucs_table[w];
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
next_iteration: ;
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_gb18030(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w == 0) {
			out = mb_convert_buf_add(out, 0);
			continue;
		} else if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			if (w == 0x1F9) {
				s = 0xA8Bf;
			} else {
				s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
			}
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			if (w == 0x20AC) { /* Euro sign */
				s = 0xA2E3;
			} else {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
		} else if (w >= ucs_i_cp936_table_min && w < ucs_i_cp936_table_max) {
			s = ucs_i_cp936_table[w - ucs_i_cp936_table_min];
		} else if (w >= ucs_ci_cp936_table_min && w < ucs_ci_cp936_table_max) {
			/* U+F900-U+FA2F CJK Compatibility Ideographs */
			if (w == 0xF92C) {
				s = 0xFD9C;
			} else if (w == 0xF979) {
				s = 0xFD9D;
			} else if (w == 0xF995) {
				s = 0xFD9E;
			} else if (w == 0xF9E7) {
				s = 0xFD9F;
			} else if (w == 0xF9F1) {
				s = 0xFDA0;
			} else if (w >= 0xFA0C && w <= 0xFA29) {
				s = ucs_ci_s_cp936_table[w - 0xFA0C];
			}
		} else if (w >= ucs_cf_cp936_table_min && w < ucs_cf_cp936_table_max) {
			/* CJK Compatibility Forms  */
			s = ucs_cf_cp936_table[w - ucs_cf_cp936_table_min];
		} else if (w >= ucs_sfv_cp936_table_min && w < ucs_sfv_cp936_table_max) {
			/* U+FE50-U+FE6F Small Form Variants */
			s = ucs_sfv_cp936_table[w - ucs_sfv_cp936_table_min];
		} else if (w >= ucs_hff_cp936_table_min && w < ucs_hff_cp936_table_max) {
			/* U+FF00-U+FFFF HW/FW Forms */
			if (w == 0xFF04) {
				s = 0xA1E7;
			} else if (w == 0xFF5E) {
				s = 0xA1AB;
			} else if (w >= 0xFF01 && w <= 0xFF5D) {
				s = w - 0xFF01 + 0xA3A1;
			} else if (w >= 0xFFE0 && w <= 0xFFE5) {
				s = ucs_hff_s_cp936_table[w - 0xFFE0];
			}
		} else if (w >= 0xE000 && w <= 0xE864) {
			/* PUA */
			if (w < 0xE766) {
				if (w < 0xE4C6) {
					unsigned int c1 = w - 0xE000;
					s = (c1 % 94) + 0xA1;
					c1 /= 94;
					s |= (c1 + (c1 < 0x06 ? 0xAA : 0xF2)) << 8;
				} else {
					unsigned int c1 = w - 0xE4C6;
					s = ((c1 / 96) + 0xA1) << 8;
					c1 %= 96;
					s |= c1 + (c1 >= 0x3F ? 0x41 : 0x40);
				}
			} else {
				/* U+E766-U+E864 */
				unsigned int k1 = 0, k2 = mbfl_gb18030_pua_tbl_max;
				while (k1 < k2) {
					unsigned int k = (k1 + k2) >> 1;
					if (w < mbfl_gb18030_pua_tbl[k][0]) {
						k2 = k;
					} else if (w > mbfl_gb18030_pua_tbl[k][1]) {
						k1 = k + 1;
					} else {
						s = w - mbfl_gb18030_pua_tbl[k][0] + mbfl_gb18030_pua_tbl[k][2];
						break;
					}
				}
			}
		}

		/* While GB18030 and CP936 are very similar, some mappings are different between these encodings;
		 * do a binary search in a table of differing codepoints to see if we have one */
		if (!s && w >= mbfl_gb18030_c_tbl_key[0] && w <= mbfl_gb18030_c_tbl_key[mbfl_gb18030_c_tbl_max-1]) {
			int i = mbfl_bisec_srch2(w, mbfl_gb18030_c_tbl_key, mbfl_gb18030_c_tbl_max);
			if (i >= 0) {
				s = mbfl_gb18030_c_tbl_val[i];
			}
		}

		/* If we have not yet found a suitable mapping for this codepoint, it requires a 4-byte code */
		if (!s && w >= 0x80 && w <= 0xFFFF) {
			/* BMP */
			int i = mbfl_bisec_srch(w, mbfl_uni2gb_tbl, mbfl_gb_uni_max);
			if (i >= 0) {
				unsigned int c1 = w - mbfl_gb_uni_ofst[i];
				s = (c1 % 10) + 0x30;
				c1 /= 10;
				s |= ((c1 % 126) + 0x81) << 8;
				c1 /= 126;
				s |= ((c1 % 10) + 0x30) << 16;
				c1 /= 10;
				s |= (c1 + 0x81) << 24;
			}
		} else if (w >= 0x10000 && w <= 0x10FFFF) {
			/* Code set 3: Unicode U+10000-U+10FFFF */
			unsigned int c1 = w - 0x10000;
			s = (c1 % 10) + 0x30;
			c1 /= 10;
			s |= ((c1 % 126) + 0x81) << 8;
			c1 /= 126;
			s |= ((c1 % 10) + 0x30) << 16;
			c1 /= 10;
			s |= (c1 + 0x90) << 24;
		}

		if (!s) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_gb18030);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		} else if (s < 0x80) {
			out = mb_convert_buf_add(out, s);
		} else if (s > 0xFFFFFF) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			out = mb_convert_buf_add4(out, (s >> 24) & 0xFF, (s >> 16) & 0xFF, (s >> 8) & 0xFF, s & 0xFF);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
