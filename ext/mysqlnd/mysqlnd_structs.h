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

#ifndef MYSQLND_STRUCTS_H
#define MYSQLND_STRUCTS_H

#include "zend_smart_str_public.h"

#define MYSQLND_TYPEDEFED_METHODS

#define MYSQLND_CLASS_METHOD_TABLE_NAME(class) mysqlnd_##class##_methods
#define MYSQLND_CLASS_METHODS_TYPE(class) struct st_##class##_methods
#define MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(class) MYSQLND_CLASS_METHODS_TYPE(class) MYSQLND_CLASS_METHOD_TABLE_NAME(class)

#define MYSQLND_CLASS_METHODS_START(class)	MYSQLND_CLASS_METHOD_TABLE_NAME_FORWARD(class) = {
#define MYSQLND_CLASS_METHODS_END			}

#define MYSQLND_CLASS_METHODS_INSTANCE_NAME(class)		mysqlnd_##class##_methods_ptr
#define MYSQLND_CLASS_METHODS_INSTANCE_DECLARE(class)	extern const MYSQLND_CLASS_METHODS_TYPE(class) * MYSQLND_CLASS_METHODS_INSTANCE_NAME(class)
#define MYSQLND_CLASS_METHODS_INSTANCE_DEFINE(class)	const MYSQLND_CLASS_METHODS_TYPE(class) * MYSQLND_CLASS_METHODS_INSTANCE_NAME(class) = & MYSQLND_CLASS_METHOD_TABLE_NAME(class)

typedef struct st_mysqlnd_string
{
	char	*s;
	size_t	l;
} MYSQLND_STRING;

typedef struct st_mysqlnd_const_string
{
	const char *s;
	size_t	l;
} MYSQLND_CSTRING;


typedef struct st_mysqlnd_memory_pool MYSQLND_MEMORY_POOL;

struct st_mysqlnd_memory_pool
{
	zend_arena		*arena;
	void            *checkpoint;

	void*	(*get_chunk)(MYSQLND_MEMORY_POOL * pool, size_t size);
};


typedef struct st_mysqlnd_row_buffer MYSQLND_ROW_BUFFER;

struct st_mysqlnd_row_buffer
{
	void			*ptr;
	size_t			size;
};


typedef struct st_mysqlnd_cmd_buffer
{
	zend_uchar		*buffer;
	size_t			length;
} MYSQLND_CMD_BUFFER;


typedef struct st_mysqlnd_field
{
	zend_string *sname;			/* Name of column */
	bool    is_numeric;
	zend_ulong	 num_key;
	const char  *name;          /* Name of column in C string */
	const char  *org_name;		/* Original column name, if an alias */
	const char  *table;			/* Table of column if column was a field */
	const char  *org_table;		/* Org table name, if table was an alias */
	const char  *db;			/* Database for table */
	const char  *catalog;		/* Catalog for table */
	char  *def;                 /* Default value */
	zend_ulong length;		/* Width of column (create length) */
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


typedef struct st_mysqlnd_upsert_status MYSQLND_UPSERT_STATUS;
typedef void (*func_mysqlnd_upsert_status__reset)(MYSQLND_UPSERT_STATUS * const upsert_status);
typedef void (*func_mysqlnd_upsert_status__set_affected_rows_to_error)(MYSQLND_UPSERT_STATUS * const upsert_status);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_upsert_status)
{
	func_mysqlnd_upsert_status__reset reset;
	func_mysqlnd_upsert_status__set_affected_rows_to_error set_affected_rows_to_error;
};

struct st_mysqlnd_upsert_status
{
	unsigned int	warning_count;
	unsigned int	server_status;
	uint64_t		affected_rows;
	uint64_t		last_insert_id;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_upsert_status) *m;
};

#define SET_EMPTY_ERROR(info)							(info)->m->reset((info))
#define SET_CLIENT_ERROR(info, err_no, sqlstate, error)	(err_no)? (info)->m->set_client_error((info), (err_no), (sqlstate), (error)) : (info)->m->reset((info))
#define SET_OOM_ERROR(info) 							SET_CLIENT_ERROR((info), CR_OUT_OF_MEMORY, UNKNOWN_SQLSTATE, mysqlnd_out_of_memory)
#define COPY_CLIENT_ERROR(dest, source)					SET_CLIENT_ERROR((dest), (source).error_no, (source).sqlstate, (source).error)


typedef struct st_mysqlnd_error_info MYSQLND_ERROR_INFO;
typedef void (*func_mysqlnd_error_info__reset)(MYSQLND_ERROR_INFO * const info);
typedef void (*func_mysqlnd_error_info__set_client_error)(MYSQLND_ERROR_INFO * const info, const unsigned int err_no, const char * const sqlstate, const char * const error);


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_error_info)
{
	func_mysqlnd_error_info__reset reset;
	func_mysqlnd_error_info__set_client_error set_client_error;
};

struct st_mysqlnd_error_info
{
	char error[MYSQLND_ERRMSG_SIZE+1];
	char sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int error_no;
	zend_llist error_list;

	bool persistent;
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_error_info) *m;
};


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


typedef int (*func_mysqlnd_local_infile__init)(void ** ptr, const char * const filename);
typedef int (*func_mysqlnd_local_infile__read)(void * ptr, zend_uchar * buf, unsigned int buf_len);
typedef int (*func_mysqlnd_local_infile__error)(void * ptr, char * error_msg, unsigned int error_msg_len);
typedef void (*func_mysqlnd_local_infile__end)(void * ptr);


/* character set information */
typedef struct st_mysqlnd_charset
{
	unsigned int	nr;
	const char		*name;
	const char		*collation;
	unsigned int	char_minlen;
	unsigned int	char_maxlen;
	const char		*comment;
	short			mb_charlen;
	short			mb_valid;
} MYSQLND_CHARSET;


/* local infile handler */
typedef struct st_mysqlnd_infile
{
	func_mysqlnd_local_infile__init		local_infile_init;
	func_mysqlnd_local_infile__read		local_infile_read;
	func_mysqlnd_local_infile__error	local_infile_error;
	func_mysqlnd_local_infile__end		local_infile_end;
} MYSQLND_INFILE;


typedef struct st_mysqlnd_session_options
{
	unsigned int		flags;

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
	char		* unused1;
	char		* unused2;
	char		* unused3;

	enum_mysqlnd_session_protocol_type protocol;

	char 		*charset_name;
	/* maximum allowed packet size for communication */
	unsigned int		max_allowed_packet;

	bool	int_and_float_native;

	char		*local_infile_directory;
} MYSQLND_SESSION_OPTIONS;


