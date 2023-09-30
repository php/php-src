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
 * The source code included in this file was separated from mbfilter.c
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
#include "utf7_helper.h"

static int mbfl_filt_conv_wchar_utf7imap_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf7imap_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_utf7imap_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf7imap(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static bool mb_check_utf7imap(unsigned char *in, size_t in_len);

static const char *mbfl_encoding_utf7imap_aliases[] = {"mUTF-7", NULL};

const mbfl_encoding mbfl_encoding_utf7imap = {
	mbfl_no_encoding_utf7imap,
	"UTF7-IMAP",
	NULL,
	mbfl_encoding_utf7imap_aliases,
	NULL,
	0,
	&vtbl_utf7imap_wchar,
	&vtbl_wchar_utf7imap,
	mb_utf7imap_to_wchar,
	mb_wchar_to_utf7imap,
	mb_check_utf7imap,
	NULL
};

const struct mbfl_convert_vtbl vtbl_utf7imap_wchar = {
	mbfl_no_encoding_utf7imap,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf7imap_wchar,
	mbfl_filt_conv_utf7imap_wchar_flush,
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

int mbfl_filt_conv_utf7imap_wchar(int c, mbfl_convert_filter *filter)
{
	int s, n = -1;

	if (filter->status != 0) { /* Modified Base64 */
		if (c >= 'A' && c <= 'Z') {
			n = c - 65;
		} else if (c >= 'a' && c <= 'z') {
			n = c - 71;
		} else if (c >= '0' && c <= '9') {
			n = c + 4;
		} else if (c == '+') {
			n = 62;
		} else if (c == ',') {
			n = 63;
		}

		if (n < 0 || n > 63) {
			if (c == '-') {
				if (filter->status == 1) { /* "&-" -> "&" */
					filter->cache = filter->status = 0;
					CK((*filter->output_function)('&', filter->data));
				} else if (filter->cache) {
					/* Base64-encoded section ended abruptly, with partially encoded characters,
					 * or it could be that it ended on the first half of a surrogate pair */
					filter->cache = filter->status = 0;
					CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				} else {
					/* Base64-encoded section properly terminated by - */
					filter->cache = filter->status = 0;
				}
			} else { /* illegal character */
				filter->cache = filter->status = 0;
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
			return 0;
		}
	}

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == '&') { /* shift character */
			filter->status++;
		} else if (c >= 0x20 && c <= 0x7E) { /* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else { /* illegal character */
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
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
			/* 1st part of surrogate pair */
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			/* 2nd part of surrogate pair */
			if (filter->cache & 0xfff0000) {
				s &= 0x3ff;
				s |= (filter->cache & 0xfff0000) >> 6;
				filter->cache = n;
				CK((*filter->output_function)(s, filter->data));
			} else { /* illegal character */
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		} else {
			filter->cache = n;
			/* Characters which can be expressed as literal, ASCII characters
			 * should not be Base64-encoded */
			if (s < 0x20 || s > 0x7E || s == '&') {
				CK((*filter->output_function)(s, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
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
			if (filter->cache & 0xfff0000) {
				s &= 0x3ff;
				s |= (filter->cache & 0xfff0000) >> 6;
				filter->cache = n;
				CK((*filter->output_function)(s, filter->data));
			} else { /* illegal character */
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		} else {
			filter->cache = n;
			/* Characters which can be expressed as literal, ASCII characters
			 * should not be Base64-encoded */
			if (s < 0x20 || s > 0x7E || s == '&') {
				CK((*filter->output_function)(s, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
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
			if (filter->cache & 0xfff0000) {
				s &= 0x3ff;
				s |= (filter->cache & 0xfff0000) >> 6;
				filter->cache = 0;
				CK((*filter->output_function)(s, filter->data));
			} else { /* illegal character */
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		} else {
			filter->cache = 0;
			/* Characters which can be expressed as literal, ASCII characters
			 * should not be Base64-encoded */
			if (s < 0x20 || s > 0x7E || s == '&') {
				CK((*filter->output_function)(s, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_utf7imap_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* It is illegal for a UTF-7 IMAP string to end in a Base-64 encoded
		 * section. It should always change back to ASCII before the end. */
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
		filter->status = 0;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
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

int mbfl_filt_conv_wchar_utf7imap(int c, mbfl_convert_filter *filter)
{
	int n = 0, s;

	if (c == '&') {
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
		return 0;
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
		return 0;
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
			CK((*filter->output_function)('-', filter->data));
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)('-', filter->data));
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
			CK((*filter->output_function)('-', filter->data));
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)('-', filter->data));
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
			CK((*filter->output_function)('-', filter->data));
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)('-', filter->data));
			}
			filter->status = 0;
		} else {
			filter->status = 1;
			filter->cache = c;
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_wchar_utf7imap_flush(mbfl_convert_filter *filter)
{
	int status = filter->status, cache = filter->cache;
	filter->status = filter->cache = 0;

	/* flush fragments */
	switch (status) {
	case 1:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 4) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache << 2) & 0x3c], filter->data));
		CK((*filter->output_function)('-', filter->data));
		break;

	case 2:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 2) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache << 4) & 0x30], filter->data));
		CK((*filter->output_function)('-', filter->data));
		break;

	case 3:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[cache & 0x3f], filter->data));
		CK((*filter->output_function)('-', filter->data));
		break;
	}

	return 0;
}

