/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef MYSQLND_STRUCTS_H
#define MYSQLND_STRUCTS_H

#include "zend_smart_str_public.h"

#define MYSQLND_TYPEDEFED_METHODS

#define MYSQLND_CLASS_METHOD_TABLE_NAME(class) mysqlnd_##class##_methods
#define MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(class) struct st_##class##_methods MYSQLND_CLASS_METHOD_TABLE_NAME(class)

#define MYSQLND_CLASS_METHODS_START(class)	MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(class) = {
#define MYSQLND_CLASS_METHODS_END			}


typedef struct st_mysqlnd_memory_pool MYSQLND_MEMORY_POOL;
typedef struct st_mysqlnd_memory_pool_chunk MYSQLND_MEMORY_POOL_CHUNK;
typedef struct st_mysqlnd_memory_pool_chunk_llist MYSQLND_MEMORY_POOL_CHUNK_LLIST;


#define MYSQLND_MEMORY_POOL_CHUNK_LIST_SIZE 100

struct st_mysqlnd_memory_pool
{
	zend_uchar *arena;
	unsigned int refcount;
	unsigned int arena_size;
	unsigned int free_size;

	MYSQLND_MEMORY_POOL_CHUNK*	(*get_chunk)(MYSQLND_MEMORY_POOL * pool, unsigned int size);
};

struct st_mysqlnd_memory_pool_chunk
{
	size_t				app;
	MYSQLND_MEMORY_POOL	*pool;
	zend_uchar			*ptr;
	enum_func_status	(*resize_chunk)(MYSQLND_MEMORY_POOL_CHUNK * chunk, unsigned int size);
	void				(*free_chunk)(MYSQLND_MEMORY_POOL_CHUNK * chunk);
	unsigned int		size;
	zend_bool			from_pool;
};


typedef struct st_mysqlnd_cmd_buffer
{
	zend_uchar		*buffer;
	size_t			length;
} MYSQLND_CMD_BUFFER;


typedef struct st_mysqlnd_field
{
	zend_string *sname;			/* Name of column */
	const char  *name;          /* Name of column in C string */
	const char  *org_name;		/* Original column name, if an alias */
	const char  *table;			/* Table of column if column was a field */
	const char  *org_table;		/* Org table name, if table was an alias */
	const char  *db;			/* Database for table */
	const char  *catalog;		/* Catalog for table */
	char  *def;                 /* Default value (set by mysql_list_fields) */
	zend_ulong length;		/* Width of column (create length) */
	zend_ulong max_length;	/* Max width for selected set */
	unsigned int name_length;
	unsigned int org_name_length;
	unsigned int table_length;
	unsigned int org_table_length;
	unsigned int db_length;
	unsigned int catalog_length;
	unsigned int def_length;
	unsigned int flags;			/* Diverse flags */
	unsigned int decimals;		/* Number of decimals in field */
	unsigned int charsetnr;		/* Character set */
	enum mysqlnd_field_types type;	/* Type of field. See mysql_com.h for types */
	char *root;
	size_t root_len;
} MYSQLND_FIELD;


typedef struct st_mysqlnd_upsert_result
{
	unsigned int	warning_count;
	unsigned int	server_status;
	uint64_t		affected_rows;
	uint64_t		last_insert_id;
} MYSQLND_UPSERT_STATUS;


typedef struct st_mysqlnd_error_info
{
	char error[MYSQLND_ERRMSG_SIZE+1];
	char sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int error_no;
	zend_llist * error_list;
} MYSQLND_ERROR_INFO;


typedef struct st_mysqlnd_error_list_element
{
	char * error;
	char sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int error_no;
} MYSQLND_ERROR_LIST_ELEMENT;


typedef struct st_mysqlnd_infile_info
{
	php_stream	*fd;
	int			error_no;
	char		error_msg[MYSQLND_ERRMSG_SIZE + 1];
	const char	*filename;
} MYSQLND_INFILE_INFO;


/* character set information */
typedef struct st_mysqlnd_charset
{
	unsigned int	nr;
	const char		*name;
	const char		*collation;
	unsigned int	char_minlen;
	unsigned int	char_maxlen;
	const char		*comment;
	unsigned int 	(*mb_charlen)(unsigned int c);
	unsigned int 	(*mb_valid)(const char *start, const char *end);
} MYSQLND_CHARSET;


/* local infile handler */
typedef struct st_mysqlnd_infile
{
	int		(*local_infile_init)(void **ptr, const char * const filename);
	int		(*local_infile_read)(void *ptr, zend_uchar * buf, unsigned int buf_len);
	int		(*local_infile_error)(void *ptr, char * error_msg, unsigned int error_msg_len);
	void	(*local_infile_end)(void *ptr);
} MYSQLND_INFILE;

typedef struct st_mysqlnd_options
{
	ulong		flags;

	/* init commands - we need to send them to server directly after connect */
	unsigned int	num_commands;
	char			**init_commands;

	/* configuration file information */
	char 		*cfg_file;
	char		*cfg_section;

	char		*auth_protocol;
	/*
	  We need to keep these because otherwise st_mysqlnd_conn will be changed.
	  The ABI will be broken and the methods structure will be somewhere else
	  in the memory which can crash external code. Feel free to reuse these.
	*/
	HashTable	* connect_attr;
	char		* unused3;
	char		* unused4;
	char		* unused5;

	enum_mysqlnd_protocol_type protocol;

	char 		*charset_name;
	/* maximum allowed packet size for communication */
	ulong		max_allowed_packet;

#ifdef MYSQLND_STRING_TO_INT_CONVERSION
	zend_bool	int_and_float_native;
#endif
} MYSQLND_OPTIONS;

typedef struct st_mysqlnd_net_options
{
	/* timeouts */
	unsigned int timeout_connect;
	unsigned int timeout_read;
	unsigned int timeout_write;

	size_t		net_read_buffer_size;

	/* SSL information */
	char		*ssl_key;
	char		*ssl_cert;
	char		*ssl_ca;
	char		*ssl_capath;
	char		*ssl_cipher;
	char		*ssl_passphrase;
	zend_bool	ssl_verify_peer;
	uint64_t	flags;

	char *		sha256_server_public_key;

	char *		unused1;
	char *		unused2;
	char *		unused3;
	char *		unused4;
} MYSQLND_NET_OPTIONS;


typedef struct st_mysqlnd_connection MYSQLND;
typedef struct st_mysqlnd_connection_data MYSQLND_CONN_DATA;
typedef struct st_mysqlnd_net	MYSQLND_NET;
typedef struct st_mysqlnd_net_data	MYSQLND_NET_DATA;
typedef struct st_mysqlnd_protocol	MYSQLND_PROTOCOL;
typedef struct st_mysqlnd_res	MYSQLND_RES;
typedef char** 					MYSQLND_ROW_C;		/* return data as array of strings */
typedef struct st_mysqlnd_stmt_data	MYSQLND_STMT_DATA;
typedef struct st_mysqlnd_stmt	MYSQLND_STMT;
typedef unsigned int			MYSQLND_FIELD_OFFSET;

typedef struct st_mysqlnd_param_bind MYSQLND_PARAM_BIND;

typedef struct st_mysqlnd_result_bind MYSQLND_RESULT_BIND;

typedef struct st_mysqlnd_result_metadata MYSQLND_RES_METADATA;
typedef struct st_mysqlnd_buffered_result_parent MYSQLND_RES_BUFFERED;
typedef struct st_mysqlnd_buffered_result_zval MYSQLND_RES_BUFFERED_ZVAL;
typedef struct st_mysqlnd_buffered_result_c MYSQLND_RES_BUFFERED_C;
typedef struct st_mysqlnd_unbuffered_result MYSQLND_RES_UNBUFFERED;

