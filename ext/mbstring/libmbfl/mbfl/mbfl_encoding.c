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

#include "filters/mbfilter_base64.h"
#include "filters/mbfilter_cjk.h"
#include "filters/mbfilter_qprint.h"
#include "filters/mbfilter_uuencode.h"
#include "filters/mbfilter_7bit.h"
#include "filters/mbfilter_utf7.h"
#include "filters/mbfilter_utf7imap.h"
#include "filters/mbfilter_utf8.h"
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
	&mbfl_encoding_gb18030_2022,
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

/* The following perfect hashing table was amended from gperf, and hashing code was generated using gperf.
 * The table was amended to refer to the table above such that it is lighter for the data cache.
 * Command used: gperf encodings.txt --readonly-tables --null-strings --ignore-case
 * The encodings.txt contains all the contents of the name fields of the mbfl_encoding_ptr_list table. */

static const int8_t mbfl_encoding_ptr_list_after_hashing[231] = {
	-1, -1,
	61,
	66,
	23,
	73,
	59,
	-1,
	1,
	-1, -1, -1,
	11,
	-1,
	5,
	9,
	-1,
	10,
	38,
	-1,
	52,
	54,
	-1,
	2,
	40,
	46,
	27,
	76,
	26,
	-1,
	49,
	57,
	-1,
	75,
	-1,
	47,
	55,
	78,
	36,
	-1,
	50,
	58,
	8,
	-1, -1,
	69,
	39,
	7,
	-1, -1,
	64,
	67,
	-1, -1,
	30,
	48,
	56,
	-1, -1, -1,
	35,
	74,
	-1, -1,
	24,
	53,
	62,
	43,
	-1, -1,
	45,
	22,
	-1, -1, -1,
	6,
	3,
	-1, -1, -1,
	18,
	71,
	-1, -1, -1,
	21,
	-1,
	37,
	-1,
	4,
	60,
	25,
	-1, -1,
	72,
	51,
	-1,
	44,
	29,
	-1,
	28,
	0,
	-1,
	14,
	31,
	63,
	12,
	-1,
	13,
	33,
	-1, -1,
	68,
	-1, -1, -1, -1,
	-1, -1,
	20,
	-1, -1, -1, -1,
	-1, -1, -1,
	77,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	65,
	-1, -1, -1, -1,
	70,
	-1, -1, -1, -1,
	-1,
	41,
	-1, -1, -1, -1,
	-1,
	17,
	-1, -1, -1,
	42,
	16,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	15,
	-1, -1, -1, -1,
	34,
	-1, -1, -1, -1,
	-1, -1, -1,
	32,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1,
	19
};

static unsigned int mbfl_name2encoding_perfect_hash(const char *str, size_t len)
{
	static const unsigned char asso_values[] =
	{
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231,   5, 231, 231,   0,  50,
			5,  15,  35,  10,  20,  75,   0,  45, 231, 231,
		231, 231, 231, 231, 231,  80,   5,   0,   0,   0,
		 75,  75,   0,   0,  15,  70,   0,   5,   0,   0,
		 25,  55,  30,   0,  10,   0, 231,  25, 231, 231,
			0, 231, 231, 231, 231, 231, 231,  80,   5,   0,
			0,   0,  75,  75,   0,   0,  15,  70,   0,   5,
			0,   0,  25,  55,  30,   0,  10,   0, 231,  25,
		231, 231,   0, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231, 231, 231, 231, 231,
		231, 231, 231, 231, 231, 231
	};
	unsigned int hval = len;

	switch (hval)
	{
		default:
			hval += asso_values[(unsigned char)str[6]];
			ZEND_FALLTHROUGH;
		case 6:
			hval += asso_values[(unsigned char)str[5]];
			ZEND_FALLTHROUGH;
		case 5:
			hval += asso_values[(unsigned char)str[4]];
			ZEND_FALLTHROUGH;
		case 4:
		case 3:
			hval += asso_values[(unsigned char)str[2]];
			ZEND_FALLTHROUGH;
		case 2:
		case 1:
			hval += asso_values[(unsigned char)str[0]];
			break;
	}
	return hval + asso_values[(unsigned char)str[len - 1]];
}

#define NAME_HASH_MIN_NAME_LENGTH 2
#define NAME_HASH_MAX_NAME_LENGTH 23

const mbfl_encoding *mbfl_name2encoding(const char *name)
{
	return mbfl_name2encoding_ex(name, strlen(name));
}

const mbfl_encoding *mbfl_name2encoding_ex(const char *name, size_t name_len)
{
	const mbfl_encoding *const *encoding;

	/* Sanity check perfect hash for name.
	 * Never enable this in production, this is only a development-time sanity check! */
#if ZEND_DEBUG && 0
	for (encoding = mbfl_encoding_ptr_list; *encoding; encoding++) {
		size_t name_length = strlen((*encoding)->name);
		if (!(name_length <= NAME_HASH_MAX_NAME_LENGTH && name_length >= NAME_HASH_MIN_NAME_LENGTH)) {
			fprintf(stderr, "name length is not satisfying bound check: %zu %s\n", name_length, (*encoding)->name);
			abort();
		}
		unsigned int key = mbfl_name2encoding_perfect_hash((*encoding)->name, name_length);
		if (mbfl_encoding_ptr_list[mbfl_encoding_ptr_list_after_hashing[key]] != *encoding) {
			fprintf(stderr, "mbfl_name2encoding_perfect_hash: key %u %s mismatch\n", key, (*encoding)->name);
			abort();
		}
	}
#endif

	/* Use perfect hash lookup for name */
	if (name_len <= NAME_HASH_MAX_NAME_LENGTH && name_len >= NAME_HASH_MIN_NAME_LENGTH) {
		unsigned int key = mbfl_name2encoding_perfect_hash(name, name_len);
		if (key <= sizeof(mbfl_encoding_ptr_list_after_hashing)) {
			int8_t offset = mbfl_encoding_ptr_list_after_hashing[key];
			if (offset >= 0) {
				encoding = mbfl_encoding_ptr_list + offset;
				if (strncasecmp((*encoding)->name, name, name_len) == 0) {
					return *encoding;
				}
			}
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

const char *mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding)
{
	const mbfl_encoding *encoding = mbfl_no2encoding(no_encoding);
	return encoding ? encoding->name : "";
}

const mbfl_encoding **mbfl_get_supported_encodings(void)
{
	return mbfl_encoding_ptr_list;
}

const char *mbfl_encoding_preferred_mime_name(const mbfl_encoding *encoding)
{
	if (encoding->mime_name && encoding->mime_name[0] != '\0') {
		return encoding->mime_name;
	}
	return NULL;
}
