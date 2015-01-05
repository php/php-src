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

#include "mbfl_allocators.h"
#include "mbfl_string.h"
#include "mbfl_memory_device.h"

/*
 * memory device output functions
 */
void
mbfl_memory_device_init(mbfl_memory_device *device, int initsz, int allocsz)
{
	if (device) {
		device->length = 0;
		device->buffer = (unsigned char *)0;
		if (initsz > 0) {
			device->buffer = (unsigned char *)mbfl_malloc(initsz*sizeof(unsigned char));
			if (device->buffer != NULL) {
				device->length = initsz;
			}
		}
		device->pos= 0;
		if (allocsz > MBFL_MEMORY_DEVICE_ALLOC_SIZE) {
			device->allocsz = allocsz;
		} else {
			device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		}
	}
}

void
mbfl_memory_device_realloc(mbfl_memory_device *device, int initsz, int allocsz)
{
	unsigned char *tmp;

	if (device) {
		if (initsz > device->length) {
			tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, initsz*sizeof(unsigned char));
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
		device->buffer = (unsigned char *)0;
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
		mbfl_memory_device_output4('\0', device);
		result->val = device->buffer;
		device->buffer = (unsigned char *)0;
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
		int newlen;
		unsigned char *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
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

	if ((device->pos + 2) >= device->length) {
		/* reallocate buffer */
		int newlen;
		unsigned char *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
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

	if ((device->pos + 4) >= device->length) {
		/* reallocate buffer */
		int newlen;
		unsigned char *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
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
	int len;
	unsigned char *w;
	const unsigned char *p;

	len = 0;
	p = (const unsigned char*)psrc;
	while (*p) {
		p++;
		len++;
	}

	if ((device->pos + len) >= device->length) {
		/* reallocate buffer */
		int newlen = device->length + (len + MBFL_MEMORY_DEVICE_ALLOC_SIZE)*sizeof(unsigned char);
		unsigned char *tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	p = (const unsigned char*)psrc;
	w = &device->buffer[device->pos];
	device->pos += len;
	while (len > 0) {
		*w++ = *p++;
		len--;
	}

	return len;
}

int
mbfl_memory_device_strncat(mbfl_memory_device *device, const char *psrc, int len)
{
	unsigned char *w;

	if ((device->pos + len) >= device->length) {
		/* reallocate buffer */
		int newlen = device->length + len + MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		unsigned char *tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	w = &device->buffer[device->pos];
	device->pos += len;
	while (len > 0) {
		*w++ = *psrc++;
		len--;
	}

	return len;
}

int
mbfl_memory_device_devcat(mbfl_memory_device *dest, mbfl_memory_device *src)
{
	int n;
	unsigned char *p, *w;

	if ((dest->pos + src->pos) >= dest->length) {
		/* reallocate buffer */
		int newlen = dest->length + src->pos + MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		unsigned char *tmp = (unsigned char *)mbfl_realloc((void *)dest->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		dest->length = newlen;
		dest->buffer = tmp;
	}

	p = src->buffer;
	w = &dest->buffer[dest->pos];
	n = src->pos;
	dest->pos += n;
	while (n > 0) {
		*w++ = *p++;
		n--;
	}

	return n;
}

void
mbfl_wchar_device_init(mbfl_wchar_device *device)
{
	if (device) {
		device->buffer = (unsigned int *)0;
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
		device->buffer = (unsigned int*)0;
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
		int newlen;
		unsigned int *tmp;

		newlen = device->length + device->allocsz;
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