static inline bool is_base64_end(unsigned char c)
{
	return c >= DASH;
}

static unsigned char decode_base64(unsigned char c)
{
	if (c >= 'A' && c <= 'Z') {
		return c - 65;
	} else if (c >= 'a' && c <= 'z') {
		return c - 71;
	} else if (c >= '0' && c <= '9') {
		return c + 4;
	} else if (c == '+') {
		return 62;
	} else if (c == ',') {
		return 63;
	} else if (c == '-') {
		return DASH;
	}
	return ILLEGAL;
}

static uint32_t* handle_utf16_cp(uint16_t cp, uint32_t *out, uint16_t *surrogate1)
{
retry:
	if (*surrogate1) {
		if (cp >= 0xDC00 && cp <= 0xDFFF) {
			*out++ = ((*surrogate1 & 0x3FF) << 10) + (cp & 0x3FF) + 0x10000;
			*surrogate1 = 0;
		} else {
			*out++ = MBFL_BAD_INPUT;
			*surrogate1 = 0;
			goto retry;
		}
	} else if (cp >= 0xD800 && cp <= 0xDBFF) {
		*surrogate1 = cp;
	} else if (cp >= 0xDC00 && cp <= 0xDFFF) {
		/* 2nd part of surrogate pair came unexpectedly */
		*out++ = MBFL_BAD_INPUT;
	} else if (cp >= 0x20 && cp <= 0x7E && cp != '&') {
		*out++ = MBFL_BAD_INPUT;
	} else {
		*out++ = cp;
	}
	return out;
}

static uint32_t* handle_base64_end(unsigned char n, uint32_t *out, bool *base64, bool abrupt, uint16_t *surrogate1)
{
	if (abrupt || n == ILLEGAL || *surrogate1) {
		*out++ = MBFL_BAD_INPUT;
		*surrogate1 = 0;
	}

	*base64 = false;
	return out;
}