typedef struct st_mysqlnd_debug MYSQLND_DEBUG;


typedef MYSQLND_RES* (*mysqlnd_stmt_use_or_store_func)(MYSQLND_STMT * const);
typedef enum_func_status  (*mysqlnd_fetch_row_func)(MYSQLND_RES *result,
													void * param,
													const unsigned int flags,
													zend_bool * fetched_anything
													);


typedef struct st_mysqlnd_stats MYSQLND_STATS;

typedef void (*mysqlnd_stat_trigger)(MYSQLND_STATS * stats, enum_mysqlnd_collected_stats stat, int64_t change);

struct st_mysqlnd_stats
{
	uint64_t				*values;
	mysqlnd_stat_trigger	*triggers;
	size_t					count;
	zend_bool				in_trigger;
#ifdef ZTS
	MUTEX_T	LOCK_access;
#endif
};


typedef struct st_mysqlnd_read_buffer {
	zend_uchar 	* data;
	size_t 		offset;
	size_t 		size;
	size_t		len;
	zend_bool	(*is_empty)(struct st_mysqlnd_read_buffer *);
	void		(*read)(struct st_mysqlnd_read_buffer *, size_t count, zend_uchar * dest);
	size_t		(*bytes_left)(struct st_mysqlnd_read_buffer *);
	void		(*free_buffer)(struct st_mysqlnd_read_buffer **);
} MYSQLND_READ_BUFFER;



