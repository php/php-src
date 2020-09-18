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
#include "mysqlnd_connection.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_read_buffer.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#ifdef MYSQLND_COMPRESSION_ENABLED
#include <zlib.h>
#endif


/* {{{ mysqlnd_pfc::reset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, reset)(MYSQLND_PFC * const pfc, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("mysqlnd_pfc::reset");
	pfc->data->packet_no = pfc->data->compressed_envelope_packet_no = 0;
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

#ifdef MYSQLND_COMPRESSION_ENABLED
static ssize_t write_compressed_packet(
		const MYSQLND_PFC *pfc, MYSQLND_VIO *vio,
		MYSQLND_STATS *conn_stats, MYSQLND_ERROR_INFO *error_info,
		zend_uchar *uncompressed_payload, size_t to_be_sent, zend_uchar *compress_buf) {
	DBG_ENTER("write_compressed_packet");
	/* here we need to compress the data and then write it, first comes the compressed header */
	size_t tmp_complen = to_be_sent;
	size_t payload_size;
	if (PASS == pfc->data->m.encode((compress_buf + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE), &tmp_complen,
							   uncompressed_payload, to_be_sent))
	{
		int3store(compress_buf + MYSQLND_HEADER_SIZE, to_be_sent);
		payload_size = tmp_complen;
	} else {
		int3store(compress_buf + MYSQLND_HEADER_SIZE, 0);
		memcpy(compress_buf + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, uncompressed_payload, to_be_sent);
		payload_size = to_be_sent;
	}

	int3store(compress_buf, payload_size);
	int1store(compress_buf + 3, pfc->data->compressed_envelope_packet_no);
	DBG_INF_FMT("writing "MYSQLND_SZ_T_SPEC" bytes to the network", payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE);

	ssize_t bytes_sent = vio->data->m.network_write(vio, compress_buf, payload_size + MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE, conn_stats, error_info);
	pfc->data->compressed_envelope_packet_no++;
