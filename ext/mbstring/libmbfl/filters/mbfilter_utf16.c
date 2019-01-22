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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_utf16.h"

/* Macros to create char length table */
#define B2(n) n,n
#define B4(n) B2(n),B2(n)
#define B8(n) B4(n),B4(n)
#define B16(n) B8(n),B8(n)
#define B32(n) B16(n),B16(n)
#define B64(n) B32(n),B32(n)
#define B128(n) B64(n),B64(n)
#define B256(n) B128(n),B128(n)
#define B512(n) B256(n),B256(n)
#define B1024(n) B512(n),B512(n)
#define B2048(n) B1024(n),B1024(n)
#define B4096(n) B2048(n),B2048(n)
#define B8192(n) B4096(n),B4096(n)
#define B16384(n) B8192(n),B8192(n)

/* UTF-16 character length table */
const char unsigned mblen_table_utf16_le[65536] = {
	B16384(2),
	B16384(2),
	B16384(2),
	B4096(2),
	B2048(2),
	B1024(4), /* surrogate pairs: 0xD800-0xDFFF. High surrogate first: 0xD800, last: 0xDBFF */
	B1024(2), /* Low surrogate first: 0xDC00, last: 0xDFFF */
	B8192(2),
};

/* macro to make swapped length table */
#define BY B128(2),B64(2),B16(2),B8(2),B4(4),B4(2),B32(2)

/* swapped bytes table */
const char unsigned mblen_table_utf16_be[65536] = {
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,

	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,

	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,

	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
	BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,BY,
};

static const char *mbfl_encoding_utf16_aliases[] = {"utf16", NULL};

const mbfl_encoding mbfl_encoding_utf16 = {
	mbfl_no_encoding_utf16,
	"UTF-16",
	"UTF-16",
	(const char *(*)[])&mbfl_encoding_utf16_aliases,
	mblen_table_utf16_be,
	MBFL_ENCTYPE_MWC2BE,
	&vtbl_utf16_wchar,
	&vtbl_wchar_utf16
};

const mbfl_encoding mbfl_encoding_utf16be = {
	mbfl_no_encoding_utf16be,
	"UTF-16BE",
	"UTF-16BE",
	NULL,
	mblen_table_utf16_be,
	MBFL_ENCTYPE_MWC2BE,
	&vtbl_utf16be_wchar,
	&vtbl_wchar_utf16be
};

const mbfl_encoding mbfl_encoding_utf16le = {
	mbfl_no_encoding_utf16le,
	"UTF-16LE",
	"UTF-16LE",
	NULL,
	mblen_table_utf16_le,
	MBFL_ENCTYPE_MWC2LE,
	&vtbl_utf16le_wchar,
	&vtbl_wchar_utf16le
};

const struct mbfl_convert_vtbl vtbl_utf16_wchar = {
	mbfl_no_encoding_utf16,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf16_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_utf16be_wchar = {
	mbfl_no_encoding_utf16be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf16be_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_utf16le_wchar = {
	mbfl_no_encoding_utf16le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf16le_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf16le,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * UTF-16 => wchar
 */
int mbfl_filt_conv_utf16_wchar(int c, mbfl_convert_filter *filter)
{
	int n, endian;

	endian = filter->status & 0xff00;
	switch (filter->status & 0x0f) {
	case 0:
		if (endian) {
			n = c & 0xff;
		} else {
			n = (c & 0xff) << 8;
		}
		filter->cache |= n;
		filter->status++;
		break;
	default:
		if (endian) {
			n = (c & 0xff) << 8;
		} else {
			n = c & 0xff;
		}
		n |= filter->cache & 0xffff;
		filter->status &= ~0x0f;
		if (n >= 0xd800 && n < 0xdc00) {
			filter->cache = ((n & 0x3ff) << 16) + 0x400000;
		} else if (n >= 0xdc00 && n < 0xe000) {
			n &= 0x3ff;
			n |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (n >= MBFL_WCSPLANE_SUPMIN && n < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(n, filter->data));
			} else {		/* illegal character */
				n &= MBFL_WCSGROUP_MASK;
				n |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(n, filter->data));
			}
		} else {
			int is_first = filter->status & 0x10;
			filter->cache = 0;
			filter->status |= 0x10;
			if (!is_first) {
				if (n == 0xfffe) {
					if (endian) {
						filter->status &= ~0x100;		/* big-endian */
					} else {
						filter->status |= 0x100;		/* little-endian */
					}
					break;
				} else if (n == 0xfeff) {
					break;
				}
			}
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * UTF-16BE => wchar
 */
int mbfl_filt_conv_utf16be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0:
		filter->status = 1;
		n = (c & 0xff) << 8;
		filter->cache |= n;
		break;
	default:
		filter->status = 0;
		n = (filter->cache & 0xff00) | (c & 0xff);
		if (n >= 0xd800 && n < 0xdc00) {
			filter->cache = ((n & 0x3ff) << 16) + 0x400000;
		} else if (n >= 0xdc00 && n < 0xe000) {
			n &= 0x3ff;
			n |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (n >= MBFL_WCSPLANE_SUPMIN && n < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(n, filter->data));
			} else {		/* illegal character */
				n &= MBFL_WCSGROUP_MASK;
				n |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(n, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * wchar => UTF-16BE
 */
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * UTF-16LE => wchar
 */
int mbfl_filt_conv_utf16le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0:
		filter->status = 1;
		n = c & 0xff;
		filter->cache |= n;
		break;
	default:
		filter->status = 0;
		n = (filter->cache & 0xff) | ((c & 0xff) << 8);
		if (n >= 0xd800 && n < 0xdc00) {
			filter->cache = ((n & 0x3ff) << 16) + 0x400000;
		} else if (n >= 0xdc00 && n < 0xe000) {
			n &= 0x3ff;
			n |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (n >= MBFL_WCSPLANE_SUPMIN && n < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(n, filter->data));
			} else {		/* illegal character */
				n &= MBFL_WCSGROUP_MASK;
				n |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(n, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * wchar => UTF-16LE
 */
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}