typedef enum_func_status	(*func_mysqlnd_net__set_client_option)(MYSQLND_NET * const net, enum_mysqlnd_option option, const char * const value);
typedef enum_func_status	(*func_mysqlnd_net__decode)(zend_uchar * uncompressed_data, const size_t uncompressed_data_len, const zend_uchar * const compressed_data, const size_t compressed_data_len);
typedef enum_func_status	(*func_mysqlnd_net__encode)(zend_uchar * compress_buffer, size_t * compress_buffer_len, const zend_uchar * const uncompressed_data, const size_t uncompressed_data_len);
typedef size_t				(*func_mysqlnd_net__consume_uneaten_data)(MYSQLND_NET * const net, enum php_mysqlnd_server_command cmd);
typedef void				(*func_mysqlnd_net__free_contents)(MYSQLND_NET * net);
typedef enum_func_status	(*func_mysqlnd_net__enable_ssl)(MYSQLND_NET * const net);
typedef enum_func_status	(*func_mysqlnd_net__disable_ssl)(MYSQLND_NET * const net);
typedef enum_func_status	(*func_mysqlnd_net__network_read_ex)(MYSQLND_NET * const net, zend_uchar * const buffer, const size_t count, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef size_t				(*func_mysqlnd_net__network_write_ex)(MYSQLND_NET * const net, const zend_uchar * const buf, const size_t count, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef size_t				(*func_mysqlnd_net__send_ex)(MYSQLND_NET * const net, zend_uchar * const buffer, const size_t count, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_net__receive_ex)(MYSQLND_NET * const net, zend_uchar * const buffer, const size_t count, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_net__init)(MYSQLND_NET * const net, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef void				(*func_mysqlnd_net__dtor)(MYSQLND_NET * const net, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_net__connect_ex)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len, const zend_bool persistent, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef void				(*func_mysqlnd_net__close_stream)(MYSQLND_NET * const net, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef php_stream *		(*func_mysqlnd_net__open_stream)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len, const zend_bool persistent, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef php_stream *		(*func_mysqlnd_net__get_stream)(const MYSQLND_NET * const net);
typedef php_stream *		(*func_mysqlnd_net__set_stream)(MYSQLND_NET * const net, php_stream * net_stream);
typedef func_mysqlnd_net__open_stream (*func_mysqlnd_net__get_open_stream)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len, MYSQLND_ERROR_INFO * const error_info);
typedef void				(*func_mysqlnd_net__post_connect_set_opt)(MYSQLND_NET * const net, const char * const scheme, const size_t scheme_len, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_net__read_compressed_packet_from_stream_and_fill_read_buffer)(MYSQLND_NET * net, size_t net_payload_size, MYSQLND_STATS * conn_stats, MYSQLND_ERROR_INFO * error_info);

struct st_mysqlnd_net_methods
{
	func_mysqlnd_net__init init;
	func_mysqlnd_net__dtor dtor;
	func_mysqlnd_net__connect_ex connect_ex;
	func_mysqlnd_net__close_stream close_stream;
	func_mysqlnd_net__open_stream open_pipe;
	func_mysqlnd_net__open_stream open_tcp_or_unix;

	func_mysqlnd_net__get_stream get_stream;
	func_mysqlnd_net__set_stream set_stream;
	func_mysqlnd_net__get_open_stream get_open_stream;

	func_mysqlnd_net__post_connect_set_opt post_connect_set_opt;

	func_mysqlnd_net__set_client_option set_client_option;
	func_mysqlnd_net__decode decode;
	func_mysqlnd_net__encode encode;
	func_mysqlnd_net__consume_uneaten_data consume_uneaten_data;
	func_mysqlnd_net__free_contents free_contents;
	func_mysqlnd_net__enable_ssl enable_ssl;
	func_mysqlnd_net__disable_ssl disable_ssl;

	func_mysqlnd_net__network_read_ex network_read_ex;
	func_mysqlnd_net__network_write_ex network_write_ex;
	func_mysqlnd_net__send_ex send_ex;
	func_mysqlnd_net__receive_ex receive_ex;

	func_mysqlnd_net__read_compressed_packet_from_stream_and_fill_read_buffer read_compressed_packet_from_stream_and_fill_read_buffer;

	void * unused1;
	void * unused2;
	void * unused3;
	void * unused4;
	void * unused5;
};


struct st_mysqlnd_packet_greet;
struct st_mysqlnd_packet_greet;
struct st_mysqlnd_packet_auth;
struct st_mysqlnd_packet_ok;
struct st_mysqlnd_packet_command;
struct st_mysqlnd_packet_eof;
struct st_mysqlnd_packet_rset_header;
struct st_mysqlnd_packet_res_field;
struct st_mysqlnd_packet_row;
struct st_mysqlnd_packet_stats;
struct st_mysqlnd_packet_prepare_response;
struct st_mysqlnd_packet_chg_user_resp;
struct st_mysqlnd_packet_auth_pam;
struct st_mysqlnd_packet_sha256_pk_request;
struct st_mysqlnd_packet_sha256_pk_request_response;

typedef struct st_mysqlnd_packet_greet *		(*func_mysqlnd_protocol__get_greet_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_auth *			(*func_mysqlnd_protocol__get_auth_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_auth_response *(*func_mysqlnd_protocol__get_auth_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_change_auth_response *	(*func_mysqlnd_protocol__get_change_auth_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_ok *			(*func_mysqlnd_protocol__get_ok_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_command *		(*func_mysqlnd_protocol__get_command_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_eof *			(*func_mysqlnd_protocol__get_eof_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_rset_header *	(*func_mysqlnd_protocol__get_rset_header_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_res_field *	(*func_mysqlnd_protocol__get_result_field_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_row *			(*func_mysqlnd_protocol__get_row_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_stats *		(*func_mysqlnd_protocol__get_stats_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_prepare_response *(*func_mysqlnd_protocol__get_prepare_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_chg_user_resp*(*func_mysqlnd_protocol__get_change_user_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_sha256_pk_request *(*func_mysqlnd_protocol__get_sha256_pk_request_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);
typedef struct st_mysqlnd_packet_sha256_pk_request_response *(*func_mysqlnd_protocol__get_sha256_pk_request_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent);

struct st_mysqlnd_protocol_methods
{
	func_mysqlnd_protocol__get_greet_packet get_greet_packet;
	func_mysqlnd_protocol__get_auth_packet get_auth_packet;
	func_mysqlnd_protocol__get_auth_response_packet get_auth_response_packet;
	func_mysqlnd_protocol__get_change_auth_response_packet get_change_auth_response_packet;
	func_mysqlnd_protocol__get_ok_packet get_ok_packet;
	func_mysqlnd_protocol__get_command_packet get_command_packet;
	func_mysqlnd_protocol__get_eof_packet get_eof_packet;
	func_mysqlnd_protocol__get_rset_header_packet get_rset_header_packet;
	func_mysqlnd_protocol__get_result_field_packet get_result_field_packet;
	func_mysqlnd_protocol__get_row_packet get_row_packet;
	func_mysqlnd_protocol__get_stats_packet get_stats_packet;
	func_mysqlnd_protocol__get_prepare_response_packet get_prepare_response_packet;
	func_mysqlnd_protocol__get_change_user_response_packet get_change_user_response_packet;
	func_mysqlnd_protocol__get_sha256_pk_request_packet get_sha256_pk_request_packet;
	func_mysqlnd_protocol__get_sha256_pk_request_response_packet get_sha256_pk_request_response_packet;

	void * unused1;
	void * unused2;
	void * unused3;
};


typedef MYSQLND * (*func_mysqlnd_object_factory__get_connection)(zend_bool persistent);
typedef MYSQLND * (*func_mysqlnd_object_factory__clone_connection_object)(MYSQLND * conn);
typedef MYSQLND_STMT * (*func_mysqlnd_object_factory__get_prepared_statement)(MYSQLND_CONN_DATA * conn);
typedef MYSQLND_NET * (*func_mysqlnd_object_factory__get_io_channel)(zend_bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);
typedef MYSQLND_PROTOCOL * (*func_mysqlnd_object_factory__get_protocol_decoder)(zend_bool persistent);


struct st_mysqlnd_object_factory_methods
{
	func_mysqlnd_object_factory__get_connection get_connection;
	func_mysqlnd_object_factory__clone_connection_object clone_connection_object;
	func_mysqlnd_object_factory__get_prepared_statement get_prepared_statement;
	func_mysqlnd_object_factory__get_io_channel get_io_channel;
	func_mysqlnd_object_factory__get_protocol_decoder get_protocol_decoder;
};


typedef enum_func_status	(*func_mysqlnd_conn_data__init)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__connect)(MYSQLND_CONN_DATA * conn, const char * host, const char * user, const char * passwd, unsigned int passwd_len, const char * db, unsigned int db_len, unsigned int port, const char * socket_or_pipe, unsigned int mysql_flags);
typedef zend_ulong				(*func_mysqlnd_conn_data__escape_string)(MYSQLND_CONN_DATA * const conn, char *newstr, const char *escapestr, size_t escapestr_len);
typedef enum_func_status	(*func_mysqlnd_conn_data__set_charset)(MYSQLND_CONN_DATA * const conn, const char * const charset);
typedef enum_func_status	(*func_mysqlnd_conn_data__query)(MYSQLND_CONN_DATA * conn, const char * query, unsigned int query_len);
typedef enum_func_status	(*func_mysqlnd_conn_data__send_query)(MYSQLND_CONN_DATA * conn, const char *query, unsigned int query_len, enum_mysqlnd_send_query_type type, zval *read_cb, zval *err_cb);
typedef enum_func_status	(*func_mysqlnd_conn_data__reap_query)(MYSQLND_CONN_DATA * conn, enum_mysqlnd_reap_result_type type);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__use_result)(MYSQLND_CONN_DATA * const conn, const unsigned int flags);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__store_result)(MYSQLND_CONN_DATA * const conn, const unsigned int flags);
typedef enum_func_status	(*func_mysqlnd_conn_data__next_result)(MYSQLND_CONN_DATA * const conn);
typedef zend_bool			(*func_mysqlnd_conn_data__more_results)(const MYSQLND_CONN_DATA * const conn);

typedef MYSQLND_STMT *		(*func_mysqlnd_conn_data__stmt_init)(MYSQLND_CONN_DATA * const conn);

typedef enum_func_status	(*func_mysqlnd_conn_data__shutdown_server)(MYSQLND_CONN_DATA * const conn, uint8_t level);
typedef enum_func_status	(*func_mysqlnd_conn_data__refresh_server)(MYSQLND_CONN_DATA * const conn, uint8_t options);

typedef enum_func_status	(*func_mysqlnd_conn_data__ping)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__kill_connection)(MYSQLND_CONN_DATA * conn, unsigned int pid);
typedef enum_func_status	(*func_mysqlnd_conn_data__select_db)(MYSQLND_CONN_DATA * const conn, const char * const db, unsigned int db_len);
typedef enum_func_status	(*func_mysqlnd_conn_data__server_dump_debug_information)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__change_user)(MYSQLND_CONN_DATA * const conn, const char * user, const char * passwd, const char * db, zend_bool silent, size_t passwd_len);

typedef unsigned int		(*func_mysqlnd_conn_data__get_error_no)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_error_str)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_sqlstate)(const MYSQLND_CONN_DATA * const conn);
typedef uint64_t			(*func_mysqlnd_conn_data__get_thread_id)(const MYSQLND_CONN_DATA * const conn);
typedef void				(*func_mysqlnd_conn_data__get_statistics)(const MYSQLND_CONN_DATA * const conn, zval *return_value ZEND_FILE_LINE_DC);

typedef zend_ulong		(*func_mysqlnd_conn_data__get_server_version)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_server_information)(const MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__get_server_statistics)(MYSQLND_CONN_DATA * conn, zend_string **message);
typedef const char *		(*func_mysqlnd_conn_data__get_host_information)(const MYSQLND_CONN_DATA * const conn);
typedef unsigned int		(*func_mysqlnd_conn_data__get_protocol_information)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_last_message)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__charset_name)(const MYSQLND_CONN_DATA * const conn);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__list_fields)(MYSQLND_CONN_DATA * conn, const char * table, const char * achtung_wild);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__list_method)(MYSQLND_CONN_DATA * conn, const char * query, const char * achtung_wild, char *par1);

typedef uint64_t			(*func_mysqlnd_conn_data__get_last_insert_id)(const MYSQLND_CONN_DATA * const conn);
typedef uint64_t			(*func_mysqlnd_conn_data__get_affected_rows)(const MYSQLND_CONN_DATA * const conn);
typedef unsigned int		(*func_mysqlnd_conn_data__get_warning_count)(const MYSQLND_CONN_DATA * const conn);

typedef unsigned int		(*func_mysqlnd_conn_data__get_field_count)(const MYSQLND_CONN_DATA * const conn);

typedef unsigned int		(*func_mysqlnd_conn_data__get_server_status)(const MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__set_server_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_server_option option);
typedef enum_func_status	(*func_mysqlnd_conn_data__set_client_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_option option, const char * const value);
typedef void				(*func_mysqlnd_conn_data__free_contents)(MYSQLND_CONN_DATA * conn);/* private */
typedef void				(*func_mysqlnd_conn_data__free_options)(MYSQLND_CONN_DATA * conn);	/* private */
typedef void				(*func_mysqlnd_conn_data__dtor)(MYSQLND_CONN_DATA * conn);	/* private */

typedef enum_func_status	(*func_mysqlnd_conn_data__query_read_result_set_header)(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * stmt);

typedef MYSQLND_CONN_DATA *	(*func_mysqlnd_conn_data__get_reference)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__free_reference)(MYSQLND_CONN_DATA * const conn);
typedef enum mysqlnd_connection_state (*func_mysqlnd_conn_data__get_state)(const MYSQLND_CONN_DATA * const conn);
typedef void				(*func_mysqlnd_conn_data__set_state)(MYSQLND_CONN_DATA * const conn, enum mysqlnd_connection_state new_state);

typedef enum_func_status	(*func_mysqlnd_conn_data__simple_command)(MYSQLND_CONN_DATA * conn, enum php_mysqlnd_server_command command, const zend_uchar * const arg, size_t arg_len, enum mysqlnd_packet_type ok_packet, zend_bool silent, zend_bool ignore_upsert_status);
typedef enum_func_status	(*func_mysqlnd_conn_data__simple_command_handle_response)(MYSQLND_CONN_DATA * conn, enum mysqlnd_packet_type ok_packet, zend_bool silent, enum php_mysqlnd_server_command command, zend_bool ignore_upsert_status);

typedef enum_func_status	(*func_mysqlnd_conn_data__restart_psession)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__end_psession)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__send_close)(MYSQLND_CONN_DATA * conn);

typedef enum_func_status    (*func_mysqlnd_conn_data__ssl_set)(MYSQLND_CONN_DATA * const conn, const char * key, const char * const cert, const char * const ca, const char * const capath, const char * const cipher);

typedef MYSQLND_RES * 		(*func_mysqlnd_conn_data__result_init)(unsigned int field_count, zend_bool persistent);

typedef enum_func_status	(*func_mysqlnd_conn_data__set_autocommit)(MYSQLND_CONN_DATA * conn, unsigned int mode);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_commit)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_rollback)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_begin)(MYSQLND_CONN_DATA * conn, const unsigned int mode, const char * const name);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_commit_or_rollback)(MYSQLND_CONN_DATA * conn, const zend_bool commit, const unsigned int flags, const char * const name);
typedef void				(*func_mysqlnd_conn_data__tx_cor_options_to_string)(const MYSQLND_CONN_DATA * const conn, smart_str * tmp_str, const unsigned int mode);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_savepoint)(MYSQLND_CONN_DATA * conn, const char * const name);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_savepoint_release)(MYSQLND_CONN_DATA * conn, const char * const name);

