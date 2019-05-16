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
 * The author of this part: Haluk AKIN <halukakin@gmail.com>
 *
 */
/*
 * The source code included in this files was separated from mbfilter_ru.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_cp1254.h"
#include "unicode_table_cp1254.h"

static int mbfl_filt_ident_cp1254(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_cp1254_aliases[] = {"CP1254", "CP-1254", "WINDOWS-1254", NULL};

const mbfl_encoding mbfl_encoding_cp1254 = {
	mbfl_no_encoding_cp1254,
	"Windows-1254",
	"Windows-1254",
	(const char *(*)[])&mbfl_encoding_cp1254_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_cp1254_wchar,
	&vtbl_wchar_cp1254
};

const struct mbfl_identify_vtbl vtbl_identify_cp1254 = {
	mbfl_no_encoding_cp1254,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp1254
};

const struct mbfl_convert_vtbl vtbl_cp1254_wchar = {
	mbfl_no_encoding_cp1254,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_cp1254_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp1254 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp1254,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_cp1254,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * wchar => cp1254
 */
int
mbfl_filt_conv_wchar_cp1254(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c < 0x80) {
		s = c;
	} else {
		s = -1;
		n = cp1254_ucs_table_len-1;
		while (n >= 0) {
			if (c == cp1254_ucs_table[n] && c != 0xfffe) {
				s = cp1254_ucs_table_min + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_CP1254) {
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

/*
 * cp1254 => wchar
 */
int
mbfl_filt_conv_cp1254_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < cp1254_ucs_table_min) {
		s = c;
	} else if (c >= cp1254_ucs_table_min && c < 0x100) {
		s = cp1254_ucs_table[c - cp1254_ucs_table_min];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_CP1254;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/* We only distinguish the MS extensions to ISO-8859-1.
 * Actually, this is pretty much a NO-OP, since the identification
 * system doesn't allow us to discriminate between a positive match,
 * a possible match and a definite non-match.
 * The problem here is that cp1254 looks like SJIS for certain chars.
 * */
static int mbfl_filt_ident_cp1254(int c, mbfl_identify_filter *filter)
{
	if (c >= 0x80 && c < 0xff)
		filter->flag = 0;
	else
		filter->flag = 1; /* not it */
	return c;
}
