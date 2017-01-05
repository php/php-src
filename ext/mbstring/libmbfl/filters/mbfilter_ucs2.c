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
#include "mbfilter_ucs2.h"

static const char *mbfl_encoding_ucs2_aliases[] = {"ISO-10646-UCS-2", "UCS2" , "UNICODE", NULL};

const mbfl_encoding mbfl_encoding_ucs2 = {
	mbfl_no_encoding_ucs2,
	"UCS-2",
	"UCS-2",
	(const char *(*)[])&mbfl_encoding_ucs2_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2BE
};

const mbfl_encoding mbfl_encoding_ucs2be = {
	mbfl_no_encoding_ucs2be,
	"UCS-2BE",
	"UCS-2BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS2BE
};

const mbfl_encoding mbfl_encoding_ucs2le = {
	mbfl_no_encoding_ucs2le,
	"UCS-2LE",
	"UCS-2LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS2LE
};

const struct mbfl_convert_vtbl vtbl_ucs2_wchar = {
	mbfl_no_encoding_ucs2,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_ucs2_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs2 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs2be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_ucs2be_wchar = {
	mbfl_no_encoding_ucs2be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_ucs2be_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs2be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs2be,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_ucs2le_wchar = {
	mbfl_no_encoding_ucs2le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_ucs2le_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_ucs2le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs2le,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * UCS-2 => wchar
 */
int mbfl_filt_conv_ucs2_wchar(int c, mbfl_convert_filter *filter)
{
	int n, endian;

	endian = filter->status & 0xff00;
	switch (filter->status & 0xff) {
	case 0:
		if (endian) {
			n = c & 0xff;
		} else {
			n = (c & 0xff) << 8;
		}
		filter->cache = n;
		filter->status++;
		break;
	default:
		if (endian) {
			n = (c & 0xff) << 8;
		} else {
			n = c & 0xff;
		}
		n |= filter->cache;
		if (n == 0xfffe) {
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
 * UCS-2BE => wchar
 */
int mbfl_filt_conv_ucs2be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = (c & 0xff) << 8;
		filter->cache = n;
	} else {
		filter->status = 0;
		n = (c & 0xff) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return c;
}

/*
 * wchar => UCS-2BE
 */
int mbfl_filt_conv_wchar_ucs2be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * UCS-2LE => wchar
 */
int mbfl_filt_conv_ucs2le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = c & 0xff;
		filter->cache = n;
	} else {
		filter->status = 0;
		n = ((c & 0xff) << 8) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return c;
}


/*
 * wchar => UCS-2LE
 */
int mbfl_filt_conv_wchar_ucs2le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}



