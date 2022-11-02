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
#include "mysqlnd_structs.h"
#include "mysqlnd_auth.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_connection.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"
#include <string.h>
#ifdef MYSQLND_HAVE_SASL
#ifdef MYSQLND_HAVE_KRB5
#include <krb5/krb5.h>
#endif
#include <sasl/sasl.h>
#include <profile.h>
#endif
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
			const bool silent,
			const bool is_change_user
			)
{
	enum_func_status ret = FAIL;
	bool first_call = TRUE;

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
				char * msg;
				mnd_sprintf(&msg, 0, "The server requested authentication method unknown to the client [%s]", requested_protocol);
				SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, msg);
				mnd_sprintf_free(msg);
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

			mysqlnd_set_persistent_string(&conn->authentication_plugin_data, NULL, 0, conn->persistent);
			conn->authentication_plugin_data.l = plugin_data_len;
			conn->authentication_plugin_data.s = mnd_pemalloc(conn->authentication_plugin_data.l, conn->persistent);
			memcpy(conn->authentication_plugin_data.s, plugin_data, plugin_data_len);

			DBG_INF_FMT("salt(%zu)=[%.*s]", plugin_data_len, (int) plugin_data_len, plugin_data);
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

            if( auth_plugin
                && requested_protocol
                && !strcmp(requested_protocol,"authentication_ldap_sasl_client")
                && auth_plugin->methods.handle_server_response)
            {
                if (FAIL == auth_plugin->methods.handle_server_response(auth_plugin, conn,
                            scrambled_data, scrambled_data_len, user, passwd, passwd_len,
                            &switch_to_auth_protocol, &switch_to_auth_protocol_len,
                            &switch_to_auth_protocol_data, &switch_to_auth_protocol_data_len)){
                    ret = FAIL;
                }
            }
            else
            {
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
							  const bool use_full_blown_auth_packet,
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
                orig_auth_plugin_data, orig_auth_plugin_data_len, user, passwd, passwd_len,
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

	mysqlnd_set_string(&conn->last_message, auth_resp_packet.message, auth_resp_packet.message_len);
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
								const bool silent,
								const bool use_full_blown_auth_packet,
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
                orig_auth_plugin_data, orig_auth_plugin_data_len, user, passwd, passwd_len,
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
			DBG_INF_FMT("Server is " ZEND_ULONG_FMT ", buggy, sends two ERR messages", conn->m->get_server_version(conn));
		}
	}
	if (ret == PASS) {
		ZEND_ASSERT(conn->username.s != user && conn->password.s != passwd);
		mysqlnd_set_persistent_string(&conn->username, user, user_len, conn->persistent);
		mysqlnd_set_persistent_string(&conn->password, passwd, passwd_len, conn->persistent);

		mysqlnd_set_string(&conn->last_message, NULL, 0);
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
		DBG_ERR_FMT("The server sent wrong length for scramble %zu. Expected %u", auth_plugin_data_len, SCRAMBLE_LENGTH);
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

typedef EVP_PKEY * mysqlnd_rsa_t;

/* {{{ mysqlnd_sha256_get_rsa_from_pem */
static mysqlnd_rsa_t
mysqlnd_sha256_get_rsa_from_pem(const char *buf, size_t len)
{
	BIO *bio = BIO_new_mem_buf(buf, len);
	EVP_PKEY *ret = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
	BIO_free(bio);
	return ret;
}
/* }}} */

/* {{{ mysqlnd_sha256_public_encrypt */
static zend_uchar *
mysqlnd_sha256_public_encrypt(MYSQLND_CONN_DATA * conn, mysqlnd_rsa_t server_public_key, size_t passwd_len, size_t * auth_data_len, char *xor_str)
{
	zend_uchar * ret = NULL;
	size_t server_public_key_len = (size_t) EVP_PKEY_size(server_public_key);

	DBG_ENTER("mysqlnd_sha256_public_encrypt");
	/*
		Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
		RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
		http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
	*/
	if (server_public_key_len <= passwd_len + 41) {
		/* password message is to long */
		EVP_PKEY_free(server_public_key);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
		DBG_ERR("password is too long");
		DBG_RETURN(NULL);
	}

	*auth_data_len = server_public_key_len;
	ret = malloc(*auth_data_len);
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(server_public_key, NULL);
	if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0 ||
			EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0 ||
			EVP_PKEY_encrypt(ctx, ret, &server_public_key_len, (zend_uchar *) xor_str, passwd_len + 1) <= 0) {
		DBG_ERR("encrypt failed");
		free(ret);
		ret = NULL;
	}
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(server_public_key);
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
			DBG_INF_FMT("Public key(%zu):\n%s", pk_resp_packet.public_key_len, pk_resp_packet.public_key);
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
				DBG_INF_FMT("Public key:%*.s", (int) ZSTR_LEN(key_str), ZSTR_VAL(key_str));
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
	DBG_INF_FMT("salt(%zu)=[%.*s]", auth_plugin_data_len, (int) auth_plugin_data_len, auth_plugin_data);


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
	size_t server_public_key_len = (size_t) EVP_PKEY_size(server_public_key);

	DBG_ENTER("mysqlnd_caching_sha2_public_encrypt");
	/*
		Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
		RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
		http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
	*/
	if (server_public_key_len <= passwd_len + 41) {
		/* password message is to long */
		EVP_PKEY_free(server_public_key);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
		DBG_ERR("password is too long");
		DBG_RETURN(0);
	}

	*crypted = emalloc(server_public_key_len);
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(server_public_key, NULL);
	if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0 ||
			EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0 ||
			EVP_PKEY_encrypt(ctx, *crypted, &server_public_key_len, (zend_uchar *) xor_str, passwd_len + 1) <= 0) {
		DBG_ERR("encrypt failed");
		server_public_key_len = 0;
	}
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(server_public_key);
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
	DBG_INF_FMT("salt(%zu)=[%.*s]", auth_plugin_data_len, (int) auth_plugin_data_len, auth_plugin_data);
	*auth_data_len = 0;

	if (auth_plugin_data_len < SCRAMBLE_LENGTH) {
		SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "The server sent wrong length for scramble");
		DBG_ERR_FMT("The server sent wrong length for scramble %zu. Expected %u", auth_plugin_data_len, SCRAMBLE_LENGTH);
		DBG_RETURN(NULL);
	}

	DBG_INF("First auth step: send hashed password");
	/* copy scrambled pass*/
	if (passwd && passwd_len) {
		ret = malloc(SHA256_LENGTH + 1);
		*auth_data_len = SHA256_LENGTH;
		php_mysqlnd_scramble_sha2((zend_uchar*)ret, auth_plugin_data, (zend_uchar*)passwd, passwd_len);
		ret[SHA256_LENGTH] = '\0';
		DBG_INF_FMT("hash(%zu)=[%.*s]", *auth_data_len, (int) *auth_data_len, ret);
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
			DBG_INF_FMT("Public key(%zu):\n%s", pk_resp_packet.public_key_len, pk_resp_packet.public_key);
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
				DBG_INF_FMT("Public key: %*.s", (int) ZSTR_LEN(key_str), ZSTR_VAL(key_str));
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
        const char * const user,
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
		default: {
			char * msg;
			mnd_sprintf(&msg, 0, "Unexpected server response while doing caching_sha2 auth: %i", result_packet.response_code);
			SET_CLIENT_ERROR(conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, msg);
			mnd_sprintf_free(msg);
		}
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

#ifdef MYSQLND_HAVE_SASL

/******************************************* LDAP SASL ***********************************/

#define SASL_SERVICE_NAME "ldap"
#define SASL_MAX_PKT_SIZE 1518

const char SASL_GSSAPI[] = "GSSAPI";
const char SASL_SCRAM_SHA1[] = "SCRAM-SHA-1";
const char SASL_SCRAM_SHA256[] = "SCRAM-SHA-256";

static const sasl_callback_t sasl_op_callbacks[] = {
    #ifdef SASL_CB_GETREALM
    {SASL_CB_GETREALM, NULL, NULL},
    #endif
    {SASL_CB_USER, NULL, NULL},
    {SASL_CB_AUTHNAME, NULL, NULL},
    {SASL_CB_PASS, NULL, NULL},
    {SASL_CB_ECHOPROMPT, NULL, NULL},
    {SASL_CB_NOECHOPROMPT, NULL, NULL},
    {SASL_CB_LIST_END, NULL, NULL}
};

/*
  MAX SSF - The maximum Security Strength Factor supported by the mechanism
  (roughly the number of bits of encryption provided, but may have other
  meanings, for example an SSF of 1 indicates integrity protection only, no
  encryption). SECURITY PROPERTIES are: NOPLAIN, NOACTIVE, NODICT, FORWARD,
  NOANON, CRED, MUTUAL. More details are in:
  https://www.sendmail.org/~ca/email/cyrus2/mechanisms.html
*/
sasl_security_properties_t security_properties = {
    /** Minimum acceptable final level. (min_ssf) */
    56,
    /** Maximum acceptable final level. (max_ssf) */
    0,
    /** Maximum security layer receive buffer size. */
    0,
    /** security flags (security_flags) */
    0,
    /** Property names. (property_names) */
    NULL,
    /** Property values. (property_values)*/
    NULL,
};


/* {{{ handle_comm */
void handle_comm(
        sasl_interact_t *ilist,
        const char * const user,
        const char * const passwd
)
{
    DBG_ENTER("handle_comm");

    while (ilist->id != SASL_CB_LIST_END) {
        switch (ilist->id) {
        /*
        the name of the user authenticating
      */
        case SASL_CB_USER:
            ilist->result = user;
            ilist->len = strlen((const char *)ilist->result);
            break;
        case SASL_CB_AUTHNAME:
            ilist->result = user;
            ilist->len = strlen((const char *)ilist->result);
            break;
        case SASL_CB_PASS:
            ilist->result = passwd;
            ilist->len = strlen((const char *)ilist->result);
            break;
        default:
            ilist->result = NULL;
            ilist->len = 0;
        }
        ilist++;
    }
    DBG_VOID_RETURN;
}
/* }}} */


/* {{{ sasl_run */
static
int sasl_run(
        sasl_conn_t* connection,
        const char* auth_mechanism,
        const char * const user,
        const char * const passwd,
        char **client_output,
        int *client_output_length
        )
{
    DBG_ENTER("sasl_run");

    int rc_sasl = SASL_FAIL;
    const char *mechanism = NULL;
    char *sasl_client_output = NULL;
    sasl_interact_t *interactions = NULL;

    void *sasl_client_output_p = &sasl_client_output;
    do {
        rc_sasl = sasl_client_start(connection, auth_mechanism, &interactions,
                                  (const char **)(sasl_client_output_p),
                                  (unsigned int *)client_output_length, &mechanism);
        if (rc_sasl == SASL_INTERACT) {
            handle_comm(interactions,user,passwd);
        }
    } while (rc_sasl == SASL_INTERACT);

    if (rc_sasl == SASL_NOMECH) {
        DBG_RETURN( FAIL);
    }
    if (client_output != NULL) {
        *client_output = sasl_client_output;
    }
    DBG_RETURN( rc_sasl );
}
/* }}} */


/* {{{ sasl_step */
static
int sasl_step(sasl_conn_t *connection,
              const char * const user,
              const char * const passwd,
              zend_uchar *server_in,
              int server_in_length,
              zend_uchar **client_out,
              int *client_out_length
)
{
    DBG_ENTER("sasl_step");

    int rc_sasl = SASL_FAIL;
    sasl_interact_t *interactions = NULL;

    if (connection == NULL) {
        DBG_RETURN( rc_sasl );
    }
    void *client_out_p = client_out;
    do {
        if (server_in && server_in[0] == 0x0) {
            server_in_length = 0;
            server_in = NULL;
        }
        rc_sasl = sasl_client_step(
                    connection, (server_in == NULL) ? NULL : server_in,
                    (server_in == NULL) ? 0 : server_in_length, &interactions,
                    (const char **)(client_out_p),
                    (unsigned int *)client_out_length);
        if (rc_sasl == SASL_INTERACT) {
            handle_comm(interactions,user,passwd);
        } else if (rc_sasl != SASL_OK && rc_sasl != SASL_CONTINUE){
            DBG_ERR_FMT("Failed while handshaking with the SASL server: %d",
                        rc_sasl);
        }
    } while (rc_sasl == SASL_INTERACT);
    DBG_RETURN( rc_sasl );
}
/* }}} */


/* {{{ sasl_server_comm */
static
int sasl_server_comm(
        MYSQLND_CONN_DATA * conn,
        zend_uchar *request,
        int request_len,
        zend_uchar*response,
        int response_len,
        int only_resp
)
{
    DBG_ENTER("sasl_server_comm");

    if( !only_resp ){
        MYSQLND_PACKET_SASL_PK_REQUEST sasl_req;
        conn->payload_decoder_factory->m.init_sasl_pk_request_packet(&sasl_req);
        sasl_req.data = request;
        sasl_req.data_len = request_len;

        if (! PACKET_WRITE(conn, &sasl_req)) {
            DBG_ERR_FMT("Error while sending a sasl packet");
            SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
            DBG_RETURN( SASL_FAIL );
        }
    }

    MYSQLND_PACKET_SASL_PK_REQUEST_RESPONSE sasl_resp;
    conn->payload_decoder_factory->m.init_sasl_pk_request_response_packet(&sasl_resp);
    sasl_resp.data = response;
    sasl_resp.data_len = response_len;

    if (FAIL == PACKET_READ(conn, &sasl_resp) || NULL == sasl_resp.data) {
        DBG_ERR_FMT("Error while receiving a SASL response.");
        SET_CONNECTION_STATE(&conn->state, CONN_QUIT_SENT);
        DBG_RETURN( SASL_FAIL );
    }
    DBG_RETURN( sasl_resp.data_len );
}
/* }}} */


/* {{{ sasl_auth_exchange */
static
int sasl_auth_exchange(
        MYSQLND_CONN_DATA * conn,
        sasl_conn_t* connection,
        const char * const user,
        const char * const passwd,
        zend_uchar *request,
        int request_len,
        int second_step
)
{
    DBG_ENTER("sasl_auth_exchange");

    zend_uchar* server_packet = (zend_uchar*)malloc( SASL_MAX_PKT_SIZE );
    if( !server_packet ) {
        DBG_RETURN(FAIL);
    }
    int rc_sasl = SASL_FAIL;
    int pkt_len = 0;
    zend_uchar *sasl_client_output = request;
    int sasl_client_output_len = request_len;
    if( second_step ){
        memcpy(server_packet,request,request_len);
        pkt_len = request_len;
    }
    do {
      if( !second_step ) {
          pkt_len = sasl_server_comm(conn,
                                sasl_client_output,
                                sasl_client_output_len,
                                server_packet,
                                SASL_MAX_PKT_SIZE,
                                FALSE);
          if (pkt_len < 0) {
              DBG_ERR_FMT("Error while communicating with the SASL server");
              php_error(E_ERROR, "Error while communicating with the SASL server");
              rc_sasl = SASL_FAIL;
              goto cleanup;
          }
      }
      sasl_client_output = NULL;
      sasl_client_output_len = 0;
      if( pkt_len > 0) {
          rc_sasl = sasl_step(connection,
                              user,passwd,
                              server_packet,
                              pkt_len,
                              &sasl_client_output,
                              &sasl_client_output_len);
      }
      if( sasl_client_output_len == 0) {
          DBG_INF_FMT("Got empty response while handshaking with the SASL server.");
      }
      second_step = FALSE;
    } while (rc_sasl == SASL_CONTINUE);

    // ONLY FOR KRB
    if (rc_sasl == SASL_OK) {
        pkt_len = sasl_server_comm(conn,
                                   sasl_client_output,
                                   sasl_client_output_len,
                                   server_packet,
                                   SASL_MAX_PKT_SIZE,
                                   FALSE);
      }

cleanup:
    if( server_packet ) {
        free(server_packet);
    }
    DBG_RETURN( rc_sasl );
}
/* }}} */

#ifdef MYSQLND_HAVE_KRB5

#define SASL_KRB_REALMS_HEADING      "realms"
#define SASL_KRB_HOST_DEFAULT        ""
#define SASL_KRB_APPS_HEADING        "appdefaults"
#define SASL_KRB_MYSQL_APPS          "mysql"
#define SASL_KRB_LDAP_HOST_OPTION    "ldap_server_host"
#define SASL_KRB_LDAP_DESTROY_OPTION "ldap_destroy_tgt"
#define SASL_KRK_KDC_INFO            "kdc"
#define SASL_KRB_TGT_INFO            "krbtgt"
#define SASL_KRB_GENERIC_ACCCESS_DENIED "28000Access denied for user"

/* {{{ sasl_krb_configure */
static int
sasl_krb_configure(
        krb5_context    krb_ctx,
        char**          ldap_server_host,
        int*            destroy_tgt
)
{
    DBG_ENTER("sasl_krb_configure");

    krb5_error_code rc_krb = 0;
    profile_t       *profile = NULL;
    char            *host_value = NULL;
    char            *default_realm = NULL;
    int             opt_destroy_tgt = 0;

    rc_krb = krb5_get_default_realm(krb_ctx, &default_realm);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to get default realm.");
        php_error(E_ERROR, "SASL kerberos setup: failed to get default realm.");
        goto cleanup;
    }

    rc_krb = krb5_get_profile(krb_ctx, &profile);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to get the profile.");
        php_error(E_ERROR, "SASL kerberos setup: failed to get the profile.");
        goto cleanup;
    }

    rc_krb = profile_get_string(profile,
                                SASL_KRB_APPS_HEADING,
                                SASL_KRB_MYSQL_APPS,
                                SASL_KRB_LDAP_HOST_OPTION,
                                SASL_KRB_HOST_DEFAULT,
                                &host_value);
    if (rc_krb || !strcmp(host_value, "")) {
        if (host_value) {
            profile_release_string(host_value);
            host_value = NULL;
        }
        rc_krb = profile_get_string(profile,
                                    SASL_KRB_REALMS_HEADING,
                                    default_realm,
                                    SASL_KRK_KDC_INFO,
                                    SASL_KRB_HOST_DEFAULT,
                                    &host_value);
        if (rc_krb) {
            if (host_value) {
                profile_release_string(host_value);
                host_value = NULL;
            }
            DBG_ERR_FMT("SASL kerberos setup: failed to get ldap server host.");
            php_error(E_ERROR, "SASL kerberos setup: failed to get ldap server host.");
            goto cleanup;
        }
    }
    *ldap_server_host = host_value;
    rc_krb = profile_get_boolean( profile,
                                  SASL_KRB_REALMS_HEADING,
                                  default_realm,
                                  SASL_KRB_LDAP_DESTROY_OPTION,
                                  opt_destroy_tgt,
                                  (int *)&opt_destroy_tgt);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to get destroy TGT flag, default is set.");
        php_error(E_ERROR, "SASL kerberos setup: failed to get destroy TGT flag, default is set.");
    }
    else {
        *destroy_tgt = opt_destroy_tgt;
    }
