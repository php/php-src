/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli.h"


/* {{{ mysqli_functions[]
 *
 * Every user visible function must have an entry in mysqli_functions[].
 */
function_entry mysqli_functions[] = {
	PHP_FE(mysqli_affected_rows,						NULL)
	PHP_FE(mysqli_autocommit,							NULL)
	PHP_FE(mysqli_bind_param,							NULL)
	PHP_FE(mysqli_bind_result,							NULL)
	PHP_FE(mysqli_change_user,							NULL)
	PHP_FE(mysqli_character_set_name,					NULL)
	PHP_FALIAS(mysqli_client_encoding, 
			   mysqli_character_set_name,				NULL)
	PHP_FE(mysqli_close,								NULL)
	PHP_FE(mysqli_commit,								NULL)
	PHP_FE(mysqli_connect, 								NULL)
	PHP_FE(mysqli_data_seek,							NULL)
	PHP_FE(mysqli_debug,								NULL)
	PHP_FE(mysqli_disable_reads_from_master,			NULL)
	PHP_FE(mysqli_disable_rpl_parse,					NULL)
	PHP_FE(mysqli_dump_debug_info,						NULL)
	PHP_FE(mysqli_enable_reads_from_master,				NULL)
	PHP_FE(mysqli_enable_rpl_parse,						NULL)
	PHP_FE(mysqli_errno,								NULL)
	PHP_FE(mysqli_error,								NULL)
	PHP_FE(mysqli_execute,								NULL)
	PHP_FE(mysqli_fetch,								NULL)
	PHP_FE(mysqli_fetch_field,							NULL)
	PHP_FE(mysqli_fetch_fields,							NULL)
	PHP_FE(mysqli_fetch_field_direct,					NULL)
	PHP_FE(mysqli_fetch_lengths,						NULL)
	PHP_FE(mysqli_fetch_array,							NULL)
	PHP_FE(mysqli_fetch_assoc,							NULL) 
	PHP_FE(mysqli_fetch_object,							NULL) 
	PHP_FE(mysqli_fetch_row,							NULL)
	PHP_FE(mysqli_field_count,							NULL)
	PHP_FE(mysqli_field_seek,							NULL)
	PHP_FE(mysqli_field_tell,							NULL)
	PHP_FE(mysqli_free_result,							NULL)
	PHP_FE(mysqli_get_client_info,						NULL)
	PHP_FE(mysqli_get_host_info,						NULL)
	PHP_FE(mysqli_get_proto_info,						NULL)
	PHP_FE(mysqli_get_server_info,						NULL)
	PHP_FE(mysqli_get_server_version,					NULL)
	PHP_FE(mysqli_init, 								NULL)
	PHP_FE(mysqli_info,									NULL)
	PHP_FE(mysqli_insert_id,							NULL)
	PHP_FE(mysqli_kill,									NULL)
	PHP_FE(mysqli_master_query,							NULL)
	PHP_FE(mysqli_num_fields,							NULL)
	PHP_FE(mysqli_num_rows,								NULL)
	PHP_FE(mysqli_options, 								NULL)
	PHP_FE(mysqli_param_count,							NULL)
	PHP_FE(mysqli_ping,									NULL)
	PHP_FE(mysqli_prepare,								NULL)
	PHP_FE(mysqli_prepare_result,						NULL)
	PHP_FE(mysqli_profiler,								NULL)
	PHP_FE(mysqli_query,								NULL)
	PHP_FE(mysqli_read_query_result,					NULL)
	PHP_FE(mysqli_real_connect,							NULL)
	PHP_FE(mysqli_real_escape_string,					NULL)
	PHP_FALIAS(mysqli_escape_string, 
				mysqli_real_escape_string,				NULL)
	PHP_FE(mysqli_real_query,							NULL)
	PHP_FE(mysqli_reload,								NULL)
	PHP_FE(mysqli_rollback,								NULL)
	PHP_FE(mysqli_rpl_parse_enabled,					NULL)
	PHP_FE(mysqli_rpl_probe,							NULL)
	PHP_FE(mysqli_rpl_query_type,						NULL)
	PHP_FE(mysqli_select_db,							NULL)
	PHP_FE(mysqli_send_long_data,						NULL)
	PHP_FE(mysqli_send_query,							NULL)
	PHP_FALIAS(mysqli_set_opt, 		mysqli_options, 	NULL)
	PHP_FE(mysqli_slave_query,							NULL)
	PHP_FE(mysqli_ssl_set,								NULL)
	PHP_FE(mysqli_stat,									NULL)
	PHP_FE(mysqli_stmt_affected_rows,					NULL)
	PHP_FE(mysqli_stmt_close,							NULL)
	PHP_FE(mysqli_stmt_errno,							NULL)
	PHP_FE(mysqli_stmt_error,							NULL)
	PHP_FE(mysqli_store_result,							NULL)
	PHP_FE(mysqli_stmt_store_result,					NULL)
	PHP_FE(mysqli_thread_id,							NULL)
	PHP_FE(mysqli_thread_safe,							NULL)
	PHP_FE(mysqli_use_result,							NULL)
	PHP_FE(mysqli_warning_count,						NULL)
	{NULL, NULL, NULL}	/* Must be the last line in mysqli_functions[] */
};
/* }}} */

