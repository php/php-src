/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
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
#include "ext/standard/basic_functions.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/info.h"

#define MYSQLND_SILENT

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




static const char * mysqlnd_server_gone = "MySQL server has gone away";
const char * mysqlnd_out_of_sync = "Commands out of sync; you can't run this command now";

MYSQLND_STATS *mysqlnd_global_stats = NULL;
static zend_bool mysqlnd_library_initted = FALSE;


/* {{{ mysqlnd_library_init */
PHPAPI void mysqlnd_library_init()
{
	if (mysqlnd_library_initted == FALSE) {
		mysqlnd_library_initted = TRUE;
		_mysqlnd_init_ps_subsystem();
		mysqlnd_global_stats = calloc(1, sizeof(MYSQLND_STATS));
#ifdef ZTS
		mysqlnd_global_stats->LOCK_access = tsrm_mutex_alloc();
#endif
	}
}
/* }}} */


/* {{{ mysqlnd_library_end */
PHPAPI void mysqlnd_library_end()
{
	if (mysqlnd_library_initted == TRUE) {
#ifdef ZTS
		tsrm_mutex_free(mysqlnd_global_stats->LOCK_access);
#endif
		free(mysqlnd_global_stats);
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

	mysqlnd_local_infile_default(conn, TRUE);
	if (conn->current_result) {
		conn->current_result->m.free_result_contents(conn->current_result TSRMLS_CC);
		efree(conn->current_result);
		conn->current_result = NULL;
	}

	if (conn->net.stream) {
		php_stream_free(conn->net.stream, (pers) ? PHP_STREAM_FREE_RSRC_DTOR :
												   PHP_STREAM_FREE_CLOSE);
		conn->net.stream = NULL;
	}
	if (conn->host) {
		pefree(conn->host, pers);
		conn->host = NULL;
	}
	if (conn->user) {
		pefree(conn->user, pers);
		conn->user = NULL;
	}
	if (conn->passwd) {
		pefree(conn->passwd, pers);
		conn->passwd = NULL;
	}
	if (conn->unix_socket) {
		pefree(conn->unix_socket, pers);
		conn->unix_socket = NULL;
	}
	if (conn->scheme) {
		pefree(conn->scheme, pers);
		conn->scheme = NULL;
	}
	if (conn->server_version) {
		pefree(conn->server_version, pers);
		conn->server_version = NULL;
	}
	if (conn->host_info) {
		pefree(conn->host_info, pers);
		conn->host_info = NULL;
	}
	if (conn->scramble) {
		pefree(conn->scramble, pers);
		conn->scramble = NULL;
	}
	if (conn->last_message) {
		pefree(conn->last_message, pers);
		conn->last_message = NULL;
	}
	if (conn->options.charset_name) {
		pefree(conn->options.charset_name, pers);
		conn->options.charset_name = NULL;
	}
	if (conn->options.num_commands) {
		unsigned int i;
		for (i=0; i < conn->options.num_commands; i++) {
			pefree(conn->options.init_commands[i], pers);
		}
		pefree(conn->options.init_commands, pers);
		conn->options.init_commands = NULL;
	}
	if (conn->options.cfg_file) {
		pefree(conn->options.cfg_file, pers);
		conn->options.cfg_file = NULL;
	}
	if (conn->options.cfg_section) {
		pefree(conn->options.cfg_section, pers);
		conn->options.cfg_section = NULL;
	}
	if (conn->options.ssl_key) {
		pefree(conn->options.ssl_key, pers);
		conn->options.ssl_key = NULL;
	}
	if (conn->options.ssl_cert) {
		pefree(conn->options.ssl_cert, pers);
		conn->options.ssl_cert = NULL;
	}
	if (conn->options.ssl_ca) {
		pefree(conn->options.ssl_ca, pers);
		conn->options.ssl_ca = NULL;
	}
	if (conn->options.ssl_capath) {
		pefree(conn->options.ssl_capath, pers);
		conn->options.ssl_capath = NULL;
	}
	if (conn->options.ssl_cipher) {
		pefree(conn->options.ssl_cipher, pers);
		conn->options.ssl_cipher = NULL;
	}
	if (conn->zval_cache) {
		mysqlnd_palloc_free_thd_cache_reference(&conn->zval_cache);
		conn->zval_cache = NULL;
	}
	if (conn->qcache) {
		mysqlnd_qcache_free_cache_reference(&conn->qcache);
		conn->qcache = NULL;
	}
	if (conn->net.cmd_buffer.buffer) {
		pefree(conn->net.cmd_buffer.buffer, pers);
		conn->net.cmd_buffer.buffer = NULL;
	}
}
/* }}} */


/* {{{ mysqlnd_conn::dtor */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, dtor)(MYSQLND *conn TSRMLS_DC)
{
	conn->m->free_contents(conn TSRMLS_CC);

	pefree(conn, conn->persistent);
}
/* }}} */


