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
 * The source code included in this files was separated from mbfilter_kr.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_iso2022_kr.h"
#include "unicode_table_uhc.h"

static int mbfl_filt_ident_2022kr(int c, mbfl_identify_filter *filter);

const mbfl_encoding mbfl_encoding_2022kr = {
	mbfl_no_encoding_2022kr,
	"ISO-2022-KR",
	"ISO-2022-KR",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE
};

const struct mbfl_identify_vtbl vtbl_identify_2022kr = {
	mbfl_no_encoding_2022kr,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_2022kr
};

const struct mbfl_convert_vtbl vtbl_wchar_2022kr = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022kr,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_2022kr,
	mbfl_filt_conv_any_2022kr_flush
};

const struct mbfl_convert_vtbl vtbl_2022kr_wchar = {
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_2022kr_wchar,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * ISO-2022-KR => wchar
 */
int
mbfl_filt_conv_2022kr_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, w, flag;

retry:
	switch (filter->status & 0xf) {
		/* case 0x00: ASCII */
		/* case 0x10: KSC5601 */
	case 0:
		if (c == 0x1b) { /* ESC */
			filter->status += 2;
		} else if (c == 0x0f) { /* SI (ASCII) */
			filter->status &= ~0xff; 
		} else if (c == 0x0e) { /* SO (KSC5601) */
			filter->status |= 0x10; 
		} else if ((filter->status & 0x10) != 0  && c > 0x20 && c < 0x7f) {
			/* KSC5601 lead byte */
			filter->cache = c;
			filter->status += 1;
		} else if ((filter->status & 0x10) == 0 &&  c >= 0 && c < 0x80) {
			/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 1:		/* dbcs second byte */
		filter->status &= ~0xf;
		c1 = filter->cache;
		flag = 0;
		if (c1 > 0x20 && c1 < 0x47) {
			flag = 1;
		} else if (c1 >= 0x47 && c1 <= 0x7e && c1 != 0x49) {
			flag = 2;
		}
		if (flag > 0 && c > 0x20 && c < 0x7f) {
			if (flag == 1){
				w = (c1 - 0x21)*190 + (c - 0x41) + 0x80;
				if (w >= 0 && w < uhc2_ucs_table_size) {
					w = uhc2_ucs_table[w];
				} else {
					w = 0;
				}
			} else {
				w = (c1 - 0x47)*94 + (c - 0x21);
				if (w >= 0 && w < uhc3_ucs_table_size) {
					w = uhc3_ucs_table[w];
				} else {
					w = 0;
				}
			}
			
			if (w <= 0) {
				w = (c1 << 8) | c;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_KSC5601;
			}
			CK((*filter->output_function)(w, filter->data));
		} else if (c == 0x1b) {	 /* ESC */
			filter->status++;
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 2: 		/* ESC */
		if (c == 0x24) { /* '$' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data));
			goto retry;
		}
		break;
	case 3:         /* ESC $ */
		if (c == 0x29) { /* ')' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			goto retry;
		}
		break;
	case 4:         /* ESC $ )  */
		if (c == 0x43) { /* 'C' */
			filter->status &= ~0xf;
			filter->status |= 0x100;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(0x29, filter->data));
			goto retry;
		}
		break;
	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => ISO-2022-KR
 */
int
mbfl_filt_conv_wchar_2022kr(int c, mbfl_convert_filter *filter)
{
	int c1, c2, s;

	s = 0;

	if (c >= ucs_a1_uhc_table_min && c < ucs_a1_uhc_table_max) {
		s = ucs_a1_uhc_table[c - ucs_a1_uhc_table_min];
	} else if (c >= ucs_a2_uhc_table_min && c < ucs_a2_uhc_table_max) {
		s = ucs_a2_uhc_table[c - ucs_a2_uhc_table_min];
	} else if (c >= ucs_a3_uhc_table_min && c < ucs_a3_uhc_table_max) {
		s = ucs_a3_uhc_table[c - ucs_a3_uhc_table_min];
	} else if (c >= ucs_i_uhc_table_min && c < ucs_i_uhc_table_max) {
		s = ucs_i_uhc_table[c - ucs_i_uhc_table_min];
	} else if (c >= ucs_s_uhc_table_min && c < ucs_s_uhc_table_max) {
		s = ucs_s_uhc_table[c - ucs_s_uhc_table_min];
	} else if (c >= ucs_r1_uhc_table_min && c < ucs_r1_uhc_table_max) {
		s = ucs_r1_uhc_table[c - ucs_r1_uhc_table_min];
	} else if (c >= ucs_r2_uhc_table_min && c < ucs_r2_uhc_table_max) {
		s = ucs_r2_uhc_table[c - ucs_r2_uhc_table_min];
	}

	c1 = (s >> 8) & 0xff;
	c2 = s & 0xff;
	/* exclude UHC extension area */
	if (c1 < 0xa1 || c2 < 0xa1){ 
		s = c;
	}
	if (s & 0x8000) {
		s -= 0x8080;
	}

	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_KSC5601) {
			s = c & MBFL_WCSPLANE_MASK;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
		s = -1;
	}
	if (s >= 0) {
		if (s < 0x80 && s > 0) {	/* ASCII */
			if ((filter->status & 0x10) != 0) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SI */
				filter->status &= ~0x10;
			}
			CK((*filter->output_function)(s, filter->data));
		} else {
			if ( (filter->status & 0x100) == 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x29, filter->data));		/* ')' */
				CK((*filter->output_function)(0x43, filter->data));		/* 'C' */
				filter->status |= 0x100;
			}
			if ((filter->status & 0x10) == 0) {
				CK((*filter->output_function)(0x0e, filter->data));		/* SO */
				filter->status |= 0x10;
			}
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

int
mbfl_filt_conv_any_2022kr_flush(mbfl_convert_filter *filter)
{
	/* back to ascii */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x0f, filter->data));		/* SI */
	}
	filter->status &= 0xff;
	return 0;
}

static int mbfl_filt_ident_2022kr(int c, mbfl_identify_filter *filter)
{
retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 KSC5601 mode */
/*	case 0x20:	 KSC5601 DBCS */
/*	case 0x40:	 KSC5601 SBCS */
	case 0:
		if (!(filter->status & 0x10)) {
			if (c == 0x1b)
				filter->status += 2;
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x21:	 KSC5601 second char */
	case 1:
		filter->status &= ~0xf;
		if (c < 0x21 || c > 0x7e) {		/* bad */
			filter->flag = 1;
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x29) {		/* ')' */
			filter->status++;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $) */
	case 5:
		if (c == 0x43) {		/* 'C' */
			filter->status = 0x10;
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


