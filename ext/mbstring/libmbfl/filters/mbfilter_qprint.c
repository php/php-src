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
#include "mbfilter_qprint.h"
#include "unicode_prop.h"

static const char *mbfl_encoding_qprint_aliases[] = {"qprint", NULL};

const mbfl_encoding mbfl_encoding_qprint = {
	mbfl_no_encoding_qprint,
	"Quoted-Printable",
	"Quoted-Printable",
	(const char *(*)[])&mbfl_encoding_qprint_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

const struct mbfl_convert_vtbl vtbl_8bit_qprint = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_qprint,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_qprintenc,
	mbfl_filt_conv_qprintenc_flush };

const struct mbfl_convert_vtbl vtbl_qprint_8bit = {
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_qprintdec,
	mbfl_filt_conv_qprintdec_flush };


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * any => Quoted-Printable
 */

int mbfl_filt_conv_qprintenc(int c, mbfl_convert_filter *filter)
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

		if (s == 0) {		/* null */
			CK((*filter->output_function)(s, filter->data));
			filter->status &= ~0xff00;
			break;
		}

		if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
			if (s == 0x0a || (s == 0x0d && c != 0x0a)) {	/* line feed */
				CK((*filter->output_function)(0x0d, filter->data));		/* CR */
				CK((*filter->output_function)(0x0a, filter->data));		/* LF */
				filter->status &= ~0xff00;
				break;
			} else if (s == 0x0d) {
				break;
			}
		}

		if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0  && n >= 72) {	/* soft line feed */
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(0x0d, filter->data));		/* CR */
			CK((*filter->output_function)(0x0a, filter->data));		/* LF */
			filter->status &= ~0xff00;
		}

		if (s <= 0 || s >= 0x80 || s == 0x3d		/* not ASCII or '=' */
		   || ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) != 0 && 
		       (mbfl_charprop_table[s] & MBFL_CHP_MMHQENC) != 0)) {
			/* hex-octet */
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			n = (s >> 4) & 0xf;
			if (n < 10) {
				n += 48;		/* '0' */
			} else {
				n += 55;		/* 'A' - 10 */
			}
			CK((*filter->output_function)(n, filter->data));
			n = s & 0xf;
			if (n < 10) {
				n += 48;
			} else {
				n += 55;
			}
			CK((*filter->output_function)(n, filter->data));
			if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
				filter->status += 0x300;
			}
		} else {
			CK((*filter->output_function)(s, filter->data));
			if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
				filter->status += 0x100;
			}
		}
		break;
	}

	return c;
}

int mbfl_filt_conv_qprintenc_flush(mbfl_convert_filter *filter)
{
	/* flush filter cache */
	(*filter->filter_function)('\0', filter);
	filter->status &= ~0xffff;
	filter->cache = 0;
	return 0;
}

/*
 * Quoted-Printable => any
 */
int mbfl_filt_conv_qprintdec(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 1:
		if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x46)) {	/* 0 - 9 or A - F */
			filter->cache = c;
			filter->status = 2;
		} else if (c == 0x0d) {	/* soft line feed */
			filter->status = 3;
		} else if (c == 0x0a) {	/* soft line feed */
			filter->status = 0;
		} else {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		}
		break;
	case 2:
		n = filter->cache;
		if (n >= 0x30 && n <= 0x39) {		/* '0' - '9' */
			n -= 48;		/* 48 = '0' */
		} else {
			n -= 55;		/* 55 = 'A' - 10 */
		}
		n <<= 4;
		if (c >= 0x30 && c <= 0x39) {		/* '0' - '9' */
			n += (c - 48);
		} else if (c >= 0x41 && c <= 0x46) {	/* 'A' - 'F' */
			n += (c - 55);
		} else {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(filter->cache, filter->data));
			n = c;
		}
		CK((*filter->output_function)(n, filter->data));
		filter->status = 0;
		break;
	case 3:
		if (c != 0x0a) {		/* LF */
			CK((*filter->output_function)(c, filter->data));
		}
		filter->status = 0;
		break;
	default:
		if (c == 0x3d) {		/* '=' */
			filter->status = 1;
		} else {
			CK((*filter->output_function)(c, filter->data));
		}
		break;
	}

	return c;
}

int mbfl_filt_conv_qprintdec_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	if (status == 1) {
		CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
	} else if (status == 2) {
		CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		CK((*filter->output_function)(cache, filter->data));
	}

	return 0;
}



