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
 * The author of this file: Rui Hirokawa <hirokawa@php.net>
 *
 */
/*
 * The source code included in this files was separated from mbfilter_tw.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_big5.h"

#include "unicode_table_big5.h"

static int mbfl_filt_conv_big5_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_big5_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_big5(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_cp950_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_cp950(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const unsigned char mblen_table_big5[] = { /* 0x81-0xFE */
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

static const char *mbfl_encoding_big5_aliases[] = {"CN-BIG5", "BIG-FIVE", "BIGFIVE", NULL};

const mbfl_encoding mbfl_encoding_big5 = {
	mbfl_no_encoding_big5,
	"BIG-5",
	"BIG5",
	mbfl_encoding_big5_aliases,
	mblen_table_big5,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_big5_wchar,
	&vtbl_wchar_big5,
	mb_big5_to_wchar,
	mb_wchar_to_big5,
	NULL
};

const mbfl_encoding mbfl_encoding_cp950 = {
	mbfl_no_encoding_cp950,
	"CP950",
	"BIG5",
	NULL,
	mblen_table_big5,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp950_wchar,
	&vtbl_wchar_cp950,
	mb_cp950_to_wchar,
	mb_wchar_to_cp950,
	NULL
};

const struct mbfl_convert_vtbl vtbl_big5_wchar = {
	mbfl_no_encoding_big5,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_big5_wchar,
	mbfl_filt_conv_big5_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_big5 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_big5,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_big5,
	mbfl_filt_conv_common_flush,
	NULL
};

