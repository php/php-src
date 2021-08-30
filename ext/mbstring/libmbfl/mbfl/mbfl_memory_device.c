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

void mbfl_memory_device_realloc(mbfl_memory_device *device, size_t initsz, size_t allocsz)
{
	if (initsz > device->length) {
		device->buffer = erealloc(device->buffer, initsz);
		device->length = initsz;
	}
	device->allocsz = MAX(allocsz, MBFL_MEMORY_DEVICE_ALLOC_SIZE);
}

void mbfl_memory_device_clear(mbfl_memory_device *device)
{
	if (device->buffer) {
		efree(device->buffer);
	}
	device->buffer = NULL;
	device->length = device->pos = 0;
}

void mbfl_memory_device_reset(mbfl_memory_device *device)
{
	device->pos = 0;
}

void mbfl_memory_device_unput(mbfl_memory_device *device)
{
	if (device->pos > 0) {
		device->pos--;
	}
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

int mbfl_memory_device_strcat(mbfl_memory_device *device, const char *psrc)
{
	return mbfl_memory_device_strncat(device, psrc, strlen(psrc));
}

int mbfl_memory_device_strncat(mbfl_memory_device *device, const char *psrc, size_t len)
{
	if (len > device->length - device->pos) {
		/* reallocate buffer */

		if (len > SIZE_MAX - MBFL_MEMORY_DEVICE_ALLOC_SIZE
				|| device->length > SIZE_MAX - (len + MBFL_MEMORY_DEVICE_ALLOC_SIZE)) {
			/* overflow */
			return -1;
		}

		size_t newlen = device->length + len + MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		device->buffer = erealloc(device->buffer, newlen);
		device->length = newlen;
	}

	unsigned char *w = &device->buffer[device->pos];
	memcpy(w, psrc, len);
	device->pos += len;

	return 0;
}

int mbfl_memory_device_devcat(mbfl_memory_device *dest, mbfl_memory_device *src)
{
	return mbfl_memory_device_strncat(dest, (const char*)src->buffer, src->pos);
}

void mbfl_wchar_device_init(mbfl_wchar_device *device)
{
	device->buffer = NULL;
	device->length = 0;
	device->pos = 0;
	device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
}

void mbfl_wchar_device_clear(mbfl_wchar_device *device)
{
	if (device->buffer) {
		efree(device->buffer);
	}
	device->buffer = NULL;
	device->length = device->pos = 0;
}

int mbfl_wchar_device_output(int c, void *data)
{
	mbfl_wchar_device *device = (mbfl_wchar_device *)data;

	if (device->pos >= device->length) {
		/* reallocate buffer */
		size_t newlen;

		if (device->length > SIZE_MAX - device->allocsz) {
			/* overflow */
			return -1;
		}

		newlen = device->length + device->allocsz;
		if (newlen > SIZE_MAX / sizeof(int)) {
			/* overflow */
			return -1;
		}

		device->buffer = erealloc(device->buffer, newlen * sizeof(int));
		device->length = newlen;
	}

	device->buffer[device->pos++] = c;
	return 0;
}
