/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#ifndef _ALLOC_H
#define _ALLOC_H

#include <stdio.h>

#define MEM_BLOCK_START_MAGIC	0x7312F8DCL
#define MEM_BLOCK_END_MAGIC		0x2A8FCC84L
#define MEM_BLOCK_FREED_MAGIC	0x99954317L
#define MEM_BLOCK_CACHED_MAGIC	0xFB8277DCL

typedef struct _mem_header {
#if ZEND_DEBUG
	long magic;
	char *filename;
	uint lineno;
#endif
    struct _mem_header *pNext;
    struct _mem_header *pLast;
	unsigned int size:30;
	unsigned int persistent:1;
	unsigned int cached:1;
} mem_header;

typedef union _align_test {
	void *ptr;
	double dbl;
	long lng;
} align_test;

#define MAX_CACHED_MEMORY   64
#define MAX_CACHED_ENTRIES	16

#define PLATFORM_ALIGNMENT (sizeof(align_test))
#define PLATFORM_PADDING (((PLATFORM_ALIGNMENT-sizeof(mem_header))%PLATFORM_ALIGNMENT+PLATFORM_ALIGNMENT)%PLATFORM_ALIGNMENT)

ZEND_API char *zend_strndup(const char *s, unsigned int length);

BEGIN_EXTERN_C()

#if ZEND_DEBUG
ZEND_API void *_emalloc(size_t size,char *filename,uint lineno);
ZEND_API void _efree(void *ptr,char *filename,uint lineno);
ZEND_API void *_ecalloc(size_t nmemb, size_t size,char *filename,uint lineno);
ZEND_API void *_erealloc(void *ptr, size_t size,char *filename,uint lineno);
ZEND_API char *_estrdup(const char *s,char *filename,uint lineno);
ZEND_API char *_estrndup(const char *s, unsigned int length,char *filename,uint lineno);
ZEND_API void _persist_alloc(void *ptr, char *filename, uint lineno);
#define emalloc(size)			_emalloc((size),__FILE__,__LINE__)
#define efree(ptr)				_efree((ptr),__FILE__,__LINE__)
#define ecalloc(nmemb,size)		_ecalloc((nmemb),(size),__FILE__,__LINE__)
#define erealloc(ptr,size)		_erealloc((ptr),(size),__FILE__,__LINE__)
#define estrdup(s)				_estrdup((s),__FILE__,__LINE__)
#define estrndup(s,length)		_estrndup((s),(length),__FILE__,__LINE__)
#define persist_alloc(p)		_persist_alloc((p),__FILE__,__LINE__)
#else
ZEND_API void *_emalloc(size_t size);
ZEND_API void _efree(void *ptr);
ZEND_API void *_ecalloc(size_t nmemb, size_t size);
ZEND_API void *_erealloc(void *ptr, size_t size);
ZEND_API char *_estrdup(const char *s);
ZEND_API char *_estrndup(const char *s, unsigned int length);
ZEND_API void _persist_alloc(void *ptr);
#define emalloc(size)			_emalloc((size))
#define efree(ptr)				_efree((ptr))
#define ecalloc(nmemb,size)		_ecalloc((nmemb),(size))
#define erealloc(ptr,size)		_erealloc((ptr),(size))
#define estrdup(s)				_estrdup((s))
#define estrndup(s,length)		_estrndup((s),(length))
#define persist_alloc(p)		_persist_alloc((p))
#endif

#define pemalloc(size,persistent) ((persistent)?malloc(size):emalloc(size))
#define pefree(ptr,persistent)  ((persistent)?free(ptr):efree(ptr))
#define pecalloc(nmemb,size,persistent) ((persistent)?calloc((nmemb),(size)):ecalloc((nmemb),(size)))
#define perealloc(ptr,size,persistent) ((persistent)?realloc((ptr),(size)):erealloc((ptr),(size)))
#define pestrdup(s,persistent) ((persistent)?strdup(s):estrdup(s))

#define safe_estrdup(ptr)  ((ptr)?(estrdup(ptr)):(empty_string))
#define safe_estrndup(ptr,len) ((ptr)?(estrndup((ptr),(len))):(empty_string))

ZEND_API int zend_set_memory_limit(unsigned int memory_limit);

ZEND_API void start_memory_manager(void);
ZEND_API void shutdown_memory_manager(int silent, int clean_cache);

#if ZEND_DEBUG
ZEND_API int _mem_block_check(void *ptr, int silent, char *filename, int lineno);
ZEND_API void _full_mem_check(int silent, char *filename, uint lineno);
#define mem_block_check(ptr, silent) _mem_block_check(ptr, silent, __FILE__, __LINE__)
#define mem_block_check(ptr, silent) _mem_block_check(ptr, silent, __FILE__, __LINE__)
#define full_mem_check(silent) _full_mem_check(silent, __FILE__, __LINE__)
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
 * End:
 */
