/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2008 The PHP Group                                |
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
#include "ext/standard/sha1.h"
#include "php_network.h"
#include "zend_ini.h"

#ifndef PHP_WIN32
#include <netinet/tcp.h>
#else
#include <winsock.h>
#endif


#define USE_CORK 0

#define MYSQLND_SILENT 1

#define MYSQLND_DUMP_HEADER_N_BODY2
#define MYSQLND_DUMP_HEADER_N_BODY_FULL2


#define	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_size, packet_type) \
	{ \
		if (FAIL == mysqlnd_read_header((conn), &((packet)->header) TSRMLS_CC)) {\
			CONN_SET_STATE(conn, CONN_QUIT_SENT); \
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);\
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysqlnd_server_gone); \
			DBG_ERR_FMT("Can't read %s's header", (packet_type)); \
			DBG_RETURN(FAIL);\
		}\
		if ((buf_size) < (packet)->header.size) { \
			DBG_ERR_FMT("Packet buffer wasn't big enough %u bytes will be unread", \
						(packet)->header.size - (buf_size)); \
		}\
		if (!mysqlnd_read_body((conn), (buf), \
							   MIN((buf_size), (packet)->header.size) TSRMLS_CC)) { \
			CONN_SET_STATE(conn, CONN_QUIT_SENT); \
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);\
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysqlnd_server_gone); \
			DBG_ERR_FMT("Empty %s packet body", (packet_type)); \
			DBG_RETURN(FAIL);\
		} \
	}


extern mysqlnd_packet_methods packet_methods[];

static const char *unknown_sqlstate= "HY000";

char * const mysqlnd_empty_string = "";

/* Used in mysqlnd_debug.c */
char * mysqlnd_read_header_name	= "mysqlnd_read_header";
char * mysqlnd_read_body_name		= "mysqlnd_read_body";


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
uint64 php_mysqlnd_net_field_length_ll(zend_uchar **packet)
{
	register zend_uchar *p= (zend_uchar *)*packet;

	if (*p < 251) {
		(*packet)++;
		return (uint64) *p;
	}

	switch (*p) {
		case 251:
			(*packet)++;
			return (uint64) MYSQLND_NULL_LENGTH;
		case 252:
			(*packet) += 3;
			return (uint64) uint2korr(p + 1);
		case 253:
			(*packet) += 4;
			return (uint64) uint3korr(p + 1);
		default:
			(*packet) += 9;
			return (uint64) uint8korr(p + 1);
	}
}
/* }}} */


/* {{{ php_mysqlnd_net_store_length */
zend_uchar *php_mysqlnd_net_store_length(zend_uchar *packet, uint64 length)
{
	if (length < (uint64) L64(251)) {
		*packet = (zend_uchar) length;
		return packet + 1;
	}

	if (length < (uint64) L64(65536)) {
		*packet++ = 252;
		int2store(packet,(uint) length);
		return packet + 2;
	}

	if (length < (uint64) L64(16777216)) {
		*packet++ = 253;
		int3store(packet,(ulong) length);
		return packet + 3;
	}
	*packet++ = 254;
	int8store(packet, length);
	return packet + 8;
}
/* }}} */


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
							 "consumed all the output from the server. PID=%d",
							 bytes_consumed, mysqlnd_command_to_text[net->last_command], getpid());
		}
	}
	net->last_command = cmd;

	DBG_RETURN(skipped_bytes);
}
#endif
/* }}} */


