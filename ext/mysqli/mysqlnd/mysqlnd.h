/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
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

#ifndef MYSQLND_H
#define MYSQLND_H

#define MYSQLND_VERSION "mysqlnd 5.0.2-dev - 070702 - $Revision$"
#define MYSQLND_VERSION_ID 50002

#define phpext_mysqlnd_ptr &mysqlnd_module_entry
extern zend_module_entry mysqlnd_module_entry;


/* This forces inlining of some accessor functions */
#define MYSQLND_USE_OPTIMISATIONS 0

/* #define MYSQLND_STRING_TO_INT_CONVERSION */
/*
  This force mysqlnd to do a single (or more depending on ammount of data)
  non-blocking read() calls before sending a command to the server. Useful
  for debugging, if previous function hasn't consumed all the output sent
  to it - like stmt_send_long_data() error because the data was larger that
  max_allowed_packet_size, and COM_STMT_SEND_LONG_DATA by protocol doesn't
  use response packets, thus letting the next command to fail miserably, if
  the connector implementor is not aware of this deficiency. Should be off
  on production systems, if of course measured performance degradation is not
  minimal.
*/
#define MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND 1

#include "portability.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#include "mysqlnd_enum_n_def.h"

/*
          /-----> CONN_CLOSE  <---------------\
         |           ^                         \
         |           |                         \
	CONN_READY -> CONN_QUERY_SENT -> CONN_FETCHING_DATA
	    ^                                      |
		\-------------------------------------/
*/
typedef enum mysqlnd_connection_state
{
	CONN_ALLOCED = 0,
	CONN_READY,
	CONN_QUERY_SENT,
	CONN_SENDING_LOAD_DATA,
	CONN_FETCHING_DATA,
	CONN_NEXT_RESULT_PENDING,
	CONN_QUIT_SENT, /* object is "destroyed" at this stage */
} enum_mysqlnd_connection_state;


typedef enum mysqlnd_stmt_state {
	MYSQLND_STMT_INITTED = 0,
	MYSQLND_STMT_PREPARED,
	MYSQLND_STMT_EXECUTED,
	MYSQLND_STMT_WAITING_USE_OR_STORE,
	MYSQLND_STMT_USE_OR_STORE_CALLED,
	MYSQLND_STMT_USER_FETCHING, /* fetch_row_buff or fetch_row_unbuf */
} enum_mysqlnd_stmt_state;


typedef enum param_bind_flags {
	MYSQLND_PARAM_BIND_BLOB_USED = 1
} enum_param_bind_flags;


/* PS */
enum mysqlnd_stmt_attr
{
	STMT_ATTR_UPDATE_MAX_LENGTH,
	STMT_ATTR_CURSOR_TYPE,
	STMT_ATTR_PREFETCH_ROWS
};

enum myslqnd_cursor_type
{
	CURSOR_TYPE_NO_CURSOR= 0,
	CURSOR_TYPE_READ_ONLY= 1,
	CURSOR_TYPE_FOR_UPDATE= 2,
	CURSOR_TYPE_SCROLLABLE= 4
};

typedef enum mysqlnd_connection_close_type
{
	MYSQLND_CLOSE_EXPLICIT = 0,
	MYSQLND_CLOSE_IMPLICIT,
	MYSQLND_CLOSE_DISCONNECTED,
	MYSQLND_CLOSE_LAST	/* for checking, should always be last */
} enum_connection_close_type;

typedef enum mysqlnd_collected_stats {
	STAT_BYTES_SENT,
	STAT_BYTES_RECEIVED,
	STAT_PACKETS_SENT,
	STAT_PACKETS_RECEIVED,
	STAT_PROTOCOL_OVERHEAD_IN,
	STAT_PROTOCOL_OVERHEAD_OUT,
	STAT_RSET_QUERY,
	STAT_NON_RSET_QUERY,
	STAT_NO_INDEX_USED,
	STAT_BAD_INDEX_USED,
	STAT_BUFFERED_SETS,
	STAT_UNBUFFERED_SETS,
	STAT_PS_BUFFERED_SETS,
	STAT_PS_UNBUFFERED_SETS,
	STAT_FLUSHED_NORMAL_SETS,
	STAT_FLUSHED_PS_SETS,
	STAT_ROWS_FETCHED_FROM_SERVER,
	STAT_ROWS_FETCHED_FROM_CLIENT,
	STAT_ROWS_SKIPPED,
	STAT_COPY_ON_WRITE_SAVED,
	STAT_COPY_ON_WRITE_PERFORMED,
	STAT_CMD_BUFFER_TOO_SMALL,
	STAT_CONNECT_SUCCESS,
	STAT_CONNECT_FAILURE,
	STAT_CONNECT_REUSED,
	STAT_CLOSE_EXPLICIT,
	STAT_CLOSE_IMPLICIT,
	STAT_CLOSE_DISCONNECT,
	STAT_CLOSE_IN_MIDDLE,
	STAT_FREE_RESULT_EXPLICIT,
	STAT_FREE_RESULT_IMPLICIT,
	STAT_STMT_CLOSE_EXPLICIT,
	STAT_STMT_CLOSE_IMPLICIT,
	STAT_LAST /* Should be always the last */
} enum_mysqlnd_collected_stats;


