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
 * The source code included in this files was separated from mbfilter_jis.c
 * by rui hirokawa <hirokawa@php.net> on 18 aug 2011.
 *
 */

#include "mbfilter.h"
#include "mbfilter_iso2022jp_2004.h"
#include "mbfilter_sjis_2004.h"

#include "unicode_table_jis.h"
#include "unicode_table_jis2004.h"

extern void mbfl_filt_conv_any_jis_flush(mbfl_convert_filter *filter);
static void mbfl_filt_ident_2022jp_2004(unsigned char c, mbfl_identify_filter *filter);

const mbfl_encoding mbfl_encoding_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	"ISO-2022-JP-2004",
	"ISO-2022-JP-2004",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_2004_wchar,
	&vtbl_wchar_2022jp_2004
};

const struct mbfl_identify_vtbl vtbl_identify_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_2022jp_2004
};

const struct mbfl_convert_vtbl vtbl_2022jp_2004_wchar = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_jis2004_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jp_2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush
};

static void mbfl_filt_ident_2022jp_2004_0208(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_2022jp_2004_0213_1(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_2022jp_2004_0213_2(unsigned char c, mbfl_identify_filter *filter);

/* ISO 2022-JP-2004 has different modes, which can be selected by a sequence
 * starting with ESC (0x1B). In each mode, characters can be selected from a
 * different character set.
 *
 * Handle these escape sequences: */
static int handle_esc_sequence(int c, mbfl_identify_filter *filter)
{
	/* If we are on the 2nd byte of a 2-byte character, `filter->status` will
	 * be `first_byte << 8` */

	switch (filter->status) {
	case 0: /* Starting new character */
		if (c == 0x1B) { /* ESC */
			filter->status = 1;
			return 1;
		}
		break;

	case 1: /* Already saw ESC */
		if (c == '$') {
			filter->status = 2;
		} else if (c == '(') {
			filter->status = 3;
		} else {
			filter->flag = 1;
			filter->status = 0;
		}
		return 1;

	case 2: /* Already saw ESC $ */
		/* According to the ISO standard, ESC $ @ should switch to the
		 * JIS X 0208-1978 character set, but that is not implemented */
		if (c == 'B') {
			/* Switch to JIS X 0208 */
			filter->filter_function = mbfl_filt_ident_2022jp_2004_0208;
			filter->status = 0;
		} else if (c == '(') {
			filter->status = 4;
		} else {
			filter->flag = 1;
			filter->status = 0;
		}
		return 1;

	case 3: /* Already saw ESC ( */
		/* According to the ISO standard, ESC ( J should switch to
		 * the JIS X 0201 character set, but that is not implemented.
		 * Additionally, ISO-2022-JP-2004 includes another added escape sequence
		 * ESC ( I, which should switch to the JIS X 0201-1976 Kana set */
		if (c == 'B') {
			/* Switch to ASCII */
			filter->filter_function = mbfl_filt_ident_2022jp_2004;
		} else {
			filter->flag = 1;
		}
		filter->status = 0;
		return 1;

	case 4: /* Already saw ESC $ ( */
		/* ISO-2022-JP-2004 also includes another escape sequence: ESC ( $ O,
		 * which should switch to the JIS X 0213-2000 plane 1 character set,
		 * but that is not implemented */
		if (c == 'Q') {
			/* Switch to JIS X 0213-2004 plane 1 */
			filter->filter_function = mbfl_filt_ident_2022jp_2004_0213_1;
		} else if (c == 'P') {
			/* Switch to JIS X 0213-2000 plane 2 */
			filter->filter_function = mbfl_filt_ident_2022jp_2004_0213_2;
		} else {
			filter->flag = 1;
		}
		filter->status = 0;
		return 1;
	}

	return 0;
}

#undef IN
#define IN(c,lo,hi) ((c) >= lo && (c) <= hi)

/* Not all byte sequences in JIS X 0208 which would otherwise be valid are
 * actually mapped to a character */
static inline int in_unused_jisx0208_range(int c1, int c2)
{
	unsigned int s = (c1 - 0x21)*94 + c2 - 0x21;
	return s >= jisx0208_ucs_table_size || !jisx0208_ucs_table[s];
}

/* In JIS X 0208 mode */
static void mbfl_filt_ident_2022jp_2004_0208(unsigned char c, mbfl_identify_filter *filter)
{
	if (!handle_esc_sequence(c, filter)) {
		if (filter->status == 0) {
			if (c >= 0x21 && c <= 0x74) {
				/* First byte of a 2-byte character */
				filter->status = c << 8;
			} else if (c > 0x74 && c != 0x7F) {
				/* In JISX 0208, single bytes from 0x0-0x1F and 0x7F represent
				 * control characters, 0x20 is space, others are unmapped */
				filter->flag = 1;
			}
		} else if (c < 0x21 || c > 0x7E || in_unused_jisx0208_range(filter->status >> 8, c)) {
			filter->flag = 1; /* Illegal 2nd byte of a 2-byte character */
		} else {
			filter->status = 0; /* Passed by 2-byte character, starting a new one */
		}
	}
}

static inline int is_reserved_jisx0213_plane1_range(int c1, int c2)
{
	unsigned int s = (c1 - 0x21)*94 + c2 - 0x21;
	return s >= jisx0213_ucs_table_size || !jisx0213_ucs_table[s];
}

/* In JIS X 0213:2004 plane 1 */
static void mbfl_filt_ident_2022jp_2004_0213_1(unsigned char c, mbfl_identify_filter *filter)
{
	if (!handle_esc_sequence(c, filter)) {
		if (filter->status == 0) {
			if (c >= 0x21 && c <= 0x7E) {
				filter->status = c << 8;
			} else if (c > 0x7F) {
				filter->flag = 1;
			}
		} else if (c < 0x21 || c > 0x7E || is_reserved_jisx0213_plane1_range(filter->status >> 8, c)) {
			filter->flag = 1; /* Illegal 2nd byte of a 2-byte character */
		} else {
			filter->status = 0;
		}
	}
}

static inline int is_reserved_jisx0213_plane2_range(int c1, int c2)
{
	unsigned int s = (c1 - 0x21)*94 + (94*94) + c2 - 0x21;
	return s >= jisx0213_ucs_table_size || !jisx0213_ucs_table[s];
}

/* In JIS X 0213:2000 plane 2 */
static void mbfl_filt_ident_2022jp_2004_0213_2(unsigned char c, mbfl_identify_filter *filter)
{
	if (!handle_esc_sequence(c, filter)) {
		if (filter->status == 0) {
			if (c >= 0x21 && c <= 0x7E) {
				filter->status = c << 8;
			} else if (c > 0x7F) {
				filter->flag = 1;
			}
		} else if (c < 0x21 || c > 0x7E || is_reserved_jisx0213_plane2_range(filter->status >> 8, c)) {
			filter->flag = 1; /* Illegal 2nd byte of a 2-byte character */
		} else {
			filter->status = 0;
		}
	}
}

/* In ASCII mode */
static void mbfl_filt_ident_2022jp_2004(unsigned char c, mbfl_identify_filter *filter)
{
	if (!handle_esc_sequence(c, filter)) {
		if (c > 0x7F) { /* non-ASCII */
			filter->flag = 1;
		}
	}
}
