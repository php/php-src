/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2018, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2018, Sebastian Pipping <sebastian@pipping.org>
 * All rights reserved.
 *
 * Redistribution and use in source  and binary forms, with or without
 * modification, are permitted provided  that the following conditions
 * are met:
 *
 *     1. Redistributions  of  source  code   must  retain  the  above
 *        copyright notice, this list  of conditions and the following
 *        disclaimer.
 *
 *     2. Redistributions  in binary  form  must  reproduce the  above
 *        copyright notice, this list  of conditions and the following
 *        disclaimer  in  the  documentation  and/or  other  materials
 *        provided with the distribution.
 *
 *     3. Neither the  name of the  copyright holder nor the  names of
 *        its contributors may be used  to endorse or promote products
 *        derived from  this software  without specific  prior written
 *        permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING, BUT NOT
 * LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
 * FOR  A  PARTICULAR  PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL
 * THE  COPYRIGHT HOLDER  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA,  OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file UriMemory.c
 * Holds memory manager implementation.
 */

#include "UriConfig.h"  /* for HAVE_REALLOCARRAY */

#ifdef HAVE_REALLOCARRAY
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
# endif
# ifdef __NetBSD__
#  define _OPENBSD_SOURCE 1
# endif
#endif

#include <errno.h>
#include <stdlib.h>



#ifndef URI_DOXYGEN
# include "UriMemory.h"
#endif



#define URI_CHECK_ALLOC_OVERFLOW(total_size, nmemb, size) \
		do { \
			/* check for unsigned overflow */ \
			if ((nmemb != 0) && (total_size / nmemb != size)) { \
				errno = ENOMEM; \
				return NULL; \
			} \
		} while (0)



static void * uriDefaultMalloc(UriMemoryManager * URI_UNUSED(memory),
		size_t size) {
	return malloc(size);
}



static void * uriDefaultCalloc(UriMemoryManager * URI_UNUSED(memory),
		size_t nmemb, size_t size) {
	return calloc(nmemb, size);
}



static void * uriDefaultRealloc(UriMemoryManager * URI_UNUSED(memory),
		void * ptr, size_t size) {
	return realloc(ptr, size);
}



static void * uriDefaultReallocarray(UriMemoryManager * URI_UNUSED(memory),
		void * ptr, size_t nmemb, size_t size) {
#ifdef HAVE_REALLOCARRAY
	return reallocarray(ptr, nmemb, size);
#else
	const size_t total_size = nmemb * size;

	URI_CHECK_ALLOC_OVERFLOW(total_size, nmemb, size);  /* may return */

	return realloc(ptr, total_size);
#endif
}



static void uriDefaultFree(UriMemoryManager * URI_UNUSED(memory),
		void * ptr) {
	free(ptr);
}



UriBool uriMemoryManagerIsComplete(const UriMemoryManager * memory) {
	return (memory
			&& memory->malloc
			&& memory->calloc
			&& memory->realloc
			&& memory->reallocarray
			&& memory->free) ? URI_TRUE : URI_FALSE;
}



void * uriEmulateCalloc(UriMemoryManager * memory, size_t nmemb, size_t size) {
	void * buffer;
	const size_t total_size = nmemb * size;

	if (memory == NULL) {
		errno = EINVAL;
		return NULL;
	}

	URI_CHECK_ALLOC_OVERFLOW(total_size, nmemb, size);  /* may return */

	buffer = memory->malloc(memory, total_size);
	if (buffer == NULL) {
		/* errno set by malloc */
		return NULL;
	}
	memset(buffer, 0, total_size);
	return buffer;
}



void * uriEmulateReallocarray(UriMemoryManager * memory,
		void * ptr, size_t nmemb, size_t size) {
	const size_t total_size = nmemb * size;

	if (memory == NULL) {
		errno = EINVAL;
		return NULL;
	}

	URI_CHECK_ALLOC_OVERFLOW(total_size, nmemb, size);  /* may return */

	return memory->realloc(memory, ptr, total_size);
}



