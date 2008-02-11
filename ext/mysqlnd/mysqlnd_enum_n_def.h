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
#ifndef MYSQLND_ENUM_N_DEF_H
#define MYSQLND_ENUM_N_DEF_H

#define MYSQLND_MAX_PACKET_SIZE (256L*256L*256L-1)

#define MYSQLND_ERRMSG_SIZE			512
#define MYSQLND_SQLSTATE_LENGTH		5
#define MYSQLND_SQLSTATE_NULL		"00000"

#define MYSQLND_SERVER_QUERY_NO_GOOD_INDEX_USED	16
#define MYSQLND_SERVER_QUERY_NO_INDEX_USED		32

#define MYSQLND_NO_DATA			100
#define MYSQLND_DATA_TRUNCATED	101

#define SHA1_MAX_LENGTH 20
#define SCRAMBLE_LENGTH 20
#define SCRAMBLE_LENGTH_323 8

#define CLIENT_LONG_PASSWORD		1		/* new more secure passwords */
#define CLIENT_FOUND_ROWS			2		/* Found instead of affected rows */
#define CLIENT_LONG_FLAG			4		/* Get all column flags */
#define CLIENT_CONNECT_WITH_DB		8		/* One can specify db on connect */
#define CLIENT_NO_SCHEMA			16		/* Don't allow database.table.column */
#define CLIENT_COMPRESS				32		/* Can use compression protocol */
#define CLIENT_ODBC					64		/* Odbc client */
#define CLIENT_LOCAL_FILES			128		/* Can use LOAD DATA LOCAL */
#define CLIENT_IGNORE_SPACE			256		/* Ignore spaces before '(' */
#define CLIENT_PROTOCOL_41			512		/* New 4.1 protocol */
#define CLIENT_INTERACTIVE			1024	/* This is an interactive client */
#define CLIENT_SSL					2048	/* Switch to SSL after handshake */
#define CLIENT_IGNORE_SIGPIPE		4096	/* IGNORE sigpipes */
#define CLIENT_TRANSACTIONS			8192	/* Client knows about transactions */
#define CLIENT_RESERVED				16384	/* Old flag for 4.1 protocol */
#define CLIENT_SECURE_CONNECTION	32768	/* New 4.1 authentication */
#define CLIENT_MULTI_STATEMENTS		(1UL << 16) /* Enable/disable multi-stmt support */
#define CLIENT_MULTI_RESULTS		(1UL << 17) /* Enable/disable multi-results */

typedef enum mysqlnd_extension
{
	MYSQLND_MYSQL = 0,
	MYSQLND_MYSQLI,
} enum_mysqlnd_extension;

enum
{
	MYSQLND_FETCH_ASSOC = 1,
	MYSQLND_FETCH_NUM = 2,
	MYSQLND_FETCH_BOTH = 1|2,
};

/* Follow libmysql convention */
typedef enum func_status
{
	PASS = 0,
	FAIL = 1,
} enum_func_status;

typedef enum mysqlnd_query_type
{
	QUERY_UPSERT,
	QUERY_SELECT,
	QUERY_LOAD_LOCAL
} enum_mysqlnd_query_type;

typedef enum mysqlnd_res_type
{
	MYSQLND_RES_NORMAL = 1,
	MYSQLND_RES_PS_BUF,
	MYSQLND_RES_PS_UNBUF
} enum_mysqlnd_res_type;

typedef enum mysqlnd_option
{
	MYSQL_OPT_CONNECT_TIMEOUT,
	MYSQL_OPT_COMPRESS,
	MYSQL_OPT_NAMED_PIPE,
	MYSQL_INIT_COMMAND,
	MYSQL_READ_DEFAULT_FILE,
	MYSQL_READ_DEFAULT_GROUP,
	MYSQL_SET_CHARSET_DIR,
	MYSQL_SET_CHARSET_NAME,
	MYSQL_OPT_LOCAL_INFILE,
	MYSQL_OPT_PROTOCOL,
	MYSQL_SHARED_MEMORY_BASE_NAME,
	MYSQL_OPT_READ_TIMEOUT,
	MYSQL_OPT_WRITE_TIMEOUT,
	MYSQL_OPT_USE_RESULT,
	MYSQL_OPT_USE_REMOTE_CONNECTION,
	MYSQL_OPT_USE_EMBEDDED_CONNECTION,
	MYSQL_OPT_GUESS_CONNECTION,
	MYSQL_SET_CLIENT_IP,
	MYSQL_SECURE_AUTH,
	MYSQL_REPORT_DATA_TRUNCATION,
	MYSQL_OPT_RECONNECT,
	MYSQL_OPT_SSL_VERIFY_SERVER_CERT,
#if PHP_MAJOR_VERSION >= 6
	MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE = 200,
#endif
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
	MYSQLND_OPT_INT_AND_YEAR_AS_INT = 201,
#endif
	MYSQLND_OPT_NET_CMD_BUFFER_SIZE = 202,
	MYSQLND_OPT_NET_READ_BUFFER_SIZE = 203,
} enum_mysqlnd_option;


