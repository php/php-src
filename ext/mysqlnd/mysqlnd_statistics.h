/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
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

#ifndef MYSQLND_STATISTICS_H
#define MYSQLND_STATISTICS_H


extern MYSQLND_STATS *mysqlnd_global_stats;

#ifdef ZTS

#define MYSQLND_INC_CONN_STATISTIC(conn_stats, statistic) \
 { \
 	if (mysqlnd_global_stats) { \
 		tsrm_mutex_lock(mysqlnd_global_stats->LOCK_access); \
		mysqlnd_global_stats->values[statistic]++; \
		tsrm_mutex_unlock(mysqlnd_global_stats->LOCK_access); \
		if ((conn_stats)) { \
			((MYSQLND_STATS *) conn_stats)->values[statistic]++; \
		} \
	}\
 }

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn_stats, statistic, value) \
 { \
	if (mysqlnd_global_stats) {\
		my_uint64 v = (value); \
									\
		tsrm_mutex_lock(mysqlnd_global_stats->LOCK_access); \
		mysqlnd_global_stats->values[statistic] += v; \
		tsrm_mutex_unlock(mysqlnd_global_stats->LOCK_access); \
		if ((conn_stats)) { \
			((MYSQLND_STATS *) conn_stats)->values[statistic]+= v; \
		} \
	}\
 }

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats, statistic1, value1, statistic2, value2, statistic3, value3) \
 { \
 	if (mysqlnd_global_stats) { \
		my_uint64 v1 = (value1); \
		my_uint64 v2 = (value2); \
		my_uint64 v3 = (value3); \
								 \
		tsrm_mutex_lock(mysqlnd_global_stats->LOCK_access); \
		mysqlnd_global_stats->values[statistic1]+= v1; \
		mysqlnd_global_stats->values[statistic2]+= v2; \
		mysqlnd_global_stats->values[statistic3]+= v3; \
		tsrm_mutex_unlock(mysqlnd_global_stats->LOCK_access); \
		if ((conn_stats)) { \
			((MYSQLND_STATS *) conn_stats)->values[statistic1]+= v1; \
			((MYSQLND_STATS *) conn_stats)->values[statistic2]+= v2; \
			((MYSQLND_STATS *) conn_stats)->values[statistic3]+= v3; \
		} \
	} \
 }


#else /* NON-ZTS */

#define MYSQLND_INC_CONN_STATISTIC(conn_stats, statistic) \
 { \
	if (mysqlnd_global_stats) { \
		mysqlnd_global_stats->values[statistic]++; \
		if ((conn_stats)) { \
			((MYSQLND_STATS *) conn_stats)->values[statistic]++; \
		} \
	} \
 }

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn_stats, statistic, value) \
 { \
	my_uint64 v = (value); \
							\
	if (mysqlnd_global_stats) { \
		mysqlnd_global_stats->values[statistic]+= v; \
		if ((conn_stats)) { \
			((MYSQLND_STATS *) conn_stats)->values[statistic]+= v; \
		} \
	} \
 } 

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats, statistic1, value1, statistic2, value2, statistic3, value3) \
 { \							\
 	if (mysqlnd_global_stats) { \
		my_uint64 v1 = (value1); \
		my_uint64 v2 = (value2); \
		my_uint64 v3 = (value3); \
								\
		mysqlnd_global_stats->values[statistic1]+= v1; \
		mysqlnd_global_stats->values[statistic2]+= v2; \
		mysqlnd_global_stats->values[statistic3]+= v3; \
		if ((conn_stats)) { \
			((MYSQLND_STATS *) conn_stats)->values[statistic1]+= v1; \
			((MYSQLND_STATS *) conn_stats)->values[statistic2]+= v2; \
			((MYSQLND_STATS *) conn_stats)->values[statistic3]+= v3; \
		} \
	} \
 }

#endif

void mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, zval *return_value
							 TSRMLS_DC ZEND_FILE_LINE_DC);

#endif	/* MYSQLND_STATISTICS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
