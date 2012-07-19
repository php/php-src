/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2012 The PHP Group                                |
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
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"

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



PHPAPI const char * const mysqlnd_old_passwd  = "mysqlnd cannot connect to MySQL 4.1+ using the old insecure authentication. "
"Please use an administration tool to reset your password with the command SET PASSWORD = PASSWORD('your_existing_password'). This will "
"store a new, and more secure, hash value in mysql.user. If this user is used in other scripts executed by PHP 5.2 or earlier you might need to remove the old-passwords "
"flag from your my.cnf file";

PHPAPI const char * const mysqlnd_server_gone = "MySQL server has gone away";
PHPAPI const char * const mysqlnd_out_of_sync = "Commands out of sync; you can't run this command now";
PHPAPI const char * const mysqlnd_out_of_memory = "Out of memory";

PHPAPI MYSQLND_STATS *mysqlnd_global_stats = NULL;


/* {{{ mysqlnd_conn_data::free_options */
static void
MYSQLND_METHOD(mysqlnd_conn_data, free_options)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	zend_bool pers = conn->persistent;

	if (conn->options->charset_name) {
		mnd_pefree(conn->options->charset_name, pers);
		conn->options->charset_name = NULL;
	}
	if (conn->options->auth_protocol) {
		mnd_pefree(conn->options->auth_protocol, pers);
		conn->options->auth_protocol = NULL;
	}
	if (conn->options->num_commands) {
		unsigned int i;
		for (i = 0; i < conn->options->num_commands; i++) {
			/* allocated with pestrdup */
			mnd_pefree(conn->options->init_commands[i], pers);
		}
		mnd_pefree(conn->options->init_commands, pers);
		conn->options->init_commands = NULL;
	}
	if (conn->options->cfg_file) {
		mnd_pefree(conn->options->cfg_file, pers);
		conn->options->cfg_file = NULL;
	}
	if (conn->options->cfg_section) {
		mnd_pefree(conn->options->cfg_section, pers);
		conn->options->cfg_section = NULL;
	}
}
/* }}} */


/* {{{ mysqlnd_conn_data::free_contents */
static void
MYSQLND_METHOD(mysqlnd_conn_data, free_contents)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	zend_bool pers = conn->persistent;

	DBG_ENTER("mysqlnd_conn_data::free_contents");

	mysqlnd_local_infile_default(conn);
	if (conn->current_result) {
		conn->current_result->m.free_result(conn->current_result, TRUE TSRMLS_CC);
		conn->current_result = NULL;
	}

	if (conn->net) {
		conn->net->m.free_contents(conn->net TSRMLS_CC);
	}

	DBG_INF("Freeing memory of members");

	if (conn->host) {
		mnd_pefree(conn->host, pers);
		conn->host = NULL;
	}
	if (conn->user) {
		mnd_pefree(conn->user, pers);
		conn->user = NULL;
	}
	if (conn->passwd) {
		mnd_pefree(conn->passwd, pers);
		conn->passwd = NULL;
	}
	if (conn->connect_or_select_db) {
		mnd_pefree(conn->connect_or_select_db, pers);
		conn->connect_or_select_db = NULL;
	}
	if (conn->unix_socket) {
		mnd_pefree(conn->unix_socket, pers);
		conn->unix_socket = NULL;
	}
	DBG_INF_FMT("scheme=%s", conn->scheme);
	if (conn->scheme) {
		mnd_pefree(conn->scheme, pers);
		conn->scheme = NULL;
	}
	if (conn->server_version) {
		mnd_pefree(conn->server_version, pers);
		conn->server_version = NULL;
	}
	if (conn->host_info) {
		mnd_pefree(conn->host_info, pers);
		conn->host_info = NULL;
	}
	if (conn->auth_plugin_data) {
		mnd_pefree(conn->auth_plugin_data, pers);
		conn->auth_plugin_data = NULL;
	}
	if (conn->last_message) {
		mnd_pefree(conn->last_message, pers);
		conn->last_message = NULL;
	}
	if (conn->error_info->error_list) {
		zend_llist_clean(conn->error_info->error_list);
		mnd_pefree(conn->error_info->error_list, pers);
		conn->error_info->error_list = NULL;
	}
	conn->charset = NULL;
	conn->greet_charset = NULL;

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::dtor */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, dtor)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::dtor");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	conn->m->free_contents(conn TSRMLS_CC);
	conn->m->free_options(conn TSRMLS_CC);

	if (conn->net) {
		mysqlnd_net_free(conn->net, conn->stats, conn->error_info TSRMLS_CC);
		conn->net = NULL;
	}

	if (conn->protocol) {
		mysqlnd_protocol_free(conn->protocol TSRMLS_CC);
		conn->protocol = NULL;
	}

	if (conn->stats) {
		mysqlnd_stats_end(conn->stats);
	}

	mnd_pefree(conn, conn->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::simple_command_handle_response */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, simple_command_handle_response)(MYSQLND_CONN_DATA * conn, enum mysqlnd_packet_type ok_packet,
															 zend_bool silent, enum php_mysqlnd_server_command command,
															 zend_bool ignore_upsert_status TSRMLS_DC)
{
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::simple_command_handle_response");
	DBG_INF_FMT("silent=%u packet=%u command=%s", silent, ok_packet, mysqlnd_command_to_text[command]);

	switch (ok_packet) {
		case PROT_OK_PACKET:{
			MYSQLND_PACKET_OK * ok_response = conn->protocol->m.get_ok_packet(conn->protocol, FALSE TSRMLS_CC);
			if (!ok_response) {
				SET_OOM_ERROR(*conn->error_info);
				break;
			}
			if (FAIL == (ret = PACKET_READ(ok_response, conn))) {
				if (!silent) {
					DBG_ERR_FMT("Error while reading %s's OK packet", mysqlnd_command_to_text[command]);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading %s's OK packet. PID=%u",
									 mysqlnd_command_to_text[command], getpid());
				}
			} else {
				DBG_INF_FMT("OK from server");
				if (0xFF == ok_response->field_count) {
					/* The server signalled error. Set the error */
					SET_CLIENT_ERROR(*conn->error_info, ok_response->error_no, ok_response->sqlstate, ok_response->error);
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
					conn->upsert_status->server_status &= ~SERVER_MORE_RESULTS_EXISTS;
					SET_ERROR_AFF_ROWS(conn);
				} else {
					SET_NEW_MESSAGE(conn->last_message, conn->last_message_len,
									ok_response->message, ok_response->message_len,
									conn->persistent);

					if (!ignore_upsert_status) {
						conn->upsert_status->warning_count = ok_response->warning_count;
						conn->upsert_status->server_status = ok_response->server_status;
						conn->upsert_status->affected_rows = ok_response->affected_rows;
						conn->upsert_status->last_insert_id = ok_response->last_insert_id;
					}
				}
			}
			PACKET_FREE(ok_response);
			break;
		}
		case PROT_EOF_PACKET:{
			MYSQLND_PACKET_EOF * ok_response = conn->protocol->m.get_eof_packet(conn->protocol, FALSE TSRMLS_CC);
			if (!ok_response) {
				SET_OOM_ERROR(*conn->error_info);
				break;
			}
			if (FAIL == (ret = PACKET_READ(ok_response, conn))) {
				SET_CLIENT_ERROR(*conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE,
								 "Malformed packet");
				if (!silent) {
					DBG_ERR_FMT("Error while reading %s's EOF packet", mysqlnd_command_to_text[command]);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading %s's EOF packet. PID=%d",
									 mysqlnd_command_to_text[command], getpid());
				}
			} else if (0xFF == ok_response->field_count) {
				/* The server signalled error. Set the error */
				SET_CLIENT_ERROR(*conn->error_info, ok_response->error_no, ok_response->sqlstate, ok_response->error);
				SET_ERROR_AFF_ROWS(conn);
			} else if (0xFE != ok_response->field_count) {
				SET_CLIENT_ERROR(*conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
				if (!silent) {
					DBG_ERR_FMT("EOF packet expected, field count wasn't 0xFE but 0x%2X", ok_response->field_count);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "EOF packet expected, field count wasn't 0xFE but 0x%2X",
									ok_response->field_count);
				}
			} else {
				DBG_INF_FMT("OK from server");
			}
			PACKET_FREE(ok_response);
			break;
		}
		default:
			SET_CLIENT_ERROR(*conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Wrong response packet %u passed to the function", ok_packet);
			break;
	}
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::simple_command */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, simple_command)(MYSQLND_CONN_DATA * conn, enum php_mysqlnd_server_command command,
			   const zend_uchar * const arg, size_t arg_len, enum mysqlnd_packet_type ok_packet, zend_bool silent,
			   zend_bool ignore_upsert_status TSRMLS_DC)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_COMMAND * cmd_packet;

	DBG_ENTER("mysqlnd_conn_data::simple_command");
	DBG_INF_FMT("command=%s ok_packet=%u silent=%u", mysqlnd_command_to_text[command], ok_packet, silent);

	switch (CONN_GET_STATE(conn)) {
		case CONN_READY:
			break;
		case CONN_QUIT_SENT:
			SET_CLIENT_ERROR(*conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			DBG_ERR("Server is gone");
			DBG_RETURN(FAIL);
		default:
			SET_CLIENT_ERROR(*conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
			DBG_ERR_FMT("Command out of sync. State=%u", CONN_GET_STATE(conn));
			DBG_RETURN(FAIL);
	}

	/* clean UPSERT info */
	if (!ignore_upsert_status) {
		memset(conn->upsert_status, 0, sizeof(*conn->upsert_status));
	}
	SET_ERROR_AFF_ROWS(conn);
	SET_EMPTY_ERROR(*conn->error_info);

	cmd_packet = conn->protocol->m.get_command_packet(conn->protocol, FALSE TSRMLS_CC);
	if (!cmd_packet) {
		SET_OOM_ERROR(*conn->error_info);
		DBG_RETURN(FAIL);
	}

	cmd_packet->command = command;
	if (arg && arg_len) {
		cmd_packet->argument = arg;
		cmd_packet->arg_len  = arg_len;
	}

	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_COM_QUIT + command - 1 /* because of COM_SLEEP */ );

	if (! PACKET_WRITE(cmd_packet, conn)) {
		if (!silent) {
			DBG_ERR_FMT("Error while sending %s packet", mysqlnd_command_to_text[command]);
			php_error(E_WARNING, "Error while sending %s packet. PID=%d", mysqlnd_command_to_text[command], getpid());
		}
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
		DBG_ERR("Server is gone");
		ret = FAIL;
	} else if (ok_packet != PROT_LAST) {
		ret = conn->m->simple_command_handle_response(conn, ok_packet, silent, command, ignore_upsert_status TSRMLS_CC);
	}

	PACKET_FREE(cmd_packet);
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_server_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_server_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_server_option option TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, set_server_option);
	zend_uchar buffer[2];
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::set_server_option");
	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {

		int2store(buffer, (unsigned int) option);
		ret = conn->m->simple_command(conn, COM_SET_OPTION, buffer, sizeof(buffer), PROT_EOF_PACKET, FALSE, TRUE TSRMLS_CC);
	
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::restart_psession */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, restart_psession)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::restart_psession");
	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CONNECT_REUSED);
	/* Free here what should not be seen by the next script */
	if (conn->last_message) {
		mnd_pefree(conn->last_message, conn->persistent);
		conn->last_message = NULL;
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_conn_data::end_psession */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, end_psession)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::end_psession");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_switch_to_ssl_if_needed */
static enum_func_status
mysqlnd_switch_to_ssl_if_needed(
			MYSQLND_CONN_DATA * conn,
			const MYSQLND_PACKET_GREET * const greet_packet,
			const MYSQLND_OPTIONS * const options,
			unsigned long mysql_flags
			TSRMLS_DC
		)
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * charset;
	MYSQLND_PACKET_AUTH * auth_packet;
	DBG_ENTER("mysqlnd_switch_to_ssl_if_needed");

	auth_packet = conn->protocol->m.get_auth_packet(conn->protocol, FALSE TSRMLS_CC);
	if (!auth_packet) {
		SET_OOM_ERROR(*conn->error_info);
		goto end;
	}
	auth_packet->client_flags = mysql_flags;
	auth_packet->max_packet_size = MYSQLND_ASSEMBLED_PACKET_MAX_SIZE;

	if (options->charset_name && (charset = mysqlnd_find_charset_name(options->charset_name))) {
		auth_packet->charset_no	= charset->nr;
	} else {
#if MYSQLND_UNICODE
		auth_packet->charset_no	= 200;/* utf8 - swedish collation, check mysqlnd_charset.c */
#else
		auth_packet->charset_no	= greet_packet->charset_no;
#endif
	}

