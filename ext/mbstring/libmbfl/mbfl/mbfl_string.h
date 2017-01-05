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

#ifndef MBFL_STRING_H
#define MBFL_STRING_H

#include "mbfl_defs.h"
#include "mbfl_encoding.h"
#include "mbfl_language.h"

/*
 * string object
 */
typedef struct _mbfl_string {
	enum mbfl_no_language no_language;
	enum mbfl_no_encoding no_encoding;
	unsigned char *val;
	unsigned int len;
} mbfl_string;

MBFLAPI extern void mbfl_string_init(mbfl_string *string);
MBFLAPI extern void mbfl_string_init_set(mbfl_string *string, mbfl_language_id no_language, mbfl_encoding_id no_encoding);
MBFLAPI extern void mbfl_string_clear(mbfl_string *string);

#ifndef NULL
#define NULL 0
#endif

#endif /* MBFL_STRING_H */