typedef enum_func_status	(*func_mysqlnd_conn_data__local_tx_start)(MYSQLND_CONN_DATA * conn, size_t this_func);
typedef enum_func_status	(*func_mysqlnd_conn_data__local_tx_end)(MYSQLND_CONN_DATA * conn, size_t this_func, enum_func_status status);
typedef enum_func_status	(*func_mysqlnd_conn_data__execute_init_commands)(MYSQLND_CONN_DATA * conn);
typedef unsigned int		(*func_mysqlnd_conn_data__get_updated_connect_flags)(MYSQLND_CONN_DATA * conn, unsigned int mysql_flags);
typedef enum_func_status	(*func_mysqlnd_conn_data__connect_handshake)(MYSQLND_CONN_DATA * conn, const char * const host, const char * const user, const char * const passwd, const unsigned int passwd_len, const char * const db, const unsigned int db_len, const unsigned int mysql_flags);
typedef enum_func_status	(*func_mysqlnd_conn_data__simple_command_send_request)(MYSQLND_CONN_DATA * conn, enum php_mysqlnd_server_command command, const zend_uchar * const arg, size_t arg_len, zend_bool silent, zend_bool ignore_upsert_status);
typedef struct st_mysqlnd_authentication_plugin * (*func_mysqlnd_conn_data__fetch_auth_plugin_by_name)(const char * const requested_protocol);

typedef enum_func_status	(*func_mysqlnd_conn_data__set_client_option_2d)(MYSQLND_CONN_DATA * const conn, enum mysqlnd_option option, const char * const key, const char * const value);


typedef unsigned int		(*func_mysqlnd_conn_data__negotiate_client_api_capabilities)(MYSQLND_CONN_DATA * const conn, const unsigned int flags);
typedef unsigned int		(*func_mysqlnd_conn_data__get_client_api_capabilities)(const MYSQLND_CONN_DATA * const conn);


struct st_mysqlnd_conn_data_methods
{
	func_mysqlnd_conn_data__init init;
	func_mysqlnd_conn_data__connect connect;
	func_mysqlnd_conn_data__escape_string escape_string;
	func_mysqlnd_conn_data__set_charset set_charset;
	func_mysqlnd_conn_data__query query;
	func_mysqlnd_conn_data__send_query send_query;
	func_mysqlnd_conn_data__reap_query reap_query;
	func_mysqlnd_conn_data__use_result use_result;
	func_mysqlnd_conn_data__store_result store_result;
	func_mysqlnd_conn_data__next_result next_result;
	func_mysqlnd_conn_data__more_results more_results;

	func_mysqlnd_conn_data__stmt_init stmt_init;

	func_mysqlnd_conn_data__shutdown_server shutdown_server;
	func_mysqlnd_conn_data__refresh_server refresh_server;

	func_mysqlnd_conn_data__ping ping;
	func_mysqlnd_conn_data__kill_connection kill_connection;
	func_mysqlnd_conn_data__select_db select_db;
	func_mysqlnd_conn_data__server_dump_debug_information server_dump_debug_information;
	func_mysqlnd_conn_data__change_user change_user;

	func_mysqlnd_conn_data__get_error_no get_error_no;
	func_mysqlnd_conn_data__get_error_str get_error_str;
	func_mysqlnd_conn_data__get_sqlstate get_sqlstate;
	func_mysqlnd_conn_data__get_thread_id get_thread_id;
	func_mysqlnd_conn_data__get_statistics get_statistics;

	func_mysqlnd_conn_data__get_server_version get_server_version;
	func_mysqlnd_conn_data__get_server_information get_server_information;
	func_mysqlnd_conn_data__get_server_statistics get_server_statistics;
	func_mysqlnd_conn_data__get_host_information get_host_information;
	func_mysqlnd_conn_data__get_protocol_information get_protocol_information;
	func_mysqlnd_conn_data__get_last_message get_last_message;
	func_mysqlnd_conn_data__charset_name charset_name;
	func_mysqlnd_conn_data__list_fields list_fields;
	func_mysqlnd_conn_data__list_method list_method;

	func_mysqlnd_conn_data__get_last_insert_id get_last_insert_id;
	func_mysqlnd_conn_data__get_affected_rows get_affected_rows;
	func_mysqlnd_conn_data__get_warning_count get_warning_count;

	func_mysqlnd_conn_data__get_field_count get_field_count;

	func_mysqlnd_conn_data__get_server_status get_server_status;

	func_mysqlnd_conn_data__set_server_option set_server_option;
	func_mysqlnd_conn_data__set_client_option set_client_option;
	func_mysqlnd_conn_data__free_contents free_contents;
	func_mysqlnd_conn_data__free_options free_options;
	func_mysqlnd_conn_data__dtor dtor;

	func_mysqlnd_conn_data__query_read_result_set_header query_read_result_set_header;

