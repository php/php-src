/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2009 The PHP Group                                |
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
#include "php.h"
#include "php_globals.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_block_alloc.h"
#include "ext/standard/sha1.h"
#include "php_network.h"
#include "zend_ini.h"

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
	TSRMLS_FETCH();

	DBG_ENTER("mysqlnd_set_sock_no_delay");

	if (result == -1) {
		ret = FAILURE;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_net::read_from_stream */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, read_from_stream)(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC)
{
	size_t to_read = count, ret;
	size_t old_chunk_size = conn->net->stream->chunk_size;
	DBG_ENTER("mysqlnd_net::read_from_stream");
	DBG_INF_FMT("count=%u", count);
	conn->net->stream->chunk_size = MIN(to_read, conn->net->options.net_read_buffer_size);
	while (to_read) {
		if (!(ret = php_stream_read(conn->net->stream, (char *) buffer, to_read))) {
			DBG_ERR_FMT("Error while reading header from socket");
			DBG_RETURN(FAIL);
		}
		buffer += ret;
		to_read -= ret;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(&conn->stats, STAT_BYTES_RECEIVED, count);
	conn->net->stream->chunk_size = old_chunk_size;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::stream_write */
static size_t
MYSQLND_METHOD(mysqlnd_net, stream_write)(MYSQLND * const conn, const zend_uchar * const buf, size_t count TSRMLS_DC)
{
	size_t ret;
	DBG_ENTER("mysqlnd_net::stream_write");
	ret = php_stream_write(conn->net->stream, (char *)buf, count);
	DBG_RETURN(ret);
}
/* }}} */




/* {{{ mysqlnd_net::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, connect)(MYSQLND_NET * net, const char * const scheme, size_t scheme_len, zend_bool persistent, char **errstr, int * errcode TSRMLS_DC)
{
	unsigned int streams_options = ENFORCE_SAFE_MODE;
	unsigned int streams_flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
	char * hashed_details = NULL;
	int hashed_details_len = 0;
	struct timeval tv;
	DBG_ENTER("mysqlnd_net::connect");

	if (persistent) {
		hashed_details_len = spprintf(&hashed_details, 0, "%p", net);
		DBG_INF_FMT("hashed_details=%s", hashed_details);
	}

	net->packet_no = net->compressed_envelope_packet_no = 0;

	if (net->options.timeout_connect) {
		tv.tv_sec = net->options.timeout_connect;
		tv.tv_usec = 0;
	}

	net->stream = php_stream_xport_create(scheme, scheme_len, streams_options, streams_flags,
										  hashed_details, (net->options.timeout_connect) ? &tv : NULL,
										  NULL /*ctx*/, errstr, errcode);


	if (*errstr || !net->stream) {
		if (hashed_details) {
			efree(hashed_details);
		}
		*errcode = CR_CONNECTION_ERROR;
		DBG_RETURN(FAIL);
	}

	if (hashed_details) {
		/*
		  If persistent, the streams register it in EG(persistent_list).
		  This is unwanted. ext/mysql or ext/mysqli are responsible to clean,
		  whatever they have to.
		*/
		zend_rsrc_list_entry *le;

		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_len + 1,
						   (void*) &le) == SUCCESS) {
			/*
			  in_free will let streams code skip destructing - big HACK,
			  but STREAMS suck big time regarding persistent streams.
			  Just not compatible for extensions that need persistency.
			*/
			net->stream->in_free = 1;
			zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_len + 1);
			net->stream->in_free = 0;
		}
#if ZEND_DEBUG
		/* Shut-up the streams, they don't know what they are doing */
		net->stream->__exposed = 1;
