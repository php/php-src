/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_ALLOC_H
#define ZEND_ALLOC_H

#include <stdio.h>

#include "../TSRM/TSRM.h"
#include "zend.h"

#ifndef ZEND_MM_ALIGNMENT
# define ZEND_MM_ALIGNMENT Z_L(8)
# define ZEND_MM_ALIGNMENT_LOG2 Z_L(3)
#elif ZEND_MM_ALIGNMENT < 4
# undef ZEND_MM_ALIGNMENT
# undef ZEND_MM_ALIGNMENT_LOG2
# define ZEND_MM_ALIGNMENT Z_L(4)
# define ZEND_MM_ALIGNMENT_LOG2 Z_L(2)
#endif

#define ZEND_MM_ALIGNMENT_MASK ~(ZEND_MM_ALIGNMENT - Z_L(1))

#define ZEND_MM_ALIGNED_SIZE(size)	(((size) + ZEND_MM_ALIGNMENT - Z_L(1)) & ZEND_MM_ALIGNMENT_MASK)

typedef struct _zend_leak_info {
	void *addr;
	size_t size;
	const char *filename;
	const char *orig_filename;
	uint lineno;
	uint orig_lineno;
} zend_leak_info;

BEGIN_EXTERN_C()

ZEND_API char*  ZEND_FASTCALL zend_strndup(const char *s, size_t length) ZEND_ATTRIBUTE_MALLOC;

