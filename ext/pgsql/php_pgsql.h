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
   | Authors: Zeev Suraski <zeev@php.net>                                 |
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_PGSQL_H
#define PHP_PGSQL_H

#if HAVE_PGSQL

#define PHP_PGSQL_API_VERSION 20140217

extern zend_module_entry pgsql_module_entry;
#define pgsql_module_ptr &pgsql_module_entry

#include "php_version.h"
#define PHP_PGSQL_VERSION PHP_VERSION

#ifdef PHP_PGSQL_PRIVATE
#undef SOCKET_SIZE_TYPE
#include <libpq-fe.h>

#include <libpq/libpq-fs.h>
#ifdef PHP_WIN32
#undef PHP_PGSQL_API
#ifdef PGSQL_EXPORTS
#define PHP_PGSQL_API __declspec(dllexport)
#else
#define PHP_PGSQL_API __declspec(dllimport)
#endif
#else
# if defined(__GNUC__) && __GNUC__ >= 4
#  define PHP_PGSQL_API __attribute__ ((visibility("default")))
# else
#  define PHP_PGSQL_API
# endif
#endif

#ifdef HAVE_PG_CONFIG_H
#undef SIZEOF_OFF_T
#include <pg_config.h>
#endif

#ifdef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
const char * pg_encoding_to_char(int encoding);
#endif

PHP_MINIT_FUNCTION(pgsql);
PHP_MSHUTDOWN_FUNCTION(pgsql);
PHP_RINIT_FUNCTION(pgsql);
PHP_RSHUTDOWN_FUNCTION(pgsql);
PHP_MINFO_FUNCTION(pgsql);
/* connection functions */
PHP_FUNCTION(pg_connect);
PHP_FUNCTION(pg_pconnect);
PHP_FUNCTION(pg_connect_poll);
PHP_FUNCTION(pg_close);
PHP_FUNCTION(pg_connection_reset);
PHP_FUNCTION(pg_connection_status);
PHP_FUNCTION(pg_connection_busy);
PHP_FUNCTION(pg_host);
PHP_FUNCTION(pg_dbname);
PHP_FUNCTION(pg_port);
PHP_FUNCTION(pg_tty);
PHP_FUNCTION(pg_options);
PHP_FUNCTION(pg_version);
PHP_FUNCTION(pg_ping);
#if HAVE_PQPARAMETERSTATUS
PHP_FUNCTION(pg_parameter_status);
#endif
#if HAVE_PGTRANSACTIONSTATUS
PHP_FUNCTION(pg_transaction_status);
#endif
/* query functions */
PHP_FUNCTION(pg_query);
#if HAVE_PQEXECPARAMS
PHP_FUNCTION(pg_query_params);
#endif
#if HAVE_PQPREPARE
PHP_FUNCTION(pg_prepare);
#endif
#if HAVE_PQEXECPREPARED
PHP_FUNCTION(pg_execute);
#endif
PHP_FUNCTION(pg_send_query);
#if HAVE_PQSENDQUERYPARAMS
PHP_FUNCTION(pg_send_query_params);
#endif
#if HAVE_PQSENDPREPARE
PHP_FUNCTION(pg_send_prepare);
#endif
#if HAVE_PQSENDQUERYPREPARED
PHP_FUNCTION(pg_send_execute);
#endif
PHP_FUNCTION(pg_cancel_query);
/* result functions */
PHP_FUNCTION(pg_fetch_assoc);
PHP_FUNCTION(pg_fetch_array);
PHP_FUNCTION(pg_fetch_object);
PHP_FUNCTION(pg_fetch_result);
PHP_FUNCTION(pg_fetch_row);
PHP_FUNCTION(pg_fetch_all);
PHP_FUNCTION(pg_fetch_all_columns);
#if HAVE_PQCMDTUPLES
PHP_FUNCTION(pg_affected_rows);
#endif
PHP_FUNCTION(pg_get_result);
PHP_FUNCTION(pg_result_seek);
PHP_FUNCTION(pg_result_status);
PHP_FUNCTION(pg_free_result);
PHP_FUNCTION(pg_last_oid);
PHP_FUNCTION(pg_num_rows);
PHP_FUNCTION(pg_num_fields);
PHP_FUNCTION(pg_field_name);
PHP_FUNCTION(pg_field_num);
PHP_FUNCTION(pg_field_size);
PHP_FUNCTION(pg_field_type);
PHP_FUNCTION(pg_field_type_oid);
PHP_FUNCTION(pg_field_prtlen);
PHP_FUNCTION(pg_field_is_null);
PHP_FUNCTION(pg_field_table);
/* async message functions */
PHP_FUNCTION(pg_get_notify);
PHP_FUNCTION(pg_socket);
PHP_FUNCTION(pg_consume_input);
PHP_FUNCTION(pg_flush);
PHP_FUNCTION(pg_get_pid);
/* error message functions */
PHP_FUNCTION(pg_result_error);
#if HAVE_PQRESULTERRORFIELD
PHP_FUNCTION(pg_result_error_field);
#endif
PHP_FUNCTION(pg_last_error);
PHP_FUNCTION(pg_last_notice);
/* copy functions */
PHP_FUNCTION(pg_put_line);
PHP_FUNCTION(pg_end_copy);
PHP_FUNCTION(pg_copy_to);
PHP_FUNCTION(pg_copy_from);
/* large object functions */
PHP_FUNCTION(pg_lo_create);
PHP_FUNCTION(pg_lo_unlink);
PHP_FUNCTION(pg_lo_open);
PHP_FUNCTION(pg_lo_close);
PHP_FUNCTION(pg_lo_read);
PHP_FUNCTION(pg_lo_write);
PHP_FUNCTION(pg_lo_read_all);
PHP_FUNCTION(pg_lo_import);
PHP_FUNCTION(pg_lo_export);
PHP_FUNCTION(pg_lo_seek);
PHP_FUNCTION(pg_lo_tell);
#if HAVE_PG_LO_TRUNCATE
PHP_FUNCTION(pg_lo_truncate);
#endif

