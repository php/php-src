/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
#else
#include <libpq/libpq-fs.h>
#endif

#ifdef PHP_WIN32
#define PHP_PGSQL_API __declspec(dllexport)
#else
#define PHP_PGSQL_API
#endif

PHP_MINIT_FUNCTION(pgsql);
PHP_MSHUTDOWN_FUNCTION(pgsql);
PHP_RINIT_FUNCTION(pgsql);
PHP_RSHUTDOWN_FUNCTION(pgsql);
PHP_MINFO_FUNCTION(pgsql);
PHP_FUNCTION(pg_connect);
PHP_FUNCTION(pg_pconnect);
PHP_FUNCTION(pg_close);
PHP_FUNCTION(pg_dbname);
PHP_FUNCTION(pg_errormessage);
PHP_FUNCTION(pg_trace);
PHP_FUNCTION(pg_untrace);
PHP_FUNCTION(pg_options);
PHP_FUNCTION(pg_port);
PHP_FUNCTION(pg_tty);
PHP_FUNCTION(pg_host);
PHP_FUNCTION(pg_exec);
PHP_FUNCTION(pg_numrows);
PHP_FUNCTION(pg_numfields);
PHP_FUNCTION(pg_cmdtuples);
PHP_FUNCTION(pg_last_notice);
PHP_FUNCTION(pg_fieldname);
PHP_FUNCTION(pg_fieldsize);
PHP_FUNCTION(pg_fieldtype);
PHP_FUNCTION(pg_fieldnum);
PHP_FUNCTION(pg_result);
PHP_FUNCTION(pg_fetch_row);
PHP_FUNCTION(pg_fetch_array);
PHP_FUNCTION(pg_fetch_object);
PHP_FUNCTION(pg_fieldprtlen);
PHP_FUNCTION(pg_fieldisnull);
PHP_FUNCTION(pg_freeresult);
PHP_FUNCTION(pg_getlastoid);
PHP_FUNCTION(pg_locreate);
PHP_FUNCTION(pg_lounlink);
PHP_FUNCTION(pg_loopen);
PHP_FUNCTION(pg_loclose);
PHP_FUNCTION(pg_loread);
PHP_FUNCTION(pg_lowrite);
PHP_FUNCTION(pg_loreadall);
PHP_FUNCTION(pg_loimport);
PHP_FUNCTION(pg_loexport);
PHP_FUNCTION(pg_lolseek);
PHP_FUNCTION(pg_lotell);
PHP_FUNCTION(pg_put_line);
PHP_FUNCTION(pg_end_copy);
#if HAVE_PQCLIENTENCODING
PHP_FUNCTION(pg_client_encoding);
PHP_FUNCTION(pg_set_client_encoding);
#endif

void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS);
void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list);
void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);


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
	int le_lofp,le_string;
	int ignore_notices;
	char *last_notice;
	uint last_notice_len;
} php_pgsql_globals;


#ifdef ZTS
# define PGG(v) TSRMG(pgsql_globals_id, php_pgsql_globals *, v)
#else
# define PGG(v) (pgsql_globals.v)
extern PHP_PGSQL_API php_pgsql_globals pgsql_globals;
#endif

#endif

#else

#define pgsql_module_ptr NULL

#endif

#define phpext_pgsql_ptr pgsql_module_ptr

#endif /* PHP_PGSQL_H */
