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

#ifndef MYSQLND_H
#define MYSQLND_H

#define MYSQLND_VERSION "mysqlnd 5.0.3-dev - 080129 - $Revision$"
#define MYSQLND_VERSION_ID 50002

/* This forces inlining of some accessor functions */
#define MYSQLND_USE_OPTIMISATIONS 0


/* #define MYSQLND_STRING_TO_INT_CONVERSION */
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
#if PHP_DEBUG
#define MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND 1
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "mysqlnd_portability.h"
#include "mysqlnd_enum_n_def.h"
#include "mysqlnd_structs.h"




/* Library related */
void mysqlnd_library_init(TSRMLS_D);
void mysqlnd_library_end(TSRMLS_D);

#define mysqlnd_restart_psession(conn) _mysqlnd_restart_psession((conn) TSRMLS_CC)
PHPAPI void _mysqlnd_restart_psession(MYSQLND *conn TSRMLS_DC);
PHPAPI void mysqlnd_end_psession(MYSQLND *conn);
PHPAPI void mysqlnd_minfo_print_hash(zval *values);
#define mysqlnd_thread_safe()	TRUE

PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_nr(uint charsetno);
PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_name(const char * const charsetname);


/* Connect */
#define mysqlnd_init(persistent) _mysqlnd_init((persistent) TSRMLS_CC)
PHPAPI MYSQLND * _mysqlnd_init(zend_bool persistent TSRMLS_DC);
PHPAPI MYSQLND * mysqlnd_connect(MYSQLND *conn,
						  const char *host, const char *user,
						  const char *passwd, unsigned int passwd_len,
						  const char *db, unsigned int db_len,
						  unsigned int port,
						  const char *socket,
						  unsigned int mysql_flags,
						  MYSQLND_THD_ZVAL_PCACHE *zval_cache
						  TSRMLS_DC);

#define mysqlnd_change_user(conn, user, passwd, db)		(conn)->m->change_user((conn), (user), (passwd), (db) TSRMLS_CC)

#define mysqlnd_debug(x)								_mysqlnd_debug((x) TSRMLS_CC)
void _mysqlnd_debug(const char *mode TSRMLS_DC);

/* Query */
#define mysqlnd_fetch_into(result, flags, ret_val, ext)	(result)->m.fetch_into((result), (flags), (ret_val), (ext) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_fetch_row_c(result)						(result)->m.fetch_row_c((result) TSRMLS_CC)
#define mysqlnd_fetch_all(result, flags, return_value)	(result)->m.fetch_all((result), (flags), (return_value) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_result_fetch_field_data(res,offset,ret)	(res)->m.fetch_field_data((res), (offset), (ret) TSRMLS_CC)
#define mysqlnd_get_connection_stats(conn, values)		(conn)->m->get_statistics((conn),  (values) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_get_client_stats(values)				_mysqlnd_get_client_stats((values) TSRMLS_CC ZEND_FILE_LINE_CC)

#define mysqlnd_close(conn,is_forced)					(conn)->m->close((conn), (is_forced) TSRMLS_CC)
#define mysqlnd_query(conn, query_str, query_len)		(conn)->m->query((conn), (query_str), (query_len) TSRMLS_CC)
#define mysqlnd_unbuffered_skip_result(result)			(result)->m.skip_result((result) TSRMLS_CC)


#define mysqlnd_use_result(conn)		(conn)->m->use_result((conn) TSRMLS_CC)
#define mysqlnd_store_result(conn)		(conn)->m->store_result((conn) TSRMLS_CC)
#define mysqlnd_bg_store_result(conn) 	(conn)->m->background_store_result((conn) TSRMLS_CC)
#define mysqlnd_next_result(conn)		(conn)->m->next_result((conn) TSRMLS_CC)
#define mysqlnd_more_results(conn)		(conn)->m->more_results((conn))
#define mysqlnd_free_result(r,e_or_i)	((MYSQLND_RES*)r)->m.free_result(((MYSQLND_RES*)(r)), (e_or_i) TSRMLS_CC)
#define mysqlnd_data_seek(result, row)	(result)->m.seek_data((result), (row) TSRMLS_CC)

/*****************************************************************************************************/
#if defined(MYSQLND_USE_OPTIMISATIONS) && MYSQLND_USE_OPTIMISATIONS == 1

