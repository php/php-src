/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifndef MYSQLND_H
#define MYSQLND_H

#define MYSQLND_VERSION "mysqlnd 5.0.11-dev - 20120503 - $Id$"
#define MYSQLND_VERSION_ID 50011

#define MYSQLND_PLUGIN_API_VERSION 2

#define MYSQLND_STRING_TO_INT_CONVERSION
/*
  This force mysqlnd to do a single (or more depending on ammount of data)
  non-blocking read() calls before sending a command to the server. Useful
  for debugging, if previous function hasn't consumed all the output sent
  to it - like stmt_send_long_data() error because the data was larger that
  max_allowed_packet_size, and COM_STMT_SEND_LONG_DATA by protocol doesn't
  use response packets, thus letting the next command to fail miserably, if
  the connector implementor is not aware of this deficiency. Should be off
  on production systems, if of course measured performance degradation is not
  minimal.
*/
#if A0 && PHP_DEBUG
#define MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND 1
#endif

#if PHP_DEBUG
#define MYSQLND_DBG_ENABLED 1
#else
#define MYSQLND_DBG_ENABLED 0
#endif

#if defined(MYSQLND_COMPRESSION_WANTED) && defined(HAVE_ZLIB)
#define MYSQLND_COMPRESSION_ENABLED 1
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "mysqlnd_portability.h"
#include "mysqlnd_enum_n_def.h"
#include "mysqlnd_structs.h"


/* Library related */
PHPAPI void mysqlnd_library_init(TSRMLS_D);
PHPAPI void mysqlnd_library_end(TSRMLS_D);

PHPAPI unsigned int mysqlnd_plugin_register();
PHPAPI unsigned int mysqlnd_plugin_register_ex(struct st_mysqlnd_plugin_header * plugin TSRMLS_DC);
PHPAPI unsigned int mysqlnd_plugin_count();
PHPAPI void * _mysqlnd_plugin_find(const char * const name TSRMLS_DC);
#define mysqlnd_plugin_find(name) _mysqlnd_plugin_find((name) TSRMLS_CC);

PHPAPI void _mysqlnd_plugin_apply_with_argument(apply_func_arg_t apply_func, void * argument TSRMLS_DC);
#define mysqlnd_plugin_apply_with_argument(func, argument) _mysqlnd_plugin_apply_with_argument((func), (argument) TSRMLS_CC);

#define mysqlnd_restart_psession(conn)	((conn)->data)->m->restart_psession((conn)->data TSRMLS_CC)
#define mysqlnd_end_psession(conn)		((conn)->data)->m->end_psession((conn)->data TSRMLS_CC)
PHPAPI void mysqlnd_minfo_print_hash(zval *values);
#define mysqlnd_thread_safe()	TRUE

PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_nr(unsigned int charsetno);
PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_name(const char * const charsetname);


/* Connect */
#define mysqlnd_init(persistent) _mysqlnd_init((persistent) TSRMLS_CC)
PHPAPI MYSQLND * _mysqlnd_init(zend_bool persistent TSRMLS_DC);
PHPAPI MYSQLND * mysqlnd_connect(MYSQLND * conn,
						  const char * host, const char * user,
						  const char * passwd, unsigned int passwd_len,
						  const char * db, unsigned int db_len,
						  unsigned int port,
						  const char * socket_or_pipe,
						  unsigned int mysql_flags
						  TSRMLS_DC);

#define mysqlnd_change_user(conn, user, passwd, db, silent)		((conn)->data)->m->change_user((conn)->data, (user), (passwd), (db), (silent), strlen((passwd)) TSRMLS_CC)
#define mysqlnd_change_user_ex(conn, user, passwd, db, silent, passwd_len)	((conn)->data)->m->change_user((conn)->data, (user), (passwd), (db), (silent), (passwd_len) TSRMLS_CC)

#define mysqlnd_debug(x)								_mysqlnd_debug((x) TSRMLS_CC)
PHPAPI void _mysqlnd_debug(const char *mode TSRMLS_DC);