typedef struct st_mysqlnd_cmd_buffer {
	zend_uchar		*buffer;
	size_t			length;
} MYSQLND_CMD_BUFFER;


typedef struct st_mysqlnd_field {
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



typedef struct st_mysqlnd_upsert_result {
	unsigned int		warning_count;
	unsigned int		server_status;
	unsigned long long	affected_rows;
	unsigned long long	last_insert_id;
} mysqlnd_upsert_status;


typedef struct st_mysqlnd_error_info {
	char error[MYSQLND_ERRMSG_SIZE+1];
	char sqlstate[MYSQLND_SQLSTATE_LENGTH + 1];
	unsigned int error_no;
} mysqlnd_error_info;


typedef struct st_mysqlnd_zval_pcache			MYSQLND_ZVAL_PCACHE;
typedef struct st_mysqlnd_thread_zval_pcache	MYSQLND_THD_ZVAL_PCACHE;
typedef struct st_mysqlnd_qcache				MYSQLND_QCACHE;


typedef struct st_mysqlnd_infile_info {
	php_stream			*fd;
	int					error_no;
	char				error_msg[MYSQLND_ERRMSG_SIZE + 1];
	const char			*filename;
	zval				*callback;
} MYSQLND_INFILE_INFO;


/* character set information */
typedef struct st_mysqlnd_charset
{
	uint	nr;
	char	*name;
	char	*collation;
	uint	char_minlen;
	uint	char_maxlen;
	uint	dangerous_for_escape_backslash;
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

typedef struct st_mysqlnd_option {
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
} MYSQLND_OPTION;


typedef struct st_mysqlnd_connection MYSQLND;
typedef struct st_mysqlnd_res	MYSQLND_RES;
typedef char** 					MYSQLND_ROW;		/* return data as array of strings */
typedef struct st_mysqlnd_stmt	MYSQLND_STMT;
typedef unsigned int			MYSQLND_FIELD_OFFSET;

typedef struct st_mysqlnd_param_bind MYSQLND_PARAM_BIND;

typedef struct st_mysqlnd_result_bind MYSQLND_RESULT_BIND;

typedef struct st_mysqlnd_result_metadata MYSQLND_RES_METADATA;
typedef struct st_mysqlnd_buffered_result MYSQLND_RES_BUFFERED;
typedef struct st_mysqlnd_unbuffered_result MYSQLND_RES_UNBUFFERED;


typedef MYSQLND_RES* (*mysqlnd_stmt_use_or_store_func)(MYSQLND_STMT * const TSRMLS_DC);
typedef enum_func_status  (*mysqlnd_fetch_row_func)(MYSQLND_RES *result,
													void *param,
													unsigned int flags,
													zend_bool *fetched_anything
													TSRMLS_DC);

typedef struct st_mysqlnd_stats {
	my_uint64	values[STAT_LAST];
#ifdef ZTS
	MUTEX_T		LOCK_access;
#endif
} MYSQLND_STATS;


typedef struct st_mysqlnd_net {
	php_stream		*stream;
	/* sequence for simple checking of correct packets */
	zend_uchar		packet_no;

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	zend_uchar		last_command;
#endif

	/* cmd buffer */
	MYSQLND_CMD_BUFFER	cmd_buffer;
} MYSQLND_NET;


struct st_mysqlnd_conn_methods {
	ulong				(*escape_string)(const MYSQLND * const conn, char *newstr, const char *escapestr, int escapestr_len);
	enum_func_status	(*set_charset)(MYSQLND * const conn, const char * const charset TSRMLS_DC);
	enum_func_status	(*query)(MYSQLND *conn, const char *query, unsigned int query_len TSRMLS_DC);
	MYSQLND_RES *		(*use_result)(MYSQLND * const conn TSRMLS_DC);
	MYSQLND_RES *		(*store_result)(MYSQLND * const conn TSRMLS_DC);
	enum_func_status	(*next_result)(MYSQLND * const conn TSRMLS_DC);
	zend_bool			(*more_results)(const MYSQLND * const conn);

	MYSQLND_STMT *		(*stmt_init)(MYSQLND * const conn);

	enum_func_status	(*shutdown_server)(MYSQLND * const conn, unsigned long level TSRMLS_DC);
	enum_func_status	(*refresh_server)(MYSQLND * const conn, unsigned long options TSRMLS_DC);

	enum_func_status	(*ping)(MYSQLND * const conn TSRMLS_DC);
	enum_func_status	(*kill_connection)(MYSQLND *conn, unsigned long pid TSRMLS_DC);
	enum_func_status	(*select_db)(MYSQLND * const conn, const char * const db, unsigned int db_len TSRMLS_DC);
	enum_func_status	(*server_dump_debug_information)(MYSQLND * const conn TSRMLS_DC);
	enum_func_status	(*change_user)(MYSQLND * const conn, const char * user, const char * passwd, const char * db TSRMLS_DC);

	unsigned int		(*get_error_no)(const MYSQLND * const conn);
	const char *		(*get_error_str)(const MYSQLND * const conn);
	const char *		(*get_sqlstate)(const MYSQLND * const conn);
	mynd_ulonglong		(*get_thread_id)(const MYSQLND * const conn);
	void				(*get_statistics)(const MYSQLND * const conn, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);

	unsigned long		(*get_server_version)(const MYSQLND * const conn);
	const char *		(*get_server_information)(const MYSQLND * const conn);
	enum_func_status	(*get_server_statistics)(MYSQLND *conn, char **message, unsigned int * message_len TSRMLS_DC);
	const char *		(*get_host_information)(const MYSQLND * const conn);
	unsigned int		(*get_protocol_information)(const MYSQLND * const conn);
	const char *		(*get_last_message)(const MYSQLND * const conn);
	const char *		(*charset_name)(const MYSQLND * const conn);
	MYSQLND_RES *		(*list_method)(MYSQLND *conn, const char *query, char *achtung_wild, char *par1 TSRMLS_DC);

	mynd_ulonglong		(*get_last_insert_id)(const MYSQLND * const conn);
	mynd_ulonglong		(*get_affected_rows)(const MYSQLND * const conn);
	unsigned int		(*get_warning_count)(const MYSQLND * const conn);

	unsigned int		(*get_field_count)(const MYSQLND * const conn);

	enum_func_status	(*set_server_option)(MYSQLND * const conn, enum_mysqlnd_server_option option TSRMLS_DC);
	enum_func_status	(*set_client_option)(MYSQLND * const conn, enum_mysqlnd_option option, const char * const value);
	void				(*free_contents)(MYSQLND *conn TSRMLS_DC);	/* private */
	enum_func_status	(*close)(MYSQLND *conn, enum_connection_close_type close_type TSRMLS_DC);
	void				(*dtor)(MYSQLND *conn TSRMLS_DC);	/* private */

	MYSQLND *			(*get_reference)(MYSQLND * const conn);
	void				(*free_reference)(MYSQLND * const conn TSRMLS_DC);
};


struct st_mysqlnd_res_methods {
	mysqlnd_fetch_row_func	fetch_row;
	mysqlnd_fetch_row_func	fetch_row_normal_buffered; /* private */
	mysqlnd_fetch_row_func	fetch_row_normal_unbuffered; /* private */

	MYSQLND_RES *		(*use_result)(MYSQLND_RES * const result, zend_bool ps_protocol TSRMLS_DC);
	MYSQLND_RES *		(*store_result)(MYSQLND_RES * result, MYSQLND * const conn, zend_bool ps TSRMLS_DC);
	void 				(*fetch_into)(MYSQLND_RES *result, unsigned int flags, zval *return_value, enum_mysqlnd_extension ext TSRMLS_DC ZEND_FILE_LINE_DC);
	void 				(*fetch_all)(MYSQLND_RES *result, unsigned int flags, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);
	void 				(*fetch_field_data)(MYSQLND_RES *result, unsigned int offset, zval *return_value TSRMLS_DC);
	mynd_ulonglong		(*num_rows)(const MYSQLND_RES * const result);
	unsigned int		(*num_fields)(const MYSQLND_RES * const result);
	enum_func_status	(*skip_result)(MYSQLND_RES * const result TSRMLS_DC);
	enum_func_status	(*seek_data)(MYSQLND_RES * result, mynd_ulonglong row);
	MYSQLND_FIELD_OFFSET (*seek_field)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET field_offset);
	MYSQLND_FIELD_OFFSET (*field_tell)(const MYSQLND_RES * const result);
	MYSQLND_FIELD *		(*fetch_field)(MYSQLND_RES * const result);
	MYSQLND_FIELD *		(*fetch_field_direct)(const MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET fieldnr);

	enum_func_status	(*read_result_metadata)(MYSQLND_RES *result, MYSQLND *conn TSRMLS_DC);
	unsigned long *		(*fetch_lengths)(MYSQLND_RES * const result);
	void				(*free_result_buffers)(MYSQLND_RES * result TSRMLS_DC);	/* private */
	enum_func_status	(*free_result)(MYSQLND_RES * result, zend_bool implicit TSRMLS_DC);
	void 				(*free_result_internal)(MYSQLND_RES *result TSRMLS_DC);
	void 				(*free_result_contents)(MYSQLND_RES *result TSRMLS_DC);
};


struct st_mysqlnd_res_meta_methods {
	MYSQLND_FIELD *			(*fetch_field)(MYSQLND_RES_METADATA * const meta);
	MYSQLND_FIELD_OFFSET	(*field_tell)(const MYSQLND_RES_METADATA * const meta);
	enum_func_status		(*read_metadata)(MYSQLND_RES_METADATA * const meta, MYSQLND *conn TSRMLS_DC);
	MYSQLND_RES_METADATA *	(*clone_metadata)(const MYSQLND_RES_METADATA * const meta, zend_bool persistent);
	void					(*free_metadata)(MYSQLND_RES_METADATA *meta, zend_bool persistent TSRMLS_DC);
};


struct st_mysqlnd_stmt_methods {
	enum_func_status	(*prepare)(MYSQLND_STMT * const stmt, const char * const query, unsigned int query_len TSRMLS_DC);
	enum_func_status	(*execute)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*use_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*store_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	MYSQLND_RES *		(*get_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*free_result)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*seek_data)(const MYSQLND_STMT * const stmt, mynd_ulonglong row);
	enum_func_status	(*reset)(MYSQLND_STMT * const stmt TSRMLS_DC);
	enum_func_status	(*close)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC); /* private */
	enum_func_status	(*dtor)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC); /* use this for mysqlnd_stmt_close */

	enum_func_status	(*fetch)(MYSQLND_STMT * const stmt, zend_bool * const fetched_anything TSRMLS_DC);

	enum_func_status	(*bind_param)(MYSQLND_STMT * const stmt, MYSQLND_PARAM_BIND * const param_bind);
	enum_func_status	(*bind_result)(MYSQLND_STMT * const stmt, MYSQLND_RESULT_BIND * const result_bind);
	enum_func_status	(*send_long_data)(MYSQLND_STMT * const stmt, unsigned int param_num,
										  const char * const data, unsigned long length TSRMLS_DC);
	MYSQLND_RES	*		(*get_parameter_metadata)(MYSQLND_STMT * const stmt);
	MYSQLND_RES	*		(*get_result_metadata)(MYSQLND_STMT * const stmt);

	mynd_ulonglong		(*get_last_insert_id)(const MYSQLND_STMT * const stmt);
	mynd_ulonglong		(*get_affected_rows)(const MYSQLND_STMT * const stmt);
	mynd_ulonglong		(*get_num_rows)(const MYSQLND_STMT * const stmt);

	unsigned int		(*get_param_count)(const MYSQLND_STMT * const stmt);
	unsigned int		(*get_field_count)(const MYSQLND_STMT * const stmt);
	unsigned int		(*get_warning_count)(const MYSQLND_STMT * const stmt);

	unsigned int		(*get_error_no)(const MYSQLND_STMT * const stmt);
	const char *		(*get_error_str)(const MYSQLND_STMT * const stmt);
	const char *		(*get_sqlstate)(const MYSQLND_STMT * const stmt);

	enum_func_status	(*get_attribute)(MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, void * const value);
	enum_func_status	(*set_attribute)(MYSQLND_STMT * const stmt, enum mysqlnd_stmt_attr attr_type, const void * const value);
};


