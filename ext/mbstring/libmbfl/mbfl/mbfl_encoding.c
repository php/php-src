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

#include <stddef.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "mbfl_encoding.h"
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
#include "filters/mbfilter_sjis_mobile.h"
#include "filters/mbfilter_sjis_mac.h"
#include "filters/mbfilter_sjis_2004.h"
#include "filters/mbfilter_cp51932.h"
#include "filters/mbfilter_jis.h"
#include "filters/mbfilter_iso2022_jp_ms.h"
#include "filters/mbfilter_iso2022jp_2004.h"
#include "filters/mbfilter_iso2022jp_mobile.h"
#include "filters/mbfilter_euc_jp.h"
#include "filters/mbfilter_euc_jp_win.h"
#include "filters/mbfilter_euc_jp_2004.h"
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
#include "filters/mbfilter_iso8859_16.h"
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

#ifndef HAVE_STRCASECMP
#ifdef HAVE_STRICMP
#define strcasecmp stricmp
#endif
#endif


static const mbfl_encoding *mbfl_encoding_ptr_list[] = {
	&mbfl_encoding_pass,
	&mbfl_encoding_wchar,
	&mbfl_encoding_byte2be,
	&mbfl_encoding_byte2le,
	&mbfl_encoding_byte4be,
	&mbfl_encoding_byte4le,
	&mbfl_encoding_base64,
	&mbfl_encoding_uuencode,
	&mbfl_encoding_html_ent,
	&mbfl_encoding_qprint,
	&mbfl_encoding_7bit,
	&mbfl_encoding_8bit,
	&mbfl_encoding_ucs4,
	&mbfl_encoding_ucs4be,
	&mbfl_encoding_ucs4le,
	&mbfl_encoding_ucs2,
	&mbfl_encoding_ucs2be,
	&mbfl_encoding_ucs2le,
	&mbfl_encoding_utf32,
	&mbfl_encoding_utf32be,
	&mbfl_encoding_utf32le,
	&mbfl_encoding_utf16,
	&mbfl_encoding_utf16be,
	&mbfl_encoding_utf16le,
	&mbfl_encoding_utf8,
	&mbfl_encoding_utf7,
	&mbfl_encoding_utf7imap,
	&mbfl_encoding_ascii,
	&mbfl_encoding_euc_jp,
	&mbfl_encoding_sjis,
	&mbfl_encoding_eucjp_win,
	&mbfl_encoding_eucjp2004,
	&mbfl_encoding_sjis_open,
 	&mbfl_encoding_sjis_docomo,
 	&mbfl_encoding_sjis_kddi,
 	&mbfl_encoding_sjis_sb,
 	&mbfl_encoding_sjis_mac,
	&mbfl_encoding_sjis2004,
	&mbfl_encoding_utf8_docomo,
	&mbfl_encoding_utf8_kddi_a,
	&mbfl_encoding_utf8_kddi_b,
	&mbfl_encoding_utf8_sb,
	&mbfl_encoding_cp932,
	&mbfl_encoding_cp51932,
	&mbfl_encoding_jis,
	&mbfl_encoding_2022jp,
	&mbfl_encoding_2022jpms,
	&mbfl_encoding_gb18030,
	&mbfl_encoding_cp1252,
	&mbfl_encoding_cp1254,
	&mbfl_encoding_8859_1,
	&mbfl_encoding_8859_2,
	&mbfl_encoding_8859_3,
	&mbfl_encoding_8859_4,
	&mbfl_encoding_8859_5,
	&mbfl_encoding_8859_6,
	&mbfl_encoding_8859_7,
	&mbfl_encoding_8859_8,
	&mbfl_encoding_8859_9,
	&mbfl_encoding_8859_10,
	&mbfl_encoding_8859_13,
	&mbfl_encoding_8859_14,
	&mbfl_encoding_8859_15,
	&mbfl_encoding_8859_16,
	&mbfl_encoding_euc_cn,
	&mbfl_encoding_cp936,
	&mbfl_encoding_hz,
	&mbfl_encoding_euc_tw,
	&mbfl_encoding_big5,
	&mbfl_encoding_cp950,
	&mbfl_encoding_euc_kr,
	&mbfl_encoding_uhc,
	&mbfl_encoding_2022kr,
	&mbfl_encoding_cp1251,
	&mbfl_encoding_cp866,
	&mbfl_encoding_koi8r,
	&mbfl_encoding_koi8u,
	&mbfl_encoding_armscii8,
	&mbfl_encoding_cp850,
	&mbfl_encoding_jis_ms,
	&mbfl_encoding_2022jp_2004,
	&mbfl_encoding_2022jp_kddi,
	&mbfl_encoding_cp50220,
	&mbfl_encoding_cp50220raw,
	&mbfl_encoding_cp50221,
	&mbfl_encoding_cp50222,
	NULL
};

/* encoding resolver */
const mbfl_encoding *
mbfl_name2encoding(const char *name)
{
	const mbfl_encoding *encoding;
	int i, j;

	if (name == NULL) {
		return NULL;
	}

 	i = 0;
 	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL){
		if (strcasecmp(encoding->name, name) == 0) {
			return encoding;
		}
	}

 	/* serch MIME charset name */
 	i = 0;
 	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL) {
		if (encoding->mime_name != NULL) {
			if (strcasecmp(encoding->mime_name, name) == 0) {
				return encoding;
			}
		}
	}

 	/* serch aliases */
 	i = 0;
 	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL) {
		if (encoding->aliases != NULL) {
 			j = 0;
 			while ((*encoding->aliases)[j] != NULL) {
				if (strcasecmp((*encoding->aliases)[j], name) == 0) {
					return encoding;
				}
				j++;
			}
		}
	}

	return NULL;
}

const mbfl_encoding *
mbfl_no2encoding(enum mbfl_no_encoding no_encoding)
{
	const mbfl_encoding *encoding;
	int i;

	i = 0;
	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL){
		if (encoding->no_encoding == no_encoding) {
			return encoding;
		}
	}

	return NULL;
}

enum mbfl_no_encoding
mbfl_name2no_encoding(const char *name)
{
	const mbfl_encoding *encoding;

	encoding = mbfl_name2encoding(name);
	if (encoding == NULL) {
		return mbfl_no_encoding_invalid;
	} else {
		return encoding->no_encoding;
	}
}

const char *
mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding)
{
	const mbfl_encoding *encoding;

	encoding = mbfl_no2encoding(no_encoding);
	if (encoding == NULL) {
		return "";
	} else {
		return encoding->name;
	}
}

const mbfl_encoding **
mbfl_get_supported_encodings(void)
{
	return mbfl_encoding_ptr_list;
}

const char *
mbfl_no2preferred_mime_name(enum mbfl_no_encoding no_encoding)
{
	const mbfl_encoding *encoding;

	encoding = mbfl_no2encoding(no_encoding);
	if (encoding != NULL && encoding->mime_name != NULL && encoding->mime_name[0] != '\0') {
		return encoding->mime_name;
	} else {
		return NULL;
	}
}

int
mbfl_is_support_encoding(const char *name)
{
	const mbfl_encoding *encoding;

	encoding = mbfl_name2encoding(name);
	if (encoding == NULL) {
		return 0;
	} else {
		return 1;
	}
}
