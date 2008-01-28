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

#ifndef MYSQLND_STRUCTS_H
#define MYSQLND_STRUCTS_H

typedef struct st_mysqlnd_memory_pool MYSQLND_MEMORY_POOL;
typedef struct st_mysqlnd_memory_pool_chunk MYSQLND_MEMORY_POOL_CHUNK;
typedef struct st_mysqlnd_memory_pool_chunk_llist MYSQLND_MEMORY_POOL_CHUNK_LLIST;


#define MYSQLND_MEMORY_POOL_CHUNK_LIST_SIZE 100

struct st_mysqlnd_memory_pool
{
	zend_uchar *arena;
	uint refcount;
	uint arena_size;
	uint free_size;

	MYSQLND_MEMORY_POOL_CHUNK* free_chunk_list[MYSQLND_MEMORY_POOL_CHUNK_LIST_SIZE];
	uint free_chunk_list_elements;

	MYSQLND_MEMORY_POOL_CHUNK*	(*get_chunk)(MYSQLND_MEMORY_POOL * pool, uint size TSRMLS_DC);
};

struct st_mysqlnd_memory_pool_chunk
{
	uint64				app;
	MYSQLND_MEMORY_POOL *pool;
	zend_uchar			*ptr;
	uint				size;
	void				(*resize_chunk)(MYSQLND_MEMORY_POOL_CHUNK * chunk, uint size TSRMLS_DC);
	void				(*free_chunk)(MYSQLND_MEMORY_POOL_CHUNK * chunk, zend_bool cache_it TSRMLS_DC);
	zend_bool			from_pool;
};


typedef struct st_mysqlnd_cmd_buffer
{
	zend_uchar		*buffer;
	size_t			length;
} MYSQLND_CMD_BUFFER;


typedef struct st_mysqlnd_field
{
	char *name;					/* Name of column */
	char *org_name;				/* Original column name, if an alias */
	char *table;				/* Table of column if column was a field */
	char *org_table;			/* Org table name, if table was an alias */
	char *db;					/* Database for table */
	char *catalog;				/* Catalog for table */
	char *def;					/* Default value (set by mysql_list_fields) */
	unsigned long length;		/* Width of column (create length) */
	unsigned long max_length;	/* Max width for selected set */
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
	unsigned int		warning_count;
	unsigned int		server_status;
	uint64		affected_rows;
	uint64		last_insert_id;
} mysqlnd_upsert_status;


typedef struct st_mysqlnd_error_info
{
	char error[MYSQLND_ERRMSG_SIZE+1];
	char sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int error_no;
} mysqlnd_error_info;


typedef struct st_mysqlnd_zval_pcache			MYSQLND_ZVAL_PCACHE;
typedef struct st_mysqlnd_thread_zval_pcache	MYSQLND_THD_ZVAL_PCACHE;
typedef struct st_mysqlnd_qcache				MYSQLND_QCACHE;


typedef struct st_mysqlnd_infile_info
{
	php_stream			*fd;
	int					error_no;
	char				error_msg[MYSQLND_ERRMSG_SIZE + 1];
	const char			*filename;
} MYSQLND_INFILE_INFO;


/* character set information */
typedef struct st_mysqlnd_charset
{
	uint	nr;
	char	*name;
	char	*collation;
	uint	char_minlen;
	uint	char_maxlen;
	uint 	(*mb_charlen)(uint c);
	uint 	(*mb_valid)(const char *start, const char *end);
} MYSQLND_CHARSET;


/* local infile handler */
typedef struct st_mysqlnd_infile
{
	int		(*local_infile_init)(void **ptr, char *filename, void **userdata TSRMLS_DC);
	int		(*local_infile_read)(void *ptr, char *buf, uint buf_len TSRMLS_DC);
	int		(*local_infile_error)(void *ptr, char *error_msg, uint error_msg_len TSRMLS_DC);
	void	(*local_infile_end)(void *ptr TSRMLS_DC);
	zval	*callback;
	void	*userdata;
} MYSQLND_INFILE;

typedef struct st_mysqlnd_option
{
	/* timeouts */
	uint		timeout_connect;
	uint		timeout_read;
	uint		timeout_write;

	ulong		flags;

	/* init commands - we need to send them to server directly after connect */
	uint		num_commands;
	char		**init_commands;

	/* configuration file information */
	char 		*cfg_file;
	char		*cfg_section;

	/* SSL information */
	char		*ssl_key;
	char		*ssl_cert;
	char		*ssl_ca;
	char		*ssl_capath;
	char		*ssl_cipher;
	zend_bool	use_ssl;

	char 		*charset_name;
	/* maximum allowed packet size for communication */
	ulong		max_allowed_packet;

	zend_bool	numeric_and_datetime_as_unicode;
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
	zend_bool	int_and_year_as_int;
#endif
	unsigned int net_read_buffer_size;
} MYSQLND_OPTION;