cleanup:
    profile_release(profile);
    if (rc_krb && host_value) {
        profile_release_string(host_value);
        host_value = NULL;
    }
    if (default_realm) {
        krb5_free_default_realm(krb_ctx, default_realm);
        default_realm = NULL;
    }
    DBG_RETURN(rc_krb);
}
/* }}} */


/* {{{ sasl_krb_get_username */
static int
sasl_krb_get_username(
        krb5_ccache     krc_cred_cache,
        krb5_context    krb_ctx,
        char**          name
)
{
    DBG_ENTER("sasl_krb_get_username");

    krb5_error_code rc_krb = 0;
    krb5_principal  principal = NULL;
    krb5_context    context = NULL;
    char            *user_name = NULL;

    if (krc_cred_cache == NULL) {
        rc_krb = krb5_cc_default(krb_ctx, &krc_cred_cache);
        if (rc_krb) {
            DBG_ERR_FMT("SASL kerberos setup: failed to get default credentials cache.");
            php_error(E_ERROR, "SASL kerberos setup: failed to get default credentials cache.");
            goto cleanup;
        }
    }

    rc_krb = krb5_cc_get_principal(krb_ctx, krc_cred_cache, &principal);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to get principal.");
        php_error(E_ERROR, "SASL kerberos setup: failed to get principal.");
        goto cleanup;
    }
    /*
      Parsing user name from principal.
    */
    rc_krb = krb5_unparse_name(krb_ctx, principal, &user_name);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to parse principal name.");
        php_error(E_ERROR, "SASL kerberos setup: failed to parse principal name.");
        goto cleanup;
    } else {
        *name = user_name;
    }
