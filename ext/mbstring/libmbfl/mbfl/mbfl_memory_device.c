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

#include <stddef.h>
#include <string.h>

#include "zend.h"
#include "mbfl_memory_device.h"

/*
 * memory device output functions
 */
void mbfl_memory_device_init(mbfl_memory_device *device, size_t initsz, size_t allocsz)
{
	device->buffer = (initsz > 0) ? emalloc(initsz) : NULL;
	device->length = initsz;
	device->pos = 0;
	device->allocsz = MAX(allocsz, MBFL_MEMORY_DEVICE_ALLOC_SIZE);
}

mbfl_string* mbfl_memory_device_result(mbfl_memory_device *device, mbfl_string *result)
{
	result->len = device->pos;
	mbfl_memory_device_output('\0', device);
	result->val = device->buffer;
	device->buffer = NULL;
	device->length = device->pos = 0;
	return result;
}

int mbfl_memory_device_output(int c, void *data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if (device->pos >= device->length) {
		/* reallocate buffer */

		if (device->length > SIZE_MAX - device->allocsz) {
			/* overflow */
			return -1;
		}

		size_t newlen = device->length + device->allocsz;
		device->buffer = erealloc(device->buffer, newlen);
		device->length = newlen;
	}

	device->buffer[device->pos++] = (unsigned char)c;
	return 0;
}