struct st_mysqlnd_connection {
/* Operation related */
	MYSQLND_NET		net;

/* Information related */
	char			*host;
	char			*unix_socket;
	char			*user;
	char			*passwd;
	unsigned int	*passwd_len;
	char			*scheme;
	unsigned long long	thread_id;
	char			*server_version;
	char			*host_info;
	unsigned char	*scramble;
	const MYSQLND_CHARSET *charset;
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
	MYSQLND_QCACHE		*qcache;

	/* stats */
	MYSQLND_STATS	stats;

	struct st_mysqlnd_conn_methods *m;
};

typedef struct st_php_mysql_packet_row php_mysql_packet_row;


struct mysqlnd_field_hash_key {
	zend_bool		is_numeric;
	unsigned long	key;
#if PHP_MAJOR_VERSION >= 6
	zstr			ustr;
	unsigned int	ulen;
#endif	
};


struct st_mysqlnd_result_metadata {
	MYSQLND_FIELD					*fields;
	struct mysqlnd_field_hash_key	*zend_hash_keys;
	unsigned int					current_field;
	unsigned int					field_count;
	/* We need this to make fast allocs in rowp_read */
	unsigned int					bit_fields_count;
	size_t							bit_fields_total_len; /* trailing \0 not counted */

	struct st_mysqlnd_res_meta_methods *m;
};


