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
 * The source code included in this files was separated from mbfilter.c
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfl_encoding.h"
#include "mbfl_allocators.h"
#include "mbfl_filter_output.h"
#include "mbfilter_pass.h"
#include "mbfilter_8bit.h"
#include "mbfilter_wchar.h"

#include "filters/mbfilter_euc_cn.h"
#include "filters/mbfilter_hz.h"
#include "filters/mbfilter_euc_tw.h"
#include "filters/mbfilter_big5.h"
#include "filters/mbfilter_uhc.h"
#include "filters/mbfilter_euc_kr.h"
#include "filters/mbfilter_iso2022_kr.h"
#include "filters/mbfilter_sjis.h"
#include "filters/mbfilter_sjis_open.h"
#include "filters/mbfilter_sjis_2004.h"
#include "filters/mbfilter_sjis_mobile.h"
#include "filters/mbfilter_sjis_mac.h"
#include "filters/mbfilter_cp51932.h"
#include "filters/mbfilter_jis.h"
#include "filters/mbfilter_iso2022_jp_ms.h"
#include "filters/mbfilter_iso2022jp_2004.h"
#include "filters/mbfilter_iso2022jp_mobile.h"
#include "filters/mbfilter_euc_jp.h"
#include "filters/mbfilter_euc_jp_2004.h"
#include "filters/mbfilter_euc_jp_win.h"
#include "filters/mbfilter_gb18030.h"
#include "filters/mbfilter_ascii.h"
#include "filters/mbfilter_koi8r.h"
#include "filters/mbfilter_koi8u.h"
#include "filters/mbfilter_cp866.h"
#include "filters/mbfilter_cp932.h"
#include "filters/mbfilter_cp936.h"
#include "filters/mbfilter_cp1251.h"
#include "filters/mbfilter_cp1252.h"
#include "filters/mbfilter_cp1254.h"
#include "filters/mbfilter_cp5022x.h"
#include "filters/mbfilter_iso8859_1.h"
#include "filters/mbfilter_iso8859_2.h"
#include "filters/mbfilter_iso8859_3.h"
#include "filters/mbfilter_iso8859_4.h"
#include "filters/mbfilter_iso8859_5.h"
#include "filters/mbfilter_iso8859_6.h"
#include "filters/mbfilter_iso8859_7.h"
#include "filters/mbfilter_iso8859_8.h"
#include "filters/mbfilter_iso8859_9.h"
#include "filters/mbfilter_iso8859_10.h"
#include "filters/mbfilter_iso8859_13.h"
#include "filters/mbfilter_iso8859_14.h"
#include "filters/mbfilter_iso8859_15.h"
#include "filters/mbfilter_base64.h"
#include "filters/mbfilter_qprint.h"
#include "filters/mbfilter_uuencode.h"
#include "filters/mbfilter_7bit.h"
#include "filters/mbfilter_utf7.h"
#include "filters/mbfilter_utf7imap.h"
#include "filters/mbfilter_utf8.h"
#include "filters/mbfilter_utf8_mobile.h"
#include "filters/mbfilter_utf16.h"
#include "filters/mbfilter_utf32.h"
#include "filters/mbfilter_byte2.h"
#include "filters/mbfilter_byte4.h"
#include "filters/mbfilter_ucs4.h"
#include "filters/mbfilter_ucs2.h"
#include "filters/mbfilter_htmlent.h"
#include "filters/mbfilter_armscii8.h"
#include "filters/mbfilter_cp850.h"

/* hex character table "0123456789ABCDEF" */
static char mbfl_hexchar_table[] = {
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46
};

