/*
 * xmlmemory.h: interface for the memory allocation debug.
 *
 * daniel@veillard.com
 */


#ifndef _DEBUG_MEMORY_ALLOC_
#define _DEBUG_MEMORY_ALLOC_

#include <stdio.h>
#include <libxml/xmlversion.h>

/**
 * DEBUG_MEMORY:
 *
 * DEBUG_MEMORY replaces the allocator with a collect and debug
 * shell to the libc allocator.
 * DEBUG_MEMORY should only be activated when debugging 
 * libxml i.e. if libxml has been configured with --with-debug-mem too.
 */
/* #define DEBUG_MEMORY_FREED */
/* #define DEBUG_MEMORY_LOCATION */

#ifdef DEBUG
#ifndef DEBUG_MEMORY
#define DEBUG_MEMORY
#endif
#endif

/**
 * DEBUG_MEMORY_LOCATION:
 *
 * DEBUG_MEMORY_LOCATION should be activated only when debugging 
 * libxml i.e. if libxml has been configured with --with-debug-mem too.
 */
#ifdef DEBUG_MEMORY_LOCATION
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The XML memory wrapper support 4 basic overloadable functions.
 */
/**
 * xmlFreeFunc:
 * @mem: an already allocated block of memory
 *
 * Signature for a free() implementation.
 */
typedef void (*xmlFreeFunc)(void *mem);
/**
 * xmlMallocFunc:
 * @size:  the size requested in bytes
 *
 * Signature for a malloc() implementation.
 *
 * Returns a pointer to the newly allocated block or NULL in case of error.
 */
typedef void *(*xmlMallocFunc)(size_t size);

/**
 * xmlReallocFunc:
 * @mem: an already allocated block of memory
 * @size:  the new size requested in bytes
 *
 * Signature for a realloc() implementation.
 *
 * Returns a pointer to the newly reallocated block or NULL in case of error.
 */
typedef void *(*xmlReallocFunc)(void *mem, size_t size);

/**
 * xmlStrdupFunc:
 * @str: a zero terminated string
 *
 * Signature for an strdup() implementation.
 *
 * Returns the copy of the string or NULL in case of error.
 */
typedef char *(*xmlStrdupFunc)(const char *str);

/*
 * The 4 interfaces used for all memory handling within libxml.
LIBXML_DLL_IMPORT extern xmlFreeFunc xmlFree;
LIBXML_DLL_IMPORT extern xmlMallocFunc xmlMalloc;
LIBXML_DLL_IMPORT extern xmlReallocFunc xmlRealloc;
LIBXML_DLL_IMPORT extern xmlStrdupFunc xmlMemStrdup;
 */

/*
 * The way to overload the existing functions.
 */
int     xmlMemSetup	(xmlFreeFunc freeFunc,
			 xmlMallocFunc mallocFunc,
			 xmlReallocFunc reallocFunc,
			 xmlStrdupFunc strdupFunc);
int     xmlMemGet	(xmlFreeFunc *freeFunc,
			 xmlMallocFunc *mallocFunc,
			 xmlReallocFunc *reallocFunc,
			 xmlStrdupFunc *strdupFunc);

/*
 * Initialization of the memory layer.
 */
int	xmlInitMemory	(void);

/*
 * Those are specific to the XML debug memory wrapper.
 */
int	xmlMemUsed	(void);
void	xmlMemDisplay	(FILE *fp);
void	xmlMemShow	(FILE *fp, int nr);
void	xmlMemoryDump	(void);
void *	xmlMemMalloc	(size_t size);
void *	xmlMemRealloc	(void *ptr,size_t size);
void	xmlMemFree	(void *ptr);
char *	xmlMemoryStrdup	(const char *str);

#ifdef DEBUG_MEMORY_LOCATION
/**
 * xmlMalloc:
 * @size:  number of bytes to allocate
 *
 * Wrapper for the malloc() function used in the XML library.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlMalloc(size) xmlMallocLoc((size), __FILE__, __LINE__)
/**
 * xmlRealloc:
 * @ptr:  pointer to the existing allocated area
 * @size:  number of bytes to allocate
 *
 * Wrapper for the realloc() function used in the XML library.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlRealloc(ptr, size) xmlReallocLoc((ptr), (size), __FILE__, __LINE__)
/**
 * xmlMemStrdup:
 * @str:  pointer to the existing string
 *
 * Wrapper for the strdup() function, xmlStrdup() is usually preferred.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlMemStrdup(str) xmlMemStrdupLoc((str), __FILE__, __LINE__)

void *	xmlMallocLoc(size_t size, const char *file, int line);
void *	xmlReallocLoc(void *ptr,size_t size, const char *file, int line);
char *	xmlMemStrdupLoc(const char *str, const char *file, int line);
#endif /* DEBUG_MEMORY_LOCATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifndef __XML_GLOBALS_H
#ifndef __XML_THREADS_H__
#include <libxml/threads.h>
#include <libxml/globals.h>
#endif
#endif

#endif  /* _DEBUG_MEMORY_ALLOC_ */

