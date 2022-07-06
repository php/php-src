/*
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
#include "mysqlnd_structs.h"
#include "mysqlnd_auth.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_connection.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"

static const char * const mysqlnd_old_passwd  = "mysqlnd cannot connect to MySQL 4.1+ using the old insecure authentication. "
"Please use an administration tool to reset your password with the command SET PASSWORD = PASSWORD('your_existing_password'). This will "
"store a new, and more secure, hash value in mysql.user. If this user is used in other scripts executed by PHP 5.2 or earlier you might need to remove the old-passwords "
"flag from your my.cnf file";


/* {{{ mysqlnd_run_authentication */
enum_func_status
mysqlnd_run_authentication(
			MYSQLND_CONN_DATA * const conn,
			const char * const user,
			const char * const passwd,
			const size_t passwd_len,
			const char * const db,
			const size_t db_len,
			const MYSQLND_STRING auth_plugin_data,
			const char * const auth_protocol,
			const unsigned int charset_no,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			const zend_ulong mysql_flags,
			const zend_bool silent,
			const zend_bool is_change_user
			)
{
	enum_func_status ret = FAIL;
	zend_bool first_call = TRUE;

	char * switch_to_auth_protocol = NULL;
	size_t switch_to_auth_protocol_len = 0;
	char * requested_protocol = NULL;
	zend_uchar * plugin_data;
	size_t plugin_data_len;

	DBG_ENTER("mysqlnd_run_authentication");

	plugin_data_len = auth_plugin_data.l;
	plugin_data = mnd_emalloc(plugin_data_len + 1);
	if (!plugin_data) {
		goto end;
	}
	memcpy(plugin_data, auth_plugin_data.s, plugin_data_len);
	plugin_data[plugin_data_len] = '\0';

	requested_protocol = mnd_pestrdup(auth_protocol? auth_protocol : MYSQLND_DEFAULT_AUTH_PROTOCOL, FALSE);
	if (!requested_protocol) {
		goto end;
	}

	do {
		struct st_mysqlnd_authentication_plugin * auth_plugin = conn->m->fetch_auth_plugin_by_name(requested_protocol);

		if (!auth_plugin) {
			if (first_call) {
				mnd_pefree(requested_protocol, FALSE);
				requested_protocol = mnd_pestrdup(MYSQLND_DEFAULT_AUTH_PROTOCOL, FALSE);
			} else {
				php_error_docref(NULL, E_WARNING, "The server requested authentication method unknown to the client [%s]", requested_protocol);
				SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "The server requested authentication method unknown to the client");
				goto end;
			}
		}


		{
			zend_uchar * switch_to_auth_protocol_data = NULL;
			size_t switch_to_auth_protocol_data_len = 0;
			zend_uchar * scrambled_data = NULL;
			size_t scrambled_data_len = 0;

			switch_to_auth_protocol = NULL;
			switch_to_auth_protocol_len = 0;

			if (conn->authentication_plugin_data.s) {
				mnd_pefree(conn->authentication_plugin_data.s, conn->persistent);
				conn->authentication_plugin_data.s = NULL;
			}
			conn->authentication_plugin_data.l = plugin_data_len;
			conn->authentication_plugin_data.s = mnd_pemalloc(conn->authentication_plugin_data.l, conn->persistent);
			if (!conn->authentication_plugin_data.s) {
				SET_OOM_ERROR(conn->error_info);
				goto end;
			}
			memcpy(conn->authentication_plugin_data.s, plugin_data, plugin_data_len);

			DBG_INF_FMT("salt(%d)=[%.*s]", plugin_data_len, plugin_data_len, plugin_data);
			/* The data should be allocated with malloc() */
			if (auth_plugin) {
				scrambled_data = auth_plugin->methods.get_auth_data(
					NULL, &scrambled_data_len, conn, user, passwd,
					passwd_len, plugin_data, plugin_data_len,
					session_options, conn->protocol_frame_codec->data,
					mysql_flags);
			}

			if (conn->error_info->error_no) {
				goto end;
			}
			if (FALSE == is_change_user) {
				ret = mysqlnd_auth_handshake(conn, user, passwd, passwd_len, db, db_len, session_options, mysql_flags,
											charset_no,
											first_call,
											requested_protocol,
											auth_plugin, plugin_data, plugin_data_len,
											scrambled_data, scrambled_data_len,
											&switch_to_auth_protocol, &switch_to_auth_protocol_len,
											&switch_to_auth_protocol_data, &switch_to_auth_protocol_data_len
											);
			} else {
				ret = mysqlnd_auth_change_user(conn, user, strlen(user), passwd, passwd_len, db, db_len, silent,
											   first_call,
											   requested_protocol,
											   auth_plugin, plugin_data, plugin_data_len,
											   scrambled_data, scrambled_data_len,
											   &switch_to_auth_protocol, &switch_to_auth_protocol_len,
											   &switch_to_auth_protocol_data, &switch_to_auth_protocol_data_len
											  );
			}
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

	if (ret == PASS) {
		DBG_INF_FMT("saving requested_protocol=%s", requested_protocol);
		conn->m->set_client_option(conn, MYSQLND_OPT_AUTH_PROTOCOL, requested_protocol);
	}
end:
	if (plugin_data) {
		mnd_efree(plugin_data);
	}
	if (requested_protocol) {
		mnd_efree(requested_protocol);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_switch_to_ssl_if_needed */
static enum_func_status
mysqlnd_switch_to_ssl_if_needed(MYSQLND_CONN_DATA * const conn,
								unsigned int charset_no,
								const size_t server_capabilities,
								const MYSQLND_SESSION_OPTIONS * const session_options,
								const zend_ulong mysql_flags)
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * charset;
	DBG_ENTER("mysqlnd_switch_to_ssl_if_needed");

	if (session_options->charset_name && (charset = mysqlnd_find_charset_name(session_options->charset_name))) {
		charset_no = charset->nr;
	}

	{
		const size_t client_capabilities = mysql_flags;
		ret = conn->command->enable_ssl(conn, client_capabilities, server_capabilities, charset_no);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_connect_run_authentication */
enum_func_status
mysqlnd_connect_run_authentication(
			MYSQLND_CONN_DATA * const conn,
			const char * const user,
			const char * const passwd,
			const char * const db,
			const size_t db_len,
			const size_t passwd_len,
			const MYSQLND_STRING authentication_plugin_data,
			const char * const authentication_protocol,
			const unsigned int charset_no,
			const size_t server_capabilities,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			const zend_ulong mysql_flags
			)
{
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_connect_run_authentication");

	ret = mysqlnd_switch_to_ssl_if_needed(conn, charset_no, server_capabilities, session_options, mysql_flags);
	if (PASS == ret) {
		ret = mysqlnd_run_authentication(conn, user, passwd, passwd_len, db, db_len,
										 authentication_plugin_data, authentication_protocol,
										 charset_no, session_options, mysql_flags, FALSE /*silent*/, FALSE/*is_change*/);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_auth_handshake */
enum_func_status
mysqlnd_auth_handshake(MYSQLND_CONN_DATA * conn,
							  const char * const user,
							  const char * const passwd,
							  const size_t passwd_len,
							  const char * const db,
							  const size_t db_len,
							  const MYSQLND_SESSION_OPTIONS * const session_options,
							  const zend_ulong mysql_flags,
							  const unsigned int server_charset_no,
							  const zend_bool use_full_blown_auth_packet,
							  const char * const auth_protocol,
							  struct st_mysqlnd_authentication_plugin * auth_plugin,
							  const zend_uchar * const orig_auth_plugin_data,
							  const size_t orig_auth_plugin_data_len,
							  const zend_uchar * const auth_plugin_data,
							  const size_t auth_plugin_data_len,
							  char ** switch_to_auth_protocol,
							  size_t * const switch_to_auth_protocol_len,
							  zend_uchar ** switch_to_auth_protocol_data,
							  size_t * const switch_to_auth_protocol_data_len
							 )
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * charset = NULL;
	MYSQLND_PACKET_AUTH_RESPONSE auth_resp_packet;

	DBG_ENTER("mysqlnd_auth_handshake");

	conn->payload_decoder_factory->m.init_auth_response_packet(&auth_resp_packet);

	if (use_full_blown_auth_packet != TRUE) {
		MYSQLND_PACKET_CHANGE_AUTH_RESPONSE change_auth_resp_packet;

		conn->payload_decoder_factory->m.init_change_auth_response_packet(&change_auth_resp_packet);

		change_auth_resp_packet.auth_data = auth_plugin_data;
		change_auth_resp_packet.auth_data_len = auth_plugin_data_len;

		if (!PACKET_WRITE(conn, &change_auth_resp_packet)) {
			SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			PACKET_FREE(&change_auth_resp_packet);
			goto end;
		}
		PACKET_FREE(&change_auth_resp_packet);
	} else {
		MYSQLND_PACKET_AUTH auth_packet;

		conn->payload_decoder_factory->m.init_auth_packet(&auth_packet);

		auth_packet.client_flags = mysql_flags;
		auth_packet.max_packet_size = session_options->max_allowed_packet;
		if (session_options->charset_name && (charset = mysqlnd_find_charset_name(session_options->charset_name))) {
			auth_packet.charset_no	= charset->nr;
		} else {
			auth_packet.charset_no	= server_charset_no;
		}

		auth_packet.send_auth_data = TRUE;
		auth_packet.user		= user;
		auth_packet.db			= db;
		auth_packet.db_len		= db_len;

		auth_packet.auth_data = auth_plugin_data;
		auth_packet.auth_data_len = auth_plugin_data_len;
		auth_packet.auth_plugin_name = auth_protocol;

		if (conn->server_capabilities & CLIENT_CONNECT_ATTRS) {
			auth_packet.connect_attr = conn->options->connect_attr;
		}

		if (!PACKET_WRITE(conn, &auth_packet)) {
			PACKET_FREE(&auth_packet);
			goto end;
		}

		if (use_full_blown_auth_packet == TRUE) {
			conn->charset = mysqlnd_find_charset_nr(auth_packet.charset_no);
		}

		PACKET_FREE(&auth_packet);
	}

	if (auth_plugin && auth_plugin->methods.handle_server_response) {
		if (FAIL == auth_plugin->methods.handle_server_response(auth_plugin, conn,
				orig_auth_plugin_data, orig_auth_plugin_data_len, passwd, passwd_len,
				switch_to_auth_protocol, switch_to_auth_protocol_len,
				switch_to_auth_protocol_data, switch_to_auth_protocol_data_len)) {
			goto end;
		}
	}

	if (FAIL == PACKET_READ(conn, &auth_resp_packet) || auth_resp_packet.response_code >= 0xFE) {
		if (auth_resp_packet.response_code == 0xFE) {
			/* old authentication with new server  !*/
			if (!auth_resp_packet.new_auth_protocol) {
				DBG_ERR(mysqlnd_old_passwd);
				SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
			} else {
				*switch_to_auth_protocol = mnd_pestrndup(auth_resp_packet.new_auth_protocol, auth_resp_packet.new_auth_protocol_len, FALSE);
				*switch_to_auth_protocol_len = auth_resp_packet.new_auth_protocol_len;
				if (auth_resp_packet.new_auth_protocol_data) {
					*switch_to_auth_protocol_data_len = auth_resp_packet.new_auth_protocol_data_len;
					*switch_to_auth_protocol_data = mnd_emalloc(*switch_to_auth_protocol_data_len);
					memcpy(*switch_to_auth_protocol_data, auth_resp_packet.new_auth_protocol_data, *switch_to_auth_protocol_data_len);
				} else {
					*switch_to_auth_protocol_data = NULL;
					*switch_to_auth_protocol_data_len = 0;
				}
			}
		} else if (auth_resp_packet.response_code == 0xFF) {
			if (auth_resp_packet.sqlstate[0]) {
				strlcpy(conn->error_info->sqlstate, auth_resp_packet.sqlstate, sizeof(conn->error_info->sqlstate));
				DBG_ERR_FMT("ERROR:%u [SQLSTATE:%s] %s", auth_resp_packet.error_no, auth_resp_packet.sqlstate, auth_resp_packet.error);
			}
			SET_CLIENT_ERROR(conn->error_info, auth_resp_packet.error_no, UNKNOWN_SQLSTATE, auth_resp_packet.error);
		}
		goto end;
	}

	SET_NEW_MESSAGE(conn->last_message.s, conn->last_message.l, auth_resp_packet.message, auth_resp_packet.message_len);
	ret = PASS;
end:
	PACKET_FREE(&auth_resp_packet);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_auth_change_user */
enum_func_status
mysqlnd_auth_change_user(MYSQLND_CONN_DATA * const conn,
								const char * const user,
								const size_t user_len,
								const char * const passwd,
								const size_t passwd_len,
								const char * const db,
								const size_t db_len,
								const zend_bool silent,
								const zend_bool use_full_blown_auth_packet,
								const char * const auth_protocol,
								struct st_mysqlnd_authentication_plugin * auth_plugin,
								const zend_uchar * const orig_auth_plugin_data,
								const size_t orig_auth_plugin_data_len,
								const zend_uchar * const auth_plugin_data,
								const size_t auth_plugin_data_len,
								char ** switch_to_auth_protocol,
								size_t * const switch_to_auth_protocol_len,
								zend_uchar ** switch_to_auth_protocol_data,
								size_t * const switch_to_auth_protocol_data_len
								)
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * old_cs = conn->charset;
	MYSQLND_PACKET_CHG_USER_RESPONSE chg_user_resp;

	DBG_ENTER("mysqlnd_auth_change_user");

	conn->payload_decoder_factory->m.init_change_user_response_packet(&chg_user_resp);

	if (use_full_blown_auth_packet != TRUE) {
		MYSQLND_PACKET_CHANGE_AUTH_RESPONSE change_auth_resp_packet;

		conn->payload_decoder_factory->m.init_change_auth_response_packet(&change_auth_resp_packet);

		change_auth_resp_packet.auth_data = auth_plugin_data;
		change_auth_resp_packet.auth_data_len = auth_plugin_data_len;

		if (!PACKET_WRITE(conn, &change_auth_resp_packet)) {
			SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			PACKET_FREE(&change_auth_resp_packet);
			goto end;
		}

		PACKET_FREE(&change_auth_resp_packet);
	} else {
		MYSQLND_PACKET_AUTH auth_packet;

		conn->payload_decoder_factory->m.init_auth_packet(&auth_packet);

		auth_packet.is_change_user_packet = TRUE;
		auth_packet.user		= user;
		auth_packet.db			= db;
		auth_packet.db_len		= db_len;
		auth_packet.silent		= silent;

		auth_packet.auth_data = auth_plugin_data;
		auth_packet.auth_data_len = auth_plugin_data_len;
		auth_packet.auth_plugin_name = auth_protocol;

        if (conn->server_capabilities & CLIENT_CONNECT_ATTRS) {
			auth_packet.connect_attr = conn->options->connect_attr;
        }

		if (conn->m->get_server_version(conn) >= 50123) {
			auth_packet.charset_no	= conn->charset->nr;
		}

		if (!PACKET_WRITE(conn, &auth_packet)) {
			SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			PACKET_FREE(&auth_packet);
			goto end;
		}

		PACKET_FREE(&auth_packet);
	}

	if (auth_plugin && auth_plugin->methods.handle_server_response) {
		if (FAIL == auth_plugin->methods.handle_server_response(auth_plugin, conn,
				orig_auth_plugin_data, orig_auth_plugin_data_len, passwd, passwd_len,
				switch_to_auth_protocol, switch_to_auth_protocol_len,
				switch_to_auth_protocol_data, switch_to_auth_protocol_data_len)) {
			goto end;
		}
	}

	ret = PACKET_READ(conn, &chg_user_resp);
	COPY_CLIENT_ERROR(conn->error_info, chg_user_resp.error_info);

	if (0xFE == chg_user_resp.response_code) {
		ret = FAIL;
		if (!chg_user_resp.new_auth_protocol) {
			DBG_ERR(mysqlnd_old_passwd);
			SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
		} else {
			*switch_to_auth_protocol = mnd_pestrndup(chg_user_resp.new_auth_protocol, chg_user_resp.new_auth_protocol_len, FALSE);
			*switch_to_auth_protocol_len = chg_user_resp.new_auth_protocol_len;
			if (chg_user_resp.new_auth_protocol_data) {
				*switch_to_auth_protocol_data_len = chg_user_resp.new_auth_protocol_data_len;
				*switch_to_auth_protocol_data = mnd_emalloc(*switch_to_auth_protocol_data_len);
				memcpy(*switch_to_auth_protocol_data, chg_user_resp.new_auth_protocol_data, *switch_to_auth_protocol_data_len);
			} else {
				*switch_to_auth_protocol_data = NULL;
				*switch_to_auth_protocol_data_len = 0;
			}
		}
	}

	if (conn->error_info->error_no) {
		ret = FAIL;
		/*
		  COM_CHANGE_USER is broken in 5.1. At least in 5.1.15 and 5.1.14, 5.1.11 is immune.
		  bug#25371 mysql_change_user() triggers "packets out of sync"
		  When it gets fixed, there should be one more check here
		*/
		if (conn->m->get_server_version(conn) > 50113L &&conn->m->get_server_version(conn) < 50118L) {
			MYSQLND_PACKET_OK redundant_error_packet;

			conn->payload_decoder_factory->m.init_ok_packet(&redundant_error_packet);
			PACKET_READ(conn, &redundant_error_packet);
			PACKET_FREE(&redundant_error_packet);
			DBG_INF_FMT("Server is %u, buggy, sends two ERR messages", conn->m->get_server_version(conn));
		}
	}
	if (ret == PASS) {
		char * tmp = NULL;
		/* if we get conn->username as parameter and then we first free it, then estrndup it, we will crash */
		tmp = mnd_pestrndup(user, user_len, conn->persistent);
		if (conn->username.s) {
			mnd_pefree(conn->username.s, conn->persistent);
		}
		conn->username.s = tmp;

		tmp = mnd_pestrdup(passwd, conn->persistent);
		if (conn->password.s) {
			mnd_pefree(conn->password.s, conn->persistent);
		}
		conn->password.s = tmp;

		if (conn->last_message.s) {
			mnd_efree(conn->last_message.s);
			conn->last_message.s = NULL;
		}
		UPSERT_STATUS_RESET(conn->upsert_status);
		/* set charset for old servers */
		if (conn->m->get_server_version(conn) < 50123) {
			ret = conn->m->set_charset(conn, old_cs->name);
		}
	} else if (ret == FAIL && chg_user_resp.server_asked_323_auth == TRUE) {
		/* old authentication with new server  !*/
		DBG_ERR(mysqlnd_old_passwd);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
	}
end:
	PACKET_FREE(&chg_user_resp);
	DBG_RETURN(ret);
}
/* }}} */


/******************************************* MySQL Native Password ***********************************/

#include "ext/standard/sha1.h"

/* {{{ php_mysqlnd_crypt */
static void
php_mysqlnd_crypt(zend_uchar *buffer, const zend_uchar *s1, const zend_uchar *s2, size_t len)
{
	const zend_uchar *s1_end = s1 + len;
	while (s1 < s1_end) {
		*buffer++= *s1++ ^ *s2++;
	}
}
/* }}} */


/* {{{ php_mysqlnd_scramble */
void php_mysqlnd_scramble(zend_uchar * const buffer, const zend_uchar * const scramble, const zend_uchar * const password, const size_t password_len)
{
	PHP_SHA1_CTX context;
	zend_uchar sha1[SHA1_MAX_LENGTH];
	zend_uchar sha2[SHA1_MAX_LENGTH];

	/* Phase 1: hash password */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, password, password_len);
	PHP_SHA1Final(sha1, &context);

	/* Phase 2: hash sha1 */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, (zend_uchar*)sha1, SHA1_MAX_LENGTH);
	PHP_SHA1Final(sha2, &context);

	/* Phase 3: hash scramble + sha2 */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, scramble, SCRAMBLE_LENGTH);
	PHP_SHA1Update(&context, (zend_uchar*)sha2, SHA1_MAX_LENGTH);
	PHP_SHA1Final(buffer, &context);

	/* let's crypt buffer now */
	php_mysqlnd_crypt(buffer, (const zend_uchar *)buffer, (const zend_uchar *)sha1, SHA1_MAX_LENGTH);
}
/* }}} */


/* {{{ mysqlnd_native_auth_get_auth_data */
static zend_uchar *
mysqlnd_native_auth_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
								  size_t * auth_data_len,
								  MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
								  const size_t passwd_len, zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
								  const MYSQLND_SESSION_OPTIONS * const session_options,
								  const MYSQLND_PFC_DATA * const pfc_data,
								  const zend_ulong mysql_flags
								 )
{
	zend_uchar * ret = NULL;
	DBG_ENTER("mysqlnd_native_auth_get_auth_data");
	*auth_data_len = 0;

	/* 5.5.x reports 21 as scramble length because it needs to show the length of the data before the plugin name */
	if (auth_plugin_data_len < SCRAMBLE_LENGTH) {
		/* mysql_native_password only works with SCRAMBLE_LENGTH scramble */
		SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "The server sent wrong length for scramble");
		DBG_ERR_FMT("The server sent wrong length for scramble %u. Expected %u", auth_plugin_data_len, SCRAMBLE_LENGTH);
		DBG_RETURN(NULL);
	}

	/* copy scrambled pass*/
	if (passwd && passwd_len) {
		ret = malloc(SCRAMBLE_LENGTH);
		*auth_data_len = SCRAMBLE_LENGTH;
		/* In 4.1 we use CLIENT_SECURE_CONNECTION and thus the len of the buf should be passed */
		php_mysqlnd_scramble((zend_uchar*)ret, auth_plugin_data, (zend_uchar*)passwd, passwd_len);
	}
	DBG_RETURN(ret);
}
/* }}} */