typedef struct st_mysqlnd_vio_options
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
	enum mysqlnd_ssl_peer {
		MYSQLND_SSL_PEER_DEFAULT = 0,
		MYSQLND_SSL_PEER_VERIFY = 1,
		MYSQLND_SSL_PEER_DONT_VERIFY = 2,

#define MYSQLND_SSL_PEER_DEFAULT_ACTION  MYSQLND_SSL_PEER_VERIFY
	} ssl_verify_peer;
} MYSQLND_VIO_OPTIONS;



typedef struct st_mysqlnd_connection MYSQLND;
typedef struct st_mysqlnd_connection_data MYSQLND_CONN_DATA;
typedef struct st_mysqlnd_protocol_frame_codec		MYSQLND_PFC;
typedef struct st_mysqlnd_protocol_frame_codec_data	MYSQLND_PFC_DATA;
typedef struct st_mysqlnd_vio		MYSQLND_VIO;
typedef struct st_mysqlnd_vio_data	MYSQLND_VIO_DATA;
typedef struct st_mysqlnd_protocol_payload_decoder_factory	MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY;
typedef struct st_mysqlnd_res	MYSQLND_RES;
typedef char** 					MYSQLND_ROW_C;		/* return data as array of strings */
typedef struct st_mysqlnd_stmt_data	MYSQLND_STMT_DATA;
typedef struct st_mysqlnd_stmt	MYSQLND_STMT;
typedef unsigned int			MYSQLND_FIELD_OFFSET;

typedef struct st_mysqlnd_param_bind MYSQLND_PARAM_BIND;

typedef struct st_mysqlnd_result_bind MYSQLND_RESULT_BIND;

typedef struct st_mysqlnd_result_metadata MYSQLND_RES_METADATA;
typedef struct st_mysqlnd_buffered_result MYSQLND_RES_BUFFERED;
typedef struct st_mysqlnd_unbuffered_result MYSQLND_RES_UNBUFFERED;

typedef struct st_mysqlnd_debug MYSQLND_DEBUG;


typedef MYSQLND_RES* (*mysqlnd_stmt_use_or_store_func)(MYSQLND_STMT * const);
typedef enum_func_status  (*mysqlnd_fetch_row_func)(MYSQLND_RES *result,
													zval **row,
													const unsigned int flags,
													bool * fetched_anything
													);


typedef struct st_mysqlnd_stats MYSQLND_STATS;

struct st_mysqlnd_stats
{
	uint64_t				*values;
	size_t					count;
#ifdef ZTS
	MUTEX_T	LOCK_access;
#endif
};


typedef enum_func_status (*func_mysqlnd_execute_com_set_option)(MYSQLND_CONN_DATA * const conn, const enum_mysqlnd_server_option option);
typedef enum_func_status (*func_mysqlnd_execute_com_debug)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status (*func_mysqlnd_execute_com_init_db)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING db);
typedef enum_func_status (*func_mysqlnd_execute_com_ping)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status (*func_mysqlnd_execute_com_statistics)(MYSQLND_CONN_DATA * const conn, zend_string ** message);
typedef enum_func_status (*func_mysqlnd_execute_com_process_kill)(MYSQLND_CONN_DATA * const conn, const unsigned int process_id, const bool read_response);
typedef enum_func_status (*func_mysqlnd_execute_com_refresh)(MYSQLND_CONN_DATA * const conn, const uint8_t options);
typedef enum_func_status (*func_mysqlnd_execute_com_quit)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status (*func_mysqlnd_execute_com_query)(MYSQLND_CONN_DATA * const conn, MYSQLND_CSTRING query);
typedef enum_func_status (*func_mysqlnd_execute_com_change_user)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING payload, const bool silent);
typedef enum_func_status (*func_mysqlnd_execute_com_reap_result)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status (*func_mysqlnd_execute_com_stmt_prepare)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING query);
typedef enum_func_status (*func_mysqlnd_execute_com_stmt_execute)(MYSQLND_CONN_DATA * conn, const MYSQLND_CSTRING payload);
typedef enum_func_status (*func_mysqlnd_execute_com_stmt_fetch)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING payload);
typedef enum_func_status (*func_mysqlnd_execute_com_stmt_reset)(MYSQLND_CONN_DATA * const conn, const zend_ulong stmt_id);
typedef enum_func_status (*func_mysqlnd_execute_com_stmt_send_long_data)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING payload);
typedef enum_func_status (*func_mysqlnd_execute_com_stmt_close)(MYSQLND_CONN_DATA * const conn, const zend_ulong stmt_id);
typedef enum_func_status (*func_mysqlnd_execute_com_enable_ssl)(MYSQLND_CONN_DATA * const conn, const size_t client_capabilities, const size_t server_capabilities, const unsigned int charset_no);
typedef enum_func_status (*func_mysqlnd_execute_com_handshake)(MYSQLND_CONN_DATA * const conn, const MYSQLND_CSTRING username, const MYSQLND_CSTRING password, const MYSQLND_CSTRING database, const size_t client_flags);


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_command)
{
	func_mysqlnd_execute_com_set_option set_option;
	func_mysqlnd_execute_com_debug debug;
	func_mysqlnd_execute_com_init_db init_db;
	func_mysqlnd_execute_com_ping ping;
	func_mysqlnd_execute_com_statistics statistics;
	func_mysqlnd_execute_com_process_kill process_kill;
	func_mysqlnd_execute_com_refresh refresh;
	func_mysqlnd_execute_com_quit quit;
	func_mysqlnd_execute_com_query query;
	func_mysqlnd_execute_com_change_user change_user;
	func_mysqlnd_execute_com_reap_result reap_result;
	func_mysqlnd_execute_com_stmt_prepare stmt_prepare;
	func_mysqlnd_execute_com_stmt_execute stmt_execute;
	func_mysqlnd_execute_com_stmt_fetch stmt_fetch;
	func_mysqlnd_execute_com_stmt_reset stmt_reset;
	func_mysqlnd_execute_com_stmt_send_long_data stmt_send_long_data;
	func_mysqlnd_execute_com_stmt_close stmt_close;
	func_mysqlnd_execute_com_enable_ssl enable_ssl;
	func_mysqlnd_execute_com_handshake handshake;
};



typedef void				(*func_mysqlnd_vio__init)(MYSQLND_VIO * const vio, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef void				(*func_mysqlnd_vio__dtor)(MYSQLND_VIO * const vio, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);

typedef enum_func_status	(*func_mysqlnd_vio__connect)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, const bool persistent, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);