const struct mbfl_convert_vtbl vtbl_cp950_wchar = {
	mbfl_no_encoding_cp950,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_big5_wchar,
	mbfl_filt_conv_big5_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp950 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp950,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_big5,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/* 63 + 94 = 157 or 94 */
static unsigned short cp950_pua_tbl[][4] = {
	{0xe000, 0xe310, 0xfa40, 0xfefe},
	{0xe311, 0xeeb7, 0x8e40, 0xa0fe},
	{0xeeb8, 0xf6b0, 0x8140, 0x8dfe},
	{0xf6b1, 0xf70e, 0xc6a1, 0xc6fe},
	{0xf70f, 0xf848, 0xc740, 0xc8fe},
};

static inline int is_in_cp950_pua(int c1, int c)
{
	if ((c1 >= 0xfa && c1 <= 0xfe) || (c1 >= 0x8e && c1 <= 0xa0) ||
			(c1 >= 0x81 && c1 <= 0x8d) || (c1 >= 0xc7 && c1 <= 0xc8)) {
		return (c >= 0x40 && c <= 0x7e) || (c >= 0xa1 && c <= 0xfe);
	} else if (c1 == 0xc6) {
		return c >= 0xa1 && c <= 0xfe;
	}
	return 0;
}

int mbfl_filt_conv_big5_wchar(int c, mbfl_convert_filter *filter)
{
	int k, c1, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) { /* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (filter->from->no_encoding != mbfl_no_encoding_cp950 && c > 0xA0 && c <= 0xF9 && c != 0xC8) {
			filter->status = 1;
			filter->cache = c;
		} else if (filter->from->no_encoding == mbfl_no_encoding_cp950 && c > 0x80 && c <= 0xFE) {
			filter->status = 1;
			filter->cache = c;
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	case 1: /* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		if ((c > 0x3f && c < 0x7f) || (c > 0xa0 && c < 0xff)) {
			if (c < 0x7f) {
				w = (c1 - 0xa1)*157 + (c - 0x40);
			} else {
				w = (c1 - 0xa1)*157 + (c - 0xa1) + 0x3f;
			}
			if (w >= 0 && w < big5_ucs_table_size) {
				w = big5_ucs_table[w];
			} else {
				w = 0;
			}

			if (filter->from->no_encoding == mbfl_no_encoding_cp950) {
				/* PUA for CP950 */
				if (is_in_cp950_pua(c1, c)) {
					int c2 = (c1 << 8) | c;

					for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
						if (c2 >= cp950_pua_tbl[k][2] && c2 <= cp950_pua_tbl[k][3]) {
							break;
						}
					}

					if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
						w = 157*(c1 - (cp950_pua_tbl[k][2]>>8)) + c - (c >= 0xa1 ? 0x62 : 0x40) + cp950_pua_tbl[k][0];
					} else {
						w = c2 - cp950_pua_tbl[k][2] + cp950_pua_tbl[k][0];
					}
				} else if (c1 == 0xA1) {
					if (c == 0x45) {
						w = 0x2027;
					} else if (c == 0x4E) {
						w = 0xFE51;
					} else if (c == 0x5A) {
						w = 0x2574;
					} else if (c == 0xC2) {
						w = 0x00AF;
					} else if (c == 0xC3) {
						w = 0xFFE3;
					} else if (c == 0xC5) {
						w = 0x02CD;
					} else if (c == 0xE3) {
						w = 0xFF5E;
					} else if (c == 0xF2) {
						w = 0x2295;
					} else if (c == 0xF3) {
						w = 0x2299;
					} else if (c == 0xFE) {
						w = 0xFF0F;
					}
				} else if (c1 == 0xA2) {
					if (c == 0x40) {
						w = 0xFF3C;
					} else if (c == 0x41) {
						w = 0x2215;
					} else if (c == 0x42) {
						w = 0xFE68;
					} else if (c == 0x46) {
						w = 0xFFE0;
					} else if (c == 0x47) {
						w = 0xFFE1;
					} else if (c == 0xCC) {
						w = 0x5341;
					} else if (c == 0xCE) {
						w = 0x5345;
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

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_big5_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status == 1) {
		/* 2-byte character was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_big5(int c, mbfl_convert_filter *filter)
{
	int k, s = 0;

	if (c >= ucs_a1_big5_table_min && c < ucs_a1_big5_table_max) {
		s = ucs_a1_big5_table[c - ucs_a1_big5_table_min];
	} else if (c >= ucs_a2_big5_table_min && c < ucs_a2_big5_table_max) {
		s = ucs_a2_big5_table[c - ucs_a2_big5_table_min];
	} else if (c >= ucs_a3_big5_table_min && c < ucs_a3_big5_table_max) {
		s = ucs_a3_big5_table[c - ucs_a3_big5_table_min];
	} else if (c >= ucs_i_big5_table_min && c < ucs_i_big5_table_max) {
		s = ucs_i_big5_table[c - ucs_i_big5_table_min];
	} else if (c >= ucs_r1_big5_table_min && c < ucs_r1_big5_table_max) {
		s = ucs_r1_big5_table[c - ucs_r1_big5_table_min];
	} else if (c >= ucs_r2_big5_table_min && c < ucs_r2_big5_table_max) {
		s = ucs_r2_big5_table[c - ucs_r2_big5_table_min];
	}

	if (filter->to->no_encoding == mbfl_no_encoding_cp950) {
		if (c >= 0xe000 && c <= 0xf848) { /* PUA for CP950 */
			for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
				if (c <= cp950_pua_tbl[k][1]) {
					break;
				}
			}

			int c1 = c - cp950_pua_tbl[k][0];
			if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
				int c2 = cp950_pua_tbl[k][2] >> 8;
				s = ((c1 / 157) + c2) << 8;
				c1 %= 157;
				s |= c1 + (c1 >= 0x3f ? 0x62 : 0x40);
			} else {
				s = c1 + cp950_pua_tbl[k][2];
			}
		} else if (c == 0x00A2) {
			s = 0;
		} else if (c == 0x00A3) {
			s = 0;
		} else if (c == 0x00AF) {
			s = 0xA1C2;
		} else if (c == 0x02CD) {
			s = 0xA1C5;
		} else if (c == 0x0401) {
			s = 0;
		} else if (c >= 0x0414 && c <= 0x041C) {
			s = 0;
		} else if (c >= 0x0423 && c <= 0x044F) {
			s = 0;
		} else if (c == 0x0451) {
			s = 0;
		} else if (c == 0x2022) {
			s = 0;
		} else if (c == 0x2027) {
			s = 0xA145;
		} else if (c == 0x203E) {
			s = 0;
		} else if (c == 0x2215) {
			s = 0xA241;
		} else if (c == 0x223C) {
			s = 0;
		} else if (c == 0x2295) {
			s = 0xA1F2;
		} else if (c == 0x2299) {
			s = 0xA1F3;
		} else if (c >= 0x2460 && c <= 0x247D) {
			s = 0;
		} else if (c == 0x2574) {
			s = 0xA15A;
		} else if (c == 0x2609) {
			s = 0;
		} else if (c == 0x2641) {
			s = 0;
		} else if (c == 0x3005 || (c >= 0x302A && c <= 0x30FF)) {
			s = 0;
		} else if (c == 0xFE51) {
			s = 0xA14E;
		} else if (c == 0xFE68) {
			s = 0xA242;
		} else if (c == 0xFF3C) {
			s = 0xA240;
		} else if (c == 0xFF5E) {
			s = 0xA1E3;
		} else if (c == 0xFF64) {
			s = 0;
		} else if (c == 0xFFE0) {
			s = 0xA246;
		} else if (c == 0xFFE1) {
			s = 0xA247;
		} else if (c == 0xFFE3) {
			s = 0xA1C3;
		} else if (c == 0xFF0F) {
			s = 0xA1FE;
		}
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s <= 0x80) { /* latin */
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

static size_t mb_big5_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c > 0xA0 && c <= 0xF9 && c != 0xC8 && p < e) {
			unsigned char c2 = *p++;

			if ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0xA1 && c2 <= 0xFE)) {
				unsigned int w = ((c - 0xA1)*157) + c2 - ((c2 <= 0x7E) ? 0x40 : 0xA1 - 0x3F);
				w = (w < big5_ucs_table_size) ? big5_ucs_table[w] : 0;
				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
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

static void mb_wchar_to_big5(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_big5_table_min && w < ucs_a1_big5_table_max) {
			s = ucs_a1_big5_table[w - ucs_a1_big5_table_min];
		} else if (w >= ucs_a2_big5_table_min && w < ucs_a2_big5_table_max) {
			s = ucs_a2_big5_table[w - ucs_a2_big5_table_min];
		} else if (w >= ucs_a3_big5_table_min && w < ucs_a3_big5_table_max) {
			s = ucs_a3_big5_table[w - ucs_a3_big5_table_min];
		} else if (w >= ucs_i_big5_table_min && w < ucs_i_big5_table_max) {
			s = ucs_i_big5_table[w - ucs_i_big5_table_min];
		} else if (w >= ucs_r1_big5_table_min && w < ucs_r1_big5_table_max) {
			s = ucs_r1_big5_table[w - ucs_r1_big5_table_min];
		} else if (w >= ucs_r2_big5_table_min && w < ucs_r2_big5_table_max) {
			s = ucs_r2_big5_table[w - ucs_r2_big5_table_min];
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_big5);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s <= 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_cp950_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c <= 0x7F) {
			*out++ = c;
		} else if (c > 0x80 && c <= 0xFE && p < e) {
			unsigned char c2 = *p++;

			if ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0xA1 && c2 <= 0xFE)) {
				unsigned int w = ((c - 0xA1)*157) + c2 - ((c2 <= 0x7E) ? 0x40 : 0xA1 - 0x3F);
				w = (w < big5_ucs_table_size) ? big5_ucs_table[w] : 0;

				/* PUA for CP950 */
				if (is_in_cp950_pua(c, c2)) {
					unsigned int s = (c << 8) | c2;

					int k;
					for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
						if (s >= cp950_pua_tbl[k][2] && s <= cp950_pua_tbl[k][3]) {
							break;
						}
					}

					if ((cp950_pua_tbl[k][2] & 0xFF) == 0x40) {
						w = 157*(c - (cp950_pua_tbl[k][2] >> 8)) + c2 - (c2 >= 0xA1 ? 0x62 : 0x40) + cp950_pua_tbl[k][0];
					} else {
						w = s - cp950_pua_tbl[k][2] + cp950_pua_tbl[k][0];
					}
				} else if (c == 0xA1) {
					if (c2 == 0x45) {
						w = 0x2027;
					} else if (c2 == 0x4E) {
						w = 0xFE51;
					} else if (c2 == 0x5A) {
						w = 0x2574;
					} else if (c2 == 0xC2) {
						w = 0x00AF;
					} else if (c2 == 0xC3) {
						w = 0xFFE3;
					} else if (c2 == 0xC5) {
						w = 0x02CD;
					} else if (c2 == 0xE3) {
						w = 0xFF5E;
					} else if (c2 == 0xF2) {
						w = 0x2295;
					} else if (c2 == 0xF3) {
						w = 0x2299;
					} else if (c2 == 0xFE) {
						w = 0xFF0F;
					}
				} else if (c == 0xA2) {
					if (c2 == 0x40) {
						w = 0xFF3C;
					} else if (c2 == 0x41) {
						w = 0x2215;
					} else if (c2 == 0x42) {
						w = 0xFE68;
					} else if (c2 == 0x46) {
						w = 0xFFE0;
					} else if (c2 == 0x47) {
						w = 0xFFE1;
					} else if (c2 == 0xCC) {
						w = 0x5341;
					} else if (c2 == 0xCE) {
						w = 0x5345;
					}
				}

				if (!w)
					w = MBFL_BAD_INPUT;
				*out++ = w;
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

