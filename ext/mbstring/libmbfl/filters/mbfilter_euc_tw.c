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
 * The author of this file: Rui Hirokawa <hirokawa@php.net>
 *
 */
/*
 * The source code included in this files was separated from mbfilter_tw.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_euc_tw.h"

#include "unicode_table_cns11643.h"

static int mbfl_filt_ident_euctw(int c, mbfl_identify_filter *filter);

static const unsigned char mblen_table_euctw[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};


static const char *mbfl_encoding_euc_tw_aliases[] = {"EUC_TW", "eucTW", "x-euc-tw", NULL};

const mbfl_encoding mbfl_encoding_euc_tw = {
	mbfl_no_encoding_euc_tw,
	"EUC-TW",
	"EUC-TW",
	(const char *(*)[])&mbfl_encoding_euc_tw_aliases,
	mblen_table_euctw,
	MBFL_ENCTYPE_MBCS
};

const struct mbfl_identify_vtbl vtbl_identify_euctw = {
	mbfl_no_encoding_euc_tw,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_euctw
};

const struct mbfl_convert_vtbl vtbl_euctw_wchar = {
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_euctw_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_euctw = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_tw,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_euctw,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * EUC-TW => wchar
 */
int
mbfl_filt_conv_euctw_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w, plane;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xff) {	/* dbcs first byte */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* mbcs first byte */
			filter->status = 2;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 1:		/* mbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			w = (c1 - 0xa1)*94 + (c - 0xa1);
			if (w >= 0 && w < cns11643_1_ucs_table_size) {
				w = cns11643_1_ucs_table[w];
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = (c1 << 8) | c;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_CNS11643;
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

	case 2:	/* got 0x8e,  first char */
		c1 = filter->cache;
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else if (c > 0xa0 && c < 0xaf) {
			filter->status = 3;
			filter->cache = c - 0xa1;
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 3:	/* got 0x8e,  third char */
		filter->status = 0;
		c1 = filter->cache;
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else if (c > 0xa0 && c < 0xff) {
			filter->status = 4;
			filter->cache = (c1 << 8) + c - 0xa1;
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 4:	/* mbcs fourth char */
		filter->status = 0;
		c1 = filter->cache;
		if (c1 >= 0x100 && c1 <= 0xdff && c > 0xa0 && c < 0xff) {
			plane = (c1 & 0xf00) >> 8; 
			s = (c1 & 0xff)*94 + c - 0xa1;
			w = 0;
			if (s >= 0) {
				if (plane == 1 && s < cns11643_2_ucs_table_size) {
					w = cns11643_2_ucs_table[s];
				}
				if (plane == 13 && s < cns11643_14_ucs_table_size) {
					w = cns11643_14_ucs_table[s];
				}
			}
			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_CNS11643;
			}
			CK((*filter->output_function)(w, filter->data));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = (c1 << 8) | c | 0x8e0000;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => EUC-TW
 */
int
mbfl_filt_conv_wchar_euctw(int c, mbfl_convert_filter *filter)
{
	int c1, s, plane;

	s = 0;
	if (c >= ucs_a1_cns11643_table_min && c < ucs_a1_cns11643_table_max) {
		s = ucs_a1_cns11643_table[c - ucs_a1_cns11643_table_min];
	} else if (c >= ucs_a2_cns11643_table_min && c < ucs_a2_cns11643_table_max) {
		s = ucs_a2_cns11643_table[c - ucs_a2_cns11643_table_min];
	} else if (c >= ucs_a3_cns11643_table_min && c < ucs_a3_cns11643_table_max) {
		s = ucs_a3_cns11643_table[c - ucs_a3_cns11643_table_min];
	} else if (c >= ucs_i_cns11643_table_min && c < ucs_i_cns11643_table_max) {
		s = ucs_i_cns11643_table[c - ucs_i_cns11643_table_min];
	} else if (c >= ucs_r_cns11643_table_min && c < ucs_r_cns11643_table_max) {
		s = ucs_r_cns11643_table[c - ucs_r_cns11643_table_min];
	}
	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_CNS11643) {
			s = c & MBFL_WCSPLANE_MASK;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}
	if (s >= 0) {
		plane = (s & 0x1f0000) >> 16;
		if (plane <= 1){
			if (s < 0x80) {	/* latin */
				CK((*filter->output_function)(s, filter->data));
			} else {
				s = (s & 0xffff) | 0x8080;
				CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
				CK((*filter->output_function)(s & 0xff, filter->data));
			} 
		} else {
			s = (0x8ea00000 + (plane << 16)) | ((s & 0xffff) | 0x8080);
			CK((*filter->output_function)(0x8e , filter->data));
			CK((*filter->output_function)((s >> 16) & 0xff, filter->data));
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}
	return c;
}

static int mbfl_filt_ident_euctw(int c, mbfl_identify_filter *filter)
{
	switch (filter->status) {
	case  0:	/* latin */
		if (c >= 0 && c < 0x80) {	/* ok */
			;
		} else if (c > 0xa0 && c < 0xff) {	/* DBCS lead byte */
			filter->status = 1;
		} else if (c == 0x8e) {	/* DBCS lead byte */
			filter->status = 2;
		} else {							/* bad */
			filter->flag = 1;
		}
		break;

	case  1:	/* got lead byte */
		if (c < 0xa1 || c > 0xfe) {		/* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;

	case  2:	/* got lead byte */
		if (c >= 0xa1 && c < 0xaf) {	/* ok */
			filter->status = 3;
		} else {
			filter->flag = 1; /* bad */
		}
		break;

	case  3:	/* got lead byte */
		if (c < 0xa1 || c > 0xfe) {		/* bad */
			filter->flag = 1;
		}
		filter->status = 4;
		break;

	case  4:	/* got lead byte */
		if (c < 0xa1 || c > 0xfe) {		/* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

