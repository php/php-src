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

#ifndef MBFL_IDENT_H
#define MBFL_IDENT_H

#include "mbfl_defs.h"
#include "mbfl_encoding.h"

/*
 * identify filter
 */
typedef struct _mbfl_identify_filter mbfl_identify_filter;

struct _mbfl_identify_filter {
	void (*filter_ctor)(mbfl_identify_filter *filter);
	int (*filter_function)(int c, mbfl_identify_filter *filter);
	int status;
	int flag;
	int score;
	const mbfl_encoding *encoding;
};

struct mbfl_identify_vtbl {
	enum mbfl_no_encoding encoding;
	void (*filter_ctor)(mbfl_identify_filter *filter);
	int (*filter_function)(int c, mbfl_identify_filter *filter);
};

MBFLAPI extern const struct mbfl_identify_vtbl * mbfl_identify_filter_get_vtbl(enum mbfl_no_encoding encoding);
MBFLAPI extern mbfl_identify_filter * mbfl_identify_filter_new(enum mbfl_no_encoding encoding);
MBFLAPI extern mbfl_identify_filter * mbfl_identify_filter_new2(const mbfl_encoding *encoding);
MBFLAPI extern void mbfl_identify_filter_delete(mbfl_identify_filter *filter);
MBFLAPI extern int mbfl_identify_filter_init(mbfl_identify_filter *filter, enum mbfl_no_encoding encoding);
MBFLAPI extern int mbfl_identify_filter_init2(mbfl_identify_filter *filter, const mbfl_encoding *encoding);

MBFLAPI extern void mbfl_filt_ident_common_ctor(mbfl_identify_filter *filter);
MBFLAPI extern void mbfl_filt_ident_false_ctor(mbfl_identify_filter *filter);

MBFLAPI extern int mbfl_filt_ident_false(int c, mbfl_identify_filter *filter);
MBFLAPI extern int mbfl_filt_ident_true(int c, mbfl_identify_filter *filter);

#endif /* MBFL_IDENT_H */