static size_t mb_utf7imap_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 5); /* This function will infinite-loop if called with a tiny output buffer */

	/* Why does this require a minimum output buffer size of 5?
	 * See comment in mb_utf7_to_wchar; the worst case for this function is similar,
	 * though not exactly the same. */

	unsigned char *p = *in, *e = p + *in_len;
	/* Always leave one empty space in output buffer in case the string ends while
	 * in Base64 mode and we need to emit an error marker */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	bool base64 = *state & 1;
	uint16_t surrogate1 = (*state >> 1); /* First half of a surrogate pair */

	while (p < e && out < limit) {
		if (base64) {
			/* Base64 section */
			if ((limit - out) < 4) {
				break;
			}

			unsigned char n1 = decode_base64(*p++);
			if (is_base64_end(n1)) {
				out = handle_base64_end(n1, out, &base64, false, &surrogate1);
				continue;
			} else if (p == e) {
				out = handle_base64_end(n1, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n2 = decode_base64(*p++);
			if (is_base64_end(n2) || p == e) {
				out = handle_base64_end(n2, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n3 = decode_base64(*p++);
			if (is_base64_end(n3)) {
				out = handle_base64_end(n3, out, &base64, true, &surrogate1);
				continue;
			}
			out = handle_utf16_cp((n1 << 10) | (n2 << 4) | ((n3 & 0x3C) >> 2), out, &surrogate1);
			if (p == e) {
				/* It is an error if trailing padding bits are not zeroes or if we were
				 * expecting the 2nd part of a surrogate pair when Base64 section ends */
				if ((n3 & 0x3) || surrogate1)
					*out++ = MBFL_BAD_INPUT;
				break;
			}

			unsigned char n4 = decode_base64(*p++);
			if (is_base64_end(n4)) {
				out = handle_base64_end(n4, out, &base64, n3 & 0x3, &surrogate1);
				continue;
			} else if (p == e) {
				out = handle_base64_end(n4, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n5 = decode_base64(*p++);
			if (is_base64_end(n5) || p == e) {
				out = handle_base64_end(n5, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n6 = decode_base64(*p++);
			if (is_base64_end(n6)) {
				out = handle_base64_end(n6, out, &base64, true, &surrogate1);
				continue;
			}
			out = handle_utf16_cp((n3 << 14) | (n4 << 8) | (n5 << 2) | ((n6 & 0x30) >> 4), out, &surrogate1);
			if (p == e) {
				if ((n6 & 0xF) || surrogate1)
					*out++ = MBFL_BAD_INPUT;
				break;
			}

			unsigned char n7 = decode_base64(*p++);
			if (is_base64_end(n7)) {
				out = handle_base64_end(n7, out, &base64, n6 & 0xF, &surrogate1);
				continue;
			} else if (p == e) {
				out = handle_base64_end(n7, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n8 = decode_base64(*p++);
			if (is_base64_end(n8)) {
				out = handle_base64_end(n8, out, &base64, true, &surrogate1);
				continue;
			}
			out = handle_utf16_cp((n6 << 12) | (n7 << 6) | n8, out, &surrogate1);
		} else {
			unsigned char c = *p++;

			if (c == '&') {
				if (p < e && *p == '-') {
					*out++ = '&';
					p++;
				} else {
					base64 = true;
				}
			} else if (c >= 0x20 && c <= 0x7E) {
				*out++ = c;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		}
	}

	if (p == e && base64) {
		/* UTF7-IMAP doesn't allow strings to end in Base64 mode
		 * One space in output buffer was reserved just for this */
		*out++ = MBFL_BAD_INPUT;
	}

	*state = (surrogate1 << 1) | base64;
	*in_len = e - p;
	*in = p;
	return out - buf;
}

#define SAVE_CONVERSION_STATE() buf->state = (cache << 4) | (nbits << 1) | base64
#define RESTORE_CONVERSION_STATE() base64 = (buf->state & 1); nbits = (buf->state >> 1) & 0x7; cache = (buf->state >> 4)

static const unsigned char mbfl_base64_table[] = {
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

static void mb_wchar_to_utf7imap(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	bool base64;
	unsigned char nbits, cache; /* `nbits` is the number of cached bits; either 0, 2, or 4 */
	RESTORE_CONVERSION_STATE();

	while (len--) {
		uint32_t w = *in++;
		if (base64) {
			if (w >= 0x20 && w <= 0x7E) {
				/* End of Base64 section. Drain buffered bits (if any), close Base64 section
				 * Leave enough space in the output buffer such that even if the remainder of
				 * the input string is ASCII, we can output the whole thing without having to
				 * check for output buffer space again */
				base64 = false;
				in--; len++; /* Unconsume codepoint; it will be handled by 'ASCII section' code below */
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				if (nbits) {
					out = mb_convert_buf_add(out, mbfl_base64_table[(cache << (6 - nbits)) & 0x3F]);
				}
				nbits = cache = 0;
				out = mb_convert_buf_add(out, '-');
			} else if (w >= MBFL_WCSPLANE_UTF32MAX) {
				/* Make recursive call to add an error marker character */
				SAVE_CONVERSION_STATE();
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf7imap);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				RESTORE_CONVERSION_STATE();
			} else {
				/* Encode codepoint, preceded by any cached bits, as Base64
				 * Make enough space in the output buffer to hold both any bytes that
				 * we emit right here, plus any finishing byte which might need to
				 * be emitted if the input string ends abruptly */
				uint64_t bits;
				if (w >= MBFL_WCSPLANE_SUPMIN) {
					/* Must use surrogate pair */
					MB_CONVERT_BUF_ENSURE(buf, out, limit, 7);
					w -= 0x10000;
					bits = ((uint64_t)cache << 32) | 0xD800DC00L | ((w & 0xFFC00) << 6) | (w & 0x3FF);
					nbits += 32;
				} else {
					MB_CONVERT_BUF_ENSURE(buf, out, limit, 4);
					bits = (cache << 16) | w;
					nbits += 16;
				}

				while (nbits >= 6) {
					out = mb_convert_buf_add(out, mbfl_base64_table[(bits >> (nbits - 6)) & 0x3F]);
					nbits -= 6;
				}
				cache = bits;
			}
		} else {
			/* ASCII section */
			if (w == '&') {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				out = mb_convert_buf_add2(out, '&', '-');
			} else if (w >= 0x20 && w <= 0x7E) {
				out = mb_convert_buf_add(out, w);
			} else if (w >= MBFL_WCSPLANE_UTF32MAX) {
				buf->state = 0;
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf7imap);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				RESTORE_CONVERSION_STATE();
			} else {
				out = mb_convert_buf_add(out, '&');
				base64 = true;
				in--; len++; /* Unconsume codepoint; it will be handled by Base64 code above */
			}
		}
	}

	if (end) {
		if (nbits) {
			out = mb_convert_buf_add(out, mbfl_base64_table[(cache << (6 - nbits)) & 0x3F]);
		}
		if (base64) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, 1);
			out = mb_convert_buf_add(out, '-');
		}
	} else {
		SAVE_CONVERSION_STATE();
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static bool is_utf16_cp_valid(uint16_t cp, bool is_surrogate)
{
	if (is_surrogate) {
		return cp >= 0xDC00 && cp <= 0xDFFF;
	} else if (cp >= 0xDC00 && cp <= 0xDFFF) {
		/* 2nd part of surrogate pair came unexpectedly */
		return false;
	} else if (cp >= 0x20 && cp <= 0x7E && cp != '&') {
		return false;
	}
	return true;
}

static bool mb_check_utf7imap(unsigned char *in, size_t in_len)
{
	unsigned char *p = in, *e = p + in_len;
	bool base64 = false;
	bool is_surrogate = false;

	while (p < e) {
		if (base64) {
			/* Base64 section */
			unsigned char n1 = decode_base64(*p++);
			if (is_base64_end(n1)) {
				if (!is_base64_end_valid(n1, false, is_surrogate)) {
					return false;
				}
				base64 = false;
				continue;
			} else if (p == e) {
				return false;
			}
			unsigned char n2 = decode_base64(*p++);
			if (is_base64_end(n2) || p == e) {
				return false;
			}
			unsigned char n3 = decode_base64(*p++);
			if (is_base64_end(n3)) {
				return false;
			}
			uint16_t cp1 = (n1 << 10) | (n2 << 4) | ((n3 & 0x3C) >> 2);
			if (!is_utf16_cp_valid(cp1, is_surrogate)) {
				return false;
			}
			is_surrogate = has_surrogate(cp1, is_surrogate);
			if (p == e) {
				return false;
			}

			unsigned char n4 = decode_base64(*p++);
			if (is_base64_end(n4)) {
				if (!is_base64_end_valid(n4, n3 & 0x3, is_surrogate)) {
					return false;
				}
				base64 = false;
				continue;
			} else if (p == e) {
				return false;
			}
			unsigned char n5 = decode_base64(*p++);
			if (is_base64_end(n5) || p == e) {
				return false;
			}
			unsigned char n6 = decode_base64(*p++);
			if (is_base64_end(n6)) {
				return false;
			}
			uint16_t cp2 = (n3 << 14) | (n4 << 8) | (n5 << 2) | ((n6 & 0x30) >> 4);
			if (!is_utf16_cp_valid(cp2, is_surrogate)) {
				return false;
			}
			is_surrogate = has_surrogate(cp2, is_surrogate);
			if (p == e) {
				return false;
			}

			unsigned char n7 = decode_base64(*p++);
			if (is_base64_end(n7)) {
				if (!is_base64_end_valid(n7, n6 & 0xF, is_surrogate)) {
					return false;
				}
				base64 = false;
				continue;
			} else if (p == e) {
				return false;
			}
			unsigned char n8 = decode_base64(*p++);
			if (is_base64_end(n8)) {
				return false;
			}
			uint16_t cp3 = (n6 << 12) | (n7 << 6) | n8;
			if (!is_utf16_cp_valid(cp3, is_surrogate)) {
				return false;
			}
			is_surrogate = has_surrogate(cp3, is_surrogate);
		} else {
			/* ASCII text section */
			unsigned char c = *p++;

			if (c == '&') {
				if (p == e) {
					return false;
				}
				unsigned char n = decode_base64(*p);
				if (n == DASH) {
					p++;
				} else if (n == ILLEGAL) {
					return false;
				} else {
					base64 = true;
				}
			} else if (c >= 0x20 && c <= 0x7E) {
				continue;
			} else {
				return false;
			}
		}
	}
	return !base64;
}
