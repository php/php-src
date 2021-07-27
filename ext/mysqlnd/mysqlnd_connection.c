/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
#include "mysqlnd_vio.h"
#include "mysqlnd_protocol_frame_codec.h"
#include "mysqlnd_auth.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_ext_plugin.h"
#include "zend_smart_str.h"


extern MYSQLND_CHARSET *mysqlnd_charsets;

PHPAPI const char * const mysqlnd_server_gone = "MySQL server has gone away";
PHPAPI const char * const mysqlnd_out_of_sync = "Commands out of sync; you can't run this command now";
PHPAPI const char * const mysqlnd_out_of_memory = "Out of memory";

PHPAPI MYSQLND_STATS * mysqlnd_global_stats = NULL;


/* {{{ mysqlnd_upsert_status::reset */
void
MYSQLND_METHOD(mysqlnd_upsert_status, reset)(MYSQLND_UPSERT_STATUS * const upsert_status)
{
	upsert_status->warning_count = 0;
	upsert_status->server_status = 0;
	upsert_status->affected_rows = 0;
	upsert_status->last_insert_id = 0;
}
/* }}} */


/* {{{ mysqlnd_upsert_status::set_affected_rows_to_error */
void
MYSQLND_METHOD(mysqlnd_upsert_status, set_affected_rows_to_error)(MYSQLND_UPSERT_STATUS * upsert_status)
{
	upsert_status->affected_rows = (uint64_t) ~0;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_upsert_status)
	MYSQLND_METHOD(mysqlnd_upsert_status, reset),
	MYSQLND_METHOD(mysqlnd_upsert_status, set_affected_rows_to_error),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_upsert_status_init */
void
mysqlnd_upsert_status_init(MYSQLND_UPSERT_STATUS * const upsert_status)
{
	upsert_status->m = &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_upsert_status);
	upsert_status->m->reset(upsert_status);
}
/* }}} */


/* {{{ mysqlnd_error_list_pdtor */
static void
mysqlnd_error_list_pdtor(void * pDest)
{
	MYSQLND_ERROR_LIST_ELEMENT * element = (MYSQLND_ERROR_LIST_ELEMENT *) pDest;

	DBG_ENTER("mysqlnd_error_list_pdtor");
	if (element->error) {
		mnd_pefree(element->error, TRUE);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_error_info::reset */
static void
MYSQLND_METHOD(mysqlnd_error_info, reset)(MYSQLND_ERROR_INFO * const info)
{
	DBG_ENTER("mysqlnd_error_info::reset");

	info->error_no = 0;
	info->error[0] = '\0';
	memset(&info->sqlstate, 0, sizeof(info->sqlstate));
	zend_llist_clean(&info->error_list);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_error_info::set_client_error */
static void
MYSQLND_METHOD(mysqlnd_error_info, set_client_error)(MYSQLND_ERROR_INFO * const info,
													 const unsigned int err_no,
													 const char * const sqlstate,
													 const char * const error)
{
	DBG_ENTER("mysqlnd_error_info::set_client_error");
	if (err_no) {
		MYSQLND_ERROR_LIST_ELEMENT error_for_the_list = {0};

		info->error_no = err_no;
		strlcpy(info->sqlstate, sqlstate, sizeof(info->sqlstate));
		strlcpy(info->error, error, sizeof(info->error));

		error_for_the_list.error_no = err_no;
		strlcpy(error_for_the_list.sqlstate, sqlstate, sizeof(error_for_the_list.sqlstate));
		error_for_the_list.error = mnd_pestrdup(error, TRUE);
		if (error_for_the_list.error) {
			DBG_INF_FMT("adding error [%s] to the list", error_for_the_list.error);
			zend_llist_add_element(&info->error_list, &error_for_the_list);
		}
	} else {
		info->m->reset(info);
	}
	DBG_VOID_RETURN;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_error_info)
	MYSQLND_METHOD(mysqlnd_error_info, reset),
	MYSQLND_METHOD(mysqlnd_error_info, set_client_error),
MYSQLND_CLASS_METHODS_END;



/* {{{ mysqlnd_error_info_init */
PHPAPI enum_func_status
mysqlnd_error_info_init(MYSQLND_ERROR_INFO * const info, const zend_bool persistent)
{
	DBG_ENTER("mysqlnd_error_info_init");
	info->m = mysqlnd_error_info_get_methods();
	info->m->reset(info);

	zend_llist_init(&info->error_list, sizeof(MYSQLND_ERROR_LIST_ELEMENT), (llist_dtor_func_t) mysqlnd_error_list_pdtor, persistent);
	info->persistent = persistent;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_error_info_free_contents */
PHPAPI void
mysqlnd_error_info_free_contents(MYSQLND_ERROR_INFO * const info)
{
	DBG_ENTER("mysqlnd_error_info_free_contents");
	info->m->reset(info);
	DBG_VOID_RETURN;
}
/* }}} */




/* {{{ mysqlnd_connection_state::get */
static enum mysqlnd_connection_state
MYSQLND_METHOD(mysqlnd_connection_state, get)(const struct st_mysqlnd_connection_state * const state_struct)
{
	DBG_ENTER("mysqlnd_connection_state::get");
	DBG_INF_FMT("State=%u", state_struct->state);
	DBG_RETURN(state_struct->state);
}
/* }}} */


/* {{{ mysqlnd_connection_state::set */
static void
MYSQLND_METHOD(mysqlnd_connection_state, set)(struct st_mysqlnd_connection_state * const state_struct, const enum mysqlnd_connection_state state)
{
	DBG_ENTER("mysqlnd_connection_state::set");
	DBG_INF_FMT("New state=%u", state);
	state_struct->state = state;
	DBG_VOID_RETURN;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_connection_state)
	MYSQLND_METHOD(mysqlnd_connection_state, get),
	MYSQLND_METHOD(mysqlnd_connection_state, set),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_connection_state_init */
PHPAPI void
mysqlnd_connection_state_init(struct st_mysqlnd_connection_state * const state)
{
	DBG_ENTER("mysqlnd_connection_state_init");
	state->m = &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_connection_state);
	state->state = CONN_ALLOCED;
	DBG_VOID_RETURN;
}
/* }}} */



/* {{{ mysqlnd_conn_data::free_options */
static void
MYSQLND_METHOD(mysqlnd_conn_data, free_options)(MYSQLND_CONN_DATA * conn)
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
	if (conn->options->connect_attr) {
		zend_hash_destroy(conn->options->connect_attr);
		mnd_pefree(conn->options->connect_attr, pers);
		conn->options->connect_attr = NULL;
	}
}
/* }}} */


/* {{{ mysqlnd_conn_data::free_contents */
static void
MYSQLND_METHOD(mysqlnd_conn_data, free_contents)(MYSQLND_CONN_DATA * conn)
{
	zend_bool pers = conn->persistent;

	DBG_ENTER("mysqlnd_conn_data::free_contents");

	if (conn->current_result) {
		conn->current_result->m.free_result(conn->current_result, TRUE);
		conn->current_result = NULL;
	}

	if (conn->protocol_frame_codec) {
		conn->protocol_frame_codec->data->m.free_contents(conn->protocol_frame_codec);
	}

	if (conn->vio) {
		conn->vio->data->m.free_contents(conn->vio);
	}

	DBG_INF("Freeing memory of members");

	if (conn->hostname.s) {
		mnd_pefree(conn->hostname.s, pers);
		conn->hostname.s = NULL;
	}
	if (conn->username.s) {
		mnd_pefree(conn->username.s, pers);
		conn->username.s = NULL;
	}
	if (conn->password.s) {
		mnd_pefree(conn->password.s, pers);
		conn->password.s = NULL;
	}
	if (conn->connect_or_select_db.s) {
		mnd_pefree(conn->connect_or_select_db.s, pers);
		conn->connect_or_select_db.s = NULL;
	}
	if (conn->unix_socket.s) {
		mnd_pefree(conn->unix_socket.s, pers);
		conn->unix_socket.s = NULL;
	}
	DBG_INF_FMT("scheme=%s", conn->scheme.s);
	if (conn->scheme.s) {
		mnd_pefree(conn->scheme.s, pers);
		conn->scheme.s = NULL;
	}
	if (conn->server_version) {
		mnd_pefree(conn->server_version, pers);
		conn->server_version = NULL;
	}
	if (conn->host_info) {
		mnd_pefree(conn->host_info, pers);
		conn->host_info = NULL;
	}
	if (conn->authentication_plugin_data.s) {
		mnd_pefree(conn->authentication_plugin_data.s, pers);
		conn->authentication_plugin_data.s = NULL;
	}
	if (conn->last_message.s) {
		mnd_efree(conn->last_message.s);
		conn->last_message.s = NULL;
	}

	conn->charset = NULL;
	conn->greet_charset = NULL;

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::dtor */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, dtor)(MYSQLND_CONN_DATA * conn)
{
	DBG_ENTER("mysqlnd_conn_data::dtor");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	conn->m->free_contents(conn);
	conn->m->free_options(conn);

	if (conn->error_info) {
		mysqlnd_error_info_free_contents(conn->error_info);
		conn->error_info = NULL;
	}

	if (conn->protocol_frame_codec) {
		mysqlnd_pfc_free(conn->protocol_frame_codec, conn->stats, conn->error_info);
		conn->protocol_frame_codec = NULL;
	}

	if (conn->vio) {
		mysqlnd_vio_free(conn->vio, conn->stats, conn->error_info);
		conn->vio = NULL;
	}

	if (conn->payload_decoder_factory) {
		mysqlnd_protocol_payload_decoder_factory_free(conn->payload_decoder_factory);
		conn->payload_decoder_factory = NULL;
	}

	if (conn->stats) {
		mysqlnd_stats_end(conn->stats, conn->persistent);
	}

	mnd_pefree(conn, conn->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_server_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_server_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_server_option option)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), set_server_option);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::set_server_option");
	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->command->set_option(conn, option);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::restart_psession */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, restart_psession)(MYSQLND_CONN_DATA * conn)
{
	DBG_ENTER("mysqlnd_conn_data::restart_psession");
	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CONNECT_REUSED);
	conn->current_result = NULL;
	conn->last_message.s = NULL;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_conn_data::end_psession */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, end_psession)(MYSQLND_CONN_DATA * conn)
{
	DBG_ENTER("mysqlnd_conn_data::end_psession");
	/* Free here what should not be seen by the next script */
	if (conn->current_result) {
		conn->current_result->m.free_result(conn->current_result, TRUE);
		conn->current_result = NULL;
	}
	if (conn->last_message.s) {
		mnd_efree(conn->last_message.s);
		conn->last_message.s = NULL;
	}
	conn->error_info = &conn->error_info_impl;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_conn_data::fetch_auth_plugin_by_name */
static struct st_mysqlnd_authentication_plugin *
MYSQLND_METHOD(mysqlnd_conn_data, fetch_auth_plugin_by_name)(const char * const requested_protocol)
{
	struct st_mysqlnd_authentication_plugin * auth_plugin;
	char * plugin_name = NULL;
	DBG_ENTER("mysqlnd_conn_data::fetch_auth_plugin_by_name");

	mnd_sprintf(&plugin_name, 0, "auth_plugin_%s", requested_protocol);
	DBG_INF_FMT("looking for %s auth plugin", plugin_name);
	auth_plugin = mysqlnd_plugin_find(plugin_name);
	mnd_sprintf_free(plugin_name);

	DBG_RETURN(auth_plugin);
}
/* }}} */


/* {{{ mysqlnd_conn_data::execute_init_commands */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, execute_init_commands)(MYSQLND_CONN_DATA * conn)
{
	enum_func_status ret = PASS;

	DBG_ENTER("mysqlnd_conn_data::execute_init_commands");
	if (conn->options->init_commands) {
		unsigned int current_command = 0;
		for (; current_command < conn->options->num_commands; ++current_command) {
			const char * const command = conn->options->init_commands[current_command];
			if (command) {
				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_INIT_COMMAND_EXECUTED_COUNT);
				if (PASS != conn->m->query(conn, command, strlen(command))) {
					MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_INIT_COMMAND_FAILED_COUNT);
					ret = FAIL;
					break;
				}
				do {
					if (conn->last_query_type == QUERY_SELECT) {
						MYSQLND_RES * result = conn->m->use_result(conn, 0);
						if (result) {
							result->m.free_result(result, TRUE);
						}
					}
				} while (conn->m->next_result(conn) != FAIL);
			}
		}
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_updated_connect_flags */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, get_updated_connect_flags)(MYSQLND_CONN_DATA * conn, unsigned int mysql_flags)
{
#ifdef MYSQLND_COMPRESSION_ENABLED
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
#endif
	MYSQLND_VIO * vio = conn->vio;

	DBG_ENTER("mysqlnd_conn_data::get_updated_connect_flags");
	/* allow CLIENT_LOCAL_FILES capability, although extensions basing on mysqlnd
		shouldn't allow 'load data local infile' by default due to security issues */
	mysql_flags |= MYSQLND_CAPABILITIES;

	mysql_flags |= conn->options->flags; /* use the flags from set_client_option() */

#ifndef MYSQLND_COMPRESSION_ENABLED
	if (mysql_flags & CLIENT_COMPRESS) {
		mysql_flags &= ~CLIENT_COMPRESS;
	}
#else
	if (pfc && pfc->data->flags & MYSQLND_PROTOCOL_FLAG_USE_COMPRESSION) {
		mysql_flags |= CLIENT_COMPRESS;
	}
#endif
#ifndef MYSQLND_SSL_SUPPORTED
	if (mysql_flags & CLIENT_SSL) {
		mysql_flags &= ~CLIENT_SSL;
	}
#else
	if (vio && (vio->data->options.ssl_key ||
				vio->data->options.ssl_cert ||
				vio->data->options.ssl_ca ||
				vio->data->options.ssl_capath ||
				vio->data->options.ssl_cipher))
	{
		mysql_flags |= CLIENT_SSL;
	}
#endif

    if (conn->options->connect_attr && zend_hash_num_elements(conn->options->connect_attr)) {
		mysql_flags |= CLIENT_CONNECT_ATTRS;
	}

	DBG_RETURN(mysql_flags);
}
/* }}} */


