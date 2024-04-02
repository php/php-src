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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#include <stddef.h>

#include "mbfl_encoding.h"
#include "mbfl_filter_output.h"
#include "mbfilter_pass.h"
#include "mbfilter_8bit.h"
#include "mbfilter_wchar.h"

#include "filters/mbfilter_base64.h"
#include "filters/mbfilter_cjk.h"
#include "filters/mbfilter_qprint.h"
#include "filters/mbfilter_uuencode.h"
#include "filters/mbfilter_7bit.h"
#include "filters/mbfilter_utf7.h"
#include "filters/mbfilter_utf7imap.h"
#include "filters/mbfilter_utf8.h"
#include "filters/mbfilter_utf16.h"
#include "filters/mbfilter_utf32.h"
#include "filters/mbfilter_ucs4.h"
#include "filters/mbfilter_ucs2.h"
#include "filters/mbfilter_htmlent.h"
#include "filters/mbfilter_singlebyte.h"

/* hex character table "0123456789ABCDEF" */
static char mbfl_hexchar_table[] = {
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46
};

static const struct mbfl_convert_vtbl *mbfl_special_filter_list[] = {
	&vtbl_8bit_b64,
	&vtbl_b64_8bit,
	&vtbl_uuencode_8bit,
	&vtbl_8bit_qprint,
	&vtbl_qprint_8bit,
	&vtbl_pass,
	NULL
};

static void mbfl_convert_filter_init(mbfl_convert_filter *filter, const mbfl_encoding *from, const mbfl_encoding *to,
	const struct mbfl_convert_vtbl *vtbl, output_function_t output_function, flush_function_t flush_function, void* data)
{
	/* encoding structure */
	filter->from = from;
	filter->to = to;

	if (output_function != NULL) {
		filter->output_function = output_function;
	} else {
		filter->output_function = mbfl_filter_output_null;
	}

	filter->flush_function = flush_function;
	filter->data = data;
	filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	filter->illegal_substchar = '?';
	filter->num_illegalchar = 0;
	filter->filter_dtor = vtbl->filter_dtor;
	filter->filter_function = vtbl->filter_function;
	filter->filter_flush = (filter_flush_t)vtbl->filter_flush;
	filter->filter_copy = vtbl->filter_copy;

	(*vtbl->filter_ctor)(filter);
}

mbfl_convert_filter* mbfl_convert_filter_new(const mbfl_encoding *from, const mbfl_encoding *to, output_function_t output_function,
	flush_function_t flush_function, void* data)
{
	const struct mbfl_convert_vtbl *vtbl = mbfl_convert_filter_get_vtbl(from, to);
	if (vtbl == NULL) {
		return NULL;
	}

	mbfl_convert_filter *filter = emalloc(sizeof(mbfl_convert_filter));
	mbfl_convert_filter_init(filter, from, to, vtbl, output_function, flush_function, data);
	return filter;
}

mbfl_convert_filter* mbfl_convert_filter_new2(const struct mbfl_convert_vtbl *vtbl, output_function_t output_function,
	flush_function_t flush_function, void* data)
{
	const mbfl_encoding *from_encoding = mbfl_no2encoding(vtbl->from);
	const mbfl_encoding *to_encoding = mbfl_no2encoding(vtbl->to);

	mbfl_convert_filter *filter = emalloc(sizeof(mbfl_convert_filter));
	mbfl_convert_filter_init(filter, from_encoding, to_encoding, vtbl, output_function, flush_function, data);
	return filter;
}

void mbfl_convert_filter_delete(mbfl_convert_filter *filter)
{
	if (filter->filter_dtor) {
		(*filter->filter_dtor)(filter);
	}
	efree(filter);
}

/* Feed a char, return 0 if ok - used by mailparse ext */
int mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter)
{
	return (*filter->filter_function)(c, filter);
}

/* Feed string into `filter` byte by byte; return pointer to first byte not processed */
unsigned char* mbfl_convert_filter_feed_string(mbfl_convert_filter *filter, unsigned char *p, size_t len)
{
	while (len--) {
		if ((*filter->filter_function)(*p++, filter) < 0) {
			break;
		}
	}
	return p;
}

int mbfl_convert_filter_flush(mbfl_convert_filter *filter)
{
	(*filter->filter_flush)(filter);
	return 0;
}