static void * uriDecorateMalloc(UriMemoryManager * memory,
		size_t size) {
	UriMemoryManager * backend;
	const size_t extraBytes = sizeof(size_t);
	void * buffer;

	if (memory == NULL) {
		errno = EINVAL;
		return NULL;
	}

	/* check for unsigned overflow */
	if (size > ((size_t)-1) - extraBytes) {
		errno = ENOMEM;
		return NULL;
	}

	backend = (UriMemoryManager *)memory->userData;
	if (backend == NULL) {
		errno = EINVAL;
		return NULL;
	}

	buffer = backend->malloc(backend, extraBytes + size);
	if (buffer == NULL) {
		return NULL;
	}

	*(size_t *)buffer = size;

	return (char *)buffer + extraBytes;
}



static void * uriDecorateRealloc(UriMemoryManager * memory,
		void * ptr, size_t size) {
	void * newBuffer;
	size_t prevSize;

	if (memory == NULL) {
		errno = EINVAL;
		return NULL;
	}

	/* man realloc: "If ptr is NULL, then the call is equivalent to
	 * malloc(size), for *all* values of size" */
	if (ptr == NULL) {
		return memory->malloc(memory, size);
	}

	/* man realloc: "If size is equal to zero, and ptr is *not* NULL,
	 * then the call is equivalent to free(ptr)." */
	if (size == 0) {
		memory->free(memory, ptr);
		return NULL;
	}

	prevSize = *((size_t *)((char *)ptr - sizeof(size_t)));

	/* Anything to do? */
	if (size <= prevSize) {
		return ptr;
	}

	newBuffer = memory->malloc(memory, size);
	if (newBuffer == NULL) {
		/* errno set by malloc */
		return NULL;
	}

	memcpy(newBuffer, ptr, prevSize);

	memory->free(memory, ptr);

	return newBuffer;
}



static void uriDecorateFree(UriMemoryManager * memory, void * ptr) {
	UriMemoryManager * backend;

	if ((ptr == NULL) || (memory == NULL)) {
		return;
	}

	backend = (UriMemoryManager *)memory->userData;
	if (backend == NULL) {
		return;
	}

	backend->free(backend, (char *)ptr - sizeof(size_t));
}



int uriCompleteMemoryManager(UriMemoryManager * memory,
		UriMemoryManager * backend) {
	if ((memory == NULL) || (backend == NULL)) {
		return URI_ERROR_NULL;
	}

	if ((backend->malloc == NULL) || (backend->free == NULL)) {
		return URI_ERROR_MEMORY_MANAGER_INCOMPLETE;
	}

	memory->calloc = uriEmulateCalloc;
	memory->reallocarray = uriEmulateReallocarray;

	memory->malloc = uriDecorateMalloc;
	memory->realloc = uriDecorateRealloc;
	memory->free = uriDecorateFree;

	memory->userData = backend;

	return URI_SUCCESS;
}



