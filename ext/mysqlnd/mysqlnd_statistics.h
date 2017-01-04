/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_STATISTICS_H
#define MYSQLND_STATISTICS_H


PHPAPI extern MYSQLND_STATS * mysqlnd_global_stats;

extern const MYSQLND_STRING mysqlnd_stats_values_names[];

#ifdef ZTS
#define MYSQLND_STATS_LOCK(stats) tsrm_mutex_lock((stats)->LOCK_access)
#define MYSQLND_STATS_UNLOCK(stats) tsrm_mutex_unlock((stats)->LOCK_access)
#else
#define MYSQLND_STATS_LOCK(stats)
#define MYSQLND_STATS_UNLOCK(stats)
#endif

#ifndef MYSQLND_CORE_STATISTICS_TRIGGERS_DISABLED
#define MYSQLND_STAT_CALL_TRIGGER(s_array, statistic, val) \
			if ((s_array)->triggers[(statistic)] && (s_array)->in_trigger == FALSE) { \
				(s_array)->in_trigger = TRUE; \
				MYSQLND_STATS_UNLOCK((s_array)); \
																						\
				(s_array)->triggers[(statistic)]((s_array), (statistic), (val)); \
																						\
				MYSQLND_STATS_LOCK((s_array)); \
				(s_array)->in_trigger = FALSE; \
			}
#else
#define MYSQLND_STAT_CALL_TRIGGER(s_array, statistic, val)
#endif /* MYSQLND_CORE_STATISTICS_TRIGGERS_DISABLED */

#define MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(stats, statistic, value) \
	{ \
			MYSQLND_STATS_LOCK(stats); \
			(stats)->values[(statistic)] += (value); \
			MYSQLND_STAT_CALL_TRIGGER((stats), (statistic), (value)); \
			MYSQLND_STATS_UNLOCK(_p_s); \
	}

#define MYSQLND_DEC_STATISTIC(enabler, stats, statistic) \
 { \
	enum_mysqlnd_collected_stats _s = (statistic);\
	MYSQLND_STATS * _p_s = (MYSQLND_STATS *) (stats); \
	if ((enabler) && _p_s && _s != _p_s->count) { \
		MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s, -1); \
	}\
 }

#define MYSQLND_INC_STATISTIC(enabler, stats, statistic) \
 { \
	enum_mysqlnd_collected_stats _s = (statistic);\
	MYSQLND_STATS * _p_s = (MYSQLND_STATS *) (stats); \
	if ((enabler) && _p_s && _s != _p_s->count) { \
		MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s, 1); \
	}\
 }

#define MYSQLND_INC_STATISTIC_W_VALUE(enabler, stats, statistic, value) \
 { \
	enum_mysqlnd_collected_stats _s = (statistic);\
	MYSQLND_STATS * _p_s = (MYSQLND_STATS *) (stats); \
	if ((enabler) && _p_s && _s != _p_s->count) { \
		uint64_t v = (uint64_t) (value); \
		MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s, v); \
	}\
 }

#define MYSQLND_INC_STATISTIC_W_VALUE2(enabler, stats, statistic1, value1, statistic2, value2) \
 { \
	MYSQLND_STATS * _p_s = (MYSQLND_STATS *) (stats); \
	if ((enabler) && _p_s) { \
		uint64_t v1 = (uint64_t) (value1); \
		uint64_t v2 = (uint64_t) (value2); \
		enum_mysqlnd_collected_stats _s1 = (statistic1);\
		enum_mysqlnd_collected_stats _s2 = (statistic2);\
		if (_s1 != _p_s->count) MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s1, v1); \
		if (_s2 != _p_s->count) MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s2, v2); \
	}\
 }