#ifdef MYSQLND_SSL_SUPPORTED
	if ((greet_packet->server_capabilities & CLIENT_SSL) && (mysql_flags & CLIENT_SSL)) {
		zend_bool verify = mysql_flags & CLIENT_SSL_VERIFY_SERVER_CERT? TRUE:FALSE;
		DBG_INF("Switching to SSL");
		if (!PACKET_WRITE(auth_packet, conn)) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
			SET_CLIENT_ERROR(*conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			goto end;
		}

		conn->net->m.set_client_option(conn->net, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, (const char *) &verify TSRMLS_CC);

		if (FAIL == conn->net->m.enable_ssl(conn->net TSRMLS_CC)) {
			goto end;
		}
	}
#endif
	ret = PASS;
end:
	PACKET_FREE(auth_packet);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_connect_run_authentication */
static enum_func_status
mysqlnd_connect_run_authentication(
			MYSQLND_CONN_DATA * conn,
			const char * const user,
			const char * const passwd,
			const char * const db,
			size_t db_len,
			size_t passwd_len,
			const MYSQLND_PACKET_GREET * const greet_packet,
			const MYSQLND_OPTIONS * const options,
			unsigned long mysql_flags
			TSRMLS_DC)
{
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_connect_run_authentication");

	ret = mysqlnd_switch_to_ssl_if_needed(conn, greet_packet, options, mysql_flags TSRMLS_CC);
	if (PASS == ret) {
		zend_bool first_call = TRUE;

		char * switch_to_auth_protocol = NULL;
		size_t switch_to_auth_protocol_len = 0;
		char * requested_protocol = NULL;
		zend_uchar * plugin_data;
		size_t plugin_data_len;

		plugin_data_len = greet_packet->auth_plugin_data_len;
		plugin_data = mnd_emalloc(plugin_data_len + 1);
		if (!plugin_data) {
			ret = FAIL;
			goto end;
		}
		memcpy(plugin_data, greet_packet->auth_plugin_data, plugin_data_len);
		plugin_data[plugin_data_len] = '\0';

		requested_protocol = mnd_pestrdup(greet_packet->auth_protocol? greet_packet->auth_protocol: "mysql_native_password", FALSE);
		if (!requested_protocol) {
			ret = FAIL;
			goto end;
		}

		do {
			struct st_mysqlnd_authentication_plugin * auth_plugin;
			{
				char * plugin_name = NULL;

				mnd_sprintf(&plugin_name, 0, "auth_plugin_%s", requested_protocol);

				DBG_INF_FMT("looking for %s auth plugin", plugin_name);
				auth_plugin = mysqlnd_plugin_find(plugin_name);
				mnd_sprintf_free(plugin_name);

				if (!auth_plugin) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "The server requested authentication method unknown to the client [%s]", requested_protocol);
					SET_CLIENT_ERROR(*conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "The server requested authentication method umknown to the client");
					break;
				}
			}
			DBG_INF("plugin found");

			{
				zend_uchar * switch_to_auth_protocol_data = NULL;
				size_t switch_to_auth_protocol_data_len = 0;
				zend_uchar * scrambled_data = NULL;
				size_t scrambled_data_len = 0;

				switch_to_auth_protocol = NULL;
				switch_to_auth_protocol_len = 0;

				if (conn->auth_plugin_data) {
					mnd_pefree(conn->auth_plugin_data, conn->persistent);
					conn->auth_plugin_data = NULL;
				}
				conn->auth_plugin_data_len = plugin_data_len;
				conn->auth_plugin_data = mnd_pemalloc(conn->auth_plugin_data_len, conn->persistent);
				if (!conn->auth_plugin_data) {
					SET_OOM_ERROR(*conn->error_info);
					goto end;
				}
				memcpy(conn->auth_plugin_data, plugin_data, plugin_data_len);

				DBG_INF_FMT("salt=[%*s]", plugin_data_len - 1, plugin_data);
				/* The data should be allocated with malloc() */
				scrambled_data =
					auth_plugin->methods.get_auth_data(NULL, &scrambled_data_len, conn, user, passwd, passwd_len,
													   plugin_data, plugin_data_len, options, mysql_flags TSRMLS_CC);


				ret = mysqlnd_auth_handshake(conn, user, passwd, passwd_len, db, db_len, options, mysql_flags,
											greet_packet->charset_no,
											first_call,
											requested_protocol,
											scrambled_data, scrambled_data_len,
											&switch_to_auth_protocol, &switch_to_auth_protocol_len,
											&switch_to_auth_protocol_data, &switch_to_auth_protocol_data_len
											TSRMLS_CC);
				first_call = FALSE;
				free(scrambled_data);

				DBG_INF_FMT("switch_to_auth_protocol=%s", switch_to_auth_protocol? switch_to_auth_protocol:"n/a");
				if (requested_protocol && switch_to_auth_protocol) {
					mnd_efree(requested_protocol);
					requested_protocol = switch_to_auth_protocol;
				}

				if (plugin_data) {
					mnd_efree(plugin_data);
				}
				plugin_data_len = switch_to_auth_protocol_data_len;
				plugin_data = switch_to_auth_protocol_data;
			}
			DBG_INF_FMT("conn->error_info->error_no = %d", conn->error_info->error_no);
		} while (ret == FAIL && conn->error_info->error_no == 0 && switch_to_auth_protocol != NULL);
		if (plugin_data) {
			mnd_efree(plugin_data);
		}
		
		if (ret == PASS) {
			DBG_INF_FMT("saving requested_protocol=%s", requested_protocol);
			conn->m->set_client_option(conn, MYSQLND_OPT_AUTH_PROTOCOL, requested_protocol TSRMLS_CC);
		}

		if (requested_protocol) {
			mnd_efree(requested_protocol);
		}
	}