struct st_mysqlnd_buffered_result {
	zval				***data;
	zval				***data_cursor;
	zend_uchar			**row_buffers;
	mynd_ulonglong		row_count;
	zend_bool			persistent;

	MYSQLND_QCACHE		*qcache;
	unsigned int		references;

	zend_bool			async_invalid;
	mysqlnd_error_info	error_info;
};


struct st_mysqlnd_unbuffered_result {
	/* For unbuffered (both normal and PS) */
	zval				**last_row_data;
	zend_uchar			*last_row_buffer;

	mynd_ulonglong		row_count;
	zend_bool			eof_reached;
};


struct st_mysqlnd_res {
	struct st_mysqlnd_res_methods m;

	MYSQLND					*conn;
	enum_mysqlnd_res_type	type;
	unsigned int			field_count;

	/* For metadata functions */
	MYSQLND_RES_METADATA	*meta;

	/* To be used with store_result() - both normal and PS */
	MYSQLND_RES_BUFFERED	*data;

	MYSQLND_RES_UNBUFFERED	*unbuf;

	/*
	  Column lengths of current row - both buffered and unbuffered.
	  For buffered results it duplicates the data found in **data 
	*/
	unsigned long			*lengths;

	php_mysql_packet_row	*row_packet;	/* Unused for PS */