/* {{{ mysqlnd_conn_data::connect_handshake */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, connect_handshake)(MYSQLND_CONN_DATA * conn,
						const MYSQLND_CSTRING * const scheme,
						const MYSQLND_CSTRING * const username,
						const MYSQLND_CSTRING * const password,
						const MYSQLND_CSTRING * const database,
						const unsigned int mysql_flags)
{
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::connect_handshake");

	if (PASS == conn->vio->data->m.connect(conn->vio, *scheme, conn->persistent, conn->stats, conn->error_info) &&
		PASS == conn->protocol_frame_codec->data->m.reset(conn->protocol_frame_codec, conn->stats, conn->error_info))
	{
		size_t client_flags = mysql_flags;

		ret = conn->command->handshake(conn, *username, *password, *database, client_flags);
	}
	DBG_RETURN(ret);
}
/* }}} */

/* {{{ mysqlnd_conn_data::get_scheme */
static MYSQLND_STRING
MYSQLND_METHOD(mysqlnd_conn_data, get_scheme)(MYSQLND_CONN_DATA * conn, MYSQLND_CSTRING hostname, MYSQLND_CSTRING *socket_or_pipe, unsigned int port, zend_bool * unix_socket, zend_bool * named_pipe)
{
	MYSQLND_STRING transport;
	DBG_ENTER("mysqlnd_conn_data::get_scheme");
#ifndef PHP_WIN32
	if (hostname.l == sizeof("localhost") - 1 && !strncasecmp(hostname.s, "localhost", hostname.l)) {
		DBG_INF_FMT("socket=%s", socket_or_pipe->s? socket_or_pipe->s:"n/a");
		if (!socket_or_pipe->s) {
			socket_or_pipe->s = "/tmp/mysql.sock";
			socket_or_pipe->l = strlen(socket_or_pipe->s);
		}
		transport.l = mnd_sprintf(&transport.s, 0, "unix://%s", socket_or_pipe->s);
		*unix_socket = TRUE;
#else
	if (hostname.l == sizeof(".") - 1 && hostname.s[0] == '.') {
		/* named pipe in socket */
		if (!socket_or_pipe->s) {
			socket_or_pipe->s = "\\\\.\\pipe\\MySQL";
			socket_or_pipe->l = strlen(socket_or_pipe->s);
		}
		transport.l = mnd_sprintf(&transport.s, 0, "pipe://%s", socket_or_pipe->s);
		*named_pipe = TRUE;
#endif
	} else {
		if (!port) {
			port = 3306;
		}
		transport.l = mnd_sprintf(&transport.s, 0, "tcp://%s:%u", hostname.s, port);
	}
	DBG_INF_FMT("transport=%s", transport.s? transport.s:"OOM");
	DBG_RETURN(transport);
}
/* }}} */


/* {{{ mysqlnd_conn_data::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, connect)(MYSQLND_CONN_DATA * conn,
						MYSQLND_CSTRING hostname,
						MYSQLND_CSTRING username,
						MYSQLND_CSTRING password,
						MYSQLND_CSTRING database,
						unsigned int port,
						MYSQLND_CSTRING socket_or_pipe,
						unsigned int mysql_flags
					)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), connect);
	zend_bool unix_socket = FALSE;
	zend_bool named_pipe = FALSE;
	zend_bool reconnect = FALSE;
	zend_bool saved_compression = FALSE;
	zend_bool local_tx_started = FALSE;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_STRING transport = { NULL, 0 };

	DBG_ENTER("mysqlnd_conn_data::connect");
	DBG_INF_FMT("conn=%p", conn);

	if (PASS != conn->m->local_tx_start(conn, this_func)) {
		goto err;
	}
	local_tx_started = TRUE;

	SET_EMPTY_ERROR(conn->error_info);
	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);

	DBG_INF_FMT("host=%s user=%s db=%s port=%u flags=%u persistent=%u state=%u",
				hostname.s?hostname.s:"", username.s?username.s:"", database.s?database.s:"", port, mysql_flags,
				conn? conn->persistent:0, conn? (int)GET_CONNECTION_STATE(&conn->state):-1);

	if (GET_CONNECTION_STATE(&conn->state) > CONN_ALLOCED) {
		DBG_INF("Connecting on a connected handle.");

		if (GET_CONNECTION_STATE(&conn->state) < CONN_QUIT_SENT) {
			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CLOSE_IMPLICIT);
			reconnect = TRUE;
			conn->m->send_close(conn);
		}

		conn->m->free_contents(conn);
		/* Now reconnect using the same handle */
		if (pfc->data->compressed) {
			/*
			  we need to save the state. As we will re-connect, pfc->compressed should be off, or
			  we will look for a compression header as part of the greet message, but there will
			  be none.
			*/
			saved_compression = TRUE;
			pfc->data->compressed = FALSE;
		}
		if (pfc->data->ssl) {
			pfc->data->ssl = FALSE;
		}
	} else {
		unsigned int max_allowed_size = MYSQLND_ASSEMBLED_PACKET_MAX_SIZE;
		conn->m->set_client_option(conn, MYSQLND_OPT_MAX_ALLOWED_PACKET, (char *)&max_allowed_size);
	}

	if (!hostname.s || !hostname.s[0]) {
		hostname.s = "localhost";
		hostname.l = strlen(hostname.s);
	}
	if (!username.s) {
		DBG_INF_FMT("no user given, using empty string");
		username.s = "";
		username.l = 0;
	}
	if (!password.s) {
		DBG_INF_FMT("no password given, using empty string");
		password.s = "";
		password.l = 0;
	}
	if (!database.s || !database.s[0]) {
		DBG_INF_FMT("no db given, using empty string");
		database.s = "";
		database.l = 0;
	} else {
		mysql_flags |= CLIENT_CONNECT_WITH_DB;
	}

	transport = conn->m->get_scheme(conn, hostname, &socket_or_pipe, port, &unix_socket, &named_pipe);

	mysql_flags = conn->m->get_updated_connect_flags(conn, mysql_flags);

	{
		const MYSQLND_CSTRING scheme = { transport.s, transport.l };
		if (FAIL == conn->m->connect_handshake(conn, &scheme, &username, &password, &database, mysql_flags)) {
			goto err;
		}
	}

	{
		SET_CONNECTION_STATE(&conn->state, CONN_READY);

		if (saved_compression) {
			pfc->data->compressed = TRUE;
		}
		/*
		  If a connect on a existing handle is performed and mysql_flags is
		  passed which doesn't CLIENT_COMPRESS, then we need to overwrite the value
		  which we set based on saved_compression.
		*/
		pfc->data->compressed = mysql_flags & CLIENT_COMPRESS? TRUE:FALSE;


		conn->scheme.s = mnd_pestrndup(transport.s, transport.l, conn->persistent);
		conn->scheme.l = transport.l;
		if (transport.s) {
			mnd_sprintf_free(transport.s);
			transport.s = NULL;
		}

		if (!conn->scheme.s) {
			goto err; /* OOM */
		}

		conn->username.l		= username.l;
		conn->username.s		= mnd_pestrndup(username.s, conn->username.l, conn->persistent);
		conn->password.l		= password.l;
		conn->password.s		= mnd_pestrndup(password.s, conn->password.l, conn->persistent);
		conn->port				= port;
		conn->connect_or_select_db.l = database.l;
		conn->connect_or_select_db.s = mnd_pestrndup(database.s, conn->connect_or_select_db.l, conn->persistent);

		if (!conn->username.s || !conn->password.s|| !conn->connect_or_select_db.s) {
			SET_OOM_ERROR(conn->error_info);
			goto err; /* OOM */
		}

		if (!unix_socket && !named_pipe) {
			conn->hostname.s = mnd_pestrndup(hostname.s, hostname.l, conn->persistent);
			if (!conn->hostname.s) {
				SET_OOM_ERROR(conn->error_info);
				goto err; /* OOM */
			}
			conn->hostname.l = hostname.l;
			{
				char *p;
				mnd_sprintf(&p, 0, "%s via TCP/IP", conn->hostname.s);
				if (!p) {
					SET_OOM_ERROR(conn->error_info);
					goto err; /* OOM */
				}
				conn->host_info = mnd_pestrdup(p, conn->persistent);
				mnd_sprintf_free(p);
				if (!conn->host_info) {
					SET_OOM_ERROR(conn->error_info);
					goto err; /* OOM */
				}
			}
		} else {
			conn->unix_socket.s = mnd_pestrdup(socket_or_pipe.s, conn->persistent);
			if (unix_socket) {
				conn->host_info = mnd_pestrdup("Localhost via UNIX socket", conn->persistent);
			} else if (named_pipe) {
				char *p;
				mnd_sprintf(&p, 0, "%s via named pipe", conn->unix_socket.s);
				if (!p) {
					SET_OOM_ERROR(conn->error_info);
					goto err; /* OOM */
				}
				conn->host_info =  mnd_pestrdup(p, conn->persistent);
				mnd_sprintf_free(p);
				if (!conn->host_info) {
					SET_OOM_ERROR(conn->error_info);
					goto err; /* OOM */
				}
			} else {
				php_error_docref(NULL, E_WARNING, "Impossible. Should be either socket or a pipe. Report a bug!");
			}
			if (!conn->unix_socket.s || !conn->host_info) {
				SET_OOM_ERROR(conn->error_info);
				goto err; /* OOM */
			}
			conn->unix_socket.l = strlen(conn->unix_socket.s);
		}

		SET_EMPTY_ERROR(conn->error_info);

		mysqlnd_local_infile_default(conn);

		if (FAIL == conn->m->execute_init_commands(conn)) {
			goto err;
		}

		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats, STAT_CONNECT_SUCCESS, 1, STAT_OPENED_CONNECTIONS, 1);
		if (reconnect) {
			MYSQLND_INC_GLOBAL_STATISTIC(STAT_RECONNECT);
		}
		if (conn->persistent) {
			MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats, STAT_PCONNECT_SUCCESS, 1, STAT_OPENED_PERSISTENT_CONNECTIONS, 1);
		}

		DBG_INF_FMT("connection_id=%llu", conn->thread_id);

		conn->m->local_tx_end(conn, this_func, PASS);
		DBG_RETURN(PASS);
	}