static void mb_wchar_to_cp950(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_big5_table_min && w < ucs_a1_big5_table_max) {
			s = ucs_a1_big5_table[w - ucs_a1_big5_table_min];
		} else if (w >= ucs_a2_big5_table_min && w < ucs_a2_big5_table_max) {
			s = ucs_a2_big5_table[w - ucs_a2_big5_table_min];
		} else if (w >= ucs_a3_big5_table_min && w < ucs_a3_big5_table_max) {
			s = ucs_a3_big5_table[w - ucs_a3_big5_table_min];
		} else if (w >= ucs_i_big5_table_min && w < ucs_i_big5_table_max) {
			s = ucs_i_big5_table[w - ucs_i_big5_table_min];
		} else if (w >= ucs_r1_big5_table_min && w < ucs_r1_big5_table_max) {
			s = ucs_r1_big5_table[w - ucs_r1_big5_table_min];
		} else if (w >= ucs_r2_big5_table_min && w < ucs_r2_big5_table_max) {
			s = ucs_r2_big5_table[w - ucs_r2_big5_table_min];
		}

		if (w >= 0xE000 && w <= 0xF848) {
			int k;
			for (k = 0; k < sizeof(cp950_pua_tbl) / (sizeof(unsigned short)*4); k++) {
				if (w <= cp950_pua_tbl[k][1]) {
					break;
				}
			}

			int c1 = w - cp950_pua_tbl[k][0];
			if ((cp950_pua_tbl[k][2] & 0xFF) == 0x40) {
				int c2 = cp950_pua_tbl[k][2] >> 8;
				s = ((c1 / 157) + c2) << 8;
				c1 %= 157;
				s |= c1 + (c1 >= 0x3F ? 0x62 : 0x40);
			} else {
				s = c1 + cp950_pua_tbl[k][2];
			}
		} else if (w == 0xA2 || w == 0xA3 || w == 0x401 || (w >= 0x414 && w <= 0x41C) || (w >= 0x423 && w <= 0x44F) || w == 0x451 || w == 0x2022 || w == 0x203E || w == 0x223C || (w >= 0x2460 && w <= 0x247D) || w == 0x2609 || w == 0x2641 || w == 0x3005 || (w >= 0x302A && w <= 0x30FF) || w == 0xFF64) {
			s = 0;
		} else if (w == 0xAF) {
			s = 0xA1C2;
		} else if (w == 0x2CD) {
			s = 0xA1C5;
		} else if (w == 0x2027) {
			s = 0xA145;
		} else if (w == 0x2215) {
			s = 0xA241;
		} else if (w == 0x2295) {
			s = 0xA1F2;
		} else if (w == 0x2299) {
			s = 0xA1F3;
		} else if (w == 0x2574) {
			s = 0xA15A;
		} else if (w == 0xFE51) {
			s = 0xA14E;
		} else if (w == 0xFE68) {
			s = 0xA242;
		} else if (w == 0xFF3C) {
			s = 0xA240;
		} else if (w == 0xFF5E) {
			s = 0xA1E3;
		} else if (w == 0xFFE0) {
			s = 0xA246;
		} else if (w == 0xFFE1) {
			s = 0xA247;
		} else if (w == 0xFFE3) {
			s = 0xA1C3;
		} else if (w == 0xFF0F) {
			s = 0xA1FE;
		}

		if (!s) {
			if (w == 0) {
				out = mb_convert_buf_add(out, 0);
			} else {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_big5);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
			}
		} else if (s <= 0x80) {
			out = mb_convert_buf_add(out, s);
		} else {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