/* {{{ mysqlnd_simple_command_handle_response */
enum_func_status
mysqlnd_simple_command_handle_response(MYSQLND *conn, enum php_mysql_packet_type ok_packet,
									   zend_bool silent, enum php_mysqlnd_server_command command
									   TSRMLS_DC)
{
	enum_func_status ret;
	switch (ok_packet) {
		case PROT_OK_PACKET:{
			php_mysql_packet_ok ok_response;
			PACKET_INIT_ALLOCA(ok_response, PROT_OK_PACKET);
			if (FAIL == (ret = PACKET_READ_ALLOCA(ok_response, conn))) {
				if (!silent) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading %s's OK packet",
									 mysqlnd_command_to_text[command]);
				}
			} else {
#ifndef MYSQLND_SILENT
				php_printf("\tOK from server\n");
#endif
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
									ok_response.message, ok_response.message_len);

					conn->upsert_status.warning_count = ok_response.warning_count;
					conn->upsert_status.server_status = ok_response.server_status;
					conn->upsert_status.affected_rows = ok_response.affected_rows;
					conn->upsert_status.last_insert_id = ok_response.last_insert_id;
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading %s's EOF packet",
								 	mysqlnd_command_to_text[command]);
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
									"EOF packet expected, field count wasn't 0xFE but 0x%2X",
									ok_response.field_count);
				}
			} else {
#ifndef MYSQLND_SILENT
				php_printf("\tOK from server\n");
#endif
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
	return ret;
}
/* }}} */


/* {{{ mysqlnd_simple_command */
enum_func_status
mysqlnd_simple_command(MYSQLND *conn, enum php_mysqlnd_server_command command,
					   const char * const arg, size_t arg_len,
					   enum php_mysql_packet_type ok_packet, zend_bool silent TSRMLS_DC)
{
	enum_func_status ret = PASS;
	php_mysql_packet_command cmd_packet;

	switch (conn->state) {
		case CONN_READY:
			break;
		case CONN_QUIT_SENT:
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			return FAIL;
		default:
			SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
							 mysqlnd_out_of_sync);
			return FAIL;
	}

	/* clean UPSERT info */
	memset(&conn->upsert_status, 0, sizeof(conn->upsert_status));
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
			php_error(E_WARNING, "Error while sending %s packet", mysqlnd_command_to_text[command]);
		}
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		ret = FAIL;
	} else if (ok_packet != PROT_LAST) {
		ret = mysqlnd_simple_command_handle_response(conn, ok_packet, silent, command TSRMLS_CC);
	}

	/*
	  There is no need to call FREE_ALLOCA on cmd_packet as the
	  only allocated string is cmd_packet.argument and it was passed
	  to us. We should not free it.
	*/

	return ret;
}
/* }}} */


/* {{{ mysqlnd_conn::set_server_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, set_server_option)(MYSQLND * const conn,
												enum_mysqlnd_server_option option TSRMLS_DC)
{
	char buffer[2];
	int2store(buffer, (uint) option);
	return mysqlnd_simple_command(conn, COM_SET_OPTION, buffer, sizeof(buffer),
								  PROT_EOF_PACKET, FALSE TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_start_psession */
PHPAPI void mysqlnd_restart_psession(MYSQLND *conn) 
{
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CONNECT_REUSED);
}
/* }}} */


/* {{{ mysqlnd_end_psession */
PHPAPI void mysqlnd_end_psession(MYSQLND *conn)
{

}
/* }}} */


