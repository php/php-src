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

/* $Id$ */

#ifndef MYSQLND_WIREPROTOCOL_H
#define MYSQLND_WIREPROTOCOL_H

#define MYSQLND_HEADER_SIZE 4

#define MYSQLND_NULL_LENGTH	(unsigned long) ~0

typedef zend_uchar	mysqlnd_1b;
typedef zend_ushort mysqlnd_2b;
typedef zend_uint	mysqlnd_4b;

/* Used in mysqlnd_debug.c */
extern char * mysqlnd_read_header_name;
extern char * mysqlnd_read_body_name;


/* Packet handling */
#define PACKET_INIT(packet, enum_type, c_type)  \
	{ \
		packet = (c_type) ecalloc(1, packet_methods[enum_type].struct_size); \
		((c_type) (packet))->header.m = &packet_methods[enum_type]; \
	}
#define PACKET_WRITE(packet, conn)	((packet)->header.m->write_to_net((packet), (conn) TSRMLS_CC))
#define PACKET_READ(packet, conn)	((packet)->header.m->read_from_net((packet), (conn) TSRMLS_CC))
#define PACKET_FREE(packet) \
	do { \
		((packet)->header.m->free_mem((packet), FALSE TSRMLS_CC)); \
	} while (0);

#define PACKET_INIT_ALLOCA(packet, enum_type)  \
	{ \
		memset(&(packet), 0, packet_methods[enum_type].struct_size); \
		(packet).header.m = &packet_methods[enum_type]; \
	}
#define PACKET_WRITE_ALLOCA(packet, conn)	PACKET_WRITE(&(packet), (conn))
#define PACKET_READ_ALLOCA(packet, conn)	PACKET_READ(&(packet), (conn))
#define PACKET_FREE_ALLOCA(packet)			(packet.header.m->free_mem(&(packet), TRUE TSRMLS_CC))

/* Enums */
enum php_mysql_packet_type
{
	PROT_GREET_PACKET= 0,
	PROT_AUTH_PACKET,
	PROT_OK_PACKET,
	PROT_EOF_PACKET,
	PROT_CMD_PACKET,
	PROT_RSET_HEADER_PACKET,
	PROT_RSET_FLD_PACKET,
	PROT_ROW_PACKET,
	PROT_STATS_PACKET,
	PROT_PREPARE_RESP_PACKET,
	PROT_CHG_USER_PACKET,
	PROT_LAST, /* should always be last */
};


enum php_mysqlnd_server_command
{
	COM_SLEEP = 0,
	COM_QUIT,
	COM_INIT_DB,
	COM_QUERY,
	COM_FIELD_LIST,
	COM_CREATE_DB,
	COM_DROP_DB,
	COM_REFRESH,
	COM_SHUTDOWN,
	COM_STATISTICS,
	COM_PROCESS_INFO,
	COM_CONNECT,
	COM_PROCESS_KILL,
	COM_DEBUG,
	COM_PING,
	COM_TIME = 15,
	COM_DELAYED_INSERT,
	COM_CHANGE_USER,
	COM_BINLOG_DUMP,
	COM_TABLE_DUMP,
	COM_CONNECT_OUT = 20,
	COM_REGISTER_SLAVE,
	COM_STMT_PREPARE = 22,
	COM_STMT_EXECUTE = 23,
	COM_STMT_SEND_LONG_DATA = 24,
	COM_STMT_CLOSE = 25,
	COM_STMT_RESET = 26,
	COM_SET_OPTION = 27,
	COM_STMT_FETCH = 28,
	COM_DAEMON,
	COM_END
};

extern const char * const mysqlnd_command_to_text[COM_END];

/* Low-level extraction functionality */
typedef struct st_mysqlnd_packet_methods {
	size_t				struct_size;
	enum_func_status	(*read_from_net)(void *packet, MYSQLND *conn TSRMLS_DC);
	size_t				(*write_to_net)(void *packet, MYSQLND *conn TSRMLS_DC);
	void				(*free_mem)(void *packet, zend_bool alloca TSRMLS_DC);
} mysqlnd_packet_methods;

extern mysqlnd_packet_methods packet_methods[];


typedef struct st_mysqlnd_packet_header {
	size_t		size;
	zend_uchar	packet_no;
	mysqlnd_packet_methods *m;
} mysqlnd_packet_header;

/* Server greets the client */
typedef struct st_php_mysql_packet_greet {
	mysqlnd_packet_header		header;
	mysqlnd_1b		protocol_version;
	char			*server_version;
	mysqlnd_4b		thread_id;
	zend_uchar		scramble_buf[SCRAMBLE_LENGTH];
	/* 1 byte pad */
	mysqlnd_2b		server_capabilities;
	mysqlnd_1b		charset_no;
	mysqlnd_2b		server_status;
	/* 13 byte pad*/
	zend_bool		pre41;
	/* If error packet, we use these */
	char 			error[MYSQLND_ERRMSG_SIZE+1];
	char 			sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;
} php_mysql_packet_greet;


/* Client authenticates */
typedef struct st_php_mysql_packet_auth {
	mysqlnd_packet_header		header;
	mysqlnd_4b	client_flags;
	uint32		max_packet_size;
	mysqlnd_1b	charset_no;
	/* 23 byte pad */
	const char	*user;
	/* 8 byte scramble */
	const char	*db;
	/* 12 byte scramble */

	/* Here the packet ends. This is user supplied data */
	const char	*password;
	/* +1 for \0 because of scramble() */
	unsigned char	*server_scramble_buf;
	size_t			db_len;
} php_mysql_packet_auth;

