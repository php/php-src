/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 The PHP Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rui Hirokawa <hirokawa@php.net>                              |
   +----------------------------------------------------------------------+
 */

/*
 * "streamable traditional chinese code filter and converter"
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"

#if defined(HAVE_MBSTR_TW)
#include "mbfilter.h"
#include "mbfilter_tw.h"

#include "unicode_table_tw.h"

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * EUC-TW => wchar
 */
int
mbfl_filt_conv_euctw_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s, w, plane;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xff) {	/* dbcs first byte */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* mbcs first byte */
			filter->status = 2;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
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
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 2:	/* got 0x8e,  first char */
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
			filter->status = 0;
		} else if (c > 0xa0 && c < 0xaf) {
			filter->status = 3;
			filter->cache = c - 0xa1;
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 3:	/* got 0x8e,  third char */
		filter->status = 0;
		c1 = filter->cache;
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
			filter->status = 0;
		} else if (c > 0xa0 && c < 0xff) {
			filter->status = 4;
			filter->cache = (c1 << 8) + c - 0xa1;
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
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
				if (plane == 1 & s < cns11643_2_ucs_table_size) {
					w = cns11643_2_ucs_table[s];
				}
				if (plane == 13 & s < cns11643_14_ucs_table_size) {
					w = cns11643_14_ucs_table[s];
				}
			}
			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_CNS11643;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c | 0x8e0000;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
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
mbfl_filt_conv_wchar_euctw(int c, mbfl_convert_filter *filter TSRMLS_DC)
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
				CK((*filter->output_function)(s, filter->data TSRMLS_CC));
			} else {
				s = (s & 0xffff) | 0x8080;
				CK((*filter->output_function)((s >> 8) & 0xff, filter->data TSRMLS_CC));
				CK((*filter->output_function)(s & 0xff, filter->data TSRMLS_CC));
			} 
		} else {
			s = (0x8ea00000 + (plane << 16)) | ((s & 0xffff) | 0x8080);
			CK((*filter->output_function)(0x8e , filter->data TSRMLS_CC));
			CK((*filter->output_function)((s >> 16) & 0xff, filter->data TSRMLS_CC));
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s & 0xff, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}
	return c;
}

/*
 * Big5 => wchar
 */
int
mbfl_filt_conv_big5_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xff) {	/* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 1:		/* dbcs second byte */
		filter->status = 0;
		c1 = filter->cache;
		if ((c > 0x39 && c < 0x7f) | (c > 0xa0 && c < 0xff)) {
			if (c < 0x7f){
				w = (c1 - 0xa1)*157 + (c - 0x40);
			} else {
				w = (c1 - 0xa1)*157 + (c - 0xa1) + 0x3f;
			}
			if (w >= 0 && w < big5_ucs_table_size) {
				w = big5_ucs_table[w];
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = (c1 << 8) | c;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_BIG5;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => Big5
 */
int
mbfl_filt_conv_wchar_big5(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s;

	s = 0;
	if (c >= ucs_a1_big5_table_min && c < ucs_a1_big5_table_max) {
		s = ucs_a1_big5_table[c - ucs_a1_big5_table_min];
	} else if (c >= ucs_a2_big5_table_min && c < ucs_a2_big5_table_max) {
		s = ucs_a2_big5_table[c - ucs_a2_big5_table_min];
	} else if (c >= ucs_a3_big5_table_min && c < ucs_a3_big5_table_max) {
		s = ucs_a3_big5_table[c - ucs_a3_big5_table_min];
	} else if (c >= ucs_i_big5_table_min && c < ucs_i_big5_table_max) {
		s = ucs_i_big5_table[c - ucs_i_big5_table_min];
	} else if (c >= ucs_pua_big5_table_min && c < ucs_pua_big5_table_max) {
		s = ucs_pua_big5_table[c - ucs_pua_big5_table_min];
	} else if (c >= ucs_r1_big5_table_min && c < ucs_r1_big5_table_max) {
		s = ucs_r1_big5_table[c - ucs_r1_big5_table_min];
	} else if (c >= ucs_r2_big5_table_min && c < ucs_r2_big5_table_max) {
		s = ucs_r2_big5_table[c - ucs_r2_big5_table_min];
	}
	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_BIG5) {
			s = c & MBFL_WCSPLANE_MASK;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s < 0x80) {	/* latin */
			CK((*filter->output_function)(s, filter->data TSRMLS_CC));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s & 0xff, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}

#endif /* HAVE_MBSTR_TW */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
