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
#include "mbfilter_ucs4.h"

static int mbfl_filt_conv_ucs4_wchar_flush(mbfl_convert_filter *filter);
static int mbfl_filt_ident_ucs4(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_ucs4be(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_ucs4le(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_ucs4_aliases[] = {"ISO-10646-UCS-4", "UCS4", NULL};

const mbfl_encoding mbfl_encoding_ucs4 = {
	mbfl_no_encoding_ucs4,
	"UCS-4",
	"UCS-4",
	mbfl_encoding_ucs4_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4_wchar,
	&vtbl_wchar_ucs4
};

const mbfl_encoding mbfl_encoding_ucs4be = {
	mbfl_no_encoding_ucs4be,
	"UCS-4BE",
	"UCS-4BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4be_wchar,
	&vtbl_wchar_ucs4be
};

const mbfl_encoding mbfl_encoding_ucs4le = {
	mbfl_no_encoding_ucs4le,
	"UCS-4LE",
	"UCS-4LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4le_wchar,
	&vtbl_wchar_ucs4le
};

const struct mbfl_identify_vtbl vtbl_identify_ucs4 = {
	mbfl_no_encoding_ucs4,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_ucs4
};

const struct mbfl_identify_vtbl vtbl_identify_ucs4be = {
	mbfl_no_encoding_ucs4be,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_ucs4be
};

const struct mbfl_identify_vtbl vtbl_identify_ucs4le = {
	mbfl_no_encoding_ucs4le,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_ucs4le
};

const struct mbfl_convert_vtbl vtbl_ucs4_wchar = {
	mbfl_no_encoding_ucs4,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4_wchar,
	mbfl_filt_conv_ucs4_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs4 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_ucs4be_wchar = {
	mbfl_no_encoding_ucs4be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4be_wchar,
	mbfl_filt_conv_ucs4_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs4be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_ucs4le_wchar = {
	mbfl_no_encoding_ucs4le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4le_wchar,
	mbfl_filt_conv_ucs4_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs4le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_ucs4le,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

static int emit_wchar_if_valid(unsigned int w, mbfl_convert_filter *filter)
{
	/* FIXME: UCS-4 should actually accept code unit values up to 0x7FFFFFFF
	 * This is a hack to get around the fact that values of 0x70000000 and up
	 * are being used to represent illegal characters, which are passed through
	 * for error handling by a later conversion filter stage
	 * After we adjust the range of values used for such, change this to 0x7FFFFFFF */
	if (w < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)(w, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(w, filter));
	}
	return 0;
}

int mbfl_filt_conv_ucs4_wchar(int c, mbfl_convert_filter *filter)
{
	/* We start out with the assumption that the string is big-endian
	 * If we find a little-endian BOM, then we will change that assumption */
	filter->cache = (filter->cache << 8) | (c & 0xFF);

	if (filter->status < 3) {
		filter->status++;
	} else {
		if (filter->cache == 0xFFFE0000) {
			/* If we had the endianness right, the byte-order mark should be 0x0000feff,
			 * but we got 0xfffe0000, meaning that we are currently using the wrong
			 * endianness and should switch to little-endian mode */
			filter->filter_function = mbfl_filt_conv_ucs4le_wchar;
		} else {
			if (filter->cache != 0xFEFF) {
				CK(emit_wchar_if_valid(filter->cache, filter));
			}
			filter->filter_function = mbfl_filt_conv_ucs4be_wchar;
		}
		filter->status = filter->cache = 0;
	}

	return c;
}

int mbfl_filt_conv_ucs4be_wchar(int c, mbfl_convert_filter *filter)
{
	filter->cache = (filter->cache << 8) | (c & 0xFF);
	if (filter->status < 3) {
		filter->status++;
	} else {
		CK(emit_wchar_if_valid(filter->cache, filter));
		filter->status = filter->cache = 0;
	}
	return c;
}

int mbfl_filt_conv_wchar_ucs4be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)((c >> 24) & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xFF, filter->data));
		CK((*filter->output_function)(c & 0xFF, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

int mbfl_filt_conv_ucs4le_wchar(int c, mbfl_convert_filter *filter)
{
	filter->cache = (filter->cache >> 8) | ((c & 0xFF) << 24);
	if (filter->status < 3) {
		filter->status++;
	} else {
		CK(emit_wchar_if_valid(filter->cache, filter));
		filter->status = filter->cache = 0;
	}
	return c;
}

int mbfl_filt_conv_wchar_ucs4le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)(c & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 24) & 0xFF, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

static int mbfl_filt_conv_ucs4_wchar_flush(mbfl_convert_filter *filter)
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

static int mbfl_filt_ident_ucs4be(int c, mbfl_identify_filter *filter)
{
	int n = filter->status & 0xFF; /* # of bytes already consumed in code unit */
	if (n < 3) {
		filter->status = (filter->status + 1) | (c << (8 * (3 - n)));
	} else {
		int value = (filter->status & 0xFFFFFF00) | c;
		if (value >= MBFL_WCSGROUP_UCS4MAX) {
			filter->flag = 1;
		}
		filter->status = 0;
	}
	return c;
}

static int mbfl_filt_ident_ucs4le(int c, mbfl_identify_filter *filter)
{
	int n = filter->status & 0xFF; /* # of bytes already consumed in code unit */
	if (n < 3) {
		filter->status = (filter->status + 1) | (c << (8 * (1 + n)));
	} else {
		int value = (filter->status >> 8) | (c << 24);
		if (value >= MBFL_WCSGROUP_UCS4MAX) {
			filter->flag = 1;
		}
		filter->status = 0;
	}
	return c;
}

static int mbfl_filt_ident_ucs4(int c, mbfl_identify_filter *filter)
{
	int n = filter->status & 0xFF; /* # of bytes already consumed in code unit */
	if (n < 3) {
		filter->status = (filter->status + 1) | (c << (8 * (3 - n)));
	} else {
		int value = (filter->status & 0xFFFFFF00) | c;
		if (value == 0xFFFE0000) {
			filter->filter_function = mbfl_filt_ident_ucs4le;
		} else {
			filter->filter_function = mbfl_filt_ident_ucs4be;
			if (value >= MBFL_WCSGROUP_UCS4MAX) {
				filter->flag = 1;
			}
		}
		filter->status = 0;
	}
	return c;
}