end:
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, connect)(MYSQLND_CONN_DATA * conn,
						 const char *host, const char *user,
						 const char *passwd, unsigned int passwd_len,
						 const char *db, unsigned int db_len,
						 unsigned int port,
						 const char *socket_or_pipe,
						 unsigned int mysql_flags
						 TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, connect);
	size_t host_len;
	zend_bool unix_socket = FALSE;
	zend_bool named_pipe = FALSE;
	zend_bool reconnect = FALSE;
	zend_bool saved_compression = FALSE;
	zend_bool local_tx_started = FALSE;

	MYSQLND_PACKET_GREET * greet_packet = NULL;

	DBG_ENTER("mysqlnd_conn_data::connect");

	if (PASS != conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		goto err;
	}
	local_tx_started = TRUE;

	SET_EMPTY_ERROR(*conn->error_info);
	SET_ERROR_AFF_ROWS(conn);

	DBG_INF_FMT("host=%s user=%s db=%s port=%u flags=%u persistent=%u state=%u",
				host?host:"", user?user:"", db?db:"", port, mysql_flags,
				conn? conn->persistent:0, conn? CONN_GET_STATE(conn):-1);

	if (CONN_GET_STATE(conn) > CONN_ALLOCED && CONN_GET_STATE(conn) ) {
		DBG_INF("Connecting on a connected handle.");

		if (CONN_GET_STATE(conn) < CONN_QUIT_SENT) {
			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CLOSE_IMPLICIT);
			reconnect = TRUE;
			conn->m->send_close(conn TSRMLS_CC);
		}

		conn->m->free_contents(conn TSRMLS_CC);
		MYSQLND_DEC_CONN_STATISTIC(conn->stats, STAT_OPENED_CONNECTIONS);
		if (conn->persistent) {
			MYSQLND_DEC_CONN_STATISTIC(conn->stats, STAT_OPENED_PERSISTENT_CONNECTIONS);
		}
		/* Now reconnect using the same handle */
		if (conn->net->compressed) {
			/*
			  we need to save the state. As we will re-connect, net->compressed should be off, or
			  we will look for a compression header as part of the greet message, but there will
			  be none.
			*/
			saved_compression = TRUE;
			conn->net->compressed = FALSE;
		}
	} else {
		unsigned int max_allowed_size = MYSQLND_ASSEMBLED_PACKET_MAX_SIZE;
		conn->m->set_client_option(conn, MYSQLND_OPT_MAX_ALLOWED_PACKET, (char *)&max_allowed_size TSRMLS_CC);
	}

	if (!host || !host[0]) {
		host = "localhost";
	}
	if (!user) {
		DBG_INF_FMT("no user given, using empty string");
		user = "";
	}
	if (!passwd) {
		DBG_INF_FMT("no password given, using empty string");
		passwd = "";
		passwd_len = 0;
	}
	if (!db) {
		DBG_INF_FMT("no db given, using empty string");
		db = "";
		db_len = 0;
	}

	host_len = strlen(host);
	{
		char * transport = NULL;
		int transport_len;
#ifndef PHP_WIN32
		if (host_len == sizeof("localhost") - 1 && !strncasecmp(host, "localhost", host_len)) {
			DBG_INF_FMT("socket=%s", socket_or_pipe? socket_or_pipe:"n/a");
			if (!socket_or_pipe) {
				socket_or_pipe = "/tmp/mysql.sock";
			}
			transport_len = mnd_sprintf(&transport, 0, "unix://%s", socket_or_pipe);
			unix_socket = TRUE;
#else
		if (host_len == sizeof(".") - 1 && host[0] == '.') {
			/* named pipe in socket */
			if (!socket_or_pipe) {
				socket_or_pipe = "\\\\.\\pipe\\MySQL";
			}
			transport_len = mnd_sprintf(&transport, 0, "pipe://%s", socket_or_pipe);
			named_pipe = TRUE;
#endif
		} else {
			if (!port) {
				port = 3306;
			}
			transport_len = mnd_sprintf(&transport, 0, "tcp://%s:%u", host, port);
		}
		if (!transport) {
			SET_OOM_ERROR(*conn->error_info);
			goto err; /* OOM */
		}
		DBG_INF_FMT("transport=%s conn->scheme=%s", transport, conn->scheme);
		conn->scheme = mnd_pestrndup(transport, transport_len, conn->persistent);
		conn->scheme_len = transport_len;
		mnd_sprintf_free(transport);
		transport = NULL;
		if (!conn->scheme) {
			goto err; /* OOM */
		}
	}

	greet_packet = conn->protocol->m.get_greet_packet(conn->protocol, FALSE TSRMLS_CC);
	if (!greet_packet) {
		SET_OOM_ERROR(*conn->error_info);
		goto err; /* OOM */
	}

	if (FAIL == conn->net->m.connect_ex(conn->net, conn->scheme, conn->scheme_len, conn->persistent,
										conn->stats, conn->error_info TSRMLS_CC))
	{
		goto err;
	}

	DBG_INF_FMT("stream=%p", conn->net->stream);

	if (FAIL == PACKET_READ(greet_packet, conn)) {
		DBG_ERR("Error while reading greeting packet");
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading greeting packet. PID=%d", getpid());
		goto err;
	} else if (greet_packet->error_no) {
		DBG_ERR_FMT("errorno=%u error=%s", greet_packet->error_no, greet_packet->error);
		SET_CLIENT_ERROR(*conn->error_info, greet_packet->error_no, greet_packet->sqlstate, greet_packet->error);
		goto err;
	} else if (greet_packet->pre41) {
		DBG_ERR_FMT("Connecting to 3.22, 3.23 & 4.0 is not supported. Server is %-.32s", greet_packet->server_version);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connecting to 3.22, 3.23 & 4.0 "
						" is not supported. Server is %-.32s", greet_packet->server_version);
		SET_CLIENT_ERROR(*conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE,
						 "Connecting to 3.22, 3.23 & 4.0 servers is not supported");
		goto err;
	}

	conn->thread_id			= greet_packet->thread_id;
	conn->protocol_version	= greet_packet->protocol_version;
	conn->server_version	= mnd_pestrdup(greet_packet->server_version, conn->persistent);

	conn->greet_charset = mysqlnd_find_charset_nr(greet_packet->charset_no);
	if (!conn->greet_charset) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Server sent charset (%d) unknown to the client. Please, report to the developers", greet_packet->charset_no);
		SET_CLIENT_ERROR(*conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE,
			"Server sent charset unknown to the client. Please, report to the developers");
		goto err;
	}
	/* we allow load data local infile by default */
	mysql_flags |= MYSQLND_CAPABILITIES;

	if (db) {
		mysql_flags |= CLIENT_CONNECT_WITH_DB;
	}

	if (PG(open_basedir) && strlen(PG(open_basedir))) {
		mysql_flags ^= CLIENT_LOCAL_FILES;
	}

#ifndef MYSQLND_COMPRESSION_ENABLED
	if (mysql_flags & CLIENT_COMPRESS) {
		mysql_flags &= ~CLIENT_COMPRESS;
	}
#else
	if (conn->net->options.flags & MYSQLND_NET_FLAG_USE_COMPRESSION) {
		mysql_flags |= CLIENT_COMPRESS;
	}
#endif
#ifndef MYSQLND_SSL_SUPPORTED
	if (mysql_flags & CLIENT_SSL) {
		mysql_flags &= ~CLIENT_SSL;
	}
#else
	if (conn->net->options.ssl_key || conn->net->options.ssl_cert ||
		conn->net->options.ssl_ca || conn->net->options.ssl_capath || conn->net->options.ssl_cipher)
	{
		mysql_flags |= CLIENT_SSL;
	}