/* OK packet */
typedef struct st_php_mysql_packet_ok {
	mysqlnd_packet_header		header;
	mysqlnd_1b		field_count; /* always 0x0 */
	uint64	affected_rows;
	uint64	last_insert_id;
	mysqlnd_2b		server_status;
	mysqlnd_2b		warning_count;
	char			*message;
	size_t			message_len;
	/* If error packet, we use these */
	char 			error[MYSQLND_ERRMSG_SIZE+1];
	char 			sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;
} php_mysql_packet_ok;


/* Command packet */
typedef struct st_php_mysql_packet_command {
	mysqlnd_packet_header			header;
	enum php_mysqlnd_server_command	command;
	const char						*argument;
	size_t							arg_len;
} php_mysql_packet_command;


/* EOF packet */
typedef struct st_php_mysql_packet_eof {
	mysqlnd_packet_header		header;
	mysqlnd_1b		field_count; /* 0xFE */
	mysqlnd_2b		warning_count;
	mysqlnd_2b		server_status;
	/* If error packet, we use these */
	char 			error[MYSQLND_ERRMSG_SIZE+1];
	char 			sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int 	error_no;
} php_mysql_packet_eof;
/* EOF packet */


/* Result Set header*/
typedef struct st_php_mysql_packet_rset_header {
	mysqlnd_packet_header		header;
	/*
	  0x00 => ok
	  ~0   => LOAD DATA LOCAL
	  error_no != 0 => error
	  others => result set -> Read res_field packets up to field_count
	*/
	unsigned long		field_count;
	/*
	  These are filled if no SELECT query. For SELECT warning_count
	  and server status are in the last row packet, the EOF packet.
	*/
	mysqlnd_2b			warning_count;
	mysqlnd_2b			server_status;
	uint64		affected_rows;
	uint64		last_insert_id;
	/* This is for both LOAD DATA or info, when no result set */
	char				*info_or_local_file;
	size_t				info_or_local_file_len;
	/* If error packet, we use these */
	mysqlnd_error_info	error_info;
} php_mysql_packet_rset_header;


/* Result set field packet */
typedef struct st_php_mysql_packet_res_field {
	mysqlnd_packet_header	header;
	MYSQLND_FIELD			*metadata;
	/* For table definitions, empty for result sets */
	zend_bool				skip_parsing;
	zend_bool				stupid_list_fields_eof;
} php_mysql_packet_res_field;


/* Row packet */
struct st_php_mysql_packet_row {
	mysqlnd_packet_header	header;
	zval			**fields;
	mysqlnd_4b		field_count;
	zend_bool		eof;
	/*
	  These are, of course, only for SELECT in the EOF packet,
	  which is detected by this packet
	*/
	mysqlnd_2b		warning_count;
	mysqlnd_2b		server_status;

	struct st_mysqlnd_memory_pool_chunk	*row_buffer;

	zend_bool		skip_extraction;
	zend_bool		binary_protocol;
	zend_bool		persistent_alloc;
	MYSQLND_FIELD	*fields_metadata;
	/* We need this to alloc bigger bufs in non-PS mode */
	unsigned int	bit_fields_count;
	size_t			bit_fields_total_len; /* trailing \0 not counted */

	/* If error packet, we use these */
	mysqlnd_error_info	error_info;
};


/* Statistics packet */
typedef struct st_php_mysql_packet_stats {
	mysqlnd_packet_header	header;
	char *message;
	/* message_len is not part of the packet*/
	size_t message_len;
} php_mysql_packet_stats;


/* COM_PREPARE response packet */
typedef struct st_php_mysql_packet_prepare_response {
	mysqlnd_packet_header	header;
	/* also known as field_count 0x00=OK , 0xFF=error */
	unsigned char	error_code;
	unsigned long	stmt_id;
	unsigned int	field_count;
	unsigned int	param_count;
	unsigned int	warning_count;

	/* present in case of error */
	mysqlnd_error_info	error_info;
} php_mysql_packet_prepare_response;


/* Statistics packet */
typedef struct st_php_mysql_packet_chg_user_resp {
	mysqlnd_packet_header	header;
	mysqlnd_4b			field_count;
	
	/* message_len is not part of the packet*/
	mysqlnd_2b			server_capabilities;
	/* If error packet, we use these */
	mysqlnd_error_info	error_info;
} php_mysql_packet_chg_user_resp;


size_t mysqlnd_stream_write(MYSQLND * const conn, char * const buf, size_t count TSRMLS_DC);
size_t mysqlnd_stream_write_w_header(MYSQLND * const conn, char * const buf, size_t count TSRMLS_DC);

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
size_t php_mysqlnd_consume_uneaten_data(MYSQLND * const conn, enum php_mysqlnd_server_command cmd TSRMLS_DC);
#endif

void php_mysqlnd_scramble(zend_uchar * const buffer, const zend_uchar * const scramble, const zend_uchar * const pass);

unsigned long	php_mysqlnd_net_field_length(zend_uchar **packet);
zend_uchar *	php_mysqlnd_net_store_length(zend_uchar *packet, uint64 length);

extern char * const mysqlnd_empty_string;


void php_mysqlnd_rowp_read_binary_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
										   uint field_count, MYSQLND_FIELD *fields_metadata, MYSQLND *conn TSRMLS_DC);

void php_mysqlnd_rowp_read_text_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
										 uint field_count, MYSQLND_FIELD *fields_metadata, MYSQLND *conn TSRMLS_DC);

#endif /* MYSQLND_WIREPROTOCOL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