/* {{{ php_mysqlnd_read_error_from_line */
static
enum_func_status php_mysqlnd_read_error_from_line(zend_uchar *buf, size_t buf_len,
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


/* We assume that MYSQLND_HEADER_SIZE is 4 bytes !! */
#define STORE_HEADER_SIZE(safe_storage, buffer)  int4store((safe_storage), (*(uint32 *)(buffer)))
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

	DBG_ENTER("mysqlnd_stream_write_w_header");
	DBG_INF_FMT("conn=%llu count=%lu", conn->thread_id, count);

	net->stream->chunk_size = MYSQLND_MAX_PACKET_SIZE;

	while (left > MYSQLND_MAX_PACKET_SIZE) {
		STORE_HEADER_SIZE(safe_storage, p);
		int3store(p, MYSQLND_MAX_PACKET_SIZE);
		int1store(p + 3, net->packet_no);		
		net->packet_no++;
		ret = php_stream_write(net->stream, (char *)p, MYSQLND_MAX_PACKET_SIZE + MYSQLND_HEADER_SIZE);
		RESTORE_HEADER_SIZE(p, safe_storage);

		p += MYSQLND_MAX_PACKET_SIZE;
		left -= MYSQLND_MAX_PACKET_SIZE;

		packets_sent++;
	}
	/* Even for zero size payload we have to send a packet */
	STORE_HEADER_SIZE(safe_storage, p);
	int3store(p, left);
	int1store(p + 3, net->packet_no);		
	net->packet_no++;
	ret = php_stream_write(net->stream, (char *)p, left + MYSQLND_HEADER_SIZE);
	RESTORE_HEADER_SIZE(p, safe_storage);

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

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stream_write_w_command */
#if USE_CORK && defined(TCP_CORK)
static
size_t mysqlnd_stream_write_w_command(MYSQLND * const conn, enum php_mysqlnd_server_command command,
									  const char * const buf, size_t count TSRMLS_DC)
{
	zend_uchar safe_buf[((MYSQLND_HEADER_SIZE) + (sizeof(zend_uchar)) - 1) / (sizeof(zend_uchar))];
	zend_uchar *safe_storage = (char *) &safe_buf;
	MYSQLND_NET *net = &conn->net;
	size_t old_chunk_size = net->stream->chunk_size;
	size_t ret, left = count, header_len = MYSQLND_HEADER_SIZE + 1, packets_sent = 1;
	const zend_uchar *p = (zend_uchar *) buf;
	zend_bool command_sent = FALSE;
	int corked = 1;

	DBG_ENTER("mysqlnd_stream_write_w_command");

	net->stream->chunk_size = MYSQLND_MAX_PACKET_SIZE;

	setsockopt(((php_netstream_data_t*)net->stream->abstract)->socket,
				IPPROTO_TCP, TCP_CORK, &corked, sizeof(corked));

	int1store(safe_storage + MYSQLND_HEADER_SIZE, command);		
	while (left > MYSQLND_MAX_PACKET_SIZE) {
		size_t body_size = MYSQLND_MAX_PACKET_SIZE;

		int3store(safe_storage, MYSQLND_MAX_PACKET_SIZE);
		int1store(safe_storage + 3, net->packet_no);		
		net->packet_no++;
		
		ret = php_stream_write(net->stream, (char *)safe_storage, header_len);
		if (command_sent == FALSE) {
			--header_len;
			/* Sent one byte less*/
			--body_size;
			command_sent = TRUE;
		}

		ret = php_stream_write(net->stream, (char *)p, body_size);

		p += body_size;
		left -= body_size;

		packets_sent++;
	}
	/* Even for zero size payload we have to send a packet */
	int3store(safe_storage, header_len == MYSQLND_HEADER_SIZE? left:left+1);
	int1store(safe_storage + 3, net->packet_no);
	net->packet_no++;
	
	ret = php_stream_write(net->stream, (char *)safe_storage, header_len);

	if (left) {
		ret = php_stream_write(net->stream, (char *)p, left);
	}
	corked = 0;
	setsockopt(((php_netstream_data_t*)net->stream->abstract)->socket,
				IPPROTO_TCP, TCP_CORK, &corked, sizeof(corked));

	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(&conn->stats, STAT_BYTES_SENT,
				count + packets_sent * MYSQLND_HEADER_SIZE);
				STAT_PROTOCOL_OVERHEAD_OUT, packets_sent * MYSQLND_HEADER_SIZE);
				STAT_PACKETS_SENT, packets_sent);

	net->stream->chunk_size = old_chunk_size;

	DBG_RETURN(ret);
}
#endif
/* }}} */


