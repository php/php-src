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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 4 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#include "mbfilter.h"
#include "mbfilter_base64.h"

static size_t mb_base64_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_base64(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

const mbfl_encoding mbfl_encoding_base64 = {
	mbfl_no_encoding_base64,
	"BASE64",
	"BASE64",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	NULL,
	NULL,
	mb_base64_to_wchar,
	mb_wchar_to_base64,
	NULL,
	NULL
};

const struct mbfl_convert_vtbl vtbl_8bit_b64 = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_base64,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_base64enc,
	mbfl_filt_conv_base64enc_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_b64_8bit = {
	mbfl_no_encoding_base64,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_base64dec,
	mbfl_filt_conv_base64dec_flush,
	NULL,
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * any => BASE64
 */
static const unsigned char mbfl_base64_table[] = {
 /* 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', */
   0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,
 /* 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
   0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
 /* 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', */
   0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
 /* 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
   0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
 /* '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0' */
   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2b,0x2f,0x00
};

int mbfl_filt_conv_base64enc(int c, mbfl_convert_filter *filter)
{
	int n;

	n = (filter->status & 0xff);
	if (n == 0) {
		filter->status++;
		filter->cache = (c & 0xff) << 16;
	} else if (n == 1) {
		filter->status++;
		filter->cache |= (c & 0xff) << 8;
	} else {
		filter->status &= ~0xff;
		n = (filter->status & 0xff00) >> 8;
		if (n > 72) {
			CK((*filter->output_function)(0x0d, filter->data));		/* CR */
			CK((*filter->output_function)(0x0a, filter->data));		/* LF */
			filter->status &= ~0xff00;
		}
		filter->status += 0x400;
		n = filter->cache | (c & 0xff);
		CK((*filter->output_function)(mbfl_base64_table[(n >> 18) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(n >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(n >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[n & 0x3f], filter->data));
	}

	return 0;
}

int mbfl_filt_conv_base64enc_flush(mbfl_convert_filter *filter)
{
	int status, cache, len;

	status = filter->status & 0xff;
	cache = filter->cache;
	len = (filter->status & 0xff00) >> 8;
	filter->status &= ~0xffff;
	filter->cache = 0;
	/* flush fragments */
	if (status >= 1) {
		if (len > 72){
			CK((*filter->output_function)(0x0d, filter->data));		/* CR */
			CK((*filter->output_function)(0x0a, filter->data));		/* LF */
		}
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 18) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 12) & 0x3f], filter->data));
		if (status == 1) {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		} else {
			CK((*filter->output_function)(mbfl_base64_table[(cache >> 6) & 0x3f], filter->data));
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		}
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

/*
 * BASE64 => any
 */
int mbfl_filt_conv_base64dec(int c, mbfl_convert_filter *filter)
{
	int n;

	if (c == 0x0d || c == 0x0a || c == 0x20 || c == 0x09 || c == 0x3d) {	/* CR or LF or SPACE or HTAB or '=' */
		return 0;
	}

	n = 0;
	if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
		n = c - 65;
	} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
		n = c - 71;
	} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
		n = c + 4;
	} else if (c == 0x2b) {			/* '+' */
		n = 62;
	} else if (c == 0x2f) {			/* '/' */
		n = 63;
	} else {
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		return 0;
	}
	n &= 0x3f;

	switch (filter->status) {
	case 0:
		filter->status = 1;
		filter->cache = n << 18;
		break;
	case 1:
		filter->status = 2;
		filter->cache |= n << 12;
		break;
	case 2:
		filter->status = 3;
		filter->cache |= n << 6;
		break;
	default:
		filter->status = 0;
		n |= filter->cache;
		CK((*filter->output_function)((n >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(n & 0xff, filter->data));
		break;
	}

	return 0;
}

int mbfl_filt_conv_base64dec_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	if (status >= 2) {
		CK((*filter->output_function)((cache >> 16) & 0xff, filter->data));
		if (status >= 3) {
			CK((*filter->output_function)((cache >> 8) & 0xff, filter->data));
		}
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int decode_base64(char c)
{
	if (c >= 'A' && c <= 'Z') {
		return c - 'A';
	} else if (c >= 'a' && c <= 'z') {	/* a - z */
		return c - 'a' + 26;
	} else if (c >= '0' && c <= '9') {	/* 0 - 9 */
		return c - '0' + 52;
	} else if (c == '+') {
		return 62;
	} else if (c == '/') {
		return 63;
	}
	return -1;
}

static size_t mb_base64_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	unsigned int bits = *state & 0xFF, cache = *state >> 8;

	while (p < e && (limit - out) >= 3) {
		unsigned char c = *p++;

		if (c == '\r' || c == '\n' || c == ' ' || c == '\t' || c == '=') {
			continue;
		}

		int value = decode_base64(c);

		if (value == -1) {
			*out++ = MBFL_BAD_INPUT;
		} else {
			bits += 6;
			cache = (cache << 6) | (value & 0x3F);
			if (bits == 24) {
				*out++ = (cache >> 16) & 0xFF;
				*out++ = (cache >> 8) & 0xFF;
				*out++ = cache & 0xFF;
				bits = cache = 0;
			}
		}
	}

	if (p == e) {
		if (bits) {
			/* If we reach here, there will be at least 3 spaces remaining in output buffer */
			if (bits == 18) {
				*out++ = (cache >> 10) & 0xFF;
				*out++ = (cache >> 2) & 0xFF;
			} else if (bits == 12) {
				*out++ = (cache >> 4) & 0xFF;
			}
		}
	} else {
		*state = (cache << 8) | (bits & 0xFF);
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_base64(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned int bits = (buf->state & 0x3) * 8;
	unsigned int chars_output = ((buf->state >> 2) & 0x3F) * 4;
	unsigned int cache = buf->state >> 8;

	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	/* Every 3 bytes of input converts to 4 bytes of output... but if the number of input
	 * bytes is not a multiple of 3, we still pad the output out to a multiple of 4
	 * That's `(len + 2) * 4 / 3`, to calculate the amount of space needed in the output buffer
	 *
	 * But also, we add a CR+LF line ending (2 bytes) for every 76 bytes of output
	 * That means we must multiply the above number by 78/76
	 * Use `zend_safe_address_guarded` to check that the multiplication doesn't overflow
	 *
	 * And since we may enter this function multiple times when converting a large string, and
	 * we might already be close to where a CR+LF needs to be emitted, make space for an extra
	 * CR+LF pair in the output buffer */
	MB_CONVERT_BUF_ENSURE(buf, out, limit, (zend_safe_address_guarded(len + (bits / 8), 26, 52) / 19) + 2);

	while (len--) {
		uint32_t w = *in++;
		cache = (cache << 8) | (w & 0xFF);
		bits += 8;
		if (bits == 24) {
			if (chars_output > 72) {
				out = mb_convert_buf_add2(out, '\r', '\n');
				chars_output = 0;
			}
			out = mb_convert_buf_add4(out,
				mbfl_base64_table[(cache >> 18) & 0x3F],
				mbfl_base64_table[(cache >> 12) & 0x3F],
				mbfl_base64_table[(cache >> 6) & 0x3F],
				mbfl_base64_table[cache & 0x3F]);
			chars_output += 4;
			bits = cache = 0;
		}
	}

	if (end && bits) {
		if (chars_output > 72) {
			out = mb_convert_buf_add2(out, '\r', '\n');
			chars_output = 0;
		}
		if (bits == 8) {
			out = mb_convert_buf_add4(out, mbfl_base64_table[(cache >> 2) & 0x3F], mbfl_base64_table[(cache & 0x3) << 4], '=', '=');
		} else {
			out = mb_convert_buf_add4(out, mbfl_base64_table[(cache >> 10) & 0x3F], mbfl_base64_table[(cache >> 4) & 0x3F], mbfl_base64_table[(cache & 0xF) << 2], '=');
		}
	} else {
		buf->state = (cache << 8) | (((chars_output / 4) & 0x3F) << 2) | ((bits / 8) & 0x3);
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
