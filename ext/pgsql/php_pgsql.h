/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#ifndef PHP_PGSQL_H
#define PHP_PGSQL_H

#if HAVE_PGSQL

extern zend_module_entry pgsql_module_entry;
#define pgsql_module_ptr &pgsql_module_entry

#ifdef PHP_PGSQL_PRIVATE
#undef SOCKET_SIZE_TYPE
#include <libpq-fe.h>

#ifdef PHP_WIN32
#define INV_WRITE            0x00020000
#define INV_READ             0x00040000
#elif defined NETWARE
#include <libpq-fs.h>
#else
#include <libpq/libpq-fs.h>
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
PHP_FUNCTION(pg_close);
PHP_FUNCTION(pg_connection_reset);
PHP_FUNCTION(pg_connection_status);
PHP_FUNCTION(pg_connection_busy);
PHP_FUNCTION(pg_host);
PHP_FUNCTION(pg_dbname);
PHP_FUNCTION(pg_port);
PHP_FUNCTION(pg_tty);
PHP_FUNCTION(pg_options);
/* query functions */
PHP_FUNCTION(pg_query);
PHP_FUNCTION(pg_send_query);
PHP_FUNCTION(pg_cancel_query);
/* result functions */
PHP_FUNCTION(pg_fetch_array);
PHP_FUNCTION(pg_fetch_object);
PHP_FUNCTION(pg_fetch_result);
PHP_FUNCTION(pg_fetch_row);
PHP_FUNCTION(pg_affected_rows);
PHP_FUNCTION(pg_get_result);
PHP_FUNCTION(pg_result_status);
PHP_FUNCTION(pg_free_result);
PHP_FUNCTION(pg_last_oid);
PHP_FUNCTION(pg_num_rows);
PHP_FUNCTION(pg_num_fields);
PHP_FUNCTION(pg_field_name);
PHP_FUNCTION(pg_field_num);
PHP_FUNCTION(pg_field_size);
PHP_FUNCTION(pg_field_type);
PHP_FUNCTION(pg_field_prtlen);
PHP_FUNCTION(pg_field_is_null);
/* error message functions */
PHP_FUNCTION(pg_result_error);
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

/* debugging functions */
PHP_FUNCTION(pg_trace);
PHP_FUNCTION(pg_untrace);

/* utility functions */
#if HAVE_PQCLIENTENCODING
PHP_FUNCTION(pg_client_encoding);
PHP_FUNCTION(pg_set_client_encoding);
#endif
#if HAVE_PQESCAPE
PHP_FUNCTION(pg_escape_string);
PHP_FUNCTION(pg_escape_bytea);
#endif

static void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
/* static int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS); */
static void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list TSRMLS_DC);
static void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void php_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS,int entry_type);

typedef struct pgLofp {
        PGconn *conn;
	int lofd;
} pgLofp;

typedef struct _php_pgsql_result_handle {
	PGconn *conn;
	PGresult *result;
	int row;
} pgsql_result_handle;

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	long auto_reset_persistent;
	int le_lofp,le_string;
	int ignore_notices;
	char *last_notice;
	uint last_notice_len;
} php_pgsql_globals;


#ifdef ZTS
# define PGG(v) TSRMG(pgsql_globals_id, php_pgsql_globals *, v)
extern int pgsql_globals_id;
#else
# define PGG(v) (pgsql_globals.v)
extern php_pgsql_globals pgsql_globals;
#endif

#endif

#else

#define pgsql_module_ptr NULL

#endif

#define phpext_pgsql_ptr pgsql_module_ptr

#endif /* PHP_PGSQL_H */