err:
	if (transport.s) {
		mnd_sprintf_free(transport.s);
		transport.s = NULL;
	}

	DBG_ERR_FMT("[%u] %.128s (trying to connect via %s)", conn->error_info->error_no, conn->error_info->error, conn->scheme.s);
	if (!conn->error_info->error_no) {
		SET_CLIENT_ERROR(conn->error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, conn->error_info->error);
		php_error_docref(NULL, E_WARNING, "[%u] %.128s (trying to connect via %s)", conn->error_info->error_no, conn->error_info->error, conn->scheme.s);
	}

	conn->m->free_contents(conn);
	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CONNECT_FAILURE);
	if (TRUE == local_tx_started) {
		conn->m->local_tx_end(conn, this_func, FAIL);
	}

	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_conn::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, connect)(MYSQLND * conn_handle,
						const MYSQLND_CSTRING hostname,
						const MYSQLND_CSTRING username,
						const MYSQLND_CSTRING password,
						const MYSQLND_CSTRING database,
						unsigned int port,
						const MYSQLND_CSTRING socket_or_pipe,
						unsigned int mysql_flags)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), connect);
	enum_func_status ret = FAIL;
	MYSQLND_CONN_DATA * conn = conn_handle->data;

	DBG_ENTER("mysqlnd_conn::connect");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		mysqlnd_options4(conn_handle, MYSQL_OPT_CONNECT_ATTR_ADD, "_client_name", "mysqlnd");
        if (hostname.l > 0) {
            mysqlnd_options4(conn_handle, MYSQL_OPT_CONNECT_ATTR_ADD, "_server_host", hostname.s);
        }
		ret = conn->m->connect(conn, hostname, username, password, database, port, socket_or_pipe, mysql_flags);

		conn->m->local_tx_end(conn, this_func, FAIL);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::query */
