/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
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
#include "zend_globals_macros.h"

#include "zend_mm.h"

#define MEM_BLOCK_START_MAGIC	0x7312F8DCL
#define MEM_BLOCK_END_MAGIC		0x2A8FCC84L
#define MEM_BLOCK_FREED_MAGIC	0x99954317L
#define MEM_BLOCK_CACHED_MAGIC	0xFB8277DCL

typedef struct _zend_mem_header {
#if ZEND_DEBUG
	long magic;
	char *filename;
	uint lineno;
	int reported;
	char *orig_filename;
	uint orig_lineno;
# ifdef ZTS
	THREAD_T thread_id;
# endif
#endif
#if ZEND_DEBUG || !defined(ZEND_MM)
    struct _zend_mem_header *pNext;
    struct _zend_mem_header *pLast;
#endif
	unsigned int size:31;
	unsigned int cached:1;
} zend_mem_header;

typedef union _align_test {
	void *ptr;
	double dbl;
	long lng;
} align_test;

#define MAX_CACHED_MEMORY	11
#define MAX_CACHED_ENTRIES	256
#define PRE_INIT_CACHE_ENTRIES	32

#if (defined (__GNUC__) && __GNUC__ >= 2)
#define PLATFORM_ALIGNMENT (__alignof__ (align_test))
#else
#define PLATFORM_ALIGNMENT (sizeof(align_test))
#endif

#define MEM_HEADER_PADDING (((PLATFORM_ALIGNMENT-sizeof(zend_mem_header))%PLATFORM_ALIGNMENT+PLATFORM_ALIGNMENT)%PLATFORM_ALIGNMENT)


BEGIN_EXTERN_C()

ZEND_API char *zend_strndup(const char *s, unsigned int length);

ZEND_API void *_emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void *_ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API void *_erealloc(void *ptr, size_t size, int allow_failure ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API char *_estrdup(const char *s ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
ZEND_API char *_estrndup(const char *s, unsigned int length ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);

/* Standard wrapper macros */
#define emalloc(size)					_emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define efree(ptr)						_efree((ptr) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define ecalloc(nmemb, size)			_ecalloc((nmemb), (size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define erealloc(ptr, size)				_erealloc((ptr), (size), 0 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define erealloc_recoverable(ptr, size)	_erealloc((ptr), (size), 1 ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define estrdup(s)						_estrdup((s) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define estrndup(s, length)				_estrndup((s), (length) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

/* Relay wrapper macros */
#define emalloc_rel(size)					_emalloc((size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define efree_rel(ptr)						_efree((ptr) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define ecalloc_rel(nmemb, size)			_ecalloc((nmemb), (size) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define erealloc_rel(ptr, size)				_erealloc((ptr), (size), 0 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define erealloc_recoverable_rel(ptr, size)	_erealloc((ptr), (size), 1 ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define estrdup_rel(s)						_estrdup((s) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)
#define estrndup_rel(s, length)				_estrndup((s), (length) ZEND_FILE_LINE_RELAY_CC ZEND_FILE_LINE_CC)

/* Selective persistent/non persistent allocation macros */
#define pemalloc(size, persistent) ((persistent)?malloc(size):emalloc(size))
#define pefree(ptr, persistent)  ((persistent)?free(ptr):efree(ptr))
#define pecalloc(nmemb, size, persistent) ((persistent)?calloc((nmemb), (size)):ecalloc((nmemb), (size)))
#define perealloc(ptr, size, persistent) ((persistent)?realloc((ptr), (size)):erealloc((ptr), (size)))
#define perealloc_recoverable(ptr, size, persistent) ((persistent)?realloc((ptr), (size)):erealloc_recoverable((ptr), (size)))
#define pestrdup(s, persistent) ((persistent)?strdup(s):estrdup(s))

#define safe_estrdup(ptr)  ((ptr)?(estrdup(ptr)):(empty_string))
#define safe_estrndup(ptr, len) ((ptr)?(estrndup((ptr), (len))):(empty_string))

ZEND_API int zend_set_memory_limit(unsigned int memory_limit);

ZEND_API void start_memory_manager(TSRMLS_D);
ZEND_API void shutdown_memory_manager(int silent, int clean_cache TSRMLS_DC);

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


END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
