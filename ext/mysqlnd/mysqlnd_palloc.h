/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2008 The PHP Group                                |
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
#ifndef MYSQLND_PALLOC_H
#define MYSQLND_PALLOC_H

/* Used in mysqlnd_debug.c */
extern char * mysqlnd_palloc_zval_ptr_dtor_name;
extern char * mysqlnd_palloc_get_zval_name;


/* Session caching allocator */
struct st_mysqlnd_zval_list {
	zval 	**ptr_line;
	zval	**last_added;
};

typedef struct st_mysqlnd_zval_cache	MYSQLND_ZVAL_CACHE;

struct st_mysqlnd_zval_cache {
	struct st_mysqlnd_zval_list	*free_list;
	unsigned int		free_items;
	unsigned int		max_items;
	unsigned int		references;
	unsigned long		get_hits;
	unsigned long		get_misses;
	unsigned long		put_hits;
	unsigned long		put_full_misses;
	unsigned long		put_refcount_misses;
};


enum mysqlnd_zval_ptr_type
{
	MYSQLND_POINTS_INT_BUFFER,
	MYSQLND_POINTS_EXT_BUFFER,
	MYSQLND_POINTS_FREE
};

/* Persistent caching allocator */
typedef struct st_mysqlnd_zval {
	/* Should be first */
	zval		zv;
	enum mysqlnd_zval_ptr_type	point_type;
#ifdef ZTS
	THREAD_T	thread_id;
#endif
} mysqlnd_zval;


typedef struct st_mysqlnd_ndzval_list {
	mysqlnd_zval 	**ptr_line;		/* we allocate this, all are pointers to the block */
	void			*canary1;
	void			*canary2;
	mysqlnd_zval	**last_added;	/* this points to the ptr_line, and moves left-right. It's our stack */
} mysqlnd_ndzval_list;


struct st_mysqlnd_zval_pcache {
	mysqlnd_zval		*block;
	mysqlnd_zval		*last_in_block;
	mysqlnd_ndzval_list	free_list;	/* Fetch from here */

#ifdef ZTS
	MUTEX_T 			LOCK_access;
#endif
	unsigned int		references;

	/* These are just for statistics and not used for operational purposes */
	unsigned int		free_items;
	unsigned int		max_items;

	unsigned long		get_hits;
	unsigned long		get_misses;
	unsigned long		put_hits;
	unsigned long		put_misses;
};

struct st_mysqlnd_thread_zval_pcache {
	struct st_mysqlnd_zval_pcache *parent;

	unsigned int		references;
#ifdef ZTS
	THREAD_T			thread_id;
#endif
	mysqlnd_ndzval_list	gc_list;		/* GC these from time to time */
};

#endif /* MYSQLND_PALLOC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
