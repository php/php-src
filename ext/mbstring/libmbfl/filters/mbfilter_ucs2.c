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
#include "mbfilter_ucs2.h"

static int mbfl_filt_conv_ucs2_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_ucs2_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static size_t mb_ucs2be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_ucs2be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_ucs2le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_ucs2le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_ucs2_aliases[] = {"ISO-10646-UCS-2", "UCS2" , "UNICODE", NULL};

/* This library historically had encodings called 'byte2be' and 'byte2le'
 * which were almost identical to UCS-2, except that they would truncate
 * Unicode codepoints higher than 0xFFFF quietly
 * Maintain minimal support by aliasing to UCS-2 */
static const char *mbfl_encoding_ucs2be_aliases[] = {"byte2be", NULL};
static const char *mbfl_encoding_ucs2le_aliases[] = {"byte2le", NULL};

const mbfl_encoding mbfl_encoding_ucs2 = {
	mbfl_no_encoding_ucs2,
	"UCS-2",
	"UCS-2",
	mbfl_encoding_ucs2_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2,
	&vtbl_ucs2_wchar,
	&vtbl_wchar_ucs2,
	mb_ucs2_to_wchar,
	mb_wchar_to_ucs2be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_ucs2be = {
	mbfl_no_encoding_ucs2be,
	"UCS-2BE",
	"UCS-2BE",
	mbfl_encoding_ucs2be_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2,
	&vtbl_ucs2be_wchar,
	&vtbl_wchar_ucs2be,
	mb_ucs2be_to_wchar,
	mb_wchar_to_ucs2be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_ucs2le = {
	mbfl_no_encoding_ucs2le,
	"UCS-2LE",
	"UCS-2LE",
	mbfl_encoding_ucs2le_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2,
	&vtbl_ucs2le_wchar,
	&vtbl_wchar_ucs2le,
	mb_ucs2le_to_wchar,
	mb_wchar_to_ucs2le,
	NULL,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_ucs2_wchar = {
	mbfl_no_encoding_ucs2,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs2_wchar,
	mbfl_filt_conv_ucs2_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs2 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs2be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_ucs2be_wchar = {
	mbfl_no_encoding_ucs2be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs2be_wchar,
	mbfl_filt_conv_ucs2_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs2be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs2be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_ucs2le_wchar = {
	mbfl_no_encoding_ucs2le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs2le_wchar,
	mbfl_filt_conv_ucs2_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs2le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs2le,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_ucs2_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status == 0) {
		filter->status = 1;
		filter->cache = c & 0xFF;
	} else {
		filter->status = 0;
		int n = (filter->cache << 8) | (c & 0xFF);
		if (n == 0xFFFE) {
			/* Found little-endian byte order mark */
			filter->filter_function = mbfl_filt_conv_ucs2le_wchar;
		} else {
			filter->filter_function = mbfl_filt_conv_ucs2be_wchar;
			if (n != 0xFEFF) {
				CK((*filter->output_function)(n, filter->data));
			}
		}
	}
	return 0;
}

int mbfl_filt_conv_ucs2be_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status == 0) {
		filter->status = 1;
		filter->cache = (c & 0xFF) << 8;
	} else {
		filter->status = 0;
		CK((*filter->output_function)((c & 0xFF) | filter->cache, filter->data));
	}
	return 0;
}

int mbfl_filt_conv_wchar_ucs2be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)((c >> 8) & 0xFF, filter->data));
		CK((*filter->output_function)(c & 0xFF, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return 0;
}

int mbfl_filt_conv_ucs2le_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status == 0) {
		filter->status = 1;
		filter->cache = c & 0xFF;
	} else {
		filter->status = 0;
		CK((*filter->output_function)(((c & 0xFF) << 8) | filter->cache, filter->data));
	}
	return 0;
}

int mbfl_filt_conv_wchar_ucs2le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)(c & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xFF, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return 0;
}

static int mbfl_filt_conv_ucs2_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define DETECTED_BE 1
#define DETECTED_LE 2

static size_t mb_ucs2_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	if (*state == DETECTED_BE) {
		return mb_ucs2be_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*state == DETECTED_LE) {
		return mb_ucs2le_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*in_len >= 2) {
		unsigned char *p = *in;
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint32_t w = (c1 << 8) | c2;

		if (w == 0xFFFE) {
			/* Little-endian BOM */
			*in = p;
			*in_len -= 2;
			*state = DETECTED_LE;
			return mb_ucs2le_to_wchar(in, in_len, buf, bufsize, NULL);
		} else if (w == 0xFEFF) {
			/* Big-endian BOM; don't send it to output */
			*in = p;
			*in_len -= 2;
		}
	}

	*state = DETECTED_BE;
	return mb_ucs2be_to_wchar(in, in_len, buf, bufsize, NULL);
}

static size_t mb_ucs2be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + (*in_len & ~1);
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint32_t w = (c1 << 8) | c2;
		*out++ = w;
	}

	if (p == e && (*in_len & 0x1) && out < limit) {
		/* There is 1 trailing byte, which shouldn't be there */
		*out++ = MBFL_BAD_INPUT;
		p++;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_ucs2be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		if (w < MBFL_WCSPLANE_UCS2MAX) {
			out = mb_convert_buf_add2(out, (w >> 8) & 0xFF, w & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_ucs2be);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_ucs2le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + (*in_len & ~1);
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint32_t w = (c2 << 8) | c1;
		*out++ = w;
	}

	if (p == e && (*in_len & 0x1) && out < limit) {
		/* There is 1 trailing byte, which shouldn't be there */
		*out++ = MBFL_BAD_INPUT;
		p++;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_ucs2le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		if (w < MBFL_WCSPLANE_UCS2MAX) {
			out = mb_convert_buf_add2(out, w & 0xFF, (w >> 8) & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_ucs2le);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
