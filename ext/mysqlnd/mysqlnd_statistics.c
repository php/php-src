/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
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
#include "mysqlnd_debug.h"


/* {{{ mysqlnd_stats_values_names 
 */

const MYSQLND_STRING mysqlnd_stats_values_names[STAT_LAST] =
{
	{ MYSQLND_STR_W_LEN("bytes_sent") },
	{ MYSQLND_STR_W_LEN("bytes_received") }, 
	{ MYSQLND_STR_W_LEN("packets_sent") },
	{ MYSQLND_STR_W_LEN("packets_received") },
	{ MYSQLND_STR_W_LEN("protocol_overhead_in") },
	{ MYSQLND_STR_W_LEN("protocol_overhead_out") },
	{ MYSQLND_STR_W_LEN("bytes_received_ok_packet") }, 
	{ MYSQLND_STR_W_LEN("bytes_received_eof_packet") }, 
	{ MYSQLND_STR_W_LEN("bytes_received_rset_header_packet") }, 
	{ MYSQLND_STR_W_LEN("bytes_received_rset_field_meta_packet") }, 
	{ MYSQLND_STR_W_LEN("bytes_received_rset_row_packet") }, 
	{ MYSQLND_STR_W_LEN("bytes_received_prepare_response_packet") }, 
	{ MYSQLND_STR_W_LEN("bytes_received_change_user_packet") }, 
	{ MYSQLND_STR_W_LEN("packets_sent_command") }, 
	{ MYSQLND_STR_W_LEN("packets_received_ok") }, 
	{ MYSQLND_STR_W_LEN("packets_received_eof") }, 
	{ MYSQLND_STR_W_LEN("packets_received_rset_header") }, 
	{ MYSQLND_STR_W_LEN("packets_received_rset_field_meta") }, 
	{ MYSQLND_STR_W_LEN("packets_received_rset_row") }, 
	{ MYSQLND_STR_W_LEN("packets_received_prepare_response") }, 
	{ MYSQLND_STR_W_LEN("packets_received_change_user") }, 
	{ MYSQLND_STR_W_LEN("result_set_queries") },
	{ MYSQLND_STR_W_LEN("non_result_set_queries") },
	{ MYSQLND_STR_W_LEN("no_index_used") },
	{ MYSQLND_STR_W_LEN("bad_index_used") },
	{ MYSQLND_STR_W_LEN("slow_queries") },
	{ MYSQLND_STR_W_LEN("buffered_sets") },
	{ MYSQLND_STR_W_LEN("unbuffered_sets") },
	{ MYSQLND_STR_W_LEN("ps_buffered_sets") },
	{ MYSQLND_STR_W_LEN("ps_unbuffered_sets") },
	{ MYSQLND_STR_W_LEN("flushed_normal_sets") },
	{ MYSQLND_STR_W_LEN("flushed_ps_sets") },
	{ MYSQLND_STR_W_LEN("ps_prepared_never_executed") },
	{ MYSQLND_STR_W_LEN("ps_prepared_once_executed") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_server_normal") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_server_ps") },
	{ MYSQLND_STR_W_LEN("rows_buffered_from_client_normal") },
	{ MYSQLND_STR_W_LEN("rows_buffered_from_client_ps") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_client_normal_buffered") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_client_normal_unbuffered") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_client_ps_buffered") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_client_ps_unbuffered") },
	{ MYSQLND_STR_W_LEN("rows_fetched_from_client_ps_cursor") },
	{ MYSQLND_STR_W_LEN("rows_affected_normal") },
	{ MYSQLND_STR_W_LEN("rows_affected_ps") },
	{ MYSQLND_STR_W_LEN("rows_skipped_normal") },
	{ MYSQLND_STR_W_LEN("rows_skipped_ps") },
	{ MYSQLND_STR_W_LEN("copy_on_write_saved") },
	{ MYSQLND_STR_W_LEN("copy_on_write_performed") },
	{ MYSQLND_STR_W_LEN("command_buffer_too_small") },
	{ MYSQLND_STR_W_LEN("connect_success") },
	{ MYSQLND_STR_W_LEN("connect_failure") },
	{ MYSQLND_STR_W_LEN("connection_reused") },
	{ MYSQLND_STR_W_LEN("reconnect") },
	{ MYSQLND_STR_W_LEN("pconnect_success") },
	{ MYSQLND_STR_W_LEN("active_connections") },
	{ MYSQLND_STR_W_LEN("active_persistent_connections") },
	{ MYSQLND_STR_W_LEN("explicit_close") },
	{ MYSQLND_STR_W_LEN("implicit_close") },
	{ MYSQLND_STR_W_LEN("disconnect_close") },
	{ MYSQLND_STR_W_LEN("in_middle_of_command_close") },
	{ MYSQLND_STR_W_LEN("explicit_free_result") },
	{ MYSQLND_STR_W_LEN("implicit_free_result") },
	{ MYSQLND_STR_W_LEN("explicit_stmt_close") },
	{ MYSQLND_STR_W_LEN("implicit_stmt_close") },
	{ MYSQLND_STR_W_LEN("mem_emalloc_count") },
	{ MYSQLND_STR_W_LEN("mem_emalloc_amount") },
	{ MYSQLND_STR_W_LEN("mem_ecalloc_count") },
	{ MYSQLND_STR_W_LEN("mem_ecalloc_amount") },
	{ MYSQLND_STR_W_LEN("mem_erealloc_count") },
	{ MYSQLND_STR_W_LEN("mem_erealloc_amount") },
	{ MYSQLND_STR_W_LEN("mem_efree_count") },
	{ MYSQLND_STR_W_LEN("mem_efree_amount") },
	{ MYSQLND_STR_W_LEN("mem_malloc_count") },
	{ MYSQLND_STR_W_LEN("mem_malloc_amount") },
	{ MYSQLND_STR_W_LEN("mem_calloc_count") },
	{ MYSQLND_STR_W_LEN("mem_calloc_amount") },
	{ MYSQLND_STR_W_LEN("mem_realloc_count") },
	{ MYSQLND_STR_W_LEN("mem_realloc_amount") },
	{ MYSQLND_STR_W_LEN("mem_free_count") },
	{ MYSQLND_STR_W_LEN("mem_free_amount") },
	{ MYSQLND_STR_W_LEN("mem_estrndup_count") },
	{ MYSQLND_STR_W_LEN("mem_strndup_count") },
	{ MYSQLND_STR_W_LEN("mem_estndup_count") },
	{ MYSQLND_STR_W_LEN("mem_strdup_count") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_null") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_bit") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_tinyint") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_short") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_int24") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_int") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_bigint") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_decimal") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_float") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_double") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_date") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_year") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_time") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_datetime") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_timestamp") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_string") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_blob") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_enum") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_set") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_geometry") },
	{ MYSQLND_STR_W_LEN("proto_text_fetched_other") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_null") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_bit") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_tinyint") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_short") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_int24") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_int") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_bigint") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_decimal") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_float") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_double") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_date") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_year") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_time") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_datetime") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_timestamp") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_string") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_json") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_blob") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_enum") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_set") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_geometry") },
	{ MYSQLND_STR_W_LEN("proto_binary_fetched_other") },
	{ MYSQLND_STR_W_LEN("init_command_executed_count") },
	{ MYSQLND_STR_W_LEN("init_command_failed_count") },
	{ MYSQLND_STR_W_LEN("com_quit") },
	{ MYSQLND_STR_W_LEN("com_init_db") },
	{ MYSQLND_STR_W_LEN("com_query") },
	{ MYSQLND_STR_W_LEN("com_field_list") },
	{ MYSQLND_STR_W_LEN("com_create_db") },
	{ MYSQLND_STR_W_LEN("com_drop_db") },
	{ MYSQLND_STR_W_LEN("com_refresh") },
	{ MYSQLND_STR_W_LEN("com_shutdown") },
	{ MYSQLND_STR_W_LEN("com_statistics") },
	{ MYSQLND_STR_W_LEN("com_process_info") },
	{ MYSQLND_STR_W_LEN("com_connect") },
	{ MYSQLND_STR_W_LEN("com_process_kill") },
	{ MYSQLND_STR_W_LEN("com_debug") },
	{ MYSQLND_STR_W_LEN("com_ping") },
	{ MYSQLND_STR_W_LEN("com_time") },
	{ MYSQLND_STR_W_LEN("com_delayed_insert") },
	{ MYSQLND_STR_W_LEN("com_change_user") },
	{ MYSQLND_STR_W_LEN("com_binlog_dump") },
	{ MYSQLND_STR_W_LEN("com_table_dump") },
	{ MYSQLND_STR_W_LEN("com_connect_out") },
	{ MYSQLND_STR_W_LEN("com_register_slave") },
	{ MYSQLND_STR_W_LEN("com_stmt_prepare") },
	{ MYSQLND_STR_W_LEN("com_stmt_execute") },
	{ MYSQLND_STR_W_LEN("com_stmt_send_long_data") },
	{ MYSQLND_STR_W_LEN("com_stmt_close") },
	{ MYSQLND_STR_W_LEN("com_stmt_reset") },
	{ MYSQLND_STR_W_LEN("com_stmt_set_option") },
	{ MYSQLND_STR_W_LEN("com_stmt_fetch") },
	{ MYSQLND_STR_W_LEN("com_deamon") },
	{ MYSQLND_STR_W_LEN("bytes_received_real_data_normal") },
	{ MYSQLND_STR_W_LEN("bytes_received_real_data_ps") }
};
/* }}} */


