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

#ifndef ZEND_FAST_CACHE_H
#define ZEND_FAST_CACHE_H

#ifndef ZEND_ENABLE_FAST_CACHE
# if ZEND_DEBUG
# define ZEND_ENABLE_FAST_CACHE 0
# else
# define ZEND_ENABLE_FAST_CACHE 0
# endif
#endif

typedef struct _zend_fast_cache_list_entry {
	struct _zend_fast_cache_list_entry *next;
} zend_fast_cache_list_entry;

#define MAX_FAST_CACHE_TYPES	4


#define ZVAL_CACHE_LIST			0
#define HASHTABLE_CACHE_LIST	1

#if ZEND_ENABLE_FAST_CACHE


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


#define ZEND_FAST_ALLOC(p, type, fc_type)								\
	{																\
		TSRMLS_FETCH();												\
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
		TSRMLS_FETCH();												\
																	\
		((zend_fast_cache_list_entry *) (p))->next = (zend_fast_cache_list_entry *) AG(fast_cache_list_head)[fc_type];	\
		AG(fast_cache_list_head)[fc_type] = (zend_fast_cache_list_entry *) (p);			\
	}

#define ZEND_FAST_ALLOC_REL(p, type, fc_type)	\
	ZEND_FAST_ALLOC(p, type, fc_type)

#define ZEND_FAST_FREE_REL(p, fc_type)	\
	ZEND_FAST_FREE(p, fc_type)


#else /* !ZEND_ENABLE_FAST_CACHE */

#define ZEND_FAST_ALLOC(p, type, fc_type)	\
	(p) = (type *) emalloc(sizeof(type))

#define ZEND_FAST_FREE(p, fc_type)	\
	efree(p)

#define ZEND_FAST_ALLOC_REL(p, type, fc_type)	\
	(p) = (type *) emalloc_rel(sizeof(type))

#define ZEND_FAST_FREE_REL(p, fc_type)	\
	efree_rel(p)

#endif /* ZEND_ENABLE_FAST_CACHE */




/* fast cache for zval's */
#define ALLOC_ZVAL(z)	\
	ZEND_FAST_ALLOC(z, zval, ZVAL_CACHE_LIST)

#define FREE_ZVAL(z)	\
	ZEND_FAST_FREE(z, ZVAL_CACHE_LIST)

#define ALLOC_ZVAL_REL(z)	\
	ZEND_FAST_ALLOC_REL(z, zval, ZVAL_CACHE_LIST)

#define FREE_ZVAL_REL(z)	\
	ZEND_FAST_FREE_REL(z, ZVAL_CACHE_LIST)

/* fast cache for HashTables */
#define ALLOC_HASHTABLE(ht)	\
	ZEND_FAST_ALLOC(ht, HashTable, HASHTABLE_CACHE_LIST)

#define FREE_HASHTABLE(ht)	\
	ZEND_FAST_FREE(ht, HASHTABLE_CACHE_LIST)

#define ALLOC_HASHTABLE_REL(ht)	\
	ZEND_FAST_ALLOC_REL(ht, HashTable, HASHTABLE_CACHE_LIST)

#define FREE_HASHTABLE_REL(ht)	\
	ZEND_FAST_FREE_REL(ht, HASHTABLE_CACHE_LIST)

#endif /* ZEND_FAST_CACHE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
