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

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include <string.h>
#include "mbfl_allocators.h"
#include "mbfl_string.h"
#include "mbfl_memory_device.h"

/*
 * memory device output functions
 */
void
mbfl_memory_device_init(mbfl_memory_device *device, size_t initsz, size_t allocsz)
{
	if (device) {
		device->length = 0;
		device->buffer = NULL;
		if (initsz > 0) {
			device->buffer = (unsigned char *)mbfl_malloc(initsz);
			if (device->buffer != NULL) {
				device->length = initsz;
			}
		}
		device->pos = 0;
		if (allocsz > MBFL_MEMORY_DEVICE_ALLOC_SIZE) {
			device->allocsz = allocsz;
		} else {
			device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		}
	}
}

void
mbfl_memory_device_realloc(mbfl_memory_device *device, size_t initsz, size_t allocsz)
{
	unsigned char *tmp;

	if (device) {
		if (initsz > device->length) {
			tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, initsz);
			if (tmp != NULL) {
				device->buffer = tmp;
				device->length = initsz;
			}
		}
		if (allocsz > MBFL_MEMORY_DEVICE_ALLOC_SIZE) {
			device->allocsz = allocsz;
		} else {
			device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		}
	}
}

void
mbfl_memory_device_clear(mbfl_memory_device *device)
{
	if (device) {
		if (device->buffer) {
			mbfl_free(device->buffer);
		}
		device->buffer = NULL;
		device->length = 0;
		device->pos = 0;
	}
}

void
mbfl_memory_device_reset(mbfl_memory_device *device)
{
	if (device) {
		device->pos = 0;
	}
}

void
mbfl_memory_device_unput(mbfl_memory_device *device)
{
	if (device->pos > 0) {
		device->pos--;
	}
}

mbfl_string *
mbfl_memory_device_result(mbfl_memory_device *device, mbfl_string *result)
{
	if (device && result) {
		result->len = device->pos;
		mbfl_memory_device_output('\0', device);
		result->val = device->buffer;
		device->buffer = NULL;
		device->length = 0;
		device->pos= 0;
		if (result->val == NULL) {
			result->len = 0;
			result = NULL;
		}
	} else {
		result = NULL;
	}

	return result;
}

int
mbfl_memory_device_output(int c, void *data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if (device->pos >= device->length) {
		/* reallocate buffer */
		size_t newlen;
		unsigned char *tmp;

		if (device->length > SIZE_MAX - device->allocsz) {
			/* overflow */
			return -1;
		}

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen);
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = (unsigned char)c;
	return c;
}

int
mbfl_memory_device_output2(int c, void *data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if (2 > device->length - device->pos) {
		/* reallocate buffer */
		size_t newlen;
		unsigned char *tmp;

		if (device->length > SIZE_MAX - device->allocsz) {
			/* overflow */
			return -1;
		}

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen);
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = (unsigned char)((c >> 8) & 0xff);
	device->buffer[device->pos++] = (unsigned char)(c & 0xff);

	return c;
}

int
mbfl_memory_device_output4(int c, void* data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if (4 > device->length - device->pos) {
		/* reallocate buffer */
		size_t newlen;
		unsigned char *tmp;

		if (device->length > SIZE_MAX - device->allocsz) {
			/* overflow */
			return -1;
		}

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen);
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = (unsigned char)((c >> 24) & 0xff);
	device->buffer[device->pos++] = (unsigned char)((c >> 16) & 0xff);
	device->buffer[device->pos++] = (unsigned char)((c >> 8) & 0xff);
	device->buffer[device->pos++] = (unsigned char)(c & 0xff);

	return c;
}

int
mbfl_memory_device_strcat(mbfl_memory_device *device, const char *psrc)
{
	return mbfl_memory_device_strncat(device, psrc, strlen(psrc));
}

int
mbfl_memory_device_strncat(mbfl_memory_device *device, const char *psrc, size_t len)
{
	unsigned char *w;

	if (len > device->length - device->pos) {
		/* reallocate buffer */
		size_t newlen;
		unsigned char *tmp;

		if (len > SIZE_MAX - MBFL_MEMORY_DEVICE_ALLOC_SIZE
				|| device->length > SIZE_MAX - (len + MBFL_MEMORY_DEVICE_ALLOC_SIZE)) {
			/* overflow */
			return -1;
		}

		newlen = device->length + len + MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen);
		if (tmp == NULL) {
			return -1;
		}

		device->length = newlen;
		device->buffer = tmp;
	}

	w = &device->buffer[device->pos];
	memcpy(w, psrc, len);
	device->pos += len;

	return 0;
}

int
mbfl_memory_device_devcat(mbfl_memory_device *dest, mbfl_memory_device *src)
{
	return mbfl_memory_device_strncat(dest, (const char *) src->buffer, src->pos);
}

void
mbfl_wchar_device_init(mbfl_wchar_device *device)
{
	if (device) {
		device->buffer = NULL;
		device->length = 0;
		device->pos= 0;
		device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
	}
}

void
mbfl_wchar_device_clear(mbfl_wchar_device *device)
{
	if (device) {
		if (device->buffer) {
			mbfl_free(device->buffer);
		}
		device->buffer = NULL;
		device->length = 0;
		device->pos = 0;
	}
}

int
mbfl_wchar_device_output(int c, void *data)
{
	mbfl_wchar_device *device = (mbfl_wchar_device *)data;

	if (device->pos >= device->length) {
		/* reallocate buffer */
		size_t newlen;
		unsigned int *tmp;

		if (device->length > SIZE_MAX - device->allocsz) {
			/* overflow */
			return -1;
		}

		newlen = device->length + device->allocsz;
		if (newlen > SIZE_MAX / sizeof(int)) {
			/* overflow */
			return -1;
		}

		tmp = (unsigned int *)mbfl_realloc((void *)device->buffer, newlen*sizeof(int));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = c;

	return c;
}