static struct st_mysqlnd_authentication_plugin mysqlnd_native_auth_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"auth_plugin_mysql_native_password",
		MYSQLND_VERSION_ID,
		PHP_MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@php.net>,  Ulf Wendel <uwendel@mysql.com>, Georg Richter <georg@mysql.com>",
		{
			NULL, /* no statistics , will be filled later if there are some */
			NULL, /* no statistics */
		},
		{
			NULL /* plugin shutdown */
		}
	},
	{/* methods */
		mysqlnd_native_auth_get_auth_data,
		NULL
	}
};


/******************************************* PAM Authentication ***********************************/

/* {{{ mysqlnd_pam_auth_get_auth_data */
static zend_uchar *
mysqlnd_pam_auth_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
							   size_t * auth_data_len,
							   MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
							   const size_t passwd_len, zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
							   const MYSQLND_SESSION_OPTIONS * const session_options,
							   const MYSQLND_PFC_DATA * const pfc_data,
							   const zend_ulong mysql_flags
							  )
{
	zend_uchar * ret = NULL;

	/* copy pass*/
	if (passwd && passwd_len) {
		ret = (zend_uchar*) zend_strndup(passwd, passwd_len);
	}
	/*
	  Trailing null required. bug#78680
	  https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_connection_phase_authentication_methods_clear_text_password.html
	*/
	*auth_data_len = passwd_len + 1;

	return ret;
}
/* }}} */