cleanup:
    if (principal) {
        krb5_free_principal(krb_ctx, principal);
    }
    if (krc_cred_cache) {
        krb5_cc_close(krb_ctx, krc_cred_cache);
    }
    return rc_krb;
}
/* }}} */


/* {{{ sasl_krb_tgt_credentials_valid */
int static
sasl_krb_tgt_credentials_valid(
        krb5_context    krb_ctx,
        krb5_ccache     krc_cred_cache,
        const char*     username
)
{
    DBG_ENTER("sasl_krb_tgt_credentials_valid");

    krb5_error_code rc_krb = 0;
    krb5_creds      credentials;
    krb5_timestamp  krb_current_time = NULL;
    int             cred_ok = 0;
    krb5_creds      matching_credential;
    char            *realm = NULL;
    int             ret = FALSE;

    memset(&matching_credential, 0, sizeof(matching_credential));
    memset(&credentials, 0, sizeof(credentials));

    if (krc_cred_cache == NULL) {
        rc_krb = krb5_cc_default(krb_ctx, &krc_cred_cache);
        if (rc_krb) {
            DBG_ERR_FMT("SASL kerberos setup: failed to get default credentials cache.");
            php_error(E_ERROR, "SASL kerberos setup: failed to get default credentials cache.");
            goto cleanup;
        }
    }

    rc_krb = krb5_parse_name(krb_ctx,
                             username,
                             &matching_credential.client);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to parse client principal.");
        php_error(E_ERROR, "SASL kerberos setup: failed to parse client principal.");
        goto cleanup;
    }
    rc_krb = krb5_get_default_realm(krb_ctx, &realm);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to get default realm.");
        php_error(E_ERROR, "SASL kerberos setup: failed to get default realm.");
        goto cleanup;
    }
    rc_krb = krb5_build_principal(krb_ctx,
                                  &matching_credential.server,
                                  strlen(realm),
                                  realm,
                                  SASL_KRB_TGT_INFO,
                                  realm,
                                  NULL);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to build krbtgt principal.");
        php_error(E_ERROR, "SASL kerberos setup: failed to build krbtgt principal.");
        goto cleanup;
    }

    rc_krb = krb5_cc_retrieve_cred(krb_ctx,
                                   krc_cred_cache,
                                   0,
                                   &matching_credential,
                                   &credentials);
    if (rc_krb) {
        DBG_INF_FMT("SASL kerberos setup: failed to retrieve credentials.");
        //php_error(E_ERROR, "SASL kerberos setup: failed to retrieve credentials.");
        goto cleanup;
    }
    cred_ok = 1;

    rc_krb = krb5_timeofday(krb_ctx, &krb_current_time);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to retrieve current time.");
        php_error(E_ERROR, "SASL kerberos setup: failed to retrieve current time.");
        goto cleanup;
    }

    if (credentials.times.endtime < krb_current_time) {
        DBG_ERR_FMT("SASL kerberos setup:  credentials are expired.");
        php_error(E_ERROR, "SASL kerberos setup:  credentials are expired.");
        goto cleanup;
    } else {
        ret = TRUE;
    }