typedef void				(*func_mysqlnd_vio__close_stream)(MYSQLND_VIO * const vio, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef php_stream *		(*func_mysqlnd_vio__open_stream)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, const bool persistent, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef php_stream *		(*func_mysqlnd_vio__get_stream)(const MYSQLND_VIO * const vio);
typedef enum_func_status	(*func_mysqlnd_vio__set_stream)(MYSQLND_VIO * const vio, php_stream * vio_stream);
typedef bool			(*func_mysqlnd_vio__has_valid_stream)(const MYSQLND_VIO * const vio);
typedef func_mysqlnd_vio__open_stream (*func_mysqlnd_vio__get_open_stream)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, MYSQLND_ERROR_INFO * const error_info);

typedef enum_func_status	(*func_mysqlnd_vio__set_client_option)(MYSQLND_VIO * const vio, enum_mysqlnd_client_option option, const char * const value);
typedef void				(*func_mysqlnd_vio__post_connect_set_opt)(MYSQLND_VIO * const vio, const MYSQLND_CSTRING scheme, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);

typedef enum_func_status	(*func_mysqlnd_vio__enable_ssl)(MYSQLND_VIO * const vio);
typedef enum_func_status	(*func_mysqlnd_vio__disable_ssl)(MYSQLND_VIO * const vio);
typedef enum_func_status	(*func_mysqlnd_vio__network_read)(MYSQLND_VIO * const vio, zend_uchar * const buffer, const size_t count, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef ssize_t				(*func_mysqlnd_vio__network_write)(MYSQLND_VIO * const vio, const zend_uchar * const buf, const size_t count, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);

typedef size_t				(*func_mysqlnd_vio__consume_uneaten_data)(MYSQLND_VIO * const vio, enum php_mysqlnd_server_command cmd);

typedef void				(*func_mysqlnd_vio__free_contents)(MYSQLND_VIO * vio);


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_vio)
{
	func_mysqlnd_vio__init init;
	func_mysqlnd_vio__dtor dtor;
	func_mysqlnd_vio__connect connect;

	func_mysqlnd_vio__close_stream close_stream;
	func_mysqlnd_vio__open_stream open_pipe;
	func_mysqlnd_vio__open_stream open_tcp_or_unix;

	func_mysqlnd_vio__get_stream get_stream;
	func_mysqlnd_vio__set_stream set_stream;
	func_mysqlnd_vio__has_valid_stream has_valid_stream;
	func_mysqlnd_vio__get_open_stream get_open_stream;

	func_mysqlnd_vio__set_client_option set_client_option;
	func_mysqlnd_vio__post_connect_set_opt post_connect_set_opt;

	func_mysqlnd_vio__enable_ssl enable_ssl;
	func_mysqlnd_vio__disable_ssl disable_ssl;

	func_mysqlnd_vio__network_read network_read;
	func_mysqlnd_vio__network_write network_write;

	func_mysqlnd_vio__consume_uneaten_data consume_uneaten_data;

	func_mysqlnd_vio__free_contents free_contents;
};


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory);

typedef MYSQLND * (*func_mysqlnd_object_factory__get_connection)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) * factory, const bool persistent);
typedef MYSQLND * (*func_mysqlnd_object_factory__clone_connection_object)(MYSQLND * conn);
typedef MYSQLND_STMT * (*func_mysqlnd_object_factory__get_prepared_statement)(MYSQLND_CONN_DATA * conn);
typedef MYSQLND_PFC * (*func_mysqlnd_object_factory__get_pfc)(const bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);
typedef MYSQLND_VIO * (*func_mysqlnd_object_factory__get_vio)(const bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info);
typedef MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * (*func_mysqlnd_object_factory__get_protocol_payload_decoder_factory)(MYSQLND_CONN_DATA * conn, const bool persistent);


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory)
{
	func_mysqlnd_object_factory__get_connection get_connection;
	func_mysqlnd_object_factory__clone_connection_object clone_connection_object;
	func_mysqlnd_object_factory__get_prepared_statement get_prepared_statement;
	func_mysqlnd_object_factory__get_pfc get_protocol_frame_codec;
	func_mysqlnd_object_factory__get_vio get_vio;
	func_mysqlnd_object_factory__get_protocol_payload_decoder_factory get_protocol_payload_decoder_factory;
};


typedef enum_func_status	(*func_mysqlnd_conn_data__connect)(MYSQLND_CONN_DATA * conn, MYSQLND_CSTRING hostname, MYSQLND_CSTRING username, MYSQLND_CSTRING password, MYSQLND_CSTRING database, unsigned int port, MYSQLND_CSTRING socket_or_pipe, unsigned int mysql_flags);
typedef zend_ulong			(*func_mysqlnd_conn_data__escape_string)(MYSQLND_CONN_DATA * const conn, char *newstr, const char *escapestr, size_t escapestr_len);
typedef enum_func_status	(*func_mysqlnd_conn_data__set_charset)(MYSQLND_CONN_DATA * const conn, const char * const charset);
typedef enum_func_status	(*func_mysqlnd_conn_data__query)(MYSQLND_CONN_DATA * conn, const char * const query, const size_t query_len);
typedef enum_func_status	(*func_mysqlnd_conn_data__send_query)(MYSQLND_CONN_DATA * conn, const char * const query, const size_t query_len, zval *read_cb, zval *err_cb);
typedef enum_func_status	(*func_mysqlnd_conn_data__reap_query)(MYSQLND_CONN_DATA * conn);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__use_result)(MYSQLND_CONN_DATA * const conn);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__store_result)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__next_result)(MYSQLND_CONN_DATA * const conn);
typedef bool			(*func_mysqlnd_conn_data__more_results)(const MYSQLND_CONN_DATA * const conn);

typedef MYSQLND_STMT *		(*func_mysqlnd_conn_data__stmt_init)(MYSQLND_CONN_DATA * const conn);

typedef enum_func_status	(*func_mysqlnd_conn_data__shutdown_server)(MYSQLND_CONN_DATA * const conn, uint8_t level);
typedef enum_func_status	(*func_mysqlnd_conn_data__refresh_server)(MYSQLND_CONN_DATA * const conn, uint8_t options);

typedef enum_func_status	(*func_mysqlnd_conn_data__ping)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__kill_connection)(MYSQLND_CONN_DATA * conn, unsigned int pid);
typedef enum_func_status	(*func_mysqlnd_conn_data__select_db)(MYSQLND_CONN_DATA * const conn, const char * const db, const size_t db_len);
typedef enum_func_status	(*func_mysqlnd_conn_data__server_dump_debug_information)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__change_user)(MYSQLND_CONN_DATA * const conn, const char * user, const char * passwd, const char * db, bool silent, size_t passwd_len);

