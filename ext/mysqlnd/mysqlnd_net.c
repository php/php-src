/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2010 The PHP Group                                |
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
	TSRMLS_FETCH();

	DBG_ENTER("mysqlnd_set_sock_no_delay");

	if (result == -1) {
		ret = FAILURE;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_net::network_read */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, network_read)(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC)
{
	size_t to_read = count, ret;
	size_t old_chunk_size = conn->net->stream->chunk_size;
	DBG_ENTER("mysqlnd_net::network_read");
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
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats, STAT_BYTES_RECEIVED, count);
	conn->net->stream->chunk_size = old_chunk_size;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::network_write */
static size_t
MYSQLND_METHOD(mysqlnd_net, network_write)(MYSQLND * const conn, const zend_uchar * const buf, size_t count TSRMLS_DC)
{
	size_t ret;
	DBG_ENTER("mysqlnd_net::network_write");
	ret = php_stream_write(conn->net->stream, (char *)buf, count);
	DBG_RETURN(ret);
}
/* }}} */



/* {{{ mysqlnd_net::connect */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, connect)(MYSQLND_NET * net, const char * const scheme, size_t scheme_len, zend_bool persistent, char **errstr, int * errcode TSRMLS_DC)
{
#if PHP_API_VERSION < 20100412
	unsigned int streams_options = ENFORCE_SAFE_MODE;
#else
	unsigned int streams_options = 0;
#endif
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

	DBG_INF_FMT("calling php_stream_xport_create");
	net->stream = php_stream_xport_create(scheme, scheme_len, streams_options, streams_flags,
										  hashed_details, (net->options.timeout_connect) ? &tv : NULL,
										  NULL /*ctx*/, errstr, errcode);

	if (*errstr || !net->stream) {
		if (hashed_details) {
			efree(hashed_details); /* allocated by spprintf */
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

		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_len + 1, (void*) &le) == SUCCESS) {
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
	/*
	  Streams are not meant for C extensions! Thus we need a hack. Every connected stream will
	  be registered as resource (in EG(regular_list). So far, so good. However, it won't be
	  unregistered till the script ends. So, we need to take care of that.
	*/
	net->stream->in_free = 1;
	zend_hash_index_del(&EG(regular_list), net->stream->rsrc_id);
	net->stream->in_free = 0;

	if (!net->options.timeout_read) {
		/* should always happen because read_timeout cannot be set via API */
		net->options.timeout_read = (unsigned int) MYSQLND_G(net_read_timeout);
	}
	if (net->options.timeout_read) {
		DBG_INF_FMT("setting %u as PHP_STREAM_OPTION_READ_TIMEOUT", net->options.timeout_read);
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


/* We assume that MYSQLND_HEADER_SIZE is 4 bytes !! */
#define COPY_HEADER(T,A)  do { \
		*(((char *)(T)))   = *(((char *)(A)));\
		*(((char *)(T))+1) = *(((char *)(A))+1);\
		*(((char *)(T))+2) = *(((char *)(A))+2);\
		*(((char *)(T))+3) = *(((char *)(A))+3); } while (0)
#define STORE_HEADER_SIZE(safe_storage, buffer)  COPY_HEADER((safe_storage), (buffer))
#define RESTORE_HEADER_SIZE(buffer, safe_storage) STORE_HEADER_SIZE((safe_storage), (buffer))

/* {{{ mysqlnd_net::send */
/*
  IMPORTANT : It's expected that buf has place in the beginning for MYSQLND_HEADER_SIZE !!!!
			  This is done for performance reasons in the caller of this function.
			  Otherwise we will have to do send two TCP packets, or do new alloc and memcpy.
			  Neither are quick, thus the clients of this function are obligated to do
			  what they are asked for.

  `count` is actually the length of the payload data. Thus :
  count + MYSQLND_HEADER_SIZE = sizeof(buf) (not the pointer but the actual buffer)
*/
size_t
MYSQLND_METHOD(mysqlnd_net, send)(MYSQLND * const conn, char * const buf, size_t count TSRMLS_DC)
{
	zend_uchar safe_buf[((MYSQLND_HEADER_SIZE) + (sizeof(zend_uchar)) - 1) / (sizeof(zend_uchar))];
	zend_uchar *safe_storage = safe_buf;
	MYSQLND_NET *net = conn->net;
	size_t old_chunk_size = net->stream->chunk_size;
	size_t ret, packets_sent = 1;
	size_t left = count;
	zend_uchar *p = (zend_uchar *) buf;
	zend_uchar * compress_buf = NULL;
	size_t to_be_sent;

	DBG_ENTER("mysqlnd_net::send");
	DBG_INF_FMT("conn=%llu count=%lu compression=%u", conn->thread_id, count, net->compressed);

	net->stream->chunk_size = MYSQLND_MAX_PACKET_SIZE;

	if (net->compressed == TRUE) {
		size_t comp_buf_size = MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE + MIN(left, MYSQLND_MAX_PACKET_SIZE);
		DBG_INF_FMT("compress_buf_size="MYSQLND_SZ_T_SPEC, comp_buf_size);
		compress_buf = mnd_emalloc(comp_buf_size);
	}

	do {
		to_be_sent = MIN(left, MYSQLND_MAX_PACKET_SIZE);
#ifdef MYSQLND_COMPRESSION_ENABLED
		if (net->compressed == TRUE) {
			/* here we need to compress the data and then write it, first comes the compressed header */
			size_t tmp_complen = to_be_sent;
			size_t payload_size;
			zend_uchar * uncompressed_payload = p; /* should include the header */

			STORE_HEADER_SIZE(safe_storage, uncompressed_payload);
			int3store(uncompressed_payload, to_be_sent);
			int1store(uncompressed_payload + 3, net->packet_no);
			if (PASS == net->m.encode((compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), tmp_complen,
									   uncompressed_payload, to_be_sent + MYSQLND_HEADER_SIZE TSRMLS_CC))
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
			ret = conn->net->m.network_write(conn, compress_buf, payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE TSRMLS_CC);
			net->compressed_envelope_packet_no++;
  #if WHEN_WE_NEED_TO_CHECK_WHETHER_COMPRESSION_WORKS_CORRECTLY
			if (res == Z_OK) {
				size_t decompressed_size = left + MYSQLND_HEADER_SIZE;
				zend_uchar * decompressed_data = mnd_malloc(decompressed_size);
				int error = net->m.decode(decompressed_data, decompressed_size,
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
			ret = conn->net->m.network_write(conn, p, to_be_sent + MYSQLND_HEADER_SIZE TSRMLS_CC);
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
	} while (ret && (left > 0 || to_be_sent == MYSQLND_MAX_PACKET_SIZE));

	DBG_INF_FMT("packet_size="MYSQLND_SZ_T_SPEC" packet_no=%u", left, net->packet_no);
	/* Even for zero size payload we have to send a packet */
	if (!ret) {
		DBG_ERR_FMT("Can't %u send bytes", count);
		conn->state = CONN_QUIT_SENT;
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
	}

	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn->stats,
			STAT_BYTES_SENT, count + packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PROTOCOL_OVERHEAD_OUT, packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PACKETS_SENT, packets_sent);

	net->stream->chunk_size = old_chunk_size;
	if (compress_buf) {
		mnd_efree(compress_buf);
	}
	DBG_RETURN(ret);
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
php_mysqlnd_read_buffer_free(MYSQLND_READ_BUFFER ** buffer TSRMLS_DC)
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
mysqlnd_create_read_buffer(size_t count TSRMLS_DC)
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


/* {{{ mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer */
static enum_func_status
mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer(MYSQLND * conn, size_t net_payload_size TSRMLS_DC)
{
	MYSQLND_NET * net = conn->net;
	size_t decompressed_size;
	enum_func_status ret = PASS;
	zend_uchar * compressed_data = NULL;
	zend_uchar comp_header[COMPRESSED_HEADER_SIZE];
	DBG_ENTER("mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer");

	/* Read the compressed header */
	if (FAIL == conn->net->m.network_read(conn, comp_header, COMPRESSED_HEADER_SIZE TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}
	decompressed_size = uint3korr(comp_header);

	/* When decompressed_size is 0, then the data is not compressed, and we have wasted 3 bytes */
	/* we need to decompress the data */

	if (decompressed_size) {
		compressed_data = mnd_emalloc(net_payload_size);
		if (FAIL == conn->net->m.network_read(conn, compressed_data, net_payload_size TSRMLS_CC)) {
			ret = FAIL;
			goto end;
		}
		net->uncompressed_data = mysqlnd_create_read_buffer(decompressed_size TSRMLS_CC);
		ret = net->m.decode(net->uncompressed_data->data, decompressed_size, compressed_data, net_payload_size TSRMLS_CC);
		if (ret == FAIL) {
			goto end;
		}
	} else {
		DBG_INF_FMT("The server decided not to compress the data. Our job is easy. Copying %u bytes", net_payload_size);
		net->uncompressed_data = mysqlnd_create_read_buffer(net_payload_size TSRMLS_CC);
		if (FAIL == conn->net->m.network_read(conn, net->uncompressed_data->data, net_payload_size TSRMLS_CC)) {
			ret = FAIL;
			goto end;
		}
	}
end:
	if (compressed_data) {
		mnd_efree(compressed_data);
	}
	DBG_RETURN(ret);
}
/* }}} */
#endif /* MYSQLND_COMPRESSION_ENABLED */


/* {{{ mysqlnd_net::decode */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, decode)(zend_uchar * uncompressed_data, size_t uncompressed_data_len,
									const zend_uchar * const compressed_data, size_t compressed_data_len TSRMLS_DC)
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
MYSQLND_METHOD(mysqlnd_net, encode)(zend_uchar * compress_buffer, size_t compress_buffer_len,
									const zend_uchar * const uncompressed_data, size_t uncompressed_data_len TSRMLS_DC)
{
#ifdef MYSQLND_COMPRESSION_ENABLED
	int error;
	uLongf tmp_complen = compress_buffer_len;
	DBG_ENTER("mysqlnd_net::encode");
	error = compress(compress_buffer, &tmp_complen, uncompressed_data, uncompressed_data_len);

	if (error != Z_OK) {
		DBG_INF_FMT("compression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", error, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
	} else {
		DBG_INF_FMT("compression successful. compressed size=%lu", tmp_complen);
	}
	DBG_RETURN(error == Z_OK? PASS:FAIL);
#else
	DBG_ENTER("mysqlnd_net::encode");
	DBG_RETURN(FAIL);
#endif
}
/* }}} */


/* {{{ mysqlnd_net::receive */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, receive)(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC)
{
	size_t to_read = count;
	zend_uchar * p = buffer;
	MYSQLND_NET * net = conn->net;

	DBG_ENTER("mysqlnd_net::receive");
#ifdef MYSQLND_COMPRESSION_ENABLED
	if (net->compressed) {
		if (net->uncompressed_data) {
			size_t to_read_from_buffer = MIN(net->uncompressed_data->bytes_left(net->uncompressed_data), to_read);
			DBG_INF_FMT("reading %u from uncompressed_data buffer", to_read_from_buffer);
			if (to_read_from_buffer) {
				net->uncompressed_data->read(net->uncompressed_data, to_read_from_buffer, (zend_uchar *) p);
				p += to_read_from_buffer;
				to_read -= to_read_from_buffer;
			}
			DBG_INF_FMT("left %u to read", to_read);
			if (TRUE == net->uncompressed_data->is_empty(net->uncompressed_data)) {
				/* Everything was consumed. This should never happen here, but for security */
				net->uncompressed_data->free_buffer(&net->uncompressed_data TSRMLS_CC);
			}
		}
		if (to_read) {
			zend_uchar net_header[MYSQLND_HEADER_SIZE];
			size_t net_payload_size;
			zend_uchar packet_no;

			if (FAIL == net->m.network_read(conn, net_header, MYSQLND_HEADER_SIZE TSRMLS_CC)) {
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
			DBG_INF_FMT("HEADER: hwd_packet_no=%u size=%3u", packet_no, (unsigned long) net_payload_size);
#endif
			/* Now let's read from the wire, decompress it and fill the read buffer */
			mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer(conn, net_payload_size TSRMLS_CC);

			/*
			  Now a bit of recursion - read from the read buffer,
			  if the data which we have just read from the wire
			  is not enough, then the recursive call will try to
			  satisfy it until it is satisfied.
			*/
			DBG_RETURN(net->m.receive(conn, p, to_read TSRMLS_CC));
		}
		DBG_RETURN(PASS);
	}
#endif /* MYSQLND_COMPRESSION_ENABLED */
	DBG_RETURN(net->m.network_read(conn, p, to_read TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_net::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, set_client_option)(MYSQLND_NET * const net, enum mysqlnd_option option, const char * const value TSRMLS_DC)
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
		case MYSQLND_OPT_SSL_KEY:
			{
				zend_bool pers = net->persistent;
				if (net->options.ssl_key) {
					mnd_pefree(net->options.ssl_key, pers);
				}
				net->options.ssl_key = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CERT:
			{
				zend_bool pers = net->persistent;
				if (net->options.ssl_cert) {
					mnd_pefree(net->options.ssl_cert, pers);
				}
				net->options.ssl_cert = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CA:
			{
				zend_bool pers = net->persistent;
				if (net->options.ssl_ca) {
					mnd_pefree(net->options.ssl_ca, pers);
				}
				net->options.ssl_ca = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CAPATH:
			{
				zend_bool pers = net->persistent;
				if (net->options.ssl_capath) {
					mnd_pefree(net->options.ssl_capath, pers);
				}
				net->options.ssl_capath = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_CIPHER:
			{
				zend_bool pers = net->persistent;
				if (net->options.ssl_cipher) {
					mnd_pefree(net->options.ssl_cipher, pers);
				}
				net->options.ssl_cipher = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQLND_OPT_SSL_PASSPHRASE:
			{
				zend_bool pers = net->persistent;
				if (net->options.ssl_passphrase) {
					mnd_pefree(net->options.ssl_passphrase, pers);
				}
				net->options.ssl_passphrase = value? mnd_pestrdup(value, pers) : NULL;
				break;
			}
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
			net->options.ssl_verify_peer = value? ((*(zend_bool *)value)? TRUE:FALSE): FALSE;
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

/* {{{ mysqlnd_net::consume_uneaten_data */
size_t 
MYSQLND_METHOD(mysqlnd_net, consume_uneaten_data)(MYSQLND_NET * const net, enum php_mysqlnd_server_command cmd TSRMLS_DC)
{
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	/*
	  Switch to non-blocking mode and try to consume something from
	  the line, if possible, then continue. This saves us from looking for
	  the actuall place where out-of-order packets have been sent.
	  If someone is completely sure that everything is fine, he can switch it
	  off.
	*/
	char tmp_buf[256];
	size_t skipped_bytes = 0;
	int opt = PHP_STREAM_OPTION_BLOCKING;
	int was_blocked = net->stream->ops->set_option(net->stream, opt, 0, NULL TSRMLS_CC);

	DBG_ENTER("mysqlnd_net::consume_uneaten_data");

	if (PHP_STREAM_OPTION_RETURN_ERR != was_blocked) {
		/* Do a read of 1 byte */
		int bytes_consumed;

		do {
			skipped_bytes += (bytes_consumed = php_stream_read(net->stream, tmp_buf, sizeof(tmp_buf)));
		} while (bytes_consumed == sizeof(tmp_buf));

		if (was_blocked) {
			net->stream->ops->set_option(net->stream, opt, 1, NULL TSRMLS_CC);
		}

		if (bytes_consumed) {
			DBG_ERR_FMT("Skipped %u bytes. Last command %s hasn't consumed all the output from the server",
						bytes_consumed, mysqlnd_command_to_text[net->last_command]);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Skipped %u bytes. Last command %s hasn't "
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
MYSQLND_METHOD(mysqlnd_net, enable_ssl)(MYSQLND_NET * const net TSRMLS_DC)
{
#ifdef MYSQLND_SSL_SUPPORTED
	php_stream_context *context = php_stream_context_alloc();
	DBG_ENTER("mysqlnd_net::enable_ssl");
	if (!context) {
		DBG_RETURN(FAIL);
	}

	if (net->options.ssl_key) {
		zval key_zval;
		ZVAL_STRING(&key_zval, net->options.ssl_key, 0);
		DBG_INF("key");
		php_stream_context_set_option(context, "ssl", "local_pk", &key_zval);
	}
	if (net->options.ssl_verify_peer) {
		zval verify_peer_zval;
		ZVAL_TRUE(&verify_peer_zval);
		DBG_INF("verify peer");
		php_stream_context_set_option(context, "ssl", "verify_peer", &verify_peer_zval);
	}
	if (net->options.ssl_cert) {
		zval cert_zval;
		ZVAL_STRING(&cert_zval, net->options.ssl_cert, 0);
		DBG_INF_FMT("local_cert=%s", net->options.ssl_cert);
		php_stream_context_set_option(context, "ssl", "local_cert", &cert_zval);
		if (!net->options.ssl_key) {
			php_stream_context_set_option(context, "ssl", "local_pk", &cert_zval);
		}
	}
	if (net->options.ssl_ca) {
		zval cafile_zval;
		ZVAL_STRING(&cafile_zval, net->options.ssl_ca, 0);
		DBG_INF_FMT("cafile=%s", net->options.ssl_ca);
		php_stream_context_set_option(context, "ssl", "cafile", &cafile_zval);
	}
	if (net->options.ssl_capath) {
		zval capath_zval;
		ZVAL_STRING(&capath_zval, net->options.ssl_capath, 0);
		DBG_INF_FMT("capath=%s", net->options.ssl_capath);
		php_stream_context_set_option(context, "ssl", "cafile", &capath_zval);
	}
	if (net->options.ssl_passphrase) {
		zval passphrase_zval;
		ZVAL_STRING(&passphrase_zval, net->options.ssl_passphrase, 0);
		php_stream_context_set_option(context, "ssl", "passphrase", &passphrase_zval);
	}
	if (net->options.ssl_cipher) {
		zval cipher_zval;
		ZVAL_STRING(&cipher_zval, net->options.ssl_cipher, 0);
		DBG_INF_FMT("ciphers=%s", net->options.ssl_cipher);
		php_stream_context_set_option(context, "ssl", "ciphers", &cipher_zval);
	}
	php_stream_context_set(net->stream, context);
	if (php_stream_xport_crypto_setup(net->stream, STREAM_CRYPTO_METHOD_TLS_CLIENT, NULL TSRMLS_CC) < 0 ||
	    php_stream_xport_crypto_enable(net->stream, 1 TSRMLS_CC) < 0)
	{
		DBG_ERR("Cannot connect to MySQL by using SSL");
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot connect to MySQL by using SSL");
		DBG_RETURN(FAIL);
	}
	/*
	  get rid of the context. we are persistent and if this is a real pconn used by mysql/mysqli,
	  then the context would not survive cleaning of EG(regular_list), where it is registered, as a
	  resource. What happens is that after this destruction any use of the network will mean usage
	  of the context, which means usage of already freed memory, bad. Actually we don't need this
	  context anymore after we have enabled SSL on the connection. Thus it is very simple, we remove it.
	*/
	php_stream_context_set(net->stream, NULL);

	if (net->options.timeout_read) {
		struct timeval tv;
		DBG_INF_FMT("setting %u as PHP_STREAM_OPTION_READ_TIMEOUT", net->options.timeout_read);
		tv.tv_sec = net->options.timeout_read;
		tv.tv_usec = 0;
		php_stream_set_option(net->stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
	}

	DBG_RETURN(PASS);
#else
	DBG_ENTER("mysqlnd_net::enable_ssl");
	DBG_RETURN(PASS);
#endif
}
/* }}} */


/* {{{ mysqlnd_net::disable_ssl */
static enum_func_status
MYSQLND_METHOD(mysqlnd_net, disable_ssl)(MYSQLND_NET * const net TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_net::disable_ssl");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_net::set_client_option */
static void
MYSQLND_METHOD(mysqlnd_net, free_contents)(MYSQLND_NET * net TSRMLS_DC)
{
	zend_bool pers = net->persistent;
	DBG_ENTER("mysqlnd_net::free_contents");

#ifdef MYSQLND_COMPRESSION_ENABLED
	if (net->uncompressed_data) {
		net->uncompressed_data->free_buffer(&net->uncompressed_data TSRMLS_CC);
	}
#endif
	if (net->options.ssl_key) {
		mnd_pefree(net->options.ssl_key, pers);
		net->options.ssl_key = NULL;
	}
	if (net->options.ssl_cert) {
		mnd_pefree(net->options.ssl_cert, pers);
		net->options.ssl_cert = NULL;
	}
	if (net->options.ssl_ca) {
		mnd_pefree(net->options.ssl_ca, pers);
		net->options.ssl_ca = NULL;
	}
	if (net->options.ssl_capath) {
		mnd_pefree(net->options.ssl_capath, pers);
		net->options.ssl_capath = NULL;
	}
	if (net->options.ssl_cipher) {
		mnd_pefree(net->options.ssl_cipher, pers);
		net->options.ssl_cipher = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */

static 
MYSQLND_CLASS_METHODS_START(mysqlnd_net)
	MYSQLND_METHOD(mysqlnd_net, connect),
	MYSQLND_METHOD(mysqlnd_net, send),
	MYSQLND_METHOD(mysqlnd_net, receive),
	MYSQLND_METHOD(mysqlnd_net, set_client_option),
	MYSQLND_METHOD(mysqlnd_net, network_read),
	MYSQLND_METHOD(mysqlnd_net, network_write),
	MYSQLND_METHOD(mysqlnd_net, decode),
	MYSQLND_METHOD(mysqlnd_net, encode),
	MYSQLND_METHOD(mysqlnd_net, consume_uneaten_data),
	MYSQLND_METHOD(mysqlnd_net, free_contents),
	MYSQLND_METHOD(mysqlnd_net, enable_ssl),
	MYSQLND_METHOD(mysqlnd_net, disable_ssl)
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_net_init */
PHPAPI MYSQLND_NET *
mysqlnd_net_init(zend_bool persistent TSRMLS_DC)
{
	size_t alloc_size = sizeof(MYSQLND_NET) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_NET * net = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_net_init");
	DBG_INF_FMT("persistent=%u", persistent);
	if (net) {
		net->persistent = persistent;
		net->m = mysqlnd_mysqlnd_net_methods;

		{
			unsigned int buf_size = MYSQLND_G(net_cmd_buffer_size); /* this is long, cast to unsigned int*/
			net->m.set_client_option(net, MYSQLND_OPT_NET_CMD_BUFFER_SIZE, (char *) &buf_size TSRMLS_CC);
		}
	}
	DBG_RETURN(net);
}
/* }}} */


/* {{{ mysqlnd_net_free */
PHPAPI void
mysqlnd_net_free(MYSQLND_NET * const net TSRMLS_DC)
{
	zend_bool pers = net->persistent;

	DBG_ENTER("mysqlnd_net_free");

	if (net) {
		net->m.free_contents(net TSRMLS_CC);
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


/* {{{ _mysqlnd_plugin_get_plugin_net_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_net_data(const MYSQLND_NET * net, unsigned int plugin_id TSRMLS_DC)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_net_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!net || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)net + sizeof(MYSQLND_NET) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_net_get_methods */
PHPAPI struct st_mysqlnd_net_methods *
mysqlnd_net_get_methods()
{
	return &mysqlnd_mysqlnd_net_methods;
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
