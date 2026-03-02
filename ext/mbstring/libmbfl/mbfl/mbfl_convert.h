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
 * The source code included in this files was separated from mbfilter.h
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.h is included in this package .
 *
 */

#ifndef MBFL_CONVERT_H
#define MBFL_CONVERT_H

#include "mbfl_defs.h"
#include "mbfl_encoding.h"
#include "mbfl_memory_device.h"

typedef struct _mbfl_convert_filter mbfl_convert_filter;

/* internal */
typedef int (*filter_flush_t)(mbfl_convert_filter*);

/* defined by mbfl_convert_filter_{new,new2,init} */
typedef int (*output_function_t)(int, void*);
typedef int (*flush_function_t)(void *);

struct _mbfl_convert_filter {
	void (*filter_dtor)(mbfl_convert_filter *filter);
	void (*filter_copy)(mbfl_convert_filter *src, mbfl_convert_filter *dest);
	int (*filter_function)(int c, mbfl_convert_filter *filter);
	filter_flush_t  filter_flush;
	output_function_t output_function;
	flush_function_t flush_function;
	void *data;
	int status;
	int cache;
	const mbfl_encoding *from;
	const mbfl_encoding *to;
	int illegal_mode;
	uint32_t illegal_substchar;
	size_t num_illegalchar;
	void *opaque;
};

MBFLAPI extern mbfl_convert_filter *mbfl_convert_filter_new(const mbfl_encoding *from, const mbfl_encoding *to, output_function_t output_function,
	flush_function_t flush_function, void *data);
MBFLAPI extern mbfl_convert_filter *mbfl_convert_filter_new2(const struct mbfl_convert_vtbl *vtbl, output_function_t output_function,
	flush_function_t flush_function, void *data);
MBFLAPI extern void mbfl_convert_filter_delete(mbfl_convert_filter *filter);
MBFLAPI extern int mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter);
MBFLAPI extern unsigned char* mbfl_convert_filter_feed_string(mbfl_convert_filter *filter, unsigned char *p, size_t len);
MBFLAPI extern int mbfl_convert_filter_flush(mbfl_convert_filter *filter);
MBFLAPI extern void mbfl_convert_filter_reset(mbfl_convert_filter *filter, const mbfl_encoding *from, const mbfl_encoding *to);
MBFLAPI extern void mbfl_convert_filter_copy(mbfl_convert_filter *src, mbfl_convert_filter *dist);
MBFLAPI extern int mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter);
MBFLAPI extern const struct mbfl_convert_vtbl * mbfl_convert_filter_get_vtbl(const mbfl_encoding *from, const mbfl_encoding *to);

MBFLAPI extern void mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter);
MBFLAPI extern int mbfl_filt_conv_common_flush(mbfl_convert_filter *filter);

MBFLAPI extern void mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src);
MBFLAPI extern int mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p);

MBFLAPI extern zend_string* mb_fast_convert(unsigned char *in, size_t in_len, const mbfl_encoding *from, const mbfl_encoding *to, uint32_t replacement_char, unsigned int error_mode, unsigned int *num_errors);
MBFLAPI extern void mb_illegal_output(uint32_t bad_cp, mb_from_wchar_fn fn, mb_convert_buf* buf);

#endif /* MBFL_CONVERT_H */
