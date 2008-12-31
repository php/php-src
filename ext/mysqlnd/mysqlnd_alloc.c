/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+

*/

/* $Id$ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_palloc.h"


#define MYSQLND_SILENT
#define MYSQLND_DONT_DUMP_STATS

#define MYSQLND_ZVALS_MAX_CACHE 5000


#if A0
/* Caching zval allocator */
zval * 				mysqlnd_alloc_get_zval(MYSQLND_ZVAL_CACHE * const cache);
void 				mysqlnd_alloc_zval_ptr_dtor(zval **zv, MYSQLND_ZVAL_CACHE * const cache);
MYSQLND_ZVAL_CACHE* mysqlnd_alloc_init_cache();
MYSQLND_ZVAL_CACHE* mysqlnd_alloc_get_cache_reference(MYSQLND_ZVAL_CACHE *cache);
void				mysqlnd_alloc_free_cache_reference(MYSQLND_ZVAL_CACHE **cache);
#endif


/*
 The cache line is a big contiguous array of zval pointers.
 Because the CPU cache will cache starting from an address, and not
 before it, then we have to organize our structure according to this.
 Thus, if 'last_added' is valid pointer (not NULL) then last_added is
 increased. When zval is cached, if there is room, last_added is decreased
 and then the zval pointer will be assigned to it. This means that some
 positions may become hot points and stay in the cache.
 Imagine we have 5 pointers in a line
 1. last_added = list_item->ptr_line + cache->max_items;
 2. get_zval -> *last_added = NULL. Use MAKE_STD_ZVAL
 3. get_zval -> *last_added = NULL. Use MAKE_STD_ZVAL
 4. get_zval -> *last_added = NULL. Use MAKE_STD_ZVAL
    0x0
	0x0
	0x0
	0x0 
	0x0
	---
    empty_position, always 0x0 <-- last_added

 5. free_zval -> if (free_items++ != max_items) {// we can add more
                   *(--last_added) = zval_ptr;
				}
	(memory addresses increase downwards)
    0x0
	0x0
	0x0
	0x0 
	0xA <-- last_added
	---
	0x0

 6. free_zval -> if (free_items++ != max_items) {// we can add more
                   *(--last_added) = zval_ptr;
				}
    0x0
	0x0
	0x0
	0xB <-- last_added
	0xA 
	---
	0x0

 7. free_zval -> if (free_items++ != max_items) {// we can add more
                   *(--last_added) = zval_ptr;
				}
    0x0
	0x0
	0xC <-- last_added
	0xB 
	0xA 
	---
	0x0

 8. get_zval -> *last_added != NULL. -> p = *last_added; *last_added++ = NULL;
    0x0
	0x0
	0x0 
	0xB <-- last_added
	0xA 
	---
	0x0

 9. get_zval -> *last_added != NULL. -> p = *last_added; *last_added++ = NULL;
    0x0
	0x0
	0x0 
	0x0 
	0xA <-- last_added
	---
	0x0

10. get_zval -> *last_added != NULL. -> p = *last_added; *last_added++ = NULL;
    0x0
	0x0
	0x0 
	0x0 
	0x0 
	---
	0x0 <-- last_added

*/


zval * mysqlnd_alloc_get_zval(MYSQLND_ZVAL_CACHE * const cache)
{
	zval *ret = NULL;

#ifndef MYSQLND_SILENT
	php_printf("[mysqlnd_alloc_get_zval %p] *last_added=%p free_items=%d  ", cache, cache? cache->free_list->last_added:NULL, cache->free_items);
#endif

	if (cache) {
		if ((ret = *cache->free_list->last_added)) {
			*cache->free_list->last_added++ = NULL;

			--cache->free_items;
			++cache->get_hits;
		} else {
			++cache->get_misses;
		}
	}
	if (!ret) {
		ALLOC_ZVAL(ret);
	}
	INIT_PZVAL(ret);

#ifndef MYSQLND_SILENT
	php_printf("ret=%p\n", ret);
#endif
	return ret;
}

static
void mysqlnd_alloc_cache_prealloc(MYSQLND_ZVAL_CACHE * const cache, unsigned int count)
{
	zval *data;
	cache->free_items = count;
	while (count--) {
		MAKE_STD_ZVAL(data);
		ZVAL_NULL(data);
#ifndef MYSQLND_SILENT
		php_printf("[mysqlnd_alloc_prealloc %p] items=%d data=%p\n", cache, cache->free_items, data);
#endif
		
		*(--cache->free_list->last_added) = data;
	}
}