/* Errors */
#define mysqlnd_errno(conn)				(conn)->error_info.error_no
#define mysqlnd_error(conn)				(conn)->error_info.error
#define mysqlnd_sqlstate(conn)  		((conn)->error_info.sqlstate[0] ? conn->error_info.sqlstate:MYSQLND_SQLSTATE_NULL)

/* Charset */
#define mysqlnd_character_set_name(conn) (conn)->charset->name

/* Simple metadata */
#define mysqlnd_field_count(conn)		(conn)->field_count
#define mysqlnd_insert_id(conn)			(conn)->upsert_status.last_insert_id
#define mysqlnd_affected_rows(conn)		(conn)->upsert_status.affected_rows
#define mysqlnd_warning_count(conn)		(conn)->upsert_status.warning_count
#define mysqlnd_info(conn)  			(conn)->last_message
#define mysqlnd_get_server_info(conn)	(conn)->server_version
#define mysqlnd_get_host_info(conn)		(conn)->host_info
#define mysqlnd_get_proto_info(conn)	(conn)->protocol_version
#define mysqlnd_thread_id(conn)			(conn)->thread_id

#define mysqlnd_num_rows(result)		((result)->data? (result)->data->row_count:0)
#define mysqlnd_num_fields(result)		(result)->field_count

#define mysqlnd_fetch_lengths(result)	((result)->m.fetch_lengths? (result)->m.fetch_lengths((result)):NULL)

#define mysqlnd_field_seek(result, ofs)			(result)->m.seek_field((result), (ofs))
#define mysqlnd_field_tell(result)				(result)->meta? (result)->meta->current_field:0)
#define mysqlnd_fetch_field(result)				(result)->m.fetch_field((result) TSRMLS_CC)
#define mysqlnd_fetch_field_direct(result,fnr)	((result)->meta? &((result)->meta->fields[(fnr)]):NULL)

/* mysqlnd metadata */
#define mysqlnd_get_client_info()		MYSQLND_VERSION
#define mysqlnd_get_client_version()	MYSQLND_VERSION_ID

/* PS */
#define mysqlnd_stmt_insert_id(stmt)		(stmt)->upsert_status.last_insert_id
#define mysqlnd_stmt_affected_rows(stmt)	(stmt)->upsert_status.affected_rows
#define mysqlnd_stmt_num_rows(stmt)			(stmt)->result? mysqlnd_num_rows((stmt)->result):0
#define mysqlnd_stmt_param_count(stmt)		(stmt)->param_count
#define mysqlnd_stmt_field_count(stmt)		(stmt)->field_count
#define mysqlnd_stmt_warning_count(stmt)	(stmt)->upsert_status.warning_count
#define mysqlnd_stmt_errno(stmt)			(stmt)->error_info.error_no
#define mysqlnd_stmt_error(stmt)			(stmt)->error_info.error
#define mysqlnd_stmt_sqlstate(stmt)			((stmt)->error_info.sqlstate[0] ? (stmt)->error_info.sqlstate:MYSQLND_SQLSTATE_NULL)



/*****************************************************************************************************/
#else /* Using plain functions */
/*****************************************************************************************************/

/* Errors */
#define mysqlnd_errno(conn)				(conn)->m->get_error_no((conn))
#define mysqlnd_error(conn)				(conn)->m->get_error_str((conn))
#define mysqlnd_sqlstate(conn)  		(conn)->m->get_sqlstate((conn))

/* Charset */
#define mysqlnd_character_set_name(conn) (conn)->m->charset_name((conn))

/* Simple metadata */
#define mysqlnd_field_count(conn)		(conn)->m->get_field_count((conn))
#define mysqlnd_insert_id(conn)			(conn)->m->get_last_insert_id((conn))
#define mysqlnd_affected_rows(conn)		(conn)->m->get_affected_rows((conn))
#define mysqlnd_warning_count(conn)		(conn)->m->get_warning_count((conn))
#define mysqlnd_info(conn)				(conn)->m->get_last_message((conn))
#define mysqlnd_get_server_info(conn)	(conn)->m->get_server_information((conn))
#define mysqlnd_get_host_info(conn)		(conn)->m->get_host_information((conn))
#define mysqlnd_get_proto_info(conn)	(conn)->m->get_protocol_information((conn))
#define mysqlnd_thread_id(conn)			(conn)->m->get_thread_id((conn))

