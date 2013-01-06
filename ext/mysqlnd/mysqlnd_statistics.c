/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2013 The PHP Group                                |
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


#define STR_W_LEN(str)  str, (sizeof(str) - 1)

/* {{{ mysqlnd_stats_values_names 
 */

const MYSQLND_STRING mysqlnd_stats_values_names[STAT_LAST] =
{
	{ STR_W_LEN("bytes_sent") },
	{ STR_W_LEN("bytes_received") }, 
	{ STR_W_LEN("packets_sent") },
	{ STR_W_LEN("packets_received") },
	{ STR_W_LEN("protocol_overhead_in") },
	{ STR_W_LEN("protocol_overhead_out") },
	{ STR_W_LEN("bytes_received_ok_packet") }, 
	{ STR_W_LEN("bytes_received_eof_packet") }, 
	{ STR_W_LEN("bytes_received_rset_header_packet") }, 
	{ STR_W_LEN("bytes_received_rset_field_meta_packet") }, 
	{ STR_W_LEN("bytes_received_rset_row_packet") }, 
	{ STR_W_LEN("bytes_received_prepare_response_packet") }, 
	{ STR_W_LEN("bytes_received_change_user_packet") }, 
	{ STR_W_LEN("packets_sent_command") }, 
	{ STR_W_LEN("packets_received_ok") }, 
	{ STR_W_LEN("packets_received_eof") }, 
	{ STR_W_LEN("packets_received_rset_header") }, 
	{ STR_W_LEN("packets_received_rset_field_meta") }, 
	{ STR_W_LEN("packets_received_rset_row") }, 
	{ STR_W_LEN("packets_received_prepare_response") }, 
	{ STR_W_LEN("packets_received_change_user") }, 
	{ STR_W_LEN("result_set_queries") },
	{ STR_W_LEN("non_result_set_queries") },
	{ STR_W_LEN("no_index_used") },
	{ STR_W_LEN("bad_index_used") },
	{ STR_W_LEN("slow_queries") },
	{ STR_W_LEN("buffered_sets") },
	{ STR_W_LEN("unbuffered_sets") },
	{ STR_W_LEN("ps_buffered_sets") },
	{ STR_W_LEN("ps_unbuffered_sets") },
	{ STR_W_LEN("flushed_normal_sets") },
	{ STR_W_LEN("flushed_ps_sets") },
	{ STR_W_LEN("ps_prepared_never_executed") },
	{ STR_W_LEN("ps_prepared_once_executed") },
	{ STR_W_LEN("rows_fetched_from_server_normal") },
	{ STR_W_LEN("rows_fetched_from_server_ps") },
	{ STR_W_LEN("rows_buffered_from_client_normal") },
	{ STR_W_LEN("rows_buffered_from_client_ps") },
	{ STR_W_LEN("rows_fetched_from_client_normal_buffered") },
	{ STR_W_LEN("rows_fetched_from_client_normal_unbuffered") },
	{ STR_W_LEN("rows_fetched_from_client_ps_buffered") },
	{ STR_W_LEN("rows_fetched_from_client_ps_unbuffered") },
	{ STR_W_LEN("rows_fetched_from_client_ps_cursor") },
	{ STR_W_LEN("rows_affected_normal") },
	{ STR_W_LEN("rows_affected_ps") },
	{ STR_W_LEN("rows_skipped_normal") },
	{ STR_W_LEN("rows_skipped_ps") },
	{ STR_W_LEN("copy_on_write_saved") },
	{ STR_W_LEN("copy_on_write_performed") },
	{ STR_W_LEN("command_buffer_too_small") },
	{ STR_W_LEN("connect_success") },
	{ STR_W_LEN("connect_failure") },
	{ STR_W_LEN("connection_reused") },
	{ STR_W_LEN("reconnect") },
	{ STR_W_LEN("pconnect_success") },
	{ STR_W_LEN("active_connections") },
	{ STR_W_LEN("active_persistent_connections") },
	{ STR_W_LEN("explicit_close") },
	{ STR_W_LEN("implicit_close") },
	{ STR_W_LEN("disconnect_close") },
	{ STR_W_LEN("in_middle_of_command_close") },
	{ STR_W_LEN("explicit_free_result") },
	{ STR_W_LEN("implicit_free_result") },
	{ STR_W_LEN("explicit_stmt_close") },
	{ STR_W_LEN("implicit_stmt_close") },
	{ STR_W_LEN("mem_emalloc_count") },
	{ STR_W_LEN("mem_emalloc_amount") },
	{ STR_W_LEN("mem_ecalloc_count") },
	{ STR_W_LEN("mem_ecalloc_amount") },
	{ STR_W_LEN("mem_erealloc_count") },
	{ STR_W_LEN("mem_erealloc_amount") },
	{ STR_W_LEN("mem_efree_count") },
	{ STR_W_LEN("mem_efree_amount") },
	{ STR_W_LEN("mem_malloc_count") },
	{ STR_W_LEN("mem_malloc_amount") },
	{ STR_W_LEN("mem_calloc_count") },
	{ STR_W_LEN("mem_calloc_amount") },
	{ STR_W_LEN("mem_realloc_count") },
	{ STR_W_LEN("mem_realloc_amount") },
	{ STR_W_LEN("mem_free_count") },
	{ STR_W_LEN("mem_free_amount") },
	{ STR_W_LEN("mem_estrndup_count") },
	{ STR_W_LEN("mem_strndup_count") },
	{ STR_W_LEN("mem_estndup_count") },
	{ STR_W_LEN("mem_strdup_count") },
	{ STR_W_LEN("proto_text_fetched_null") },
	{ STR_W_LEN("proto_text_fetched_bit") },
	{ STR_W_LEN("proto_text_fetched_tinyint") },
	{ STR_W_LEN("proto_text_fetched_short") },
	{ STR_W_LEN("proto_text_fetched_int24") },
	{ STR_W_LEN("proto_text_fetched_int") },
	{ STR_W_LEN("proto_text_fetched_bigint") },
	{ STR_W_LEN("proto_text_fetched_decimal") },
	{ STR_W_LEN("proto_text_fetched_float") },
	{ STR_W_LEN("proto_text_fetched_double") },
	{ STR_W_LEN("proto_text_fetched_date") },
	{ STR_W_LEN("proto_text_fetched_year") },
	{ STR_W_LEN("proto_text_fetched_time") },
	{ STR_W_LEN("proto_text_fetched_datetime") },
	{ STR_W_LEN("proto_text_fetched_timestamp") },
	{ STR_W_LEN("proto_text_fetched_string") },
	{ STR_W_LEN("proto_text_fetched_blob") },
	{ STR_W_LEN("proto_text_fetched_enum") },
	{ STR_W_LEN("proto_text_fetched_set") },
	{ STR_W_LEN("proto_text_fetched_geometry") },
	{ STR_W_LEN("proto_text_fetched_other") },
	{ STR_W_LEN("proto_binary_fetched_null") },
	{ STR_W_LEN("proto_binary_fetched_bit") },
	{ STR_W_LEN("proto_binary_fetched_tinyint") },
	{ STR_W_LEN("proto_binary_fetched_short") },
	{ STR_W_LEN("proto_binary_fetched_int24") },
	{ STR_W_LEN("proto_binary_fetched_int") },
	{ STR_W_LEN("proto_binary_fetched_bigint") },
	{ STR_W_LEN("proto_binary_fetched_decimal") },
	{ STR_W_LEN("proto_binary_fetched_float") },
	{ STR_W_LEN("proto_binary_fetched_double") },
	{ STR_W_LEN("proto_binary_fetched_date") },
	{ STR_W_LEN("proto_binary_fetched_year") },
	{ STR_W_LEN("proto_binary_fetched_time") },
	{ STR_W_LEN("proto_binary_fetched_datetime") },
	{ STR_W_LEN("proto_binary_fetched_timestamp") },
	{ STR_W_LEN("proto_binary_fetched_string") },
	{ STR_W_LEN("proto_binary_fetched_blob") },
	{ STR_W_LEN("proto_binary_fetched_enum") },
	{ STR_W_LEN("proto_binary_fetched_set") },
	{ STR_W_LEN("proto_binary_fetched_geometry") },
	{ STR_W_LEN("proto_binary_fetched_other") },
	{ STR_W_LEN("init_command_executed_count") },
	{ STR_W_LEN("init_command_failed_count") },
	{ STR_W_LEN("com_quit") },
	{ STR_W_LEN("com_init_db") },
	{ STR_W_LEN("com_query") },
	{ STR_W_LEN("com_field_list") },
	{ STR_W_LEN("com_create_db") },
	{ STR_W_LEN("com_drop_db") },
	{ STR_W_LEN("com_refresh") },
	{ STR_W_LEN("com_shutdown") },
	{ STR_W_LEN("com_statistics") },
	{ STR_W_LEN("com_process_info") },
	{ STR_W_LEN("com_connect") },
	{ STR_W_LEN("com_process_kill") },
	{ STR_W_LEN("com_debug") },
	{ STR_W_LEN("com_ping") },
	{ STR_W_LEN("com_time") },
	{ STR_W_LEN("com_delayed_insert") },
	{ STR_W_LEN("com_change_user") },
	{ STR_W_LEN("com_binlog_dump") },
	{ STR_W_LEN("com_table_dump") },
	{ STR_W_LEN("com_connect_out") },
	{ STR_W_LEN("com_register_slave") },
	{ STR_W_LEN("com_stmt_prepare") },
	{ STR_W_LEN("com_stmt_execute") },
	{ STR_W_LEN("com_stmt_send_long_data") },
	{ STR_W_LEN("com_stmt_close") },
	{ STR_W_LEN("com_stmt_reset") },
	{ STR_W_LEN("com_stmt_set_option") },
	{ STR_W_LEN("com_stmt_fetch") },
	{ STR_W_LEN("com_deamon") },
	{ STR_W_LEN("bytes_received_real_data_normal") },
	{ STR_W_LEN("bytes_received_real_data_ps") }
};
/* }}} */


/* {{{ mysqlnd_fill_stats_hash */
PHPAPI void
mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, const MYSQLND_STRING * names, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	unsigned int i;

	mysqlnd_array_init(return_value, stats->count);
	for (i = 0; i < stats->count; i++) {
#if MYSQLND_UNICODE
		UChar *ustr, *tstr;
		int ulen, tlen;
#endif
		char tmp[25];

		sprintf((char *)&tmp, MYSQLND_LLU_SPEC, stats->values[i]);
#if MYSQLND_UNICODE
		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, names[i].s, names[i].l + 1 TSRMLS_CC);
		zend_string_to_unicode(UG(utf8_conv), &tstr, &tlen, tmp, strlen(tmp) + 1 TSRMLS_CC);
		add_u_assoc_unicode_ex(return_value, IS_UNICODE, ZSTR(ustr), ulen, tstr, 1);
		efree(ustr);
		efree(tstr);
#else
		add_assoc_string_ex(return_value, names[i].s, names[i].l + 1, tmp, 1);
#endif
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
