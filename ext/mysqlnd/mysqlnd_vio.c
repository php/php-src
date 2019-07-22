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
#include "mysqlnd_priv.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_ext_plugin.h"
#include "php_network.h"

#ifndef PHP_WIN32
#include <netinet/tcp.h>
#else
#include <winsock.h>
#endif


/* {{{ mysqlnd_set_sock_no_delay */
static int
mysqlnd_set_sock_no_delay(php_stream * stream)
{
	int socketd = ((php_netstream_data_t*)stream->abstract)->socket;
	int ret = SUCCESS;
	int flag = 1;
	int result = setsockopt(socketd, IPPROTO_TCP,  TCP_NODELAY, (char *) &flag, sizeof(int));

	DBG_ENTER("mysqlnd_set_sock_no_delay");

	if (result == -1) {
		ret = FAILURE;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_set_sock_keepalive */
static int
mysqlnd_set_sock_keepalive(php_stream * stream)
{
	int socketd = ((php_netstream_data_t*)stream->abstract)->socket;
	int ret = SUCCESS;
	int flag = 1;
	int result = setsockopt(socketd, SOL_SOCKET, SO_KEEPALIVE, (char *) &flag, sizeof(int));

	DBG_ENTER("mysqlnd_set_sock_keepalive");

	if (result == -1) {
		ret = FAILURE;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_vio::network_read */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, network_read)(MYSQLND_VIO * const vio, zend_uchar * const buffer, const size_t count,
										  MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	enum_func_status return_value = PASS;
	php_stream * net_stream = vio->data->m.get_stream(vio);
	size_t to_read = count;
	zend_uchar * p = buffer;

	DBG_ENTER("mysqlnd_vio::network_read");
	DBG_INF_FMT("count="MYSQLND_SZ_T_SPEC, count);

	while (to_read) {
		ssize_t ret = php_stream_read(net_stream, (char *) p, to_read);
		if (ret <= 0) {
			DBG_ERR_FMT("Error while reading header from socket");
			return_value = FAIL;
			break;
		}
		p += ret;
		to_read -= ret;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(stats, STAT_BYTES_RECEIVED, count - to_read);
	DBG_RETURN(return_value);
}
/* }}} */


/* {{{ mysqlnd_vio::network_write */
static ssize_t
MYSQLND_METHOD(mysqlnd_vio, network_write)(MYSQLND_VIO * const vio, const zend_uchar * const buffer, const size_t count,
										   MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	ssize_t ret;
	DBG_ENTER("mysqlnd_vio::network_write");
	DBG_INF_FMT("sending %u bytes", count);
	ret = php_stream_write(vio->data->m.get_stream(vio), (char *)buffer, count);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_vio::open_pipe */
static php_stream *
MYSQLND_METHOD(mysqlnd_vio, open_pipe)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, const zend_bool persistent,
									   MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	unsigned int streams_options = 0;
	dtor_func_t origin_dtor;
	php_stream * net_stream = NULL;

	DBG_ENTER("mysqlnd_vio::open_pipe");
	if (persistent) {
		streams_options |= STREAM_OPEN_PERSISTENT;
	}
	streams_options |= IGNORE_URL;
	net_stream = php_stream_open_wrapper(scheme.s + sizeof("pipe://") - 1, "r+", streams_options, NULL);
	if (!net_stream) {
		SET_CLIENT_ERROR(error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, "Unknown errror while connecting");
		DBG_RETURN(NULL);
	}
	/*
	  Streams are not meant for C extensions! Thus we need a hack. Every connected stream will
	  be registered as resource (in EG(regular_list). So far, so good. However, it won't be
	  unregistered until the script ends. So, we need to take care of that.
	*/
	origin_dtor = EG(regular_list).pDestructor;
	EG(regular_list).pDestructor = NULL;
	zend_hash_index_del(&EG(regular_list), net_stream->res->handle); /* ToDO: should it be res->handle, do streams register with addref ?*/
	EG(regular_list).pDestructor = origin_dtor;
	net_stream->res = NULL;

	DBG_RETURN(net_stream);
}
/* }}} */


/* {{{ mysqlnd_vio::open_tcp_or_unix */
static php_stream *
MYSQLND_METHOD(mysqlnd_vio, open_tcp_or_unix)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, const zend_bool persistent,
											  MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	unsigned int streams_options = 0;
	unsigned int streams_flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
	char * hashed_details = NULL;
	int hashed_details_len = 0;
	zend_string *errstr = NULL;
	int errcode = 0;
	struct timeval tv;
	dtor_func_t origin_dtor;
	php_stream * net_stream = NULL;

	DBG_ENTER("mysqlnd_vio::open_tcp_or_unix");

	vio->data->stream = NULL;

	if (persistent) {
		hashed_details_len = mnd_sprintf(&hashed_details, 0, "%p", vio);
		DBG_INF_FMT("hashed_details=%s", hashed_details);
	}

	if (vio->data->options.timeout_connect) {
		tv.tv_sec = vio->data->options.timeout_connect;
		tv.tv_usec = 0;
	}

	DBG_INF_FMT("calling php_stream_xport_create");
	net_stream = php_stream_xport_create(scheme.s, scheme.l, streams_options, streams_flags,
										  hashed_details, (vio->data->options.timeout_connect) ? &tv : NULL,
										  NULL /*ctx*/, &errstr, &errcode);
	if (errstr || !net_stream) {
		DBG_ERR("Error");
		if (hashed_details) {
			mnd_sprintf_free(hashed_details);
		}
		errcode = CR_CONNECTION_ERROR;
		SET_CLIENT_ERROR(error_info,
						 CR_CONNECTION_ERROR,
						 UNKNOWN_SQLSTATE,
						 errstr? ZSTR_VAL(errstr):"Unknown error while connecting");
		if (errstr) {
			zend_string_release_ex(errstr, 0);
		}
		DBG_RETURN(NULL);
	}
	if (hashed_details) {
		/*
		  If persistent, the streams register it in EG(persistent_list).
		  This is unwanted. ext/mysql or ext/mysqli are responsible to clean,
		  whatever they have to.
		*/
		zend_resource *le;

		if ((le = zend_hash_str_find_ptr(&EG(persistent_list), hashed_details, hashed_details_len))) {
			origin_dtor = EG(persistent_list).pDestructor;
			/*
			  in_free will let streams code skip destructing - big HACK,
			  but STREAMS suck big time regarding persistent streams.
			  Just not compatible for extensions that need persistency.
			*/
			EG(persistent_list).pDestructor = NULL;
			zend_hash_str_del(&EG(persistent_list), hashed_details, hashed_details_len);
			EG(persistent_list).pDestructor = origin_dtor;
			pefree(le, 1);
		}
#if ZEND_DEBUG
		/* Shut-up the streams, they don't know what they are doing */
		net_stream->__exposed = 1;
#endif
		mnd_sprintf_free(hashed_details);
	}

	/*
	  Streams are not meant for C extensions! Thus we need a hack. Every connected stream will
	  be registered as resource (in EG(regular_list). So far, so good. However, it won't be
	  unregistered until the script ends. So, we need to take care of that.
	*/
	origin_dtor = EG(regular_list).pDestructor;
	EG(regular_list).pDestructor = NULL;
	zend_hash_index_del(&EG(regular_list), net_stream->res->handle); /* ToDO: should it be res->handle, do streams register with addref ?*/
	efree(net_stream->res);
	net_stream->res = NULL;
	EG(regular_list).pDestructor = origin_dtor;
	DBG_RETURN(net_stream);
}
/* }}} */


/* {{{ mysqlnd_vio::post_connect_set_opt */
static void
MYSQLND_METHOD(mysqlnd_vio, post_connect_set_opt)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme,
												  MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	php_stream * net_stream = vio->data->m.get_stream(vio);
	DBG_ENTER("mysqlnd_vio::post_connect_set_opt");
	if (net_stream) {
		if (vio->data->options.timeout_read) {
			struct timeval tv;
			DBG_INF_FMT("setting %u as PHP_STREAM_OPTION_READ_TIMEOUT", vio->data->options.timeout_read);
			tv.tv_sec = vio->data->options.timeout_read;
			tv.tv_usec = 0;
			php_stream_set_option(net_stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
		}

		if (!memcmp(scheme.s, "tcp://", sizeof("tcp://") - 1)) {
			/* TCP -> Set TCP_NODELAY */
			mysqlnd_set_sock_no_delay(net_stream);
			/* TCP -> Set SO_KEEPALIVE */
			mysqlnd_set_sock_keepalive(net_stream);
		}

		net_stream->chunk_size = vio->data->options.net_read_buffer_size;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_vio::get_open_stream */
static func_mysqlnd_vio__open_stream
MYSQLND_METHOD(mysqlnd_vio, get_open_stream)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme,
											 MYSQLND_ERROR_INFO * const error_info)
{
	func_mysqlnd_vio__open_stream ret = NULL;
	DBG_ENTER("mysqlnd_vio::get_open_stream");
	if (scheme.l > (sizeof("pipe://") - 1) && !memcmp(scheme.s, "pipe://", sizeof("pipe://") - 1)) {
		ret = vio->data->m.open_pipe;
	} else if ((scheme.l > (sizeof("tcp://") - 1) && !memcmp(scheme.s, "tcp://", sizeof("tcp://") - 1))
				||
				(scheme.l > (sizeof("unix://") - 1) && !memcmp(scheme.s, "unix://", sizeof("unix://") - 1)))
	{
		ret = vio->data->m.open_tcp_or_unix;
	}

	if (!ret) {
		SET_CLIENT_ERROR(error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, "No handler for this scheme");
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_vio::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, connect)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, const zend_bool persistent,
									 MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	enum_func_status ret = FAIL;
	func_mysqlnd_vio__open_stream open_stream = NULL;
	DBG_ENTER("mysqlnd_vio::connect");

	vio->data->m.close_stream(vio, conn_stats, error_info);

	open_stream = vio->data->m.get_open_stream(vio, scheme, error_info);
	if (open_stream) {
		php_stream * net_stream = open_stream(vio, scheme, persistent, conn_stats, error_info);
		if (net_stream && PASS == vio->data->m.set_stream(vio, net_stream)) {
			vio->data->m.post_connect_set_opt(vio, scheme, conn_stats, error_info);
			ret = PASS;
		}
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_vio::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, set_client_option)(MYSQLND_VIO * const net, enum_mysqlnd_client_option option, const char * const value)
{
	DBG_ENTER("mysqlnd_vio::set_client_option");
	DBG_INF_FMT("option=%u", option);
	switch (option) {
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
			DBG_INF("MYSQLND_OPT_NET_READ_BUFFER_SIZE");
			net->data->options.net_read_buffer_size = *(unsigned int*) value;
			DBG_INF_FMT("new_length="MYSQLND_SZ_T_SPEC, net->data->options.net_read_buffer_size);
			break;
		case MYSQL_OPT_CONNECT_TIMEOUT:
			DBG_INF("MYSQL_OPT_CONNECT_TIMEOUT");
			net->data->options.timeout_connect = *(unsigned int*) value;
			break;
		case MYSQLND_OPT_SSL_KEY:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.ssl_key) {
					mnd_pefree(net->data->options.ssl_key, pers);
				}
				net->data->options.ssl_key = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CERT:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.ssl_cert) {
					mnd_pefree(net->data->options.ssl_cert, pers);
				}
				net->data->options.ssl_cert = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CA:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.ssl_ca) {
					mnd_pefree(net->data->options.ssl_ca, pers);
				}
				net->data->options.ssl_ca = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CAPATH:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.ssl_capath) {
					mnd_pefree(net->data->options.ssl_capath, pers);
				}
				net->data->options.ssl_capath = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CIPHER:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.ssl_cipher) {
					mnd_pefree(net->data->options.ssl_cipher, pers);
				}
				net->data->options.ssl_cipher = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_PASSPHRASE:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.ssl_passphrase) {
					mnd_pefree(net->data->options.ssl_passphrase, pers);
				}
				net->data->options.ssl_passphrase = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
		{
			enum mysqlnd_ssl_peer val = *((enum mysqlnd_ssl_peer *)value);
			switch (val) {
				case MYSQLND_SSL_PEER_VERIFY:
					DBG_INF("MYSQLND_SSL_PEER_VERIFY");
					break;
				case MYSQLND_SSL_PEER_DONT_VERIFY:
					DBG_INF("MYSQLND_SSL_PEER_DONT_VERIFY");
					break;
				case MYSQLND_SSL_PEER_DEFAULT:
					DBG_INF("MYSQLND_SSL_PEER_DEFAULT");
					val = MYSQLND_SSL_PEER_DEFAULT;
					break;
				default:
					DBG_INF("default = MYSQLND_SSL_PEER_DEFAULT_ACTION");
					val = MYSQLND_SSL_PEER_DEFAULT;
					break;
			}
			net->data->options.ssl_verify_peer = val;
			break;
		}
		case MYSQL_OPT_READ_TIMEOUT:
			net->data->options.timeout_read = *(unsigned int*) value;
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_WRITE_TIMEOUT:
			net->data->options.timeout_write = *(unsigned int*) value;
			break;
#endif
		default:
			DBG_RETURN(FAIL);
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_vio::consume_uneaten_data */
size_t
MYSQLND_METHOD(mysqlnd_vio, consume_uneaten_data)(MYSQLND_VIO * const net, enum php_mysqlnd_server_command cmd)
{
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	/*
	  Switch to non-blocking mode and try to consume something from
	  the line, if possible, then continue. This saves us from looking for
	  the actual place where out-of-order packets have been sent.
	  If someone is completely sure that everything is fine, he can switch it
	  off.
	*/
	char tmp_buf[256];
	size_t skipped_bytes = 0;
	int opt = PHP_STREAM_OPTION_BLOCKING;
	php_stream * net_stream = net->data->get_stream(net);
	int was_blocked = net_stream->ops->set_option(net_stream, opt, 0, NULL);

	DBG_ENTER("mysqlnd_vio::consume_uneaten_data");

	if (PHP_STREAM_OPTION_RETURN_ERR != was_blocked) {
		/* Do a read of 1 byte */
		ssize_t bytes_consumed;

		do {
			bytes_consumed = php_stream_read(net_stream, tmp_buf, sizeof(tmp_buf));
			if (bytes_consumed <= 0) {
				break;
			}
			skipped_bytes += bytes_consumed;
		} while (bytes_consumed == sizeof(tmp_buf));

		if (was_blocked) {
			net_stream->ops->set_option(net_stream, opt, 1, NULL);
		}

		if (bytes_consumed) {
			DBG_ERR_FMT("Skipped %u bytes. Last command hasn't consumed all the output from the server",
						bytes_consumed, mysqlnd_command_to_text[net->last_command]);
			php_error_docref(NULL, E_WARNING, "Skipped %u bytes. Last command %s hasn't "
							 "consumed all the output from the server",
							 bytes_consumed, mysqlnd_command_to_text[net->last_command]);
		}
	}
	net->last_command = cmd;

	DBG_RETURN(skipped_bytes);
#else
	return 0;
#endif
}
/* }}} */

/*
  in libmyusql, if cert and !key then key=cert
*/
/* {{{ mysqlnd_vio::enable_ssl */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, enable_ssl)(MYSQLND_VIO * const net)
{
#ifdef MYSQLND_SSL_SUPPORTED
	php_stream_context * context = php_stream_context_alloc();
	php_stream * net_stream = net->data->m.get_stream(net);
	zend_bool any_flag = FALSE;

	DBG_ENTER("mysqlnd_vio::enable_ssl");

	if (net->data->options.ssl_key) {
		zval key_zval;
		ZVAL_STRING(&key_zval, net->data->options.ssl_key);
		php_stream_context_set_option(context, "ssl", "local_pk", &key_zval);
		zval_ptr_dtor(&key_zval);
		any_flag = TRUE;
	}
	if (net->data->options.ssl_cert) {
		zval cert_zval;
		ZVAL_STRING(&cert_zval, net->data->options.ssl_cert);
		php_stream_context_set_option(context, "ssl", "local_cert", &cert_zval);
		if (!net->data->options.ssl_key) {
			php_stream_context_set_option(context, "ssl", "local_pk", &cert_zval);
		}
		zval_ptr_dtor(&cert_zval);
		any_flag = TRUE;
	}
	if (net->data->options.ssl_ca) {
		zval cafile_zval;
		ZVAL_STRING(&cafile_zval, net->data->options.ssl_ca);
		php_stream_context_set_option(context, "ssl", "cafile", &cafile_zval);
		zval_ptr_dtor(&cafile_zval);
		any_flag = TRUE;
	}
	if (net->data->options.ssl_capath) {
		zval capath_zval;
		ZVAL_STRING(&capath_zval, net->data->options.ssl_capath);
		php_stream_context_set_option(context, "ssl", "capath", &capath_zval);
		zval_ptr_dtor(&capath_zval);
		any_flag = TRUE;
	}
	if (net->data->options.ssl_passphrase) {
		zval passphrase_zval;
		ZVAL_STRING(&passphrase_zval, net->data->options.ssl_passphrase);
		php_stream_context_set_option(context, "ssl", "passphrase", &passphrase_zval);
		zval_ptr_dtor(&passphrase_zval);
		any_flag = TRUE;
	}
	if (net->data->options.ssl_cipher) {
		zval cipher_zval;
		ZVAL_STRING(&cipher_zval, net->data->options.ssl_cipher);
		php_stream_context_set_option(context, "ssl", "ciphers", &cipher_zval);
		zval_ptr_dtor(&cipher_zval);
		any_flag = TRUE;
	}
	{
		zval verify_peer_zval;
		zend_bool verify;

		if (net->data->options.ssl_verify_peer == MYSQLND_SSL_PEER_DEFAULT) {
			net->data->options.ssl_verify_peer = any_flag? MYSQLND_SSL_PEER_DEFAULT_ACTION:MYSQLND_SSL_PEER_DONT_VERIFY;
		}

		verify = net->data->options.ssl_verify_peer == MYSQLND_SSL_PEER_VERIFY? TRUE:FALSE;

		DBG_INF_FMT("VERIFY=%d", verify);
		ZVAL_BOOL(&verify_peer_zval, verify);
		php_stream_context_set_option(context, "ssl", "verify_peer", &verify_peer_zval);
		php_stream_context_set_option(context, "ssl", "verify_peer_name", &verify_peer_zval);
		if (net->data->options.ssl_verify_peer == MYSQLND_SSL_PEER_DONT_VERIFY) {
			ZVAL_TRUE(&verify_peer_zval);
			php_stream_context_set_option(context, "ssl", "allow_self_signed", &verify_peer_zval);
		}
	}
	php_stream_context_set(net_stream, context);
	if (php_stream_xport_crypto_setup(net_stream, STREAM_CRYPTO_METHOD_TLS_CLIENT, NULL) < 0 ||
	    php_stream_xport_crypto_enable(net_stream, 1) < 0)
	{
		DBG_ERR("Cannot connect to MySQL by using SSL");
		php_error_docref(NULL, E_WARNING, "Cannot connect to MySQL by using SSL");
		DBG_RETURN(FAIL);
	}
	net->data->ssl = TRUE;
	/*
	  get rid of the context. we are persistent and if this is a real pconn used by mysql/mysqli,
	  then the context would not survive cleaning of EG(regular_list), where it is registered, as a
	  resource. What happens is that after this destruction any use of the network will mean usage
	  of the context, which means usage of already freed memory, bad. Actually we don't need this
	  context anymore after we have enabled SSL on the connection. Thus it is very simple, we remove it.
	*/
	php_stream_context_set(net_stream, NULL);

	if (net->data->options.timeout_read) {
		struct timeval tv;
		DBG_INF_FMT("setting %u as PHP_STREAM_OPTION_READ_TIMEOUT", net->data->options.timeout_read);
		tv.tv_sec = net->data->options.timeout_read;
		tv.tv_usec = 0;
		php_stream_set_option(net_stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
	}

	DBG_RETURN(PASS);
#else
	DBG_ENTER("mysqlnd_vio::enable_ssl");
	DBG_INF("MYSQLND_SSL_SUPPORTED is not defined");
	DBG_RETURN(PASS);
#endif
}
/* }}} */


/* {{{ mysqlnd_vio::disable_ssl */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, disable_ssl)(MYSQLND_VIO * const vio)
{
	DBG_ENTER("mysqlnd_vio::disable_ssl");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_vio::free_contents */
static void
MYSQLND_METHOD(mysqlnd_vio, free_contents)(MYSQLND_VIO * net)
{
	zend_bool pers = net->persistent;
	DBG_ENTER("mysqlnd_vio::free_contents");

	if (net->data->options.ssl_key) {
		mnd_pefree(net->data->options.ssl_key, pers);
		net->data->options.ssl_key = NULL;
	}
	if (net->data->options.ssl_cert) {
		mnd_pefree(net->data->options.ssl_cert, pers);
		net->data->options.ssl_cert = NULL;
	}
	if (net->data->options.ssl_ca) {
		mnd_pefree(net->data->options.ssl_ca, pers);
		net->data->options.ssl_ca = NULL;
	}
	if (net->data->options.ssl_capath) {
		mnd_pefree(net->data->options.ssl_capath, pers);
		net->data->options.ssl_capath = NULL;
	}
	if (net->data->options.ssl_cipher) {
		mnd_pefree(net->data->options.ssl_cipher, pers);
		net->data->options.ssl_cipher = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_vio::close_stream */
static void
MYSQLND_METHOD(mysqlnd_vio, close_stream)(MYSQLND_VIO * const net, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	php_stream * net_stream;
	DBG_ENTER("mysqlnd_vio::close_stream");
	if (net && (net_stream = net->data->m.get_stream(net))) {
		zend_bool pers = net->persistent;
		DBG_INF_FMT("Freeing stream. abstract=%p", net_stream->abstract);
		/* We removed the resource from the stream, so pass FREE_RSRC_DTOR now to force
		 * destruction to occur during shutdown, because it won't happen through the resource. */
		/* TODO: The EG(active) check here is dead -- check IN_SHUTDOWN? */
		if (pers && EG(active)) {
			php_stream_free(net_stream, PHP_STREAM_FREE_CLOSE_PERSISTENT | PHP_STREAM_FREE_RSRC_DTOR);
		} else {
			/*
			  otherwise we will crash because the EG(persistent_list) has been freed already,
			  before the modules are shut down
			*/
			php_stream_free(net_stream, PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_RSRC_DTOR);
		}
		net->data->m.set_stream(net, NULL);
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_vio::init */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, init)(MYSQLND_VIO * const net, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	unsigned int buf_size;
	DBG_ENTER("mysqlnd_vio::init");

	buf_size = MYSQLND_G(net_read_buffer_size); /* this is long, cast to unsigned int*/
	net->data->m.set_client_option(net, MYSQLND_OPT_NET_READ_BUFFER_SIZE, (char *)&buf_size);

	buf_size = MYSQLND_G(net_read_timeout); /* this is long, cast to unsigned int*/
	net->data->m.set_client_option(net, MYSQL_OPT_READ_TIMEOUT, (char *)&buf_size);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_vio::dtor */
static void
MYSQLND_METHOD(mysqlnd_vio, dtor)(MYSQLND_VIO * const vio, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("mysqlnd_vio::dtor");
	if (vio) {
		vio->data->m.free_contents(vio);
		vio->data->m.close_stream(vio, stats, error_info);

		mnd_pefree(vio, vio->persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_vio::get_stream */
static php_stream *
MYSQLND_METHOD(mysqlnd_vio, get_stream)(const MYSQLND_VIO * const net)
{
	DBG_ENTER("mysqlnd_vio::get_stream");
	DBG_INF_FMT("%p", net? net->data->stream:NULL);
	DBG_RETURN(net? net->data->stream:NULL);
}
/* }}} */


/* {{{ mysqlnd_vio::set_stream */
static enum_func_status
MYSQLND_METHOD(mysqlnd_vio, set_stream)(MYSQLND_VIO * const vio, php_stream * net_stream)
{
	DBG_ENTER("mysqlnd_vio::set_stream");
	if (vio) {
		vio->data->stream = net_stream;
		DBG_RETURN(PASS);
	}
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_vio::has_valid_stream */
static zend_bool
MYSQLND_METHOD(mysqlnd_vio, has_valid_stream)(const MYSQLND_VIO * const vio)
{
	DBG_ENTER("mysqlnd_vio::has_valid_stream");
	DBG_INF_FMT("%p %p", vio, vio? vio->data->stream:NULL);
	DBG_RETURN((vio && vio->data->stream)? TRUE: FALSE);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_vio)
	MYSQLND_METHOD(mysqlnd_vio, init),
	MYSQLND_METHOD(mysqlnd_vio, dtor),

	MYSQLND_METHOD(mysqlnd_vio, connect),

	MYSQLND_METHOD(mysqlnd_vio, close_stream),
	MYSQLND_METHOD(mysqlnd_vio, open_pipe),
	MYSQLND_METHOD(mysqlnd_vio, open_tcp_or_unix),

	MYSQLND_METHOD(mysqlnd_vio, get_stream),
	MYSQLND_METHOD(mysqlnd_vio, set_stream),
	MYSQLND_METHOD(mysqlnd_vio, has_valid_stream),
	MYSQLND_METHOD(mysqlnd_vio, get_open_stream),

	MYSQLND_METHOD(mysqlnd_vio, set_client_option),
	MYSQLND_METHOD(mysqlnd_vio, post_connect_set_opt),

	MYSQLND_METHOD(mysqlnd_vio, enable_ssl),
	MYSQLND_METHOD(mysqlnd_vio, disable_ssl),

	MYSQLND_METHOD(mysqlnd_vio, network_read),
	MYSQLND_METHOD(mysqlnd_vio, network_write),

	MYSQLND_METHOD(mysqlnd_vio, consume_uneaten_data),

	MYSQLND_METHOD(mysqlnd_vio, free_contents),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_vio_init */
PHPAPI MYSQLND_VIO *
mysqlnd_vio_init(zend_bool persistent, MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *object_factory, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *factory = object_factory? object_factory : &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory);
	MYSQLND_VIO * vio;
	DBG_ENTER("mysqlnd_vio_init");
	vio = factory->get_vio(persistent, stats, error_info);
	DBG_RETURN(vio);
}
/* }}} */


/* {{{ mysqlnd_vio_free */
PHPAPI void
mysqlnd_vio_free(MYSQLND_VIO * const vio, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("mysqlnd_vio_free");
	if (vio) {
		vio->data->m.dtor(vio, stats, error_info);
	}
	DBG_VOID_RETURN;
}
/* }}} */
