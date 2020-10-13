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

static const char *mbfl_encoding_utf16_aliases[] = {"utf16", NULL};

const mbfl_encoding mbfl_encoding_utf16 = {
	mbfl_no_encoding_utf16,
	"UTF-16",
	"UTF-16",
	mbfl_encoding_utf16_aliases,
	NULL,
	MBFL_ENCTYPE_MWC2,
	&vtbl_utf16_wchar,
	&vtbl_wchar_utf16
};

const mbfl_encoding mbfl_encoding_utf16be = {
	mbfl_no_encoding_utf16be,
	"UTF-16BE",
	"UTF-16BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_MWC2,
	&vtbl_utf16be_wchar,
	&vtbl_wchar_utf16be
};

const mbfl_encoding mbfl_encoding_utf16le = {
	mbfl_no_encoding_utf16le,
	"UTF-16LE",
	"UTF-16LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_MWC2,
	&vtbl_utf16le_wchar,
	&vtbl_wchar_utf16le
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
		if (n == 0xFFFE) {
			/* Switch to little-endian mode */
			filter->filter_function = mbfl_filt_conv_utf16le_wchar;
			filter->cache = filter->status = 0;
		} else {
			filter->filter_function = mbfl_filt_conv_utf16be_wchar;
			if (n >= 0xD800 && n <= 0xDBFF) {
				filter->cache = n & 0x3FF; /* Pick out 10 data bits */
				filter->status = 2;
				return c;
			} else if (n >= 0xDC00 && n <= 0xDFFF) {
				/* This is wrong; second part of surrogate pair has come first */
				CK((*filter->output_function)(n | MBFL_WCSGROUP_THROUGH, filter->data));
			} else if (n != 0xFEFF) {
				CK((*filter->output_function)(n, filter->data));
			}
			filter->cache = filter->status = 0;
		}
	}

	return c;
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
			CK((*filter->output_function)(n | MBFL_WCSGROUP_THROUGH, filter->data));
			filter->status = 0;
		} else {
			CK((*filter->output_function)(n, filter->data));
			filter->status = 0;
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
			CK((*filter->output_function)((0xD8 << 10) | (filter->cache >> 8) | MBFL_WCSGROUP_THROUGH, filter->data));
			filter->cache = n & 0x3FF;
			filter->status = 2;
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			n = ((filter->cache & 0x3FF00) << 2) + (n & 0x3FF) + 0x10000;
			CK((*filter->output_function)(n, filter->data));
			filter->status = 0;
		} else {
			CK((*filter->output_function)((0xD8 << 10) | (filter->cache >> 8) | MBFL_WCSGROUP_THROUGH, filter->data));
			CK((*filter->output_function)(n, filter->data));
			filter->status = 0;
		}
	}

	return c;
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

	return c;
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
			/* This is wrong; the second part of the surrogate pair has come first
			 * Flag it with `MBFL_WCSGROUP_THROUGH`; the following filter will handle
			 * the error */
			n = (filter->cache + ((c & 0xff) << 8)) | MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(n, filter->data));
			filter->status = 0;
		} else {
			CK((*filter->output_function)(filter->cache + ((c & 0xff) << 8), filter->data));
			filter->status = 0;
		}
		break;

	case 2:
		filter->cache = (filter->cache << 10) + (c & 0xff);
		filter->status = 3;
		break;

	case 3:
		n = (filter->cache & 0xFF) | ((c & 0xFF) << 8);
		if (n >= 0xD800 && n <= 0xDBFF) {
			CK((*filter->output_function)((0xD8 << 10) | (filter->cache >> 10) | MBFL_WCSGROUP_THROUGH, filter->data));
			filter->cache = n & 0x3FF;
			filter->status = 2;
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			n = filter->cache + ((c & 0x3) << 8) + 0x10000;
			CK((*filter->output_function)(n, filter->data));
			filter->status = 0;
		} else {
			CK((*filter->output_function)((0xD8 << 10) | (filter->cache >> 10) | MBFL_WCSGROUP_THROUGH, filter->data));
			CK((*filter->output_function)(n, filter->data));
			filter->status = 0;
		}
		break;
	}

	return c;
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

	return c;
}

static int mbfl_filt_conv_utf16_wchar_flush(mbfl_convert_filter *filter)
{
	int status = filter->status;
	int cache = filter->cache;
	filter->status = filter->cache = 0;

	if (status) {
		/* Input string was truncated */
		CK((*filter->output_function)(cache | MBFL_WCSGROUP_THROUGH, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}
