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
#include "mbfilter_qprint.h"

static size_t mb_qprint_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_qprint(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_qprint_aliases[] = {"qprint", NULL};

const mbfl_encoding mbfl_encoding_qprint = {
	mbfl_no_encoding_qprint,
	"Quoted-Printable",
	"Quoted-Printable",
	mbfl_encoding_qprint_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	NULL,
	NULL,
	mb_qprint_to_wchar,
	mb_wchar_to_qprint,
	NULL
};

const struct mbfl_convert_vtbl vtbl_8bit_qprint = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_qprint,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_qprintenc,
	mbfl_filt_conv_qprintenc_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_qprint_8bit = {
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_qprintdec,
	mbfl_filt_conv_qprintdec_flush,
	NULL,
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * any => Quoted-Printable
 */

int mbfl_filt_conv_qprintenc(int c, mbfl_convert_filter *filter)
{
	int s, n;

	switch (filter->status & 0xff) {
	case 0:
		filter->cache = c;
		filter->status++;
		break;
	default:
		s = filter->cache;
		filter->cache = c;
		n = (filter->status & 0xff00) >> 8;

		if (s == 0) {		/* null */
			CK((*filter->output_function)(s, filter->data));
			filter->status &= ~0xff00;
			break;
		}

		if (s == '\n' || (s == '\r' && c != '\n')) {	/* line feed */
			CK((*filter->output_function)('\r', filter->data));
			CK((*filter->output_function)('\n', filter->data));
			filter->status &= ~0xff00;
			break;
		} else if (s == 0x0d) {
			break;
		}

		if (n >= 72) {	/* soft line feed */
			CK((*filter->output_function)('=', filter->data));
			CK((*filter->output_function)('\r', filter->data));
			CK((*filter->output_function)('\n', filter->data));
			filter->status &= ~0xff00;
		}

		if (s <= 0 || s >= 0x80 || s == '=') { /* not ASCII or '=' */
			/* hex-octet */
			CK((*filter->output_function)('=', filter->data));
			n = (s >> 4) & 0xf;
			if (n < 10) {
				n += 48;		/* '0' */
			} else {
				n += 55;		/* 'A' - 10 */
			}
			CK((*filter->output_function)(n, filter->data));
			n = s & 0xf;
			if (n < 10) {
				n += 48;
			} else {
				n += 55;
			}
			CK((*filter->output_function)(n, filter->data));
			filter->status += 0x300;
		} else {
			CK((*filter->output_function)(s, filter->data));
			filter->status += 0x100;
		}
		break;
	}

	return 0;
}

int mbfl_filt_conv_qprintenc_flush(mbfl_convert_filter *filter)
{
	/* flush filter cache */
	(*filter->filter_function)('\0', filter);
	filter->status &= ~0xffff;
	filter->cache = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int hex2code_map[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/*
 * Quoted-Printable => any
 */
int mbfl_filt_conv_qprintdec(int c, mbfl_convert_filter *filter)
{
	int n, m;

	switch (filter->status) {
	case 1:
		if (hex2code_map[c & 0xff] >= 0) {
			filter->cache = c;
			filter->status = 2;
		} else if (c == 0x0d) {	/* soft line feed */
			filter->status = 3;
		} else if (c == 0x0a) {	/* soft line feed */
			filter->status = 0;
		} else {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		}
		break;
	case 2:
		m = hex2code_map[c & 0xff];
		if (m < 0) {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(filter->cache, filter->data));
			n = c;
		} else {
			n = hex2code_map[filter->cache] << 4 | m;
		}
		CK((*filter->output_function)(n, filter->data));
		filter->status = 0;
		break;
	case 3:
		if (c != 0x0a) {		/* LF */
			CK((*filter->output_function)(c, filter->data));
		}
		filter->status = 0;
		break;
	default:
		if (c == 0x3d) {		/* '=' */
			filter->status = 1;
		} else {
			CK((*filter->output_function)(c, filter->data));
		}
		break;
	}

	return 0;
}

int mbfl_filt_conv_qprintdec_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	if (status == 1) {
		CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
	} else if (status == 2) {
		CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		CK((*filter->output_function)(cache, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static size_t mb_qprint_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize - 2;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == '=' && p < e) {
			unsigned char c2 = *p++;

			if (hex2code_map[c2] >= 0 && p < e) {
				unsigned char c3 = *p++;

				if (hex2code_map[c3] >= 0) {
					*out++ = hex2code_map[c2] << 4 | hex2code_map[c3];
				} else {
					*out++ = '=';
					*out++ = c2;
					*out++ = c3;
				}
			} else if (c2 == '\r' && p < e) {
				unsigned char c3 = *p++;

				if (c3 != '\n') {
					*out++ = c3;
				}
			} else if (c2 != '\n') {
				*out++ = '=';
				*out++ = c2;
			}
		} else {
			*out++ = c;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static unsigned char qprint_enc_nibble(unsigned char nibble)
{
	if (nibble < 10) {
		return nibble + '0';
	} else {
		return nibble - 10 + 'A';
	}
}

static void mb_wchar_to_qprint(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	unsigned int chars_output = buf->state;

	while (len--) {
		/* We assume that all the input 'codepoints' are not really Unicode codepoints at all,
		 * but raw bytes from 0x00-0xFF */
		uint32_t w = *in++;

		if (!w) {
			out = mb_convert_buf_add(out, '\0');
			chars_output = 0;
			continue;
		} else if (w == '\n') {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			out = mb_convert_buf_add2(out, '\r', '\n');
			chars_output = 0;
			continue;
		} else if (w == '\r') {
			/* No output */
			continue;
		}

		/* QPrint actually mandates that line length should not be more than 76 characters,
		 * but mbstring stops slightly short of that */
		if (chars_output >= 72) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			out = mb_convert_buf_add3(out, '=', '\r', '\n');
			chars_output = 0;
		}

		if (w >= 0x80 || w == '=') {
			/* Not ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 3);
			out = mb_convert_buf_add3(out, '=', qprint_enc_nibble((w >> 4) & 0xF), qprint_enc_nibble(w & 0xF));
			chars_output += 3;
		} else {
			/* Plain ASCII */
			out = mb_convert_buf_add(out, w);
			chars_output++;
		}
	}

	buf->state = chars_output;
	MB_CONVERT_BUF_STORE(buf, out, limit);
}
