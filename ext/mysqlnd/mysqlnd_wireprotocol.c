
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
#include "mysqlnd_palloc.h"
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

#define MYSQLND_SILENT 1

#define MYSQLND_DUMP_HEADER_N_BODY


#define	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_size, packet_type_as_text, packet_type) \
	{ \
		DBG_INF_FMT("buf=%p size=%u", (buf), (buf_size)); \
		if (FAIL == mysqlnd_read_header((conn), &((packet)->header) TSRMLS_CC)) {\
			CONN_SET_STATE(conn, CONN_QUIT_SENT); \
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);\
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysqlnd_server_gone); \
			DBG_ERR_FMT("Can't read %s's header", (packet_type_as_text)); \
			DBG_RETURN(FAIL);\
		}\
		if ((buf_size) < (packet)->header.size) { \
			DBG_ERR_FMT("Packet buffer %u wasn't big enough %u, %u bytes will be unread", \
						(buf_size), (packet)->header.size, (packet)->header.size - (buf_size)); \
						DBG_RETURN(FAIL); \
		}\
		if (FAIL == mysqlnd_read_body((conn), &((packet)->header), (buf) TSRMLS_CC)) { \
			CONN_SET_STATE(conn, CONN_QUIT_SENT); \
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);\
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysqlnd_server_gone); \
			DBG_ERR_FMT("Empty '%s' packet body", (packet_type_as_text)); \
			DBG_RETURN(FAIL);\
		} \
		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(&conn->stats, packet_type_to_statistic_byte_count[packet_type], \
											MYSQLND_HEADER_SIZE + (packet)->header.size, \
											packet_type_to_statistic_packet_count[packet_type], \
											1); \
	}


extern mysqlnd_packet_methods packet_methods[];

static const char *unknown_sqlstate= "HY000";

char * const mysqlnd_empty_string = "";

/* Used in mysqlnd_debug.c */
char * mysqlnd_read_header_name	= "mysqlnd_read_header";
char * mysqlnd_read_body_name	= "mysqlnd_read_body";


/* {{{ mysqlnd_command_to_text
 */
const char * const mysqlnd_command_to_text[COM_END] =
{
  "SLEEP", "QUIT", "INIT_DB", "QUERY", "FIELD_LIST",
  "CREATE_DB", "DROP_DB", "REFRESH", "SHUTDOWN", "STATISTICS",
  "PROCESS_INFO", "CONNECT", "PROCESS_KILL", "DEBUG", "PING",
  "TIME", "DELAYED_INSERT", "CHANGE_USER", "BINLOG_DUMP",
  "TABLE_DUMP", "CONNECT_OUT", "REGISTER_SLAVE",
  "STMT_PREPARE", "STMT_EXECUTE", "STMT_SEND_LONG_DATA", "STMT_CLOSE",
  "STMT_RESET", "SET_OPTION", "STMT_FETCH", "DAEMON"
};
/* }}} */



static enum_mysqlnd_collected_stats packet_type_to_statistic_byte_count[PROT_LAST] =
{
	STAT_LAST,
	STAT_LAST,
	STAT_BYTES_RECEIVED_OK,
	STAT_BYTES_RECEIVED_EOF,
	STAT_LAST,
	STAT_BYTES_RECEIVED_RSET_HEADER,
	STAT_BYTES_RECEIVED_RSET_FIELD_META,
	STAT_BYTES_RECEIVED_RSET_ROW,
	STAT_BYTES_RECEIVED_PREPARE_RESPONSE,
	STAT_BYTES_RECEIVED_CHANGE_USER,
};

static enum_mysqlnd_collected_stats packet_type_to_statistic_packet_count[PROT_LAST] =
{
	STAT_LAST,
	STAT_LAST,
	STAT_PACKETS_RECEIVED_OK,
	STAT_PACKETS_RECEIVED_EOF,
	STAT_LAST,
	STAT_PACKETS_RECEIVED_RSET_HEADER,
	STAT_PACKETS_RECEIVED_RSET_FIELD_META,
	STAT_PACKETS_RECEIVED_RSET_ROW,
	STAT_PACKETS_RECEIVED_PREPARE_RESPONSE,
	STAT_PACKETS_RECEIVED_CHANGE_USER,
};


/* {{{ php_mysqlnd_net_field_length
   Get next field's length */
unsigned long php_mysqlnd_net_field_length(zend_uchar **packet)
{
	register zend_uchar *p= (zend_uchar *)*packet;

	if (*p < 251) {
		(*packet)++;
		return (unsigned long) *p;
	}

	switch (*p) {
		case 251:
			(*packet)++;
			return MYSQLND_NULL_LENGTH;
		case 252:
			(*packet) += 3;
			return (unsigned long) uint2korr(p+1);
		case 253:
			(*packet) += 4;
			return (unsigned long) uint3korr(p+1);
		default:
			(*packet) += 9;
			return (unsigned long) uint4korr(p+1);
	}
}
/* }}} */


/* {{{ php_mysqlnd_net_field_length_ll
   Get next field's length */
uint64_t php_mysqlnd_net_field_length_ll(zend_uchar **packet)
{
	register zend_uchar *p= (zend_uchar *)*packet;

	if (*p < 251) {
		(*packet)++;
		return (uint64_t) *p;
	}

	switch (*p) {
		case 251:
			(*packet)++;
			return (uint64_t) MYSQLND_NULL_LENGTH;
		case 252:
			(*packet) += 3;
			return (uint64_t) uint2korr(p + 1);
		case 253:
			(*packet) += 4;
			return (uint64_t) uint3korr(p + 1);
		default:
			(*packet) += 9;
			return (uint64_t) uint8korr(p + 1);
	}
}
/* }}} */