/*
  If conn->error_info->error_no is not zero, then we had an error.
  Still the result from the query is PASS
*/
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, query)(MYSQLND_CONN_DATA * conn, const char * const query, const size_t query_len)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), query);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::query");
	DBG_INF_FMT("conn=%p conn=%llu query=%s", conn, conn->thread_id, query);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		if (PASS == conn->m->send_query(conn, query, query_len, MYSQLND_SEND_QUERY_IMPLICIT, NULL, NULL) &&
			PASS == conn->m->reap_query(conn, MYSQLND_REAP_RESULT_IMPLICIT))
		{
			ret = PASS;
			if (conn->last_query_type == QUERY_UPSERT && UPSERT_STATUS_GET_AFFECTED_ROWS(conn->upsert_status)) {
				MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats, STAT_ROWS_AFFECTED_NORMAL, UPSERT_STATUS_GET_AFFECTED_ROWS(conn->upsert_status));
			}
		}
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::send_query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, send_query)(MYSQLND_CONN_DATA * conn, const char * const query, const size_t query_len,
											  enum_mysqlnd_send_query_type type, zval *read_cb, zval *err_cb)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), send_query);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::send_query");
	DBG_INF_FMT("conn=%llu query=%s", conn->thread_id, query);
	DBG_INF_FMT("conn->server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status));

	if (type == MYSQLND_SEND_QUERY_IMPLICIT || PASS == conn->m->local_tx_start(conn, this_func))
	{
		const MYSQLND_CSTRING query_string = {query, query_len};

		ret = conn->command->query(conn, query_string);

		if (type == MYSQLND_SEND_QUERY_EXPLICIT) {
			conn->m->local_tx_end(conn, this_func, ret);
		}
	}
	DBG_INF_FMT("conn->server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status));
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::reap_query */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, reap_query)(MYSQLND_CONN_DATA * conn, enum_mysqlnd_reap_result_type type)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), reap_query);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::reap_query");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	DBG_INF_FMT("conn->server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status));
	if (type == MYSQLND_REAP_RESULT_IMPLICIT || PASS == conn->m->local_tx_start(conn, this_func))
	{
		ret = conn->command->reap_result(conn);

		if (type == MYSQLND_REAP_RESULT_EXPLICIT) {
			conn->m->local_tx_end(conn, this_func, ret);
		}
	}
	DBG_INF_FMT("conn->server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status));
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::list_method */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, list_method)(MYSQLND_CONN_DATA * conn, const char * const query, const char * const achtung_wild, const char * const par1)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), list_method);
	char * show_query = NULL;
	size_t show_query_len;
	MYSQLND_RES * result = NULL;

	DBG_ENTER("mysqlnd_conn_data::list_method");
	DBG_INF_FMT("conn=%llu query=%s wild=%u", conn->thread_id, query, achtung_wild);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
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

		if (PASS == conn->m->query(conn, show_query, show_query_len)) {
			result = conn->m->store_result(conn, MYSQLND_STORE_NO_COPY);
		}
		if (show_query != query) {
			mnd_sprintf_free(show_query);
		}
		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS);
	}
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::err_no */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, err_no)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->error_info->error_no;
}
/* }}} */


/* {{{ mysqlnd_conn_data::error */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, error)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->error_info->error;
}
/* }}} */


/* {{{ mysqlnd_conn_data::sqlstate */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, sqlstate)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->error_info->sqlstate[0] ? conn->error_info->sqlstate:MYSQLND_SQLSTATE_NULL;
}
/* }}} */


/* {{{ mysqlnd_old_escape_string */
PHPAPI  zend_ulong
mysqlnd_old_escape_string(char * newstr, const char * escapestr, size_t escapestr_len)
{
	DBG_ENTER("mysqlnd_old_escape_string");
	DBG_RETURN(mysqlnd_cset_escape_slashes(mysqlnd_find_charset_name("latin1"), newstr, escapestr, escapestr_len));
}
/* }}} */


/* {{{ mysqlnd_conn_data::ssl_set */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, ssl_set)(MYSQLND_CONN_DATA * const conn, const char * key, const char * const cert,
									  const char * const ca, const char * const capath, const char * const cipher)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), ssl_set);
	enum_func_status ret = FAIL;
	MYSQLND_VIO * vio = conn->vio;
	DBG_ENTER("mysqlnd_conn_data::ssl_set");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = (PASS == vio->data->m.set_client_option(vio, MYSQLND_OPT_SSL_KEY, key) &&
			PASS == vio->data->m.set_client_option(vio, MYSQLND_OPT_SSL_CERT, cert) &&
			PASS == vio->data->m.set_client_option(vio, MYSQLND_OPT_SSL_CA, ca) &&
			PASS == vio->data->m.set_client_option(vio, MYSQLND_OPT_SSL_CAPATH, capath) &&
			PASS == vio->data->m.set_client_option(vio, MYSQLND_OPT_SSL_CIPHER, cipher)) ? PASS : FAIL;

		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::escape_string */
static zend_ulong
MYSQLND_METHOD(mysqlnd_conn_data, escape_string)(MYSQLND_CONN_DATA * const conn, char * newstr, const char * escapestr, size_t escapestr_len)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), escape_string);
	zend_ulong ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::escape_string");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		DBG_INF_FMT("server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status));
		if (UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & SERVER_STATUS_NO_BACKSLASH_ESCAPES) {
			ret = mysqlnd_cset_escape_quotes(conn->charset, newstr, escapestr, escapestr_len);
		} else {
			ret = mysqlnd_cset_escape_slashes(conn->charset, newstr, escapestr, escapestr_len);
		}
		conn->m->local_tx_end(conn, this_func, PASS);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::dump_debug_info */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, dump_debug_info)(MYSQLND_CONN_DATA * const conn)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), server_dump_debug_information);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::dump_debug_info");
	DBG_INF_FMT("conn=%llu", conn->thread_id);
	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->command->debug(conn);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::select_db */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, select_db)(MYSQLND_CONN_DATA * const conn, const char * const db, const size_t db_len)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), select_db);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::select_db");
	DBG_INF_FMT("conn=%llu db=%s", conn->thread_id, db);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		const MYSQLND_CSTRING database = {db, db_len};

		ret = conn->command->init_db(conn, database);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::ping */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, ping)(MYSQLND_CONN_DATA * const conn)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), ping);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::ping");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->command->ping(conn);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_INF_FMT("ret=%u", ret);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::statistic */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, statistic)(MYSQLND_CONN_DATA * conn, zend_string **message)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), get_server_statistics);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::statistic");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->command->statistics(conn, message);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::kill */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, kill)(MYSQLND_CONN_DATA * conn, unsigned int pid)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), kill_connection);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::kill");
	DBG_INF_FMT("conn=%llu pid=%u", conn->thread_id, pid);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		const unsigned int process_id = pid;
		/* 'unsigned char' is promoted to 'int' when passed through '...' */
		const unsigned int read_response = (pid != conn->thread_id);

		ret = conn->command->process_kill(conn, process_id, read_response);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_charset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_charset)(MYSQLND_CONN_DATA * const conn, const char * const csname)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), set_charset);
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * const charset = mysqlnd_find_charset_name(csname);

	DBG_ENTER("mysqlnd_conn_data::set_charset");
	DBG_INF_FMT("conn=%llu cs=%s", conn->thread_id, csname);

	if (!charset) {
		SET_CLIENT_ERROR(conn->error_info, CR_CANT_FIND_CHARSET, UNKNOWN_SQLSTATE,
						 "Invalid characterset or character set not supported");
		DBG_RETURN(ret);
	}

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		char * query;
		size_t query_len = mnd_sprintf(&query, 0, "SET NAMES %s", csname);

		if (FAIL == (ret = conn->m->query(conn, query, query_len))) {
			php_error_docref(NULL, E_WARNING, "Error executing query");
		} else if (conn->error_info->error_no) {
			ret = FAIL;
		} else {
			conn->charset = charset;
		}
		mnd_sprintf_free(query);

		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::refresh */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, refresh)(MYSQLND_CONN_DATA * const conn, uint8_t options)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), refresh_server);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::refresh");
	DBG_INF_FMT("conn=%llu options=%lu", conn->thread_id, options);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->command->refresh(conn, options);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::shutdown */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, shutdown)(MYSQLND_CONN_DATA * const conn, uint8_t level)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), shutdown_server);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::shutdown");
	DBG_INF_FMT("conn=%llu level=%lu", conn->thread_id, level);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->command->shutdown(conn, level);
		conn->m->local_tx_end(conn, this_func, ret);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_send_close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, send_close)(MYSQLND_CONN_DATA * const conn)
{
	enum_func_status ret = PASS;
	MYSQLND_VIO * vio = conn->vio;
	php_stream * net_stream = vio->data->m.get_stream(vio);
	enum mysqlnd_connection_state state = GET_CONNECTION_STATE(&conn->state);

	DBG_ENTER("mysqlnd_send_close");
	DBG_INF_FMT("conn=%llu vio->data->stream->abstract=%p", conn->thread_id, net_stream? net_stream->abstract:NULL);
	DBG_INF_FMT("state=%u", state);

	if (state >= CONN_READY) {
		MYSQLND_DEC_GLOBAL_STATISTIC(STAT_OPENED_CONNECTIONS);
		if (conn->persistent) {
			MYSQLND_DEC_GLOBAL_STATISTIC(STAT_OPENED_PERSISTENT_CONNECTIONS);
		}
	}
	switch (state) {
		case CONN_READY:
			DBG_INF("Connection clean, sending COM_QUIT");
			if (net_stream) {
				ret = conn->command->quit(conn);
				vio->data->m.close_stream(vio, conn->stats, conn->error_info);
			}
			SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
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
			DBG_ERR_FMT("Brutally closing connection [%p][%s]", conn, conn->scheme.s);
			/*
			  Do nothing, the connection will be brutally closed
			  and the server will catch it and free close from its side.
			*/
			/* Fall-through */
		case CONN_ALLOCED:
			/*
			  Allocated but not connected or there was failure when trying
			  to connect with pre-allocated connect.

			  Fall-through
			*/
			SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
			/* Fall-through */
		case CONN_QUIT_SENT:
			/* The user has killed its own connection */
			vio->data->m.close_stream(vio, conn->stats, conn->error_info);
			break;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_reference */
static MYSQLND_CONN_DATA *
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, get_reference)(MYSQLND_CONN_DATA * const conn)
{
	DBG_ENTER("mysqlnd_conn_data::get_reference");
	++conn->refcount;
	DBG_INF_FMT("conn=%llu new_refcount=%u", conn->thread_id, conn->refcount);
	DBG_RETURN(conn);
}
/* }}} */


/* {{{ mysqlnd_conn_data::free_reference */
static enum_func_status
MYSQLND_METHOD_PRIVATE(mysqlnd_conn_data, free_reference)(MYSQLND_CONN_DATA * const conn)
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
		ret = conn->m->send_close(conn);
		conn->m->dtor(conn);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::field_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, field_count)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->field_count;
}
/* }}} */


