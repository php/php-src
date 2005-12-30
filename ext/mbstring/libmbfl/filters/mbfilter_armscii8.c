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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_armscii8.h"
#include "unicode_table_armscii8.h"

static int mbfl_filt_ident_armscii8(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_armscii8_aliases[] = {"ArmSCII-8", "ArmSCII8", "ARMSCII-8", "ARMSCII8", NULL};

const mbfl_encoding mbfl_encoding_armscii8 = {
	mbfl_no_encoding_armscii8,
	"ArmSCII-8",
	"ArmSCII-8",
	(const char *(*)[])&mbfl_encoding_armscii8_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

const struct mbfl_identify_vtbl vtbl_identify_armscii8 = {
	mbfl_no_encoding_armscii8,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_armscii8
};

const struct mbfl_convert_vtbl vtbl_wchar_armscii8 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_armscii8,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_armscii8,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_armscii8_wchar = {
	mbfl_no_encoding_armscii8,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_armscii8_wchar,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * armscii8 => wchar
 */
int mbfl_filt_conv_armscii8_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < armscii8_ucs_table_min) {
		s = c;
	} else if (c >= armscii8_ucs_table_min && c < 0x100) {
		s = armscii8_ucs_table[c - armscii8_ucs_table_min];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_ARMSCII8;
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
 * wchar => armscii8
 */
int mbfl_filt_conv_wchar_armscii8(int c, mbfl_convert_filter *filter)
{

	int s, n;

	if (c >= 0x28 && c < 0x30) {
		s = ucs_armscii8_table[c-0x28];
	} else if (c < armscii8_ucs_table_min) {
		s = c;
	} else {
		s = -1;
		n = armscii8_ucs_table_len-1;
		while (n >= 0) {
			if (c == armscii8_ucs_table[n]) {
				s = armscii8_ucs_table_min + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_ARMSCII8) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

static int mbfl_filt_ident_armscii8(int c, mbfl_identify_filter *filter)
{
	if (c >= armscii8_ucs_table_min && c <= 0xff)
		filter->flag = 0;
	else
		filter->flag = 1; /* not it */
	return c;
}