static struct st_mysqlnd_authentication_plugin mysqlnd_pam_authentication_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"auth_plugin_mysql_clear_password",
		MYSQLND_VERSION_ID,
		PHP_MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@php.net>,  Ulf Wendel <uw@php.net>, Georg Richter <georg@php.net>",
		{
			NULL, /* no statistics , will be filled later if there are some */
			NULL, /* no statistics */
		},
		{
			NULL /* plugin shutdown */
		}
	},
	{/* methods */
		mysqlnd_pam_auth_get_auth_data,
		NULL
	}
};


/******************************************* SHA256 Password ***********************************/
#ifdef MYSQLND_HAVE_SSL
static void
mysqlnd_xor_string(char * dst, const size_t dst_len, const char * xor_str, const size_t xor_str_len)
{
	unsigned int i;
	for (i = 0; i <= dst_len; ++i) {
		dst[i] ^= xor_str[i % xor_str_len];
	}
}

#ifndef PHP_WIN32

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

typedef RSA * mysqlnd_rsa_t;

/* {{{ mysqlnd_sha256_get_rsa_from_pem */
static mysqlnd_rsa_t
mysqlnd_sha256_get_rsa_from_pem(const char *buf, size_t len)
{
	BIO * bio = BIO_new_mem_buf(buf, len);
	RSA * ret = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	BIO_free(bio);
	return ret;
}
/* }}} */

