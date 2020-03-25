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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "mbfl_allocators.h"

static void *__mbfl__malloc(size_t);
static void *__mbfl__realloc(void *, size_t);
static void *__mbfl__calloc(size_t, size_t);
static void __mbfl__free(void *);

static mbfl_allocators default_allocators = {
	__mbfl__malloc,
	__mbfl__realloc,
	__mbfl__calloc,
	__mbfl__free,
	__mbfl__malloc,
	__mbfl__realloc,
	__mbfl__free
};

mbfl_allocators *__mbfl_allocators = &default_allocators;

static void *__mbfl__malloc(size_t sz)
{
	return malloc(sz);
}

static void *__mbfl__realloc(void *ptr, size_t sz)
{
	return realloc(ptr, sz);
}

static void *__mbfl__calloc(size_t nelems, size_t szelem)
{
	return calloc(nelems, szelem);
}

static void __mbfl__free(void *ptr)
{
	free(ptr);
}