typedef struct st_mysqlnd_connection MYSQLND;
typedef struct st_mysqlnd_res	MYSQLND_RES;
typedef char** 					MYSQLND_ROW_C;		/* return data as array of strings */
typedef struct st_mysqlnd_stmt	MYSQLND_STMT;
typedef unsigned int			MYSQLND_FIELD_OFFSET;

typedef struct st_mysqlnd_param_bind MYSQLND_PARAM_BIND;

typedef struct st_mysqlnd_result_bind MYSQLND_RESULT_BIND;

typedef struct st_mysqlnd_result_metadata MYSQLND_RES_METADATA;
typedef struct st_mysqlnd_buffered_result MYSQLND_RES_BUFFERED;
typedef struct st_mysqlnd_background_buffered_result MYSQLND_RES_BG_BUFFERED;
typedef struct st_mysqlnd_unbuffered_result MYSQLND_RES_UNBUFFERED;

typedef struct st_mysqlnd_debug MYSQLND_DEBUG;


typedef MYSQLND_RES* (*mysqlnd_stmt_use_or_store_func)(MYSQLND_STMT * const TSRMLS_DC);
typedef enum_func_status  (*mysqlnd_fetch_row_func)(MYSQLND_RES *result,
													void *param,
													unsigned int flags,
													zend_bool *fetched_anything
													TSRMLS_DC);

typedef struct st_mysqlnd_stats
{
	uint64	values[STAT_LAST];
#ifdef ZTS
	MUTEX_T		LOCK_access;
#endif
} MYSQLND_STATS;


typedef struct st_mysqlnd_net
{
	php_stream		*stream;
	/* sequence for simple checking of correct packets */
	zend_uchar		packet_no;

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	zend_uchar		last_command;
#endif

	/* cmd buffer */
	MYSQLND_CMD_BUFFER	cmd_buffer;
} MYSQLND_NET;


struct st_mysqlnd_conn_methods
{
	ulong				(*escape_string)(const MYSQLND * const conn, char *newstr, const char *escapestr, size_t escapestr_len TSRMLS_DC);
	enum_func_status	(*set_charset)(MYSQLND * const conn, const char * const charset TSRMLS_DC);
	enum_func_status	(*query)(MYSQLND *conn, const char *query, unsigned int query_len TSRMLS_DC);
	MYSQLND_RES *		(*use_result)(MYSQLND * const conn TSRMLS_DC);
	MYSQLND_RES *		(*store_result)(MYSQLND * const conn TSRMLS_DC);
	MYSQLND_RES *		(*background_store_result)(MYSQLND * const conn TSRMLS_DC);
	enum_func_status	(*next_result)(MYSQLND * const conn TSRMLS_DC);
	zend_bool			(*more_results)(const MYSQLND * const conn);

	MYSQLND_STMT *		(*stmt_init)(MYSQLND * const conn TSRMLS_DC);

	enum_func_status	(*shutdown_server)(MYSQLND * const conn, unsigned long level TSRMLS_DC);
	enum_func_status	(*refresh_server)(MYSQLND * const conn, unsigned long options TSRMLS_DC);

	enum_func_status	(*ping)(MYSQLND * const conn TSRMLS_DC);
	enum_func_status	(*kill_connection)(MYSQLND *conn, unsigned int pid TSRMLS_DC);
	enum_func_status	(*select_db)(MYSQLND * const conn, const char * const db, unsigned int db_len TSRMLS_DC);
	enum_func_status	(*server_dump_debug_information)(MYSQLND * const conn TSRMLS_DC);
	enum_func_status	(*change_user)(MYSQLND * const conn, const char * user, const char * passwd, const char * db TSRMLS_DC);

	unsigned int		(*get_error_no)(const MYSQLND * const conn);
	const char *		(*get_error_str)(const MYSQLND * const conn);
	const char *		(*get_sqlstate)(const MYSQLND * const conn);
	uint64		(*get_thread_id)(const MYSQLND * const conn);
	void				(*get_statistics)(const MYSQLND * const conn, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);