void mbfl_convert_filter_reset(mbfl_convert_filter *filter, const mbfl_encoding *from, const mbfl_encoding *to)
{
	if (filter->filter_dtor) {
		(*filter->filter_dtor)(filter);
	}

	const struct mbfl_convert_vtbl *vtbl = mbfl_convert_filter_get_vtbl(from, to);

	if (vtbl == NULL) {
		vtbl = &vtbl_pass;
	}

	mbfl_convert_filter_init(filter, from, to, vtbl, filter->output_function, filter->flush_function, filter->data);
}

void mbfl_convert_filter_copy(mbfl_convert_filter *src, mbfl_convert_filter *dest)
{
	if (src->filter_copy != NULL) {
		src->filter_copy(src, dest);
		return;
	}

	*dest = *src;
}

void mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src)
{
	mbfl_convert_filter_feed_string(filter, src->buffer, src->pos);
}

int mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p)
{
	int c;
	while ((c = *p++)) {
		if ((*filter->filter_function)(c, filter) < 0) {
			return -1;
		}
	}

	return 0;
}

static int mbfl_filt_conv_output_hex(unsigned int w, mbfl_convert_filter *filter)
{
	bool nonzero = false;
	int shift = 28, ret = 0;

	while (shift >= 0) {
		int n = (w >> shift) & 0xF;
		if (n || nonzero) {
			nonzero = true;
			ret = (*filter->filter_function)(mbfl_hexchar_table[n], filter);
			if (ret < 0) {
				return ret;
			}
		}
		shift -= 4;
	}

	if (!nonzero) {
		/* No hex digits were output by above loop */
		ret = (*filter->filter_function)('0', filter);
	}

	return ret;
}

/* illegal character output function for conv-filter */
int mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter)
{
	unsigned int w = c;
	int ret = 0;
	int mode_backup = filter->illegal_mode;
	uint32_t substchar_backup = filter->illegal_substchar;

	/* The used substitution character may not be supported by the target character encoding.
	 * If that happens, first try to use "?" instead and if that also fails, silently drop the
	 * character. */
	if (filter->illegal_mode == MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR
			&& filter->illegal_substchar != '?') {
		filter->illegal_substchar = '?';
	} else {
		filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
	}

	switch (mode_backup) {
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR:
		ret = (*filter->filter_function)(substchar_backup, filter);
		break;

	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG:
		if (w != MBFL_BAD_INPUT) {
			ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"U+");
			if (ret < 0)
				break;
			ret = mbfl_filt_conv_output_hex(w, filter);
		} else {
			ret = (*filter->filter_function)(substchar_backup, filter);
		}
		break;

	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY:
		if (w != MBFL_BAD_INPUT) {
			ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"&#x");
			if (ret < 0)
				break;
			ret = mbfl_filt_conv_output_hex(w, filter);
			if (ret < 0)
				break;
			ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)";");
		} else {
			ret = (*filter->filter_function)(substchar_backup, filter);
		}
		break;

	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE:
	default:
		break;
	}

	filter->illegal_mode = mode_backup;
	filter->illegal_substchar = substchar_backup;
	filter->num_illegalchar++;

	return ret;
}

const struct mbfl_convert_vtbl* mbfl_convert_filter_get_vtbl(const mbfl_encoding *from, const mbfl_encoding *to)
{
	if (to->no_encoding == mbfl_no_encoding_base64 ||
	    to->no_encoding == mbfl_no_encoding_qprint) {
		from = &mbfl_encoding_8bit;
	} else if (from->no_encoding == mbfl_no_encoding_base64 ||
			   from->no_encoding == mbfl_no_encoding_qprint ||
			   from->no_encoding == mbfl_no_encoding_uuencode) {
		to = &mbfl_encoding_8bit;
	}

	if (to == from && (to == &mbfl_encoding_wchar || to == &mbfl_encoding_8bit)) {
		return &vtbl_pass;
	}

	if (to->no_encoding == mbfl_no_encoding_wchar) {
		return from->input_filter;
	} else if (from->no_encoding == mbfl_no_encoding_wchar) {
		return to->output_filter;
	} else {
		int i = 0;
		const struct mbfl_convert_vtbl *vtbl;
		while ((vtbl = mbfl_special_filter_list[i++])) {
			if (vtbl->from == from->no_encoding && vtbl->to == to->no_encoding) {
				return vtbl;
			}
		}
		return NULL;
	}
}

