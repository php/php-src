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

#include <stdlib.h>

#include "zend.h"
#include "zend_alloc.h"
#include "zend_globals.h"

#ifndef ZTS
static zend_alloc_globals alloc_globals;
#endif


#if ZEND_DEBUG
# define END_MAGIC_SIZE sizeof(long)
# define END_ALIGNMENT(size) (((size)%PLATFORM_ALIGNMENT)?(PLATFORM_ALIGNMENT-((size)%PLATFORM_ALIGNMENT)):0)
#else
# define END_MAGIC_SIZE 0
# define END_ALIGNMENT(size) 0
#endif


# if MEMORY_LIMIT
#  if ZEND_DEBUG
#define CHECK_MEMORY_LIMIT(s) _CHECK_MEMORY_LIMIT(s,filename,lineno)
#  else
#define CHECK_MEMORY_LIMIT(s)	_CHECK_MEMORY_LIMIT(s,NULL,0)
#  endif

#define _CHECK_MEMORY_LIMIT(s,file,lineno) { AG(allocated_memory) += (s);\
								if (AG(memory_limit)<AG(allocated_memory)) {\
									if (!file) { \
										zend_error(E_ERROR,"Allowed memory size of %d bytes exhausted (tried to allocate %d bytes)", AG(memory_limit),s); \
									} else { \
										zend_error(E_ERROR,"Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes)", AG(memory_limit), file, lineno, s); \
									} \
								} \
							}
# endif

#ifndef CHECK_MEMORY_LIMIT
#define CHECK_MEMORY_LIMIT(s)
#endif


#define REMOVE_POINTER_FROM_LIST(p)				\
	if (!p->persistent && p==AG(head)) {		\
		AG(head) = p->pNext;					\
	} else if (p->persistent && p==AG(phead)) {	\
		AG(phead) = p->pNext;					\
	} else {									\
		p->pLast->pNext = p->pNext;				\
	}											\
	if (p->pNext) {								\
		p->pNext->pLast = p->pLast;				\
	}

#define ADD_POINTER_TO_LIST(p)		\
	if (p->persistent) {			\
		p->pNext = AG(phead);		\
		if (AG(phead)) {			\
			AG(phead)->pLast = p;	\
		}							\
		AG(phead) = p;				\
	} else {						\
		p->pNext = AG(head);		\
		if (AG(head)) {				\
			AG(head)->pLast = p;	\
		}							\
		AG(head) = p;				\
	}								\
	p->pLast = (mem_header *) NULL;


/* used by ISAPI and NSAPI */

#if ZEND_DEBUG
ZEND_API void *_emalloc(size_t size, char *filename, uint lineno)
#else
ZEND_API void *_emalloc(size_t size)
#endif
{
	mem_header *p;

	HANDLE_BLOCK_INTERRUPTIONS();

	if ((size < MAX_CACHED_MEMORY) && (AG(cache_count)[size] > 0)) {
		p = AG(cache)[size][--AG(cache_count)[size]];
#if ZEND_DEBUG
		p->filename = filename;
		p->lineno = lineno;
		p->magic = MEM_BLOCK_START_MAGIC;
#endif
		HANDLE_UNBLOCK_INTERRUPTIONS();
		p->persistent = 0;
		p->cached = 0;
		return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
	} else {
		p  = (mem_header *) malloc(sizeof(mem_header) + size + PLATFORM_PADDING + END_ALIGNMENT(size) + END_MAGIC_SIZE);
	}

	if (!p) {
		fprintf(stderr,"FATAL:  emalloc():  Unable to allocate %d bytes\n", size);
		exit(1);
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return (void *)p;
	}
	p->persistent = p->cached = 0;
	ADD_POINTER_TO_LIST(p);
	p->size = size;
#if ZEND_DEBUG
	p->filename = filename;
	p->lineno = lineno;
	p->magic = MEM_BLOCK_START_MAGIC;
	*((long *)(((char *) p) + sizeof(mem_header)+size+PLATFORM_PADDING+END_ALIGNMENT(size))) = MEM_BLOCK_END_MAGIC;
#endif
#if MEMORY_LIMIT
	CHECK_MEMORY_LIMIT(size);
#endif
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
}


#if ZEND_DEBUG
ZEND_API void _efree(void *ptr, char *filename, uint lineno)
#else
ZEND_API void _efree(void *ptr)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr - sizeof(mem_header) - PLATFORM_PADDING);

#if ZEND_DEBUG
	_mem_block_check(ptr, 1, filename, lineno);
	memset(ptr, 0x5a, p->size);
#endif

	if (!p->persistent && (p->size < MAX_CACHED_MEMORY) && (AG(cache_count)[p->size] < MAX_CACHED_ENTRIES)) {
		AG(cache)[p->size][AG(cache_count)[p->size]++] = p;
		p->cached = 1;
#if ZEND_DEBUG
		p->magic = MEM_BLOCK_CACHED_MAGIC;
#endif
		return;
	}
	HANDLE_BLOCK_INTERRUPTIONS();
	REMOVE_POINTER_FROM_LIST(p);

