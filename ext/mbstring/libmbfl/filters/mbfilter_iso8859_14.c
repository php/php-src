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
#include "mbfilter_iso8859_14.h"
#include "unicode_table_iso8859_14.h"

static const char *mbfl_encoding_8859_14_aliases[] = {"ISO8859-14", "latin8", NULL};

const mbfl_encoding mbfl_encoding_8859_14 = {
	mbfl_no_encoding_8859_14,
	"ISO-8859-14",
	"ISO-8859-14",
	(const char *(*)[])&mbfl_encoding_8859_14_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_8859_14_wchar,
	&vtbl_wchar_8859_14
};

const struct mbfl_identify_vtbl vtbl_identify_8859_14 = {
	mbfl_no_encoding_8859_14,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_true
};

const struct mbfl_convert_vtbl vtbl_8859_14_wchar = {
	mbfl_no_encoding_8859_14,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_8859_14_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_8859_14 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_14,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_8859_14,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * ISO-8859-14 => wchar
 */
int mbfl_filt_conv_8859_14_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_14_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_14;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-14
 */
int mbfl_filt_conv_wchar_8859_14(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_14_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_14) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}