/*
 * commonly used constructor
 */
void mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter)
{
	filter->status = filter->cache = 0;
}

int mbfl_filt_conv_common_flush(mbfl_convert_filter *filter)
{
	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}
	return 0;
}

zend_string* mb_fast_convert(unsigned char *in, size_t in_len, const mbfl_encoding *from, const mbfl_encoding *to, uint32_t replacement_char, unsigned int error_mode, unsigned int *num_errors)
{
	uint32_t wchar_buf[128];
	unsigned int state = 0;

	if (to == &mbfl_encoding_base64 || to == &mbfl_encoding_qprint) {
		from = &mbfl_encoding_8bit;
	} else if (from == &mbfl_encoding_base64 || from == &mbfl_encoding_qprint || from == &mbfl_encoding_uuencode) {
		to = &mbfl_encoding_8bit;
	}

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, in_len, replacement_char, error_mode);

	while (in_len) {
		size_t out_len = from->to_wchar(&in, &in_len, wchar_buf, 128, &state);
		ZEND_ASSERT(out_len <= 128);
		to->from_wchar(wchar_buf, out_len, &buf, !in_len);
	}

	*num_errors = buf.errors;
	return mb_convert_buf_result(&buf, to);
}

static uint32_t* convert_cp_to_hex(uint32_t cp, uint32_t *out)
{
	bool nonzero = false;
	int shift = 28;

	while (shift >= 0) {
		int n = (cp >> shift) & 0xF;
		if (n || nonzero) {
			nonzero = true;
			*out++ = mbfl_hexchar_table[n];
		}
		shift -= 4;
	}

	if (!nonzero) {
		/* No hex digits were output by above loop */
		*out++ = '0';
	}

	return out;
}

static size_t mb_illegal_marker(uint32_t bad_cp, uint32_t *out, unsigned int err_mode, uint32_t replacement_char)
{
	uint32_t *start = out;

	if (bad_cp == MBFL_BAD_INPUT) {
		/* Input string contained a byte sequence which was invalid in the 'from' encoding
		 * Unless the error handling mode is set to NONE, insert the replacement character */
		if (err_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			*out++ = replacement_char;
		}
	} else {
		/* Input string contained a byte sequence which was valid in the 'from' encoding,
		 * but decoded to a Unicode codepoint which cannot be represented in the 'to' encoding */
		switch (err_mode) {
		case MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR:
			*out++ = replacement_char;
			break;

		case MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG:
			out[0] = 'U';
			out[1] = '+';
			out = convert_cp_to_hex(bad_cp, &out[2]);
			break;

		case MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY:
			out[0] = '&'; out[1] = '#'; out[2] = 'x';
			out = convert_cp_to_hex(bad_cp, &out[3]);
			*out++ = ';';
			break;
		}
	}

	return out - start;
}

void mb_illegal_output(uint32_t bad_cp, mb_from_wchar_fn fn, mb_convert_buf* buf)
{
	buf->errors++;

	uint32_t temp[12];
	uint32_t repl_char = buf->replacement_char;
	unsigned int err_mode = buf->error_mode;

	if (err_mode == MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8) {
		/* This mode is for internal use only, when converting a string to
		 * UTF-8 before searching it; it uses a byte which is illegal in
		 * UTF-8 as an error marker. This ensures that error markers will
		 * never 'accidentally' match valid text, as could happen when a
		 * character like '?' is used as an error marker. */
		MB_CONVERT_BUF_ENSURE(buf, buf->out, buf->limit, 1);
		buf->out = mb_convert_buf_add(buf->out, 0xFF);
		return;
	}

	size_t len = mb_illegal_marker(bad_cp, temp, err_mode, repl_char);

	/* Avoid infinite loop if `fn` is not able to handle `repl_char` */
	if (err_mode == MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR && repl_char != '?') {
		buf->replacement_char = '?';
	} else {
		buf->error_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
	}

	fn(temp, len, buf, false);

	buf->replacement_char = repl_char;
	buf->error_mode = err_mode;
}
