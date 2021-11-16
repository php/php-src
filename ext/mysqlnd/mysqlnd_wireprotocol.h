/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_WIREPROTOCOL_H
#define MYSQLND_WIREPROTOCOL_H

#define MYSQLND_HEADER_SIZE 4
#define COMPRESSED_HEADER_SIZE 3

#define MYSQLND_NULL_LENGTH	(zend_ulong) ~0

/* Used in mysqlnd_debug.c */
PHPAPI extern const char mysqlnd_read_header_name[];
PHPAPI extern const char mysqlnd_read_body_name[];


/* Packet handling */
#define PACKET_WRITE(conn, packet)	((packet)->header.m->write_to_net((conn), (packet)))
#define PACKET_READ(conn, packet)	((packet)->header.m->read_from_net((conn), (packet)))
#define PACKET_FREE(packet) \
	do { \
		DBG_INF_FMT("PACKET_FREE(%p)", packet); \
		if ((packet)->header.m->free_mem) { \
			((packet)->header.m->free_mem((packet))); \
		} \
	} while (0);

PHPAPI extern const char * const mysqlnd_command_to_text[COM_END];

/* Low-level extraction functionality */
typedef struct st_mysqlnd_packet_methods {
	enum_func_status	(*read_from_net)(MYSQLND_CONN_DATA * conn, void * packet);
	size_t				(*write_to_net)(MYSQLND_CONN_DATA * conn, void * packet);
	void				(*free_mem)(void *packet);
} mysqlnd_packet_methods;


typedef struct st_mysqlnd_packet_header {
	size_t		size;
	zend_uchar	packet_no;

	mysqlnd_packet_methods *m;
} MYSQLND_PACKET_HEADER;

/* Server greets the client */
typedef struct st_mysqlnd_packet_greet {
	MYSQLND_PACKET_HEADER		header;
	uint8_t		protocol_version;
	char		*server_version;
	uint32_t	thread_id;
	char		intern_auth_plugin_data[SCRAMBLE_LENGTH];
	MYSQLND_STRING authentication_plugin_data;
	/* 1 byte pad */
	uint32_t	server_capabilities;
	uint8_t		charset_no;
	uint16_t	server_status;
	/* 13 byte pad, in 5.5 first 2 bytes are more capabilities followed by 1 byte scramble_length */
	bool	pre41;
	/* If error packet, we use these */
	char 		error[MYSQLND_ERRMSG_SIZE+1];
	char 		sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int	error_no;
	char		*auth_protocol;
} MYSQLND_PACKET_GREET;


/* Client authenticates */
typedef struct st_mysqlnd_packet_auth {
	MYSQLND_PACKET_HEADER		header;
	const char	*user;
	const zend_uchar	*auth_data;
	size_t		auth_data_len;
	const char	*db;
	const char	*auth_plugin_name;
	uint32_t	client_flags;
	uint32_t	max_packet_size;
	uint8_t		charset_no;
	/* Here the packet ends. This is user supplied data */
	bool	send_auth_data;
	bool	is_change_user_packet;
	bool	silent;
	HashTable	*connect_attr;
	size_t		db_len;
} MYSQLND_PACKET_AUTH;

/* Auth response packet */
typedef struct st_mysqlnd_packet_auth_response {
	MYSQLND_PACKET_HEADER		header;
	uint8_t		response_code;
	uint64_t	affected_rows;
	uint64_t	last_insert_id;
	uint16_t	server_status;
	uint16_t	warning_count;
	char		*message;
	size_t		message_len;
	/* If error packet, we use these */
	char 		error[MYSQLND_ERRMSG_SIZE+1];
	char 		sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;

	char		*new_auth_protocol;
	size_t		new_auth_protocol_len;
	zend_uchar	*new_auth_protocol_data;
	size_t		new_auth_protocol_data_len;
} MYSQLND_PACKET_AUTH_RESPONSE;


/* Auth response packet */
typedef struct st_mysqlnd_packet_change_auth_response {
	MYSQLND_PACKET_HEADER		header;
	const zend_uchar	*auth_data;
	size_t				auth_data_len;
} MYSQLND_PACKET_CHANGE_AUTH_RESPONSE;


/* OK packet */
typedef struct st_mysqlnd_packet_ok {
	MYSQLND_PACKET_HEADER		header;
	uint8_t		field_count; /* always 0x0 */
	uint64_t	affected_rows;
	uint64_t	last_insert_id;
	uint16_t	server_status;
	uint16_t	warning_count;
	char		*message;
	size_t		message_len;
	/* If error packet, we use these */
	char 		error[MYSQLND_ERRMSG_SIZE+1];
	char 		sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;
} MYSQLND_PACKET_OK;


/* Command packet */
typedef struct st_mysqlnd_packet_command {
	MYSQLND_PACKET_HEADER			header;
	enum php_mysqlnd_server_command	command;
	MYSQLND_CSTRING	argument;
} MYSQLND_PACKET_COMMAND;


/* EOF packet */
typedef struct st_mysqlnd_packet_eof {
	MYSQLND_PACKET_HEADER		header;
	uint8_t		field_count; /* 0xFE */
	uint16_t	warning_count;
	uint16_t	server_status;
	/* If error packet, we use these */
	char 		error[MYSQLND_ERRMSG_SIZE+1];
	char 		sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;
} MYSQLND_PACKET_EOF;
/* EOF packet */


