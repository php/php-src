/* 
 * gdcache.h
 *
 * Caches of pointers to user structs in which the least-recently-used 
 * element is replaced in the event of a cache miss after the cache has 
 * reached a given size.
 *
 * John Ellson  (ellson@graphviz.org)  Oct 31, 1997
 *
 * Test this with:
 *		 gcc -o gdcache -g -Wall -DTEST gdcache.c
 *
 * The cache is implemented by a singly-linked list of elements
 * each containing a pointer to a user struct that is being managed by
 * the cache.
 *
 * The head structure has a pointer to the most-recently-used
 * element, and elements are moved to this position in the list each
 * time they are used.  The head also contains pointers to three
 * user defined functions: 
 *		- a function to test if a cached userdata matches some keydata 
 *		- a function to provide a new userdata struct to the cache 
 *          if there has been a cache miss.
 *		- a function to release a userdata struct when it is
 *          no longer being managed by the cache
 *
 * In the event of a cache miss the cache is allowed to grow up to
 * a specified maximum size.  After the maximum size is reached then
 * the least-recently-used element is discarded to make room for the 
 * new.  The most-recently-returned value is always left at the 
 * beginning of the list after retrieval.
 *
 * In the current implementation the cache is traversed by a linear
 * search from most-recent to least-recent.  This linear search
 * probably limits the usefulness of this implementation to cache
 * sizes of a few tens of elements.
 */

/*********************************************************/
/* header                                                */
/*********************************************************/

#include <stdlib.h>
#ifdef HAVE_MALLOC_H
 #include <malloc.h>
#endif
#ifndef NULL
#define NULL (void *)0
#endif

/* user defined function templates */
typedef int (*gdCacheTestFn_t)(void *userdata, void *keydata);
typedef void *(*gdCacheFetchFn_t)(char **error, void *keydata);
typedef void (*gdCacheReleaseFn_t)(void *userdata);

/* element structure */
typedef struct gdCache_element_s gdCache_element_t;
struct gdCache_element_s {
	gdCache_element_t	*next;
	void			*userdata;
};

/* head structure */
typedef struct gdCache_head_s gdCache_head_t;
struct gdCache_head_s {
	gdCache_element_t	*mru;
	int					size;
	char				*error;
	gdCacheTestFn_t		gdCacheTest;
	gdCacheFetchFn_t	gdCacheFetch;
	gdCacheReleaseFn_t	gdCacheRelease;
};

/* function templates */
gdCache_head_t *
gdCacheCreate(
	int					size,
	gdCacheTestFn_t		gdCacheTest,
	gdCacheFetchFn_t	gdCacheFetch,
	gdCacheReleaseFn_t	gdCacheRelease );

void
gdCacheDelete( gdCache_head_t *head );

void *
gdCacheGet( gdCache_head_t *head, void *keydata );