#endif
		efree(hashed_details);
	}

	if (!net->options.timeout_read) {
		/* should always happen because read_timeout cannot be set via API */
		net->options.timeout_read = (unsigned int) MYSQLND_G(net_read_timeout);
	}
	if (net->options.timeout_read)
	{
		tv.tv_sec = net->options.timeout_read;
		tv.tv_usec = 0;
		php_stream_set_option(net->stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
	}

	if (!memcmp(scheme, "tcp://", sizeof("tcp://") - 1)) {
		/* TCP -> Set TCP_NODELAY */
		mysqlnd_set_sock_no_delay(net->stream);
	}

	{
		unsigned int buf_size = MYSQLND_G(net_read_buffer_size); /* this is long, cast to unsigned int*/
		net->m.set_client_option(net, MYSQLND_OPT_NET_READ_BUFFER_SIZE, (char *)&buf_size TSRMLS_CC);
	}


	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, set_client_option)(MYSQLND_NET * const net, enum mysqlnd_option option, const char * const value TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_net::set_client_option");
	DBG_INF_FMT("option=%d", option);
	switch (option) {
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
			DBG_INF("MYSQLND_OPT_NET_CMD_BUFFER_SIZE");
			if (*(unsigned int*) value < MYSQLND_NET_CMD_BUFFER_MIN_SIZE) {
				DBG_RETURN(FAIL);
			}
			net->cmd_buffer.length = *(unsigned int*) value;
			DBG_INF_FMT("new_length=%u", net->cmd_buffer.length);
			if (!net->cmd_buffer.buffer) {
				net->cmd_buffer.buffer = mnd_pemalloc(net->cmd_buffer.length, net->persistent);
			} else {
				net->cmd_buffer.buffer = mnd_perealloc(net->cmd_buffer.buffer, net->cmd_buffer.length, net->persistent);
			}
			break;
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
			DBG_INF("MYSQLND_OPT_NET_READ_BUFFER_SIZE");
			net->options.net_read_buffer_size = *(unsigned int*) value;
			DBG_INF_FMT("new_length=%u", net->options.net_read_buffer_size);
			break;
		case MYSQL_OPT_CONNECT_TIMEOUT:
			DBG_INF("MYSQL_OPT_CONNECT_TIMEOUT");
			net->options.timeout_connect = *(unsigned int*) value;
			break;
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_READ_TIMEOUT:
			DBG_INF("MYSQL_OPT_READ_TIMEOUT");
			net->options.timeout_read = *(unsigned int*) value;
			break;
		case MYSQL_OPT_WRITE_TIMEOUT:
			DBG_INF("MYSQL_OPT_WRITE_TIMEOUT");
			net->options.timeout_write = *(unsigned int*) value;
			break;
#endif
#ifdef WHEN_SUPPORTED_BY_MYSQLI
		case MYSQL_OPT_COMPRESS:
#endif
			/* currently not supported. Todo!! */
			break;
		default:
			DBG_RETURN(FAIL);
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::set_client_option */
static void
MYSQLND_METHOD(mysqlnd_net, free_contents)(MYSQLND_NET * net TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_net::free_contents");

#ifdef MYSQLND_COMPRESSION_ENABLED
	if (net->uncompressed_data) {
		net->uncompressed_data->free_buffer(&net->uncompressed_data TSRMLS_CC);
	}
#endif
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_net_init */
MYSQLND_NET *
mysqlnd_net_init(zend_bool persistent TSRMLS_DC)
{
	MYSQLND_NET * net = mnd_pecalloc(1, sizeof(MYSQLND_NET), persistent);

	DBG_ENTER("mysqlnd_net_init");
	DBG_INF_FMT("persistent=%d", persistent);
	net->persistent = persistent;

	net->m.connect = MYSQLND_METHOD(mysqlnd_net, connect);
	net->m.stream_read = MYSQLND_METHOD(mysqlnd_net, read_from_stream);
	net->m.stream_write = MYSQLND_METHOD(mysqlnd_net, stream_write);
	net->m.set_client_option = MYSQLND_METHOD(mysqlnd_net, set_client_option);
	net->m.free_contents = MYSQLND_METHOD(mysqlnd_net, free_contents);

	{
		unsigned int buf_size = MYSQLND_G(net_read_buffer_size); /* this is long, cast to unsigned int*/
		net->m.set_client_option(net, MYSQLND_OPT_NET_CMD_BUFFER_SIZE, (char *) &buf_size TSRMLS_CC);
	}
	DBG_RETURN(net);
}
/* }}} */


/* {{{ mysqlnd_net_init */
void
mysqlnd_net_free(MYSQLND_NET * net TSRMLS_DC)
{
	zend_bool pers = net->persistent;

	DBG_ENTER("mysqlnd_net_free");

	if (net) {
		if (net->cmd_buffer.buffer) {
			DBG_INF("Freeing cmd buffer");
			mnd_pefree(net->cmd_buffer.buffer, pers);
			net->cmd_buffer.buffer = NULL;
		}

		if (net->stream) {
			DBG_INF_FMT("Freeing stream. abstract=%p", net->stream->abstract);
			if (pers) {
				php_stream_free(net->stream, PHP_STREAM_FREE_CLOSE_PERSISTENT | PHP_STREAM_FREE_RSRC_DTOR);
			} else {
				php_stream_free(net->stream, PHP_STREAM_FREE_CLOSE);	
			}
			net->stream = NULL;
		}
		mnd_pefree(net, pers);
	}
	DBG_VOID_RETURN;
}
/* }}} */

