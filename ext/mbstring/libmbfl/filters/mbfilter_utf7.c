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
#include "utf7_helper.h"

static int mbfl_filt_conv_utf7_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_utf7_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf7(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static bool mb_check_utf7(unsigned char *in, size_t in_len);

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
	&vtbl_wchar_utf7,
	mb_utf7_to_wchar,
	mb_wchar_to_utf7,
	mb_check_utf7
};

const struct mbfl_convert_vtbl vtbl_utf7_wchar = {
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf7_wchar,
	mbfl_filt_conv_utf7_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf7 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf7,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf7,
	mbfl_filt_conv_wchar_utf7_flush,
	NULL,
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

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

int mbfl_filt_conv_utf7_wchar(int c, mbfl_convert_filter *filter)
{
	int s, n = -1;

	if (filter->status) { /* Modified Base64 */
		n = decode_base64_char(c);
		if (n < 0) {
			if (filter->cache) {
				/* Either we were expecting the 2nd half of a surrogate pair which
				 * never came, or else the last Base64 data was not padded with zeroes */
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			if (c == '-') {
				if (filter->status == 1) { /* "+-" -> "+" */
					CK((*filter->output_function)('+', filter->data));
				}
			} else if (c >= 0 && c < 0x80) { /* ASCII exclude '-' */
				CK((*filter->output_function)(c, filter->data));
			} else { /* illegal character */
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			}
			filter->cache = filter->status = 0;
			return 0;
		}
	}

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == '+') { /* '+' shift character */
			filter->status = 1;
		} else if (c >= 0 && c < 0x80) { /* ASCII */
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
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			/* 2nd part of surrogate pair */
			if (filter->cache & 0xfff0000) {
				s &= 0x3ff;
				s |= (filter->cache & 0xfff0000) >> 6;
				filter->cache = n;
				CK((*filter->output_function)(s, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->cache = n;
			}
		} else {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			filter->cache = n;
			CK((*filter->output_function)(s, filter->data));
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
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			/* 2nd part of surrogate pair */
			if (filter->cache & 0xfff0000) {
				s &= 0x3ff;
				s |= (filter->cache & 0xfff0000) >> 6;
				filter->cache = n;
				CK((*filter->output_function)(s, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->cache = n;
			}
		} else {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			filter->cache = n;
			CK((*filter->output_function)(s, filter->data));
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
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			s = (((s & 0x3ff) << 16) + 0x400000);
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			if (filter->cache & 0xfff0000) {
				s &= 0x3ff;
				s |= (filter->cache & 0xfff0000) >> 6;
				filter->cache = 0;
				CK((*filter->output_function)(s, filter->data));
			} else {
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
				filter->cache = 0;
			}
		} else {
			if (filter->cache & 0xfff0000) {
				/* We were waiting for the 2nd part of a surrogate pair */
				(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
			}
			filter->cache = 0;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_utf7_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->cache) {
		/* Either we were expecting the 2nd half of a surrogate pair which
		 * never came, or else the last Base64 data was not padded with zeroes */
		filter->cache = 0;
		(*filter->output_function)(MBFL_BAD_INPUT, filter->data);
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

int mbfl_filt_conv_wchar_utf7(int c, mbfl_convert_filter *filter)
{
	int s;

	int n = 0;
	if (c >= 0 && c < 0x80) { /* ASCII */
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '\0' || c == '/' || c == '-') {
			n = 1;
		} else if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\'' || c == '(' || c == ')' || c == ',' || c == '.' || c == ':' || c == '?') {
			n = 2;
		}
	} else if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		;
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_UTF32MAX) {
		CK((*filter->filter_function)(((c >> 10) - 0x40) | 0xd800, filter));
		CK((*filter->filter_function)((c & 0x3ff) | 0xdc00, filter));
		return 0;
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
		return 0;
	}

	switch (filter->status) {
	case 0:
		if (n != 0) { /* directly encode characters */
			CK((*filter->output_function)(c, filter->data));
		} else { /* Modified Base64 */
			CK((*filter->output_function)('+', filter->data));
			filter->status = 1;
			filter->cache = c;
		}
		break;

	/* encode Modified Base64 */
	case 1:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_base64_table[(s >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 4) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_base64_table[(s << 2) & 0x3c], filter->data));
			if (n == 1) {
				CK((*filter->output_function)('-', filter->data));
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status = 2;
			filter->cache = ((s & 0xf) << 16) | c;
		}
		break;

	case 2:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_base64_table[(s >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 2) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_base64_table[(s << 4) & 0x30], filter->data));
			if (n == 1) {
				CK((*filter->output_function)('-', filter->data));
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status = 3;
			filter->cache = ((s & 0x3) << 16) | c;
		}
		break;

	case 3:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_base64_table[(s >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[s & 0x3f], filter->data));
		if (n != 0) {
			if (n == 1) {
				CK((*filter->output_function)('-', filter->data));
			}
			CK((*filter->output_function)(c, filter->data));
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

int mbfl_filt_conv_wchar_utf7_flush(mbfl_convert_filter *filter)
{
	int status = filter->status;
	int cache = filter->cache;
	filter->status = filter->cache = 0;

	/* flush fragments */
	switch (status) {
	case 1:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 4) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache << 2) & 0x3c], filter->data));
		CK((*filter->output_function)('-', filter->data));
		break;

	case 2:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 2) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache << 4) & 0x30], filter->data));
		CK((*filter->output_function)('-', filter->data));
		break;

	case 3:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[cache & 0x3f], filter->data));
		CK((*filter->output_function)('-', filter->data));
		break;
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static inline bool is_base64_end(unsigned char c)
{
	return c >= DASH;
}