/* {{{ mysqlnd_connect */
PHPAPI MYSQLND *mysqlnd_connect(MYSQLND *conn,
						 char *host, char *user,
						 char *passwd, unsigned int passwd_len,
						 char *db, unsigned int db_len,
						 unsigned int port,
						 char *socket,
						 unsigned int mysql_flags,
						 MYSQLND_THD_ZVAL_PCACHE *zval_cache
						 TSRMLS_DC)
{
	char *transport = NULL, *errstr = NULL;
	char *hashed_details = NULL;
	int transport_len, errcode = 0;
	unsigned int streams_options = ENFORCE_SAFE_MODE;
	unsigned int streams_flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
	zend_bool self_alloced = FALSE;
	struct timeval tv;
	zend_bool unix_socket = FALSE;
	const MYSQLND_CHARSET * charset;

	php_mysql_packet_greet greet_packet;
	php_mysql_packet_auth *auth_packet;
	php_mysql_packet_ok ok_packet;

	if (conn && conn->state != CONN_ALLOCED) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync);
		return NULL;
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
#ifndef PHP_WIN32
	if (!strncasecmp(host, "localhost", sizeof("localhost") - 1)) {
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

	if (conn->persistent) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		/* We should generate something unique */
		spprintf(&hashed_details, 0, "%s@%s@%s@%ld@%ld@%0.8F",
				 transport, user, db, tv.tv_sec, (long int)tv.tv_usec,
				 php_combined_lcg(TSRMLS_C) * 10); 
	} 

	PACKET_INIT_ALLOCA(greet_packet, PROT_GREET_PACKET);
	PACKET_INIT(auth_packet, PROT_AUTH_PACKET, php_mysql_packet_auth *);
	PACKET_INIT_ALLOCA(ok_packet, PROT_OK_PACKET);

	if (!conn) {
		conn = mysqlnd_init(FALSE);
		self_alloced = TRUE;
	}

	conn->state	= CONN_ALLOCED;
	conn->net.packet_no = 0;

	if (conn->options.timeout_connect) {
		tv.tv_sec = conn->options.timeout_connect;
		tv.tv_usec = 0;
	}
	if (conn->persistent) {
		conn->scheme = pestrndup(transport, transport_len, 1);
		efree(transport);
	} else {
		conn->scheme = transport;
	}
	conn->net.stream = php_stream_xport_create(conn->scheme, transport_len, streams_options, streams_flags,
											   hashed_details, 
											   (conn->options.timeout_connect) ? &tv : NULL,
											    NULL /*ctx*/, &errstr, &errcode);
	if (hashed_details) {
		efree(hashed_details);
	}
	if (errstr || !conn->net.stream) {
		goto err;
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
#ifndef MYSQLND_SILENT
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading greeting packet");
#endif
		goto err;
	} else if (greet_packet.error_no) {
		SET_CLIENT_ERROR(conn->error_info, greet_packet.error_no,
						 greet_packet.sqlstate, greet_packet.error);
		goto err;	
	} else if (greet_packet.pre41) {
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

	conn->scramble = auth_packet->server_scramble_buf = pemalloc(SCRAMBLE_LENGTH, conn->persistent);
	memcpy(auth_packet->server_scramble_buf, greet_packet.scramble_buf, SCRAMBLE_LENGTH);
	PACKET_WRITE(auth_packet, conn);

	if (FAIL == PACKET_READ_ALLOCA(ok_packet, conn) || ok_packet.field_count >= 0xFE) {
		if (ok_packet.field_count == 0xFE) {
			/* old authentication with new server  !*/
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mysqlnd cannot connect to MySQL 4.1+ using old authentication");
		} else if (ok_packet.field_count == 0xFF) {
			if (ok_packet.sqlstate[0]) {
				if (!self_alloced) {
					strncpy(conn->error_info.sqlstate, ok_packet.sqlstate, sizeof(conn->error_info.sqlstate));
				}
#ifndef MYSQLND_SILENT
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "ERROR:%d [SQLSTATE:%s] %s",
								 ok_packet.error_no, ok_packet.sqlstate, ok_packet.error);
#endif
			}
			if (!self_alloced) {
				conn->error_info.error_no = ok_packet.error_no;
				strncpy(conn->error_info.error, ok_packet.error, sizeof(conn->error_info.error));
			}
		}
	} else {
		conn->state				= CONN_READY;

		conn->user				= pestrdup(user, conn->persistent);
		conn->passwd			= pestrndup(passwd, passwd_len, conn->persistent);
		conn->port				= port;
		if (host && !socket) {
			char *p;

			conn->host = pestrdup(host, conn->persistent);
			spprintf(&p, 0, "MySQL host info: %s via TCP/IP", conn->host);
			if (conn->persistent) {
				conn->host_info = pestrdup(p, 1);
				efree(p);
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
						ok_packet.message, ok_packet.message_len);

		SET_EMPTY_ERROR(conn->error_info);

		PACKET_FREE_ALLOCA(greet_packet);
		PACKET_FREE(auth_packet);
		PACKET_FREE_ALLOCA(ok_packet);

		conn->zval_cache = mysqlnd_palloc_get_thd_cache_reference(zval_cache);
		conn->net.cmd_buffer.length = 128L*1024L;
		conn->net.cmd_buffer.buffer = pemalloc(conn->net.cmd_buffer.length, conn->persistent);

		MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CONNECT_SUCCESS);

		{
			uint as_unicode = 1;
			conn->m->set_client_option(conn, MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE,
									   (char *)&as_unicode);
		}

		return conn;
	}