#define mysqlnd_num_rows(result)		(result)->m.num_rows((result))
#define mysqlnd_num_fields(result)		(result)->m.num_fields((result))

PHPAPI unsigned long * mysqlnd_fetch_lengths(MYSQLND_RES * const result);

#define mysqlnd_field_seek(result, ofs)			(result)->m.seek_field((result), (ofs))
#define mysqlnd_field_tell(result)				(result)->m.field_tell((result))
#define mysqlnd_fetch_field(result)				(result)->m.fetch_field((result) TSRMLS_CC)
#define mysqlnd_fetch_field_direct(result,fnr)	(result)->m.fetch_field_direct((result), (fnr) TSRMLS_CC)

/* mysqlnd metadata */
PHPAPI const char *	mysqlnd_get_client_info();
PHPAPI unsigned int	mysqlnd_get_client_version();

/* PS */
#define mysqlnd_stmt_insert_id(stmt)		(stmt)->m->get_last_insert_id((stmt))
#define mysqlnd_stmt_affected_rows(stmt)	(stmt)->m->get_affected_rows((stmt))
#define mysqlnd_stmt_num_rows(stmt)			(stmt)->m->get_num_rows((stmt))
#define mysqlnd_stmt_param_count(stmt)		(stmt)->m->get_param_count((stmt))
#define mysqlnd_stmt_field_count(stmt)		(stmt)->m->get_field_count((stmt))
#define mysqlnd_stmt_warning_count(stmt)	(stmt)->m->get_warning_count((stmt))
#define mysqlnd_stmt_errno(stmt)			(stmt)->m->get_error_no((stmt))
#define mysqlnd_stmt_error(stmt)			(stmt)->m->get_error_str((stmt))
#define mysqlnd_stmt_sqlstate(stmt)			(stmt)->m->get_sqlstate((stmt))
#endif /* MYSQLND_USE_OPTIMISATIONS */
/*****************************************************************************************************/



PHPAPI const char * mysqlnd_field_type_name(enum mysqlnd_field_types field_type);

/* LOAD DATA LOCAL */
PHPAPI void mysqlnd_local_infile_default(MYSQLND *conn);
PHPAPI void mysqlnd_set_local_infile_handler(MYSQLND * const conn, const char * const funcname);

/* Simple commands */
#define mysqlnd_autocommit(conn, mode)		(conn)->m->query((conn),(mode) ? "SET AUTOCOMMIT=1":"SET AUTOCOMMIT=0", 16 TSRMLS_CC)
#define mysqlnd_commit(conn)				(conn)->m->query((conn), "COMMIT", sizeof("COMMIT")-1 TSRMLS_CC)
#define mysqlnd_rollback(conn)				(conn)->m->query((conn), "ROLLBACK", sizeof("ROLLBACK")-1 TSRMLS_CC)
#define mysqlnd_list_dbs(conn, wild)		(conn)->m->list_method((conn), wild? "SHOW DATABASES LIKE %s":"SHOW DATABASES", (wild), NULL TSRMLS_CC)
#define mysqlnd_list_fields(conn, tab,wild)	(conn)->m->list_fields((conn), (tab), (wild) TSRMLS_CC)
#define mysqlnd_list_processes(conn)		(conn)->m->list_method((conn), "SHOW PROCESSLIST", NULL, NULL TSRMLS_CC)
#define mysqlnd_list_tables(conn, wild)		(conn)->m->list_method((conn), wild? "SHOW TABLES LIKE %s":"SHOW TABLES", (wild), NULL TSRMLS_CC)
#define mysqlnd_dump_debug_info(conn)		(conn)->m->server_dump_debug_information((conn) TSRMLS_CC)
#define mysqlnd_select_db(conn, db, db_len)	(conn)->m->select_db((conn), (db), (db_len) TSRMLS_CC)
#define mysqlnd_ping(conn)					(conn)->m->ping((conn) TSRMLS_CC)
#define mysqlnd_kill(conn, pid)				(conn)->m->kill_connection((conn), (pid) TSRMLS_CC)
#define mysqlnd_refresh(conn, options)		(conn)->m->refresh_server((conn), (options) TSRMLS_CC) 
#define mysqlnd_shutdown(conn, level)		(conn)->m->shutdown_server((conn), (level) TSRMLS_CC)
#define mysqlnd_get_server_version(conn)	(conn)->m->get_server_version((conn))
#define mysqlnd_set_character_set(conn, cs)	(conn)->m->set_charset((conn), (cs) TSRMLS_CC)
#define mysqlnd_stat(conn, msg, msg_len)	(conn)->m->get_server_statistics((conn), (msg), (msg_len) TSRMLS_CC)
#define mysqlnd_options(conn, opt, value)	(conn)->m->set_client_option((conn), (opt), (value) TSRMLS_CC)
#define mysqlnd_set_server_option(conn, op)	(conn)->m->set_server_option((conn), (op) TSRMLS_CC)

