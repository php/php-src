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
#include "mysqlnd_ps.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"

#define BAIL_IF_NO_MORE_DATA \
	if (UNEXPECTED((size_t)(p - begin) > packet->header.size)) { \
		php_error_docref(NULL, E_WARNING, "Premature end of data (mysqlnd_wireprotocol.c:%u)", __LINE__); \
		goto premature_end; \
	} \


static const char *unknown_sqlstate= "HY000";
const char * const mysqlnd_empty_string = "";

/* Used in mysqlnd_debug.c */
const char mysqlnd_read_header_name[]	= "mysqlnd_read_header";
const char mysqlnd_read_body_name[]		= "mysqlnd_read_body";

#define ERROR_MARKER 0xFF
#define EODATA_MARKER 0xFE

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
  "STMT_RESET", "SET_OPTION", "STMT_FETCH", "DAEMON", "BINLOG_DUMP_GTID",
  "RESET_CONNECTION"
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
zend_ulong
php_mysqlnd_net_field_length(const zend_uchar **packet)
{
	register const zend_uchar *p= (const zend_uchar *)*packet;

	if (*p < 251) {
		(*packet)++;
		return (zend_ulong) *p;
	}

	switch (*p) {
		case 251:
			(*packet)++;
			return MYSQLND_NULL_LENGTH;
		case 252:
			(*packet) += 3;
			return (zend_ulong) uint2korr(p+1);
		case 253:
			(*packet) += 4;
			return (zend_ulong) uint3korr(p+1);
		default:
			(*packet) += 9;
			return (zend_ulong) uint4korr(p+1);
	}
}
/* }}} */


/* {{{ php_mysqlnd_net_field_length_ll
   Get next field's length */
uint64_t
php_mysqlnd_net_field_length_ll(const zend_uchar **packet)
{
	register const zend_uchar *p = (zend_uchar *)*packet;

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
zend_uchar *
php_mysqlnd_net_store_length(zend_uchar *packet, const uint64_t length)
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
		int3store(packet,(zend_ulong) length);
		return packet + 3;
	}
	*packet++ = 254;
	int8store(packet, length);
	return packet + 8;
}
/* }}} */


/* {{{ php_mysqlnd_net_store_length_size */
size_t
php_mysqlnd_net_store_length_size(uint64_t length)
{
	if (length < (uint64_t) L64(251)) {
		return 1;
	}
	if (length < (uint64_t) L64(65536)) {
		return 3;
	}
	if (length < (uint64_t) L64(16777216)) {
		return 4;
	}
	return 9;
}
/* }}} */