err:
	PACKET_FREE_ALLOCA(greet_packet);
	PACKET_FREE(auth_packet);
	PACKET_FREE_ALLOCA(ok_packet);

	if (errstr) {
		SET_CLIENT_ERROR(conn->error_info, errcode, UNKNOWN_SQLSTATE, errstr);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%d] %.64s (trying to connect via %s)", errcode, errstr, conn->scheme);

		efree(errstr);
	}
	if (conn->scheme) {
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
	return NULL;
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

	if (PASS != mysqlnd_simple_command(conn, COM_QUERY, query, query_len,
									   PROT_LAST /* we will handle the OK packet*/,
									   FALSE TSRMLS_CC)) {
		return FAIL;
	}

	/*
	  Here read the result set. We don't do it in simple_command because it need
	  information from the ok packet. We will fetch it ourselves.
	*/
	ret = mysqlnd_query_read_result_set_header(conn, NULL TSRMLS_CC);

	return ret;
}
/* }}} */



/* {{{ mysqlnd_conn::list_method */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, list_method)(MYSQLND *conn, const char *query, char *achtung_wild, char *par1 TSRMLS_DC)
{
	char *show_query = NULL;
	size_t show_query_len;
	MYSQLND_RES *result = NULL;

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
		efree(show_query);
	}
	return result;
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
PHPAPI ulong mysqlnd_old_escape_string(char *newstr, const char *escapestr, int escapestr_len)
{
	return mysqlnd_cset_escape_slashes(mysqlnd_find_charset_name("latin1"),
									   newstr, escapestr, escapestr_len);
}
/* }}} */


/* {{{ mysqlnd_conn::escape_string */
static ulong
MYSQLND_METHOD(mysqlnd_conn, escape_string)(const MYSQLND * const conn, char *newstr, const char *escapestr, int escapestr_len)
{
	if (conn->upsert_status.server_status & SERVER_STATUS_NO_BACKSLASH_ESCAPES) {
		return mysqlnd_cset_escape_quotes(conn->charset, newstr, escapestr, escapestr_len);
	}
	return mysqlnd_cset_escape_slashes(conn->charset, newstr, escapestr, escapestr_len);
}
/* }}} */


/* {{{ mysqlnd_conn::dump_debug_info */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, dump_debug_info)(MYSQLND * const conn TSRMLS_DC)
{
	return mysqlnd_simple_command(conn, COM_DEBUG, NULL, 0, PROT_EOF_PACKET, FALSE TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_conn::select_db */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, select_db)(MYSQLND * const conn,
										const char * const db,
										unsigned int db_len TSRMLS_DC)
{
	enum_func_status ret;
	ret = mysqlnd_simple_command(conn, COM_INIT_DB, db, db_len, PROT_OK_PACKET, FALSE TSRMLS_CC);
	/*
	  The server sends 0 but libmysql doesn't read it and has established
	  a protocol of giving back -1. Thus we have to follow it :(
	*/
	SET_ERROR_AFF_ROWS(conn);

	return ret;
}
/* }}} */


/* {{{ mysqlnd_conn::ping */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, ping)(MYSQLND * const conn TSRMLS_DC)
{
	enum_func_status ret;
	ret = mysqlnd_simple_command(conn, COM_PING, NULL, 0, PROT_OK_PACKET, FALSE TSRMLS_CC);
	/*
	  The server sends 0 but libmysql doesn't read it and has established
	  a protocol of giving back -1. Thus we have to follow it :(
	*/
	SET_ERROR_AFF_ROWS(conn);

	return ret;
}
/* }}} */