static bool is_optional_direct(unsigned char c)
{
	/* Characters that are allowed to be encoded by Base64 or directly encoded */
	return c == '!' || c == '"' || c == '#' || c == '$' || c == '%' || c == '&' || c == '*' || c == ';' || c == '<' ||
		   c == '=' || c == '>' || c == '@' || c == '[' || c == ']' || c == '^' || c == '_' || c == '`' || c == '{' ||
		   c == '|' || c == '}';
}

static bool can_end_base64(uint32_t c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\'' || c == '(' || c == ')' || c == ',' || c == '.' || c == ':' || c == '?';
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
	} else if (c == '/') {
		return 63;
	} else if (c == '-') {
		return DASH;
	} else if (can_end_base64(c) || is_optional_direct(c) || c == '\0') {
		return DIRECT;
	} else if (c <= 0x7F) {
		return ASCII;
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
	} else {
		*out++ = cp;
	}
	return out;
}

static uint32_t* handle_base64_end(unsigned char n, unsigned char **p, uint32_t *out, bool *base64, bool abrupt, uint16_t *surrogate1)
{
	if (abrupt || *surrogate1) {
		*out++ = MBFL_BAD_INPUT;
		*surrogate1 = 0;
	}

	if (n == ILLEGAL) {
		*out++ = MBFL_BAD_INPUT;
	} else if (n == DIRECT || n == ASCII) {
		(*p)--; /* Unconsume byte */
	}

	*base64 = false;
	return out;
}