	func_mysqlnd_conn_data__get_reference get_reference;
	func_mysqlnd_conn_data__free_reference free_reference;
	func_mysqlnd_conn_data__get_state get_state;
	func_mysqlnd_conn_data__set_state set_state;

	func_mysqlnd_conn_data__simple_command simple_command;
	func_mysqlnd_conn_data__simple_command_handle_response simple_command_handle_response;

	func_mysqlnd_conn_data__restart_psession restart_psession;
	func_mysqlnd_conn_data__end_psession end_psession;
	func_mysqlnd_conn_data__send_close send_close;

	func_mysqlnd_conn_data__ssl_set ssl_set;

	func_mysqlnd_conn_data__result_init result_init;
	func_mysqlnd_conn_data__set_autocommit set_autocommit;
	func_mysqlnd_conn_data__tx_commit tx_commit;
	func_mysqlnd_conn_data__tx_rollback tx_rollback;
	func_mysqlnd_conn_data__tx_begin tx_begin;
	func_mysqlnd_conn_data__tx_commit_or_rollback tx_commit_or_rollback;
	func_mysqlnd_conn_data__tx_cor_options_to_string tx_cor_options_to_string;
	func_mysqlnd_conn_data__tx_savepoint tx_savepoint;
	func_mysqlnd_conn_data__tx_savepoint_release tx_savepoint_release;

	func_mysqlnd_conn_data__local_tx_start local_tx_start;
	func_mysqlnd_conn_data__local_tx_end local_tx_end;

	func_mysqlnd_conn_data__execute_init_commands execute_init_commands;
	func_mysqlnd_conn_data__get_updated_connect_flags get_updated_connect_flags;
	func_mysqlnd_conn_data__connect_handshake connect_handshake;
	func_mysqlnd_conn_data__simple_command_send_request simple_command_send_request;
	func_mysqlnd_conn_data__fetch_auth_plugin_by_name fetch_auth_plugin_by_name;

	func_mysqlnd_conn_data__set_client_option_2d set_client_option_2d;

	func_mysqlnd_conn_data__negotiate_client_api_capabilities negotiate_client_api_capabilities;
	func_mysqlnd_conn_data__get_client_api_capabilities get_client_api_capabilities;
};


typedef enum_func_status	(*func_mysqlnd_data__connect)(MYSQLND * conn, const char * host, const char * user, const char * passwd, unsigned int passwd_len, const char * db, unsigned int db_len, unsigned int port, const char * socket_or_pipe, unsigned int mysql_flags);
typedef MYSQLND *			(*func_mysqlnd_conn__clone_object)(MYSQLND * const conn);
typedef void				(*func_mysqlnd_conn__dtor)(MYSQLND * conn);
typedef enum_func_status	(*func_mysqlnd_conn__close)(MYSQLND * conn, enum_connection_close_type close_type);

struct st_mysqlnd_conn_methods
{
	func_mysqlnd_data__connect connect;
	func_mysqlnd_conn__clone_object clone_object;
	func_mysqlnd_conn__dtor dtor;
	func_mysqlnd_conn__close close;
};

	/* for decoding - binary or text protocol */
typedef enum_func_status	(*func_mysqlnd_res__row_decoder)(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval * fields,
									unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
									zend_bool as_int_or_float, MYSQLND_STATS * stats);


typedef MYSQLND_RES *		(*func_mysqlnd_res__use_result)(MYSQLND_RES * const result, zend_bool ps_protocol);
typedef MYSQLND_RES *		(*func_mysqlnd_res__store_result)(MYSQLND_RES * result, MYSQLND_CONN_DATA * const conn, const unsigned int flags);
typedef void 				(*func_mysqlnd_res__fetch_into)(MYSQLND_RES *result, const unsigned int flags, zval *return_value, enum_mysqlnd_extension ext ZEND_FILE_LINE_DC);
typedef MYSQLND_ROW_C 		(*func_mysqlnd_res__fetch_row_c)(MYSQLND_RES *result);
typedef void 				(*func_mysqlnd_res__fetch_all)(MYSQLND_RES *result, const unsigned int flags, zval *return_value ZEND_FILE_LINE_DC);
typedef void 				(*func_mysqlnd_res__fetch_field_data)(MYSQLND_RES *result, unsigned int offset, zval *return_value);
typedef uint64_t			(*func_mysqlnd_res__num_rows)(const MYSQLND_RES * const result);
typedef unsigned int		(*func_mysqlnd_res__num_fields)(const MYSQLND_RES * const result);
typedef enum_func_status	(*func_mysqlnd_res__skip_result)(MYSQLND_RES * const result);
typedef enum_func_status	(*func_mysqlnd_res__seek_data)(MYSQLND_RES * const result, const uint64_t row);
typedef MYSQLND_FIELD_OFFSET (*func_mysqlnd_res__seek_field)(MYSQLND_RES * const result, const MYSQLND_FIELD_OFFSET field_offset);
typedef MYSQLND_FIELD_OFFSET (*func_mysqlnd_res__field_tell)(const MYSQLND_RES * const result);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_field)(MYSQLND_RES * const result);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_field_direct)(MYSQLND_RES * const result, const MYSQLND_FIELD_OFFSET fieldnr);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_fields)(MYSQLND_RES * const result);

typedef enum_func_status	(*func_mysqlnd_res__read_result_metadata)(MYSQLND_RES * result, MYSQLND_CONN_DATA * conn);
typedef zend_ulong *		(*func_mysqlnd_res__fetch_lengths)(MYSQLND_RES * const result);
typedef enum_func_status	(*func_mysqlnd_res__store_result_fetch_data)(MYSQLND_CONN_DATA * const conn, MYSQLND_RES * result, MYSQLND_RES_METADATA * meta, MYSQLND_MEMORY_POOL_CHUNK *** row_buffers, zend_bool binary_protocol);

typedef void				(*func_mysqlnd_res__free_result_buffers)(MYSQLND_RES * result);	/* private */
typedef enum_func_status	(*func_mysqlnd_res__free_result)(MYSQLND_RES * result, zend_bool implicit);
typedef void				(*func_mysqlnd_res__free_result_internal)(MYSQLND_RES *result);
typedef void				(*func_mysqlnd_res__free_result_contents)(MYSQLND_RES *result);
typedef void				(*func_mysqlnd_res__free_buffered_data)(MYSQLND_RES *result);
typedef void				(*func_mysqlnd_res__unbuffered_free_last_data)(MYSQLND_RES *result);


typedef MYSQLND_RES_METADATA * (*func_mysqlnd_res__result_meta_init)(unsigned int field_count, zend_bool persistent);

struct st_mysqlnd_res_methods
{
	mysqlnd_fetch_row_func	fetch_row;

	func_mysqlnd_res__use_result use_result;
	func_mysqlnd_res__store_result store_result;
	func_mysqlnd_res__fetch_into fetch_into;
	func_mysqlnd_res__fetch_row_c fetch_row_c;
	func_mysqlnd_res__fetch_all fetch_all;
	func_mysqlnd_res__fetch_field_data fetch_field_data;
	func_mysqlnd_res__num_rows num_rows;
	func_mysqlnd_res__num_fields num_fields;
	func_mysqlnd_res__skip_result skip_result;
	func_mysqlnd_res__seek_data seek_data;
	func_mysqlnd_res__seek_field seek_field;
	func_mysqlnd_res__field_tell field_tell;
	func_mysqlnd_res__fetch_field fetch_field;
	func_mysqlnd_res__fetch_field_direct fetch_field_direct;
	func_mysqlnd_res__fetch_fields fetch_fields;
	func_mysqlnd_res__read_result_metadata read_result_metadata;
	func_mysqlnd_res__fetch_lengths fetch_lengths;
	func_mysqlnd_res__store_result_fetch_data store_result_fetch_data;
	func_mysqlnd_res__free_result_buffers free_result_buffers;
	func_mysqlnd_res__free_result free_result;
	func_mysqlnd_res__free_result_internal free_result_internal;
	func_mysqlnd_res__free_result_contents free_result_contents;

