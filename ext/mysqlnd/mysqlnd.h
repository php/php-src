/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_H
#define MYSQLND_H

#define PHP_MYSQLND_VERSION "mysqlnd " PHP_VERSION
#define MYSQLND_VERSION_ID PHP_VERSION_ID

#define MYSQLND_PLUGIN_API_VERSION 2

/*
  This force mysqlnd to do a single (or more depending on amount of data)
  non-blocking read() calls before sending a command to the server. Useful
  for debugging, if previous function hasn't consumed all the output sent
  to it - like stmt_send_long_data() error because the data was larger that
  max_allowed_packet_size, and COM_STMT_SEND_LONG_DATA by protocol doesn't
  use response packets, thus letting the next command to fail miserably, if
  the connector implementor is not aware of this deficiency. Should be off
  on production systems, if of course measured performance degradation is not
  minimal.
*/
#if defined(A0) && PHP_DEBUG
#define MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND 1
#endif

#if PHP_DEBUG
#define MYSQLND_DBG_ENABLED 1
#else
#define MYSQLND_DBG_ENABLED 0
#endif

#define MYSQLND_CHARSETS_SANITY_CHECK 0

#ifdef ZTS
#include "TSRM.h"
#endif

#include "mysqlnd_portability.h"
#include "mysqlnd_enum_n_def.h"
#include "mysqlnd_structs.h"

#define MYSQLND_STR_W_LEN(str)  str, (sizeof(str) - 1)

/* Library related */
PHPAPI void mysqlnd_library_init(void);
PHPAPI void mysqlnd_library_end(void);

PHPAPI unsigned int mysqlnd_plugin_register(void);
PHPAPI unsigned int mysqlnd_plugin_register_ex(struct st_mysqlnd_plugin_header * plugin);
PHPAPI unsigned int mysqlnd_plugin_count(void);
PHPAPI void * mysqlnd_plugin_find(const char * const name);

PHPAPI void mysqlnd_plugin_apply_with_argument(apply_func_arg_t apply_func, void * argument);

#define mysqlnd_restart_psession(conn)	((conn)->data)->m->restart_psession((conn)->data)
#define mysqlnd_end_psession(conn)		((conn)->data)->m->end_psession((conn)->data)
PHPAPI void mysqlnd_minfo_print_hash(zval *values);
#define mysqlnd_thread_safe()	true

PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_nr(const unsigned int charsetno);
PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_name(const char * const charsetname);


/* Connect */
#define mysqlnd_init(flags, persistent)		mysqlnd_connection_init((flags), (persistent), NULL /*use default factory*/)
#define mysqlnd_connect(conn, host, user, pass, pass_len, db, db_len, port, socket, mysql_flags, client_api_flags) \
			mysqlnd_connection_connect((conn), (host), (user), (pass), (pass_len), (db), (db_len), (port), (socket), (mysql_flags), (client_api_flags))

PHPAPI MYSQLND * mysqlnd_connection_init(const size_t client_flags, const bool persistent, MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *object_factory);
PHPAPI MYSQLND * mysqlnd_connection_connect(MYSQLND * conn,
											const char * const host,
											const char * const user,
											const char * const passwd, unsigned int passwd_len,
											const char * const db, unsigned int db_len,
											unsigned int port,
											const char * const socket_or_pipe,
											unsigned int mysql_flags,
											unsigned int client_api_flags
										);

#define mysqlnd_change_user(conn, user, passwd, db, silent)					((conn)->data)->m->change_user((conn)->data, (user), (passwd), (db), (silent), strlen((passwd)))
#define mysqlnd_change_user_ex(conn, user, passwd, db, silent, passwd_len)	((conn)->data)->m->change_user((conn)->data, (user), (passwd), (db), (silent), (passwd_len))

PHPAPI void mysqlnd_debug(const char *mode);

/* Query */
#define mysqlnd_fetch_into(result, flags, ret_val)	(result)->m.fetch_into((result), (flags), (ret_val) ZEND_FILE_LINE_CC)
#define mysqlnd_fetch_row_c(result)						(result)->m.fetch_row_c((result))
#define mysqlnd_fetch_row_zval(result, row_ptr, fetched) \
	(result)->m.fetch_row((result), (row_ptr), 0, (fetched))
#define mysqlnd_get_connection_stats(conn, values)		((conn)->data)->m->get_statistics((conn)->data,  (values) ZEND_FILE_LINE_CC)
#define mysqlnd_get_client_stats(values)				_mysqlnd_get_client_stats(mysqlnd_global_stats, (values) ZEND_FILE_LINE_CC)