/* Query */
#define mysqlnd_fetch_into(result, flags, ret_val, ext)	(result)->m.fetch_into((result), (flags), (ret_val), (ext) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_fetch_row_c(result)						(result)->m.fetch_row_c((result) TSRMLS_CC)
#define mysqlnd_fetch_all(result, flags, return_value)	(result)->m.fetch_all((result), (flags), (return_value) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_result_fetch_field_data(res,offset,ret)	(res)->m.fetch_field_data((res), (offset), (ret) TSRMLS_CC)
#define mysqlnd_get_connection_stats(conn, values)		((conn)->data)->m->get_statistics((conn)->data,  (values) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_get_client_stats(values)				_mysqlnd_get_client_stats((values) TSRMLS_CC ZEND_FILE_LINE_CC)

#define mysqlnd_close(conn,is_forced)					(conn)->m->close((conn), (is_forced) TSRMLS_CC)
#define mysqlnd_query(conn, query_str, query_len)		((conn)->data)->m->query((conn)->data, (query_str), (query_len) TSRMLS_CC)
#define mysqlnd_async_query(conn, query_str, query_len)	((conn)->data)->m->send_query((conn)->data, (query_str), (query_len) TSRMLS_CC)
#define mysqlnd_poll(r, err, d_pull,sec,usec,desc_num)	_mysqlnd_poll((r), (err), (d_pull), (sec), (usec), (desc_num) TSRMLS_CC)
#define mysqlnd_reap_async_query(conn)					((conn)->data)->m->reap_query((conn)->data TSRMLS_CC)
#define mysqlnd_unbuffered_skip_result(result)			(result)->m.skip_result((result) TSRMLS_CC)

PHPAPI enum_func_status _mysqlnd_poll(MYSQLND **r_array, MYSQLND **e_array, MYSQLND ***dont_poll, long sec, long usec, int * desc_num TSRMLS_DC);

#define mysqlnd_use_result(conn)		((conn)->data)->m->use_result((conn)->data TSRMLS_CC)
#define mysqlnd_store_result(conn)		((conn)->data)->m->store_result((conn)->data TSRMLS_CC)
#define mysqlnd_next_result(conn)		((conn)->data)->m->next_result((conn)->data TSRMLS_CC)
#define mysqlnd_more_results(conn)		((conn)->data)->m->more_results((conn)->data TSRMLS_CC)
#define mysqlnd_free_result(r,e_or_i)	((MYSQLND_RES*)r)->m.free_result(((MYSQLND_RES*)(r)), (e_or_i) TSRMLS_CC)
#define mysqlnd_data_seek(result, row)	(result)->m.seek_data((result), (row) TSRMLS_CC)

/* Errors */
#define mysqlnd_errno(conn)				((conn)->data)->m->get_error_no((conn)->data TSRMLS_CC)
#define mysqlnd_error(conn)				((conn)->data)->m->get_error_str((conn)->data TSRMLS_CC)
#define mysqlnd_sqlstate(conn)			((conn)->data)->m->get_sqlstate((conn)->data TSRMLS_CC)

/* Charset */
#define mysqlnd_character_set_name(conn) ((conn)->data)->m->charset_name((conn)->data TSRMLS_CC)

/* Simple metadata */
#define mysqlnd_field_count(conn)		((conn)->data)->m->get_field_count((conn)->data TSRMLS_CC)
#define mysqlnd_insert_id(conn)			((conn)->data)->m->get_last_insert_id((conn)->data TSRMLS_CC)
#define mysqlnd_affected_rows(conn)		((conn)->data)->m->get_affected_rows((conn)->data TSRMLS_CC)
#define mysqlnd_warning_count(conn)		((conn)->data)->m->get_warning_count((conn)->data TSRMLS_CC)
#define mysqlnd_info(conn)				((conn)->data)->m->get_last_message((conn)->data TSRMLS_CC)
#define mysqlnd_get_server_info(conn)	((conn)->data)->m->get_server_information((conn)->data TSRMLS_CC)
#define mysqlnd_get_server_version(conn) ((conn)->data)->m->get_server_version((conn)->data TSRMLS_CC)
#define mysqlnd_get_host_info(conn)		((conn)->data)->m->get_host_information((conn)->data TSRMLS_CC)
#define mysqlnd_get_proto_info(conn)	((conn)->data)->m->get_protocol_information((conn)->data TSRMLS_CC)
#define mysqlnd_thread_id(conn)			((conn)->data)->m->get_thread_id((conn)->data TSRMLS_CC)
#define mysqlnd_get_server_status(conn)	((conn)->data)->m->get_server_status((conn)->data TSRMLS_CC)

#define mysqlnd_num_rows(result)		(result)->m.num_rows((result) TSRMLS_CC)
#define mysqlnd_num_fields(result)		(result)->m.num_fields((result) TSRMLS_CC)