/* {{{ mysqlnd_conn_data::server_status */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, server_status)(const MYSQLND_CONN_DATA * const conn)
{
	return UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status);
}
/* }}} */


/* {{{ mysqlnd_conn_data::insert_id */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn_data, insert_id)(const MYSQLND_CONN_DATA * const conn)
{
	return UPSERT_STATUS_GET_LAST_INSERT_ID(conn->upsert_status);
}
/* }}} */


/* {{{ mysqlnd_conn_data::affected_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn_data, affected_rows)(const MYSQLND_CONN_DATA * const conn)
{
	return UPSERT_STATUS_GET_AFFECTED_ROWS(conn->upsert_status);
}
/* }}} */


/* {{{ mysqlnd_conn_data::warning_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, warning_count)(const MYSQLND_CONN_DATA * const conn)
{
	return UPSERT_STATUS_GET_WARNINGS(conn->upsert_status);
}
/* }}} */


/* {{{ mysqlnd_conn_data::info */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, info)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->last_message.s;
}
/* }}} */


/* {{{ mysqlnd_get_client_info */
PHPAPI const char * mysqlnd_get_client_info()
{
	return PHP_MYSQLND_VERSION;
}
/* }}} */


/* {{{ mysqlnd_get_client_version */
PHPAPI unsigned long mysqlnd_get_client_version()
{
	return MYSQLND_VERSION_ID;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_server_info */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, get_server_info)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->server_version;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_host_info */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, get_host_info)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->host_info;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_proto_info */
static unsigned int
MYSQLND_METHOD(mysqlnd_conn_data, get_proto_info)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->protocol_version;
}
/* }}} */


/* {{{ mysqlnd_conn_data::charset_name */
static const char *
MYSQLND_METHOD(mysqlnd_conn_data, charset_name)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->charset->name;
}
/* }}} */


/* {{{ mysqlnd_conn_data::thread_id */
static uint64_t
MYSQLND_METHOD(mysqlnd_conn_data, thread_id)(const MYSQLND_CONN_DATA * const conn)
{
	return conn->thread_id;
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_server_version */
static zend_ulong
MYSQLND_METHOD(mysqlnd_conn_data, get_server_version)(const MYSQLND_CONN_DATA * const conn)
{
	zend_long major, minor, patch;
	char *p;

	if (!(p = conn->server_version)) {
		return 0;
	}

#define MARIA_DB_VERSION_HACK_PREFIX "5.5.5-"

	if (conn->server_capabilities & CLIENT_PLUGIN_AUTH
		&& !strncmp(p, MARIA_DB_VERSION_HACK_PREFIX, sizeof(MARIA_DB_VERSION_HACK_PREFIX)-1))
	{
		p += sizeof(MARIA_DB_VERSION_HACK_PREFIX)-1;
	}

	major = ZEND_STRTOL(p, &p, 10);
	p += 1; /* consume the dot */
	minor = ZEND_STRTOL(p, &p, 10);
	p += 1; /* consume the dot */
	patch = ZEND_STRTOL(p, &p, 10);

	return (zend_ulong)(major * Z_L(10000) + (zend_ulong)(minor * Z_L(100) + patch));
}
/* }}} */


/* {{{ mysqlnd_conn_data::more_results */
static zend_bool
MYSQLND_METHOD(mysqlnd_conn_data, more_results)(const MYSQLND_CONN_DATA * const conn)
{
	DBG_ENTER("mysqlnd_conn_data::more_results");
	/* (conn->state == CONN_NEXT_RESULT_PENDING) too */
	DBG_RETURN(UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & SERVER_MORE_RESULTS_EXISTS? TRUE:FALSE);
}
/* }}} */


/* {{{ mysqlnd_conn_data::next_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, next_result)(MYSQLND_CONN_DATA * const conn)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), next_result);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::next_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	SET_EMPTY_ERROR(conn->error_info);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			if (GET_CONNECTION_STATE(&conn->state) != CONN_NEXT_RESULT_PENDING) {
				break;
			}

			UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);
			/*
			  We are sure that there is a result set, since conn->state is set accordingly
			  in mysqlnd_store_result() or mysqlnd_fetch_row_unbuffered()
			*/
			if (FAIL == (ret = conn->m->query_read_result_set_header(conn, NULL))) {
				/*
				  There can be an error in the middle of a multi-statement, which will cancel the multi-statement.
				  So there are no more results and we should just return FALSE, error_no has been set
				*/
				if (!conn->error_info->error_no) {
					DBG_ERR_FMT("Serious error. %s::%u", __FILE__, __LINE__);
					php_error_docref(NULL, E_WARNING, "Serious error. PID=%d", getpid());
					SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
					conn->m->send_close(conn);
				} else {
					DBG_INF_FMT("Error from the server : (%u) %s", conn->error_info->error_no, conn->error_info->error);
				}
				break;
			}
			if (conn->last_query_type == QUERY_UPSERT && UPSERT_STATUS_GET_AFFECTED_ROWS(conn->upsert_status)) {
				MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats, STAT_ROWS_AFFECTED_NORMAL, UPSERT_STATUS_GET_AFFECTED_ROWS(conn->upsert_status));
			}
		} while (0);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_field_type_name */
PHPAPI const char * mysqlnd_field_type_name(const enum mysqlnd_field_types field_type)
{
	switch(field_type) {
		case FIELD_TYPE_JSON:
			return "json";
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
			)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), change_user);
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn_data::change_user");
	DBG_INF_FMT("conn=%llu user=%s passwd=%s db=%s silent=%u",
				conn->thread_id, user?user:"", passwd?"***":"null", db?db:"", silent == TRUE);

	if (PASS != conn->m->local_tx_start(conn, this_func)) {
		goto end;
	}

	SET_EMPTY_ERROR(conn->error_info);
	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);

	if (!user) {
		user = "";
	}
	if (!passwd) {
		passwd = "";
		passwd_len = 0;
	}
	if (!db) {
		db = "";
	}

	/* XXX: passwords that have \0 inside work during auth, but in this case won't work with change user */
	ret = mysqlnd_run_authentication(conn, user, passwd, passwd_len, db, strlen(db),
									 conn->authentication_plugin_data, conn->options->auth_protocol,
									0 /*charset not used*/, conn->options, conn->server_capabilities, silent, TRUE/*is_change*/);

	/*
	  Here we should close all statements. Unbuffered queries should not be a
	  problem as we won't allow sending COM_CHANGE_USER.
	*/
	conn->m->local_tx_end(conn, this_func, ret);