typedef unsigned int		(*func_mysqlnd_conn_data__get_error_no)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_error_str)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_sqlstate)(const MYSQLND_CONN_DATA * const conn);
typedef uint64_t			(*func_mysqlnd_conn_data__get_thread_id)(const MYSQLND_CONN_DATA * const conn);
typedef void				(*func_mysqlnd_conn_data__get_statistics)(const MYSQLND_CONN_DATA * const conn, zval *return_value ZEND_FILE_LINE_DC);

typedef zend_ulong			(*func_mysqlnd_conn_data__get_server_version)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_server_information)(const MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__get_server_statistics)(MYSQLND_CONN_DATA * conn, zend_string **message);
typedef const char *		(*func_mysqlnd_conn_data__get_host_information)(const MYSQLND_CONN_DATA * const conn);
typedef unsigned int		(*func_mysqlnd_conn_data__get_protocol_information)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__get_last_message)(const MYSQLND_CONN_DATA * const conn);
typedef const char *		(*func_mysqlnd_conn_data__charset_name)(const MYSQLND_CONN_DATA * const conn);
typedef MYSQLND_RES *		(*func_mysqlnd_conn_data__list_method)(MYSQLND_CONN_DATA * conn, const char * const query, const char * const achtung_wild, const char * const par1);

typedef uint64_t			(*func_mysqlnd_conn_data__get_last_insert_id)(const MYSQLND_CONN_DATA * const conn);
typedef uint64_t			(*func_mysqlnd_conn_data__get_affected_rows)(const MYSQLND_CONN_DATA * const conn);
typedef unsigned int		(*func_mysqlnd_conn_data__get_warning_count)(const MYSQLND_CONN_DATA * const conn);

typedef unsigned int		(*func_mysqlnd_conn_data__get_field_count)(const MYSQLND_CONN_DATA * const conn);

typedef unsigned int		(*func_mysqlnd_conn_data__get_server_status)(const MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__set_server_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_server_option option);
typedef enum_func_status	(*func_mysqlnd_conn_data__set_client_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_client_option option, const char * const value);
typedef void				(*func_mysqlnd_conn_data__free_contents)(MYSQLND_CONN_DATA * conn);/* private */
typedef void				(*func_mysqlnd_conn_data__free_options)(MYSQLND_CONN_DATA * conn);	/* private */
typedef void				(*func_mysqlnd_conn_data__dtor)(MYSQLND_CONN_DATA * conn);	/* private */

typedef enum_func_status	(*func_mysqlnd_conn_data__query_read_result_set_header)(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * stmt);

typedef MYSQLND_CONN_DATA *	(*func_mysqlnd_conn_data__get_reference)(MYSQLND_CONN_DATA * const conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__free_reference)(MYSQLND_CONN_DATA * const conn);

typedef enum_func_status	(*func_mysqlnd_conn_data__send_command_do_request)(MYSQLND_CONN_DATA * const conn, const enum php_mysqlnd_server_command command, const zend_uchar * const arg, const size_t arg_len, const bool silent, const bool ignore_upsert_status);
typedef enum_func_status	(*func_mysqlnd_conn_data__send_command_handle_response)(MYSQLND_CONN_DATA * const conn, const enum mysqlnd_packet_type ok_packet, const bool silent, const enum php_mysqlnd_server_command command, const bool ignore_upsert_status);

typedef void				(*func_mysqlnd_conn_data__restart_psession)(MYSQLND_CONN_DATA * conn);
typedef void				(*func_mysqlnd_conn_data__end_psession)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__send_close)(MYSQLND_CONN_DATA * conn);

typedef enum_func_status    (*func_mysqlnd_conn_data__ssl_set)(MYSQLND_CONN_DATA * const conn, const char * key, const char * const cert, const char * const ca, const char * const capath, const char * const cipher);

typedef MYSQLND_RES * 		(*func_mysqlnd_conn_data__result_init)(unsigned int field_count);

typedef enum_func_status	(*func_mysqlnd_conn_data__set_autocommit)(MYSQLND_CONN_DATA * conn, unsigned int mode);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_commit)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_rollback)(MYSQLND_CONN_DATA * conn);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_begin)(MYSQLND_CONN_DATA * conn, const unsigned int mode, const char * const name);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_commit_or_rollback)(MYSQLND_CONN_DATA * conn, const bool commit, const unsigned int flags, const char * const name);
typedef void				(*func_mysqlnd_conn_data__tx_cor_options_to_string)(const MYSQLND_CONN_DATA * const conn, smart_str * tmp_str, const unsigned int mode);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_savepoint)(MYSQLND_CONN_DATA * conn, const char * const name);
typedef enum_func_status	(*func_mysqlnd_conn_data__tx_savepoint_release)(MYSQLND_CONN_DATA * conn, const char * const name);

typedef enum_func_status	(*func_mysqlnd_conn_data__execute_init_commands)(MYSQLND_CONN_DATA * conn);
typedef unsigned int		(*func_mysqlnd_conn_data__get_updated_connect_flags)(MYSQLND_CONN_DATA * conn, unsigned int mysql_flags);
typedef enum_func_status	(*func_mysqlnd_conn_data__connect_handshake)(MYSQLND_CONN_DATA * conn, const MYSQLND_CSTRING * const scheme, const MYSQLND_CSTRING * const username, const MYSQLND_CSTRING * const password, const MYSQLND_CSTRING * const database, const unsigned int mysql_flags);
typedef struct st_mysqlnd_authentication_plugin * (*func_mysqlnd_conn_data__fetch_auth_plugin_by_name)(const char * const requested_protocol);

typedef enum_func_status	(*func_mysqlnd_conn_data__set_client_option_2d)(MYSQLND_CONN_DATA * const conn, const enum_mysqlnd_client_option option, const char * const key, const char * const value);


typedef size_t				(*func_mysqlnd_conn_data__negotiate_client_api_capabilities)(MYSQLND_CONN_DATA * const conn, const size_t flags);
typedef size_t				(*func_mysqlnd_conn_data__get_client_api_capabilities)(const MYSQLND_CONN_DATA * const conn);

typedef MYSQLND_STRING		(*func_mysqlnd_conn_data__get_scheme)(MYSQLND_CONN_DATA * conn, MYSQLND_CSTRING hostname, MYSQLND_CSTRING *socket_or_pipe, unsigned int port, bool * unix_socket, bool * named_pipe);



MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data)
{
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

	func_mysqlnd_conn_data__execute_init_commands execute_init_commands;
	func_mysqlnd_conn_data__get_updated_connect_flags get_updated_connect_flags;
	func_mysqlnd_conn_data__connect_handshake connect_handshake;
	func_mysqlnd_conn_data__fetch_auth_plugin_by_name fetch_auth_plugin_by_name;

	func_mysqlnd_conn_data__set_client_option_2d set_client_option_2d;

	func_mysqlnd_conn_data__negotiate_client_api_capabilities negotiate_client_api_capabilities;
	func_mysqlnd_conn_data__get_client_api_capabilities get_client_api_capabilities;

	func_mysqlnd_conn_data__get_scheme get_scheme;
};


typedef enum_func_status	(*func_mysqlnd_data__connect)(MYSQLND * conn, const MYSQLND_CSTRING hostname, const MYSQLND_CSTRING username, const MYSQLND_CSTRING password, const MYSQLND_CSTRING database, unsigned int port, const MYSQLND_CSTRING socket_or_pipe, unsigned int mysql_flags);
typedef MYSQLND *			(*func_mysqlnd_conn__clone_object)(MYSQLND * const conn);
typedef void				(*func_mysqlnd_conn__dtor)(MYSQLND * conn);
typedef enum_func_status	(*func_mysqlnd_conn__close)(MYSQLND * conn, const enum_connection_close_type close_type);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn)
{
	func_mysqlnd_data__connect connect;
	func_mysqlnd_conn__clone_object clone_object;
	func_mysqlnd_conn__dtor dtor;
	func_mysqlnd_conn__close close;
};


	/* for decoding - binary or text protocol */
typedef enum_func_status	(*func_mysqlnd_res__row_decoder)(MYSQLND_ROW_BUFFER * row_buffer, zval * fields,
															 const unsigned int field_count, const MYSQLND_FIELD * const fields_metadata,
															 const bool as_int_or_float, MYSQLND_STATS * const stats);


typedef MYSQLND_RES *		(*func_mysqlnd_res__use_result)(MYSQLND_RES * const result, MYSQLND_STMT_DATA *stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_res__store_result)(MYSQLND_RES * result, MYSQLND_CONN_DATA * const conn, MYSQLND_STMT_DATA *stmt);
typedef void 				(*func_mysqlnd_res__fetch_into)(MYSQLND_RES *result, const unsigned int flags, zval *return_value ZEND_FILE_LINE_DC);
typedef MYSQLND_ROW_C 		(*func_mysqlnd_res__fetch_row_c)(MYSQLND_RES *result);
typedef uint64_t			(*func_mysqlnd_res__num_rows)(const MYSQLND_RES * const result);
typedef unsigned int		(*func_mysqlnd_res__num_fields)(const MYSQLND_RES * const result);
typedef void				(*func_mysqlnd_res__skip_result)(MYSQLND_RES * const result);
typedef enum_func_status	(*func_mysqlnd_res__seek_data)(MYSQLND_RES * const result, const uint64_t row);
typedef MYSQLND_FIELD_OFFSET (*func_mysqlnd_res__seek_field)(MYSQLND_RES * const result, const MYSQLND_FIELD_OFFSET field_offset);
typedef MYSQLND_FIELD_OFFSET (*func_mysqlnd_res__field_tell)(const MYSQLND_RES * const result);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_field)(MYSQLND_RES * const result);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_field_direct)(MYSQLND_RES * const result, const MYSQLND_FIELD_OFFSET fieldnr);
typedef const MYSQLND_FIELD *(*func_mysqlnd_res__fetch_fields)(MYSQLND_RES * const result);

typedef enum_func_status	(*func_mysqlnd_res__read_result_metadata)(MYSQLND_RES * result, MYSQLND_CONN_DATA * conn);
typedef const size_t *		(*func_mysqlnd_res__fetch_lengths)(const MYSQLND_RES * const result);
typedef enum_func_status	(*func_mysqlnd_res__store_result_fetch_data)(MYSQLND_CONN_DATA * const conn, MYSQLND_RES * result, MYSQLND_RES_METADATA * meta, MYSQLND_ROW_BUFFER ** row_buffers, bool binary_protocol);

typedef void				(*func_mysqlnd_res__free_result_buffers)(MYSQLND_RES * result);	/* private */
typedef enum_func_status	(*func_mysqlnd_res__free_result)(MYSQLND_RES * result, const bool implicit);
typedef void				(*func_mysqlnd_res__free_result_contents)(MYSQLND_RES *result);
typedef void				(*func_mysqlnd_res__free_buffered_data)(MYSQLND_RES *result);


typedef MYSQLND_RES_METADATA * (*func_mysqlnd_res__result_meta_init)(MYSQLND_RES *result, unsigned int field_count);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res)
{
	mysqlnd_fetch_row_func	fetch_row;

	func_mysqlnd_res__use_result use_result;
	func_mysqlnd_res__store_result store_result;
	func_mysqlnd_res__fetch_into fetch_into;
	func_mysqlnd_res__fetch_row_c fetch_row_c;
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
	func_mysqlnd_res__free_result_contents free_result_contents;

	func_mysqlnd_res__result_meta_init result_meta_init;

	void * unused1;
	void * unused2;
	void * unused3;
	void * unused4;
	void * unused5;
};


typedef uint64_t		(*func_mysqlnd_result_unbuffered__num_rows)(const MYSQLND_RES_UNBUFFERED * const result);
typedef const size_t *	(*func_mysqlnd_result_unbuffered__fetch_lengths)(const MYSQLND_RES_UNBUFFERED * const result);
typedef void			(*func_mysqlnd_result_unbuffered__free_result)(MYSQLND_RES_UNBUFFERED * const result, MYSQLND_STATS * const global_stats);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_unbuffered)
{
	mysqlnd_fetch_row_func							fetch_row;
	func_mysqlnd_res__row_decoder					row_decoder;
	func_mysqlnd_result_unbuffered__num_rows		num_rows;
	func_mysqlnd_result_unbuffered__fetch_lengths	fetch_lengths;
	func_mysqlnd_result_unbuffered__free_result		free_result;
};

typedef uint64_t			(*func_mysqlnd_result_buffered__num_rows)(const MYSQLND_RES_BUFFERED * const result);
typedef const size_t *		(*func_mysqlnd_result_buffered__fetch_lengths)(const MYSQLND_RES_BUFFERED * const result);
typedef enum_func_status	(*func_mysqlnd_result_buffered__data_seek)(MYSQLND_RES_BUFFERED * const result, const uint64_t row);
typedef void				(*func_mysqlnd_result_buffered__free_result)(MYSQLND_RES_BUFFERED * const result);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_buffered)
{
	mysqlnd_fetch_row_func						fetch_row;
	func_mysqlnd_res__row_decoder				row_decoder;
	func_mysqlnd_result_buffered__num_rows		num_rows;
	func_mysqlnd_result_buffered__fetch_lengths	fetch_lengths;
	func_mysqlnd_result_buffered__data_seek		data_seek;
	func_mysqlnd_result_buffered__free_result	free_result;
};


typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_field)(MYSQLND_RES_METADATA * const meta);
typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_field_direct)(const MYSQLND_RES_METADATA * const meta, const MYSQLND_FIELD_OFFSET fieldnr);
typedef const MYSQLND_FIELD *	(*func_mysqlnd_res_meta__fetch_fields)(MYSQLND_RES_METADATA * const meta);
typedef MYSQLND_FIELD_OFFSET	(*func_mysqlnd_res_meta__field_tell)(const MYSQLND_RES_METADATA * const meta);
typedef MYSQLND_FIELD_OFFSET	(*func_mysqlnd_res_meta__field_seek)(MYSQLND_RES_METADATA * const meta, const MYSQLND_FIELD_OFFSET field_offset);
typedef enum_func_status		(*func_mysqlnd_res_meta__read_metadata)(MYSQLND_RES_METADATA * const meta, MYSQLND_CONN_DATA * conn, MYSQLND_RES * result);
typedef MYSQLND_RES_METADATA *	(*func_mysqlnd_res_meta__clone_metadata)(MYSQLND_RES *result, const MYSQLND_RES_METADATA * const meta);
typedef void					(*func_mysqlnd_res_meta__free_metadata)(MYSQLND_RES_METADATA * meta);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res_meta)
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


typedef enum_func_status	(*func_mysqlnd_stmt__prepare)(MYSQLND_STMT * const stmt, const char * const query, const size_t query_len);
typedef enum_func_status	(*func_mysqlnd_stmt__send_execute)(MYSQLND_STMT * const s, const enum_mysqlnd_send_execute_type type, zval * read_cb, zval * err_cb);
typedef enum_func_status	(*func_mysqlnd_stmt__execute)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__use_result)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__store_result)(MYSQLND_STMT * const stmt);
typedef MYSQLND_RES *		(*func_mysqlnd_stmt__get_result)(MYSQLND_STMT * const stmt);
typedef bool			(*func_mysqlnd_stmt__more_results)(const MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__next_result)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__free_result)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__seek_data)(const MYSQLND_STMT * const stmt, uint64_t row);
typedef enum_func_status	(*func_mysqlnd_stmt__reset)(MYSQLND_STMT * const stmt);
typedef enum_func_status	(*func_mysqlnd_stmt__close_on_server)(MYSQLND_STMT * const stmt, bool implicit); /* private */
typedef enum_func_status	(*func_mysqlnd_stmt__dtor)(MYSQLND_STMT * const stmt, bool implicit); /* use this for mysqlnd_stmt_close */
typedef enum_func_status	(*func_mysqlnd_stmt__fetch)(MYSQLND_STMT * const stmt, bool * const fetched_anything);
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
typedef enum_func_status 	(*func_mysqlnd_stmt__generate_execute_request)(MYSQLND_STMT * const s, zend_uchar ** request, size_t *request_len, bool * free_buffer);
typedef enum_func_status	(*func_mysqlnd_stmt__parse_execute_response)(MYSQLND_STMT * const s, enum_mysqlnd_parse_exec_response_type type);
typedef void 				(*func_mysqlnd_stmt__free_stmt_content)(MYSQLND_STMT * const s);
typedef enum_func_status	(*func_mysqlnd_stmt__flush)(MYSQLND_STMT * const stmt);
typedef void 				(*func_mysqlnd_stmt__free_stmt_result)(MYSQLND_STMT * const s);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_stmt)
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
	func_mysqlnd_stmt__close_on_server close_on_server;
	func_mysqlnd_stmt__dtor dtor;
	func_mysqlnd_stmt__fetch fetch;

	func_mysqlnd_stmt__bind_parameters bind_parameters;
	func_mysqlnd_stmt__bind_one_parameter bind_one_parameter;
	func_mysqlnd_stmt__refresh_bind_param refresh_bind_param;
	func_mysqlnd_stmt__bind_result bind_result;
	func_mysqlnd_stmt__bind_one_result bind_one_result;
	func_mysqlnd_stmt__send_long_data send_long_data;
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


struct st_mysqlnd_vio_data
{
	php_stream			*stream;
	bool			ssl;
	MYSQLND_VIO_OPTIONS	options;
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	zend_uchar			last_command;
#else
	zend_uchar			unused_pad1;
#endif

	bool			persistent;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_vio) m;
};


struct st_mysqlnd_vio
{
	struct st_mysqlnd_vio_data * data;

	bool persistent;
};



typedef struct st_mysqlnd_connection_state MYSQLND_CONNECTION_STATE;
typedef enum mysqlnd_connection_state (*func_mysqlnd_connection_state__get)(const MYSQLND_CONNECTION_STATE * const state_struct);
typedef void (*func_mysqlnd_connection_state__set)(MYSQLND_CONNECTION_STATE * const state_struct, const enum mysqlnd_connection_state state);


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_connection_state)
{
	func_mysqlnd_connection_state__get get;
	func_mysqlnd_connection_state__set set;
};

struct st_mysqlnd_connection_state
{
	enum mysqlnd_connection_state state;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_connection_state) *m;
};

struct st_mysqlnd_connection_data
{
/* Operation related */
	MYSQLND_PFC		* protocol_frame_codec;
	MYSQLND_VIO		* vio;
	MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory;

/* Information related */
	MYSQLND_STRING	hostname;
	MYSQLND_STRING	unix_socket;
	MYSQLND_STRING	username;
	MYSQLND_STRING	password;
	MYSQLND_STRING	scheme;
	uint64_t		thread_id;
	char			*server_version;
	char			*host_info;
	MYSQLND_STRING	authentication_plugin_data;
	const MYSQLND_CHARSET *charset;
	const MYSQLND_CHARSET *greet_charset;
	MYSQLND_STRING	connect_or_select_db;
	MYSQLND_INFILE	infile;
	unsigned int	protocol_version;
	unsigned int	port;
	zend_ulong		server_capabilities;

	/* For UPSERT queries */
	MYSQLND_UPSERT_STATUS * upsert_status;
	MYSQLND_UPSERT_STATUS upsert_status_impl;
	MYSQLND_STRING last_message;

	/* If error packet, we use these */
	MYSQLND_ERROR_INFO	* error_info;
	MYSQLND_ERROR_INFO	error_info_impl;

	MYSQLND_CONNECTION_STATE	state;
	enum_mysqlnd_query_type		last_query_type;
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
	MYSQLND_SESSION_OPTIONS	* options;
	MYSQLND_SESSION_OPTIONS	options_impl;