/* {{{ mysqlnd_sha256_public_encrypt */
static zend_uchar *
mysqlnd_sha256_public_encrypt(MYSQLND_CONN_DATA * conn, mysqlnd_rsa_t server_public_key, size_t passwd_len, size_t * auth_data_len, char *xor_str)
{
	zend_uchar * ret = NULL;
	size_t server_public_key_len = (size_t) RSA_size(server_public_key);

	DBG_ENTER("mysqlnd_sha256_public_encrypt");
	/*
		Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
		RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
		http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
	*/
	if (server_public_key_len <= passwd_len + 41) {
		/* password message is to long */
		RSA_free(server_public_key);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
		DBG_ERR("password is too long");
		DBG_RETURN(NULL);
	}

	*auth_data_len = server_public_key_len;
	ret = malloc(*auth_data_len);
	RSA_public_encrypt(passwd_len + 1, (zend_uchar *) xor_str, ret, server_public_key, RSA_PKCS1_OAEP_PADDING);
	RSA_free(server_public_key);
	DBG_RETURN(ret);
}
/* }}} */

#else

#include <wincrypt.h>
#include <bcrypt.h>

typedef HANDLE mysqlnd_rsa_t;

/* {{{ mysqlnd_sha256_get_rsa_from_pem */
static mysqlnd_rsa_t
mysqlnd_sha256_get_rsa_from_pem(const char *buf, size_t len)
{
	BCRYPT_KEY_HANDLE ret = 0;
	LPSTR der_buf = NULL;
	DWORD der_len;
	CERT_PUBLIC_KEY_INFO *key_info = NULL;
	DWORD key_info_len;
	ALLOCA_FLAG(use_heap);

	if (!CryptStringToBinaryA(buf, len, CRYPT_STRING_BASE64HEADER, NULL, &der_len, NULL, NULL)) {
		goto finish;
	}
	der_buf = do_alloca(der_len, use_heap);
	if (!CryptStringToBinaryA(buf, len, CRYPT_STRING_BASE64HEADER, der_buf, &der_len, NULL, NULL)) {
		goto finish;
	}
	if (!CryptDecodeObjectEx(X509_ASN_ENCODING, X509_PUBLIC_KEY_INFO, der_buf, der_len, CRYPT_ENCODE_ALLOC_FLAG, NULL, &key_info, &key_info_len)) {
		goto finish;
	}
	if (!CryptImportPublicKeyInfoEx2(X509_ASN_ENCODING, key_info, CRYPT_OID_INFO_PUBKEY_ENCRYPT_KEY_FLAG, NULL, &ret)) {
		goto finish;
	}

finish:
	if (key_info) {
		LocalFree(key_info);
	}
	if (der_buf) {
		free_alloca(der_buf, use_heap);
	}
	return (mysqlnd_rsa_t) ret;
}
/* }}} */

