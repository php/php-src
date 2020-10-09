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
 * The source code included in this files was separated from mbfilter_cn.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_hz.h"

#include "unicode_table_cp936.h"

static int mbfl_filt_ident_hz(int c, mbfl_identify_filter *filter);

const mbfl_encoding mbfl_encoding_hz = {
	mbfl_no_encoding_hz,
	"HZ",
	"HZ-GB-2312",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_hz_wchar,
	&vtbl_wchar_hz
};

const struct mbfl_identify_vtbl vtbl_identify_hz = {
	mbfl_no_encoding_hz,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_hz
};

const struct mbfl_convert_vtbl vtbl_hz_wchar = {
	mbfl_no_encoding_hz,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_hz_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_hz = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_hz,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_hz,
	mbfl_filt_conv_any_hz_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * HZ => wchar
 */
int
mbfl_filt_conv_hz_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 GB2312 */
	case 0:
		if (c == 0x7e) {
			filter->status += 2;
		} else if (filter->status == 0x10 && c > 0x20 && c < 0x7f) {	/* DBCS first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

/*	case 0x11:	 GB2312 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c1 > 0x20 && c1 < 0x7f && c > 0x20 && c < 0x7f) {
			s = (c1 - 1)*192 + c + 0x40; /* GB2312 */
			if (s >= 0 && s < cp936_ucs_table_size) {
				w = cp936_ucs_table[s];
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = (c1 << 8) | c;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_GB2312;
			}
			CK((*filter->output_function)(w, filter->data));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	/* '~' */
	case 2:
		if (c == 0x7d) {		/* '}' */
			filter->status = 0x0;
		} else if (c == 0x7b) {		/* '{' */
			filter->status = 0x10;
		} else if (c == 0x7e) { /* '~' */
			filter->status = 0x0;
			CK((*filter->output_function)(0x007e, filter->data));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => HZ
 */
int
mbfl_filt_conv_wchar_hz(int c, mbfl_convert_filter *filter)
{
	int s;

	s = 0;
	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		if (c == 0xff04) {
			s = 0xa1e7;
		} else if (c == 0xff5e) {
			s = 0xa1ab;
		} else if (c >= 0xff01 && c <= 0xff5d) {
			s = c - 0xff01 + 0xa3a1;
		} else if (c >= 0xffe0 && c <= 0xffe5) {
			s = ucs_hff_s_cp936_table[c-0xffe0];
		}
	}
	if (s & 0x8000) {
		s -= 0x8080;
	}

	if (s <= 0) {
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
		s = -1;
	}
	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x7e, filter->data));		/* '~' */
				CK((*filter->output_function)(0x7d, filter->data));		/* '}' */
			}
			filter->status = 0;
			if (s == 0x7e){
				CK((*filter->output_function)(0x7e, filter->data));
			}
			CK((*filter->output_function)(s, filter->data));
		} else { /* GB 2312-80 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x7e, filter->data));		/* '~' */
				CK((*filter->output_function)(0x7b, filter->data));		/* '{' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

int
mbfl_filt_conv_any_hz_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x7e, filter->data));		/* ~ */
		CK((*filter->output_function)(0x7d, filter->data));		/* '{' */
	}
	filter->status &= 0xff;
	return 0;
}

static int mbfl_filt_ident_hz(int c, mbfl_identify_filter *filter)
{
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 GB2312 */
	case 0:
		if (c == 0x7e) {
			filter->status += 2;
		} else if (filter->status == 0x10 && c > 0x20 && c < 0x7f) {		/* DBCS first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x11:	 GB2312 second char */
	case 1:
		filter->status &= ~0xf;
		if (c < 0x21 || c > 0x7e) {		/* bad */
			filter->flag = 1;
		}
		break;

	case 2:
		if (c == 0x7d) {		/* '}' */
			filter->status = 0;
		} else if (c == 0x7b) {		/* '{' */
			filter->status = 0x10;
		} else if (c == 0x7e) {		/* '~' */
			filter->status = 0;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}
