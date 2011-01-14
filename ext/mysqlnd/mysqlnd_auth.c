/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2011 The PHP Group                                |
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

/* $Id: mysqlnd.c 307377 2011-01-11 13:02:57Z andrey $ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_structs.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"


/* {{{ mysqlnd_native_auth_handshake */
enum_func_status
mysqlnd_auth_handshake(MYSQLND * conn,
							  const char * const user,
							  const char * const passwd,
							  const char * const db,
							  const size_t db_len,
							  const size_t passwd_len,
							  const MYSQLND_PACKET_GREET * const greet_packet,
							  const MYSQLND_OPTIONS * const options,
							  unsigned long mysql_flags,
							  struct st_mysqlnd_authentication_plugin * auth_plugin,
							  char ** switch_to_auth_protocol
							  TSRMLS_DC)
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * charset = NULL;
	MYSQLND_PACKET_OK * ok_packet = NULL;
	MYSQLND_PACKET_AUTH * auth_packet = NULL;

	DBG_ENTER("mysqlnd_native_auth_handshake");

	ok_packet = conn->protocol->m.get_ok_packet(conn->protocol, FALSE TSRMLS_CC);
	auth_packet = conn->protocol->m.get_auth_packet(conn->protocol, FALSE TSRMLS_CC);

	if (!ok_packet || !auth_packet) {
		SET_OOM_ERROR(conn->error_info);
		goto end;
	}

	auth_packet->client_flags = mysql_flags;
	auth_packet->max_packet_size = options->max_allowed_packet;
	if (options->charset_name && (charset = mysqlnd_find_charset_name(options->charset_name))) {
		auth_packet->charset_no	= charset->nr;
	} else {
#if MYSQLND_UNICODE
		auth_packet->charset_no	= 200;/* utf8 - swedish collation, check mysqlnd_charset.c */
#else
		auth_packet->charset_no	= greet_packet->charset_no;
#endif
	}

	auth_packet->send_auth_data = TRUE;
	auth_packet->user		= user;
	auth_packet->db			= db;
	auth_packet->db_len		= db_len;

	conn->auth_plugin_data_len = greet_packet->auth_plugin_data_len;
	conn->auth_plugin_data = mnd_pemalloc(conn->auth_plugin_data_len, conn->persistent);
	if (!conn->auth_plugin_data) {
		SET_OOM_ERROR(conn->error_info);
		goto end;
	}
	memcpy(conn->auth_plugin_data, greet_packet->auth_plugin_data, greet_packet->auth_plugin_data_len);

	auth_packet->auth_data =
		auth_plugin->methods.get_auth_data(NULL, &auth_packet->auth_data_len, conn, user, passwd, passwd_len,
										   conn->auth_plugin_data, conn->auth_plugin_data_len, options, mysql_flags TSRMLS_CC);
							

	if (!PACKET_WRITE(auth_packet, conn)) {
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		goto end;
	}

	if (FAIL == PACKET_READ(ok_packet, conn) || ok_packet->field_count >= 0xFE) {
		if (ok_packet->field_count == 0xFE) {
			/* old authentication with new server  !*/
			DBG_ERR(mysqlnd_old_passwd);
			SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
		} else if (ok_packet->field_count == 0xFF) {
			if (ok_packet->sqlstate[0]) {
				strlcpy(conn->error_info.sqlstate, ok_packet->sqlstate, sizeof(conn->error_info.sqlstate));
				DBG_ERR_FMT("ERROR:%u [SQLSTATE:%s] %s", ok_packet->error_no, ok_packet->sqlstate, ok_packet->error);
			}
			conn->error_info.error_no = ok_packet->error_no;
			strlcpy(conn->error_info.error, ok_packet->error, sizeof(conn->error_info.error));
		}
		goto end;
	}

	SET_NEW_MESSAGE(conn->last_message, conn->last_message_len, ok_packet->message, ok_packet->message_len, conn->persistent);
	conn->charset = mysqlnd_find_charset_nr(auth_packet->charset_no);
	ret = PASS;
end:
	mnd_efree(auth_packet->auth_data);
	PACKET_FREE(auth_packet);
	PACKET_FREE(ok_packet);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_native_auth_change_user */
