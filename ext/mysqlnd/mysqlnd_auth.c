/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
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

/* {{{ mysqlnd_auth_handshake */
enum_func_status
mysqlnd_auth_handshake(MYSQLND_CONN_DATA * conn,
							  const char * const user,
							  const char * const passwd,
							  const size_t passwd_len,
							  const char * const db,
							  const size_t db_len,
							  const MYSQLND_OPTIONS * const options,
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
	MYSQLND_PACKET_CHANGE_AUTH_RESPONSE * change_auth_resp_packet = NULL;
	MYSQLND_PACKET_AUTH_RESPONSE * auth_resp_packet = NULL;
	MYSQLND_PACKET_AUTH * auth_packet = NULL;

	DBG_ENTER("mysqlnd_auth_handshake");

	auth_resp_packet = conn->protocol->m.get_auth_response_packet(conn->protocol, FALSE);

	if (!auth_resp_packet) {
		SET_OOM_ERROR(*conn->error_info);
		goto end;
	}

	if (use_full_blown_auth_packet != TRUE) {
		change_auth_resp_packet = conn->protocol->m.get_change_auth_response_packet(conn->protocol, FALSE);
		if (!change_auth_resp_packet) {
			SET_OOM_ERROR(*conn->error_info);
			goto end;
		}

		change_auth_resp_packet->auth_data = auth_plugin_data;
		change_auth_resp_packet->auth_data_len = auth_plugin_data_len;

		if (!PACKET_WRITE(change_auth_resp_packet, conn)) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
			SET_CLIENT_ERROR(*conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			goto end;
		}
	} else {
		auth_packet = conn->protocol->m.get_auth_packet(conn->protocol, FALSE);

		auth_packet->client_flags = mysql_flags;
		auth_packet->max_packet_size = options->max_allowed_packet;
		if (options->charset_name && (charset = mysqlnd_find_charset_name(options->charset_name))) {
			auth_packet->charset_no	= charset->nr;
		} else {
			auth_packet->charset_no	= server_charset_no;
		}

		auth_packet->send_auth_data = TRUE;
		auth_packet->user		= user;
		auth_packet->db			= db;
		auth_packet->db_len		= db_len;

		auth_packet->auth_data = auth_plugin_data;
		auth_packet->auth_data_len = auth_plugin_data_len;
		auth_packet->auth_plugin_name = auth_protocol;

		if (conn->server_capabilities & CLIENT_CONNECT_ATTRS) {
			auth_packet->connect_attr = conn->options->connect_attr;
		}

		if (!PACKET_WRITE(auth_packet, conn)) {
			goto end;
		}
	}
	if (use_full_blown_auth_packet == TRUE) {
		conn->charset = mysqlnd_find_charset_nr(auth_packet->charset_no);
	}

	if (FAIL == PACKET_READ(auth_resp_packet, conn) || auth_resp_packet->response_code >= 0xFE) {
		if (auth_resp_packet->response_code == 0xFE) {
			/* old authentication with new server  !*/
			if (!auth_resp_packet->new_auth_protocol) {
				DBG_ERR(mysqlnd_old_passwd);
				SET_CLIENT_ERROR(*conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
			} else {
				*switch_to_auth_protocol = mnd_pestrndup(auth_resp_packet->new_auth_protocol, auth_resp_packet->new_auth_protocol_len, FALSE);
				*switch_to_auth_protocol_len = auth_resp_packet->new_auth_protocol_len;
				if (auth_resp_packet->new_auth_protocol_data) {
					*switch_to_auth_protocol_data_len = auth_resp_packet->new_auth_protocol_data_len;
					*switch_to_auth_protocol_data = mnd_emalloc(*switch_to_auth_protocol_data_len);
					memcpy(*switch_to_auth_protocol_data, auth_resp_packet->new_auth_protocol_data, *switch_to_auth_protocol_data_len);
				} else {
					*switch_to_auth_protocol_data = NULL;
					*switch_to_auth_protocol_data_len = 0;
				}
			}
		} else if (auth_resp_packet->response_code == 0xFF) {
			if (auth_resp_packet->sqlstate[0]) {
				strlcpy(conn->error_info->sqlstate, auth_resp_packet->sqlstate, sizeof(conn->error_info->sqlstate));
				DBG_ERR_FMT("ERROR:%u [SQLSTATE:%s] %s", auth_resp_packet->error_no, auth_resp_packet->sqlstate, auth_resp_packet->error);
			}
			SET_CLIENT_ERROR(*conn->error_info, auth_resp_packet->error_no, UNKNOWN_SQLSTATE, auth_resp_packet->error);
		}
		goto end;
	}

	SET_NEW_MESSAGE(conn->last_message, conn->last_message_len, auth_resp_packet->message, auth_resp_packet->message_len, conn->persistent);
	ret = PASS;
end:
	PACKET_FREE(change_auth_resp_packet);
	PACKET_FREE(auth_packet);
	PACKET_FREE(auth_resp_packet);
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
	MYSQLND_PACKET_CHANGE_AUTH_RESPONSE * change_auth_resp_packet = NULL;
	MYSQLND_PACKET_CHG_USER_RESPONSE * chg_user_resp = NULL;
	MYSQLND_PACKET_AUTH * auth_packet = NULL;

	DBG_ENTER("mysqlnd_auth_change_user");

	chg_user_resp = conn->protocol->m.get_change_user_response_packet(conn->protocol, FALSE);

	if (!chg_user_resp) {
		SET_OOM_ERROR(*conn->error_info);
		goto end;
	}

	if (use_full_blown_auth_packet != TRUE) {
		change_auth_resp_packet = conn->protocol->m.get_change_auth_response_packet(conn->protocol, FALSE);
		if (!change_auth_resp_packet) {
			SET_OOM_ERROR(*conn->error_info);
			goto end;
		}

		change_auth_resp_packet->auth_data = auth_plugin_data;
		change_auth_resp_packet->auth_data_len = auth_plugin_data_len;

		if (!PACKET_WRITE(change_auth_resp_packet, conn)) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
			SET_CLIENT_ERROR(*conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			goto end;
		}
	} else {
		auth_packet = conn->protocol->m.get_auth_packet(conn->protocol, FALSE);

		if (!auth_packet) {
			SET_OOM_ERROR(*conn->error_info);
			goto end;
		}

		auth_packet->is_change_user_packet = TRUE;
		auth_packet->user		= user;
		auth_packet->db			= db;
		auth_packet->db_len		= db_len;
		auth_packet->silent		= silent;

		auth_packet->auth_data = auth_plugin_data;
		auth_packet->auth_data_len = auth_plugin_data_len;
		auth_packet->auth_plugin_name = auth_protocol;


		if (conn->m->get_server_version(conn) >= 50123) {
			auth_packet->charset_no	= conn->charset->nr;
		}

		if (!PACKET_WRITE(auth_packet, conn)) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
				SET_CLIENT_ERROR(*conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			goto end;
		}
	}

	ret = PACKET_READ(chg_user_resp, conn);
	COPY_CLIENT_ERROR(*conn->error_info, chg_user_resp->error_info);

	if (0xFE == chg_user_resp->response_code) {
		ret = FAIL;
		if (!chg_user_resp->new_auth_protocol) {
			DBG_ERR(mysqlnd_old_passwd);
			SET_CLIENT_ERROR(*conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
		} else {
			*switch_to_auth_protocol = mnd_pestrndup(chg_user_resp->new_auth_protocol, chg_user_resp->new_auth_protocol_len, FALSE);
			*switch_to_auth_protocol_len = chg_user_resp->new_auth_protocol_len;
			if (chg_user_resp->new_auth_protocol_data) {
				*switch_to_auth_protocol_data_len = chg_user_resp->new_auth_protocol_data_len;
				*switch_to_auth_protocol_data = mnd_emalloc(*switch_to_auth_protocol_data_len);
				memcpy(*switch_to_auth_protocol_data, chg_user_resp->new_auth_protocol_data, *switch_to_auth_protocol_data_len);
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
			MYSQLND_PACKET_OK * redundant_error_packet = conn->protocol->m.get_ok_packet(conn->protocol, FALSE);
			if (redundant_error_packet) {
				PACKET_READ(redundant_error_packet, conn);
				PACKET_FREE(redundant_error_packet);
				DBG_INF_FMT("Server is %u, buggy, sends two ERR messages", conn->m->get_server_version(conn));
			} else {
				SET_OOM_ERROR(*conn->error_info);
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
		memset(conn->upsert_status, 0, sizeof(*conn->upsert_status));
		/* set charset for old servers */
		if (conn->m->get_server_version(conn) < 50123) {
			ret = conn->m->set_charset(conn, old_cs->name);
		}
	} else if (ret == FAIL && chg_user_resp->server_asked_323_auth == TRUE) {
		/* old authentication with new server  !*/
		DBG_ERR(mysqlnd_old_passwd);
		SET_CLIENT_ERROR(*conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, mysqlnd_old_passwd);
	}
end:
	PACKET_FREE(change_auth_resp_packet);
	PACKET_FREE(auth_packet);
	PACKET_FREE(chg_user_resp);
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
void php_mysqlnd_scramble(zend_uchar * const buffer, const zend_uchar * const scramble, const zend_uchar * const password, size_t password_len)
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
								  const MYSQLND_OPTIONS * const options,
								  const MYSQLND_NET_OPTIONS * const net_options,
								  zend_ulong mysql_flags
								 )
{
	zend_uchar * ret = NULL;
	DBG_ENTER("mysqlnd_native_auth_get_auth_data");
	*auth_data_len = 0;

	/* 5.5.x reports 21 as scramble length because it needs to show the length of the data before the plugin name */
	if (auth_plugin_data_len < SCRAMBLE_LENGTH) {
		/* mysql_native_password only works with SCRAMBLE_LENGTH scramble */
		SET_CLIENT_ERROR(*conn->error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "The server sent wrong length for scramble");
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


/******************************************* PAM Authentication ***********************************/

/* {{{ mysqlnd_pam_auth_get_auth_data */
static zend_uchar *
mysqlnd_pam_auth_get_auth_data(struct st_mysqlnd_authentication_plugin * self,
							   size_t * auth_data_len,
							   MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
							   const size_t passwd_len, zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
							   const MYSQLND_OPTIONS * const options,
							   const MYSQLND_NET_OPTIONS * const net_options,
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
		MYSQLND_VERSION,
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
						   const MYSQLND_OPTIONS * const options,
						   const MYSQLND_NET_OPTIONS * const net_options
						  )
{
	RSA * ret = NULL;
	const char * fname = (net_options->sha256_server_public_key && net_options->sha256_server_public_key[0] != '\0')?
								net_options->sha256_server_public_key:
								MYSQLND_G(sha256_server_public_key);
	php_stream * stream;
	DBG_ENTER("mysqlnd_sha256_get_rsa_key");
	DBG_INF_FMT("options_s256_pk=[%s] MYSQLND_G(sha256_server_public_key)=[%s]",
				 net_options->sha256_server_public_key? net_options->sha256_server_public_key:"n/a",
				 MYSQLND_G(sha256_server_public_key)? MYSQLND_G(sha256_server_public_key):"n/a");
	if (!fname || fname[0] == '\0') {
		MYSQLND_PACKET_SHA256_PK_REQUEST * pk_req_packet = NULL;
		MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE * pk_resp_packet = NULL;

		do {
			DBG_INF("requesting the public key from the server");
			pk_req_packet = conn->protocol->m.get_sha256_pk_request_packet(conn->protocol, FALSE);
			if (!pk_req_packet) {
				SET_OOM_ERROR(*conn->error_info);
				break;
			}
			pk_resp_packet = conn->protocol->m.get_sha256_pk_request_response_packet(conn->protocol, FALSE);
			if (!pk_resp_packet) {
				SET_OOM_ERROR(*conn->error_info);
				PACKET_FREE(pk_req_packet);
				break;
			}

			if (! PACKET_WRITE(pk_req_packet, conn)) {
				DBG_ERR_FMT("Error while sending public key request packet");
				php_error(E_WARNING, "Error while sending public key request packet. PID=%d", getpid());
				CONN_SET_STATE(conn, CONN_QUIT_SENT);
				break;
			}
			if (FAIL == PACKET_READ(pk_resp_packet, conn) || NULL == pk_resp_packet->public_key) {
				DBG_ERR_FMT("Error while receiving public key");
				php_error(E_WARNING, "Error while receiving public key. PID=%d", getpid());
				CONN_SET_STATE(conn, CONN_QUIT_SENT);
				break;
			}
			DBG_INF_FMT("Public key(%d):\n%s", pk_resp_packet->public_key_len, pk_resp_packet->public_key);
			/* now extract the public key */
			{
				BIO * bio = BIO_new_mem_buf(pk_resp_packet->public_key, pk_resp_packet->public_key_len);
				ret = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
				BIO_free(bio);
			}
		} while (0);
		PACKET_FREE(pk_req_packet);
		PACKET_FREE(pk_resp_packet);

		DBG_INF_FMT("ret=%p", ret);
		DBG_RETURN(ret);

		SET_CLIENT_ERROR(*conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE,
			"sha256_server_public_key is not set for the connection or as mysqlnd.sha256_server_public_key");
		DBG_ERR("server_public_key is not set");
		DBG_RETURN(NULL);
	} else {
		zend_string * key_str;
		DBG_INF_FMT("Key in a file. [%s]", fname);
		stream = php_stream_open_wrapper((char *) fname, "rb", REPORT_ERRORS, NULL);

		if (stream) {
			if ((key_str = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0)) != NULL) {
				BIO * bio = BIO_new_mem_buf(key_str->val, key_str->len);
				ret = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
				BIO_free(bio);
				DBG_INF("Successfully loaded");
				DBG_INF_FMT("Public key:%*.s", key_str->len, key_str->val);
				zend_string_release(key_str);
			}
			php_stream_free(stream, PHP_STREAM_FREE_CLOSE);
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
								  const MYSQLND_OPTIONS * const options,
								  const MYSQLND_NET_OPTIONS * const net_options,
								  zend_ulong mysql_flags
								 )
{
	RSA * server_public_key;
	zend_uchar * ret = NULL;
	DBG_ENTER("mysqlnd_sha256_auth_get_auth_data");
	DBG_INF_FMT("salt(%d)=[%.*s]", auth_plugin_data_len, auth_plugin_data_len, auth_plugin_data);


	if (conn->net->data->ssl) {
		DBG_INF("simple clear text under SSL");
		/* clear text under SSL */
		*auth_data_len = passwd_len;
		ret = malloc(passwd_len);
		memcpy(ret, passwd, passwd_len);
	} else {
		*auth_data_len = 0;
		server_public_key = mysqlnd_sha256_get_rsa_key(conn, options, net_options);

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
				SET_CLIENT_ERROR(*conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, "password is too long");
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
		MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@mysql.com>,  Ulf Wendel <uwendel@mysql.com>",
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
