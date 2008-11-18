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
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_block_alloc.h"
/* for php_get_current_user() */
#include "ext/standard/basic_functions.h" 

/* the server doesn't support 4byte utf8, but let's make it forward compatible */
#define MYSQLND_MAX_ALLOWED_USER_LEN	256  /* 64 char * 4byte */
#define MYSQLND_MAX_ALLOWED_DB_LEN		256  /* 64 char * 4byte */
/*
  TODO :
  - Don't bind so tightly the metadata with the result set. This means
	that the metadata reading should not expect a MYSQLND_RES pointer, it
	does not need it, but return a pointer to the metadata (MYSQLND_FIELD *).
	For normal statements we will then just assign it to a member of
	MYSQLND_RES. For PS statements, it will stay as part of the statement
	(MYSQLND_STMT) between prepare and execute. At execute the new metadata
	will be sent by the server, so we will discard the old one and then
	finally attach it to the result set. This will make the code more clean,
	as a prepared statement won't have anymore stmt->result != NULL, as it
	is now, just to have where to store the metadata.

  - Change mysqlnd_simple_command to accept a heap dynamic array of MYSQLND_STRING
	terminated by a string with ptr being NULL. Thus, multi-part messages can be
	sent to the network like writev() and this can save at least for
	mysqlnd_stmt_send_long_data() new malloc. This change will probably make the
	code in few other places cleaner.
*/

extern MYSQLND_CHARSET *mysqlnd_charsets;



const char * const mysqlnd_old_passwd  = "mysqlnd cannot connect to MySQL 4.1+ using old authentication";
const char * const mysqlnd_server_gone = "MySQL server has gone away";
const char * const mysqlnd_out_of_sync = "Commands out of sync; you can't run this command now";

MYSQLND_STATS *mysqlnd_global_stats = NULL;
static zend_bool mysqlnd_library_initted = FALSE;


static enum_func_status mysqlnd_send_close(MYSQLND * conn TSRMLS_DC);


/* {{{ mysqlnd_library_init */
void mysqlnd_library_init(TSRMLS_D)
{
	if (mysqlnd_library_initted == FALSE) {
		mysqlnd_library_initted = TRUE;
		_mysqlnd_init_ps_subsystem();
		/* Should be calloc, as mnd_calloc will reference LOCK_access*/
		mysqlnd_stats_init(&mysqlnd_global_stats);
	}
}
/* }}} */


/* {{{ mysqlnd_library_end */
void mysqlnd_library_end(TSRMLS_D)
{
	if (mysqlnd_library_initted == TRUE) {
		mysqlnd_stats_end(mysqlnd_global_stats);
		mysqlnd_global_stats = NULL;
		mysqlnd_library_initted = FALSE;
	}
}
/* }}} */