	unsigned long		(*get_server_version)(const MYSQLND * const conn);
	const char *		(*get_server_information)(const MYSQLND * const conn);
	enum_func_status	(*get_server_statistics)(MYSQLND *conn, char **message, unsigned int * message_len TSRMLS_DC);
	const char *		(*get_host_information)(const MYSQLND * const conn);
	unsigned int		(*get_protocol_information)(const MYSQLND * const conn);
	const char *		(*get_last_message)(const MYSQLND * const conn);
	const char *		(*charset_name)(const MYSQLND * const conn);
	MYSQLND_RES *		(*list_fields)(MYSQLND *conn, const char *table, const char *achtung_wild TSRMLS_DC);
	MYSQLND_RES *		(*list_method)(MYSQLND *conn, const char *query, const char *achtung_wild, char *par1 TSRMLS_DC);

	uint64		(*get_last_insert_id)(const MYSQLND * const conn);
	uint64		(*get_affected_rows)(const MYSQLND * const conn);
	unsigned int		(*get_warning_count)(const MYSQLND * const conn);

	unsigned int		(*get_field_count)(const MYSQLND * const conn);

	enum_func_status	(*set_server_option)(MYSQLND * const conn, enum_mysqlnd_server_option option TSRMLS_DC);
	enum_func_status	(*set_client_option)(MYSQLND * const conn, enum_mysqlnd_option option, const char * const value TSRMLS_DC);
	void				(*free_contents)(MYSQLND *conn TSRMLS_DC);	/* private */
	enum_func_status	(*close)(MYSQLND *conn, enum_connection_close_type close_type TSRMLS_DC);
	void				(*dtor)(MYSQLND *conn TSRMLS_DC);	/* private */

	MYSQLND *			(*get_reference)(MYSQLND * const conn);
	enum_func_status	(*free_reference)(MYSQLND * const conn TSRMLS_DC);
	enum mysqlnd_connection_state (*get_state)(MYSQLND * const conn TSRMLS_DC);
	void				(*set_state)(MYSQLND * const conn, enum mysqlnd_connection_state new_state TSRMLS_DC);
};


struct st_mysqlnd_res_methods
{
	mysqlnd_fetch_row_func	fetch_row;
	mysqlnd_fetch_row_func	fetch_row_normal_buffered; /* private */
	mysqlnd_fetch_row_func	fetch_row_normal_unbuffered; /* private */

	MYSQLND_RES *		(*use_result)(MYSQLND_RES * const result, zend_bool ps_protocol TSRMLS_DC);
	MYSQLND_RES *		(*store_result)(MYSQLND_RES * result, MYSQLND * const conn, zend_bool ps TSRMLS_DC);
	MYSQLND_RES *		(*background_store_result)(MYSQLND_RES * result, MYSQLND * const conn, zend_bool ps TSRMLS_DC);
	void 				(*fetch_into)(MYSQLND_RES *result, unsigned int flags, zval *return_value, enum_mysqlnd_extension ext TSRMLS_DC ZEND_FILE_LINE_DC);
	MYSQLND_ROW_C 		(*fetch_row_c)(MYSQLND_RES *result TSRMLS_DC);
	void 				(*fetch_all)(MYSQLND_RES *result, unsigned int flags, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);
	void 				(*fetch_field_data)(MYSQLND_RES *result, unsigned int offset, zval *return_value TSRMLS_DC);
	uint64		(*num_rows)(const MYSQLND_RES * const result);
	unsigned int		(*num_fields)(const MYSQLND_RES * const result);
	enum_func_status	(*skip_result)(MYSQLND_RES * const result TSRMLS_DC);
	enum_func_status	(*seek_data)(MYSQLND_RES * result, uint64 row TSRMLS_DC);
	MYSQLND_FIELD_OFFSET (*seek_field)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET field_offset);
	MYSQLND_FIELD_OFFSET (*field_tell)(const MYSQLND_RES * const result);
	MYSQLND_FIELD *		(*fetch_field)(MYSQLND_RES * const result TSRMLS_DC);
	MYSQLND_FIELD *		(*fetch_field_direct)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC);

	enum_func_status	(*read_result_metadata)(MYSQLND_RES *result, MYSQLND *conn TSRMLS_DC);
	unsigned long *		(*fetch_lengths)(MYSQLND_RES * const result);
	void				(*free_result_buffers)(MYSQLND_RES * result TSRMLS_DC);	/* private */
	enum_func_status	(*free_result)(MYSQLND_RES * result, zend_bool implicit TSRMLS_DC);
	void 				(*free_result_internal)(MYSQLND_RES *result TSRMLS_DC);
	void 				(*free_result_contents)(MYSQLND_RES *result TSRMLS_DC);

	/* for decoding - binary or text protocol */	
	void 				(*row_decoder)(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields, uint field_count, MYSQLND_FIELD *fields_metadata, MYSQLND *conn TSRMLS_DC);
};