typedef enum mysqlnd_field_types
{
	MYSQL_TYPE_DECIMAL,
	MYSQL_TYPE_TINY,
	MYSQL_TYPE_SHORT,
	MYSQL_TYPE_LONG,
	MYSQL_TYPE_FLOAT,
	MYSQL_TYPE_DOUBLE,
	MYSQL_TYPE_NULL,
	MYSQL_TYPE_TIMESTAMP,
	MYSQL_TYPE_LONGLONG,
	MYSQL_TYPE_INT24,
	MYSQL_TYPE_DATE,
	MYSQL_TYPE_TIME,
	MYSQL_TYPE_DATETIME,
	MYSQL_TYPE_YEAR,
	MYSQL_TYPE_NEWDATE,
	MYSQL_TYPE_VARCHAR,
	MYSQL_TYPE_BIT,
	MYSQL_TYPE_NEWDECIMAL=246,
	MYSQL_TYPE_ENUM=247,
	MYSQL_TYPE_SET=248,
	MYSQL_TYPE_TINY_BLOB=249,
	MYSQL_TYPE_MEDIUM_BLOB=250,
	MYSQL_TYPE_LONG_BLOB=251,
	MYSQL_TYPE_BLOB=252,
	MYSQL_TYPE_VAR_STRING=253,
	MYSQL_TYPE_STRING=254,
	MYSQL_TYPE_GEOMETRY=255
} enum_mysqlnd_field_types;

/* Please update this if there is a new type after MYSQL_TYPE_GEOMETRY */
#define MYSQL_TYPE_LAST		MYSQL_TYPE_GEOMETRY


typedef enum mysqlnd_server_option
{
	MYSQL_OPTION_MULTI_STATEMENTS_ON,
	MYSQL_OPTION_MULTI_STATEMENTS_OFF
} enum_mysqlnd_server_option;


#define FIELD_TYPE_DECIMAL		MYSQL_TYPE_DECIMAL
#define FIELD_TYPE_NEWDECIMAL	MYSQL_TYPE_NEWDECIMAL
#define FIELD_TYPE_TINY			MYSQL_TYPE_TINY
#define FIELD_TYPE_SHORT		MYSQL_TYPE_SHORT
#define FIELD_TYPE_LONG			MYSQL_TYPE_LONG
#define FIELD_TYPE_FLOAT		MYSQL_TYPE_FLOAT
#define FIELD_TYPE_DOUBLE		MYSQL_TYPE_DOUBLE
#define FIELD_TYPE_NULL			MYSQL_TYPE_NULL
#define FIELD_TYPE_TIMESTAMP	MYSQL_TYPE_TIMESTAMP
#define FIELD_TYPE_LONGLONG		MYSQL_TYPE_LONGLONG
#define FIELD_TYPE_INT24		MYSQL_TYPE_INT24
#define FIELD_TYPE_DATE			MYSQL_TYPE_DATE
#define FIELD_TYPE_TIME			MYSQL_TYPE_TIME
#define FIELD_TYPE_DATETIME		MYSQL_TYPE_DATETIME
#define FIELD_TYPE_YEAR			MYSQL_TYPE_YEAR
#define FIELD_TYPE_NEWDATE		MYSQL_TYPE_NEWDATE
#define FIELD_TYPE_ENUM			MYSQL_TYPE_ENUM
#define FIELD_TYPE_SET			MYSQL_TYPE_SET
#define FIELD_TYPE_TINY_BLOB	MYSQL_TYPE_TINY_BLOB
#define FIELD_TYPE_MEDIUM_BLOB	MYSQL_TYPE_MEDIUM_BLOB
#define FIELD_TYPE_LONG_BLOB	MYSQL_TYPE_LONG_BLOB
#define FIELD_TYPE_BLOB			MYSQL_TYPE_BLOB
#define FIELD_TYPE_VAR_STRING	MYSQL_TYPE_VAR_STRING
#define FIELD_TYPE_STRING		MYSQL_TYPE_STRING
#define FIELD_TYPE_CHAR			MYSQL_TYPE_TINY
#define FIELD_TYPE_INTERVAL		MYSQL_TYPE_ENUM
#define FIELD_TYPE_GEOMETRY		MYSQL_TYPE_GEOMETRY
#define FIELD_TYPE_BIT			MYSQL_TYPE_BIT

