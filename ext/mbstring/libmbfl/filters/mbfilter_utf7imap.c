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

/* Modified UTF-7 used for 'international mailbox names' in the IMAP protocol
 * Also known as mUTF-7
 * Defined in RFC 3501 5.1.3 (https://tools.ietf.org/html/rfc3501)
 *
 * Quoting from the RFC:
 *
 ***********************************************************************
 * In modified UTF-7, printable US-ASCII characters, except for "&",
 * represent themselves; that is, characters with octet values 0x20-0x25
 * and 0x27-0x7e. The character "&" (0x26) is represented by the
 * two-octet sequence "&-".
 *
 * All other characters (octet values 0x00-0x1f and 0x7f-0xff) are
 * represented in modified BASE64, with a further modification from
 * UTF-7 that "," is used instead of "/". Modified BASE64 MUST NOT be
 * used to represent any printing US-ASCII character which can represent
 * itself.
 *
 * "&" is used to shift to modified BASE64 and "-" to shift back to
 * US-ASCII. There is no implicit shift from BASE64 to US-ASCII, and
 * null shifts ("-&" while in BASE64; note that "&-" while in US-ASCII
 * means "&") are not permitted.  However, all names start in US-ASCII,
 * and MUST end in US-ASCII; that is, a name that ends with a non-ASCII
 * ISO-10646 character MUST end with a "-").
 ***********************************************************************
 *
 * The purpose of all this is: 1) to keep all parts of IMAP messages 7-bit clean,
 * 2) to avoid giving special treatment to +, /, \, and ~, since these are
 * commonly used in mailbox names, and 3) to ensure there is only one
 * representation of any mailbox name (vanilla UTF-7 does allow multiple
 * representations of the same string, by Base64-encoding characters which
 * could have been included as ASCII literals.)
 *
 * RFC 2152 also applies, since it defines vanilla UTF-7 (minus IMAP modifications)
 * The following paragraph is notable:
 *
 ***********************************************************************
 * Unicode is encoded using Modified Base64 by first converting Unicode
 * 16-bit quantities to an octet stream (with the most significant octet first).
 * Surrogate pairs (UTF-16) are converted by treating each half of the pair as
 * a separate 16 bit quantity (i.e., no special treatment). Text with an odd
 * number of octets is ill-formed. ISO 10646 characters outside the range
 * addressable via surrogate pairs cannot be encoded.
 ***********************************************************************
 *
 * So after reversing the modified Base64 encoding on an encoded section,
 * the contents are interpreted as UTF-16BE. */

#include "mbfilter.h"
#include "mbfilter_utf7imap.h"

static const char *mbfl_encoding_utf7imap_aliases[] = {"mUTF-7", NULL};

const mbfl_encoding mbfl_encoding_utf7imap = {
	mbfl_no_encoding_utf7imap,
	"UTF7-IMAP",
	NULL,
	mbfl_encoding_utf7imap_aliases,
	NULL,
	MBFL_ENCTYPE_MBCS,
	&vtbl_utf7imap_wchar,
	&vtbl_wchar_utf7imap
};

const struct mbfl_convert_vtbl vtbl_utf7imap_wchar = {
	mbfl_no_encoding_utf7imap,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf7imap_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf7imap = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf7imap,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf7imap,
	mbfl_filt_conv_wchar_utf7imap_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * UTF7-IMAP => wchar
 */
int mbfl_filt_conv_utf7imap_wchar(int c, mbfl_convert_filter *filter)
{
	int s, n;

	n = -1;
	if (filter->status != 0) {		/* Modified Base64 */
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = c - 65;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = c - 71;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = c + 4;
		} else if (c == 0x2b) {			/* '+' */
			n = 62;
		} else if (c == 0x2c) {			/* ',' */
			n = 63;
		}
		if (n < 0 || n > 63) {
			if (c == 0x2d) {
				if (filter->status == 1) {		/* "&-" -> "&" */
					CK((*filter->output_function)(0x26, filter->data));
				}
			} else if (c >= 0 && c < 0x80) {	/* ASCII exclude '-' */
				CK((*filter->output_function)(c, filter->data));
			} else {		/* illegal character */
				s = c & MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
			filter->cache = 0;
			filter->status = 0;
			return c;
		}
	}

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == 0x26) {	/* '&'  shift character */
			filter->status++;
		} else if (c >= 0x20 && c <= 0x7E) {	/* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else {		/* illegal character */
			s = c & MBFL_WCSGROUP_MASK;
			s |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(s, filter->data));
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
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = n;
			/* Characters which can be expressed as literal, ASCII characters
			 * should not be Base64-encoded */
			if (s < 0x20 || s > 0x7E || s == '&') {
				CK((*filter->output_function)(s, filter->data));
			} else {
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
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
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = n;
			/* Characters which can be expressed as literal, ASCII characters
			 * should not be Base64-encoded */
			if (s < 0x20 || s > 0x7E || s == '&') {
				CK((*filter->output_function)(s, filter->data));
			} else {
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
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
			s = (((s & 0x3ff) << 16) + 0x400000);
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = 0;
			/* Characters which can be expressed as literal, ASCII characters
			 * should not be Base64-encoded */
			if (s < 0x20 || s > 0x7E || s == '&') {
				CK((*filter->output_function)(s, filter->data));
			} else {
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static const unsigned char mbfl_utf7imap_base64_table[] =
{
 /* 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', */
   0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,
 /* 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
   0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
 /* 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', */
   0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
 /* 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
   0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
 /* '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', ',', '\0' */
   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2b,0x2c,0x00
};

/*
 * wchar => UTF7-IMAP
 */
int mbfl_filt_conv_wchar_utf7imap(int c, mbfl_convert_filter *filter)
{
	int n, s;

	n = 0;
	if (c == 0x26) {
		n = 1;
	} else if ((c >= 0x20 && c <= 0x7e) || c == 0) {
		n = 2;
	} else if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		;
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		s = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->filter_function)(s, filter));
		s = (c & 0x3ff) | 0xdc00;
		CK((*filter->filter_function)(s, filter));
		return c;
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
		return c;
	}

	switch (filter->status) {
	case 0:
		if (n != 0) {	/* directly encode characters */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
		} else {	/* Modified Base64 */
			CK((*filter->output_function)(0x26, filter->data));		/* '&' */
			filter->status = 1;
			filter->cache = c;
		}
		break;

	/* encode Modified Base64 */
	case 1:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 4) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s << 2) & 0x3c], filter->data));
			CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			filter->status = 0;
		} else {
			filter->status = 2;
			filter->cache = ((s & 0xf) << 16) | c;
		}
		break;

	case 2:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 2) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s << 4) & 0x30], filter->data));
			CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			filter->status = 0;
		} else {
			filter->status = 3;
			filter->cache = ((s & 0x3) << 16) | c;
		}
		break;

	case 3:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[s & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
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

	return c;

}

int mbfl_filt_conv_wchar_utf7imap_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	switch (status) {
	case 1:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 4) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache << 2) & 0x3c], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 2:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 2) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache << 4) & 0x30], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 3:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[cache & 0x3f], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;
	}
	return 0;
}