/* {{{ mysqlnd_fill_stats_hash */
PHPAPI void
mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, const MYSQLND_STRING * names, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	unsigned int i;

	mysqlnd_array_init(return_value, stats->count);
	for (i = 0; i < stats->count; i++) {
		char tmp[25];

		sprintf((char *)&tmp, MYSQLND_LLU_SPEC, stats->values[i]);
		add_assoc_string_ex(return_value, names[i].s, names[i].l + 1, tmp, 1);
	}
}
/* }}} */


/* {{{ _mysqlnd_get_client_stats */
PHPAPI void
_mysqlnd_get_client_stats(zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	MYSQLND_STATS stats, *stats_ptr = mysqlnd_global_stats;
	DBG_ENTER("_mysqlnd_get_client_stats");
	if (!stats_ptr) {
		memset(&stats, 0, sizeof(stats));
		stats_ptr = &stats;
	}
	mysqlnd_fill_stats_hash(stats_ptr, mysqlnd_stats_values_names, return_value TSRMLS_CC ZEND_FILE_LINE_CC);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_stats_init */
PHPAPI void
mysqlnd_stats_init(MYSQLND_STATS ** stats, size_t statistic_count)
{
	*stats = calloc(1, sizeof(MYSQLND_STATS));
	if (*stats == NULL) {
		return;
	}
	(*stats)->values = calloc(statistic_count, sizeof(uint64_t));
	(*stats)->triggers = calloc(statistic_count, sizeof(mysqlnd_stat_trigger));
	(*stats)->in_trigger = FALSE;
	(*stats)->count = statistic_count;
#ifdef ZTS
	(*stats)->LOCK_access = tsrm_mutex_alloc();
#endif
}
/* }}} */


/* {{{ mysqlnd_stats_end */
PHPAPI void
mysqlnd_stats_end(MYSQLND_STATS * stats)
{
#ifdef ZTS
	tsrm_mutex_free(stats->LOCK_access);
#endif
	free(stats->triggers);
	free(stats->values);
	/* mnd_free will reference LOCK_access and crash...*/
	free(stats);
}
/* }}} */


/* {{{ mysqlnd_stats_set_trigger */
PHPAPI mysqlnd_stat_trigger
mysqlnd_stats_set_trigger(MYSQLND_STATS * const stats, enum_mysqlnd_collected_stats statistic, mysqlnd_stat_trigger trigger TSRMLS_DC)
{
	mysqlnd_stat_trigger ret = NULL;
	DBG_ENTER("mysqlnd_stats_set_trigger");
	if (stats) {
		MYSQLND_STATS_LOCK(stats);
		ret = stats->triggers[statistic];
		stats->triggers[statistic] = trigger;
		MYSQLND_STATS_UNLOCK(stats);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stats_set_handler */
PHPAPI mysqlnd_stat_trigger
mysqlnd_stats_reset_triggers(MYSQLND_STATS * const stats TSRMLS_DC)
{
	mysqlnd_stat_trigger ret = NULL;
	DBG_ENTER("mysqlnd_stats_reset_trigger");
	if (stats) {
		MYSQLND_STATS_LOCK(stats);
		memset(stats->triggers, 0, stats->count * sizeof(mysqlnd_stat_trigger));
		MYSQLND_STATS_UNLOCK(stats);
	}
	DBG_RETURN(ret);
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
