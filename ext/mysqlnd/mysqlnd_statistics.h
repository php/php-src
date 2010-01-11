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

#ifndef MYSQLND_STATISTICS_H
#define MYSQLND_STATISTICS_H


extern MYSQLND_STATS *mysqlnd_global_stats;

typedef struct st_mysqlnd_string
{
	char	*s;
	size_t	l;
} MYSQLND_STRING;

extern const MYSQLND_STRING mysqlnd_stats_values_names[];

#ifdef ZTS
#define MYSQLND_STATS_LOCK(stats) tsrm_mutex_lock((stats)->LOCK_access)
#define MYSQLND_STATS_UNLOCK(stats) tsrm_mutex_unlock((stats)->LOCK_access)
#else
#define MYSQLND_STATS_LOCK(stats)
#define MYSQLND_STATS_UNLOCK(stats)
#endif

#define MYSQLND_CHECK_AND_CALL_HANDLER(stats, statistic, value) \
			if ((stats)->triggers[(statistic)] && (stats)->in_trigger == FALSE) { \
				(stats)->in_trigger = TRUE; \
				(stats)->triggers[(statistic)]((stats), (statistic), (value) TSRMLS_CC); \
				(stats)->in_trigger = FALSE; \
			} \



#define MYSQLND_INC_GLOBAL_STATISTIC(statistic) \
 { \
 	if (MYSQLND_G(collect_statistics) && (statistic) != STAT_LAST) { \
		DBG_INF_FMT("Global stat increase [%s]", mysqlnd_stats_values_names[(statistic)].s); \
			\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, (statistic), 1); \
		mysqlnd_global_stats->values[(statistic)]++; \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
	}\
 }

#define MYSQLND_DEC_CONN_STATISTIC(conn_stats, statistic) \
 { \
 	if (MYSQLND_G(collect_statistics) && (statistic) != STAT_LAST) { \
		DBG_INF_FMT("Global&conn stat decrease [%s]", mysqlnd_stats_values_names[(statistic)].s); \
			\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, (statistic), -1); \
		mysqlnd_global_stats->values[(statistic)]--; \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
		if ((conn_stats)) {  \
			MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), (statistic), -1); \
		} \
	}\
 }

#define MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(statistic1, value1, statistic2, value2) \
 { \
 	if (MYSQLND_G(collect_statistics)) { \
		uint64_t v1 = (uint64_t) (value1); \
		uint64_t v2 = (uint64_t) (value2); \
		enum_mysqlnd_collected_stats _s1 = (statistic1);\
		enum_mysqlnd_collected_stats _s2 = (statistic2);\
								 \
		if (_s1 != STAT_LAST) DBG_INF_FMT("Global stat increase1 [%s]", mysqlnd_stats_values_names[_s1].s); \
		if (_s2 != STAT_LAST) DBG_INF_FMT("Global stat increase2 [%s]", mysqlnd_stats_values_names[_s2].s); \
										\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		if (_s1 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s1, v1); \
			mysqlnd_global_stats->values[_s1]+= v1; \
		} \
		if (_s2 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s2, v2); \
			mysqlnd_global_stats->values[_s2]+= v2; \
		} \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
	}\
 }