#if MEMORY_LIMIT
	AG(allocated_memory) -= p->size;
#endif
	
	free(p);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}


#if ZEND_DEBUG
ZEND_API void *_ecalloc(size_t nmemb, size_t size, char *filename, uint lineno)
#else
ZEND_API void *_ecalloc(size_t nmemb, size_t size)
#endif
{
	void *p;
	int final_size=size*nmemb;

	HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
	p = _emalloc(final_size,filename,lineno);
#else
	p = emalloc(final_size);
#endif
	if (!p) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return (void *) p;
	}
	memset(p,(int)NULL,final_size);
	HANDLE_UNBLOCK_INTERRUPTIONS();
	return p;
}


#if ZEND_DEBUG
ZEND_API void *_erealloc(void *ptr, size_t size, char *filename, uint lineno)
#else
ZEND_API void *_erealloc(void *ptr, size_t size)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr-sizeof(mem_header)-PLATFORM_PADDING);
	mem_header *orig = p;

	if (!ptr) {
#if ZEND_DEBUG
		return _emalloc(size, filename, lineno);
#else
		return emalloc(size);
#endif
	}
	HANDLE_BLOCK_INTERRUPTIONS();
	REMOVE_POINTER_FROM_LIST(p);
	p = (mem_header *) realloc(p,sizeof(mem_header)+size+PLATFORM_PADDING+END_ALIGNMENT(size)+END_MAGIC_SIZE);
	if (!p) {
		fprintf(stderr,"FATAL:  erealloc():  Unable to allocate %d bytes\n", size);
		HANDLE_UNBLOCK_INTERRUPTIONS();
		zend_bailout();
		ADD_POINTER_TO_LIST(orig);
		return (void *)NULL;
	}
	ADD_POINTER_TO_LIST(p);
#if ZEND_DEBUG
	p->filename = filename;
	p->lineno = lineno;
	p->magic = MEM_BLOCK_START_MAGIC;
	*((long *)(((char *) p) + sizeof(mem_header)+size+PLATFORM_PADDING+END_ALIGNMENT(size))) = MEM_BLOCK_END_MAGIC;
#endif	
#if MEMORY_LIMIT
	CHECK_MEMORY_LIMIT(size - p->size);
#endif
	p->size = size;

	HANDLE_UNBLOCK_INTERRUPTIONS();
	return (void *)((char *)p+sizeof(mem_header)+PLATFORM_PADDING);
}


#if ZEND_DEBUG
ZEND_API char *_estrdup(const char *s, char *filename, uint lineno)
#else
ZEND_API char *_estrdup(const char *s)
#endif
{
	int length;
	char *p;

	length = strlen(s)+1;
	HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
	p = (char *) _emalloc(length,filename,lineno);
#else
	p = (char *) emalloc(length);
#endif
	if (!p) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return (char *)NULL;
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
	memcpy(p,s,length);
	return p;
}