end:
	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_client_option)(MYSQLND_CONN_DATA * const conn,
												enum_mysqlnd_client_option option,
												const char * const value
												)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), set_client_option);
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_conn_data::set_client_option");
	DBG_INF_FMT("conn=%llu option=%u", conn->thread_id, option);

	if (PASS != conn->m->local_tx_start(conn, this_func)) {
		goto end;
	}
	switch (option) {
		case MYSQL_OPT_READ_TIMEOUT:
		case MYSQL_OPT_WRITE_TIMEOUT:
		case MYSQLND_OPT_SSL_KEY:
		case MYSQLND_OPT_SSL_CERT:
		case MYSQLND_OPT_SSL_CA:
		case MYSQLND_OPT_SSL_CAPATH:
		case MYSQLND_OPT_SSL_CIPHER:
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
		case MYSQL_OPT_CONNECT_TIMEOUT:
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
			ret = conn->vio->data->m.set_client_option(conn->vio, option, value);
			break;
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
		case MYSQL_OPT_COMPRESS:
		case MYSQL_SERVER_PUBLIC_KEY:
			ret = conn->protocol_frame_codec->data->m.set_client_option(conn->protocol_frame_codec, option, value);
			break;
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
		case MYSQLND_OPT_INT_AND_FLOAT_NATIVE:
			conn->options->int_and_float_native = *(unsigned int*) value;
			break;
#endif
		case MYSQL_OPT_LOCAL_INFILE:
			if (value && (*(unsigned int*) value) ? 1 : 0) {
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
			char * new_charset_name;
			if (!mysqlnd_find_charset_name(value)) {
				SET_CLIENT_ERROR(conn->error_info, CR_CANT_FIND_CHARSET, UNKNOWN_SQLSTATE, "Unknown character set");
				ret = FAIL;
				break;
			}

			new_charset_name = mnd_pestrdup(value, conn->persistent);
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
		case MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS:
			if (value && (*(unsigned int*) value) ? 1 : 0) {
				conn->options->flags |= CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS;
			} else {
				conn->options->flags &= ~CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS;
			}
			break;
		case MYSQL_OPT_CONNECT_ATTR_RESET:
			if (conn->options->connect_attr) {
				DBG_INF_FMT("Before reset %d attribute(s)", zend_hash_num_elements(conn->options->connect_attr));
				zend_hash_clean(conn->options->connect_attr);
			}
			break;
		case MYSQL_OPT_CONNECT_ATTR_DELETE:
			if (conn->options->connect_attr && value) {
				DBG_INF_FMT("Before delete %d attribute(s)", zend_hash_num_elements(conn->options->connect_attr));
				zend_hash_str_del(conn->options->connect_attr, value, strlen(value));
				DBG_INF_FMT("%d left", zend_hash_num_elements(conn->options->connect_attr));
			}
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_SHARED_MEMORY_BASE_NAME:
		case MYSQL_OPT_USE_RESULT:
		case MYSQL_SECURE_AUTH:
			/* not sure, todo ? */
#endif
		default:
			ret = FAIL;
	}
	conn->m->local_tx_end(conn, this_func, ret);
	DBG_RETURN(ret);
oom:
	SET_OOM_ERROR(conn->error_info);
	conn->m->local_tx_end(conn, this_func, FAIL);
end:
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_client_option_2d */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_client_option_2d)(MYSQLND_CONN_DATA * const conn,
														const enum_mysqlnd_client_option option,
														const char * const key,
														const char * const value
														)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), set_client_option_2d);
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_conn_data::set_client_option_2d");
	DBG_INF_FMT("conn=%llu option=%u", conn->thread_id, option);

	if (PASS != conn->m->local_tx_start(conn, this_func)) {
		goto end;
	}
	switch (option) {
		case MYSQL_OPT_CONNECT_ATTR_ADD:
			if (!conn->options->connect_attr) {
				DBG_INF("Initializing connect_attr hash");
				conn->options->connect_attr = mnd_pemalloc(sizeof(HashTable), conn->persistent);
				if (!conn->options->connect_attr) {
					goto oom;
				}
				zend_hash_init(conn->options->connect_attr, 0, NULL, conn->persistent ? ZVAL_INTERNAL_PTR_DTOR : ZVAL_PTR_DTOR, conn->persistent);
			}
			DBG_INF_FMT("Adding [%s][%s]", key, value);
			{
				zval attrz;
				zend_string *str;

				if (conn->persistent) {
					str = zend_string_init(key, strlen(key), 1);
					GC_MAKE_PERSISTENT_LOCAL(str);
					ZVAL_NEW_STR(&attrz, zend_string_init(value, strlen(value), 1));
					GC_MAKE_PERSISTENT_LOCAL(Z_COUNTED(attrz));
				} else {
					str = zend_string_init(key, strlen(key), 0);
					ZVAL_NEW_STR(&attrz, zend_string_init(value, strlen(value), 0));
				}
				zend_hash_update(conn->options->connect_attr, str, &attrz);
				zend_string_release_ex(str, 1);
			}
			break;
		default:
			ret = FAIL;
	}
	conn->m->local_tx_end(conn, this_func, ret);
	DBG_RETURN(ret);
oom:
	SET_OOM_ERROR(conn->error_info);
	conn->m->local_tx_end(conn, this_func, FAIL);
end:
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_conn_data::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, use_result)(MYSQLND_CONN_DATA * const conn, const unsigned int flags)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), use_result);
	MYSQLND_RES * result = NULL;

	DBG_ENTER("mysqlnd_conn_data::use_result");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			if (!conn->current_result) {
				break;
			}

			/* Nothing to store for UPSERT/LOAD DATA */
			if (conn->last_query_type != QUERY_SELECT || GET_CONNECTION_STATE(&conn->state) != CONN_FETCHING_DATA) {
				SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
				DBG_ERR("Command out of sync");
				break;
			}

			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_UNBUFFERED_SETS);

			conn->current_result->conn = conn->m->get_reference(conn);
			result = conn->current_result->m.use_result(conn->current_result, FALSE);

			if (!result) {
				conn->current_result->m.free_result(conn->current_result, TRUE);
			}
			conn->current_result = NULL;
		} while (0);

		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS);
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_conn_data, store_result)(MYSQLND_CONN_DATA * const conn, const unsigned int flags)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), store_result);
	MYSQLND_RES * result = NULL;

	DBG_ENTER("mysqlnd_conn_data::store_result");
	DBG_INF_FMT("conn=%llu conn=%p", conn->thread_id, conn);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			unsigned int f = flags;
			if (!conn->current_result) {
				break;
			}

			/* Nothing to store for UPSERT/LOAD DATA*/
			if (conn->last_query_type != QUERY_SELECT || GET_CONNECTION_STATE(&conn->state) != CONN_FETCHING_DATA) {
				SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
				DBG_ERR("Command out of sync");
				break;
			}

			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_BUFFERED_SETS);

			/* overwrite */
			if ((conn->m->get_client_api_capabilities(conn) & MYSQLND_CLIENT_KNOWS_RSET_COPY_DATA)) {
				if (MYSQLND_G(fetch_data_copy)) {
					f &= ~MYSQLND_STORE_NO_COPY;
					f |= MYSQLND_STORE_COPY;
				}
			} else {
				/* if for some reason PDO borks something */
				if (!(f & (MYSQLND_STORE_NO_COPY | MYSQLND_STORE_COPY))) {
					f |= MYSQLND_STORE_COPY;
				}
			}
			if (!(f & (MYSQLND_STORE_NO_COPY | MYSQLND_STORE_COPY))) {
				SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "Unknown fetch mode");
				DBG_ERR("Unknown fetch mode");
				break;
			}
			result = conn->current_result->m.store_result(conn->current_result, conn, f);
			if (!result) {
				conn->current_result->m.free_result(conn->current_result, TRUE);
			}
			conn->current_result = NULL;
		} while (0);

		conn->m->local_tx_end(conn, this_func, result == NULL? FAIL:PASS);
	}
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_connection_stats */
static void
MYSQLND_METHOD(mysqlnd_conn_data, get_connection_stats)(const MYSQLND_CONN_DATA * const conn,
														zval * return_value ZEND_FILE_LINE_DC)
{
	DBG_ENTER("mysqlnd_conn_data::get_connection_stats");
	mysqlnd_fill_stats_hash(conn->stats, mysqlnd_stats_values_names, return_value ZEND_FILE_LINE_CC);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::set_autocommit */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, set_autocommit)(MYSQLND_CONN_DATA * conn, unsigned int mode)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), set_autocommit);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::set_autocommit");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		ret = conn->m->query(conn, (mode) ? "SET AUTOCOMMIT=1":"SET AUTOCOMMIT=0", sizeof("SET AUTOCOMMIT=1") - 1);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_commit */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_commit)(MYSQLND_CONN_DATA * conn)
{
	return conn->m->tx_commit_or_rollback(conn, TRUE, TRANS_COR_NO_OPT, NULL);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_rollback */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_rollback)(MYSQLND_CONN_DATA * conn)
{
	return conn->m->tx_commit_or_rollback(conn, FALSE, TRANS_COR_NO_OPT, NULL);
}
/* }}} */


/* {{{ mysqlnd_tx_cor_options_to_string */
static void
MYSQLND_METHOD(mysqlnd_conn_data, tx_cor_options_to_string)(const MYSQLND_CONN_DATA * const conn, smart_str * str, const unsigned int mode)
{
	if (mode & TRANS_COR_AND_CHAIN && !(mode & TRANS_COR_AND_NO_CHAIN)) {
		if (str->s && ZSTR_LEN(str->s)) {
			smart_str_appendl(str, " ", sizeof(" ") - 1);
		}
		smart_str_appendl(str, "AND CHAIN", sizeof("AND CHAIN") - 1);
	} else if (mode & TRANS_COR_AND_NO_CHAIN && !(mode & TRANS_COR_AND_CHAIN)) {
		if (str->s && ZSTR_LEN(str->s)) {
			smart_str_appendl(str, " ", sizeof(" ") - 1);
		}
		smart_str_appendl(str, "AND NO CHAIN", sizeof("AND NO CHAIN") - 1);
	}

	if (mode & TRANS_COR_RELEASE && !(mode & TRANS_COR_NO_RELEASE)) {
		if (str->s && ZSTR_LEN(str->s)) {
			smart_str_appendl(str, " ", sizeof(" ") - 1);
		}
		smart_str_appendl(str, "RELEASE", sizeof("RELEASE") - 1);
	} else if (mode & TRANS_COR_NO_RELEASE && !(mode & TRANS_COR_RELEASE)) {
		if (str->s && ZSTR_LEN(str->s)) {
			smart_str_appendl(str, " ", sizeof(" ") - 1);
		}
		smart_str_appendl(str, "NO RELEASE", sizeof("NO RELEASE") - 1);
	}
	smart_str_0(str);
}
/* }}} */