#define mysqlnd_close(conn,is_forced)					(conn)->m->close((conn), (is_forced))
#define mysqlnd_query(conn, query_str, query_len)		((conn)->data)->m->query((conn)->data, (query_str), (query_len))
#define mysqlnd_async_query(conn, query_str, query_len)	((conn)->data)->m->send_query((conn)->data, (query_str), (query_len), NULL, NULL)
#define mysqlnd_reap_async_query(conn)					((conn)->data)->m->reap_query((conn)->data)

PHPAPI enum_func_status mysqlnd_poll(MYSQLND **r_array, MYSQLND **e_array, MYSQLND ***dont_poll, long sec, long usec, int * desc_num);

#define mysqlnd_use_result(conn)		((conn)->data)->m->use_result((conn)->data)
#define mysqlnd_store_result(conn)		((conn)->data)->m->store_result((conn)->data)
#define mysqlnd_next_result(conn)		((conn)->data)->m->next_result((conn)->data)
#define mysqlnd_more_results(conn)		((conn)->data)->m->more_results((conn)->data)
#define mysqlnd_free_result(r,e_or_i)	((MYSQLND_RES*)r)->m.free_result(((MYSQLND_RES*)(r)), (e_or_i))
#define mysqlnd_data_seek(result, row)	(result)->m.seek_data((result), (row))

/* Errors */
#define mysqlnd_errno(conn)				((conn)->data)->m->get_error_no((conn)->data)
#define mysqlnd_error(conn)				((conn)->data)->m->get_error_str((conn)->data)
#define mysqlnd_sqlstate(conn)			((conn)->data)->m->get_sqlstate((conn)->data)

/* Charset */
#define mysqlnd_character_set_name(conn) ((conn)->data)->m->charset_name((conn)->data)

/* Simple metadata */
#define mysqlnd_field_count(conn)		((conn)->data)->m->get_field_count((conn)->data)
#define mysqlnd_insert_id(conn)			((conn)->data)->m->get_last_insert_id((conn)->data)
#define mysqlnd_affected_rows(conn)		((conn)->data)->m->get_affected_rows((conn)->data)
#define mysqlnd_warning_count(conn)		((conn)->data)->m->get_warning_count((conn)->data)
#define mysqlnd_info(conn)				((conn)->data)->m->get_last_message((conn)->data)
#define mysqlnd_get_server_info(conn)	((conn)->data)->m->get_server_information((conn)->data)
#define mysqlnd_get_server_version(conn) ((conn)->data)->m->get_server_version((conn)->data)
#define mysqlnd_get_host_info(conn)		((conn)->data)->m->get_host_information((conn)->data)
#define mysqlnd_get_proto_info(conn)	((conn)->data)->m->get_protocol_information((conn)->data)
#define mysqlnd_thread_id(conn)			((conn)->data)->m->get_thread_id((conn)->data)
#define mysqlnd_get_server_status(conn)	((conn)->data)->m->get_server_status((conn)->data)

#define mysqlnd_num_rows(result)		(result)->m.num_rows((result))
#define mysqlnd_num_fields(result)		(result)->m.num_fields((result))

#define mysqlnd_fetch_lengths(result)	(result)->m.fetch_lengths((result))

#define mysqlnd_field_seek(result, ofs)			(result)->m.seek_field((result), (ofs))
#define mysqlnd_field_tell(result)				(result)->m.field_tell((result))
#define mysqlnd_fetch_field(result)				(result)->m.fetch_field((result))
#define mysqlnd_fetch_field_direct(result,fnr)	(result)->m.fetch_field_direct((result), (fnr))
#define mysqlnd_fetch_fields(result)			(result)->m.fetch_fields((result))

/* mysqlnd metadata */
PHPAPI const char *	mysqlnd_get_client_info(void);
PHPAPI unsigned long mysqlnd_get_client_version(void);

#define mysqlnd_ssl_set(conn, key, cert, ca, capath, cipher) ((conn)->data)->m->ssl_set((conn)->data, (key), (cert), (ca), (capath), (cipher))

/* PS */
#define mysqlnd_stmt_insert_id(stmt)		(stmt)->m->get_last_insert_id((stmt))
#define mysqlnd_stmt_affected_rows(stmt)	(stmt)->m->get_affected_rows((stmt))
#define mysqlnd_stmt_num_rows(stmt)			(stmt)->m->get_num_rows((stmt))
#define mysqlnd_stmt_param_count(stmt)		(stmt)->m->get_param_count((stmt))
#define mysqlnd_stmt_field_count(stmt)		(stmt)->m->get_field_count((stmt))
#define mysqlnd_stmt_warning_count(stmt)	(stmt)->m->get_warning_count((stmt))
#define mysqlnd_stmt_server_status(stmt)	(stmt)->m->get_server_status((stmt))
#define mysqlnd_stmt_errno(stmt)			(stmt)->m->get_error_no((stmt))
#define mysqlnd_stmt_error(stmt)			(stmt)->m->get_error_str((stmt))
#define mysqlnd_stmt_sqlstate(stmt)			(stmt)->m->get_sqlstate((stmt))