const struct mbfl_convert_vtbl *mbfl_convert_filter_list[] = {
	&vtbl_utf8_wchar,
	&vtbl_wchar_utf8,
	&vtbl_eucjp_wchar,
	&vtbl_wchar_eucjp,
	&vtbl_sjis_wchar,
	&vtbl_wchar_sjis,
	&vtbl_sjis_open_wchar,
	&vtbl_wchar_sjis_open,
	&vtbl_sjis2004_wchar,
	&vtbl_wchar_sjis2004,
	&vtbl_cp51932_wchar,
	&vtbl_wchar_cp51932,
	&vtbl_jis_wchar,
	&vtbl_wchar_jis,
	&vtbl_jis_ms_wchar,
	&vtbl_wchar_jis_ms,
	&vtbl_2022jp_wchar,
	&vtbl_wchar_2022jp,
	&vtbl_2022jpms_wchar,
	&vtbl_wchar_2022jpms,
	&vtbl_2022jp_2004_wchar,
	&vtbl_wchar_2022jp_2004,
	&vtbl_2022jp_kddi_wchar,
	&vtbl_wchar_2022jp_kddi,
	&vtbl_eucjpwin_wchar,
	&vtbl_wchar_eucjpwin,
	&vtbl_eucjp2004_wchar,
	&vtbl_wchar_eucjp2004,
	&vtbl_cp932_wchar,
	&vtbl_wchar_cp932,
 	&vtbl_sjis_docomo_wchar,
 	&vtbl_wchar_sjis_docomo,
 	&vtbl_sjis_kddi_wchar,
 	&vtbl_wchar_sjis_kddi,
 	&vtbl_sjis_sb_wchar,
 	&vtbl_wchar_sjis_sb,
 	&vtbl_sjis_mac_wchar,
 	&vtbl_wchar_sjis_mac,
	&vtbl_utf8_docomo_wchar,
	&vtbl_wchar_utf8_docomo,
	&vtbl_utf8_kddi_a_wchar,
	&vtbl_wchar_utf8_kddi_a,
	&vtbl_utf8_kddi_b_wchar,
	&vtbl_wchar_utf8_kddi_b,
	&vtbl_utf8_sb_wchar,
	&vtbl_wchar_utf8_sb,
	&vtbl_euccn_wchar,
	&vtbl_wchar_euccn,
	&vtbl_cp936_wchar,
	&vtbl_wchar_cp936,
	&vtbl_gb18030_wchar,
	&vtbl_wchar_gb18030,
	&vtbl_hz_wchar,
	&vtbl_wchar_hz,
	&vtbl_euctw_wchar,
	&vtbl_wchar_euctw,
	&vtbl_big5_wchar,
	&vtbl_wchar_big5,
	&vtbl_cp950_wchar,
	&vtbl_wchar_cp950,
	&vtbl_euckr_wchar,
	&vtbl_wchar_euckr,
	&vtbl_uhc_wchar,
	&vtbl_wchar_uhc,
	&vtbl_2022kr_wchar,
	&vtbl_wchar_2022kr,
	&vtbl_cp1251_wchar,
	&vtbl_wchar_cp1251,
	&vtbl_cp866_wchar,
	&vtbl_wchar_cp866,
	&vtbl_koi8r_wchar,
	&vtbl_wchar_koi8r,
	&vtbl_koi8u_wchar,
	&vtbl_wchar_koi8u,
	&vtbl_cp1252_wchar,
	&vtbl_wchar_cp1252,
	&vtbl_cp1254_wchar,
	&vtbl_wchar_cp1254,
	&vtbl_cp50220_wchar,
	&vtbl_wchar_cp50220,
	&vtbl_cp50220raw_wchar,
	&vtbl_wchar_cp50220raw,
	&vtbl_cp50221_wchar,
	&vtbl_wchar_cp50221,
	&vtbl_cp50222_wchar,
	&vtbl_wchar_cp50222,
	&vtbl_ascii_wchar,
	&vtbl_wchar_ascii,
	&vtbl_8859_1_wchar,
	&vtbl_wchar_8859_1,
	&vtbl_8859_2_wchar,
	&vtbl_wchar_8859_2,
	&vtbl_8859_3_wchar,
	&vtbl_wchar_8859_3,
	&vtbl_8859_4_wchar,
	&vtbl_wchar_8859_4,
	&vtbl_8859_5_wchar,
	&vtbl_wchar_8859_5,
	&vtbl_8859_6_wchar,
	&vtbl_wchar_8859_6,
	&vtbl_8859_7_wchar,
	&vtbl_wchar_8859_7,
	&vtbl_8859_8_wchar,
	&vtbl_wchar_8859_8,
	&vtbl_8859_9_wchar,
	&vtbl_wchar_8859_9,
	&vtbl_8859_10_wchar,
	&vtbl_wchar_8859_10,
	&vtbl_8859_13_wchar,
	&vtbl_wchar_8859_13,
	&vtbl_8859_14_wchar,
	&vtbl_wchar_8859_14,
	&vtbl_8859_15_wchar,
	&vtbl_wchar_8859_15,
	&vtbl_8bit_b64,
	&vtbl_b64_8bit,
	&vtbl_uuencode_8bit,
	&vtbl_wchar_html,
	&vtbl_html_wchar,
	&vtbl_8bit_qprint,
	&vtbl_qprint_8bit,
	&vtbl_8bit_7bit,
	&vtbl_7bit_8bit,
	&vtbl_utf7_wchar,
	&vtbl_wchar_utf7,
	&vtbl_utf7imap_wchar,
	&vtbl_wchar_utf7imap,
	&vtbl_utf16_wchar,
	&vtbl_wchar_utf16,
	&vtbl_utf16be_wchar,
	&vtbl_wchar_utf16be,
	&vtbl_utf16le_wchar,
	&vtbl_wchar_utf16le,
	&vtbl_utf32_wchar,
	&vtbl_wchar_utf32,
	&vtbl_utf32be_wchar,
	&vtbl_wchar_utf32be,
	&vtbl_utf32le_wchar,
	&vtbl_wchar_utf32le,
	&vtbl_ucs4_wchar,
	&vtbl_wchar_ucs4,
	&vtbl_ucs4be_wchar,
	&vtbl_wchar_ucs4be,
	&vtbl_ucs4le_wchar,
	&vtbl_wchar_ucs4le,
	&vtbl_ucs2_wchar,
	&vtbl_wchar_ucs2,
	&vtbl_ucs2be_wchar,
	&vtbl_wchar_ucs2be,
	&vtbl_ucs2le_wchar,
	&vtbl_wchar_ucs2le,
	&vtbl_byte4be_wchar,
	&vtbl_wchar_byte4be,
	&vtbl_byte4le_wchar,
	&vtbl_wchar_byte4le,
	&vtbl_byte2be_wchar,
	&vtbl_wchar_byte2be,
	&vtbl_byte2le_wchar,
	&vtbl_wchar_byte2le,
	&vtbl_armscii8_wchar,
	&vtbl_wchar_armscii8,
	&vtbl_cp850_wchar,
	&vtbl_wchar_cp850,
	&vtbl_pass,
	NULL
};

