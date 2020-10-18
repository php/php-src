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
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_utf7.h"

static void mbfl_filt_conv_utf7_wchar_flush(mbfl_convert_filter *filter);
static void mbfl_filt_ident_utf7(unsigned char c, mbfl_identify_filter *filter);

static const unsigned char mbfl_base64_table[] = {
 /* 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', */
   0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,
 /* 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
   0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
 /* 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', */
   0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
 /* 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
   0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
 /* '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0' */
   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2b,0x2f,0x00
};

static const char *mbfl_encoding_utf7_aliases[] = {"utf7", NULL};

const mbfl_encoding mbfl_encoding_utf7 = {
	mbfl_no_encoding_utf7,
	"UTF-7",
	"UTF-7",
	mbfl_encoding_utf7_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_utf7_wchar,
	&vtbl_wchar_utf7
};

const struct mbfl_identify_vtbl vtbl_identify_utf7 = {
	mbfl_no_encoding_utf7,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_utf7
};

const struct mbfl_convert_vtbl vtbl_utf7_wchar = {
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf7_wchar,
	mbfl_filt_conv_utf7_wchar_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf7 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf7,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf7,
	mbfl_filt_conv_wchar_utf7_flush
};

static unsigned int decode_base64_char(unsigned char c)
{
	if (c >= 'A' && c <= 'Z') {
		return c - 65;
	} else if (c >= 'a' && c <= 'z') {
		return c - 71;
	} else if (c >= '0' && c <= '9') {
		return c + 4;
	} else if (c == '+') {
		return 62;
	} else if (c == '/') {
		return 63;
	}
	return -1;
}

