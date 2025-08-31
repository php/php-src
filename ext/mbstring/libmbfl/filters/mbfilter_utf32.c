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
 * The source code included in this file was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 20 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_utf32.h"

static int mbfl_filt_conv_utf32_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_utf32_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static size_t mb_utf32be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf32be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf32le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf32le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_utf32_aliases[] = {"utf32", NULL};

const mbfl_encoding mbfl_encoding_utf32 = {
	mbfl_no_encoding_utf32,
	"UTF-32",
	"UTF-32",
	mbfl_encoding_utf32_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_utf32_wchar,
	&vtbl_wchar_utf32,
	mb_utf32_to_wchar,
	mb_wchar_to_utf32be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_utf32be = {
	mbfl_no_encoding_utf32be,
	"UTF-32BE",
	"UTF-32BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_utf32be_wchar,
	&vtbl_wchar_utf32be,
	mb_utf32be_to_wchar,
	mb_wchar_to_utf32be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_utf32le = {
	mbfl_no_encoding_utf32le,
	"UTF-32LE",
	"UTF-32LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_utf32le_wchar,
	&vtbl_wchar_utf32le,
	mb_utf32le_to_wchar,
	mb_wchar_to_utf32le,
	NULL,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf32_wchar = {
	mbfl_no_encoding_utf32,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf32_wchar,
	mbfl_filt_conv_utf32_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf32 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf32be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf32be_wchar = {
	mbfl_no_encoding_utf32be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf32be_wchar,
	mbfl_filt_conv_utf32_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf32be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf32be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf32le_wchar = {
	mbfl_no_encoding_utf32le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf32le_wchar,
	mbfl_filt_conv_utf32_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf32le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf32le,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

static int emit_char_if_valid(int n, mbfl_convert_filter *filter)
{
	if (n >= 0 && n < MBFL_WCSPLANE_UTF32MAX && (n < 0xD800 || n > 0xDFFF)) {
		CK((*filter->output_function)(n, filter->data));
	} else {
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	return 0;
}

int mbfl_filt_conv_utf32_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status < 3) {
		filter->cache = (filter->cache << 8) | (c & 0xFF);
		filter->status++;
	} else {
		int n = ((unsigned int)filter->cache << 8) | (c & 0xFF);
		filter->cache = filter->status = 0;

		if (n == 0xFFFE0000) {
			/* Found a little-endian byte order mark */
			filter->filter_function = mbfl_filt_conv_utf32le_wchar;
		} else {
			filter->filter_function = mbfl_filt_conv_utf32be_wchar;
			if (n != 0xFEFF) {
				CK(emit_char_if_valid(n, filter));
			}
		}
	}

	return 0;
}

int mbfl_filt_conv_utf32be_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status < 3) {
		filter->cache = (filter->cache << 8) | (c & 0xFF);
		filter->status++;
	} else {
		int n = ((unsigned int)filter->cache << 8) | (c & 0xFF);
		filter->cache = filter->status = 0;
		CK(emit_char_if_valid(n, filter));
	}
	return 0;
}

int mbfl_filt_conv_wchar_utf32be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UTF32MAX) {
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_utf32le_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status < 3) {
		filter->cache |= ((c & 0xFFU) << (8 * filter->status));
		filter->status++;
	} else {
		int n = ((c & 0xFFU) << 24) | filter->cache;
		filter->cache = filter->status = 0;
		CK(emit_char_if_valid(n, filter));
	}
	return 0;
}

int mbfl_filt_conv_wchar_utf32le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UTF32MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_utf32_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->cache = filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define DETECTED_BE 1
#define DETECTED_LE 2

static size_t mb_utf32_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	if (*state == DETECTED_BE) {
		return mb_utf32be_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*state == DETECTED_LE) {
		return mb_utf32le_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*in_len >= 4) {
		unsigned char *p = *in;
		uint32_t c1 = *p++;
		uint32_t c2 = *p++;
		uint32_t c3 = *p++;
		uint32_t c4 = *p++;
		uint32_t w = (c1 << 24) | (c2 << 16) | (c3 << 8) | c4;

		if (w == 0xFFFE0000) {
			/* Little-endian BOM */
			*in = p;
			*in_len -= 4;
			*state = DETECTED_LE;
			return mb_utf32le_to_wchar(in, in_len, buf, bufsize, NULL);
		} else if (w == 0xFEFF) {
			/* Big-endian BOM; don't send it to output */
			*in = p;
			*in_len -= 4;
		}
	}

	*state = DETECTED_BE;
	return mb_utf32be_to_wchar(in, in_len, buf, bufsize, NULL);
}

static size_t mb_utf32be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + (*in_len & ~3);
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		uint32_t c1 = *p++;
		uint32_t c2 = *p++;
		uint32_t c3 = *p++;
		uint32_t c4 = *p++;
		uint32_t w = (c1 << 24) | (c2 << 16) | (c3 << 8) | c4;

		if (w < MBFL_WCSPLANE_UTF32MAX && (w < 0xD800 || w > 0xDFFF)) {
			*out++ = w;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	if (p == e && (*in_len & 0x3) && out < limit) {
		/* There are 1-3 trailing bytes, which shouldn't be there */
		*out++ = MBFL_BAD_INPUT;
		p = *in + *in_len;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf32be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);

	while (len--) {
		uint32_t w = *in++;
		if (w < MBFL_WCSPLANE_UTF32MAX) {
			out = mb_convert_buf_add4(out, (w >> 24) & 0xFF, (w >> 16) & 0xFF, (w >> 8) & 0xFF, w & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf32be);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_utf32le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + (*in_len & ~3);
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		uint32_t c1 = *p++;
		uint32_t c2 = *p++;
		uint32_t c3 = *p++;
		uint32_t c4 = *p++;
		uint32_t w = (c4 << 24) | (c3 << 16) | (c2 << 8) | c1;

		if (w < MBFL_WCSPLANE_UTF32MAX && (w < 0xD800 || w > 0xDFFF)) {
			*out++ = w;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	if (p == e && (*in_len & 0x3) && out < limit) {
		/* There are 1-3 trailing bytes, which shouldn't be there */
		*out++ = MBFL_BAD_INPUT;
		p = *in + *in_len;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf32le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);

	while (len--) {
		uint32_t w = *in++;
		if (w < MBFL_WCSPLANE_UTF32MAX) {
			out = mb_convert_buf_add4(out, w & 0xFF, (w >> 8) & 0xFF, (w >> 16) & 0xFF, (w >> 24) & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf32le);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