cleanup:
    if (realm) {
        krb5_free_default_realm(krb_ctx, realm);
    }
    if (matching_credential.server) {
        krb5_free_principal(krb_ctx, matching_credential.server);
    }
    if (matching_credential.client) {
        krb5_free_principal(krb_ctx, matching_credential.client);
    }
    if (cred_ok) {
        krb5_free_cred_contents(krb_ctx, &credentials);
    }
    if (krc_cred_cache) {
        krb5_cc_close(krb_ctx, krc_cred_cache);
    }
    return ret;

}
/* }}} */


/* {{{ sasl_krb_tgt_retrieve_credentials */
krb5_error_code
sasl_krb_tgt_retrieve_credentials(
        krb5_context    krb_ctx,
        krb5_ccache*    krc_cred_cache,
        const char*     username,
        const char*     password,
        krb5_creds*     cred_out
        )
{
    DBG_ENTER("sasl_krb_tgt_retrieve_credentials");

    krb5_error_code         rc_krb = 0;
    krb5_get_init_creds_opt *options = NULL;
    krb5_principal          principal = NULL;
    krb5_creds              credentials;
    int                     krb_cred_created = 0;
    memset(&credentials, 0, sizeof(krb5_creds));


    if( username ) {
        rc_krb = krb5_parse_name(krb_ctx, username, &principal);
    } else {
        goto cleanup;
    }
    krb5_ccache new_cred_cache = NULL;
    if (*krc_cred_cache == NULL) {
        rc_krb = krb5_cc_default(krb_ctx, &new_cred_cache);
        *krc_cred_cache = new_cred_cache;
    }
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to get default credentials cache.");
        //php_error(E_ERROR, "SASL kerberos setup: failed to get default credentials cache.");
        goto cleanup;
    }

    memset(&credentials, 0, sizeof(credentials));
    krb5_get_init_creds_opt_alloc(krb_ctx, &options);
    /*
      Getting TGT from TGT server.
    */
    rc_krb = krb5_get_init_creds_password(krb_ctx,
                                          &credentials,
                                          principal,
                                          password,
                                          NULL, NULL, 0, NULL,
                                          options);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to obtain credentials.");
        goto cleanup;
    }
    krb_cred_created = 1;

    rc_krb = krb5_verify_init_creds(krb_ctx, &credentials, NULL, NULL, NULL, NULL);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to verify credentials.");
        goto cleanup;
    }
    if (principal) {
        rc_krb =krb5_cc_initialize(krb_ctx, new_cred_cache, principal);
        if (rc_krb) {
            DBG_ERR_FMT("SASL kerberos setup: failed to initialize credentials.");
            goto cleanup;
        }
    }
    *cred_out = credentials;

