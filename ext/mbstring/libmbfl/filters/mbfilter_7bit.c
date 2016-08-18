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
#include "mbfilter_7bit.h"

const mbfl_encoding mbfl_encoding_7bit = {
	mbfl_no_encoding_7bit,
	"7bit",
	"7bit",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

const struct mbfl_convert_vtbl vtbl_8bit_7bit = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_7bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_any_7bit,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_7bit_8bit = {
	mbfl_no_encoding_7bit,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_7bit_any,
	mbfl_filt_conv_common_flush
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_7bit_any(int c, mbfl_convert_filter *filter)
{
	return (*filter->output_function)(c, filter->data);
}


int mbfl_filt_conv_any_7bit(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x80) {
		CK((*filter->output_function)(c, filter->data));
	}
	return c;
}


