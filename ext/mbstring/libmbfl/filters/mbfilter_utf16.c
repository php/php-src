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
#include "mbfilter_utf16.h"

static int mbfl_filt_conv_utf16_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_utf16_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static size_t mb_utf16be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf16le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_utf16_aliases[] = {"utf16", NULL};

const mbfl_encoding mbfl_encoding_utf16 = {
	mbfl_no_encoding_utf16,
	"UTF-16",
	"UTF-16",
	mbfl_encoding_utf16_aliases,
	NULL,
	0,
	&vtbl_utf16_wchar,
	&vtbl_wchar_utf16,
	mb_utf16_to_wchar,
	mb_wchar_to_utf16be,
	NULL
};

const mbfl_encoding mbfl_encoding_utf16be = {
	mbfl_no_encoding_utf16be,
	"UTF-16BE",
	"UTF-16BE",
	NULL,
	NULL,
	0,
	&vtbl_utf16be_wchar,
	&vtbl_wchar_utf16be,
	mb_utf16be_to_wchar,
	mb_wchar_to_utf16be,
	NULL
};

const mbfl_encoding mbfl_encoding_utf16le = {
	mbfl_no_encoding_utf16le,
	"UTF-16LE",
	"UTF-16LE",
	NULL,
	NULL,
	0,
	&vtbl_utf16le_wchar,
	&vtbl_wchar_utf16le,
	mb_utf16le_to_wchar,
	mb_wchar_to_utf16le,
	NULL
};

