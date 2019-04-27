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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_utf7.h"

static int mbfl_filt_ident_utf7(int c, mbfl_identify_filter *filter);

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
	(const char *(*)[])&mbfl_encoding_utf7_aliases,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_utf7_wchar,
	&vtbl_wchar_utf7
};

const struct mbfl_identify_vtbl vtbl_identify_utf7 = {
	mbfl_no_encoding_utf7,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_utf7
};

const struct mbfl_convert_vtbl vtbl_utf7_wchar = {
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf7_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf7 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf7,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf7,
	mbfl_filt_conv_wchar_utf7_flush
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * UTF-7 => wchar
 */
int mbfl_filt_conv_utf7_wchar(int c, mbfl_convert_filter *filter)
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
		} else if (c == 0x2f) {			/* '/' */
			n = 63;
		}
		if (n < 0 || n > 63) {
			if (c == 0x2d) {
				if (filter->status == 1) {		/* "+-" -> "+" */
					CK((*filter->output_function)(0x2b, filter->data));
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
		if (c == 0x2b) {	/* '+'  shift character */
			filter->status = 1;
		} else if (c >= 0 && c < 0x80) {	/* ASCII */
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
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => UTF-7
 */
int mbfl_filt_conv_wchar_utf7(int c, mbfl_convert_filter *filter)
{
	int s, n;

	n = 0;
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
		s = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->filter_function)(s, filter));
		s = (c & 0x3ff) | 0xdc00;
		CK((*filter->filter_function)(s, filter));
		return c;
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
		return c;
	}

	switch (filter->status) {
	case 0:
		if (n != 0) {	/* directly encode characters */
			CK((*filter->output_function)(c, filter->data));
		} else {	/* Modified Base64 */
			CK((*filter->output_function)(0x2b, filter->data));		/* '+' */
			filter->status++;
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
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status++;
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
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status++;
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
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			CK((*filter->output_function)(c, filter->data));
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

int mbfl_filt_conv_wchar_utf7_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	switch (status) {
	case 1:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 4) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache << 2) & 0x3c], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 2:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 2) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache << 4) & 0x30], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 3:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[cache & 0x3f], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;
	}

	if (filter->flush_function != NULL) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

static int mbfl_filt_ident_utf7(int c, mbfl_identify_filter *filter)
{
	int n;

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == 0x2b) {	/* '+'  shift character */
			filter->status++;
		} else if (c == 0x5c || c == 0x7e || c < 0 || c > 0x7f) {	/* illegal character */
			filter->flag = 1;	/* bad */
		}
		break;

	/* Modified Base64 */
	case 1:
	case 2:
		n = 0;
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = 1;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = 1;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = 1;
		} else if (c == 0x2b) {			/* '+' */
			n = 1;
		} else if (c == 0x2f) {			/* '/' */
			n = 1;
		}
		if (n <= 0) {
			if (filter->status == 1 && c != 0x2d) {
				filter->flag = 1;	/* bad */
			} else if (c < 0 || c > 0x7f) {
				filter->flag = 1;	/* bad */
			}
			filter->status = 0;
		} else {
			filter->status = 2;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}