/* {{{ mysqlnd_escape_string_for_tx_name_in_comment */
static char *
mysqlnd_escape_string_for_tx_name_in_comment(const char * const name)
{
	char * ret = NULL;
	DBG_ENTER("mysqlnd_escape_string_for_tx_name_in_comment");
	if (name) {
		zend_bool warned = FALSE;
		const char * p_orig = name;
		char * p_copy;
		p_copy = ret = mnd_emalloc(strlen(name) + 1 + 2 + 2 + 1); /* space, open, close, NullS */
		*p_copy++ = ' ';
		*p_copy++ = '/';
		*p_copy++ = '*';
		while (1) {
			register char v = *p_orig;
			if (v == 0) {
				break;
			}
			if ((v >= '0' && v <= '9') ||
				(v >= 'a' && v <= 'z') ||
				(v >= 'A' && v <= 'Z') ||
				v == '-' ||
				v == '_' ||
				v == ' ' ||
				v == '=')
			{
				*p_copy++ = v;
			} else if (warned == FALSE) {
				php_error_docref(NULL, E_WARNING, "Transaction name truncated. Must be only [0-9A-Za-z\\-_=]+");
				warned = TRUE;
			}
			++p_orig;
		}
		*p_copy++ = '*';
		*p_copy++ = '/';
		*p_copy++ = 0;
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_commit_ex */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_commit_or_rollback)(MYSQLND_CONN_DATA * conn, const zend_bool commit, const unsigned int flags, const char * const name)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), tx_commit_or_rollback);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::tx_commit_or_rollback");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			smart_str tmp_str = {0, 0};
			conn->m->tx_cor_options_to_string(conn, &tmp_str, flags);
			smart_str_0(&tmp_str);


			{
				char * query;
				size_t query_len;
				char * name_esc = mysqlnd_escape_string_for_tx_name_in_comment(name);

				query_len = mnd_sprintf(&query, 0, (commit? "COMMIT%s %s":"ROLLBACK%s %s"),
										name_esc? name_esc:"", tmp_str.s? ZSTR_VAL(tmp_str.s):"");
				smart_str_free(&tmp_str);
				if (name_esc) {
					mnd_efree(name_esc);
					name_esc = NULL;
				}
				if (!query) {
					SET_OOM_ERROR(conn->error_info);
					break;
				}

				ret = conn->m->query(conn, query, query_len);
				mnd_sprintf_free(query);
			}
		} while (0);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_begin */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_begin)(MYSQLND_CONN_DATA * conn, const unsigned int mode, const char * const name)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), tx_begin);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::tx_begin");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			smart_str tmp_str = {0, 0};
			if (mode & TRANS_START_WITH_CONSISTENT_SNAPSHOT) {
				if (tmp_str.s) {
					smart_str_appendl(&tmp_str, ", ", sizeof(", ") - 1);
				}
				smart_str_appendl(&tmp_str, "WITH CONSISTENT SNAPSHOT", sizeof("WITH CONSISTENT SNAPSHOT") - 1);
			}
			if (mode & TRANS_START_READ_WRITE) {
				if (tmp_str.s && ZSTR_LEN(tmp_str.s)) {
					smart_str_appendl(&tmp_str, ", ", sizeof(", ") - 1);
				}
				smart_str_appendl(&tmp_str, "READ WRITE", sizeof("READ WRITE") - 1);
			} else if (mode & TRANS_START_READ_ONLY) {
				if (tmp_str.s && ZSTR_LEN(tmp_str.s)) {
					smart_str_appendl(&tmp_str, ", ", sizeof(", ") - 1);
				}
				smart_str_appendl(&tmp_str, "READ ONLY", sizeof("READ ONLY") - 1);
			}
			smart_str_0(&tmp_str);

			{
				char * name_esc = mysqlnd_escape_string_for_tx_name_in_comment(name);
				char * query;
				unsigned int query_len = mnd_sprintf(&query, 0, "START TRANSACTION%s %s", name_esc? name_esc:"", tmp_str.s? ZSTR_VAL(tmp_str.s):"");
				smart_str_free(&tmp_str);
				if (name_esc) {
					mnd_efree(name_esc);
					name_esc = NULL;
				}
				if (!query) {
					SET_OOM_ERROR(conn->error_info);
					break;
				}
				ret = conn->m->query(conn, query, query_len);
				mnd_sprintf_free(query);
				if (ret && mode & (TRANS_START_READ_WRITE | TRANS_START_READ_ONLY) &&
					mysqlnd_stmt_errno(conn) == 1064) {
					php_error_docref(NULL, E_WARNING, "This server version doesn't support 'READ WRITE' and 'READ ONLY'. Minimum 5.6.5 is required");
					break;
				}
			}
		} while (0);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_savepoint */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_savepoint)(MYSQLND_CONN_DATA * conn, const char * const name)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), tx_savepoint);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::tx_savepoint");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			char * query;
			unsigned int query_len;
			if (!name) {
				SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "Savepoint name not provided");
				break;
			}
			query_len = mnd_sprintf(&query, 0, "SAVEPOINT `%s`", name);
			if (!query) {
				SET_OOM_ERROR(conn->error_info);
				break;
			}
			ret = conn->m->query(conn, query, query_len);
			mnd_sprintf_free(query);
		} while (0);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::tx_savepoint_release */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, tx_savepoint_release)(MYSQLND_CONN_DATA * conn, const char * const name)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data), tx_savepoint_release);
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::tx_savepoint_release");

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		do {
			char * query;
			unsigned int query_len;
			if (!name) {
				SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "Savepoint name not provided");
				break;
			}
			query_len = mnd_sprintf(&query, 0, "RELEASE SAVEPOINT `%s`", name);
			if (!query) {
				SET_OOM_ERROR(conn->error_info);
				break;
			}
			ret = conn->m->query(conn, query, query_len);
			mnd_sprintf_free(query);
		} while (0);
		conn->m->local_tx_end(conn, this_func, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::negotiate_client_api_capabilities */
static size_t
MYSQLND_METHOD(mysqlnd_conn_data, negotiate_client_api_capabilities)(MYSQLND_CONN_DATA * const conn, const size_t flags)
{
	unsigned int ret = 0;
	DBG_ENTER("mysqlnd_conn_data::negotiate_client_api_capabilities");
	if (conn) {
		ret = conn->client_api_capabilities;
		conn->client_api_capabilities = flags;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::get_client_api_capabilities */
static size_t
MYSQLND_METHOD(mysqlnd_conn_data, get_client_api_capabilities)(const MYSQLND_CONN_DATA * const conn)
{
	DBG_ENTER("mysqlnd_conn_data::get_client_api_capabilities");
	DBG_RETURN(conn? conn->client_api_capabilities : 0);
}
/* }}} */


/* {{{ mysqlnd_conn_data::local_tx_start */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, local_tx_start)(MYSQLND_CONN_DATA * conn, const size_t this_func)
{
	DBG_ENTER("mysqlnd_conn_data::local_tx_start");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_conn_data::local_tx_end */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn_data, local_tx_end)(MYSQLND_CONN_DATA * conn, const size_t this_func, const enum_func_status status)
{
	DBG_ENTER("mysqlnd_conn_data::local_tx_end");
	DBG_RETURN(status);
}
/* }}} */


/* {{{ _mysqlnd_stmt_init */
MYSQLND_STMT *
MYSQLND_METHOD(mysqlnd_conn_data, stmt_init)(MYSQLND_CONN_DATA * const conn)
{
	MYSQLND_STMT * ret;
	DBG_ENTER("mysqlnd_conn_data::stmt_init");
	ret = conn->object_factory.get_prepared_statement(conn);
	DBG_RETURN(ret);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_conn_data)
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

	MYSQLND_METHOD(mysqlnd_conn_data, stmt_init),

	MYSQLND_METHOD(mysqlnd_conn_data, shutdown),
	MYSQLND_METHOD(mysqlnd_conn_data, refresh),

	MYSQLND_METHOD(mysqlnd_conn_data, ping),
	MYSQLND_METHOD(mysqlnd_conn_data, kill),
	MYSQLND_METHOD(mysqlnd_conn_data, select_db),
	MYSQLND_METHOD(mysqlnd_conn_data, dump_debug_info),
	MYSQLND_METHOD(mysqlnd_conn_data, change_user),

	MYSQLND_METHOD(mysqlnd_conn_data, err_no),
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

	MYSQLND_METHOD(mysqlnd_conn_data, restart_psession),
	MYSQLND_METHOD(mysqlnd_conn_data, end_psession),
	MYSQLND_METHOD(mysqlnd_conn_data, send_close),

	MYSQLND_METHOD(mysqlnd_conn_data, ssl_set),
	mysqlnd_result_init,
	MYSQLND_METHOD(mysqlnd_conn_data, set_autocommit),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_commit),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_rollback),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_begin),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_commit_or_rollback),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_cor_options_to_string),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_savepoint),
	MYSQLND_METHOD(mysqlnd_conn_data, tx_savepoint_release),

	MYSQLND_METHOD(mysqlnd_conn_data, local_tx_start),
	MYSQLND_METHOD(mysqlnd_conn_data, local_tx_end),
	MYSQLND_METHOD(mysqlnd_conn_data, execute_init_commands),
	MYSQLND_METHOD(mysqlnd_conn_data, get_updated_connect_flags),
	MYSQLND_METHOD(mysqlnd_conn_data, connect_handshake),
	MYSQLND_METHOD(mysqlnd_conn_data, fetch_auth_plugin_by_name),

	MYSQLND_METHOD(mysqlnd_conn_data, set_client_option_2d),

	MYSQLND_METHOD(mysqlnd_conn_data, negotiate_client_api_capabilities),
	MYSQLND_METHOD(mysqlnd_conn_data, get_client_api_capabilities),

	MYSQLND_METHOD(mysqlnd_conn_data, get_scheme)
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_conn::get_reference */
static MYSQLND *
MYSQLND_METHOD(mysqlnd_conn, clone_object)(MYSQLND * const conn)
{
	MYSQLND * ret;
	DBG_ENTER("mysqlnd_conn::get_reference");
	ret = conn->data->object_factory.clone_connection_object(conn);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_conn_data::dtor */
static void
MYSQLND_METHOD_PRIVATE(mysqlnd_conn, dtor)(MYSQLND * conn)
{
	DBG_ENTER("mysqlnd_conn::dtor");
	DBG_INF_FMT("conn=%llu", conn->data->thread_id);

	conn->data->m->free_reference(conn->data);

	mnd_pefree(conn, conn->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_conn_data::close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_conn, close)(MYSQLND * conn_handle, const enum_connection_close_type close_type)
{
	const size_t this_func = STRUCT_OFFSET(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn), close);
	MYSQLND_CONN_DATA * conn = conn_handle->data;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_conn::close");
	DBG_INF_FMT("conn=%llu", conn->thread_id);

	if (PASS == conn->m->local_tx_start(conn, this_func)) {
		if (GET_CONNECTION_STATE(&conn->state) >= CONN_READY) {
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
		ret = conn->m->send_close(conn);

		/* If we do it after free_reference/dtor then we might crash */
		conn->m->local_tx_end(conn, this_func, ret);

		conn_handle->m->dtor(conn_handle);
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


#include "php_network.h"

/* {{{ mysqlnd_stream_array_to_fd_set */
MYSQLND **
mysqlnd_stream_array_check_for_readiness(MYSQLND ** conn_array)
{
	unsigned int cnt = 0;
	MYSQLND **p = conn_array, **p_p;
	MYSQLND **ret = NULL;

	while (*p) {
		const enum mysqlnd_connection_state conn_state = GET_CONNECTION_STATE(&((*p)->data->state));
		if (conn_state <= CONN_READY || conn_state == CONN_QUIT_SENT) {
			cnt++;
		}
		p++;
	}
	if (cnt) {
		MYSQLND **ret_p = ret = ecalloc(cnt + 1, sizeof(MYSQLND *));
		p_p = p = conn_array;
		while (*p) {
			const enum mysqlnd_connection_state conn_state = GET_CONNECTION_STATE(&((*p)->data->state));
			if (conn_state <= CONN_READY || conn_state == CONN_QUIT_SENT) {
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
/* }}} */


/* {{{ mysqlnd_stream_array_to_fd_set */
static unsigned int
mysqlnd_stream_array_to_fd_set(MYSQLND ** conn_array, fd_set * fds, php_socket_t * max_fd)
{
	php_socket_t this_fd;
	php_stream *stream = NULL;
	unsigned int cnt = 0;
	MYSQLND **p = conn_array;
	DBG_ENTER("mysqlnd_stream_array_to_fd_set");

	while (*p) {
		/* get the fd.
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 * */
		stream = (*p)->data->vio->data->m.get_stream((*p)->data->vio);
		DBG_INF_FMT("conn=%llu stream=%p", (*p)->data->thread_id, stream);
		if (stream != NULL &&
			SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1) &&
			ZEND_VALID_SOCKET(this_fd))
		{

			PHP_SAFE_FD_SET(this_fd, fds);

			if (this_fd > *max_fd) {
				*max_fd = this_fd;
			}
			++cnt;
		}
		++p;
	}
	DBG_RETURN(cnt ? 1 : 0);
}
/* }}} */


/* {{{ mysqlnd_stream_array_from_fd_set */
static unsigned int
mysqlnd_stream_array_from_fd_set(MYSQLND ** conn_array, fd_set * fds)
{
	php_socket_t this_fd;
	php_stream *stream = NULL;
	unsigned int ret = 0;
	zend_bool disproportion = FALSE;
	MYSQLND **fwd = conn_array, **bckwd = conn_array;
	DBG_ENTER("mysqlnd_stream_array_from_fd_set");

	while (*fwd) {
		stream = (*fwd)->data->vio->data->m.get_stream((*fwd)->data->vio);
		DBG_INF_FMT("conn=%llu stream=%p", (*fwd)->data->thread_id, stream);
		if (stream != NULL && SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
										(void*)&this_fd, 1) && ZEND_VALID_SOCKET(this_fd)) {
			if (PHP_SAFE_FD_ISSET(this_fd, fds)) {
				if (disproportion) {
					*bckwd = *fwd;
				}
				++bckwd;
				++fwd;
				++ret;
				continue;
			}
		}
		disproportion = TRUE;
		++fwd;
	}
	*bckwd = NULL;/* NULL-terminate the list */

	DBG_RETURN(ret);
}
/* }}} */


#ifndef PHP_WIN32
#define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#else
#include "win32/select.h"
#endif


/* {{{ mysqlnd_poll */
PHPAPI enum_func_status
mysqlnd_poll(MYSQLND **r_array, MYSQLND **e_array, MYSQLND ***dont_poll, long sec, long usec, int * desc_num)
{
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	php_socket_t	max_fd = 0;
	int				retval, sets = 0;
	int				set_count, max_set_count = 0;

	DBG_ENTER("_mysqlnd_poll");
	if (sec < 0 || usec < 0) {
		php_error_docref(NULL, E_WARNING, "Negative values passed for sec and/or usec");
		DBG_RETURN(FAIL);
	}

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) {
		*dont_poll = mysqlnd_stream_array_check_for_readiness(r_array);
		set_count = mysqlnd_stream_array_to_fd_set(r_array, &rfds, &max_fd);
		if (set_count > max_set_count) {
			max_set_count = set_count;
		}
		sets += set_count;
	}

	if (e_array != NULL) {
		set_count = mysqlnd_stream_array_to_fd_set(e_array, &efds, &max_fd);
		if (set_count > max_set_count) {
			max_set_count = set_count;
		}
		sets += set_count;
	}

	if (!sets) {
		php_error_docref(NULL, E_WARNING, *dont_poll ? "All arrays passed are clear":"No stream arrays were passed");
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
		php_error_docref(NULL, E_WARNING, "unable to select [%d]: %s (max_fd=%d)",
						errno, strerror(errno), max_fd);
		DBG_RETURN(FAIL);
	}

	if (r_array != NULL) {
		mysqlnd_stream_array_from_fd_set(r_array, &rfds);
	}
	if (e_array != NULL) {
		mysqlnd_stream_array_from_fd_set(e_array, &efds);
	}

	*desc_num = retval;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_connect */
PHPAPI MYSQLND * mysqlnd_connection_connect(MYSQLND * conn_handle,
											const char * const host,
											const char * const user,
											const char * const passwd, unsigned int passwd_len,
											const char * const db, unsigned int db_len,
											unsigned int port,
											const char * const sock_or_pipe,
											unsigned int mysql_flags,
											unsigned int client_api_flags
						)
{
	enum_func_status ret = FAIL;
	zend_bool self_alloced = FALSE;
	MYSQLND_CSTRING hostname = { host, host? strlen(host) : 0 };
	MYSQLND_CSTRING username = { user, user? strlen(user) : 0 };
	MYSQLND_CSTRING password = { passwd, passwd_len };
	MYSQLND_CSTRING database = { db, db_len };
	MYSQLND_CSTRING socket_or_pipe = { sock_or_pipe, sock_or_pipe? strlen(sock_or_pipe) : 0 };

	DBG_ENTER("mysqlnd_connect");
	DBG_INF_FMT("host=%s user=%s db=%s port=%u flags=%u", host? host:"", user? user:"", db? db:"", port, mysql_flags);

	if (!conn_handle) {
		self_alloced = TRUE;
		if (!(conn_handle = mysqlnd_connection_init(client_api_flags, FALSE, NULL))) {
			/* OOM */
			DBG_RETURN(NULL);
		}
	}

	ret = conn_handle->m->connect(conn_handle, hostname, username, password, database, port, socket_or_pipe, mysql_flags);

	if (ret == FAIL) {
		if (self_alloced) {
			/*
			  We have alloced, thus there are no references to this
			  object - we are free to kill it!
			*/
			conn_handle->m->dtor(conn_handle);
		}
		DBG_RETURN(NULL);
	}
	DBG_RETURN(conn_handle);
}
/* }}} */


/* {{{ mysqlnd_connection_init */
PHPAPI MYSQLND *
mysqlnd_connection_init(const size_t client_flags, const zend_bool persistent, MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *object_factory)
{
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *factory = object_factory? object_factory : &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory);
	MYSQLND * ret;
	DBG_ENTER("mysqlnd_connection_init");
	ret = factory->get_connection(factory, persistent);
	if (ret && ret->data) {
		ret->data->m->negotiate_client_api_capabilities(ret->data, client_flags);
	}
	DBG_RETURN(ret);
}
/* }}} */
