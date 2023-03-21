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
 * the source code included in this files was separated from mbfilter_cn.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_cp936.h"
#define UNICODE_TABLE_CP936_DEF
#include "unicode_table_cp936.h"

static int mbfl_filt_conv_cp936_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_cp936_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_cp936(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);


static const unsigned char mblen_table_cp936[] = { /* 0x81-0xFE */
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
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_cp936_aliases[] = {"CP-936", "GBK", NULL};

const mbfl_encoding mbfl_encoding_cp936 = {
	mbfl_no_encoding_cp936,
	"CP936",
	"CP936",
	mbfl_encoding_cp936_aliases,
	mblen_table_cp936,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp936_wchar,
	&vtbl_wchar_cp936,
	mb_cp936_to_wchar,
	mb_wchar_to_cp936,
	NULL
};

const struct mbfl_convert_vtbl vtbl_cp936_wchar = {
	mbfl_no_encoding_cp936,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp936_wchar,
	mbfl_filt_conv_cp936_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp936 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp936,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp936,
	mbfl_filt_conv_common_flush,
	NULL,
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_cp936_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, w = -1;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c == 0x80) {	/* euro sign */
			CK((*filter->output_function)(0x20ac, filter->data));
		} else if (c < 0xff) {	/* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else { /* 0xff */
			CK((*filter->output_function)(0xf8f5, filter->data));
		}
		break;

	case 1:		/* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;

		if (((c1 >= 0xaa && c1 <= 0xaf) || (c1 >= 0xf8 && c1 <= 0xfe)) &&
			(c >= 0xa1 && c <= 0xfe)) {
			/* UDA part1,2: U+E000-U+E4C5 */
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
			for (k = 0; k < mbfl_cp936_pua_tbl_max; k++) {
				if (c2 >= mbfl_cp936_pua_tbl[k][2] &&
					c2 <= mbfl_cp936_pua_tbl[k][2] +
					mbfl_cp936_pua_tbl[k][1] -  mbfl_cp936_pua_tbl[k][0]) {
					w = c2 -  mbfl_cp936_pua_tbl[k][2] + mbfl_cp936_pua_tbl[k][0];
					CK((*filter->output_function)(w, filter->data));
					break;
				}
			}
		}

		if (w <= 0) {
			if (c1 < 0xff && c1 > 0x80 && c >= 0x40 && c < 0xff && c != 0x7f) {
				w = (c1 - 0x81)*192 + c - 0x40;
				ZEND_ASSERT(w < cp936_ucs_table_size);
				CK((*filter->output_function)(cp936_ucs_table[w], filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_cp936_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_cp936(int c, mbfl_convert_filter *filter)
{
	int k, k1, k2;
	int c1, s = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		/* U+0000 - U+0451 */
		s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		/* U+2000 - U+26FF */
		if (c == 0x203e) {
			s = 0xa3fe;
		} else if (c == 0x2218) {
			s = 0xa1e3;
		} else if (c == 0x223c) {
			s = 0xa1ab;
		} else {
			s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
		}
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		/* U+2F00 - U+33FF */
		s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		/* U+4D00-9FFF CJK Unified Ideographs (+ Extension A) */
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= 0xe000 && c <= 0xe864) { /* PUA */
		if (c < 0xe766) {
			if (c < 0xe4c6) {
				c1 = c - 0xe000;
				s = (c1 % 94) + 0xa1; c1 /= 94;
				s |= (c1 < 0x06 ? c1 + 0xaa : c1 + 0xf2) << 8;
			} else {
				c1 = c - 0xe4c6;
				s = ((c1 / 96) + 0xa1) << 8; c1 %= 96;
				s |= c1 + (c1 >= 0x3f ? 0x41 : 0x40);
			}
		} else {
			/* U+E766..U+E864 */
			k1 = 0; k2 = mbfl_cp936_pua_tbl_max;
			while (k1 < k2) {
				k = (k1 + k2) >> 1;
				if (c < mbfl_cp936_pua_tbl[k][0]) {
					k2 = k;
				} else if (c > mbfl_cp936_pua_tbl[k][1]) {
					k1 = k + 1;
				} else {
					s = c - mbfl_cp936_pua_tbl[k][0] + mbfl_cp936_pua_tbl[k][2];
					break;
				}
			}
		}
	} else if (c == 0xf8f5) {
		s = 0xff;
	} else if (c >= ucs_ci_cp936_table_min && c < ucs_ci_cp936_table_max) {
		/* U+F900-FA2F CJK Compatibility Ideographs */
		s = ucs_ci_cp936_table[c - ucs_ci_cp936_table_min];
	} else if (c >= ucs_cf_cp936_table_min && c < ucs_cf_cp936_table_max) {
		s = ucs_cf_cp936_table[c - ucs_cf_cp936_table_min];
	} else if (c >= ucs_sfv_cp936_table_min && c < ucs_sfv_cp936_table_max) {
		s = ucs_sfv_cp936_table[c - ucs_sfv_cp936_table_min]; /* U+FE50-FE6F Small Form Variants */
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

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s <= 0x80 || s == 0xff) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_cp936_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c == 0x80) {
			*out++ = 0x20AC; /* Euro sign */
		} else if (c < 0xFF) {
			if (p >= e) {
				*out++ = MBFL_BAD_INPUT;
				continue;
			}

			unsigned char c2 = *p++;

			if (((c >= 0xAA && c <= 0xAF) || (c >= 0xF8 && c <= 0xFE)) && (c2 >= 0xA1 && c2 <= 0xFE)) {
				/* UDA part 1, 2: U+E000-U+E4C5 */
				*out++ = 94*(c >= 0xF8 ? c - 0xF2 : c - 0xAA) + (c2 - 0xA1) + 0xE000;
			} else if (c >= 0xA1 && c <= 0xA7 && c2 >= 0x40 && c2 < 0xA1 && c2 != 0x7F) {
				/* UDA part 3: U+E4C6-U+E765*/
				*out++ = 96*(c - 0xA1) + c2 - (c2 >= 0x80 ? 0x41 : 0x40) + 0xE4C6;
			} else {
				unsigned int w = (c << 8) | c2;

				if ((w >= 0xA2AB && w <= 0xA9FE) || (w >= 0xD7FA && w <= 0xD7FE) || (w >= 0xFE50 && w <= 0xFEA0)) {
					for (int k = 0; k < mbfl_cp936_pua_tbl_max; k++) {
						if (w >= mbfl_cp936_pua_tbl[k][2] && w <= mbfl_cp936_pua_tbl[k][2] + mbfl_cp936_pua_tbl[k][1] - mbfl_cp936_pua_tbl[k][0]) {
							*out++ = w -  mbfl_cp936_pua_tbl[k][2] + mbfl_cp936_pua_tbl[k][0];
							goto next_iteration;
						}
					}
				}

				if (c < 0xFF && c > 0x80 && c2 >= 0x40 && c2 < 0xFF && c2 != 0x7F) {
					w = (c - 0x81)*192 + c2 - 0x40;
					ZEND_ASSERT(w < cp936_ucs_table_size);
					*out++ = cp936_ucs_table[w];
				} else {
					*out++ = MBFL_BAD_INPUT;
				}
			}
		} else {
			*out++ = 0xF8F5;
		}
next_iteration: ;
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_cp936(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_cp936_table_min && w < ucs_a1_cp936_table_max) {
			/* U+0000-U+0451 */
			s = ucs_a1_cp936_table[w - ucs_a1_cp936_table_min];
		} else if (w >= ucs_a2_cp936_table_min && w < ucs_a2_cp936_table_max) {
			/* U+2000-U+26FF */
			if (w == 0x203E) {
				s = 0xA3FE;
			} else if (w == 0x2218) {
				s = 0xA1E3;
			} else if (w == 0x223C) {
				s = 0xA1AB;
			} else {
				s = ucs_a2_cp936_table[w - ucs_a2_cp936_table_min];
			}
		} else if (w >= ucs_a3_cp936_table_min && w < ucs_a3_cp936_table_max) {
			/* U+2F00-U+33FF */
			s = ucs_a3_cp936_table[w - ucs_a3_cp936_table_min];
		} else if (w >= ucs_i_cp936_table_min && w < ucs_i_cp936_table_max) {
			/* U+4D00-9FFF CJK Unified Ideographs (+ Extension A) */
			s = ucs_i_cp936_table[w - ucs_i_cp936_table_min];
		} else if (w >= 0xE000 && w <= 0xE864) {
			/* PUA */
			if (w < 0xe766) {
				if (w < 0xe4c6) {
					unsigned int c1 = w - 0xE000;
					s = (c1 % 94) + 0xA1;
					c1 /= 94;
					s |= (c1 < 0x6 ? c1 + 0xAA : c1 + 0xF2) << 8;
				} else {
					unsigned int c1 = w - 0xE4C6;
					s = ((c1 / 96) + 0xA1) << 8;
					c1 %= 96;
					s |= c1 + (c1 >= 0x3F ? 0x41 : 0x40);
				}
			} else {
				/* U+E766-U+E864 */
				unsigned int k1 = 0;
				unsigned int k2 = mbfl_cp936_pua_tbl_max;
				while (k1 < k2) {
					int k = (k1 + k2) >> 1;
					if (w < mbfl_cp936_pua_tbl[k][0]) {
						k2 = k;
					} else if (w > mbfl_cp936_pua_tbl[k][1]) {
						k1 = k + 1;
					} else {
						s = w - mbfl_cp936_pua_tbl[k][0] + mbfl_cp936_pua_tbl[k][2];
						break;
					}
				}
			}
		} else if (w == 0xF8F5) {
			s = 0xFF;
		} else if (w >= ucs_ci_cp936_table_min && w < ucs_ci_cp936_table_max) {
			/* U+F900-U+FA2F CJK Compatibility Ideographs */
			s = ucs_ci_cp936_table[w - ucs_ci_cp936_table_min];
		} else if (w >= ucs_cf_cp936_table_min && w < ucs_cf_cp936_table_max) {
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
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp936);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			}
		} else if (s <= 0x80 || s == 0xFF) {
			out = mb_convert_buf_add(out, s);
		} else {
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