	func_mysqlnd_res__result_meta_init result_meta_init;

	void * unused1;
	void * unused2;
	void * unused3;
	void * unused4;
	void * unused5;
};


typedef uint64_t			(*func_mysqlnd_result_unbuffered__num_rows)(const MYSQLND_RES_UNBUFFERED * const result);
typedef zend_ulong *		(*func_mysqlnd_result_unbuffered__fetch_lengths)(MYSQLND_RES_UNBUFFERED * const result);
typedef void				(*func_mysqlnd_result_unbuffered__free_last_data)(MYSQLND_RES_UNBUFFERED * result, MYSQLND_STATS * const global_stats);
typedef void				(*func_mysqlnd_result_unbuffered__free_result)(MYSQLND_RES_UNBUFFERED * const result, MYSQLND_STATS * const global_stats);

struct st_mysqlnd_result_unbuffered_methods
{
	mysqlnd_fetch_row_func							fetch_row;
	func_mysqlnd_res__row_decoder					row_decoder;
	func_mysqlnd_result_unbuffered__num_rows		num_rows;
	func_mysqlnd_result_unbuffered__fetch_lengths	fetch_lengths;
	func_mysqlnd_result_unbuffered__free_last_data	free_last_data;
	func_mysqlnd_result_unbuffered__free_result		free_result;
};

typedef uint64_t			(*func_mysqlnd_result_buffered__num_rows)(const MYSQLND_RES_BUFFERED * const result);
typedef enum_func_status	(*func_mysqlnd_result_buffered__initialize_result_set_rest)(MYSQLND_RES_BUFFERED * const result, MYSQLND_RES_METADATA * const meta,
																						MYSQLND_STATS * stats, zend_bool int_and_float_native);
typedef zend_ulong *		(*func_mysqlnd_result_buffered__fetch_lengths)(MYSQLND_RES_BUFFERED * const result);
typedef enum_func_status	(*func_mysqlnd_result_buffered__data_seek)(MYSQLND_RES_BUFFERED * const result, const uint64_t row);
typedef void				(*func_mysqlnd_result_buffered__free_result)(MYSQLND_RES_BUFFERED * const result);

struct st_mysqlnd_result_buffered_methods
{
	mysqlnd_fetch_row_func						fetch_row;
	func_mysqlnd_res__row_decoder				row_decoder;
	func_mysqlnd_result_buffered__num_rows		num_rows;
	func_mysqlnd_result_buffered__fetch_lengths	fetch_lengths;
	func_mysqlnd_result_buffered__data_seek		data_seek;
	func_mysqlnd_result_buffered__initialize_result_set_rest initialize_result_set_rest;
	func_mysqlnd_result_buffered__free_result	free_result;
};


typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_field)(MYSQLND_RES_METADATA * const meta);
typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_field_direct)(const MYSQLND_RES_METADATA * const meta, const MYSQLND_FIELD_OFFSET fieldnr);
typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_fields)(MYSQLND_RES_METADATA * const meta);
typedef MYSQLND_FIELD_OFFSET	(*func_mysqlnd_res_meta__field_tell)(const MYSQLND_RES_METADATA * const meta);
typedef MYSQLND_FIELD_OFFSET	(*func_mysqlnd_res_meta__field_seek)(MYSQLND_RES_METADATA * const meta, const MYSQLND_FIELD_OFFSET field_offset);
typedef enum_func_status		(*func_mysqlnd_res_meta__read_metadata)(MYSQLND_RES_METADATA * const meta, MYSQLND_CONN_DATA * conn);
typedef MYSQLND_RES_METADATA *	(*func_mysqlnd_res_meta__clone_metadata)(const MYSQLND_RES_METADATA * const meta, zend_bool persistent);
typedef void					(*func_mysqlnd_res_meta__free_metadata)(MYSQLND_RES_METADATA * meta);

struct st_mysqlnd_res_meta_methods
{
	func_mysqlnd_res_meta__fetch_field fetch_field;
	func_mysqlnd_res_meta__fetch_field_direct fetch_field_direct;
	func_mysqlnd_res_meta__fetch_fields fetch_fields;
	func_mysqlnd_res_meta__field_tell field_tell;
	func_mysqlnd_res_meta__field_seek field_seek;
	func_mysqlnd_res_meta__read_metadata read_metadata;
	func_mysqlnd_res_meta__clone_metadata clone_metadata;
	func_mysqlnd_res_meta__free_metadata free_metadata;
};


typedef enum_func_status	(*func_mysqlnd_stmt__prepare)(MYSQLND_STMT * const stmt, const char * const query, unsigned int query_len);
typedef enum_func_status	(*func_mysqlnd_stmt__send_execute)(MYSQLND_STMT * const s, enum_mysqlnd_send_execute_type type, zval * read_cb, zval * err_cb);
typedef enum_func_status	(*func_mysqlnd_stmt__execute)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__use_result)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__store_result)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_result)(MYSQLND_STMT * const stmt);
typedef zend_bool			(*func_mysqlnd_stmt__more_results)(const MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__next_result)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__free_result)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__seek_data)(const MYSQLND_STMT * const stmt, uint64_t row);
typedef enum_func_status	(*func_mysqlnd_stmt__reset)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__net_close)(MYSQLND_STMT * const stmt, zend_bool implicit); /* private */
typedef enum_func_status	(*func_mysqlnd_stmt__dtor)(MYSQLND_STMT * const stmt, zend_bool implicit); /* use this for mysqlnd_stmt_close */
typedef enum_func_status	(*func_mysqlnd_stmt__fetch)(MYSQLND_STMT * const stmt, zend_bool * const fetched_anything);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_parameters)(MYSQLND_STMT * const stmt, MYSQLND_PARAM_BIND * const param_bind);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_one_parameter)(MYSQLND_STMT * const stmt, unsigned int param_no, zval * const zv, zend_uchar	type);
typedef enum_func_status	(*func_mysqlnd_stmt__refresh_bind_param)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_result)(MYSQLND_STMT * const stmt, MYSQLND_RESULT_BIND * const result_bind);
typedef enum_func_status	(*func_mysqlnd_stmt__bind_one_result)(MYSQLND_STMT * const stmt, unsigned int param_no);
typedef enum_func_status	(*func_mysqlnd_stmt__send_long_data)(MYSQLND_STMT * const stmt, unsigned int param_num, const char * const data, zend_ulong length);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_parameter_metadata)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_result_metadata)(MYSQLND_STMT * const stmt);
typedef uint64_t			(*func_mysqlnd_stmt__get_last_insert_id)(const MYSQLND_STMT * const stmt);
typedef uint64_t			(*func_mysqlnd_stmt__get_affected_rows)(const MYSQLND_STMT * const stmt);
typedef uint64_t			(*func_mysqlnd_stmt__get_num_rows)(const MYSQLND_STMT * const stmt);
typedef unsigned int		(*func_mysqlnd_stmt__get_param_count)(const MYSQLND_STMT * const stmt);
typedef unsigned int		(*func_mysqlnd_stmt__get_field_count)(const MYSQLND_STMT * const stmt);
typedef unsigned int		(*func_mysqlnd_stmt__get_warning_count)(const MYSQLND_STMT * const stmt);
typedef unsigned int		(*func_mysqlnd_stmt__get_error_no)(const MYSQLND_STMT * const stmt);
typedef const char *		(*func_mysqlnd_stmt__get_error_str)(const MYSQLND_STMT * const stmt);
typedef const char *		(*func_mysqlnd_stmt__get_sqlstate)(const MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__get_attribute)(const MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, void * const value);
typedef enum_func_status	(*func_mysqlnd_stmt__set_attribute)(MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, const void * const value);
typedef MYSQLND_PARAM_BIND *(*func_mysqlnd_stmt__alloc_param_bind)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RESULT_BIND*(*func_mysqlnd_stmt__alloc_result_bind)(MYSQLND_STMT * const stmt);
typedef	void 				(*func_mysqlnd_stmt__free_parameter_bind)(MYSQLND_STMT * const stmt, MYSQLND_PARAM_BIND *);
typedef	void 				(*func_mysqlnd_stmt__free_result_bind)(MYSQLND_STMT * const stmt, MYSQLND_RESULT_BIND *);
typedef unsigned int		(*func_mysqlnd_stmt__server_status)(const MYSQLND_STMT * const stmt);
typedef enum_func_status 	(*func_mysqlnd_stmt__generate_execute_request)(MYSQLND_STMT * const s, zend_uchar ** request, size_t *request_len, zend_bool * free_buffer);
typedef enum_func_status	(*func_mysqlnd_stmt__parse_execute_response)(MYSQLND_STMT * const s, enum_mysqlnd_parse_exec_response_type type);
typedef void 				(*func_mysqlnd_stmt__free_stmt_content)(MYSQLND_STMT * const s);
typedef enum_func_status	(*func_mysqlnd_stmt__flush)(MYSQLND_STMT * const stmt);
typedef void 				(*func_mysqlnd_stmt__free_stmt_result)(MYSQLND_STMT * const s);