/* {{{ mysqlnd_conn::free_contents */
static void
MYSQLND_METHOD(mysqlnd_conn, free_contents)(MYSQLND *conn TSRMLS_DC)
{
	zend_bool pers = conn->persistent;

	DBG_ENTER("mysqlnd_conn::free_contents");

	mysqlnd_local_infile_default(conn);
	if (conn->current_result) {
		conn->current_result->m.free_result_contents(conn->current_result TSRMLS_CC);
		mnd_efree(conn->current_result);
		conn->current_result = NULL;
	}

	if (conn->net.stream) {
		DBG_INF_FMT("Freeing stream. abstract=%p", conn->net.stream->abstract);
		if (pers) {
			php_stream_free(conn->net.stream, PHP_STREAM_FREE_CLOSE_PERSISTENT | PHP_STREAM_FREE_RSRC_DTOR);
		} else {
			php_stream_free(conn->net.stream, PHP_STREAM_FREE_CLOSE);
		
		}
		conn->net.stream = NULL;
	}

	DBG_INF("Freeing memory of members");
	if (conn->host) {
		DBG_INF("Freeing host");
		mnd_pefree(conn->host, pers);
		conn->host = NULL;
	}
	if (conn->user) {
		DBG_INF("Freeing user");
		mnd_pefree(conn->user, pers);
		conn->user = NULL;
	}
	if (conn->passwd) {
		DBG_INF("Freeing passwd");
		mnd_pefree(conn->passwd, pers);
		conn->passwd = NULL;
	}
	if (conn->unix_socket) {
		DBG_INF("Freeing unix_socket");
		mnd_pefree(conn->unix_socket, pers);
		conn->unix_socket = NULL;
	}
	if (conn->scheme) {
		DBG_INF("Freeing scheme");
		mnd_pefree(conn->scheme, pers);
		conn->scheme = NULL;
	}
	if (conn->server_version) {
		DBG_INF("Freeing server_version");
		mnd_pefree(conn->server_version, pers);
		conn->server_version = NULL;
	}
	if (conn->host_info) {
		DBG_INF("Freeing host_info");
		mnd_pefree(conn->host_info, pers);
		conn->host_info = NULL;
	}
	if (conn->scramble) {
		DBG_INF("Freeing scramble");
		mnd_pefree(conn->scramble, pers);
		conn->scramble = NULL;
	}
	if (conn->last_message) {
		mnd_pefree(conn->last_message, pers);
		conn->last_message = NULL;
	}
	if (conn->options.charset_name) {
		mnd_pefree(conn->options.charset_name, pers);
		conn->options.charset_name = NULL;
	}
	if (conn->options.num_commands) {
		unsigned int i;
		for (i = 0; i < conn->options.num_commands; i++) {
			mnd_pefree(conn->options.init_commands[i], pers);
		}
		mnd_pefree(conn->options.init_commands, pers);
		conn->options.init_commands = NULL;
	}
	if (conn->options.cfg_file) {
		mnd_pefree(conn->options.cfg_file, pers);
		conn->options.cfg_file = NULL;
	}
	if (conn->options.cfg_section) {
		mnd_pefree(conn->options.cfg_section, pers);
		conn->options.cfg_section = NULL;
	}
	if (conn->options.ssl_key) {
		mnd_pefree(conn->options.ssl_key, pers);
		conn->options.ssl_key = NULL;
	}
	if (conn->options.ssl_cert) {
		mnd_pefree(conn->options.ssl_cert, pers);
		conn->options.ssl_cert = NULL;
	}
	if (conn->options.ssl_ca) {
		mnd_pefree(conn->options.ssl_ca, pers);
		conn->options.ssl_ca = NULL;
	}
	if (conn->options.ssl_capath) {
		mnd_pefree(conn->options.ssl_capath, pers);
		conn->options.ssl_capath = NULL;
	}
	if (conn->options.ssl_cipher) {
		mnd_pefree(conn->options.ssl_cipher, pers);
		conn->options.ssl_cipher = NULL;
	}
	if (conn->zval_cache) {
		DBG_INF("Freeing zval cache reference");
		mysqlnd_palloc_free_thd_cache_reference(&conn->zval_cache);
		conn->zval_cache = NULL;
	}
	if (conn->result_set_memory_pool) {
		mysqlnd_mempool_destroy(conn->result_set_memory_pool TSRMLS_CC);
		conn->result_set_memory_pool = NULL;
	}
	if (conn->qcache) {
		DBG_INF("Freeing qcache reference");
		mysqlnd_qcache_free_cache_reference(&conn->qcache);
		conn->qcache = NULL;
	}
	if (conn->net.cmd_buffer.buffer) {
		DBG_INF("Freeing cmd buffer");
		mnd_pefree(conn->net.cmd_buffer.buffer, pers);
		conn->net.cmd_buffer.buffer = NULL;
	}

	conn->charset = NULL;
	conn->greet_charset = NULL;

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn::dtor */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, dtor)(MYSQLND *conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::dtor");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	conn->m->free_contents(conn TSRMLS_CC);

#ifdef MYSQLND_THREADED
	if (conn->thread_is_running) {
		pthread_mutex_lock(&conn->LOCK_work);
		conn->thread_killed = TRUE;
		pthread_cond_signal(&conn->COND_work);
		pthread_cond_wait(&conn->COND_thread_ended, &conn->LOCK_work);
		pthread_mutex_unlock(&conn->LOCK_work);
	}

	tsrm_mutex_free(conn->LOCK_state);

	pthread_cond_destroy(&conn->COND_work);
	pthread_cond_destroy(&conn->COND_work_done);
	pthread_mutex_destroy(&conn->LOCK_work);
#endif

	mnd_pefree(conn, conn->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_simple_command_handle_response */
enum_func_status
mysqlnd_simple_command_handle_response(MYSQLND *conn, enum php_mysql_packet_type ok_packet,
									   zend_bool silent, enum php_mysqlnd_server_command command,
									   zend_bool ignore_upsert_status
									   TSRMLS_DC)
{
	enum_func_status ret;

	DBG_ENTER("mysqlnd_simple_command_handle_response");
	DBG_INF_FMT("silent=%d packet=%d command=%s", silent, ok_packet, mysqlnd_command_to_text[command]);

	switch (ok_packet) {
		case PROT_OK_PACKET:{
			php_mysql_packet_ok ok_response;
			PACKET_INIT_ALLOCA(ok_response, PROT_OK_PACKET);
			if (FAIL == (ret = PACKET_READ_ALLOCA(ok_response, conn))) {
				if (!silent) {
					DBG_ERR_FMT("Error while reading %s's OK packet", mysqlnd_command_to_text[command]);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading %s's OK packet. PID=%d",
									 mysqlnd_command_to_text[command], getpid());
				}
			} else {
				DBG_INF_FMT("OK from server");
				if (0xFF == ok_response.field_count) {
					/* The server signalled error. Set the error */
					SET_CLIENT_ERROR(conn->error_info, ok_response.error_no,
									 ok_response.sqlstate, ok_response.error); 
					ret = FAIL;
					/*
					  Cover a protocol design error: error packet does not
					  contain the server status. Therefore, the client has no way
					  to find out whether there are more result sets of
					  a multiple-result-set statement pending. Luckily, in 5.0 an
					  error always aborts execution of a statement, wherever it is
					  a multi-statement or a stored procedure, so it should be
					  safe to unconditionally turn off the flag here.
					*/
					conn->upsert_status.server_status &= ~SERVER_MORE_RESULTS_EXISTS;
					SET_ERROR_AFF_ROWS(conn);
				} else {
					SET_NEW_MESSAGE(conn->last_message, conn->last_message_len,
									ok_response.message, ok_response.message_len,
									conn->persistent);

					if (!ignore_upsert_status) {
						conn->upsert_status.warning_count = ok_response.warning_count;
						conn->upsert_status.server_status = ok_response.server_status;
						conn->upsert_status.affected_rows = ok_response.affected_rows;
						conn->upsert_status.last_insert_id = ok_response.last_insert_id;
					}
				}
			}
			PACKET_FREE_ALLOCA(ok_response);
			break;
		}
		case PROT_EOF_PACKET:{
			php_mysql_packet_eof ok_response;
			PACKET_INIT_ALLOCA(ok_response, PROT_EOF_PACKET);
			if (FAIL == (ret = PACKET_READ_ALLOCA(ok_response, conn))) {
				SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE,
								 "Malformed packet");
				if (!silent) {
					DBG_ERR_FMT("Error while reading %s's EOF packet", mysqlnd_command_to_text[command]);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading %s's EOF packet. PID=%d",
								 	mysqlnd_command_to_text[command], getpid());
				}
			} else if (0xFF == ok_response.field_count) {
				/* The server signalled error. Set the error */
				SET_CLIENT_ERROR(conn->error_info, ok_response.error_no,
								 ok_response.sqlstate, ok_response.error);
				SET_ERROR_AFF_ROWS(conn);
			} else if (0xFE != ok_response.field_count) {
				SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE,
								 "Malformed packet");
				if (!silent) {
					DBG_ERR_FMT("EOF packet expected, field count wasn't 0xFE but 0x%2X", ok_response.field_count);
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
									"EOF packet expected, field count wasn't 0xFE but 0x%2X",
									ok_response.field_count);
				}
			} else {
				DBG_INF_FMT("OK from server");
			}
			PACKET_FREE_ALLOCA(ok_response);
			break;
		}
		default:
			ret = FAIL;
			SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE,
							 "Malformed packet");
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Wrong response packet %d passed to the function",
							 ok_packet);
			break;
	}
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_simple_command */
enum_func_status
mysqlnd_simple_command(MYSQLND *conn, enum php_mysqlnd_server_command command,
					   const char * const arg, size_t arg_len,
					   enum php_mysql_packet_type ok_packet, zend_bool silent, 
					   zend_bool ignore_upsert_status TSRMLS_DC)
{
	enum_func_status ret = PASS;
	php_mysql_packet_command cmd_packet;

	DBG_ENTER("mysqlnd_simple_command");
	DBG_INF_FMT("command=%s ok_packet=%d silent=%d", mysqlnd_command_to_text[command], ok_packet, silent);

	switch (CONN_GET_STATE(conn)) {
		case CONN_READY:
			break;
		case CONN_QUIT_SENT:
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			DBG_ERR("Server is gone");
			DBG_RETURN(FAIL);
		default:
			SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
							 mysqlnd_out_of_sync);
			DBG_ERR("Command out of sync");
			DBG_RETURN(FAIL);
	}

	/* clean UPSERT info */
	if (!ignore_upsert_status) {
		memset(&conn->upsert_status, 0, sizeof(conn->upsert_status));
	}
	SET_ERROR_AFF_ROWS(conn);
	SET_EMPTY_ERROR(conn->error_info);

	PACKET_INIT_ALLOCA(cmd_packet, PROT_CMD_PACKET);
	cmd_packet.command = command;
	if (arg && arg_len) {
		cmd_packet.argument = arg;
		cmd_packet.arg_len  = arg_len;
	}

	if (! PACKET_WRITE_ALLOCA(cmd_packet, conn)) {
		if (!silent) {
			DBG_ERR_FMT("Error while sending %s packet", mysqlnd_command_to_text[command]);
			php_error(E_WARNING, "Error while sending %s packet. PID=%d", mysqlnd_command_to_text[command], getpid());
		}
		DBG_ERR("Server is gone");
		ret = FAIL;
	} else if (ok_packet != PROT_LAST) {
		ret = mysqlnd_simple_command_handle_response(conn, ok_packet, silent, command, ignore_upsert_status TSRMLS_CC);
	}

	/*
	  There is no need to call FREE_ALLOCA on cmd_packet as the
	  only allocated string is cmd_packet.argument and it was passed
	  to us. We should not free it.
	*/

	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::set_server_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, set_server_option)(MYSQLND * const conn,
												enum_mysqlnd_server_option option TSRMLS_DC)
{
	enum_func_status ret;
	char buffer[2];
	DBG_ENTER("mysqlnd_conn::set_server_option");

	int2store(buffer, (unsigned int) option);
	ret = mysqlnd_simple_command(conn, COM_SET_OPTION, buffer, sizeof(buffer),
								 PROT_EOF_PACKET, FALSE, TRUE TSRMLS_CC);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_restart_psession */
PHPAPI void _mysqlnd_restart_psession(MYSQLND *conn, MYSQLND_THD_ZVAL_PCACHE *cache TSRMLS_DC) 
{
	DBG_ENTER("_mysqlnd_restart_psession");
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CONNECT_REUSED);
	/* Free here what should not be seen by the next script */
	if (conn->last_message) {
		mnd_pefree(conn->last_message, conn->persistent);
		conn->last_message = NULL;
	}
	/*
	  The thd zval cache is always freed on request shutdown, so this has happened already.
	  Don't touch the old value! Get new reference
	*/
	conn->zval_cache = mysqlnd_palloc_get_thd_cache_reference(cache);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ _mysqlnd_end_psession */
PHPAPI void _mysqlnd_end_psession(MYSQLND *conn TSRMLS_DC)
{
	DBG_ENTER("_mysqlnd_end_psession");
	/* The thd zval cache is always freed on request shutdown, so this has happened already */
	mysqlnd_palloc_free_thd_cache_reference(&conn->zval_cache);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_connect */
PHPAPI MYSQLND *mysqlnd_connect(MYSQLND *conn,
						 const char *host, const char *user,
						 const char *passwd, unsigned int passwd_len,
						 const char *db, unsigned int db_len,
						 unsigned int port,
						 const char *socket,
						 unsigned int mysql_flags,
						 MYSQLND_THD_ZVAL_PCACHE *zval_cache
						 TSRMLS_DC)
{
	char *transport = NULL, *errstr = NULL;
	char *hashed_details = NULL;
	int transport_len, hashed_details_len, errcode = 0, host_len;
	unsigned int streams_options = ENFORCE_SAFE_MODE;
	unsigned int streams_flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
	zend_bool self_alloced = FALSE;
	struct timeval tv;
	zend_bool unix_socket = FALSE;
	const MYSQLND_CHARSET * charset;
	zend_bool reconnect = FALSE;

	php_mysql_packet_greet greet_packet;
	php_mysql_packet_auth *auth_packet;
	php_mysql_packet_ok ok_packet;

	DBG_ENTER("mysqlnd_connect");
	DBG_INF_FMT("host=%s user=%s db=%s port=%d flags=%d persistent=%d state=%d",
				host?host:"", user?user:"", db?db:"", port, mysql_flags,
				conn? conn->persistent:0, conn? CONN_GET_STATE(conn):-1);

	DBG_INF_FMT("state=%d", CONN_GET_STATE(conn));
	if (conn && CONN_GET_STATE(conn) > CONN_ALLOCED && CONN_GET_STATE(conn) ) {
		DBG_INF("Connecting on a connected handle.");

		if (CONN_GET_STATE(conn) < CONN_QUIT_SENT) {
			MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CLOSE_IMPLICIT);
			reconnect = TRUE;
			mysqlnd_send_close(conn TSRMLS_CC);
		}

		conn->m->free_contents(conn TSRMLS_CC);
		MYSQLND_DEC_CONN_STATISTIC(&conn->stats, STAT_OPENED_CONNECTIONS);
		if (conn->persistent) {
			MYSQLND_DEC_CONN_STATISTIC(&conn->stats, STAT_OPENED_PERSISTENT_CONNECTIONS);
		}
		/* Now reconnect using the same handle */
	}

	if (!host || !host[0]) {
		host = "localhost";
	}
	if (!user || !user[0]) {
		user = php_get_current_user();
	}
	if (!passwd) {
		passwd = "";
		passwd_len = 0;
	}
	if (!db) {
		db = "";
		db_len = 0;
	}
	if (!port && !socket) {
		port = 3306;
	}
	host_len = strlen(host);
#ifndef PHP_WIN32
	if (host_len == sizeof("localhost") - 1 && !strncasecmp(host, "localhost", host_len)) {
		if (!socket) {
			socket = "/tmp/mysql.sock";
		}
		transport_len = spprintf(&transport, 0, "unix://%s", socket);
		unix_socket = TRUE;
	} else 
#endif
	{
		transport_len = spprintf(&transport, 0, "tcp://%s:%d", host, port);
	}
	DBG_INF_FMT("transport=%p", transport);


	PACKET_INIT_ALLOCA(greet_packet, PROT_GREET_PACKET);
	PACKET_INIT(auth_packet, PROT_AUTH_PACKET, php_mysql_packet_auth *, FALSE);
	PACKET_INIT_ALLOCA(ok_packet, PROT_OK_PACKET);

	if (!conn) {
		conn = mysqlnd_init(FALSE);
		self_alloced = TRUE;
	}

	if (conn->persistent) {
		hashed_details_len = spprintf(&hashed_details, 0, "%p", conn);
		DBG_INF_FMT("hashed_details=%s", hashed_details);
	} 

	CONN_SET_STATE(conn, CONN_ALLOCED);
	conn->net.packet_no = 0;

	if (conn->options.timeout_connect) {
		tv.tv_sec = conn->options.timeout_connect;
		tv.tv_usec = 0;
	}
	if (conn->persistent) {
		conn->scheme = pestrndup(transport, transport_len, 1);
		mnd_efree(transport);
	} else {
		conn->scheme = transport;
	}
	DBG_INF(conn->scheme);
	conn->net.stream = php_stream_xport_create(conn->scheme, transport_len, streams_options, streams_flags,
											   hashed_details, 
											   (conn->options.timeout_connect) ? &tv : NULL,
											    NULL /*ctx*/, &errstr, &errcode);
	DBG_INF_FMT("stream=%p", conn->net.stream);

	if (errstr || !conn->net.stream) {
		if (hashed_details) {
			mnd_efree(hashed_details);
		}
		errcode = CR_CONNECTION_ERROR;
		goto err;
	}

	if (hashed_details) {
		/*
		  If persistent, the streams register it in EG(persistent_list).
		  This is unwanted. ext/mysql or ext/mysqli are responsible to clean,
		  whatever they have to.
		*/
		zend_rsrc_list_entry *le;

		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_len + 1,
						   (void*) &le) == SUCCESS) {
			/*
			  in_free will let streams code skip destructing - big HACK,
			  but STREAMS suck big time regarding persistent streams.
			  Just not compatible for extensions that need persistency.
			*/
			conn->net.stream->in_free = 1;
			zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_len + 1);
			conn->net.stream->in_free = 0;
		}
