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
 * The author of this file: Hayk Chamyan <hamshen@gmail.com>
 *
 */

/*
 * "armenian code filter and converter"
 */

#include "mbfilter.h"
#include "mbfilter_armscii8.h"
#include "unicode_table_armscii8.h"

static int mbfl_filt_ident_armscii8(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_armscii8_aliases[] = {"ArmSCII-8", "ArmSCII8", "ARMSCII-8", "ARMSCII8", NULL};

const mbfl_encoding mbfl_encoding_armscii8 = {
	mbfl_no_encoding_armscii8,
	"ArmSCII-8",
	"ArmSCII-8",
	mbfl_encoding_armscii8_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_armscii8_wchar,
	&vtbl_wchar_armscii8
};

const struct mbfl_identify_vtbl vtbl_identify_armscii8 = {
	mbfl_no_encoding_armscii8,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_armscii8
};

const struct mbfl_convert_vtbl vtbl_wchar_armscii8 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_armscii8,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_armscii8,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_armscii8_wchar = {
	mbfl_no_encoding_armscii8,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_armscii8_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_armscii8_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c < armscii8_ucs_table_min) {
		s = c;
	} else {
		s = armscii8_ucs_table[c - armscii8_ucs_table_min];
		if (!s) {
			s = c | MBFL_WCSGROUP_THROUGH;
		}
	}

	CK((*filter->output_function)(s, filter->data));
	return c;
}

int mbfl_filt_conv_wchar_armscii8(int c, mbfl_convert_filter *filter)
{
	if (c >= 0x28 && c <= 0x2F) {
		CK((*filter->output_function)(ucs_armscii8_table[c - 0x28], filter->data));
	} else if (c < armscii8_ucs_table_min) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		for (int n = 0; n < armscii8_ucs_table_len; n++) {
			if (c == armscii8_ucs_table[n]) {
				CK((*filter->output_function)(armscii8_ucs_table_min + n, filter->data));
				return c;
			}
		}
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

static int mbfl_filt_ident_armscii8(int c, mbfl_identify_filter *filter)
{
	if (c >= armscii8_ucs_table_min && !armscii8_ucs_table[c - armscii8_ucs_table_min]) {
		filter->flag = 1;
	}
	return c;
}