/* {{{ php_mysqlnd_net_store_length */
zend_uchar *php_mysqlnd_net_store_length(zend_uchar *packet, uint64_t length)
{
	if (length < (uint64_t) L64(251)) {
		*packet = (zend_uchar) length;
		return packet + 1;
	}

	if (length < (uint64_t) L64(65536)) {
		*packet++ = 252;
		int2store(packet,(unsigned int) length);
		return packet + 2;
	}

	if (length < (uint64_t) L64(16777216)) {
		*packet++ = 253;
		int3store(packet,(ulong) length);
		return packet + 3;
	}
	*packet++ = 254;
	int8store(packet, length);
	return packet + 8;
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
#endif


/* {{{ php_mysqlnd_consume_uneaten_data */
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
size_t php_mysqlnd_consume_uneaten_data(MYSQLND * const conn, enum php_mysqlnd_server_command cmd TSRMLS_DC)
{

	/*
	  Switch to non-blocking mode and try to consume something from
	  the line, if possible, then continue. This saves us from looking for
	  the actuall place where out-of-order packets have been sent.
	  If someone is completely sure that everything is fine, he can switch it
	  off.
	*/
	char tmp_buf[256];
	MYSQLND_NET *net = &conn->net;
	size_t skipped_bytes = 0;
	int opt = PHP_STREAM_OPTION_BLOCKING;
	int was_blocked = net->stream->ops->set_option(net->stream, opt, 0, NULL TSRMLS_CC);

	DBG_ENTER("php_mysqlnd_consume_uneaten_data");

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
}
#endif
/* }}} */


/* {{{ php_mysqlnd_read_error_from_line */
static enum_func_status
php_mysqlnd_read_error_from_line(zend_uchar *buf, size_t buf_len,
								char *error, int error_buf_len,
								unsigned int *error_no, char *sqlstate TSRMLS_DC)
{
	zend_uchar *p = buf;
	int error_msg_len= 0;

	DBG_ENTER("php_mysqlnd_read_error_from_line");

	if (buf_len > 2) {
		*error_no = uint2korr(p);
		p+= 2;
		/* sqlstate is following */
		if (*p == '#') {
			memcpy(sqlstate, ++p, MYSQLND_SQLSTATE_LENGTH);
			p+= MYSQLND_SQLSTATE_LENGTH;
		}
		error_msg_len = buf_len - (p - buf);
		error_msg_len = MIN(error_msg_len, error_buf_len - 1);
		memcpy(error, p, error_msg_len);
	} else {
		*error_no = CR_UNKNOWN_ERROR;
		memcpy(sqlstate, unknown_sqlstate, MYSQLND_SQLSTATE_LENGTH);
	}
	sqlstate[MYSQLND_SQLSTATE_LENGTH] = '\0';
	error[error_msg_len]= '\0';

	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_set_sock_no_delay */
int mysqlnd_set_sock_no_delay(php_stream *stream)
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


/* {{{ mysqlnd_stream_write */
size_t mysqlnd_stream_write(MYSQLND * const conn, const zend_uchar * const buf, size_t count TSRMLS_DC)
{
	size_t ret;
	DBG_ENTER("mysqlnd_stream_write");
	ret = php_stream_write(conn->net.stream, (char *)buf, count);
	DBG_RETURN(ret);
}
/* }}} */


/* We assume that MYSQLND_HEADER_SIZE is 4 bytes !! */
#define STORE_HEADER_SIZE(safe_storage, buffer)  int4store((safe_storage), (*(uint32_t *)(buffer)))
#define RESTORE_HEADER_SIZE(buffer, safe_storage) STORE_HEADER_SIZE((safe_storage), (buffer))

/* {{{ mysqlnd_stream_write_w_header */
/*
  IMPORTANT : It's expected that buf has place in the beginning for MYSQLND_HEADER_SIZE !!!!
  			  This is done for performance reasons in the caller of this function.
			  Otherwise we will have to do send two TCP packets, or do new alloc and memcpy.
			  Neither are quick, thus the clients of this function are obligated to do
			  what they are asked for.

  `count` is actually the length of the payload data. Thus :
  count + MYSQLND_HEADER_SIZE = sizeof(buf) (not the pointer but the actual buffer)
*/
size_t mysqlnd_stream_write_w_header(MYSQLND * const conn, char * const buf, size_t count TSRMLS_DC)
{
	zend_uchar safe_buf[((MYSQLND_HEADER_SIZE) + (sizeof(zend_uchar)) - 1) / (sizeof(zend_uchar))];
	zend_uchar *safe_storage = safe_buf;
	MYSQLND_NET *net = &conn->net;
	size_t old_chunk_size = net->stream->chunk_size;
	size_t ret, left = count, packets_sent = 1;
	zend_uchar *p = (zend_uchar *) buf;
	zend_uchar * compress_buf = NULL;
	size_t comp_buf_size = 0;

	DBG_ENTER("mysqlnd_stream_write_w_header");
	DBG_INF_FMT("conn=%llu count=%lu compression=%d", conn->thread_id, count, net->compressed);

	net->stream->chunk_size = MYSQLND_MAX_PACKET_SIZE;

	if (net->compressed == TRUE) {
		comp_buf_size = MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE + MIN(left, MYSQLND_MAX_PACKET_SIZE);
		DBG_INF_FMT("compress_buf_size=%d", comp_buf_size);
		compress_buf = emalloc(comp_buf_size);
	}

	while (left > MYSQLND_MAX_PACKET_SIZE) {
#ifdef MYSQLND_COMPRESSION_ENABLED

		if (net->compressed == TRUE) {

			/* here we need to compress the data and then write it, first comes the compressed header */
			uLong tmp_complen = MYSQLND_MAX_PACKET_SIZE;
			size_t payload_size;
			zend_uchar * uncompressed_payload = p; /* should include the header */
			int res;

			STORE_HEADER_SIZE(safe_storage, uncompressed_payload);
			int3store(uncompressed_payload, MYSQLND_MAX_PACKET_SIZE);
			int1store(uncompressed_payload + 3, net->packet_no);

			DBG_INF_FMT("compress(%p, %p, %p, %d)", (compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), &tmp_complen, p, MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE);
			res = compress((compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), &tmp_complen, uncompressed_payload, MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE);
			if (res == Z_OK) {
				DBG_INF_FMT("compression successful. compressed size=%d", tmp_complen);
				int3store(compress_buf + MYSQLND_HEADER_SIZE, MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE);
				payload_size = tmp_complen;
			} else {
				DBG_INF_FMT("compression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", res, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
				int3store(compress_buf + MYSQLND_HEADER_SIZE, 0);
				memcpy(compress_buf + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, uncompressed_payload, MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE);
				payload_size = MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE;
			}
			RESTORE_HEADER_SIZE(uncompressed_payload, safe_storage);

			int3store(compress_buf, payload_size);
			int1store(compress_buf + 3, net->packet_no);
			DBG_INF_FMT("writing %d bytes to the network", payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE);
			ret = conn->net.stream_write(conn, compress_buf, payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE TSRMLS_CC);
			net->compressed_envelope_packet_no++;
		} else
#endif /* MYSQLND_COMPRESSION_ENABLED */
		{
			DBG_INF("no compression");
			STORE_HEADER_SIZE(safe_storage, p);
			int3store(p, MYSQLND_MAX_PACKET_SIZE);
			int1store(p + 3, net->packet_no);
			ret = conn->net.stream_write(conn, p, MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE TSRMLS_CC);
			RESTORE_HEADER_SIZE(p, safe_storage);
			net->compressed_envelope_packet_no++;
		}
		net->packet_no++;

		p += MYSQLND_MAX_PACKET_SIZE;
		left -= MYSQLND_MAX_PACKET_SIZE;

		packets_sent++;
	}

	DBG_INF_FMT("packet_size=%d packet_no=%d", left, net->packet_no);
	/* Even for zero size payload we have to send a packet */

#ifdef MYSQLND_COMPRESSION_ENABLED
	if (net->compressed == TRUE) {
		/* here we need to compress the data and then write it, first comes the compressed header */
		uLong tmp_complen = left;
		size_t payload_size;
		zend_uchar * uncompressed_payload = p; /* should include the header */
		int res = Z_BUF_ERROR;

		STORE_HEADER_SIZE(safe_storage, uncompressed_payload);
		int3store(uncompressed_payload, left);
		int1store(uncompressed_payload + 3, net->packet_no);
		if ((left + MYSQLND_HEADER_SIZE) > MYSQLND_MIN_COMPRESS_LEN) {
			DBG_INF_FMT("compress(%p, %p, %p, %d)", (compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), &tmp_complen, p, left + MYSQLND_HEADER_SIZE);
			res = compress((compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), &tmp_complen, uncompressed_payload, left + MYSQLND_HEADER_SIZE);
			if (res == Z_OK) {
				DBG_INF_FMT("compression successful. compressed size=%d", tmp_complen);
				int3store(compress_buf + MYSQLND_HEADER_SIZE, left + MYSQLND_HEADER_SIZE);
				payload_size = tmp_complen;
			}
		} else {
			DBG_INF("Too short for compression");
		}
		if (res != Z_OK) {
			DBG_INF_FMT("compression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", res, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
			int3store(compress_buf + MYSQLND_HEADER_SIZE, 0);
			memcpy(compress_buf + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, uncompressed_payload, left + MYSQLND_HEADER_SIZE);
			payload_size = left + MYSQLND_HEADER_SIZE;
		}
		RESTORE_HEADER_SIZE(uncompressed_payload, safe_storage);

		int3store(compress_buf, payload_size);
		int1store(compress_buf + 3, net->packet_no);
		DBG_INF_FMT("writing %d bytes to the network", payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE);
		ret = conn->net.stream_write(conn, compress_buf, payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE TSRMLS_CC);

		net->compressed_envelope_packet_no++;
  #if WHEN_WE_NEED_TO_CHECK_WHETHER_COMPRESSION_WORKS_CORRECTLY
		if (res == Z_OK) {
			size_t decompressed_size = left + MYSQLND_HEADER_SIZE;
			uLongf tmp_complen2 = decompressed_size;
			zend_uchar * decompressed_data = malloc(decompressed_size);
			int error = uncompress(decompressed_data, &tmp_complen2, compress_buf + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, payload_size);
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
			free(decompressed_data);
		}
  #endif /* WHEN_WE_NEED_TO_CHECK_WHETHER_COMPRESSION_WORKS_CORRECTLY */

	} else
#endif /* MYSQLND_COMPRESSION_ENABLED */
	{
		DBG_INF("no compression");
		STORE_HEADER_SIZE(safe_storage, p);
		int3store(p, left);
		int1store(p + 3, net->packet_no);
		ret = conn->net.stream_write(conn, p, left + MYSQLND_HEADER_SIZE TSRMLS_CC);
		RESTORE_HEADER_SIZE(p, safe_storage);
	}
	net->packet_no++;

	if (!ret) {
		DBG_ERR_FMT("Can't %u send bytes", count);
		conn->state = CONN_QUIT_SENT;
		SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
	}

	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(&conn->stats,
			STAT_BYTES_SENT, count + packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PROTOCOL_OVERHEAD_OUT, packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PACKETS_SENT, packets_sent);

	net->stream->chunk_size = old_chunk_size;
	if (compress_buf) {
		efree(compress_buf);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_read_from_stream */
enum_func_status
mysqlnd_read_from_stream(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC)
{
	size_t to_read = count, ret;
	size_t old_chunk_size = conn->net.stream->chunk_size;
	DBG_ENTER("mysqlnd_read_from_stream");
	DBG_INF_FMT("count=%u", count);
	conn->net.stream->chunk_size = MIN(to_read, conn->options.net_read_buffer_size);
	while (to_read) {
		if (!(ret = php_stream_read(conn->net.stream, (char *) buffer, to_read))) {
			DBG_ERR_FMT("Error while reading header from socket");
			DBG_RETURN(FAIL);
		}
		buffer += ret;
		to_read -= ret;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(&conn->stats, STAT_BYTES_RECEIVED, count);
	conn->net.stream->chunk_size = old_chunk_size;
	DBG_RETURN(PASS);
}
/* }}} */


#ifdef MYSQLND_COMPRESSION_ENABLED
/* {{{ mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer */
static enum_func_status
mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer(MYSQLND * conn, size_t net_payload_size TSRMLS_DC)
{
	MYSQLND_NET * net = &conn->net;
	size_t decompressed_size;
	enum_func_status ret = PASS;
	zend_uchar * compressed_data = NULL;
	zend_uchar comp_header[COMPRESSED_HEADER_SIZE];
	DBG_ENTER("mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer");

	/* Read the compressed header */
	if (FAIL == conn->net.stream_read(conn, comp_header, COMPRESSED_HEADER_SIZE TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}
	decompressed_size = uint3korr(comp_header);

	/* When decompressed_size is 0, then the data is not compressed, and we have wasted 3 bytes */
	/* we need to decompress the data */

	if (decompressed_size) {
		int error;
		uLongf tmp_complen = decompressed_size;
		compressed_data = emalloc(net_payload_size);
		if (FAIL == conn->net.stream_read(conn, compressed_data, net_payload_size TSRMLS_CC)) {
			ret = FAIL;
			goto end;
		}

		net->uncompressed_data = mysqlnd_create_read_buffer(decompressed_size TSRMLS_CC);
		error = uncompress(net->uncompressed_data->data, &tmp_complen, compressed_data, net_payload_size);

		DBG_INF_FMT("compressed data: decomp_len=%d compressed_size=%d", decompressed_size, net_payload_size);
		if (error != Z_OK) {
			DBG_ERR_FMT("Can't uncompress packet, error: %d", error);
			ret = FAIL;
			goto end;
		}
	} else {
		DBG_INF_FMT("The server decided not to compress the data. Our job is easy. Copying %u bytes", net_payload_size);
		net->uncompressed_data = mysqlnd_create_read_buffer(net_payload_size TSRMLS_CC);
		if (FAIL == conn->net.stream_read(conn, net->uncompressed_data->data, net_payload_size TSRMLS_CC)) {
			ret = FAIL;
			goto end;
		}
	}
end:
	if (compressed_data) {
		efree(compressed_data);
	}
	DBG_RETURN(ret);
}
#endif /* MYSQLND_COMPRESSION_ENABLED */


/* {{{ mysqlnd_real_read */
static enum_func_status
mysqlnd_real_read(MYSQLND * conn, zend_uchar * buffer, size_t count TSRMLS_DC)
{
	size_t to_read = count;
	zend_uchar * p = buffer;

	DBG_ENTER("mysqlnd_real_read");
#ifdef MYSQLND_COMPRESSION_ENABLED
	if (conn->net.compressed) {
		MYSQLND_NET * net = &conn->net;
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

			if (FAIL == conn->net.stream_read(conn, net_header, MYSQLND_HEADER_SIZE TSRMLS_CC)) {
				DBG_RETURN(FAIL);
			}
			net_payload_size = uint3korr(net_header);
			packet_no = uint1korr(net_header + 3);
			if (net->compressed_envelope_packet_no != packet_no) {
				DBG_ERR_FMT("Transport level: packets out of order. Expected %d received %d. Packet size=%d",
							net->compressed_envelope_packet_no, packet_no, net_payload_size);

				php_error(E_WARNING, "Packets out of order. Expected %d received %d. Packet size="MYSQLND_SZ_T_SPEC,
						  net->compressed_envelope_packet_no, packet_no, net_payload_size);
#if 0
				*(int *) NULL = 0;
#endif
				DBG_RETURN(FAIL);
			}
			net->compressed_envelope_packet_no++;
#ifdef MYSQLND_DUMP_HEADER_N_BODY
			DBG_INF_FMT("HEADER: hwd_packet_no=%d size=%3d", packet_no, net_payload_size);
#endif
			/* Now let's read from the wire, decompress it and fill the read buffer */
			mysqlnd_read_compressed_packet_from_stream_and_fill_read_buffer(conn, net_payload_size TSRMLS_CC);

			/*
			  Now a bit of recursion - read from the read buffer,
			  if the data which we have just read from the wire
			  is not enough, then the recursive call will try to
			  satisfy it until it is satisfied.
			*/
			DBG_RETURN(mysqlnd_real_read(conn, p, to_read TSRMLS_CC));
		}
		DBG_RETURN(PASS);
	}
#endif /* MYSQLND_COMPRESSION_ENABLED */
	DBG_RETURN(conn->net.stream_read(conn, p, to_read TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_read_header */
static enum_func_status
mysqlnd_read_header(MYSQLND * conn, mysqlnd_packet_header * header TSRMLS_DC)
{
	MYSQLND_NET *net = &conn->net;
	zend_uchar buffer[MYSQLND_HEADER_SIZE];

	DBG_ENTER("mysqlnd_read_header_name");
	DBG_INF_FMT("compressed=%d conn_id=%u", net->compressed, conn->thread_id);
	if (FAIL == mysqlnd_real_read(conn, buffer, MYSQLND_HEADER_SIZE TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}

	header->size = uint3korr(buffer);
	header->packet_no = uint1korr(buffer + 3);

#ifdef MYSQLND_DUMP_HEADER_N_BODY
	DBG_INF_FMT("HEADER: prot_packet_no=%d size=%3d", header->packet_no, header->size);
#endif
	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(&conn->stats,
							STAT_BYTES_RECEIVED, MYSQLND_HEADER_SIZE,
							STAT_PROTOCOL_OVERHEAD_IN, MYSQLND_HEADER_SIZE,
							STAT_PACKETS_RECEIVED, 1);

	if (net->compressed || net->packet_no == header->packet_no) {
		/*
		  Have to increase the number, so we can send correct number back. It will
		  round at 255 as this is unsigned char. The server needs this for simple
		  flow control checking.
		*/
		net->packet_no++;
		DBG_RETURN(PASS);
	}

	DBG_ERR_FMT("Logical link: packets out of order. Expected %d received %d. Packet size=%d",
				net->packet_no, header->packet_no, header->size);

	php_error(E_WARNING, "Packets out of order. Expected %d received %d. Packet size="MYSQLND_SZ_T_SPEC,
			  net->packet_no, header->packet_no, header->size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_read_body */
static enum_func_status
mysqlnd_read_body(MYSQLND *conn, mysqlnd_packet_header * header, zend_uchar * store_buf TSRMLS_DC)
{
	MYSQLND_NET *net = &conn->net;

	DBG_ENTER(mysqlnd_read_body_name);
	DBG_INF_FMT("chunk_size=%d compression=%d", net->stream->chunk_size, net->compressed);

	DBG_RETURN(mysqlnd_real_read(conn, store_buf, header->size TSRMLS_CC));
}
/* }}} */


/* {{{ php_mysqlnd_greet_read */
static enum_func_status
php_mysqlnd_greet_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	zend_uchar buf[2048];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	php_mysql_packet_greet *packet= (php_mysql_packet_greet *) _packet;

	DBG_ENTER("php_mysqlnd_greet_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "greeting", PROT_GREET_PACKET);

	packet->protocol_version = uint1korr(p);
	p++;

	if (packet->protocol_version == 0xFF) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		/*
		  The server doesn't send sqlstate in the greet packet.
		  It's a bug#26426 , so we have to set it correctly ourselves.
		  It's probably "Too many connections, which has SQL state 08004".
		*/
		if (packet->error_no == 1040) {
			memcpy(packet->sqlstate, "08004", MYSQLND_SQLSTATE_LENGTH);
		}
		DBG_RETURN(PASS);
	}

	packet->server_version = estrdup((char *)p);
	p+= strlen(packet->server_version) + 1; /* eat the '\0' */

	packet->thread_id = uint4korr(p);
	p+=4;

	memcpy(packet->scramble_buf, p, SCRAMBLE_LENGTH_323);
	p+= 8;

	/* pad1 */
	p++;

	packet->server_capabilities = uint2korr(p);
	p+= 2;

	packet->charset_no = uint1korr(p);
	p++;

	packet->server_status = uint2korr(p);
	p+= 2;

	/* pad2 */
	p+= 13;

	if (p - buf < packet->header.size) {
		/* scramble_buf is split into two parts */
		memcpy(packet->scramble_buf + SCRAMBLE_LENGTH_323,
				p, SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323);
	} else {
		packet->pre41 = TRUE;
	}

	DBG_INF_FMT("proto=%d server=%s thread_id=%ld",
				packet->protocol_version, packet->server_version, packet->thread_id);

	DBG_INF_FMT("server_capabilities=%d charset_no=%d server_status=%d",
				packet->server_capabilities, packet->charset_no, packet->server_status);

	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("GREET packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "GREET packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_greet_free_mem */
static
void php_mysqlnd_greet_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_greet *p= (php_mysql_packet_greet *) _packet;
	if (p->server_version) {
		mnd_efree(p->server_version);
		p->server_version = NULL;
	}
	if (!alloca) {
		mnd_efree(p);
	}
}
/* }}} */


#define MYSQLND_CAPABILITIES (CLIENT_LONG_PASSWORD | CLIENT_LONG_FLAG | CLIENT_TRANSACTIONS | \
				CLIENT_PROTOCOL_41 | CLIENT_SECURE_CONNECTION | \
				CLIENT_MULTI_RESULTS)


/* {{{ php_mysqlnd_crypt */
static
void php_mysqlnd_crypt(zend_uchar *buffer, const zend_uchar *s1, const zend_uchar *s2, size_t len)
{
	const zend_uchar *s1_end = s1 + len;
	while (s1 < s1_end) {
		*buffer++= *s1++ ^ *s2++;
	}
}
/* }}} */


/* {{{ php_mysqlnd_scramble */
void php_mysqlnd_scramble(zend_uchar * const buffer, const zend_uchar * const scramble,
						  const zend_uchar * const password)
{
	PHP_SHA1_CTX context;
	zend_uchar sha1[SHA1_MAX_LENGTH];
	zend_uchar sha2[SHA1_MAX_LENGTH];


	/* Phase 1: hash password */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, password, strlen((char *)password));
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


/* {{{ php_mysqlnd_auth_write */
static
size_t php_mysqlnd_auth_write(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	char buffer[1024];
	register char *p= buffer + MYSQLND_HEADER_SIZE; /* start after the header */
	int len;
	register php_mysql_packet_auth *packet= (php_mysql_packet_auth *) _packet;

	DBG_ENTER("php_mysqlnd_auth_write");

	packet->client_flags |= MYSQLND_CAPABILITIES;

	if (packet->db) {
		packet->client_flags |= CLIENT_CONNECT_WITH_DB;
	}

	if (PG(open_basedir) && strlen(PG(open_basedir))) {
		packet->client_flags ^= CLIENT_LOCAL_FILES;
	}

	int4store(p, packet->client_flags);
	p+= 4;

	int4store(p, packet->max_packet_size);
	p+= 4;

	int1store(p, packet->charset_no);
	p++;

	memset(p, 0, 23); /* filler */
	p+= 23;

	len= strlen(packet->user);
	memcpy(p, packet->user, len);
	p+= len;
	*p++ = '\0';

	/* copy scrambled pass*/
	if (packet->password && packet->password[0]) {
		/* In 4.1 we use CLIENT_SECURE_CONNECTION and thus the len of the buf should be passed */
		int1store(p, 20);
		p++;
		php_mysqlnd_scramble((zend_uchar*)p, packet->server_scramble_buf, (zend_uchar*)packet->password);
		p+= 20;
	} else {
		/* Zero length */
		int1store(p, 0);
		p++;
	}

	if (packet->db) {
		memcpy(p, packet->db, packet->db_len);
		p+= packet->db_len;
		*p++= '\0';
	}
	/* Handle CLIENT_CONNECT_WITH_DB */
	/* no \0 for no DB */

	DBG_RETURN(mysqlnd_stream_write_w_header(conn, buffer, p - buffer - MYSQLND_HEADER_SIZE TSRMLS_CC));
}
/* }}} */


/* {{{ php_mysqlnd_auth_free_mem */
static
void php_mysqlnd_auth_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	if (!alloca) {
		mnd_pefree((php_mysql_packet_auth *) _packet, ((php_mysql_packet_auth *)_packet)->header.persistent);
	}
}
/* }}} */


#define OK_BUFFER_SIZE 2048

/* {{{ php_mysqlnd_ok_read */
static enum_func_status
php_mysqlnd_ok_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	zend_uchar local_buf[OK_BUFFER_SIZE];
	size_t buf_len = conn->net.cmd_buffer.buffer? conn->net.cmd_buffer.length : OK_BUFFER_SIZE;
	zend_uchar *buf = conn->net.cmd_buffer.buffer? (zend_uchar *) conn->net.cmd_buffer.buffer : local_buf;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned long i;
	register php_mysql_packet_ok *packet= (php_mysql_packet_ok *) _packet;

	DBG_ENTER("php_mysqlnd_ok_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "OK", PROT_OK_PACKET);

	/* Should be always 0x0 or 0xFF for error */
	packet->field_count = uint1korr(p);
	p++;

	if (0xFF == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}
	/* Everything was fine! */
	packet->affected_rows  = php_mysqlnd_net_field_length_ll(&p);
	packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);

	packet->server_status = uint2korr(p);
	p+= 2;

	packet->warning_count = uint2korr(p);
	p+= 2;

	/* There is a message */
	if (packet->header.size > p - buf && (i = php_mysqlnd_net_field_length(&p))) {
		packet->message = estrndup((char *)p, MIN(i, buf_len - (p - begin)));
		packet->message_len = i;
	} else {
		packet->message = NULL;
	}

	DBG_INF_FMT("OK packet: aff_rows=%lld last_ins_id=%ld server_status=%d warnings=%d",
				packet->affected_rows, packet->last_insert_id, packet->server_status,
				packet->warning_count);

	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "OK packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_ok_free_mem */
static
void php_mysqlnd_ok_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_ok *p= (php_mysql_packet_ok *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_eof_read */
static enum_func_status
php_mysqlnd_eof_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/*
	  EOF packet is since 4.1 five bytes long,
	  but we can get also an error, make it bigger.

	  Error : error_code + '#' + sqlstate + MYSQLND_ERRMSG_SIZE
	*/
	php_mysql_packet_eof *packet= (php_mysql_packet_eof *) _packet;
	size_t buf_len = conn->net.cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;

	DBG_ENTER("php_mysqlnd_eof_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "EOF", PROT_EOF_PACKET);

	/* Should be always 0xFE */
	packet->field_count = uint1korr(p);
	p++;

	if (0xFF == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	/*
		4.1 sends 1 byte EOF packet after metadata of
		PREPARE/EXECUTE but 5 bytes after the result. This is not
		according to the Docs@Forge!!!
	*/
	if (packet->header.size > 1) {
		packet->warning_count = uint2korr(p);
		p+= 2;
		packet->server_status = uint2korr(p);
		p+= 2;
	} else {
		packet->warning_count = 0;
		packet->server_status = 0;
	}

	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("EOF packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "EOF packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	}

	DBG_INF_FMT("EOF packet: fields=%d status=%d warnings=%d",
				packet->field_count, packet->server_status, packet->warning_count);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_eof_free_mem */
static
void php_mysqlnd_eof_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	if (!alloca) {
		mnd_pefree(_packet, ((php_mysql_packet_eof *)_packet)->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_cmd_write */
size_t php_mysqlnd_cmd_write(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* Let's have some space, which we can use, if not enough, we will allocate new buffer */
	php_mysql_packet_command *packet= (php_mysql_packet_command *) _packet;
	MYSQLND_NET *net = &conn->net;
	unsigned int error_reporting = EG(error_reporting);
	size_t written;

	DBG_ENTER("php_mysqlnd_cmd_write");
	/*
	  Reset packet_no, or we will get bad handshake!
	  Every command starts a new TX and packet numbers are reset to 0.
	*/
	net->packet_no = 0;
	net->compressed_envelope_packet_no = 0; /* this is for the response */

	if (error_reporting) {
		EG(error_reporting) = 0;
	}

	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_PACKETS_SENT_CMD);

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	php_mysqlnd_consume_uneaten_data(conn, packet->command TSRMLS_CC);
#endif

	if (!packet->argument || !packet->arg_len) {
		char buffer[MYSQLND_HEADER_SIZE + 1];

		int1store(buffer + MYSQLND_HEADER_SIZE, packet->command);
		written = mysqlnd_stream_write_w_header(conn, buffer, 1 TSRMLS_CC);
	} else {
		size_t tmp_len = packet->arg_len + 1 + MYSQLND_HEADER_SIZE, ret;
		zend_uchar *tmp, *p;
		tmp = (tmp_len > net->cmd_buffer.length)? mnd_emalloc(tmp_len):net->cmd_buffer.buffer;
		p = tmp + MYSQLND_HEADER_SIZE; /* skip the header */

		int1store(p, packet->command);
		p++;

		memcpy(p, packet->argument, packet->arg_len);

		ret = mysqlnd_stream_write_w_header(conn, (char *)tmp, tmp_len - MYSQLND_HEADER_SIZE TSRMLS_CC);
		if (tmp != net->cmd_buffer.buffer) {
			MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_CMD_BUFFER_TOO_SMALL);
			mnd_efree(tmp);
		}
		written = ret;
	}
	if (error_reporting) {
		/* restore error reporting */
		EG(error_reporting) = error_reporting;
	}
	DBG_RETURN(written);
}
/* }}} */


/* {{{ php_mysqlnd_cmd_free_mem */
static
void php_mysqlnd_cmd_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	if (!alloca) {
		mnd_pefree(_packet, ((php_mysql_packet_command *)_packet)->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_read */
static enum_func_status
php_mysqlnd_rset_header_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	size_t buf_len = conn->net.cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	size_t len;
	php_mysql_packet_rset_header *packet= (php_mysql_packet_rset_header *) _packet;

	DBG_ENTER("php_mysqlnd_rset_header_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "resultset header", PROT_RSET_HEADER_PACKET);

	/*
	  Don't increment. First byte is 0xFF on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/
	if (*p == 0xFF) {
		/* Error */
		p++;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	packet->field_count= php_mysqlnd_net_field_length(&p);
	switch (packet->field_count) {
		case MYSQLND_NULL_LENGTH:
			DBG_INF("LOAD LOCAL");
			/*
			  First byte in the packet is the field count.
			  Thus, the name is size - 1. And we add 1 for a trailing \0.
			*/
			len = packet->header.size - 1;
			packet->info_or_local_file = mnd_emalloc(len + 1);
			memcpy(packet->info_or_local_file, p, len);
			packet->info_or_local_file[len] = '\0';
			packet->info_or_local_file_len = len;
			break;
		case 0x00:
			DBG_INF("UPSERT");
			packet->affected_rows = php_mysqlnd_net_field_length_ll(&p);
			packet->last_insert_id= php_mysqlnd_net_field_length_ll(&p);
			packet->server_status = uint2korr(p);
			p+=2;
			packet->warning_count = uint2korr(p);
			p+=2;
			/* Check for additional textual data */
			if (packet->header.size  > (p - buf) && (len = php_mysqlnd_net_field_length(&p))) {
				packet->info_or_local_file = mnd_emalloc(len + 1);
				memcpy(packet->info_or_local_file, p, len);
				packet->info_or_local_file[len] = '\0';
				packet->info_or_local_file_len = len;
			}
			DBG_INF_FMT("affected_rows=%llu last_insert_id=%llu server_status=%d warning_count=%d",
						packet->affected_rows, packet->last_insert_id,
						packet->server_status, packet->warning_count);
			break;
		default:
			DBG_INF("SELECT");
			/* Result set */
			break;
	}
	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("RSET_HEADER packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "GREET packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_free_mem */
static
void php_mysqlnd_rset_header_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	DBG_ENTER("php_mysqlnd_rset_header_free_mem");
	php_mysql_packet_rset_header *p= (php_mysql_packet_rset_header *) _packet;
	if (p->info_or_local_file) {
		mnd_efree(p->info_or_local_file);
		p->info_or_local_file = NULL;
	}
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */

static size_t rset_field_offsets[] =
{
	STRUCT_OFFSET(MYSQLND_FIELD, catalog),
	STRUCT_OFFSET(MYSQLND_FIELD, catalog_length),
	STRUCT_OFFSET(MYSQLND_FIELD, db),
	STRUCT_OFFSET(MYSQLND_FIELD, db_length),
	STRUCT_OFFSET(MYSQLND_FIELD, table),
	STRUCT_OFFSET(MYSQLND_FIELD, table_length),
	STRUCT_OFFSET(MYSQLND_FIELD, org_table),
	STRUCT_OFFSET(MYSQLND_FIELD, org_table_length),
	STRUCT_OFFSET(MYSQLND_FIELD, name),
	STRUCT_OFFSET(MYSQLND_FIELD, name_length),
	STRUCT_OFFSET(MYSQLND_FIELD, org_name),
	STRUCT_OFFSET(MYSQLND_FIELD, org_name_length)
};


/* {{{ php_mysqlnd_rset_field_read */
static enum_func_status
php_mysqlnd_rset_field_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* Should be enough for the metadata of a single row */
	php_mysql_packet_res_field *packet= (php_mysql_packet_res_field *) _packet;
	size_t buf_len = conn->net.cmd_buffer.length, total_len = 0;
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	char *root_ptr;
	unsigned long len;
	MYSQLND_FIELD *meta;
	unsigned int i, field_count = sizeof(rset_field_offsets)/sizeof(size_t);

	DBG_ENTER("php_mysqlnd_rset_field_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "field", PROT_RSET_FLD_PACKET);

	if (packet->skip_parsing) {
		DBG_RETURN(PASS);
	}
	if (*p == 0xFF) {
		/* Error */
		p++;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_ERR_FMT("Server error : (%d) %s", packet->error_info.error_no, packet->error_info.error);
		DBG_RETURN(PASS);
	} else if (*p == 0xFE && packet->header.size < 8) {
		/* Premature EOF. That should be COM_FIELD_LIST */
		DBG_INF("Premature EOF. That should be COM_FIELD_LIST");
		packet->stupid_list_fields_eof = TRUE;
		DBG_RETURN(PASS);
	}

	meta = packet->metadata;

	for (i = 0; i < field_count; i += 2) {
		len = php_mysqlnd_net_field_length(&p);
		switch ((len)) {
			case 0:
				*(char **)(((char*)meta) + rset_field_offsets[i]) = mysqlnd_empty_string;
				*(unsigned int *)(((char*)meta) + rset_field_offsets[i+1]) = 0;
				break;
			case MYSQLND_NULL_LENGTH:
				goto faulty_or_fake;
			default:
				*(char **)(((char *)meta) + rset_field_offsets[i]) = (char *)p;
				*(unsigned int *)(((char*)meta) + rset_field_offsets[i+1]) = len;
				p += len;
				total_len += len + 1;
				break;
		}
	}

	/* 1 byte filler */
	p++;

	meta->charsetnr = uint2korr(p);
	p += 2;

	meta->length = uint4korr(p);
	p += 4;

	meta->type = uint1korr(p);
	p += 1;

	meta->flags = uint2korr(p);
	p += 2;

	meta->decimals = uint2korr(p);
	p += 1;

	/* 2 byte filler */
	p +=2;

	/* Should we set NUM_FLAG (libmysql does it) ? */
	if (
		(meta->type <= MYSQL_TYPE_INT24 &&
			(meta->type != MYSQL_TYPE_TIMESTAMP || meta->length == 14 || meta->length == 8)
		) || meta->type == MYSQL_TYPE_YEAR)
	{
		meta->flags |= NUM_FLAG;
	}


	/*
	  def could be empty, thus don't allocate on the root.
	  NULL_LENGTH (0xFB) comes from COM_FIELD_LIST when the default value is NULL.
	  Otherwise the string is length encoded.
	*/
	if (packet->header.size > (p - buf) &&
		(len = php_mysqlnd_net_field_length(&p)) &&
		len != MYSQLND_NULL_LENGTH)
	{
		DBG_INF_FMT("Def found, length %lu", len);
		meta->def = mnd_emalloc(len + 1);
		memcpy(meta->def, p, len);
		meta->def[len] = '\0';
		meta->def_length = len;
		p += len;
	}

	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("RSET field packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result set field packet "MYSQLND_SZ_T_SPEC" bytes "
				 		"shorter than expected", p - begin - packet->header.size);
	}

	root_ptr = meta->root = mnd_emalloc(total_len);
	meta->root_len = total_len;
	/* Now do allocs */
	if (meta->catalog && meta->catalog != mysqlnd_empty_string) {
		len = meta->catalog_length;
		meta->catalog = memcpy(root_ptr, meta->catalog, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->db && meta->db != mysqlnd_empty_string) {
		len = meta->db_length;
		meta->db = memcpy(root_ptr, meta->db, len);
		*(root_ptr + len) = '\0';
	}

	if (meta->table && meta->table != mysqlnd_empty_string) {
		len = meta->table_length;
		meta->table = memcpy(root_ptr, meta->table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_table && meta->org_table != mysqlnd_empty_string) {
		len = meta->org_table_length;
		meta->org_table = memcpy(root_ptr, meta->org_table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->name && meta->name != mysqlnd_empty_string) {
		len = meta->name_length;
		meta->name = memcpy(root_ptr, meta->name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_name && meta->org_name != mysqlnd_empty_string) {
		len = meta->org_name_length;
		meta->org_name = memcpy(root_ptr, meta->org_name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	DBG_INF_FMT("FIELD=[%s.%s.%s]", meta->db? meta->db:"*NA*", meta->table? meta->table:"*NA*",
				meta->name? meta->name:"*NA*");

	DBG_RETURN(PASS);

faulty_or_fake:
	DBG_ERR_FMT("Protocol error. Server sent NULL_LENGTH. The server is faulty");
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Protocol error. Server sent NULL_LENGTH."
					 " The server is faulty");
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_rset_field_free_mem */
static
void php_mysqlnd_rset_field_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_res_field *p= (php_mysql_packet_res_field *) _packet;

	/* p->metadata was passed to us as temporal buffer */
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


static enum_func_status
php_mysqlnd_read_row_ex(MYSQLND *conn, MYSQLND_MEMORY_POOL * result_set_memory_pool,
						MYSQLND_MEMORY_POOL_CHUNK **buffer,
						uint64_t *data_size, zend_bool persistent_alloc,
						unsigned int prealloc_more_bytes TSRMLS_DC)
{
	enum_func_status ret = PASS;
	mysqlnd_packet_header header;
	zend_uchar *p = NULL;
	zend_bool first_iteration = TRUE;

	DBG_ENTER("php_mysqlnd_read_row_ex");

	/*
	  To ease the process the server splits everything in packets up to 2^24 - 1.
	  Even in the case the payload is evenly divisible by this value, the last
	  packet will be empty, namely 0 bytes. Thus, we can read every packet and ask
	  for next one if they have 2^24 - 1 sizes. But just read the header of a
	  zero-length byte, don't read the body, there is no such.
	*/

	*data_size = prealloc_more_bytes;
	while (1) {
		if (FAIL == mysqlnd_read_header(conn , &header TSRMLS_CC)) {
			ret = FAIL;
			break;
		}

		*data_size += header.size;

		if (first_iteration) {
			first_iteration = FALSE;
			/*
			  We need a trailing \0 for the last string, in case of text-mode,
			  to be able to implement read-only variables. Thus, we add + 1.
			*/
			*buffer = result_set_memory_pool->get_chunk(result_set_memory_pool, *data_size + 1 TSRMLS_CC);
			p = (*buffer)->ptr;
		} else if (!first_iteration) {
			/* Empty packet after MYSQLND_MAX_PACKET_SIZE packet. That's ok, break */
			if (!header.size) {
				break;
			}

			/*
			  We have to realloc the buffer.

			  We need a trailing \0 for the last string, in case of text-mode,
			  to be able to implement read-only variables.
			*/
			(*buffer)->resize_chunk((*buffer), *data_size + 1 TSRMLS_CC);
			/* The position could have changed, recalculate */
			p = (*buffer)->ptr + (*data_size - header.size);
		}

		if ((ret = mysqlnd_read_body(conn, &header, p TSRMLS_CC))) {
			DBG_ERR("Empty row packet body");
			php_error(E_WARNING, "Empty row packet body");
			break;
		}

		if (header.size < MYSQLND_MAX_PACKET_SIZE) {
			break;
		}
	}
	if (ret == FAIL && *buffer) {
		(*buffer)->free_chunk((*buffer), TRUE TSRMLS_CC);
		*buffer = NULL;
	}
	*data_size -= prealloc_more_bytes;
	DBG_RETURN(ret);
}


/* {{{ php_mysqlnd_rowp_read_binary_protocol */
void php_mysqlnd_rowp_read_binary_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
										 unsigned int field_count, MYSQLND_FIELD *fields_metadata,
										 zend_bool persistent,
										 zend_bool as_unicode, zend_bool as_int_or_float,
										 MYSQLND_THD_ZVAL_PCACHE * zval_cache,
										 MYSQLND_STATS * stats TSRMLS_DC)
{
	int i;
	zend_uchar *p = row_buffer->ptr;
	zend_uchar *null_ptr, bit;
	zval **current_field, **end_field, **start_field;
#ifdef USE_ZVAL_CACHE
	zend_bool allocated;
	void *obj;
#endif

	DBG_ENTER("php_mysqlnd_rowp_read_binary_protocol");

	end_field = (current_field = start_field = fields) + field_count;


	/* skip the first byte, not 0xFE -> 0x0, status */
	p++;
	null_ptr= p;
	p += (field_count + 9)/8;		/* skip null bits */
	bit	= 4;						/* first 2 bits are reserved */

	for (i = 0; current_field < end_field; current_field++, i++) {
#ifdef USE_ZVAL_CACHE
		DBG_INF("Trying to use the zval cache");
		obj = mysqlnd_palloc_get_zval(zval_cache, &allocated TSRMLS_CC);
		if (allocated) {
			*current_field = (zval *) obj;
		} else {
			/* It's from the cache, so we can upcast here */
			*current_field = &((mysqlnd_zval *) obj)->zv;
			((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_EXT_BUFFER;
		}
#else
		DBG_INF("Directly creating zval");
		MAKE_STD_ZVAL(*current_field);
#endif

		DBG_INF_FMT("Into zval=%p decoding column %d [%s.%s.%s] type=%d field->flags&unsigned=%d flags=%u is_bit=%d as_unicode=%d",
			*current_field, i,
			fields_metadata[i].db, fields_metadata[i].table, fields_metadata[i].name, fields_metadata[i].type,
			fields_metadata[i].flags & UNSIGNED_FLAG, fields_metadata[i].flags, fields_metadata[i].type == MYSQL_TYPE_BIT, as_unicode);
		if (*null_ptr & bit) {
			DBG_INF("It's null");
			ZVAL_NULL(*current_field);
			MYSQLND_INC_CONN_STATISTIC(stats, STAT_BINARY_TYPE_FETCHED_NULL);
		} else {
			enum_mysqlnd_field_types type = fields_metadata[i].type;
			mysqlnd_ps_fetch_functions[type].func(*current_field, &fields_metadata[i],
												  0, &p, as_unicode TSRMLS_CC);

			if (MYSQLND_G(collect_statistics)) {
				enum_mysqlnd_collected_stats statistic;
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_BINARY_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_BINARY_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_BINARY_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_BINARY_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_BINARY_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_BINARY_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_BINARY_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_BINARY_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_BINARY_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_BINARY_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_BINARY_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_BINARY_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_BINARY_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_BINARY_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_BINARY_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_BINARY_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_BINARY_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_BINARY_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_BINARY_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_BINARY_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_BINARY_TYPE_FETCHED_OTHER; break;
				}
				MYSQLND_INC_CONN_STATISTIC(stats, statistic);
			}
		}
		if (!((bit<<=1) & 255)) {
			bit = 1;	/* to the following byte */
			null_ptr++;
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_text_protocol */
void php_mysqlnd_rowp_read_text_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
										 unsigned int field_count, MYSQLND_FIELD *fields_metadata,
										 zend_bool persistent,
										 zend_bool as_unicode, zend_bool as_int_or_float,
										 MYSQLND_THD_ZVAL_PCACHE * zval_cache,
										 MYSQLND_STATS * stats TSRMLS_DC)
{
	int i;
	zend_bool last_field_was_string = FALSE;
	zval **current_field, **end_field, **start_field;
	zend_uchar *p = row_buffer->ptr;
	size_t data_size = row_buffer->app;
	zend_uchar *bit_area = (zend_uchar*) row_buffer->ptr + data_size + 1; /* we allocate from here */

	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol");

	end_field = (current_field = start_field = fields) + field_count;
	for (i = 0; current_field < end_field; current_field++, i++) {
		/* Don't reverse the order. It is significant!*/
		void *obj = NULL;
		zend_bool allocated = TRUE;
		zend_uchar *this_field_len_pos = p;
		/* php_mysqlnd_net_field_length() call should be after *this_field_len_pos = p; */
		unsigned long len = php_mysqlnd_net_field_length(&p);

#ifdef USE_ZVAL_CACHE
		obj = mysqlnd_palloc_get_zval(zval_cache, &allocated TSRMLS_CC);
		if (allocated) {
			*current_field = (zval *) obj;
		} else {
			/* It's from the cache, so we can upcast here */
			*current_field = &((mysqlnd_zval *) obj)->zv;
			((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_FREE;
		}
#else
		DBG_INF("Directly creating zval");
		MAKE_STD_ZVAL(*current_field);
#endif

		if (current_field > start_field && last_field_was_string) {
			/*
			  Normal queries:
			  We have to put \0 now to the end of the previous field, if it was
			  a string. IS_NULL doesn't matter. Because we have already read our
			  length, then we can overwrite it in the row buffer.
			  This statement terminates the previous field, not the current one.

			  NULL_LENGTH is encoded in one byte, so we can stick a \0 there.
			  Any string's length is encoded in at least one byte, so we can stick
			  a \0 there.
			*/

			*this_field_len_pos = '\0';
		}

		/* NULL or NOT NULL, this is the question! */
		if (len == MYSQLND_NULL_LENGTH) {
			ZVAL_NULL(*current_field);
			last_field_was_string = FALSE;
		} else {
#if PHP_MAJOR_VERSION >= 6 || defined(MYSQLND_STRING_TO_INT_CONVERSION)
			struct st_mysqlnd_perm_bind perm_bind =
					mysqlnd_ps_fetch_functions[fields_metadata[i].type];
#endif
			if (MYSQLND_G(collect_statistics)) {
				enum_mysqlnd_collected_stats statistic;
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_TEXT_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_TEXT_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_TEXT_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_TEXT_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_TEXT_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_TEXT_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_TEXT_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_TEXT_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_TEXT_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_TEXT_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_TEXT_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_TEXT_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_TEXT_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_TEXT_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_TEXT_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_TEXT_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_TEXT_TYPE_FETCHED_OTHER; break;
				}
				MYSQLND_INC_CONN_STATISTIC(stats, statistic);
			}

#ifdef MYSQLND_STRING_TO_INT_CONVERSION
			if (as_int_or_float && perm_bind.php_type == IS_LONG &&
				perm_bind.pack_len <= SIZEOF_LONG)
			{
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				if (perm_bind.pack_len < SIZEOF_LONG)
				{
					/* direct conversion */
					int64_t v =
#ifndef PHP_WIN32
						atoll((char *) p);
#else
						_atoi64((char *) p);
#endif
					ZVAL_LONG(*current_field, v);
				} else {
					uint64_t v =
#ifndef PHP_WIN32
						(uint64_t) atoll((char *) p);
#else
						(uint64_t) _atoi64((char *) p);
#endif
					zend_bool uns = fields_metadata[i].flags & UNSIGNED_FLAG? TRUE:FALSE;
					/* We have to make it ASCIIZ temporarily */
#if SIZEOF_LONG==8
					if (uns == TRUE && v > 9223372036854775807L)
#elif SIZEOF_LONG==4
					if ((uns == TRUE && v > L64(2147483647)) ||
						(uns == FALSE && (( L64(2147483647) < (int64_t) v) ||
						(L64(-2147483648) > (int64_t) v))))
#endif /* SIZEOF */
					{
						ZVAL_STRINGL(*current_field, (char *)p, len, 0);
					} else {
						ZVAL_LONG(*current_field, (int64_t)v);
					}
				}
				*(p + len) = save;
			} else if (as_int_or_float && perm_bind.php_type == IS_DOUBLE) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				ZVAL_DOUBLE(*current_field, atof((char *) p));
				*(p + len) = save;
			} else
#endif /* MYSQLND_STRING_TO_INT_CONVERSION */
			if (fields_metadata[i].type == MYSQL_TYPE_BIT) {
				/*
				  BIT fields are specially handled. As they come as bit mask, we have
				  to convert it to human-readable representation. As the bits take
				  less space in the protocol than the numbers they represent, we don't
				  have enough space in the packet buffer to overwrite inside.
				  Thus, a bit more space is pre-allocated at the end of the buffer,
				  see php_mysqlnd_rowp_read(). And we add the strings at the end.
				  Definitely not nice, _hackish_ :(, but works.
				*/
				zend_uchar *start = bit_area;
				ps_fetch_from_1_to_8_bytes(*current_field, &(fields_metadata[i]),
							   			   0, &p, as_unicode, len TSRMLS_CC);
				/*
				  We have advanced in ps_fetch_from_1_to_8_bytes. We should go back because
				  later in this function there will be an advancement.
				*/
				p -= len;
				if (Z_TYPE_PP(current_field) == IS_LONG) {
					bit_area += 1 + sprintf((char *)start, "%ld", Z_LVAL_PP(current_field));
#if PHP_MAJOR_VERSION >= 6
					if (as_unicode) {
						ZVAL_UTF8_STRINGL(*current_field, start, bit_area - start - 1, 0);
					} else
#endif
					{
						ZVAL_STRINGL(*current_field, (char *) start, bit_area - start - 1, 0);
					}
					if (allocated == FALSE) {
						((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_INT_BUFFER;
					}
				} else if (Z_TYPE_PP(current_field) == IS_STRING){
					memcpy(bit_area, Z_STRVAL_PP(current_field), Z_STRLEN_PP(current_field));
					bit_area += Z_STRLEN_PP(current_field);
					*bit_area++ = '\0';
					zval_dtor(*current_field);
#if PHP_MAJOR_VERSION >= 6
					if (as_unicode) {
						ZVAL_UTF8_STRINGL(*current_field, start, bit_area - start - 1, 0);
					} else
#endif
					{
						ZVAL_STRINGL(*current_field, (char *) start, bit_area - start - 1, 0);
					}
					if (allocated == FALSE) {
						((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_INT_BUFFER;
					}
				}
				/*
				  IS_UNICODE should not be specially handled. In unicode mode
				  the buffers are not referenced - everything is copied.
				*/
			} else
#if PHP_MAJOR_VERSION < 6
			{
				ZVAL_STRINGL(*current_field, (char *)p, len, 0);
				if (allocated == FALSE) {
					((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_INT_BUFFER;
				}
			}
#else
			/*
			  Here we have to convert to UTF16, which means not reusing the buffer.
			  Which in turn means that we can free the buffers once we have
			  stored the result set, if we use store_result().

			  Also the destruction of the zvals should not call zval_copy_ctor()
			  because then we will leak.

			  I suppose we can use UG(unicode) in mysqlnd.c/mysqlnd_palloc.c when
			  freeing a result set
			  to check if we need to call copy_ctor().

			  XXX: Keep in mind that up there there is an open `else` in
				   #ifdef MYSQLND_STRING_TO_INT_CONVERSION
				   which will make with this `if` an `else if`.
			*/
			if ((perm_bind.is_possibly_blob == TRUE &&
				 fields_metadata[i].charsetnr == MYSQLND_BINARY_CHARSET_NR) ||
				(!as_unicode && perm_bind.can_ret_as_str_in_uni == TRUE))
			{
				/* BLOB - no conversion please */
				ZVAL_STRINGL(*current_field, (char *)p, len, 0);
			} else {
				ZVAL_UTF8_STRINGL(*current_field, (char *)p, len, 0);
			}
			if (allocated == FALSE) {
				/*
				  The zval cache will check and see that the type is IS_STRING.
				  In this case it will call copy_ctor(). This is valid when
				  allocated == TRUE . In this case we can't upcast. Thus for non-PS
				  point_type doesn't matter much, as the valuable information is
				  in the type of result set. Still good to set it.
				*/
				if (Z_TYPE_P(*current_field) == IS_STRING) {
					((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_INT_BUFFER;
				} else {
					((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_EXT_BUFFER;
				}
			}
#endif
			p += len;
			last_field_was_string = TRUE;
		}
	}
	if (last_field_was_string) {
		/* Normal queries: The buffer has one more byte at the end, because we need it */
		row_buffer->ptr[data_size] = '\0';
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read */
/*
  if normal statements => packet->fields is created by this function,
  if PS => packet->fields is passed from outside
*/
static enum_func_status
php_mysqlnd_rowp_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	MYSQLND_NET *net = &conn->net;
	zend_uchar *p;
	enum_func_status ret = PASS;
	size_t old_chunk_size = net->stream->chunk_size;
	php_mysql_packet_row *packet= (php_mysql_packet_row *) _packet;
	size_t post_alloc_for_bit_fields = 0;
	uint64_t data_size = 0;

	DBG_ENTER("php_mysqlnd_rowp_read");

	if (!packet->binary_protocol && packet->bit_fields_count) {
		/* For every field we need terminating \0 */
		post_alloc_for_bit_fields =
			packet->bit_fields_total_len + packet->bit_fields_count;
	}

	ret = php_mysqlnd_read_row_ex(conn, packet->result_set_memory_pool, &packet->row_buffer, &data_size,
								  packet->persistent_alloc, post_alloc_for_bit_fields
								  TSRMLS_CC);
	if (FAIL == ret) {
		goto end;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(&conn->stats, packet_type_to_statistic_byte_count[PROT_ROW_PACKET],
										MYSQLND_HEADER_SIZE + packet->header.size,
										packet_type_to_statistic_packet_count[PROT_ROW_PACKET],
										1);

	/* packet->row_buffer->ptr is of size 'data_size + 1' */
	packet->header.size = data_size;
	packet->row_buffer->app = data_size;

	if ((*(p = packet->row_buffer->ptr)) == 0xFF) {
		/*
		   Error message as part of the result set,
		   not good but we should not hang. See:
		   Bug #27876 : SF with cyrillic variable name fails during execution
		*/
		ret = FAIL;
		php_mysqlnd_read_error_from_line(p + 1, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
	} else if (*p == 0xFE && data_size < 8) { /* EOF */
		packet->eof = TRUE;
		p++;
		if (data_size > 1) {
			packet->warning_count = uint2korr(p);
			p += 2;
			packet->server_status = uint2korr(p);
			/* Seems we have 3 bytes reserved for future use */
			DBG_INF_FMT("server_status=%d warning_count=%d",
						packet->server_status, packet->warning_count);
		}
	} else {
		MYSQLND_INC_CONN_STATISTIC(&conn->stats,
									packet->binary_protocol? STAT_ROWS_FETCHED_FROM_SERVER_PS:
															 STAT_ROWS_FETCHED_FROM_SERVER_NORMAL);

		packet->eof = FALSE;
		/* packet->field_count is set by the user of the packet */

		if (!packet->skip_extraction) {
			if (!packet->fields) {
				DBG_INF("Allocating packet->fields");
				/*
				  old-API will probably set packet->fields to NULL every time, though for
				  unbuffered sets it makes not much sense as the zvals in this buffer matter,
				  not the buffer. Constantly allocating and deallocating brings nothing.

				  For PS - if stmt_store() is performed, thus we don't have a cursor, it will
				  behave just like old-API buffered. Cursors will behave like a bit different,
				  but mostly like old-API unbuffered and thus will populate this array with
				  value.
				*/
				packet->fields = (zval **) mnd_pecalloc(packet->field_count, sizeof(zval *),
														packet->persistent_alloc);
			}
		} else {
			MYSQLND_INC_CONN_STATISTIC(&conn->stats,
										packet->binary_protocol? STAT_ROWS_SKIPPED_PS:
																 STAT_ROWS_SKIPPED_NORMAL);
		}
	}

end:
	net->stream->chunk_size = old_chunk_size;
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_free_mem */
static
void php_mysqlnd_rowp_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_row *p;

	DBG_ENTER("php_mysqlnd_rowp_free_mem");
	p = (php_mysql_packet_row *) _packet;
	if (p->row_buffer) {
		p->row_buffer->free_chunk(p->row_buffer, TRUE TSRMLS_CC);
		p->row_buffer = NULL;
	}
	DBG_INF_FMT("alloca=%d persistent=%d", (int)alloca, (int)p->header.persistent);
	/*
	  Don't free packet->fields :
	  - normal queries -> store_result() | fetch_row_unbuffered() will transfer
		the ownership and NULL it.
	  - PS will pass in it the bound variables, we have to use them! and of course
		not free the array. As it is passed to us, we should not clean it ourselves.
	*/
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */



/* {{{ php_mysqlnd_stats_read */
static enum_func_status
php_mysqlnd_stats_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	php_mysql_packet_stats *packet= (php_mysql_packet_stats *) _packet;
	size_t buf_len = conn->net.cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;

	DBG_ENTER("php_mysqlnd_stats_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "statistics", PROT_STATS_PACKET);

	packet->message = mnd_emalloc(packet->header.size + 1);
	memcpy(packet->message, buf, packet->header.size);
	packet->message[packet->header.size] = '\0';
	packet->message_len = packet->header.size;

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_stats_free_mem */
static
void php_mysqlnd_stats_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_stats *p= (php_mysql_packet_stats *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* 1 + 4 (id) + 2 (field_c) + 2 (param_c) + 1 (filler) + 2 (warnings ) */
#define PREPARE_RESPONSE_SIZE_41 9
#define PREPARE_RESPONSE_SIZE_50 12

/* {{{ php_mysqlnd_prepare_read */
static enum_func_status
php_mysqlnd_prepare_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* In case of an error, we should have place to put it */
	size_t buf_len = conn->net.cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned int data_size;
	php_mysql_packet_prepare_response *packet= (php_mysql_packet_prepare_response *) _packet;

	DBG_ENTER("php_mysqlnd_prepare_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "prepare", PROT_PREPARE_RESP_PACKET);

	data_size = packet->header.size;
	packet->error_code = uint1korr(p);
	p++;

	if (0xFF == packet->error_code) {
		php_mysqlnd_read_error_from_line(p, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	if (data_size != PREPARE_RESPONSE_SIZE_41 &&
		data_size != PREPARE_RESPONSE_SIZE_50 &&
		!(data_size > PREPARE_RESPONSE_SIZE_50)) {
		DBG_ERR_FMT("Wrong COM_STMT_PREPARE response size. Received %d", data_size);
		php_error(E_WARNING, "Wrong COM_STMT_PREPARE response size. Received %d", data_size);
		DBG_RETURN(FAIL);
	}

	packet->stmt_id = uint4korr(p);
	p += 4;

	/* Number of columns in result set */
	packet->field_count = uint2korr(p);
	p += 2;

	packet->param_count = uint2korr(p);
	p += 2;

	if (data_size > 9) {
		/* 0x0 filler sent by the server for 5.0+ clients */
		p++;

		packet->warning_count = uint2korr(p);
	}

	DBG_INF_FMT("Prepare packet read: stmt_id=%d fields=%d params=%d",
				packet->stmt_id, packet->field_count, packet->param_count);

	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("PREPARE packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PREPARE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_prepare_free_mem */
static
void php_mysqlnd_prepare_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_prepare_response *p= (php_mysql_packet_prepare_response *) _packet;
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_chg_user_read */
static enum_func_status
php_mysqlnd_chg_user_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* There could be an error message */
	size_t buf_len = conn->net.cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	php_mysql_packet_chg_user_resp *packet= (php_mysql_packet_chg_user_resp *) _packet;

	DBG_ENTER("php_mysqlnd_chg_user_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "change user response", PROT_CHG_USER_PACKET);

	/*
	  Don't increment. First byte is 0xFF on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/

	/* Should be always 0x0 or 0xFF for error */
	packet->field_count= uint1korr(p);
	p++;

	if (packet->header.size == 1 && buf[0] == 0xFE &&
		packet->server_capabilities & CLIENT_SECURE_CONNECTION) {
		/* We don't handle 3.23 authentication */
		DBG_RETURN(FAIL);
	}

	if (0xFF == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
	}
	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("CHANGE_USER packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "CHANGE_USER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_chg_user_free_mem */
static
void php_mysqlnd_chg_user_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	if (!alloca) {
		mnd_pefree(_packet, ((php_mysql_packet_chg_user_resp *)_packet)->header.persistent);
	}
}
/* }}} */


/* {{{ packet_methods
 */
mysqlnd_packet_methods packet_methods[PROT_LAST] =
{
	{
		sizeof(php_mysql_packet_greet),
		php_mysqlnd_greet_read,
		NULL, /* write */
		php_mysqlnd_greet_free_mem,
	}, /* PROT_GREET_PACKET */
	{
		sizeof(php_mysql_packet_auth),
		NULL, /* read */
		php_mysqlnd_auth_write,
		php_mysqlnd_auth_free_mem,
	}, /* PROT_AUTH_PACKET */
	{
		sizeof(php_mysql_packet_ok),
		php_mysqlnd_ok_read, /* read */
		NULL, /* write */
		php_mysqlnd_ok_free_mem,
	}, /* PROT_OK_PACKET */
	{
		sizeof(php_mysql_packet_eof),
		php_mysqlnd_eof_read, /* read */
		NULL, /* write */
		php_mysqlnd_eof_free_mem,
	}, /* PROT_EOF_PACKET */
	{
		sizeof(php_mysql_packet_command),
		NULL, /* read */
		php_mysqlnd_cmd_write, /* write */
		php_mysqlnd_cmd_free_mem,
	}, /* PROT_CMD_PACKET */
	{
		sizeof(php_mysql_packet_rset_header),
		php_mysqlnd_rset_header_read, /* read */
		NULL, /* write */
		php_mysqlnd_rset_header_free_mem,
	}, /* PROT_RSET_HEADER_PACKET */
	{
		sizeof(php_mysql_packet_res_field),
		php_mysqlnd_rset_field_read, /* read */
		NULL, /* write */
		php_mysqlnd_rset_field_free_mem,
	}, /* PROT_RSET_FLD_PACKET */
	{
		sizeof(php_mysql_packet_row),
		php_mysqlnd_rowp_read, /* read */
		NULL, /* write */
		php_mysqlnd_rowp_free_mem,
	}, /* PROT_ROW_PACKET */
	{
		sizeof(php_mysql_packet_stats),
		php_mysqlnd_stats_read, /* read */
		NULL, /* write */
		php_mysqlnd_stats_free_mem,
	}, /* PROT_STATS_PACKET */
	{
		sizeof(php_mysql_packet_prepare_response),
		php_mysqlnd_prepare_read, /* read */
		NULL, /* write */
		php_mysqlnd_prepare_free_mem,
	}, /* PROT_PREPARE_RESP_PACKET */
	{
		sizeof(php_mysql_packet_chg_user_resp),
		php_mysqlnd_chg_user_read, /* read */
		NULL, /* write */
		php_mysqlnd_chg_user_free_mem,
	} /* PROT_CHG_USER_PACKET */
};
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
