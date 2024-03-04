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
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_connection.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_auth.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_charset.h"


/* {{{ mysqlnd_command::set_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, set_option)(MYSQLND_CONN_DATA * const conn, const enum_mysqlnd_server_option option)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	zend_uchar buffer[2];
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::set_option");
	int2store(buffer, (unsigned int) option);

	ret = send_command(conn->payload_decoder_factory, COM_SET_OPTION, buffer, sizeof(buffer), FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_EOF_PACKET, FALSE, COM_SET_OPTION, TRUE,
		                                   conn->error_info, conn->upsert_status, &conn->last_message);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::debug */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, debug)(MYSQLND_CONN_DATA * const conn)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::debug");

	ret = send_command(conn->payload_decoder_factory, COM_DEBUG, NULL, 0, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_EOF_PACKET, FALSE, COM_DEBUG, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::init_db */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, init_db)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING db)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::init_db");

	ret = send_command(conn->payload_decoder_factory, COM_INIT_DB, (const zend_uchar*) db.s, db.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_OK_PACKET, FALSE, COM_INIT_DB, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}

	/*
	  The server sends 0 but libmysql doesn't read it and has established
	  a protocol of giving back -1. Thus we have to follow it :(
	*/
	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);
	if (ret == PASS) {
		mysqlnd_set_persistent_string(&conn->connect_or_select_db, db.s, db.l, conn->persistent);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::ping */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, ping)(MYSQLND_CONN_DATA * const conn)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::ping");

	ret = send_command(conn->payload_decoder_factory, COM_PING, NULL, 0, TRUE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_OK_PACKET, TRUE, COM_PING, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}
	/*
	  The server sends 0 but libmysql doesn't read it and has established
	  a protocol of giving back -1. Thus we have to follow it :(
	*/
	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::statistics */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, statistics)(MYSQLND_CONN_DATA * const conn, zend_string ** message)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::statistics");

	ret = send_command(conn->payload_decoder_factory, COM_STATISTICS, NULL, 0, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	if (PASS == ret) {
		MYSQLND_PACKET_STATS stats_header;

		conn->payload_decoder_factory->m.init_stats_packet(&stats_header);
		if (PASS == (ret = PACKET_READ(conn, &stats_header))) {
			/* will be freed by Zend, thus don't use the mnd_ allocator */
			*message = zend_string_init(stats_header.message.s, stats_header.message.l, 0);
			DBG_INF(ZSTR_VAL(*message));
		}
		PACKET_FREE(&stats_header);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::process_kill */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, process_kill)(MYSQLND_CONN_DATA * const conn, const unsigned int process_id, const bool read_response)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	zend_uchar buff[4];
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::process_kill");
	int4store(buff, process_id);

	ret = send_command(conn->payload_decoder_factory, COM_PROCESS_KILL, buff, 4, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret && read_response) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_OK_PACKET, FALSE, COM_PROCESS_KILL, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}

	if (read_response) {
		/*
		  The server sends 0 but libmysql doesn't read it and has established
		  a protocol of giving back -1. Thus we have to follow it :(
		*/
		UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);
	} else if (PASS == ret) {
		SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
		conn->m->send_close(conn);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::refresh */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, refresh)(MYSQLND_CONN_DATA * const conn, const uint8_t options)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	zend_uchar bits[1];
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::refresh");
	int1store(bits, options);

	ret = send_command(conn->payload_decoder_factory, COM_REFRESH, bits, 1, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_OK_PACKET, FALSE, COM_REFRESH, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::shutdown */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, shutdown)(MYSQLND_CONN_DATA * const conn, const uint8_t level)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	zend_uchar bits[1];
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::shutdown");
	int1store(bits, level);

	ret = send_command(conn->payload_decoder_factory, COM_SHUTDOWN, bits, 1, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_OK_PACKET, FALSE, COM_SHUTDOWN, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::quit */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, quit)(MYSQLND_CONN_DATA * const conn)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::quit");

	ret = send_command(conn->payload_decoder_factory, COM_QUIT, NULL, 0, TRUE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, query)(MYSQLND_CONN_DATA * const conn, MYSQLND_CSTRING query)
{
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::query");

	ret = send_command(conn->payload_decoder_factory, COM_QUERY, (const zend_uchar*) query.s, query.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	if (PASS == ret) {
		SET_CONNECTION_STATE(&conn->state, CONN_QUERY_SENT);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::change_user */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, change_user)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING payload, const bool silent)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::change_user");

	ret = send_command(conn->payload_decoder_factory, COM_CHANGE_USER, (const zend_uchar*) payload.s, payload.l, silent,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::reap_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, reap_result)(MYSQLND_CONN_DATA * const conn)
{
	const enum_mysqlnd_connection_state state = GET_CONNECTION_STATE(&conn->state);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::reap_result");
	if (state <= CONN_READY || state == CONN_QUIT_SENT) {
		php_error_docref(NULL, E_WARNING, "Connection not opened, clear or has been closed");
		DBG_ERR_FMT("Connection not opened, clear or has been closed. State=%u", state);
		DBG_RETURN(ret);
	}
	ret = conn->m->query_read_result_set_header(conn, NULL);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::stmt_prepare */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, stmt_prepare)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING query)
{
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::stmt_prepare");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_PREPARE, (const zend_uchar*) query.s, query.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::stmt_execute */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, stmt_execute)(MYSQLND_CONN_DATA * conn, const MYSQLND_CSTRING payload)
{
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::stmt_execute");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_EXECUTE,
					   (const unsigned char *) payload.s, payload.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::stmt_fetch */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, stmt_fetch)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING payload)
{
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::stmt_fetch");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_FETCH, (const zend_uchar*) payload.s, payload.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::stmt_reset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, stmt_reset)(MYSQLND_CONN_DATA * const conn, const zend_ulong stmt_id)
{
	const func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	const func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;
	zend_uchar cmd_buf[MYSQLND_STMT_ID_LENGTH /* statement id */];
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::stmt_reset");

	int4store(cmd_buf, stmt_id);
	ret = send_command(conn->payload_decoder_factory, COM_STMT_RESET, cmd_buf, sizeof(cmd_buf), FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	if (PASS == ret) {
		ret = send_command_handle_response(conn->payload_decoder_factory, PROT_OK_PACKET, FALSE, COM_STMT_RESET, TRUE,
										   conn->error_info, conn->upsert_status, &conn->last_message);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::stmt_send_long_data */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, stmt_send_long_data)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING payload)
{
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::stmt_send_long_data");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_SEND_LONG_DATA, (const zend_uchar*) payload.s, payload.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);
	/* COM_STMT_SEND_LONG_DATA - doesn't read result, the server doesn't send ACK */
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::stmt_close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, stmt_close)(MYSQLND_CONN_DATA * const conn, const zend_ulong stmt_id)
{
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	zend_uchar cmd_buf[MYSQLND_STMT_ID_LENGTH /* statement id */];
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_command::stmt_close");

	int4store(cmd_buf, stmt_id);
	ret = send_command(conn->payload_decoder_factory, COM_STMT_CLOSE, cmd_buf, sizeof(cmd_buf), FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::enable_ssl */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, enable_ssl)(MYSQLND_CONN_DATA * const conn, const size_t client_capabilities, const size_t server_capabilities, const unsigned int charset_no)
{
	enum_func_status ret = FAIL;
	MYSQLND_PACKET_AUTH auth_packet;

	DBG_ENTER("mysqlnd_command::enable_ssl");

	DBG_INF_FMT("client_capability_flags=%zu", client_capabilities);
	DBG_INF_FMT("CLIENT_LONG_PASSWORD=	%d", client_capabilities & CLIENT_LONG_PASSWORD? 1:0);
	DBG_INF_FMT("CLIENT_FOUND_ROWS=		%d", client_capabilities & CLIENT_FOUND_ROWS? 1:0);
	DBG_INF_FMT("CLIENT_LONG_FLAG=		%d", client_capabilities & CLIENT_LONG_FLAG? 1:0);
	DBG_INF_FMT("CLIENT_NO_SCHEMA=		%d", client_capabilities & CLIENT_NO_SCHEMA? 1:0);
	DBG_INF_FMT("CLIENT_COMPRESS=		%d", client_capabilities & CLIENT_COMPRESS? 1:0);
	DBG_INF_FMT("CLIENT_ODBC=			%d", client_capabilities & CLIENT_ODBC? 1:0);
	DBG_INF_FMT("CLIENT_LOCAL_FILES=	%d", client_capabilities & CLIENT_LOCAL_FILES? 1:0);
	DBG_INF_FMT("CLIENT_IGNORE_SPACE=	%d", client_capabilities & CLIENT_IGNORE_SPACE? 1:0);
	DBG_INF_FMT("CLIENT_PROTOCOL_41=	%d", client_capabilities & CLIENT_PROTOCOL_41? 1:0);
	DBG_INF_FMT("CLIENT_INTERACTIVE=	%d", client_capabilities & CLIENT_INTERACTIVE? 1:0);
	DBG_INF_FMT("CLIENT_SSL=			%d", client_capabilities & CLIENT_SSL? 1:0);
	DBG_INF_FMT("CLIENT_IGNORE_SIGPIPE=	%d", client_capabilities & CLIENT_IGNORE_SIGPIPE? 1:0);
	DBG_INF_FMT("CLIENT_TRANSACTIONS=	%d", client_capabilities & CLIENT_TRANSACTIONS? 1:0);
	DBG_INF_FMT("CLIENT_RESERVED=		%d", client_capabilities & CLIENT_RESERVED? 1:0);
	DBG_INF_FMT("CLIENT_SECURE_CONNECTION=%d", client_capabilities & CLIENT_SECURE_CONNECTION? 1:0);
	DBG_INF_FMT("CLIENT_MULTI_STATEMENTS=%d", client_capabilities & CLIENT_MULTI_STATEMENTS? 1:0);
	DBG_INF_FMT("CLIENT_MULTI_RESULTS=	%d", client_capabilities & CLIENT_MULTI_RESULTS? 1:0);
	DBG_INF_FMT("CLIENT_PS_MULTI_RESULTS=%d", client_capabilities & CLIENT_PS_MULTI_RESULTS? 1:0);
	DBG_INF_FMT("CLIENT_CONNECT_ATTRS=	%d", client_capabilities & CLIENT_PLUGIN_AUTH? 1:0);
	DBG_INF_FMT("CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA=	%d", client_capabilities & CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA? 1:0);
	DBG_INF_FMT("CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS=	%d", client_capabilities & CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS? 1:0);
	DBG_INF_FMT("CLIENT_SESSION_TRACK=		%d", client_capabilities & CLIENT_SESSION_TRACK? 1:0);
	DBG_INF_FMT("CLIENT_SSL_VERIFY_SERVER_CERT=	%d", client_capabilities & CLIENT_SSL_VERIFY_SERVER_CERT? 1:0);
	DBG_INF_FMT("CLIENT_REMEMBER_OPTIONS=		%d", client_capabilities & CLIENT_REMEMBER_OPTIONS? 1:0);

	conn->payload_decoder_factory->m.init_auth_packet(&auth_packet);
	auth_packet.client_flags = client_capabilities;
	auth_packet.max_packet_size = MYSQLND_ASSEMBLED_PACKET_MAX_SIZE;

	auth_packet.charset_no = charset_no;

#ifdef MYSQLND_SSL_SUPPORTED
	if (client_capabilities & CLIENT_SSL) {
		const bool server_has_ssl = (server_capabilities & CLIENT_SSL)? TRUE:FALSE;
		if (server_has_ssl == FALSE) {
			goto close_conn;
		} else {
			enum mysqlnd_ssl_peer verify = client_capabilities & CLIENT_SSL_VERIFY_SERVER_CERT?
												MYSQLND_SSL_PEER_VERIFY:
												(client_capabilities & CLIENT_SSL_DONT_VERIFY_SERVER_CERT?
													MYSQLND_SSL_PEER_DONT_VERIFY:
													MYSQLND_SSL_PEER_DEFAULT);
			DBG_INF("Switching to SSL");
			if (!PACKET_WRITE(conn, &auth_packet)) {
				goto close_conn;
			}

			conn->vio->data->m.set_client_option(conn->vio, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, (const char *) &verify);

			if (FAIL == conn->vio->data->m.enable_ssl(conn->vio)) {
				SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
				SET_CLIENT_ERROR(conn->error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, "Cannot connect to MySQL using SSL");
				goto end;
			}
		}
	}
#else
	auth_packet.client_flags &= ~CLIENT_SSL;
	if (!PACKET_WRITE(conn, &auth_packet)) {
		goto close_conn;
	}
#endif
	ret = PASS;
end:
	PACKET_FREE(&auth_packet);
	DBG_RETURN(ret);

close_conn:
	SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
	conn->m->send_close(conn);
	SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
	PACKET_FREE(&auth_packet);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_command::handshake */
static enum_func_status
MYSQLND_METHOD(mysqlnd_command, handshake)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING username, const MYSQLND_CSTRING password, const MYSQLND_CSTRING database, const size_t client_flags)
{
	const char * const user = username.s;

	const char * const passwd = password.s;
	const size_t passwd_len = password.l;

	const char * const db = database.s;
	const size_t db_len = database.l;

	const size_t mysql_flags = client_flags;

	MYSQLND_PACKET_GREET greet_packet;

	DBG_ENTER("mysqlnd_command::handshake");

	DBG_INF_FMT("stream=%p", conn->vio->data->m.get_stream(conn->vio));
	DBG_INF_FMT("[user=%s] [db=%s:%zu] [flags=%zu]", user, db, db_len, mysql_flags);

	conn->payload_decoder_factory->m.init_greet_packet(&greet_packet);

	if (FAIL == PACKET_READ(conn, &greet_packet)) {
		DBG_ERR("Error while reading greeting packet");
		php_error_docref(NULL, E_WARNING, "Error while reading greeting packet. PID=%d", getpid());
		goto err;
	} else if (greet_packet.error_no) {
		DBG_ERR_FMT("errorno=%u error=%s", greet_packet.error_no, greet_packet.error);
		SET_CLIENT_ERROR(conn->error_info, greet_packet.error_no, greet_packet.sqlstate, greet_packet.error);
		goto err;
	} else if (greet_packet.pre41) {
		char * msg;
		mnd_sprintf(&msg, 0, "Connecting to 3.22, 3.23 & 4.0 is not supported. Server is %-.32s", greet_packet.server_version);
		DBG_ERR(msg);
		SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, msg);
		mnd_sprintf_free(msg);
		goto err;
	}

	conn->thread_id			= greet_packet.thread_id;
	conn->protocol_version	= greet_packet.protocol_version;
	conn->server_version	= mnd_pestrdup(greet_packet.server_version, conn->persistent);

	const MYSQLND_CHARSET *read_charset = mysqlnd_find_charset_nr(greet_packet.charset_no);
	if (!read_charset) {
		greet_packet.charset_no = conn->m->get_server_version(conn) >= 50500 ? MYSQLND_UTF8_MB4_DEFAULT_ID : MYSQLND_UTF8_MB3_DEFAULT_ID;
		conn->greet_charset = mysqlnd_find_charset_nr(greet_packet.charset_no);
	} else {
		conn->greet_charset = read_charset;
	}

	conn->server_capabilities 	= greet_packet.server_capabilities;

	if (FAIL == mysqlnd_connect_run_authentication(conn, user, passwd, db, db_len, (size_t) passwd_len,
												   greet_packet.authentication_plugin_data, greet_packet.auth_protocol,
												   greet_packet.charset_no, greet_packet.server_capabilities,
												   conn->options, mysql_flags))
	{
		goto err;
	}

	UPSERT_STATUS_RESET(conn->upsert_status);
	UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status, greet_packet.server_status);

	PACKET_FREE(&greet_packet);
	DBG_RETURN(PASS);
err:
	conn->server_capabilities = 0;
	PACKET_FREE(&greet_packet);
	DBG_RETURN(FAIL);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_command)
	MYSQLND_METHOD(mysqlnd_command, set_option),
	MYSQLND_METHOD(mysqlnd_command, debug),
	MYSQLND_METHOD(mysqlnd_command, init_db),
	MYSQLND_METHOD(mysqlnd_command, ping),
	MYSQLND_METHOD(mysqlnd_command, statistics),
	MYSQLND_METHOD(mysqlnd_command, process_kill),
	MYSQLND_METHOD(mysqlnd_command, refresh),
	MYSQLND_METHOD(mysqlnd_command, shutdown),
	MYSQLND_METHOD(mysqlnd_command, quit),
	MYSQLND_METHOD(mysqlnd_command, query),
	MYSQLND_METHOD(mysqlnd_command, change_user),
	MYSQLND_METHOD(mysqlnd_command, reap_result),
	MYSQLND_METHOD(mysqlnd_command, stmt_prepare),
	MYSQLND_METHOD(mysqlnd_command, stmt_execute),
	MYSQLND_METHOD(mysqlnd_command, stmt_fetch),
	MYSQLND_METHOD(mysqlnd_command, stmt_reset),
	MYSQLND_METHOD(mysqlnd_command, stmt_send_long_data),
	MYSQLND_METHOD(mysqlnd_command, stmt_close),
	MYSQLND_METHOD(mysqlnd_command, enable_ssl),
	MYSQLND_METHOD(mysqlnd_command, handshake),
MYSQLND_CLASS_METHODS_END;
