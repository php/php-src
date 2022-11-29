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

#include "libmbfl/config.h"

#ifdef HAVE_STRINGS_H
	/* For strcasecmp */
	#include <strings.h>
#endif

#include "mbfl_encoding.h"
#include "mbfilter_pass.h"
#include "mbfilter_8bit.h"

#include "filters/mbfilter_euc_cn.h"
#include "filters/mbfilter_hz.h"
#include "filters/mbfilter_euc_tw.h"
#include "filters/mbfilter_big5.h"
#include "filters/mbfilter_uhc.h"
#include "filters/mbfilter_euc_kr.h"
#include "filters/mbfilter_iso2022_kr.h"
#include "filters/mbfilter_sjis.h"
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
#include "filters/mbfilter_cp932.h"
#include "filters/mbfilter_cp936.h"
#include "filters/mbfilter_cp5022x.h"
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
#include "filters/mbfilter_ucs4.h"
#include "filters/mbfilter_ucs2.h"
#include "filters/mbfilter_htmlent.h"
#include "filters/mbfilter_singlebyte.h"

#ifndef HAVE_STRCASECMP
#ifdef HAVE_STRICMP
#define strcasecmp stricmp
#endif
#endif


static const mbfl_encoding *mbfl_encoding_ptr_list[] = {
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
	&mbfl_encoding_sjiswin,
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
	&mbfl_encoding_2022jp_2004,
	&mbfl_encoding_2022jp_kddi,
	&mbfl_encoding_cp50220,
	&mbfl_encoding_cp50221,
	&mbfl_encoding_cp50222,
	NULL
};

const mbfl_encoding *mbfl_name2encoding(const char *name)
{
	const mbfl_encoding **encoding;

	for (encoding = mbfl_encoding_ptr_list; *encoding; encoding++) {
		if (strcasecmp((*encoding)->name, name) == 0) {
			return *encoding;
		}
	}

	/* search MIME charset name */
	for (encoding = mbfl_encoding_ptr_list; *encoding; encoding++) {
		if ((*encoding)->mime_name) {
			if (strcasecmp((*encoding)->mime_name, name) == 0) {
				return *encoding;
			}
		}
	}

	/* search aliases */
	for (encoding = mbfl_encoding_ptr_list; *encoding; encoding++) {
		if ((*encoding)->aliases) {
			for (const char **alias = (*encoding)->aliases; *alias; alias++) {
				if (strcasecmp(*alias, name) == 0) {
					return *encoding;
				}
			}
		}
	}

	return NULL;
}

const mbfl_encoding *mbfl_no2encoding(enum mbfl_no_encoding no_encoding)
{
	const mbfl_encoding **encoding;

	for (encoding = mbfl_encoding_ptr_list; *encoding; encoding++) {
		if ((*encoding)->no_encoding == no_encoding) {
			return *encoding;
		}
	}

	return NULL;
}

enum mbfl_no_encoding mbfl_name2no_encoding(const char *name)
{
	const mbfl_encoding *encoding = mbfl_name2encoding(name);
	return encoding ? encoding->no_encoding : mbfl_no_encoding_invalid;
}

const char *mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding)
{
	const mbfl_encoding *encoding = mbfl_no2encoding(no_encoding);
	return encoding ? encoding->name : "";
}

const mbfl_encoding **mbfl_get_supported_encodings(void)
{
	return mbfl_encoding_ptr_list;
}

const char *mbfl_no2preferred_mime_name(enum mbfl_no_encoding no_encoding)
{
	return mbfl_encoding_preferred_mime_name(mbfl_no2encoding(no_encoding));
}

const char *mbfl_encoding_preferred_mime_name(const mbfl_encoding *encoding)
{
	if (encoding->mime_name && encoding->mime_name[0] != '\0') {
		return encoding->mime_name;
	}
	return NULL;
}