	/* stats */
	MYSQLND_STATS	* stats;

	size_t			client_api_capabilities;

	zval			async_read_cb;
	zval			async_err_cb;
	bool		in_async_read_cb;
	bool		in_async_err_cb;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) object_factory;
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_command) * command;
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data) * m;

	/* persistent connection */
	bool		persistent;
};


struct st_mysqlnd_connection
{
	MYSQLND_CONN_DATA * data;
	bool persistent;
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn) * m;
};



struct st_mysqlnd_packet_greet;
struct st_mysqlnd_packet_auth;
struct st_mysqlnd_packet_auth_response;
struct st_mysqlnd_packet_change_auth_response;
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
struct st_mysqlnd_packet_cached_sha2_result;

typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_greet_packet)(struct st_mysqlnd_packet_greet *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_auth_packet)(struct st_mysqlnd_packet_auth *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_auth_response_packet)(struct st_mysqlnd_packet_auth_response *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_change_auth_response_packet)(struct st_mysqlnd_packet_change_auth_response *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_ok_packet)(struct st_mysqlnd_packet_ok *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_command_packet)(struct st_mysqlnd_packet_command *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_eof_packet)(struct st_mysqlnd_packet_eof *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_rset_header_packet)(struct st_mysqlnd_packet_rset_header *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_result_field_packet)(struct st_mysqlnd_packet_res_field *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_row_packet)(struct st_mysqlnd_packet_row *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_stats_packet)(struct st_mysqlnd_packet_stats *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_prepare_response_packet)(struct st_mysqlnd_packet_prepare_response *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_change_user_response_packet)(struct st_mysqlnd_packet_chg_user_resp *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_sha256_pk_request_packet)(struct st_mysqlnd_packet_sha256_pk_request *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_sha256_pk_request_response_packet)(struct st_mysqlnd_packet_sha256_pk_request_response *packet);
typedef void (*func_mysqlnd_protocol_payload_decoder_factory__init_cached_sha2_result_packet)(struct st_mysqlnd_packet_cached_sha2_result *packet);

typedef enum_func_status (*func_mysqlnd_protocol_payload_decoder_factory__send_command)(
			MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
			const enum php_mysqlnd_server_command command,
			const zend_uchar * const arg, const size_t arg_len,
			const bool silent,

			MYSQLND_CONNECTION_STATE * connection_state,
			MYSQLND_ERROR_INFO	* error_info,
			MYSQLND_UPSERT_STATUS * upsert_status,
			MYSQLND_STATS * stats,
			func_mysqlnd_conn_data__send_close send_close,
			void * send_close_ctx);

typedef enum_func_status (*func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_OK)(
			MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory,
			MYSQLND_ERROR_INFO * const error_info,
			MYSQLND_UPSERT_STATUS * const upsert_status,
			const bool ignore_upsert_status,  /* actually used only by LOAD DATA. COM_QUERY and COM_EXECUTE handle the responses themselves */
			MYSQLND_STRING * const last_message);

typedef enum_func_status (*func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_EOF)(
			MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * const payload_decoder_factory,
			MYSQLND_ERROR_INFO * const error_info,
			MYSQLND_UPSERT_STATUS * const upsert_status);

typedef enum_func_status (*func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response)(
			MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
			const enum mysqlnd_packet_type ok_packet,
			const bool silent,
			const enum php_mysqlnd_server_command command,
			const bool ignore_upsert_status, /* actually used only by LOAD DATA. COM_QUERY and COM_EXECUTE handle the responses themselves */

			MYSQLND_ERROR_INFO	* error_info,
			MYSQLND_UPSERT_STATUS * upsert_status,
			MYSQLND_STRING * last_message);


MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_payload_decoder_factory)
{
	func_mysqlnd_protocol_payload_decoder_factory__init_greet_packet init_greet_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_auth_packet init_auth_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_auth_response_packet init_auth_response_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_change_auth_response_packet init_change_auth_response_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_ok_packet init_ok_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_command_packet init_command_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_eof_packet init_eof_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_rset_header_packet init_rset_header_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_result_field_packet init_result_field_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_row_packet init_row_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_stats_packet init_stats_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_prepare_response_packet init_prepare_response_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_change_user_response_packet init_change_user_response_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_sha256_pk_request_packet init_sha256_pk_request_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_sha256_pk_request_response_packet init_sha256_pk_request_response_packet;
	func_mysqlnd_protocol_payload_decoder_factory__init_cached_sha2_result_packet init_cached_sha2_result_packet;

	func_mysqlnd_protocol_payload_decoder_factory__send_command send_command;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_response send_command_handle_response;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_OK send_command_handle_OK;
	func_mysqlnd_protocol_payload_decoder_factory__send_command_handle_EOF send_command_handle_EOF;
};

struct st_mysqlnd_protocol_payload_decoder_factory
{
	MYSQLND_CONN_DATA * conn;
	bool persistent;
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_payload_decoder_factory) m;
};


typedef struct st_mysqlnd_read_buffer {
	zend_uchar 	* data;
	size_t 		offset;
	size_t 		size;
	size_t		len;
	bool	(*is_empty)(const struct st_mysqlnd_read_buffer *);
	void		(*read)(struct st_mysqlnd_read_buffer *, size_t count, zend_uchar * dest);
	size_t		(*bytes_left)(const struct st_mysqlnd_read_buffer *);
	void		(*free_buffer)(struct st_mysqlnd_read_buffer **);
} MYSQLND_READ_BUFFER;



