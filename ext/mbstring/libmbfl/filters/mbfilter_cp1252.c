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
 * The author of this part: Wez Furlong <wez@thebrainroom.com>
 *
 */
/*
 * the source code included in this files was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_cp1252.h"
#include "unicode_table_cp1252.h"

static int mbfl_filt_ident_cp1252(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_cp1252_aliases[] = {"cp1252", NULL};

const mbfl_encoding mbfl_encoding_cp1252 = {
	mbfl_no_encoding_cp1252,
	"Windows-1252",
	"Windows-1252",
	(const char *(*)[])&mbfl_encoding_cp1252_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_cp1252_wchar,
	&vtbl_wchar_cp1252
};

const struct mbfl_identify_vtbl vtbl_identify_cp1252 = {
	mbfl_no_encoding_cp1252,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp1252
};

const struct mbfl_convert_vtbl vtbl_cp1252_wchar = {
	mbfl_no_encoding_cp1252,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_cp1252_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp1252 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp1252,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_cp1252,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * wchar => cp1252
 */
int mbfl_filt_conv_wchar_cp1252(int c, mbfl_convert_filter *filter)
{
	int s=-1, n;

	if (c >= 0x100)	{
		/* look it up from the cp1252 table */
		s = -1;
		n = 31;
		while (n >= 0) {
			if (c == cp1252_ucs_table[n] && c != 0xfffe) {
				s = 0x80 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_1)
		{
			s = c & MBFL_WCSPLANE_MASK;
		}
	}
	else if (c >= 0 && c < 0x100) {
		s = c;
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
 * cp1252 => wchar
 */
int mbfl_filt_conv_cp1252_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0x80 && c < 0xa0) {
		s = cp1252_ucs_table[c - 0x80];
	} else {
		s = c;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/* We only distinguish the MS extensions to ISO-8859-1.
 * Actually, this is pretty much a NO-OP, since the identification
 * system doesn't allow us to discriminate between a positive match,
 * a possible match and a definite non-match.
 * The problem here is that cp1252 looks like SJIS for certain chars.
 * */
static int mbfl_filt_ident_cp1252(int c, mbfl_identify_filter *filter)
{
	if (c >= 0x80 && c < 0xa0)
		filter->flag = 0;
	else
		filter->flag = 1; /* not it */
	return c;
}