#define MYSQLND_INC_STATISTIC_W_VALUE3(enabler, stats, statistic1, value1, statistic2, value2, statistic3, value3) \
 { \
	MYSQLND_STATS * _p_s = (MYSQLND_STATS *) (stats); \
	if ((enabler) && _p_s) { \
		uint64_t v1 = (uint64_t) (value1); \
		uint64_t v2 = (uint64_t) (value2); \
		uint64_t v3 = (uint64_t) (value3); \
		enum_mysqlnd_collected_stats _s1 = (statistic1);\
		enum_mysqlnd_collected_stats _s2 = (statistic2);\
		enum_mysqlnd_collected_stats _s3 = (statistic3);\
		if (_s1 != _p_s->count) MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s1, v1); \
		if (_s2 != _p_s->count) MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s2, v2); \
		if (_s3 != _p_s->count) MYSQLND_UPDATE_VALUE_AND_CALL_TRIGGER(_p_s, _s3, v3); \
	}\
 }



#ifndef MYSQLND_CORE_STATISTICS_DISABLED

#define MYSQLND_INC_GLOBAL_STATISTIC(statistic) \
	MYSQLND_INC_STATISTIC(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic))

#define MYSQLND_DEC_CONN_STATISTIC(conn_stats, statistic) \
	MYSQLND_DEC_STATISTIC(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic))

#define MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(statistic1, value1, statistic2, value2) \
	MYSQLND_INC_STATISTIC_W_VALUE2(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic1), (value1), (statistic2), (value2))

#define MYSQLND_INC_CONN_STATISTIC(conn_stats, statistic) \
	MYSQLND_INC_STATISTIC(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic)); \
	MYSQLND_INC_STATISTIC(MYSQLND_G(collect_statistics), (conn_stats), (statistic));

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn_stats, statistic, value) \
	MYSQLND_INC_STATISTIC_W_VALUE(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic), (value)); \
	MYSQLND_INC_STATISTIC_W_VALUE(MYSQLND_G(collect_statistics), (conn_stats), (statistic), (value));

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn_stats, statistic1, value1, statistic2, value2) \
	MYSQLND_INC_STATISTIC_W_VALUE2(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic1), (value1), (statistic2), (value2)); \
	MYSQLND_INC_STATISTIC_W_VALUE2(MYSQLND_G(collect_statistics), (conn_stats), (statistic1), (value1), (statistic2), (value2));

#define MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats, statistic1, value1, statistic2, value2, statistic3, value3) \
	MYSQLND_INC_STATISTIC_W_VALUE3(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic1), (value1), (statistic2), (value2), (statistic3), (value3)); \
	MYSQLND_INC_STATISTIC_W_VALUE3(MYSQLND_G(collect_statistics), (conn_stats), (statistic1), (value1), (statistic2), (value2), (statistic3), (value3));

#else

#define MYSQLND_INC_GLOBAL_STATISTIC(statistic)
#define MYSQLND_DEC_CONN_STATISTIC(conn_stats, statistic)
#define MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(statistic1, value1, statistic2, value2)
#define MYSQLND_INC_CONN_STATISTIC(conn_stats, statistic)
#define MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn_stats, statistic, value)
#define MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn_stats, statistic1, value1, statistic2, value2)
#define MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats, statistic1, value1, statistic2, value2, statistic3, value3)

#endif /* MYSQLND_CORE_STATISTICS_DISABLED */

PHPAPI void mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, const MYSQLND_STRING * names, zval *return_value ZEND_FILE_LINE_DC);

PHPAPI void mysqlnd_stats_init(MYSQLND_STATS ** stats, size_t statistic_count, int persistent);
PHPAPI void mysqlnd_stats_end(MYSQLND_STATS * stats, int persistent);

PHPAPI mysqlnd_stat_trigger mysqlnd_stats_set_trigger(MYSQLND_STATS * const stats, enum_mysqlnd_collected_stats stat, mysqlnd_stat_trigger trigger);
PHPAPI mysqlnd_stat_trigger mysqlnd_stats_reset_triggers(MYSQLND_STATS * const stats);

#endif	/* MYSQLND_STATISTICS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