struct st_mysqlnd_stmt_methods
{
	func_mysqlnd_stmt__prepare prepare;
	func_mysqlnd_stmt__send_execute send_execute;
	func_mysqlnd_stmt__execute execute;
	func_mysqlnd_stmt__use_result use_result;
	func_mysqlnd_stmt__store_result store_result;
	func_mysqlnd_stmt__get_result get_result;
	func_mysqlnd_stmt__more_results more_results;
	func_mysqlnd_stmt__next_result next_result;
	func_mysqlnd_stmt__free_result free_result;
	func_mysqlnd_stmt__seek_data seek_data;
	func_mysqlnd_stmt__reset reset;
	func_mysqlnd_stmt__net_close net_close;
	func_mysqlnd_stmt__dtor dtor;
	func_mysqlnd_stmt__fetch fetch;

	func_mysqlnd_stmt__bind_parameters bind_parameters;
	func_mysqlnd_stmt__bind_one_parameter bind_one_parameter;
	func_mysqlnd_stmt__refresh_bind_param refresh_bind_param;
	func_mysqlnd_stmt__bind_result bind_result;
	func_mysqlnd_stmt__bind_one_result bind_one_result;
	func_mysqlnd_stmt__send_long_data send_long_data;
	func_mysqlnd_stmt__get_parameter_metadata get_parameter_metadata;
	func_mysqlnd_stmt__get_result_metadata get_result_metadata;

	func_mysqlnd_stmt__get_last_insert_id get_last_insert_id;
	func_mysqlnd_stmt__get_affected_rows get_affected_rows;
	func_mysqlnd_stmt__get_num_rows get_num_rows;

	func_mysqlnd_stmt__get_param_count get_param_count;
	func_mysqlnd_stmt__get_field_count get_field_count;
	func_mysqlnd_stmt__get_warning_count get_warning_count;

	func_mysqlnd_stmt__get_error_no get_error_no;
	func_mysqlnd_stmt__get_error_str get_error_str;
	func_mysqlnd_stmt__get_sqlstate get_sqlstate;

	func_mysqlnd_stmt__get_attribute get_attribute;
	func_mysqlnd_stmt__set_attribute set_attribute;

	func_mysqlnd_stmt__alloc_param_bind alloc_parameter_bind;
	func_mysqlnd_stmt__alloc_result_bind alloc_result_bind;

	func_mysqlnd_stmt__free_parameter_bind free_parameter_bind;
	func_mysqlnd_stmt__free_result_bind free_result_bind;

	func_mysqlnd_stmt__server_status get_server_status;

	func_mysqlnd_stmt__generate_execute_request generate_execute_request;
	func_mysqlnd_stmt__parse_execute_response parse_execute_response;

	func_mysqlnd_stmt__free_stmt_content free_stmt_content;

	func_mysqlnd_stmt__flush flush;

	func_mysqlnd_stmt__free_stmt_result free_stmt_result;
};


struct st_mysqlnd_net_data
{
	php_stream			*stream;
	zend_bool			compressed;
	zend_bool			ssl;
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	zend_uchar			last_command;
#else
	zend_uchar			unused_pad1;
#endif
	MYSQLND_NET_OPTIONS	options;

	unsigned int		refcount;

	zend_bool			persistent;

	struct st_mysqlnd_net_methods m;
};


struct st_mysqlnd_net
{
	/* cmd buffer */
	MYSQLND_CMD_BUFFER	cmd_buffer;

	struct st_mysqlnd_net_data * data;

#ifdef MYSQLND_COMPRESSION_ENABLED
	MYSQLND_READ_BUFFER	* uncompressed_data;
#else
	void * 				unused_pad1;
#endif

	zend_bool persistent;

	/* sequence for simple checking of correct packets */
	zend_uchar			packet_no;
	zend_uchar			compressed_envelope_packet_no;
};


struct st_mysqlnd_protocol
{
	zend_bool persistent;
	struct st_mysqlnd_protocol_methods m;
};


struct st_mysqlnd_connection_data
{
/* Operation related */
	MYSQLND_NET		* net;
	MYSQLND_PROTOCOL * protocol;

/* Information related */
	char			*host;
	unsigned int	host_len;
	char			*unix_socket;
	unsigned int	unix_socket_len;
	char			*user;
	unsigned int	user_len;
	char			*passwd;
	unsigned int	passwd_len;
	char			*scheme;
	unsigned int	scheme_len;
	uint64_t		thread_id;
	char			*server_version;
	char			*host_info;
	zend_uchar		*auth_plugin_data;
	size_t			auth_plugin_data_len;
	const MYSQLND_CHARSET *charset;
	const MYSQLND_CHARSET *greet_charset;
	char			*connect_or_select_db;
	unsigned int	connect_or_select_db_len;
	MYSQLND_INFILE	infile;
	unsigned int	protocol_version;
	zend_ulong	max_packet_size;
	unsigned int	port;
	zend_ulong	client_flag;
	zend_ulong	server_capabilities;

	/* For UPSERT queries */
	MYSQLND_UPSERT_STATUS * upsert_status;
	MYSQLND_UPSERT_STATUS upsert_status_impl;
	char			*last_message;
	unsigned int	last_message_len;

	/* If error packet, we use these */
	MYSQLND_ERROR_INFO	* error_info;
	MYSQLND_ERROR_INFO	error_info_impl;

	/*
	  To prevent queries during unbuffered fetches. Also to
	  mark the connection as destroyed for garbage collection.
	*/
	enum mysqlnd_connection_state	state;
	enum_mysqlnd_query_type			last_query_type;
	/* Temporary storage between query and (use|store)_result() call */
	MYSQLND_RES						*current_result;