#endif

	if (FAIL == mysqlnd_connect_run_authentication(conn, user, passwd, db, db_len, (size_t) passwd_len,
												   greet_packet, conn->options, mysql_flags TSRMLS_CC))
	{
		goto err;
	}

	{
		CONN_SET_STATE(conn, CONN_READY);

		if (saved_compression) {
			conn->net->compressed = TRUE;
		}
		/*
		  If a connect on a existing handle is performed and mysql_flags is
		  passed which doesn't CLIENT_COMPRESS, then we need to overwrite the value
		  which we set based on saved_compression.
		*/
		conn->net->compressed = mysql_flags & CLIENT_COMPRESS? TRUE:FALSE;

		conn->user				= mnd_pestrdup(user, conn->persistent);
		conn->user_len			= strlen(conn->user);
		conn->passwd			= mnd_pestrndup(passwd, passwd_len, conn->persistent);
		conn->passwd_len		= passwd_len;
		conn->port				= port;
		conn->connect_or_select_db = mnd_pestrndup(db, db_len, conn->persistent);
		conn->connect_or_select_db_len = db_len;

		if (!conn->user || !conn->passwd || !conn->connect_or_select_db) {
			SET_OOM_ERROR(*conn->error_info);
			goto err; /* OOM */
		}

		if (!unix_socket && !named_pipe) {
			conn->host = mnd_pestrdup(host, conn->persistent);
			if (!conn->host) {
				SET_OOM_ERROR(*conn->error_info);
				goto err; /* OOM */
			}
			conn->host_len = strlen(conn->host);
			{
				char *p;
				mnd_sprintf(&p, 0, "%s via TCP/IP", conn->host);
				if (!p) {
					SET_OOM_ERROR(*conn->error_info);
					goto err; /* OOM */
				}
				conn->host_info =  mnd_pestrdup(p, conn->persistent);
				mnd_sprintf_free(p);
				if (!conn->host_info) {
					SET_OOM_ERROR(*conn->error_info);
					goto err; /* OOM */
				}
			}
		} else {
			conn->unix_socket = mnd_pestrdup(socket_or_pipe, conn->persistent);
			if (unix_socket) {
				conn->host_info = mnd_pestrdup("Localhost via UNIX socket", conn->persistent);
			} else if (named_pipe) {
				char *p;
				mnd_sprintf(&p, 0, "%s via named pipe", conn->unix_socket);
				if (!p) {
					SET_OOM_ERROR(*conn->error_info);
					goto err; /* OOM */
				}
				conn->host_info =  mnd_pestrdup(p, conn->persistent);
				mnd_sprintf_free(p);
				if (!conn->host_info) {
					SET_OOM_ERROR(*conn->error_info);
					goto err; /* OOM */
				}
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Impossible. Should be either socket or a pipe. Report a bug!");
			}
			if (!conn->unix_socket || !conn->host_info) {
				SET_OOM_ERROR(*conn->error_info);
				goto err; /* OOM */
			}
			conn->unix_socket_len = strlen(conn->unix_socket);
		}
		conn->client_flag		= mysql_flags;
		conn->max_packet_size	= MYSQLND_ASSEMBLED_PACKET_MAX_SIZE;
		/* todo: check if charset is available */
		conn->server_capabilities = greet_packet->server_capabilities;
		conn->upsert_status->warning_count = 0;
		conn->upsert_status->server_status = greet_packet->server_status;
		conn->upsert_status->affected_rows = 0;

		SET_EMPTY_ERROR(*conn->error_info);

		mysqlnd_local_infile_default(conn);

#if MYSQLND_UNICODE
		{
			unsigned int as_unicode = 1;
			conn->m->set_client_option(conn, MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE, (char *)&as_unicode TSRMLS_CC);
			DBG_INF("unicode set");
		}
#endif
		if (conn->options->init_commands) {
			unsigned int current_command = 0;
			for (; current_command < conn->options->num_commands; ++current_command) {
				const char * const command = conn->options->init_commands[current_command];
				if (command) {
					MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_INIT_COMMAND_EXECUTED_COUNT);
					if (PASS != conn->m->query(conn, command, strlen(command) TSRMLS_CC)) {
						MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_INIT_COMMAND_FAILED_COUNT);
						goto err;
					}
					if (conn->last_query_type == QUERY_SELECT) {
						MYSQLND_RES * result = conn->m->use_result(conn TSRMLS_CC);
						if (result) {
							result->m.free_result(result, TRUE TSRMLS_CC);
						}
					}
				}
			}
		}


		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats, STAT_CONNECT_SUCCESS, 1, STAT_OPENED_CONNECTIONS, 1);
		if (reconnect) {
			MYSQLND_INC_GLOBAL_STATISTIC(STAT_RECONNECT);
		}
		if (conn->persistent) {
			MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats, STAT_PCONNECT_SUCCESS, 1, STAT_OPENED_PERSISTENT_CONNECTIONS, 1);
		}

		DBG_INF_FMT("connection_id=%llu", conn->thread_id);

		PACKET_FREE(greet_packet);

		conn->m->local_tx_end(conn, this_func, PASS TSRMLS_CC);
		DBG_RETURN(PASS);
	}
err:
	PACKET_FREE(greet_packet);

	DBG_ERR_FMT("[%u] %.128s (trying to connect via %s)", conn->error_info->error_no, conn->error_info->error, conn->scheme);
	if (!conn->error_info->error_no) {
		SET_CLIENT_ERROR(*conn->error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, conn->error_info->error? conn->error_info->error:"Unknown error");
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "[%u] %.128s (trying to connect via %s)",
						 conn->error_info->error_no, conn->error_info->error, conn->scheme);
	}

	conn->m->free_contents(conn TSRMLS_CC);
	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CONNECT_FAILURE);
	if (TRUE == local_tx_started) {
		conn->m->local_tx_end(conn, this_func, FAIL TSRMLS_CC);
	}

	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_conn::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, connect)(MYSQLND * conn_handle,
						 const char * host, const char * user,
						 const char * passwd, unsigned int passwd_len,
						 const char * db, unsigned int db_len,
						 unsigned int port,
						 const char * socket_or_pipe,
						 unsigned int mysql_flags
						 TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, connect);
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = conn_handle->data;

	DBG_ENTER("mysqlnd_conn::connect");

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->connect(conn, host, user, passwd, passwd_len, db, db_len, port, socket_or_pipe, mysql_flags TSRMLS_CC);

		conn->m->local_tx_end(conn, this_func, FAIL TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */

/* {{{ mysqlnd_connect */
PHPAPI MYSQLND * mysqlnd_connect(MYSQLND * conn_handle,
						 const char * host, const char * user,
						 const char * passwd, unsigned int passwd_len,
						 const char * db, unsigned int db_len,
						 unsigned int port,
						 const char * socket_or_pipe,
						 unsigned int mysql_flags
						 TSRMLS_DC)
{
	enum_func_status ret = FAIL;
	zend_bool self_alloced = FALSE;

	DBG_ENTER("mysqlnd_connect");
	DBG_INF_FMT("host=%s user=%s db=%s port=%u flags=%u", host?host:"", user?user:"", db?db:"", port, mysql_flags);

	if (!conn_handle) {
		self_alloced = TRUE;
		if (!(conn_handle = mysqlnd_init(FALSE))) {
			/* OOM */
			DBG_RETURN(NULL);
		}
	}

	ret = conn_handle->m->connect(conn_handle, host, user, passwd, passwd_len, db, db_len, port, socket_or_pipe, mysql_flags TSRMLS_CC);

	if (ret == FAIL) {
		if (self_alloced) {
			/*
			  We have alloced, thus there are no references to this
			  object - we are free to kill it!
			*/
			conn_handle->m->dtor(conn_handle TSRMLS_CC);
		}
		DBG_RETURN(NULL);
	}
	DBG_RETURN(conn_handle);
}
/* }}} */


/* {{{ mysqlnd_conn_data::query */
/*
  If conn->error_info->error_no is not zero, then we had an error.
  Still the result from the query is PASS
*/
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, query)(MYSQLND_CONN_DATA * conn, const char * query, unsigned int query_len TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, query);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::query");
	DBG_INF_FMT("conn=%llu query=%s", conn->thread_id, query);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		if (PASS == conn->m->send_query(conn, query, query_len TSRMLS_CC) &&
			PASS == conn->m->reap_query(conn TSRMLS_CC))
		{
			ret = PASS;
			if (conn->last_query_type == QUERY_UPSERT && conn->upsert_status->affected_rows) {
				MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats, STAT_ROWS_AFFECTED_NORMAL, conn->upsert_status->affected_rows);
			}
		}
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::send_query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, send_query)(MYSQLND_CONN_DATA * conn, const char * query, unsigned int query_len TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, send_query);
	enum_func_status ret;
	DBG_ENTER("mysqlnd_conn_data::send_query");
	DBG_INF_FMT("conn=%llu query=%s", conn->thread_id, query);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->simple_command(conn, COM_QUERY, (zend_uchar *) query, query_len,
											 PROT_LAST /* we will handle the OK packet*/,
											 FALSE, FALSE TSRMLS_CC);
		if (PASS == ret) {
			CONN_SET_STATE(conn, CONN_QUERY_SENT);
		}
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::reap_query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, reap_query)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, reap_query);
	enum_mysqlnd_connection_state state = CONN_GET_STATE(conn);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::reap_query");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		if (state <= CONN_READY || state == CONN_QUIT_SENT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connection not opened, clear or has been closed");
			DBG_ERR_FMT("Connection not opened, clear or has been closed. State=%u", state);
			DBG_RETURN(ret);
		}
		ret = conn->m->query_read_result_set_header(conn, NULL TSRMLS_CC);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


#include "php_network.h"

