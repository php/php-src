#include "gd.h"
#include "gdhelpers.h"

#ifdef HAVE_LIBTTF
#define NEED_CACHE 1
#else
#ifdef HAVE_LIBFREETYPE
#define NEED_CACHE 1
#endif
#endif

#ifdef NEED_CACHE

/* 
 * gdcache.c
 *
 * Caches of pointers to user structs in which the least-recently-used 
 * element is replaced in the event of a cache miss after the cache has 
 * reached a given size.
 *
 * John Ellson  (ellson@lucent.com)  Oct 31, 1997
 *
 * Test this with:
 *               gcc -o gdcache -g -Wall -DTEST gdcache.c
 *
 * The cache is implemented by a singly-linked list of elements
 * each containing a pointer to a user struct that is being managed by
 * the cache.
 *
 * The head structure has a pointer to the most-recently-used
 * element, and elements are moved to this position in the list each
 * time they are used.  The head also contains pointers to three
 * user defined functions: 
 *              - a function to test if a cached userdata matches some keydata 
 *              - a function to provide a new userdata struct to the cache 
 *          if there has been a cache miss.
 *              - a function to release a userdata struct when it is
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

#include "gdcache.h"

/*********************************************************/
/* implementation                                        */
/*********************************************************/


/* create a new cache */
gdCache_head_t *
gdCacheCreate (
		int size,
		gdCacheTestFn_t gdCacheTest,
		gdCacheFetchFn_t gdCacheFetch,
		gdCacheReleaseFn_t gdCacheRelease)
{
  gdCache_head_t *head;

  head = (gdCache_head_t *) gdPMalloc(sizeof (gdCache_head_t));
  head->mru = NULL;
  head->size = size;
  head->gdCacheTest = gdCacheTest;
  head->gdCacheFetch = gdCacheFetch;
  head->gdCacheRelease = gdCacheRelease;
  return head;
}

void
gdCacheDelete (gdCache_head_t * head)
{
  gdCache_element_t *elem, *prev;

  elem = head->mru;
  while (elem)
    {
      (*(head->gdCacheRelease)) (elem->userdata);
      prev = elem;
      elem = elem->next;
      gdFree ((char *) prev);
    }
  gdFree ((char *) head);
}

void *
gdCacheGet (gdCache_head_t * head, void *keydata)
{
  int i = 0;
  gdCache_element_t *elem, *prev = NULL, *prevprev = NULL;
  void *userdata;

  elem = head->mru;
  while (elem)
    {
      if ((*(head->gdCacheTest)) (elem->userdata, keydata))
	{
	  if (i)
	    {			/* if not already most-recently-used */
	      /* relink to top of list */
	      prev->next = elem->next;
	      elem->next = head->mru;
	      head->mru = elem;
	    }
	  return elem->userdata;
	}
      prevprev = prev;
      prev = elem;
      elem = elem->next;
      i++;
    }
  userdata = (*(head->gdCacheFetch)) (&(head->error), keydata);
  if (!userdata)
    {
      /* if there was an error in the fetch then don't cache */
      return NULL;
    }
  if (i < head->size)
    {				/* cache still growing - add new elem */
      elem = (gdCache_element_t *) gdPMalloc(sizeof (gdCache_element_t));
    }
  else
    {				/* cache full - replace least-recently-used */
      /* preveprev becomes new end of list */
      prevprev->next = NULL;
      elem = prev;
      (*(head->gdCacheRelease)) (elem->userdata);
    }
  /* relink to top of list */
  elem->next = head->mru;
  head->mru = elem;
  elem->userdata = userdata;
  return userdata;
}



/*********************************************************/
/* test stub                                             */
/*********************************************************/


#ifdef TEST

#include <stdio.h>

typedef struct
{
  int key;
  int value;
}
key_value_t;

static int
cacheTest (void *map, void *key)
{
  return (((key_value_t *) map)->key == *(int *) key);
}

static void *
cacheFetch (char **error, void *key)
{
  key_value_t *map;

  map = (key_value_t *) gdMalloc (sizeof (key_value_t));
  map->key = *(int *) key;
  map->value = 3;

  *error = NULL;
  return (void *) map;
}

static void
cacheRelease (void *map)
{
  gdFree ((char *) map);
}

int
main (char *argv[], int argc)
{
  gdCache_head_t *cacheTable;
  int elem, key;

  cacheTable = gdCacheCreate (3, cacheTest, cacheFetch, cacheRelease);

  key = 20;
  elem = *(int *) gdCacheGet (cacheTable, &key);
  key = 30;
  elem = *(int *) gdCacheGet (cacheTable, &key);
  key = 40;
  elem = *(int *) gdCacheGet (cacheTable, &key);
  key = 50;
  elem = *(int *) gdCacheGet (cacheTable, &key);
  key = 30;
  elem = *(int *) gdCacheGet (cacheTable, &key);
  key = 30;
  elem = *(int *) gdCacheGet (cacheTable, &key);

  gdCacheDelete (cacheTable);

  return 0;
}

#endif /* TEST */
#endif /* HAVE_LIBTTF */