#define mysqlnd_fetch_lengths(result)	_mysqlnd_fetch_lengths((result) TSRMLS_CC)
PHPAPI unsigned long * _mysqlnd_fetch_lengths(MYSQLND_RES * const result  TSRMLS_DC);

#define mysqlnd_field_seek(result, ofs)			(result)->m.seek_field((result), (ofs) TSRMLS_CC)
#define mysqlnd_field_tell(result)				(result)->m.field_tell((result) TSRMLS_CC)
#define mysqlnd_fetch_field(result)				(result)->m.fetch_field((result) TSRMLS_CC)
#define mysqlnd_fetch_field_direct(result,fnr)	(result)->m.fetch_field_direct((result), (fnr) TSRMLS_CC)
#define mysqlnd_fetch_fields(result)			(result)->m.fetch_fields((result) TSRMLS_CC)

/* mysqlnd metadata */
PHPAPI const char *	mysqlnd_get_client_info();
PHPAPI unsigned int	mysqlnd_get_client_version();

#define mysqlnd_ssl_set(conn, key, cert, ca, capath, cipher) ((conn)->data)->m->ssl_set((conn)->data, (key), (cert), (ca), (capath), (cipher) TSRMLS_CC)

/* PS */
#define mysqlnd_stmt_insert_id(stmt)		(stmt)->m->get_last_insert_id((stmt) TSRMLS_CC)
#define mysqlnd_stmt_affected_rows(stmt)	(stmt)->m->get_affected_rows((stmt) TSRMLS_CC)
#define mysqlnd_stmt_num_rows(stmt)			(stmt)->m->get_num_rows((stmt) TSRMLS_CC)
#define mysqlnd_stmt_param_count(stmt)		(stmt)->m->get_param_count((stmt) TSRMLS_CC)
#define mysqlnd_stmt_field_count(stmt)		(stmt)->m->get_field_count((stmt) TSRMLS_CC)
#define mysqlnd_stmt_warning_count(stmt)	(stmt)->m->get_warning_count((stmt) TSRMLS_CC)
#define mysqlnd_stmt_server_status(stmt)	(stmt)->m->get_server_status((stmt) TSRMLS_CC)
#define mysqlnd_stmt_errno(stmt)			(stmt)->m->get_error_no((stmt) TSRMLS_CC)
#define mysqlnd_stmt_error(stmt)			(stmt)->m->get_error_str((stmt) TSRMLS_CC)
#define mysqlnd_stmt_sqlstate(stmt)			(stmt)->m->get_sqlstate((stmt) TSRMLS_CC)


PHPAPI void mysqlnd_efree_param_bind_dtor(MYSQLND_PARAM_BIND * param_bind TSRMLS_DC);
PHPAPI void mysqlnd_efree_result_bind_dtor(MYSQLND_RESULT_BIND * result_bind TSRMLS_DC);
PHPAPI void mysqlnd_free_param_bind_dtor(MYSQLND_PARAM_BIND * param_bind TSRMLS_DC);
PHPAPI void mysqlnd_free_result_bind_dtor(MYSQLND_RESULT_BIND * result_bind TSRMLS_DC);


PHPAPI const char * mysqlnd_field_type_name(enum mysqlnd_field_types field_type);

/* LOAD DATA LOCAL */
PHPAPI void mysqlnd_local_infile_default(MYSQLND_CONN_DATA * conn);
PHPAPI void mysqlnd_set_local_infile_handler(MYSQLND_CONN_DATA * const conn, const char * const funcname);