#if ZEND_DEBUG
		/* Shut-up the streams, they don't know what they are doing */
		conn->net.stream->__exposed = 1;
#endif
		mnd_efree(hashed_details);
	}

	if (conn->options.timeout_read)
	{
		tv.tv_sec = conn->options.timeout_read;
		tv.tv_usec = 0;
		php_stream_set_option(conn->net.stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
	}

	if (!unix_socket) {
		/* Set TCP_NODELAY */
		mysqlnd_set_sock_no_delay(conn->net.stream);
	}

	if (FAIL == PACKET_READ_ALLOCA(greet_packet, conn)) {
		DBG_ERR("Error while reading greeting packet");
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading greeting packet. PID=%d", getpid());
		goto err;
	} else if (greet_packet.error_no) {
		DBG_ERR_FMT("errorno=%d error=%s", greet_packet.error_no, greet_packet.error);
		SET_CLIENT_ERROR(conn->error_info, greet_packet.error_no,
						 greet_packet.sqlstate, greet_packet.error);
		goto err;	
	} else if (greet_packet.pre41) {
		DBG_ERR_FMT("Connecting to 3.22, 3.23 & 4.0 is not supported. Server is %-.32s",
					greet_packet.server_version);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connecting to 3.22, 3.23 & 4.0 "
						" is not supported. Server is %-.32s", greet_packet.server_version);
		SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE,
						 "Connecting to 3.22, 3.23 & 4.0 servers is not supported");
		goto err;
	}

	conn->thread_id			= greet_packet.thread_id;
	conn->protocol_version	= greet_packet.protocol_version;
	conn->server_version	= greet_packet.server_version;
	greet_packet.server_version = NULL; /* The string will be freed otherwise */

	conn->greet_charset = mysqlnd_find_charset_nr(greet_packet.charset_no);
	/* we allow load data local infile by default */
	mysql_flags  |= CLIENT_LOCAL_FILES;

	auth_packet->user		= user;
	auth_packet->password	= passwd;

	if (conn->options.charset_name &&
		(charset = mysqlnd_find_charset_name(conn->options.charset_name)))
	{
		auth_packet->charset_no	= charset->nr;
#if PHP_MAJOR_VERSION >= 6
	} else if (UG(unicode)) {
		auth_packet->charset_no	= 200;/* utf8 - swedish collation, check mysqlnd_charset.c */
#endif
	} else {
		auth_packet->charset_no	= greet_packet.charset_no;
	}
	auth_packet->db			= db;
	auth_packet->db_len		= db_len;
	auth_packet->max_packet_size= 3UL*1024UL*1024UL*1024UL;
	auth_packet->client_flags= mysql_flags;

	conn->scramble = auth_packet->server_scramble_buf = mnd_pemalloc(SCRAMBLE_LENGTH, conn->persistent);
	memcpy(auth_packet->server_scramble_buf, greet_packet.scramble_buf, SCRAMBLE_LENGTH);
	if (!PACKET_WRITE(auth_packet, conn)) {
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		goto err;	
	}

	if (FAIL == PACKET_READ_ALLOCA(ok_packet, conn) || ok_packet.field_count >= 0xFE) {
		if (ok_packet.field_count == 0xFE) {
			/* old authentication with new server  !*/
			DBG_ERR(mysqlnd_old_passwd);
			SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
		} else if (ok_packet.field_count == 0xFF) {
			if (ok_packet.sqlstate[0]) {
				if (!self_alloced) {
					strlcpy(conn->error_info.sqlstate, ok_packet.sqlstate, sizeof(conn->error_info.sqlstate));
				}
				DBG_ERR_FMT("ERROR:%d [SQLSTATE:%s] %s",
							ok_packet.error_no, ok_packet.sqlstate, ok_packet.error);
			}
			if (!self_alloced) {
				conn->error_info.error_no = ok_packet.error_no;
				strlcpy(conn->error_info.error, ok_packet.error, sizeof(conn->error_info.error));
			}
		}
	} else {
		CONN_SET_STATE(conn, CONN_READY);

		conn->user				= pestrdup(user, conn->persistent);
		conn->passwd			= pestrndup(passwd, passwd_len, conn->persistent);
		conn->port				= port;
		if (host && !socket) {
			char *p;

			conn->host = pestrdup(host, conn->persistent);
			spprintf(&p, 0, "MySQL host info: %s via TCP/IP", conn->host);
			if (conn->persistent) {
				conn->host_info = pestrdup(p, 1);
				mnd_efree(p);
			} else {
				conn->host_info = p;
			}
		} else {
			conn->unix_socket	= pestrdup(socket, conn->persistent);
			conn->host_info		= pestrdup("MySQL host info: Localhost via UNIX socket", conn->persistent);
		}
		conn->client_flag		= auth_packet->client_flags;
		conn->max_packet_size	= auth_packet->max_packet_size;
		/* todo: check if charset is available */
		conn->charset			= mysqlnd_find_charset_nr(auth_packet->charset_no);
		conn->server_capabilities = greet_packet.server_capabilities;
		conn->upsert_status.warning_count = 0;
		conn->upsert_status.server_status = greet_packet.server_status;
		conn->upsert_status.affected_rows = 0;
		SET_NEW_MESSAGE(conn->last_message, conn->last_message_len,
						ok_packet.message, ok_packet.message_len,
						conn->persistent);

		SET_EMPTY_ERROR(conn->error_info);

		PACKET_FREE_ALLOCA(greet_packet);
		PACKET_FREE(auth_packet);
		PACKET_FREE_ALLOCA(ok_packet);

		conn->zval_cache = mysqlnd_palloc_get_thd_cache_reference(zval_cache);
		conn->net.cmd_buffer.length = 128L*1024L;
		conn->net.cmd_buffer.buffer = mnd_pemalloc(conn->net.cmd_buffer.length, conn->persistent);

		mysqlnd_local_infile_default(conn);
		{
			unsigned int buf_size;
			buf_size = MYSQLND_G(net_read_buffer_size); /* this is long, cast to unsigned int*/
			conn->m->set_client_option(conn, MYSQLND_OPT_NET_READ_BUFFER_SIZE,
								   		(char *)&buf_size TSRMLS_CC);

			buf_size = MYSQLND_G(net_cmd_buffer_size); /* this is long, cast to unsigned int*/
			conn->m->set_client_option(conn, MYSQLND_OPT_NET_CMD_BUFFER_SIZE,
								   		(char *)&buf_size TSRMLS_CC);			
		}

		MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CONNECT_SUCCESS);
		if (reconnect) {
			MYSQLND_INC_GLOBAL_STATISTIC(STAT_RECONNECT);	
		}
		MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_OPENED_CONNECTIONS);
		if (conn->persistent) {
			MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_OPENED_PERSISTENT_CONNECTIONS);
		}

		DBG_INF_FMT("connection_id=%llu", conn->thread_id);
		conn->result_set_memory_pool = mysqlnd_mempool_create(16000 TSRMLS_CC);
