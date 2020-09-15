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

#include <stddef.h>

#include "mbfl_encoding.h"
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

static const struct mbfl_convert_vtbl *mbfl_special_filter_list[] = {
	&vtbl_8bit_b64,
	&vtbl_b64_8bit,
	&vtbl_uuencode_8bit,
	&vtbl_8bit_qprint,
	&vtbl_qprint_8bit,
	&vtbl_8bit_7bit,
	&vtbl_7bit_8bit,
	&vtbl_pass,
	NULL
};

static void mbfl_convert_filter_common_init(mbfl_convert_filter *filter, const mbfl_encoding *from, const mbfl_encoding *to,
	const struct mbfl_convert_vtbl *vtbl, output_function_t output_function, flush_function_t flush_function, void* data)
{
	/* encoding structure */
	filter->from = from;
	filter->to = to;

	if (output_function != NULL) {
		filter->output_function = output_function;
	} else {
		filter->output_function = mbfl_filter_output_null;
	}

	filter->flush_function = flush_function;
	filter->data = data;
	filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	filter->illegal_substchar = '?';
	filter->num_illegalchar = 0;
	filter->filter_ctor = vtbl->filter_ctor;
	filter->filter_dtor = vtbl->filter_dtor;
	filter->filter_function = vtbl->filter_function;
	filter->filter_flush = vtbl->filter_flush;
	filter->filter_copy = vtbl->filter_copy;

	(*filter->filter_ctor)(filter);
}


mbfl_convert_filter* mbfl_convert_filter_new(const mbfl_encoding *from, const mbfl_encoding *to, output_function_t output_function,
	flush_function_t flush_function, void* data)
{
	const struct mbfl_convert_vtbl *vtbl = mbfl_convert_filter_get_vtbl(from, to);
	if (vtbl == NULL) {
		return NULL;
	}

	mbfl_convert_filter *filter = emalloc(sizeof(mbfl_convert_filter));
	mbfl_convert_filter_common_init(filter, from, to, vtbl, output_function, flush_function, data);
	return filter;
}

mbfl_convert_filter* mbfl_convert_filter_new2(const struct mbfl_convert_vtbl *vtbl, output_function_t output_function,
	flush_function_t flush_function, void* data)
{
	const mbfl_encoding *from_encoding = mbfl_no2encoding(vtbl->from);
	const mbfl_encoding *to_encoding = mbfl_no2encoding(vtbl->to);

	mbfl_convert_filter *filter = emalloc(sizeof(mbfl_convert_filter));
	mbfl_convert_filter_common_init(filter, from_encoding, to_encoding, vtbl, output_function, flush_function, data);
	return filter;
}

void mbfl_convert_filter_delete(mbfl_convert_filter *filter)
{
	if (filter->filter_dtor) {
		(*filter->filter_dtor)(filter);
	}
	efree(filter);
}

/* Feed a char, return 0 if ok - used by mailparse ext */
int mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter)
{
	return (*filter->filter_function)(c, filter);
}

/* Feed string into `filter` byte by byte; return pointer to first byte not processed */
unsigned char* mbfl_convert_filter_feed_string(mbfl_convert_filter *filter, unsigned char *p, size_t len)
{
	while (len--) {
		if ((*filter->filter_function)(*p++, filter) < 0) {
			break;
		}
	}
	return p;
}

int mbfl_convert_filter_flush(mbfl_convert_filter *filter)
{
	(*filter->filter_flush)(filter);
	return filter->flush_function ? (*filter->flush_function)(filter->data) : 0;
}

void mbfl_convert_filter_reset(mbfl_convert_filter *filter, const mbfl_encoding *from, const mbfl_encoding *to)
{
	if (filter->filter_dtor) {
		(*filter->filter_dtor)(filter);
	}

	const struct mbfl_convert_vtbl *vtbl = mbfl_convert_filter_get_vtbl(from, to);

	if (vtbl == NULL) {
		vtbl = &vtbl_pass;
	}

	mbfl_convert_filter_common_init(filter, from, to, vtbl,
			filter->output_function, filter->flush_function, filter->data);
}

void mbfl_convert_filter_copy(mbfl_convert_filter *src, mbfl_convert_filter *dest)
{
	if (src->filter_copy != NULL) {
		src->filter_copy(src, dest);
		return;
	}

	*dest = *src;
}

void mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src)
{
	mbfl_convert_filter_feed_string(filter, src->buffer, src->pos);
}

int mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p)
{
	int c;
	while ((c = *p++)) {
		if ((*filter->filter_function)(c, filter) < 0) {
			return -1;
		}
	}

	return 0;
}

/* illegal character output function for conv-filter */
int mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter)
{
	int n, m, r;

	int ret = 0;
	int mode_backup = filter->illegal_mode;
	int substchar_backup = filter->illegal_substchar;

	/* The used substitution character may not be supported by the target character encoding.
	 * If that happens, first try to use "?" instead and if that also fails, silently drop the
	 * character. */
	if (filter->illegal_mode == MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR
			&& filter->illegal_substchar != '?') {
		filter->illegal_substchar = '?';
	} else {
		filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
	}

	switch (mode_backup) {
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR:
		ret = (*filter->filter_function)(substchar_backup, filter);
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
				if (m == 0) {
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
				if (m == 0) {
					/* illegal character was zero; no hex digits were output by above loop */
					ret = (*filter->filter_function)('0', filter);
				}
				ret = mbfl_convert_filter_strcat(filter, (const unsigned char *)";");
			} else {
				ret = (*filter->filter_function)(substchar_backup, filter);
			}
		}
		break;
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE:
	default:
		break;
	}

	filter->illegal_mode = mode_backup;
	filter->illegal_substchar = substchar_backup;
	filter->num_illegalchar++;

	return ret;
}

const struct mbfl_convert_vtbl* mbfl_convert_filter_get_vtbl(const mbfl_encoding *from, const mbfl_encoding *to)
{
	if (to->no_encoding == mbfl_no_encoding_base64 ||
	    to->no_encoding == mbfl_no_encoding_qprint ||
	    to->no_encoding == mbfl_no_encoding_7bit) {
		from = &mbfl_encoding_8bit;
	} else if (from->no_encoding == mbfl_no_encoding_base64 ||
			   from->no_encoding == mbfl_no_encoding_qprint ||
			   from->no_encoding == mbfl_no_encoding_uuencode) {
		to = &mbfl_encoding_8bit;
	}

	if (to == from && (to == &mbfl_encoding_wchar || to == &mbfl_encoding_8bit)) {
		return &vtbl_pass;
	}

	if (to->no_encoding == mbfl_no_encoding_wchar) {
		return from->input_filter;
	} else if (from->no_encoding == mbfl_no_encoding_wchar) {
		return to->output_filter;
	} else {
		int i = 0;
		const struct mbfl_convert_vtbl *vtbl;
		while ((vtbl = mbfl_special_filter_list[i++])) {
			if (vtbl->from == from->no_encoding && vtbl->to == to->no_encoding) {
				return vtbl;
			}
		}
		return NULL;
	}
}

/*
 * commonly used constructor
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
