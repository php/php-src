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
#include "mbfilter_big5.h"

#include "unicode_table_big5.h"

static int mbfl_filt_ident_big5(int c, mbfl_identify_filter *filter);

static const unsigned char mblen_table_big5[] = { /* 0x81-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static const char *mbfl_encoding_big5_aliases[] = {"CN-BIG5", "BIG-FIVE", "BIGFIVE", NULL};

const mbfl_encoding mbfl_encoding_big5 = {
	mbfl_no_encoding_big5,
	"BIG-5",
	"BIG5",
	(const char *(*)[])&mbfl_encoding_big5_aliases,
	mblen_table_big5,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_big5_wchar,
	&vtbl_wchar_big5
};

const mbfl_encoding mbfl_encoding_cp950 = {
	mbfl_no_encoding_cp950,
	"CP950",
	"BIG5",
	NULL,
	mblen_table_big5,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp950_wchar,
	&vtbl_wchar_cp950
};

const struct mbfl_identify_vtbl vtbl_identify_big5 = {
	mbfl_no_encoding_big5,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_big5
};

const struct mbfl_identify_vtbl vtbl_identify_cp950 = {
	mbfl_no_encoding_cp950,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_big5
};

const struct mbfl_convert_vtbl vtbl_big5_wchar = {
	mbfl_no_encoding_big5,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_big5_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_big5 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_big5,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_big5,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_cp950_wchar = {
	mbfl_no_encoding_cp950,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_big5_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_cp950 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp950,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_big5,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/* 63 + 94 = 157 or 94 */
static unsigned short cp950_pua_tbl[][4] = {
	{0xe000,0xe310,0xfa40,0xfefe},
	{0xe311,0xeeb7,0x8e40,0xa0fe},
	{0xeeb8,0xf6b0,0x8140,0x8dfe},
	{0xf6b1,0xf70e,0xc6a1,0xc6fe},
	{0xf70f,0xf848,0xc740,0xc8fe},
};

static inline int is_in_cp950_pua(int c1, int c) {
	if ((c1 >= 0xfa && c1 <= 0xfe) || (c1 >= 0x8e && c1 <= 0xa0) ||
			(c1 >= 0x81 && c1 <= 0x8d) || (c1 >= 0xc7 && c1 <= 0xc8)) {
		return (c >=0x40 && c <= 0x7e) || (c >= 0xa1 && c <= 0xfe);
	}
	if (c1 == 0xc6) {
		return c >= 0xa1 && c <= 0xfe;
	}
	return 0;
}

/*
 * Big5 => wchar
 */
int
mbfl_filt_conv_big5_wchar(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, w, c2;

	switch (filter->status) {
	case 0:
		if (filter->from->no_encoding == mbfl_no_encoding_cp950) {
			c1 = 0x80;
		} else {
			c1 = 0xa0;
		}

		if (c >= 0 && c <= 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data));
		} else if (c == 0xff) {
			CK((*filter->output_function)(0xf8f8, filter->data));
		} else if (c > c1 && c < 0xff) {	/* dbcs lead byte */
			filter->status = 1;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data));
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

			if (filter->from->no_encoding == mbfl_no_encoding_cp950) {
				/* PUA for CP950 */
				if (w <= 0 && is_in_cp950_pua(c1, c)) {
					c2 = c1 << 8 | c;
					for (k = 0; k < sizeof(cp950_pua_tbl)/(sizeof(unsigned short)*4); k++) {
						if (c2 >= cp950_pua_tbl[k][2] && c2 <= cp950_pua_tbl[k][3]) {
							break;
						}
					}

					if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
						w = 157*(c1 - (cp950_pua_tbl[k][2]>>8)) + c - (c >= 0xa1 ? 0x62 : 0x40)
							+ cp950_pua_tbl[k][0];
					} else {
						w = c2 - cp950_pua_tbl[k][2] + cp950_pua_tbl[k][0];
					}
				}
			}

			if (w <= 0) {
				w = (c1 << 8) | c;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_BIG5;
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
mbfl_filt_conv_wchar_big5(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, s, c2;

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

	if (filter->to->no_encoding == mbfl_no_encoding_cp950) {
		if (c >= 0xe000 && c <= 0xf848) { /* PUA for CP950 */
			for (k = 0; k < sizeof(cp950_pua_tbl)/(sizeof(unsigned short)*4); k++) {
				if (c <= cp950_pua_tbl[k][1]) {
					break;
				}
			}
			c1 = c - cp950_pua_tbl[k][0];
			if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
				c2 = cp950_pua_tbl[k][2] >> 8;
				s = ((c1 / 157) + c2) << 8; c1 %= 157;
				s |= c1 + (c1 >= 0x3f ? 0x62 : 0x40);
			} else {
				s = c1 + cp950_pua_tbl[k][2];
			}
		}

		if (c == 0x80) {
			s = 0x80;
		} else if (c == 0xf8f8) {
			s = 0xff;
		} else if (c == 0x256d) {
			s = 0xa27e;
		} else if (c == 0x256e) {
			s = 0xa2a1;
		} else if (c == 0x256f) {
			s = 0xa2a3;
		} else if (c == 0x2570) {
			s = 0xa2a2;
		}
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
		if (s <= 0x80 || s == 0xff) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return c;
}

static int mbfl_filt_ident_big5(int c, mbfl_identify_filter *filter)
{
	int c1;
	if (filter->encoding->no_encoding == mbfl_no_encoding_cp950) {
		c1 = 0x80;
	} else {
		c1 = 0xa0;
	}

	if (filter->status) {		/* kanji second char */
		if (c < 0x40 || (c > 0x7e && c < 0xa1) ||c > 0xfe) {	/* bad */
		    filter->flag = 1;
		}
		filter->status = 0;
	} else if (c >= 0 && c < 0x80) {	/* latin  ok */
		;
	} else if (c > c1 && c < 0xff) {	/* DBCS lead byte */
		filter->status = 1;
	} else {							/* bad */
		filter->flag = 1;
	}

	return c;
}