MYSQLND ** mysqlnd_stream_array_check_for_readiness(MYSQLND ** conn_array TSRMLS_DC)
{
	int cnt = 0;
	MYSQLND **p = conn_array, **p_p;
	MYSQLND **ret = NULL;

	while (*p) {
		if (CONN_GET_STATE((*p)->data) <= CONN_READY || CONN_GET_STATE((*p)->data) == CONN_QUIT_SENT) {
			cnt++;
		}
		p++;
	}
	if (cnt) {
		MYSQLND **ret_p = ret = ecalloc(cnt + 1, sizeof(MYSQLND *));
		p_p = p = conn_array;
		while (*p) {
			if (CONN_GET_STATE((*p)->data) <= CONN_READY || CONN_GET_STATE((*p)->data) == CONN_QUIT_SENT) {
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
static int mysqlnd_stream_array_to_fd_set(MYSQLND ** conn_array, fd_set * fds, php_socket_t * max_fd TSRMLS_DC)
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
		if (SUCCESS == php_stream_cast((*p)->data->net->stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
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

static int mysqlnd_stream_array_from_fd_set(MYSQLND ** conn_array, fd_set * fds TSRMLS_DC)
{
	php_socket_t this_fd;
	int ret = 0;
	zend_bool disproportion = FALSE;


	MYSQLND **fwd = conn_array, **bckwd = conn_array;

	while (*fwd) {
		if (SUCCESS == php_stream_cast((*fwd)->data->net->stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
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
/* }}} */


#ifndef PHP_WIN32
#define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#else
#include "win32/select.h"
#endif


/* {{{ _mysqlnd_poll */
PHPAPI enum_func_status
_mysqlnd_poll(MYSQLND **r_array, MYSQLND **e_array, MYSQLND ***dont_poll, long sec, long usec, uint * desc_num TSRMLS_DC)
{
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	php_socket_t	max_fd = 0;
	int				retval, sets = 0;
	int				set_count, max_set_count = 0;

	DBG_ENTER("_mysqlnd_poll");
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
		DBG_ERR_FMT(*dont_poll ? "All arrays passed are clear":"No stream arrays were passed");
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

/* {{{ mysqlnd_conn_data::list_fields */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, list_fields)(MYSQLND_CONN_DATA * conn, const char *table, const char *achtung_wild TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, list_fields);
	/* db + \0 + wild + \0 (for wild) */
	zend_uchar buff[MYSQLND_MAX_ALLOWED_DB_LEN * 2 + 1 + 1], *p;
	size_t table_len, wild_len;
	MYSQLND_RES * result = NULL;
	DBG_ENTER("mysqlnd_conn_data::list_fields");
	DBG_INF_FMT("conn=%llu table=%s wild=%s", conn->thread_id, table? table:"",achtung_wild? achtung_wild:"");

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		do {
			p = buff;
			if (table && (table_len = strlen(table))) {
				size_t to_copy = MIN(table_len, MYSQLND_MAX_ALLOWED_DB_LEN);
				memcpy(p, table, to_copy);
				p += to_copy;
				*p++ = '\0';
			}

			if (achtung_wild && (wild_len = strlen(achtung_wild))) {
				size_t to_copy = MIN(wild_len, MYSQLND_MAX_ALLOWED_DB_LEN);
				memcpy(p, achtung_wild, to_copy);
				p += to_copy;
				*p++ = '\0';
			}

			if (PASS != conn->m->simple_command(conn, COM_FIELD_LIST, buff, p - buff,
											   PROT_LAST /* we will handle the OK packet*/,
											   FALSE, TRUE TSRMLS_CC)) {
				conn->m->local_tx_end(conn, 0, FAIL TSRMLS_CC);
				break;
			}

			/*
			   Prepare for the worst case.
			   MyISAM goes to 2500 BIT columns, double it for safety.
			*/
			result = conn->m->result_init(5000, conn->persistent TSRMLS_CC);
			if (!result) {
				break;
			}

			if (FAIL == result->m.read_result_metadata(result, conn TSRMLS_CC)) {
				DBG_ERR("Error ocurred while reading metadata");
				result->m.free_result(result, TRUE TSRMLS_CC);
				result = NULL;
				break;
			}

			result->type = MYSQLND_RES_NORMAL;
			result->m.fetch_row = result->m.fetch_row_normal_unbuffered;
			result->unbuf = mnd_ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));
			if (!result->unbuf) {
				/* OOM */
				SET_OOM_ERROR(*conn->error_info);
				result->m.free_result(result, TRUE TSRMLS_CC);
				result = NULL;
				break;
			}
			result->unbuf->eof_reached = TRUE;
		} while (0);
		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS TSRMLS_CC);
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::list_method */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, list_method)(MYSQLND_CONN_DATA * conn, const char * query, const char *achtung_wild, char *par1 TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, list_method);
	char * show_query = NULL;
	size_t show_query_len;
	MYSQLND_RES * result = NULL;

	DBG_ENTER("mysqlnd_conn_data::list_method");
	DBG_INF_FMT("conn=%llu query=%s wild=%u", conn->thread_id, query, achtung_wild);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		if (par1) {
			if (achtung_wild) {
				show_query_len = mnd_sprintf(&show_query, 0, query, par1, achtung_wild);
			} else {
				show_query_len = mnd_sprintf(&show_query, 0, query, par1);
			}
		} else {
			if (achtung_wild) {
				show_query_len = mnd_sprintf(&show_query, 0, query, achtung_wild);
			} else {
				show_query_len = strlen(show_query = (char *)query);
			}
		}

		if (PASS == conn->m->query(conn, show_query, show_query_len TSRMLS_CC)) {
			result = conn->m->store_result(conn TSRMLS_CC);
		}
		if (show_query != query) {
			mnd_sprintf_free(show_query);
		}
		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS TSRMLS_CC);
	}
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::errno */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, errno)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->error_info->error_no;
}
/* }}} */


/* {{{ mysqlnd_conn_data::error */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, error)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->error_info->error;
}
/* }}} */


/* {{{ mysqlnd_conn_data::sqlstate */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, sqlstate)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->error_info->sqlstate[0] ? conn->error_info->sqlstate:MYSQLND_SQLSTATE_NULL;
}
/* }}} */


/* {{{ mysqlnd_old_escape_string */
PHPAPI ulong 
mysqlnd_old_escape_string(char * newstr, const char * escapestr, size_t escapestr_len TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_old_escape_string");
	DBG_RETURN(mysqlnd_cset_escape_slashes(mysqlnd_find_charset_name("latin1"), newstr, escapestr, escapestr_len TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_conn_data::ssl_set */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, ssl_set)(MYSQLND_CONN_DATA * const conn, const char * key, const char * const cert,
									  const char * const ca, const char * const capath, const char * const cipher TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, ssl_set);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::ssl_set");

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = (PASS == conn->net->m.set_client_option(conn->net, MYSQLND_OPT_SSL_KEY, key TSRMLS_CC) &&
			PASS == conn->net->m.set_client_option(conn->net, MYSQLND_OPT_SSL_CERT, cert TSRMLS_CC) &&
			PASS == conn->net->m.set_client_option(conn->net, MYSQLND_OPT_SSL_CA, ca TSRMLS_CC) &&
			PASS == conn->net->m.set_client_option(conn->net, MYSQLND_OPT_SSL_CAPATH, capath TSRMLS_CC) &&
			PASS == conn->net->m.set_client_option(conn->net, MYSQLND_OPT_SSL_CIPHER, cipher TSRMLS_CC)) ? PASS : FAIL;

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::escape_string */
static ulong
MYSQLND_METHOD(mysqlnd_conn_data, escape_string)(MYSQLND_CONN_DATA * const conn, char * newstr, const char * escapestr, size_t escapestr_len TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, escape_string);
	ulong ret;
	DBG_ENTER("mysqlnd_conn_data::escape_string");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		if (conn->upsert_status->server_status & SERVER_STATUS_NO_BACKSLASH_ESCAPES) {
			ret = mysqlnd_cset_escape_quotes(conn->charset, newstr, escapestr, escapestr_len TSRMLS_CC);
		} else {
			ret = mysqlnd_cset_escape_slashes(conn->charset, newstr, escapestr, escapestr_len TSRMLS_CC);
		}
		conn->m->local_tx_end(conn, this_func, PASS TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::dump_debug_info */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, dump_debug_info)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, server_dump_debug_information);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::dump_debug_info");
	DBG_INF_FMT("conn=%llu", conn->thread_id);
	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->simple_command(conn, COM_DEBUG, NULL, 0, PROT_EOF_PACKET, FALSE, TRUE TSRMLS_CC);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::select_db */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, select_db)(MYSQLND_CONN_DATA * const conn, const char * const db, unsigned int db_len TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, select_db);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::select_db");
	DBG_INF_FMT("conn=%llu db=%s", conn->thread_id, db);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->simple_command(conn, COM_INIT_DB, (zend_uchar*) db, db_len, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC);
		/*
		  The server sends 0 but libmysql doesn't read it and has established
		  a protocol of giving back -1. Thus we have to follow it :(
		*/
		SET_ERROR_AFF_ROWS(conn);
		if (ret == PASS) {
			if (conn->connect_or_select_db) {
				mnd_pefree(conn->connect_or_select_db, conn->persistent);
			}
			conn->connect_or_select_db = mnd_pestrndup(db, db_len, conn->persistent);
			conn->connect_or_select_db_len = db_len;
			if (!conn->connect_or_select_db) {
				/* OOM */
				SET_OOM_ERROR(*conn->error_info);
				ret = FAIL;
			}
		}
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::ping */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, ping)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, ping);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::ping");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->simple_command(conn, COM_PING, NULL, 0, PROT_OK_PACKET, TRUE, TRUE TSRMLS_CC);
		/*
		  The server sends 0 but libmysql doesn't read it and has established
		  a protocol of giving back -1. Thus we have to follow it :(
		*/
		SET_ERROR_AFF_ROWS(conn);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_INF_FMT("ret=%u", ret);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::statistic */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, statistic)(MYSQLND_CONN_DATA * conn, char **message, unsigned int * message_len TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, get_server_statistics);
	enum_func_status ret = FAIL;
	MYSQLND_PACKET_STATS * stats_header;

	DBG_ENTER("mysqlnd_conn_data::statistic");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		do {
			ret = conn->m->simple_command(conn, COM_STATISTICS, NULL, 0, PROT_LAST, FALSE, TRUE TSRMLS_CC);
			if (FAIL == ret) {
				break;
			}
			stats_header = conn->protocol->m.get_stats_packet(conn->protocol, FALSE TSRMLS_CC);
			if (!stats_header) {
				SET_OOM_ERROR(*conn->error_info);
				break;
			}

			if (PASS == (ret = PACKET_READ(stats_header, conn))) {
				/* will be freed by Zend, thus don't use the mnd_ allocator */
				*message = estrndup(stats_header->message, stats_header->message_len); 
				*message_len = stats_header->message_len;
				DBG_INF(*message);
			}
			PACKET_FREE(stats_header);
		} while (0);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::kill */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, kill)(MYSQLND_CONN_DATA * conn, unsigned int pid TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, kill_connection);
	enum_func_status ret = FAIL;
	zend_uchar buff[4];

	DBG_ENTER("mysqlnd_conn_data::kill");
	DBG_INF_FMT("conn=%llu pid=%u", conn->thread_id, pid);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		int4store(buff, pid);

		/* If we kill ourselves don't expect OK packet, PROT_LAST will skip it */
		if (pid != conn->thread_id) {
			ret = conn->m->simple_command(conn, COM_PROCESS_KILL, buff, 4, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC);
			/*
			  The server sends 0 but libmysql doesn't read it and has established
			  a protocol of giving back -1. Thus we have to follow it :(
			*/
			SET_ERROR_AFF_ROWS(conn);
		} else if (PASS == (ret = conn->m->simple_command(conn, COM_PROCESS_KILL, buff, 4, PROT_LAST, FALSE, TRUE TSRMLS_CC))) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
		}

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_charset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_charset)(MYSQLND_CONN_DATA * const conn, const char * const csname TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, set_charset);
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * const charset = mysqlnd_find_charset_name(csname);

	DBG_ENTER("mysqlnd_conn_data::set_charset");
	DBG_INF_FMT("conn=%llu cs=%s", conn->thread_id, csname);

	if (!charset) {
		SET_CLIENT_ERROR(*conn->error_info, CR_CANT_FIND_CHARSET, UNKNOWN_SQLSTATE,
						 "Invalid characterset or character set not supported");
		DBG_RETURN(ret);
	}

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		char * query;
		size_t query_len = mnd_sprintf(&query, 0, "SET NAMES %s", csname);

		if (FAIL == (ret = conn->m->query(conn, query, query_len TSRMLS_CC))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error executing query");
		} else if (conn->error_info->error_no) {
			ret = FAIL;
		} else {
			conn->charset = charset;
		}
		mnd_sprintf_free(query);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::refresh */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, refresh)(MYSQLND_CONN_DATA * const conn, uint8_t options TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, refresh_server);
	enum_func_status ret = FAIL;
	zend_uchar bits[1];
	DBG_ENTER("mysqlnd_conn_data::refresh");
	DBG_INF_FMT("conn=%llu options=%lu", conn->thread_id, options);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		int1store(bits, options);

		ret = conn->m->simple_command(conn, COM_REFRESH, bits, 1, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::shutdown */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, shutdown)(MYSQLND_CONN_DATA * const conn, uint8_t level TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, shutdown_server);
	enum_func_status ret = FAIL;
	zend_uchar bits[1];
	DBG_ENTER("mysqlnd_conn_data::shutdown");
	DBG_INF_FMT("conn=%llu level=%lu", conn->thread_id, level);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		int1store(bits, level);

		ret = conn->m->simple_command(conn, COM_SHUTDOWN, bits, 1, PROT_OK_PACKET, FALSE, TRUE TSRMLS_CC);

		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
	}
	DBG_RETURN(ret);	
}
/* }}} */


