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
 * the source code included in this files was separated from mbfilter_cp936.c
 * by rui hirokawa <hirokawa@php.net> on 11 Aug 2011.
 *
 */

#include "mbfilter.h"
#include "mbfilter_gb18030.h"

#include "unicode_table_cp936.h"
#include "unicode_table_gb18030.h"

static int mbfl_filt_ident_gb18030(int c, mbfl_identify_filter *filter);

static const char *mbfl_encoding_gb18030_aliases[] = {"gb-18030", "gb-18030-2000", NULL};

const mbfl_encoding mbfl_encoding_gb18030 = {
	mbfl_no_encoding_gb18030,
	"GB18030",
	"GB18030",
	(const char *(*)[])&mbfl_encoding_gb18030_aliases,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_gb18030_wchar,
	&vtbl_wchar_gb18030
};

const struct mbfl_identify_vtbl vtbl_identify_gb18030 = {
	mbfl_no_encoding_gb18030,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_gb18030
};

const struct mbfl_convert_vtbl vtbl_gb18030_wchar = {
	mbfl_no_encoding_gb18030,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_gb18030_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_gb18030 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_gb18030,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_gb18030,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)


int
mbfl_bisec_srch(int w, const unsigned short *tbl, int n)
{
	int k, k1 = 0, k2 = n-1;

	while (k1 < k2) {
		k = (k1+k2) >> 1;
		if (w <= tbl[2*k+1]) {
			k2 = k;
		} else if (w >= tbl[2*k+2]) {
			k1 = k + 1;
		} else {
			return -1;
		}
	}
	return k1;
}

int
mbfl_bisec_srch2(int w, const unsigned short tbl[], int n)
{
	int k, k1 = 0, k2 = n;

	if (w == tbl[0]) {
		return 0;
	}

	while (k2 - k1 > 1) {
		k = (k1 + k2) >> 1;
		if (w < tbl[k]) {
			k2 = k;
		} else if (w > tbl[k]) {
			k1 = k;
		} else {
			return k;
		}
	}
	return -1;
}

/*
 * GB18030 => wchar
 */