/* Escaping */
#define mysqlnd_real_escape_string(conn, newstr, escapestr, escapestr_len) \
		(conn)->m->escape_string((conn), (newstr), (escapestr), (escapestr_len) TSRMLS_CC)
#define mysqlnd_escape_string(newstr, escapestr, escapestr_len) \
		mysqlnd_old_escape_string((newstr), (escapestr), (escapestr_len) TSRMLS_CC)

PHPAPI ulong mysqlnd_old_escape_string(char *newstr, const char *escapestr, size_t escapestr_len TSRMLS_DC);


/* PS */
#define mysqlnd_stmt_init(conn)				(conn)->m->stmt_init((conn) TSRMLS_CC)
#define mysqlnd_stmt_store_result(stmt)		(!mysqlnd_stmt_field_count((stmt)) ? PASS:((stmt)->m->store_result((stmt) TSRMLS_CC)? PASS:FAIL))
#define mysqlnd_stmt_bg_store_result(stmt)	(!mysqlnd_stmt_field_count((stmt)) ? PASS:((stmt)->m->background_store_result((stmt) TSRMLS_CC)? PASS:FAIL))
#define mysqlnd_stmt_get_result(stmt)		(stmt)->m->get_result((stmt) TSRMLS_CC)
#define mysqlnd_stmt_data_seek(stmt, row)	(stmt)->m->seek_data((stmt), (row) TSRMLS_CC)
#define mysqlnd_stmt_prepare(stmt, q, qlen)	(stmt)->m->prepare((stmt), (q), (qlen) TSRMLS_CC)
#define mysqlnd_stmt_execute(stmt) 			(stmt)->m->execute((stmt) TSRMLS_CC)
#define mysqlnd_stmt_send_long_data(s,p,d,l) (s)->m->send_long_data((s), (p), (d), (l) TSRMLS_CC)
#define mysqlnd_stmt_bind_param(stmt,bind)	(stmt)->m->bind_param((stmt), (bind) TSRMLS_CC)
#define mysqlnd_stmt_refresh_bind_param(s)	(s)->m->refresh_bind_param((s) TSRMLS_CC)
#define mysqlnd_stmt_bind_result(stmt,bind)	(stmt)->m->bind_result((stmt), (bind) TSRMLS_CC)
#define mysqlnd_stmt_param_metadata(stmt)	(stmt)->m->get_parameter_metadata((stmt))
#define mysqlnd_stmt_result_metadata(stmt)	(stmt)->m->get_result_metadata((stmt) TSRMLS_CC)

#define	mysqlnd_stmt_free_result(stmt)		(stmt)->m->free_result((stmt) TSRMLS_CC)
#define	mysqlnd_stmt_close(stmt, implicit)	(stmt)->m->dtor((stmt), (implicit) TSRMLS_CC)
#define	mysqlnd_stmt_reset(stmt)			(stmt)->m->reset((stmt) TSRMLS_CC)


#define mysqlnd_stmt_attr_get(stmt, attr, value)	(stmt)->m->get_attribute((stmt), (attr), (value) TSRMLS_CC)
#define mysqlnd_stmt_attr_set(stmt, attr, value)	(stmt)->m->set_attribute((stmt), (attr), (value) TSRMLS_CC)

#define mysqlnd_stmt_fetch(stmt, fetched)	(stmt)->m->fetch((stmt), (fetched) TSRMLS_CC)


/* Performance statistics */
PHPAPI void			_mysqlnd_get_client_stats(zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);

/* Persistent caching zval allocator */
#define mysqlnd_palloc_init_cache(size)		_mysqlnd_palloc_init_cache((size) TSRMLS_CC)
#define mysqlnd_palloc_free_cache(cache)	_mysqlnd_palloc_free_cache((cache) TSRMLS_CC)
PHPAPI MYSQLND_ZVAL_PCACHE*		_mysqlnd_palloc_init_cache(unsigned int cache_size TSRMLS_DC);
PHPAPI void 					_mysqlnd_palloc_free_cache(MYSQLND_ZVAL_PCACHE *cache TSRMLS_DC);
PHPAPI void						mysqlnd_palloc_stats(const MYSQLND_ZVAL_PCACHE * const cache,
													 zval *return_value);