ZEND_API void*  ZEND_FASTCALL _emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(1);
ZEND_API void*  ZEND_FASTCALL _safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API void*  ZEND_FASTCALL _safe_malloc(size_t nmemb, size_t size, size_t offset) ZEND_ATTRIBUTE_MALLOC;
ZEND_API void   ZEND_FASTCALL _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void*  ZEND_FASTCALL _ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE2(1,2);
ZEND_API void*  ZEND_FASTCALL _erealloc(void *ptr, size_t size, int allow_failure ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_ALLOC_SIZE(2);
ZEND_API void*  ZEND_FASTCALL _safe_erealloc(void *ptr, size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void*  ZEND_FASTCALL _safe_realloc(void *ptr, size_t nmemb, size_t size, size_t offset);
ZEND_API char*  ZEND_FASTCALL _estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API char*  ZEND_FASTCALL _estrndup(const char *s, size_t length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API size_t ZEND_FASTCALL _zend_mem_block_size(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);

#include "zend_alloc_sizes.h"

/* _emalloc() & _efree() specialization */
#if !ZEND_DEBUG && !defined(_WIN32)

# define _ZEND_BIN_ALLOCATOR_DEF(_num, _size, _elements, _pages, x, y) \
	ZEND_API void* ZEND_FASTCALL _emalloc_  ## _size(void) ZEND_ATTRIBUTE_MALLOC;

ZEND_MM_BINS_INFO(_ZEND_BIN_ALLOCATOR_DEF, x, y)

ZEND_API void* ZEND_FASTCALL _emalloc_large(size_t size) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(1);
ZEND_API void* ZEND_FASTCALL _emalloc_huge(size_t size) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(1);

# define _ZEND_BIN_ALLOCATOR_SELECTOR_START(_num, _size, _elements, _pages, size, y) \
	((size <= _size) ? _emalloc_ ## _size() :
# define _ZEND_BIN_ALLOCATOR_SELECTOR_END(_num, _size, _elements, _pages, size, y) \
	)

# define ZEND_ALLOCATOR(size) \
	ZEND_MM_BINS_INFO(_ZEND_BIN_ALLOCATOR_SELECTOR_START, size, y) \
	((size <= ZEND_MM_MAX_LARGE_SIZE) ? _emalloc_large(size) : _emalloc_huge(size)) \
	ZEND_MM_BINS_INFO(_ZEND_BIN_ALLOCATOR_SELECTOR_END, size, y)

# define _emalloc(size) \
	(__builtin_constant_p(size) ? \
		ZEND_ALLOCATOR(size) \
	: \
		_emalloc(size) \
	)

# define _ZEND_BIN_DEALLOCATOR_DEF(_num, _size, _elements, _pages, x, y) \
	ZEND_API void ZEND_FASTCALL _efree_ ## _size(void *);

ZEND_MM_BINS_INFO(_ZEND_BIN_DEALLOCATOR_DEF, x, y)

ZEND_API void ZEND_FASTCALL _efree_large(void *, size_t size);
ZEND_API void ZEND_FASTCALL _efree_huge(void *, size_t size);

# define _ZEND_BIN_DEALLOCATOR_SELECTOR_START(_num, _size, _elements, _pages, ptr, size) \
	if (size <= _size) { _efree_ ## _size(ptr); } else

# define ZEND_DEALLOCATOR(ptr, size) \
	ZEND_MM_BINS_INFO(_ZEND_BIN_DEALLOCATOR_SELECTOR_START, ptr, size) \
	if (size <= ZEND_MM_MAX_LARGE_SIZE) { _efree_large(ptr, size); } \
	else { _efree_huge(ptr, size); }

# define efree_size(ptr, size) do { \
		if (__builtin_constant_p(size)) { \
			ZEND_DEALLOCATOR(ptr, size) \
		} else { \
			_efree(ptr); \
		} \
	} while (0)
# define efree_size_rel(ptr, size) \
	efree_size(ptr, size)

#else

# define efree_size(ptr, size) \
	efree(ptr)
# define efree_size_rel(ptr, size) \
	efree_rel(ptr)

#define _emalloc_large _emalloc
#define _emalloc_huge  _emalloc
#define _efree_large   _efree
#define _efree_huge    _efree

#endif

/* Standard wrapper macros */
#define emalloc(size)						_emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define emalloc_large(size)					_emalloc_large((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define emalloc_huge(size)					_emalloc_huge((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define safe_emalloc(nmemb, size, offset)	_safe_emalloc((nmemb), (size), (offset) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define efree(ptr)							_efree((ptr) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define efree_large(ptr)					_efree_large((ptr) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define efree_huge(ptr)						_efree_huge((ptr) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define ecalloc(nmemb, size)				_ecalloc((nmemb), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define erealloc(ptr, size)					_erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define safe_erealloc(ptr, nmemb, size, offset)	_safe_erealloc((ptr), (nmemb), (size), (offset) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define erealloc_recoverable(ptr, size)		_erealloc((ptr), (size), 1 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define estrdup(s)							_estrdup((s) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define estrndup(s, length)					_estrndup((s), (length) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mem_block_size(ptr)			_zend_mem_block_size((ptr) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

/* Relay wrapper macros */
#define emalloc_rel(size)						_emalloc((size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define safe_emalloc_rel(nmemb, size, offset)	_safe_emalloc((nmemb), (size), (offset) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define efree_rel(ptr)							_efree((ptr) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define ecalloc_rel(nmemb, size)				_ecalloc((nmemb), (size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define erealloc_rel(ptr, size)					_erealloc((ptr), (size), 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define erealloc_recoverable_rel(ptr, size)		_erealloc((ptr), (size), 1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define safe_erealloc_rel(ptr, nmemb, size, offset)	_safe_erealloc((ptr), (nmemb), (size), (offset) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define estrdup_rel(s)							_estrdup((s) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define estrndup_rel(s, length)					_estrndup((s), (length) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mem_block_size_rel(ptr)			_zend_mem_block_size((ptr) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)

zend_always_inline static void * __zend_malloc(size_t len)
{
	void *tmp = malloc(len);
	if (tmp) {
		return tmp;
	}
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

zend_always_inline static void * __zend_calloc(size_t nmemb, size_t len)
{
	void *tmp = _safe_malloc(nmemb, len, 0);
	memset(tmp, 0, nmemb * len);
	return tmp;
}

zend_always_inline static void * __zend_realloc(void *p, size_t len)
{
	p = realloc(p, len);
	if (p) {
		return p;
	}
	fprintf(stderr, "Out of memory\n");
	exit(1);
}


/* Selective persistent/non persistent allocation macros */
#define pemalloc(size, persistent) ((persistent)?__zend_malloc(size):emalloc(size))
#define safe_pemalloc(nmemb, size, offset, persistent)	((persistent)?_safe_malloc(nmemb, size, offset):safe_emalloc(nmemb, size, offset))
#define pefree(ptr, persistent)  ((persistent)?free(ptr):efree(ptr))
#define pefree_size(ptr, size, persistent)  ((persistent)?free(ptr):efree_size(ptr, size))
#define pecalloc(nmemb, size, persistent) ((persistent)?__zend_calloc((nmemb), (size)):ecalloc((nmemb), (size)))
#define perealloc(ptr, size, persistent) ((persistent)?__zend_realloc((ptr), (size)):erealloc((ptr), (size)))
#define safe_perealloc(ptr, nmemb, size, offset, persistent)	((persistent)?_safe_realloc((ptr), (nmemb), (size), (offset)):safe_erealloc((ptr), (nmemb), (size), (offset)))
#define perealloc_recoverable(ptr, size, persistent) ((persistent)?__zend_realloc((ptr), (size)):erealloc_recoverable((ptr), (size)))
#define pestrdup(s, persistent) ((persistent)?strdup(s):estrdup(s))
#define pestrndup(s, length, persistent) ((persistent)?zend_strndup((s),(length)):estrndup((s),(length)))

#define pemalloc_rel(size, persistent) ((persistent)?__zend_malloc(size):emalloc_rel(size))
#define pefree_rel(ptr, persistent)	((persistent)?free(ptr):efree_rel(ptr))
#define pecalloc_rel(nmemb, size, persistent) ((persistent)?__zend_calloc((nmemb), (size)):ecalloc_rel((nmemb), (size)))
#define perealloc_rel(ptr, size, persistent) ((persistent)?__zend_realloc((ptr), (size)):erealloc_rel((ptr), (size)))
#define perealloc_recoverable_rel(ptr, size, persistent) ((persistent)?__zend_realloc((ptr), (size)):erealloc_recoverable_rel((ptr), (size)))
#define pestrdup_rel(s, persistent) ((persistent)?strdup(s):estrdup_rel(s))

ZEND_API int zend_set_memory_limit(size_t memory_limit);

ZEND_API void start_memory_manager(void);
ZEND_API void shutdown_memory_manager(int silent, int full_shutdown);
ZEND_API int is_zend_mm(void);

ZEND_API size_t zend_memory_usage(int real_usage);
ZEND_API size_t zend_memory_peak_usage(int real_usage);

/* fast cache for HashTables */
#define ALLOC_HASHTABLE(ht)	\
	(ht) = (HashTable *) emalloc(sizeof(HashTable))

#define FREE_HASHTABLE(ht)	\
	efree_size(ht, sizeof(HashTable))

#define ALLOC_HASHTABLE_REL(ht)	\
	(ht) = (HashTable *) emalloc_rel(sizeof(HashTable))

#define FREE_HASHTABLE_REL(ht)	\
	efree_size_rel(ht, sizeof(HashTable))

/* Heap functions */
typedef struct _zend_mm_heap zend_mm_heap;

ZEND_API zend_mm_heap *zend_mm_startup(void);
ZEND_API void zend_mm_shutdown(zend_mm_heap *heap, int full_shutdown, int silent);
ZEND_API void*  ZEND_FASTCALL _zend_mm_alloc(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API void   ZEND_FASTCALL _zend_mm_free(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void*  ZEND_FASTCALL _zend_mm_realloc(zend_mm_heap *heap, void *p, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API size_t ZEND_FASTCALL _zend_mm_block_size(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);

#define zend_mm_alloc(heap, size)			_zend_mm_alloc((heap), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mm_free(heap, p)				_zend_mm_free((heap), (p) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mm_realloc(heap, p, size)		_zend_mm_realloc((heap), (p), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mm_block_size(heap, p)			_zend_mm_block_size((heap), (p) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

#define zend_mm_alloc_rel(heap, size)		_zend_mm_alloc((heap), (size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mm_free_rel(heap, p)			_zend_mm_free((heap), (p) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mm_realloc_rel(heap, p, size)	_zend_mm_realloc((heap), (p), (size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mm_block_size_rel(heap, p)		_zend_mm_block_size((heap), (p) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

ZEND_API zend_mm_heap *zend_mm_set_heap(zend_mm_heap *new_heap);

ZEND_API void zend_mm_set_custom_handlers(zend_mm_heap *heap,
                                          void* (*_malloc)(size_t),
                                          void  (*_free)(void*),
                                          void* (*_realloc)(void*, size_t));
ZEND_API void zend_mm_get_custom_handlers(zend_mm_heap *heap,
                                          void* (**_malloc)(size_t),
                                          void  (**_free)(void*),
                                          void* (**_realloc)(void*, size_t));

typedef struct _zend_mm_storage zend_mm_storage;

typedef	void* (*zend_mm_chunk_alloc_t)(zend_mm_storage *storage, size_t size, size_t alignment);
typedef void  (*zend_mm_chunk_free_t)(zend_mm_storage *storage, void *chunk, size_t size);
typedef void  (*zend_mm_chunk_truncate_t)(zend_mm_storage *storage, void *chunk, size_t old_size, size_t new_size);
typedef void  (*zend_mm_storage_dtor_t)(zend_mm_storage *storage);

struct _zend_mm_storage {
	zend_mm_chunk_alloc_t       chunk_alloc;
	zend_mm_chunk_free_t        chunk_free;
	zend_mm_chunk_truncate_t    chunk_truncate;
	zend_mm_storage_dtor_t      dtor;
};

ZEND_API zend_mm_storage *zend_mm_get_storage(zend_mm_heap *heap);
ZEND_API zend_mm_heap *zend_mm_startup_ex(zend_mm_storage *storage);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