int uriTestMemoryManager(UriMemoryManager * memory) {
	const size_t mallocSize = 7;
	const size_t callocNmemb = 3;
	const size_t callocSize = 5;
	const size_t callocTotalSize = callocNmemb * callocSize;
	const size_t reallocSize = 11;
	const size_t reallocarrayNmemb = 5;
	const size_t reallocarraySize = 7;
	const size_t reallocarrayTotal = reallocarrayNmemb * reallocarraySize;
	size_t index;
	char * buffer;

	if (memory == NULL) {
		return URI_ERROR_NULL;
	}

	if (uriMemoryManagerIsComplete(memory) != URI_TRUE) {
		return URI_ERROR_MEMORY_MANAGER_INCOMPLETE;
	}

	/* malloc + free*/
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[mallocSize - 1] = '\xF1';
	memory->free(memory, buffer);
	buffer = NULL;

	/* calloc + free */
	buffer = memory->calloc(memory, callocNmemb, callocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	for (index = 0; index < callocTotalSize; index++) {  /* all zeros? */
		if (buffer[index] != '\0') {
			return URI_ERROR_MEMORY_MANAGER_FAULTY;
		}
	}
	buffer[callocTotalSize - 1] = '\xF2';
	memory->free(memory, buffer);
	buffer = NULL;

	/* malloc + realloc + free */
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	for (index = 0; index < mallocSize; index++) {
		buffer[index] = '\xF3';
	}
	buffer = memory->realloc(memory, buffer, reallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	for (index = 0; index < mallocSize; index++) {  /* previous content? */
		if (buffer[index] != '\xF3') {
			return URI_ERROR_MEMORY_MANAGER_FAULTY;
		}
	}
	buffer[reallocSize - 1] = '\xF4';
	memory->free(memory, buffer);
	buffer = NULL;

	/* malloc + realloc ptr!=NULL size==0 (equals free) */
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[mallocSize - 1] = '\xF5';
	memory->realloc(memory, buffer, 0);
	buffer = NULL;

	/* realloc ptr==NULL size!=0 (equals malloc) + free */
	buffer = memory->realloc(memory, NULL, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[mallocSize - 1] = '\xF6';
	memory->free(memory, buffer);
	buffer = NULL;

	/* realloc ptr==NULL size==0 (equals malloc) + free */
	buffer = memory->realloc(memory, NULL, 0);
	if (buffer != NULL) {
		memory->free(memory, buffer);
		buffer = NULL;
	}

	/* malloc + reallocarray + free */
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	for (index = 0; index < mallocSize; index++) {
		buffer[index] = '\xF7';
	}
	buffer = memory->reallocarray(memory, buffer, reallocarrayNmemb,
			reallocarraySize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	for (index = 0; index < mallocSize; index++) {  /* previous content? */
		if (buffer[index] != '\xF7') {
			return URI_ERROR_MEMORY_MANAGER_FAULTY;
		}
	}
	buffer[reallocarrayTotal - 1] = '\xF8';
	memory->free(memory, buffer);
	buffer = NULL;

	/* malloc + reallocarray ptr!=NULL nmemb==0 size!=0 (equals free) */
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[mallocSize - 1] = '\xF9';
	memory->reallocarray(memory, buffer, 0, reallocarraySize);
	buffer = NULL;

	/* malloc + reallocarray ptr!=NULL nmemb!=0 size==0 (equals free) */
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[mallocSize - 1] = '\xFA';
	memory->reallocarray(memory, buffer, reallocarrayNmemb, 0);
	buffer = NULL;

	/* malloc + reallocarray ptr!=NULL nmemb==0 size==0 (equals free) */
	buffer = memory->malloc(memory, mallocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[mallocSize - 1] = '\xFB';
	memory->reallocarray(memory, buffer, 0, 0);
	buffer = NULL;

	/* reallocarray ptr==NULL nmemb!=0 size!=0 (equals malloc) + free */
	buffer = memory->reallocarray(memory, NULL, callocNmemb, callocSize);
	if (buffer == NULL) {
		return URI_ERROR_MEMORY_MANAGER_FAULTY;
	}
	buffer[callocTotalSize - 1] = '\xFC';
	memory->free(memory, buffer);
	buffer = NULL;

	/* reallocarray ptr==NULL nmemb==0 size!=0 (equals malloc) + free */
	buffer = memory->reallocarray(memory, NULL, 0, callocSize);
	if (buffer != NULL) {
		memory->free(memory, buffer);
		buffer = NULL;
	}

	/* reallocarray ptr==NULL nmemb!=0 size==0 (equals malloc) + free */
	buffer = memory->reallocarray(memory, NULL, callocNmemb, 0);
	if (buffer != NULL) {
		memory->free(memory, buffer);
		buffer = NULL;
	}

	/* reallocarray ptr==NULL nmemb==0 size==0 (equals malloc) + free */
	buffer = memory->reallocarray(memory, NULL, 0, 0);
	if (buffer != NULL) {
		memory->free(memory, buffer);
		buffer = NULL;
	}

	return URI_SUCCESS;
}



/*extern*/ UriMemoryManager defaultMemoryManager = {
	uriDefaultMalloc,
	uriDefaultCalloc,
	uriDefaultRealloc,
	uriDefaultReallocarray,
	uriDefaultFree,
	NULL  /* userData */
};