/* {{{ mysqlnd_read_header */
static enum_func_status
mysqlnd_read_header(MYSQLND *conn, mysqlnd_packet_header *header TSRMLS_DC)
{
	MYSQLND_NET *net = &conn->net;
	char buffer[MYSQLND_HEADER_SIZE];
	char *p = buffer;
	int to_read = MYSQLND_HEADER_SIZE, ret;

	DBG_ENTER(mysqlnd_read_header_name);

	do {
		if (!(ret= php_stream_read(net->stream, p, to_read))) {
			DBG_ERR_FMT("Error while reading header from socket");
			return FAIL;
		}
		p += ret;
		to_read -= ret;
	} while (to_read);

	header->size = uint3korr(buffer);
	header->packet_no = uint1korr(buffer + 3);

	MYSQLND_INC_CONN_STATISTIC_W_VALUE3(&conn->stats,
							STAT_BYTES_RECEIVED, MYSQLND_HEADER_SIZE,
							STAT_PROTOCOL_OVERHEAD_IN, MYSQLND_HEADER_SIZE,
							STAT_PACKETS_RECEIVED, 1);

	if (net->packet_no == header->packet_no) {
		/*
		  Have to increase the number, so we can send correct number back. It will
		  round at 255 as this is unsigned char. The server needs this for simple
		  flow control checking.
		*/
		net->packet_no++;
#ifdef MYSQLND_DUMP_HEADER_N_BODY
		DBG_ERR_FMT("HEADER: packet_no=%d size=%3d", header->packet_no, header->size);
#endif
		DBG_RETURN(PASS);
	}

#if !MYSQLND_SILENT
	DBG_ERR_FMT("Packets out of order. Expected %d received %d. Packet size=%d",
				net->packet_no, header->packet_no, header->size);
#endif
	php_error(E_WARNING, "Packets out of order. Expected %d received %d. Packet size=%d. PID=%d",
			  net->packet_no, header->packet_no, header->size, getpid());
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_read_body */
static
size_t mysqlnd_read_body(MYSQLND *conn, zend_uchar *buf, size_t size TSRMLS_DC)
{
	size_t ret;
	char *p = (char *)buf;
	unsigned int iter = 0;
	MYSQLND_NET *net = &conn->net;
	size_t old_chunk_size = net->stream->chunk_size;

	DBG_ENTER(mysqlnd_read_body_name);
	DBG_INF_FMT("chunk_size=%d", net->stream->chunk_size);

	net->stream->chunk_size = MIN(size, conn->options.net_read_buffer_size);
	do {
		size -= (ret = php_stream_read(net->stream, p, size));
		if (size > 0 || iter++) {
			DBG_INF_FMT("read=%d buf=%p p=%p chunk_size=%d left=%d",
						ret, buf, p , net->stream->chunk_size, size);
		}
		p += ret;
	} while (size > 0);

	MYSQLND_INC_CONN_STATISTIC_W_VALUE(&conn->stats, STAT_BYTES_RECEIVED, p - (char*)buf);
	net->stream->chunk_size = old_chunk_size;

#ifdef MYSQLND_DUMP_HEADER_N_BODY_FULL
	{
		unsigned int i;
		DBG_INF_FMT("BODY: requested=%d last_read=%3d", p - (char*)buf, ret);
		for (i = 0 ; i < p - (char*)buf; i++) {
			if (i && (i % 30 == 0)) {
				printf("\n\t\t");
			}
			printf("[%c] ", *(char *)(&(buf[i])));
		}
		for (i = 0 ; i < p - (char*)buf; i++) {
			if (i && (i % 30 == 0)) {
				printf("\n\t\t");
			}
			printf("%.2X ", (int)*((char*)&(buf[i])));
		}
		php_printf("\n\t\t\t-=-=-=-=-\n");
	}
#endif

	DBG_RETURN(p - (char*)buf);
}
/* }}} */


/* {{{ php_mysqlnd_greet_read */
static enum_func_status
php_mysqlnd_greet_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	zend_uchar buf[512];
	zend_uchar *p= buf;
	zend_uchar *begin = buf;
	php_mysql_packet_greet *packet= (php_mysql_packet_greet *) _packet;

	DBG_ENTER("php_mysqlnd_greet_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "greeting");

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

	packet->server_version = pestrdup((char *)p, conn->persistent);
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
	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("GREET packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "GREET packet %d bytes shorter than expected. PID=%d",
						 p - begin - packet->header.size, getpid());
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
	const unsigned char *s1_end= s1 + len;
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
	unsigned char sha1[SHA1_MAX_LENGTH];
	unsigned char sha2[SHA1_MAX_LENGTH];
	

	/* Phase 1: hash password */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, password, strlen((char *)password));
	PHP_SHA1Final(sha1, &context);

	/* Phase 2: hash sha1 */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, (unsigned char*)sha1, SHA1_MAX_LENGTH);
	PHP_SHA1Final(sha2, &context);

	/* Phase 3: hash scramble + sha2 */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, scramble, SCRAMBLE_LENGTH);
	PHP_SHA1Update(&context, (unsigned char*)sha2, SHA1_MAX_LENGTH);
	PHP_SHA1Final(buffer, &context);

	/* let's crypt buffer now */
	php_mysqlnd_crypt(buffer, (const uchar *)buffer, (const uchar *)sha1, SHA1_MAX_LENGTH);
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
	
	/* don't allow multi_queries via connect parameter */
	packet->client_flags ^= CLIENT_MULTI_STATEMENTS;
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
		php_mysqlnd_scramble((unsigned char*)p, packet->server_scramble_buf,
							 (unsigned char *)packet->password);
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