/* {{{ php_mysqlnd_read_error_from_line */
static enum_func_status
php_mysqlnd_read_error_from_line(const zend_uchar * const buf, const size_t buf_len,
								 char *error, const size_t error_buf_len,
								 unsigned int *error_no, char *sqlstate)
{
	const zend_uchar *p = buf;
	size_t error_msg_len = 0;

	DBG_ENTER("php_mysqlnd_read_error_from_line");

	*error_no = CR_UNKNOWN_ERROR;
	memcpy(sqlstate, unknown_sqlstate, MYSQLND_SQLSTATE_LENGTH);

	if (buf_len > 2) {
		*error_no = uint2korr(p);
		p+= 2;
		/*
		  sqlstate is following. No need to check for buf_left_len as we checked > 2 above,
		  if it was >=2 then we would need a check
		*/
		if (*p == '#') {
			++p;
			if ((buf_len - (p - buf)) >= MYSQLND_SQLSTATE_LENGTH) {
				memcpy(sqlstate, p, MYSQLND_SQLSTATE_LENGTH);
				p+= MYSQLND_SQLSTATE_LENGTH;
			} else {
				goto end;
			}
		}
		if ((buf_len - (p - buf)) > 0) {
			error_msg_len = MIN((int)((buf_len - (p - buf))), (int) (error_buf_len - 1));
			memcpy(error, p, error_msg_len);
		}
	}
end:
	sqlstate[MYSQLND_SQLSTATE_LENGTH] = '\0';
	error[error_msg_len]= '\0';

	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_read_header */
static enum_func_status
mysqlnd_read_header(MYSQLND_PFC * pfc, MYSQLND_VIO * vio, MYSQLND_PACKET_HEADER * header,
					MYSQLND_STATS * conn_stats, MYSQLND_ERROR_INFO * error_info)
{
	zend_uchar buffer[MYSQLND_HEADER_SIZE];

	DBG_ENTER(mysqlnd_read_header_name);
	DBG_INF_FMT("compressed=%u", pfc->data->compressed);
	if (FAIL == pfc->data->m.receive(pfc, vio, buffer, MYSQLND_HEADER_SIZE, conn_stats, error_info)) {
		DBG_RETURN(FAIL);
	}

	header->size = uint3korr(buffer);
	header->packet_no = uint1korr(buffer + 3);

	DBG_INF_FMT("HEADER: prot_packet_no=%u size=%3u", header->packet_no, header->size);
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn_stats,
							STAT_PROTOCOL_OVERHEAD_IN, MYSQLND_HEADER_SIZE,
							STAT_PACKETS_RECEIVED, 1);

	if (pfc->data->compressed || pfc->data->packet_no == header->packet_no) {
		/*
		  Have to increase the number, so we can send correct number back. It will
		  round at 255 as this is unsigned char. The server needs this for simple
		  flow control checking.
		*/
		pfc->data->packet_no++;
		DBG_RETURN(PASS);
	}

	DBG_ERR_FMT("Logical link: packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
				pfc->data->packet_no, header->packet_no, header->size);

	php_error(E_WARNING, "Packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
			  pfc->data->packet_no, header->packet_no, header->size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_read_packet_header_and_body */
static enum_func_status
mysqlnd_read_packet_header_and_body(MYSQLND_PACKET_HEADER * packet_header,
									MYSQLND_PFC * pfc,
									MYSQLND_VIO * vio,
									MYSQLND_STATS * stats,
									MYSQLND_ERROR_INFO * error_info,
									MYSQLND_CONNECTION_STATE * connection_state,
									zend_uchar * const buf, const size_t buf_size,
									const char * const packet_type_as_text,
									enum mysqlnd_packet_type packet_type)
{
	DBG_ENTER("mysqlnd_read_packet_header_and_body");
	DBG_INF_FMT("buf=%p size=%u", buf, buf_size);
	if (FAIL == mysqlnd_read_header(pfc, vio, packet_header, stats, error_info)) {
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		DBG_ERR_FMT("Can't read %s's header", packet_type_as_text);
		DBG_RETURN(FAIL);
	}
	if (buf_size < packet_header->size) {
		DBG_ERR_FMT("Packet buffer %u wasn't big enough %u, %u bytes will be unread",
					buf_size, packet_header->size, packet_header->size - buf_size);
		DBG_RETURN(FAIL);
	}
	if (FAIL == pfc->data->m.receive(pfc, vio, buf, packet_header->size, stats, error_info)) {
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
		DBG_ERR_FMT("Empty '%s' packet body", packet_type_as_text);
		DBG_RETURN(FAIL);
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, packet_type_to_statistic_byte_count[packet_type],
										MYSQLND_HEADER_SIZE + packet_header->size,
										packet_type_to_statistic_packet_count[packet_type],
										1);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_greet_read */
static enum_func_status
php_mysqlnd_greet_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	zend_uchar buf[2048];
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	const zend_uchar * pad_start = NULL;
	MYSQLND_PACKET_GREET *packet= (MYSQLND_PACKET_GREET *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;

	DBG_ENTER("php_mysqlnd_greet_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, sizeof(buf), "greeting", PROT_GREET_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	packet->authentication_plugin_data.s = packet->intern_auth_plugin_data;
	packet->authentication_plugin_data.l = sizeof(packet->intern_auth_plugin_data);

	if (packet->header.size < sizeof(buf)) {
		/*
		  Null-terminate the string, so strdup can work even if the packets have a string at the end,
		  which is not ASCIIZ
		*/
		buf[packet->header.size] = '\0';
	}

	packet->protocol_version = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->protocol_version) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
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
	BAIL_IF_NO_MORE_DATA;

	packet->thread_id = uint4korr(p);
	p+=4;
	BAIL_IF_NO_MORE_DATA;

	memcpy(packet->authentication_plugin_data.s, p, SCRAMBLE_LENGTH_323);
	p+= SCRAMBLE_LENGTH_323;
	BAIL_IF_NO_MORE_DATA;

	/* pad1 */
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->server_capabilities = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;
	DBG_INF_FMT("4.1 server_caps=%u\n", (uint32_t) packet->server_capabilities);

	packet->charset_no = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->server_status = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	/* pad2 */
	pad_start = p;
	p+= 13;
	BAIL_IF_NO_MORE_DATA;

	if ((size_t) (p - buf) < packet->header.size) {
		/* auth_plugin_data is split into two parts */
		memcpy(packet->authentication_plugin_data.s + SCRAMBLE_LENGTH_323, p, SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323);
		p+= SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323;
		p++; /* 0x0 at the end of the scramble and thus last byte in the packet in 5.1 and previous */
	} else {
		packet->pre41 = TRUE;
	}

	/* Is this a 5.5+ server ? */
	if ((size_t) (p - buf) < packet->header.size) {
		 /* backtrack one byte, the 0x0 at the end of the scramble in 5.1 and previous */
		p--;

    	/* Additional 16 bits for server capabilities */
		DBG_INF_FMT("additional 5.5+ caps=%u\n", (uint32_t) uint2korr(pad_start));
		packet->server_capabilities |= ((uint32_t) uint2korr(pad_start)) << 16;
		/* And a length of the server scramble in one byte */
		packet->authentication_plugin_data.l = uint1korr(pad_start + 2);
		if (packet->authentication_plugin_data.l > SCRAMBLE_LENGTH) {
			/* more data*/
			char * new_auth_plugin_data = emalloc(packet->authentication_plugin_data.l);

			/* copy what we already have */
			memcpy(new_auth_plugin_data, packet->authentication_plugin_data.s, SCRAMBLE_LENGTH);
			/* add additional scramble data 5.5+ sent us */
			memcpy(new_auth_plugin_data + SCRAMBLE_LENGTH, p, packet->authentication_plugin_data.l - SCRAMBLE_LENGTH);
			p+= (packet->authentication_plugin_data.l - SCRAMBLE_LENGTH);
			packet->authentication_plugin_data.s = new_auth_plugin_data;
		}
	}

	if (packet->server_capabilities & CLIENT_PLUGIN_AUTH) {
		BAIL_IF_NO_MORE_DATA;
		/* The server is 5.5.x and supports authentication plugins */
		packet->auth_protocol = estrdup((char *)p);
		p+= strlen(packet->auth_protocol) + 1; /* eat the '\0' */
	}

	DBG_INF_FMT("proto=%u server=%s thread_id=%u",
				packet->protocol_version, packet->server_version, packet->thread_id);

	DBG_INF_FMT("server_capabilities=%u charset_no=%u server_status=%i auth_protocol=%s scramble_length=%u",
				packet->server_capabilities, packet->charset_no, packet->server_status,
				packet->auth_protocol? packet->auth_protocol:"n/a", packet->authentication_plugin_data.l);

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("GREET packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "GREET packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_greet_free_mem */
static
void php_mysqlnd_greet_free_mem(void * _packet)
{
	MYSQLND_PACKET_GREET *p= (MYSQLND_PACKET_GREET *) _packet;
	if (p->server_version) {
		efree(p->server_version);
		p->server_version = NULL;
	}
	if (p->authentication_plugin_data.s && p->authentication_plugin_data.s != p->intern_auth_plugin_data) {
		efree(p->authentication_plugin_data.s);
		p->authentication_plugin_data.s = NULL;
	}
	if (p->auth_protocol) {
		efree(p->auth_protocol);
		p->auth_protocol = NULL;
	}
}
/* }}} */


#define AUTH_WRITE_BUFFER_LEN (MYSQLND_HEADER_SIZE + MYSQLND_MAX_ALLOWED_USER_LEN + SCRAMBLE_LENGTH + MYSQLND_MAX_ALLOWED_DB_LEN + 1 + 4096)

/* {{{ php_mysqlnd_auth_write */
static
size_t php_mysqlnd_auth_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	zend_uchar buffer[AUTH_WRITE_BUFFER_LEN];
	zend_uchar *p = buffer + MYSQLND_HEADER_SIZE; /* start after the header */
	size_t len;
	MYSQLND_PACKET_AUTH * packet= (MYSQLND_PACKET_AUTH *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;

	DBG_ENTER("php_mysqlnd_auth_write");

	if (!packet->is_change_user_packet) {
		int4store(p, packet->client_flags);
		p+= 4;

		int4store(p, packet->max_packet_size);
		p+= 4;

		int1store(p, packet->charset_no);
		p++;

		memset(p, 0, 23); /* filler */
		p+= 23;
	}

	if (packet->send_auth_data || packet->is_change_user_packet) {
		len = MIN(strlen(packet->user), MYSQLND_MAX_ALLOWED_USER_LEN);
		memcpy(p, packet->user, len);
		p+= len;
		*p++ = '\0';

		/* defensive coding */
		if (packet->auth_data == NULL) {
			packet->auth_data_len = 0;
		}
		if (packet->auth_data_len > 0xFF) {
			const char * const msg = "Authentication data too long. "
				"Won't fit into the buffer and will be truncated. Authentication will thus fail";
			SET_CLIENT_ERROR(error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, msg);
			php_error_docref(NULL, E_WARNING, "%s", msg);
			DBG_RETURN(0);
		}

		int1store(p, (int8_t)packet->auth_data_len);
		++p;
/*!!!!! is the buffer big enough ??? */
		if (sizeof(buffer) < (packet->auth_data_len + (p - buffer))) {
			DBG_ERR("the stack buffer was not enough!!");
			DBG_RETURN(0);
		}
		if (packet->auth_data_len) {
			memcpy(p, packet->auth_data, packet->auth_data_len);
			p+= packet->auth_data_len;
		}

		if (packet->db_len > 0) {
			/* CLIENT_CONNECT_WITH_DB should have been set */
			size_t real_db_len = MIN(MYSQLND_MAX_ALLOWED_DB_LEN, packet->db_len);
			memcpy(p, packet->db, real_db_len);
			p+= real_db_len;
			*p++= '\0';
		} else if (packet->is_change_user_packet) {
			*p++= '\0';
		}
		/* no \0 for no DB */

		if (packet->is_change_user_packet) {
			if (packet->charset_no) {
				int2store(p, packet->charset_no);
				p+= 2;
			}
		}

		if (packet->auth_plugin_name) {
			len = MIN(strlen(packet->auth_plugin_name), sizeof(buffer) - (p - buffer) - 1);
			memcpy(p, packet->auth_plugin_name, len);
			p+= len;
			*p++= '\0';
		}

		if (packet->connect_attr && zend_hash_num_elements(packet->connect_attr)) {
			size_t ca_payload_len = 0;

			{
				zend_string * key;
				zval * entry_value;
				ZEND_HASH_FOREACH_STR_KEY_VAL(packet->connect_attr, key, entry_value) {
					if (key) { /* HASH_KEY_IS_STRING */
						size_t value_len = Z_STRLEN_P(entry_value);

						ca_payload_len += php_mysqlnd_net_store_length_size(ZSTR_LEN(key));
						ca_payload_len += ZSTR_LEN(key);
						ca_payload_len += php_mysqlnd_net_store_length_size(value_len);
						ca_payload_len += value_len;
					}
				} ZEND_HASH_FOREACH_END();
			}

			if (sizeof(buffer) >= (ca_payload_len + php_mysqlnd_net_store_length_size(ca_payload_len) + (p - buffer))) {
				p = php_mysqlnd_net_store_length(p, ca_payload_len);

				{
					zend_string * key;
					zval * entry_value;
					ZEND_HASH_FOREACH_STR_KEY_VAL(packet->connect_attr, key, entry_value) {
						if (key) { /* HASH_KEY_IS_STRING */
							size_t value_len = Z_STRLEN_P(entry_value);

							/* copy key */
							p = php_mysqlnd_net_store_length(p, ZSTR_LEN(key));
							memcpy(p, ZSTR_VAL(key), ZSTR_LEN(key));
							p+= ZSTR_LEN(key);
							/* copy value */
							p = php_mysqlnd_net_store_length(p, value_len);
							memcpy(p, Z_STRVAL_P(entry_value), value_len);
							p+= value_len;
						}
					} ZEND_HASH_FOREACH_END();
				}
			} else {
				/* cannot put the data - skip */
			}
		}
	}
	if (packet->is_change_user_packet) {
		enum_func_status ret = FAIL;
		const MYSQLND_CSTRING payload = {(char*) buffer + MYSQLND_HEADER_SIZE, p - (buffer + MYSQLND_HEADER_SIZE)};
		const unsigned int silent = packet->silent;

		ret = conn->command->change_user(conn, payload, silent);
		DBG_RETURN(ret == PASS? (p - buffer - MYSQLND_HEADER_SIZE) : 0);
	} else {
		/*
		  The auth handshake packet has no command in it. Thus we can't go over conn->command directly.
		  Well, we can have a command->no_command(conn, payload)
		*/
		const size_t sent = pfc->data->m.send(pfc, vio, buffer, p - buffer - MYSQLND_HEADER_SIZE, stats, error_info);
		if (!sent) {
			SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		}
		DBG_RETURN(sent);
	}
}
/* }}} */


#define AUTH_RESP_BUFFER_SIZE 2048

/* {{{ php_mysqlnd_auth_response_read */
static enum_func_status
php_mysqlnd_auth_response_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	register MYSQLND_PACKET_AUTH_RESPONSE * packet= (MYSQLND_PACKET_AUTH_RESPONSE *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar local_buf[AUTH_RESP_BUFFER_SIZE];
	size_t buf_len = pfc->cmd_buffer.buffer? pfc->cmd_buffer.length: AUTH_RESP_BUFFER_SIZE;
	zend_uchar *buf = pfc->cmd_buffer.buffer? (zend_uchar *) pfc->cmd_buffer.buffer : local_buf;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_auth_response_read");

	/* leave space for terminating safety \0 */
	buf_len--;
	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "OK", PROT_OK_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/*
	  zero-terminate the buffer for safety. We are sure there is place for the \0
	  because buf_len is -1 the size of the buffer pointed
	*/
	buf[packet->header.size] = '\0';

	/* Should be always 0x0 or ERROR_MARKER for error */
	packet->response_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->response_code) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
		DBG_RETURN(PASS);
	}
	if (0xFE == packet->response_code) {
		/* Authentication Switch Response */
		if (packet->header.size > (size_t) (p - buf)) {
			packet->new_auth_protocol = mnd_pestrdup((char *)p, FALSE);
			packet->new_auth_protocol_len = strlen(packet->new_auth_protocol);
			p+= packet->new_auth_protocol_len + 1; /* +1 for the \0 */

			packet->new_auth_protocol_data_len = packet->header.size - (size_t) (p - buf);
			if (packet->new_auth_protocol_data_len) {
				packet->new_auth_protocol_data = mnd_emalloc(packet->new_auth_protocol_data_len);
				memcpy(packet->new_auth_protocol_data, p, packet->new_auth_protocol_data_len);
			}
			DBG_INF_FMT("The server requested switching auth plugin to : %s", packet->new_auth_protocol);
			DBG_INF_FMT("Server salt : [%d][%.*s]", packet->new_auth_protocol_data_len, packet->new_auth_protocol_data_len, packet->new_auth_protocol_data);
		}
	} else {
		zend_ulong net_len;
		/* Everything was fine! */
		packet->affected_rows  = php_mysqlnd_net_field_length_ll(&p);
		BAIL_IF_NO_MORE_DATA;

		packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
		BAIL_IF_NO_MORE_DATA;

		packet->server_status = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;

		packet->warning_count = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;

		/* There is a message */
		if (packet->header.size > (size_t) (p - buf) && (net_len = php_mysqlnd_net_field_length(&p))) {
			packet->message_len = MIN(net_len, buf_len - (p - begin));
			packet->message = mnd_pestrndup((char *)p, packet->message_len, FALSE);
		} else {
			packet->message = NULL;
			packet->message_len = 0;
		}

		DBG_INF_FMT("OK packet: aff_rows=%lld last_ins_id=%pd server_status=%u warnings=%u",
					packet->affected_rows, packet->last_insert_id, packet->server_status,
					packet->warning_count);
	}

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "AUTH_RESPONSE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_auth_response_free_mem */
static void
php_mysqlnd_auth_response_free_mem(void * _packet)
{
	MYSQLND_PACKET_AUTH_RESPONSE * p = (MYSQLND_PACKET_AUTH_RESPONSE *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
	if (p->new_auth_protocol) {
		mnd_efree(p->new_auth_protocol);
		p->new_auth_protocol = NULL;
	}
	p->new_auth_protocol_len = 0;

	if (p->new_auth_protocol_data) {
		mnd_efree(p->new_auth_protocol_data);
		p->new_auth_protocol_data = NULL;
	}
	p->new_auth_protocol_data_len = 0;
}
/* }}} */


/* {{{ php_mysqlnd_change_auth_response_write */
static size_t
php_mysqlnd_change_auth_response_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_CHANGE_AUTH_RESPONSE *packet= (MYSQLND_PACKET_CHANGE_AUTH_RESPONSE *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar * const buffer = pfc->cmd_buffer.length >= packet->auth_data_len? pfc->cmd_buffer.buffer : mnd_emalloc(packet->auth_data_len);
	zend_uchar * p = buffer + MYSQLND_HEADER_SIZE; /* start after the header */

	DBG_ENTER("php_mysqlnd_change_auth_response_write");

	if (packet->auth_data_len) {
		memcpy(p, packet->auth_data, packet->auth_data_len);
		p+= packet->auth_data_len;
	}

	{
		/*
		  The auth handshake packet has no command in it. Thus we can't go over conn->command directly.
		  Well, we can have a command->no_command(conn, payload)
		*/
		const size_t sent = pfc->data->m.send(pfc, vio, buffer, p - buffer - MYSQLND_HEADER_SIZE, stats, error_info);
		if (buffer != pfc->cmd_buffer.buffer) {
			mnd_efree(buffer);
		}
		if (!sent) {
			SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
		}
		DBG_RETURN(sent);
	}
}
/* }}} */


#define OK_BUFFER_SIZE 2048

/* {{{ php_mysqlnd_ok_read */
static enum_func_status
php_mysqlnd_ok_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	register MYSQLND_PACKET_OK *packet= (MYSQLND_PACKET_OK *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar local_buf[OK_BUFFER_SIZE];
	const size_t buf_len = pfc->cmd_buffer.buffer? pfc->cmd_buffer.length : OK_BUFFER_SIZE;
	zend_uchar * const buf = pfc->cmd_buffer.buffer? (zend_uchar *) pfc->cmd_buffer.buffer : local_buf;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	zend_ulong net_len;

	DBG_ENTER("php_mysqlnd_ok_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "OK", PROT_OK_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/* Should be always 0x0 or ERROR_MARKER for error */
	packet->field_count = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
		DBG_RETURN(PASS);
	}
	/* Everything was fine! */
	packet->affected_rows  = php_mysqlnd_net_field_length_ll(&p);
	BAIL_IF_NO_MORE_DATA;

	packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
	BAIL_IF_NO_MORE_DATA;

	packet->server_status = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	packet->warning_count = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	/* There is a message */
	if (packet->header.size > (size_t) (p - buf) && (net_len = php_mysqlnd_net_field_length(&p))) {
		packet->message_len = MIN(net_len, buf_len - (p - begin));
		packet->message = mnd_pestrndup((char *)p, packet->message_len, FALSE);
	} else {
		packet->message = NULL;
		packet->message_len = 0;
	}

	DBG_INF_FMT("OK packet: aff_rows=%lld last_ins_id=%ld server_status=%u warnings=%u",
				packet->affected_rows, packet->last_insert_id, packet->server_status,
				packet->warning_count);

	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "OK packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_ok_free_mem */
static void
php_mysqlnd_ok_free_mem(void * _packet)
{
	MYSQLND_PACKET_OK *p= (MYSQLND_PACKET_OK *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
}
/* }}} */


/* {{{ php_mysqlnd_eof_read */
static enum_func_status
php_mysqlnd_eof_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	/*
	  EOF packet is since 4.1 five bytes long,
	  but we can get also an error, make it bigger.

	  Error : error_code + '#' + sqlstate + MYSQLND_ERRMSG_SIZE
	*/
	MYSQLND_PACKET_EOF *packet= (MYSQLND_PACKET_EOF *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar * const buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_eof_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "EOF", PROT_EOF_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/* Should be always EODATA_MARKER */
	packet->field_count = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
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
		BAIL_IF_NO_MORE_DATA;

		packet->server_status = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;
	} else {
		packet->warning_count = 0;
		packet->server_status = 0;
	}

	BAIL_IF_NO_MORE_DATA;

	DBG_INF_FMT("EOF packet: fields=%u status=%u warnings=%u",
				packet->field_count, packet->server_status, packet->warning_count);

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("EOF packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "EOF packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_cmd_write */
size_t php_mysqlnd_cmd_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	/* Let's have some space, which we can use, if not enough, we will allocate new buffer */
	MYSQLND_PACKET_COMMAND * packet= (MYSQLND_PACKET_COMMAND *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const unsigned int error_reporting = EG(error_reporting);
	size_t sent = 0;

	DBG_ENTER("php_mysqlnd_cmd_write");
	/*
	  Reset packet_no, or we will get bad handshake!
	  Every command starts a new TX and packet numbers are reset to 0.
	*/
	pfc->data->m.reset(pfc, stats, error_info);

	if (error_reporting) {
		EG(error_reporting) = 0;
	}

	MYSQLND_INC_CONN_STATISTIC(stats, STAT_PACKETS_SENT_CMD);

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	vio->data->m.consume_uneaten_data(vio, packet->command);
#endif

	if (!packet->argument.s || !packet->argument.l) {
		zend_uchar buffer[MYSQLND_HEADER_SIZE + 1];

		int1store(buffer + MYSQLND_HEADER_SIZE, packet->command);
		sent = pfc->data->m.send(pfc, vio, buffer, 1, stats, error_info);
	} else {
		size_t tmp_len = packet->argument.l + 1 + MYSQLND_HEADER_SIZE;
		zend_uchar *tmp, *p;
		tmp = (tmp_len > pfc->cmd_buffer.length)? mnd_emalloc(tmp_len):pfc->cmd_buffer.buffer;
		if (!tmp) {
			goto end;
		}
		p = tmp + MYSQLND_HEADER_SIZE; /* skip the header */

		int1store(p, packet->command);
		p++;

		memcpy(p, packet->argument.s, packet->argument.l);

		sent = pfc->data->m.send(pfc, vio, tmp, tmp_len - MYSQLND_HEADER_SIZE, stats, error_info);
		if (tmp != pfc->cmd_buffer.buffer) {
			MYSQLND_INC_CONN_STATISTIC(stats, STAT_CMD_BUFFER_TOO_SMALL);
			mnd_efree(tmp);
		}
	}
end:
	if (error_reporting) {
		/* restore error reporting */
		EG(error_reporting) = error_reporting;
	}
	if (!sent) {
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
	}
	DBG_RETURN(sent);
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_read */
static enum_func_status
php_mysqlnd_rset_header_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_RSET_HEADER * packet= (MYSQLND_PACKET_RSET_HEADER *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	enum_func_status ret = PASS;
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar * const buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	size_t len;

	DBG_ENTER("php_mysqlnd_rset_header_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "resultset header", PROT_RSET_HEADER_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/*
	  Don't increment. First byte is ERROR_MARKER on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										);
		DBG_RETURN(PASS);
	}

	packet->field_count = php_mysqlnd_net_field_length(&p);
	BAIL_IF_NO_MORE_DATA;

	switch (packet->field_count) {
		case MYSQLND_NULL_LENGTH:
			DBG_INF("LOAD LOCAL");
			/*
			  First byte in the packet is the field count.
			  Thus, the name is size - 1. And we add 1 for a trailing \0.
			  Because we have BAIL_IF_NO_MORE_DATA before the switch, we are guaranteed
			  that packet->header.size is > 0. Which means that len can't underflow, that
			  would lead to 0 byte allocation but 2^32 or 2^64 bytes copied.
			*/
			len = packet->header.size - 1;
			packet->info_or_local_file.s = mnd_emalloc(len + 1);
			if (packet->info_or_local_file.s) {
				memcpy(packet->info_or_local_file.s, p, len);
				packet->info_or_local_file.s[len] = '\0';
				packet->info_or_local_file.l = len;
			} else {
				SET_OOM_ERROR(error_info);
				ret = FAIL;
			}
			break;
		case 0x00:
			DBG_INF("UPSERT");
			packet->affected_rows = php_mysqlnd_net_field_length_ll(&p);
			BAIL_IF_NO_MORE_DATA;

			packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
			BAIL_IF_NO_MORE_DATA;

			packet->server_status = uint2korr(p);
			p+=2;
			BAIL_IF_NO_MORE_DATA;

			packet->warning_count = uint2korr(p);
			p+=2;
			BAIL_IF_NO_MORE_DATA;
			/* Check for additional textual data */
			if (packet->header.size  > (size_t) (p - buf) && (len = php_mysqlnd_net_field_length(&p))) {
				packet->info_or_local_file.s = mnd_emalloc(len + 1);
				if (packet->info_or_local_file.s) {
					memcpy(packet->info_or_local_file.s, p, len);
					packet->info_or_local_file.s[len] = '\0';
					packet->info_or_local_file.l = len;
				} else {
					SET_OOM_ERROR(error_info);
					ret = FAIL;
				}
			}
			DBG_INF_FMT("affected_rows=%llu last_insert_id=%llu server_status=%u warning_count=%u",
						packet->affected_rows, packet->last_insert_id,
						packet->server_status, packet->warning_count);
			break;
		default:
			DBG_INF("SELECT");
			/* Result set */
			break;
	}
	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(ret);
premature_end:
	DBG_ERR_FMT("RSET_HEADER packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "RSET_HEADER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_free_mem */
static
void php_mysqlnd_rset_header_free_mem(void * _packet)
{
	MYSQLND_PACKET_RSET_HEADER *p= (MYSQLND_PACKET_RSET_HEADER *) _packet;
	DBG_ENTER("php_mysqlnd_rset_header_free_mem");
	if (p->info_or_local_file.s) {
		mnd_efree(p->info_or_local_file.s);
		p->info_or_local_file.s = NULL;
	}
	DBG_VOID_RETURN;
}
/* }}} */

#define READ_RSET_FIELD(field_name) do { \
		len = php_mysqlnd_net_field_length(&p); \
		if (UNEXPECTED(len == MYSQLND_NULL_LENGTH)) { \
			goto faulty_or_fake; \
		} else if (len != 0) { \
			meta->field_name = (const char *)p; \
			meta->field_name ## _length = len; \
			p += len; \
			total_len += len + 1; \
		} else { \
			meta->field_name = mysqlnd_empty_string; \
			meta->field_name ## _length = 0; \
		} \
	} while (0)


/* {{{ php_mysqlnd_rset_field_read */
static enum_func_status
php_mysqlnd_rset_field_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	/* Should be enough for the metadata of a single row */
	MYSQLND_PACKET_RES_FIELD *packet = (MYSQLND_PACKET_RES_FIELD *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const size_t buf_len = pfc->cmd_buffer.length;
	size_t total_len = 0;
	zend_uchar * const buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	const zend_uchar * p = buf;
	const zend_uchar * const begin = buf;
	char *root_ptr;
	zend_ulong len;
	MYSQLND_FIELD *meta;

	DBG_ENTER("php_mysqlnd_rset_field_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "field", PROT_RSET_FLD_PACKET)) {
		DBG_RETURN(FAIL);
	}

	if (packet->skip_parsing) {
		DBG_RETURN(PASS);
	}

	BAIL_IF_NO_MORE_DATA;
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										);
		DBG_ERR_FMT("Server error : (%u) %s", packet->error_info.error_no, packet->error_info.error);
		DBG_RETURN(PASS);
	} else if (EODATA_MARKER == *p && packet->header.size < 8) {
		/* Premature EOF. That should be COM_FIELD_LIST. But we don't support COM_FIELD_LIST anymore, thus this should not happen */
		DBG_INF("Premature EOF. That should be COM_FIELD_LIST");
		DBG_RETURN(PASS);
	}

	meta = packet->metadata;

	READ_RSET_FIELD(catalog);
	READ_RSET_FIELD(db);
	READ_RSET_FIELD(table);
	READ_RSET_FIELD(org_table);
	READ_RSET_FIELD(name);
	READ_RSET_FIELD(org_name);

	/* 1 byte length */
	if (UNEXPECTED(12 != *p)) {
		DBG_ERR_FMT("Protocol error. Server sent false length. Expected 12 got %d", (int) *p);
		php_error_docref(NULL, E_WARNING, "Protocol error. Server sent false length. Expected 12");
	}

	if ((size_t)((p - begin) + 12) > packet->header.size) {
		php_error_docref(NULL, E_WARNING, "Premature end of data (mysqlnd_wireprotocol.c:%u)", __LINE__);
		goto premature_end;
	}

	p++;

	meta->charsetnr = uint2korr(p);
	p += 2;

	meta->length = uint4korr(p);
	p += 4;

	meta->type = uint1korr(p);
	p += 1;

	meta->flags = uint2korr(p);
	p += 2;

	meta->decimals = uint1korr(p);
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
	if (packet->header.size > (size_t) (p - buf) &&
		(len = php_mysqlnd_net_field_length(&p)) &&
		len != MYSQLND_NULL_LENGTH)
	{
		BAIL_IF_NO_MORE_DATA;
		DBG_INF_FMT("Def found, length %lu", len);
		meta->def = packet->memory_pool->get_chunk(packet->memory_pool, len + 1);
		memcpy(meta->def, p, len);
		meta->def[len] = '\0';
		meta->def_length = len;
		p += len;
	}

	root_ptr = meta->root = packet->memory_pool->get_chunk(packet->memory_pool, total_len);
	meta->root_len = total_len;

	if (EXPECTED(meta->name_length != 0)) {
		meta->sname = zend_string_init_interned(meta->name, meta->name_length, 0);
		meta->name = ZSTR_VAL(meta->sname);
	} else {
		meta->sname = ZSTR_EMPTY_ALLOC();
	}

	/* Now do allocs */
	if (meta->catalog_length != 0) {
		len = meta->catalog_length;
		meta->catalog = memcpy(root_ptr, meta->catalog, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->db_length != 0) {
		len = meta->db_length;
		meta->db = memcpy(root_ptr, meta->db, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->table_length != 0) {
		len = meta->table_length;
		meta->table = memcpy(root_ptr, meta->table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_table_length != 0) {
		len = meta->org_table_length;
		meta->org_table = memcpy(root_ptr, meta->org_table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_name_length != 0) {
		len = meta->org_name_length;
		meta->org_name = memcpy(root_ptr, meta->org_name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	DBG_INF_FMT("allocing root.");

	DBG_INF_FMT("FIELD=[%s.%s.%s]", meta->db? meta->db:"*NA*", meta->table? meta->table:"*NA*",
				meta->name? meta->name:"*NA*");

	DBG_RETURN(PASS);

faulty_or_fake:
	DBG_ERR_FMT("Protocol error. Server sent NULL_LENGTH. The server is faulty");
	php_error_docref(NULL, E_WARNING, "Protocol error. Server sent NULL_LENGTH."
					 " The server is faulty");
	DBG_RETURN(FAIL);
premature_end:
	DBG_ERR_FMT("RSET field packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "Result set field packet "MYSQLND_SZ_T_SPEC" bytes "
			 		"shorter than expected", p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_read_row_ex */
static enum_func_status
php_mysqlnd_read_row_ex(MYSQLND_PFC * pfc,
						MYSQLND_VIO * vio,
						MYSQLND_STATS * stats,
						MYSQLND_ERROR_INFO * error_info,
						MYSQLND_MEMORY_POOL * pool,
						MYSQLND_ROW_BUFFER * buffer,
						size_t * const data_size)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_HEADER header;
	zend_uchar * p = NULL;
	size_t prealloc_more_bytes;

	DBG_ENTER("php_mysqlnd_read_row_ex");

	/*
	  To ease the process the server splits everything in packets up to 2^24 - 1.
	  Even in the case the payload is evenly divisible by this value, the last
	  packet will be empty, namely 0 bytes. Thus, we can read every packet and ask
	  for next one if they have 2^24 - 1 sizes. But just read the header of a
	  zero-length byte, don't read the body, there is no such.
	*/

	/*
	  We're allocating an extra byte, as php_mysqlnd_rowp_read_text_protocol_aux
	  needs to be able to append a terminating \0 for atoi/atof.
	*/
	prealloc_more_bytes = 1;

	*data_size = 0;
	if (UNEXPECTED(FAIL == mysqlnd_read_header(pfc, vio, &header, stats, error_info))) {
		ret = FAIL;
	} else {
		*data_size += header.size;
		buffer->ptr = pool->get_chunk(pool, *data_size + prealloc_more_bytes);
		p = buffer->ptr;

		if (UNEXPECTED(PASS != (ret = pfc->data->m.receive(pfc, vio, p, header.size, stats, error_info)))) {
			DBG_ERR("Empty row packet body");
			php_error(E_WARNING, "Empty row packet body");
		} else {
			while (header.size >= MYSQLND_MAX_PACKET_SIZE) {
				if (FAIL == mysqlnd_read_header(pfc, vio, &header, stats, error_info)) {
					ret = FAIL;
					break;
				}

				*data_size += header.size;

				/* Empty packet after MYSQLND_MAX_PACKET_SIZE packet. That's ok, break */
				if (!header.size) {
					break;
				}

				/*
				  We have to realloc the buffer.
				*/
				buffer->ptr = pool->resize_chunk(pool, buffer->ptr, *data_size - header.size, *data_size + prealloc_more_bytes);
				if (!buffer->ptr) {
					SET_OOM_ERROR(error_info);
					ret = FAIL;
					break;
				}
				/* The position could have changed, recalculate */
				p = (zend_uchar *) buffer->ptr + (*data_size - header.size);

				if (PASS != (ret = pfc->data->m.receive(pfc, vio, p, header.size, stats, error_info))) {
					DBG_ERR("Empty row packet body");
					php_error(E_WARNING, "Empty row packet body");
					break;
				}
			}
		}
	}
	if (ret == FAIL && buffer->ptr) {
		pool->free_chunk(pool, buffer->ptr);
		buffer->ptr = NULL;
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_binary_protocol */
enum_func_status
php_mysqlnd_rowp_read_binary_protocol(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
									  const unsigned int field_count, const MYSQLND_FIELD * const fields_metadata,
									  const zend_bool as_int_or_float, MYSQLND_STATS * const stats)
{
	unsigned int i;
	const zend_uchar * p = row_buffer->ptr;
	const zend_uchar * null_ptr;
	zend_uchar bit;
	zval *current_field, *end_field, *start_field;

	DBG_ENTER("php_mysqlnd_rowp_read_binary_protocol");

	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	/* skip the first byte, not EODATA_MARKER -> 0x0, status */
	p++;
	null_ptr= p;
	p += (field_count + 9)/8;	/* skip null bits */
	bit	= 4;					/* first 2 bits are reserved */

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		enum_mysqlnd_collected_stats statistic;
		const zend_uchar * orig_p = p;

		DBG_INF_FMT("Into zval=%p decoding column %u [%s.%s.%s] type=%u field->flags&unsigned=%u flags=%u is_bit=%u",
			current_field, i,
			fields_metadata[i].db, fields_metadata[i].table, fields_metadata[i].name, fields_metadata[i].type,
			fields_metadata[i].flags & UNSIGNED_FLAG, fields_metadata[i].flags, fields_metadata[i].type == MYSQL_TYPE_BIT);
		if (*null_ptr & bit) {
			DBG_INF("It's null");
			ZVAL_NULL(current_field);
			statistic = STAT_BINARY_TYPE_FETCHED_NULL;
		} else {
			enum_mysqlnd_field_types type = fields_metadata[i].type;
			mysqlnd_ps_fetch_functions[type].func(current_field, &fields_metadata[i], 0, &p);

			if (MYSQLND_G(collect_statistics)) {
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
			}
		}
		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1,
										STAT_BYTES_RECEIVED_PURE_DATA_PS,
										(Z_TYPE_P(current_field) == IS_STRING)?
											Z_STRLEN_P(current_field) : (size_t)(p - orig_p));

		if (!((bit<<=1) & 255)) {
			bit = 1;	/* to the following byte */
			null_ptr++;
		}
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_text_protocol */
enum_func_status
php_mysqlnd_rowp_read_text_protocol_aux(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
									unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
									zend_bool as_int_or_float, MYSQLND_STATS * stats)
{
	unsigned int i;
	zval *current_field, *end_field, *start_field;
	zend_uchar * p = row_buffer->ptr;
	const size_t data_size = row_buffer->size;
	const zend_uchar * const packet_end = (zend_uchar*) p + data_size;

	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol_aux");

	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		/* php_mysqlnd_net_field_length() call should be after *this_field_len_pos = p; */
		const zend_ulong len = php_mysqlnd_net_field_length((const zend_uchar **) &p);

		/* NULL or NOT NULL, this is the question! */
		if (len == MYSQLND_NULL_LENGTH) {
			ZVAL_NULL(current_field);
		} else if ((p + len) > packet_end) {
			php_error_docref(NULL, E_WARNING, "Malformed server packet. Field length pointing "MYSQLND_SZ_T_SPEC
											  " bytes after end of packet", (p + len) - packet_end - 1);
			DBG_RETURN(FAIL);
		} else {
#if defined(MYSQLND_STRING_TO_INT_CONVERSION)
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
					case MYSQL_TYPE_JSON:		statistic = STAT_TEXT_TYPE_FETCHED_JSON; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_TEXT_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_TEXT_TYPE_FETCHED_OTHER; break;
				}
				MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1, STAT_BYTES_RECEIVED_PURE_DATA_TEXT, len);
			}
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
			if (as_int_or_float && perm_bind.php_type == IS_LONG) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				if (perm_bind.pack_len < SIZEOF_ZEND_LONG) {
					/* direct conversion */
					int64_t v =
#ifndef PHP_WIN32
						atoll((char *) p);
#else
						_atoi64((char *) p);
#endif
					ZVAL_LONG(current_field, (zend_long) v); /* the cast is safe */
				} else {
					uint64_t v =
#ifndef PHP_WIN32
						(uint64_t) atoll((char *) p);
#else
						(uint64_t) _atoi64((char *) p);
#endif
					zend_bool uns = fields_metadata[i].flags & UNSIGNED_FLAG? TRUE:FALSE;
					/* We have to make it ASCIIZ temporarily */
#if SIZEOF_ZEND_LONG==8
					if (uns == TRUE && v > 9223372036854775807L)
#elif SIZEOF_ZEND_LONG==4
					if ((uns == TRUE && v > L64(2147483647)) ||
						(uns == FALSE && (( L64(2147483647) < (int64_t) v) ||
						(L64(-2147483648) > (int64_t) v))))
#else
#error Need fix for this architecture
#endif /* SIZEOF */
					{
						ZVAL_STRINGL(current_field, (char *)p, len);
					} else {
						ZVAL_LONG(current_field, (zend_long) v); /* the cast is safe */
					}
				}
				*(p + len) = save;
			} else if (as_int_or_float && perm_bind.php_type == IS_DOUBLE) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				ZVAL_DOUBLE(current_field, atof((char *) p));
				*(p + len) = save;
			} else
#endif /* MYSQLND_STRING_TO_INT_CONVERSION */
			if (fields_metadata[i].type == MYSQL_TYPE_BIT) {
				/*
				  BIT fields are specially handled. As they come as bit mask, they have
				  to be converted to human-readable representation.
				*/
				ps_fetch_from_1_to_8_bytes(current_field, &(fields_metadata[i]), 0, (const zend_uchar **) &p, len);
				/*
				  We have advanced in ps_fetch_from_1_to_8_bytes. We should go back because
				  later in this function there will be an advancement.
				*/
				p -= len;
				if (Z_TYPE_P(current_field) == IS_LONG && !as_int_or_float) {
					/* we are using the text protocol, so convert to string */
					char tmp[22];
					const size_t tmp_len = sprintf((char *)&tmp, ZEND_ULONG_FMT, Z_LVAL_P(current_field));
					ZVAL_STRINGL(current_field, tmp, tmp_len);
				} else if (Z_TYPE_P(current_field) == IS_STRING) {
					/* nothing to do here, as we want a string and ps_fetch_from_1_to_8_bytes() has given us one */
				}
			} else if (len == 0) {
				ZVAL_EMPTY_STRING(current_field);
			} else if (len == 1) {
				ZVAL_INTERNED_STR(current_field, ZSTR_CHAR((zend_uchar)*(char *)p));
			} else {
				ZVAL_STRINGL(current_field, (char *)p, len);
			}
			p += len;
		}
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_text_protocol_zval */
enum_func_status
php_mysqlnd_rowp_read_text_protocol_zval(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
										 const unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
										 const zend_bool as_int_or_float, MYSQLND_STATS * stats)
{
	enum_func_status ret;
	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol_zval");
	ret = php_mysqlnd_rowp_read_text_protocol_aux(row_buffer, fields, field_count, fields_metadata, as_int_or_float, stats);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_text_protocol_c */
enum_func_status
php_mysqlnd_rowp_read_text_protocol_c(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
									  const unsigned int field_count, const MYSQLND_FIELD * const fields_metadata,
									  const zend_bool as_int_or_float, MYSQLND_STATS * const stats)
{
	enum_func_status ret;
	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol_c");
	ret = php_mysqlnd_rowp_read_text_protocol_aux(row_buffer, fields, field_count, fields_metadata, as_int_or_float, stats);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read */
/*
  if normal statements => packet->fields is created by this function,
  if PS => packet->fields is passed from outside
*/
static enum_func_status
php_mysqlnd_rowp_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_ROW *packet= (MYSQLND_PACKET_ROW *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	zend_uchar *p;
	enum_func_status ret = PASS;
	size_t data_size = 0;

	DBG_ENTER("php_mysqlnd_rowp_read");

	ret = php_mysqlnd_read_row_ex(pfc, vio, stats, error_info,
								  packet->result_set_memory_pool, &packet->row_buffer, &data_size);
	if (FAIL == ret) {
		goto end;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, packet_type_to_statistic_byte_count[PROT_ROW_PACKET],
										MYSQLND_HEADER_SIZE + packet->header.size,
										packet_type_to_statistic_packet_count[PROT_ROW_PACKET],
										1);

	/*
	  packet->row_buffer->ptr is of size 'data_size'
	  in pre-7.0 it was really 'data_size + 1' although it was counted as 'data_size'
	  The +1 was for the additional byte needed to \0 terminate the last string in the row.
	  This was needed as the zvals of pre-7.0 could use external memory (no copy param to ZVAL_STRINGL).
	  However, in 7.0+ the strings always copy. Thus this +1 byte was removed. Also the optimization or \0
	  terminating every string, which did overwrite the lengths from the packet. For this reason we needed
	  to keep (and copy) the lengths externally.
	*/
	packet->header.size = data_size;
	packet->row_buffer.size = data_size;

	if (ERROR_MARKER == (*(p = packet->row_buffer.ptr))) {
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
										);
	} else if (EODATA_MARKER == *p && data_size < 8) { /* EOF */
		packet->eof = TRUE;
		p++;
		if (data_size > 1) {
			packet->warning_count = uint2korr(p);
			p += 2;
			packet->server_status = uint2korr(p);
			/* Seems we have 3 bytes reserved for future use */
			DBG_INF_FMT("server_status=%u warning_count=%u", packet->server_status, packet->warning_count);
		}
	} else {
		MYSQLND_INC_CONN_STATISTIC(stats,
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
				packet->fields = mnd_ecalloc(packet->field_count, sizeof(zval));
			}
		} else {
			MYSQLND_INC_CONN_STATISTIC(stats,
										packet->binary_protocol? STAT_ROWS_SKIPPED_PS:
																 STAT_ROWS_SKIPPED_NORMAL);
		}
	}

end:
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_free_mem */
static void
php_mysqlnd_rowp_free_mem(void * _packet)
{
	MYSQLND_PACKET_ROW *p;

	DBG_ENTER("php_mysqlnd_rowp_free_mem");
	p = (MYSQLND_PACKET_ROW *) _packet;
	if (p->row_buffer.ptr) {
		p->result_set_memory_pool->free_chunk(p->result_set_memory_pool, p->row_buffer.ptr);
		p->row_buffer.ptr = NULL;
	}
	/*
	  Don't free packet->fields :
	  - normal queries -> store_result() | fetch_row_unbuffered() will transfer
	    the ownership and NULL it.
	  - PS will pass in it the bound variables, we have to use them! and of course
	    not free the array. As it is passed to us, we should not clean it ourselves.
	*/
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ php_mysqlnd_stats_read */
static enum_func_status
php_mysqlnd_stats_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_STATS *packet= (MYSQLND_PACKET_STATS *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) pfc->cmd_buffer.buffer;

	DBG_ENTER("php_mysqlnd_stats_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "statistics", PROT_STATS_PACKET)) {
		DBG_RETURN(FAIL);
	}

	packet->message.s = mnd_emalloc(packet->header.size + 1);
	memcpy(packet->message.s, buf, packet->header.size);
	packet->message.s[packet->header.size] = '\0';
	packet->message.l = packet->header.size;

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_stats_free_mem */
static
void php_mysqlnd_stats_free_mem(void * _packet)
{
	MYSQLND_PACKET_STATS *p= (MYSQLND_PACKET_STATS *) _packet;
	if (p->message.s) {
		mnd_efree(p->message.s);
		p->message.s = NULL;
	}
}
/* }}} */


/* 1 + 4 (id) + 2 (field_c) + 2 (param_c) + 1 (filler) + 2 (warnings ) */
#define PREPARE_RESPONSE_SIZE_41 9
#define PREPARE_RESPONSE_SIZE_50 12

/* {{{ php_mysqlnd_prepare_read */
static enum_func_status
php_mysqlnd_prepare_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_PREPARE_RESPONSE *packet= (MYSQLND_PACKET_PREPARE_RESPONSE *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	/* In case of an error, we should have place to put it */
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	zend_uchar *p = buf;
	const zend_uchar * const begin = buf;
	unsigned int data_size;

	DBG_ENTER("php_mysqlnd_prepare_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "prepare", PROT_PREPARE_RESP_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	data_size = packet->header.size;
	packet->error_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->error_code) {
		php_mysqlnd_read_error_from_line(p, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										);
		DBG_RETURN(PASS);
	}

	if (data_size != PREPARE_RESPONSE_SIZE_41 &&
		data_size != PREPARE_RESPONSE_SIZE_50 &&
		!(data_size > PREPARE_RESPONSE_SIZE_50)) {
		DBG_ERR_FMT("Wrong COM_STMT_PREPARE response size. Received %u", data_size);
		php_error(E_WARNING, "Wrong COM_STMT_PREPARE response size. Received %u", data_size);
		DBG_RETURN(FAIL);
	}

	packet->stmt_id = uint4korr(p);
	p += 4;
	BAIL_IF_NO_MORE_DATA;

	/* Number of columns in result set */
	packet->field_count = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	packet->param_count = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	if (data_size > 9) {
		/* 0x0 filler sent by the server for 5.0+ clients */
		p++;
		BAIL_IF_NO_MORE_DATA;

		packet->warning_count = uint2korr(p);
	}

	DBG_INF_FMT("Prepare packet read: stmt_id=%u fields=%u params=%u",
				packet->stmt_id, packet->field_count, packet->param_count);

	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("PREPARE packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "PREPARE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_chg_user_read */
static enum_func_status
php_mysqlnd_chg_user_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_CHG_USER_RESPONSE *packet= (MYSQLND_PACKET_CHG_USER_RESPONSE *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	/* There could be an error message */
	const size_t buf_len = pfc->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) pfc->cmd_buffer.buffer;
	zend_uchar *p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_chg_user_read");

	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, buf_len, "change user response", PROT_CHG_USER_RESP_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	/*
	  Don't increment. First byte is ERROR_MARKER on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/

	/* Should be always 0x0 or ERROR_MARKER for error */
	packet->response_code = uint1korr(p);
	p++;

	if (packet->header.size == 1 && buf[0] == EODATA_MARKER && packet->server_capabilities & CLIENT_SECURE_CONNECTION) {
		/* We don't handle 3.23 authentication */
		packet->server_asked_323_auth = TRUE;
		DBG_RETURN(FAIL);
	}

	if (ERROR_MARKER == packet->response_code) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										);
	}
	BAIL_IF_NO_MORE_DATA;
	if (packet->response_code == 0xFE && packet->header.size > (size_t) (p - buf)) {
		packet->new_auth_protocol = mnd_pestrdup((char *)p, FALSE);
		packet->new_auth_protocol_len = strlen(packet->new_auth_protocol);
		p+= packet->new_auth_protocol_len + 1; /* +1 for the \0 */
		packet->new_auth_protocol_data_len = packet->header.size - (size_t) (p - buf);
		if (packet->new_auth_protocol_data_len) {
			packet->new_auth_protocol_data = mnd_emalloc(packet->new_auth_protocol_data_len);
			memcpy(packet->new_auth_protocol_data, p, packet->new_auth_protocol_data_len);
		}
		DBG_INF_FMT("The server requested switching auth plugin to : %s", packet->new_auth_protocol);
		DBG_INF_FMT("Server salt : [%*s]", packet->new_auth_protocol_data_len, packet->new_auth_protocol_data);
	}

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("CHANGE_USER packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "CHANGE_USER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_chg_user_free_mem */
static void
php_mysqlnd_chg_user_free_mem(void * _packet)
{
	MYSQLND_PACKET_CHG_USER_RESPONSE * p = (MYSQLND_PACKET_CHG_USER_RESPONSE *) _packet;

	if (p->new_auth_protocol) {
		mnd_efree(p->new_auth_protocol);
		p->new_auth_protocol = NULL;
	}
	p->new_auth_protocol_len = 0;

	if (p->new_auth_protocol_data) {
		mnd_efree(p->new_auth_protocol_data);
		p->new_auth_protocol_data = NULL;
	}
	p->new_auth_protocol_data_len = 0;
}
/* }}} */


/* {{{ php_mysqlnd_sha256_pk_request_write */
static
size_t php_mysqlnd_sha256_pk_request_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	zend_uchar buffer[MYSQLND_HEADER_SIZE + 1];
	size_t sent;

	DBG_ENTER("php_mysqlnd_sha256_pk_request_write");

	int1store(buffer + MYSQLND_HEADER_SIZE, '\1');
	sent = pfc->data->m.send(pfc, vio, buffer, 1, stats, error_info);

	DBG_RETURN(sent);
}
/* }}} */


#define SHA256_PK_REQUEST_RESP_BUFFER_SIZE 2048

/* {{{ php_mysqlnd_sha256_pk_request_response_read */
static enum_func_status
php_mysqlnd_sha256_pk_request_response_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE * packet= (MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar buf[SHA256_PK_REQUEST_RESP_BUFFER_SIZE];
	zend_uchar *p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_sha256_pk_request_response_read");

	/* leave space for terminating safety \0 */
	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, sizeof(buf), "SHA256_PK_REQUEST_RESPONSE", PROT_SHA256_PK_REQUEST_RESPONSE_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->public_key_len = packet->header.size - (p - buf);
	packet->public_key = mnd_emalloc(packet->public_key_len + 1);
	memcpy(packet->public_key, p, packet->public_key_len);
	packet->public_key[packet->public_key_len] = '\0';

	DBG_RETURN(PASS);

premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "SHA256_PK_REQUEST_RESPONSE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_sha256_pk_request_response_free_mem */
static void
php_mysqlnd_sha256_pk_request_response_free_mem(void * _packet)
{
	MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE * p = (MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE *) _packet;
	if (p->public_key) {
		mnd_efree(p->public_key);
		p->public_key = NULL;
	}
	p->public_key_len = 0;
}
/* }}} */

static
size_t php_mysqlnd_cached_sha2_result_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_CACHED_SHA2_RESULT * packet= (MYSQLND_PACKET_CACHED_SHA2_RESULT *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	ALLOCA_FLAG(use_heap)
	zend_uchar *buffer = do_alloca(MYSQLND_HEADER_SIZE + packet->password_len + 1, use_heap);
	size_t sent;

	DBG_ENTER("php_mysqlnd_cached_sha2_result_write");

	if (packet->request == 1) {
		int1store(buffer + MYSQLND_HEADER_SIZE, '\2');
		sent = pfc->data->m.send(pfc, vio, buffer, 1, stats, error_info);
	} else {
		memcpy(buffer + MYSQLND_HEADER_SIZE, packet->password, packet->password_len);
		sent = pfc->data->m.send(pfc, vio, buffer, packet->password_len, stats, error_info);
	}

	free_alloca(buffer, use_heap);
	DBG_RETURN(sent);
}

static enum_func_status
php_mysqlnd_cached_sha2_result_read(MYSQLND_CONN_DATA * conn, void * _packet)
{
	MYSQLND_PACKET_CACHED_SHA2_RESULT * packet= (MYSQLND_PACKET_CACHED_SHA2_RESULT *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	zend_uchar buf[SHA256_PK_REQUEST_RESP_BUFFER_SIZE];
	zend_uchar *p = buf;
	const zend_uchar * const begin = buf;

	DBG_ENTER("php_mysqlnd_cached_sha2_result_read");
	if (FAIL == mysqlnd_read_packet_header_and_body(&(packet->header), pfc, vio, stats, error_info, connection_state, buf, sizeof(buf), "PROT_CACHED_SHA2_RESULT_PACKET", PROT_CACHED_SHA2_RESULT_PACKET)) {
		DBG_RETURN(FAIL);
	}
	BAIL_IF_NO_MORE_DATA;

	packet->response_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->response_code) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										);
		DBG_RETURN(PASS);
	}
	if (0xFE == packet->response_code) {
		/* Authentication Switch Response */
		if (packet->header.size > (size_t) (p - buf)) {
			packet->new_auth_protocol = mnd_pestrdup((char *)p, FALSE);
			packet->new_auth_protocol_len = strlen(packet->new_auth_protocol);
			p+= packet->new_auth_protocol_len + 1; /* +1 for the \0 */

			packet->new_auth_protocol_data_len = packet->header.size - (size_t) (p - buf);
			if (packet->new_auth_protocol_data_len) {
				packet->new_auth_protocol_data = mnd_emalloc(packet->new_auth_protocol_data_len);
				memcpy(packet->new_auth_protocol_data, p, packet->new_auth_protocol_data_len);
			}
			DBG_INF_FMT("The server requested switching auth plugin to : %s", packet->new_auth_protocol);
			DBG_INF_FMT("Server salt : [%d][%.*s]", packet->new_auth_protocol_data_len, packet->new_auth_protocol_data_len, packet->new_auth_protocol_data);
		}
		DBG_RETURN(PASS);
	}

	if (0x1 != packet->response_code) {
		DBG_ERR_FMT("Unexpected response code %d", packet->response_code);
	}

	/* This is not really the response code, but we reuse the field. */
	packet->response_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->result = uint1korr(p);
	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);

premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL, E_WARNING, "SHA256_PK_REQUEST_RESPONSE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}

/* {{{ packet_methods */
static
mysqlnd_packet_methods packet_methods[PROT_LAST] =
{
	{
		php_mysqlnd_greet_read,
		NULL, /* write */
		php_mysqlnd_greet_free_mem,
	}, /* PROT_GREET_PACKET */
	{
		NULL, /* read */
		php_mysqlnd_auth_write,
		NULL,
	}, /* PROT_AUTH_PACKET */
	{
		php_mysqlnd_auth_response_read, /* read */
		NULL, /* write */
		php_mysqlnd_auth_response_free_mem,
	}, /* PROT_AUTH_RESP_PACKET */
	{
		NULL, /* read */
		php_mysqlnd_change_auth_response_write, /* write */
		NULL,
	}, /* PROT_CHANGE_AUTH_RESP_PACKET */
	{
		php_mysqlnd_ok_read, /* read */
		NULL, /* write */
		php_mysqlnd_ok_free_mem,
	}, /* PROT_OK_PACKET */
	{
		php_mysqlnd_eof_read, /* read */
		NULL, /* write */
		NULL,
	}, /* PROT_EOF_PACKET */
	{
		NULL, /* read */
		php_mysqlnd_cmd_write, /* write */
		NULL,
	}, /* PROT_CMD_PACKET */
	{
		php_mysqlnd_rset_header_read, /* read */
		NULL, /* write */
		php_mysqlnd_rset_header_free_mem,
	}, /* PROT_RSET_HEADER_PACKET */
	{
		php_mysqlnd_rset_field_read, /* read */
		NULL, /* write */
		NULL,
	}, /* PROT_RSET_FLD_PACKET */
	{
		php_mysqlnd_rowp_read, /* read */
		NULL, /* write */
		php_mysqlnd_rowp_free_mem,
	}, /* PROT_ROW_PACKET */
	{
		php_mysqlnd_stats_read, /* read */
		NULL, /* write */
		php_mysqlnd_stats_free_mem,
	}, /* PROT_STATS_PACKET */
	{
		php_mysqlnd_prepare_read, /* read */
		NULL, /* write */
		NULL,
	}, /* PROT_PREPARE_RESP_PACKET */
	{
		php_mysqlnd_chg_user_read, /* read */
		NULL, /* write */
		php_mysqlnd_chg_user_free_mem,
	}, /* PROT_CHG_USER_RESP_PACKET */
	{
		NULL, /* read */
		php_mysqlnd_sha256_pk_request_write,
		NULL,
	}, /* PROT_SHA256_PK_REQUEST_PACKET */
	{
		php_mysqlnd_sha256_pk_request_response_read,
		NULL, /* write */
		php_mysqlnd_sha256_pk_request_response_free_mem,
	}, /* PROT_SHA256_PK_REQUEST_RESPONSE_PACKET */
	{
		php_mysqlnd_cached_sha2_result_read,
		php_mysqlnd_cached_sha2_result_write,
		NULL
	} /* PROT_CACHED_SHA2_RESULT_PACKET */
};
/* }}} */


/* {{{ mysqlnd_protocol::init_greet_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_greet_packet)(struct st_mysqlnd_packet_greet *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_greet_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_GREET_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_auth_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_auth_packet)(struct st_mysqlnd_packet_auth *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_auth_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_AUTH_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_auth_response_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_auth_response_packet)(struct st_mysqlnd_packet_auth_response *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_auth_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_AUTH_RESP_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_change_auth_response_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_change_auth_response_packet)(struct st_mysqlnd_packet_change_auth_response *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_change_auth_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CHANGE_AUTH_RESP_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_ok_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_ok_packet)(struct st_mysqlnd_packet_ok *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_ok_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_OK_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_eof_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_eof_packet)(struct st_mysqlnd_packet_eof *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_eof_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_EOF_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_command_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_command_packet)(struct st_mysqlnd_packet_command *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_command_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CMD_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_rset_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_rset_header_packet)(struct st_mysqlnd_packet_rset_header *packet)
{
	DBG_ENTER("mysqlnd_protocol::get_rset_header_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_RSET_HEADER_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_result_field_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_result_field_packet)(struct st_mysqlnd_packet_res_field *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_result_field_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_RSET_FLD_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_row_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_row_packet)(struct st_mysqlnd_packet_row *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_row_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_ROW_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_stats_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_stats_packet)(struct st_mysqlnd_packet_stats *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_stats_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_STATS_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_prepare_response_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_prepare_response_packet)(struct st_mysqlnd_packet_prepare_response *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_prepare_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_PREPARE_RESP_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_change_user_response_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_change_user_response_packet)(struct st_mysqlnd_packet_chg_user_resp *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_change_user_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CHG_USER_RESP_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_sha256_pk_request_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_sha256_pk_request_packet)(struct st_mysqlnd_packet_sha256_pk_request *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_sha256_pk_request_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_SHA256_PK_REQUEST_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::init_sha256_pk_request_response_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_sha256_pk_request_response_packet)(struct st_mysqlnd_packet_sha256_pk_request_response *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_sha256_pk_request_response_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_SHA256_PK_REQUEST_RESPONSE_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */

/* {{{ mysqlnd_protocol::init_cached_sha2_result_packet */
static void
MYSQLND_METHOD(mysqlnd_protocol, init_cached_sha2_result_packet)(struct st_mysqlnd_packet_cached_sha2_result *packet)
{
	DBG_ENTER("mysqlnd_protocol::init_cached_sha2_result_packet");
	memset(packet, 0, sizeof(*packet));
	packet->header.m = &packet_methods[PROT_CACHED_SHA2_RESULT_PACKET];
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_protocol::send_command */
static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command)(
		MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
		const enum php_mysqlnd_server_command command,
		const zend_uchar * const arg, const size_t arg_len,
		const zend_bool silent,

		struct st_mysqlnd_connection_state * connection_state,
		MYSQLND_ERROR_INFO * error_info,
		MYSQLND_UPSERT_STATUS * upsert_status,
		MYSQLND_STATS * stats,
		func_mysqlnd_conn_data__send_close send_close,
		void * send_close_ctx)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_COMMAND cmd_packet;
	enum mysqlnd_connection_state state;
	DBG_ENTER("mysqlnd_protocol::send_command");
	DBG_INF_FMT("command=%s silent=%u", mysqlnd_command_to_text[command], silent);
	DBG_INF_FMT("server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(upsert_status));
	DBG_INF_FMT("sending %u bytes", arg_len + 1); /* + 1 is for the command */
	state = connection_state->m->get(connection_state);

	switch (state) {
		case CONN_READY:
			break;
		case CONN_QUIT_SENT:
			SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			DBG_ERR("Server is gone");
			DBG_RETURN(FAIL);
		default:
			SET_CLIENT_ERROR(error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
			DBG_ERR_FMT("Command out of sync. State=%u", state);
			DBG_RETURN(FAIL);
	}

	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(upsert_status);
	SET_EMPTY_ERROR(error_info);

	payload_decoder_factory->m.init_command_packet(&cmd_packet);

	cmd_packet.command = command;
	if (arg && arg_len) {
		cmd_packet.argument.s = (char *) arg;
		cmd_packet.argument.l = arg_len;
	}

	MYSQLND_INC_CONN_STATISTIC(stats, STAT_COM_QUIT + command - 1 /* because of COM_SLEEP */ );

	if (! PACKET_WRITE(payload_decoder_factory->conn, &cmd_packet)) {
		if (!silent && error_info->error_no != CR_SERVER_GONE_ERROR) {
			DBG_ERR_FMT("Error while sending %s packet", mysqlnd_command_to_text[command]);
			php_error(E_WARNING, "Error while sending %s packet. PID=%d", mysqlnd_command_to_text[command], getpid());
		}
		connection_state->m->set(connection_state, CONN_QUIT_SENT);
		send_close(send_close_ctx);
		DBG_ERR("Server is gone");
		ret = FAIL;
	}
	PACKET_FREE(&cmd_packet);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_protocol::send_command_handle_OK */
static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_OK)(
						MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory,
						MYSQLND_ERROR_INFO * const error_info,
						MYSQLND_UPSERT_STATUS * const upsert_status,
						const zend_bool ignore_upsert_status,  /* actually used only by LOAD DATA. COM_QUERY and COM_EXECUTE handle the responses themselves */
						MYSQLND_STRING * const last_message)
{
	enum_func_status ret = FAIL;
	MYSQLND_PACKET_OK ok_response;

	payload_decoder_factory->m.init_ok_packet(&ok_response);
	DBG_ENTER("mysqlnd_protocol::send_command_handle_OK");
	if (FAIL == (ret = PACKET_READ(payload_decoder_factory->conn, &ok_response))) {
		DBG_INF("Error while reading OK packet");
		SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
		goto end;
	}
	DBG_INF_FMT("OK from server");
	if (0xFF == ok_response.field_count) {
		/* The server signalled error. Set the error */
		SET_CLIENT_ERROR(error_info, ok_response.error_no, ok_response.sqlstate, ok_response.error);
		ret = FAIL;
		/*
		  Cover a protocol design error: error packet does not
		  contain the server status. Therefore, the client has no way
		  to find out whether there are more result sets of
		  a multiple-result-set statement pending. Luckily, in 5.0 an
		  error always aborts execution of a statement, wherever it is
		  a multi-statement or a stored procedure, so it should be
		  safe to unconditionally turn off the flag here.
		*/
		upsert_status->server_status &= ~SERVER_MORE_RESULTS_EXISTS;
		UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(upsert_status);
	} else {
		SET_NEW_MESSAGE(last_message->s, last_message->l,
						ok_response.message, ok_response.message_len);
		if (!ignore_upsert_status) {
			UPSERT_STATUS_RESET(upsert_status);
			UPSERT_STATUS_SET_WARNINGS(upsert_status, ok_response.warning_count);
			UPSERT_STATUS_SET_SERVER_STATUS(upsert_status, ok_response.server_status);
			UPSERT_STATUS_SET_AFFECTED_ROWS(upsert_status, ok_response.affected_rows);
			UPSERT_STATUS_SET_LAST_INSERT_ID(upsert_status, ok_response.last_insert_id);
		} else {
			/* LOAD DATA */
		}
	}
end:
	PACKET_FREE(&ok_response);
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_protocol::send_command_handle_EOF */
static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_EOF)(
						MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory,
						MYSQLND_ERROR_INFO * const error_info,
						MYSQLND_UPSERT_STATUS * const upsert_status)
{
	enum_func_status ret = FAIL;
	MYSQLND_PACKET_EOF response;

	payload_decoder_factory->m.init_eof_packet(&response);

	DBG_ENTER("mysqlnd_protocol::send_command_handle_EOF");

	if (FAIL == (ret = PACKET_READ(payload_decoder_factory->conn, &response))) {
		DBG_INF("Error while reading EOF packet");
		SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
	} else if (0xFF == response.field_count) {
		/* The server signalled error. Set the error */
		DBG_INF_FMT("Error_no=%d SQLstate=%s Error=%s", response.error_no, response.sqlstate, response.error);

		SET_CLIENT_ERROR(error_info, response.error_no, response.sqlstate, response.error);

		UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(upsert_status);
	} else if (0xFE != response.field_count) {
		SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
		DBG_ERR_FMT("EOF packet expected, field count wasn't 0xFE but 0x%2X", response.field_count);
		php_error_docref(NULL, E_WARNING, "EOF packet expected, field count wasn't 0xFE but 0x%2X", response.field_count);
	} else {
		DBG_INF_FMT("EOF from server");
	}
	PACKET_FREE(&response);

	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ send_command_handle_response */
static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_response)(
		MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
		const enum mysqlnd_packet_type ok_packet,
		const zend_bool silent,
		const enum php_mysqlnd_server_command command,
		const zend_bool ignore_upsert_status, /* actually used only by LOAD DATA. COM_QUERY and COM_EXECUTE handle the responses themselves */

		MYSQLND_ERROR_INFO	* error_info,
		MYSQLND_UPSERT_STATUS * upsert_status,
		MYSQLND_STRING * last_message
	)
{
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_protocol::send_command_handle_response");
	DBG_INF_FMT("silent=%u packet=%u command=%s", silent, ok_packet, mysqlnd_command_to_text[command]);

	switch (ok_packet) {
		case PROT_OK_PACKET:
			ret = payload_decoder_factory->m.send_command_handle_OK(payload_decoder_factory, error_info, upsert_status, ignore_upsert_status, last_message);
			break;
		case PROT_EOF_PACKET:
			ret = payload_decoder_factory->m.send_command_handle_EOF(payload_decoder_factory, error_info, upsert_status);
			break;
		default:
			SET_CLIENT_ERROR(error_info, CR_MALFORMED_PACKET, UNKNOWN_SQLSTATE, "Malformed packet");
			php_error_docref(NULL, E_ERROR, "Wrong response packet %u passed to the function", ok_packet);
			break;
	}
	if (!silent && error_info->error_no == CR_MALFORMED_PACKET) {
		php_error_docref(NULL, E_WARNING, "Error while reading %s's response packet. PID=%d", mysqlnd_command_to_text[command], getpid());
	}
	DBG_INF(ret == PASS ? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_protocol_payload_decoder_factory)
	MYSQLND_METHOD(mysqlnd_protocol, init_greet_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_auth_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_auth_response_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_change_auth_response_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_ok_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_command_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_eof_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_rset_header_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_result_field_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_row_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_stats_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_prepare_response_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_change_user_response_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_sha256_pk_request_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_sha256_pk_request_response_packet),
	MYSQLND_METHOD(mysqlnd_protocol, init_cached_sha2_result_packet),

	MYSQLND_METHOD(mysqlnd_protocol, send_command),
	MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_response),
	MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_OK),
	MYSQLND_METHOD(mysqlnd_protocol, send_command_handle_EOF),
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_protocol_payload_decoder_factory_init */
PHPAPI MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY *
mysqlnd_protocol_payload_decoder_factory_init(MYSQLND_CONN_DATA * conn, const zend_bool persistent)
{
	MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * ret;
	DBG_ENTER("mysqlnd_protocol_payload_decoder_factory_init");
	ret = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory).get_protocol_payload_decoder_factory(conn, persistent);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_protocol_payload_decoder_factory_free */
PHPAPI void
mysqlnd_protocol_payload_decoder_factory_free(MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const factory)
{
	DBG_ENTER("mysqlnd_protocol_payload_decoder_factory_free");

	if (factory) {
		zend_bool pers = factory->persistent;
		mnd_pefree(factory, pers);
	}
	DBG_VOID_RETURN;
}
/* }}} */
