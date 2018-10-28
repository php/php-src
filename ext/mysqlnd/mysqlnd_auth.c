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
			MYSQLND_CONN_DATA * conn,
			const char * const user,
			const char * const passwd,
			const size_t passwd_len,
			const char * const db,
			const size_t db_len,
			const MYSQLND_STRING auth_plugin_data,
			const char * const auth_protocol,
			unsigned int charset_no,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			zend_ulong mysql_flags,
			zend_bool silent,
			zend_bool is_change_user
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
				scrambled_data =
					auth_plugin->methods.get_auth_data(NULL, &scrambled_data_len, conn, user, passwd, passwd_len,
													   plugin_data, plugin_data_len, session_options,
													   conn->protocol_frame_codec->data, mysql_flags);
			}

			if (conn->error_info->error_no) {
				goto end;
			}
			if (FALSE == is_change_user) {
				ret = mysqlnd_auth_handshake(conn, user, passwd, passwd_len, db, db_len, session_options, mysql_flags,
											charset_no,
											first_call,
											requested_protocol,
											scrambled_data, scrambled_data_len,
											&switch_to_auth_protocol, &switch_to_auth_protocol_len,
											&switch_to_auth_protocol_data, &switch_to_auth_protocol_data_len
											);
			} else {
				ret = mysqlnd_auth_change_user(conn, user, strlen(user), passwd, passwd_len, db, db_len, silent,
											   first_call,
											   requested_protocol,
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
mysqlnd_switch_to_ssl_if_needed(MYSQLND_CONN_DATA * conn,
								unsigned int charset_no,
								size_t server_capabilities,
								const MYSQLND_SESSION_OPTIONS * const session_options,
								zend_ulong mysql_flags)
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * charset;
	DBG_ENTER("mysqlnd_switch_to_ssl_if_needed");

	if (session_options->charset_name && (charset = mysqlnd_find_charset_name(session_options->charset_name))) {
		charset_no	= charset->nr;
	}

	{
		size_t client_capabilities = mysql_flags;
		ret = conn->run_command(COM_ENABLE_SSL, conn, client_capabilities, server_capabilities, charset_no);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_connect_run_authentication */
enum_func_status
mysqlnd_connect_run_authentication(
			MYSQLND_CONN_DATA * conn,
			const char * const user,
			const char * const passwd,
			const char * const db,
			size_t db_len,
			size_t passwd_len,
			MYSQLND_STRING authentication_plugin_data,
			const char * const authentication_protocol,
			const unsigned int charset_no,
			size_t server_capabilities,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			zend_ulong mysql_flags
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
							  zend_ulong mysql_flags,
							  unsigned int server_charset_no,
							  zend_bool use_full_blown_auth_packet,
							  const char * const auth_protocol,
							  const zend_uchar * const auth_plugin_data,
							  const size_t auth_plugin_data_len,
							  char ** switch_to_auth_protocol,
							  size_t * switch_to_auth_protocol_len,
							  zend_uchar ** switch_to_auth_protocol_data,
							  size_t * switch_to_auth_protocol_data_len
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
								zend_bool use_full_blown_auth_packet,
								const char * const auth_protocol,
								zend_uchar * auth_plugin_data,
								size_t auth_plugin_data_len,
								char ** switch_to_auth_protocol,
								size_t * switch_to_auth_protocol_len,
								zend_uchar ** switch_to_auth_protocol_data,
								size_t * switch_to_auth_protocol_data_len
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
			mnd_pefree(conn->last_message.s, conn->persistent);
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
								  const size_t passwd_len, zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
								  const MYSQLND_SESSION_OPTIONS * const session_options,
								  const MYSQLND_PFC_DATA * const pfc_data,
								  zend_ulong mysql_flags
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
		mysqlnd_native_auth_get_auth_data
	}
};


/******************************************* PAM Authentication ***********************************/

/* {{{ mysqlnd_pam_auth_get_auth_data */
static zend_uchar *
mysqlnd_pam_auth_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
							   size_t * auth_data_len,
							   MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
							   const size_t passwd_len, zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
							   const MYSQLND_SESSION_OPTIONS * const session_options,
							   const MYSQLND_PFC_DATA * const pfc_data,
							   zend_ulong mysql_flags
							  )
{
	zend_uchar * ret = NULL;

	/* copy pass*/
	if (passwd && passwd_len) {
		ret = (zend_uchar*) zend_strndup(passwd, passwd_len);
	}
	*auth_data_len = passwd_len;

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
		mysqlnd_pam_auth_get_auth_data
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


#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>


/* {{{ mysqlnd_sha256_get_rsa_key */
static RSA *
mysqlnd_sha256_get_rsa_key(MYSQLND_CONN_DATA * conn,
						   const MYSQLND_SESSION_OPTIONS * const session_options,
						   const MYSQLND_PFC_DATA * const pfc_data
						  )
{
	RSA * ret = NULL;
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
			{
				BIO * bio = BIO_new_mem_buf(pk_resp_packet.public_key, pk_resp_packet.public_key_len);
				ret = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
				BIO_free(bio);
			}
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
				BIO * bio = BIO_new_mem_buf(ZSTR_VAL(key_str), ZSTR_LEN(key_str));
				ret = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
				BIO_free(bio);
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
								  const size_t passwd_len, zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
								  const MYSQLND_SESSION_OPTIONS * const session_options,
								  const MYSQLND_PFC_DATA * const pfc_data,
								  zend_ulong mysql_flags
								 )
{
	RSA * server_public_key;
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
			int server_public_key_len;
			char xor_str[passwd_len + 1];
			memcpy(xor_str, passwd, passwd_len);
			xor_str[passwd_len] = '\0';
			mysqlnd_xor_string(xor_str, passwd_len, (char *) auth_plugin_data, auth_plugin_data_len);

			server_public_key_len = RSA_size(server_public_key);
			/*
			  Because RSA_PKCS1_OAEP_PADDING is used there is a restriction on the passwd_len.
			  RSA_PKCS1_OAEP_PADDING is recommended for new applications. See more here:
			  http://www.openssl.org/docs/crypto/RSA_public_encrypt.html
			*/
			if ((size_t) server_public_key_len - 41 <= passwd_len) {
				/* password message is to long */
				SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
				DBG_ERR("password is too long");
				DBG_RETURN(NULL);
			}

			*auth_data_len = server_public_key_len;
			ret = malloc(*auth_data_len);
			RSA_public_encrypt(passwd_len + 1, (zend_uchar *) xor_str, ret, server_public_key, RSA_PKCS1_OAEP_PADDING);
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
		mysqlnd_sha256_auth_get_auth_data
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
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_sha256_authentication_plugin);
#endif
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