static size_t mb_utf7_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 5); /* This function will infinite-loop if called with a tiny output buffer */

	/* Why does this require a minimum output buffer size of 5?
	 * There is one case where one iteration of the main 'while' loop below will emit 5 wchars:
	 * that is if the first half of a surrogate pair is followed by an otherwise valid codepoint which
	 * is not the 2nd half of a surrogate pair, then another valid codepoint, then the Base64-encoded
	 * section ends with a byte which is not a valid Base64 character, AND which also is not in a
	 * position where we would expect the Base64-encoded section to end */

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	bool base64 = *state & 1;
	uint16_t surrogate1 = (*state >> 1); /* First half of a surrogate pair which still needs 2nd half */

	while (p < e && out < limit) {
		if (base64) {
			/* Base64 section */
			if ((limit - out) < 5) {
				break;
			}

			unsigned char n1 = decode_base64(*p++);
			if (is_base64_end(n1)) {
				out = handle_base64_end(n1, &p, out, &base64, false, &surrogate1);
				continue;
			} else if (p == e) {
				out = handle_base64_end(n1, &p, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n2 = decode_base64(*p++);
			if (is_base64_end(n2) || p == e) {
				out = handle_base64_end(n2, &p, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n3 = decode_base64(*p++);
			if (is_base64_end(n3)) {
				out = handle_base64_end(n3, &p, out, &base64, true, &surrogate1);
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
				out = handle_base64_end(n4, &p, out, &base64, n3 & 0x3, &surrogate1);
				continue;
			} else if (p == e) {
				out = handle_base64_end(n4, &p, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n5 = decode_base64(*p++);
			if (is_base64_end(n5) || p == e) {
				out = handle_base64_end(n5, &p, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n6 = decode_base64(*p++);
			if (is_base64_end(n6)) {
				out = handle_base64_end(n6, &p, out, &base64, true, &surrogate1);
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
				out = handle_base64_end(n7, &p, out, &base64, n6 & 0xF, &surrogate1);
				continue;
			} else if (p == e) {
				out = handle_base64_end(n7, &p, out, &base64, true, &surrogate1);
				continue;
			}
			unsigned char n8 = decode_base64(*p++);
			if (is_base64_end(n8)) {
				out = handle_base64_end(n8, &p, out, &base64, true, &surrogate1);
				continue;
			}
			out = handle_utf16_cp((n6 << 12) | (n7 << 6) | n8, out, &surrogate1);
		} else {
			/* ASCII text section */
			unsigned char c = *p++;

			if (c == '+') {
				if (p < e) {
					if (*p == '-') {
						*out++ = '+';
						p++;
					} else {
						base64 = true;
					}
				}
				/* If a + comes at the end of the input string... do nothing about it */
			} else if (c <= 0x7F) {
				*out++ = c;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		}
	}

	*state = (surrogate1 << 1) | base64;
	*in_len = e - p;
	*in = p;
	return out - buf;
}

static bool should_direct_encode(uint32_t c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '\0' || c == '/' || c == '-' || can_end_base64(c);
}

#define SAVE_CONVERSION_STATE() buf->state = (cache << 4) | (nbits << 1) | base64
#define RESTORE_CONVERSION_STATE() base64 = (buf->state & 1); nbits = (buf->state >> 1) & 0x7; cache = (buf->state >> 4)

static void mb_wchar_to_utf7(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);

	/* Make enough space such that if the input string is all ASCII (not including '+'),
	 * we can copy it to the output buffer without checking for available space.
	 * However, if we find anything which is not plain ASCII, additional checks for
	 * output buffer space will be needed. */
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	bool base64;
	unsigned char nbits, cache; /* `nbits` is the number of cached bits; either 0, 2, or 4 */
	RESTORE_CONVERSION_STATE();

	while (len--) {
		uint32_t w = *in++;
		if (base64) {
			if (should_direct_encode(w)) {
				/* End of Base64 section. Drain buffered bits (if any), close Base64 section */
				base64 = false;
				in--; len++; /* Unconsume codepoint; it will be handled by 'ASCII section' code below */
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
				if (nbits) {
					out = mb_convert_buf_add(out, mbfl_base64_table[(cache << (6 - nbits)) & 0x3F]);
				}
				nbits = cache = 0;
				if (!can_end_base64(w)) {
					out = mb_convert_buf_add(out, '-');
				}
			} else if (w >= MBFL_WCSPLANE_UTF32MAX) {
				/* Make recursive call to add an error marker character */
				SAVE_CONVERSION_STATE();
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf7);
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
			if (should_direct_encode(w)) {
				out = mb_convert_buf_add(out, w);
			} else if (w >= MBFL_WCSPLANE_UTF32MAX) {
				buf->state = 0;
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf7);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
				RESTORE_CONVERSION_STATE();
			} else {
				out = mb_convert_buf_add(out, '+');
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
	} else {
		/* 2nd part of surrogate pair came unexpectedly */
		return !(cp >= 0xDC00 && cp <= 0xDFFF);
	}
}

static bool can_encode_directly(unsigned char c)
{
	return should_direct_encode(c) || is_optional_direct(c) || c == '\0';
}

static bool mb_check_utf7(unsigned char *in, size_t in_len)
{
	unsigned char *p = in, *e = p + in_len;
	bool base64 = false;
	bool is_surrogate = false;

	while (p < e) {
		if (base64) {
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
				/* It is an error if trailing padding bits are not zeroes or if we were
				 * expecting the 2nd part of a surrogate pair when Base64 section ends */
				return !((n3 & 0x3) || is_surrogate);
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
				return !((n6 & 0xF) || is_surrogate);
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

			if (c == '+') {
				if (p == e) {
					base64 = true;
					return !is_surrogate;
				}
				unsigned char n = decode_base64(*p);
				if (n == DASH) {
					p++;
				} else if (n > DASH) {
					/* If a "+" character followed immediately by any character other than base64 or "-" */
					return false;
				} else {
					base64 = true;
				}
			} else if (can_encode_directly(c)) {
				continue;
			} else {
				return false;
			}
		}
	}
	return !is_surrogate;
}
