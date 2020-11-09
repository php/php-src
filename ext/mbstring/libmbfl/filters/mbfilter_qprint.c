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
#include "mbfilter_qprint.h"
#include "mbfilter_ascii.h"
#include "unicode_prop.h"

static const char *mbfl_encoding_qprint_aliases[] = {"qprint", NULL};

const mbfl_encoding mbfl_encoding_qprint = {
	mbfl_no_encoding_qprint,
	"Quoted-Printable",
	"Quoted-Printable",
	mbfl_encoding_qprint_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	NULL,
	NULL
};

const struct mbfl_identify_vtbl vtbl_identify_qprint = {
	mbfl_no_encoding_qprint,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_ascii
};

const struct mbfl_convert_vtbl vtbl_8bit_qprint = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_qprint,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_qprintenc,
	mbfl_filt_conv_qprintenc_flush
};

const struct mbfl_convert_vtbl vtbl_qprint_8bit = {
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_qprintdec,
	mbfl_filt_conv_qprintdec_flush
};

/*
 * any => Quoted-Printable
 */
static inline int nibble2hex(int nibble)
{
	if (nibble < 10) {
		return nibble + '0';
	} else {
		return nibble + 'A' - 10;
	}
}

void mbfl_filt_conv_qprintenc(int c, mbfl_convert_filter *filter)
{
	int s, n;

	switch (filter->status & 0xff) {
	case 0:
		filter->cache = c;
		filter->status++;
		break;
	default:
		s = filter->cache;
		filter->cache = c;
		n = (filter->status & 0xff00) >> 8;

		if (s == 0) { /* null */
			(*filter->output_function)(s, filter->data);
			filter->status &= ~0xff00;
			break;
		}

		if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
			if (s == '\n' || (s == '\r' && c != '\n')) { /* line feed */
				(*filter->output_function)('\r', filter->data);
				(*filter->output_function)('\n', filter->data);
				filter->status &= ~0xff00;
				break;
			} else if (s == '\r') {
				break;
			}
		}

		if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0 && n >= 72) { /* soft line feed */
			(*filter->output_function)('=', filter->data);
			(*filter->output_function)('\r', filter->data);
			(*filter->output_function)('\n', filter->data);
			filter->status &= ~0xff00;
		}

		if (s <= 0 || s >= 0x80 || s == '=' /* not ASCII or '=' */
		   || ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) && mime_char_needs_qencode[s])) {
			/* hex-octet */
			(*filter->output_function)('=', filter->data);
			(*filter->output_function)(nibble2hex((s >> 4) & 0xf), filter->data);
			(*filter->output_function)(nibble2hex(s & 0xf), filter->data);
			if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
				filter->status += 0x300;
			}
		} else {
			(*filter->output_function)(s, filter->data);
			if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
				filter->status += 0x100;
			}
		}
		break;
	}
}

void mbfl_filt_conv_qprintenc_flush(mbfl_convert_filter *filter)
{
	(*filter->filter_function)('\0', filter);
	filter->status &= ~0xffff;
	filter->cache = 0;
}

/*
 * Quoted-Printable => any
 */
void mbfl_filt_conv_qprintdec(int c, mbfl_convert_filter *filter)
{
	int n, m;

	static int hex2code_map[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};

	switch (filter->status) {
	case 1:
		if (hex2code_map[c & 0xff] >= 0) {
			filter->cache = c;
			filter->status = 2;
		} else if (c == '\r') {	/* soft line feed */
			filter->status = 3;
		} else if (c == '\n') {	/* soft line feed */
			filter->status = 0;
		} else {
			(*filter->output_function)('=', filter->data);
			(*filter->output_function)(c, filter->data);
			filter->status = 0;
		}
		break;
	case 2:
		m = hex2code_map[c & 0xff];
		if (m < 0) {
			(*filter->output_function)('=', filter->data);
			(*filter->output_function)(filter->cache, filter->data);
			n = c;
		} else {
			n = hex2code_map[filter->cache] << 4 | m;
		}
		(*filter->output_function)(n, filter->data);
		filter->status = 0;
		break;
	case 3:
		if (c != '\n') {
			(*filter->output_function)(c, filter->data);
		}
		filter->status = 0;
		break;
	default:
		if (c == '=') {
			filter->status = 1;
		} else {
			(*filter->output_function)(c, filter->data);
		}
		break;
	}
}

void mbfl_filt_conv_qprintdec_flush(mbfl_convert_filter *filter)
{
	int status = filter->status;
	int cache = filter->cache;
	filter->status = filter->cache = 0;
	/* flush fragments */
	if (status == 1) {
		(*filter->output_function)('=', filter->data);
	} else if (status == 2) {
		(*filter->output_function)('=', filter->data);
		(*filter->output_function)(cache, filter->data);
	}
}