cleanup:
    if (options) {
        krb5_get_init_creds_opt_free(krb_ctx, options);
        options = NULL;
    }
    if (principal) {
        krb5_free_principal(krb_ctx, principal);
        principal = NULL;
    }
    if (krb_cred_created && rc_krb) {
        krb5_free_cred_contents(krb_ctx, &credentials);
    }
    return rc_krb;
}
/* }}} */


/* {{{ sasl_krb_tgt_store_credentials */
krb5_error_code static
sasl_krb_tgt_store_credentials(
        krb5_context    krb_ctx,
        krb5_ccache     krc_cred_cache,
        krb5_creds      credentials
        )
{
    DBG_ENTER("sasl_krb_tgt_store_credentials");

    krb5_error_code rc_krb = 0;
    rc_krb = krb5_cc_store_cred(krb_ctx, krc_cred_cache, &credentials);
    if (rc_krb) {
        DBG_ERR_FMT("SASL kerberos setup: failed to store credentials.");
        php_error(E_ERROR, "SASL kerberos setup: failed to store credentials.");
    }
    return rc_krb;
}
/* }}} */


/* {{{ sasl_krb_get_credentials */
int static
sasl_krb_get_credentials(
        krb5_context    krb_ctx,
        krb5_ccache     krc_cred_cache,
        const char*     username,
        const char*     password,
        int             destroy_tgt
        )
{
    DBG_ENTER("sasl_krb_get_credentials");
    krb5_creds krb_cred;
    int        success = 0;
    memset(&krb_cred,0,sizeof(krb5_creds));
    if ( sasl_krb_tgt_credentials_valid(krb_ctx,krc_cred_cache,username) == TRUE ) {
        DBG_ERR_FMT("SASL kerberos setup: Valid TGT exists.");
        //php_error(E_ERROR, "SASL kerberos setup: Valid TGT exists.");
        return TRUE;
    }

    if (sasl_krb_tgt_retrieve_credentials(krb_ctx,
                                          &krc_cred_cache,
                                          username,
                                          password,
                                          &krb_cred)) {
        DBG_ERR_FMT("SASL kerberos setup: failed to obtain TGT/credentials.");
        //php_error(E_ERROR, "SASL kerberos setup: failed to obtain TGT/credentials.");
        goto cleanup;
    }

    if (sasl_krb_tgt_store_credentials(krb_ctx,
                                       krc_cred_cache,
                                       krb_cred)) {
        goto cleanup;
    }
    success = 1;
cleanup:
    if ( !destroy_tgt ) {
        krb5_free_cred_contents(krb_ctx, &krb_cred);
        if (krc_cred_cache) {
            DBG_ERR_FMT("SASL kerberos setup: Storing credentials into cache, closing krb5 cc.");
            //php_error(E_ERROR, "SASL kerberos setup: Storing credentials into cache, closing krb5 cc.");
            krb5_cc_close(krb_ctx, krc_cred_cache);
        }
    }
    return success;
}
/* }}} */

