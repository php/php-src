/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _ZEND_FAST_CACHE_H
#define _ZEND_FAST_CACHE_H

#define ZEND_ENABLE_FAST_CACHE	1

typedef struct _zend_fast_cache_list_entry {
	struct _zend_fast_cache_list_entry *next;
} zend_fast_cache_list_entry;

#define MAX_FAST_CACHE_TYPES	4





#if ZEND_ENABLE_FAST_CACHE

#define ZVAL_CACHE_LIST			0
#define HASHTABLE_CACHE_LIST	1

#include "zend_globals.h"
#include "zend_globals_macros.h"
#include "zend_alloc.h"


#if ZEND_DEBUG
# define RECORD_ZVAL_CACHE_HIT(fc_type)		AG(fast_cache_stats)[fc_type][1]++;
# define RECORD_ZVAL_CACHE_MISS(fc_type)	AG(fast_cache_stats)[fc_type][0]++;
#else
# define RECORD_ZVAL_CACHE_HIT(fc_type)
# define RECORD_ZVAL_CACHE_MISS(fc_type)
#endif

#ifndef ZTS
extern zend_alloc_globals alloc_globals;
#endif

#define ZEND_FAST_ALLOC(p, type, fc_type)								\
	{																\
		ALS_FETCH();												\
																	\
		if (((p) = (type *) AG(fast_cache_list_head)[fc_type])) {	\
			AG(fast_cache_list_head)[fc_type] = ((zend_fast_cache_list_entry *) AG(fast_cache_list_head)[fc_type])->next;	\
			RECORD_ZVAL_CACHE_HIT(fc_type);							\
		} else {													\
			(p) = (type *) emalloc(sizeof(type));					\
			RECORD_ZVAL_CACHE_MISS(fc_type);						\
		}															\
	}


#define ZEND_FAST_FREE(p, fc_type)										\
	{																\
		ALS_FETCH();												\
																	\
		((zend_fast_cache_list_entry *) (p))->next = AG(fast_cache_list_head)[fc_type];	\
		AG(fast_cache_list_head)[fc_type] = (zend_fast_cache_list_entry *) (p);			\
	}


#else /* !ZEND_ENABLE_FAST_CACHE */

#define ZEND_FAST_ALLOC(p, type, fc_type)	\
	(p) = (type *) emalloc(sizeof(type))

#define ZEND_FAST_FREE(p, fc_type)	\
	efree(p)

#endif /* ZEND_ENABLE_FAST_CACHE */




/* fast cache for zval's */
#define ALLOC_ZVAL(z)	\
	ZEND_FAST_ALLOC(z, zval, ZVAL_CACHE_LIST)

#define FREE_ZVAL(z)	\
	ZEND_FAST_FREE(z, ZVAL_CACHE_LIST)

/* fast cache for HashTable's */
#define ALLOC_HASHTABLE(b)	\
	ZEND_FAST_ALLOC(b, HashTable, HASHTABLE_CACHE_LIST)

#define FREE_HASHTABLE(ht)	\
	ZEND_FAST_FREE(ht, HASHTABLE_CACHE_LIST)

#endif /* _ZEND_FAST_CACHE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