void mbfl_filt_conv_utf7_wchar(int c, mbfl_convert_filter *filter)
{
	int s, n = -1;

	if (filter->status) { /* Modified Base64 */
		n = decode_base64_char(c);
		if (n < 0) {
			if (filter->cache) {
				/* Either we were expecting the 2nd half of a surrogate pair which
				 * never came, or else the last Base64 data was not padded with zeroes */
				(*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			if (c == '-') {
				if (filter->status == 1) { /* "+-" -> "+" */
					(*filter->output_function)('+', filter->data);
				}
			} else if (c >= 0 && c < 0x80) { /* ASCII exclude '-' */
				(*filter->output_function)(c, filter->data);
			} else { /* illegal character */
				s = c & MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				(*filter->output_function)(s, filter->data);
			}
			filter->cache = filter->status = 0;
		}
	}

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == '+') { /* '+' shift character */
			filter->status = 1;
		} else if (c >= 0 && c < 0x80) { /* ASCII */
			(*filter->output_function)(c, filter->data);
		} else { /* illegal character */
			(*filter->output_function)((c & MBFL_WCSGROUP_MASK) | MBFL_WCSGROUP_THROUGH, filter->data);
		}
		break;

	/* decode Modified Base64 */
	case 1:
	case 2:
		filter->cache |= n << 10;
		filter->status = 3;
		break;
	case 3:
		filter->cache |= n << 4;
		filter->status = 4;
		break;
	case 4:
		s = ((n >> 2) & 0xf) | (filter->cache & 0xffff);
		n = (n & 0x3) << 14;
		filter->status = 5;
		if (s >= 0xd800 && s < 0xdc00) {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(((filter->cache & 0xfff0000) >> 6) | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				(*filter->output_function)(s, filter->data);
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				(*filter->output_function)(s, filter->data);
			}
		} else {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(((filter->cache & 0xfff0000) >> 6) | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			filter->cache = n;
			(*filter->output_function)(s, filter->data);
		}
		break;

	case 5:
		filter->cache |= n << 8;
		filter->status = 6;
		break;
	case 6:
		filter->cache |= n << 2;
		filter->status = 7;
		break;
	case 7:
		s = ((n >> 4) & 0x3) | (filter->cache & 0xffff);
		n = (n & 0xf) << 12;
		filter->status = 8;
		if (s >= 0xd800 && s < 0xdc00) {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(((filter->cache & 0xfff0000) >> 6) | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				(*filter->output_function)(s, filter->data);
			} else { /* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				(*filter->output_function)(s, filter->data);
			}
		} else {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(((filter->cache & 0xfff0000) >> 6) | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			filter->cache = n;
			(*filter->output_function)(s, filter->data);
		}
		break;

	case 8:
		filter->cache |= n << 6;
		filter->status = 9;
		break;
	case 9:
		s = n | (filter->cache & 0xffff);
		filter->status = 2;
		if (s >= 0xd800 && s < 0xdc00) {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(((filter->cache & 0xfff0000) >> 6) | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			s = (((s & 0x3ff) << 16) + 0x400000);
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				(*filter->output_function)(s, filter->data);
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				(*filter->output_function)(s, filter->data);
			}
		} else {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(((filter->cache & 0xfff0000) >> 6) | MBFL_WCSGROUP_THROUGH, filter->data);
			}
			filter->cache = 0;
			(*filter->output_function)(s, filter->data);
		}
		break;

	default:
		filter->status = 0;
		break;
	}
}

static void mbfl_filt_conv_utf7_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->cache) {
		/* Either we were expecting the 2nd half of a surrogate pair which
		 * never came, or else the last Base64 data was not padded with zeroes */
		(*filter->output_function)(filter->cache | MBFL_WCSGROUP_THROUGH, filter->data);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}
}

/*
 * wchar => UTF-7
 */
void mbfl_filt_conv_wchar_utf7(int c, mbfl_convert_filter *filter)
{
	int s;

	int n = 0;
	if (c >= 0 && c < 0x80) {	/* ASCII */
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = 1;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = 1;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = 1;
		} else if (c == '\0') {			/* '\0' */
			n = 1;
		} else if (c == 0x2f) {			/* '/' */
			n = 1;
		} else if (c == 0x2d) {			/* '-' */
			n = 1;
		} else if (c == 0x20) {			/* SPACE */
			n = 2;
		} else if (c == 0x09) {			/* HTAB */
			n = 2;
		} else if (c == 0x0d) {			/* CR */
			n = 2;
		} else if (c == 0x0a) {			/* LF */
			n = 2;
		} else if (c == 0x27) {			/* "'" */
			n = 2;
		} else if (c == 0x28) {			/* '(' */
			n = 2;
		} else if (c == 0x29) {			/* ')' */
			n = 2;
		} else if (c == 0x2c) {			/* ',' */
			n = 2;
		} else if (c == 0x2e) {			/* '.' */
			n = 2;
		} else if (c == 0x3a) {			/* ':' */
			n = 2;
		} else if (c == 0x3f) {			/* '?' */
			n = 2;
		}
	} else if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		;
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		(*filter->filter_function)(((c >> 10) - 0x40) | 0xd800, filter);
		(*filter->filter_function)((c & 0x3ff) | 0xdc00, filter);
		return;
	} else {
		mbfl_filt_conv_illegal_output(c, filter);
		return;
	}

	switch (filter->status) {
	case 0:
		if (n != 0) { /* directly encode characters */
			(*filter->output_function)(c, filter->data);
		} else { /* Modified Base64 */
			(*filter->output_function)('+', filter->data);
			filter->status = 1;
			filter->cache = c;
		}
		break;

	/* encode Modified Base64 */
	case 1:
		s = filter->cache;
		(*filter->output_function)(mbfl_base64_table[(s >> 10) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(s >> 4) & 0x3f], filter->data);
		if (n != 0) {
			(*filter->output_function)(mbfl_base64_table[(s << 2) & 0x3c], filter->data);
			if (n == 1) {
				(*filter->output_function)('-', filter->data);
			}
			(*filter->output_function)(c, filter->data);
			filter->status = 0;
		} else {
			filter->status = 2;
			filter->cache = ((s & 0xf) << 16) | c;
		}
		break;

	case 2:
		s = filter->cache;
		(*filter->output_function)(mbfl_base64_table[(s >> 14) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(s >> 8) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(s >> 2) & 0x3f], filter->data);
		if (n != 0) {
			(*filter->output_function)(mbfl_base64_table[(s << 4) & 0x30], filter->data);
			if (n == 1) {
				(*filter->output_function)('-', filter->data);
			}
			(*filter->output_function)(c, filter->data);
			filter->status = 0;
		} else {
			filter->status = 3;
			filter->cache = ((s & 0x3) << 16) | c;
		}
		break;

	case 3:
		s = filter->cache;
		(*filter->output_function)(mbfl_base64_table[(s >> 12) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(s >> 6) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[s & 0x3f], filter->data);
		if (n != 0) {
			if (n == 1) {
				(*filter->output_function)('-', filter->data);
			}
			(*filter->output_function)(c, filter->data);
			filter->status = 0;
		} else {
			filter->status = 1;
			filter->cache = c;
		}
		break;

	default:
		filter->status = 0;
		break;
	}
}

void mbfl_filt_conv_wchar_utf7_flush(mbfl_convert_filter *filter)
{
	int status = filter->status;
	int cache = filter->cache;
	filter->status = filter->cache = 0;
	/* flush fragments */
	switch (status) {
	case 1:
		(*filter->output_function)(mbfl_base64_table[(cache >> 10) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(cache >> 4) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(cache << 2) & 0x3c], filter->data);
		(*filter->output_function)('-', filter->data);
		break;

	case 2:
		(*filter->output_function)(mbfl_base64_table[(cache >> 14) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(cache >> 8) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(cache >> 2) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(cache << 4) & 0x30], filter->data);
		(*filter->output_function)('-', filter->data);
		break;

	case 3:
		(*filter->output_function)(mbfl_base64_table[(cache >> 12) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[(cache >> 6) & 0x3f], filter->data);
		(*filter->output_function)(mbfl_base64_table[cache & 0x3f], filter->data);
		(*filter->output_function)('-', filter->data);
		break;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}
}

static void mbfl_filt_ident_utf7_base64(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_2(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_4(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_6(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_8(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_10(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_12(unsigned char c, mbfl_identify_filter *filter);
static void mbfl_filt_ident_utf7_base64_14(unsigned char c, mbfl_identify_filter *filter);

void (*filt_ident_utf7_functions[])(unsigned char, mbfl_identify_filter*) = {
	mbfl_filt_ident_utf7_base64,
	mbfl_filt_ident_utf7_base64_2,
	mbfl_filt_ident_utf7_base64_4,
	mbfl_filt_ident_utf7_base64_6,
	mbfl_filt_ident_utf7_base64_8,
	mbfl_filt_ident_utf7_base64_10,
	mbfl_filt_ident_utf7_base64_12,
	mbfl_filt_ident_utf7_base64_14
};

static void identify_utf7_base64(unsigned char c, unsigned char bits, mbfl_identify_filter *filter)
{
	/* Cached bits are in low 2 bytes of `filter->status`
	 * If expecting the 2nd part of a UTF-16BE surrogate pair, bit 17 is set */
	int cache, surr;

	if (c == '\\' || c == '~' || c > 0x7F) {
		filter->flag = 1;
	} else if (c == '-' || ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' || c > '9') && c != '+' && c != '/')) {
		cache = filter->status & 0xFFFF;
		surr  = filter->status & 0x10000;
		if (surr) {
			/* Error; the 2nd part of a surrogate pair was missing */
			filter->flag = 1;
		} else if (cache & ((1 << bits) - 1)) {
			/* Error; padding was bad */
			filter->flag = 1;
		}
		filter->status = 0;
		filter->filter_function = mbfl_filt_ident_utf7;
	} else {
		cache = ((filter->status & 0xFFFF) << 6) | decode_base64_char(c);
		surr  = filter->status & 0x10000;
		bits += 6;
		if (bits >= 16) {
			unsigned short codeunit = cache >> (bits - 16);
			bits -= 16;
			cache &= ((1 << bits) - 1);
			if (surr) {
				if ((codeunit & 0xFC00) != 0xDC00) {
					/* Error; the 2nd part of a surrogate pair was missing */
					filter->flag = 1;
				}
				if ((codeunit & 0xFC00) != 0xD800) {
					surr = 0;
				}
			} else if ((codeunit & 0xFC00) == 0xD800) {
				surr = 0x10000;
			} else if ((codeunit & 0xFC00) == 0xDC00) {
				/* Error; 2nd part of a surrogate pair appeared when not expected */
				filter->flag = 1;
			}
		}

		filter->status = surr | cache;
		filter->filter_function = filt_ident_utf7_functions[bits / 2];
	}
}

static void mbfl_filt_ident_utf7_base64(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 0, filter);
}

static void mbfl_filt_ident_utf7_base64_2(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 2, filter);
}

static void mbfl_filt_ident_utf7_base64_4(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 4, filter);
}

static void mbfl_filt_ident_utf7_base64_6(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 6, filter);
}

static void mbfl_filt_ident_utf7_base64_8(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 8, filter);
}

static void mbfl_filt_ident_utf7_base64_10(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 10, filter);
}

static void mbfl_filt_ident_utf7_base64_12(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 12, filter);
}

static void mbfl_filt_ident_utf7_base64_14(unsigned char c, mbfl_identify_filter *filter)
{
	identify_utf7_base64(c, 14, filter);
}

static void mbfl_filt_ident_utf7(unsigned char c, mbfl_identify_filter *filter)
{
	if (c == '+') { /* '+' shift character */
		filter->filter_function = mbfl_filt_ident_utf7_base64;
	} else if (c == '\\' || c == '~' || c > 0x7F) { /* illegal character */
		filter->flag = 1;	/* bad */
	}
}