	/* zval cache */
	MYSQLND_THD_ZVAL_PCACHE		*zval_cache;

};




#define MYSQLND_DEFAULT_PREFETCH_ROWS (ulong) 1

struct st_mysqlnd_param_bind {
	zval		*zv;
	zend_uchar	type;
	enum_param_bind_flags	flags;
};

struct st_mysqlnd_result_bind {
	zval		*zv;
	zend_uchar	original_type;
	zend_bool	bound;
};


struct st_mysqlnd_stmt {
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

	struct st_mysqlnd_stmt_methods	*m;
};



/* Library related */
PHPAPI void mysqlnd_library_init();
PHPAPI void mysqlnd_library_end();
PHPAPI void mysqlnd_restart_psession(MYSQLND *conn);
PHPAPI void mysqlnd_end_psession(MYSQLND *conn);
PHPAPI void mysqlnd_minfo_print_hash(zval *values);
#define mysqlnd_thread_safe()	TRUE

PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_nr(uint charsetno);
PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_name(const char * const charsetname);


/* Connect */
PHPAPI MYSQLND * mysqlnd_init(zend_bool persistent);
PHPAPI MYSQLND * mysqlnd_connect(MYSQLND *conn,
						  char *host, char *user,
						  char *passwd, unsigned int passwd_len,
						  char *db, unsigned int db_len,
						  unsigned int port,
						  char *socket,
						  unsigned int mysql_flags,
						  MYSQLND_THD_ZVAL_PCACHE *zval_cache
						  TSRMLS_DC);

#define mysqlnd_change_user(conn, user, passwd, db)		(conn)->m->change_user((conn), (user), (passwd), (db) TSRMLS_CC)

#define mysqlnd_debug(x)

/* Query */
#define mysqlnd_fetch_into(result, flags, ret_val, ext)	(result)->m.fetch_into((result), (flags), (ret_val), (ext) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_fetch_all(result, flags, return_value)	(result)->m.fetch_all((result), (flags), (return_value) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_result_fetch_field_data(res,offset,ret)	(res)->m.fetch_field_data((res), (offset), (ret) TSRMLS_CC)
#define mysqlnd_get_connection_stats(conn, values)		(conn)->m->get_statistics((conn),  (values) TSRMLS_CC ZEND_FILE_LINE_CC)
#define mysqlnd_get_client_stats(values)				_mysqlnd_get_client_stats((values) TSRMLS_CC ZEND_FILE_LINE_CC)

#define mysqlnd_close(conn,is_forced)					(conn)->m->close((conn), (is_forced) TSRMLS_CC)
#define mysqlnd_query(conn, query_str, query_len)		(conn)->m->query((conn), (query_str), (query_len) TSRMLS_CC)
#define mysqlnd_unbuffered_skip_result(result)			(result)->m.skip_result((result) TSRMLS_CC)


#define mysqlnd_use_result(conn)		(conn)->m->use_result((conn) TSRMLS_CC)
#define mysqlnd_store_result(conn)		(conn)->m->store_result((conn) TSRMLS_CC)
#define mysqlnd_next_result(conn)		(conn)->m->next_result((conn) TSRMLS_CC)
#define mysqlnd_more_results(conn)		(conn)->m->more_results((conn))
#define mysqlnd_free_result(r,e_or_i)	((MYSQLND_RES*)r)->m.free_result(((MYSQLND_RES*)(r)), (e_or_i) TSRMLS_CC)
#define mysqlnd_data_seek(result, row)	(result)->m.seek_data((result), (row))

/*****************************************************************************************************/
#if defined(MYSQLND_USE_OPTIMISATIONS) && MYSQLND_USE_OPTIMISATIONS == 1

/* Errors */
#define mysqlnd_errno(conn)				(conn)->error_info.error_no
#define mysqlnd_error(conn)				(conn)->error_info.error
#define mysqlnd_sqlstate(conn)  		((conn)->error_info.sqlstate[0] ? conn->error_info.sqlstate:MYSQLND_SQLSTATE_NULL)

/* Charset */
#define mysqlnd_character_set_name(conn) (conn)->charset->name

