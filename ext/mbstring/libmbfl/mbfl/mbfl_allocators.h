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

#ifndef MBFL_ALLOCATORS_H
#define MBFL_ALLOCATORS_H

#include <stddef.h>
#include "mbfl_defs.h"

/* All allocation functions are required to be infallible.
 * That is, they must never return NULL. */

typedef struct _mbfl_allocators {
	void *(*malloc)(size_t);
	void *(*realloc)(void *, size_t);
	void *(*calloc)(size_t, size_t);
	void (*free)(void *);
} mbfl_allocators;

MBFLAPI extern mbfl_allocators *__mbfl_allocators;

#define mbfl_malloc (__mbfl_allocators->malloc)
#define mbfl_realloc (__mbfl_allocators->realloc)
#define mbfl_calloc (__mbfl_allocators->calloc)
#define mbfl_free (__mbfl_allocators->free)

#endif /* MBFL_ALLOCATORS_H */