enum_func_status
mysqlnd_auth_change_user(MYSQLND * const conn,
								const char * const user,
								const size_t user_len,
								const char * const passwd,
								const char * const db,
								const size_t db_len,
								const size_t passwd_len,
								const zend_bool silent,
								struct st_mysqlnd_authentication_plugin * auth_plugin,
								char ** switch_to_auth_protocol
								TSRMLS_DC)
{
	/*
	  User could be max 16 * 3 (utf8), pass is 20 usually, db is up to 64*3
	  Stack space is not that expensive, so use a bit more to be protected against
	  buffer overflows.
	*/
	enum_func_status ret = FAIL;
	char buffer[MYSQLND_MAX_ALLOWED_USER_LEN + 1 + SCRAMBLE_LENGTH + MYSQLND_MAX_ALLOWED_DB_LEN + 1 + 2 /* charset*/ ];
	char *p = buffer;
	const MYSQLND_CHARSET * old_cs = conn->charset;
	MYSQLND_PACKET_AUTH * auth_packet = conn->protocol->m.get_auth_packet(conn->protocol, FALSE TSRMLS_CC);
	MYSQLND_PACKET_CHG_USER_RESPONSE * chg_user_resp = conn->protocol->m.get_change_user_response_packet(conn->protocol, FALSE TSRMLS_CC);

	DBG_ENTER("mysqlnd_native_auth_change_user");

	if (!auth_packet || !chg_user_resp) {
		SET_OOM_ERROR(conn->error_info);
		goto end;
	}

	auth_packet->is_change_user_packet = TRUE;
	auth_packet->user		= user;
	auth_packet->db			= db;
	auth_packet->db_len		= db_len;
	auth_packet->silent		= silent;

	auth_packet->auth_data =
		auth_plugin->methods.get_auth_data(NULL, &auth_packet->auth_data_len, conn, user, passwd, passwd_len,
										   conn->auth_plugin_data, conn->auth_plugin_data_len, NULL /* options */, 0 /* mysql_flags */ TSRMLS_CC);

	if (mysqlnd_get_server_version(conn) >= 50123) {
		auth_packet->charset_no	= conn->charset->nr;
		p+=2;
	}
	
	if (!PACKET_WRITE(auth_packet, conn)) {
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		goto end;
	}

	ret = PACKET_READ(chg_user_resp, conn);
	conn->error_info = chg_user_resp->error_info;

	if (conn->error_info.error_no) {
		ret = FAIL;
		/*
		  COM_CHANGE_USER is broken in 5.1. At least in 5.1.15 and 5.1.14, 5.1.11 is immune.
		  bug#25371 mysql_change_user() triggers "packets out of sync"
		  When it gets fixed, there should be one more check here
		*/
		if (mysqlnd_get_server_version(conn) > 50113L && mysqlnd_get_server_version(conn) < 50118L) {
			MYSQLND_PACKET_OK * redundant_error_packet = conn->protocol->m.get_ok_packet(conn->protocol, FALSE TSRMLS_CC);
			if (redundant_error_packet) {
				PACKET_READ(redundant_error_packet, conn);
				PACKET_FREE(redundant_error_packet);
				DBG_INF_FMT("Server is %u, buggy, sends two ERR messages", mysqlnd_get_server_version(conn));
			} else {
				SET_OOM_ERROR(conn->error_info);
			}
		}
	}
	if (ret == PASS) {
		char * tmp = NULL;
		/* if we get conn->user as parameter and then we first free it, then estrndup it, we will crash */
		tmp = mnd_pestrndup(user, user_len, conn->persistent);
		if (conn->user) {
			mnd_pefree(conn->user, conn->persistent);
		}
		conn->user = tmp;

		tmp = mnd_pestrdup(passwd, conn->persistent);
		if (conn->passwd) {
			mnd_pefree(conn->passwd, conn->persistent);
		}
		conn->passwd = tmp;

		if (conn->last_message) {
			mnd_pefree(conn->last_message, conn->persistent);
			conn->last_message = NULL;
		}
		memset(&conn->upsert_status, 0, sizeof(conn->upsert_status));
		/* set charset for old servers */
		if (mysqlnd_get_server_version(conn) < 50123) {
			ret = conn->m->set_charset(conn, old_cs->name TSRMLS_CC);
		}
	} else if (ret == FAIL && chg_user_resp->server_asked_323_auth == TRUE) {
		/* old authentication with new server  !*/
		DBG_ERR(mysqlnd_old_passwd);
		SET_CLIENT_ERROR(conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
	}
end:
	mnd_efree(auth_packet->auth_data);
	PACKET_FREE(auth_packet);
	PACKET_FREE(chg_user_resp);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_native_auth_get_auth_data */
static zend_uchar *
mysqlnd_native_auth_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
								  size_t * auth_data_len,
								  MYSQLND * conn, const char * const user, const char * const passwd,
								  const size_t passwd_len, zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
								  const MYSQLND_OPTIONS * const options, unsigned long mysql_flags
								  TSRMLS_DC)
{
	zend_uchar * ret = NULL;
	DBG_ENTER("mysqlnd_native_auth_get_auth_data");
	*auth_data_len = 0;

	/* 5.5.x reports 21 as scramble length because it needs to show the length of the data before the plugin name */
	if (auth_plugin_data_len != SCRAMBLE_LENGTH && (auth_plugin_data_len != 21)) {
		/* mysql_native_password only works with SCRAMBLE_LENGTH scramble */
		SET_CLIENT_ERROR(conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "The server sent wrong length for scramble");
		DBG_ERR_FMT("The server sent wrong length for scramble %u. Expected %u", auth_plugin_data_len, SCRAMBLE_LENGTH);
		DBG_RETURN(NULL);
	}

	/* copy scrambled pass*/
	if (passwd && passwd_len) {
		ret = mnd_emalloc(SCRAMBLE_LENGTH);
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
		MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@mysql.com>,  Ulf Wendel <uwendel@mysql.com>, Georg Richter <georg@mysql.com>",
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


/* {{{ mysqlnd_native_authentication_plugin_register */
void
mysqlnd_native_authentication_plugin_register(TSRMLS_D)
{
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_native_auth_plugin TSRMLS_CC);
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