#if PHP_MAJOR_VERSION >= 6
		{
			unsigned int as_unicode = 1;
			conn->m->set_client_option(conn, MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE,
									   (char *)&as_unicode TSRMLS_CC);
			DBG_INF("unicode set");
		}
#endif
#ifdef MYSQLND_THREADED
		{
			pthread_t th;
			pthread_attr_t connection_attrib;
			conn->tsrm_ls = tsrm_ls;

			pthread_attr_init(&connection_attrib);
			pthread_attr_setdetachstate(&connection_attrib, PTHREAD_CREATE_DETACHED);

			conn->thread_is_running = TRUE;
			if (pthread_create(&th, &connection_attrib, mysqlnd_fetch_thread, (void*)conn)) {
				conn->thread_is_running = FALSE;
			}
		}
#endif


		DBG_RETURN(conn);
	}
err:
	PACKET_FREE_ALLOCA(greet_packet);
	PACKET_FREE(auth_packet);
	PACKET_FREE_ALLOCA(ok_packet);

	if (errstr) {
		DBG_ERR_FMT("[%d] %.64s (trying to connect via %s)", errcode, errstr, conn->scheme);
		SET_CLIENT_ERROR(conn->error_info, errcode? errcode:CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, errstr);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d] %.64s (trying to connect via %s)", errcode, errstr, conn->scheme);
		/* no mnd_ since we don't allocate it */
		efree(errstr);
	}
	if (conn->scheme) {
		/* no mnd_ since we don't allocate it */
		pefree(conn->scheme, conn->persistent);
		conn->scheme = NULL;
	}


	/* This will also close conn->net.stream if it has been opened */
	conn->m->free_contents(conn TSRMLS_CC);

	if (self_alloced) {
		/*
		  We have alloced, thus there are no references to this
		  object - we are free to kill it!
		*/
		conn->m->dtor(conn TSRMLS_CC);
	} else {
		MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CONNECT_FAILURE);
	}
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_conn::query */
/* 
  If conn->error_info.error_no is not zero, then we had an error.
  Still the result from the query is PASS
*/
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, query)(MYSQLND *conn, const char *query, unsigned int query_len TSRMLS_DC)
{
	enum_func_status ret;
	DBG_ENTER("mysqlnd_conn::query");
	DBG_INF_FMT("conn=%llu query=%s", conn->thread_id, query);

	if (PASS != mysqlnd_simple_command(conn, COM_QUERY, query, query_len,
									   PROT_LAST /* we will handle the OK packet*/,
									   FALSE, FALSE TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}
	CONN_SET_STATE(conn, CONN_QUERY_SENT);
	/*
	  Here read the result set. We don't do it in simple_command because it need
	  information from the ok packet. We will fetch it ourselves.
	*/
	ret = mysqlnd_query_read_result_set_header(conn, NULL TSRMLS_CC);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::send_query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, send_query)(MYSQLND *conn, const char *query, unsigned int query_len TSRMLS_DC)
{
	enum_func_status ret;
	DBG_ENTER("mysqlnd_conn::send_query");
	DBG_INF_FMT("conn=%llu query=%s", conn->thread_id, query);

	ret = mysqlnd_simple_command(conn, COM_QUERY, query, query_len,
								 PROT_LAST /* we will handle the OK packet*/,
								 FALSE, FALSE TSRMLS_CC);
	CONN_SET_STATE(conn, CONN_QUERY_SENT);
	DBG_RETURN(ret);
}
/* }}} */

/* {{{ mysqlnd_conn::send_query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, reap_query)(MYSQLND * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::reap_query");
	DBG_INF_FMT("conn=%llu", conn->thread_id);
	enum_mysqlnd_connection_state state = CONN_GET_STATE(conn);
	if (state <= CONN_READY || state == CONN_QUIT_SENT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connection not opened, clear or has been closed");
		DBG_RETURN(FAIL);	
	}
	DBG_RETURN(mysqlnd_query_read_result_set_header(conn, NULL TSRMLS_CC));
}
/* }}} */


#include "php_network.h"

MYSQLND ** mysqlnd_stream_array_check_for_readiness(MYSQLND ** conn_array TSRMLS_DC)
{
	int cnt = 0;
	MYSQLND **p = conn_array, **p_p;
	MYSQLND **ret = NULL;

	while (*p) {
		if (CONN_GET_STATE(*p) <= CONN_READY || CONN_GET_STATE(*p) == CONN_QUIT_SENT) {
			cnt++;
		}
		p++;
	}
	if (cnt) {
		MYSQLND **ret_p = ret = ecalloc(cnt + 1, sizeof(MYSQLND *));
		p_p = p = conn_array;
		while (*p) {
			if (CONN_GET_STATE(*p) <= CONN_READY || CONN_GET_STATE(*p) == CONN_QUIT_SENT) {
				*ret_p = *p;
				*p = NULL;
				ret_p++;
			} else {
				*p_p = *p;
				p_p++;
			}
			p++;
		}
		*ret_p = NULL; 
	}
	return ret;
}


/* {{{ stream_select mysqlnd_stream_array_to_fd_set functions */
static int mysqlnd_stream_array_to_fd_set(MYSQLND **conn_array, fd_set *fds, php_socket_t *max_fd TSRMLS_DC)
{
	php_socket_t this_fd;
	int cnt = 0;
	MYSQLND **p = conn_array;

	while (*p) {
		/* get the fd.
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 * */
		if (SUCCESS == php_stream_cast((*p)->net.stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
										(void*)&this_fd, 1) && this_fd >= 0) {
			
			PHP_SAFE_FD_SET(this_fd, fds);

			if (this_fd > *max_fd) {
				*max_fd = this_fd;
			}
			cnt++;
		}
		p++;
	}
	return cnt ? 1 : 0;
}

static int mysqlnd_stream_array_from_fd_set(MYSQLND **conn_array, fd_set *fds TSRMLS_DC)
{
	php_socket_t this_fd;
	int ret = 0;
	zend_bool disproportion = FALSE;


	MYSQLND **fwd = conn_array, **bckwd = conn_array;

	while (*fwd) {
		if (SUCCESS == php_stream_cast((*fwd)->net.stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
										(void*)&this_fd, 1) && this_fd >= 0) {
			if (PHP_SAFE_FD_ISSET(this_fd, fds)) {
				if (disproportion) {
					*bckwd = *fwd;
				}
				bckwd++;
				fwd++;
				ret++;
				continue;
			}
		}
		disproportion = TRUE;
		fwd++;
	}
	*bckwd = NULL;/* NULL-terminate the list */
	
	return ret;
}


#ifndef PHP_WIN32
#define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#else
#include "win32/select.h"
#endif

/* {{{ _mysqlnd_poll */
enum_func_status
_mysqlnd_poll(MYSQLND **r_array, MYSQLND **e_array, MYSQLND ***dont_poll, long sec, long usec, uint * desc_num TSRMLS_DC)
{

	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	php_socket_t	max_fd = 0;
	int				retval, sets = 0;
	int				set_count, max_set_count = 0;
	DBG_ENTER("mysqlnd_poll");

	if (sec < 0 || usec < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Negative values passed for sec and/or usec");
		DBG_RETURN(FAIL);
	}

	*dont_poll = mysqlnd_stream_array_check_for_readiness(r_array TSRMLS_CC);

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) {
		set_count = mysqlnd_stream_array_to_fd_set(r_array, &rfds, &max_fd TSRMLS_CC);
		if (set_count > max_set_count) {
			max_set_count = set_count;
		}
		sets += set_count;
	}

	if (e_array != NULL) {
		set_count = mysqlnd_stream_array_to_fd_set(e_array, &efds, &max_fd TSRMLS_CC);
		if (set_count > max_set_count) {
			max_set_count = set_count;
		}
		sets += set_count;
	}

	if (!sets) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, *dont_poll ? "All arrays passed are clear":"No stream arrays were passed");
		DBG_RETURN(FAIL);
	}

	PHP_SAFE_MAX_FD(max_fd, max_set_count);

	/* Solaris + BSD do not like microsecond values which are >= 1 sec */
	if (usec > 999999) {
		tv.tv_sec = sec + (usec / 1000000);
		tv.tv_usec = usec % 1000000;			
	} else {
		tv.tv_sec = sec;
		tv.tv_usec = usec;
	}

	tv_p = &tv;
	
	retval = php_select(max_fd + 1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to select [%d]: %s (max_fd=%d)",
						errno, strerror(errno), max_fd);
		DBG_RETURN(FAIL);
	}

	if (r_array != NULL) {
		mysqlnd_stream_array_from_fd_set(r_array, &rfds TSRMLS_CC);
	}
	if (e_array != NULL) {
		mysqlnd_stream_array_from_fd_set(e_array, &efds TSRMLS_CC);
	}

	*desc_num = retval;

	DBG_RETURN(PASS);
}
/* }}} */