int
mbfl_filt_conv_gb18030_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, c2, c3, w = -1;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c == 0x80) {	/* euro sign */
			CK((*filter->output_function)(0x20ac, filter->data));
		} else if (c == 0xff) {
			CK((*filter->output_function)(0x00ff, filter->data));
		} else if (c > 0x80 && c < 0xff) {	/* dbcs/qbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
		break;

	case 1:		/* dbcs/qbcs second byte */
		c1 = filter->cache;
		filter->status = 0;

		if (c1 >= 0x81 && c1 <= 0x84 && c >= 0x30 && c <= 0x39) { /* 4 byte range: Unicode BMP */
			filter->status = 2;
			filter->cache = (c1 << 8) | c;
			return c;
		} else if (c1 >= 0x90 && c1 <= 0xe3 && c >= 0x30 && c <= 0x39) {
			/* 4 byte range: Unicode 16 planes */
			filter->status = 2;
			filter->cache = (c1 << 8) | c;
			return c;
		} else if (((c1 >= 0xaa && c1 <= 0xaf) || (c1 >= 0xf8 && c1 <= 0xfe)) &&
				   (c >= 0xa1 && c <= 0xfe)) { /* UDA part1,2: U+E000-U+E4C5 */
			w = 94*(c1 >= 0xf8 ? c1 - 0xf2 : c1 - 0xaa) + (c - 0xa1) + 0xe000;
			CK((*filter->output_function)(w, filter->data));
		} else if (c1 >= 0xa1 && c1 <= 0xa7 && c >= 0x40 && c < 0xa1 && c != 0x7f) {
			/* UDA part3 : U+E4C6-U+E765*/
			w = 96*(c1 - 0xa1) + c - (c >= 0x80 ? 0x41 : 0x40) + 0xe4c6;
			CK((*filter->output_function)(w, filter->data));
		}

		c2 = (c1 << 8) | c;

		if (w <= 0 &&
			((c2 >= 0xa2ab && c2 <= 0xa9f0 + (0xe80f-0xe801)) ||
			 (c2 >= 0xd7fa && c2 <= 0xd7fa + (0xe814-0xe810)) ||
			 (c2 >= 0xfe50 && c2 <= 0xfe80 + (0xe864-0xe844)))) {
			for (k = 0; k < mbfl_gb18030_pua_tbl_max; k++) {
				if (c2 >= mbfl_gb18030_pua_tbl[k][2] &&
					c2 <= mbfl_gb18030_pua_tbl[k][2] +  mbfl_gb18030_pua_tbl[k][1]
					-  mbfl_gb18030_pua_tbl[k][0]) {
					w = c2 -  mbfl_gb18030_pua_tbl[k][2] + mbfl_gb18030_pua_tbl[k][0];
					CK((*filter->output_function)(w, filter->data));
					break;
				}
			}
		}

		if (w <= 0) {
			if ((c1 >= 0xa1 && c1 <= 0xa9 && c >= 0xa1 && c <= 0xfe) ||
				(c1 >= 0xb0 && c1 <= 0xf7 && c >= 0xa1 && c <= 0xfe) ||
				(c1 >= 0x81 && c1 <= 0xa0 && c >= 0x40 && c <= 0xfe && c != 0x7f) ||
				(c1 >= 0xaa && c1 <= 0xfe && c >= 0x40 && c <= 0xa0 && c != 0x7f) ||
				(c1 >= 0xa8 && c1 <= 0xa9 && c >= 0x40 && c <= 0xa0 && c != 0x7f)) {
				w = (c1 - 0x81)*192 + (c - 0x40);
				if (w >= 0 && w < cp936_ucs_table_size) {
					w = cp936_ucs_table[w];
				} else {
					w = 0;
				}
				if (w <= 0) {
					w = (c1 << 8) | c;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_GB18030;
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
		}
		break;
	case 2: /* qbcs third byte */
		c1 = (filter->cache >> 8) & 0xff;
		c2 = filter->cache & 0xff;
		filter->status = 0;
		filter->cache = 0;
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) &&
			c2 >= 0x30 && c2 <= 0x39 && c >= 0x81 && c <= 0xfe) {
			filter->cache = (c1 << 16) | (c2 << 8) | c;
			filter->status = 3;
		} else {
			w = (c1 << 16) | (c2 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
		}
 		break;

	case 3: /* qbcs fourth byte */
		c1 = (filter->cache >> 16) & 0xff;
		c2 = (filter->cache >> 8) & 0xff;
		c3 = filter->cache & 0xff;
		filter->status = 0;
		filter->cache = 0;
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) &&
			c2 >= 0x30 && c2 <= 0x39 && c3 >= 0x81 && c3 <= 0xfe && c >= 0x30 && c <= 0x39) {
			if (c1 >= 0x90 && c1 <= 0xe3) {
				w = ((((c1 - 0x90)*10 + (c2 - 0x30))*126 + (c3 - 0x81)))*10 + (c - 0x30) + 0x10000;
			} else { /* Unicode BMP */
				w = (((c1 - 0x81)*10 + (c2 - 0x30))*126 + (c3 - 0x81))*10 + (c - 0x30);
				if (w >= 0 && w <= 39419) {
					k = mbfl_bisec_srch(w, mbfl_gb2uni_tbl, mbfl_gb_uni_max);
					if (k<0) {
						/* error */
						w = (c1 << 24) | (c2 << 16) | (c3 << 8) | c;
						w &= MBFL_WCSGROUP_MASK;
						w |= MBFL_WCSGROUP_THROUGH;
						CK((*filter->output_function)(w, filter->data));
						return c;
					}
					w += mbfl_gb_uni_ofst[k];
				} else {
					w = (c1 << 24) | (c2 << 16) | (c3 << 8) | c;
					w &= MBFL_WCSGROUP_MASK;
					w |= MBFL_WCSGROUP_THROUGH;
					CK((*filter->output_function)(w, filter->data));
					return c;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			w = (c1 << 24) | (c2 << 16) | (c3 << 8) | c;
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
 * wchar => GB18030
 */
int
mbfl_filt_conv_wchar_gb18030(int c, mbfl_convert_filter *filter)
{
	int k, k1, k2;
	int c1, s = 0, s1 = 0;

	if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max) {
		s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
	} else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max) {
		s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
	} else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max) {
		s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
	} else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max) {
		s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
	} else if (c >= ucs_ci_cp936_table_min && c < ucs_ci_cp936_table_max) {
		/* U+F900-FA2F CJK Compatibility Ideographs */
		if (c == 0xf92c) {
			s = 0xfd9c;
		} else if (c == 0xf979) {
			s = 0xfd9d;
		} else if (c == 0xf995) {
			s = 0xfd9e;
		} else if (c == 0xf9e7) {
			s = 0xfd9f;
		} else if (c == 0xf9f1) {
			s = 0xfda0;
		} else if (c >= 0xfa0c && c <= 0xfa29) {
			s = ucs_ci_s_cp936_table[c - 0xfa0c];
		}
	} else if (c >= ucs_cf_cp936_table_min && c < ucs_cf_cp936_table_max) {
		/* FE30h CJK Compatibility Forms  */
		s = ucs_cf_cp936_table[c - ucs_cf_cp936_table_min];
	} else if (c >= ucs_sfv_cp936_table_min && c < ucs_sfv_cp936_table_max) {
		/* U+FE50-FE6F Small Form Variants */
		s = ucs_sfv_cp936_table[c - ucs_sfv_cp936_table_min];
	} else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max) {
		/* U+FF00-FFFF HW/FW Forms */
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

	if (c == 0x20ac) { /* euro-sign */
		s = 0xa2e3;
	}

	if (s <= 0 && c >= mbfl_gb18030_c_tbl_key[0] &&
		c <= mbfl_gb18030_c_tbl_key[mbfl_gb18030_c_tbl_max-1]) {
		k1 = mbfl_bisec_srch2(c, mbfl_gb18030_c_tbl_key, mbfl_gb18030_c_tbl_max);
		if (k1 >= 0) {
			s = mbfl_gb18030_c_tbl_val[k1];
		}
	}

	if (c >= 0xe000 && c <= 0xe864) { /* PUA */
		if (c < 0xe766) {
			if (c < 0xe4c6) {
				c1 = c - 0xe000;
				s = (c1 % 94) + 0xa1; c1 /= 94;
				s |= (c1 < 0x06 ? c1 + 0xaa : c1 + 0xf2) << 8;
			} else {
				c1 = c - 0xe4c6;
				s = ((c1 / 96) + 0xa1) << 8; c1 %= 96;
				s |= c1 + (c1 >= 0x3f ? 0x41 : 0x40);
			}
		} else {
			/* U+E766..U+E864 */
			k1 = 0; k2 = mbfl_gb18030_pua_tbl_max;
			while (k1 < k2) {
				k = (k1 + k2) >> 1;
				if (c < mbfl_gb18030_pua_tbl[k][0]) {
					k2 = k;
				} else if (c > mbfl_gb18030_pua_tbl[k][1]) {
					k1 = k + 1;
				} else {
					s = c - mbfl_gb18030_pua_tbl[k][0] + mbfl_gb18030_pua_tbl[k][2];
					break;
				}
			}
		}
	}

	if (s <= 0 && c >= 0x0080 && c <= 0xffff) { /* BMP */
		s = mbfl_bisec_srch(c, mbfl_uni2gb_tbl, mbfl_gb_uni_max);
		if (s >= 0) {
			c1 = c - mbfl_gb_uni_ofst[s];
			s = (c1 % 10) + 0x30; c1 /= 10;
			s |= ((c1 % 126) + 0x81) << 8; c1 /= 126;
			s |= ((c1 % 10) + 0x30) << 16; c1 /= 10;
			s1 = c1 + 0x81;
		}
	} else if (c >= 0x10000 && c <= 0x10ffff) { /* Code set 3: Unicode U+10000..U+10FFFF */
		c1 = c - 0x10000;
		s = (c1 % 10) + 0x30; c1 /= 10;
		s |= ((c1 % 126) + 0x81) << 8; c1 /= 126;
		s |= ((c1 % 10) + 0x30) << 16; c1 /= 10;
		s1 = c1 + 0x90;
	}

	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_WINCP936) {
			s = c & MBFL_WCSPLANE_MASK;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s <= 0x80) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else if (s1 > 0) { /* qbcs */
			CK((*filter->output_function)(s1 & 0xff, filter->data));
			CK((*filter->output_function)((s >> 16) & 0xff, filter->data));
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else { /* dbcs */
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

static int mbfl_filt_ident_gb18030(int c, mbfl_identify_filter *filter)
{
	int c1;

	c1 = (filter->status >> 8) & 0xff;
	filter->status &= 0xff;

	if (filter->status == 0) {
		if (c <= 0x80 || c == 0xff) {
			filter->status = 0;
		} else {
			filter->status = 1;
			filter->status |= (c << 8);
		}
	} else if (filter->status == 1) { /* dbcs/qbcs 2nd byte */
		if (((c1 >= 0x81 && c1 <= 0x84) || (c1 >= 0x90 && c1 <= 0xe3)) && c >= 0x30 && c <= 0x39) { /* qbcs */
			filter->status = 2;
		} else if (((c1 >= 0xaa && c1 <= 0xaf) || (c1 >= 0xf8 && c1 <= 0xfe)) && (c >= 0xa1 && c <= 0xfe)) {
			filter->status = 0; /* UDA part 1,2 */
		} else if (c1 >= 0xa1 && c1 <= 0xa7 && c >= 0x40 && c < 0xa1 && c != 0x7f) {
			filter->status = 0; /* UDA part 3 */
		} else if ((c1 >= 0xa1 && c1 <= 0xa9 && c >= 0xa1 && c <= 0xfe) ||
				   (c1 >= 0xb0 && c1 <= 0xf7 && c >= 0xa1 && c <= 0xfe) ||
				   (c1 >= 0x81 && c1 <= 0xa0 && c >= 0x40 && c <= 0xfe && c != 0x7f) ||
				   (c1 >= 0xaa && c1 <= 0xfe && c >= 0x40 && c <= 0xa0 && c != 0x7f) ||
				   (c1 >= 0xa8 && c1 <= 0xa9 && c >= 0x40 && c <= 0xa0 && c != 0x7f)) {
			filter->status = 0; /* DBCS */
		} else {
			filter->flag = 1; /* bad */
			filter->status = 0;
		}
	} else if (filter->status == 2) { /* qbcs 3rd byte */
		if (c > 0x80 && c < 0xff) {
			filter->status = 3;
		} else {
			filter->flag = 1; /* bad */
			filter->status = 0;
		}
	} else if (filter->status == 3) { /* qbcs 4th byte */
		if (c >= 0x30 && c < 0x40) {
			filter->status = 0;
		} else {
			filter->flag = 1; /* bad */
			filter->status = 0;
		}
	} else {							/* bad */
		filter->flag = 1;
	}

	return c;
}