/* debugging functions */
PHP_FUNCTION(pg_trace);
PHP_FUNCTION(pg_untrace);

/* utility functions */
PHP_FUNCTION(pg_client_encoding);
PHP_FUNCTION(pg_set_client_encoding);
#if HAVE_PQSETERRORVERBOSITY
PHP_FUNCTION(pg_set_error_verbosity);
#endif
#if HAVE_PQESCAPE
PHP_FUNCTION(pg_escape_string);
PHP_FUNCTION(pg_escape_bytea);
PHP_FUNCTION(pg_unescape_bytea);
PHP_FUNCTION(pg_escape_literal);
PHP_FUNCTION(pg_escape_identifier);
#endif

/* misc functions */
PHP_FUNCTION(pg_meta_data);
PHP_FUNCTION(pg_convert);
PHP_FUNCTION(pg_insert);
PHP_FUNCTION(pg_update);
PHP_FUNCTION(pg_delete);
PHP_FUNCTION(pg_select);

/* connection options - ToDo: Add async connection option */
#define PGSQL_CONNECT_FORCE_NEW     (1<<1)
#define PGSQL_CONNECT_ASYNC         (1<<2)
/* php_pgsql_convert options */
#define PGSQL_CONV_IGNORE_DEFAULT   (1<<1)     /* Do not use DEFAULT value by removing field from returned array */
#define PGSQL_CONV_FORCE_NULL       (1<<2)     /* Convert to NULL if string is null string */
#define PGSQL_CONV_IGNORE_NOT_NULL  (1<<3)     /* Ignore NOT NULL constraints */
#define PGSQL_CONV_OPTS             (PGSQL_CONV_IGNORE_DEFAULT|PGSQL_CONV_FORCE_NULL|PGSQL_CONV_IGNORE_NOT_NULL)
/* php_pgsql_insert/update/select/delete options */
#define PGSQL_DML_NO_CONV           (1<<8)     /* Do not call php_pgsql_convert() */
#define PGSQL_DML_EXEC              (1<<9)     /* Execute query */
#define PGSQL_DML_ASYNC             (1<<10)    /* Do async query */
#define PGSQL_DML_STRING            (1<<11)    /* Return query string */
#define PGSQL_DML_ESCAPE            (1<<12)    /* No convert, but escape only */