/* Simple metadata */
#define mysqlnd_field_count(conn)		(conn)->field_count
#define mysqlnd_insert_id(conn)			(conn)->upsert_status.last_insert_id
#define mysqlnd_affected_rows(conn)		(conn)->upsert_status.affected_rows
#define mysqlnd_warning_count(conn)		(conn)->upsert_status.warning_count
#define mysqlnd_info(conn)  			(conn)->last_message
#define mysqlnd_get_server_info(conn)	(conn)->server_version
#define mysqlnd_get_host_info(conn)		(conn)->host_info
#define mysqlnd_get_proto_info(conn)	(conn)->protocol_version
#define mysqlnd_thread_id(conn)			(conn)->thread_id

#define mysqlnd_num_rows(result)		((result)->data? (result)->data->row_count:0)
#define mysqlnd_num_fields(result)		(result)->field_count

#define mysqlnd_fetch_lengths(result)	((result)->m.fetch_lengths? (result)->m.fetch_lengths((result)):NULL)

#define mysqlnd_field_seek(result, ofs)			(result)->m.seek_field((result), (ofs))
#define mysqlnd_field_tell(result)				(result)->meta? (result)->meta->current_field:0)
#define mysqlnd_fetch_field(result)				(result)->m.fetch_field((result))
#define mysqlnd_fetch_field_direct(result,fnr)	((result)->meta? &((result)->meta->fields[(fnr)]):NULL)

/* mysqlnd metadata */
#define mysqlnd_get_client_info()		MYSQLND_VERSION
#define mysqlnd_get_client_version()	MYSQLND_VERSION_ID

/* PS */
#define mysqlnd_stmt_insert_id(stmt)		(stmt)->upsert_status.last_insert_id
#define mysqlnd_stmt_affected_rows(stmt)	(stmt)->upsert_status.affected_rows
#define mysqlnd_stmt_num_rows(stmt)			(stmt)->result? mysqlnd_num_rows((stmt)->result):0
#define mysqlnd_stmt_param_count(stmt)		(stmt)->param_count
#define mysqlnd_stmt_field_count(stmt)		(stmt)->field_count
#define mysqlnd_stmt_warning_count(stmt)	(stmt)->upsert_status.warning_count
#define mysqlnd_stmt_errno(stmt)			(stmt)->error_info.error_no
#define mysqlnd_stmt_error(stmt)			(stmt)->error_info.error
#define mysqlnd_stmt_sqlstate(stmt)			((stmt)->error_info.sqlstate[0] ? (stmt)->error_info.sqlstate:MYSQLND_SQLSTATE_NULL)



/*****************************************************************************************************/
#else /* Using plain functions */
/*****************************************************************************************************/

/* Errors */
#define mysqlnd_errno(conn)				(conn)->m->get_error_no((conn))
#define mysqlnd_error(conn)				(conn)->m->get_error_str((conn))
#define mysqlnd_sqlstate(conn)  		(conn)->m->get_sqlstate((conn))

/* Charset */
#define mysqlnd_character_set_name(conn) (conn)->m->charset_name((conn))

/* Simple metadata */
#define mysqlnd_field_count(conn)		(conn)->m->get_field_count((conn))
#define mysqlnd_insert_id(conn)			(conn)->m->get_last_insert_id((conn))
#define mysqlnd_affected_rows(conn)		(conn)->m->get_affected_rows((conn))
#define mysqlnd_warning_count(conn)		(conn)->m->get_warning_count((conn))
#define mysqlnd_info(conn)				(conn)->m->get_last_message((conn))
#define mysqlnd_get_server_info(conn)	(conn)->m->get_server_information((conn))
#define mysqlnd_get_host_info(conn)		(conn)->m->get_host_information((conn))
#define mysqlnd_get_proto_info(conn)	(conn)->m->get_protocol_information((conn))
#define mysqlnd_thread_id(conn)			(conn)->m->get_thread_id((conn))

#define mysqlnd_num_rows(result)		(result)->m.num_rows((result))
#define mysqlnd_num_fields(result)		(result)->m.num_fields((result))

PHPAPI unsigned long * mysqlnd_fetch_lengths(MYSQLND_RES * const result);

#define mysqlnd_field_seek(result, ofs)			(result)->m.seek_field((result), (ofs))
#define mysqlnd_field_tell(result)				(result)->m.field_tell((result))
#define mysqlnd_fetch_field(result)				(result)->m.fetch_field((result))
#define mysqlnd_fetch_field_direct(result,fnr)	(result)->m.fetch_field_direct((result), (fnr))

/* mysqlnd metadata */
PHPAPI const char *	mysqlnd_get_client_info();
PHPAPI unsigned int	mysqlnd_get_client_version();

