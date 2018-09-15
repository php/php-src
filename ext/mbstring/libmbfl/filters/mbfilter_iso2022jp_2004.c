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
 * The source code included in this files was separated from mbfilter_jis.c
 * by rui hirokawa <hirokawa@php.net> on 18 aug 2011.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_iso2022jp_2004.h"
#include "mbfilter_sjis_2004.h"

#include "unicode_table_jis2004.h"
#include "unicode_table_jis.h"

extern int mbfl_filt_conv_any_jis_flush(mbfl_convert_filter *filter);
static int mbfl_filt_ident_2022jp_2004(int c, mbfl_identify_filter *filter);

const mbfl_encoding mbfl_encoding_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	"ISO-2022-JP-2004",
	"ISO-2022-JP-2004",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_2004_wchar,
	&vtbl_wchar_2022jp_2004
};

const struct mbfl_identify_vtbl vtbl_identify_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_2022jp_2004
};

const struct mbfl_convert_vtbl vtbl_2022jp_2004_wchar = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jp_2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_jis2004_flush
};

static int mbfl_filt_ident_2022jp_2004(int c, mbfl_identify_filter *filter)
{
retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x80:	 X 0212 */
/*	case 0x90:	 X 0213 plane 1 */
/*	case 0xa0:	 X 0213 plane 2 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (filter->status == 0x80 && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
	case 1:
		if (c == 0x1b) {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			if (c < 0x21 || c > 0x7e) {		/* bad */
				filter->flag = 1;
			}
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x42) {		/* 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {		/* '(' */
			filter->status++;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == 0x51) {		/* JIS X 0213 plane 1 */
			filter->status = 0x90;
		} else if (c == 0x50) {		/* JIS X 0213 plane 2 */
			filter->status = 0xa0;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42) {		/* 'B' */
			filter->status = 0;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}