/* exported functions */
PHP_PGSQL_API int php_pgsql_meta_data(PGconn *pg_link, const char *table_name, zval *meta, zend_bool extended);
PHP_PGSQL_API int php_pgsql_convert(PGconn *pg_link, const char *table_name, const zval *values, zval *result, zend_ulong opt);
PHP_PGSQL_API int php_pgsql_insert(PGconn *pg_link, const char *table, zval *values, zend_ulong opt, zend_string **sql);
PHP_PGSQL_API int php_pgsql_update(PGconn *pg_link, const char *table, zval *values, zval *ids, zend_ulong opt , zend_string **sql);
PHP_PGSQL_API int php_pgsql_delete(PGconn *pg_link, const char *table, zval *ids, zend_ulong opt, zend_string **sql);
PHP_PGSQL_API int php_pgsql_select(PGconn *pg_link, const char *table, zval *ids, zval *ret_array, zend_ulong opt, long fetch_option, zend_string **sql );
PHP_PGSQL_API int php_pgsql_result2array(PGresult *pg_result, zval *ret_array, long fetch_option);

/* internal functions */
static void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
static void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list);
static void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS,int entry_type);

static ssize_t php_pgsql_fd_write(php_stream *stream, const char *buf, size_t count);
static ssize_t php_pgsql_fd_read(php_stream *stream, char *buf, size_t count);
static int php_pgsql_fd_close(php_stream *stream, int close_handle);
static int php_pgsql_fd_flush(php_stream *stream);
static int php_pgsql_fd_set_option(php_stream *stream, int option, int value, void *ptrparam);
static int php_pgsql_fd_cast(php_stream *stream, int cast_as, void **ret);

typedef enum _php_pgsql_data_type {
	/* boolean */
	PG_BOOL,
	/* number */
	PG_OID,
	PG_INT2,
	PG_INT4,
	PG_INT8,
	PG_FLOAT4,
	PG_FLOAT8,
	PG_NUMERIC,
	PG_MONEY,
	/* character */
	PG_TEXT,
	PG_CHAR,
	PG_VARCHAR,
	/* time and interval */
	PG_UNIX_TIME,
	PG_UNIX_TIME_INTERVAL,
	PG_DATE,
	PG_TIME,
	PG_TIME_WITH_TIMEZONE,
	PG_TIMESTAMP,
	PG_TIMESTAMP_WITH_TIMEZONE,
	PG_INTERVAL,
	/* binary */
	PG_BYTEA,
	/* network */
	PG_CIDR,
	PG_INET,
	PG_MACADDR,
	/* bit */
	PG_BIT,
	PG_VARBIT,
	/* geometoric */
	PG_LINE,
	PG_LSEG,
	PG_POINT,
	PG_BOX,
	PG_PATH,
	PG_POLYGON,
	PG_CIRCLE,
	/* unknown and system */
	PG_UNKNOWN
} php_pgsql_data_type;

typedef struct pgLofp {
	PGconn *conn;
	int lofd;
} pgLofp;

typedef struct _php_pgsql_result_handle {
	PGconn *conn;
	PGresult *result;
	int row;
} pgsql_result_handle;

typedef struct _php_pgsql_notice {
	char *message;
	size_t len;
} php_pgsql_notice;

static const php_stream_ops php_stream_pgsql_fd_ops = {
	php_pgsql_fd_write,
	php_pgsql_fd_read,
	php_pgsql_fd_close,
	php_pgsql_fd_flush,
	"PostgreSQL link",
	NULL, /* seek */
	php_pgsql_fd_cast, /* cast */
	NULL, /* stat */
	php_pgsql_fd_set_option
};

ZEND_BEGIN_MODULE_GLOBALS(pgsql)
	zend_long num_links,num_persistent;
	zend_long max_links,max_persistent;
	zend_long allow_persistent;
	zend_long auto_reset_persistent;
	int le_lofp,le_string;
	int ignore_notices,log_notices;
	HashTable notices;  /* notice message for each connection */
	zend_resource *default_link; /* default link when connection is omitted */
	HashTable hashes; /* hashes for each connection */
ZEND_END_MODULE_GLOBALS(pgsql)

ZEND_EXTERN_MODULE_GLOBALS(pgsql)
# define PGG(v) ZEND_MODULE_GLOBALS_ACCESSOR(pgsql, v)

#if defined(ZTS) && defined(COMPILE_DL_PGSQL)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif

#else

#define pgsql_module_ptr NULL

#endif

#define phpext_pgsql_ptr pgsql_module_ptr

#endif /* PHP_PGSQL_H */