/* {{{ mysqlnd_sha256_public_encrypt */
static zend_uchar *
mysqlnd_sha256_public_encrypt(MYSQLND_CONN_DATA * conn, mysqlnd_rsa_t server_public_key, size_t passwd_len, size_t * auth_data_len, char *xor_str)
{
	zend_uchar * ret = NULL;
	DWORD server_public_key_len = passwd_len;
	BCRYPT_OAEP_PADDING_INFO padding_info;

	DBG_ENTER("mysqlnd_sha256_public_encrypt");

	ZeroMemory(&padding_info, sizeof padding_info);
	padding_info.pszAlgId = BCRYPT_SHA1_ALGORITHM;
	if (BCryptEncrypt((BCRYPT_KEY_HANDLE) server_public_key, xor_str, passwd_len + 1, &padding_info,
			NULL, 0, NULL, 0, &server_public_key_len, BCRYPT_PAD_OAEP)) {
		DBG_RETURN(0);
	}

	/*
		Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
		RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
		http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
	*/
	if ((size_t) server_public_key_len <= passwd_len + 41) {
		/* password message is to long */
		BCryptDestroyKey((BCRYPT_KEY_HANDLE) server_public_key);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
		DBG_ERR("password is too long");
		DBG_RETURN(0);
	}

	*auth_data_len = server_public_key_len;
	ret = malloc(*auth_data_len);
	if (BCryptEncrypt((BCRYPT_KEY_HANDLE) server_public_key, xor_str, passwd_len + 1, &padding_info,
			NULL, 0, ret, server_public_key_len, &server_public_key_len, BCRYPT_PAD_OAEP)) {
		BCryptDestroyKey((BCRYPT_KEY_HANDLE) server_public_key);
		DBG_RETURN(0);
	}
	BCryptDestroyKey((BCRYPT_KEY_HANDLE) server_public_key);
	DBG_RETURN(ret);
}
/* }}} */

#endif

