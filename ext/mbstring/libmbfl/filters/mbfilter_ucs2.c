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
	&vtbl_wchar_ucs2
};

const mbfl_encoding mbfl_encoding_ucs2be = {
	mbfl_no_encoding_ucs2be,
	"UCS-2BE",
	"UCS-2BE",
	mbfl_encoding_ucs2be_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2,
	&vtbl_ucs2be_wchar,
	&vtbl_wchar_ucs2be
};

const mbfl_encoding mbfl_encoding_ucs2le = {
	mbfl_no_encoding_ucs2le,
	"UCS-2LE",
	"UCS-2LE",
	mbfl_encoding_ucs2le_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2,
	&vtbl_ucs2le_wchar,
	&vtbl_wchar_ucs2le
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
	return c;
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
	return c;
}

int mbfl_filt_conv_wchar_ucs2be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)((c >> 8) & 0xFF, filter->data));
		CK((*filter->output_function)(c & 0xFF, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return c;
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
	return c;
}

int mbfl_filt_conv_wchar_ucs2le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)(c & 0xFF, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xFF, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return c;
}

static int mbfl_filt_conv_ucs2_wchar_flush(mbfl_convert_filter *filter)
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
