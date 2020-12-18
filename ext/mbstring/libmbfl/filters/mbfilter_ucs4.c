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

static const char *mbfl_encoding_ucs4_aliases[] = {"ISO-10646-UCS-4", "UCS4", NULL};

/* This library historically had encodings called 'byte4be' and 'byte4le'
 * which were almost identical to UCS-4
 * Maintain minimal support by aliasing to UCS-2 */
static const char *mbfl_encoding_ucs4be_aliases[] = {"byte4be", NULL};
static const char *mbfl_encoding_ucs4le_aliases[] = {"byte4le", NULL};

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
	mbfl_encoding_ucs4be_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4be_wchar,
	&vtbl_wchar_ucs4be
};

const mbfl_encoding mbfl_encoding_ucs4le = {
	mbfl_no_encoding_ucs4le,
	"UCS-4LE",
	"UCS-4LE",
	mbfl_encoding_ucs4le_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4,
	&vtbl_ucs4le_wchar,
	&vtbl_wchar_ucs4le
};

const struct mbfl_convert_vtbl vtbl_ucs4_wchar = {
	mbfl_no_encoding_ucs4,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_ucs4_wchar,
	mbfl_filt_conv_common_flush,
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
	mbfl_filt_conv_common_flush,
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
	mbfl_filt_conv_common_flush,
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
		if ((n & 0xffff) == 0 && ((n >> 16) & 0xffff) == 0xfffe) {
			if (endian) {
				filter->status = 0;		/* big-endian */
			} else {
				filter->status = 0x100;		/* little-endian */
			}
			CK((*filter->output_function)(0xfeff, filter->data));
		} else {
			filter->status &= ~0xff;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
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
	return c;
}

/*
 * wchar => UCS-4BE
 */
int mbfl_filt_conv_wchar_ucs4be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
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
	return c;
}

/*
 * wchar => UCS-4LE
 */
int mbfl_filt_conv_wchar_ucs4le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}
