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
	{ STR_W_LEN("mem_emalloc_ammount") },
	{ STR_W_LEN("mem_ecalloc_count") },
	{ STR_W_LEN("mem_ecalloc_ammount") },
	{ STR_W_LEN("mem_erealloc_count") },
	{ STR_W_LEN("mem_erealloc_ammount") },
	{ STR_W_LEN("mem_efree_count") },
	{ STR_W_LEN("mem_malloc_count") },
	{ STR_W_LEN("mem_malloc_ammount") },
	{ STR_W_LEN("mem_calloc_count") },
	{ STR_W_LEN("mem_calloc_ammount") },
	{ STR_W_LEN("mem_realloc_count") },
	{ STR_W_LEN("mem_realloc_ammount") },
	{ STR_W_LEN("mem_free_count") },
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
	{ STR_W_LEN("proto_binary_fetched_other") }	
};
/* }}} */


/* {{{ mysqlnd_fill_stats_hash */
void
mysqlnd_fill_stats_hash(const MYSQLND_STATS * const stats, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	unsigned int i;

	mysqlnd_array_init(return_value, STAT_LAST);
	for (i = 0; i < STAT_LAST; i++) {
		char tmp[22];
		
		sprintf((char *)&tmp, MYSQLND_LLU_SPEC, stats->values[i]);
#if PHP_MAJOR_VERSION >= 6
		if (UG(unicode)) {
			UChar *ustr, *tstr;
			int ulen, tlen;

			zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, mysqlnd_stats_values_names[i].s,
									mysqlnd_stats_values_names[i].l + 1 TSRMLS_CC);
			zend_string_to_unicode(UG(utf8_conv), &tstr, &tlen, tmp, strlen(tmp) + 1 TSRMLS_CC);
			add_u_assoc_unicode_ex(return_value, IS_UNICODE, ZSTR(ustr), ulen, tstr, 1);
			efree(ustr);
			efree(tstr);
		} else
#endif		
		{	
			add_assoc_string_ex(return_value, mysqlnd_stats_values_names[i].s,
								mysqlnd_stats_values_names[i].l + 1, tmp, 1);
		}
	}
}
/* }}} */


/* {{{ _mysqlnd_get_client_stats */
PHPAPI void _mysqlnd_get_client_stats(zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	MYSQLND_STATS stats, *stats_ptr = mysqlnd_global_stats;
	DBG_ENTER("_mysqlnd_get_client_stats");
	if (!stats_ptr) {
		memset(&stats, 0, sizeof(stats));
		stats_ptr = &stats;
	}
	mysqlnd_fill_stats_hash(stats_ptr, return_value TSRMLS_CC ZEND_FILE_LINE_CC);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_stats_init */
void
mysqlnd_stats_init(MYSQLND_STATS ** stats)
{
	*stats = calloc(1, sizeof(MYSQLND_STATS));
#ifdef ZTS
	(*stats)->LOCK_access = tsrm_mutex_alloc();
#endif

}
/* }}} */


/* {{{ mysqlnd_stats_end */
void
mysqlnd_stats_end(MYSQLND_STATS * stats)
{
#ifdef ZTS
	tsrm_mutex_free(stats->LOCK_access);
#endif
	/* mnd_free will reference LOCK_access and crash...*/
	free(stats);
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