#ifdef WHEN_WE_NEED_TO_CHECK_WHETHER_COMPRESSION_WORKS_CORRECTLY
	if (res == Z_OK) {
		size_t decompressed_size = left + MYSQLND_HEADER_SIZE;
		zend_uchar * decompressed_data = mnd_malloc(decompressed_size);
		int error = pfc->data->m.decode(decompressed_data, decompressed_size,
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
	DBG_RETURN(bytes_sent);
}
#endif

/* {{{ mysqlnd_pfc::send */
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
MYSQLND_METHOD(mysqlnd_pfc, send)(MYSQLND_PFC * const pfc, MYSQLND_VIO * const vio, zend_uchar * const buffer, const size_t count,
								  MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	zend_uchar safe_buf[((MYSQLND_HEADER_SIZE) + (sizeof(zend_uchar)) - 1) / (sizeof(zend_uchar))];
	zend_uchar * safe_storage = safe_buf;
	size_t packets_sent = 1;
	size_t left = count;
	zend_uchar * p = (zend_uchar *) buffer;
	zend_uchar * compress_buf = NULL;
	size_t to_be_sent;
	ssize_t bytes_sent;

	DBG_ENTER("mysqlnd_pfc::send");
	DBG_INF_FMT("count=" MYSQLND_SZ_T_SPEC " compression=%u", count, pfc->data->compressed);

	if (pfc->data->compressed == TRUE) {
		size_t comp_buf_size = MYSQLND_HEADER_SIZE + COMPRESSED_HEADER_SIZE + MYSQLND_HEADER_SIZE + MIN(left, MYSQLND_MAX_PACKET_SIZE);
		DBG_INF_FMT("compress_buf_size="MYSQLND_SZ_T_SPEC, comp_buf_size);
		compress_buf = mnd_emalloc(comp_buf_size);
	}

	do {
		to_be_sent = MIN(left, MYSQLND_MAX_PACKET_SIZE);
		DBG_INF_FMT("to_be_sent=%u", to_be_sent);
		DBG_INF_FMT("packets_sent=%u", packets_sent);
		DBG_INF_FMT("compressed_envelope_packet_no=%u", pfc->data->compressed_envelope_packet_no);
		DBG_INF_FMT("packet_no=%u", pfc->data->packet_no);
#ifdef MYSQLND_COMPRESSION_ENABLED
		if (pfc->data->compressed == TRUE) {
			zend_uchar * uncompressed_payload = p; /* should include the header */
			STORE_HEADER_SIZE(safe_storage, uncompressed_payload);
			int3store(uncompressed_payload, to_be_sent);
			int1store(uncompressed_payload + 3, pfc->data->packet_no);
			if (to_be_sent <= MYSQLND_MAX_PACKET_SIZE - MYSQLND_HEADER_SIZE) {
				bytes_sent = write_compressed_packet(
					pfc, vio, conn_stats, error_info,
					uncompressed_payload, to_be_sent + MYSQLND_HEADER_SIZE, compress_buf);
			} else {
				/* The uncompressed size including the header would overflow. Split into two
				 * compressed packets. The size of the first one is relatively arbitrary here. */
				const size_t split_off_bytes = 8192;
				bytes_sent = write_compressed_packet(
					pfc, vio, conn_stats, error_info,
					uncompressed_payload, split_off_bytes, compress_buf);
				bytes_sent = write_compressed_packet(
					pfc, vio, conn_stats, error_info,
					uncompressed_payload + split_off_bytes,
					to_be_sent + MYSQLND_HEADER_SIZE - split_off_bytes, compress_buf);
			}
			RESTORE_HEADER_SIZE(uncompressed_payload, safe_storage);
		} else
#endif /* MYSQLND_COMPRESSION_ENABLED */
		{
			DBG_INF("no compression");
			STORE_HEADER_SIZE(safe_storage, p);
			int3store(p, to_be_sent);
			int1store(p + 3, pfc->data->packet_no);
			bytes_sent = vio->data->m.network_write(vio, p, to_be_sent + MYSQLND_HEADER_SIZE, conn_stats, error_info);
			RESTORE_HEADER_SIZE(p, safe_storage);
			pfc->data->compressed_envelope_packet_no++;
		}
		pfc->data->packet_no++;

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
	} while (bytes_sent > 0 && (left > 0 || to_be_sent == MYSQLND_MAX_PACKET_SIZE));

	DBG_INF_FMT("packet_size="MYSQLND_SZ_T_SPEC" packet_no=%u", left, pfc->data->packet_no);

	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(conn_stats,
			STAT_BYTES_SENT, count + packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PROTOCOL_OVERHEAD_OUT, packets_sent * MYSQLND_HEADER_SIZE,
			STAT_PACKETS_SENT, packets_sent);

	if (compress_buf) {
		mnd_efree(compress_buf);
	}

	/* Even for zero size payload we have to send a packet */
	if (bytes_sent <= 0) {
		DBG_ERR_FMT("Can't %u send bytes", count);
		SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
	}
	DBG_RETURN(bytes_sent);
}
/* }}} */


#ifdef MYSQLND_COMPRESSION_ENABLED

/* {{{ mysqlnd_pfc::read_compressed_packet_from_stream_and_fill_read_buffer */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, read_compressed_packet_from_stream_and_fill_read_buffer)
		(MYSQLND_PFC * pfc, MYSQLND_VIO * vio, size_t net_payload_size, MYSQLND_STATS * conn_stats, MYSQLND_ERROR_INFO * error_info)
{
	size_t decompressed_size;
	enum_func_status retval = PASS;
	zend_uchar * compressed_data = NULL;
	zend_uchar comp_header[COMPRESSED_HEADER_SIZE];
	DBG_ENTER("mysqlnd_pfc::read_compressed_packet_from_stream_and_fill_read_buffer");

	/* Read the compressed header */
	if (FAIL == vio->data->m.network_read(vio, comp_header, COMPRESSED_HEADER_SIZE, conn_stats, error_info)) {
		DBG_RETURN(FAIL);
	}
	decompressed_size = uint3korr(comp_header);

	/* When decompressed_size is 0, then the data is not compressed, and we have wasted 3 bytes */
	/* we need to decompress the data */

	if (decompressed_size) {
		compressed_data = mnd_emalloc(net_payload_size);
		if (FAIL == vio->data->m.network_read(vio, compressed_data, net_payload_size, conn_stats, error_info)) {
			retval = FAIL;
			goto end;
		}
		pfc->data->uncompressed_data = mysqlnd_create_read_buffer(decompressed_size);
		retval = pfc->data->m.decode(pfc->data->uncompressed_data->data, decompressed_size, compressed_data, net_payload_size);
		if (FAIL == retval) {
			goto end;
		}
	} else {
		DBG_INF_FMT("The server decided not to compress the data. Our job is easy. Copying %u bytes", net_payload_size);
		pfc->data->uncompressed_data = mysqlnd_create_read_buffer(net_payload_size);
		if (FAIL == vio->data->m.network_read(vio, pfc->data->uncompressed_data->data, net_payload_size, conn_stats, error_info)) {
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


/* {{{ mysqlnd_pfc::decode */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, decode)(zend_uchar * uncompressed_data, const size_t uncompressed_data_len,
									const zend_uchar * const compressed_data, const size_t compressed_data_len)
{
#ifdef MYSQLND_COMPRESSION_ENABLED
	int error;
	uLongf tmp_complen = uncompressed_data_len;
	DBG_ENTER("mysqlnd_pfc::decode");
	error = uncompress(uncompressed_data, &tmp_complen, compressed_data, compressed_data_len);

	DBG_INF_FMT("compressed data: decomp_len=%lu compressed_size="MYSQLND_SZ_T_SPEC, tmp_complen, compressed_data_len);
	if (error != Z_OK) {
		DBG_INF_FMT("decompression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", error, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
	}
	DBG_RETURN(error == Z_OK? PASS:FAIL);
#else
	DBG_ENTER("mysqlnd_pfc::decode");
	DBG_RETURN(FAIL);
#endif
}
/* }}} */


/* {{{ mysqlnd_pfc::encode */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, encode)(zend_uchar * compress_buffer, size_t * compress_buffer_len,
									const zend_uchar * const uncompressed_data, const size_t uncompressed_data_len)
{
#ifdef MYSQLND_COMPRESSION_ENABLED
	int error;
	uLongf tmp_complen = *compress_buffer_len;
	DBG_ENTER("mysqlnd_pfc::encode");
	error = compress(compress_buffer, &tmp_complen, uncompressed_data, uncompressed_data_len);

	if (error != Z_OK) {
		DBG_INF_FMT("compression NOT successful. error=%d Z_OK=%d Z_BUF_ERROR=%d Z_MEM_ERROR=%d", error, Z_OK, Z_BUF_ERROR, Z_MEM_ERROR);
	} else {
		*compress_buffer_len = tmp_complen;
		DBG_INF_FMT("compression successful. compressed size=%lu", tmp_complen);
	}

	DBG_RETURN(error == Z_OK? PASS:FAIL);
#else
	DBG_ENTER("mysqlnd_pfc::encode");
	DBG_RETURN(FAIL);
#endif
}
/* }}} */


/* {{{ mysqlnd_pfc::receive */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, receive)(MYSQLND_PFC * const pfc, MYSQLND_VIO * const vio, zend_uchar * const buffer, const size_t count,
									 MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info)
{
	size_t to_read = count;
	zend_uchar * p = buffer;

	DBG_ENTER("mysqlnd_pfc::receive");
#ifdef MYSQLND_COMPRESSION_ENABLED
	if (pfc->data->compressed) {
		if (pfc->data->uncompressed_data) {
			size_t to_read_from_buffer = MIN(pfc->data->uncompressed_data->bytes_left(pfc->data->uncompressed_data), to_read);
			DBG_INF_FMT("reading "MYSQLND_SZ_T_SPEC" from uncompressed_data buffer", to_read_from_buffer);
			if (to_read_from_buffer) {
				pfc->data->uncompressed_data->read(pfc->data->uncompressed_data, to_read_from_buffer, (zend_uchar *) p);
				p += to_read_from_buffer;
				to_read -= to_read_from_buffer;
			}
			DBG_INF_FMT("left "MYSQLND_SZ_T_SPEC" to read", to_read);
			if (TRUE == pfc->data->uncompressed_data->is_empty(pfc->data->uncompressed_data)) {
				/* Everything was consumed. This should never happen here, but for security */
				pfc->data->uncompressed_data->free_buffer(&pfc->data->uncompressed_data);
			}
		}
		if (to_read) {
			zend_uchar net_header[MYSQLND_HEADER_SIZE];
			size_t net_payload_size;
			zend_uchar packet_no;

			if (FAIL == vio->data->m.network_read(vio, net_header, MYSQLND_HEADER_SIZE, conn_stats, error_info)) {
				DBG_RETURN(FAIL);
			}
			net_payload_size = uint3korr(net_header);
			packet_no = uint1korr(net_header + 3);
			if (pfc->data->compressed_envelope_packet_no != packet_no) {
				DBG_ERR_FMT("Transport level: packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
							pfc->data->compressed_envelope_packet_no, packet_no, net_payload_size);

				php_error(E_WARNING, "Packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
						  pfc->data->compressed_envelope_packet_no, packet_no, net_payload_size);
				DBG_RETURN(FAIL);
			}
			pfc->data->compressed_envelope_packet_no++;
#ifdef MYSQLND_DUMP_HEADER_N_BODY
			DBG_INF_FMT("HEADER: hwd_packet_no=%u size=%3u", packet_no, (zend_ulong) net_payload_size);
#endif
			/* Now let's read from the wire, decompress it and fill the read buffer */
			pfc->data->m.read_compressed_packet_from_stream_and_fill_read_buffer(pfc, vio, net_payload_size, conn_stats, error_info);

			/*
			  Now a bit of recursion - read from the read buffer,
			  if the data which we have just read from the wire
			  is not enough, then the recursive call will try to
			  satisfy it until it is satisfied.
			*/
			DBG_RETURN(pfc->data->m.receive(pfc, vio, p, to_read, conn_stats, error_info));
		}
		DBG_RETURN(PASS);
	}
#endif /* MYSQLND_COMPRESSION_ENABLED */
	DBG_RETURN(vio->data->m.network_read(vio, p, to_read, conn_stats, error_info));
}
/* }}} */


/* {{{ mysqlnd_pfc::set_client_option */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, set_client_option)(MYSQLND_PFC * const pfc, enum_mysqlnd_client_option option, const char * const value)
{
	DBG_ENTER("mysqlnd_pfc::set_client_option");
	DBG_INF_FMT("option=%u", option);
	switch (option) {
		case MYSQL_OPT_COMPRESS:
			pfc->data->flags |= MYSQLND_PROTOCOL_FLAG_USE_COMPRESSION;
			break;
		case MYSQL_SERVER_PUBLIC_KEY: {
			const zend_bool pers = pfc->persistent;
			if (pfc->data->sha256_server_public_key) {
				mnd_pefree(pfc->data->sha256_server_public_key, pers);
			}
			pfc->data->sha256_server_public_key = value? mnd_pestrdup(value, pers) : NULL;
			break;
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
			DBG_INF("MYSQLND_OPT_NET_CMD_BUFFER_SIZE");
			if (*(unsigned int*) value < MYSQLND_NET_CMD_BUFFER_MIN_SIZE) {
				DBG_RETURN(FAIL);
			}
			pfc->cmd_buffer.length = *(unsigned int*) value;
			DBG_INF_FMT("new_length="MYSQLND_SZ_T_SPEC, pfc->cmd_buffer.length);
			if (!pfc->cmd_buffer.buffer) {
				pfc->cmd_buffer.buffer = mnd_pemalloc(pfc->cmd_buffer.length, pfc->persistent);
			} else {
				pfc->cmd_buffer.buffer = mnd_perealloc(pfc->cmd_buffer.buffer, pfc->cmd_buffer.length, pfc->persistent);
			}
			break;
		}
		default:
			DBG_RETURN(FAIL);
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_pfc::free_contents */
static void
MYSQLND_METHOD(mysqlnd_pfc, free_contents)(MYSQLND_PFC * pfc)
{
	DBG_ENTER("mysqlnd_pfc::free_contents");

#ifdef MYSQLND_COMPRESSION_ENABLED
	if (pfc->data->uncompressed_data) {
		pfc->data->uncompressed_data->free_buffer(&pfc->data->uncompressed_data);
	}
#endif
	if (pfc->data->sha256_server_public_key) {
		mnd_pefree(pfc->data->sha256_server_public_key, pfc->persistent);
		pfc->data->sha256_server_public_key = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_pfc::init */
static enum_func_status
MYSQLND_METHOD(mysqlnd_pfc, init)(MYSQLND_PFC * const pfc, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	unsigned int buf_size;
	DBG_ENTER("mysqlnd_pfc::init");

	buf_size = MYSQLND_G(net_cmd_buffer_size); /* this is long, cast to unsigned int*/
	pfc->data->m.set_client_option(pfc, MYSQLND_OPT_NET_CMD_BUFFER_SIZE, (char *) &buf_size);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_pfc::dtor */
static void
MYSQLND_METHOD(mysqlnd_pfc, dtor)(MYSQLND_PFC * const pfc, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info)
{
	DBG_ENTER("mysqlnd_pfc::dtor");
	if (pfc) {
		pfc->data->m.free_contents(pfc);

		if (pfc->cmd_buffer.buffer) {
			DBG_INF("Freeing cmd buffer");
			mnd_pefree(pfc->cmd_buffer.buffer, pfc->persistent);
			pfc->cmd_buffer.buffer = NULL;
		}

		mnd_pefree(pfc, pfc->persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_protocol_packet_frame_codec)
	MYSQLND_METHOD(mysqlnd_pfc, init),
	MYSQLND_METHOD(mysqlnd_pfc, dtor),
	MYSQLND_METHOD(mysqlnd_pfc, reset),

	MYSQLND_METHOD(mysqlnd_pfc, set_client_option),

	MYSQLND_METHOD(mysqlnd_pfc, decode),
	MYSQLND_METHOD(mysqlnd_pfc, encode),

	MYSQLND_METHOD(mysqlnd_pfc, send),
	MYSQLND_METHOD(mysqlnd_pfc, receive),

#ifdef MYSQLND_COMPRESSION_ENABLED
	MYSQLND_METHOD(mysqlnd_pfc, read_compressed_packet_from_stream_and_fill_read_buffer),
#else
	NULL,
#endif

	MYSQLND_METHOD(mysqlnd_pfc, free_contents),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_pfc_init */
PHPAPI MYSQLND_PFC *
mysqlnd_pfc_init(const zend_bool persistent, MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *object_factory, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *factory = object_factory? object_factory : &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory);
	MYSQLND_PFC * pfc;
	DBG_ENTER("mysqlnd_pfc_init");
	pfc = factory->get_protocol_frame_codec(persistent, stats, error_info);
	DBG_RETURN(pfc);
}
/* }}} */


/* {{{ mysqlnd_pfc_free */
PHPAPI void
mysqlnd_pfc_free(MYSQLND_PFC * const pfc, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	DBG_ENTER("mysqlnd_pfc_free");
	if (pfc) {
		pfc->data->m.dtor(pfc, stats, error_info);
	}
	DBG_VOID_RETURN;
}
/* }}} */