#endif

/* {{{ mysqlnd_ldap_sasl_get_auth_data */
static zend_uchar *
mysqlnd_ldap_sasl__get_auth_data(
        struct st_mysqlnd_authentication_plugin * self,
        size_t * auth_data_len,
        MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
        const size_t passwd_len, zend_uchar * auth_plugin_data, const size_t auth_plugin_data_len,
        const MYSQLND_SESSION_OPTIONS * const session_options,
        const MYSQLND_PFC_DATA * const pfc_data,
        const zend_ulong mysql_flags
)
{
    DBG_ENTER("mysqlnd_ldap_sasl_get_auth_data");

    int         rc_sasl = SASL_FAIL;
    sasl_conn_t *connection;
    char        *sasl_client_output = NULL;
    int         sasl_client_output_len = 0;
    char        *ldap_server_host = NULL;

#ifdef MYSQLND_HAVE_KRB5
    short          using_krb = !strcmp(auth_plugin_data, SASL_GSSAPI);
    krb5_error_code rc_krb = 0;
    krb5_context    krb_ctx = NULL;
    krb5_ccache     krc_cred_cache = NULL;
    char*           krb_username = NULL;
    int             destroy_tgt = 0;

    if( using_krb ) {
        rc_krb = krb5_init_context(&krb_ctx);
        if(rc_krb) {
            DBG_ERR_FMT("SASL kerberos setup: failed to initialize context.");
            php_error(E_ERROR, "SASL kerberos setup: failed to initialize context.");
            DBG_RETURN( NULL );
        }
        rc_krb = sasl_krb_configure( krb_ctx,
                                     &ldap_server_host,
                                     &destroy_tgt);
        if(rc_krb) {
            DBG_ERR_FMT("SASL kerberos setup: failed to configure kerberos.");
            php_error(E_ERROR, "SASL kerberos setup: failed to configure kerberos.");
            DBG_RETURN( NULL );
        }
        if(!user) {
            if(!sasl_krb_get_username(krc_cred_cache,krb_ctx,&krb_username) ){
                DBG_ERR_FMT("Failed to obtain the Kerberos username");
                DBG_RETURN( NULL );
            }
            DBG_INF_FMT("Username provided by Kerberos TGT: %s",
                        krb_username);
        }

        if(!sasl_krb_get_credentials(krb_ctx,krc_cred_cache,
                                     user,passwd,destroy_tgt)) {
            DBG_ERR_FMT("Failed to store the credentials.");
            DBG_RETURN( NULL );
        }
    }
    else
#endif
    if (strcmp(auth_plugin_data, SASL_SCRAM_SHA1) &&
            strcmp(auth_plugin_data, SASL_SCRAM_SHA256)) {
        DBG_ERR_FMT("Not supported SASL method: %s", auth_plugin_data);
        php_error(E_ERROR, "Not supported SASL method: %s, "
                  "please make sure correct method is set in "
                  "LDAP SASL server side plug-in", auth_plugin_data);
        DBG_RETURN( NULL );
    }

    rc_sasl = sasl_client_init(NULL);
    if (rc_sasl == SASL_OK) {
        rc_sasl = sasl_client_new(SASL_SERVICE_NAME, ldap_server_host, NULL, NULL, sasl_op_callbacks, 0,
                                  &connection);
    }
    if (rc_sasl != SASL_OK) {
        DBG_ERR_FMT("Error while configuring the SASL client: %d", rc_sasl);
        php_error(E_ERROR, "Error while configuring the SASL client: %d", rc_sasl);
        DBG_RETURN( NULL );
    }

    conn->sasl_connection = connection;
    sasl_setprop(connection, SASL_SEC_PROPS, &security_properties);
    rc_sasl = sasl_run(connection,conn->authentication_plugin_data.s,
                       user,passwd,
                       &sasl_client_output, &sasl_client_output_len);
    if ((rc_sasl != SASL_OK) && (rc_sasl != SASL_CONTINUE)) {
        DBG_ERR_FMT("Error while starting up the SASL authentication: %d", rc_sasl);
        php_error(E_ERROR, "Error while starting up the SASL authentication: %d", rc_sasl);
        goto cleanup;
    }

    zend_uchar* data = (zend_uchar*)malloc(sasl_client_output_len);
    memcpy(data,sasl_client_output,sasl_client_output_len);
    *auth_data_len = sasl_client_output_len;
    return data;
cleanup:
    if( connection ) {
        sasl_dispose(&connection);
    }
    DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_ldap_sasl_get_auth_data */
enum_func_status mysqlnd_ldap_sasl__handle_server_response(
        struct st_mysqlnd_authentication_plugin * self,
        MYSQLND_CONN_DATA * conn,
        const zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
        const char * const user,
        const char * const passwd,
        const size_t passwd_len,
        char **new_auth_protocol, size_t *new_auth_protocol_len,
        zend_uchar **new_auth_protocol_data, size_t *new_auth_protocol_data_len
        )
{
    DBG_ENTER("mysqlnd_ldap_sasl__handle_server_response");
    char* server_packet = (char*)malloc(SASL_MAX_PKT_SIZE);
    if( !server_packet ) {
        DBG_RETURN(FAIL);
    }
    int rc_sasl = SASL_FAIL;
    int pkt_size = sasl_server_comm(conn,
                auth_plugin_data,
                auth_plugin_data_len,
                server_packet,
                SASL_MAX_PKT_SIZE,
                FALSE);

    if( server_packet ){
        if( pkt_size > strlen( SASL_KRB_GENERIC_ACCCESS_DENIED ) ){
            if(NULL != strstr( server_packet, SASL_KRB_GENERIC_ACCCESS_DENIED ) ){
                DBG_INF("Access has been denied for the provided credentials.");
                goto EXIT;
            }
        }
    }

    if( pkt_size <= 0 ){
        DBG_ERR_FMT("Received invalid packet response from server.");
        return FAIL;
    }
    if( conn->sasl_connection ) {
        rc_sasl = sasl_auth_exchange(conn,
                                     conn->sasl_connection,
                                     user,
                                     passwd,
                                     server_packet,
                                     pkt_size,
                                     TRUE);
        sasl_dispose(&conn->sasl_connection);
    }

    if( server_packet ) {
        free(server_packet);
    }
EXIT:
    DBG_RETURN( rc_sasl == SASL_OK ? PASS : FAIL );
}
/* }}} */


static struct st_mysqlnd_authentication_plugin mysqlnd_ldap_sasl_auth_plugin =
{
    {
        MYSQLND_PLUGIN_API_VERSION,
        "auth_plugin_authentication_ldap_sasl_client",
        MYSQLND_VERSION_ID,
        PHP_MYSQLND_VERSION,
        "PHP License 3.01",
        "Filip Janiszewski <fjanisze@php.net>",
        {
            NULL, /* no statistics , will be filled later if there are some */
            NULL, /* no statistics */
        },
        {
            NULL /* plugin shutdown */
        }
    },
    {/* methods */
        mysqlnd_ldap_sasl__get_auth_data,
        mysqlnd_ldap_sasl__handle_server_response
    }
};

#endif

/* {{{ mysqlnd_register_builtin_authentication_plugins */
void
mysqlnd_register_builtin_authentication_plugins(void)
{
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_native_auth_plugin);
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_pam_authentication_plugin);
#ifdef MYSQLND_HAVE_SASL
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_ldap_sasl_auth_plugin);
#endif
#ifdef MYSQLND_HAVE_SSL
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_caching_sha2_auth_plugin);
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_sha256_authentication_plugin);
#endif
}
/* }}} */
