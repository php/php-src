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

static size_t mb_utf8_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf8(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

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
	NULL
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

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_put_invalid_char(mbfl_convert_filter *filter)
{
	filter->status = filter->cache = 0;
	CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	return 0;
}

int mbfl_filt_conv_utf8_wchar(int c, mbfl_convert_filter *filter)
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

int mbfl_filt_conv_utf8_wchar_flush(mbfl_convert_filter *filter)
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

int mbfl_filt_conv_wchar_utf8(int c, mbfl_convert_filter *filter)
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
