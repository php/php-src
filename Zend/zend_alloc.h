/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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
# define ZEND_MM_ALIGNMENT 8
# define ZEND_MM_ALIGNMENT_LOG2 3
#elif ZEND_MM_ALIGNMENT < 4
# undef ZEND_MM_ALIGNMENT
# undef ZEND_MM_ALIGNMENT_LOG2
# define ZEND_MM_ALIGNMENT 4
# define ZEND_MM_ALIGNMENT_LOG2 2
#endif

#define ZEND_MM_ALIGNMENT_MASK ~(ZEND_MM_ALIGNMENT-1)

#define ZEND_MM_ALIGNED_SIZE(size)	(((size) + ZEND_MM_ALIGNMENT - 1) & ZEND_MM_ALIGNMENT_MASK)

typedef struct _zend_leak_info {
	void *addr;
	size_t size;
	const char *filename;
	uint lineno;
	const char *orig_filename;
	uint orig_lineno;
} zend_leak_info;

BEGIN_EXTERN_C()

ZEND_API char *zend_strndup(const char *s, unsigned int length) ZEND_ATTRIBUTE_MALLOC;

ZEND_API void *_emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(1);
ZEND_API void *_safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API void *_safe_malloc(size_t nmemb, size_t size, size_t offset) ZEND_ATTRIBUTE_MALLOC;
ZEND_API void _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void *_ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE2(1,2);
ZEND_API void *_erealloc(void *ptr, size_t size, int allow_failure ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_ALLOC_SIZE(2);
ZEND_API void *_safe_erealloc(void *ptr, size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void *_safe_realloc(void *ptr, size_t nmemb, size_t size, size_t offset);
ZEND_API char *_estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API char *_estrndup(const char *s, unsigned int length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API size_t _zend_mem_block_size(void *ptr TSRMLS_DC ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);

/* Standard wrapper macros */
#define emalloc(size)						_emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define safe_emalloc(nmemb, size, offset)	_safe_emalloc((nmemb), (size), (offset) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define efree(ptr)							_efree((ptr) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define ecalloc(nmemb, size)				_ecalloc((nmemb), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define erealloc(ptr, size)					_erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define safe_erealloc(ptr, nmemb, size, offset)	_safe_erealloc((ptr), (nmemb), (size), (offset) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define erealloc_recoverable(ptr, size)		_erealloc((ptr), (size), 1 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define estrdup(s)							_estrdup((s) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define estrndup(s, length)					_estrndup((s), (length) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mem_block_size(ptr)			_zend_mem_block_size((ptr) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

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
#define zend_mem_block_size_rel(ptr)			_zend_mem_block_size((ptr) TSRMLS_CC ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)

inline static void * __zend_malloc(size_t len)
{
	void *tmp = malloc(len);
	if (tmp) {
		return tmp;
	}
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

inline static void * __zend_calloc(size_t nmemb, size_t len)
{
	void *tmp = _safe_malloc(nmemb, len, 0);
	memset(tmp, 0, nmemb * len);
	return tmp;
}

inline static void * __zend_realloc(void *p, size_t len)
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

#define safe_estrdup(ptr)  ((ptr)?(estrdup(ptr)):STR_EMPTY_ALLOC())
#define safe_estrndup(ptr, len) ((ptr)?(estrndup((ptr), (len))):STR_EMPTY_ALLOC())

ZEND_API int zend_set_memory_limit(size_t memory_limit TSRMLS_DC);

ZEND_API void start_memory_manager(TSRMLS_D);
ZEND_API void shutdown_memory_manager(int silent, int full_shutdown TSRMLS_DC);
ZEND_API int is_zend_mm(TSRMLS_D);

#if ZEND_DEBUG
ZEND_API int _mem_block_check(void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void _full_mem_check(int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
void zend_debug_alloc_output(char *format, ...);
#define mem_block_check(ptr, silent) _mem_block_check(ptr, silent ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define full_mem_check(silent) _full_mem_check(silent ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#else
#define mem_block_check(type, ptr, silent)
#define full_mem_check(silent)
#endif

ZEND_API size_t zend_memory_usage(int real_usage TSRMLS_DC);
ZEND_API size_t zend_memory_peak_usage(int real_usage TSRMLS_DC);

END_EXTERN_C()

/* fast cache for zval's */
#define ALLOC_ZVAL(z)	\
	(z) = (zval *) emalloc(sizeof(zval))

#define FREE_ZVAL(z)	\
	efree_rel(z)

#define ALLOC_ZVAL_REL(z)	\
	(z) = (zval *) emalloc_rel(sizeof(zval))

#define FREE_ZVAL_REL(z)	\
	efree_rel(z)

/* fast cache for HashTables */
#define ALLOC_HASHTABLE(ht)	\
	(ht) = (HashTable *) emalloc(sizeof(HashTable))

#define FREE_HASHTABLE(ht)	\
	efree(ht)

#define ALLOC_HASHTABLE_REL(ht)	\
	(ht) = (HashTable *) emalloc_rel(sizeof(HashTable))

#define FREE_HASHTABLE_REL(ht)	\
	efree_rel(ht)

/* Heap functions */
typedef struct _zend_mm_heap zend_mm_heap;

ZEND_API zend_mm_heap *zend_mm_startup(void);
ZEND_API void zend_mm_shutdown(zend_mm_heap *heap, int full_shutdown, int silent TSRMLS_DC);
ZEND_API void *_zend_mm_alloc(zend_mm_heap *heap, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
ZEND_API void _zend_mm_free(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void *_zend_mm_realloc(zend_mm_heap *heap, void *p, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API size_t _zend_mm_block_size(zend_mm_heap *heap, void *p ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);

#define zend_mm_alloc(heap, size)			_zend_mm_alloc((heap), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mm_free(heap, p)				_zend_mm_free((heap), (p) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mm_realloc(heap, p, size)		_zend_mm_realloc((heap), (p), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define zend_mm_block_size(heap, p)			_zend_mm_block_size((heap), (p) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

#define zend_mm_alloc_rel(heap, size)		_zend_mm_alloc((heap), (size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mm_free_rel(heap, p)			_zend_mm_free((heap), (p) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mm_realloc_rel(heap, p, size)	_zend_mm_realloc((heap), (p), (size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define zend_mm_block_size_rel(heap, p)		_zend_mm_block_size((heap), (p) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

/* Heaps with user defined storage */
typedef struct _zend_mm_storage zend_mm_storage;

typedef struct _zend_mm_segment {
	size_t	size;
	struct _zend_mm_segment *next_segment;
} zend_mm_segment;

typedef struct _zend_mm_mem_handlers {
	const char *name;
	zend_mm_storage* (*init)(void *params);
	void (*dtor)(zend_mm_storage *storage);
	void (*compact)(zend_mm_storage *storage);
	zend_mm_segment* (*_alloc)(zend_mm_storage *storage, size_t size);
	zend_mm_segment* (*_realloc)(zend_mm_storage *storage, zend_mm_segment *ptr, size_t size);
	void (*_free)(zend_mm_storage *storage, zend_mm_segment *ptr);
} zend_mm_mem_handlers;

struct _zend_mm_storage {
	const zend_mm_mem_handlers *handlers;
	void *data;
};

ZEND_API zend_mm_heap *zend_mm_startup_ex(const zend_mm_mem_handlers *handlers, size_t block_size, size_t reserve_size, int internal, void *params);
ZEND_API zend_mm_heap *zend_mm_set_heap(zend_mm_heap *new_heap TSRMLS_DC);
ZEND_API zend_mm_storage *zend_mm_get_storage(zend_mm_heap *heap);

ZEND_API void zend_mm_set_custom_handlers(zend_mm_heap *heap,
                                          void* (*_malloc)(size_t),
                                          void  (*_free)(void*),
                                          void* (*_realloc)(void*, size_t));

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