/* PS */
#define mysqlnd_stmt_insert_id(stmt)		(stmt)->m->get_last_insert_id((stmt))
#define mysqlnd_stmt_affected_rows(stmt)	(stmt)->m->get_affected_rows((stmt))
#define mysqlnd_stmt_num_rows(stmt)			(stmt)->m->get_num_rows((stmt))
#define mysqlnd_stmt_param_count(stmt)		(stmt)->m->get_param_count((stmt))
#define mysqlnd_stmt_field_count(stmt)		(stmt)->m->get_field_count((stmt))
#define mysqlnd_stmt_warning_count(stmt)	(stmt)->m->get_warning_count((stmt))
#define mysqlnd_stmt_errno(stmt)			(stmt)->m->get_error_no((stmt))
#define mysqlnd_stmt_error(stmt)			(stmt)->m->get_error_str((stmt))
#define mysqlnd_stmt_sqlstate(stmt)			(stmt)->m->get_sqlstate((stmt))
#endif /* MYSQLND_USE_OPTIMISATIONS */
/*****************************************************************************************************/



PHPAPI const char * mysqlnd_field_type_name(enum mysqlnd_field_types field_type);

/* LOAD DATA LOCAL */
PHPAPI void mysqlnd_local_infile_default(MYSQLND *conn, zend_bool free_callback);
PHPAPI void mysqlnd_set_local_infile_handler(MYSQLND * const conn, const char * const funcname);

/* Simple commands */
#define mysqlnd_autocommit(conn, mode)		(conn)->m->query((conn),(mode) ? "SET AUTOCOMMIT=1":"SET AUTOCOMMIT=0", 16 TSRMLS_CC)
#define mysqlnd_commit(conn)				(conn)->m->query((conn), "COMMIT", sizeof("COMMIT")-1 TSRMLS_CC)
#define mysqlnd_rollback(conn)				(conn)->m->query((conn), "ROLLBACK", sizeof("ROLLBACK")-1 TSRMLS_CC)
#define mysqlnd_list_dbs(conn, wild)		(conn)->m->list_method((conn), wild? "SHOW DATABASES LIKE %s":"SHOW DATABASES", (wild), NULL TSRMLS_CC)
#define mysqlnd_list_fields(conn, tab,wild)	(conn)->m->list_method((conn), wild? "SHOW FIELDS FROM %s LIKE %s":"SHOW FIELDS FROM %s", wild, tab TSRMLS_CC)
#define mysqlnd_list_processes(conn)		(conn)->m->list_method((conn), "SHOW PROCESSLIST", NULL, NULL TSRMLS_CC)
#define mysqlnd_list_tables(conn, wild)		(conn)->m->list_method((conn), wild? "SHOW TABLES LIKE %s":"SHOW TABLES", (wild), NULL TSRMLS_CC)
#define mysqlnd_dump_debug_info(conn)		(conn)->m->server_dump_debug_information((conn) TSRMLS_CC)
#define mysqlnd_select_db(conn, db, db_len)	(conn)->m->select_db((conn), (db), (db_len) TSRMLS_CC)
#define mysqlnd_ping(conn)					(conn)->m->ping((conn) TSRMLS_CC)
#define mysqlnd_kill(conn, pid)				(conn)->m->kill_connection((conn), (pid) TSRMLS_CC)
#define mysqlnd_refresh(conn, options)		(conn)->m->refresh_server((conn), (options) TSRMLS_CC) 
#define mysqlnd_shutdown(conn, level)		(conn)->m->shutdown_server((conn), (level) TSRMLS_CC)
#define mysqlnd_get_server_version(conn)	(conn)->m->get_server_version((conn))
#define mysqlnd_set_character_set(conn, cs)	(conn)->m->set_charset((conn), (cs) TSRMLS_CC)
#define mysqlnd_stat(conn, msg, msg_len)	(conn)->m->get_server_statistics((conn), (msg), (msg_len) TSRMLS_CC)
#define mysqlnd_options(conn, opt, value)	(conn)->m->set_client_option((conn), (opt), (value))
#define mysqlnd_set_server_option(conn, op)	(conn)->m->set_server_option((conn), (op) TSRMLS_CC)

/* Escaping */
#define mysqlnd_real_escape_string(conn, newstr, escapestr, escapestr_len) \
		(conn)->m->escape_string((conn), (newstr), (escapestr), (escapestr_len))
#define mysqlnd_escape_string(newstr, escapestr, escapestr_len) \
		mysqlnd_old_escape_string((newstr), (escapestr), (escapestr_len))

PHPAPI ulong mysqlnd_old_escape_string(char *newstr, const char *escapestr, int escapestr_len);