/* {{{ mysqli_link_methods[]
 *
 * Every user visible function must have an entry in mysqli_functions[].
 */
function_entry mysqli_link_methods[] = {
	PHP_FALIAS(affected_rows,mysqli_affected_rows,NULL)
	PHP_FALIAS(autocommit,mysqli_autocommit,NULL)
	PHP_FALIAS(change_user,mysqli_change_user,NULL)
	PHP_FALIAS(character_set_name,mysqli_character_set_name,NULL)
	PHP_FALIAS(client_encoding, mysqli_character_set_name,NULL)
	PHP_FALIAS(close,mysqli_close,NULL)
	PHP_FALIAS(commit,mysqli_commit,NULL)
	PHP_FALIAS(connect,mysqli_connect,NULL)
	PHP_FALIAS(debug,mysqli_debug,NULL)
	PHP_FALIAS(disable_reads_from_master,mysqli_disable_reads_from_master,NULL)
	PHP_FALIAS(disable_rpl_parse,mysqli_disable_rpl_parse,NULL)
	PHP_FALIAS(dump_debug_info,mysqli_dump_debug_info,NULL)
	PHP_FALIAS(enable_reads_from_master,mysqli_enable_reads_from_master,NULL)
	PHP_FALIAS(enable_rpl_parse,mysqli_enable_rpl_parse,NULL)
	PHP_FALIAS(errno,mysqli_errno,NULL)
	PHP_FALIAS(error,mysqli_error,NULL)
	PHP_FALIAS(get_client_info,mysqli_get_client_info,NULL)
	PHP_FALIAS(get_host_info,mysqli_get_host_info,NULL)
	PHP_FALIAS(get_proto_info,mysqli_get_proto_info,NULL)
	PHP_FALIAS(get_server_info,mysqli_get_server_info,NULL)
	PHP_FALIAS(get_server_version,mysqli_get_server_version,NULL)
	PHP_FALIAS(init,mysqli_init,NULL)
	PHP_FALIAS(info,mysqli_info,NULL)
	PHP_FALIAS(insert_id,mysqli_insert_id,NULL)
	PHP_FALIAS(kill,mysqli_kill,NULL)
	PHP_FALIAS(master_query,mysqli_master_query,NULL)
	PHP_FALIAS(options,mysqli_options,NULL)
	PHP_FALIAS(ping,mysqli_ping,NULL)
	PHP_FALIAS(prepare,mysqli_prepare,NULL)
	PHP_FALIAS(query,mysqli_query,NULL)
	PHP_FALIAS(read_query_result,mysqli_read_query_result,NULL)
	PHP_FALIAS(real_connect,mysqli_real_connect,NULL)
	PHP_FALIAS(real_escape_string,mysqli_real_escape_string,NULL)
	PHP_FALIAS(escape_string, mysqli_real_escape_string,NULL)
	PHP_FALIAS(real_query,mysqli_real_query,NULL)
	PHP_FALIAS(reload,mysqli_reload,NULL)
	PHP_FALIAS(rollback,mysqli_rollback,NULL)
	PHP_FALIAS(rpl_parse_enabled,mysqli_rpl_parse_enabled,NULL)
	PHP_FALIAS(rpl_probe,mysqli_rpl_probe,NULL)
	PHP_FALIAS(rpl_query_type,mysqli_rpl_query_type,NULL)
	PHP_FALIAS(select_db,mysqli_select_db,NULL)
	PHP_FALIAS(set_opt, mysqli_options,NULL)
	PHP_FALIAS(slave_query,mysqli_slave_query,NULL)
	PHP_FALIAS(ssl_set,mysqli_ssl_set,NULL)
	PHP_FALIAS(stat,mysqli_stat,NULL)
	PHP_FALIAS(store_result,mysqli_store_result,NULL)
	PHP_FALIAS(thread_id, mysqli_thread_id,NULL)
	PHP_FALIAS(thread_safe,mysqli_thread_safe,NULL)
	PHP_FALIAS(use_result,mysqli_use_result,NULL)
	PHP_FALIAS(warning_count,mysqli_warning_count,NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mysqli_result_methods[]
 *
 * Every user visible function must have an entry in mysqli_result_functions[].
 */
function_entry mysqli_result_methods[] = {
	PHP_FALIAS(close,mysqli_free_result,NULL)
	PHP_FALIAS(free,mysqli_free_result,NULL)
	PHP_FALIAS(data_seek,mysqli_data_seek,NULL)
	PHP_FALIAS(fetch_field,mysqli_fetch_field,NULL)
	PHP_FALIAS(fetch_fields,mysqli_fetch_fields,NULL)
	PHP_FALIAS(fetvh_field_direct,mysqli_fetch_field_direct,NULL)
	PHP_FALIAS(fetch_lengths,mysqli_fetch_lengths,NULL)
	PHP_FALIAS(fetch_array,mysqli_fetch_array,NULL)
	PHP_FALIAS(fetch_assoc,mysqli_fetch_assoc,NULL) 
	PHP_FALIAS(fetch_object,mysqli_fetch_object,NULL) 
	PHP_FALIAS(fetch_row,mysqli_fetch_row,NULL)
	PHP_FALIAS(field_count,mysqli_field_count,NULL)
	PHP_FALIAS(field_seek,mysqli_field_seek,NULL)
	PHP_FALIAS(field_tell,mysqli_field_tell,NULL)
	PHP_FALIAS(free_result,mysqli_free_result,NULL)
	PHP_FALIAS(num_fields,mysqli_num_fields,NULL)
	PHP_FALIAS(num_rows,mysqli_num_rows,NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mysqli_stmt_methods[]
 *
 * Every user visible function must have an entry in mysqli_stmt_functions[].
 */
function_entry mysqli_stmt_methods[] = {
	PHP_FALIAS(affected_rows,mysqli_stmt_affected_rows,NULL)
	PHP_FALIAS(bind_param,mysqli_bind_param,NULL)
	PHP_FALIAS(bind_result,mysqli_bind_result,NULL)
	PHP_FALIAS(execute,mysqli_execute,NULL)
	PHP_FALIAS(fetch,mysqli_fetch,NULL)
	PHP_FALIAS(param_count,mysqli_param_count,NULL)
	PHP_FALIAS(send_long_data,mysqli_send_long_data,NULL)
	PHP_FALIAS(store_result,mysqli_stmt_store_result,NULL)
	PHP_FALIAS(close,mysqli_stmt_close,NULL)
	PHP_FALIAS(errno,mysqli_stmt_errno,NULL)
	PHP_FALIAS(error,mysqli_stmt_error,NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