	/*
	  How many result sets reference this connection.
	  It won't be freed until this number reaches 0.
	  The last one, please close the door! :-)
	  The result set objects can determine by inspecting
	  'quit_sent' whether the connection is still valid.
	*/
	unsigned int	refcount;

	/* Temporal storage for mysql_query */
	unsigned int	field_count;

	/* options */
	MYSQLND_OPTIONS	* options;
	MYSQLND_OPTIONS	options_impl;

	/* stats */
	MYSQLND_STATS	* stats;

	unsigned int	client_api_capabilities;

	zval			async_read_cb;
	zval			async_err_cb;
	zend_bool		in_async_read_cb;
	zend_bool		in_async_err_cb;

	struct st_mysqlnd_conn_data_methods * m;

	/* persistent connection */
	zend_bool		persistent;
};


struct st_mysqlnd_connection
{
	MYSQLND_CONN_DATA * data;
	zend_bool persistent;
	struct st_mysqlnd_conn_methods * m;
};


struct mysqlnd_field_hash_key
{
	zend_bool		is_numeric;
	zend_ulong	key;
};


struct st_mysqlnd_result_metadata
{
	MYSQLND_FIELD					*fields;
	struct mysqlnd_field_hash_key	*zend_hash_keys;

	struct st_mysqlnd_res_meta_methods * m;

	size_t							bit_fields_total_len; /* trailing \0 not counted */
	/* We need this to make fast allocs in rowp_read */
	unsigned int					bit_fields_count;

	unsigned int					current_field;
	unsigned int					field_count;

	zend_bool						persistent;
};


#define def_mysqlnd_buffered_result_parent 			\
	MYSQLND_MEMORY_POOL_CHUNK **row_buffers;		\
	uint64_t			row_count;					\
	uint64_t			initialized_rows;			\
													\
	/*  Column lengths of current row - both buffered and unbuffered. For buffered results it duplicates the data found in **data */ \
	zend_ulong		*lengths;					\
													\
	MYSQLND_MEMORY_POOL	*result_set_memory_pool;	\
													\
	unsigned int		references;					\
													\
	MYSQLND_ERROR_INFO	error_info;					\
													\
	unsigned int		field_count;				\
	zend_bool			ps;							\
	zend_bool			persistent;					\
	struct st_mysqlnd_result_buffered_methods m;	\
	enum mysqlnd_buffered_type type;				\
	void				* unused1;					\
	void				* unused2;					\
	void				* unused3


struct st_mysqlnd_buffered_result_parent
{
	def_mysqlnd_buffered_result_parent;
};


struct st_mysqlnd_buffered_result_zval
{
	def_mysqlnd_buffered_result_parent;

	zval	*data;
	zval	*data_cursor;
};


struct st_mysqlnd_buffered_result_c
{
	def_mysqlnd_buffered_result_parent;

	zend_uchar	*initialized; /* every row is a single bit */
	uint64_t	current_row;
};


struct st_mysqlnd_unbuffered_result
{
	struct st_mysqlnd_result_unbuffered_methods m;
	uint64_t			row_count;

	/* For unbuffered (both normal and PS) */
	zval				*last_row_data;
	MYSQLND_MEMORY_POOL_CHUNK *last_row_buffer;

	/*
	  Column lengths of current row - both buffered and unbuffered.
	  For buffered results it duplicates the data found in **data
	*/
	zend_ulong		*lengths;

	MYSQLND_MEMORY_POOL	*result_set_memory_pool;

	struct st_mysqlnd_packet_row * row_packet;

	unsigned int		field_count;

	zend_bool			eof_reached;

	zend_bool			ps;
	zend_bool			persistent;

};


struct st_mysqlnd_res
{
	MYSQLND_CONN_DATA		*conn;
	enum_mysqlnd_res_type	type;
	unsigned int			field_count;

	/* For metadata functions */
	MYSQLND_RES_METADATA	*meta;

	/* To be used with store_result() - both normal and PS */
	MYSQLND_RES_BUFFERED	*stored_data;
	MYSQLND_RES_UNBUFFERED	*unbuf;

	zend_bool				persistent;

	struct st_mysqlnd_res_methods m;
};


struct st_mysqlnd_param_bind
{
	zval		zv;
	zend_uchar	type;
	enum_param_bind_flags	flags;
};

struct st_mysqlnd_result_bind
{
	zval		zv;
	zend_bool	bound;
};


struct st_mysqlnd_stmt_data
{
	MYSQLND_CONN_DATA			*conn;
	zend_ulong					stmt_id;
	zend_ulong					flags;/* cursor is set here */
	enum_mysqlnd_stmt_state		state;
	unsigned int				warning_count;
	MYSQLND_RES					*result;
	unsigned int				field_count;
	unsigned int				param_count;
	unsigned char				send_types_to_server;
	MYSQLND_PARAM_BIND			*param_bind;
	MYSQLND_RESULT_BIND			*result_bind;
	zend_bool					result_zvals_separated_once;
	zend_bool					persistent;

	MYSQLND_UPSERT_STATUS * 	upsert_status;
	MYSQLND_UPSERT_STATUS 		upsert_status_impl;

	MYSQLND_ERROR_INFO *		error_info;
	MYSQLND_ERROR_INFO			error_info_impl;

	zend_bool					update_max_length;
	zend_ulong					prefetch_rows;

	zend_bool					cursor_exists;
	mysqlnd_stmt_use_or_store_func default_rset_handler;

	zval						execute_read_cb;
	zval						execute_err_cb;
	zend_bool					in_execute_read_cb;
	zend_bool					in_execute_err_cb;

	MYSQLND_CMD_BUFFER			execute_cmd_buffer;
	unsigned int				execute_count;/* count how many times the stmt was executed */
};


struct st_mysqlnd_stmt
{
	MYSQLND_STMT_DATA * data;
	struct st_mysqlnd_stmt_methods	*m;
	zend_bool persistent;
};


typedef struct st_mysqlnd_string
{
	char	*s;
	size_t	l;
} MYSQLND_STRING;


struct st_mysqlnd_plugin_header
{
	unsigned int	plugin_api_version;
	const char *	plugin_name;
	zend_ulong	plugin_version;
	const char *	plugin_string_version;
	const char *	plugin_license;
	const char *	plugin_author;
	struct
	{
		MYSQLND_STATS *			values;
		const MYSQLND_STRING *	names;
	} plugin_stats;

	struct
	{
		enum_func_status (*plugin_shutdown)(void * plugin);
	} m;
};


struct st_mysqlnd_plugin_core
{
	struct st_mysqlnd_plugin_header plugin_header;
};


struct st_mysqlnd_typeii_plugin_example
{
	struct st_mysqlnd_plugin_header plugin_header;
	void * methods;
	unsigned int counter;
};

struct st_mysqlnd_authentication_plugin;

typedef zend_uchar * (*func_auth_plugin__get_auth_data)(struct st_mysqlnd_authentication_plugin * self,
														size_t * auth_data_len,
														MYSQLND_CONN_DATA * conn, const char * const user, const char * const passwd,
														const size_t passwd_len, zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
														const MYSQLND_OPTIONS * const options,
														const MYSQLND_NET_OPTIONS * const net_options, zend_ulong mysql_flags
														);

struct st_mysqlnd_authentication_plugin
{
	struct st_mysqlnd_plugin_header plugin_header;
	struct {
		func_auth_plugin__get_auth_data get_auth_data;
	} methods;
};


#endif /* MYSQLND_STRUCTS_H */