/* {{{ mysqlnd_conn::stat */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, stat)(MYSQLND *conn, char **message, unsigned int * message_len TSRMLS_DC)
{
	enum_func_status ret;
	php_mysql_packet_stats stats_header;

	ret = mysqlnd_simple_command(conn, COM_STATISTICS, NULL, 0, PROT_LAST, FALSE TSRMLS_CC);
	if (FAIL == ret) {
		return FAIL;
	}
	PACKET_INIT_ALLOCA(stats_header, PROT_STATS_PACKET);
	if (FAIL == (ret = PACKET_READ_ALLOCA(stats_header, conn))) {
		return FAIL;
	}
	*message = stats_header.message;
	*message_len = stats_header.message_len;
	/* Ownership transfer */
	stats_header.message = NULL;
	PACKET_FREE_ALLOCA(stats_header);

	return PASS;
}
/* }}} */


/* {{{ mysqlnd_conn::kill */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, kill)(MYSQLND *conn, unsigned long pid TSRMLS_DC)
{
	enum_func_status ret;
	char buff[4];

	int4store(buff, pid);

	/* If we kill ourselves don't expect OK packet, PROT_LAST will skip it */
	if (pid != conn->thread_id) {
		ret = mysqlnd_simple_command(conn, COM_PROCESS_KILL, buff, 4, PROT_OK_PACKET, FALSE TSRMLS_CC);
		/*
		  The server sends 0 but libmysql doesn't read it and has established
		  a protocol of giving back -1. Thus we have to follow it :(
		*/
		SET_ERROR_AFF_ROWS(conn);
	} else if (PASS == (ret = mysqlnd_simple_command(conn, COM_PROCESS_KILL, buff,
													 4, PROT_LAST, FALSE TSRMLS_CC))) {
		conn->state = CONN_QUIT_SENT;
	}
	return ret;
}
/* }}} */


/* {{{ _mysqlnd_conn::set_charset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, set_charset)(MYSQLND * const conn, const char * const csname TSRMLS_DC)
{
	enum_func_status ret = PASS;
	char *query;
	size_t query_len;
	const MYSQLND_CHARSET * const charset = mysqlnd_find_charset_name(csname);

	if (!charset) {
		SET_CLIENT_ERROR(conn->error_info, CR_CANT_FIND_CHARSET, UNKNOWN_SQLSTATE,
						 "Invalid characterset or character set not supported");
		return FAIL;
	}

	query_len = spprintf(&query, 0, "SET NAMES %s", csname);

	if (FAIL == conn->m->query(conn, query, query_len TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error executing query");
	} else if (conn->error_info.error_no) {
		ret = FAIL;
	} else {
		conn->charset = charset;
	}
	efree(query);
	return ret;
}
/* }}} */