/* {{{ mysqlnd_send_close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, send_close)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	enum_func_status ret = PASS;

	DBG_ENTER("mysqlnd_send_close");
	DBG_INF_FMT("conn=%llu conn->net->stream->abstract=%p",
				conn->thread_id, conn->net->stream? conn->net->stream->abstract:NULL);

	if (CONN_GET_STATE(conn) >= CONN_READY) {
		MYSQLND_DEC_CONN_STATISTIC(conn->stats, STAT_OPENED_CONNECTIONS);
		if (conn->persistent) {
			MYSQLND_DEC_CONN_STATISTIC(conn->stats, STAT_OPENED_PERSISTENT_CONNECTIONS);
		}
	}
	switch (CONN_GET_STATE(conn)) {
		case CONN_READY:
			DBG_INF("Connection clean, sending COM_QUIT");
			if (conn->net->stream) {
				ret = conn->m->simple_command(conn, COM_QUIT, NULL, 0, PROT_LAST, TRUE, TRUE TSRMLS_CC);
			}
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


/* {{{ mysqlnd_conn_data::get_reference */
static MYSQLND_CONN_DATA *
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, get_reference)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::get_reference");
	++conn->refcount;
	DBG_INF_FMT("conn=%llu new_refcount=%u", conn->thread_id, conn->refcount);
	DBG_RETURN(conn);
}
/* }}} */


/* {{{ mysqlnd_conn_data::free_reference */
static enum_func_status
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, free_reference)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_conn_data::free_reference");
	DBG_INF_FMT("conn=%llu old_refcount=%u", conn->thread_id, conn->refcount);
	if (!(--conn->refcount)) {
		/*
		  No multithreading issues as we don't share the connection :)
		  This will free the object too, of course because references has
		  reached zero.
		*/
		ret = conn->m->send_close(conn TSRMLS_CC);
		conn->m->dtor(conn TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_state */
static enum mysqlnd_connection_state
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, get_state)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::get_state");
	DBG_RETURN(conn->state);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_state */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, set_state)(MYSQLND_CONN_DATA * const conn, enum mysqlnd_connection_state new_state TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::set_state");
	DBG_INF_FMT("New state=%u", new_state);
	conn->state = new_state;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::field_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, field_count)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->field_count;
}
/* }}} */


/* {{{ mysqlnd_conn_data::server_status */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, server_status)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->upsert_status->server_status;
}
/* }}} */


/* {{{ mysqlnd_conn_data::insert_id */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn_data, insert_id)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->upsert_status->last_insert_id;
}
/* }}} */


/* {{{ mysqlnd_conn_data::affected_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn_data, affected_rows)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->upsert_status->affected_rows;
}
/* }}} */


/* {{{ mysqlnd_conn_data::warning_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, warning_count)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->upsert_status->warning_count;
}
/* }}} */


/* {{{ mysqlnd_conn_data::info */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, info)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
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


/* {{{ mysqlnd_conn_data::get_server_info */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, get_server_info)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->server_version;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_host_info */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, get_host_info)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->host_info;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_proto_info */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, get_proto_info)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->protocol_version;
}
/* }}} */


/* {{{ mysqlnd_conn_data::charset_name */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, charset_name)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->charset->name;
}
/* }}} */


/* {{{ mysqlnd_conn_data::thread_id */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn_data, thread_id)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	return conn->thread_id;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_server_version */
static unsigned long
MYSQLND_METHOD(mysqlnd_conn_data, get_server_version)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
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


/* {{{ mysqlnd_conn_data::more_results */
static zend_bool
MYSQLND_METHOD(mysqlnd_conn_data, more_results)(const MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::more_results");
	/* (conn->state == CONN_NEXT_RESULT_PENDING) too */
	DBG_RETURN(conn->upsert_status->server_status & SERVER_MORE_RESULTS_EXISTS? TRUE:FALSE);
}
/* }}} */


/* {{{ mysqlnd_conn_data::next_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, next_result)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, next_result);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::next_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		do {
			if (CONN_GET_STATE(conn) != CONN_NEXT_RESULT_PENDING) {
				break;
			}

			SET_EMPTY_ERROR(*conn->error_info);
			SET_ERROR_AFF_ROWS(conn);
			/*
			  We are sure that there is a result set, since conn->state is set accordingly
			  in mysqlnd_store_result() or mysqlnd_fetch_row_unbuffered()
			*/
			if (FAIL == (ret = conn->m->query_read_result_set_header(conn, NULL TSRMLS_CC))) {
				/*
				  There can be an error in the middle of a multi-statement, which will cancel the multi-statement.
				  So there are no more results and we should just return FALSE, error_no has been set
				*/
				if (!conn->error_info->error_no) {
					DBG_ERR_FMT("Serious error. %s::%u", __FILE__, __LINE__);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Serious error. PID=%d", getpid());
					CONN_SET_STATE(conn, CONN_QUIT_SENT);
				} else {
					DBG_INF_FMT("Error from the server : (%u) %s", conn->error_info->error_no, conn->error_info->error);
				}
				break;
			}
			if (conn->last_query_type == QUERY_UPSERT && conn->upsert_status->affected_rows) {
				MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats, STAT_ROWS_AFFECTED_NORMAL, conn->upsert_status->affected_rows);
			}
		} while (0);
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
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