typedef void				(*func_mysqlnd_pfc__init)(MYSQLND_PFC * const pfc, MYSQLND_STATS * const stats, MYSQLND_ERROR_INFO * const error_info);
typedef void				(*func_mysqlnd_pfc__dtor)(MYSQLND_PFC * const pfc, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef void				(*func_mysqlnd_pfc__reset)(MYSQLND_PFC * const pfc, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_pfc__set_client_option)(MYSQLND_PFC * const pfc, enum_mysqlnd_client_option option, const char * const value);
typedef enum_func_status	(*func_mysqlnd_pfc__decode)(zend_uchar * uncompressed_data, const size_t uncompressed_data_len, const zend_uchar * const compressed_data, const size_t compressed_data_len);
typedef enum_func_status	(*func_mysqlnd_pfc__encode)(zend_uchar * compress_buffer, size_t * compress_buffer_len, const zend_uchar * const uncompressed_data, const size_t uncompressed_data_len);
typedef size_t				(*func_mysqlnd_pfc__send)(MYSQLND_PFC * const pfc, MYSQLND_VIO * const vio, zend_uchar * const buffer, const size_t count, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_pfc__receive)(MYSQLND_PFC * const pfc, MYSQLND_VIO * const vio, zend_uchar * const buffer, const size_t count, MYSQLND_STATS * const conn_stats, MYSQLND_ERROR_INFO * const error_info);
typedef enum_func_status	(*func_mysqlnd_pfc__read_compressed_packet_from_stream_and_fill_read_buffer)(MYSQLND_PFC * pfc, MYSQLND_VIO * const vio, size_t net_payload_size, MYSQLND_STATS * conn_stats, MYSQLND_ERROR_INFO * error_info);
typedef void				(*func_mysqlnd_pfc__free_contents)(MYSQLND_PFC * pfc);

MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_packet_frame_codec)
{
	func_mysqlnd_pfc__init init;
	func_mysqlnd_pfc__dtor dtor;
	func_mysqlnd_pfc__reset reset;
	func_mysqlnd_pfc__set_client_option set_client_option;

	func_mysqlnd_pfc__decode decode;
	func_mysqlnd_pfc__encode encode;

	func_mysqlnd_pfc__send send;
	func_mysqlnd_pfc__receive receive;

	func_mysqlnd_pfc__read_compressed_packet_from_stream_and_fill_read_buffer read_compressed_packet_from_stream_and_fill_read_buffer;

	func_mysqlnd_pfc__free_contents free_contents;
};


struct st_mysqlnd_protocol_frame_codec_data
{
	php_stream		*stream;
	bool		compressed;
	bool		ssl;
	uint64_t		flags;
	char *			sha256_server_public_key;

#ifdef MYSQLND_COMPRESSION_ENABLED
	MYSQLND_READ_BUFFER	* uncompressed_data;
#else
	void * 				unused_pad1;
#endif

	/* sequence for simple checking of correct packets */
	zend_uchar		packet_no;
	zend_uchar		compressed_envelope_packet_no;

	bool		persistent;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_packet_frame_codec) m;
};


struct st_mysqlnd_protocol_frame_codec
{
	MYSQLND_CMD_BUFFER	cmd_buffer;

	struct st_mysqlnd_protocol_frame_codec_data * data;

	bool 		persistent;
};


struct st_mysqlnd_result_metadata
{
	MYSQLND_FIELD					*fields;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res_meta) * m;

	unsigned int					current_field;
	unsigned int					field_count;
};


struct st_mysqlnd_buffered_result
{
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_buffered) m;

	MYSQLND_ROW_BUFFER	*row_buffers;
	uint64_t			row_count;

	/*  Column lengths of current row - both buffered and unbuffered. For buffered results it duplicates the data found in **data */
	size_t				*lengths;

	MYSQLND_MEMORY_POOL	*result_set_memory_pool;

	unsigned int		references;

	MYSQLND_ERROR_INFO	error_info;

	unsigned int		field_count;
	MYSQLND_STMT_DATA	*stmt;

	uint64_t	current_row;

	void				* unused1;
	void				* unused2;
	void				* unused3;
};


struct st_mysqlnd_unbuffered_result
{
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_unbuffered) m;
	uint64_t			row_count;

	/* For unbuffered (both normal and PS) */
	MYSQLND_ROW_BUFFER	 last_row_buffer;

	/*
	  Column lengths of current row - both buffered and unbuffered.
	  For buffered results it duplicates the data found in **data
	*/
	size_t				*lengths;

	MYSQLND_MEMORY_POOL	*result_set_memory_pool;

	struct st_mysqlnd_packet_row *row_packet;

	unsigned int		field_count;

	bool			eof_reached;

	MYSQLND_STMT_DATA	*stmt;
};


struct st_mysqlnd_res
{
	MYSQLND_CONN_DATA		*conn;
	enum_mysqlnd_res_type	type;
	unsigned int			field_count;

	zval					*row_data;
	bool					free_row_data;

	/* For metadata functions */
	MYSQLND_RES_METADATA	*meta;

	/* To be used with store_result() - both normal and PS */
	MYSQLND_RES_BUFFERED	*stored_data;
	MYSQLND_RES_UNBUFFERED	*unbuf;

	MYSQLND_MEMORY_POOL		*memory_pool;

	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res) m;
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
	bool	bound;
};


struct st_mysqlnd_stmt_data
{
	MYSQLND_CONN_DATA			*conn;
	zend_ulong					stmt_id;
	zend_ulong					flags;/* cursor is set here */
	enum_mysqlnd_stmt_state		state;
	MYSQLND_RES					*result;
	unsigned int				field_count;
	unsigned int				param_count;
	unsigned char				send_types_to_server;
	MYSQLND_PARAM_BIND			*param_bind;
	MYSQLND_RESULT_BIND			*result_bind;

	MYSQLND_UPSERT_STATUS * 	upsert_status;
	MYSQLND_UPSERT_STATUS 		upsert_status_impl;

	MYSQLND_ERROR_INFO *		error_info;
	MYSQLND_ERROR_INFO			error_info_impl;

	bool					update_max_length;
	zend_ulong					prefetch_rows;

	bool					cursor_exists;
	mysqlnd_stmt_use_or_store_func default_rset_handler;

	zval						execute_read_cb;
	zval						execute_err_cb;
	bool					in_execute_read_cb;
	bool					in_execute_err_cb;

	MYSQLND_CMD_BUFFER			execute_cmd_buffer;
	unsigned int				execute_count;/* count how many times the stmt was executed */
};


struct st_mysqlnd_stmt
{
	MYSQLND_STMT_DATA * data;
	MYSQLND_CLASS_METHODS_TYPE(mysqlnd_stmt) * m;
};


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
														const MYSQLND_SESSION_OPTIONS * const session_options,
														const MYSQLND_PFC_DATA * const pfc_data, const zend_ulong mysql_flags
														);

typedef enum_func_status (*func_auth_plugin__handle_server_response)(struct st_mysqlnd_authentication_plugin * self,
		MYSQLND_CONN_DATA * conn,
		const zend_uchar * auth_plugin_data, size_t auth_plugin_data_len,
		const char * const passwd,
		const size_t passwd_len,
		char **new_auth_protocol, size_t *new_auth_protocol_len,
		zend_uchar **new_auth_protocol_data, size_t *new_auth_protocol_data_len
		);

struct st_mysqlnd_authentication_plugin
{
	struct st_mysqlnd_plugin_header plugin_header;
	struct {
		func_auth_plugin__get_auth_data get_auth_data;
		func_auth_plugin__handle_server_response handle_server_response;
	} methods;
};

#endif /* MYSQLND_STRUCTS_H */
