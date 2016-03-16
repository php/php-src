/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
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
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_globals.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_ext_plugin.h"
#include "php_network.h"
#include "zend_ini.h"
#ifdef MYSQLND_COMPRESSION_ENABLED
#include <zlib.h>
#endif

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


/* {{{ mysqlnd_net::network_read_ex */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, network_read_ex)(MYSQLND_NET * const net, zend_uchar * const buffer, const size_t count,
											 MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	enum_func_status return_value = PASS;
	php_stream * net_stream = net->data->m.get_stream(net);
	size_t old_chunk_size = net_stream->chunk_size;
	size_t to_read = count, ret;
	zend_uchar * p = buffer;

	DBG_ENTER("mysqlnd_net::network_read_ex");
	DBG_INF_FMT("count="MYSQLND_SZ_T_SPEC, count);

	net_stream->chunk_size = MIN(to_read, net->data->options.net_read_buffer_size);
	while (to_read) {
		if (!(ret = php_stream_read(net_stream, (char *) p, to_read))) {
			DBG_ERR_FMT("Error while reading header from socket");
			return_value = FAIL;
			break;
		}
		p += ret;
		to_read -= ret;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(stats, STAT_BYTES_RECEIVED, count - to_read);
	net_stream->chunk_size = old_chunk_size;
	DBG_RETURN(return_value);
}
/* }}} */


/* {{{ mysqlnd_net::network_write_ex */
static size_t
MYSQLND_METHOD(mysqlnd_net, network_write_ex)(MYSQLND_NET * const net, const zend_uchar * const buffer, const size_t count,
											  MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	size_t ret;
	DBG_ENTER("mysqlnd_net::network_write_ex");
	DBG_INF_FMT("sending %u bytes", count);
	ret = php_stream_write(net->data->m.get_stream(net), (char *)buffer, count);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_net::open_pipe */
static php_stream *
MYSQLND_METHOD(mysqlnd_net, open_pipe)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len,
									   const zend_bool persistent,
									   MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
#if PHP_API_VERSION < 20100412
	unsigned int streams_options = ENFORCE_SAFE_MODE;
#else
	unsigned int streams_options = 0;
#endif
	dtor_func_t origin_dtor;
	php_stream * net_stream = NULL;

	DBG_ENTER("mysqlnd_net::open_pipe");
	if (persistent) {
		streams_options |= STREAM_OPEN_PERSISTENT;
	}
	streams_options |= IGNORE_URL;
	net_stream = php_stream_open_wrapper((char*) scheme + sizeof("pipe://") - 1, "r+", streams_options, NULL);
	if (!net_stream) {
		SET_CLIENT_ERROR(*error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, "Unknown errror while connecting");
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


/* {{{ mysqlnd_net::open_tcp_or_unix */
static php_stream *
MYSQLND_METHOD(mysqlnd_net, open_tcp_or_unix)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len,
											  const zend_bool persistent,
											  MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
#if PHP_API_VERSION < 20100412
	unsigned int streams_options = ENFORCE_SAFE_MODE;
#else
	unsigned int streams_options = 0;
#endif
	unsigned int streams_flags = STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT;
	char * hashed_details = NULL;
	int hashed_details_len = 0;
	zend_string *errstr = NULL;
	int errcode = 0;
	struct timeval tv;
	dtor_func_t origin_dtor;
	php_stream * net_stream = NULL;

	DBG_ENTER("mysqlnd_net::open_tcp_or_unix");

	net->data->stream = NULL;

	if (persistent) {
		hashed_details_len = mnd_sprintf(&hashed_details, 0, "%p", net);
		DBG_INF_FMT("hashed_details=%s", hashed_details);
	}

	if (net->data->options.timeout_connect) {
		tv.tv_sec = net->data->options.timeout_connect;
		tv.tv_usec = 0;
	}

	DBG_INF_FMT("calling php_stream_xport_create");
	net_stream = php_stream_xport_create(scheme, scheme_len, streams_options, streams_flags,
										  hashed_details, (net->data->options.timeout_connect) ? &tv : NULL,
										  NULL /*ctx*/, &errstr, &errcode);
	if (errstr || !net_stream) {
		DBG_ERR("Error");
		if (hashed_details) {
			mnd_sprintf_free(hashed_details);
		}
		errcode = CR_CONNECTION_ERROR;
		SET_CLIENT_ERROR(*error_info,
						 CR_CONNECTION_ERROR,
						 UNKNOWN_SQLSTATE,
						 errstr? ZSTR_VAL(errstr):"Unknown error while connecting");
		if (errstr) {
			zend_string_release(errstr);
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


/* {{{ mysqlnd_net::post_connect_set_opt */
static void
MYSQLND_METHOD(mysqlnd_net, post_connect_set_opt)(MYSQLND_NET * const net,
												  const char * const scheme, const size_t scheme_len,
												  MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	php_stream * net_stream = net->data->m.get_stream(net);
	DBG_ENTER("mysqlnd_net::post_connect_set_opt");
	if (net_stream) {
		if (net->data->options.timeout_read) {
			struct timeval tv;
			DBG_INF_FMT("setting %u as PHP_STREAM_OPTION_READ_TIMEOUT", net->data->options.timeout_read);
			tv.tv_sec = net->data->options.timeout_read;
			tv.tv_usec = 0;
			php_stream_set_option(net_stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
		}

		if (!memcmp(scheme, "tcp://", sizeof("tcp://") - 1)) {
			/* TCP -> Set TCP_NODELAY */
			mysqlnd_set_sock_no_delay(net_stream);
			/* TCP -> Set SO_KEEPALIVE */
			mysqlnd_set_sock_keepalive(net_stream);
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_net::get_open_stream */
static func_mysqlnd_net__open_stream
MYSQLND_METHOD(mysqlnd_net, get_open_stream)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len,
											 MYSQLND_ERROR_INFO * const error_info)
{
	func_mysqlnd_net__open_stream ret = NULL;
	DBG_ENTER("mysqlnd_net::get_open_stream");
	if (scheme_len > (sizeof("pipe://") - 1) && !memcmp(scheme, "pipe://", sizeof("pipe://") - 1)) {
		ret = net->data->m.open_pipe;
	} else if ((scheme_len > (sizeof("tcp://") - 1) && !memcmp(scheme, "tcp://", sizeof("tcp://") - 1))
				||
				(scheme_len > (sizeof("unix://") - 1) && !memcmp(scheme, "unix://", sizeof("unix://") - 1)))
	{
		ret = net->data->m.open_tcp_or_unix;
	}

	if (!ret) {
		SET_CLIENT_ERROR(*error_info, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE, "No handler for this scheme");
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_net::connect_ex */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, connect_ex)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len,
										const zend_bool persistent,
										MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	enum_func_status ret = FAIL;
	func_mysqlnd_net__open_stream open_stream = NULL;
	DBG_ENTER("mysqlnd_net::connect_ex");

	net->packet_no = net->compressed_envelope_packet_no = 0;

	net->data->m.close_stream(net, conn_stats, error_info);

	open_stream = net->data->m.get_open_stream(net, scheme, scheme_len, error_info);
	if (open_stream) {
		php_stream * net_stream = open_stream(net, scheme, scheme_len, persistent, conn_stats, error_info);
		if (net_stream) {
			(void) net->data->m.set_stream(net, net_stream);
			net->data->m.post_connect_set_opt(net, scheme, scheme_len, conn_stats, error_info);
			ret = PASS;
		}
	}

	DBG_RETURN(ret);
}
/* }}} */


/* We assume that MYSQLND_HEADER_SIZE is 4 bytes !! */
#define COPY_HEADER(T,A)  do { \
		*(((char *)(T)))   = *(((char *)(A)));\
		*(((char *)(T))+1) = *(((char *)(A))+1);\
		*(((char *)(T))+2) = *(((char *)(A))+2);\
		*(((char *)(T))+3) = *(((char *)(A))+3); } while (0)
#define STORE_HEADER_SIZE(safe_storage, buffer)  COPY_HEADER((safe_storage), (buffer))
#define RESTORE_HEADER_SIZE(buffer, safe_storage) STORE_HEADER_SIZE((safe_storage), (buffer))


/* {{{ mysqlnd_net::send_ex */
/*
  IMPORTANT : It's expected that buffer has place in the beginning for MYSQLND_HEADER_SIZE !!!!
			  This is done for performance reasons in the caller of this function.
			  Otherwise we will have to do send two TCP packets, or do new alloc and memcpy.
			  Neither are quick, thus the clients of this function are obligated to do
			  what they are asked for.

  `count` is actually the length of the payload data. Thus :
  count + MYSQLND_HEADER_SIZE = sizeof(buffer) (not the pointer but the actual buffer)
*/
static size_t
MYSQLND_METHOD(mysqlnd_net, send_ex)(MYSQLND_NET * const net, zend_uchar * const buffer, const size_t count,
									 MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	zend_uchar safe_buf[((MYSQLND_HEADER_SIZE) + (sizeof(zend_uchar)) - 1) / (sizeof(zend_uchar))];
	zend_uchar * safe_storage = safe_buf;
	size_t bytes_sent, packets_sent = 1;
	size_t left = count;
	zend_uchar * p = (zend_uchar *) buffer;
	zend_uchar * compress_buf = NULL;
	size_t to_be_sent;

	DBG_ENTER("mysqlnd_net::send_ex");
	DBG_INF_FMT("count=" MYSQLND_SZ_T_SPEC " compression=%u", count, net->data->compressed);

	if (net->data->compressed == TRUE) {
		size_t comp_buf_size = MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE + MIN(left, MYSQLND_MAX_PACKET_SIZE);
		DBG_INF_FMT("compress_buf_size="MYSQLND_SZ_T_SPEC, comp_buf_size);
		compress_buf = mnd_emalloc(comp_buf_size);
	}

	do {
		to_be_sent = MIN(left, MYSQLND_MAX_PACKET_SIZE);
		DBG_INF_FMT("to_be_sent=%u", to_be_sent);
		DBG_INF_FMT("packets_sent=%u", packets_sent);
		DBG_INF_FMT("compressed_envelope_packet_no=%u", net->compressed_envelope_packet_no);
		DBG_INF_FMT("packet_no=%u", net->packet_no);
#ifdef MYSQLND_COMPRESSION_ENABLED
		if (net->data->compressed == TRUE) {
			/* here we need to compress the data and then write it, first comes the compressed header */
			size_t tmp_complen = to_be_sent;
			size_t payload_size;
			zend_uchar * uncompressed_payload = p; /* should include the header */

			STORE_HEADER_SIZE(safe_storage, uncompressed_payload);
			int3store(uncompressed_payload, to_be_sent);
			int1store(uncompressed_payload + 3, net->packet_no);
			if (PASS == net->data->m.encode((compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), &tmp_complen,
									   uncompressed_payload, to_be_sent + MYSQLND_HEADER_SIZE))
			{
				int3store(compress_buf + MYSQLND_HEADER_SIZE, to_be_sent + MYSQLND_HEADER_SIZE);
				payload_size = tmp_complen;
			} else {
				int3store(compress_buf + MYSQLND_HEADER_SIZE, 0);
				memcpy(compress_buf + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, uncompressed_payload, to_be_sent + MYSQLND_HEADER_SIZE);
				payload_size = to_be_sent + MYSQLND_HEADER_SIZE;
			}
			RESTORE_HEADER_SIZE(uncompressed_payload, safe_storage);

			int3store(compress_buf, payload_size);
			int1store(compress_buf + 3, net->packet_no);
			DBG_INF_FMT("writing "MYSQLND_SZ_T_SPEC" bytes to the network", payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE);
			bytes_sent = net->data->m.network_write_ex(net, compress_buf, payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE,
												 conn_stats, error_info);
			net->compressed_envelope_packet_no++;
  #if WHEN_WE_NEED_TO_CHECK_WHETHER_COMPRESSION_WORKS_CORRECTLY
			if (res == Z_OK) {
				size_t decompressed_size = left + MYSQLND_HEADER_SIZE;
				zend_uchar * decompressed_data = mnd_malloc(decompressed_size);
				int error = net->data->m.decode(decompressed_data, decompressed_size,
										  compress_buf + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, payload_size);
				if (error == Z_OK) {
					int i;
					DBG_INF("success decompressing");
					for (i = 0 ; i < decompressed_size; i++) {
						if (i && (i % 30 == 0)) {
							printf("\n\t\t");
						}
						printf("%.2X ", (int)*((char*)&(decompressed_data[i])));
						DBG_INF_FMT("%.2X ", (int)*((char*)&(decompressed_data[i])));
					}
				} else {
					DBG_INF("error decompressing");
				}
				mnd_free(decompressed_data);
			}
  #endif /* WHEN_WE_NEED_TO_CHECK_WHETHER_COMPRESSION_WORKS_CORRECTLY */
		} else
#endif /* MYSQLND_COMPRESSION_ENABLED */
		{
			DBG_INF("no compression");
			STORE_HEADER_SIZE(safe_storage, p);
			int3store(p, to_be_sent);
			int1store(p + 3, net->packet_no);
			bytes_sent = net->data->m.network_write_ex(net, p, to_be_sent + MYSQLND_HEADER_SIZE, conn_stats, error_info);
			RESTORE_HEADER_SIZE(p, safe_storage);
			net->compressed_envelope_packet_no++;
		}
		net->packet_no++;

		p += to_be_sent;
		left -= to_be_sent;
		packets_sent++;
		/*
		  if left is 0 then there is nothing more to send, but if the last packet was exactly
		  with the size MYSQLND_MAX_PACKET_SIZE we need to send additional packet, which has
		  empty payload. Thus if left == 0 we check for to_be_sent being the max size. If it is
		  indeed it then loop once more, then to_be_sent will become 0, left will stay 0. Empty
		  packet will be sent and this loop will end.
		*/
	} while (bytes_sent && (left > 0 || to_be_sent == MYSQLND_MAX_PACKET_SIZE));

	DBG_INF_FMT("packet_size="MYSQLND_SZ_T_SPEC" packet_no=%u", left, net->packet_no);

	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats,
			STAT_BYTES_SENT, count + packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PROTOCOL_OVERHEAD_OUT, packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PACKETS_SENT, packets_sent);

	if (compress_buf) {
		mnd_efree(compress_buf);
	}

	/* Even for zero size payload we have to send a packet */
	if (!bytes_sent) {
		DBG_ERR_FMT("Can't %u send bytes", count);
		SET_CLIENT_ERROR(*error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
	}
	DBG_RETURN(bytes_sent);
}
/* }}} */


#ifdef MYSQLND_COMPRESSION_ENABLED
/* {{{ php_mysqlnd_read_buffer_is_empty */
static zend_bool
php_mysqlnd_read_buffer_is_empty(MYSQLND_READ_BUFFER * buffer)
{
	return buffer->len? FALSE:TRUE;
}
/* }}} */


/* {{{ php_mysqlnd_read_buffer_read */
static void
php_mysqlnd_read_buffer_read(MYSQLND_READ_BUFFER * buffer, size_t count, zend_uchar * dest)
{
	if (buffer->len >= count) {
		memcpy(dest, buffer->data + buffer->offset, count);
		buffer->offset += count;
		buffer->len -= count;
	}
}
/* }}} */


/* {{{ php_mysqlnd_read_buffer_bytes_left */
static size_t
php_mysqlnd_read_buffer_bytes_left(MYSQLND_READ_BUFFER * buffer)
{
	return buffer->len;
}
/* }}} */


/* {{{ php_mysqlnd_read_buffer_free */
static void
php_mysqlnd_read_buffer_free(MYSQLND_READ_BUFFER ** buffer)
{
	DBG_ENTER("php_mysqlnd_read_buffer_free");
	if (*buffer) {
		mnd_efree((*buffer)->data);
		mnd_efree(*buffer);
		*buffer = NULL;
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ php_mysqlnd_create_read_buffer */
static MYSQLND_READ_BUFFER *
mysqlnd_create_read_buffer(size_t count)
{
	MYSQLND_READ_BUFFER * ret = mnd_emalloc(sizeof(MYSQLND_READ_BUFFER));
	DBG_ENTER("mysqlnd_create_read_buffer");
	ret->is_empty = php_mysqlnd_read_buffer_is_empty;
	ret->read = php_mysqlnd_read_buffer_read;
	ret->bytes_left = php_mysqlnd_read_buffer_bytes_left;
	ret->free_buffer = php_mysqlnd_read_buffer_free;
	ret->data = mnd_emalloc(count);
	ret->size = ret->len = count;
	ret->offset = 0;
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_net::read_compressed_packet_from_stream_and_fill_read_buffer */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, read_compressed_packet_from_stream_and_fill_read_buffer)
		(MYSQLND_NET * net, size_t net_payload_size, MYSQLND_STATS * conn_stats, MYSQLND_ERROR_INFO * error_info)
{
	size_t decompressed_size;
	enum_func_status retval = PASS;
	zend_uchar * compressed_data = NULL;
	zend_uchar comp_header[COMPRESSED_HEADER_SIZE];
	DBG_ENTER("mysqlnd_net::read_compressed_packet_from_stream_and_fill_read_buffer");

	/* Read the compressed header */
	if (FAIL == net->data->m.network_read_ex(net, comp_header, COMPRESSED_HEADER_SIZE, conn_stats, error_info)) {
		DBG_RETURN(FAIL);
	}
	decompressed_size = uint3korr(comp_header);

	/* When decompressed_size is 0, then the data is not compressed, and we have wasted 3 bytes */
	/* we need to decompress the data */

	if (decompressed_size) {
		compressed_data = mnd_emalloc(net_payload_size);
		if (FAIL == net->data->m.network_read_ex(net, compressed_data, net_payload_size, conn_stats, error_info)) {
			retval = FAIL;
			goto end;
		}
		net->uncompressed_data = mysqlnd_create_read_buffer(decompressed_size);
		retval = net->data->m.decode(net->uncompressed_data->data, decompressed_size, compressed_data, net_payload_size);
		if (FAIL == retval) {
			goto end;
		}
	} else {
		DBG_INF_FMT("The server decided not to compress the data. Our job is easy. Copying %u bytes", net_payload_size);
		net->uncompressed_data = mysqlnd_create_read_buffer(net_payload_size);
		if (FAIL == net->data->m.network_read_ex(net, net->uncompressed_data->data, net_payload_size, conn_stats, error_info)) {
			retval = FAIL;
			goto end;
		}
	}
end:
	if (compressed_data) {
		mnd_efree(compressed_data);
	}
	DBG_RETURN(retval);
}
/* }}} */
#endif /* MYSQLND_COMPRESSION_ENABLED */


/* {{{ mysqlnd_net::decode */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, decode)(zend_uchar * uncompressed_data, const size_t uncompressed_data_len,
									const zend_uchar * const compressed_data, const size_t compressed_data_len)
{
#ifdef MYSQLND_COMPRESSION_ENABLED
	int error;
	uLongf tmp_complen = uncompressed_data_len;
	DBG_ENTER("mysqlnd_net::decode");
	error = uncompress(uncompressed_data, &tmp_complen, compressed_data, compressed_data_len);

	DBG_INF_FMT("compressed data: decomp_len=%lu compressed_size="MYSQLND_SZ_T_SPEC, tmp_complen, compressed_data_len);
	if (error != Z_OK) {
		DBG_INF_FMT("decompression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", error, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
	}
	DBG_RETURN(error == Z_OK? PASS:FAIL);
#else
	DBG_ENTER("mysqlnd_net::decode");
	DBG_RETURN(FAIL);
#endif
}
/* }}} */


/* {{{ mysqlnd_net::encode */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, encode)(zend_uchar * compress_buffer, size_t * compress_buffer_len,
									const zend_uchar * const uncompressed_data, const size_t uncompressed_data_len)
{
#ifdef MYSQLND_COMPRESSION_ENABLED
	int error;
	uLongf tmp_complen = *compress_buffer_len;
	DBG_ENTER("mysqlnd_net::encode");
	error = compress(compress_buffer, &tmp_complen, uncompressed_data, uncompressed_data_len);

	if (error != Z_OK) {
		DBG_INF_FMT("compression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", error, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
	} else {
		*compress_buffer_len = tmp_complen;
		DBG_INF_FMT("compression successful. compressed size=%lu", tmp_complen);
	}

	DBG_RETURN(error == Z_OK? PASS:FAIL);
#else
	DBG_ENTER("mysqlnd_net::encode");
	DBG_RETURN(FAIL);
#endif
}
/* }}} */


/* {{{ mysqlnd_net::receive_ex */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, receive_ex)(MYSQLND_NET * const net, zend_uchar * const buffer, const size_t count,
										MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	size_t to_read = count;
	zend_uchar * p = buffer;

	DBG_ENTER("mysqlnd_net::receive_ex");
#ifdef MYSQLND_COMPRESSION_ENABLED
	if (net->data->compressed) {
		if (net->uncompressed_data) {
			size_t to_read_from_buffer = MIN(net->uncompressed_data->bytes_left(net->uncompressed_data), to_read);
			DBG_INF_FMT("reading "MYSQLND_SZ_T_SPEC" from uncompressed_data buffer", to_read_from_buffer);
			if (to_read_from_buffer) {
				net->uncompressed_data->read(net->uncompressed_data, to_read_from_buffer, (zend_uchar *) p);
				p += to_read_from_buffer;
				to_read -= to_read_from_buffer;
			}
			DBG_INF_FMT("left "MYSQLND_SZ_T_SPEC" to read", to_read);
			if (TRUE == net->uncompressed_data->is_empty(net->uncompressed_data)) {
				/* Everything was consumed. This should never happen here, but for security */
				net->uncompressed_data->free_buffer(&net->uncompressed_data);
			}
		}
		if (to_read) {
			zend_uchar net_header[MYSQLND_HEADER_SIZE];
			size_t net_payload_size;
			zend_uchar packet_no;

			if (FAIL == net->data->m.network_read_ex(net, net_header, MYSQLND_HEADER_SIZE, conn_stats, error_info)) {
				DBG_RETURN(FAIL);
			}
			net_payload_size = uint3korr(net_header);
			packet_no = uint1korr(net_header + 3);
			if (net->compressed_envelope_packet_no != packet_no) {
				DBG_ERR_FMT("Transport level: packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
							net->compressed_envelope_packet_no, packet_no, net_payload_size);

				php_error(E_WARNING, "Packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
						  net->compressed_envelope_packet_no, packet_no, net_payload_size);
				DBG_RETURN(FAIL);
			}
			net->compressed_envelope_packet_no++;
#ifdef MYSQLND_DUMP_HEADER_N_BODY
			DBG_INF_FMT("HEADER: hwd_packet_no=%u size=%3u", packet_no, (zend_ulong) net_payload_size);
#endif
			/* Now let's read from the wire, decompress it and fill the read buffer */
			net->data->m.read_compressed_packet_from_stream_and_fill_read_buffer(net, net_payload_size, conn_stats, error_info);

			/*
			  Now a bit of recursion - read from the read buffer,
			  if the data which we have just read from the wire
			  is not enough, then the recursive call will try to
			  satisfy it until it is satisfied.
			*/
			DBG_RETURN(net->data->m.receive_ex(net, p, to_read, conn_stats, error_info));
		}
		DBG_RETURN(PASS);
	}
#endif /* MYSQLND_COMPRESSION_ENABLED */
	DBG_RETURN(net->data->m.network_read_ex(net, p, to_read, conn_stats, error_info));
}
/* }}} */


/* {{{ mysqlnd_net::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, set_client_option)(MYSQLND_NET * const net, enum mysqlnd_option option, const char * const value)
{
	DBG_ENTER("mysqlnd_net::set_client_option");
	DBG_INF_FMT("option=%u", option);
	switch (option) {
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
			DBG_INF("MYSQLND_OPT_NET_CMD_BUFFER_SIZE");
			if (*(unsigned int*) value < MYSQLND_NET_CMD_BUFFER_MIN_SIZE) {
				DBG_RETURN(FAIL);
			}
			net->cmd_buffer.length = *(unsigned int*) value;
			DBG_INF_FMT("new_length="MYSQLND_SZ_T_SPEC, net->cmd_buffer.length);
			if (!net->cmd_buffer.buffer) {
				net->cmd_buffer.buffer = mnd_pemalloc(net->cmd_buffer.length, net->persistent);
			} else {
				net->cmd_buffer.buffer = mnd_perealloc(net->cmd_buffer.buffer, net->cmd_buffer.length, net->persistent);
			}
			break;
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
		case MYSQL_OPT_COMPRESS:
			net->data->options.flags |= MYSQLND_NET_FLAG_USE_COMPRESSION;
			break;
		case MYSQL_SERVER_PUBLIC_KEY:
			{
				zend_bool pers = net->persistent;
				if (net->data->options.sha256_server_public_key) {
					mnd_pefree(net->data->options.sha256_server_public_key, pers);
				}
				net->data->options.sha256_server_public_key = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		default:
			DBG_RETURN(FAIL);
	}
	DBG_RETURN(PASS);
}
/* }}} */

/* {{{ mysqlnd_net::consume_uneaten_data */
size_t
MYSQLND_METHOD(mysqlnd_net, consume_uneaten_data)(MYSQLND_NET * const net, enum php_mysqlnd_server_command cmd)
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

	DBG_ENTER("mysqlnd_net::consume_uneaten_data");

	if (PHP_STREAM_OPTION_RETURN_ERR != was_blocked) {
		/* Do a read of 1 byte */
		int bytes_consumed;

		do {
			skipped_bytes += (bytes_consumed = php_stream_read(net_stream, tmp_buf, sizeof(tmp_buf)));
		} while (bytes_consumed == sizeof(tmp_buf));

		if (was_blocked) {
			net_stream->ops->set_option(net_stream, opt, 1, NULL);
		}

		if (bytes_consumed) {
			DBG_ERR_FMT("Skipped %u bytes. Last command %s hasn't consumed all the output from the server",
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
/* {{{ mysqlnd_net::enable_ssl */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, enable_ssl)(MYSQLND_NET * const net)
{
#ifdef MYSQLND_SSL_SUPPORTED
	php_stream_context * context = php_stream_context_alloc();
	php_stream * net_stream = net->data->m.get_stream(net);
	zend_bool any_flag = FALSE;

	DBG_ENTER("mysqlnd_net::enable_ssl");
	if (!context) {
		DBG_RETURN(FAIL);
	}

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
#if PHP_API_VERSION >= 20131106
	php_stream_context_set(net_stream, context);
#else
	php_stream_context_set(net_stream, context);
#endif
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
#if PHP_API_VERSION >= 20131106
	php_stream_context_set(net_stream, NULL);
#else
	php_stream_context_set(net_stream, NULL);
#endif

	if (net->data->options.timeout_read) {
		struct timeval tv;
		DBG_INF_FMT("setting %u as PHP_STREAM_OPTION_READ_TIMEOUT", net->data->options.timeout_read);
		tv.tv_sec = net->data->options.timeout_read;
		tv.tv_usec = 0;
		php_stream_set_option(net_stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
	}

	DBG_RETURN(PASS);
#else
	DBG_ENTER("mysqlnd_net::enable_ssl");
	DBG_INF("MYSQLND_SSL_SUPPORTED is not defined");
	DBG_RETURN(PASS);
#endif
}
/* }}} */


/* {{{ mysqlnd_net::disable_ssl */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, disable_ssl)(MYSQLND_NET * const net)
{
	DBG_ENTER("mysqlnd_net::disable_ssl");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::free_contents */
static void
MYSQLND_METHOD(mysqlnd_net, free_contents)(MYSQLND_NET * net)
{
	zend_bool pers = net->persistent;
	DBG_ENTER("mysqlnd_net::free_contents");

#ifdef MYSQLND_COMPRESSION_ENABLED
	if (net->uncompressed_data) {
		net->uncompressed_data->free_buffer(&net->uncompressed_data);
	}
#endif
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
	if (net->data->options.sha256_server_public_key) {
		mnd_pefree(net->data->options.sha256_server_public_key, pers);
		net->data->options.sha256_server_public_key = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_net::close_stream */
static void
MYSQLND_METHOD(mysqlnd_net, close_stream)(MYSQLND_NET * const net, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	php_stream * net_stream;
	DBG_ENTER("mysqlnd_net::close_stream");
	if (net && (net_stream = net->data->m.get_stream(net))) {
		zend_bool pers = net->persistent;
		DBG_INF_FMT("Freeing stream. abstract=%p", net_stream->abstract);
		if (pers) {
			if (EG(active)) {
				php_stream_free(net_stream, PHP_STREAM_FREE_CLOSE_PERSISTENT | PHP_STREAM_FREE_RSRC_DTOR);
			} else {
				/*
				  otherwise we will crash because the EG(persistent_list) has been freed already,
				  before the modules are shut down
				*/
				php_stream_free(net_stream, PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_RSRC_DTOR);
			}
		} else {
			php_stream_free(net_stream, PHP_STREAM_FREE_CLOSE);
		}
		(void) net->data->m.set_stream(net, NULL);
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_net::init */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, init)(MYSQLND_NET * const net, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	unsigned int buf_size;
	DBG_ENTER("mysqlnd_net::init");

	buf_size = MYSQLND_G(net_cmd_buffer_size); /* this is long, cast to unsigned int*/
	net->data->m.set_client_option(net, MYSQLND_OPT_NET_CMD_BUFFER_SIZE, (char *) &buf_size);

	buf_size = MYSQLND_G(net_read_buffer_size); /* this is long, cast to unsigned int*/
	net->data->m.set_client_option(net, MYSQLND_OPT_NET_READ_BUFFER_SIZE, (char *)&buf_size);

	buf_size = MYSQLND_G(net_read_timeout); /* this is long, cast to unsigned int*/
	net->data->m.set_client_option(net, MYSQL_OPT_READ_TIMEOUT, (char *)&buf_size);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::dtor */
static void
MYSQLND_METHOD(mysqlnd_net, dtor)(MYSQLND_NET * const net, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("mysqlnd_net::dtor");
	if (net) {
		net->data->m.free_contents(net);
		net->data->m.close_stream(net, stats, error_info);

		if (net->cmd_buffer.buffer) {
			DBG_INF("Freeing cmd buffer");
			mnd_pefree(net->cmd_buffer.buffer, net->persistent);
			net->cmd_buffer.buffer = NULL;
		}

		mnd_pefree(net->data, net->data->persistent);
		mnd_pefree(net, net->persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_net::get_stream */
static php_stream *
MYSQLND_METHOD(mysqlnd_net, get_stream)(const MYSQLND_NET * const net)
{
	DBG_ENTER("mysqlnd_net::get_stream");
	DBG_INF_FMT("%p", net? net->data->stream:NULL);
	DBG_RETURN(net? net->data->stream:NULL);
}
/* }}} */


/* {{{ mysqlnd_net::set_stream */
static php_stream *
MYSQLND_METHOD(mysqlnd_net, set_stream)(MYSQLND_NET * const net, php_stream * net_stream)
{
	php_stream * ret = NULL;
	DBG_ENTER("mysqlnd_net::set_stream");
	if (net) {
		net->data->stream = net_stream;
		ret = net->data->stream;
	}
	DBG_RETURN(ret);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_net)
	MYSQLND_METHOD(mysqlnd_net, init),
	MYSQLND_METHOD(mysqlnd_net, dtor),
	MYSQLND_METHOD(mysqlnd_net, connect_ex),
	MYSQLND_METHOD(mysqlnd_net, close_stream),
	MYSQLND_METHOD(mysqlnd_net, open_pipe),
	MYSQLND_METHOD(mysqlnd_net, open_tcp_or_unix),
	MYSQLND_METHOD(mysqlnd_net, get_stream),
	MYSQLND_METHOD(mysqlnd_net, set_stream),
	MYSQLND_METHOD(mysqlnd_net, get_open_stream),
	MYSQLND_METHOD(mysqlnd_net, post_connect_set_opt),
	MYSQLND_METHOD(mysqlnd_net, set_client_option),
	MYSQLND_METHOD(mysqlnd_net, decode),
	MYSQLND_METHOD(mysqlnd_net, encode),
	MYSQLND_METHOD(mysqlnd_net, consume_uneaten_data),
	MYSQLND_METHOD(mysqlnd_net, free_contents),
	MYSQLND_METHOD(mysqlnd_net, enable_ssl),
	MYSQLND_METHOD(mysqlnd_net, disable_ssl),
	MYSQLND_METHOD(mysqlnd_net, network_read_ex),
	MYSQLND_METHOD(mysqlnd_net, network_write_ex),
	MYSQLND_METHOD(mysqlnd_net, send_ex),
	MYSQLND_METHOD(mysqlnd_net, receive_ex),
#ifdef MYSQLND_COMPRESSION_ENABLED
	MYSQLND_METHOD(mysqlnd_net, read_compressed_packet_from_stream_and_fill_read_buffer),
#else
	NULL,
#endif
	NULL, /* unused 1 */
	NULL, /* unused 2 */
	NULL, /* unused 3 */
	NULL, /* unused 4 */
	NULL  /* unused 5 */
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_net_init */
PHPAPI MYSQLND_NET *
mysqlnd_net_init(zend_bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	MYSQLND_NET * net;
	DBG_ENTER("mysqlnd_net_init");
	net = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory).get_io_channel(persistent, stats, error_info);
	DBG_RETURN(net);
}
/* }}} */


/* {{{ mysqlnd_net_free */
PHPAPI void
mysqlnd_net_free(MYSQLND_NET * const net, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("mysqlnd_net_free");
	if (net) {
		net->data->m.dtor(net, stats, error_info);
	}
	DBG_VOID_RETURN;
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