static int
mbfl_convert_filter_common_init(
	mbfl_convert_filter *filter,
	enum mbfl_no_encoding from,
	enum mbfl_no_encoding to,
	const struct mbfl_convert_vtbl *vtbl,
    int (*output_function)(int, void* ),
    int (*flush_function)(void*),
    void* data)
{
	/* encoding structure */
	if ((filter->from = mbfl_no2encoding(from)) == NULL) {
		return 1;
	}

	if ((filter->to = mbfl_no2encoding(to)) == NULL) {
		return 1;
	}

	if (output_function != NULL) {
		filter->output_function = output_function;
	} else {
		filter->output_function = mbfl_filter_output_null;
	}

	filter->flush_function = flush_function;
	filter->data = data;
	filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	filter->illegal_substchar = 0x3f;		/* '?' */
	filter->num_illegalchar = 0;
	filter->filter_ctor = vtbl->filter_ctor;
	filter->filter_dtor = vtbl->filter_dtor;
	filter->filter_function = vtbl->filter_function;
	filter->filter_flush = vtbl->filter_flush;
	filter->filter_copy = vtbl->filter_copy;

	(*filter->filter_ctor)(filter);

	return 0;
}


mbfl_convert_filter *
mbfl_convert_filter_new(
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int (*output_function)(int, void* ),
    int (*flush_function)(void*),
    void* data)
{
	mbfl_convert_filter * filter;
	const struct mbfl_convert_vtbl *vtbl;

	vtbl = mbfl_convert_filter_get_vtbl(from, to);

	if (vtbl == NULL) {
		vtbl = &vtbl_pass;
	}

	/* allocate */
	filter = (mbfl_convert_filter *)mbfl_malloc(sizeof(mbfl_convert_filter));
	if (filter == NULL) {
		return NULL;
	}

	if (mbfl_convert_filter_common_init(filter, from, to, vtbl,
			output_function, flush_function, data)) {
		mbfl_free(filter);
		return NULL;
	}

	return filter;
}

mbfl_convert_filter *
mbfl_convert_filter_new2(
	const struct mbfl_convert_vtbl *vtbl,
    int (*output_function)(int, void* ),
    int (*flush_function)(void*),
    void* data)
{
	mbfl_convert_filter * filter;

	if (vtbl == NULL) {
		vtbl = &vtbl_pass;
	}

	/* allocate */
	filter = (mbfl_convert_filter *)mbfl_malloc(sizeof(mbfl_convert_filter));
	if (filter == NULL) {
		return NULL;
	}

	if (mbfl_convert_filter_common_init(filter, vtbl->from, vtbl->to, vtbl,
			output_function, flush_function, data)) {
		mbfl_free(filter);
		return NULL;
	}

	return filter;
}

void
mbfl_convert_filter_delete(mbfl_convert_filter *filter)
{
	if (filter) {
		(*filter->filter_dtor)(filter);
		mbfl_free((void*)filter);
	}
}

int
mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter)
{
	return (*filter->filter_function)(c, filter);
}

int
mbfl_convert_filter_flush(mbfl_convert_filter *filter)
{
	(*filter->filter_flush)(filter);
	return (filter->flush_function ? (*filter->flush_function)(filter->data) : 0);
}

void mbfl_convert_filter_reset(mbfl_convert_filter *filter,
	    enum mbfl_no_encoding from, enum mbfl_no_encoding to)
{
	const struct mbfl_convert_vtbl *vtbl;

	/* destruct old filter */
	(*filter->filter_dtor)(filter);

	vtbl = mbfl_convert_filter_get_vtbl(from, to);

