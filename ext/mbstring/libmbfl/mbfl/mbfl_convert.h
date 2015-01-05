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

struct _mbfl_convert_filter {
	void (*filter_ctor)(mbfl_convert_filter *filter);
	void (*filter_dtor)(mbfl_convert_filter *filter);
	void (*filter_copy)(mbfl_convert_filter *src, mbfl_convert_filter *dest);
	int (*filter_function)(int c, mbfl_convert_filter *filter);
	int (*filter_flush)(mbfl_convert_filter *filter);
	int (*output_function)(int c, void *data);
	int (*flush_function)(void *data);
	void *data;
	int status;
	int cache;
	const mbfl_encoding *from;
	const mbfl_encoding *to;
	int illegal_mode;
	int illegal_substchar;
	int num_illegalchar;
	void *opaque;
};

struct mbfl_convert_vtbl {
	enum mbfl_no_encoding from;
	enum mbfl_no_encoding to;
	void (*filter_ctor)(mbfl_convert_filter *filter);
	void (*filter_dtor)(mbfl_convert_filter *filter);
	int (*filter_function)(int c, mbfl_convert_filter *filter);
	int (*filter_flush)(mbfl_convert_filter *filter);
	void (*filter_copy)(mbfl_convert_filter *src, mbfl_convert_filter *dest);
};

MBFLAPI extern const struct mbfl_convert_vtbl *mbfl_convert_filter_list[];

MBFLAPI extern mbfl_convert_filter *mbfl_convert_filter_new(
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int (*output_function)(int, void *),
    int (*flush_function)(void *),
    void *data );
MBFLAPI extern mbfl_convert_filter *mbfl_convert_filter_new2(
	const struct mbfl_convert_vtbl *vtbl,
    int (*output_function)(int, void *),
    int (*flush_function)(void *),
    void *data );
MBFLAPI extern void mbfl_convert_filter_delete(mbfl_convert_filter *filter);
MBFLAPI extern int mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter);
MBFLAPI extern int mbfl_convert_filter_flush(mbfl_convert_filter *filter);
MBFLAPI extern void mbfl_convert_filter_reset(mbfl_convert_filter *filter, enum mbfl_no_encoding from, enum mbfl_no_encoding to);
MBFLAPI extern void mbfl_convert_filter_copy(mbfl_convert_filter *src, mbfl_convert_filter *dist);
MBFLAPI extern int mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter);
MBFLAPI extern const struct mbfl_convert_vtbl * mbfl_convert_filter_get_vtbl(enum mbfl_no_encoding from, enum mbfl_no_encoding to);

MBFLAPI extern void mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter);
MBFLAPI extern int mbfl_filt_conv_common_flush(mbfl_convert_filter *filter);
MBFLAPI extern void mbfl_filt_conv_common_dtor(mbfl_convert_filter *filter);

MBFLAPI extern int mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src);
MBFLAPI extern int mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p);

#endif /* MBFL_CONVERT_H */