void mysqlnd_alloc_zval_ptr_dtor(zval **zv, MYSQLND_ZVAL_CACHE * const cache)
{
	if (!cache || Z_REFCOUNT_PP(zv) > 1 || cache->max_items == cache->free_items) {
#ifndef MYSQLND_SILENT
		php_printf("[mysqlnd_alloc_zval_ptr_dtor %p]1 last_added-1=%p *zv=%p\n", cache->free_list->last_added, *zv);
#endif
		/* We can't cache zval's with refcount > 1 */
		zval_ptr_dtor(zv);
		if (cache) {
			if (cache->max_items == cache->free_items) {
				++cache->put_full_misses;
			} else {
				++cache->put_refcount_misses;
			}
		}
	} else {
		/* refcount is 1 and there is place. Go, cache it! */
		++cache->free_items;
		zval_dtor(*zv);
		ZVAL_NULL(*zv);
		*(--cache->free_list->last_added) = *zv;
		++cache->put_hits;
	}
#ifndef MYSQLND_SILENT
	php_printf("[mysqlnd_alloc_zval_ptr_dtor %p] free_items=%d\n", cache, cache->free_items);
#endif
}


MYSQLND_ZVAL_CACHE* mysqlnd_alloc_init_cache(void)
{
	MYSQLND_ZVAL_CACHE *ret = ecalloc(1, sizeof(MYSQLND_ZVAL_CACHE));

#ifndef MYSQLND_SILENT
	php_printf("[mysqlnd_alloc_init_cache %p]\n", ret);
#endif

	ret->max_items = MYSQLND_ZVALS_MAX_CACHE;
	ret->free_items = 0;
	ret->references = 1;

	/* Let's have always one, so we don't need to do a check in get_zval */
	ret->free_list		= ecalloc(1, sizeof(struct st_mysqlnd_zval_list));

	/* One more for empty position of last_added */
	ret->free_list->ptr_line = ecalloc(ret->max_items + 1, sizeof(zval *));
	ret->free_list->last_added = ret->free_list->ptr_line + ret->max_items;

	mysqlnd_alloc_cache_prealloc(ret, (ret->max_items / 100) * 100);

	return ret;
}


MYSQLND_ZVAL_CACHE* mysqlnd_alloc_get_cache_reference(MYSQLND_ZVAL_CACHE *cache)
{
	if (cache) {
		cache->references++;
	}
	return cache;
}


static
void mysqlnd_alloc_free_cache(MYSQLND_ZVAL_CACHE *cache)
{
#ifndef MYSQLND_SILENT
	uint i = 0;
	php_printf("[mysqlnd_alloc_free_cache %p]\n", cache);
#endif

	while (*cache->free_list->last_added) {
#ifndef MYSQLND_SILENT
		php_printf("\t[free_item %d  %p]\n", i++, *cache->free_list->last_added);
#endif
		zval_ptr_dtor(cache->free_list->last_added);
		cache->free_list->last_added++;
	}
#ifndef MYSQLND_DONT_DUMP_STATS
	php_printf("CACHE STATS:\n\tGET\n\t\tHITS:%lu\n\t\tMISSES=%lu\n\t\tHIT RATIO=%1.3f\n\t"
				"PUT\n\t\tHITS:%lu\n\t\tFULL_MISS=%lu\n\t\tREFC_MISS=%lu\n\t\tHIT RATIO=%1.3f\n\n",
		cache->get_hits, cache->get_misses, (1.0*cache->get_hits/(cache->get_hits + cache->get_misses)),
		cache->put_hits, cache->put_full_misses, cache->put_refcount_misses,
		(1.0 * cache->put_hits / (cache->put_hits + cache->put_full_misses + cache->put_refcount_misses)));
#endif
	efree(cache->free_list->ptr_line);
	efree(cache->free_list);
	efree(cache);
}



void mysqlnd_alloc_free_cache_reference(MYSQLND_ZVAL_CACHE **cache)
{
#ifndef MYSQLND_SILENT
	php_printf("[mysqlnd_alloc_free_cache_reference %p] refs=%d\n", *cache, (*cache)->references);
#endif
	if (*cache && --(*cache)->references == 0) {
		mysqlnd_alloc_free_cache(*cache);
	}
	*cache = NULL;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
