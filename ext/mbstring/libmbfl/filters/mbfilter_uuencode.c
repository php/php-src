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
#include "mbfilter_uuencode.h"

const mbfl_encoding mbfl_encoding_uuencode = {
	mbfl_no_encoding_uuencode,
	"UUENCODE",
	"x-uuencode",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS,
	NULL,
	NULL
};

const struct mbfl_convert_vtbl vtbl_uuencode_8bit = {
	mbfl_no_encoding_uuencode,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_uudec,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/* uuencode => any */
#define UUDEC(c)	(char)(((c)-' ')&077)
static const char * uuenc_begin_text = "begin ";
enum { uudec_state_ground=0, uudec_state_inbegin,
	uudec_state_until_newline,
	uudec_state_size, uudec_state_a, uudec_state_b, uudec_state_c, uudec_state_d,
	uudec_state_skip_newline};

int mbfl_filt_conv_uudec(int c, mbfl_convert_filter * filter)
{
	int n;

	switch(filter->status)	{
		case uudec_state_ground:
			/* looking for "begin 0666 filename\n" line */
			if (filter->cache == 0 && c == 'b')
			{
				filter->status = uudec_state_inbegin;
				filter->cache = 1; /* move to 'e' */
			}
			else if (c == '\n')
				filter->cache = 0;
			else
				filter->cache++;
			break;
		case uudec_state_inbegin:
			if (uuenc_begin_text[filter->cache++] != c)	{
				/* doesn't match pattern */
				filter->status = uudec_state_ground;
				break;
			}
			if (filter->cache == 5)
			{
				/* thats good enough - wait for a newline */
				filter->status = uudec_state_until_newline;
				filter->cache = 0;
			}
			break;
		case uudec_state_until_newline:
			if (c == '\n')
				filter->status = uudec_state_size;
			break;
		case uudec_state_size:
			/* get "size" byte */
			n = UUDEC(c);
			filter->cache = n << 24;
			filter->status = uudec_state_a;
			break;
		case uudec_state_a:
			/* get "a" byte */
			n = UUDEC(c);
			filter->cache |= (n << 16);
			filter->status = uudec_state_b;
			break;
		case uudec_state_b:
			/* get "b" byte */
			n = UUDEC(c);
			filter->cache |= (n << 8);
			filter->status = uudec_state_c;
			break;
		case uudec_state_c:
			/* get "c" byte */
			n = UUDEC(c);
			filter->cache |= n;
			filter->status = uudec_state_d;
			break;
		case uudec_state_d:
			/* get "d" byte */
			{
				int A, B, C, D = UUDEC(c);
				A = (filter->cache >> 16) & 0xff;
				B = (filter->cache >> 8) & 0xff;
				C = (filter->cache) & 0xff;
				n = (filter->cache >> 24) & 0xff;
				if (n-- > 0)
					CK((*filter->output_function)( (A << 2) | (B >> 4), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (B << 4) | (C >> 2), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (C << 6) | D, filter->data));
				filter->cache = n << 24;

				if (n == 0)
					filter->status = uudec_state_skip_newline;	/* skip next byte (newline) */
				else
					filter->status = uudec_state_a; /* go back to fetch "A" byte */
			}
			break;
		case uudec_state_skip_newline:
			/* skip newline */
			filter->status = uudec_state_size;
	}
	return c;
}
