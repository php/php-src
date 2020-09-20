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
 * by moriyoshi koizumi <moriyoshi@php.net> on 20 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_utf32.h"

static int mbfl_filt_conv_utf32_wchar_flush(mbfl_convert_filter *filter);

static const char *mbfl_encoding_utf32_aliases[] = {"utf32", NULL};

const mbfl_encoding mbfl_encoding_utf32 = {
	mbfl_no_encoding_utf32,
	"UTF-32",
	"UTF-32",
	mbfl_encoding_utf32_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_utf32_wchar,
	&vtbl_wchar_utf32
};

const mbfl_encoding mbfl_encoding_utf32be = {
	mbfl_no_encoding_utf32be,
	"UTF-32BE",
	"UTF-32BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_utf32be_wchar,
	&vtbl_wchar_utf32be
};

const mbfl_encoding mbfl_encoding_utf32le = {
	mbfl_no_encoding_utf32le,
	"UTF-32LE",
	"UTF-32LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_utf32le_wchar,
	&vtbl_wchar_utf32le
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
	if (n < MBFL_WCSPLANE_UTF32MAX && (n < 0xD800 || n > 0xDFFF)) {
		CK((*filter->output_function)(n, filter->data));
	} else {
		n = (n & MBFL_WCSGROUP_MASK) | MBFL_WCSGROUP_THROUGH;
		CK((*filter->output_function)(n, filter->data));
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

	return c;
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
	return c;
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

	return c;
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
	return c;
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

	return c;
}

static int mbfl_filt_conv_utf32_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		CK((*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}
