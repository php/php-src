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
#include "mbfilter_iso8859_1.h"

static const char *mbfl_encoding_8859_1_aliases[] = {"ISO_8859-1", "latin1", NULL};

const mbfl_encoding mbfl_encoding_8859_1 = {
	mbfl_no_encoding_8859_1,
	"ISO-8859-1",
	"ISO-8859-1",
	(const char *(*)[])&mbfl_encoding_8859_1_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

const struct mbfl_identify_vtbl vtbl_identify_8859_1 = {
	mbfl_no_encoding_8859_1,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true
};

const struct mbfl_convert_vtbl vtbl_8859_1_wchar = {
	mbfl_no_encoding_8859_1,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_1_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_8859_1 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_1,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_1,
	mbfl_filt_conv_common_flush
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * ISO-8859-1 => wchar
 */
int mbfl_filt_conv_8859_1_wchar(int c, mbfl_convert_filter *filter)
{
	return (*filter->output_function)(c, filter->data);
}

/*
 * wchar => ISO-8859-1
 */
int mbfl_filt_conv_wchar_8859_1(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x100) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}