struct st_mysqlnd_res_meta_methods
{
	MYSQLND_FIELD *			(*fetch_field)(MYSQLND_RES_METADATA * const meta TSRMLS_DC);
	MYSQLND_FIELD *			(*fetch_field_direct)(const MYSQLND_RES_METADATA * const meta, MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC);
	MYSQLND_FIELD_OFFSET	(*field_tell)(const MYSQLND_RES_METADATA * const meta);
	enum_func_status		(*read_metadata)(MYSQLND_RES_METADATA * const meta, MYSQLND *conn TSRMLS_DC);
	MYSQLND_RES_METADATA *	(*clone_metadata)(const MYSQLND_RES_METADATA * const meta, zend_bool persistent TSRMLS_DC);
	void					(*free_metadata)(MYSQLND_RES_METADATA *meta, zend_bool persistent TSRMLS_DC);
};


struct st_mysqlnd_stmt_methods
{
	enum_func_status	(*prepare)(MYSQLND_STMT * const stmt, const char * const query, unsigned int query_len TSRMLS_DC);
	enum_func_status	(*execute)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*use_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*store_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*background_store_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*get_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*free_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*seek_data)(const MYSQLND_STMT * const stmt, uint64 row TSRMLS_DC);
	enum_func_status	(*reset)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*close)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC); /* private */
	enum_func_status	(*dtor)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC); /* use this for mysqlnd_stmt_close */

	enum_func_status	(*fetch)(MYSQLND_STMT * const stmt, zend_bool * const fetched_anything TSRMLS_DC);

	enum_func_status	(*bind_param)(MYSQLND_STMT * const stmt, MYSQLND_PARAM_BIND * const param_bind TSRMLS_DC);
	enum_func_status	(*refresh_bind_param)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*bind_result)(MYSQLND_STMT * const stmt, MYSQLND_RESULT_BIND * const result_bind TSRMLS_DC);
	enum_func_status	(*send_long_data)(MYSQLND_STMT * const stmt, unsigned int param_num,
										  const char * const data, unsigned long length TSRMLS_DC);
	MYSQLND_RES	*		(*get_parameter_metadata)(MYSQLND_STMT * const stmt);
	MYSQLND_RES	*		(*get_result_metadata)(MYSQLND_STMT * const stmt TSRMLS_DC);

	uint64		(*get_last_insert_id)(const MYSQLND_STMT * const stmt);
	uint64		(*get_affected_rows)(const MYSQLND_STMT * const stmt);
	uint64		(*get_num_rows)(const MYSQLND_STMT * const stmt);

	unsigned int		(*get_param_count)(const MYSQLND_STMT * const stmt);
	unsigned int		(*get_field_count)(const MYSQLND_STMT * const stmt);
	unsigned int		(*get_warning_count)(const MYSQLND_STMT * const stmt);

	unsigned int		(*get_error_no)(const MYSQLND_STMT * const stmt);
	const char *		(*get_error_str)(const MYSQLND_STMT * const stmt);
	const char *		(*get_sqlstate)(const MYSQLND_STMT * const stmt);

	enum_func_status	(*get_attribute)(MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, void * const value TSRMLS_DC);
	enum_func_status	(*set_attribute)(MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, const void * const value TSRMLS_DC);
};


struct st_mysqlnd_connection
{
/* Operation related */
	MYSQLND_NET		net;

/* Information related */
	char			*host;
	char			*unix_socket;
	char			*user;
	char			*passwd;
	unsigned int	*passwd_len;
	char			*scheme;
	uint64	thread_id;
	char			*server_version;
	char			*host_info;
	unsigned char	*scramble;
	const MYSQLND_CHARSET *charset;
	const MYSQLND_CHARSET *greet_charset;
	MYSQLND_INFILE	infile;
	unsigned int	protocol_version;
	unsigned long	max_packet_size;
	unsigned int	port;
	unsigned long	client_flag;
	unsigned long	server_capabilities;

	int				tmp_int;


	/* For UPSERT queries */
	mysqlnd_upsert_status upsert_status;
	char			*last_message;
	unsigned int	last_message_len;

	/* If error packet, we use these */
	mysqlnd_error_info	error_info;

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
	
	/* persistent connection */
	zend_bool		persistent;

	/* options */
	MYSQLND_OPTION	options;

	/* zval cache */
	MYSQLND_THD_ZVAL_PCACHE	*zval_cache;

	/* qcache */
	MYSQLND_QCACHE	*qcache;

	MYSQLND_MEMORY_POOL * result_set_memory_pool;

	/* stats */
	MYSQLND_STATS	stats;

#ifdef ZTS
	MUTEX_T			LOCK_state;