/* {{{ mysqlnd_sha256_get_rsa_key */
static mysqlnd_rsa_t
mysqlnd_sha256_get_rsa_key(MYSQLND_CONN_DATA * conn,
						   const MYSQLND_SESSION_OPTIONS * const session_options,
						   const MYSQLND_PFC_DATA * const pfc_data
						  )
{
	mysqlnd_rsa_t ret = NULL;
	const char * fname = (pfc_data->sha256_server_public_key && pfc_data->sha256_server_public_key[0] != '\0')?
								pfc_data->sha256_server_public_key:
								MYSQLND_G(sha256_server_public_key);
	php_stream * stream;
	DBG_ENTER("mysqlnd_sha256_get_rsa_key");
	DBG_INF_FMT("options_s256_pk=[%s] MYSQLND_G(sha256_server_public_key)=[%s]",
				 pfc_data->sha256_server_public_key? pfc_data->sha256_server_public_key:"n/a",
				 MYSQLND_G(sha256_server_public_key)? MYSQLND_G(sha256_server_public_key):"n/a");
	if (!fname || fname[0] == '\0') {
		MYSQLND_PACKET_SHA256_PK_REQUEST pk_req_packet;
		MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE pk_resp_packet;

		do {
			DBG_INF("requesting the public key from the server");
			conn->payload_decoder_factory->m.init_sha256_pk_request_packet(&pk_req_packet);
			conn->payload_decoder_factory->m.init_sha256_pk_request_response_packet(&pk_resp_packet);

			if (! PACKET_WRITE(conn, &pk_req_packet)) {
				DBG_ERR_FMT("Error while sending public key request packet");
				php_error(E_WARNING, "Error while sending public key request packet. PID=%d", getpid());
				SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
				break;
			}
			if (FAIL == PACKET_READ(conn, &pk_resp_packet) || NULL == pk_resp_packet.public_key) {
				DBG_ERR_FMT("Error while receiving public key");
				php_error(E_WARNING, "Error while receiving public key. PID=%d", getpid());
				SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
				break;
			}
			DBG_INF_FMT("Public key(%d):\n%s", pk_resp_packet.public_key_len, pk_resp_packet.public_key);
			/* now extract the public key */
			ret = mysqlnd_sha256_get_rsa_from_pem((const char *) pk_resp_packet.public_key, pk_resp_packet.public_key_len);
		} while (0);
		PACKET_FREE(&pk_req_packet);
		PACKET_FREE(&pk_resp_packet);

		DBG_INF_FMT("ret=%p", ret);
		DBG_RETURN(ret);

		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE,
			"sha256_server_public_key is not set for the connection or as mysqlnd.sha256_server_public_key");
		DBG_ERR("server_public_key is not set");
		DBG_RETURN(NULL);
	} else {
		zend_string * key_str;
		DBG_INF_FMT("Key in a file. [%s]", fname);
		stream = php_stream_open_wrapper((char *) fname, "rb", REPORT_ERRORS, NULL);

		if (stream) {
			if ((key_str = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0)) != NULL) {
				ret = mysqlnd_sha256_get_rsa_from_pem(ZSTR_VAL(key_str), ZSTR_LEN(key_str));
				DBG_INF("Successfully loaded");
				DBG_INF_FMT("Public key:%*.s", ZSTR_LEN(key_str), ZSTR_VAL(key_str));
				zend_string_release_ex(key_str, 0);
			}
			php_stream_close(stream);
		}
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_sha256_auth_get_auth_data */
static zend_uchar *
mysqlnd_sha256_auth_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
								  size_t * auth_data_len,
								  MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
								  const size_t passwd_len, zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
								  const MYSQLND_SESSION_OPTIONS * const session_options,
								  const MYSQLND_PFC_DATA * const pfc_data,
								  const zend_ulong mysql_flags
								 )
{
	mysqlnd_rsa_t server_public_key;
	zend_uchar * ret = NULL;
	DBG_ENTER("mysqlnd_sha256_auth_get_auth_data");
	DBG_INF_FMT("salt(%d)=[%.*s]", auth_plugin_data_len, auth_plugin_data_len, auth_plugin_data);


	if (conn->vio->data->ssl) {
		DBG_INF("simple clear text under SSL");
		/* clear text under SSL */
		*auth_data_len = passwd_len;
		ret = malloc(passwd_len);
		memcpy(ret, passwd, passwd_len);
	} else {
		*auth_data_len = 0;
		server_public_key = mysqlnd_sha256_get_rsa_key(conn, session_options, pfc_data);

		if (server_public_key) {
			ALLOCA_FLAG(use_heap);
			char *xor_str = do_alloca(passwd_len + 1, use_heap);
			memcpy(xor_str, passwd, passwd_len);
			xor_str[passwd_len] = '\0';
			mysqlnd_xor_string(xor_str, passwd_len, (char *) auth_plugin_data, auth_plugin_data_len);
			ret = mysqlnd_sha256_public_encrypt(conn, server_public_key, passwd_len, auth_data_len, xor_str);
			free_alloca(xor_str, use_heap);
		}
	}

	DBG_RETURN(ret);
}
/* }}} */


static struct st_mysqlnd_authentication_plugin mysqlnd_sha256_authentication_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"auth_plugin_sha256_password",
		MYSQLND_VERSION_ID,
		PHP_MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@php.net>,  Ulf Wendel <uwendel@mysql.com>",
		{
			NULL, /* no statistics , will be filled later if there are some */
			NULL, /* no statistics */
		},
		{
			NULL /* plugin shutdown */
		}
	},
	{/* methods */
		mysqlnd_sha256_auth_get_auth_data,
		NULL
	}
};
#endif

/*************************************** CACHING SHA2 Password *******************************/
#ifdef MYSQLND_HAVE_SSL

#undef L64

#include "ext/hash/php_hash.h"
#include "ext/hash/php_hash_sha.h"

#define SHA256_LENGTH 32

/* {{{ php_mysqlnd_scramble_sha2 */
void php_mysqlnd_scramble_sha2(zend_uchar * const buffer, const zend_uchar * const scramble, const zend_uchar * const password, const size_t password_len)
{
	PHP_SHA256_CTX context;
	zend_uchar sha1[SHA256_LENGTH];
	zend_uchar sha2[SHA256_LENGTH];

	/* Phase 1: hash password */
	PHP_SHA256Init(&context);
	PHP_SHA256Update(&context, password, password_len);
	PHP_SHA256Final(sha1, &context);

	/* Phase 2: hash sha1 */
	PHP_SHA256Init(&context);
	PHP_SHA256Update(&context, (zend_uchar*)sha1, SHA256_LENGTH);
	PHP_SHA256Final(sha2, &context);

	/* Phase 3: hash scramble + sha2 */
	PHP_SHA256Init(&context);
	PHP_SHA256Update(&context, (zend_uchar*)sha2, SHA256_LENGTH);
	PHP_SHA256Update(&context, scramble, SCRAMBLE_LENGTH);
	PHP_SHA256Final(buffer, &context);

	/* let's crypt buffer now */
	php_mysqlnd_crypt(buffer, (const zend_uchar *)sha1, (const zend_uchar *)buffer, SHA256_LENGTH);
}
/* }}} */

#ifndef PHP_WIN32

/* {{{ mysqlnd_caching_sha2_public_encrypt */
static size_t
mysqlnd_caching_sha2_public_encrypt(MYSQLND_CONN_DATA * conn, mysqlnd_rsa_t server_public_key, size_t passwd_len, unsigned char **crypted, char *xor_str)
{
	size_t server_public_key_len = (size_t) RSA_size(server_public_key);

	DBG_ENTER("mysqlnd_caching_sha2_public_encrypt");
	/*
		Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
		RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
		http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
	*/
	if (server_public_key_len <= passwd_len + 41) {
		/* password message is to long */
		RSA_free(server_public_key);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
		DBG_ERR("password is too long");
		DBG_RETURN(0);
	}

	*crypted = emalloc(server_public_key_len);
	RSA_public_encrypt(passwd_len + 1, (zend_uchar *) xor_str, *crypted, server_public_key, RSA_PKCS1_OAEP_PADDING);
	RSA_free(server_public_key);
	DBG_RETURN(server_public_key_len);
}
/* }}} */

