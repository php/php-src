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
 * "streamable korean code filter and converter"
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"

#if defined(HAVE_MBSTR_KR)
#include "mbfilter.h"
#include "mbfilter_cn.h"

#include "unicode_table_kr.h"

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)


/*
 * EUC-KR => wchar
 */
int
mbfl_filt_conv_euckr_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, w, flag;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xff && c != 0xc9) {	/* dbcs lead byte */
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
		flag = 0;
		if (c1 >= 0xa1 && c1 <= 0xc6) {
			flag = 1;
		} else if (c1 >= 0xc7 && c1 <= 0xfe && c1 != 0xc9) {
			flag = 2;
		}
		if (flag > 0 && c >= 0xa1 && c <= 0xfe) {
			if (flag == 1){ /* 1st: 0xa1..0xc6, 2nd: 0x41..0x7a, 0x81..0xfe */
				w = (c1 - 0xa1)*190 + (c - 0x41);
				if (w >= 0 && w < uhc2_ucs_table_size) {
					w = uhc2_ucs_table[w];
				} else {
					w = 0;
				}
			} else { /* 1st: 0xc7..0xc8,0xca..0xfe, 2nd: 0xa1..0xfe */
				w = (c1 - 0xc7)*94 + (c - 0xa1);
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
 * wchar => EUC-KR
 */
int
mbfl_filt_conv_wchar_euckr(int c, mbfl_convert_filter *filter TSRMLS_DC)
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


/*
 * ISO-2022-KR => wchar
 */
int
mbfl_filt_conv_2022kr_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
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
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
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
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if (c == 0x1b) {	 /* ESC */
			filter->status++;
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 2: 		/* ESC */
		if (c == 0x24) { /* '$' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
			goto retry;
		}
		break;
	case 3:         /* ESC $ */
		if (c == 0x29) { /* ')' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));
			goto retry;
		}
		break;
	case 4:         /* ESC $ )  */
		if (c == 0x43) { /* 'C' */
			filter->status &= ~0xf;
			filter->status |= 0x100;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x29, filter->data TSRMLS_CC));
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
mbfl_filt_conv_wchar_2022kr(int c, mbfl_convert_filter *filter TSRMLS_DC)
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
				CK((*filter->output_function)(0x0f, filter->data TSRMLS_CC));		/* SI */
				filter->status &= ~0x10;
			}
			CK((*filter->output_function)(s, filter->data TSRMLS_CC));
		} else {
			if ( (filter->status & 0x100) == 0) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));		/* '$' */
				CK((*filter->output_function)(0x29, filter->data TSRMLS_CC));		/* ')' */
				CK((*filter->output_function)(0x43, filter->data TSRMLS_CC));		/* 'C' */
				filter->status |= 0x100;
			}
			if ((filter->status & 0x10) == 0) {
				CK((*filter->output_function)(0x0e, filter->data TSRMLS_CC));		/* SO */
				filter->status |= 0x10;
			}
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
 * UHC => wchar
 */
int
mbfl_filt_conv_uhc_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, w, flag;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0x80 && c < 0xff && c != 0xc9) {	/* dbcs lead byte */
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

		if ( c1 >= 0x81 && c1 <= 0xa0){
			w = (c1 - 0x81)*190 + (c - 0x41);
			if (w >= 0 && w < uhc1_ucs_table_size) {
				w = uhc1_ucs_table[w];
			} else {
				w = 0;
			}			
		} else if ( c1 >= 0xa1 && c1 <= 0xc6){
			w = (c1 - 0xa1)*190 + (c - 0x41);			
			if (w >= 0 && w < uhc2_ucs_table_size) {
				w = uhc2_ucs_table[w];
			} else {
				w = 0;
			}			
		} else if ( c1 >= 0xc7 && c1 <= 0xfe){
			w = (c1 - 0xc7)*94 + (c - 0xa1);		
			if (w >= 0 && w < uhc3_ucs_table_size) {
				w = uhc3_ucs_table[w];
			} else {
				w = 0;
			}			
		}
		if (flag > 0){
			if (w <= 0) {
				w = (c1 << 8) | c;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_UHC;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else {
			if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
				CK((*filter->output_function)(c, filter->data TSRMLS_CC));
			} else {
				w = (c1 << 8) | c;
				w &= MBFL_WCSGROUP_MASK;
				w |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(w, filter->data TSRMLS_CC));
			}
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => UHC
 */
int
mbfl_filt_conv_wchar_uhc(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s;

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
	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_UHC) {
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

int
mbfl_filt_conv_any_2022kr_flush(mbfl_convert_filter *filter TSRMLS_DC)
{
	/* back to ascii */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x0f, filter->data TSRMLS_CC));		/* SI */
	}
	filter->status &= 0xff;
	return 0;
}

#endif /* HAVE_MBSTR_KR */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