	pthread_cond_t	COND_work_done;

	pthread_mutex_t	LOCK_work;
	pthread_cond_t	COND_work;
	pthread_cond_t	COND_thread_ended;
	zend_bool		thread_is_running;
	zend_bool		thread_killed;
	void ***		tsrm_ls;
#endif


	struct st_mysqlnd_conn_methods *m;
};

typedef struct st_php_mysql_packet_row php_mysql_packet_row;


struct mysqlnd_field_hash_key
{
	zend_bool		is_numeric;
	unsigned long	key;
#if PHP_MAJOR_VERSION >= 6
	zstr			ustr;
	unsigned int	ulen;
#endif	
};


struct st_mysqlnd_result_metadata
{
	MYSQLND_FIELD					*fields;
	struct mysqlnd_field_hash_key	*zend_hash_keys;
	unsigned int					current_field;
	unsigned int					field_count;
	/* We need this to make fast allocs in rowp_read */
	unsigned int					bit_fields_count;
	size_t							bit_fields_total_len; /* trailing \0 not counted */

	struct st_mysqlnd_res_meta_methods *m;
};


struct st_mysqlnd_background_buffered_result
{
	zval				***data;
	uint64		data_size;
	zval				***data_cursor;
	MYSQLND_MEMORY_POOL_CHUNK **row_buffers;
	uint64		row_count;
	uint64		initialized_rows;
	zend_bool			persistent;

	MYSQLND_QCACHE		*qcache;
	unsigned int		references;

	zend_bool			decode_in_foreground;

#ifdef ZTS
	zend_bool			bg_fetch_finished;
	MUTEX_T				LOCK;
#endif

	mysqlnd_error_info		error_info;
	mysqlnd_upsert_status	upsert_status;
};


struct st_mysqlnd_buffered_result
{
	zval				**data;
	zval				**data_cursor;
	MYSQLND_MEMORY_POOL_CHUNK **row_buffers;
	uint64		row_count;
	uint64		initialized_rows;
	zend_bool			persistent;

	MYSQLND_QCACHE		*qcache;
	unsigned int		references;

	mysqlnd_error_info	error_info;
};


struct st_mysqlnd_unbuffered_result
{
	/* For unbuffered (both normal and PS) */
	zval				**last_row_data;
	MYSQLND_MEMORY_POOL_CHUNK *last_row_buffer;

	uint64		row_count;
	zend_bool			eof_reached;
};


struct st_mysqlnd_res
{
	struct st_mysqlnd_res_methods m;

	MYSQLND					*conn;
	enum_mysqlnd_res_type	type;
	unsigned int			field_count;

	/* For metadata functions */
	MYSQLND_RES_METADATA	*meta;

	/* To be used with store_result() - both normal and PS */
	MYSQLND_RES_BUFFERED		*stored_data;
	MYSQLND_RES_BG_BUFFERED		*bg_stored_data;
	MYSQLND_RES_UNBUFFERED		*unbuf;

	/*
	  Column lengths of current row - both buffered and unbuffered.
	  For buffered results it duplicates the data found in **data 
	*/
	unsigned long			*lengths;

	php_mysql_packet_row	*row_packet;	/* Unused for PS */

	/* zval cache */
	MYSQLND_THD_ZVAL_PCACHE		*zval_cache;
};


struct st_mysqlnd_param_bind
{
	zval		*zv;
	zend_uchar	type;
	enum_param_bind_flags	flags;
};

struct st_mysqlnd_result_bind
{
	zval		*zv;
	zend_uchar	original_type;
	zend_bool	bound;
};


struct st_mysqlnd_stmt
{
	MYSQLND						*conn;
	unsigned long				stmt_id;
	unsigned long				flags;/* cursor is set here */
	enum_mysqlnd_stmt_state		state;
	unsigned int				warning_count;
	MYSQLND_RES					*result;
	unsigned int				field_count;
	unsigned int				param_count;
	unsigned char				send_types_to_server;
	MYSQLND_PARAM_BIND			*param_bind;
	MYSQLND_RESULT_BIND			*result_bind;
	zend_bool					result_zvals_separated_once;

	mysqlnd_upsert_status		upsert_status;

	mysqlnd_error_info			error_info;

	zend_bool					update_max_length;
	unsigned long				prefetch_rows;

	zend_bool					cursor_exists;
	mysqlnd_stmt_use_or_store_func default_rset_handler;

	MYSQLND_CMD_BUFFER			cmd_buffer;
	unsigned int				execute_count;/* count how many times the stmt was executed */

	struct st_mysqlnd_stmt_methods	*m;
};

#endif /* MYSQLND_STRUCTS_H */