#define mysqlnd_palloc_rinit(cache)		_mysqlnd_palloc_rinit((cache) TSRMLS_CC)
#define mysqlnd_palloc_rshutdown(cache)	_mysqlnd_palloc_rshutdown((cache) TSRMLS_CC)
PHPAPI MYSQLND_THD_ZVAL_PCACHE * _mysqlnd_palloc_rinit(MYSQLND_ZVAL_PCACHE * cache TSRMLS_DC);
PHPAPI void						 _mysqlnd_palloc_rshutdown(MYSQLND_THD_ZVAL_PCACHE * cache TSRMLS_DC);


#define mysqlnd_palloc_init_thd_cache(cache) 			_mysqlnd_palloc_init_thd_cache((cache) TSRMLS_CC)
#define mysqlnd_palloc_free_thd_cache_reference(cache)	_mysqlnd_palloc_free_thd_cache_reference((cache) TSRMLS_CC)

PHPAPI MYSQLND_THD_ZVAL_PCACHE*	_mysqlnd_palloc_init_thd_cache(MYSQLND_ZVAL_PCACHE * const cache TSRMLS_DC);
MYSQLND_THD_ZVAL_PCACHE*		mysqlnd_palloc_get_thd_cache_reference(MYSQLND_THD_ZVAL_PCACHE * const cache);
PHPAPI void						_mysqlnd_palloc_free_thd_cache_reference(MYSQLND_THD_ZVAL_PCACHE **cache TSRMLS_DC);


/* There two should not be used from outside */
void *	mysqlnd_palloc_get_zval(MYSQLND_THD_ZVAL_PCACHE * const cache, zend_bool *allocated TSRMLS_DC);
void	mysqlnd_palloc_zval_ptr_dtor(zval **zv, MYSQLND_THD_ZVAL_PCACHE * const cache,
									 enum_mysqlnd_res_type type, zend_bool *copy_ctor_called TSRMLS_DC);



/* ---------------------- QUERY CACHE ---------------*/
struct st_mysqlnd_qcache {
	HashTable		*ht;
	unsigned int	references;
#ifdef ZTS
	MUTEX_T		LOCK_access;
#endif
};


typedef struct st_mysqlnd_qcache_element {
	MYSQLND_RES_BUFFERED *data;
	MYSQLND_RES_METADATA *meta;
	const char * query;
	size_t	query_len;
} MYSQLND_QCACHE_ELEMENT;


PHPAPI MYSQLND_QCACHE *	mysqlnd_qcache_init_cache();
PHPAPI MYSQLND_QCACHE *	mysqlnd_qcache_get_cache_reference(MYSQLND_QCACHE * const cache);
PHPAPI void				mysqlnd_qcache_free_cache_reference(MYSQLND_QCACHE **cache);
PHPAPI void				mysqlnd_qcache_stats(const MYSQLND_QCACHE * const cache, zval *return_value);
MYSQLND_RES * 		mysqlnd_qcache_get(MYSQLND_QCACHE * const cache, const char * query,
									   size_t query_len);
void				mysqlnd_qcache_put(MYSQLND_QCACHE * const cache, char * query, size_t query_len,
									   MYSQLND_RES_BUFFERED * const result, MYSQLND_RES_METADATA * const meta);



ZEND_BEGIN_MODULE_GLOBALS(mysqlnd)
	zend_bool		collect_statistics;
	zend_bool		collect_memory_statistics;
	char*			debug;	/* The actual string */
	MYSQLND_DEBUG	*dbg;	/* The DBG object */
	long			net_cmd_buffer_size;
	long			net_read_buffer_size;
ZEND_END_MODULE_GLOBALS(mysqlnd)

ZEND_EXTERN_MODULE_GLOBALS(mysqlnd);

#ifdef ZTS
#define MYSQLND_G(v) TSRMG(mysqlnd_globals_id, zend_mysqlnd_globals *, v)
#else
#define MYSQLND_G(v) (mysqlnd_globals.v)
#endif


#endif	/* MYSQLND_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
