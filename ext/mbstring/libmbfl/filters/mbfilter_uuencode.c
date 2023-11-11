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
#include "mbfilter_uuencode.h"

static size_t mb_uuencode_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_uuencode(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

const mbfl_encoding mbfl_encoding_uuencode = {
	mbfl_no_encoding_uuencode,
	"UUENCODE",
	"x-uuencode",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS,
	NULL,
	NULL,
	mb_uuencode_to_wchar,
	mb_wchar_to_uuencode,
	NULL
};

const struct mbfl_convert_vtbl vtbl_uuencode_8bit = {
	mbfl_no_encoding_uuencode,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_uudec,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define UUDEC(c)	(char)(((c)-' ') & 077)
static const char *uuenc_begin_text = "begin ";
enum {
	uudec_state_ground=0,
	uudec_state_inbegin,
	uudec_state_until_newline,
	uudec_state_size,
	uudec_state_a,
	uudec_state_b,
	uudec_state_c,
	uudec_state_d,
	uudec_state_skip_newline
};

int mbfl_filt_conv_uudec(int c, mbfl_convert_filter *filter)
{
	int n;

	switch(filter->status)	{
		case uudec_state_ground:
			/* looking for "begin 0666 filename\n" line */
			if (filter->cache == 0 && c == 'b')
			{
				filter->status = uudec_state_inbegin;
				filter->cache = 1; /* move to 'e' */
			}
			else if (c == '\n')
				filter->cache = 0;
			else
				filter->cache++;
			break;
		case uudec_state_inbegin:
			if (uuenc_begin_text[filter->cache++] != c)	{
				/* doesn't match pattern */
				filter->status = uudec_state_ground;
				break;
			}
			if (filter->cache == 5)
			{
				/* that's good enough - wait for a newline */
				filter->status = uudec_state_until_newline;
				filter->cache = 0;
			}
			break;
		case uudec_state_until_newline:
			if (c == '\n')
				filter->status = uudec_state_size;
			break;
		case uudec_state_size:
			/* get "size" byte */
			n = UUDEC(c);
			filter->cache = n << 24;
			filter->status = uudec_state_a;
			break;
		case uudec_state_a:
			/* get "a" byte */
			n = UUDEC(c);
			filter->cache |= (n << 16);
			filter->status = uudec_state_b;
			break;
		case uudec_state_b:
			/* get "b" byte */
			n = UUDEC(c);
			filter->cache |= (n << 8);
			filter->status = uudec_state_c;
			break;
		case uudec_state_c:
			/* get "c" byte */
			n = UUDEC(c);
			filter->cache |= n;
			filter->status = uudec_state_d;
			break;
		case uudec_state_d:
			/* get "d" byte */
			{
				int A, B, C, D = UUDEC(c);
				A = (filter->cache >> 16) & 0xff;
				B = (filter->cache >> 8) & 0xff;
				C = (filter->cache) & 0xff;
				n = (filter->cache >> 24) & 0xff;
				if (n-- > 0)
					CK((*filter->output_function)( (A << 2) | (B >> 4), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (B << 4) | (C >> 2), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (C << 6) | D, filter->data));
				if (n < 0)
					n = 0;
				filter->cache = n << 24;

				if (n == 0)
					filter->status = uudec_state_skip_newline;	/* skip next byte (newline) */
				else
					filter->status = uudec_state_a; /* go back to fetch "A" byte */
			}
			break;
		case uudec_state_skip_newline:
			/* skip newline */
			filter->status = uudec_state_size;
	}
	return 0;
}

/* Using mbstring to decode UUEncoded text is already deprecated
 * However, to facilitate the move to the new, faster internal conversion interface,
 * We will temporarily implement it for UUEncode */

static size_t mb_uuencode_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	unsigned int _state = *state & 0xFF;
	unsigned int size = *state >> 8;

	while (p < e && (limit - out) >= 3) {
		unsigned char c = *p++;

		switch (_state) {
		case uudec_state_ground:
			if (c == 'b') {
				if ((e - p) >= 5 && memcmp(p, uuenc_begin_text+1, 5) == 0) {
					p += 5;
					while (p < e && *p++ != '\n'); /* Consume everything up to newline */
					_state = uudec_state_size;
				}
				/* We didn't find "begin " */
			}
			break;

		case uudec_state_size:
			size = UUDEC(c);
			_state = uudec_state_a;
			break;

		case uudec_state_a:
			if ((e - p) < 4) {
				p = e;
				break;
			}

			unsigned int a = UUDEC(c);
			unsigned int b = UUDEC(*p++);
			unsigned int c = UUDEC(*p++);
			unsigned int d = UUDEC(*p++);

			if (size > 0) {
				*out++ = ((a << 2) | (b >> 4)) & 0xFF;
				size--;
			}
			if (size > 0) {
				*out++ = ((b << 4) | (c >> 2)) & 0xFF;
				size--;
			}
			if (size > 0) {
				*out++ = ((c << 6) | d) & 0xFF;
				size--;
			}

			_state = size ? uudec_state_a : uudec_state_skip_newline;
			break;

		case uudec_state_skip_newline:
			_state = uudec_state_size;
			break;
		}
	}

	*state = (size << 8) | _state;
	*in_len = e - p;
	*in = p;
	return out - buf;
}

static unsigned char uuencode_six_bits(unsigned int bits)
{
	if (bits == 0) {
		return '`';
	} else {
		return bits + 32;
	}
}

static void mb_wchar_to_uuencode(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	/* Every 3 bytes of input gets encoded as 4 bytes of output
	 * Additionally, we have a 'length' byte and a newline for each line of output
	 * (Maximum 45 input bytes can be encoded on a single output line)
	 * Make space for two more bytes in case we start close to where a line must end,
	 * and another two if there are cached bits remaining from the previous call */
	MB_CONVERT_BUF_ENSURE(buf, out, limit, ((len + 2) * 4 / 3) + (((len + 44) / 45) * 2) + (buf->state ? 0 : sizeof("begin 0644 filename\n")) + 4);

	unsigned int bytes_encoded = (buf->state >> 1) & 0x7F;
	/* UUEncode naturally wants to process input bytes in groups of 3, but
	 * our buffer size may not be a multiple of 3
	 * So there may be data from the previous call which we need to flush out */
	unsigned int n_cached_bits = (buf->state >> 8) & 0xFF;
	unsigned int cached_bits = buf->state >> 16;

	if (!buf->state) {
		for (char *s = "begin 0644 filename\n"; *s; s++) {
			out = mb_convert_buf_add(out, *s);
		}
		out = mb_convert_buf_add(out, MIN(len, 45) + 32);
		buf->state |= 1;
	} else if (!len && end && !bytes_encoded && !n_cached_bits) {
		/* Corner case: under EXTREMELY rare circumstances, it's possible that the
		 * final call to this conversion function will happen with an empty input
		 * buffer, leaving an unwanted trailing len byte in the output buffer. */
		buf->out--;
		return;
	} else {
		/* UUEncode starts each line with a byte which indicates how many bytes
		 * are encoded on the line
		 * This can create a problem, since we receive the incoming data one buffer
		 * at a time, and there is no requirement that the buffers should be aligned
		 * with line boundaries
		 * So if a previous line was cut off, we need to go back and fix up
		 * the preceding len byte */
		unsigned char *len_byte = out - (bytes_encoded * 4 / 3) - 1;
		if (n_cached_bits) {
			len_byte -= (n_cached_bits == 2) ? 1 : 2;
		}
		*len_byte = MIN(bytes_encoded + len + (n_cached_bits ? (n_cached_bits == 2 ? 1 : 2) : 0), 45) + 32;

		if (n_cached_bits) {
			/* Flush out bits which remained from previous call */
			if (n_cached_bits == 2) {
				uint32_t w = cached_bits;
				uint32_t w2 = 0, w3 = 0;
				if (len) {
					w2 = *in++;
					len--;
				}
				if (len) {
					w3 = *in++;
					len--;
				}
				out = mb_convert_buf_add3(out, uuencode_six_bits((w << 4) + ((w2 >> 4) & 0xF)), uuencode_six_bits(((w2 & 0xF) << 2) + ((w3 >> 6) & 0x3)), uuencode_six_bits(w3 & 0x3F));
			} else {
				uint32_t w2 = cached_bits;
				uint32_t w3 = 0;
				if (len) {
					w3 = *in++;
					len--;
				}
				out = mb_convert_buf_add2(out, uuencode_six_bits((w2 << 2) + ((w3 >> 6) & 0x3)), uuencode_six_bits(w3 & 0x3F));
			}
			n_cached_bits = cached_bits = 0;
			goto possible_line_break;
		}
	}

	while (len--) {
		uint32_t w = *in++;
		uint32_t w2 = 0, w3 = 0;

		if (!len) {
			if (!end) {
				out = mb_convert_buf_add(out, uuencode_six_bits((w >> 2) & 0x3F));
				/* Cache 2 remaining bits from 'w' */
				cached_bits = w & 0x3;
				n_cached_bits = 2;
				break;
			}
		} else {
			w2 = *in++;
			len--;
		}

		if (!len) {
			if (!end) {
				out = mb_convert_buf_add2(out, uuencode_six_bits((w >> 2) & 0x3F), uuencode_six_bits(((w & 0x3) << 4) + ((w2 >> 4) & 0xF)));
				/* Cache 4 remaining bits from 'w2' */
				cached_bits = w2 & 0xF;
				n_cached_bits = 4;
				break;
			}
		} else {
			w3 = *in++;
			len--;
		}

		out = mb_convert_buf_add4(out, uuencode_six_bits((w >> 2) & 0x3F), uuencode_six_bits(((w & 0x3) << 4) + ((w2 >> 4) & 0xF)), uuencode_six_bits(((w2 & 0xF) << 2) + ((w3 >> 6) & 0x3)), uuencode_six_bits(w3 & 0x3F));

possible_line_break:
		bytes_encoded += 3;

		if (bytes_encoded >= 45) {
			out = mb_convert_buf_add(out, '\n');
			if (len || !end) {
				out = mb_convert_buf_add(out, MIN(len, 45) + 32);
			}
			bytes_encoded = 0;
		}
	}

	if (bytes_encoded && end) {
		out = mb_convert_buf_add(out, '\n');
	}

	buf->state = ((cached_bits & 0xFF) << 16) | ((n_cached_bits & 0xFF) << 8) | ((bytes_encoded & 0x7F) << 1) | (buf->state & 1);
	MB_CONVERT_BUF_STORE(buf, out, limit);
}