#else

/* {{{ mysqlnd_caching_sha2_public_encrypt */
static size_t
mysqlnd_caching_sha2_public_encrypt(MYSQLND_CONN_DATA * conn, mysqlnd_rsa_t server_public_key, size_t passwd_len, unsigned char **crypted, char *xor_str)
{
	DWORD server_public_key_len = passwd_len;
	BCRYPT_OAEP_PADDING_INFO padding_info;

	DBG_ENTER("mysqlnd_caching_sha2_public_encrypt");

	ZeroMemory(&padding_info, sizeof padding_info);
	padding_info.pszAlgId = BCRYPT_SHA1_ALGORITHM;
	if (BCryptEncrypt((BCRYPT_KEY_HANDLE) server_public_key, xor_str, passwd_len + 1, &padding_info,
			NULL, 0, NULL, 0, &server_public_key_len, BCRYPT_PAD_OAEP)) {
		DBG_RETURN(0);
	}

	/*
		Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
		RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
		http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
	*/
	if ((size_t) server_public_key_len <= passwd_len + 41) {
		/* password message is to long */
		BCryptDestroyKey((BCRYPT_KEY_HANDLE) server_public_key);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
		DBG_ERR("password is too long");
		DBG_RETURN(0);
	}

	*crypted = emalloc(server_public_key_len);
	if (BCryptEncrypt((BCRYPT_KEY_HANDLE) server_public_key, xor_str, passwd_len + 1, &padding_info,
			NULL, 0, *crypted, server_public_key_len, &server_public_key_len, BCRYPT_PAD_OAEP)) {
		BCryptDestroyKey((BCRYPT_KEY_HANDLE) server_public_key);
		DBG_RETURN(0);
	}
	BCryptDestroyKey((BCRYPT_KEY_HANDLE) server_public_key);
	DBG_RETURN(server_public_key_len);
}
/* }}} */

#endif

/* {{{ mysqlnd_native_auth_get_auth_data */
static zend_uchar *
mysqlnd_caching_sha2_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
								   size_t * auth_data_len,
							 	   MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
								   const size_t passwd_len, zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
								   const MYSQLND_SESSION_OPTIONS * const session_options,
								   const MYSQLND_PFC_DATA * const pfc_data,
								   const zend_ulong mysql_flags
								  )
{
	zend_uchar * ret = NULL;
	DBG_ENTER("mysqlnd_caching_sha2_get_auth_data");
	DBG_INF_FMT("salt(%d)=[%.*s]", auth_plugin_data_len, auth_plugin_data_len, auth_plugin_data);
	*auth_data_len = 0;

	if (auth_plugin_data_len < SCRAMBLE_LENGTH) {
		SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "The server sent wrong length for scramble");
		DBG_ERR_FMT("The server sent wrong length for scramble %u. Expected %u", auth_plugin_data_len, SCRAMBLE_LENGTH);
		DBG_RETURN(NULL);
	}

	DBG_INF("First auth step: send hashed password");
	/* copy scrambled pass*/
	if (passwd && passwd_len) {
		ret = malloc(SHA256_LENGTH + 1);
		*auth_data_len = SHA256_LENGTH;
		php_mysqlnd_scramble_sha2((zend_uchar*)ret, auth_plugin_data, (zend_uchar*)passwd, passwd_len);
		ret[SHA256_LENGTH] = '\0';
		DBG_INF_FMT("hash(%d)=[%.*s]", *auth_data_len, *auth_data_len, ret);
	}

	DBG_RETURN(ret);
}
/* }}} */

static mysqlnd_rsa_t
mysqlnd_caching_sha2_get_key(MYSQLND_CONN_DATA *conn)
{
	mysqlnd_rsa_t ret = NULL;
	const MYSQLND_PFC_DATA * const pfc_data = conn->protocol_frame_codec->data;
	const char * fname = (pfc_data->sha256_server_public_key && pfc_data->sha256_server_public_key[0] != '\0')?
								pfc_data->sha256_server_public_key:
								MYSQLND_G(sha256_server_public_key);
	php_stream * stream;
	DBG_ENTER("mysqlnd_cached_sha2_get_key");
	DBG_INF_FMT("options_s256_pk=[%s] MYSQLND_G(sha256_server_public_key)=[%s]",
				 pfc_data->sha256_server_public_key? pfc_data->sha256_server_public_key:"n/a",
				 MYSQLND_G(sha256_server_public_key)? MYSQLND_G(sha256_server_public_key):"n/a");
	if (!fname || fname[0] == '\0') {
		MYSQLND_PACKET_CACHED_SHA2_RESULT req_packet;
		MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE pk_resp_packet;

		do {
			DBG_INF("requesting the public key from the server");
			conn->payload_decoder_factory->m.init_cached_sha2_result_packet(&req_packet);
			conn->payload_decoder_factory->m.init_sha256_pk_request_response_packet(&pk_resp_packet);
			req_packet.request = 1;

			if (! PACKET_WRITE(conn, &req_packet)) {
				DBG_ERR_FMT("Error while sending public key request packet");
				php_error(E_WARNING, "Error while sending public key request packet. PID=%d", getpid());
				SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
				break;
			}
			if (FAIL == PACKET_READ(conn, &pk_resp_packet) || NULL == pk_resp_packet.public_key) {
				DBG_ERR_FMT("Error while receiving public key");
				php_error(E_WARNING, "Error while receiving public key. PID=%d", getpid());
				SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
				break;
			}
			DBG_INF_FMT("Public key(%d):\n%s", pk_resp_packet.public_key_len, pk_resp_packet.public_key);
			/* now extract the public key */
			ret = mysqlnd_sha256_get_rsa_from_pem((const char *) pk_resp_packet.public_key, pk_resp_packet.public_key_len);
		} while (0);
		PACKET_FREE(&req_packet);
		PACKET_FREE(&pk_resp_packet);

		DBG_INF_FMT("ret=%p", ret);
		DBG_RETURN(ret);

		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE,
			"caching_sha2_server_public_key is not set for the connection or as mysqlnd.sha256_server_public_key");
		DBG_ERR("server_public_key is not set");
		DBG_RETURN(NULL);
	} else {
		zend_string * key_str;
		DBG_INF_FMT("Key in a file. [%s]", fname);
		stream = php_stream_open_wrapper((char *) fname, "rb", REPORT_ERRORS, NULL);

		if (stream) {
			if ((key_str = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0)) != NULL) {
				ret = mysqlnd_sha256_get_rsa_from_pem(ZSTR_VAL(key_str), ZSTR_LEN(key_str));
				DBG_INF("Successfully loaded");
				DBG_INF_FMT("Public key:%*.s", ZSTR_LEN(key_str), ZSTR_VAL(key_str));
				zend_string_release(key_str);
			}
			php_stream_close(stream);
		}
	}
	DBG_RETURN(ret);

}