#define MYSQLND_INC_CONN_STATISTIC(conn_stats, statistic) \
 { \
 	if (MYSQLND_G(collect_statistics) && (statistic) != STAT_LAST) { \
		DBG_INF_FMT("Global&Conn stat increase [%s]", mysqlnd_stats_values_names[(statistic)].s); \
			\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, (statistic), 1); \
		mysqlnd_global_stats->values[(statistic)]++; \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
		if (conn_stats) { \
			MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), (statistic), 1); \
			(conn_stats)->values[(statistic)]++; \
		} \
	}\
 }

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn_stats, statistic, value) \
 { \
 	if (MYSQLND_G(collect_statistics) && (statistic) != STAT_LAST) { \
		uint64_t v = (uint64_t) (value); \
		DBG_INF_FMT("Global&Conn stat increase w value [%s]", mysqlnd_stats_values_names[(statistic)].s); \
			\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, (statistic), v); \
		mysqlnd_global_stats->values[(statistic)] += v; \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
		if (conn_stats) { \
			MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), (statistic), v); \
			(conn_stats)->values[(statistic)]+= v; \
		} \
	}\
 }

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn_stats, statistic1, value1, statistic2, value2) \
 { \
 	if (MYSQLND_G(collect_statistics)) { \
		uint64_t v1 = (uint64_t) (value1); \
		uint64_t v2 = (uint64_t) (value2); \
		enum_mysqlnd_collected_stats _s1 = (statistic1);\
		enum_mysqlnd_collected_stats _s2 = (statistic2);\
								 \
		if (_s1 != STAT_LAST) DBG_INF_FMT("Global stat increase1 [%s]", mysqlnd_stats_values_names[_s1].s); \
		if (_s2 != STAT_LAST) DBG_INF_FMT("Global stat increase2 [%s]", mysqlnd_stats_values_names[_s2].s); \
					\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		if (_s1 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s1, v1); \
			mysqlnd_global_stats->values[_s1]+= v1; \
		} \
		if (_s2 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s2, v2); \
			mysqlnd_global_stats->values[_s2]+= v2; \
		} \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
		if (conn_stats) { \
			if (_s1 != STAT_LAST) { \
				MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), _s1, v1); \
				(conn_stats)->values[_s1]+= v1; \
			} \
			if (_s2 != STAT_LAST) { \
				MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), _s2, v2); \
				(conn_stats)->values[_s2]+= v2; \
			} \
		} \
	} \
 }


#define MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats, statistic1, value1, statistic2, value2, statistic3, value3) \
 { \
 	if (MYSQLND_G(collect_statistics)) { \
		uint64_t v1 = (uint64_t) (value1); \
		uint64_t v2 = (uint64_t) (value2); \
		uint64_t v3 = (uint64_t) (value3); \
		enum_mysqlnd_collected_stats _s1 = (statistic1); \
		enum_mysqlnd_collected_stats _s2 = (statistic2); \
		enum_mysqlnd_collected_stats _s3 = (statistic3); \
								 \
		if (_s1 != STAT_LAST) DBG_INF_FMT("Global stat increase1 [%s]", mysqlnd_stats_values_names[_s1].s); \
		if (_s2 != STAT_LAST) DBG_INF_FMT("Global stat increase2 [%s]", mysqlnd_stats_values_names[_s2].s); \
		if (_s3 != STAT_LAST) DBG_INF_FMT("Global stat increase3 [%s]", mysqlnd_stats_values_names[_s3].s); \
								 		\
 		MYSQLND_STATS_LOCK(mysqlnd_global_stats); \
		if (_s1 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s1, v1); \
			mysqlnd_global_stats->values[_s1]+= v1; \
		} \
		if (_s2 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s2, v2); \
			mysqlnd_global_stats->values[_s2]+= v2; \
		} \
		if (_s3 != STAT_LAST) { \
			MYSQLND_CHECK_AND_CALL_HANDLER(mysqlnd_global_stats, _s3, v3); \
			mysqlnd_global_stats->values[_s3]+= v3; \
		} \
 		MYSQLND_STATS_UNLOCK(mysqlnd_global_stats); \
		if (conn_stats) { \
			if (_s1 != STAT_LAST) { \
				MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), _s1, v1); \
				(conn_stats)->values[_s1]+= v1; \
			} \
			if (_s2 != STAT_LAST) { \
				MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), _s2, v2); \
				(conn_stats)->values[_s2]+= v2; \
			} \
			if (_s3 != STAT_LAST) { \
				MYSQLND_CHECK_AND_CALL_HANDLER((conn_stats), _s3, v3); \
				(conn_stats)->values[_s3]+= v3; \
			} \
		} \
	} \
 }


void mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, zval *return_value
							 TSRMLS_DC ZEND_FILE_LINE_DC);


PHPAPI void mysqlnd_stats_init(MYSQLND_STATS ** stats);
PHPAPI void mysqlnd_stats_end(MYSQLND_STATS * stats);

PHPAPI mysqlnd_stat_trigger mysqlnd_stats_set_trigger(MYSQLND_STATS * const stats, enum_mysqlnd_collected_stats stat, mysqlnd_stat_trigger trigger TSRMLS_DC);
PHPAPI mysqlnd_stat_trigger mysqlnd_stats_reset_triggers(MYSQLND_STATS * const stats TSRMLS_DC);


#endif	/* MYSQLND_STATISTICS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
