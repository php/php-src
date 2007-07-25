/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifndef PHP_MYSQLI_H
#define PHP_MYSQLI_H


PHP_MINIT_FUNCTION(mysqli);
PHP_MSHUTDOWN_FUNCTION(mysqli);
PHP_RINIT_FUNCTION(mysqli);
PHP_RSHUTDOWN_FUNCTION(mysqli);
PHP_MINFO_FUNCTION(mysqli);

PHP_FUNCTION(mysqli);
PHP_FUNCTION(mysqli_affected_rows);
PHP_FUNCTION(mysqli_autocommit);
PHP_FUNCTION(mysqli_change_user);
PHP_FUNCTION(mysqli_character_set_name);
PHP_FUNCTION(mysqli_set_charset);
PHP_FUNCTION(mysqli_close);
PHP_FUNCTION(mysqli_commit);
PHP_FUNCTION(mysqli_connect);
PHP_FUNCTION(mysqli_connect_errno);
PHP_FUNCTION(mysqli_connect_error);
PHP_FUNCTION(mysqli_data_seek);
PHP_FUNCTION(mysqli_debug);
PHP_FUNCTION(mysqli_disable_reads_from_master);
PHP_FUNCTION(mysqli_disable_rpl_parse);
PHP_FUNCTION(mysqli_dump_debug_info);
PHP_FUNCTION(mysqli_enable_reads_from_master);
PHP_FUNCTION(mysqli_enable_rpl_parse);
PHP_FUNCTION(mysqli_errno);
PHP_FUNCTION(mysqli_error);
PHP_FUNCTION(mysqli_fetch_all);
PHP_FUNCTION(mysqli_fetch_array);
PHP_FUNCTION(mysqli_fetch_assoc);
PHP_FUNCTION(mysqli_fetch_object);
PHP_FUNCTION(mysqli_fetch_field);
PHP_FUNCTION(mysqli_fetch_fields);
PHP_FUNCTION(mysqli_fetch_field_direct);
PHP_FUNCTION(mysqli_fetch_lengths);
PHP_FUNCTION(mysqli_fetch_row);
PHP_FUNCTION(mysqli_field_count);
PHP_FUNCTION(mysqli_field_seek);
PHP_FUNCTION(mysqli_field_tell);
PHP_FUNCTION(mysqli_free_result);
PHP_FUNCTION(mysqli_get_cache_stats);
PHP_FUNCTION(mysqli_get_client_stats);
PHP_FUNCTION(mysqli_get_connection_stats);
PHP_FUNCTION(mysqli_get_charset);
PHP_FUNCTION(mysqli_get_client_info);
PHP_FUNCTION(mysqli_get_client_version);
PHP_FUNCTION(mysqli_get_host_info);
PHP_FUNCTION(mysqli_get_proto_info);
PHP_FUNCTION(mysqli_get_server_info);
PHP_FUNCTION(mysqli_get_server_version);
PHP_FUNCTION(mysqli_get_warnings);
PHP_FUNCTION(mysqli_info);
PHP_FUNCTION(mysqli_insert_id);
PHP_FUNCTION(mysqli_init);
PHP_FUNCTION(mysqli_kill);
PHP_FUNCTION(mysqli_set_local_infile_default);
PHP_FUNCTION(mysqli_set_local_infile_handler);
PHP_FUNCTION(mysqli_master_query);
PHP_FUNCTION(mysqli_more_results);
PHP_FUNCTION(mysqli_multi_query);
PHP_FUNCTION(mysqli_next_result);
PHP_FUNCTION(mysqli_num_fields);
PHP_FUNCTION(mysqli_num_rows);
PHP_FUNCTION(mysqli_options);
PHP_FUNCTION(mysqli_ping);
PHP_FUNCTION(mysqli_prepare);
PHP_FUNCTION(mysqli_query);
PHP_FUNCTION(mysqli_stmt_result_metadata);
PHP_FUNCTION(mysqli_report);
PHP_FUNCTION(mysqli_read_query_result);
PHP_FUNCTION(mysqli_real_connect);
PHP_FUNCTION(mysqli_real_query);
PHP_FUNCTION(mysqli_real_escape_string);
PHP_FUNCTION(mysqli_rollback);
PHP_FUNCTION(mysqli_row_seek);
PHP_FUNCTION(mysqli_rpl_parse_enabled);
PHP_FUNCTION(mysqli_rpl_probe);
PHP_FUNCTION(mysqli_rpl_query_type);
PHP_FUNCTION(mysqli_select_db);
PHP_FUNCTION(mysqli_stmt_attr_get);
PHP_FUNCTION(mysqli_stmt_attr_set);
PHP_FUNCTION(mysqli_stmt_bind_param);
PHP_FUNCTION(mysqli_stmt_bind_result);
PHP_FUNCTION(mysqli_stmt_execute);
PHP_FUNCTION(mysqli_stmt_field_count);
PHP_FUNCTION(mysqli_stmt_init);
PHP_FUNCTION(mysqli_stmt_prepare);
PHP_FUNCTION(mysqli_stmt_fetch);
PHP_FUNCTION(mysqli_stmt_param_count);
PHP_FUNCTION(mysqli_stmt_send_long_data);
PHP_FUNCTION(mysqli_send_query);
PHP_FUNCTION(mysqli_embedded_server_end);
PHP_FUNCTION(mysqli_embedded_server_start);
PHP_FUNCTION(mysqli_slave_query);
PHP_FUNCTION(mysqli_sqlstate);
PHP_FUNCTION(mysqli_ssl_set);
PHP_FUNCTION(mysqli_stat);
PHP_FUNCTION(mysqli_stmt_affected_rows);
PHP_FUNCTION(mysqli_stmt_close);
PHP_FUNCTION(mysqli_stmt_data_seek);
PHP_FUNCTION(mysqli_stmt_errno);
PHP_FUNCTION(mysqli_stmt_error);
PHP_FUNCTION(mysqli_stmt_free_result);
PHP_FUNCTION(mysqli_stmt_get_result);
PHP_FUNCTION(mysqli_stmt_get_warnings);
PHP_FUNCTION(mysqli_stmt_reset);
PHP_FUNCTION(mysqli_stmt_insert_id);
PHP_FUNCTION(mysqli_stmt_num_rows);
PHP_FUNCTION(mysqli_stmt_sqlstate);
PHP_FUNCTION(mysqli_stmt_store_result);
PHP_FUNCTION(mysqli_store_result);
PHP_FUNCTION(mysqli_thread_id);
PHP_FUNCTION(mysqli_thread_safe);
PHP_FUNCTION(mysqli_use_result);
PHP_FUNCTION(mysqli_warning_count);

ZEND_FUNCTION(mysqli_stmt_construct);
ZEND_FUNCTION(mysqli_result_construct);
ZEND_FUNCTION(mysqli_driver_construct);
ZEND_METHOD(mysqli_warning,__construct);

#define phpext_mysqli_ptr &mysqli_module_entry
extern zend_module_entry mysqli_module_entry;

#endif	/* PHP_MYSQLI.H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
