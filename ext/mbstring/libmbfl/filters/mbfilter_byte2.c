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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 4 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_byte2.h"

const mbfl_encoding mbfl_encoding_byte2be = {
	mbfl_no_encoding_byte2be,
	"byte2be",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_byte2be_wchar,
	&vtbl_wchar_byte2be
};

const mbfl_encoding mbfl_encoding_byte2le = {
	mbfl_no_encoding_byte2le,
	"byte2le",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_byte2le_wchar,
	&vtbl_wchar_byte2le
};

const struct mbfl_convert_vtbl vtbl_byte2be_wchar = {
	mbfl_no_encoding_byte2be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte2be_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_byte2be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte2be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_byte2be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_byte2le_wchar = {
	mbfl_no_encoding_byte2le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte2le_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_byte2le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte2le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_byte2le,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_byte2be_wchar(int c, mbfl_convert_filter *filter)
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

int mbfl_filt_conv_wchar_byte2be(int c, mbfl_convert_filter *filter)
{
	CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	CK((*filter->output_function)(c & 0xff, filter->data));
	return c;
}

int mbfl_filt_conv_byte2le_wchar(int c, mbfl_convert_filter *filter)
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

int mbfl_filt_conv_wchar_byte2le(int c, mbfl_convert_filter *filter)
{
	CK((*filter->output_function)(c & 0xff, filter->data));
	CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	return c;
}
