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

#ifndef MBFL_MEMORY_DEVICE_H
#define MBFL_MEMORY_DEVICE_H

#include "mbfl_defs.h"
#include "mbfl_string.h"

#define MBFL_MEMORY_DEVICE_ALLOC_SIZE	64

typedef struct _mbfl_memory_device {
	unsigned char *buffer;
	size_t length;
	size_t pos;
	size_t allocsz;
} mbfl_memory_device;

typedef struct _mbfl_wchar_device {
	unsigned int *buffer;
	size_t length;
	size_t pos;
	size_t allocsz;
} mbfl_wchar_device;

MBFLAPI extern void mbfl_memory_device_init(
		mbfl_memory_device *device, size_t initsz, size_t allocsz);
MBFLAPI extern void mbfl_memory_device_realloc(
		mbfl_memory_device *device, size_t initsz, size_t allocsz);
MBFLAPI extern void mbfl_memory_device_clear(mbfl_memory_device *device);
MBFLAPI extern void mbfl_memory_device_reset(mbfl_memory_device *device);
MBFLAPI extern mbfl_string * mbfl_memory_device_result(
		mbfl_memory_device *device, mbfl_string *result);
MBFLAPI extern void mbfl_memory_device_unput(mbfl_memory_device *device);
MBFLAPI extern int mbfl_memory_device_output(int c, void *data);
MBFLAPI extern int mbfl_memory_device_output2(int c, void *data);
MBFLAPI extern int mbfl_memory_device_output4(int c, void *data);
MBFLAPI extern int mbfl_memory_device_strcat(mbfl_memory_device *device, const char *psrc);
MBFLAPI extern int mbfl_memory_device_strncat(
		mbfl_memory_device *device, const char *psrc, size_t len);
MBFLAPI extern int mbfl_memory_device_devcat(mbfl_memory_device *dest, mbfl_memory_device *src);

MBFLAPI extern void mbfl_wchar_device_init(mbfl_wchar_device *device);
MBFLAPI extern int mbfl_wchar_device_output(int c, void *data);
MBFLAPI extern void mbfl_wchar_device_clear(mbfl_wchar_device *device);



#endif /* MBFL_MEMORY_DEVICE_H */
