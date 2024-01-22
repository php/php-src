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
 * The source code included in this files was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_utf8.h"
#include "mbfilter_cjk.h"
#include "emoji2uni.h"

const unsigned char mblen_table_utf8[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

extern int mbfl_bisec_srch2(int w, const unsigned short tbl[], int n);
extern int mbfl_filt_conv_sjis_mobile_flush(mbfl_convert_filter *filter);

static int mbfl_filt_conv_utf8_mobile_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf8_mobile(int c, mbfl_convert_filter *filter);

static int mbfl_filt_conv_utf8_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf8(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf8_wchar_flush(mbfl_convert_filter *filter);

static size_t mb_utf8_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf8(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static zend_string* mb_cut_utf8(unsigned char *str, size_t from, size_t len, unsigned char *end);

static size_t mb_utf8_docomo_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf8_docomo(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf8_kddi_a_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf8_kddi_a(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf8_kddi_b_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf8_kddi_b(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf8_sb_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf8_sb(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static bool mbfilter_conv_map_tbl(int c, int *w, unsigned int n, const unsigned short map[/* n */][3])
{
	for (unsigned int i = 0; i < n; i++) {
		if (map[i][0] <= c && c <= map[i][1]) {
			*w = c - map[i][0] + map[i][2];
			return true;
		}
	}
	return false;
}

static bool mbfilter_conv_r_map_tbl(int c, int *w, unsigned int n, const unsigned short map[/* n */][3])
{
	/* Convert in reverse direction */
	for (unsigned int i = 0; i < n; i++) {
		if (map[i][2] <= c && c <= map[i][2] - map[i][0] + map[i][1]) {
			*w = c + map[i][0] - map[i][2];
			return true;
		}
	}
	return false;
}

static const unsigned short mbfl_docomo2uni_pua[4][3] = {
	{0x28c2, 0x292f, 0xe63e},
	{0x2930, 0x2934, 0xe6ac},
	{0x2935, 0x2951, 0xe6b1},
	{0x2952, 0x29db, 0xe6ce},
};

static const unsigned short mbfl_kddi2uni_pua[7][3] = {
	{0x26ec, 0x2838, 0xe468},
	{0x284c, 0x2863, 0xe5b5},
	{0x24b8, 0x24ca, 0xe5cd},
	{0x24cb, 0x2545, 0xea80},
	{0x2839, 0x284b, 0xeafb},
	{0x2546, 0x25c0, 0xeb0e},
	{0x25c1, 0x25c6, 0xeb89},
};

static const unsigned short mbfl_kddi2uni_pua_b[8][3] = {
	{0x24b8, 0x24f6, 0xec40},
	{0x24f7, 0x2573, 0xec80},
	{0x2574, 0x25b2, 0xed40},
	{0x25b3, 0x25c6, 0xed80},
	{0x26ec, 0x272a, 0xef40},
	{0x272b, 0x27a7, 0xef80},
	{0x27a8, 0x27e6, 0xf040},
	{0x27e7, 0x2863, 0xf080},
};

static const unsigned short mbfl_sb2uni_pua[6][3] = {
	{0x27a9, 0x2802, 0xe101},
	{0x2808, 0x2861, 0xe201},
	{0x2921, 0x297a, 0xe001},
	{0x2980, 0x29cc, 0xe301},
	{0x2a99, 0x2ae4, 0xe401},
	{0x2af8, 0x2b35, 0xe501},
};

static const char *mbfl_encoding_utf8_aliases[] = {"utf8", NULL};

const mbfl_encoding mbfl_encoding_utf8 = {
	mbfl_no_encoding_utf8,
	"UTF-8",
	"UTF-8",
	mbfl_encoding_utf8_aliases,
	mblen_table_utf8,
	0,
	&vtbl_utf8_wchar,
	&vtbl_wchar_utf8,
	mb_utf8_to_wchar,
	mb_wchar_to_utf8,
	NULL,
	mb_cut_utf8
};

const struct mbfl_convert_vtbl vtbl_utf8_wchar = {
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf8_wchar,
	mbfl_filt_conv_utf8_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf8 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf8,
	mbfl_filt_conv_common_flush,
	NULL,
};

static const char *mbfl_encoding_utf8_docomo_aliases[] = {"UTF-8-DOCOMO", "UTF8-DOCOMO", NULL};
static const char *mbfl_encoding_utf8_kddi_b_aliases[] = {"UTF-8-Mobile#KDDI", "UTF-8-KDDI", "UTF8-KDDI", NULL};
static const char *mbfl_encoding_utf8_sb_aliases[] = {"UTF-8-SOFTBANK", "UTF8-SOFTBANK", NULL};

const mbfl_encoding mbfl_encoding_utf8_docomo = {
	mbfl_no_encoding_utf8_docomo,
	"UTF-8-Mobile#DOCOMO",
	"UTF-8",
	mbfl_encoding_utf8_docomo_aliases,
	mblen_table_utf8,
	0,
	&vtbl_utf8_docomo_wchar,
	&vtbl_wchar_utf8_docomo,
	mb_utf8_docomo_to_wchar,
	mb_wchar_to_utf8_docomo,
	NULL,
	mb_cut_utf8,
};

const mbfl_encoding mbfl_encoding_utf8_kddi_a = {
	mbfl_no_encoding_utf8_kddi_a,
	"UTF-8-Mobile#KDDI-A",
	"UTF-8",
	NULL,
	mblen_table_utf8,
	0,
	&vtbl_utf8_kddi_a_wchar,
	&vtbl_wchar_utf8_kddi_a,
	mb_utf8_kddi_a_to_wchar,
	mb_wchar_to_utf8_kddi_a,
	NULL,
	mb_cut_utf8,
};

const mbfl_encoding mbfl_encoding_utf8_kddi_b = {
	mbfl_no_encoding_utf8_kddi_b,
	"UTF-8-Mobile#KDDI-B",
	"UTF-8",
	mbfl_encoding_utf8_kddi_b_aliases,
	mblen_table_utf8,
	0,
	&vtbl_utf8_kddi_b_wchar,
	&vtbl_wchar_utf8_kddi_b,
	mb_utf8_kddi_b_to_wchar,
	mb_wchar_to_utf8_kddi_b,
	NULL,
	mb_cut_utf8,
};

const mbfl_encoding mbfl_encoding_utf8_sb = {
	mbfl_no_encoding_utf8_sb,
	"UTF-8-Mobile#SOFTBANK",
	"UTF-8",
	mbfl_encoding_utf8_sb_aliases,
	mblen_table_utf8,
	0,
	&vtbl_utf8_sb_wchar,
	&vtbl_wchar_utf8_sb,
	mb_utf8_sb_to_wchar,
	mb_wchar_to_utf8_sb,
	NULL,
	mb_cut_utf8,
};

const struct mbfl_convert_vtbl vtbl_utf8_docomo_wchar = {
	mbfl_no_encoding_utf8_docomo,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf8_mobile_wchar,
	mbfl_filt_conv_utf8_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf8_docomo = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8_docomo,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf8_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf8_kddi_a_wchar = {
	mbfl_no_encoding_utf8_kddi_a,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf8_mobile_wchar,
	mbfl_filt_conv_utf8_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf8_kddi_a = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8_kddi_a,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf8_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf8_kddi_b_wchar = {
	mbfl_no_encoding_utf8_kddi_b,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf8_mobile_wchar,
	mbfl_filt_conv_utf8_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf8_kddi_b = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8_kddi_b,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf8_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf8_sb_wchar = {
	mbfl_no_encoding_utf8_sb,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf8_mobile_wchar,
	mbfl_filt_conv_utf8_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf8_sb = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8_sb,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf8_mobile,
	mbfl_filt_conv_sjis_mobile_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

static int mbfl_filt_put_invalid_char(mbfl_convert_filter *filter)
{
	filter->status = filter->cache = 0;
	CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	return 0;
}

static int mbfl_filt_conv_utf8_wchar(int c, mbfl_convert_filter *filter)
{
	int s, c1;

retry:
	switch (filter->status) {
	case 0x00:
		if (c < 0x80) {
			CK((*filter->output_function)(c, filter->data));
		} else if (c >= 0xc2 && c <= 0xdf) { /* 2byte code first char: 0xc2-0xdf */
			filter->status = 0x10;
			filter->cache = c & 0x1f;
		} else if (c >= 0xe0 && c <= 0xef) { /* 3byte code first char: 0xe0-0xef */
			filter->status = 0x20;
			filter->cache = c & 0xf;
		} else if (c >= 0xf0 && c <= 0xf4) { /* 3byte code first char: 0xf0-0xf4 */
			filter->status = 0x30;
			filter->cache = c & 0x7;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
		}
		break;
	case 0x10: /* 2byte code 2nd char: 0x80-0xbf */
	case 0x21: /* 3byte code 3rd char: 0x80-0xbf */
	case 0x32: /* 4byte code 4th char: 0x80-0xbf */
		if (c >= 0x80 && c <= 0xbf) {
			s = (filter->cache<<6) | (c & 0x3f);
			filter->status = filter->cache = 0;
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;
	case 0x20: /* 3byte code 2nd char: 0:0xa0-0xbf,D:0x80-9F,1-C,E-F:0x80-0x9f */
		s = (filter->cache<<6) | (c & 0x3f);
		c1 = filter->cache & 0xf;

		if ((c >= 0x80 && c <= 0xbf) &&
			((c1 == 0x0 && c >= 0xa0) ||
			 (c1 == 0xd && c < 0xa0) ||
			 (c1 > 0x0 && c1 != 0xd))) {
			filter->cache = s;
			filter->status++;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;
	case 0x30: /* 4byte code 2nd char: 0:0x90-0xbf,1-3:0x80-0xbf,4:0x80-0x8f */
		s = (filter->cache<<6) | (c & 0x3f);
		c1 = filter->cache & 0x7;

		if ((c >= 0x80 && c <= 0xbf) &&
			((c1 == 0x0 && c >= 0x90) ||
			 (c1 == 0x4 && c < 0x90) ||
			 (c1 > 0x0 && c1 != 0x4))) {
			filter->cache = s;
			filter->status++;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;
	case 0x31: /* 4byte code 3rd char: 0x80-0xbf */
		if (c >= 0x80 && c <= 0xbf) {
			filter->cache = (filter->cache<<6) | (c & 0x3f);
			filter->status++;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_utf8_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_conv_wchar_utf8(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x110000) {
		if (c < 0x80) {
			CK((*filter->output_function)(c, filter->data));
		} else if (c < 0x800) {
			CK((*filter->output_function)(((c >> 6) & 0x1f) | 0xc0, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else if (c < 0x10000) {
			CK((*filter->output_function)(((c >> 12) & 0x0f) | 0xe0, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else {
			CK((*filter->output_function)(((c >> 18) & 0x07) | 0xf0, filter->data));
			CK((*filter->output_function)(((c >> 12) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_utf8_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c < 0x80) {
			*out++ = c;
		} else if (c < 0xC2) {
			*out++ = MBFL_BAD_INPUT;
		} else if (c <= 0xDF) { /* 2 byte character */
			if (p < e) {
				unsigned char c2 = *p++;
				if ((c2 & 0xC0) != 0x80) {
					*out++ = MBFL_BAD_INPUT;
					p--;
				} else {
					*out++ = ((c & 0x1F) << 6) | (c2 & 0x3F);
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c <= 0xEF) { /* 3 byte character */
			if ((e - p) >= 2) {
				unsigned char c2 = *p++;
				unsigned char c3 = *p++;
				if ((c2 & 0xC0) != 0x80 || (c == 0xE0 && c2 < 0xA0) || (c == 0xED && c2 >= 0xA0)) {
					*out++ = MBFL_BAD_INPUT;
					p -= 2;
				} else if ((c3 & 0xC0) != 0x80) {
					*out++ = MBFL_BAD_INPUT;
					p--;
				} else {
					uint32_t decoded = ((c & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
					ZEND_ASSERT(decoded >= 0x800); /* Not an overlong code unit */
					ZEND_ASSERT(decoded < 0xD800 || decoded > 0xDFFF); /* U+D800-DFFF are reserved, illegal code points */
					*out++ = decoded;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				if (p < e && (c != 0xE0 || *p >= 0xA0) && (c != 0xED || *p < 0xA0) && (*p & 0xC0) == 0x80) {
					p++;
					if (p < e && (*p & 0xC0) == 0x80) {
						p++;
					}
				}
			}
		} else if (c <= 0xF4) { /* 4 byte character */
			if ((e - p) >= 3) {
				unsigned char c2 = *p++;
				unsigned char c3 = *p++;
				unsigned char c4 = *p++;
				/* If c == 0xF0 and c2 < 0x90, then this is an over-long code unit; it could have
				 * fit in 3 bytes only. If c == 0xF4 and c2 >= 0x90, then this codepoint is
				 * greater than U+10FFFF, which is the highest legal codepoint */
				if ((c2 & 0xC0) != 0x80 || (c == 0xF0 && c2 < 0x90) || (c == 0xF4 && c2 >= 0x90)) {
					*out++ = MBFL_BAD_INPUT;
					p -= 3;
				} else if ((c3 & 0xC0) != 0x80) {
					*out++ = MBFL_BAD_INPUT;
					p -= 2;
				} else if ((c4 & 0xC0) != 0x80) {
					*out++ = MBFL_BAD_INPUT;
					p--;
				} else {
					uint32_t decoded = ((c & 0x7) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
					ZEND_ASSERT(decoded >= 0x10000); /* Not an overlong code unit */
					*out++ = decoded;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				if (p < e) {
					unsigned char c2 = *p;
					if ((c == 0xF0 && c2 >= 0x90) || (c == 0xF4 && c2 < 0x90) || (c >= 0xF1 && c <= 0xF3)) {
						while (p < e && (*p & 0xC0) == 0x80) {
							p++;
						}
					}
				}
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf8(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		if (w < 0x80) {
			out = mb_convert_buf_add(out, w & 0xFF);
		} else if (w < 0x800) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, ((w >> 6) & 0x1F) | 0xC0, (w & 0x3F) | 0x80);
		} else if (w < 0x10000) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
			out = mb_convert_buf_add3(out, ((w >> 12) & 0xF) | 0xE0, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
		} else if (w < 0x110000) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			out = mb_convert_buf_add4(out, ((w >> 18) & 0x7) | 0xF0, ((w >> 12) & 0x3F) | 0x80, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf8);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static zend_string* mb_cut_utf8(unsigned char *str, size_t from, size_t len, unsigned char *end)
{
	unsigned char *start = str + from;
	/* Byte values less than -64 are UTF-8 continuation bytes, that is,
	 * the 2nd, 3rd, or 4th byte of a multi-byte character */
	while (start > str && ((signed char)*start) < -64) {
		start--;
	}
	unsigned char *_end = start + len;
	if (_end >= end) {
		return zend_string_init_fast((char*)start, end - start);
	}
	while (_end > start && ((signed char)*_end) < -64) {
		_end--;
	}
	return zend_string_init_fast((char*)start, _end - start);
}

static int mbfl_filt_conv_utf8_mobile_wchar(int c, mbfl_convert_filter *filter)
{
	int s, s1 = 0, c1 = 0, snd = 0;

retry:
	switch (filter->status & 0xff) {
	case 0x00:
		if (c < 0x80) {
			CK((*filter->output_function)(c, filter->data));
		} else if (c >= 0xc2 && c <= 0xdf) { /* 2byte code first char: 0xc2-0xdf */
			filter->status = 0x10;
			filter->cache = c & 0x1f;
		} else if (c >= 0xe0 && c <= 0xef) { /* 3byte code first char: 0xe0-0xef */
			filter->status = 0x20;
			filter->cache = c & 0xf;
		} else if (c >= 0xf0 && c <= 0xf4) { /* 3byte code first char: 0xf0-0xf4 */
			filter->status = 0x30;
			filter->cache = c & 0x7;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
		}
		break;

	case 0x10: /* 2byte code 2nd char: 0x80-0xbf */
	case 0x21: /* 3byte code 3rd char: 0x80-0xbf */
	case 0x32: /* 4byte code 4th char: 0x80-0xbf */
		filter->status = 0;
		if (c >= 0x80 && c <= 0xbf) {
			s = (filter->cache << 6) | (c & 0x3f);
			filter->cache = 0;

			if (filter->from->no_encoding == mbfl_no_encoding_utf8_docomo && mbfilter_conv_r_map_tbl(s, &s1, 4, mbfl_docomo2uni_pua)) {
				s = mbfilter_sjis_emoji_docomo2unicode(s1, &snd);
			} else if (filter->from->no_encoding == mbfl_no_encoding_utf8_kddi_a && mbfilter_conv_r_map_tbl(s, &s1, 7, mbfl_kddi2uni_pua)) {
				s = mbfilter_sjis_emoji_kddi2unicode(s1, &snd);
			} else if (filter->from->no_encoding == mbfl_no_encoding_utf8_kddi_b && mbfilter_conv_r_map_tbl(s, &s1, 8, mbfl_kddi2uni_pua_b)) {
				s = mbfilter_sjis_emoji_kddi2unicode(s1, &snd);
			} else if (filter->from->no_encoding == mbfl_no_encoding_utf8_sb && mbfilter_conv_r_map_tbl(s, &s1, 6, mbfl_sb2uni_pua)) {
				s = mbfilter_sjis_emoji_sb2unicode(s1, &snd);
			}

			if (snd > 0) {
				CK((*filter->output_function)(snd, filter->data));
			}
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;

	case 0x20: /* 3byte code 2nd char: 0:0xa0-0xbf,D:0x80-9F,1-C,E-F:0x80-0x9f */
		s = (filter->cache << 6) | (c & 0x3f);
		c1 = filter->cache & 0xf;

		if ((c >= 0x80 && c <= 0xbf) &&
			 ((c1 == 0x0 && c >= 0xa0) ||
			 (c1 == 0xd && c < 0xa0) ||
			 (c1 > 0x0 && c1 != 0xd))) {
			filter->cache = s;
			filter->status++;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;

	case 0x30: /* 4byte code 2nd char: 0:0x90-0xbf,1-3:0x80-0xbf,4:0x80-0x8f */
		s = (filter->cache << 6) | (c & 0x3f);
		c1 = filter->cache & 0x7;

		if ((c >= 0x80 && c <= 0xbf) &&
			 ((c1 == 0x0 && c >= 0x90) ||
			 (c1 == 0x4 && c < 0x90) ||
			 (c1 > 0x0 && c1 != 0x4))) {
			filter->cache = s;
			filter->status++;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;

	case 0x31: /* 4byte code 3rd char: 0x80-0xbf */
		if (c >= 0x80 && c <= 0xbf) {
			filter->cache = (filter->cache << 6) | (c & 0x3f);
			filter->status++;
		} else {
			CK(mbfl_filt_put_invalid_char(filter));
			goto retry;
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_utf8_mobile(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x110000) {
		int s1, c1;

		if ((filter->to->no_encoding == mbfl_no_encoding_utf8_docomo && mbfilter_unicode2sjis_emoji_docomo(c, &s1, filter) > 0 && mbfilter_conv_map_tbl(s1, &c1, 4, mbfl_docomo2uni_pua)) ||
			  (filter->to->no_encoding == mbfl_no_encoding_utf8_kddi_a && mbfilter_unicode2sjis_emoji_kddi_sjis(c, &s1, filter) > 0 && mbfilter_conv_map_tbl(s1, &c1, 7, mbfl_kddi2uni_pua)) ||
			  (filter->to->no_encoding == mbfl_no_encoding_utf8_kddi_b && mbfilter_unicode2sjis_emoji_kddi_sjis(c, &s1, filter) > 0 && mbfilter_conv_map_tbl(s1, &c1, 8, mbfl_kddi2uni_pua_b)) ||
			  (filter->to->no_encoding == mbfl_no_encoding_utf8_sb && mbfilter_unicode2sjis_emoji_sb(c, &s1, filter) > 0 && mbfilter_conv_map_tbl(s1, &c1, 6, mbfl_sb2uni_pua))) {
			c = c1;
		}

		if (filter->status) {
			return 0;
		}

		if (c < 0x80) {
			CK((*filter->output_function)(c, filter->data));
		} else if (c < 0x800) {
			CK((*filter->output_function)(((c >> 6) & 0x1f) | 0xc0, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else if (c < 0x10000) {
			CK((*filter->output_function)(((c >> 12) & 0x0f) | 0xe0, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else {
			CK((*filter->output_function)(((c >> 18) & 0x07) | 0xf0, filter->data));
			CK((*filter->output_function)(((c >> 12) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

/* Regional Indicator Unicode codepoints are from 0x1F1E6-0x1F1FF
 * These correspond to the letters A-Z
 * To display the flag emoji for a country, two unicode codepoints are combined,
 * which correspond to the two-letter code for that country
 * This macro converts uppercase ASCII values to Regional Indicator codepoints */
#define NFLAGS(c) (0x1F1A5+(int)(c))

static const char nflags_s[10][2] = {"CN","DE","ES","FR","GB","IT","JP","KR","RU","US"};
static const int nflags_code_kddi[10] = { 0x2549, 0x2546, 0x24C0, 0x2545, 0x2548, 0x2547, 0x2750, 0x254A, 0x24C1, 0x27F7 };
static const int nflags_code_sb[10] = { 0x2B0A, 0x2B05, 0x2B08, 0x2B04, 0x2B07, 0x2B06, 0x2B02, 0x2B0B, 0x2B09, 0x2B03 };

static size_t mb_mobile_utf8_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state, const unsigned short emoji_map[][3], int (*convert_emoji)(int s, int *snd), int n)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c = *p++;
		unsigned int s = 0;

		if (c <= 0x7F) {
			*out++ = c;
			continue;
		} else if (c >= 0xC2 && c <= 0xDF && p < e) {
			unsigned char c2 = *p++;

			if ((c2 & 0xC0) == 0x80) {
				s = ((c & 0x1F) << 6) | (c2 & 0x3F);
			} else {
				*out++ = MBFL_BAD_INPUT;
				p--;
				continue;
			}
		} else if (c >= 0xE0 && c <= 0xEF) {
			if ((e - p) < 2) {
				*out++ = MBFL_BAD_INPUT;
				if (p < e && (c != 0xE0 || *p >= 0xA0) && (c != 0xED || *p < 0xA0) && (*p & 0xC0) == 0x80) {
					p++;
					if (p < e && (*p & 0xC0) == 0x80) {
						p++;
					}
				}
				continue;
			}
			unsigned char c2 = *p++;
			unsigned char c3 = *p++;

			if ((c2 & 0xC0) != 0x80 || (c == 0xE0 && c2 < 0xA0) || (c == 0xED && c2 >= 0xA0)) {
				*out++ = MBFL_BAD_INPUT;
				p -= 2;
				continue;
			} else if ((c3 & 0xC0) != 0x80) {
				*out++ = MBFL_BAD_INPUT;
				p--;
				continue;
			} else {
				s = ((c & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
			}
		} else if (c >= 0xF0 && c <= 0xF4) {
			if ((e - p) < 3) {
				*out++ = MBFL_BAD_INPUT;
				if (p < e) {
					unsigned char c2 = *p;
					if ((c == 0xF0 && c2 >= 0x90) || (c == 0xF4 && c2 < 0x90) || (c >= 0xF1 && c <= 0xF3)) {
						while (p < e && (*p & 0xC0) == 0x80) {
							p++;
						}
					}
				}
				continue;
			}
			unsigned char c2 = *p++;
			unsigned char c3 = *p++;
			unsigned char c4 = *p++;

			if ((c2 & 0xC0) != 0x80 || (c == 0xF0 && c2 < 0x90) || (c == 0xF4 && c2 >= 0x90)) {
				*out++ = MBFL_BAD_INPUT;
				p -= 3;
				continue;
			} else if ((c3 & 0xC0) != 0x80) {
				*out++ = MBFL_BAD_INPUT;
				p -= 2;
				continue;
			} else if ((c4 & 0xC0) != 0x80) {
				*out++ = MBFL_BAD_INPUT;
				p--;
				continue;
			} else {
				s = ((c & 0x7) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
			}
		} else {
			*out++ = MBFL_BAD_INPUT;
			continue;
		}

		int s1 = 0, snd = 0;
		if (mbfilter_conv_r_map_tbl(s, &s1, n, emoji_map)) {
			s = convert_emoji(s1, &snd);
			if (snd) {
				*out++ = snd;
			}
		}
		*out++ = s;
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static size_t mb_utf8_docomo_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	return mb_mobile_utf8_to_wchar(in, in_len, buf, bufsize, state, mbfl_docomo2uni_pua, mbfilter_sjis_emoji_docomo2unicode, 4);
}

static void mb_wchar_to_utf8_docomo(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;
		int c1 = 0;

		if (w < 0x110000) {
			if ((w == '#' || (w >= '0' && w <= '9')) && len) {
				uint32_t w2 = *in++; len--;

				if (w2 == 0x20E3) {
					if (w == '#') {
						s = 0x2964;
					} else if (w == '0') {
						s = 0x296F;
					} else {
						s = 0x2966 + (w - '1');
					}
				} else {
					in--; len++;
				}
			} else if (w == 0xA9) { /* Copyright sign */
				s = 0x29B5;
			} else if (w == 0xAE) { /* Registered sign */
				s = 0x29BA;
			} else if (w >= mb_tbl_uni_docomo2code2_min && w <= mb_tbl_uni_docomo2code2_max) {
				int i = mbfl_bisec_srch2(w, mb_tbl_uni_docomo2code2_key, mb_tbl_uni_docomo2code2_len);
				if (i >= 0) {
					s = mb_tbl_uni_docomo2code2_value[i];
				}
			} else if (w >= mb_tbl_uni_docomo2code3_min && w <= mb_tbl_uni_docomo2code3_max) {
				int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_docomo2code3_key, mb_tbl_uni_docomo2code3_len);
				if (i >= 0) {
					s = mb_tbl_uni_docomo2code3_value[i];
				}
			} else if (w >= mb_tbl_uni_docomo2code5_min && w <= mb_tbl_uni_docomo2code5_max) {
				int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_docomo2code5_key, mb_tbl_uni_docomo2code5_len);
				if (i >= 0) {
					s = mb_tbl_uni_docomo2code5_val[i];
				}
			}

			if (s && mbfilter_conv_map_tbl(s, &c1, 4, mbfl_docomo2uni_pua)) {
				w = c1;
			}

			if (w <= 0x7F) {
				out = mb_convert_buf_add(out, w);
			} else if (w <= 0x7FF) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				out = mb_convert_buf_add2(out, ((w >> 6) & 0x1F) | 0xC0, (w & 0x3F) | 0x80);
			} else if (w <= 0xFFFF) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
				out = mb_convert_buf_add3(out, ((w >> 12) & 0xF) | 0xE0, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add4(out, ((w >> 18) & 0x7) | 0xF0, ((w >> 12) & 0x3F) | 0x80, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
			}
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf8_docomo);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_utf8_kddi_a_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	return mb_mobile_utf8_to_wchar(in, in_len, buf, bufsize, state, mbfl_kddi2uni_pua, mbfilter_sjis_emoji_kddi2unicode, 7);
}

static void mb_wchar_to_utf8_kddi(uint32_t *in, size_t len, mb_convert_buf *buf, bool end, const unsigned short emoji_map[][3], int n, mb_from_wchar_fn error_handler)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;
		int c1 = 0;

		if (w < 0x110000) {
			if ((w == '#' || (w >= '0' && w <= '9')) && len) {
				uint32_t w2 = *in++; len--;

				if (w2 == 0x20E3) {
					if (w == '#') {
						s = 0x25BC;
					} else if (w == '0') {
						s = 0x2830;
					} else {
						s = 0x27A6 + (w - '1');
					}
				} else {
					in--; len++;
				}
			} else if (w >= NFLAGS('C') && w <= NFLAGS('U')) { /* C for CN, U for US */
				if (len) {
					uint32_t w2 = *in++; len--;

					if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
						for (int i = 0; i < 10; i++) {
							if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
								s = nflags_code_kddi[i];
								goto process_kuten;
							}
						}
					}

					in--; len++;
				}

				MB_CONVERT_ERROR(buf, out, limit, w, error_handler);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				continue;
			} else if (w == 0xA9) { /* Copyright sign */
				s = 0x27DC;
			} else if (w == 0xAE) { /* Registered sign */
				s = 0x27DD;
			} else if (w >= mb_tbl_uni_kddi2code2_min && w <= mb_tbl_uni_kddi2code2_max) {
				int i = mbfl_bisec_srch2(w, mb_tbl_uni_kddi2code2_key, mb_tbl_uni_kddi2code2_len);
				if (i >= 0) {
					s = mb_tbl_uni_kddi2code2_value[i];
				}
			} else if (w >= mb_tbl_uni_kddi2code3_min && w <= mb_tbl_uni_kddi2code3_max) {
				int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_kddi2code3_key, mb_tbl_uni_kddi2code3_len);
				if (i >= 0) {
					s = mb_tbl_uni_kddi2code3_value[i];
				}
			} else if (w >= mb_tbl_uni_kddi2code5_min && w <= mb_tbl_uni_kddi2code5_max) {
				int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_kddi2code5_key, mb_tbl_uni_kddi2code5_len);
				if (i >= 0) {
					s = mb_tbl_uni_kddi2code5_val[i];
				}
			}

process_kuten:
			if (s && mbfilter_conv_map_tbl(s, &c1, n, emoji_map)) {
				w = c1;
			}

			if (w <= 0x7F) {
				out = mb_convert_buf_add(out, w);
			} else if (w <= 0x7FF) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				out = mb_convert_buf_add2(out, ((w >> 6) & 0x1F) | 0xC0, (w & 0x3F) | 0x80);
			} else if (w <= 0xFFFF) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
				out = mb_convert_buf_add3(out, ((w >> 12) & 0xF) | 0xE0, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add4(out, ((w >> 18) & 0x7) | 0xF0, ((w >> 12) & 0x3F) | 0x80, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
			}
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, error_handler);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_utf8_kddi_a(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	mb_wchar_to_utf8_kddi(in, len, buf, end, mbfl_kddi2uni_pua, 7, mb_wchar_to_utf8_kddi_a);
}

static size_t mb_utf8_kddi_b_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	return mb_mobile_utf8_to_wchar(in, in_len, buf, bufsize, state, mbfl_kddi2uni_pua_b, mbfilter_sjis_emoji_kddi2unicode, 8);
}

static void mb_wchar_to_utf8_kddi_b(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	mb_wchar_to_utf8_kddi(in, len, buf, end, mbfl_kddi2uni_pua_b, 8, mb_wchar_to_utf8_kddi_b);
}

static size_t mb_utf8_sb_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	return mb_mobile_utf8_to_wchar(in, in_len, buf, bufsize, state, mbfl_sb2uni_pua, mbfilter_sjis_emoji_sb2unicode, 6);
}

static void mb_wchar_to_utf8_sb(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;
		int c1 = 0;

		if (w < 0x110000) {
			if ((w == '#' || (w >= '0' && w <= '9')) && len) {
				uint32_t w2 = *in++; len--;

				if (w2 == 0x20E3) {
					if (w == '#') {
						s = 0x2817;
					} else if (w == '0') {
						s = 0x282C;
					} else {
						s = 0x2823 + (w - '1');
					}
				} else {
					in--; len++;
				}
			} else if (w >= NFLAGS('C') && w <= NFLAGS('U')) { /* C for CN, U for US */
				if (len) {
					uint32_t w2 = *in++; len--;

					if (w2 >= NFLAGS('B') && w2 <= NFLAGS('U')) { /* B for GB, U for RU */
						for (int i = 0; i < 10; i++) {
							if (w == NFLAGS(nflags_s[i][0]) && w2 == NFLAGS(nflags_s[i][1])) {
								s = nflags_code_sb[i];
								goto process_kuten;
							}
						}
					}

					in--; len++;
				}

				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf8_sb);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				continue;
			} else if (w == 0xA9) { /* Copyright sign */
				s = 0x2855;
			} else if (w == 0xAE) { /* Registered sign */
				s = 0x2856;
			} else if (w >= mb_tbl_uni_sb2code2_min && w <= mb_tbl_uni_sb2code2_max) {
				int i = mbfl_bisec_srch2(w, mb_tbl_uni_sb2code2_key, mb_tbl_uni_sb2code2_len);
				if (i >= 0) {
					s = mb_tbl_uni_sb2code2_value[i];
				}
			} else if (w >= mb_tbl_uni_sb2code3_min && w <= mb_tbl_uni_sb2code3_max) {
				int i = mbfl_bisec_srch2(w - 0x10000, mb_tbl_uni_sb2code3_key, mb_tbl_uni_sb2code3_len);
				if (i >= 0) {
					s = mb_tbl_uni_sb2code3_value[i];
				}
			} else if (w >= mb_tbl_uni_sb2code5_min && w <= mb_tbl_uni_sb2code5_max) {
				int i = mbfl_bisec_srch2(w - 0xF0000, mb_tbl_uni_sb2code5_key, mb_tbl_uni_sb2code5_len);
				if (i >= 0) {
					s = mb_tbl_uni_sb2code5_val[i];
				}
			}

process_kuten:
			if (s && mbfilter_conv_map_tbl(s, &c1, 6, mbfl_sb2uni_pua)) {
				w = c1;
			}

			if (w <= 0x7F) {
				out = mb_convert_buf_add(out, w);
			} else if (w <= 0x7FF) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				out = mb_convert_buf_add2(out, ((w >> 6) & 0x1F) | 0xC0, (w & 0x3F) | 0x80);
			} else if (w <= 0xFFFF) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
				out = mb_convert_buf_add3(out, ((w >> 12) & 0xF) | 0xE0, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
			} else {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
				out = mb_convert_buf_add4(out, ((w >> 18) & 0x7) | 0xF0, ((w >> 12) & 0x3F) | 0x80, ((w >> 6) & 0x3F) | 0x80, (w & 0x3F) | 0x80);
			}
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf8_sb);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