/* {{{ php_mysqlnd_ok_read */
static enum_func_status
php_mysqlnd_ok_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	zend_uchar buf[1024];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned long i;
	register php_mysql_packet_ok *packet= (php_mysql_packet_ok *) _packet;

	DBG_ENTER("php_mysqlnd_ok_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "OK");

	/* Should be always 0x0 or 0xFF for error */
	packet->field_count= uint1korr(p);
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
		packet->message = pestrndup((char *)p, MIN(i, sizeof(buf) - (p - buf)), conn->persistent);
		packet->message_len = i;
	} else {
		packet->message = NULL;
	}

	DBG_INF_FMT("OK packet: aff_rows=%lld last_ins_id=%ld server_status=%d warnings=%d",
					packet->affected_rows, packet->last_insert_id, packet->server_status,
					packet->warning_count);

	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "OK packet %d bytes shorter than expected. PID=%d",
						 p - begin - packet->header.size, getpid());
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
	zend_uchar buf[5 + 10 + sizeof(packet->sqlstate) + sizeof(packet->error)];
	zend_uchar *p= buf;
	zend_uchar *begin = buf;

	DBG_ENTER("php_mysqlnd_eof_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "EOF");

	/* Should be always 0xFE */
	packet->field_count= uint1korr(p);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "EOF packet %d bytes shorter than expected. PID=%d",
						 p - begin - packet->header.size, getpid());
	}
	
	DBG_INF_FMT("EOF packet: status=%d warnings=%d", packet->server_status, packet->warning_count);

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

	if (error_reporting) {
		EG(error_reporting) = 0;
	}
	
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	php_mysqlnd_consume_uneaten_data(conn, packet->command TSRMLS_CC);
#endif

	if (!packet->argument || !packet->arg_len) {
		char buffer[MYSQLND_HEADER_SIZE + 1];

		int1store(buffer + MYSQLND_HEADER_SIZE, packet->command);
		written = mysqlnd_stream_write_w_header(conn, buffer, 1 TSRMLS_CC);
	} else {
#if USE_CORK && defined(TCP_CORK)
		written = mysqlnd_stream_write_w_command(conn, packet->command, packet->argument,
												 packet->arg_len TSRMLS_CC));