#if ZEND_DEBUG
ZEND_API char *_estrndup(const char *s, uint length, char *filename, uint lineno)
#else
ZEND_API char *_estrndup(const char *s, uint length)
#endif
{
	char *p;

	HANDLE_BLOCK_INTERRUPTIONS();
#if ZEND_DEBUG
	p = (char *) _emalloc(length+1,filename,lineno);
#else
	p = (char *) emalloc(length+1);
#endif
	if (!p) {
		HANDLE_UNBLOCK_INTERRUPTIONS();
		return (char *)NULL;
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
	memcpy(p,s,length);
	p[length]=0;
	return p;
}


ZEND_API char *zend_strndup(const char *s, uint length)
{
	char *p;

	p = (char *) malloc(length+1);
	if (!p) {
		return (char *)NULL;
	}
	if (length) {
		memcpy(p,s,length);
	}
	p[length]=0;
	return p;
}


ZEND_API int zend_set_memory_limit(unsigned int memory_limit)
{
#if MEMORY_LIMIT
	ALS_FETCH();

	AG(memory_limit) = memory_limit;
	return SUCCESS;
#else
	return FAILURE;
#endif
}


ZEND_API void start_memory_manager(void)
{
	AG(phead) = AG(head) = NULL;
	
#if MEMORY_LIMIT
	AG(memory_limit)=1<<30;		/* rediculous limit, effectively no limit */
	AG(allocated_memory)=0;
	AG(memory_exhausted)=0;
#endif

	memset(AG(cache_count),0,MAX_CACHED_MEMORY*sizeof(unsigned char));
}


ZEND_API void shutdown_memory_manager(int silent, int clean_cache)
{
	mem_header *p, *t;

	p=AG(head);
	t=AG(head);
	while (t) {
		if (!t->cached || clean_cache) {
#if ZEND_DEBUG
			if (!t->cached) {
				/* does not use zend_error() *intentionally* */
				if (!silent) {
					zend_message_dispatcher(ZMSG_MEMORY_LEAK_DETECTED, t);
				}
			}
#endif
			p = t->pNext;
			REMOVE_POINTER_FROM_LIST(t);
			free(t);
			t = p;
		} else {
			t = t->pNext;
		}
	}
}


#if ZEND_DEBUG
ZEND_API int _mem_block_check(void *ptr, int silent, char *filename, int lineno)
{
	mem_header *p = (mem_header *) ((char *)ptr - sizeof(mem_header) - PLATFORM_PADDING);
	int no_cache_notice=0;
	int valid_beginning=1;
	int had_problems=0;

	if (silent==2) {
		silent=1;
		no_cache_notice=1;
	}
	if (silent==3) {
		silent=0;
		no_cache_notice=1;
	}
	if (!silent) {
		fprintf(stderr,"---------------------------------------\n");
		fprintf(stderr,"Block 0x%0.8lX status at %s:%d:\n", (long) p, filename, lineno);
		fprintf(stderr,"%10s\t","Beginning:  ");
	}

	switch (p->magic) {
		case MEM_BLOCK_START_MAGIC:
			if (!silent) {
				fprintf(stderr, "OK (allocated on %s:%d, %d bytes)\n", p->filename, p->lineno, p->size);
			}
			break; /* ok */
		case MEM_BLOCK_FREED_MAGIC:
			if (!silent) {
				fprintf(stderr,"Freed\n");
				had_problems=1;
			} else {
				return _mem_block_check(ptr, 0, filename, lineno);
			}
			break;
		case MEM_BLOCK_CACHED_MAGIC:
			if (!silent) {
				if (!no_cache_notice) {
					fprintf(stderr,"Cached (allocated on %s:%d, %d bytes)\n", p->filename, p->lineno, p->size);
					had_problems=1;
				}
			} else {
				if (!no_cache_notice) {
					return _mem_block_check(ptr, 0, filename, lineno);
				}
			}
			break;
		default:
			if (!silent) {
				fprintf(stderr,"Overrun (magic=0x%0.8lX, expected=0x%0.8lX)\n", p->magic, MEM_BLOCK_START_MAGIC);
			} else {
				return _mem_block_check(ptr, 0, filename, lineno);
			}
			had_problems=1;
			valid_beginning=0;
			break;
	}


	if (valid_beginning
		&& *((long *)(((char *) p)+sizeof(mem_header)+p->size+PLATFORM_PADDING+END_ALIGNMENT(p->size))) != MEM_BLOCK_END_MAGIC) {
		long magic_num = MEM_BLOCK_END_MAGIC;
		char *overflow_ptr, *magic_ptr=(char *) &magic_num;
		int overflows=0;
		int i;

		if (silent) {
			return _mem_block_check(ptr, 0, filename, lineno);
		}
		had_problems=1;
		overflow_ptr = ((char *) p)+sizeof(mem_header)+p->size+PLATFORM_PADDING;

		for (i=0; i<sizeof(long); i++) {
			if (overflow_ptr[i]!=magic_ptr[i]) {
				overflows++;
			}
		}

		fprintf(stderr,"%10s\t", "End:");
		fprintf(stderr,"Overflown (magic=0x%0.8lX instead of 0x%0.8lX)\n", 
				*((long *)(((char *) p) + sizeof(mem_header)+p->size+PLATFORM_PADDING+END_ALIGNMENT(p->size))), MEM_BLOCK_END_MAGIC);
		fprintf(stderr,"%10s\t","");
		if (overflows>=sizeof(long)) {
			fprintf(stderr, "At least %d bytes overflown\n", sizeof(long));
		} else {
			fprintf(stderr, "%d byte(s) overflown\n", overflows);
		}
	} else if (!silent) {
		fprintf(stderr,"%10s\t", "End:");
		if (valid_beginning) {
			fprintf(stderr,"OK\n");
		} else {
			fprintf(stderr,"Unknown\n");
		}
	}
		
	if (!silent) {
		fprintf(stderr,"---------------------------------------\n");
	}
	return ((!had_problems) ? 1 : 0);
}


ZEND_API void _full_mem_check(int silent, char *filename, uint lineno)
{
	mem_header *p = AG(head);
	int errors=0;

	fprintf(stderr,"------------------------------------------------\n");
	fprintf(stderr,"Full Memory Check at %s:%d\n", filename, lineno);

	while (p) {
		if (!_mem_block_check((void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING), (silent?2:3), filename, lineno)) {
			errors++;
		}
		p = p->pNext;
	}
	fprintf(stderr,"End of full memory check %s:%d (%d errors)\n", filename, lineno, errors);
	fprintf(stderr,"------------------------------------------------\n");
}
#endif


#if ZEND_DEBUG
ZEND_API void _persist_alloc(void *ptr, char *filename, uint lineno)
#else
ZEND_API void _persist_alloc(void *ptr)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr-sizeof(mem_header)-PLATFORM_PADDING);

#if ZEND_DEBUG
	_mem_block_check(ptr, 1, filename, lineno);
#endif

	HANDLE_BLOCK_INTERRUPTIONS();

	/* remove the block from the non persistent list */
	REMOVE_POINTER_FROM_LIST(p);

	p->persistent = 1;

	/* add the block to the persistent list */
	ADD_POINTER_TO_LIST(p);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