/* {{{ mysqlnd_conn_data::change_user */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, change_user)(MYSQLND_CONN_DATA * const conn,
										  const char * user,
										  const char * passwd,
										  const char * db,
										  zend_bool silent,
										  size_t passwd_len
										  TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, change_user);
	/*
	  User could be max 16 * 3 (utf8), pass is 20 usually, db is up to 64*3
	  Stack space is not that expensive, so use a bit more to be protected against
	  buffer overflows.
	*/
	enum_func_status ret = FAIL;
	zend_bool local_tx_started = FALSE;

	DBG_ENTER("mysqlnd_conn_data::change_user");
	DBG_INF_FMT("conn=%llu user=%s passwd=%s db=%s silent=%u",
				conn->thread_id, user?user:"", passwd?"***":"null", db?db:"", (silent == TRUE)?1:0 );

	if (PASS != conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		goto end;
	}
	local_tx_started = TRUE;

	SET_EMPTY_ERROR(*conn->error_info);
	SET_ERROR_AFF_ROWS(conn);

	if (!user) {
		user = "";
	}
	if (!passwd) {
		passwd = "";
	}
	if (!db) {
		db = "";
	}

	{
		zend_bool first_call = TRUE;
		char * switch_to_auth_protocol = NULL;
		size_t switch_to_auth_protocol_len = 0;
		char * requested_protocol = NULL;
		zend_uchar * plugin_data;
		size_t plugin_data_len;

		plugin_data_len = conn->auth_plugin_data_len;
		plugin_data = mnd_emalloc(plugin_data_len);
		if (!plugin_data) {
			ret = FAIL;
			goto end;
		}
		memcpy(plugin_data, conn->auth_plugin_data, plugin_data_len);

		requested_protocol = mnd_pestrdup(conn->options->auth_protocol? conn->options->auth_protocol:"mysql_native_password", FALSE);
		if (!requested_protocol) {
			ret = FAIL;
			goto end;
		}

		do {
			struct st_mysqlnd_authentication_plugin * auth_plugin;
			{
				char * plugin_name = NULL;

				mnd_sprintf(&plugin_name, 0, "auth_plugin_%s", requested_protocol);

				DBG_INF_FMT("looking for %s auth plugin", plugin_name);
				auth_plugin = mysqlnd_plugin_find(plugin_name);
				mnd_sprintf_free(plugin_name);

				if (!auth_plugin) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "The server requested authentication method unknown to the client [%s]", requested_protocol);
					SET_CLIENT_ERROR(*conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "The server requested authentication method umknown to the client");
					break;
				}
			}
			DBG_INF("plugin found");

			{
				zend_uchar * switch_to_auth_protocol_data = NULL;
				size_t switch_to_auth_protocol_data_len = 0;
				zend_uchar * scrambled_data = NULL;
				size_t scrambled_data_len = 0;

				switch_to_auth_protocol = NULL;
				switch_to_auth_protocol_len = 0;

				if (conn->auth_plugin_data) {
					mnd_pefree(conn->auth_plugin_data, conn->persistent);
					conn->auth_plugin_data = NULL;
				}
				conn->auth_plugin_data_len = plugin_data_len;
				conn->auth_plugin_data = mnd_pemalloc(conn->auth_plugin_data_len, conn->persistent);
				if (!conn->auth_plugin_data) {
					SET_OOM_ERROR(*conn->error_info);
					ret = FAIL;
					goto end;
				}
				memcpy(conn->auth_plugin_data, plugin_data, plugin_data_len);

				DBG_INF_FMT("salt=[%*.s]", plugin_data_len - 1, plugin_data);

				/* The data should be allocated with malloc() */
				scrambled_data =
						auth_plugin->methods.get_auth_data(NULL, &scrambled_data_len, conn, user, passwd, passwd_len,
														   plugin_data, plugin_data_len, 0, conn->server_capabilities TSRMLS_CC);


				ret = mysqlnd_auth_change_user(conn, user, strlen(user), passwd, passwd_len, db, strlen(db), silent,
											   first_call,
											   requested_protocol,
											   scrambled_data, scrambled_data_len,
											   &switch_to_auth_protocol, &switch_to_auth_protocol_len,
											   &switch_to_auth_protocol_data, &switch_to_auth_protocol_data_len
											   TSRMLS_CC);

				first_call = FALSE;
				free(scrambled_data);

				DBG_INF_FMT("switch_to_auth_protocol=%s", switch_to_auth_protocol? switch_to_auth_protocol:"n/a");
				if (requested_protocol) {
					mnd_efree(requested_protocol);
				}
				requested_protocol = switch_to_auth_protocol;

				if (plugin_data) {
					mnd_efree(plugin_data);
				}
				plugin_data_len = switch_to_auth_protocol_data_len;
				plugin_data = switch_to_auth_protocol_data;
			}
			DBG_INF_FMT("conn->error_info->error_no = %d", conn->error_info->error_no);
		} while (ret == FAIL && conn->error_info->error_no == 0 && switch_to_auth_protocol != NULL);
		if (plugin_data) {
			mnd_efree(plugin_data);
		}
		if (ret == PASS) {
			conn->m->set_client_option(conn, MYSQLND_OPT_AUTH_PROTOCOL, requested_protocol TSRMLS_CC);
		}
		if (requested_protocol) {
			mnd_efree(requested_protocol);
		}
	}
	/*
	  Here we should close all statements. Unbuffered queries should not be a
	  problem as we won't allow sending COM_CHANGE_USER.
	*/
end:
	if (TRUE == local_tx_started) {
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);	
	}
	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_client_option)(MYSQLND_CONN_DATA * const conn,
												enum mysqlnd_option option,
												const char * const value
												TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, set_client_option);
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_conn_data::set_client_option");
	DBG_INF_FMT("conn=%llu option=%u", conn->thread_id, option);

	if (PASS != conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		goto end;
	}
	switch (option) {
		case MYSQL_OPT_COMPRESS:
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_READ_TIMEOUT:
		case MYSQL_OPT_WRITE_TIMEOUT:
#endif
		case MYSQLND_OPT_SSL_KEY:
		case MYSQLND_OPT_SSL_CERT:
		case MYSQLND_OPT_SSL_CA:
		case MYSQLND_OPT_SSL_CAPATH:
		case MYSQLND_OPT_SSL_CIPHER:
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
		case MYSQL_OPT_CONNECT_TIMEOUT:
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
			ret = conn->net->m.set_client_option(conn->net, option, value TSRMLS_CC);
			break;
#if MYSQLND_UNICODE
		case MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE:
			conn->options->numeric_and_datetime_as_unicode = *(unsigned int*) value;
			break;
#endif
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
		case MYSQLND_OPT_INT_AND_FLOAT_NATIVE:
			conn->options->int_and_float_native = *(unsigned int*) value;
			break;
#endif
		case MYSQL_OPT_LOCAL_INFILE:
			if (!value || (*(unsigned int*) value) ? 1 : 0) {
				conn->options->flags |= CLIENT_LOCAL_FILES;
			} else {
				conn->options->flags &= ~CLIENT_LOCAL_FILES;
			}
			break;
		case MYSQL_INIT_COMMAND:
		{
			char ** new_init_commands;
			char * new_command;
			/* when num_commands is 0, then realloc will be effectively a malloc call, internally */
			/* Don't assign to conn->options->init_commands because in case of OOM we will lose the pointer and leak */
			new_init_commands = mnd_perealloc(conn->options->init_commands, sizeof(char *) * (conn->options->num_commands + 1), conn->persistent);
			if (!new_init_commands) {
				goto oom;
			}
			conn->options->init_commands = new_init_commands;
			new_command = mnd_pestrdup(value, conn->persistent);
			if (!new_command) {
				goto oom;
			}
			conn->options->init_commands[conn->options->num_commands] = new_command;
			++conn->options->num_commands;
			break;
		}
		case MYSQL_READ_DEFAULT_FILE:
		case MYSQL_READ_DEFAULT_GROUP:
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_SET_CLIENT_IP:
		case MYSQL_REPORT_DATA_TRUNCATION:
#endif
			/* currently not supported. Todo!! */
			break;
		case MYSQL_SET_CHARSET_NAME:
		{
			char * new_charset_name = mnd_pestrdup(value, conn->persistent);
			if (!new_charset_name) {
				goto oom;
			}
			if (conn->options->charset_name) {
				mnd_pefree(conn->options->charset_name, conn->persistent);
			}
			conn->options->charset_name = new_charset_name;
			DBG_INF_FMT("charset=%s", conn->options->charset_name);
			break;
		}
		case MYSQL_OPT_NAMED_PIPE:
			conn->options->protocol = MYSQL_PROTOCOL_PIPE;
			break;
		case MYSQL_OPT_PROTOCOL:
			if (*(unsigned int*) value < MYSQL_PROTOCOL_LAST) {
				conn->options->protocol = *(unsigned int*) value;
			}
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_SET_CHARSET_DIR:
		case MYSQL_OPT_RECONNECT:
			/* we don't need external character sets, all character sets are
			   compiled in. For compatibility we just ignore this setting.
			   Same for protocol, we don't support old protocol */
		case MYSQL_OPT_USE_REMOTE_CONNECTION:
		case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
		case MYSQL_OPT_GUESS_CONNECTION:
			/* todo: throw an error, we don't support embedded */
			break;
#endif
		case MYSQLND_OPT_MAX_ALLOWED_PACKET:
			if (*(unsigned int*) value > (1<<16)) {
				conn->options->max_allowed_packet = *(unsigned int*) value;
			}
			break;
		case MYSQLND_OPT_AUTH_PROTOCOL:
		{
			char * new_auth_protocol = value? mnd_pestrdup(value, conn->persistent) : NULL;
			if (value && !new_auth_protocol) {
				goto oom;
			}
			if (conn->options->auth_protocol) {
				mnd_pefree(conn->options->auth_protocol, conn->persistent);
			}
			conn->options->auth_protocol = new_auth_protocol;
			DBG_INF_FMT("auth_protocol=%s", conn->options->auth_protocol);
			break;
		}
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_SHARED_MEMORY_BASE_NAME:
		case MYSQL_OPT_USE_RESULT:
		case MYSQL_SECURE_AUTH:
			/* not sure, todo ? */
#endif
		default:
			ret = FAIL;
	}
	conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);	
	DBG_RETURN(ret);
oom:
	SET_OOM_ERROR(*conn->error_info);
	conn->m->local_tx_end(conn, this_func, FAIL TSRMLS_CC);	
