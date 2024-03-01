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
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_ucs4.h"

static size_t mb_ucs4_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static size_t mb_ucs4be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_ucs4be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_ucs4le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_ucs4le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_ucs4_aliases[] = {"ISO-10646-UCS-4", "UCS4", NULL};

/* This library historically had encodings called 'byte4be' and 'byte4le'
 * which were almost identical to UCS-4
 * Maintain minimal support by aliasing to UCS-4 */
static const char *mbfl_encoding_ucs4be_aliases[] = {"byte4be", NULL};
static const char *mbfl_encoding_ucs4le_aliases[] = {"byte4le", NULL};

static int mbfl_filt_conv_ucs4_wchar_flush(mbfl_convert_filter *filter);

const mbfl_encoding mbfl_encoding_ucs4 = {
	mbfl_no_encoding_ucs4,
	"UCS-4",
	"UCS-4",
	mbfl_encoding_ucs4_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4_wchar,
	&vtbl_wchar_ucs4,
	mb_ucs4_to_wchar,
	mb_wchar_to_ucs4be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_ucs4be = {
	mbfl_no_encoding_ucs4be,
	"UCS-4BE",
	"UCS-4BE",
	mbfl_encoding_ucs4be_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4be_wchar,
	&vtbl_wchar_ucs4be,
	mb_ucs4be_to_wchar,
	mb_wchar_to_ucs4be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_ucs4le = {
	mbfl_no_encoding_ucs4le,
	"UCS-4LE",
	"UCS-4LE",
	mbfl_encoding_ucs4le_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4le_wchar,
	&vtbl_wchar_ucs4le,
	mb_ucs4le_to_wchar,
	mb_wchar_to_ucs4le,
	NULL,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_ucs4_wchar = {
	mbfl_no_encoding_ucs4,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4_wchar,
	mbfl_filt_conv_ucs4_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs4 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_ucs4be_wchar = {
	mbfl_no_encoding_ucs4be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4be_wchar,
	mbfl_filt_conv_ucs4_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs4be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_ucs4le_wchar = {
	mbfl_no_encoding_ucs4le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4le_wchar,
	mbfl_filt_conv_ucs4_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs4le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs4le,
	mbfl_filt_conv_common_flush,
	NULL,
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * UCS-4 => wchar
 */
int mbfl_filt_conv_ucs4_wchar(int c, mbfl_convert_filter *filter)
{
	int n, endian;

	endian = filter->status & 0xff00;
	switch (filter->status & 0xff) {
	case 0:
		if (endian) {
			n = c & 0xff;
		} else {
			n = (c & 0xffu) << 24;
		}
		filter->cache = n;
		filter->status++;
		break;
	case 1:
		if (endian) {
			n = (c & 0xff) << 8;
		} else {
			n = (c & 0xff) << 16;
		}
		filter->cache |= n;
		filter->status++;
		break;
	case 2:
		if (endian) {
			n = (c & 0xff) << 16;
		} else {
			n = (c & 0xff) << 8;
		}
		filter->cache |= n;
		filter->status++;
		break;
	default:
		if (endian) {
			n = (c & 0xffu) << 24;
		} else {
			n = c & 0xff;
		}
		n |= filter->cache;
		filter->status &= ~0xff;
		if ((n & 0xffff) == 0 && ((n >> 16) & 0xffff) == 0xfffe) {
			if (endian) {
				filter->status = 0;		/* big-endian */
			} else {
				filter->status = 0x100;		/* little-endian */
			}
		} else if (n != 0xfeff) {
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return 0;
}

/*
 * UCS-4BE => wchar
 */
int mbfl_filt_conv_ucs4be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = (c & 0xffu) << 24;
		filter->cache = n;
	} else if (filter->status == 1) {
		filter->status = 2;
		n = (c & 0xff) << 16;
		filter->cache |= n;
	} else if (filter->status == 2) {
		filter->status = 3;
		n = (c & 0xff) << 8;
		filter->cache |= n;
	} else {
		filter->status = 0;
		n = (c & 0xff) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return 0;
}

/*
 * wchar => UCS-4BE
 */
int mbfl_filt_conv_wchar_ucs4be(int c, mbfl_convert_filter *filter)
{
	if (c != MBFL_BAD_INPUT) {
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

/*
 * UCS-4LE => wchar
 */
int mbfl_filt_conv_ucs4le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = (c & 0xff);
		filter->cache = n;
	} else if (filter->status == 1) {
		filter->status = 2;
		n = (c & 0xff) << 8;
		filter->cache |= n;
	} else if (filter->status == 2) {
		filter->status = 3;
		n = (c & 0xff) << 16;
		filter->cache |= n;
	} else {
		filter->status = 0;
		n = ((c & 0xffu) << 24) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return 0;
}

/*
 * wchar => UCS-4LE
 */
int mbfl_filt_conv_wchar_ucs4le(int c, mbfl_convert_filter *filter)
{
	if (c != MBFL_BAD_INPUT) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_ucs4_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* Input string was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define DETECTED_BE 1
#define DETECTED_LE 2

static size_t mb_ucs4_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	if (*state == DETECTED_BE) {
		return mb_ucs4be_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*state == DETECTED_LE) {
		return mb_ucs4le_to_wchar(in, in_len, buf, bufsize, NULL);
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
			return mb_ucs4le_to_wchar(in, in_len, buf, bufsize, NULL);
		} else if (w == 0xFEFF) {
			/* Big-endian BOM; don't send it to output */
			*in = p;
			*in_len -= 4;
		}
	}

	*state = DETECTED_BE;
	return mb_ucs4be_to_wchar(in, in_len, buf, bufsize, NULL);
}

static size_t mb_ucs4be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + (*in_len & ~3);
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		uint32_t c1 = *p++;
		uint32_t c2 = *p++;
		uint32_t c3 = *p++;
		uint32_t c4 = *p++;
		uint32_t w = (c1 << 24) | (c2 << 16) | (c3 << 8) | c4;
		*out++ = w;
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

static void mb_wchar_to_ucs4be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);

	while (len--) {
		uint32_t w = *in++;
		if (w != MBFL_BAD_INPUT) {
			out = mb_convert_buf_add4(out, (w >> 24) & 0xFF, (w >> 16) & 0xFF, (w >> 8) & 0xFF, w & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_ucs4be);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_ucs4le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + (*in_len & ~3);
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		uint32_t c1 = *p++;
		uint32_t c2 = *p++;
		uint32_t c3 = *p++;
		uint32_t c4 = *p++;
		uint32_t w = (c4 << 24) | (c3 << 16) | (c2 << 8) | c1;
		*out++ = w;
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

static void mb_wchar_to_ucs4le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);

	while (len--) {
		uint32_t w = *in++;
		if (w != MBFL_BAD_INPUT) {
			out = mb_convert_buf_add4(out, w & 0xFF, (w >> 8) & 0xFF, (w >> 16) & 0xFF, (w >> 24) & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_ucs4le);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 4);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