/* Simple commands */
#define mysqlnd_autocommit(conn, mode)		((conn)->data)->m->set_autocommit((conn)->data, (mode) TSRMLS_CC)
#define mysqlnd_begin_transaction(conn,flags,name) ((conn)->data)->m->tx_begin((conn)->data, (flags), (name) TSRMLS_CC)
#define mysqlnd_commit(conn, flags, name)	((conn)->data)->m->tx_commit_or_rollback((conn)->data, TRUE, (flags), (name) TSRMLS_CC)
#define mysqlnd_rollback(conn, flags, name)	((conn)->data)->m->tx_commit_or_rollback((conn)->data, FALSE, (flags), (name) TSRMLS_CC)
#define mysqlnd_savepoint(conn, name)		((conn)->data)->m->tx_savepoint((conn)->data, (name) TSRMLS_CC)
#define mysqlnd_release_savepoint(conn, name) ((conn)->data)->m->tx_savepoint_release((conn)->data, (name) TSRMLS_CC)
#define mysqlnd_list_dbs(conn, wild)		((conn)->data)->m->list_method((conn)->data, wild? "SHOW DATABASES LIKE %s":"SHOW DATABASES", (wild), NULL TSRMLS_CC)
#define mysqlnd_list_fields(conn, tab,wild)	((conn)->data)->m->list_fields((conn)->data, (tab), (wild) TSRMLS_CC)
#define mysqlnd_list_processes(conn)		((conn)->data)->m->list_method((conn)->data, "SHOW PROCESSLIST", NULL, NULL TSRMLS_CC)
#define mysqlnd_list_tables(conn, wild)		((conn)->data)->m->list_method((conn)->data, wild? "SHOW TABLES LIKE %s":"SHOW TABLES", (wild), NULL TSRMLS_CC)
#define mysqlnd_dump_debug_info(conn)		((conn)->data)->m->server_dump_debug_information((conn)->data TSRMLS_CC)
#define mysqlnd_select_db(conn, db, db_len)	((conn)->data)->m->select_db((conn)->data, (db), (db_len) TSRMLS_CC)
#define mysqlnd_ping(conn)					((conn)->data)->m->ping((conn)->data TSRMLS_CC)
#define mysqlnd_kill(conn, pid)				((conn)->data)->m->kill_connection((conn)->data, (pid) TSRMLS_CC)
#define mysqlnd_refresh(conn, options)		((conn)->data)->m->refresh_server((conn)->data, (options) TSRMLS_CC)
#define mysqlnd_shutdown(conn, level)		((conn)->data)->m->shutdown_server((conn)->data, (level) TSRMLS_CC)
#define mysqlnd_set_character_set(conn, cs)	((conn)->data)->m->set_charset((conn)->data, (cs) TSRMLS_CC)
#define mysqlnd_stat(conn, msg, msg_len)	((conn)->data)->m->get_server_statistics(((conn)->data), (msg), (msg_len) TSRMLS_CC)
#define mysqlnd_options(conn, opt, value)	((conn)->data)->m->set_client_option((conn)->data, (opt), (value) TSRMLS_CC)
#define mysqlnd_options4(conn, opt, k, v)	((conn)->data)->m->set_client_option_2d((conn)->data, (opt), (k), (v) TSRMLS_CC)
#define mysqlnd_set_server_option(conn, op)	((conn)->data)->m->set_server_option((conn)->data, (op) TSRMLS_CC)

/* Escaping */
#define mysqlnd_real_escape_string(conn, newstr, escapestr, escapestr_len) \
		((conn)->data)->m->escape_string((conn)->data, (newstr), (escapestr), (escapestr_len) TSRMLS_CC)
#define mysqlnd_escape_string(newstr, escapestr, escapestr_len) \
		mysqlnd_old_escape_string((newstr), (escapestr), (escapestr_len) TSRMLS_CC)

PHPAPI ulong mysqlnd_old_escape_string(char * newstr, const char * escapestr, size_t escapestr_len TSRMLS_DC);