PHPAPI void mysqlnd_efree_param_bind_dtor(MYSQLND_PARAM_BIND * param_bind);
PHPAPI void mysqlnd_efree_result_bind_dtor(MYSQLND_RESULT_BIND * result_bind);
PHPAPI void mysqlnd_free_param_bind_dtor(MYSQLND_PARAM_BIND * param_bind);
PHPAPI void mysqlnd_free_result_bind_dtor(MYSQLND_RESULT_BIND * result_bind);


/* LOAD DATA LOCAL */
PHPAPI void mysqlnd_local_infile_default(MYSQLND_CONN_DATA * conn);

/* Simple commands */
#define mysqlnd_autocommit(conn, mode)		((conn)->data)->m->set_autocommit((conn)->data, (mode))
#define mysqlnd_begin_transaction(conn,flags,name) ((conn)->data)->m->tx_begin((conn)->data, (flags), (name))
#define mysqlnd_commit(conn, flags, name)	((conn)->data)->m->tx_commit_or_rollback((conn)->data, true, (flags), (name))
#define mysqlnd_rollback(conn, flags, name)	((conn)->data)->m->tx_commit_or_rollback((conn)->data, false, (flags), (name))
#define mysqlnd_savepoint(conn, name)		((conn)->data)->m->tx_savepoint((conn)->data, (name))
#define mysqlnd_release_savepoint(conn, name) ((conn)->data)->m->tx_savepoint_release((conn)->data, (name))
#define mysqlnd_list_dbs(conn, wild)		((conn)->data)->m->list_method((conn)->data, wild? "SHOW DATABASES LIKE %s":"SHOW DATABASES", (wild), NULL)
#define mysqlnd_list_processes(conn)		((conn)->data)->m->list_method((conn)->data, "SHOW PROCESSLIST", NULL, NULL)
#define mysqlnd_list_tables(conn, wild)		((conn)->data)->m->list_method((conn)->data, wild? "SHOW TABLES LIKE %s":"SHOW TABLES", (wild), NULL)
#define mysqlnd_dump_debug_info(conn)		((conn)->data)->m->server_dump_debug_information((conn)->data)
#define mysqlnd_select_db(conn, db, db_len)	((conn)->data)->m->select_db((conn)->data, (db), (db_len))
#define mysqlnd_ping(conn)					((conn)->data)->m->ping((conn)->data)
#define mysqlnd_kill(conn, pid)				((conn)->data)->m->kill_connection((conn)->data, (pid))
#define mysqlnd_refresh(conn, options)		((conn)->data)->m->refresh_server((conn)->data, (options))
#define mysqlnd_set_character_set(conn, cs)	((conn)->data)->m->set_charset((conn)->data, (cs))
#define mysqlnd_stat(conn, msg)				((conn)->data)->m->get_server_statistics(((conn)->data), (msg))
#define mysqlnd_options(conn, opt, value)	((conn)->data)->m->set_client_option((conn)->data, (opt), (value))
#define mysqlnd_options4(conn, opt, k, v)	((conn)->data)->m->set_client_option_2d((conn)->data, (opt), (k), (v))
#define mysqlnd_set_server_option(conn, op)	((conn)->data)->m->set_server_option((conn)->data, (op))

/* Escaping */
#define mysqlnd_real_escape_string(conn, newstr, escapestr, escapestr_len) \
		((conn)->data)->m->escape_string((conn)->data, (newstr), (escapestr), (escapestr_len))
#define mysqlnd_escape_string(newstr, escapestr, escapestr_len) \
		mysqlnd_old_escape_string((newstr), (escapestr), (escapestr_len))

PHPAPI zend_ulong mysqlnd_old_escape_string(char * newstr, const char * escapestr, size_t escapestr_len);