	if (vtbl == NULL) {
		vtbl = &vtbl_pass;
	}

	mbfl_convert_filter_common_init(filter, from, to, vtbl,
			filter->output_function, filter->flush_function, filter->data);
}

void
mbfl_convert_filter_copy(
    mbfl_convert_filter *src,
    mbfl_convert_filter *dest)
{
	if (src->filter_copy != NULL) {
		src->filter_copy(src, dest);
		return;
	}

	*dest = *src;
}

int mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src) 
{
	int n;
	unsigned char *p;

	p = src->buffer;
	n = src->pos;
	while (n > 0) {
		if ((*filter->filter_function)(*p++, filter) < 0) {
			return -1;
		}
		n--;
	}

	return n;
}

int mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p)
{
	int c;

	while ((c = *p++) != '\0') {
		if ((*filter->filter_function)(c, filter) < 0) {
			return -1;
		}
	}

	return 0;
}

/* illegal character output function for conv-filter */
int
mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter)
{
	int mode_backup, ret, n, m, r;

	ret = 0;
	mode_backup = filter->illegal_mode;
	filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
	switch (mode_backup) {
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR:
		ret = (*filter->filter_function)(filter->illegal_substchar, filter);
		break;
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG:
		if (c >= 0) {
			if (c < MBFL_WCSGROUP_UCS4MAX) {	/* unicode */
				ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"U+");
			} else {
				if (c < MBFL_WCSGROUP_WCHARMAX) {
					m = c & ~MBFL_WCSPLANE_MASK;
					switch (m) {
					case MBFL_WCSPLANE_JIS0208:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"JIS+");
						break;
					case MBFL_WCSPLANE_JIS0212:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"JIS2+");
						break;
					case MBFL_WCSPLANE_JIS0213:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"JIS3+");
						break;
					case MBFL_WCSPLANE_WINCP932:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"W932+");
						break;
					case MBFL_WCSPLANE_GB18030:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"GB+");
						break;
					case MBFL_WCSPLANE_8859_1:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"I8859_1+");
						break;
					default:
						ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"?+");
						break;
					}
					c &= MBFL_WCSPLANE_MASK;
				} else {
					ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"BAD+");
					c &= MBFL_WCSGROUP_MASK;
				}
			}
			if (ret >= 0) {
				m = 0;
				r = 28;
				while (r >= 0) {
					n = (c >> r) & 0xf;
					if (n || m) {
						m = 1;
						ret = (*filter->filter_function)(mbfl_hexchar_table[n], filter);
						if (ret < 0) {
							break;
						}
					}
					r -= 4;
				}
				if (m == 0 && ret >= 0) {
					ret = (*filter->filter_function)(mbfl_hexchar_table[0], filter);
				}
			}
		}
		break;
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY:
		if (c >= 0) {
			if (c < MBFL_WCSGROUP_UCS4MAX) {	/* unicode */
				ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)"&#x");
				if (ret < 0)
					break;

				m = 0;
				r = 28;
				while (r >= 0) {
					n = (c >> r) & 0xf;
					if (n || m) {
						m = 1;
						ret = (*filter->filter_function)(mbfl_hexchar_table[n], filter);
						if (ret < 0) {
							break;
						}
					}
					r -= 4;
				}
				if (ret < 0) {
					break;
				}
				if (m == 0) {
					ret = (*filter->filter_function)(mbfl_hexchar_table[0], filter);
				}
				ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)";");
			} else {
				ret = (*filter->filter_function)(filter->illegal_substchar, filter);
			}
		}
		break;
	default:
		break;
	}
	filter->illegal_mode = mode_backup;
	filter->num_illegalchar++;

	return ret;
}

const struct mbfl_convert_vtbl * mbfl_convert_filter_get_vtbl(enum mbfl_no_encoding from, enum mbfl_no_encoding to)
{
	const struct mbfl_convert_vtbl *vtbl;
	int i;

	if (to == mbfl_no_encoding_base64 ||
	    to == mbfl_no_encoding_qprint ||
	    to == mbfl_no_encoding_7bit) {
		from = mbfl_no_encoding_8bit;
	} else if (from == mbfl_no_encoding_base64 ||
			   from == mbfl_no_encoding_qprint ||
			   from == mbfl_no_encoding_uuencode) {
		to = mbfl_no_encoding_8bit;
	}

	i = 0;
	while ((vtbl = mbfl_convert_filter_list[i++]) != NULL){
		if (vtbl->from == from && vtbl->to == to) {
			return vtbl;
		}
	}

	return NULL;
}

/*
 * commonly used constructor and destructor
 */
void mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;
}

int mbfl_filt_conv_common_flush(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;

	if (filter->flush_function != NULL) {
		(*filter->flush_function)(filter->data);
	}
	return 0;
}

void mbfl_filt_conv_common_dtor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;
}


