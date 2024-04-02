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
   | Authors: Zeev Suraski <zeev@php.net>                                 |
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_PGSQL_H
#define PHP_PGSQL_H

#ifdef HAVE_PGSQL

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

#ifdef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
const char * pg_encoding_to_char(int encoding);
#endif

PHP_MINIT_FUNCTION(pgsql);
PHP_MSHUTDOWN_FUNCTION(pgsql);
PHP_RINIT_FUNCTION(pgsql);
PHP_RSHUTDOWN_FUNCTION(pgsql);
PHP_MINFO_FUNCTION(pgsql);

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
PHP_PGSQL_API zend_result php_pgsql_meta_data(PGconn *pg_link, const zend_string *table_name, zval *meta, bool extended);
PHP_PGSQL_API zend_result php_pgsql_convert(PGconn *pg_link, const zend_string *table_name, const zval *values, zval *result, zend_ulong opt);
PHP_PGSQL_API zend_result php_pgsql_insert(PGconn *pg_link, const zend_string *table, zval *values, zend_ulong opt, zend_string **sql);
PHP_PGSQL_API zend_result php_pgsql_update(PGconn *pg_link, const zend_string *table, zval *values, zval *ids, zend_ulong opt , zend_string **sql);
PHP_PGSQL_API zend_result php_pgsql_delete(PGconn *pg_link, const zend_string *table, zval *ids, zend_ulong opt, zend_string **sql);
PHP_PGSQL_API zend_result php_pgsql_select(PGconn *pg_link, const zend_string *table, zval *ids, zval *ret_array, zend_ulong opt, long fetch_option, zend_string **sql );
PHP_PGSQL_API void php_pgsql_result2array(PGresult *pg_result, zval *ret_array, long fetch_option);

/* internal functions */
static void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
static void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type, bool nullable_row);
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

typedef struct pgsql_link_handle {
	PGconn *conn;
	zend_string *hash;
	HashTable *notices;
	bool persistent;
	zend_object std;
} pgsql_link_handle;

typedef struct pgLofp {
	PGconn *conn;
	int lofd;
	zend_object std;
} pgLofp;

typedef struct _php_pgsql_result_handle {
	PGconn *conn;
	PGresult *result;
	int row;
	zend_object std;
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
	bool allow_persistent;
    int ignore_notices;
	zend_long auto_reset_persistent;
	int log_notices;
	zend_object *default_link; /* default link when connection is omitted */
	HashTable field_oids;
	HashTable table_oids;
	HashTable connections;
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