/*
  COM_FIELD_LIST is special, different from a SHOW FIELDS FROM :
  - There is no result set header - status from the command, which
    impacts us to allocate big chunk of memory for reading the metadata.
  - The EOF packet is consumed by the metadata packet reader.
*/

/* {{{ mysqlnd_conn::list_fields */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, list_fields)(MYSQLND *conn, const char *table, const char *achtung_wild TSRMLS_DC)
{
	/* db + \0 + wild + \0 (for wild) */
	char buff[MYSQLND_MAX_ALLOWED_DB_LEN * 4 * 2 + 1 + 1], *p;
	size_t table_len, wild_len;
	MYSQLND_RES *result = NULL;
	DBG_ENTER("mysqlnd_conn::list_fields");
	DBG_INF_FMT("conn=%llu table=%s wild=%s", conn->thread_id, table? table:"",achtung_wild? achtung_wild:"");

	p = buff;
	if (table && (table_len = strlen(table))) {
		memcpy(p, table, MIN(table_len, MYSQLND_MAX_ALLOWED_DB_LEN * 4));
		p += table_len;
		*p++ = '\0';
	}

	if (achtung_wild && (wild_len = strlen(achtung_wild))) {
		memcpy(p, achtung_wild, MIN(wild_len, MYSQLND_MAX_ALLOWED_DB_LEN * 4));
		p += wild_len;	
		*p++ = '\0';
	}

	if (PASS != mysqlnd_simple_command(conn, COM_FIELD_LIST, buff, p - buff,
									   PROT_LAST /* we will handle the OK packet*/,
									   FALSE, TRUE TSRMLS_CC)) {
		DBG_RETURN(NULL);
	}
	/*
	   Prepare for the worst case.
	   MyISAM goes to 2500 BIT columns, double it for safety.
	 */
	result = mysqlnd_result_init(5000, mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache) TSRMLS_CC);


	if (FAIL == result->m.read_result_metadata(result, conn TSRMLS_CC)) {
		DBG_ERR("Error ocurred while reading metadata");
		result->m.free_result(result, TRUE TSRMLS_CC);
		DBG_RETURN(NULL);
	}

	result->type = MYSQLND_RES_NORMAL;
	result->m.fetch_row = result->m.fetch_row_normal_unbuffered;
	result->unbuf = mnd_ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));
	result->unbuf->eof_reached = TRUE;

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn::list_method */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, list_method)(MYSQLND *conn, const char *query,
										  const char *achtung_wild, char *par1 TSRMLS_DC)
{
	char *show_query = NULL;
	size_t show_query_len;
	MYSQLND_RES *result = NULL;

	DBG_ENTER("mysqlnd_conn::list_method");
	DBG_INF_FMT("conn=%llu query=%s wild=%d", conn->thread_id, query, achtung_wild);

	if (par1) {
		if (achtung_wild) {
			show_query_len = spprintf(&show_query, 0, query, par1, achtung_wild);
		} else {
			show_query_len = spprintf(&show_query, 0, query, par1);	
		}
	} else {
		if (achtung_wild) {
			show_query_len = spprintf(&show_query, 0, query, achtung_wild);
		} else {
			show_query_len = strlen(show_query = (char *)query);	
		}	
	}

	if (PASS == conn->m->query(conn, show_query, show_query_len TSRMLS_CC)) {
		result = conn->m->store_result(conn TSRMLS_CC);
	}
	if (show_query != query) {
		mnd_efree(show_query);
	}
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn::errno */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn, errno)(const MYSQLND * const conn)
{
	return conn->error_info.error_no;
}
/* }}} */


/* {{{ mysqlnd_conn::error */
static const char *
MYSQLND_METHOD(mysqlnd_conn, error)(const MYSQLND * const conn)
{
	return conn->error_info.error;
}
/* }}} */


/* {{{ mysqlnd_conn::sqlstate */
static const char *
MYSQLND_METHOD(mysqlnd_conn, sqlstate)(const MYSQLND * const conn)
{
	return conn->error_info.sqlstate[0] ? conn->error_info.sqlstate:MYSQLND_SQLSTATE_NULL;
}
/* }}} */