/* {{{ mysqlnd_conn::refresh */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, refresh)(MYSQLND * const conn, unsigned long options TSRMLS_DC)
{
	zend_uchar bits[1];
	int1store(bits, options);

	return mysqlnd_simple_command(conn, COM_REFRESH, (char *)bits, 1, PROT_OK_PACKET, FALSE TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_conn::shutdown */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, shutdown)(MYSQLND * const conn, unsigned long level TSRMLS_DC)
{
	zend_uchar bits[1];
	int1store(bits, level);

	return mysqlnd_simple_command(conn, COM_SHUTDOWN, (char *)bits, 1, PROT_OK_PACKET, FALSE TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_send_close */
static enum_func_status
mysqlnd_send_close(MYSQLND * conn TSRMLS_DC)
{
	enum_func_status ret = PASS;
	switch (conn->state) {
		case CONN_READY:
			ret =  mysqlnd_simple_command(conn, COM_QUIT, NULL, 0, PROT_LAST,
										  conn->tmp_int? TRUE : FALSE TSRMLS_CC);
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
			MYSQLND_INC_CONN_STATISTIC(NULL, STAT_CLOSE_IN_MIDDLE);
#ifndef MYSQLND_SILENT
			php_printf("Brutally closing connection [%p][%s]\n", conn, conn->scheme);
#endif
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
	conn->state = CONN_QUIT_SENT;

	return ret;
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

	MYSQLND_INC_CONN_STATISTIC(NULL, stat);

	mysqlnd_send_close(conn TSRMLS_CC);

	conn->m->free_reference(conn TSRMLS_CC);

	return ret;
}
/* }}} */


/* {{{ mysqlnd_conn::get_reference */
static MYSQLND *
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, get_reference)(MYSQLND * const conn)
{
	++conn->refcount;
	return conn;
}
/* }}} */


/* {{{ mysqlnd_conn::free_reference */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, free_reference)(MYSQLND * const conn TSRMLS_DC)
{
	if (!(--conn->refcount)) {
		/*
		  No multithreading issues as we don't share the connection :)
		  This will free the object too, of course because references has
		  reached zero.
		*/
		mysqlnd_send_close(conn TSRMLS_CC);
		conn->m->dtor(conn TSRMLS_CC);
	}
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
static mynd_ulonglong
MYSQLND_METHOD(mysqlnd_conn, insert_id)(const MYSQLND * const conn)
{
	return conn->upsert_status.last_insert_id;
}
/* }}} */


/* {{{ mysqlnd_conn::affected_rows */
static mynd_ulonglong
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
static mynd_ulonglong
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
MYSQLND_METHOD(mysqlnd_conn,more_results)(const MYSQLND * const conn)
{
	/* (conn->state == CONN_NEXT_RESULT_PENDING) too */
	return conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS? TRUE:FALSE;
}
/* }}} */


/* {{{ mysqlnd_conn::next_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, next_result)(MYSQLND * const conn TSRMLS_DC)
{
	enum_func_status ret;

	if (conn->state != CONN_NEXT_RESULT_PENDING) {
		return FAIL;
	}

	SET_EMPTY_ERROR(conn->error_info);
	conn->upsert_status.affected_rows= ~(mynd_ulonglong) 0;
	/*
	  We are sure that there is a result set, since conn->state is set accordingly
	  in mysqlnd_store_result() or mysqlnd_fetch_row_unbuffered()
	*/
	if (FAIL == (ret = mysqlnd_query_read_result_set_header(conn, NULL TSRMLS_CC))) {
#ifndef MYSQLND_SILENT
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Serious error");
#endif
		conn->state = CONN_QUIT_SENT;
	}

	return ret;
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
	user_len = strlen(user);
	memcpy(p, user, MIN(user_len, MYSQLND_MAX_ALLOWED_DB_LEN));
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
									   FALSE TSRMLS_CC)) {
		return FAIL;
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
		}
	}

	/*
	  Here we should close all statements. Unbuffered queries should not be a
	  problem as we won't allow sending COM_CHANGE_USER.
	*/

	return ret;
}
/* }}} */


/* {{{ mysqlnd_conn::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, set_client_option)(MYSQLND * const conn,
												enum mysqlnd_option option,
												const char * const value)
{
	switch (option) {
		case MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE:
			conn->options.numeric_and_datetime_as_unicode = *(uint*) value;
			break;
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
		case MYSQLND_OPT_INT_AND_YEAR_AS_INT:
			conn->options.int_and_year_as_int = *(uint*) value;
			break;
#endif
		case MYSQL_OPT_CONNECT_TIMEOUT:
			conn->options.timeout_connect = *(uint*) value;
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_READ_TIMEOUT:
			conn->options.timeout_read = *(uint*) value;
			break;
		case MYSQL_OPT_WRITE_TIMEOUT:
			conn->options.timeout_write = *(uint*) value;
			break;
#endif
		case MYSQL_OPT_LOCAL_INFILE:
			if (!value || (*(uint*) value) ? 1 : 0) {
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
			return FAIL;
	}
	return PASS;
}
/* }}} */


/* {{{ _mysqlnd_conn::use_result */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, use_result)(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND_RES *result;

	if (!conn->current_result) {
		return NULL;
	}

	/* Nothing to store for UPSERT/LOAD DATA */
	if (conn->last_query_type != QUERY_SELECT || conn->state != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync); 
		return NULL;
	}

	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_UNBUFFERED_SETS);

	result = conn->current_result;
	conn->current_result = NULL;
	result->conn = conn->m->get_reference(conn);

	return result->m.use_result(result, FALSE TSRMLS_CC);
}
/* }}} */


