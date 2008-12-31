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
#include "mysqlnd_statistics.h"

#define MYSQLND_SILENT

#ifdef ZTS
#define LOCK_QCACHE(cache)		tsrm_mutex_lock((cache)->LOCK_access)
#define UNLOCK_QCACHE(cache)	tsrm_mutex_unlock((cache)->LOCK_access)
#else
#define LOCK_QCACHE(cache)
#define UNLOCK_QCACHE(cache)
#endif


/* {{{ mysqlnd_qcache_init_cache */
PHPAPI MYSQLND_QCACHE * mysqlnd_qcache_init_cache()
{
	MYSQLND_QCACHE *cache = calloc(1, sizeof(MYSQLND_QCACHE));
#ifndef MYSQLND_SILENT
	php_printf("[mysqlnd_qcache_init_cache %p]\n", cache);
#endif

	cache->references = 1;
#ifdef ZTS
	cache->LOCK_access = tsrm_mutex_alloc();
#endif
	cache->ht = malloc(sizeof(HashTable));
	zend_hash_init(cache->ht, 10 /* init_elements */, NULL, NULL, TRUE /*pers*/);

	return cache;
}
/* }}} */


/* {{{ mysqlnd_qcache_get_cache_reference */
PHPAPI MYSQLND_QCACHE * mysqlnd_qcache_get_cache_reference(MYSQLND_QCACHE * const cache)
{
	if (cache) {
#ifndef MYSQLND_SILENT
		php_printf("[mysqlnd_qcache_get_cache_reference %p will become %d]\n", cache, cache->references+1);
#endif
		LOCK_QCACHE(cache);
		cache->references++;
		UNLOCK_QCACHE(cache);
	}
	return cache;
}
/* }}} */


/* {{{ mysqlnd_qcache_free_cache */
/*
  As this call will happen on MSHUTDOWN(), then we don't need to copy the zvals with
  copy_ctor but scrap what they point to with zval_dtor() and then just free our
  pre-allocated block. Precondition is that we ZVAL_NULL() the zvals when we put them
  to the free list after usage. We ZVAL_NULL() them when we allocate them in the 
  constructor of the cache.
*/
static
void mysqlnd_qcache_free_cache(MYSQLND_QCACHE *cache)
{
#ifndef MYSQLND_SILENT
	php_printf("[mysqlnd_qcache_free_cache %p]\n", cache);
#endif

#ifdef ZTS
	tsrm_mutex_free(cache->LOCK_access);
#endif
	zend_hash_destroy(cache->ht);
	free(cache->ht);
	free(cache);
}
/* }}} */


/* {{{ mysqlnd_qcache_free_cache_reference */
PHPAPI void mysqlnd_qcache_free_cache_reference(MYSQLND_QCACHE **cache)
{
	if (*cache) {
		zend_bool to_free;
#ifndef MYSQLND_SILENT
		php_printf("[mysqlnd_qcache_free_cache_reference %p] refs=%d\n", *cache, (*cache)->references);
#endif
		LOCK_QCACHE(*cache);
		to_free = --(*cache)->references == 0;
		/* Unlock before destroying */
		UNLOCK_QCACHE(*cache);
		if (to_free) {
			mysqlnd_qcache_free_cache(*cache);
		}
		*cache = NULL;
	}
}
/* }}} */


/* {{{ mysqlnd_qcache_free_cache_reference */
PHPAPI void mysqlnd_qcache_stats(const MYSQLND_QCACHE * const cache, zval *return_value)
{
	if (cache) {
		LOCK_QCACHE(cache);
		array_init(return_value);
		add_assoc_long_ex(return_value, "references",	sizeof("references"),	cache->references);
		UNLOCK_QCACHE(cache);
	} else {
		ZVAL_NULL(return_value);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