const struct mbfl_convert_vtbl vtbl_utf16_wchar = {
	mbfl_no_encoding_utf16,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf16_wchar,
	mbfl_filt_conv_utf16_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf16be_wchar = {
	mbfl_no_encoding_utf16be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf16be_wchar,
	mbfl_filt_conv_utf16_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf16le_wchar = {
	mbfl_no_encoding_utf16le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf16le_wchar,
	mbfl_filt_conv_utf16_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf16le,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_utf16_wchar(int c, mbfl_convert_filter *filter)
{
	/* Start with the assumption that the string is big-endian;
	 * If we find a little-endian BOM, then we will change that assumption */
	if (filter->status == 0) {
		filter->cache = c & 0xFF;
		filter->status = 1;
	} else {
		int n = (filter->cache << 8) | (c & 0xFF);
		filter->cache = filter->status = 0;
		if (n == 0xFFFE) {
			/* Switch to little-endian mode */
			filter->filter_function = mbfl_filt_conv_utf16le_wchar;
		} else {
			filter->filter_function = mbfl_filt_conv_utf16be_wchar;
			if (n >= 0xD800 && n <= 0xDBFF) {
				filter->cache = n & 0x3FF; /* Pick out 10 data bits */
				filter->status = 2;
				return 0;
			} else if (n >= 0xDC00 && n <= 0xDFFF) {
				/* This is wrong; second part of surrogate pair has come first */
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			} else if (n != 0xFEFF) {
				CK((*filter->output_function)(n, filter->data));
			}
		}
	}

	return 0;
}

int mbfl_filt_conv_utf16be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0: /* First byte */
		filter->cache = c & 0xFF;
		filter->status = 1;
		break;

	case 1: /* Second byte */
		n = (filter->cache << 8) | (c & 0xFF);
		if (n >= 0xD800 && n <= 0xDBFF) {
			filter->cache = n & 0x3FF; /* Pick out 10 data bits */
			filter->status = 2;
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			/* This is wrong; second part of surrogate pair has come first */
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else {
			filter->status = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;

	case 2: /* Second part of surrogate, first byte */
		filter->cache = (filter->cache << 8) | (c & 0xFF);
		filter->status = 3;
		break;

	case 3: /* Second part of surrogate, second byte */
		n = ((filter->cache & 0xFF) << 8) | (c & 0xFF);
		if (n >= 0xD800 && n <= 0xDBFF) {
			/* Wrong; that's the first half of a surrogate pair, not the second */
			filter->cache = n & 0x3FF;
			filter->status = 2;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			filter->status = 0;
			n = ((filter->cache & 0x3FF00) << 2) + (n & 0x3FF) + 0x10000;
			CK((*filter->output_function)(n, filter->data));
		} else {
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(n, filter->data));
		}
	}

	return 0;
}

int mbfl_filt_conv_wchar_utf16be(int c, mbfl_convert_filter *filter)
{
	int n;

	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		n = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(n & 0xff, filter->data));
		n = (c & 0x3ff) | 0xdc00;
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(n & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_utf16le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0:
		filter->cache = c & 0xff;
		filter->status = 1;
		break;

	case 1:
		if ((c & 0xfc) == 0xd8) {
			/* Looks like we have a surrogate pair here */
			filter->cache += ((c & 0x3) << 8);
			filter->status = 2;
		} else if ((c & 0xfc) == 0xdc) {
			/* This is wrong; the second part of the surrogate pair has come first */
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else {
			filter->status = 0;
			CK((*filter->output_function)(filter->cache + ((c & 0xff) << 8), filter->data));
		}
		break;

	case 2:
		filter->cache = (filter->cache << 10) + (c & 0xff);
		filter->status = 3;
		break;

	case 3:
		n = (filter->cache & 0xFF) | ((c & 0xFF) << 8);
		if (n >= 0xD800 && n <= 0xDBFF) {
			/* We previously saw the first part of a surrogate pair and were
			 * expecting the second part; this is another first part */
			filter->cache = n & 0x3FF;
			filter->status = 2;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			n = filter->cache + ((c & 0x3) << 8) + 0x10000;
			filter->status = 0;
			CK((*filter->output_function)(n, filter->data));
		} else {
			/* The first part of a surrogate pair was followed by some other codepoint
			 * which is not part of a surrogate pair at all */
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return 0;
}

int mbfl_filt_conv_wchar_utf16le(int c, mbfl_convert_filter *filter)
{
	int n;

	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		n = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->output_function)(n & 0xff, filter->data));
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		n = (c & 0x3ff) | 0xdc00;
		CK((*filter->output_function)(n & 0xff, filter->data));
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_utf16_wchar_flush(mbfl_convert_filter *filter)
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

static size_t mb_utf16_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	if (*state == DETECTED_BE) {
		return mb_utf16be_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*state == DETECTED_LE) {
		return mb_utf16le_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*in_len >= 2) {
		unsigned char *p = *in;
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint16_t n = (c1 << 8) | c2;

		if (n == 0xFFFE) {
			/* Little-endian BOM */
			*in = p;
			*in_len -= 2;
			*state = DETECTED_LE;
			return mb_utf16le_to_wchar(in, in_len, buf, bufsize, NULL);
		} if (n == 0xFEFF) {
			/* Big-endian BOM; don't send to output */
			*in = p;
			*in_len -= 2;
		}
	}

	*state = DETECTED_BE;
	return mb_utf16be_to_wchar(in, in_len, buf, bufsize, NULL);
}

static size_t mb_utf16be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	/* We only want to read 16-bit words out of `str`; any trailing byte will be handled at the end */
	unsigned char *p = *in, *e = p + (*in_len & ~1);
	/* Set `limit` to one less than the actual amount of space in the buffer; this is because
	 * on some iterations of the below loop, we might produce two output words */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint16_t n = (c1 << 8) | c2;

		if (n >= 0xD800 && n <= 0xDBFF) {
			/* Handle surrogate */
			if (p < e) {
				unsigned char c3 = *p++;
				unsigned char c4 = *p++;
				uint16_t n2 = (c3 << 8) | c4;

				if (n2 >= 0xD800 && n2 <= 0xDBFF) {
					/* Wrong; that's the first half of a surrogate pair, when we were expecting the second */
					*out++ = MBFL_BAD_INPUT;
					p -= 2;
				} else if (n2 >= 0xDC00 && n2 <= 0xDFFF) {
					*out++ = (((n & 0x3FF) << 10) | (n2 & 0x3FF)) + 0x10000;
				} else {
					/* The first half of a surrogate pair was followed by a 'normal' codepoint */
					*out++ = MBFL_BAD_INPUT;
					*out++ = n2;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			/* This is wrong; second part of surrogate pair has come first */
			*out++ = MBFL_BAD_INPUT;
		} else {
			*out++ = n;
		}
	}

	if (p == e && (*in_len & 0x1) && out < limit) {
		/* There is an extra trailing byte (which shouldn't be there) */
		*out++ = MBFL_BAD_INPUT;
		p++;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf16be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;

		if (w < MBFL_WCSPLANE_UCS2MAX) {
			out = mb_convert_buf_add2(out, (w >> 8) & 0xFF, w & 0xFF);
		} else if (w < MBFL_WCSPLANE_UTF32MAX) {
			uint16_t n1 = ((w >> 10) - 0x40) | 0xD800;
			uint16_t n2 = (w & 0x3FF) | 0xDC00;
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
			out = mb_convert_buf_add4(out, (n1 >> 8) & 0xFF, n1 & 0xFF, (n2 >> 8) & 0xFF, n2 & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf16be);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_utf16le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	/* We only want to read 16-bit words out of `str`; any trailing byte will be handled at the end */
	unsigned char *p = *in, *e = p + (*in_len & ~1);
	/* Set `limit` to one less than the actual amount of space in the buffer; this is because
	 * on some iterations of the below loop, we might produce two output words */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint16_t n = (c2 << 8) | c1;

		if (n >= 0xD800 && n <= 0xDBFF) {
			/* Handle surrogate */
			if (p < e) {
				unsigned char c3 = *p++;
				unsigned char c4 = *p++;
				uint16_t n2 = (c4 << 8) | c3;

				if (n2 >= 0xD800 && n2 <= 0xDBFF) {
					/* Wrong; that's the first half of a surrogate pair, when we were expecting the second */
					*out++ = MBFL_BAD_INPUT;
					p -= 2;
				} else if (n2 >= 0xDC00 && n2 <= 0xDFFF) {
					*out++ = (((n & 0x3FF) << 10) | (n2 & 0x3FF)) + 0x10000;
				} else {
					/* The first half of a surrogate pair was followed by a 'normal' codepoint */
					*out++ = MBFL_BAD_INPUT;
					*out++ = n2;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			/* This is wrong; second part of surrogate pair has come first */
			*out++ = MBFL_BAD_INPUT;
		} else {
			*out++ = n;
		}
	}

	if (p == e && (*in_len & 0x1) && out < limit) {
		/* There is an extra trailing byte (which shouldn't be there) */
		*out++ = MBFL_BAD_INPUT;
		p++;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf16le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;

		if (w < MBFL_WCSPLANE_UCS2MAX) {
			out = mb_convert_buf_add2(out, w & 0xFF, (w >> 8) & 0xFF);
		} else if (w < MBFL_WCSPLANE_UTF32MAX) {
			uint16_t n1 = ((w >> 10) - 0x40) | 0xD800;
			uint16_t n2 = (w & 0x3FF) | 0xDC00;
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
			out = mb_convert_buf_add4(out, n1 & 0xFF, (n1 >> 8) & 0xFF, n2 & 0xFF, (n2 >> 8) & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf16le);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