/* PS */
#define mysqlnd_stmt_init(conn)						((conn)->data)->m->stmt_init(((conn)->data))
#define mysqlnd_stmt_store_result(stmt)				(!mysqlnd_stmt_field_count((stmt)) ? PASS:((stmt)->m->store_result((stmt))? PASS:FAIL))
#define mysqlnd_stmt_get_result(stmt)				(stmt)->m->get_result((stmt))
#define mysqlnd_stmt_more_results(stmt)				(stmt)->m->more_results((stmt))
#define mysqlnd_stmt_next_result(stmt)				(stmt)->m->next_result((stmt))
#define mysqlnd_stmt_data_seek(stmt, row)			(stmt)->m->seek_data((stmt), (row))
#define mysqlnd_stmt_prepare(stmt, q, qlen)			(stmt)->m->prepare((stmt), (q), (qlen))
#define mysqlnd_stmt_execute(stmt) 					(stmt)->m->execute((stmt))
#define mysqlnd_stmt_send_long_data(stmt,p,d,l) 	(stmt)->m->send_long_data((stmt), (p), (d), (l))
#define mysqlnd_stmt_alloc_param_bind(stmt)			(stmt)->m->alloc_parameter_bind((stmt))
#define mysqlnd_stmt_free_param_bind(stmt,bind)		(stmt)->m->free_parameter_bind((stmt), (bind))
#define mysqlnd_stmt_bind_param(stmt,bind)			(stmt)->m->bind_parameters((stmt), (bind))
#define mysqlnd_stmt_bind_one_param(stmt,n,z,t)		(stmt)->m->bind_one_parameter((stmt), (n), (z), (t))
#define mysqlnd_stmt_refresh_bind_param(s)			(s)->m->refresh_bind_param((s))
#define mysqlnd_stmt_alloc_result_bind(stmt)		(stmt)->m->alloc_result_bind((stmt))
#define mysqlnd_stmt_free_result_bind(stmt,bind)	(stmt)->m->free_result_bind((stmt), (bind))
#define mysqlnd_stmt_bind_result(stmt,bind)			(stmt)->m->bind_result((stmt), (bind))
#define mysqlnd_stmt_bind_one_result(s,no)			(s)->m->bind_one_result((s), (no))
#define mysqlnd_stmt_result_metadata(stmt)			(stmt)->m->get_result_metadata((stmt))

#define	mysqlnd_stmt_free_result(stmt)				(stmt)->m->free_result((stmt))
#define	mysqlnd_stmt_close(stmt, implicit)			(stmt)->m->dtor((stmt), (implicit))
#define	mysqlnd_stmt_reset(stmt)					(stmt)->m->reset((stmt))
#define	mysqlnd_stmt_flush(stmt)					(stmt)->m->flush((stmt))


#define mysqlnd_stmt_attr_get(stmt, attr, value)	(stmt)->m->get_attribute((stmt), (attr), (value))
#define mysqlnd_stmt_attr_set(stmt, attr, value)	(stmt)->m->set_attribute((stmt), (attr), (value))

#define mysqlnd_stmt_fetch(stmt, fetched)	(stmt)->m->fetch((stmt), (fetched))


/* Performance statistics */
PHPAPI extern MYSQLND_STATS * mysqlnd_global_stats;
PHPAPI extern const MYSQLND_STRING mysqlnd_stats_values_names[];
PHPAPI void			_mysqlnd_get_client_stats(MYSQLND_STATS * stats, zval *return_value ZEND_FILE_LINE_DC);


#ifndef MYSQLND_CORE_STATISTICS_DISABLED

#define MYSQLND_INC_GLOBAL_STATISTIC(statistic) \
	MYSQLND_INC_STATISTIC(MYSQLND_G(collect_statistics), mysqlnd_global_stats, (statistic))

#define MYSQLND_DEC_GLOBAL_STATISTIC(statistic) \
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
#define MYSQLND_DEC_GLOBAL_STATISTIC(statistic)
#define MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(statistic1, value1, statistic2, value2)
#define MYSQLND_INC_CONN_STATISTIC(conn_stats, statistic)
#define MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn_stats, statistic, value)
#define MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn_stats, statistic1, value1, statistic2, value2)
#define MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats, statistic1, value1, statistic2, value2, statistic3, value3)

#endif /* MYSQLND_CORE_STATISTICS_DISABLED */


/* double check the class name to avoid naming conflicts when using these: */
#define MYSQLND_METHOD(class, method) 			mysqlnd_##class##_##method##_pub
#define MYSQLND_METHOD_PRIVATE(class, method)	mysqlnd_##class##_##method##_priv

ZEND_BEGIN_MODULE_GLOBALS(mysqlnd)
	char *			debug;	/* The actual string */
	char *			trace_alloc_settings;	/* The actual string */
	MYSQLND_DEBUG *	dbg;	/* The DBG object for standard tracing */
	MYSQLND_DEBUG *	trace_alloc;	/* The DBG object for allocation tracing */
	zend_long		net_cmd_buffer_size;
	zend_long		net_read_buffer_size;
	zend_long		log_mask;
	zend_long		net_read_timeout;
	zend_long		mempool_default_size;
	char *			sha256_server_public_key;
	bool		collect_statistics;
	bool		collect_memory_statistics;
ZEND_END_MODULE_GLOBALS(mysqlnd)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(mysqlnd)
#define MYSQLND_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(mysqlnd, v)

#if defined(ZTS) && defined(COMPILE_DL_MYSQLND)
ZEND_TSRMLS_CACHE_EXTERN()
#endif


PHPAPI void mysqlnd_minfo_print_hash(zval *values);

#endif	/* MYSQLND_H */
