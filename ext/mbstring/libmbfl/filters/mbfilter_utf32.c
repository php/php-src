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

static void mbfl_filt_ident_utf32(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf32le(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf32be(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_conv_utf32_wchar_flush(mbfl_convert_filter *filter);

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

const struct mbfl_identify_vtbl vtbl_identify_utf32 = {
	mbfl_no_encoding_utf32,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_utf32
};

const struct mbfl_identify_vtbl vtbl_identify_utf32be = {
	mbfl_no_encoding_utf32be,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_utf32be
};

const struct mbfl_identify_vtbl vtbl_identify_utf32le = {
	mbfl_no_encoding_utf32le,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_utf32le
};

const struct mbfl_convert_vtbl vtbl_utf32_wchar = {
	mbfl_no_encoding_utf32,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf32_wchar,
	mbfl_filt_conv_utf32_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf32 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf32be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_utf32be_wchar = {
	mbfl_no_encoding_utf32be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf32be_wchar,
	mbfl_filt_conv_utf32_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf32be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf32be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_utf32le_wchar = {
	mbfl_no_encoding_utf32le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf32le_wchar,
	mbfl_filt_conv_utf32_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf32le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf32le,
	mbfl_filt_conv_common_flush
};

static void emit_char_if_valid(int n, mbfl_convert_filter *filter)
{
	if (n < MBFL_WCSPLANE_UTF32MAX && (n < 0xD800 || n > 0xDFFF)) {
		(*filter->output_function)(n, filter->data);
	} else {
		n = (n & MBFL_WCSGROUP_MASK) | MBFL_WCSGROUP_THROUGH;
		(*filter->output_function)(n, filter->data);
	}
}

void mbfl_filt_conv_utf32_wchar(int c, mbfl_convert_filter *filter)
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
				emit_char_if_valid(n, filter);
			}
		}
	}
}

void mbfl_filt_conv_utf32be_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status < 3) {
		filter->cache = (filter->cache << 8) | (c & 0xFF);
		filter->status++;
	} else {
		int n = (filter->cache << 8) | (c & 0xFF);
		filter->cache = filter->status = 0;
		emit_char_if_valid(n, filter);
	}
}

void mbfl_filt_conv_wchar_utf32be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UTF32MAX) {
		(*filter->output_function)((c >> 24) & 0xff, filter->data);
		(*filter->output_function)((c >> 16) & 0xff, filter->data);
		(*filter->output_function)((c >> 8) & 0xff, filter->data);
		(*filter->output_function)(c & 0xff, filter->data);
	} else {
		mbfl_filt_conv_illegal_output(c, filter);
	}
}

void mbfl_filt_conv_utf32le_wchar(int c, mbfl_convert_filter *filter)
{
	if (filter->status < 3) {
		filter->cache |= ((c & 0xFFU) << (8 * filter->status));
		filter->status++;
	} else {
		int n = ((c & 0xFFU) << 24) | filter->cache;
		filter->cache = filter->status = 0;
		emit_char_if_valid(n, filter);
	}
}

void mbfl_filt_conv_wchar_utf32le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UTF32MAX) {
		(*filter->output_function)(c & 0xff, filter->data);
		(*filter->output_function)((c >> 8) & 0xff, filter->data);
		(*filter->output_function)((c >> 16) & 0xff, filter->data);
		(*filter->output_function)((c >> 24) & 0xff, filter->data);
	} else {
		mbfl_filt_conv_illegal_output(c, filter);
	}
}

static void mbfl_filt_conv_utf32_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		(*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}
}

static void mbfl_filt_ident_utf32(unsigned char c, mbfl_identify_filter *filter)
{
	/* The largest valid codepoint is 0x10FFFF; we don't want values above that
	 * Neither do we want to see surrogates
	 * For UTF-32 (not LE or BE), we do also need to look for a byte-order mark */
	switch (filter->status) {
	case 0: /* 1st byte */
		if (c == 0xff) {
			filter->status = 1;
			return;
		}
		filter->filter_function = mbfl_filt_ident_utf32be;
		break;

	case 1: /* 2nd byte */
		if (c == 0xfe) {
			filter->status = 2;
			return;
		}
		filter->filter_function = mbfl_filt_ident_utf32be;
		(filter->filter_function)(0xff, filter);
		break;

	case 2: /* 3rd byte */
		if (c == 0) {
			filter->status = 3;
			return;
		}
		filter->filter_function = mbfl_filt_ident_utf32be;
		(filter->filter_function)(0xff, filter);
		(filter->filter_function)(0xfe, filter);
		break;

	case 3: /* 4th byte */
		if (c == 0) {
			/* We found a little-endian byte-order mark! */
			filter->status = 0;
			filter->filter_function = mbfl_filt_ident_utf32le;
			return;
		}
		filter->filter_function = mbfl_filt_ident_utf32be;
		(filter->filter_function)(0xff, filter);
		(filter->filter_function)(0xfe, filter);
		(filter->filter_function)(0, filter);
		break;
	}

	(filter->filter_function)(c, filter);
}

static void mbfl_filt_ident_utf32le(unsigned char c, mbfl_identify_filter *filter)
{
	switch (filter->status) {
	case 0: /* 1st byte */
		filter->status = 1;
		break;

	case 1: /* 2nd byte */
		if (c >= 0xD8 && c <= 0xDF) {
			filter->status = 4; /* might be surrogate if we are in BMP */
		} else {
			filter->status = 2;
		}
		break;

	case 2: /* 3rd byte */
		if (c > 0x10) {
			filter->flag = 1; /* too big */
		}
		filter->status = 3;
		break;

	case 3: /* 4th byte */
		if (c) {
			filter->flag = 1; /* too big */
		}
		filter->status = 0;
		break;

	case 4: /* 3rd byte, previous byte looked like surrogate */
		if (!c) {
			filter->flag = 1; /* yep, it's a surrogate */
		}
		filter->status = 3;
	}
}

static void mbfl_filt_ident_utf32be(unsigned char c, mbfl_identify_filter *filter)
{
	switch (filter->status) {
	case 0: /* 1st byte */
		if (c) {
			filter->flag = 1; /* too big */
		}
		filter->status = 1;
		break;

	case 1: /* 2nd byte */
		if (c > 0x10) {
			filter->flag = 1; /* too big */
		} if (c) {
			filter->status = 4; /* not in the BMP */
		} else {
			filter->status = 2;
		}
		break;

	case 2: /* 3rd byte */
		if (c >= 0xD8 && c <= 0xDF) {
			filter->flag = 1; /* reserved range for surrogates */
		}
		filter->status = 3;
		break;

	case 3: /* 4th byte */
		filter->status = 0;
		break;

	case 4: /* 3rd byte, not in BMP */
		filter->status = 3;
	}
}