/* {{{ mysqlnd_caching_sha2_get_and_use_key */
static size_t
mysqlnd_caching_sha2_get_and_use_key(MYSQLND_CONN_DATA *conn,
		const zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
		unsigned char **crypted,
		const char * const passwd,
		const size_t passwd_len)
{
	mysqlnd_rsa_t server_public_key = mysqlnd_caching_sha2_get_key(conn);

	DBG_ENTER("mysqlnd_caching_sha2_get_and_use_key(");

	if (server_public_key) {
		int server_public_key_len;
		ALLOCA_FLAG(use_heap)
		char *xor_str = do_alloca(passwd_len + 1, use_heap);
		memcpy(xor_str, passwd, passwd_len);
		xor_str[passwd_len] = '\0';
		mysqlnd_xor_string(xor_str, passwd_len, (char *) auth_plugin_data, SCRAMBLE_LENGTH);
		server_public_key_len = mysqlnd_caching_sha2_public_encrypt(conn, server_public_key, passwd_len, crypted, xor_str);
		free_alloca(xor_str, use_heap);
		DBG_RETURN(server_public_key_len);
	}
	DBG_RETURN(0);
}
/* }}} */

static int is_secure_transport(MYSQLND_CONN_DATA *conn) {
	if (conn->vio->data->ssl) {
		return 1;
	}

	return strcmp(conn->vio->data->stream->ops->label, "unix_socket") == 0;
}

/* {{{ mysqlnd_caching_sha2_handle_server_response */
static enum_func_status
mysqlnd_caching_sha2_handle_server_response(struct st_mysqlnd_authentication_plugin *self,
		MYSQLND_CONN_DATA * conn,
		const zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
		const char * const passwd,
		const size_t passwd_len,
		char **new_auth_protocol, size_t *new_auth_protocol_len,
		zend_uchar **new_auth_protocol_data, size_t *new_auth_protocol_data_len
		)
{
	DBG_ENTER("mysqlnd_caching_sha2_handle_server_response");
	MYSQLND_PACKET_CACHED_SHA2_RESULT result_packet;

	if (passwd_len == 0) {
		DBG_INF("empty password fast path");
		DBG_RETURN(PASS);
	}

	conn->payload_decoder_factory->m.init_cached_sha2_result_packet(&result_packet);
	if (FAIL == PACKET_READ(conn, &result_packet)) {
		DBG_RETURN(PASS);
	}

	switch (result_packet.response_code) {
		case 0xFF:
			if (result_packet.sqlstate[0]) {
				strlcpy(conn->error_info->sqlstate, result_packet.sqlstate, sizeof(conn->error_info->sqlstate));
				DBG_ERR_FMT("ERROR:%u [SQLSTATE:%s] %s", result_packet.error_no, result_packet.sqlstate, result_packet.error);
			}
			SET_CLIENT_ERROR(conn->error_info, result_packet.error_no, UNKNOWN_SQLSTATE, result_packet.error);
			DBG_RETURN(FAIL);
		case 0xFE:
			DBG_INF("auth switch response");
			*new_auth_protocol = result_packet.new_auth_protocol;
			*new_auth_protocol_len = result_packet.new_auth_protocol_len;
			*new_auth_protocol_data = result_packet.new_auth_protocol_data;
			*new_auth_protocol_data_len = result_packet.new_auth_protocol_data_len;
			DBG_RETURN(FAIL);
		case 3:
			DBG_INF("fast path succeeded");
			DBG_RETURN(PASS);
		case 4:
			if (is_secure_transport(conn)) {
				DBG_INF("fast path failed, doing full auth via secure transport");
				result_packet.password = (zend_uchar *)passwd;
				result_packet.password_len = passwd_len + 1;
				PACKET_WRITE(conn, &result_packet);
			} else {
				DBG_INF("fast path failed, doing full auth via insecure transport");
				result_packet.password_len = mysqlnd_caching_sha2_get_and_use_key(conn, auth_plugin_data, auth_plugin_data_len, &result_packet.password, passwd, passwd_len);
				PACKET_WRITE(conn, &result_packet);
				efree(result_packet.password);
			}
			DBG_RETURN(PASS);
		case 2:
			// The server tried to send a key, which we didn't expect
			// fall-through
		default:
			php_error_docref(NULL, E_WARNING, "Unexpected server response while doing caching_sha2 auth: %i", result_packet.response_code);
	}

	DBG_RETURN(PASS);
}
/* }}} */

static struct st_mysqlnd_authentication_plugin mysqlnd_caching_sha2_auth_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"auth_plugin_caching_sha2_password",
		MYSQLND_VERSION_ID,
		PHP_MYSQLND_VERSION,
		"PHP License 3.01",
		"Johannes Schlüter <johannes.schlueter@php.net>",
		{
			NULL, /* no statistics , will be filled later if there are some */
			NULL, /* no statistics */
		},
		{
			NULL /* plugin shutdown */
		}
	},
	{/* methods */
		mysqlnd_caching_sha2_get_auth_data,
		mysqlnd_caching_sha2_handle_server_response
	}
};
#endif


/* {{{ mysqlnd_register_builtin_authentication_plugins */
void
mysqlnd_register_builtin_authentication_plugins(void)
{
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_native_auth_plugin);
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_pam_authentication_plugin);
#ifdef MYSQLND_HAVE_SSL
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_caching_sha2_auth_plugin);
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_sha256_authentication_plugin);
#endif
}
/* }}} */
