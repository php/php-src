/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
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
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"


struct st_mysqlnd_protocol_no_params_command
{
	struct st_mysqlnd_protocol_no_params_command_context
	{
		MYSQLND_CONN_DATA * conn;
	} context;
};


/************************** COM_SET_OPTION ******************************************/
struct st_mysqlnd_protocol_com_set_option_command
{
	struct st_mysqlnd_com_set_option_context
	{
		MYSQLND_CONN_DATA * conn;
		enum_mysqlnd_server_option option;
	} context;
};


/* {{{ mysqlnd_com_set_option_run */
static enum_func_status
mysqlnd_com_set_option_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_set_option_command * command = (struct st_mysqlnd_protocol_com_set_option_command *) cmd;
	zend_uchar buffer[2];
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	enum_mysqlnd_server_option option = command->context.option;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_set_option_run");
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


/* {{{ mysqlnd_com_set_option_run_command */
static enum_func_status
mysqlnd_com_set_option_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_set_option_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_set_option_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.option = va_arg(args, enum_mysqlnd_server_option);

	ret = mysqlnd_com_set_option_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_DEBUG ******************************************/
/* {{{ mysqlnd_com_debug_run */
static enum_func_status
mysqlnd_com_debug_run(void *cmd)
{
	struct st_mysqlnd_protocol_no_params_command * command = (struct st_mysqlnd_protocol_no_params_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_debug_run");

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


/* {{{ mysqlnd_com_debug_run_command */
static enum_func_status
mysqlnd_com_debug_run_command(va_list args)
{
	struct st_mysqlnd_protocol_no_params_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_debug_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);

	ret = mysqlnd_com_debug_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_INIT_DB ******************************************/
struct st_mysqlnd_protocol_com_init_db_command
{
	struct st_mysqlnd_com_init_db_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING db;
	} context;
};


/* {{{ mysqlnd_com_init_db_run */
static enum_func_status
mysqlnd_com_init_db_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_init_db_command * command = (struct st_mysqlnd_protocol_com_init_db_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	const MYSQLND_CSTRING db = command->context.db;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_init_db_run");

	ret = send_command(conn->payload_decoder_factory, COM_INIT_DB, (zend_uchar*) command->context.db.s, command->context.db.l, FALSE,
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
		if (conn->connect_or_select_db.s) {
			mnd_pefree(conn->connect_or_select_db.s, conn->persistent);
		}
		conn->connect_or_select_db.s = mnd_pestrndup(db.s, db.l, conn->persistent);
		conn->connect_or_select_db.l = db.l;
		if (!conn->connect_or_select_db.s) {
			/* OOM */
			SET_OOM_ERROR(conn->error_info);
			ret = FAIL;
		}
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_init_db_run_command */
static enum_func_status
mysqlnd_com_init_db_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_init_db_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_init_db_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.db = va_arg(args, MYSQLND_CSTRING);

	ret = mysqlnd_com_init_db_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_PING ******************************************/
/* {{{ mysqlnd_com_ping_run */
static enum_func_status
mysqlnd_com_ping_run(void *cmd)
{
	struct st_mysqlnd_protocol_no_params_command * command = (struct st_mysqlnd_protocol_no_params_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_ping_run");

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


/* {{{ mysqlnd_com_ping_run_command */
static enum_func_status
mysqlnd_com_ping_run_command(va_list args)
{
	struct st_mysqlnd_protocol_no_params_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_ping_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);

	ret = mysqlnd_com_ping_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STATISTICS ******************************************/
struct st_mysqlnd_protocol_com_statistics_command
{
	struct st_mysqlnd_com_statistics_context
	{
		MYSQLND_CONN_DATA * conn;
		zend_string ** message;
	} context;
};


/* {{{ mysqlnd_com_statistics_run */
static enum_func_status
mysqlnd_com_statistics_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_statistics_command * command = (struct st_mysqlnd_protocol_com_statistics_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	zend_string **message = command->context.message;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_statistics_run");

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


/* {{{ mysqlnd_com_statistics_run_command */
static enum_func_status
mysqlnd_com_statistics_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_statistics_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_statistics_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.message = va_arg(args, zend_string **);

	ret = mysqlnd_com_statistics_run(&command);

	DBG_RETURN(ret);
}
/* }}} */

/************************** COM_PROCESS_KILL ******************************************/
struct st_mysqlnd_protocol_com_process_kill_command
{
	struct st_mysqlnd_com_process_kill_context
	{
		MYSQLND_CONN_DATA * conn;
		unsigned int process_id;
		zend_bool read_response;
	} context;
};


/* {{{ mysqlnd_com_process_kill_run */
enum_func_status
mysqlnd_com_process_kill_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_process_kill_command * command = (struct st_mysqlnd_protocol_com_process_kill_command *) cmd;
	zend_uchar buff[4];
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	zend_bool read_response = command->context.read_response;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_process_kill_run");
	int4store(buff, command->context.process_id);

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


/* {{{ mysqlnd_com_process_kill_run_command */
static enum_func_status
mysqlnd_com_process_kill_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_process_kill_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_process_kill_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.process_id = va_arg(args, unsigned int);
	command.context.read_response = va_arg(args, unsigned int)? TRUE:FALSE;

	ret = mysqlnd_com_process_kill_run(&command);

	DBG_RETURN(ret);
}
/* }}} */

/************************** COM_REFRESH ******************************************/
struct st_mysqlnd_protocol_com_refresh_command
{
	struct st_mysqlnd_com_refresh_context
	{
		MYSQLND_CONN_DATA * conn;
		uint8_t options;
	} context;
};


/* {{{ mysqlnd_com_refresh_run */
enum_func_status
mysqlnd_com_refresh_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_refresh_command * command = (struct st_mysqlnd_protocol_com_refresh_command *) cmd;
	zend_uchar bits[1];
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_refresh_run");
	int1store(bits, command->context.options);

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


/* {{{ mysqlnd_com_refresh_run_command */
static enum_func_status
mysqlnd_com_refresh_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_refresh_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_refresh_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.options = va_arg(args, unsigned int);

	ret = mysqlnd_com_refresh_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_SHUTDOWN ******************************************/
struct st_mysqlnd_protocol_com_shutdown_command
{
	struct st_mysqlnd_com_shutdown_context
	{
		MYSQLND_CONN_DATA * conn;
		uint8_t level;
	} context;
};


/* {{{ mysqlnd_com_shutdown_run */
enum_func_status
mysqlnd_com_shutdown_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_shutdown_command * command = (struct st_mysqlnd_protocol_com_shutdown_command *) cmd;
	zend_uchar bits[1];
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_shutdown_run");
	int1store(bits, command->context.level);

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


/* {{{ mysqlnd_com_shutdown_run_command */
static enum_func_status
mysqlnd_com_shutdown_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_shutdown_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_shutdown_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.level = va_arg(args, unsigned int);

	ret = mysqlnd_com_shutdown_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_QUIT ******************************************/
struct st_mysqlnd_protocol_com_quit_command
{
	struct st_mysqlnd_com_quit_context
	{
		MYSQLND_CONN_DATA * conn;
	} context;
};


/* {{{ mysqlnd_com_quit_run */
enum_func_status
mysqlnd_com_quit_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_quit_command * command = (struct st_mysqlnd_protocol_com_quit_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_quit_run");

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


/* {{{ mysqlnd_com_quit_run_command */
static enum_func_status
mysqlnd_com_quit_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_quit_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_quit_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);

	ret = mysqlnd_com_quit_run(&command);

	DBG_RETURN(ret);
}
/* }}} */

/************************** COM_QUERY ******************************************/
struct st_mysqlnd_protocol_com_query_command
{
	struct st_mysqlnd_com_query_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING query;
	} context;
};


/* {{{ mysqlnd_com_query_run */
static enum_func_status
mysqlnd_com_query_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_query_command * command = (struct st_mysqlnd_protocol_com_query_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_query_run");

	ret = send_command(conn->payload_decoder_factory, COM_QUERY, (zend_uchar*) command->context.query.s, command->context.query.l, FALSE,
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


/* {{{ mysqlnd_com_query_run_command */
static enum_func_status
mysqlnd_com_query_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_query_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_query_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.query = va_arg(args, MYSQLND_CSTRING);

	ret = mysqlnd_com_query_run(&command);

	DBG_RETURN(ret);
}
/* }}} */

/************************** COM_CHANGE_USER ******************************************/
struct st_mysqlnd_protocol_com_change_user_command
{
	struct st_mysqlnd_com_change_user_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING payload;
		zend_bool silent;
	} context;
};


/* {{{ mysqlnd_com_change_user_run */
static enum_func_status
mysqlnd_com_change_user_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_change_user_command * command = (struct st_mysqlnd_protocol_com_change_user_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_change_user_run");

	ret = send_command(conn->payload_decoder_factory, COM_CHANGE_USER, (zend_uchar*) command->context.payload.s, command->context.payload.l, command->context.silent,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_change_user_run_command */
static enum_func_status
mysqlnd_com_change_user_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_change_user_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_change_user_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.payload = va_arg(args, MYSQLND_CSTRING);
	command.context.silent = va_arg(args, unsigned int);

	ret = mysqlnd_com_change_user_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_REAP_RESULT ******************************************/
struct st_mysqlnd_protocol_com_reap_result_command
{
	struct st_mysqlnd_com_reap_result_context
	{
		MYSQLND_CONN_DATA * conn;
	} context;
};


/* {{{ mysqlnd_com_reap_result_run */
static enum_func_status
mysqlnd_com_reap_result_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_reap_result_command * command = (struct st_mysqlnd_protocol_com_reap_result_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	const enum_mysqlnd_connection_state state = GET_CONNECTION_STATE(&conn->state);

	DBG_ENTER("mysqlnd_com_reap_result_run");
	if (state <= CONN_READY || state == CONN_QUIT_SENT) {
		php_error_docref(NULL, E_WARNING, "Connection not opened, clear or has been closed");
		DBG_ERR_FMT("Connection not opened, clear or has been closed. State=%u", state);
		DBG_RETURN(ret);
	}
	ret = conn->m->query_read_result_set_header(conn, NULL);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_reap_result_run_command */
static enum_func_status
mysqlnd_com_reap_result_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_reap_result_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_reap_result_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);

	ret = mysqlnd_com_reap_result_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STMT_PREPARE ******************************************/
struct st_mysqlnd_protocol_com_stmt_prepare_command
{
	struct st_mysqlnd_com_stmt_prepare_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING query;
	} context;
};


/* {{{ mysqlnd_com_stmt_prepare_run */
static enum_func_status
mysqlnd_com_stmt_prepare_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_stmt_prepare_command * command = (struct st_mysqlnd_protocol_com_stmt_prepare_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_stmt_prepare_run");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_PREPARE, (zend_uchar*) command->context.query.s, command->context.query.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_stmt_prepare_run_command */
static enum_func_status
mysqlnd_com_stmt_prepare_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_stmt_prepare_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_stmt_prepare_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.query = va_arg(args, MYSQLND_CSTRING);

	ret = mysqlnd_com_stmt_prepare_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STMT_EXECUTE ******************************************/
struct st_mysqlnd_protocol_com_stmt_execute_command
{
	struct st_mysqlnd_com_stmt_execute_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING payload;
	} context;
};


/* {{{ mysqlnd_com_stmt_execute_run */
static enum_func_status
mysqlnd_com_stmt_execute_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_stmt_execute_command * command = (struct st_mysqlnd_protocol_com_stmt_execute_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_stmt_execute_run");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_EXECUTE, (zend_uchar*) command->context.payload.s, command->context.payload.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_stmt_execute_run_command */
static enum_func_status
mysqlnd_com_stmt_execute_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_stmt_execute_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_stmt_execute_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.payload = va_arg(args, MYSQLND_CSTRING);

	ret = mysqlnd_com_stmt_execute_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STMT_FETCH ******************************************/
struct st_mysqlnd_protocol_com_stmt_fetch_command
{
	struct st_mysqlnd_com_stmt_fetch_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING payload;
	} context;
};


/* {{{ mysqlnd_com_stmt_fetch_run */
static enum_func_status
mysqlnd_com_stmt_fetch_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_stmt_fetch_command * command = (struct st_mysqlnd_protocol_com_stmt_fetch_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_stmt_fetch_run");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_FETCH, (zend_uchar*) command->context.payload.s, command->context.payload.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_stmt_fetch_run_command */
static enum_func_status
mysqlnd_com_stmt_fetch_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_stmt_fetch_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_stmt_fetch_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.payload = va_arg(args, MYSQLND_CSTRING);

	ret = mysqlnd_com_stmt_fetch_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STMT_RESET ******************************************/
struct st_mysqlnd_protocol_com_stmt_reset_command
{
	struct st_mysqlnd_com_stmt_reset_context
	{
		MYSQLND_CONN_DATA * conn;
		zend_ulong stmt_id;
	} context;
};


/* {{{ mysqlnd_com_stmt_reset_run */
static enum_func_status
mysqlnd_com_stmt_reset_run(void *cmd)
{
	zend_uchar cmd_buf[MYSQLND_STMT_ID_LENGTH /* statement id */];
	struct st_mysqlnd_protocol_com_stmt_reset_command * command = (struct st_mysqlnd_protocol_com_stmt_reset_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response = conn->payload_decoder_factory->m.send_command_handle_response;

	DBG_ENTER("mysqlnd_com_stmt_reset_run");

	int4store(cmd_buf, command->context.stmt_id);
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


/* {{{ mysqlnd_com_stmt_reset_run_command */
static enum_func_status
mysqlnd_com_stmt_reset_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_stmt_reset_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_stmt_reset_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.stmt_id = va_arg(args, size_t);

	ret = mysqlnd_com_stmt_reset_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STMT_SEND_LONG_DATA ******************************************/
struct st_mysqlnd_protocol_com_stmt_send_long_data_command
{
	struct st_mysqlnd_com_stmt_send_long_data_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING payload;
	} context;
};


/* {{{ mysqlnd_com_stmt_send_long_data_run */
static enum_func_status
mysqlnd_com_stmt_send_long_data_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_stmt_send_long_data_command * command = (struct st_mysqlnd_protocol_com_stmt_send_long_data_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_stmt_send_long_data_run");

	ret = send_command(conn->payload_decoder_factory, COM_STMT_SEND_LONG_DATA, (zend_uchar*) command->context.payload.s, command->context.payload.l, FALSE,
					   &conn->state,
					   conn->error_info,
					   conn->upsert_status,
					   conn->stats,
					   conn->m->send_close,
					   conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_com_stmt_send_long_data_run_command */
static enum_func_status
mysqlnd_com_stmt_send_long_data_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_stmt_send_long_data_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_stmt_send_long_data_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.payload = va_arg(args, MYSQLND_CSTRING);

	ret = mysqlnd_com_stmt_send_long_data_run(&command);

	DBG_RETURN(ret);
}
/* }}} */


/************************** COM_STMT_CLOSE ******************************************/
struct st_mysqlnd_protocol_com_stmt_close_command
{
	struct st_mysqlnd_com_stmt_close_context
	{
		MYSQLND_CONN_DATA * conn;
		zend_ulong stmt_id;
	} context;
};


/* {{{ mysqlnd_com_stmt_close_run */
static enum_func_status
mysqlnd_com_stmt_close_run(void *cmd)
{
	zend_uchar cmd_buf[MYSQLND_STMT_ID_LENGTH /* statement id */];
	struct st_mysqlnd_protocol_com_stmt_close_command * command = (struct st_mysqlnd_protocol_com_stmt_close_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command = conn->payload_decoder_factory->m.send_command;

	DBG_ENTER("mysqlnd_com_stmt_close_run");

	int4store(cmd_buf, command->context.stmt_id);
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


/* {{{ mysqlnd_com_stmt_close_run_command */
static enum_func_status
mysqlnd_com_stmt_close_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_stmt_close_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_stmt_close_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.stmt_id = va_arg(args, size_t);

	ret = mysqlnd_com_stmt_close_run(&command);

	DBG_RETURN(ret);
}
/* }}} */



/************************** COM_ENABLE_SSL ******************************************/
struct st_mysqlnd_protocol_com_enable_ssl_command
{
	struct st_mysqlnd_com_enable_ssl_context
	{
		MYSQLND_CONN_DATA * conn;
		size_t client_capabilities;
		size_t server_capabilities;
		unsigned int charset_no;
	} context;
};


/* {{{ mysqlnd_com_enable_ssl_run */
static enum_func_status
mysqlnd_com_enable_ssl_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_enable_ssl_command * command = (struct st_mysqlnd_protocol_com_enable_ssl_command *) cmd;
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = command->context.conn;
	MYSQLND_PACKET_AUTH auth_packet;
	size_t client_capabilities = command->context.client_capabilities;
	size_t server_capabilities = command->context.server_capabilities;

	DBG_ENTER("mysqlnd_com_enable_ssl_run");
	DBG_INF_FMT("client_capability_flags=%lu", client_capabilities);
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

	auth_packet.charset_no	= command->context.charset_no;

#ifdef MYSQLND_SSL_SUPPORTED
	if (client_capabilities & CLIENT_SSL) {
		const zend_bool server_has_ssl = (server_capabilities & CLIENT_SSL)? TRUE:FALSE;
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


/* {{{ mysqlnd_com_enable_ssl_run_command */
static enum_func_status
mysqlnd_com_enable_ssl_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_enable_ssl_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_enable_ssl_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.client_capabilities = va_arg(args, size_t);
	command.context.server_capabilities = va_arg(args, size_t);
	command.context.charset_no = va_arg(args, unsigned int);

	ret = mysqlnd_com_enable_ssl_run(&command);

	DBG_RETURN(ret);
}
/* }}} */

/************************** COM_READ_HANDSHAKE ******************************************/
struct st_mysqlnd_protocol_com_handshake_command
{
	struct st_mysqlnd_com_handshake_context
	{
		MYSQLND_CONN_DATA * conn;
		MYSQLND_CSTRING user;
		MYSQLND_CSTRING passwd;
		MYSQLND_CSTRING database;
		size_t client_flags;
	} context;
};


/* {{{ mysqlnd_com_handshake_run */
static enum_func_status
mysqlnd_com_handshake_run(void *cmd)
{
	struct st_mysqlnd_protocol_com_handshake_command * command = (struct st_mysqlnd_protocol_com_handshake_command *) cmd;
	const char * user = command->context.user.s;

	const char * passwd = command->context.passwd.s;
	size_t passwd_len = command->context.passwd.l;

	const char * db = command->context.database.s;
	size_t db_len = command->context.database.l;

	size_t mysql_flags =  command->context.client_flags;

	MYSQLND_CONN_DATA * conn = command->context.conn;
	MYSQLND_PACKET_GREET greet_packet;

	DBG_ENTER("mysqlnd_conn_data::connect_handshake");
	DBG_INF_FMT("stream=%p", conn->vio->data->m.get_stream(conn->vio));
	DBG_INF_FMT("[user=%s] [db=%s:%d] [flags=%llu]", user, db, db_len, mysql_flags);

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
		DBG_ERR_FMT("Connecting to 3.22, 3.23 & 4.0 is not supported. Server is %-.32s", greet_packet.server_version);
		php_error_docref(NULL, E_WARNING, "Connecting to 3.22, 3.23 & 4.0 "
						" is not supported. Server is %-.32s", greet_packet.server_version);
		SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE,
						 "Connecting to 3.22, 3.23 & 4.0 servers is not supported");
		goto err;
	}

	conn->thread_id			= greet_packet.thread_id;
	conn->protocol_version	= greet_packet.protocol_version;
	conn->server_version	= mnd_pestrdup(greet_packet.server_version, conn->persistent);

	conn->greet_charset = mysqlnd_find_charset_nr(greet_packet.charset_no);
	if (!conn->greet_charset) {
		php_error_docref(NULL, E_WARNING,
			"Server sent charset (%d) unknown to the client. Please, report to the developers", greet_packet.charset_no);
		SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE,
			"Server sent charset unknown to the client. Please, report to the developers");
		goto err;
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


/* {{{ mysqlnd_com_handshake_run_command */
static enum_func_status
mysqlnd_com_handshake_run_command(va_list args)
{
	struct st_mysqlnd_protocol_com_handshake_command command;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_com_handshake_run_command");
	command.context.conn = va_arg(args, MYSQLND_CONN_DATA *);
	command.context.user = *va_arg(args, const MYSQLND_CSTRING *);
	command.context.passwd = *va_arg(args, const MYSQLND_CSTRING *);
	command.context.database = *va_arg(args, const MYSQLND_CSTRING *);
	command.context.client_flags = va_arg(args, size_t);

	ret = mysqlnd_com_handshake_run(&command);

	DBG_RETURN(ret);
}
/* }}} */



/* {{{ _mysqlnd_run_command */
static enum_func_status
_mysqlnd_run_command(enum php_mysqlnd_server_command command, ...)
{
	enum_func_status ret = FAIL;
	va_list args;
	DBG_ENTER("_mysqlnd_run_command");

	va_start(args, command);
	switch (command) {
		case COM_SET_OPTION:
			ret = mysqlnd_com_set_option_run_command(args);
			break;
		case COM_DEBUG:
			ret = mysqlnd_com_debug_run_command(args);
			break;
		case COM_INIT_DB:
			ret = mysqlnd_com_init_db_run_command(args);
			break;
		case COM_PING:
			ret = mysqlnd_com_ping_run_command(args);
			break;
		case COM_STATISTICS:
			ret = mysqlnd_com_statistics_run_command(args);
			break;
		case COM_PROCESS_KILL:
			ret = mysqlnd_com_process_kill_run_command(args);
			break;
		case COM_REFRESH:
			ret = mysqlnd_com_refresh_run_command(args);
			break;
		case COM_SHUTDOWN:
			ret = mysqlnd_com_shutdown_run_command(args);
			break;
		case COM_QUIT:
			ret = mysqlnd_com_quit_run_command(args);
			break;
		case COM_QUERY:
			ret = mysqlnd_com_query_run_command(args);
			break;
		case COM_REAP_RESULT:
			ret = mysqlnd_com_reap_result_run_command(args);
			break;
		case COM_CHANGE_USER:
			ret = mysqlnd_com_change_user_run_command(args);
			break;
		case COM_STMT_PREPARE:
			ret = mysqlnd_com_stmt_prepare_run_command(args);
			break;
		case COM_STMT_EXECUTE:
			ret = mysqlnd_com_stmt_execute_run_command(args);
			break;
		case COM_STMT_FETCH:
			ret = mysqlnd_com_stmt_fetch_run_command(args);
			break;
		case COM_STMT_RESET:
			ret = mysqlnd_com_stmt_reset_run_command(args);
			break;
		case COM_STMT_SEND_LONG_DATA:
			ret = mysqlnd_com_stmt_send_long_data_run_command(args);
			break;
		case COM_STMT_CLOSE:
			ret = mysqlnd_com_stmt_close_run_command(args);
			break;
		case COM_ENABLE_SSL:
			ret = mysqlnd_com_enable_ssl_run_command(args);
			break;
		case COM_HANDSHAKE:
			ret = mysqlnd_com_handshake_run_command(args);
			break;
		default:
			break;
	}
	va_end(args);
	DBG_RETURN(ret);
}
/* }}} */

func_mysqlnd__run_command mysqlnd_run_command = _mysqlnd_run_command;

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