end:
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_conn_data::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, use_result)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, use_result);
	MYSQLND_RES * result = NULL;

	DBG_ENTER("mysqlnd_conn_data::use_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		do {
			if (!conn->current_result) {
				break;
			}

			/* Nothing to store for UPSERT/LOAD DATA */
			if (conn->last_query_type != QUERY_SELECT || CONN_GET_STATE(conn) != CONN_FETCHING_DATA) {
				SET_CLIENT_ERROR(*conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
				DBG_ERR("Command out of sync");
				break;
			}

			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_UNBUFFERED_SETS);

			conn->current_result->conn = conn->m->get_reference(conn TSRMLS_CC);
			result = conn->current_result->m.use_result(conn->current_result, FALSE TSRMLS_CC);

			if (!result) {
				conn->current_result->m.free_result(conn->current_result, TRUE TSRMLS_CC);
			}
			conn->current_result = NULL;
		} while (0);

		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS TSRMLS_CC);	
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, store_result)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, store_result);
	MYSQLND_RES * result = NULL;

	DBG_ENTER("mysqlnd_conn_data::store_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		do {
			if (!conn->current_result) {
				break;
			}

			/* Nothing to store for UPSERT/LOAD DATA*/
			if (conn->last_query_type != QUERY_SELECT || CONN_GET_STATE(conn) != CONN_FETCHING_DATA) {
				SET_CLIENT_ERROR(*conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
				DBG_ERR("Command out of sync");
				break;
			}

			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_BUFFERED_SETS);

			result = conn->current_result->m.store_result(conn->current_result, conn, FALSE TSRMLS_CC);
			if (!result) {
				conn->current_result->m.free_result(conn->current_result, TRUE TSRMLS_CC);
			}
			conn->current_result = NULL;
		} while (0);

		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS TSRMLS_CC);	
	}
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_connection_stats */
static void
MYSQLND_METHOD(mysqlnd_conn_data, get_connection_stats)(const MYSQLND_CONN_DATA * const conn,
												   zval * return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	DBG_ENTER("mysqlnd_conn_data::get_connection_stats");
	mysqlnd_fill_stats_hash(conn->stats, mysqlnd_stats_values_names, return_value TSRMLS_CC ZEND_FILE_LINE_CC);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_autocommit */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_autocommit)(MYSQLND_CONN_DATA * conn, unsigned int mode TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, set_autocommit);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::set_autocommit");

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->query(conn, (mode) ? "SET AUTOCOMMIT=1":"SET AUTOCOMMIT=0", sizeof("SET AUTOCOMMIT=1") - 1 TSRMLS_CC);
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);	
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_commit */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_commit)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, tx_commit);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::tx_commit");

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->query(conn, "COMMIT", sizeof("COMMIT") - 1 TSRMLS_CC);
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);	
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_rollback */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_rollback)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, tx_rollback);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::tx_rollback");

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		ret = conn->m->query(conn, "ROLLBACK", sizeof("ROLLBACK") - 1 TSRMLS_CC);
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);	
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::local_tx_start */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, local_tx_start)(MYSQLND_CONN_DATA * conn, size_t this_func TSRMLS_DC)
{
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_conn_data::local_tx_start");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::local_tx_end */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, local_tx_end)(MYSQLND_CONN_DATA * conn, size_t this_func, enum_func_status status TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::local_tx_end");
	DBG_RETURN(status);
}
/* }}} */


/* {{{ mysqlnd_conn_data::init */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, init)(MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn_data::init");
	mysqlnd_stats_init(&conn->stats, STAT_LAST);
	SET_ERROR_AFF_ROWS(conn);

	conn->net = mysqlnd_net_init(conn->persistent, conn->stats, conn->error_info TSRMLS_CC);
	conn->protocol = mysqlnd_protocol_init(conn->persistent TSRMLS_CC);

	DBG_RETURN(conn->stats && conn->net && conn->protocol? PASS:FAIL);
}
/* }}} */


MYSQLND_STMT * _mysqlnd_stmt_init(MYSQLND_CONN_DATA * const conn TSRMLS_DC);


MYSQLND_CLASS_METHODS_START(mysqlnd_conn_data)
	MYSQLND_METHOD(mysqlnd_conn_data, init),
	MYSQLND_METHOD(mysqlnd_conn_data, connect),

	MYSQLND_METHOD(mysqlnd_conn_data, escape_string),
	MYSQLND_METHOD(mysqlnd_conn_data, set_charset),
	MYSQLND_METHOD(mysqlnd_conn_data, query),
	MYSQLND_METHOD(mysqlnd_conn_data, send_query),
	MYSQLND_METHOD(mysqlnd_conn_data, reap_query),
	MYSQLND_METHOD(mysqlnd_conn_data, use_result),
	MYSQLND_METHOD(mysqlnd_conn_data, store_result),
	MYSQLND_METHOD(mysqlnd_conn_data, next_result),
	MYSQLND_METHOD(mysqlnd_conn_data, more_results),

	_mysqlnd_stmt_init,

	MYSQLND_METHOD(mysqlnd_conn_data, shutdown),
	MYSQLND_METHOD(mysqlnd_conn_data, refresh),

	MYSQLND_METHOD(mysqlnd_conn_data, ping),
	MYSQLND_METHOD(mysqlnd_conn_data, kill),
	MYSQLND_METHOD(mysqlnd_conn_data, select_db),
	MYSQLND_METHOD(mysqlnd_conn_data, dump_debug_info),
	MYSQLND_METHOD(mysqlnd_conn_data, change_user),

	MYSQLND_METHOD(mysqlnd_conn_data, errno),
	MYSQLND_METHOD(mysqlnd_conn_data, error),
	MYSQLND_METHOD(mysqlnd_conn_data, sqlstate),
	MYSQLND_METHOD(mysqlnd_conn_data, thread_id),

	MYSQLND_METHOD(mysqlnd_conn_data, get_connection_stats),

	MYSQLND_METHOD(mysqlnd_conn_data, get_server_version),
	MYSQLND_METHOD(mysqlnd_conn_data, get_server_info),
	MYSQLND_METHOD(mysqlnd_conn_data, statistic),
	MYSQLND_METHOD(mysqlnd_conn_data, get_host_info),
	MYSQLND_METHOD(mysqlnd_conn_data, get_proto_info),
	MYSQLND_METHOD(mysqlnd_conn_data, info),
	MYSQLND_METHOD(mysqlnd_conn_data, charset_name),
	MYSQLND_METHOD(mysqlnd_conn_data, list_fields),
	MYSQLND_METHOD(mysqlnd_conn_data, list_method),

	MYSQLND_METHOD(mysqlnd_conn_data, insert_id),
	MYSQLND_METHOD(mysqlnd_conn_data, affected_rows),
	MYSQLND_METHOD(mysqlnd_conn_data, warning_count),
	MYSQLND_METHOD(mysqlnd_conn_data, field_count),

	MYSQLND_METHOD(mysqlnd_conn_data, server_status),

	MYSQLND_METHOD(mysqlnd_conn_data, set_server_option),
	MYSQLND_METHOD(mysqlnd_conn_data, set_client_option),
	MYSQLND_METHOD(mysqlnd_conn_data, free_contents),
	MYSQLND_METHOD(mysqlnd_conn_data, free_options),

	MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, dtor),

	mysqlnd_query_read_result_set_header,

	MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, get_reference),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, free_reference),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, get_state),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, set_state),

	MYSQLND_METHOD(mysqlnd_conn_data, simple_command),
	MYSQLND_METHOD(mysqlnd_conn_data, simple_command_handle_response),
	MYSQLND_METHOD(mysqlnd_conn_data, restart_psession),
	MYSQLND_METHOD(mysqlnd_conn_data, end_psession),
	MYSQLND_METHOD(mysqlnd_conn_data, send_close),

	MYSQLND_METHOD(mysqlnd_conn_data, ssl_set),
	mysqlnd_result_init,
	MYSQLND_METHOD(mysqlnd_conn_data, set_autocommit),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_commit),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_rollback),
	MYSQLND_METHOD(mysqlnd_conn_data, local_tx_start),
	MYSQLND_METHOD(mysqlnd_conn_data, local_tx_end)
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_conn::get_reference */
static MYSQLND *
MYSQLND_METHOD(mysqlnd_conn, clone_object)(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND * ret;
	DBG_ENTER("mysqlnd_conn::get_reference");
	ret = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory).clone_connection_object(conn TSRMLS_CC);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::dtor */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, dtor)(MYSQLND * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_conn::dtor");
	DBG_INF_FMT("conn=%llu", conn->data->thread_id);

	conn->data->m->free_reference(conn->data TSRMLS_CC);

	mnd_pefree(conn, conn->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, close)(MYSQLND * conn_handle, enum_connection_close_type close_type TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_methods, close);
	MYSQLND_CONN_DATA * conn = conn_handle->data;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn::close");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
		if (CONN_GET_STATE(conn) >= CONN_READY) {
			static enum_mysqlnd_collected_stats close_type_to_stat_map[MYSQLND_CLOSE_LAST] = {
				STAT_CLOSE_EXPLICIT,
				STAT_CLOSE_IMPLICIT,
				STAT_CLOSE_DISCONNECT
			};
			MYSQLND_INC_CONN_STATISTIC(conn->stats, close_type_to_stat_map[close_type]);
		}

		/*
		  Close now, free_reference will try,
		  if we are last, but that's not a problem.
		*/
		ret = conn->m->send_close(conn TSRMLS_CC);

		/* do it after free_reference/dtor and we might crash */
		conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);

		conn_handle->m->dtor(conn_handle TSRMLS_CC);
	}
	DBG_RETURN(ret);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_conn)
	MYSQLND_METHOD(mysqlnd_conn, connect),
	MYSQLND_METHOD(mysqlnd_conn, clone_object),
	MYSQLND_METHOD_PRIVATE(mysqlnd_conn, dtor),
	MYSQLND_METHOD(mysqlnd_conn, close)
MYSQLND_CLASS_METHODS_END;


/* {{{ _mysqlnd_init */
PHPAPI MYSQLND *
_mysqlnd_init(zend_bool persistent TSRMLS_DC)
{
	MYSQLND * ret;
	DBG_ENTER("mysqlnd_init");
	ret = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory).get_connection(persistent TSRMLS_CC);
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