/* Result Set header*/
typedef struct st_mysqlnd_packet_rset_header {
	MYSQLND_PACKET_HEADER header;
	/*
	  0x00 => ok
	  ~0   => LOAD DATA LOCAL
	  error_no != 0 => error
	  others => result set -> Read res_field packets up to field_count
	*/
	zend_ulong		field_count;
	/*
	  These are filled if no SELECT query. For SELECT warning_count
	  and server status are in the last row packet, the EOF packet.
	*/
	uint16_t	warning_count;
	uint16_t	server_status;
	uint64_t	affected_rows;
	uint64_t	last_insert_id;
	/* This is for both LOAD DATA or info, when no result set */
	MYSQLND_STRING info_or_local_file;
	/* If error packet, we use these */
	MYSQLND_ERROR_INFO	error_info;
} MYSQLND_PACKET_RSET_HEADER;


/* Result set field packet */
typedef struct st_mysqlnd_packet_res_field {
	MYSQLND_PACKET_HEADER	header;
	MYSQLND_MEMORY_POOL		*memory_pool;
	MYSQLND_FIELD			*metadata;
	/* For table definitions, empty for result sets */
	bool				skip_parsing;

	MYSQLND_ERROR_INFO		error_info;
} MYSQLND_PACKET_RES_FIELD;


/* Row packet */
typedef struct st_mysqlnd_packet_row {
	MYSQLND_PACKET_HEADER	header;
	uint32_t	field_count;
	bool	eof;
	/*
	  These are, of course, only for SELECT in the EOF packet,
	  which is detected by this packet
	*/
	uint16_t	warning_count;
	uint16_t	server_status;

	MYSQLND_ROW_BUFFER	row_buffer;
	MYSQLND_MEMORY_POOL * result_set_memory_pool;

	bool		binary_protocol;
	MYSQLND_FIELD	*fields_metadata;

	/* If error packet, we use these */
	MYSQLND_ERROR_INFO	error_info;
} MYSQLND_PACKET_ROW;


/* Statistics packet */
typedef struct st_mysqlnd_packet_stats {
	MYSQLND_PACKET_HEADER	header;
	MYSQLND_STRING message;
} MYSQLND_PACKET_STATS;


/* COM_PREPARE response packet */
typedef struct st_mysqlnd_packet_prepare_response {
	MYSQLND_PACKET_HEADER	header;
	/* also known as field_count 0x00=OK , 0xFF=error */
	unsigned char	error_code;
	zend_ulong	stmt_id;
	unsigned int	field_count;
	unsigned int	param_count;
	unsigned int	warning_count;

	/* present in case of error */
	MYSQLND_ERROR_INFO	error_info;
} MYSQLND_PACKET_PREPARE_RESPONSE;


/* Statistics packet */
typedef struct st_mysqlnd_packet_chg_user_resp {
	MYSQLND_PACKET_HEADER	header;
	uint32_t			response_code;

	/* message_len is not part of the packet*/
	uint16_t			server_capabilities;
	/* If error packet, we use these */
	MYSQLND_ERROR_INFO	error_info;
	bool			server_asked_323_auth;

	char		*new_auth_protocol;
	size_t		new_auth_protocol_len;
	zend_uchar	*new_auth_protocol_data;
	size_t		new_auth_protocol_data_len;
} MYSQLND_PACKET_CHG_USER_RESPONSE;


/* Command packet */
typedef struct st_mysqlnd_packet_sha256_pk_request {
	MYSQLND_PACKET_HEADER			header;
} MYSQLND_PACKET_SHA256_PK_REQUEST;

typedef struct  st_mysqlnd_packet_sha256_pk_request_response {
	MYSQLND_PACKET_HEADER	header;
	zend_uchar 				*public_key;
	size_t					public_key_len;
} MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE;

typedef struct st_mysqlnd_packet_cached_sha2_result {
	MYSQLND_PACKET_HEADER		header;
	uint8_t		response_code;
	uint8_t 	result;
	uint8_t 	request;
	zend_uchar * password;
	size_t password_len;
	/* Used for auth switch request */
	char		*new_auth_protocol;
	size_t		new_auth_protocol_len;
	zend_uchar	*new_auth_protocol_data;
	size_t		new_auth_protocol_data_len;
	/* Used for error result */
	char 		error[MYSQLND_ERRMSG_SIZE+1];
	char 		sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;
} MYSQLND_PACKET_CACHED_SHA2_RESULT;



zend_ulong		php_mysqlnd_net_field_length(const zend_uchar **packet);
zend_uchar *	php_mysqlnd_net_store_length(zend_uchar *packet, const uint64_t length);
size_t			php_mysqlnd_net_store_length_size(uint64_t length);

PHPAPI extern const char * const mysqlnd_empty_string;

enum_func_status php_mysqlnd_rowp_read_binary_protocol(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
										 unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
										 bool as_int_or_float, MYSQLND_STATS * stats);


enum_func_status php_mysqlnd_rowp_read_text_protocol(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
										 unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
										 bool as_int_or_float, MYSQLND_STATS * stats);


PHPAPI MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * mysqlnd_protocol_payload_decoder_factory_init(MYSQLND_CONN_DATA * conn, const bool persistent);
PHPAPI void mysqlnd_protocol_payload_decoder_factory_free(MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory);

#endif /* MYSQLND_WIREPROTOCOL_H */