/* PS */
#define mysqlnd_stmt_init(conn)				((conn)->data)->m->stmt_init(((conn)->data) TSRMLS_CC)
#define mysqlnd_stmt_store_result(stmt)		(!mysqlnd_stmt_field_count((stmt)) ? PASS:((stmt)->m->store_result((stmt) TSRMLS_CC)? PASS:FAIL))
#define mysqlnd_stmt_get_result(stmt)		(stmt)->m->get_result((stmt) TSRMLS_CC)
#define mysqlnd_stmt_more_results(stmt)		(stmt)->m->more_results((stmt) TSRMLS_CC)
#define mysqlnd_stmt_next_result(stmt)		(stmt)->m->next_result((stmt) TSRMLS_CC)
#define mysqlnd_stmt_data_seek(stmt, row)	(stmt)->m->seek_data((stmt), (row) TSRMLS_CC)
#define mysqlnd_stmt_prepare(stmt, q, qlen)	(stmt)->m->prepare((stmt), (q), (qlen) TSRMLS_CC)
#define mysqlnd_stmt_execute(stmt) 			(stmt)->m->execute((stmt) TSRMLS_CC)
#define mysqlnd_stmt_send_long_data(stmt,p,d,l) 	(stmt)->m->send_long_data((stmt), (p), (d), (l) TSRMLS_CC)
#define mysqlnd_stmt_alloc_param_bind(stmt)			(stmt)->m->alloc_parameter_bind((stmt) TSRMLS_CC)
#define mysqlnd_stmt_free_param_bind(stmt,bind)		(stmt)->m->free_parameter_bind((stmt), (bind) TSRMLS_CC)
#define mysqlnd_stmt_bind_param(stmt,bind)			(stmt)->m->bind_parameters((stmt), (bind) TSRMLS_CC)
#define mysqlnd_stmt_bind_one_param(stmt,n,z,t)		(stmt)->m->bind_one_parameter((stmt), (n), (z), (t) TSRMLS_CC)
#define mysqlnd_stmt_refresh_bind_param(s)			(s)->m->refresh_bind_param((s) TSRMLS_CC)
#define mysqlnd_stmt_alloc_result_bind(stmt)		(stmt)->m->alloc_result_bind((stmt) TSRMLS_CC)
#define mysqlnd_stmt_free_result_bind(stmt,bind)	(stmt)->m->free_result_bind((stmt), (bind) TSRMLS_CC)
#define mysqlnd_stmt_bind_result(stmt,bind)			(stmt)->m->bind_result((stmt), (bind) TSRMLS_CC)
#define mysqlnd_stmt_bind_one_result(s,no)			(s)->m->bind_one_result((s), (no) TSRMLS_CC)
#define mysqlnd_stmt_param_metadata(stmt)			(stmt)->m->get_parameter_metadata((stmt))
#define mysqlnd_stmt_result_metadata(stmt)			(stmt)->m->get_result_metadata((stmt) TSRMLS_CC)

#define	mysqlnd_stmt_free_result(stmt)		(stmt)->m->free_result((stmt) TSRMLS_CC)
#define	mysqlnd_stmt_close(stmt, implicit)	(stmt)->m->dtor((stmt), (implicit) TSRMLS_CC)
#define	mysqlnd_stmt_reset(stmt)			(stmt)->m->reset((stmt) TSRMLS_CC)
#define	mysqlnd_stmt_flush(stmt)			(stmt)->m->flush((stmt) TSRMLS_CC)


#define mysqlnd_stmt_attr_get(stmt, attr, value)	(stmt)->m->get_attribute((stmt), (attr), (value) TSRMLS_CC)
#define mysqlnd_stmt_attr_set(stmt, attr, value)	(stmt)->m->set_attribute((stmt), (attr), (value) TSRMLS_CC)

#define mysqlnd_stmt_fetch(stmt, fetched)	(stmt)->m->fetch((stmt), (fetched) TSRMLS_CC)


/* Performance statistics */
PHPAPI void			_mysqlnd_get_client_stats(zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);

/* double check the class name to avoid naming conflicts when using these: */
#define MYSQLND_METHOD(class, method) php_##class##_##method##_pub
#define MYSQLND_METHOD_PRIVATE(class, method) php_##class##_##method##_priv

ZEND_BEGIN_MODULE_GLOBALS(mysqlnd)
	zend_bool		collect_statistics;
	zend_bool		collect_memory_statistics;
	char *			debug;	/* The actual string */
	char *			trace_alloc_settings;	/* The actual string */
	MYSQLND_DEBUG *	dbg;	/* The DBG object for standard tracing */
	MYSQLND_DEBUG *	trace_alloc;	/* The DBG object for allocation tracing */
	long			net_cmd_buffer_size;
	long			net_read_buffer_size;
	long			log_mask;
	long			net_read_timeout;
	long			mempool_default_size;
	long			debug_emalloc_fail_threshold;
	long			debug_ecalloc_fail_threshold;
	long			debug_erealloc_fail_threshold;
	long			debug_malloc_fail_threshold;
	long			debug_calloc_fail_threshold;
	long			debug_realloc_fail_threshold;
	char *			sha256_server_public_key;
ZEND_END_MODULE_GLOBALS(mysqlnd)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(mysqlnd)

#ifdef ZTS
#define MYSQLND_G(v) TSRMG(mysqlnd_globals_id, zend_mysqlnd_globals *, v)
#else
#define MYSQLND_G(v) (mysqlnd_globals.v)
#endif


PHPAPI void mysqlnd_minfo_print_hash(zval *values);

#endif	/* MYSQLND_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