/* {{{ mysqlnd_old_escape_string */
PHPAPI ulong mysqlnd_old_escape_string(char *newstr, const char *escapestr, size_t escapestr_len TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_old_escape_string");
	DBG_RETURN(mysqlnd_cset_escape_slashes(mysqlnd_find_charset_name("latin1"),
										   newstr, escapestr, escapestr_len TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_conn::escape_string */
static ulong
MYSQLND_METHOD(mysqlnd_conn, escape_string)(const MYSQLND * const conn, char *newstr,
											const char *escapestr, size_t escapestr_len TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::escape_string");
	DBG_INF_FMT("conn=%llu", conn->thread_id);
	if (conn->upsert_status.server_status & SERVER_STATUS_NO_BACKSLASH_ESCAPES) {
		DBG_RETURN(mysqlnd_cset_escape_quotes(conn->charset, newstr, escapestr, escapestr_len TSRMLS_CC));
	}
	DBG_RETURN(mysqlnd_cset_escape_slashes(conn->charset, newstr, escapestr, escapestr_len TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_conn::dump_debug_info */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, dump_debug_info)(MYSQLND * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::dump_debug_info");
	DBG_INF_FMT("conn=%llu", conn->thread_id);
	DBG_RETURN(mysqlnd_simple_command(conn, COM_DEBUG, NULL, 0, PROT_EOF_PACKET, FALSE, TRUE TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_conn::select_db */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, select_db)(MYSQLND * const conn,
										const char * const db,
										unsigned int db_len TSRMLS_DC)
{
	enum_func_status ret;

	DBG_ENTER("mysqlnd_conn::select_db");
	DBG_INF_FMT("conn=%llu db=%s", conn->thread_id, db);

	ret = mysqlnd_simple_command(conn, COM_INIT_DB, db, db_len, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC);
	/*
	  The server sends 0 but libmysql doesn't read it and has established
	  a protocol of giving back -1. Thus we have to follow it :(
	*/
	SET_ERROR_AFF_ROWS(conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::ping */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, ping)(MYSQLND * const conn TSRMLS_DC)
{
	enum_func_status ret;

	DBG_ENTER("mysqlnd_conn::ping");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	ret = mysqlnd_simple_command(conn, COM_PING, NULL, 0, PROT_OK_PACKET, TRUE, TRUE TSRMLS_CC);
	/*
	  The server sends 0 but libmysql doesn't read it and has established
	  a protocol of giving back -1. Thus we have to follow it :(
	*/
	SET_ERROR_AFF_ROWS(conn);

	DBG_INF_FMT("ret=%d", ret);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::stat */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, stat)(MYSQLND *conn, char **message, unsigned int * message_len TSRMLS_DC)
{
	enum_func_status ret;
	php_mysql_packet_stats stats_header;

	DBG_ENTER("mysqlnd_conn::stat");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	ret = mysqlnd_simple_command(conn, COM_STATISTICS, NULL, 0, PROT_LAST, FALSE, TRUE TSRMLS_CC);
	if (FAIL == ret) {
		DBG_RETURN(FAIL);
	}
	PACKET_INIT_ALLOCA(stats_header, PROT_STATS_PACKET);
	if (FAIL == (ret = PACKET_READ_ALLOCA(stats_header, conn))) {
		DBG_RETURN(FAIL);
	}
	*message = stats_header.message;
	*message_len = stats_header.message_len;
	/* Ownership transfer */
	stats_header.message = NULL;
	PACKET_FREE_ALLOCA(stats_header);

	DBG_INF(*message);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_conn::kill */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, kill)(MYSQLND *conn, unsigned int pid TSRMLS_DC)
{
	enum_func_status ret;
	char buff[4];

	DBG_ENTER("mysqlnd_conn::kill");
	DBG_INF_FMT("conn=%llu pid=%lu", conn->thread_id, pid);

	int4store(buff, pid);

	/* If we kill ourselves don't expect OK packet, PROT_LAST will skip it */
	if (pid != conn->thread_id) {
		ret = mysqlnd_simple_command(conn, COM_PROCESS_KILL, buff, 4, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC);
		/*
		  The server sends 0 but libmysql doesn't read it and has established
		  a protocol of giving back -1. Thus we have to follow it :(
		*/
		SET_ERROR_AFF_ROWS(conn);
	} else if (PASS == (ret = mysqlnd_simple_command(conn, COM_PROCESS_KILL, buff,
													 4, PROT_LAST, FALSE, TRUE TSRMLS_CC))) {
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::set_charset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, set_charset)(MYSQLND * const conn, const char * const csname TSRMLS_DC)
{
	enum_func_status ret = PASS;
	char *query;
	size_t query_len;
	const MYSQLND_CHARSET * const charset = mysqlnd_find_charset_name(csname);

	DBG_ENTER("mysqlnd_conn::set_charset");
	DBG_INF_FMT("conn=%llu cs=%s", conn->thread_id, csname);

	if (!charset) {
		SET_CLIENT_ERROR(conn->error_info, CR_CANT_FIND_CHARSET, UNKNOWN_SQLSTATE,
						 "Invalid characterset or character set not supported");
		DBG_RETURN(FAIL);
	}

	query_len = spprintf(&query, 0, "SET NAMES %s", csname);

	if (FAIL == conn->m->query(conn, query, query_len TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error executing query");
	} else if (conn->error_info.error_no) {
		ret = FAIL;
	} else {
		conn->charset = charset;
	}
	mnd_efree(query);

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::refresh */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, refresh)(MYSQLND * const conn, unsigned long options TSRMLS_DC)
{
	zend_uchar bits[1];
	DBG_ENTER("mysqlnd_conn::refresh");
	DBG_INF_FMT("conn=%llu options=%lu", conn->thread_id, options);

	int1store(bits, options);

	DBG_RETURN(mysqlnd_simple_command(conn, COM_REFRESH, (char *)bits, 1, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_conn::shutdown */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, shutdown)(MYSQLND * const conn, unsigned long level TSRMLS_DC)
{
	zend_uchar bits[1];
	DBG_ENTER("mysqlnd_conn::shutdown");
	DBG_INF_FMT("conn=%llu level=%lu", conn->thread_id, level);

	int1store(bits, level);

	DBG_RETURN(mysqlnd_simple_command(conn, COM_SHUTDOWN, (char *)bits, 1, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_send_close */
static enum_func_status
mysqlnd_send_close(MYSQLND * conn TSRMLS_DC)
{
	enum_func_status ret = PASS;

	DBG_ENTER("mysqlnd_send_close");
	DBG_INF_FMT("conn=%llu conn->net.stream->abstract=%p",
				conn->thread_id, conn->net.stream? conn->net.stream->abstract:NULL);

	switch (CONN_GET_STATE(conn)) {
		case CONN_READY:
			DBG_INF("Connection clean, sending COM_QUIT");
			ret =  mysqlnd_simple_command(conn, COM_QUIT, NULL, 0, PROT_LAST,
										  TRUE, TRUE TSRMLS_CC);
			/* Do nothing */
			break;
		case CONN_SENDING_LOAD_DATA:
			/*
			  Don't send COM_QUIT if we are in a middle of a LOAD DATA or we
			  will crash (assert) a debug server.
			*/
		case CONN_NEXT_RESULT_PENDING:
		case CONN_QUERY_SENT:
		case CONN_FETCHING_DATA:
			MYSQLND_INC_GLOBAL_STATISTIC(STAT_CLOSE_IN_MIDDLE);
			DBG_ERR_FMT("Brutally closing connection [%p][%s]", conn, conn->scheme);
			/*
			  Do nothing, the connection will be brutally closed
			  and the server will catch it and free close from its side.
			*/
		case CONN_ALLOCED:
			/*
			  Allocated but not connected or there was failure when trying
			  to connect with pre-allocated connect.

			  Fall-through
			*/
		case CONN_QUIT_SENT:
			/* The user has killed its own connection */
			break;
	}
	/*
	  We hold one reference, and every other object which needs the
	  connection does increase it by 1.
	*/
	CONN_SET_STATE(conn, CONN_QUIT_SENT);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, close)(MYSQLND * conn, enum_connection_close_type close_type TSRMLS_DC)
{
	enum_func_status ret = PASS;
	static enum_mysqlnd_collected_stats
	close_type_to_stat_map[MYSQLND_CLOSE_LAST] = {
		STAT_CLOSE_EXPLICIT,
		STAT_CLOSE_IMPLICIT,
		STAT_CLOSE_DISCONNECT
	};
	enum_mysqlnd_collected_stats stat = close_type_to_stat_map[close_type];

	DBG_ENTER("mysqlnd_conn::close");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (conn->state >= CONN_READY) {
		MYSQLND_INC_CONN_STATISTIC(&conn->stats, stat);
		MYSQLND_DEC_CONN_STATISTIC(&conn->stats, STAT_OPENED_CONNECTIONS);
		if (conn->persistent) {
			MYSQLND_DEC_CONN_STATISTIC(&conn->stats, STAT_OPENED_PERSISTENT_CONNECTIONS);
		}
	}

	/*
	  Close now, free_reference will try,
	  if we are last, but that's not a problem.
	*/
	ret = mysqlnd_send_close(conn TSRMLS_CC);

	ret = conn->m->free_reference(conn TSRMLS_CC);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::get_reference */
static MYSQLND *
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, get_reference)(MYSQLND * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::get_reference");
	++conn->refcount;
	DBG_INF_FMT("conn=%llu new_refcount=%u", conn->thread_id, conn->refcount);
	DBG_RETURN(conn);
}
/* }}} */


/* {{{ mysqlnd_conn::free_reference */
static enum_func_status
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, free_reference)(MYSQLND * const conn TSRMLS_DC)
{
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_conn::free_reference");
	DBG_INF_FMT("conn=%llu old_refcount=%u", conn->thread_id, conn->refcount);
	if (!(--conn->refcount)) {
		/*
		  No multithreading issues as we don't share the connection :)
		  This will free the object too, of course because references has
		  reached zero.
		*/
		ret = mysqlnd_send_close(conn TSRMLS_CC);
		conn->m->dtor(conn TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::get_state */
#ifdef MYSQLND_THREADED
static enum mysqlnd_connection_state
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, get_state)(MYSQLND * const conn TSRMLS_DC)
{
	enum mysqlnd_connection_state state;
	DBG_ENTER("mysqlnd_conn::get_state");
	tsrm_mutex_lock(conn->LOCK_state);
	state = conn->state;
	tsrm_mutex_unlock(conn->LOCK_state);
	DBG_RETURN(state);
}
#else
static enum mysqlnd_connection_state
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, get_state)(MYSQLND * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::get_state");
	DBG_RETURN(conn->state);
}
#endif
/* }}} */


/* {{{ mysqlnd_conn::set_state */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, set_state)(MYSQLND * const conn, enum mysqlnd_connection_state new_state TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::set_state");
#ifdef MYSQLND_THREADED
 	tsrm_mutex_lock(conn->LOCK_state);
#endif
	DBG_INF_FMT("New state=%d", new_state);
	conn->state = new_state;
#ifdef MYSQLND_THREADED
	tsrm_mutex_unlock(conn->LOCK_state);
#endif
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn::field_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn, field_count)(const MYSQLND * const conn)
{
	return conn->field_count;
}
/* }}} */


/* {{{ mysqlnd_conn::insert_id */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn, insert_id)(const MYSQLND * const conn)
{
	return conn->upsert_status.last_insert_id;
}
/* }}} */


/* {{{ mysqlnd_conn::affected_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn, affected_rows)(const MYSQLND * const conn)
{
	return conn->upsert_status.affected_rows;
}
/* }}} */


/* {{{ mysqlnd_conn::warning_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn, warning_count)(const MYSQLND * const conn)
{
	return conn->upsert_status.warning_count;
}
/* }}} */


/* {{{ mysqlnd_conn::info */
static const char *
MYSQLND_METHOD(mysqlnd_conn, info)(const MYSQLND * const conn)
{
	return conn->last_message;
}
/* }}} */

#if !defined(MYSQLND_USE_OPTIMISATIONS) || MYSQLND_USE_OPTIMISATIONS == 0
/* {{{ mysqlnd_get_client_info */
PHPAPI const char * mysqlnd_get_client_info()
{
	return MYSQLND_VERSION;
}
/* }}} */


/* {{{ mysqlnd_get_client_version */
PHPAPI unsigned int mysqlnd_get_client_version()
{
	return MYSQLND_VERSION_ID;
}
/* }}} */
#endif

/* {{{ mysqlnd_conn::get_server_info */
static const char *
MYSQLND_METHOD(mysqlnd_conn, get_server_info)(const MYSQLND * const conn)
{
	return conn->server_version;
}
/* }}} */


/* {{{ mysqlnd_conn::get_host_info */
static const char *
MYSQLND_METHOD(mysqlnd_conn, get_host_info)(const MYSQLND * const conn)
{
	return conn->host_info;
}
/* }}} */


/* {{{ mysqlnd_conn::get_proto_info */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn, get_proto_info)(const MYSQLND *const conn)
{
	return conn->protocol_version;
}
/* }}} */


/* {{{ mysqlnd_conn::charset_name */
static const char *
MYSQLND_METHOD(mysqlnd_conn, charset_name)(const MYSQLND * const conn)
{
	return conn->charset->name;
}
/* }}} */


/* {{{ mysqlnd_conn::thread_id */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn, thread_id)(const MYSQLND * const conn)
{
	return conn->thread_id;
}
/* }}} */


/* {{{ mysqlnd_conn::get_server_version */
static unsigned long
MYSQLND_METHOD(mysqlnd_conn, get_server_version)(const MYSQLND * const conn)
{
	long major, minor, patch;
	char *p;

	if (!(p = conn->server_version)) {
		return 0;
	}

	major = strtol(p, &p, 10);
	p += 1; /* consume the dot */
	minor = strtol(p, &p, 10);
	p += 1; /* consume the dot */
	patch = strtol(p, &p, 10);
	
	return (unsigned long)(major * 10000L + (unsigned long)(minor * 100L + patch));
}
/* }}} */


/* {{{ mysqlnd_conn::more_results */
static zend_bool
MYSQLND_METHOD(mysqlnd_conn, more_results)(const MYSQLND * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::more_results");
	/* (conn->state == CONN_NEXT_RESULT_PENDING) too */
	DBG_RETURN(conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS? TRUE:FALSE);
}
/* }}} */


/* {{{ mysqlnd_conn::next_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, next_result)(MYSQLND * const conn TSRMLS_DC)
{
	enum_func_status ret;

	DBG_ENTER("mysqlnd_conn::next_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (CONN_GET_STATE(conn) != CONN_NEXT_RESULT_PENDING) {
		DBG_RETURN(FAIL);
	}

	SET_EMPTY_ERROR(conn->error_info);
	SET_ERROR_AFF_ROWS(conn);
	/*
	  We are sure that there is a result set, since conn->state is set accordingly
	  in mysqlnd_store_result() or mysqlnd_fetch_row_unbuffered()
	*/
	if (FAIL == (ret = mysqlnd_query_read_result_set_header(conn, NULL TSRMLS_CC))) {
		DBG_ERR_FMT("Serious error. %s::%d", __FILE__, __LINE__);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Serious error. PID=%d", getpid());
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_field_type_name */
PHPAPI const char *mysqlnd_field_type_name(enum mysqlnd_field_types field_type)
{
	switch(field_type) {
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
			return "string";
		case FIELD_TYPE_TINY:
		case FIELD_TYPE_SHORT:
		case FIELD_TYPE_LONG:
		case FIELD_TYPE_LONGLONG:
		case FIELD_TYPE_INT24:
			return "int";
		case FIELD_TYPE_FLOAT:
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_DECIMAL:
		case FIELD_TYPE_NEWDECIMAL:
			return "real";
		case FIELD_TYPE_TIMESTAMP:
			return "timestamp";
		case FIELD_TYPE_YEAR:
			return "year";
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_NEWDATE:
			return "date";
		case FIELD_TYPE_TIME:
			return "time";
		case FIELD_TYPE_SET:
			return "set";
		case FIELD_TYPE_ENUM:
			return "enum";
		case FIELD_TYPE_GEOMETRY:
			return "geometry";
		case FIELD_TYPE_DATETIME:
			return "datetime";
		case FIELD_TYPE_TINY_BLOB:
		case FIELD_TYPE_MEDIUM_BLOB:
		case FIELD_TYPE_LONG_BLOB:
		case FIELD_TYPE_BLOB:
			return "blob";
		case FIELD_TYPE_NULL:
			return "null";
		case FIELD_TYPE_BIT:
			return "bit";
		default:
			return "unknown";
	}
}
/* }}} */


/* {{{ mysqlnd_conn::change_user */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, change_user)(MYSQLND * const conn,
										  const char *user,
										  const char *passwd,
										  const char *db TSRMLS_DC)
{
	/*
	  User could be max 16 * 3 (utf8), pass is 20 usually, db is up to 64*3
	  Stack space is not that expensive, so use a bit more to be protected against
	  stack overrungs.
	*/
	size_t user_len;
	enum_func_status ret;
	php_mysql_packet_chg_user_resp chg_user_resp;
	char buffer[MYSQLND_MAX_ALLOWED_USER_LEN + 1 + SCRAMBLE_LENGTH + MYSQLND_MAX_ALLOWED_DB_LEN + 1];
	char *p = buffer;

	DBG_ENTER("mysqlnd_conn::change_user");
	DBG_INF_FMT("conn=%llu user=%s passwd=%s db=%s",
				conn->thread_id, user?user:"", passwd?"***":"null", db?db:"");

	if (!user) {
		user = "";
	}
	if (!passwd) {
		passwd = "";
	}
	if (!db) {
		db = "";
	}

	/* 1. user ASCIIZ */
	user_len = MIN(strlen(user), MYSQLND_MAX_ALLOWED_DB_LEN);
	memcpy(p, user, user_len);
	p += user_len;
	*p++ = '\0';

	/* 2. password SCRAMBLE_LENGTH followed by the scramble or \0 */
	if (passwd[0]) {
		*p++ = SCRAMBLE_LENGTH;
		php_mysqlnd_scramble((unsigned char *)p, conn->scramble, (unsigned char *)passwd);
		p += SCRAMBLE_LENGTH;
	} else {
		*p++ = '\0';
	}

	/* 3. db ASCIIZ */
	if (db[0]) {
		size_t db_len = strlen(db);
		memcpy(p, db, MIN(db_len, MYSQLND_MAX_ALLOWED_DB_LEN));
		p += db_len;
	}
	*p++ = '\0';

	if (PASS != mysqlnd_simple_command(conn, COM_CHANGE_USER, buffer, p - buffer,
									   PROT_LAST /* we will handle the OK packet*/,
									   FALSE, TRUE TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}

	PACKET_INIT_ALLOCA(chg_user_resp, PROT_CHG_USER_PACKET);
	ret = PACKET_READ_ALLOCA(chg_user_resp, conn);
	conn->error_info = chg_user_resp.error_info;
	PACKET_FREE_ALLOCA(chg_user_resp);

	if (conn->error_info.error_no) {
		ret = FAIL;
		/*
		  COM_CHANGE_USER is broken in 5.1. At least in 5.1.15 and 5.1.14, 5.1.11 is immune.
		  bug#25371 mysql_change_user() triggers "packets out of sync"
		  When it gets fixed, there should be one more check here
		*/
		if (mysqlnd_get_server_version(conn) > 50113L &&
			mysqlnd_get_server_version(conn) < 50118L)
		{
			php_mysql_packet_ok redundant_error_packet;
			PACKET_INIT_ALLOCA(redundant_error_packet, PROT_OK_PACKET);
			PACKET_READ_ALLOCA(redundant_error_packet, conn);
			PACKET_FREE_ALLOCA(redundant_error_packet);
			DBG_INF_FMT("Server is %d, buggy, sends two ERR messages", mysqlnd_get_server_version(conn));
		}
	}
	if (ret == PASS) {
		mnd_pefree(conn->user, conn->persistent);
		conn->user = pestrndup(user, user_len, conn->persistent);
		mnd_pefree(conn->passwd, conn->persistent);
		conn->passwd = pestrdup(passwd, conn->persistent);
		if (conn->last_message) {
			mnd_pefree(conn->last_message, conn->persistent);
			conn->last_message = NULL;
		}
		conn->charset = conn->greet_charset;
		memset(&conn->upsert_status, 0, sizeof(conn->upsert_status));
	}

	SET_ERROR_AFF_ROWS(conn);

	/*
	  Here we should close all statements. Unbuffered queries should not be a
	  problem as we won't allow sending COM_CHANGE_USER.
	*/
	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, set_client_option)(MYSQLND * const conn,
												enum mysqlnd_option option,
												const char * const value
												TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::set_client_option");
	DBG_INF_FMT("conn=%llu option=%d", conn->thread_id, option);
	switch (option) {
#if PHP_MAJOR_VERSION >= 6
		case MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE:
			conn->options.numeric_and_datetime_as_unicode = *(unsigned int*) value;
			break;
#endif
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
			conn->net.cmd_buffer.length = *(unsigned int*) value;
			if (!conn->net.cmd_buffer.buffer) {
				conn->net.cmd_buffer.buffer = mnd_pemalloc(conn->net.cmd_buffer.length, conn->persistent);
			} else {
				conn->net.cmd_buffer.buffer = mnd_perealloc(conn->net.cmd_buffer.buffer,
															conn->net.cmd_buffer.length,
															conn->persistent);
			}
			break;
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
			conn->options.net_read_buffer_size = *(unsigned int*) value;
			break;
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
		case MYSQLND_OPT_INT_AND_FLOAT_NATIVE:
			conn->options.int_and_float_native = *(unsigned int*) value;
			break;
#endif
		case MYSQL_OPT_CONNECT_TIMEOUT:
			conn->options.timeout_connect = *(unsigned int*) value;
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_READ_TIMEOUT:
			conn->options.timeout_read = *(unsigned int*) value;
			break;
		case MYSQL_OPT_WRITE_TIMEOUT:
			conn->options.timeout_write = *(unsigned int*) value;
			break;
#endif
		case MYSQL_OPT_LOCAL_INFILE:
			if (!value || (*(unsigned int*) value) ? 1 : 0) {
				conn->options.flags |= CLIENT_LOCAL_FILES;
			} else {
				conn->options.flags &= ~CLIENT_LOCAL_FILES;
			}
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_COMPRESS:
#endif
		case MYSQL_INIT_COMMAND:
		case MYSQL_READ_DEFAULT_FILE:
		case MYSQL_READ_DEFAULT_GROUP:
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_SET_CLIENT_IP:
		case MYSQL_REPORT_DATA_TRUNCATION:
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
#endif
			/* currently not supported. Todo!! */
			break;
		case MYSQL_SET_CHARSET_NAME:
			conn->options.charset_name = pestrdup(value, conn->persistent);
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_SET_CHARSET_DIR:
		case MYSQL_OPT_RECONNECT:
		case MYSQL_OPT_PROTOCOL:
			/* we don't need external character sets, all character sets are
			   compiled in. For compatibility we just ignore this setting.
			   Same for protocol, we don't support old protocol */
		case MYSQL_OPT_USE_REMOTE_CONNECTION:
		case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
		case MYSQL_OPT_GUESS_CONNECTION:
			/* todo: throw an error, we don't support embedded */
			break;
#endif

#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_NAMED_PIPE:
		case MYSQL_SHARED_MEMORY_BASE_NAME:
		case MYSQL_OPT_USE_RESULT:
		case MYSQL_SECURE_AUTH:
			/* not sure, todo ? */
#endif
		default:
			DBG_RETURN(FAIL);
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_conn::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, use_result)(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND_RES *result;

	DBG_ENTER("mysqlnd_conn::use_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (!conn->current_result) {
		DBG_RETURN(NULL);
	}

	/* Nothing to store for UPSERT/LOAD DATA */
	if (conn->last_query_type != QUERY_SELECT || CONN_GET_STATE(conn) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync);
		DBG_ERR("Command out of sync");
		DBG_RETURN(NULL);
	}

	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_UNBUFFERED_SETS);

	result = conn->current_result;
	conn->current_result = NULL;
	result->conn = conn->m->get_reference(conn TSRMLS_CC);

	result = result->m.use_result(result, FALSE TSRMLS_CC);
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, store_result)(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND_RES *result;

	DBG_ENTER("mysqlnd_conn::store_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (!conn->current_result) {
		DBG_RETURN(NULL);
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (conn->last_query_type != QUERY_SELECT || CONN_GET_STATE(conn) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync);
		DBG_ERR("Command out of sync");
		DBG_RETURN(NULL);
	}

	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_BUFFERED_SETS);

	result = conn->current_result;
	conn->current_result = NULL;

	result = result->m.store_result(result, conn, FALSE TSRMLS_CC);
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn::background_store_result */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, background_store_result)(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND_RES *result;

	DBG_ENTER("mysqlnd_conn::store_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (!conn->current_result) {
		DBG_RETURN(NULL);
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (conn->last_query_type != QUERY_SELECT || CONN_GET_STATE(conn) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync);
		DBG_ERR("Command out of sync");
		DBG_RETURN(NULL);
	}

	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_BUFFERED_SETS);

	result = conn->current_result;

	result = result->m.background_store_result(result, conn, FALSE TSRMLS_CC);

	/*
	  Should be here, because current_result is used by the fetching thread to get data info
	  The thread is contacted in mysqlnd_res::background_store_result().
	*/
	conn->current_result = NULL;

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn::get_connection_stats */
static void
MYSQLND_METHOD(mysqlnd_conn, get_connection_stats)(const MYSQLND * const conn,
												   zval *return_value
												   TSRMLS_DC ZEND_FILE_LINE_DC)
{
	DBG_ENTER("mysqlnd_conn::get_connection_stats");
	DBG_INF_FMT("conn=%llu", conn->thread_id);
	mysqlnd_fill_stats_hash(&(conn->stats), return_value TSRMLS_CC ZEND_FILE_LINE_CC);
	DBG_VOID_RETURN;
}
/* }}} */


MYSQLND_STMT * _mysqlnd_stmt_init(MYSQLND * const conn TSRMLS_DC);


MYSQLND_CLASS_METHODS_START(mysqlnd_conn)
	MYSQLND_METHOD(mysqlnd_conn, escape_string),
	MYSQLND_METHOD(mysqlnd_conn, set_charset),
	MYSQLND_METHOD(mysqlnd_conn, query),
	MYSQLND_METHOD(mysqlnd_conn, send_query),
	MYSQLND_METHOD(mysqlnd_conn, reap_query),
	MYSQLND_METHOD(mysqlnd_conn, use_result),
	MYSQLND_METHOD(mysqlnd_conn, store_result),
	MYSQLND_METHOD(mysqlnd_conn, background_store_result),
	MYSQLND_METHOD(mysqlnd_conn, next_result),
	MYSQLND_METHOD(mysqlnd_conn, more_results),

	_mysqlnd_stmt_init,

	MYSQLND_METHOD(mysqlnd_conn, shutdown),
	MYSQLND_METHOD(mysqlnd_conn, refresh),

	MYSQLND_METHOD(mysqlnd_conn, ping),
	MYSQLND_METHOD(mysqlnd_conn, kill),
	MYSQLND_METHOD(mysqlnd_conn, select_db),
	MYSQLND_METHOD(mysqlnd_conn, dump_debug_info),
	MYSQLND_METHOD(mysqlnd_conn, change_user),

	MYSQLND_METHOD(mysqlnd_conn, errno),
	MYSQLND_METHOD(mysqlnd_conn, error),
	MYSQLND_METHOD(mysqlnd_conn, sqlstate),
	MYSQLND_METHOD(mysqlnd_conn, thread_id),

	MYSQLND_METHOD(mysqlnd_conn, get_connection_stats),

	MYSQLND_METHOD(mysqlnd_conn, get_server_version),
	MYSQLND_METHOD(mysqlnd_conn, get_server_info),
	MYSQLND_METHOD(mysqlnd_conn, stat),
	MYSQLND_METHOD(mysqlnd_conn, get_host_info),
	MYSQLND_METHOD(mysqlnd_conn, get_proto_info),
	MYSQLND_METHOD(mysqlnd_conn, info),
	MYSQLND_METHOD(mysqlnd_conn, charset_name),
	MYSQLND_METHOD(mysqlnd_conn, list_fields),
	MYSQLND_METHOD(mysqlnd_conn, list_method),

	MYSQLND_METHOD(mysqlnd_conn, insert_id),
	MYSQLND_METHOD(mysqlnd_conn, affected_rows),
	MYSQLND_METHOD(mysqlnd_conn, warning_count),
	MYSQLND_METHOD(mysqlnd_conn, field_count),

	MYSQLND_METHOD(mysqlnd_conn, set_server_option),
	MYSQLND_METHOD(mysqlnd_conn, set_client_option),
	MYSQLND_METHOD(mysqlnd_conn, free_contents),
	MYSQLND_METHOD(mysqlnd_conn, close),

	MYSQLND_METHOD_PRIVATE(mysqlnd_conn, dtor),

	MYSQLND_METHOD_PRIVATE(mysqlnd_conn, get_reference),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn, free_reference),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn, get_state),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn, set_state),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_init */
PHPAPI MYSQLND *_mysqlnd_init(zend_bool persistent TSRMLS_DC)
{
	MYSQLND *ret = mnd_pecalloc(1, sizeof(MYSQLND), persistent);

	DBG_ENTER("mysqlnd_init");
	DBG_INF_FMT("persistent=%d", persistent);

	SET_ERROR_AFF_ROWS(ret);
	ret->persistent = persistent;

	ret->m = & mysqlnd_mysqlnd_conn_methods;
	ret->m->get_reference(ret TSRMLS_CC);

#ifdef MYSQLND_THREADED
	ret->LOCK_state = tsrm_mutex_alloc();

	pthread_mutex_init(&ret->LOCK_work, NULL);
	pthread_cond_init(&ret->COND_work, NULL);
	pthread_cond_init(&ret->COND_work_done, NULL);
	pthread_cond_init(&ret->COND_thread_ended, NULL);
#endif

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