#define NOT_NULL_FLAG			    1
#define PRI_KEY_FLAG			    2
#define UNIQUE_KEY_FLAG			    4
#define MULTIPLE_KEY_FLAG		    8
#define BLOB_FLAG				   16
#define UNSIGNED_FLAG			   32
#define ZEROFILL_FLAG			   64
#define BINARY_FLAG				  128
#define ENUM_FLAG				  256
#define AUTO_INCREMENT_FLAG		  512
#define TIMESTAMP_FLAG			 1024
#define SET_FLAG				 2048
#define NO_DEFAULT_VALUE_FLAG	 4096
#define ON_UPDATE_NOW_FLAG		 8192
#define PART_KEY_FLAG			16384
#define GROUP_FLAG				32768
#define NUM_FLAG				32768

#define IS_PRI_KEY(n)	((n) & PRI_KEY_FLAG)
#define IS_NOT_NULL(n)	((n) & NOT_NULL_FLAG)
#define IS_BLOB(n)		((n) & BLOB_FLAG)
#define IS_NUM(t)		((t) <= FIELD_TYPE_INT24 || (t) == FIELD_TYPE_YEAR || (t) == FIELD_TYPE_NEWDECIMAL)


/* see mysqlnd_charset.c for more information */
#define MYSQLND_BINARY_CHARSET_NR	63


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


typedef enum mysqlnd_stmt_state
{
	MYSQLND_STMT_INITTED = 0,
	MYSQLND_STMT_PREPARED,
	MYSQLND_STMT_EXECUTED,
	MYSQLND_STMT_WAITING_USE_OR_STORE,
	MYSQLND_STMT_USE_OR_STORE_CALLED,
	MYSQLND_STMT_USER_FETCHING, /* fetch_row_buff or fetch_row_unbuf */
} enum_mysqlnd_stmt_state;


typedef enum param_bind_flags
{
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

typedef enum mysqlnd_collected_stats
{
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
	STAT_PS_PREPARED_NEVER_EXECUTED,
	STAT_PS_PREPARED_ONCE_USED,
	STAT_ROWS_FETCHED_FROM_SERVER_NORMAL,
	STAT_ROWS_FETCHED_FROM_SERVER_PS,
	STAT_ROWS_BUFFERED_FROM_CLIENT_NORMAL,
	STAT_ROWS_BUFFERED_FROM_CLIENT_PS,
	STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF,
	STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF,
	STAT_ROWS_FETCHED_FROM_CLIENT_PS_BUF,
	STAT_ROWS_FETCHED_FROM_CLIENT_PS_UNBUF,
	STAT_ROWS_FETCHED_FROM_CLIENT_PS_CURSOR,
	STAT_ROWS_SKIPPED_NORMAL,
	STAT_ROWS_SKIPPED_PS,
	STAT_COPY_ON_WRITE_SAVED,
	STAT_COPY_ON_WRITE_PERFORMED,
	STAT_CMD_BUFFER_TOO_SMALL,
	STAT_CONNECT_SUCCESS,
	STAT_CONNECT_FAILURE,
	STAT_CONNECT_REUSED,
	STAT_RECONNECT,
	STAT_PCONNECT_SUCCESS,
	STAT_OPENED_CONNECTIONS,
	STAT_OPENED_PERSISTENT_CONNECTIONS,
	STAT_CLOSE_EXPLICIT,
	STAT_CLOSE_IMPLICIT,
	STAT_CLOSE_DISCONNECT,
	STAT_CLOSE_IN_MIDDLE,
	STAT_FREE_RESULT_EXPLICIT,
	STAT_FREE_RESULT_IMPLICIT,
	STAT_STMT_CLOSE_EXPLICIT,
	STAT_STMT_CLOSE_IMPLICIT,
	STAT_MEM_EMALLOC_COUNT,
	STAT_MEM_EMALLOC_AMMOUNT,
	STAT_MEM_ECALLOC_COUNT,
	STAT_MEM_ECALLOC_AMMOUNT,
	STAT_MEM_EREALLOC_COUNT,
	STAT_MEM_EREALLOC_AMMOUNT,
	STAT_MEM_EFREE_COUNT,
	STAT_MEM_MALLOC_COUNT,
	STAT_MEM_MALLOC_AMMOUNT,
	STAT_MEM_CALLOC_COUNT,
	STAT_MEM_CALLOC_AMMOUNT,
	STAT_MEM_REALLOC_COUNT,
	STAT_MEM_REALLOC_AMMOUNT,
	STAT_MEM_FREE_COUNT,
	STAT_LAST /* Should be always the last */
} enum_mysqlnd_collected_stats;


#define MYSQLND_DEFAULT_PREFETCH_ROWS (ulong) 1


#endif	/* MYSQLND_ENUM_N_DEF_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