/* {{{ _mysqlnd_conn::store_result */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn, store_result)(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND_RES *result;

	if (!conn->current_result) {
		return NULL;
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (conn->last_query_type != QUERY_SELECT || conn->state != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync);
		return NULL;
	}

	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_BUFFERED_SETS);

	result = conn->current_result;
	conn->current_result = NULL;

	return result->m.store_result(result, conn, FALSE TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_conn::get_connection_stats */
void
MYSQLND_METHOD(mysqlnd_conn, get_connection_stats)(const MYSQLND * const conn,
												   zval *return_value
												   TSRMLS_DC ZEND_FILE_LINE_DC)
{
	mysqlnd_fill_stats_hash(&(conn->stats), return_value TSRMLS_CC ZEND_FILE_LINE_CC);
}
/* }}} */


MYSQLND_STMT * _mysqlnd_stmt_init(MYSQLND * const conn);


MYSQLND_CLASS_METHODS_START(mysqlnd_conn)
	MYSQLND_METHOD(mysqlnd_conn, escape_string),
	MYSQLND_METHOD(mysqlnd_conn, set_charset),
	MYSQLND_METHOD(mysqlnd_conn, query),
	MYSQLND_METHOD(mysqlnd_conn, use_result),
	MYSQLND_METHOD(mysqlnd_conn, store_result),
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
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_init */
PHPAPI MYSQLND *mysqlnd_init(zend_bool persistent)
{
	MYSQLND *ret = pecalloc(1, sizeof(MYSQLND), persistent);
	SET_ERROR_AFF_ROWS(ret);
	ret->persistent = persistent;

	ret->m = & mysqlnd_mysqlnd_conn_methods;
	ret->m->get_reference(ret);

	return ret;
}
/* }}} */


/* {{{ mysqlnd_functions[]
 *
 * Every user visible function must have an entry in mysqlnd_functions[].
 */
static zend_function_entry mysqlnd_functions[] = {
	{NULL, NULL, NULL}	/* Must be the last line in mysqlnd_functions[] */
};
/* }}} */


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mysqlnd)
{
	mysqlnd_library_init();
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mysqlnd)
{
	mysqlnd_library_end();
	return SUCCESS;
}
/* }}} */


/* {{{ mysqlnd_minfo_print_hash */
#if PHP_MAJOR_VERSION >= 6
PHPAPI void mysqlnd_minfo_print_hash(zval *values)
{
	zval **values_entry;
	HashPosition pos_values;

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos_values);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(values),
										(void **)&values_entry, &pos_values) == SUCCESS) {
		TSRMLS_FETCH();
		zstr	string_key;
		uint	string_key_len;
		ulong	num_key;
		char 	*s = NULL;

		zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &string_key, &string_key_len, &num_key, 0, &pos_values);

		convert_to_string(*values_entry);

		if (UG(unicode)) {
			int s_len;
			if (zend_unicode_to_string(ZEND_U_CONVERTER(UG(runtime_encoding_conv)),
									   &s, &s_len, string_key.u, string_key_len TSRMLS_CC) == SUCCESS) {
				php_info_print_table_row(2, s, Z_STRVAL_PP(values_entry));
			}
			if (s) {
				efree(s);
			}
		} else {
			php_info_print_table_row(2, string_key.s, Z_STRVAL_PP(values_entry));
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos_values);
	}
}
#else
void mysqlnd_minfo_print_hash(zval *values)
{
	zval **values_entry;
	HashPosition pos_values;

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos_values);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(values), (void **)&values_entry, &pos_values) == SUCCESS) {
		char	*string_key;
		uint	string_key_len;
		ulong	num_key;

		zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &string_key, &string_key_len, &num_key, 0, &pos_values);

		convert_to_string(*values_entry);
		php_info_print_table_row(2, string_key, Z_STRVAL_PP(values_entry));

		zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos_values);
	}
}
#endif
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mysqlnd)
{
	zval values;

	php_info_print_table_start();
	php_info_print_table_header(2, "mysqlnd", "enabled");
	php_info_print_table_row(2, "Version", mysqlnd_get_client_info());

	/* Print client stats */
	php_info_print_table_header(2, "Client statistics", "");
	mysqlnd_get_client_stats(&values);
	mysqlnd_minfo_print_hash(&values);
	zval_dtor(&values);
	php_info_print_table_end();
}
/* }}} */


/* {{{ mysqlnd_module_entry
 */
zend_module_entry mysqlnd_module_entry = {
 	STANDARD_MODULE_HEADER,
	"mysqlnd",
	mysqlnd_functions,
	PHP_MINIT(mysqlnd),
	PHP_MSHUTDOWN(mysqlnd),
	NULL,
	NULL,
	PHP_MINFO(mysqlnd),
	MYSQLND_VERSION,
	STANDARD_MODULE_PROPERTIES
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