#else
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
#endif
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
	zend_uchar buf[1024];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	size_t len;
	php_mysql_packet_rset_header *packet= (php_mysql_packet_rset_header *) _packet;

	DBG_ENTER("php_mysqlnd_rset_header_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "resultset header");

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
			/*
			  First byte in the packet is the field count.
			  Thus, the name is size - 1. And we add 1 for a trailing \0.
			*/
			len = packet->header.size - 1;
			packet->info_or_local_file = mnd_pemalloc(len + 1, conn->persistent);
			memcpy(packet->info_or_local_file, p, len);
			packet->info_or_local_file[len] = '\0';
			packet->info_or_local_file_len = len;
			break;
		case 0x00:
			packet->affected_rows = php_mysqlnd_net_field_length_ll(&p);
			packet->last_insert_id= php_mysqlnd_net_field_length_ll(&p);
			packet->server_status = uint2korr(p);
			p+=2;
			packet->warning_count = uint2korr(p);
			p+=2;
			/* Check for additional textual data */
			if (packet->header.size  > (p - buf) && (len = php_mysqlnd_net_field_length(&p))) {
				packet->info_or_local_file = mnd_pemalloc(len + 1, conn->persistent);
				memcpy(packet->info_or_local_file, p, len);
				packet->info_or_local_file[len] = '\0';
				packet->info_or_local_file_len = len;
			}
			break;
		default:
			/* Result set */
			break;
	}
	if (p - begin > packet->header.size) {
		DBG_ERR_FMT("GREET packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "GREET packet %d bytes shorter than expected. PID=%d",
						 p - begin - packet->header.size, getpid());
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_free_mem */
static
void php_mysqlnd_rset_header_free_mem(void *_packet, zend_bool alloca TSRMLS_DC)
{
	php_mysql_packet_rset_header *p= (php_mysql_packet_rset_header *) _packet;
	if (p->info_or_local_file) {
		mnd_efree(p->info_or_local_file);
		p->info_or_local_file = NULL;
	}
	if (!alloca) {
		mnd_pefree(p, p->header.persistent);
	}
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
	zend_uchar *buf = (zend_uchar *) conn->net.cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	char *root_ptr;
	size_t buf_len = conn->net.cmd_buffer.length, total_len = 0;
	unsigned long len;
	MYSQLND_FIELD *meta;
	unsigned int i, field_count = sizeof(rset_field_offsets)/sizeof(size_t);

	DBG_ENTER("php_mysqlnd_rset_field_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "field");

	if (packet->skip_parsing) {
		DBG_RETURN(PASS);
	}
	if (*p == 0xFE && packet->header.size < 8) {
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
				goto faulty_fake;
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
		DBG_ERR_FMT("Result set field packet %d bytes shorter than expected", p - begin - packet->header.size);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result set field packet %d bytes "
						 "shorter than expected. PID=%d", p - begin - packet->header.size, getpid());
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
/*
	DBG_INF_FMT("FIELD=[%s.%s.%s]", meta->db? meta->db:"*NA*", meta->table? meta->table:"*NA*",
				meta->name? meta->name:"*NA*");
*/
	DBG_RETURN(PASS);

faulty_fake:
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
php_mysqlnd_read_row_ex(MYSQLND *conn, MYSQLND_MEMORY_POOL_CHUNK **buffer,
						uint64 *data_size, zend_bool persistent_alloc,
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
			*buffer = conn->result_set_memory_pool->get_chunk(conn->result_set_memory_pool,
															  *data_size + 1 TSRMLS_CC);
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

		if (!mysqlnd_read_body(conn, p, header.size TSRMLS_CC)) {
			DBG_ERR("Empty row packet body");
			php_error(E_WARNING, "Empty row packet body. PID=%d", getpid());
			ret = FAIL;
			break;
		}

		if (header.size < MYSQLND_MAX_PACKET_SIZE) {
			break;
		}
	}
	if (ret == FAIL) {
		(*buffer)->free_chunk((*buffer), TRUE TSRMLS_CC);
		*buffer = NULL;
	}
	*data_size -= prealloc_more_bytes;
	DBG_RETURN(ret);
}


/* {{{ php_mysqlnd_rowp_read_binary_protocol */
void php_mysqlnd_rowp_read_binary_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
										   uint field_count, MYSQLND_FIELD *fields_metadata,
										   MYSQLND *conn TSRMLS_DC)
{
	int i;
	zend_uchar *p = row_buffer->ptr;
	zend_uchar *null_ptr, bit;
	zval **current_field, **end_field, **start_field;
	zend_bool as_unicode = conn->options.numeric_and_datetime_as_unicode;
	zend_bool allocated;
	void *obj;

	DBG_ENTER("php_mysqlnd_rowp_read_binary_protocol");

	end_field = (current_field = start_field = fields) + field_count;


	/* skip the first byte, not 0xFE -> 0x0, status */
	p++;
	null_ptr= p;
	p += (field_count + 9)/8;		/* skip null bits */
	bit	= 4;						/* first 2 bits are reserved */

	for (i = 0; current_field < end_field; current_field++, i++) {
#if 1
		obj = mysqlnd_palloc_get_zval(conn->zval_cache, &allocated TSRMLS_CC);
		if (allocated) {
			*current_field = (zval *) obj;
		} else {
			/* It's from the cache, so we can upcast here */
			*current_field = &((mysqlnd_zval *) obj)->zv;			
			((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_EXT_BUFFER;
		}
#else
		MAKE_STD_ZVAL(*current_field);
#endif
		if (*null_ptr & bit) {
			ZVAL_NULL(*current_field);
		} else {
			enum_mysqlnd_field_types type = fields_metadata[i].type;
			mysqlnd_ps_fetch_functions[type].func(*current_field, &fields_metadata[i],
												  0, &p, as_unicode TSRMLS_CC);
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
										 uint field_count, MYSQLND_FIELD *fields_metadata,
										 MYSQLND *conn TSRMLS_DC)
{
	int i;
	zend_bool last_field_was_string = FALSE;
	zval **current_field, **end_field, **start_field;
	zend_uchar *p = row_buffer->ptr;
	size_t data_size = row_buffer->app;
	zend_uchar *bit_area = (zend_uchar*) row_buffer->ptr + data_size + 1; /* we allocate from here */
	zend_bool as_unicode = conn->options.numeric_and_datetime_as_unicode;
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
	zend_bool as_int = conn->options.int_and_year_as_int;
#endif

	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol");

	end_field = (current_field = start_field = fields) + field_count;
	for (i = 0; current_field < end_field; current_field++, i++) {
		/* Don't reverse the order. It is significant!*/
		void *obj;
		zend_bool allocated;
		zend_uchar *this_field_len_pos = p;
		/* php_mysqlnd_net_field_length() call should be after *this_field_len_pos = p; */
		unsigned long len = php_mysqlnd_net_field_length(&p);

		obj = mysqlnd_palloc_get_zval(conn->zval_cache, &allocated TSRMLS_CC);
		if (allocated) {
			*current_field = (zval *) obj;
		} else {
			/* It's from the cache, so we can upcast here */
			*current_field = &((mysqlnd_zval *) obj)->zv;	
			((mysqlnd_zval *) obj)->point_type = MYSQLND_POINTS_FREE;
		}

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

#ifdef MYSQLND_STRING_TO_INT_CONVERSION
			if (as_int && perm_bind.php_type == IS_LONG &&
				perm_bind.pack_len <= SIZEOF_LONG)
			{
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				if (perm_bind.pack_len < SIZEOF_LONG)
				{
					/* direct conversion */
					int64 v = atoll((char *) p);
					ZVAL_LONG(*current_field, v);
				} else {
					uint64 v = (uint64) atoll((char *) p);
					zend_bool uns = packet->fields_metadata[i].flags & UNSIGNED_FLAG? TRUE:FALSE;
					/* We have to make it ASCIIZ temporarily */
#if SIZEOF_LONG==8
					if (uns == TRUE && v > 9223372036854775807L)
#elif SIZEOF_LONG==4
					if ((uns == TRUE && v > L64(2147483647)) || 
						(uns == FALSE && (( L64(2147483647) < (int64) v) ||
						(L64(-2147483648) > (int64) v))))
#endif /* SIZEOF */
					{
						ZVAL_STRINGL(*current_field, (char *)p, len, 0);
					} else {
						ZVAL_LONG(*current_field, (int64)v);
					}
				}
				*(p + len) = save;
			} else
#endif
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
					bit_area += 1 + sprintf((char *)start, MYSQLND_LLU_SPEC,
											(int64) Z_LVAL_PP(current_field));
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
	uint64 data_size = 0;

	DBG_ENTER("php_mysqlnd_rowp_read");

	if (!packet->binary_protocol && packet->bit_fields_count) {
		/* For every field we need terminating \0 */
		post_alloc_for_bit_fields =
			packet->bit_fields_total_len + packet->bit_fields_count;
	}

	ret = php_mysqlnd_read_row_ex(conn, &packet->row_buffer, &data_size,
								  packet->persistent_alloc, post_alloc_for_bit_fields
								  TSRMLS_CC);
	if (FAIL == ret) {
		goto end;
	}

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
	zend_uchar buf[1024];
	php_mysql_packet_stats *packet= (php_mysql_packet_stats *) _packet;

	DBG_ENTER("php_mysqlnd_stats_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "statistics");

	packet->message = mnd_pemalloc(packet->header.size + 1, conn->persistent);
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
	zend_uchar buf[1024];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned int data_size;
	php_mysql_packet_prepare_response *packet= (php_mysql_packet_prepare_response *) _packet;

	DBG_ENTER("php_mysqlnd_prepare_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "prepare");
	
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
		php_error(E_WARNING, "Wrong COM_STMT_PREPARE response size. Received %d. PID=%d", data_size, getpid());
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PREPARE packet %d bytes shorter than expected. PID=%d",
						 p - begin - packet->header.size, getpid());
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
	zend_uchar buf[1024];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	php_mysql_packet_chg_user_resp *packet= (php_mysql_packet_chg_user_resp *) _packet;

	DBG_ENTER("php_mysqlnd_chg_user_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "change user response ");

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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "CHANGE_USER packet %d bytes shorter than expected. PID=%d",
						 p - begin - packet->header.size, getpid());
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