/* PS */
#define mysqlnd_stmt_init(conn)				(conn)->m->stmt_init((conn))
#define mysqlnd_stmt_store_result(stmt)		(!mysqlnd_stmt_field_count((stmt)) ? PASS:((stmt)->m->store_result((stmt) TSRMLS_CC)? PASS:FAIL))
#define mysqlnd_stmt_get_result(stmt)		(stmt)->m->get_result((stmt) TSRMLS_CC)
#define mysqlnd_stmt_data_seek(stmt, row)	(stmt)->m->seek_data((stmt), (row))
#define mysqlnd_stmt_prepare(stmt, q, qlen)	(stmt)->m->prepare((stmt), (q), (qlen) TSRMLS_CC)
#define mysqlnd_stmt_execute(stmt) 			(stmt)->m->execute((stmt) TSRMLS_CC)
#define mysqlnd_stmt_send_long_data(s,p,d,l) (s)->m->send_long_data((s), (p), (d), (l) TSRMLS_CC)
#define mysqlnd_stmt_bind_param(stmt,bind)	(stmt)->m->bind_param((stmt), (bind))
#define mysqlnd_stmt_bind_result(stmt,bind)	(stmt)->m->bind_result((stmt), (bind))
#define mysqlnd_stmt_param_metadata(stmt)	(stmt)->m->get_parameter_metadata((stmt))
#define mysqlnd_stmt_result_metadata(stmt)	(stmt)->m->get_result_metadata((stmt))

#define	mysqlnd_stmt_free_result(stmt)		(stmt)->m->free_result((stmt) TSRMLS_CC)
#define	mysqlnd_stmt_close(stmt, implicit)	(stmt)->m->dtor((stmt), (implicit) TSRMLS_CC)
#define	mysqlnd_stmt_reset(stmt)			(stmt)->m->reset((stmt) TSRMLS_CC)


#define mysqlnd_stmt_attr_get(stmt, attr, value)	(stmt)->m->get_attribute((stmt), (attr), (value))
#define mysqlnd_stmt_attr_set(stmt, attr, value)	(stmt)->m->set_attribute((stmt), (attr), (value))

#define mysqlnd_stmt_fetch(stmt, fetched)	(stmt)->m->fetch((stmt), (fetched) TSRMLS_CC)


/* Performance statistics */
PHPAPI void			_mysqlnd_get_client_stats(zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC);

/* Persistent caching zval allocator */
PHPAPI MYSQLND_ZVAL_PCACHE* mysqlnd_palloc_init_cache(unsigned int cache_size);
PHPAPI void 		mysqlnd_palloc_free_cache(MYSQLND_ZVAL_PCACHE *cache);
PHPAPI void			mysqlnd_palloc_stats(const MYSQLND_ZVAL_PCACHE * const cache, zval *return_value);

PHPAPI MYSQLND_THD_ZVAL_PCACHE * mysqlnd_palloc_rinit(MYSQLND_ZVAL_PCACHE * cache);
PHPAPI void						 mysqlnd_palloc_rshutdown(MYSQLND_THD_ZVAL_PCACHE * cache);


PHPAPI MYSQLND_THD_ZVAL_PCACHE*	mysqlnd_palloc_init_thd_cache(MYSQLND_ZVAL_PCACHE * const cache);
MYSQLND_THD_ZVAL_PCACHE*	mysqlnd_palloc_get_thd_cache_reference(MYSQLND_THD_ZVAL_PCACHE * const cache);
PHPAPI void					mysqlnd_palloc_free_thd_cache_reference(MYSQLND_THD_ZVAL_PCACHE **cache);


/* There two should not be used from outside */
void *	mysqlnd_palloc_get_zval(MYSQLND_THD_ZVAL_PCACHE * const cache, zend_bool *allocated);
void	mysqlnd_palloc_zval_ptr_dtor(zval **zv, MYSQLND_THD_ZVAL_PCACHE * const cache,
									 enum_mysqlnd_res_type type, zend_bool *copy_ctor_called TSRMLS_DC);



/* ---------------------- QUERY CACHE ---------------*/
struct st_mysqlnd_qcache {
	HashTable		*ht;
	unsigned int	references;
#ifdef ZTS
	MUTEX_T		LOCK_access;
#endif
};


typedef struct st_mysqlnd_qcache_element {
	MYSQLND_RES_BUFFERED *data;
	MYSQLND_RES_METADATA *meta;
	const char * query;
	size_t	query_len;
} MYSQLND_QCACHE_ELEMENT;


PHPAPI MYSQLND_QCACHE *	mysqlnd_qcache_init_cache();
PHPAPI MYSQLND_QCACHE *	mysqlnd_qcache_get_cache_reference(MYSQLND_QCACHE * const cache);
PHPAPI void				mysqlnd_qcache_free_cache_reference(MYSQLND_QCACHE **cache);
PHPAPI void				mysqlnd_qcache_stats(const MYSQLND_QCACHE * const cache, zval *return_value);
MYSQLND_RES * 		mysqlnd_qcache_get(MYSQLND_QCACHE * const cache, const char * query,
									   size_t query_len);
void				mysqlnd_qcache_put(MYSQLND_QCACHE * const cache, char * query, size_t query_len,
									   MYSQLND_RES_BUFFERED * const result, MYSQLND_RES_METADATA * const meta);

#endif	/* MYSQLND_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
