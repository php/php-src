/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Yasuo Ohgaki <yohgaki@php.net>                              |
   |          Youichi Iwakiri <yiwakiri@st.rim.or.jp> (pg_copy_*)         | 
   |          Chris Kings-Lynne <chriskl@php.net> (v3 protocol)           | 
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#include <stdlib.h>

#define PHP_PGSQL_PRIVATE 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define SMART_STR_PREALLOC 512

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/php_smart_str.h"
#include "ext/ereg/php_regex.h"

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "php_pgsql.h"
#include "php_globals.h"
#include "zend_exceptions.h"

#if HAVE_PGSQL

#ifndef InvalidOid
#define InvalidOid ((Oid) 0)
#endif

#define PGSQL_ASSOC		1<<0
#define PGSQL_NUM		1<<1
#define PGSQL_BOTH		(PGSQL_ASSOC|PGSQL_NUM)

#define PGSQL_STATUS_LONG     1
#define PGSQL_STATUS_STRING   2

#define PGSQL_MAX_LENGTH_OF_LONG   30
#define PGSQL_MAX_LENGTH_OF_DOUBLE 60

#define PGSQL_RETURN_OID(oid) do { \
	if (oid > LONG_MAX) { \
		smart_str s = {0}; \
		smart_str_append_unsigned(&s, oid); \
		smart_str_0(&s); \
		RETURN_STRINGL(s.c, s.len, 0); \
	} \
	RETURN_LONG((long)oid); \
} while(0)


#if HAVE_PQSETNONBLOCKING
#define PQ_SETNONBLOCKING(pg_link, flag) PQsetnonblocking(pg_link, flag)
#else
#define PQ_SETNONBLOCKING(pg_link, flag) 0
#endif

#define CHECK_DEFAULT_LINK(x) if ((x) == -1) { php_error_docref(NULL TSRMLS_CC, E_WARNING, "No PostgreSQL link opened yet"); }

#ifndef HAVE_PQFREEMEM
#define PQfreemem free
#endif

ZEND_DECLARE_MODULE_GLOBALS(pgsql)
static PHP_GINIT_FUNCTION(pgsql);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_connect, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_string)
	ZEND_ARG_INFO(0, connect_type)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, tty)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_pconnect, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_string)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, tty)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()

#if HAVE_PQPARAMETERSTATUS
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_parameter_status, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, param_name)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_close, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_dbname, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_last_error, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_options, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_port, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_tty, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_host, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_version, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_ping, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_query, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

#if HAVE_PQEXECPARAMS
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_query_params, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
#endif

#if HAVE_PQPREPARE
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_prepare, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, stmtname)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()
#endif

#if HAVE_PQEXECPREPARED
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_execute, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, stmtname)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_num_rows, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_num_fields, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

#if HAVE_PQCMDTUPLES
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_affected_rows, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_last_notice, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#ifdef HAVE_PQFTABLE
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_table, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_number)
	ZEND_ARG_INFO(0, oid_only)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_name, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_size, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_type, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_type_oid, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_num, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, field_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_result, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row_number)
	ZEND_ARG_INFO(0, field_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_row, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_assoc, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_array, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_object, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, l)
	ZEND_ARG_INFO(0, ctor_params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_all, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_fetch_all_columns, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_result_seek, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_prtlen, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, field_name_or_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_field_is_null, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, field_name_or_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_free_result, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_last_oid, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_trace, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_untrace, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_create, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, large_object_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_unlink, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, large_object_oid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_open, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, large_object_oid)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_close, 0, 0, 1)
	ZEND_ARG_INFO(0, large_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_read, 0, 0, 1)
	ZEND_ARG_INFO(0, large_object)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_write, 0, 0, 2)
	ZEND_ARG_INFO(0, large_object)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_read_all, 0, 0, 1)
	ZEND_ARG_INFO(0, large_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_import, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, large_object_oid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_export, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, objoid)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_seek, 0, 0, 2)
	ZEND_ARG_INFO(0, large_object)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_tell, 0, 0, 1)
	ZEND_ARG_INFO(0, large_object)
ZEND_END_ARG_INFO()

#if HAVE_PQSETERRORVERBOSITY
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_set_error_verbosity, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, verbosity)
ZEND_END_ARG_INFO()
#endif

#if HAVE_PQCLIENTENCODING
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_set_client_encoding, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_client_encoding, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_end_copy, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_put_line, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_copy_to, 0, 0, 2)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, table_name)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, null_as)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_copy_from, 0, 0, 3)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, table_name)
	ZEND_ARG_INFO(0, rows)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, null_as)
ZEND_END_ARG_INFO()

#if HAVE_PQESCAPE
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_escape_string, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_escape_bytea, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_unescape_bytea, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
#endif

#if HAVE_PQESCAPE
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_escape_literal, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_escape_identifier, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_result_error, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

#if HAVE_PQRESULTERRORFIELD
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_result_error_field, 0, 0, 2)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, fieldcode)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_connection_status, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#if HAVE_PGTRANSACTIONSTATUS
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_transaction_status, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_connection_reset, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_cancel_query, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_connection_busy, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_send_query, 0, 0, 2)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

#if HAVE_PQSENDQUERYPARAMS
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_send_query_params, 0, 0, 3)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
#endif

#if HAVE_PQSENDPREPARE
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_send_prepare, 0, 0, 3)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, stmtname)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()
#endif

#if HAVE_PQSENDQUERYPREPARED
ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_send_execute, 0, 0, 3)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, stmtname)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_get_result, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_result_status, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_get_notify, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, e)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_get_pid, 0, 0, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_meta_data, 0, 0, 2)
	ZEND_ARG_INFO(0, db)
	ZEND_ARG_INFO(0, table)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_convert, 0, 0, 3)
	ZEND_ARG_INFO(0, db)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, values)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_insert, 0, 0, 3)
	ZEND_ARG_INFO(0, db)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, values)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_update, 0, 0, 4)
	ZEND_ARG_INFO(0, db)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, fields)
	ZEND_ARG_INFO(0, ids)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_delete, 0, 0, 3)
	ZEND_ARG_INFO(0, db)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, ids)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_select, 0, 0, 3)
	ZEND_ARG_INFO(0, db)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, ids)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ pgsql_functions[]
 */
const zend_function_entry pgsql_functions[] = {
	/* connection functions */
	PHP_FE(pg_connect,		arginfo_pg_connect)
	PHP_FE(pg_pconnect,		arginfo_pg_pconnect)
	PHP_FE(pg_close,		arginfo_pg_close)
	PHP_FE(pg_connection_status,	arginfo_pg_connection_status)
	PHP_FE(pg_connection_busy,		arginfo_pg_connection_busy)
	PHP_FE(pg_connection_reset,		arginfo_pg_connection_reset)
	PHP_FE(pg_host,			arginfo_pg_host)
	PHP_FE(pg_dbname,		arginfo_pg_dbname)
	PHP_FE(pg_port,			arginfo_pg_port)
	PHP_FE(pg_tty,			arginfo_pg_tty)
	PHP_FE(pg_options,		arginfo_pg_options)
	PHP_FE(pg_version,		arginfo_pg_version)
	PHP_FE(pg_ping,			arginfo_pg_ping)
#if HAVE_PQPARAMETERSTATUS
	PHP_FE(pg_parameter_status, arginfo_pg_parameter_status)
#endif
#if HAVE_PGTRANSACTIONSTATUS
	PHP_FE(pg_transaction_status, arginfo_pg_transaction_status)
#endif
	/* query functions */
	PHP_FE(pg_query,		arginfo_pg_query)
#if HAVE_PQEXECPARAMS
	PHP_FE(pg_query_params,		arginfo_pg_query_params)
#endif
#if HAVE_PQPREPARE
	PHP_FE(pg_prepare,		arginfo_pg_prepare)
#endif
#if HAVE_PQEXECPREPARED
	PHP_FE(pg_execute,		arginfo_pg_execute)
#endif
	PHP_FE(pg_send_query,	arginfo_pg_send_query)
#if HAVE_PQSENDQUERYPARAMS
	PHP_FE(pg_send_query_params,	arginfo_pg_send_query_params)
#endif
#if HAVE_PQSENDPREPARE
	PHP_FE(pg_send_prepare,	arginfo_pg_send_prepare)
#endif
#if HAVE_PQSENDQUERYPREPARED
	PHP_FE(pg_send_execute,	arginfo_pg_send_execute)
#endif
	PHP_FE(pg_cancel_query, arginfo_pg_cancel_query)
	/* result functions */
	PHP_FE(pg_fetch_result,	arginfo_pg_fetch_result)
	PHP_FE(pg_fetch_row,	arginfo_pg_fetch_row)
	PHP_FE(pg_fetch_assoc,	arginfo_pg_fetch_assoc)
	PHP_FE(pg_fetch_array,	arginfo_pg_fetch_array)
	PHP_FE(pg_fetch_object,	arginfo_pg_fetch_object)
	PHP_FE(pg_fetch_all,	arginfo_pg_fetch_all)
	PHP_FE(pg_fetch_all_columns,	arginfo_pg_fetch_all_columns)
#if HAVE_PQCMDTUPLES
	PHP_FE(pg_affected_rows,arginfo_pg_affected_rows)
#endif
	PHP_FE(pg_get_result,	arginfo_pg_get_result)
	PHP_FE(pg_result_seek,	arginfo_pg_result_seek)
	PHP_FE(pg_result_status,arginfo_pg_result_status)
	PHP_FE(pg_free_result,	arginfo_pg_free_result)
	PHP_FE(pg_last_oid,	    arginfo_pg_last_oid)
	PHP_FE(pg_num_rows,		arginfo_pg_num_rows)
	PHP_FE(pg_num_fields,	arginfo_pg_num_fields)
	PHP_FE(pg_field_name,	arginfo_pg_field_name)
	PHP_FE(pg_field_num,	arginfo_pg_field_num)
	PHP_FE(pg_field_size,	arginfo_pg_field_size)
	PHP_FE(pg_field_type,	arginfo_pg_field_type)
	PHP_FE(pg_field_type_oid, arginfo_pg_field_type_oid)
	PHP_FE(pg_field_prtlen,	arginfo_pg_field_prtlen)
	PHP_FE(pg_field_is_null,arginfo_pg_field_is_null)
#ifdef HAVE_PQFTABLE
	PHP_FE(pg_field_table,  arginfo_pg_field_table)
#endif
	/* async message function */
	PHP_FE(pg_get_notify,   arginfo_pg_get_notify)
	PHP_FE(pg_get_pid,      arginfo_pg_get_pid)
	/* error message functions */
	PHP_FE(pg_result_error, arginfo_pg_result_error)
#if HAVE_PQRESULTERRORFIELD
	PHP_FE(pg_result_error_field, arginfo_pg_result_error_field)
#endif
	PHP_FE(pg_last_error,   arginfo_pg_last_error)
	PHP_FE(pg_last_notice,  arginfo_pg_last_notice)
	/* copy functions */
	PHP_FE(pg_put_line,		arginfo_pg_put_line)
	PHP_FE(pg_end_copy,		arginfo_pg_end_copy)
	PHP_FE(pg_copy_to,      arginfo_pg_copy_to)
	PHP_FE(pg_copy_from,    arginfo_pg_copy_from)
	/* debug functions */
	PHP_FE(pg_trace,		arginfo_pg_trace)
	PHP_FE(pg_untrace,		arginfo_pg_untrace)
	/* large object functions */
	PHP_FE(pg_lo_create,	arginfo_pg_lo_create)
	PHP_FE(pg_lo_unlink,	arginfo_pg_lo_unlink)
	PHP_FE(pg_lo_open,		arginfo_pg_lo_open)
	PHP_FE(pg_lo_close,		arginfo_pg_lo_close)
	PHP_FE(pg_lo_read,		arginfo_pg_lo_read)
	PHP_FE(pg_lo_write,		arginfo_pg_lo_write)
	PHP_FE(pg_lo_read_all,	arginfo_pg_lo_read_all)
	PHP_FE(pg_lo_import,	arginfo_pg_lo_import)
	PHP_FE(pg_lo_export,	arginfo_pg_lo_export)
	PHP_FE(pg_lo_seek,		arginfo_pg_lo_seek)
	PHP_FE(pg_lo_tell,		arginfo_pg_lo_tell)
	/* utility functions */
#if HAVE_PQESCAPE
	PHP_FE(pg_escape_string,	arginfo_pg_escape_string)
	PHP_FE(pg_escape_bytea, 	arginfo_pg_escape_bytea)
	PHP_FE(pg_unescape_bytea, 	arginfo_pg_unescape_bytea)
	PHP_FE(pg_escape_literal,	arginfo_pg_escape_literal)
	PHP_FE(pg_escape_identifier,	arginfo_pg_escape_identifier)
#endif
#if HAVE_PQSETERRORVERBOSITY
	PHP_FE(pg_set_error_verbosity,	arginfo_pg_set_error_verbosity)
#endif
#if HAVE_PQCLIENTENCODING
	PHP_FE(pg_client_encoding,		arginfo_pg_client_encoding)
	PHP_FE(pg_set_client_encoding,	arginfo_pg_set_client_encoding)
#endif
	/* misc function */
	PHP_FE(pg_meta_data,	arginfo_pg_meta_data)
	PHP_FE(pg_convert,      arginfo_pg_convert)
	PHP_FE(pg_insert,       arginfo_pg_insert)
	PHP_FE(pg_update,       arginfo_pg_update)
	PHP_FE(pg_delete,       arginfo_pg_delete)
	PHP_FE(pg_select,       arginfo_pg_select)
	/* aliases for downwards compatibility */
	PHP_FALIAS(pg_exec,          pg_query,          arginfo_pg_query)
	PHP_FALIAS(pg_getlastoid,    pg_last_oid,       arginfo_pg_last_oid)
#if HAVE_PQCMDTUPLES
	PHP_FALIAS(pg_cmdtuples,	 pg_affected_rows,  arginfo_pg_affected_rows)
#endif
	PHP_FALIAS(pg_errormessage,	 pg_last_error,     arginfo_pg_last_error)
	PHP_FALIAS(pg_numrows,		 pg_num_rows,       arginfo_pg_num_rows)
	PHP_FALIAS(pg_numfields,	 pg_num_fields,     arginfo_pg_num_fields)
	PHP_FALIAS(pg_fieldname,	 pg_field_name,     arginfo_pg_field_name)
	PHP_FALIAS(pg_fieldsize,     pg_field_size,     arginfo_pg_field_size)
	PHP_FALIAS(pg_fieldtype,	 pg_field_type,     arginfo_pg_field_type)
	PHP_FALIAS(pg_fieldnum,	     pg_field_num,      arginfo_pg_field_num)
	PHP_FALIAS(pg_fieldprtlen,	 pg_field_prtlen,   arginfo_pg_field_prtlen)
	PHP_FALIAS(pg_fieldisnull,	 pg_field_is_null,  arginfo_pg_field_is_null)
	PHP_FALIAS(pg_freeresult,    pg_free_result,    arginfo_pg_free_result)
	PHP_FALIAS(pg_result,	     pg_fetch_result,   arginfo_pg_get_result)
	PHP_FALIAS(pg_loreadall,	 pg_lo_read_all,    arginfo_pg_lo_read_all)
	PHP_FALIAS(pg_locreate,	     pg_lo_create,      arginfo_pg_lo_create)
	PHP_FALIAS(pg_lounlink,	     pg_lo_unlink,      arginfo_pg_lo_unlink)
	PHP_FALIAS(pg_loopen,	     pg_lo_open,        arginfo_pg_lo_open)
	PHP_FALIAS(pg_loclose,	     pg_lo_close,       arginfo_pg_lo_close)
	PHP_FALIAS(pg_loread,	     pg_lo_read,        arginfo_pg_lo_read)
	PHP_FALIAS(pg_lowrite,	     pg_lo_write,       arginfo_pg_lo_write)
	PHP_FALIAS(pg_loimport,	     pg_lo_import,      arginfo_pg_lo_import)
	PHP_FALIAS(pg_loexport,	     pg_lo_export,      arginfo_pg_lo_export)
#if HAVE_PQCLIENTENCODING
	PHP_FALIAS(pg_clientencoding,		pg_client_encoding,		arginfo_pg_client_encoding)
	PHP_FALIAS(pg_setclientencoding,	pg_set_client_encoding,	arginfo_pg_set_client_encoding)
#endif
	PHP_FE_END
};
/* }}} */

/* {{{ pgsql_module_entry
 */
zend_module_entry pgsql_module_entry = {
	STANDARD_MODULE_HEADER,
	"pgsql",
	pgsql_functions,
	PHP_MINIT(pgsql),
	PHP_MSHUTDOWN(pgsql),
	PHP_RINIT(pgsql),
	PHP_RSHUTDOWN(pgsql),
	PHP_MINFO(pgsql),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(pgsql),
	PHP_GINIT(pgsql),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_PGSQL
ZEND_GET_MODULE(pgsql)
#endif

static int le_link, le_plink, le_result, le_lofp, le_string;

/* {{{ _php_pgsql_trim_message */
static char * _php_pgsql_trim_message(const char *message, int *len)
{
	register int i = strlen(message)-1;

	if (i>1 && (message[i-1] == '\r' || message[i-1] == '\n') && message[i] == '.') {
		--i;
	}
	while (i>0 && (message[i] == '\r' || message[i] == '\n')) {
		--i;
	}
	++i;
	if (len) {
		*len = i;
	}
	return estrndup(message, i);
}
/* }}} */

/* {{{ _php_pgsql_trim_result */
static inline char * _php_pgsql_trim_result(PGconn * pgsql, char **buf)
{
	return *buf = _php_pgsql_trim_message(PQerrorMessage(pgsql), NULL);
}
/* }}} */

#define PQErrorMessageTrim(pgsql, buf) _php_pgsql_trim_result(pgsql, buf)

#define PHP_PQ_ERROR(text, pgsql) { \
	char *msgbuf = _php_pgsql_trim_message(PQerrorMessage(pgsql), NULL);	\
	php_error_docref(NULL TSRMLS_CC, E_WARNING, text, msgbuf);	\
	efree(msgbuf);	\
} \

/* {{{ php_pgsql_set_default_link
 */
static void php_pgsql_set_default_link(int id TSRMLS_DC)
{   
	zend_list_addref(id);

	if (PGG(default_link) != -1) {
		zend_list_delete(PGG(default_link));
	}

	PGG(default_link) = id;
}
/* }}} */

/* {{{ _close_pgsql_link
 */
static void _close_pgsql_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link = (PGconn *)rsrc->ptr;
	PGresult *res;

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	PQfinish(link);
	PGG(num_links)--;
}
/* }}} */

/* {{{ _close_pgsql_plink
 */
static void _close_pgsql_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link = (PGconn *)rsrc->ptr;
	PGresult *res;

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	PQfinish(link);
	PGG(num_persistent)--;
	PGG(num_links)--;
}
/* }}} */

/* {{{ _php_pgsql_notice_handler
 */
static void _php_pgsql_notice_handler(void *resource_id, const char *message)
{
	php_pgsql_notice *notice;
	
	TSRMLS_FETCH();
	if (! PGG(ignore_notices)) {
		notice = (php_pgsql_notice *)emalloc(sizeof(php_pgsql_notice));
		notice->message = _php_pgsql_trim_message(message, (int *)&notice->len);
		if (PGG(log_notices)) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%s", notice->message);
		}
		zend_hash_index_update(&PGG(notices), (ulong)resource_id, (void **)&notice, sizeof(php_pgsql_notice *), NULL);
	}
}
/* }}} */

#define PHP_PGSQL_NOTICE_PTR_DTOR (void (*)(void *))_php_pgsql_notice_ptr_dtor

/* {{{ _php_pgsql_notice_dtor
 */
static void _php_pgsql_notice_ptr_dtor(void **ptr) 
{
	php_pgsql_notice *notice = (php_pgsql_notice *)*ptr;
	if (notice) {
 		efree(notice->message);
  		efree(notice);
  		notice = NULL;
  	}
}
/* }}} */

/* {{{ _rollback_transactions
 */
static int _rollback_transactions(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link;
	PGresult *res;
	int orig;

	if (Z_TYPE_P(rsrc) != le_plink) 
		return 0;

	link = (PGconn *) rsrc->ptr;

	if (PQ_SETNONBLOCKING(link, 0)) {
		php_error_docref("ref.pgsql" TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
		return -1;
	}
	
	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
#if HAVE_PGTRANSACTIONSTATUS && HAVE_PQPROTOCOLVERSION
	if ((PQprotocolVersion(link) >= 3 && PQtransactionStatus(link) != PQTRANS_IDLE) || PQprotocolVersion(link) < 3)
#endif
	{
		orig = PGG(ignore_notices);
		PGG(ignore_notices) = 1;
#if HAVE_PGTRANSACTIONSTATUS && HAVE_PQPROTOCOLVERSION
		res = PQexec(link,"ROLLBACK;");
#else
		res = PQexec(link,"BEGIN;");
		PQclear(res);
		res = PQexec(link,"ROLLBACK;");
#endif
		PQclear(res);
		PGG(ignore_notices) = orig;
	}

	return 0;
}
/* }}} */

/* {{{ _free_ptr
 */
static void _free_ptr(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pgLofp *lofp = (pgLofp *)rsrc->ptr;
	efree(lofp);
}
/* }}} */

/* {{{ _free_result
 */
static void _free_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pgsql_result_handle *pg_result = (pgsql_result_handle *)rsrc->ptr;

	PQclear(pg_result->result);
	efree(pg_result);
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
STD_PHP_INI_BOOLEAN( "pgsql.allow_persistent",      "1",  PHP_INI_SYSTEM, OnUpdateBool, allow_persistent,      zend_pgsql_globals, pgsql_globals)
STD_PHP_INI_ENTRY_EX("pgsql.max_persistent",       "-1",  PHP_INI_SYSTEM, OnUpdateLong, max_persistent,        zend_pgsql_globals, pgsql_globals, display_link_numbers)
STD_PHP_INI_ENTRY_EX("pgsql.max_links",            "-1",  PHP_INI_SYSTEM, OnUpdateLong, max_links,             zend_pgsql_globals, pgsql_globals, display_link_numbers)
STD_PHP_INI_BOOLEAN( "pgsql.auto_reset_persistent", "0",  PHP_INI_SYSTEM, OnUpdateBool, auto_reset_persistent, zend_pgsql_globals, pgsql_globals)
STD_PHP_INI_BOOLEAN( "pgsql.ignore_notice",         "0",  PHP_INI_ALL,    OnUpdateBool, ignore_notices,        zend_pgsql_globals, pgsql_globals)
STD_PHP_INI_BOOLEAN( "pgsql.log_notice",            "0",  PHP_INI_ALL,    OnUpdateBool, log_notices,           zend_pgsql_globals, pgsql_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(pgsql)
{
	memset(pgsql_globals, 0, sizeof(zend_pgsql_globals));
	/* Initilize notice message hash at MINIT only */
	zend_hash_init_ex(&pgsql_globals->notices, 0, NULL, PHP_PGSQL_NOTICE_PTR_DTOR, 1, 0); 
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pgsql)
{
	REGISTER_INI_ENTRIES();
	
	le_link = zend_register_list_destructors_ex(_close_pgsql_link, NULL, "pgsql link", module_number);
	le_plink = zend_register_list_destructors_ex(NULL, _close_pgsql_plink, "pgsql link persistent", module_number);
	le_result = zend_register_list_destructors_ex(_free_result, NULL, "pgsql result", module_number);
	le_lofp = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql large object", module_number);
	le_string = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql string", module_number);
#if HAVE_PG_CONFIG_H
	/* PG_VERSION - libpq version */
	REGISTER_STRING_CONSTANT("PGSQL_LIBPQ_VERSION", PG_VERSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("PGSQL_LIBPQ_VERSION_STR", PG_VERSION_STR, CONST_CS | CONST_PERSISTENT);
#endif
	/* For connection option */
	REGISTER_LONG_CONSTANT("PGSQL_CONNECT_FORCE_NEW", PGSQL_CONNECT_FORCE_NEW, CONST_CS | CONST_PERSISTENT);
	/* For pg_fetch_array() */
	REGISTER_LONG_CONSTANT("PGSQL_ASSOC", PGSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NUM", PGSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BOTH", PGSQL_BOTH, CONST_CS | CONST_PERSISTENT);
	/* For pg_connection_status() */
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_BAD", CONNECTION_BAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_OK", CONNECTION_OK, CONST_CS | CONST_PERSISTENT);
#if HAVE_PGTRANSACTIONSTATUS
	/* For pg_transaction_status() */
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_IDLE", PQTRANS_IDLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_ACTIVE", PQTRANS_ACTIVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_INTRANS", PQTRANS_INTRANS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_INERROR", PQTRANS_INERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_UNKNOWN", PQTRANS_UNKNOWN, CONST_CS | CONST_PERSISTENT);
#endif
#if HAVE_PQSETERRORVERBOSITY
	/* For pg_set_error_verbosity() */
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_TERSE", PQERRORS_TERSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_DEFAULT", PQERRORS_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_VERBOSE", PQERRORS_VERBOSE, CONST_CS | CONST_PERSISTENT);
#endif
	/* For lo_seek() */
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_SET", SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_CUR", SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_END", SEEK_END, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_status() return value type */
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_LONG", PGSQL_STATUS_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_STRING", PGSQL_STATUS_STRING, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_status() return value */
	REGISTER_LONG_CONSTANT("PGSQL_EMPTY_QUERY", PGRES_EMPTY_QUERY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COMMAND_OK", PGRES_COMMAND_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TUPLES_OK", PGRES_TUPLES_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_OUT", PGRES_COPY_OUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_IN", PGRES_COPY_IN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BAD_RESPONSE", PGRES_BAD_RESPONSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NONFATAL_ERROR", PGRES_NONFATAL_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_FATAL_ERROR", PGRES_FATAL_ERROR, CONST_CS | CONST_PERSISTENT);
#if HAVE_PQRESULTERRORFIELD
	/* For pg_result_error_field() field codes */
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SEVERITY", PG_DIAG_SEVERITY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SQLSTATE", PG_DIAG_SQLSTATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_PRIMARY", PG_DIAG_MESSAGE_PRIMARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_DETAIL", PG_DIAG_MESSAGE_DETAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_HINT", PG_DIAG_MESSAGE_HINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_STATEMENT_POSITION", PG_DIAG_STATEMENT_POSITION, CONST_CS | CONST_PERSISTENT);
#ifdef PG_DIAG_INTERNAL_POSITION
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_INTERNAL_POSITION", PG_DIAG_INTERNAL_POSITION, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_INTERNAL_QUERY
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_INTERNAL_QUERY", PG_DIAG_INTERNAL_QUERY, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_CONTEXT", PG_DIAG_CONTEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_FILE", PG_DIAG_SOURCE_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_LINE", PG_DIAG_SOURCE_LINE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_FUNCTION", PG_DIAG_SOURCE_FUNCTION, CONST_CS | CONST_PERSISTENT);
#endif
	/* pg_convert options */
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_DEFAULT", PGSQL_CONV_IGNORE_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_FORCE_NULL", PGSQL_CONV_FORCE_NULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_NOT_NULL", PGSQL_CONV_IGNORE_NOT_NULL, CONST_CS | CONST_PERSISTENT);
	/* pg_insert/update/delete/select options */
	REGISTER_LONG_CONSTANT("PGSQL_DML_NO_CONV", PGSQL_DML_NO_CONV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_EXEC", PGSQL_DML_EXEC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_ASYNC", PGSQL_DML_ASYNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_STRING", PGSQL_DML_STRING, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pgsql)
{
	UNREGISTER_INI_ENTRIES();
	zend_hash_destroy(&PGG(notices));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pgsql)
{
	PGG(default_link)=-1;
	PGG(num_links) = PGG(num_persistent);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pgsql)
{
	/* clean up notice messages */
	zend_hash_clean(&PGG(notices));
	/* clean up persistent connection */
	zend_hash_apply(&EG(persistent_list), (apply_func_t) _rollback_transactions TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pgsql)
{
	char buf[256];

	php_info_print_table_start();
	php_info_print_table_header(2, "PostgreSQL Support", "enabled");
#if HAVE_PG_CONFIG_H
	php_info_print_table_row(2, "PostgreSQL(libpq) Version", PG_VERSION);
	php_info_print_table_row(2, "PostgreSQL(libpq) ", PG_VERSION_STR);
#ifdef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
	php_info_print_table_row(2, "Multibyte character support", "enabled");
#else
	php_info_print_table_row(2, "Multibyte character support", "disabled");
#endif
#ifdef USE_SSL
	php_info_print_table_row(2, "SSL support", "enabled");
#else
	php_info_print_table_row(2, "SSL support", "disabled");
#endif
#endif /* HAVE_PG_CONFIG_H */	
	snprintf(buf, sizeof(buf), "%ld", PGG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	snprintf(buf, sizeof(buf), "%ld", PGG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* {{{ php_pgsql_do_connect
 */
static void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *host=NULL,*port=NULL,*options=NULL,*tty=NULL,*dbname=NULL,*connstring=NULL;
	PGconn *pgsql;
	smart_str str = {0};
	zval **args[5];
	int i, connect_type = 0;
	PGresult *pg_result;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 5
			|| zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	smart_str_appends(&str, "pgsql");
	
	for (i = 0; i < ZEND_NUM_ARGS(); i++) {
		/* make sure that the PGSQL_CONNECT_FORCE_NEW bit is not part of the hash so that subsequent connections
		 * can re-use this connection. Bug #39979
		 */ 
		if (i == 1 && ZEND_NUM_ARGS() == 2 && Z_TYPE_PP(args[i]) == IS_LONG) {
			if (Z_LVAL_PP(args[1]) == PGSQL_CONNECT_FORCE_NEW) {
				continue;
			} else if (Z_LVAL_PP(args[1]) & PGSQL_CONNECT_FORCE_NEW) {
				smart_str_append_long(&str, Z_LVAL_PP(args[1]) ^ PGSQL_CONNECT_FORCE_NEW);
			}
		}
		convert_to_string_ex(args[i]);
		smart_str_appendc(&str, '_');
		smart_str_appendl(&str, Z_STRVAL_PP(args[i]), Z_STRLEN_PP(args[i]));
	}

	smart_str_0(&str);

	if (ZEND_NUM_ARGS() == 1) { /* new style, using connection string */
		connstring = Z_STRVAL_PP(args[0]);
	} else if (ZEND_NUM_ARGS() == 2 ) { /* Safe to add conntype_option, since 2 args was illegal */
		connstring = Z_STRVAL_PP(args[0]);
		convert_to_long_ex(args[1]);
		connect_type = Z_LVAL_PP(args[1]);
	} else {
		host = Z_STRVAL_PP(args[0]);
		port = Z_STRVAL_PP(args[1]);
		dbname = Z_STRVAL_PP(args[ZEND_NUM_ARGS()-1]);

		switch (ZEND_NUM_ARGS()) {
		case 5:
			tty = Z_STRVAL_PP(args[3]);
			/* fall through */
		case 4:
			options = Z_STRVAL_PP(args[2]);
			break;
		}
	}
	
	if (persistent && PGG(allow_persistent)) {
		zend_rsrc_list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), str.c, str.len+1, (void **) &le)==FAILURE) {  /* we don't */
			zend_rsrc_list_entry new_le;
			
			if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
								 "Cannot create new link. Too many open links (%ld)", PGG(num_links));
				goto err;
			}
			if (PGG(max_persistent)!=-1 && PGG(num_persistent)>=PGG(max_persistent)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
								 "Cannot create new link. Too many open persistent links (%ld)", PGG(num_persistent));
				goto err;
			}

			/* create the link */
			if (connstring) {
				pgsql=PQconnectdb(connstring);
			} else {
				pgsql=PQsetdb(host,port,options,tty,dbname);
			}
			if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
				PHP_PQ_ERROR("Unable to connect to PostgreSQL server: %s", pgsql)
				if (pgsql) {
					PQfinish(pgsql);
				}
				goto err;
			}

			/* hash it up */
			Z_TYPE(new_le) = le_plink;
			new_le.ptr = pgsql;
			if (zend_hash_update(&EG(persistent_list), str.c, str.len+1, (void *) &new_le, sizeof(zend_rsrc_list_entry), NULL)==FAILURE) {
				goto err;
			}
			PGG(num_links)++;
			PGG(num_persistent)++;
		} else {  /* we do */
			if (Z_TYPE_P(le) != le_plink) {
				RETURN_FALSE;
			}
			/* ensure that the link did not die */
			if (PGG(auto_reset_persistent) & 1) {
				/* need to send & get something from backend to
				   make sure we catch CONNECTION_BAD everytime */
				PGresult *pg_result;
				pg_result = PQexec(le->ptr, "select 1");
				PQclear(pg_result);
			}
			if (PQstatus(le->ptr)==CONNECTION_BAD) { /* the link died */
				if (le->ptr == NULL) {
					if (connstring) {
						le->ptr=PQconnectdb(connstring);
					} else {
						le->ptr=PQsetdb(host,port,options,tty,dbname);
					}
				}
				else {
					PQreset(le->ptr);
				}
				if (le->ptr==NULL || PQstatus(le->ptr)==CONNECTION_BAD) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,"PostgreSQL link lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list),str.c,str.len+1);
					goto err;
				}
			}
			pgsql = (PGconn *) le->ptr;
#if HAVE_PQPROTOCOLVERSION && HAVE_PQPARAMETERSTATUS
			if (PQprotocolVersion(pgsql) >= 3 && atof(PQparameterStatus(pgsql, "server_version")) >= 7.2) {
#else
			if (atof(PG_VERSION) >= 7.2) {
#endif
				pg_result = PQexec(pgsql, "RESET ALL;");
				PQclear(pg_result);
			}
		}
		ZEND_REGISTER_RESOURCE(return_value, pgsql, le_plink);
	} else { /* Non persistent connection */
		zend_rsrc_list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual pgsql link sits.
		 * if it doesn't, open a new pgsql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (!(connect_type & PGSQL_CONNECT_FORCE_NEW)
			&& zend_hash_find(&EG(regular_list),str.c,str.len+1,(void **) &index_ptr)==SUCCESS) {
			int type;
			ulong link;
			void *ptr;

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (ulong) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				Z_LVAL_P(return_value) = link;
				zend_list_addref(link);
				php_pgsql_set_default_link(link TSRMLS_CC);
				Z_TYPE_P(return_value) = IS_RESOURCE;
				goto cleanup;
			} else {
				zend_hash_del(&EG(regular_list),str.c,str.len+1);
			}
		}
		if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create new link. Too many open links (%ld)", PGG(num_links));
			goto err;
		}
		if (connstring) {
			pgsql = PQconnectdb(connstring);
		} else {
			pgsql = PQsetdb(host,port,options,tty,dbname);
		}
		if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
			PHP_PQ_ERROR("Unable to connect to PostgreSQL server: %s", pgsql);
			if (pgsql) {
				PQfinish(pgsql);
			}
			goto err;
		}

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, pgsql, le_link);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list),str.c,str.len+1,(void *) &new_index_ptr, sizeof(zend_rsrc_list_entry), NULL)==FAILURE) {
			goto err;
		}
		PGG(num_links)++;
	}
	/* set notice processer */
	if (! PGG(ignore_notices) && Z_TYPE_P(return_value) == IS_RESOURCE) {
		PQsetNoticeProcessor(pgsql, _php_pgsql_notice_handler, (void*)Z_RESVAL_P(return_value));
	}
	php_pgsql_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
	
cleanup:
	smart_str_free(&str);
	return;
	
err:
	smart_str_free(&str);
	RETURN_FALSE;
}
/* }}} */

#if 0
/* {{{ php_pgsql_get_default_link
 */
static int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (PGG(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
	}
	return PGG(default_link);
}
/* }}} */
#endif

/* {{{ proto resource pg_connect(string connection_string[, int connect_type] | [string host, string port [, string options [, string tty,]]] string database)
   Open a PostgreSQL connection */
PHP_FUNCTION(pg_connect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto resource pg_pconnect(string connection_string | [string host, string port [, string options [, string tty,]]] string database)
   Open a persistent PostgreSQL connection */
PHP_FUNCTION(pg_pconnect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto bool pg_close([resource connection])
   Close a PostgreSQL connection */ 
PHP_FUNCTION(pg_close)
{
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &pgsql_link) == FAILURE) {
		return;
	}
	
	if (argc == 0) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (id==-1) { /* explicit resource number */
		zend_list_delete(Z_RESVAL_P(pgsql_link));
	}

	if (id!=-1 
		|| (pgsql_link && Z_RESVAL_P(pgsql_link)==PGG(default_link))) {
		zend_list_delete(PGG(default_link));
		PGG(default_link) = -1;
	}

	RETURN_TRUE;
}
/* }}} */


#define PHP_PG_DBNAME 1
#define PHP_PG_ERROR_MESSAGE 2
#define PHP_PG_OPTIONS 3
#define PHP_PG_PORT 4
#define PHP_PG_TTY 5
#define PHP_PG_HOST 6
#define PHP_PG_VERSION 7

/* {{{ php_pgsql_get_link_info
 */
static void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	char *msgbuf;

	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &pgsql_link) == FAILURE) {
		return;
	}
	
	if (argc == 0) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	
	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	switch(entry_type) {
		case PHP_PG_DBNAME:
			Z_STRVAL_P(return_value) = PQdb(pgsql);
			break;
		case PHP_PG_ERROR_MESSAGE:
			RETURN_STRING(PQErrorMessageTrim(pgsql, &msgbuf), 0);
			return;
		case PHP_PG_OPTIONS:
			Z_STRVAL_P(return_value) = PQoptions(pgsql);
			break;
		case PHP_PG_PORT:
			Z_STRVAL_P(return_value) = PQport(pgsql);
			break;
		case PHP_PG_TTY:
			Z_STRVAL_P(return_value) = PQtty(pgsql);
			break;
		case PHP_PG_HOST:
			Z_STRVAL_P(return_value) = PQhost(pgsql);
			break;
		case PHP_PG_VERSION:
			array_init(return_value);
			add_assoc_string(return_value, "client", PG_VERSION, 1);
#if HAVE_PQPROTOCOLVERSION
			add_assoc_long(return_value, "protocol", PQprotocolVersion(pgsql));
#if HAVE_PQPARAMETERSTATUS
			if (PQprotocolVersion(pgsql) >= 3) {
				add_assoc_string(return_value, "server", (char*)PQparameterStatus(pgsql, "server_version"), 1);
			}
#endif
#endif
			return;
		default:
			RETURN_FALSE;
	}
	if (Z_STRVAL_P(return_value)) {
		Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
		Z_STRVAL_P(return_value) = (char *) estrdup(Z_STRVAL_P(return_value));
	} else {
		Z_STRLEN_P(return_value) = 0;
		Z_STRVAL_P(return_value) = (char *) estrdup("");
	}
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto string pg_dbname([resource connection])
   Get the database name */ 
PHP_FUNCTION(pg_dbname)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_DBNAME);
}
/* }}} */

/* {{{ proto string pg_last_error([resource connection])
   Get the error message string */
PHP_FUNCTION(pg_last_error)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_ERROR_MESSAGE);
}
/* }}} */

/* {{{ proto string pg_options([resource connection])
   Get the options associated with the connection */
PHP_FUNCTION(pg_options)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_OPTIONS);
}
/* }}} */

/* {{{ proto int pg_port([resource connection])
   Return the port number associated with the connection */
PHP_FUNCTION(pg_port)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_PORT);
}
/* }}} */

/* {{{ proto string pg_tty([resource connection])
   Return the tty name associated with the connection */
PHP_FUNCTION(pg_tty)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_TTY);
}
/* }}} */

/* {{{ proto string pg_host([resource connection])
   Returns the host name associated with the connection */
PHP_FUNCTION(pg_host)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_HOST);
}
/* }}} */

/* {{{ proto array pg_version([resource connection])
   Returns an array with client, protocol and server version (when available) */
PHP_FUNCTION(pg_version)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_VERSION);
}
/* }}} */

#if HAVE_PQPARAMETERSTATUS
/* {{{ proto string|false pg_parameter_status([resource connection,] string param_name)
   Returns the value of a server parameter */
PHP_FUNCTION(pg_parameter_status)
{
	zval *pgsql_link;
	int id;
	PGconn *pgsql;
	char *param;
	int len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pgsql_link, &param, &len) == SUCCESS) {
		id = -1;
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &param, &len) == SUCCESS) {
		pgsql_link = NULL;
		id = PGG(default_link);
	} else {
		RETURN_FALSE;
	}
	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	param = (char*)PQparameterStatus(pgsql, param);
	if (param) {
		RETURN_STRING(param, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ proto bool pg_ping([resource connection])
   Ping database. If connection is bad, try to reconnect. */
PHP_FUNCTION(pg_ping)
{
	zval *pgsql_link;
	int id;
	PGconn *pgsql;
	PGresult *res;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r", &pgsql_link) == SUCCESS) {
		id = -1;
	} else {
		pgsql_link = NULL;
		id = PGG(default_link);
	}
	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	/* ping connection */
	res = PQexec(pgsql, "SELECT 1;");
	PQclear(res);

	/* check status. */
	if (PQstatus(pgsql) == CONNECTION_OK)
		RETURN_TRUE;

	/* reset connection if it's broken */
	PQreset(pgsql);
	if (PQstatus(pgsql) == CONNECTION_OK) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto resource pg_query([resource connection,] string query)
   Execute a query */
PHP_FUNCTION(pg_query)
{
	zval *pgsql_link = NULL;
	char *query;
	int id = -1, query_len, argc = ZEND_NUM_ARGS();
	int leftover = 0;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	if (argc == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &query, &query_len) == FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pgsql_link, &query, &query_len) == FAILURE) {
			return;
		}
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}
	pgsql_result = PQexec(pgsql, query);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQexec(pgsql, query);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}
	
	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

#if HAVE_PQEXECPARAMS || HAVE_PQEXECPREPARED || HAVE_PQSENDQUERYPARAMS || HAVE_PQSENDQUERYPREPARED
/* {{{ _php_pgsql_free_params */
static void _php_pgsql_free_params(char **params, int num_params)
{
	if (num_params > 0) {
		int i;
		for (i = 0; i < num_params; i++) {
			if (params[i]) {
				efree(params[i]);
			}
		}
		efree(params);
	}
}
/* }}} */
#endif

#if HAVE_PQEXECPARAMS
/* {{{ proto resource pg_query_params([resource connection,] string query, array params)
   Execute a query */
PHP_FUNCTION(pg_query_params)
{
	zval *pgsql_link = NULL;
	zval *pv_param_arr, **tmp;
	char *query;
	int query_len, id = -1, argc = ZEND_NUM_ARGS();
	int leftover = 0;
	int num_params = 0;
	char **params = NULL;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;
	
	if (argc == 2) {
		if (zend_parse_parameters(argc TSRMLS_CC, "sa", &query, &query_len, &pv_param_arr) == FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rsa", &pgsql_link, &query, &query_len, &pv_param_arr) == FAILURE) {
			return;
		}
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(pv_param_arr));
	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);
		
		for(i = 0; i < num_params; i++) {
			if (zend_hash_get_current_data(Z_ARRVAL_P(pv_param_arr), (void **) &tmp) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error getting parameter");
				_php_pgsql_free_params(params, num_params);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zval tmp_val = **tmp;
				zval_copy_ctor(&tmp_val);
				convert_to_string(&tmp_val);
				if (Z_TYPE(tmp_val) != IS_STRING) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error converting parameter");
					zval_dtor(&tmp_val);
					_php_pgsql_free_params(params, num_params);
					RETURN_FALSE;
				}
				params[i] = estrndup(Z_STRVAL(tmp_val), Z_STRLEN(tmp_val));
				zval_dtor(&tmp_val);
			}

			zend_hash_move_forward(Z_ARRVAL_P(pv_param_arr));
		}
	}

	pgsql_result = PQexecParams(pgsql, query, num_params, 
					NULL, (const char * const *)params, NULL, NULL, 0);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQexecParams(pgsql, query, num_params, 
						NULL, (const char * const *)params, NULL, NULL, 0);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}
	
	_php_pgsql_free_params(params, num_params);

	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */
#endif

#if HAVE_PQPREPARE
/* {{{ proto resource pg_prepare([resource connection,] string stmtname, string query)
   Prepare a query for future execution */
PHP_FUNCTION(pg_prepare)
{
	zval *pgsql_link = NULL;
	char *query, *stmtname;
	int query_len, stmtname_len, id = -1, argc = ZEND_NUM_ARGS();
	int leftover = 0;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	if (argc == 2) {
		if (zend_parse_parameters(argc TSRMLS_CC, "ss", &stmtname, &stmtname_len, &query, &query_len) == FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rss", &pgsql_link, &stmtname, &stmtname_len, &query, &query_len) == FAILURE) {
			return;
		}
	}
	
	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}
	pgsql_result = PQprepare(pgsql, stmtname, query, 0, NULL);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQprepare(pgsql, stmtname, query, 0, NULL);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}
	
	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */
#endif

#if HAVE_PQEXECPREPARED
/* {{{ proto resource pg_execute([resource connection,] string stmtname, array params)
   Execute a prepared query  */
PHP_FUNCTION(pg_execute)
{
	zval *pgsql_link = NULL;
	zval *pv_param_arr, **tmp;
	char *stmtname;
	int stmtname_len, id = -1, argc = ZEND_NUM_ARGS();
	int leftover = 0;
	int num_params = 0;
	char **params = NULL;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	if (argc == 2) {
		if (zend_parse_parameters(argc TSRMLS_CC, "sa/", &stmtname, &stmtname_len, &pv_param_arr)==FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rsa/", &pgsql_link, &stmtname, &stmtname_len, &pv_param_arr) == FAILURE) {
			return;
		}
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(pv_param_arr));
	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);
		
		for(i = 0; i < num_params; i++) {
			if (zend_hash_get_current_data(Z_ARRVAL_P(pv_param_arr), (void **) &tmp) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error getting parameter");
				_php_pgsql_free_params(params, num_params);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zval tmp_val = **tmp;
				zval_copy_ctor(&tmp_val);
				convert_to_string(&tmp_val);
				if (Z_TYPE(tmp_val) != IS_STRING) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error converting parameter");
					zval_dtor(&tmp_val);
					_php_pgsql_free_params(params, num_params);
					RETURN_FALSE;
				}
				params[i] = estrndup(Z_STRVAL(tmp_val), Z_STRLEN(tmp_val));
				zval_dtor(&tmp_val);
			}

			zend_hash_move_forward(Z_ARRVAL_P(pv_param_arr));
		}
	}

	pgsql_result = PQexecPrepared(pgsql, stmtname, num_params, 
					(const char * const *)params, NULL, NULL, 0);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQexecPrepared(pgsql, stmtname, num_params, 
						(const char * const *)params, NULL, NULL, 0);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}
	
	_php_pgsql_free_params(params, num_params);

	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */
#endif

#define PHP_PG_NUM_ROWS 1
#define PHP_PG_NUM_FIELDS 2
#define PHP_PG_CMD_TUPLES 3

/* {{{ php_pgsql_get_result_info
 */
static void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;

	switch (entry_type) {
		case PHP_PG_NUM_ROWS:
			Z_LVAL_P(return_value) = PQntuples(pgsql_result);
			break;
		case PHP_PG_NUM_FIELDS:
			Z_LVAL_P(return_value) = PQnfields(pgsql_result);
			break;
		case PHP_PG_CMD_TUPLES:
#if HAVE_PQCMDTUPLES
			Z_LVAL_P(return_value) = atoi(PQcmdTuples(pgsql_result));
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not supported under this build");
			Z_LVAL_P(return_value) = 0;
#endif
			break;
		default:
			RETURN_FALSE;
	}
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_num_rows(resource result)
   Return the number of rows in the result */
PHP_FUNCTION(pg_num_rows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_ROWS);
}
/* }}} */

/* {{{ proto int pg_num_fields(resource result)
   Return the number of fields in the result */
PHP_FUNCTION(pg_num_fields)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_FIELDS);
}
/* }}} */

#if HAVE_PQCMDTUPLES
/* {{{ proto int pg_affected_rows(resource result)
   Returns the number of affected tuples */
PHP_FUNCTION(pg_affected_rows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_CMD_TUPLES);
}
/* }}} */
#endif

/* {{{ proto string pg_last_notice(resource connection)
   Returns the last notice set by the backend */
PHP_FUNCTION(pg_last_notice) 
{
	zval *pgsql_link;
	PGconn *pg_link;
	int id = -1;
	php_pgsql_notice **notice;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pgsql_link) == FAILURE) {
		return;
	}
	/* Just to check if user passed valid resoruce */
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (zend_hash_index_find(&PGG(notices), Z_RESVAL_P(pgsql_link), (void **)&notice) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((*notice)->message, (*notice)->len, 1);
}
/* }}} */

/* {{{ get_field_name
 */
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list TSRMLS_DC)
{
	PGresult *result;
	smart_str str = {0};
	zend_rsrc_list_entry *field_type;
	char *ret=NULL;

	/* try to lookup the type in the resource list */
	smart_str_appends(&str, "pgsql_oid_");
	smart_str_append_unsigned(&str, oid);
	smart_str_0(&str);

	if (zend_hash_find(list,str.c,str.len+1,(void **) &field_type)==SUCCESS) {
		ret = estrdup((char *)field_type->ptr);
	} else { /* hash all oid's */
		int i,num_rows;
		int oid_offset,name_offset;
		char *tmp_oid, *end_ptr, *tmp_name;
		zend_rsrc_list_entry new_oid_entry;

		if ((result = PQexec(pgsql,"select oid,typname from pg_type")) == NULL || PQresultStatus(result) != PGRES_TUPLES_OK) {
			if (result) {
				PQclear(result);
			}
			smart_str_free(&str);
			return STR_EMPTY_ALLOC();
		}
		num_rows = PQntuples(result);
		oid_offset = PQfnumber(result,"oid");
		name_offset = PQfnumber(result,"typname");
		
		for (i=0; i<num_rows; i++) {
			if ((tmp_oid = PQgetvalue(result,i,oid_offset))==NULL) {
				continue;
			}
			
			str.len = 0;
			smart_str_appends(&str, "pgsql_oid_");
			smart_str_appends(&str, tmp_oid);
			smart_str_0(&str);
	
			if ((tmp_name = PQgetvalue(result,i,name_offset))==NULL) {
				continue;
			}
			Z_TYPE(new_oid_entry) = le_string;
			new_oid_entry.ptr = estrdup(tmp_name);
			zend_hash_update(list,str.c,str.len+1,(void *) &new_oid_entry, sizeof(zend_rsrc_list_entry), NULL);
			if (!ret && strtoul(tmp_oid, &end_ptr, 10)==oid) {
				ret = estrdup(tmp_name);
			}
		}
		PQclear(result);
	}

	smart_str_free(&str);
	return ret;
}
/* }}} */			

#ifdef HAVE_PQFTABLE
/* {{{ proto mixed pg_field_table(resource result, int field_number[, bool oid_only])
   Returns the name of the table field belongs to, or table's oid if oid_only is true */
PHP_FUNCTION(pg_field_table)
{
	zval *result;
	pgsql_result_handle *pg_result;
	long fnum = -1;
	zend_bool return_oid = 0;
	Oid oid;
	smart_str hash_key = {0};
	char *table_name;
	zend_rsrc_list_entry *field_table;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|b", &result, &fnum, &return_oid) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	if (fnum < 0 || fnum >= PQnfields(pg_result->result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad field offset specified");
		RETURN_FALSE;
	}

	oid = PQftable(pg_result->result, fnum);

	if (InvalidOid == oid) {
		RETURN_FALSE;
	}


	if (return_oid) {
#if UINT_MAX > LONG_MAX /* Oid is unsigned int, we don't need this code, where LONG is wider */
		if (oid > LONG_MAX) {
			smart_str oidstr = {0};
			smart_str_append_unsigned(&oidstr, oid);
			smart_str_0(&oidstr);
			RETURN_STRINGL(oidstr.c, oidstr.len, 0);
		} else
#endif
			RETURN_LONG((long)oid);
	}

	/* try to lookup the table name in the resource list */
	smart_str_appends(&hash_key, "pgsql_table_oid_");
	smart_str_append_unsigned(&hash_key, oid);
	smart_str_0(&hash_key);

	if (zend_hash_find(&EG(regular_list), hash_key.c, hash_key.len+1, (void **) &field_table) == SUCCESS) {
		smart_str_free(&hash_key);
		RETURN_STRING((char *)field_table->ptr, 1);
	} else { /* Not found, lookup by querying PostgreSQL system tables */
		PGresult *tmp_res;
		smart_str querystr = {0};
		zend_rsrc_list_entry new_field_table;

		smart_str_appends(&querystr, "select relname from pg_class where oid=");
		smart_str_append_unsigned(&querystr, oid);
		smart_str_0(&querystr);


		if ((tmp_res = PQexec(pg_result->conn, querystr.c)) == NULL || PQresultStatus(tmp_res) != PGRES_TUPLES_OK) {
			if (tmp_res) {
				PQclear(tmp_res);
			}
			smart_str_free(&querystr);
			smart_str_free(&hash_key);
			RETURN_FALSE;
		}

		smart_str_free(&querystr);

		if ((table_name = PQgetvalue(tmp_res, 0, 0)) == NULL) {
			PQclear(tmp_res);
			smart_str_free(&hash_key);
			RETURN_FALSE;
		}

		Z_TYPE(new_field_table) = le_string;
		new_field_table.ptr = estrdup(table_name);
		zend_hash_update(&EG(regular_list), hash_key.c, hash_key.len+1, (void *) &new_field_table, sizeof(zend_rsrc_list_entry), NULL);

		smart_str_free(&hash_key);
		PQclear(tmp_res);
		RETURN_STRING(table_name, 1);
	}

}
/* }}} */	
#endif		

#define PHP_PG_FIELD_NAME 1
#define PHP_PG_FIELD_SIZE 2
#define PHP_PG_FIELD_TYPE 3
#define PHP_PG_FIELD_TYPE_OID 4

/* {{{ php_pgsql_get_field_info
 */
static void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *result;
	long field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	Oid oid;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &result, &field) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	
	if (field < 0 || field >= PQnfields(pgsql_result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad field offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_PG_FIELD_NAME:
			Z_STRVAL_P(return_value) = PQfname(pgsql_result, field);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_STRVAL_P(return_value) = estrndup(Z_STRVAL_P(return_value),Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_PG_FIELD_SIZE:
			Z_LVAL_P(return_value) = PQfsize(pgsql_result, field);
			Z_TYPE_P(return_value) = IS_LONG;
			break;
		case PHP_PG_FIELD_TYPE:
			Z_STRVAL_P(return_value) = get_field_name(pg_result->conn, PQftype(pgsql_result, field), &EG(regular_list) TSRMLS_CC);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_PG_FIELD_TYPE_OID:
			
			oid = PQftype(pgsql_result, field);
#if UINT_MAX > LONG_MAX
			if (oid > LONG_MAX) {
				smart_str s = {0};
				smart_str_append_unsigned(&s, oid);
				smart_str_0(&s);
				Z_STRVAL_P(return_value) = s.c;
				Z_STRLEN_P(return_value) = s.len;
				Z_TYPE_P(return_value) = IS_STRING;
			} else
#endif
			{
				Z_LVAL_P(return_value) = (long)oid;
				Z_TYPE_P(return_value) = IS_LONG;
			}
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string pg_field_name(resource result, int field_number)
   Returns the name of the field */
PHP_FUNCTION(pg_field_name)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_NAME);
}
/* }}} */

/* {{{ proto int pg_field_size(resource result, int field_number)
   Returns the internal size of the field */ 
PHP_FUNCTION(pg_field_size)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_SIZE);
}
/* }}} */

/* {{{ proto string pg_field_type(resource result, int field_number)
   Returns the type name for the given field */
PHP_FUNCTION(pg_field_type)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_TYPE);
}
/* }}} */


/* {{{ proto string pg_field_type_oid(resource result, int field_number)
   Returns the type oid for the given field */
PHP_FUNCTION(pg_field_type_oid)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_TYPE_OID);
}
/* }}} */

/* {{{ proto int pg_field_num(resource result, string field_name)
   Returns the field number of the named field */
PHP_FUNCTION(pg_field_num)
{
	zval *result;
	char *field;
	int field_len;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &result, &field, &field_len) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	
	Z_LVAL_P(return_value) = PQfnumber(pgsql_result, field);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto mixed pg_fetch_result(resource result, [int row_number,] mixed field_name)
   Returns values from a result identifier */
PHP_FUNCTION(pg_fetch_result)
{
	zval *result, **field=NULL;
	long row;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int field_offset, pgsql_row, argc = ZEND_NUM_ARGS();
	
	if (argc == 2) {
		if (zend_parse_parameters(argc TSRMLS_CC, "rZ", &result, &field) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rlZ", &result, &row, &field) == FAILURE) {
			return;
		}
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (argc == 2) {
		if (pg_result->row < 0) {
			pg_result->row = 0;
		}
		pgsql_row = pg_result->row;
		if (pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
	} else {
		pgsql_row = row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to jump to row %ld on PostgreSQL result index %ld",
							row, Z_LVAL_P(result));
			RETURN_FALSE;
		}
	}
	switch(Z_TYPE_PP(field)) {
		case IS_STRING:
			field_offset = PQfnumber(pgsql_result, Z_STRVAL_PP(field));
			break;
		default:
			convert_to_long_ex(field);
			field_offset = Z_LVAL_PP(field);
			break;
	}
	if (field_offset<0 || field_offset>=PQnfields(pgsql_result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset specified");
		RETURN_FALSE;
	}
	
	if (PQgetisnull(pgsql_result, pgsql_row, field_offset)) {
		Z_TYPE_P(return_value) = IS_NULL;
	} else {
		char *value = PQgetvalue(pgsql_result, pgsql_row, field_offset);
		int value_len = PQgetlength(pgsql_result, pgsql_row, field_offset);
		ZVAL_STRINGL(return_value, value, value_len, 1);
	}
}
/* }}} */

/* {{{ void php_pgsql_fetch_hash */
static void php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, long result_type, int into_object)
{
	zval                *result, *zrow = NULL;
	PGresult            *pgsql_result;
	pgsql_result_handle *pg_result;
	int             i, num_fields, pgsql_row, use_row;
	long            row = -1;
	char            *field_name;
	zval            *ctor_params = NULL;
	zend_class_entry *ce = NULL;

	if (into_object) {
		char *class_name = NULL;
		int class_name_len;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|z!sz", &result, &zrow, &class_name, &class_name_len, &ctor_params) == FAILURE) {
			return;
		}
		if (!class_name) {
			ce = zend_standard_class_def;
		} else {
			ce = zend_fetch_class(class_name, class_name_len, ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
		}
		if (!ce) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not find class '%s'", class_name);
			return;
		}
		result_type = PGSQL_ASSOC;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|z!l", &result, &zrow, &result_type) == FAILURE) {
			return;
		}
	}
	if (zrow == NULL) {
		row = -1;
	} else {
		convert_to_long(zrow);
		row = Z_LVAL_P(zrow);
		if (row < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The row parameter must be greater or equal to zero");
			RETURN_FALSE;
		}
	}
	use_row = ZEND_NUM_ARGS() > 1 && row != -1;

	if (!(result_type & PGSQL_BOTH)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid result type");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;

	if (use_row) { 
		pgsql_row = row;
		pg_result->row = pgsql_row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to jump to row %ld on PostgreSQL result index %ld",
							row, Z_LVAL_P(result));
			RETURN_FALSE;
		}
	} else {
		/* If 2nd param is NULL, use internal row counter to access next row */
		pgsql_row = pg_result->row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
		pg_result->row++;
	}

	array_init(return_value);
	for (i = 0, num_fields = PQnfields(pgsql_result); i < num_fields; i++) {
		if (PQgetisnull(pgsql_result, pgsql_row, i)) {
			if (result_type & PGSQL_NUM) {
				add_index_null(return_value, i);
			}
			if (result_type & PGSQL_ASSOC) {
				field_name = PQfname(pgsql_result, i);
				add_assoc_null(return_value, field_name);
			}
		} else {
			char *element = PQgetvalue(pgsql_result, pgsql_row, i);
			if (element) {
				char *data;
				int data_len;
				int should_copy=0;
				const uint element_len = strlen(element);

				data = safe_estrndup(element, element_len);
				data_len = element_len;
			
				if (result_type & PGSQL_NUM) {
					add_index_stringl(return_value, i, data, data_len, should_copy);
					should_copy=1;
				}
			
				if (result_type & PGSQL_ASSOC) {
					field_name = PQfname(pgsql_result, i);
					add_assoc_stringl(return_value, field_name, data, data_len, should_copy);
				}
			}
		}
	}

	if (into_object) {
		zval dataset = *return_value;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval *retval_ptr; 
	
		object_and_properties_init(return_value, ce, NULL);
		zend_merge_properties(return_value, Z_ARRVAL(dataset), 1 TSRMLS_CC);
	
		if (ce->constructor) {
			fci.size = sizeof(fci);
			fci.function_table = &ce->function_table;
			fci.function_name = NULL;
			fci.symbol_table = NULL;
			fci.object_ptr = return_value;
			fci.retval_ptr_ptr = &retval_ptr;
			if (ctor_params && Z_TYPE_P(ctor_params) != IS_NULL) {
				if (Z_TYPE_P(ctor_params) == IS_ARRAY) {
					HashTable *ht = Z_ARRVAL_P(ctor_params);
					Bucket *p;
	
					fci.param_count = 0;
					fci.params = safe_emalloc(sizeof(zval*), ht->nNumOfElements, 0);
					p = ht->pListHead;
					while (p != NULL) {
						fci.params[fci.param_count++] = (zval**)p->pData;
						p = p->pListNext;
					}
				} else {
					/* Two problems why we throw exceptions here: PHP is typeless
					 * and hence passing one argument that's not an array could be
					 * by mistake and the other way round is possible, too. The 
					 * single value is an array. Also we'd have to make that one
					 * argument passed by reference.
					 */
					zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Parameter ctor_params must be an array", 0 TSRMLS_CC);
					return;
				}
			} else {
				fci.param_count = 0;
				fci.params = NULL;
			}
			fci.no_separation = 1;

			fcc.initialized = 1;
			fcc.function_handler = ce->constructor;
			fcc.calling_scope = EG(scope);
			fcc.called_scope = Z_OBJCE_P(return_value);
			fcc.object_ptr = return_value;
		
			if (zend_call_function(&fci, &fcc TSRMLS_CC) == FAILURE) {
				zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Could not execute %s::%s()", ce->name, ce->constructor->common.function_name);
			} else {
				if (retval_ptr) {
					zval_ptr_dtor(&retval_ptr);
				}
			}
			if (fci.params) {
				efree(fci.params);
			}
		} else if (ctor_params) {
			zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Class %s does not have a constructor hence you cannot use ctor_params", ce->name);
		}
	}
}
/* }}} */

/* {{{ proto array pg_fetch_row(resource result [, int row [, int result_type]])
   Get a row as an enumerated array */ 
PHP_FUNCTION(pg_fetch_row)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_NUM, 0);
}
/* }}} */

/* {{{ proto array pg_fetch_assoc(resource result [, int row])
   Fetch a row as an assoc array */
PHP_FUNCTION(pg_fetch_assoc)
{
	/* pg_fetch_assoc() is added from PHP 4.3.0. It should raise error, when
	   there is 3rd parameter */
	if (ZEND_NUM_ARGS() > 2)
		WRONG_PARAM_COUNT;
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_ASSOC, 0);
}
/* }}} */

/* {{{ proto array pg_fetch_array(resource result [, int row [, int result_type]])
   Fetch a row as an array */
PHP_FUNCTION(pg_fetch_array)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_BOTH, 0);
}
/* }}} */

/* {{{ proto object pg_fetch_object(resource result [, int row [, string class_name [, NULL|array ctor_params]]])
   Fetch a row as an object */
PHP_FUNCTION(pg_fetch_object)
{
	/* pg_fetch_object() allowed result_type used to be. 3rd parameter
	   must be allowed for compatibility */
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_ASSOC, 1);
}
/* }}} */

/* {{{ proto array pg_fetch_all(resource result)
   Fetch all rows into array */
PHP_FUNCTION(pg_fetch_all)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	array_init(return_value);
	if (php_pgsql_result2array(pgsql_result, return_value TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array pg_fetch_all_columns(resource result [, int column_number])
   Fetch all rows into array */
PHP_FUNCTION(pg_fetch_all_columns)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	unsigned long colno=0;
	int pg_numrows, pg_row;
	size_t num_fields;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &result, &colno) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;

	num_fields = PQnfields(pgsql_result);
	if (colno >= num_fields || colno < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid column number '%ld'", colno);
		RETURN_FALSE;
	}

	array_init(return_value);

        if ((pg_numrows = PQntuples(pgsql_result)) <= 0) {
		return;
	}

	for (pg_row = 0; pg_row < pg_numrows; pg_row++) {
		if (PQgetisnull(pgsql_result, pg_row, colno)) {
			add_next_index_null(return_value);
		} else {
			add_next_index_string(return_value, PQgetvalue(pgsql_result, pg_row, colno), 1); 
		}		
	}
}
/* }}} */

/* {{{ proto bool pg_result_seek(resource result, int offset)
   Set internal row offset */
PHP_FUNCTION(pg_result_seek)
{
	zval *result;
	long row;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &result, &row) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	if (row < 0 || row >= PQntuples(pg_result->result)) {
		RETURN_FALSE;
	}
	
	/* seek to offset */
	pg_result->row = row;
	RETURN_TRUE;
}
/* }}} */


#define PHP_PG_DATA_LENGTH 1
#define PHP_PG_DATA_ISNULL 2

/* {{{ php_pgsql_data_info
 */
static void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *result, **field;
	long row;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int field_offset, pgsql_row, argc = ZEND_NUM_ARGS();

	if (argc == 2) {
		if (zend_parse_parameters(argc TSRMLS_CC, "rZ", &result, &field) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rlZ", &result, &row, &field) == FAILURE) {
			return;
		}
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (argc == 2) {
		if (pg_result->row < 0) {
			pg_result->row = 0;
		}
		pgsql_row = pg_result->row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
	} else {
		pgsql_row = row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to jump to row %ld on PostgreSQL result index %ld",
							row, Z_LVAL_P(result));
			RETURN_FALSE;
		}
	}
	
	switch(Z_TYPE_PP(field)) {
		case IS_STRING:
			convert_to_string_ex(field);
			field_offset = PQfnumber(pgsql_result, Z_STRVAL_PP(field));
			break;
		default:
			convert_to_long_ex(field);
			field_offset = Z_LVAL_PP(field);
			break;
	}
	if (field_offset < 0 || field_offset >= PQnfields(pgsql_result)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_PG_DATA_LENGTH:
			Z_LVAL_P(return_value) = PQgetlength(pgsql_result, pgsql_row, field_offset);
			break;
		case PHP_PG_DATA_ISNULL:
			Z_LVAL_P(return_value) = PQgetisnull(pgsql_result, pgsql_row, field_offset);
			break;
	}
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_field_prtlen(resource result, [int row,] mixed field_name_or_number)
   Returns the printed length */
PHP_FUNCTION(pg_field_prtlen)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_LENGTH);
}
/* }}} */

/* {{{ proto int pg_field_is_null(resource result, [int row,] mixed field_name_or_number)
   Test if a field is NULL */
PHP_FUNCTION(pg_field_is_null)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_ISNULL);
}
/* }}} */

/* {{{ proto bool pg_free_result(resource result)
   Free result memory */
PHP_FUNCTION(pg_free_result)
{
	zval *result;
	pgsql_result_handle *pg_result;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);
	if (Z_LVAL_P(result) == 0) {
		RETURN_FALSE;
	}
	zend_list_delete(Z_RESVAL_P(result));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string pg_last_oid(resource result)
   Returns the last object identifier */
PHP_FUNCTION(pg_last_oid)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
#ifdef HAVE_PQOIDVALUE
	Oid oid;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);
	pgsql_result = pg_result->result;
#ifdef HAVE_PQOIDVALUE
	oid = PQoidValue(pgsql_result);
	if (oid == InvalidOid) {
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(oid);
#else
	Z_STRVAL_P(return_value) = (char *) PQoidStatus(pgsql_result);
	if (Z_STRVAL_P(return_value)) {
		RETURN_STRING(Z_STRVAL_P(return_value), 1);
	}
	RETURN_STRING("", 1);
#endif
}
/* }}} */

/* {{{ proto bool pg_trace(string filename [, string mode [, resource connection]])
   Enable tracing a PostgreSQL connection */
PHP_FUNCTION(pg_trace)
{
	char *z_filename, *mode = "w";
	int z_filename_len, mode_len;
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	FILE *fp = NULL;
	php_stream *stream;
	id = PGG(default_link);
	
	if (zend_parse_parameters(argc TSRMLS_CC, "s|sr", &z_filename, &z_filename_len, &mode, &mode_len, &pgsql_link) == FAILURE) {
		return;
	}

	if (argc < 3) {
		CHECK_DEFAULT_LINK(id);
	}		

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	stream = php_stream_open_wrapper(z_filename, mode, REPORT_ERRORS, NULL);

	if (!stream) {
		RETURN_FALSE;
	}

	if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)&fp, REPORT_ERRORS))	{
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_auto_cleanup(stream);
	PQtrace(pgsql, fp);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pg_untrace([resource connection])
   Disable tracing of a PostgreSQL connection */
PHP_FUNCTION(pg_untrace)
{
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	
	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &pgsql_link) == FAILURE) {
		return;
	}

	if (argc == 0) { 
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	PQuntrace(pgsql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed pg_lo_create([resource connection],[mixed large_object_oid])
   Create a large object */
PHP_FUNCTION(pg_lo_create)
{
  	zval *pgsql_link = NULL, *oid = NULL;
  	PGconn *pgsql;
  	Oid pgsql_oid, wanted_oid = InvalidOid;
  	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "|zz", &pgsql_link, &oid) == FAILURE) {
		return;
	}

	if ((argc == 1) && (Z_TYPE_P(pgsql_link) != IS_RESOURCE)) {
		oid = pgsql_link;
		pgsql_link = NULL;
	}
	
	if (pgsql_link == NULL) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
		if (id == -1) {
			RETURN_FALSE;
		}
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	if (oid) {
#ifndef HAVE_PG_LO_CREATE	
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "OID value passing not supported");
#else
		switch (Z_TYPE_P(oid)) {
		case IS_STRING:
			{	
				char *end_ptr;
				wanted_oid = (Oid)strtoul(Z_STRVAL_P(oid), &end_ptr, 10);
				if ((Z_STRVAL_P(oid)+Z_STRLEN_P(oid)) != end_ptr) {
				/* wrong integer format */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "invalid OID value passed");
				RETURN_FALSE;
				}
			}
			break;
		case IS_LONG:
			if (Z_LVAL_P(oid) < (long)InvalidOid) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "invalid OID value passed");
				RETURN_FALSE;
			}
			wanted_oid = (Oid)Z_LVAL_P(oid);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "invalid OID value passed");
			RETURN_FALSE;
        }
		if ((pgsql_oid = lo_create(pgsql, wanted_oid)) == InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create PostgreSQL large object");
			RETURN_FALSE;
		}

		PGSQL_RETURN_OID(pgsql_oid);	
#endif
	}

	if ((pgsql_oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == InvalidOid) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create PostgreSQL large object");
		RETURN_FALSE;
	}

	PGSQL_RETURN_OID(pgsql_oid);	
}
/* }}} */

/* {{{ proto bool pg_lo_unlink([resource connection,] string large_object_oid)
   Delete a large object */
PHP_FUNCTION(pg_lo_unlink)
{
	zval *pgsql_link = NULL;
	long oid_long;
	char *oid_string, *end_ptr;
	int oid_strlen;
	PGconn *pgsql;
	Oid oid;
	int id = -1;
	int argc = ZEND_NUM_ARGS();

	/* accept string type since Oid type is unsigned int */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rs", &pgsql_link, &oid_string, &oid_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rl", &pgsql_link, &oid_long) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "s", &oid_string, &oid_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "l", &oid_long) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires 1 or 2 arguments");
		RETURN_FALSE;
	}
	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (lo_unlink(pgsql, oid) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to delete PostgreSQL large object %u", oid);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource pg_lo_open([resource connection,] int large_object_oid, string mode)
   Open a large object and return fd */
PHP_FUNCTION(pg_lo_open)
{
	zval *pgsql_link = NULL;
	long oid_long;
	char *oid_string, *end_ptr, *mode_string;
	int oid_strlen, mode_strlen;
	PGconn *pgsql;
	Oid oid;
	int id = -1, pgsql_mode=0, pgsql_lofd;
	int create=0;
	pgLofp *pgsql_lofp;
	int argc = ZEND_NUM_ARGS();

	/* accept string type since Oid is unsigned int */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rss", &pgsql_link, &oid_string, &oid_strlen, &mode_string, &mode_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rls", &pgsql_link, &oid_long, &mode_string, &mode_strlen) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "ss", &oid_string, &oid_strlen, &mode_string, &mode_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "ls", &oid_long, &mode_string, &mode_strlen) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires 1 or 2 arguments");
		RETURN_FALSE;
	}
	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	/* r/w/+ is little bit more PHP-like than INV_READ/INV_WRITE and a lot of
	   faster to type. Unfortunately, doesn't behave the same way as fopen()...
	   (Jouni)
	*/

	if (strchr(mode_string, 'r') == mode_string) {
		pgsql_mode |= INV_READ;
		if (strchr(mode_string, '+') == mode_string+1) {
			pgsql_mode |= INV_WRITE;
		}
	}
	if (strchr(mode_string, 'w') == mode_string) {
		pgsql_mode |= INV_WRITE;
		create = 1;
		if (strchr(mode_string, '+') == mode_string+1) {
			pgsql_mode |= INV_READ;
		}
	}

	pgsql_lofp = (pgLofp *) emalloc(sizeof(pgLofp));

	if ((pgsql_lofd = lo_open(pgsql, oid, pgsql_mode)) == -1) {
		if (create) {
			if ((oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == 0) {
				efree(pgsql_lofp);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create PostgreSQL large object");
				RETURN_FALSE;
			} else {
				if ((pgsql_lofd = lo_open(pgsql, oid, pgsql_mode)) == -1) {
					if (lo_unlink(pgsql, oid) == -1) {
						efree(pgsql_lofp);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Something is really messed up! Your database is badly corrupted in a way NOT related to PHP");
						RETURN_FALSE;
					}
					efree(pgsql_lofp);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open PostgreSQL large object");
					RETURN_FALSE;
				} else {
					pgsql_lofp->conn = pgsql;
					pgsql_lofp->lofd = pgsql_lofd;
					Z_LVAL_P(return_value) = zend_list_insert(pgsql_lofp, le_lofp TSRMLS_CC);
					Z_TYPE_P(return_value) = IS_LONG;
				}
			}
		} else {
			efree(pgsql_lofp);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open PostgreSQL large object");
			RETURN_FALSE;
		}
	} else {
		pgsql_lofp->conn = pgsql;
		pgsql_lofp->lofd = pgsql_lofd;
		ZEND_REGISTER_RESOURCE(return_value, pgsql_lofp, le_lofp);
	}
}
/* }}} */

/* {{{ proto bool pg_lo_close(resource large_object)
   Close a large object */
PHP_FUNCTION(pg_lo_close)
{
	zval *pgsql_lofp;
	pgLofp *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pgsql_lofp) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_lofp, -1, "PostgreSQL large object", le_lofp);
	
	if (lo_close((PGconn *)pgsql->conn, pgsql->lofd) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to close PostgreSQL large object descriptor %d", pgsql->lofd);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

	zend_list_delete(Z_RESVAL_P(pgsql_lofp));
	return;
}
/* }}} */

#define PGSQL_LO_READ_BUF_SIZE  8192

/* {{{ proto string pg_lo_read(resource large_object [, int len])
   Read a large object */
PHP_FUNCTION(pg_lo_read)
{
  	zval *pgsql_id;
  	long len;
	int buf_len = PGSQL_LO_READ_BUF_SIZE, nbytes, argc = ZEND_NUM_ARGS();
	char *buf;
	pgLofp *pgsql;

	if (zend_parse_parameters(argc TSRMLS_CC, "r|l", &pgsql_id, &len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	if (argc > 1) {
		buf_len = len;
	}
	
	buf = (char *) safe_emalloc(sizeof(char), (buf_len+1), 0);
	if ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, buf_len))<0) {
		efree(buf);
		RETURN_FALSE;
	}

	buf[nbytes] = '\0';
	RETURN_STRINGL(buf, nbytes, 0);
}
/* }}} */

/* {{{ proto int pg_lo_write(resource large_object, string buf [, int len])
   Write a large object */
PHP_FUNCTION(pg_lo_write)
{
  	zval *pgsql_id;
  	char *str;
  	long z_len;
	int str_len, nbytes;
	int len;
	pgLofp *pgsql;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs|l", &pgsql_id, &str, &str_len, &z_len) == FAILURE) {
		return;
	}

	if (argc > 2) {
		if (z_len > str_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot write more than buffer size %d. Tried to write %ld", str_len, z_len);
			RETURN_FALSE;
		}
		if (z_len < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Buffer size must be larger than 0, but %ld was specified", z_len);
			RETURN_FALSE;
		}
		len = z_len;
	}
	else {
		len = str_len;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	if ((nbytes = lo_write((PGconn *)pgsql->conn, pgsql->lofd, str, len)) == -1) {
		RETURN_FALSE;
	}

	RETURN_LONG(nbytes);
}
/* }}} */

/* {{{ proto int pg_lo_read_all(resource large_object)
   Read a large object and send straight to browser */
PHP_FUNCTION(pg_lo_read_all)
{
  	zval *pgsql_id;
	int tbytes;
	volatile int nbytes;
	char buf[PGSQL_LO_READ_BUF_SIZE];
	pgLofp *pgsql;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pgsql_id) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	tbytes = 0;
	while ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, PGSQL_LO_READ_BUF_SIZE))>0) {
		PHPWRITE(buf, nbytes);
		tbytes += nbytes;
	}
	RETURN_LONG(tbytes);
}
/* }}} */

/* {{{ proto int pg_lo_import([resource connection, ] string filename [, mixed oid])
   Import large object direct from filesystem */
PHP_FUNCTION(pg_lo_import)
{
	zval *pgsql_link = NULL, *oid = NULL;
	char *file_in;
	int id = -1, name_len;
	int argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	Oid returned_oid;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rp|z", &pgsql_link, &file_in, &name_len, &oid) == SUCCESS) {
		;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									  "p|z", &file_in, &name_len, &oid) == SUCCESS) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	/* old calling convention, deprecated since PHP 4.2 */
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									  "pr", &file_in, &name_len, &pgsql_link ) == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Old API is used");
	}
	else {
		WRONG_PARAM_COUNT;
	}
	
	if (php_check_open_basedir(file_in TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (oid) {
#ifndef HAVE_PG_LO_IMPORT_WITH_OID
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "OID value passing not supported");
#else
		Oid wanted_oid;
		switch (Z_TYPE_P(oid)) {
		case IS_STRING:
			{	
				char *end_ptr;
				wanted_oid = (Oid)strtoul(Z_STRVAL_P(oid), &end_ptr, 10);
				if ((Z_STRVAL_P(oid)+Z_STRLEN_P(oid)) != end_ptr) {
				/* wrong integer format */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "invalid OID value passed");
				RETURN_FALSE;
				}
			}
			break;
		case IS_LONG:
			if (Z_LVAL_P(oid) < (long)InvalidOid) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "invalid OID value passed");
				RETURN_FALSE;
			}
			wanted_oid = (Oid)Z_LVAL_P(oid);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "invalid OID value passed");
			RETURN_FALSE;
        }

       returned_oid = lo_import_with_oid(pgsql, file_in, wanted_oid);

	   if (returned_oid == InvalidOid) {
		   RETURN_FALSE;
	   }

	   PGSQL_RETURN_OID(returned_oid);
#endif
	}

	returned_oid = lo_import(pgsql, file_in);

	if (returned_oid == InvalidOid) {
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(returned_oid);
}
/* }}} */

/* {{{ proto bool pg_lo_export([resource connection, ] int objoid, string filename)
   Export large object direct to filesystem */
PHP_FUNCTION(pg_lo_export)
{
	zval *pgsql_link = NULL;
	char *file_out, *oid_string, *end_ptr;
	int oid_strlen;
	int id = -1, name_len;
	long oid_long;
	Oid oid;
	PGconn *pgsql;
	int argc = ZEND_NUM_ARGS();

	/* allow string to handle large OID value correctly */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rlp", &pgsql_link, &oid_long, &file_out, &name_len) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rss", &pgsql_link, &oid_string, &oid_strlen, &file_out, &name_len) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									  "lp",  &oid_long, &file_out, &name_len) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "sp", &oid_string, &oid_strlen, &file_out, &name_len) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "spr", &oid_string, &oid_strlen, &file_out, &name_len, &pgsql_link) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									  "lpr", &oid_long, &file_out, &name_len, &pgsql_link) == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Old API is used");
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires 2 or 3 arguments");
		RETURN_FALSE;
	}
	
	if (php_check_open_basedir(file_out TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (lo_export(pgsql, oid, file_out)) {
		RETURN_TRUE;
	} 
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool pg_lo_seek(resource large_object, int offset [, int whence])
   Seeks position of large object */
PHP_FUNCTION(pg_lo_seek)
{
	zval *pgsql_id = NULL;
	long offset = 0, whence = SEEK_CUR;
	pgLofp *pgsql;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rl|l", &pgsql_id, &offset, &whence) == FAILURE) {
		return;
	}
	if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid whence parameter");
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	if (lo_lseek((PGconn *)pgsql->conn, pgsql->lofd, offset, whence) > -1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int pg_lo_tell(resource large_object)
   Returns current position of large object */
PHP_FUNCTION(pg_lo_tell)
{
	zval *pgsql_id = NULL;
	int offset = 0;
	pgLofp *pgsql;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &pgsql_id) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	offset = lo_tell((PGconn *)pgsql->conn, pgsql->lofd);
	RETURN_LONG(offset);
}
/* }}} */

#if HAVE_PQSETERRORVERBOSITY
/* {{{ proto int pg_set_error_verbosity([resource connection,] int verbosity)
   Set error verbosity */
PHP_FUNCTION(pg_set_error_verbosity)
{
	zval *pgsql_link = NULL;
	long verbosity;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;

	if (argc == 1) {
		if (zend_parse_parameters(argc TSRMLS_CC, "l", &verbosity) == FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rl", &pgsql_link, &verbosity) == FAILURE) {
			return;
		}
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (verbosity & (PQERRORS_TERSE|PQERRORS_DEFAULT|PQERRORS_VERBOSE)) {
		Z_LVAL_P(return_value) = PQsetErrorVerbosity(pgsql, verbosity);
		Z_TYPE_P(return_value) = IS_LONG;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

#ifdef HAVE_PQCLIENTENCODING
/* {{{ proto int pg_set_client_encoding([resource connection,] string encoding)
   Set client encoding */
PHP_FUNCTION(pg_set_client_encoding)
{
	char *encoding;
	int encoding_len;
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;

	if (argc == 1) {
		if (zend_parse_parameters(argc TSRMLS_CC, "s", &encoding, &encoding_len) == FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rs", &pgsql_link, &encoding, &encoding_len) == FAILURE) {
			return;
		}
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	Z_LVAL_P(return_value) = PQsetClientEncoding(pgsql, encoding);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto string pg_client_encoding([resource connection])
   Get the current client encoding */
PHP_FUNCTION(pg_client_encoding)
{
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;

	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &pgsql_link) == FAILURE) {
		return;
	}
	
	if (argc == 0) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	/* Just do the same as found in PostgreSQL sources... */

#ifndef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
#define pg_encoding_to_char(x) "SQL_ASCII"
#endif

	Z_STRVAL_P(return_value) = (char *) pg_encoding_to_char(PQclientEncoding(pgsql));
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_STRVAL_P(return_value) = (char *) estrdup(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */
#endif

#if !HAVE_PQGETCOPYDATA
#define	COPYBUFSIZ	8192
#endif

/* {{{ proto bool pg_end_copy([resource connection])
   Sync with backend. Completes the Copy command */
PHP_FUNCTION(pg_end_copy)
{
	zval *pgsql_link = NULL;
	int id = -1, argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	int result = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "|r", &pgsql_link) == FAILURE) {
		return;
	}
	
	if (argc == 0) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	result = PQendcopy(pgsql);

	if (result!=0) {
		PHP_PQ_ERROR("Query failed: %s", pgsql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool pg_put_line([resource connection,] string query)
   Send null-terminated string to backend server*/
PHP_FUNCTION(pg_put_line)
{
	char *query;
	zval *pgsql_link = NULL;
	int query_len, id = -1;
	PGconn *pgsql;
	int result = 0, argc = ZEND_NUM_ARGS();

	if (argc == 1) {
		if (zend_parse_parameters(argc TSRMLS_CC, "s", &query, &query_len) == FAILURE) {
			return;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	} else {
		if (zend_parse_parameters(argc TSRMLS_CC, "rs", &pgsql_link, &query, &query_len) == FAILURE) {
			return;
		}
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	result = PQputline(pgsql, query);
	if (result==EOF) {
		PHP_PQ_ERROR("Query failed: %s", pgsql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array pg_copy_to(resource connection, string table_name [, string delimiter [, string null_as]])
   Copy table to array */
PHP_FUNCTION(pg_copy_to)
{
	zval *pgsql_link;
	char *table_name, *pg_delim = NULL, *pg_null_as = NULL;
	int table_name_len, pg_delim_len, pg_null_as_len, free_pg_null = 0;
	char *query;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	int copydone = 0;
#if !HAVE_PQGETCOPYDATA
	char copybuf[COPYBUFSIZ];
#endif
	char *csv = (char *)NULL;
	int ret;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs|ss",
							  &pgsql_link, &table_name, &table_name_len,
							  &pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len) == FAILURE) {
		return;
	}
	if (!pg_delim) {
		pg_delim = "\t";
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (!pg_null_as) {
		pg_null_as = safe_estrdup("\\\\N");
		free_pg_null = 1;
	}

	spprintf(&query, 0, "COPY %s TO STDOUT DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, *pg_delim, pg_null_as);

	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(pgsql, query);
	if (free_pg_null) {
		efree(pg_null_as);
	}
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_COPY_OUT:
			if (pgsql_result) {
				PQclear(pgsql_result);
				array_init(return_value);
#if HAVE_PQGETCOPYDATA
				while (!copydone)
				{
					ret = PQgetCopyData(pgsql, &csv, 0);
					switch (ret) {
						case -1:
							copydone = 1;
							break;
						case 0:
						case -2:
							PHP_PQ_ERROR("getline failed: %s", pgsql);
							RETURN_FALSE;
							break;
						default:
							add_next_index_string(return_value, csv, 1);
							PQfreemem(csv);
							break;
					}
				}
#else
				while (!copydone)
				{
					if ((ret = PQgetline(pgsql, copybuf, COPYBUFSIZ))) {
						PHP_PQ_ERROR("getline failed: %s", pgsql);
						RETURN_FALSE;
					}
			
					if (copybuf[0] == '\\' &&
						copybuf[1] == '.' &&
						copybuf[2] == '\0')
					{
						copydone = 1;
					}
					else
					{
						if (csv == (char *)NULL) {
							csv = estrdup(copybuf);
						} else {
							csv = (char *)erealloc(csv, strlen(csv) + sizeof(char)*(COPYBUFSIZ+1));
							strcat(csv, copybuf);
						}
							
						switch (ret)
						{
							case EOF:
								copydone = 1;
							case 0:
								add_next_index_string(return_value, csv, 1);
								efree(csv);
								csv = (char *)NULL;
								break;
							case 1:
								break;
						}
					}
				}
				if (PQendcopy(pgsql)) {
					PHP_PQ_ERROR("endcopy failed: %s", pgsql);
					RETURN_FALSE;
				}
#endif
				while ((pgsql_result = PQgetResult(pgsql))) {
					PQclear(pgsql_result);
				}
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
		default:
			PQclear(pgsql_result);
			PHP_PQ_ERROR("Copy command failed: %s", pgsql);
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto bool pg_copy_from(resource connection, string table_name , array rows [, string delimiter [, string null_as]])
   Copy table from array */
PHP_FUNCTION(pg_copy_from)
{
	zval *pgsql_link = NULL, *pg_rows;
	zval **tmp;
	char *table_name, *pg_delim = NULL, *pg_null_as = NULL;
	int  table_name_len, pg_delim_len, pg_null_as_len;
	int  pg_null_as_free = 0;
	char *query;
	HashPosition pos;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|ss",
							  &pgsql_link, &table_name, &table_name_len, &pg_rows,
							  &pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len) == FAILURE) {
		return;
	}
	if (!pg_delim) {
		pg_delim = "\t";
	}
	if (!pg_null_as) {
		pg_null_as = safe_estrdup("\\\\N");
		pg_null_as_free = 1;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	spprintf(&query, 0, "COPY %s FROM STDIN DELIMITERS E'%c' WITH NULL AS E'%s'", table_name, *pg_delim, pg_null_as);
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(pgsql, query);

	if (pg_null_as_free) {
		efree(pg_null_as);
	}
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_COPY_IN:
			if (pgsql_result) {
				int command_failed = 0;
				PQclear(pgsql_result);
				zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pg_rows), &pos);
#if HAVE_PQPUTCOPYDATA
				while (zend_hash_get_current_data_ex(Z_ARRVAL_P(pg_rows), (void **) &tmp, &pos) == SUCCESS) {
					convert_to_string_ex(tmp);
					query = (char *)emalloc(Z_STRLEN_PP(tmp) + 2);
					strlcpy(query, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp) + 2);
					if(Z_STRLEN_PP(tmp) > 0 && *(query + Z_STRLEN_PP(tmp) - 1) != '\n') {
						strlcat(query, "\n", Z_STRLEN_PP(tmp) + 2);
					}
					if (PQputCopyData(pgsql, query, strlen(query)) != 1) {
						efree(query);
						PHP_PQ_ERROR("copy failed: %s", pgsql);
						RETURN_FALSE;
					}
					efree(query);
					zend_hash_move_forward_ex(Z_ARRVAL_P(pg_rows), &pos);
				}
				if (PQputCopyEnd(pgsql, NULL) != 1) {
					PHP_PQ_ERROR("putcopyend failed: %s", pgsql);
					RETURN_FALSE;
				}
#else
				while (zend_hash_get_current_data_ex(Z_ARRVAL_P(pg_rows), (void **) &tmp, &pos) == SUCCESS) {
					convert_to_string_ex(tmp);
					query = (char *)emalloc(Z_STRLEN_PP(tmp) + 2);
					strlcpy(query, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp) + 2);
					if(Z_STRLEN_PP(tmp) > 0 && *(query + Z_STRLEN_PP(tmp) - 1) != '\n') {
						strlcat(query, "\n", Z_STRLEN_PP(tmp) + 2);
					}
					if (PQputline(pgsql, query)==EOF) {
						efree(query);
						PHP_PQ_ERROR("copy failed: %s", pgsql);
						RETURN_FALSE;
					}
					efree(query);
					zend_hash_move_forward_ex(Z_ARRVAL_P(pg_rows), &pos);
				}
				if (PQputline(pgsql, "\\.\n") == EOF) {
					PHP_PQ_ERROR("putline failed: %s", pgsql);
					RETURN_FALSE;
				}
				if (PQendcopy(pgsql)) {
					PHP_PQ_ERROR("endcopy failed: %s", pgsql);
					RETURN_FALSE;
				}
#endif
				while ((pgsql_result = PQgetResult(pgsql))) {
					if (PGRES_COMMAND_OK != PQresultStatus(pgsql_result)) {
						PHP_PQ_ERROR("Copy command failed: %s", pgsql);
						command_failed = 1;
					}
					PQclear(pgsql_result);
				}
				if (command_failed) {
					RETURN_FALSE;
				}
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			RETURN_TRUE;
			break;
		default:
			PQclear(pgsql_result);
			PHP_PQ_ERROR("Copy command failed: %s", pgsql);
			RETURN_FALSE;
			break;
	}
}
/* }}} */

#ifdef HAVE_PQESCAPE
/* {{{ proto string pg_escape_string([resource connection,] string data)
   Escape string for text/char type */
PHP_FUNCTION(pg_escape_string)
{
	char *from = NULL, *to = NULL;
	zval *pgsql_link;
#ifdef HAVE_PQESCAPE_CONN
	PGconn *pgsql;
#endif
	int to_len;
	int from_len;
	int id = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &from, &from_len) == FAILURE) {
				return;
			}
			pgsql_link = NULL;
			id = PGG(default_link);
			break;

		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pgsql_link, &from, &from_len) == FAILURE) {
				return;
			}
			break;
	}

	to = (char *) safe_emalloc(from_len, 2, 1);

#ifdef HAVE_PQESCAPE_CONN
	if (pgsql_link != NULL || id != -1) {
		ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
		to_len = (int) PQescapeStringConn(pgsql, to, from, (size_t)from_len, NULL);
	} else 	
#endif
		to_len = (int) PQescapeString(to, from, (size_t)from_len);

	RETURN_STRINGL(to, to_len, 0);
}
/* }}} */

/* {{{ proto string pg_escape_bytea([resource connection,] string data)
   Escape binary for bytea type  */
PHP_FUNCTION(pg_escape_bytea)
{
	char *from = NULL, *to = NULL;
	size_t to_len;
	int from_len, id = -1;
#ifdef HAVE_PQESCAPE_BYTEA_CONN
	PGconn *pgsql;
#endif
	zval *pgsql_link;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &from, &from_len) == FAILURE) {
				return;
			}
			pgsql_link = NULL;
			id = PGG(default_link);
			break;

		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pgsql_link, &from, &from_len) == FAILURE) {
				return;
			}
			break;
	}

#ifdef HAVE_PQESCAPE_BYTEA_CONN
	if (pgsql_link != NULL || id != -1) {
		ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
		to = (char *)PQescapeByteaConn(pgsql, from, (size_t)from_len, &to_len);
	} else
#endif
		to = (char *)PQescapeBytea((unsigned char*)from, from_len, &to_len);

	RETVAL_STRINGL(to, to_len-1, 1); /* to_len includes addtional '\0' */
	PQfreemem(to);
}
/* }}} */

#if !HAVE_PQUNESCAPEBYTEA
/* PQunescapeBytea() from PostgreSQL 7.3 to provide bytea unescape feature to 7.2 users.
   Renamed to php_pgsql_unescape_bytea() */
/*
 *		PQunescapeBytea - converts the null terminated string representation
 *		of a bytea, strtext, into binary, filling a buffer. It returns a
 *		pointer to the buffer which is NULL on error, and the size of the
 *		buffer in retbuflen. The pointer may subsequently be used as an
 *		argument to the function free(3). It is the reverse of PQescapeBytea.
 *
 *		The following transformations are reversed:
 *		'\0' == ASCII  0 == \000
 *		'\'' == ASCII 39 == \'
 *		'\\' == ASCII 92 == \\
 *
 *		States:
 *		0	normal		0->1->2->3->4
 *		1	\			   1->5
 *		2	\0			   1->6
 *		3	\00
 *		4	\000
 *		5	\'
 *		6	\\
 */
static unsigned char * php_pgsql_unescape_bytea(unsigned char *strtext, size_t *retbuflen)
{
	size_t		buflen;
	unsigned char *buffer,
			   *sp,
			   *bp;
	unsigned int state = 0;

	if (strtext == NULL)
		return NULL;
	buflen = strlen(strtext);	/* will shrink, also we discover if
								 * strtext */
	buffer = (unsigned char *) emalloc(buflen);	/* isn't NULL terminated */
	for (bp = buffer, sp = strtext; *sp != '\0'; bp++, sp++)
	{
		switch (state)
		{
			case 0:
				if (*sp == '\\')
					state = 1;
				*bp = *sp;
				break;
			case 1:
				if (*sp == '\'')	/* state=5 */
				{				/* replace \' with 39 */
					bp--;
					*bp = '\'';
					buflen--;
					state = 0;
				}
				else if (*sp == '\\')	/* state=6 */
				{				/* replace \\ with 92 */
					bp--;
					*bp = '\\';
					buflen--;
					state = 0;
				}
				else
				{
					if (isdigit(*sp))
						state = 2;
					else
						state = 0;
					*bp = *sp;
				}
				break;
			case 2:
				if (isdigit(*sp))
					state = 3;
				else
					state = 0;
				*bp = *sp;
				break;
			case 3:
				if (isdigit(*sp))		/* state=4 */
				{
					unsigned char *start, *end, buf[4]; /* 000 + '\0' */
					
					bp -= 3;
					memcpy(buf, sp-2, 3);
					buf[3] = '\0';
					start = buf;
 					*bp = (unsigned char)strtoul(start, (char **)&end, 8);
					buflen -= 3;
					state = 0;
				}
				else
				{
					*bp = *sp;
					state = 0;
				}
				break;
		}
	}
	buffer = erealloc(buffer, buflen+1);
	buffer[buflen] = '\0';

	*retbuflen = buflen;
	return buffer;
}
#endif

/* {{{ proto string pg_unescape_bytea(string data)
   Unescape binary for bytea type  */
PHP_FUNCTION(pg_unescape_bytea)
{
	char *from = NULL, *to = NULL, *tmp = NULL;
	size_t to_len;
	int from_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							  &from, &from_len) == FAILURE) {
		return;
	}

#if HAVE_PQUNESCAPEBYTEA
	tmp = (char *)PQunescapeBytea((unsigned char*)from, &to_len);
	to = estrndup(tmp, to_len);
	PQfreemem(tmp);
#else
	to = (char *)php_pgsql_unescape_bytea((unsigned char*)from, &to_len);
#endif
	if (!to) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(to, to_len, 0);
}
/* }}} */
#endif

#ifdef HAVE_PQESCAPE
#if !HAVE_PQESCAPELITERAL
/* emulate libpq's PQescapeInternal() 9.0 or later */
static char* php_pgsql_PQescapeInternal(PGconn *conn, const char *str, size_t len, int escape_literal) {
	char *result, *rp;
	const char *s;
	size_t tmp_len;
	int input_len = len;
	char quote_char = escape_literal ? '\'' : '"';

	if (!conn) {
		return NULL;
	}

	/*
	 * NOTE: multibyte strings that could cointain slashes should be considered.
	 * (e.g. SJIS, BIG5) However, it cannot be done without valid PGconn and mbstring. 
	 * Therefore, this function does not support such encodings currently.
	 * FIXME: add encoding check and skip multibyte char bytes if there is vaild PGconn.
	 */

	/* allocate enough memory */
	rp = result = (char *)emalloc(len*2 + 5); /* leading " E" needs extra 2 bytes + quote_chars on both end for 2 bytes + NULL */

	if (escape_literal) {
		/* check backslashes */
		tmp_len = strspn(str, "\\");
		if (tmp_len != len) {
			/* add " E" for escaping slashes */
			*rp++ = ' ';
			*rp++ = 'E';
		}
	}
	/* open quote */
	*rp++ = quote_char;
	for (s = str; s - str < input_len; ++s) {
		if (*s == quote_char || (escape_literal && *s == '\\')) {
			*rp++ = *s;
			*rp++ = *s;
		} else {
			*rp++ = *s;
		}
	}
	*rp++ = quote_char;
	*rp = '\0';
	
	return result;
}
#endif

static void php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAMETERS, int escape_literal) {
	char *from = NULL, *to = NULL, *tmp = NULL;
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	int to_len;
	int from_len;
	int id = -1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &from, &from_len) == FAILURE) {
				return;
			}
			pgsql_link = NULL;
			id = PGG(default_link);
			break;

		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pgsql_link, &from, &from_len) == FAILURE) {
				return;
			}
			break;
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	if (pgsql == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Cannot get default pgsql link");
		RETURN_FALSE;
	}
#ifdef HAVE_PQESCAPELITERAL
	if (escape_literal) {
		tmp = PQescapeLiteral(pgsql, from, (size_t)from_len);
	} else {
		tmp = PQescapeIdentifier(pgsql, from, (size_t)from_len);
	}
	if (!tmp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Failed to escape");
		RETURN_FALSE;
	}
	to = estrdup(tmp);
	PQfreemem(tmp);
#else 
	to = php_pgsql_PQescapeInternal(pgsql, from, (size_t)from_len, escape_literal);
	if (!to) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Failed to escape");
		RETURN_FALSE;
	}
#endif

	RETURN_STRING(to, 0);
}

/* {{{ proto string pg_escape_literal([resource connection,] string data)
   Escape parameter as string literal (i.e. parameter)	*/
PHP_FUNCTION(pg_escape_literal)
{
	php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string pg_escape_identifier([resource connection,] string data)
   Escape identifier (i.e. table name, field name)	*/
PHP_FUNCTION(pg_escape_identifier)
{
	php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */
#endif


/* {{{ proto string pg_result_error(resource result)
   Get error message associated with result */
PHP_FUNCTION(pg_result_error)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	char *err = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &result) == FAILURE) {
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (!pgsql_result) {
		RETURN_FALSE;
	}
	err = (char *)PQresultErrorMessage(pgsql_result);
	RETURN_STRING(err,1);
}
/* }}} */

#if HAVE_PQRESULTERRORFIELD
/* {{{ proto string pg_result_error_field(resource result, int fieldcode)
   Get error message field associated with result */
PHP_FUNCTION(pg_result_error_field)
{
	zval *result;
	long fieldcode;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	char *field = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "rl",
								 &result, &fieldcode) == FAILURE) {
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (!pgsql_result) {
		RETURN_FALSE;
	}
	if (fieldcode & (PG_DIAG_SEVERITY|PG_DIAG_SQLSTATE|PG_DIAG_MESSAGE_PRIMARY|PG_DIAG_MESSAGE_DETAIL
				|PG_DIAG_MESSAGE_HINT|PG_DIAG_STATEMENT_POSITION
#if PG_DIAG_INTERNAL_POSITION
				|PG_DIAG_INTERNAL_POSITION
#endif
#if PG_DIAG_INTERNAL_QUERY
				|PG_DIAG_INTERNAL_QUERY
#endif
				|PG_DIAG_CONTEXT|PG_DIAG_SOURCE_FILE|PG_DIAG_SOURCE_LINE
				|PG_DIAG_SOURCE_FUNCTION)) {
		field = (char *)PQresultErrorField(pgsql_result, fieldcode);
		if (field == NULL) {
			RETURN_NULL();
		} else {
			RETURN_STRING(field, 1);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ proto int pg_connection_status(resource connection)
   Get connection status */
PHP_FUNCTION(pg_connection_status)
{
	zval *pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	RETURN_LONG(PQstatus(pgsql));
}

/* }}} */

#if HAVE_PGTRANSACTIONSTATUS
/* {{{ proto int pg_transaction_status(resource connection)
   Get transaction status */
PHP_FUNCTION(pg_transaction_status)
{
	zval *pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	RETURN_LONG(PQtransactionStatus(pgsql));
}
#endif

/* }}} */

/* {{{ proto bool pg_connection_reset(resource connection)
   Reset connection (reconnect) */
PHP_FUNCTION(pg_connection_reset)
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	PQreset(pgsql);
	if (PQstatus(pgsql) == CONNECTION_BAD) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

#define PHP_PG_ASYNC_IS_BUSY		1
#define PHP_PG_ASYNC_REQUEST_CANCEL 2
														  
/* {{{ php_pgsql_flush_query
 */
static int php_pgsql_flush_query(PGconn *pgsql TSRMLS_DC) 
{
	PGresult *res;
	int leftover = 0;
	
	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to nonblocking mode");
		return -1;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover++;
	}
	PQ_SETNONBLOCKING(pgsql, 0);
	return leftover;
}
/* }}} */
														  
/* {{{ php_pgsql_do_async
 */
static void php_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS, int entry_type) 
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	switch(entry_type) {
		case PHP_PG_ASYNC_IS_BUSY:
			PQconsumeInput(pgsql);
			Z_LVAL_P(return_value) = PQisBusy(pgsql);
			Z_TYPE_P(return_value) = IS_LONG;
			break;
		case PHP_PG_ASYNC_REQUEST_CANCEL:
			Z_LVAL_P(return_value) = PQrequestCancel(pgsql);
			Z_TYPE_P(return_value) = IS_LONG;
			while ((pgsql_result = PQgetResult(pgsql))) {
				PQclear(pgsql_result);
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "PostgreSQL module error, please report this error");
			break;
	}
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
	}
	convert_to_boolean_ex(&return_value);
}
/* }}} */

/* {{{ proto bool pg_cancel_query(resource connection)
   Cancel request */
PHP_FUNCTION(pg_cancel_query)
{
	php_pgsql_do_async(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_ASYNC_REQUEST_CANCEL);
}
/* }}} */

/* {{{ proto bool pg_connection_busy(resource connection)
   Get connection is busy or not */
PHP_FUNCTION(pg_connection_busy)
{
	php_pgsql_do_async(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_ASYNC_IS_BUSY);
}
/* }}} */

/* {{{ proto bool pg_send_query(resource connection, string query)
   Send asynchronous query */
PHP_FUNCTION(pg_send_query)
{
	zval *pgsql_link;
	char *query;
	int len;
	int id = -1;
	PGconn *pgsql;
	PGresult *res;
	int leftover = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
							  &pgsql_link, &query, &len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "There are results on this connection. Call pg_get_result() until it returns FALSE");
	}
	if (!PQsendQuery(pgsql, query)) {
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
			PQreset(pgsql);
		}
		if (!PQsendQuery(pgsql, query)) {
			RETURN_FALSE;
		}
	}
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
	}
	RETURN_TRUE;
}
/* }}} */

#if HAVE_PQSENDQUERYPARAMS
/* {{{ proto bool pg_send_query_params(resource connection, string query, array params)
   Send asynchronous parameterized query */
PHP_FUNCTION(pg_send_query_params)
{
	zval *pgsql_link, *pv_param_arr, **tmp;
	int num_params = 0;
	char **params = NULL;
	char *query;
	int query_len, id = -1;
	PGconn *pgsql;
	PGresult *res;
	int leftover = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsa/", &pgsql_link, &query, &query_len, &pv_param_arr) == FAILURE) {
		return;
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "There are results on this connection. Call pg_get_result() until it returns FALSE");
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(pv_param_arr));
	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);
		
		for(i = 0; i < num_params; i++) {
			if (zend_hash_get_current_data(Z_ARRVAL_P(pv_param_arr), (void **) &tmp) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error getting parameter");
				_php_pgsql_free_params(params, num_params);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zval tmp_val = **tmp;
				zval_copy_ctor(&tmp_val);
				convert_to_string(&tmp_val);
				if (Z_TYPE(tmp_val) != IS_STRING) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error converting parameter");
					zval_dtor(&tmp_val);
					_php_pgsql_free_params(params, num_params);
					RETURN_FALSE;
				}
				params[i] = estrndup(Z_STRVAL(tmp_val), Z_STRLEN(tmp_val));
				zval_dtor(&tmp_val);
			}

			zend_hash_move_forward(Z_ARRVAL_P(pv_param_arr));
		}
	}

	if (!PQsendQueryParams(pgsql, query, num_params, NULL, (const char * const *)params, NULL, NULL, 0)) {
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
			PQreset(pgsql);
		}
		if (!PQsendQueryParams(pgsql, query, num_params, NULL, (const char * const *)params, NULL, NULL, 0)) {
			_php_pgsql_free_params(params, num_params);
			RETURN_FALSE;
		}
	}
	_php_pgsql_free_params(params, num_params);
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
	}
	RETURN_TRUE;
}
/* }}} */
#endif

#if HAVE_PQSENDPREPARE
/* {{{ proto bool pg_send_prepare(resource connection, string stmtname, string query)
   Asynchronously prepare a query for future execution */
PHP_FUNCTION(pg_send_prepare)
{
	zval *pgsql_link;
	char *query, *stmtname;
	int stmtname_len, query_len, id = -1;
	PGconn *pgsql;
	PGresult *res;
	int leftover = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pgsql_link, &stmtname, &stmtname_len, &query, &query_len) == FAILURE) {
		return;
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}	

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "There are results on this connection. Call pg_get_result() until it returns FALSE");
	}
	if (!PQsendPrepare(pgsql, stmtname, query, 0, NULL)) {
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
			PQreset(pgsql);
		}
		if (!PQsendPrepare(pgsql, stmtname, query, 0, NULL)) {
			RETURN_FALSE;
		}
	}
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
	}
	RETURN_TRUE;
}
/* }}} */
#endif

#if HAVE_PQSENDQUERYPREPARED
/* {{{ proto bool pg_send_execute(resource connection, string stmtname, array params)
   Executes prevriously prepared stmtname asynchronously */
PHP_FUNCTION(pg_send_execute)
{
	zval *pgsql_link;
	zval *pv_param_arr, **tmp;
	int num_params = 0;
	char **params = NULL;
	char *stmtname;
	int stmtname_len, id = -1;
	PGconn *pgsql;
	PGresult *res;
	int leftover = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsa", &pgsql_link, &stmtname, &stmtname_len, &pv_param_arr) == FAILURE) {
		return;
	}

	if (pgsql_link == NULL && id == -1) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "There are results on this connection. Call pg_get_result() until it returns FALSE");
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(pv_param_arr));
	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);
		
		for(i = 0; i < num_params; i++) {
			if (zend_hash_get_current_data(Z_ARRVAL_P(pv_param_arr), (void **) &tmp) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error getting parameter");
				_php_pgsql_free_params(params, num_params);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zval tmp_val = **tmp;
				zval_copy_ctor(&tmp_val);
				convert_to_string(&tmp_val);
				if (Z_TYPE(tmp_val) != IS_STRING) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,"Error converting parameter");
					zval_dtor(&tmp_val);
					_php_pgsql_free_params(params, num_params);
					RETURN_FALSE;
				}
				params[i] = estrndup(Z_STRVAL(tmp_val), Z_STRLEN(tmp_val));
				zval_dtor(&tmp_val);
			}

			zend_hash_move_forward(Z_ARRVAL_P(pv_param_arr));
		}
	}

	if (!PQsendQueryPrepared(pgsql, stmtname, num_params, (const char * const *)params, NULL, NULL, 0)) {
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
			PQreset(pgsql);
		}
		if (!PQsendQueryPrepared(pgsql, stmtname, num_params, (const char * const *)params, NULL, NULL, 0)) {
			_php_pgsql_free_params(params, num_params);
			RETURN_FALSE;
		}
	}
	_php_pgsql_free_params(params, num_params);
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto resource pg_get_result(resource connection)
   Get asynchronous query result */
PHP_FUNCTION(pg_get_result)
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r", &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	pgsql_result = PQgetResult(pgsql);
	if (!pgsql_result) {
		/* no result */
		RETURN_FALSE;
	}
	pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
	pg_result->conn = pgsql;
	pg_result->result = pgsql_result;
	pg_result->row = 0;
	ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
}
/* }}} */

/* {{{ proto mixed pg_result_status(resource result[, long result_type])
   Get status of query result */
PHP_FUNCTION(pg_result_status)
{
	zval *result;
	long result_type = PGSQL_STATUS_LONG;
	ExecStatusType status;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r|l",
								 &result, &result_type) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (result_type == PGSQL_STATUS_LONG) {
		status = PQresultStatus(pgsql_result);
		RETURN_LONG((int)status);
	}
	else if (result_type == PGSQL_STATUS_STRING) {
		RETURN_STRING(PQcmdStatus(pgsql_result), 1);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Optional 2nd parameter should be PGSQL_STATUS_LONG or PGSQL_STATUS_STRING");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto array pg_get_notify([resource connection[, result_type]])
   Get asynchronous notification */
PHP_FUNCTION(pg_get_notify)
{
	zval *pgsql_link;
	int id = -1;
	long result_type = PGSQL_ASSOC;
	PGconn *pgsql;
	PGnotify *pgsql_notify;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r|l",
								 &pgsql_link, &result_type) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (!(result_type & PGSQL_BOTH)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid result type");
		RETURN_FALSE;
	}

	PQconsumeInput(pgsql);
	pgsql_notify = PQnotifies(pgsql);
	if (!pgsql_notify) {
		/* no notify message */
		RETURN_FALSE;
	}
	array_init(return_value);
	if (result_type & PGSQL_NUM) {
		add_index_string(return_value, 0, pgsql_notify->relname, 1);
		add_index_long(return_value, 1, pgsql_notify->be_pid);
#if HAVE_PQPROTOCOLVERSION && HAVE_PQPARAMETERSTATUS 
		if (PQprotocolVersion(pgsql) >= 3 && atof(PQparameterStatus(pgsql, "server_version")) >= 9.0) {
#else 
		if (atof(PG_VERSION) >= 9.0) {
#endif 
#if HAVE_PQPARAMETERSTATUS
			add_index_string(return_value, 2, pgsql_notify->extra, 1);
#endif
		}
	}
	if (result_type & PGSQL_ASSOC) {
		add_assoc_string(return_value, "message", pgsql_notify->relname, 1);
		add_assoc_long(return_value, "pid", pgsql_notify->be_pid);
#if HAVE_PQPROTOCOLVERSION && HAVE_PQPARAMETERSTATUS 
		if (PQprotocolVersion(pgsql) >= 3 && atof(PQparameterStatus(pgsql, "server_version")) >= 9.0) {
#else 
		if (atof(PG_VERSION) >= 9.0) {
#endif 
#if HAVE_PQPARAMETERSTATUS
			add_assoc_string(return_value, "payload", pgsql_notify->extra, 1);
#endif
		}
	}
	PQfreemem(pgsql_notify);
}
/* }}} */

/* {{{ proto int pg_get_pid([resource connection)
   Get backend(server) pid */
PHP_FUNCTION(pg_get_pid)
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	RETURN_LONG(PQbackendPID(pgsql));
}
/* }}} */

/* {{{ php_pgsql_meta_data
 * TODO: Add meta_data cache for better performance
 */
PHP_PGSQL_API int php_pgsql_meta_data(PGconn *pg_link, const char *table_name, zval *meta TSRMLS_DC) 
{
	PGresult *pg_result;
	char *src, *tmp_name, *tmp_name2 = NULL;
	smart_str querystr = {0};
	int new_len;
	int i, num_rows;
	zval *elem;
	
	if (!*table_name) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The table name must be specified");
		return FAILURE;
	}

	src = estrdup(table_name);
	tmp_name = php_strtok_r(src, ".", &tmp_name2);
	
	if (!tmp_name2 || !*tmp_name2) {
		/* Default schema */
		tmp_name2 = tmp_name;
		tmp_name = "public";
	}

	smart_str_appends(&querystr, 
			"SELECT a.attname, a.attnum, t.typname, a.attlen, a.attnotNULL, a.atthasdef, a.attndims "
			"FROM pg_class as c, pg_attribute a, pg_type t, pg_namespace n "
			"WHERE a.attnum > 0 AND a.attrelid = c.oid AND c.relname = '");
	tmp_name2 = php_addslashes(tmp_name2, strlen(tmp_name2), &new_len, 0 TSRMLS_CC);
	smart_str_appendl(&querystr, tmp_name2, new_len);
	
	smart_str_appends(&querystr, "' AND c.relnamespace = n.oid AND n.nspname = '");
	tmp_name = php_addslashes(tmp_name, strlen(tmp_name), &new_len, 0 TSRMLS_CC);
	smart_str_appendl(&querystr, tmp_name, new_len);

	smart_str_appends(&querystr, "' AND a.atttypid = t.oid ORDER BY a.attnum;");
	smart_str_0(&querystr);
	
	efree(tmp_name2);
	efree(tmp_name);
	efree(src);	
	
	pg_result = PQexec(pg_link, querystr.c);
	if (PQresultStatus(pg_result) != PGRES_TUPLES_OK || (num_rows = PQntuples(pg_result)) == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Table '%s' doesn't exists", table_name);
		smart_str_free(&querystr);
		PQclear(pg_result);
		return FAILURE;
	}
	smart_str_free(&querystr);

	for (i = 0; i < num_rows; i++) {
		char *name;
		MAKE_STD_ZVAL(elem);
		array_init(elem);
		add_assoc_long(elem, "num", atoi(PQgetvalue(pg_result,i,1)));
		add_assoc_string(elem, "type", PQgetvalue(pg_result,i,2), 1);
		add_assoc_long(elem, "len", atoi(PQgetvalue(pg_result,i,3)));
		if (!strcmp(PQgetvalue(pg_result,i,4), "t")) {
			add_assoc_bool(elem, "not null", 1);
		}
		else {
			add_assoc_bool(elem, "not null", 0);
		}
		if (!strcmp(PQgetvalue(pg_result,i,5), "t")) {
			add_assoc_bool(elem, "has default", 1);
		}
		else {
			add_assoc_bool(elem, "has default", 0);
		}
		add_assoc_long(elem, "array dims", atoi(PQgetvalue(pg_result,i,6)));
		name = PQgetvalue(pg_result,i,0);
		add_assoc_zval(meta, name, elem);
	}
	PQclear(pg_result);
	
	return SUCCESS;
}

/* }}} */


/* {{{ proto array pg_meta_data(resource db, string table)
   Get meta_data */
PHP_FUNCTION(pg_meta_data)
{
	zval *pgsql_link;
	char *table_name;
	uint table_name_len;
	PGconn *pgsql;
	int id = -1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
							  &pgsql_link, &table_name, &table_name_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	array_init(return_value);
	if (php_pgsql_meta_data(pgsql, table_name, return_value TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value); /* destroy array */
		RETURN_FALSE;
	}
} 
/* }}} */

/* {{{ php_pgsql_get_data_type
 */
static php_pgsql_data_type php_pgsql_get_data_type(const char *type_name, size_t len)
{
    /* This is stupid way to do. I'll fix it when I decied how to support
	   user defined types. (Yasuo) */
	
	/* boolean */
	if (!strcmp(type_name, "bool")|| !strcmp(type_name, "boolean"))
		return PG_BOOL;
	/* object id */
	if (!strcmp(type_name, "oid"))
		return PG_OID;
	/* integer */
	if (!strcmp(type_name, "int2") || !strcmp(type_name, "smallint"))
		return PG_INT2;
	if (!strcmp(type_name, "int4") || !strcmp(type_name, "integer"))
		return PG_INT4;
	if (!strcmp(type_name, "int8") || !strcmp(type_name, "bigint"))
		return PG_INT8;
	/* real and other */
	if (!strcmp(type_name, "float4") || !strcmp(type_name, "real"))
		return PG_FLOAT4;
	if (!strcmp(type_name, "float8") || !strcmp(type_name, "double precision"))
		return PG_FLOAT8;
	if (!strcmp(type_name, "numeric"))
		return PG_NUMERIC;
	if (!strcmp(type_name, "money"))
		return PG_MONEY;
	/* character */
	if (!strcmp(type_name, "text"))
		return PG_TEXT;
	if (!strcmp(type_name, "bpchar") || !strcmp(type_name, "character"))
		return PG_CHAR;
	if (!strcmp(type_name, "varchar") || !strcmp(type_name, "character varying"))
		return PG_VARCHAR;
	/* time and interval */
	if (!strcmp(type_name, "abstime"))
		return PG_UNIX_TIME;
	if (!strcmp(type_name, "reltime"))
		return PG_UNIX_TIME_INTERVAL;
	if (!strcmp(type_name, "tinterval"))
		return PG_UNIX_TIME_INTERVAL;
	if (!strcmp(type_name, "date"))
		return PG_DATE;
	if (!strcmp(type_name, "time"))
		return PG_TIME;
	if (!strcmp(type_name, "time with time zone") || !strcmp(type_name, "timetz"))
		return PG_TIME_WITH_TIMEZONE;
	if (!strcmp(type_name, "timestamp without time zone") || !strcmp(type_name, "timestamp"))
		return PG_TIMESTAMP;
	if (!strcmp(type_name, "timestamp with time zone") || !strcmp(type_name, "timestamptz"))
		return PG_TIMESTAMP_WITH_TIMEZONE;
	if (!strcmp(type_name, "interval"))
		return PG_INTERVAL;
	/* binary */
	if (!strcmp(type_name, "bytea"))
		return PG_BYTEA;
	/* network */
	if (!strcmp(type_name, "cidr"))
		return PG_CIDR;
	if (!strcmp(type_name, "inet"))
		return PG_INET;
	if (!strcmp(type_name, "macaddr"))
		return PG_MACADDR;
	/* bit */
	if (!strcmp(type_name, "bit"))
		return PG_BIT;
	if (!strcmp(type_name, "bit varying"))
		return PG_VARBIT;
	/* geometric */
	if (!strcmp(type_name, "line"))
		return PG_LINE;
	if (!strcmp(type_name, "lseg"))
		return PG_LSEG;
	if (!strcmp(type_name, "box"))
		return PG_BOX;
	if (!strcmp(type_name, "path"))
		return PG_PATH;
	if (!strcmp(type_name, "point"))
		return PG_POINT;
	if (!strcmp(type_name, "polygon"))
		return PG_POLYGON;
	if (!strcmp(type_name, "circle"))
		return PG_CIRCLE;
		
	return PG_UNKNOWN;
}
/* }}} */

/* {{{ php_pgsql_convert_match
 * test field value with regular expression specified.  
 */
static int php_pgsql_convert_match(const char *str, const char *regex , int icase TSRMLS_DC)
{
	regex_t re;	
	regmatch_t *subs;
	int regopt = REG_EXTENDED;
	int regerr, ret = SUCCESS;

	if (icase) {
		regopt |= REG_ICASE;
	}
	
	regerr = regcomp(&re, regex, regopt);
	if (regerr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot compile regex");
		regfree(&re);
		return FAILURE;
	}
	subs = (regmatch_t *)ecalloc(sizeof(regmatch_t), re.re_nsub+1);

	regerr = regexec(&re, str, re.re_nsub+1, subs, 0);
	if (regerr == REG_NOMATCH) {
#ifdef PHP_DEBUG		
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "'%s' does not match with '%s'", str, regex);
#endif
		ret = FAILURE;
	}
	else if (regerr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot exec regex");
		ret = FAILURE;
	}
	regfree(&re);
	efree(subs);
	return ret;
}

/* }}} */

/* {{{ php_pgsql_add_quote
 * add quotes around string.
 */
static int php_pgsql_add_quotes(zval *src, zend_bool should_free TSRMLS_DC) 
{
	smart_str str = {0};
	
	assert(Z_TYPE_P(src) == IS_STRING);
	assert(should_free == 1 || should_free == 0);

	smart_str_appendc(&str, '\'');
	smart_str_appendl(&str, Z_STRVAL_P(src), Z_STRLEN_P(src));
	smart_str_appendc(&str, '\'');
	smart_str_0(&str);
	
	if (should_free) {
		efree(Z_STRVAL_P(src));
	}
	Z_STRVAL_P(src) = str.c;
	Z_STRLEN_P(src) = str.len;

	return SUCCESS;
}
/* }}} */

#define PGSQL_CONV_CHECK_IGNORE() \
				if (!err && Z_TYPE_P(new_val) == IS_STRING && !strcmp(Z_STRVAL_P(new_val), "NULL")) { \
					/* if new_value is string "NULL" and field has default value, remove element to use default value */ \
					if (!(opt & PGSQL_CONV_IGNORE_DEFAULT) && Z_BVAL_PP(has_default)) { \
						zval_dtor(new_val); \
						FREE_ZVAL(new_val); \
						skip_field = 1; \
					} \
					/* raise error if it's not null and cannot be ignored */ \
					else if (!(opt & PGSQL_CONV_IGNORE_NOT_NULL) && Z_BVAL_PP(not_null)) { \
						php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected NULL for 'NOT NULL' field '%s'", field ); \
						err = 1; \
					} \
				} 		

/* {{{ php_pgsql_convert
 * check and convert array values (fieldname=>vlaue pair) for sql
 */
PHP_PGSQL_API int php_pgsql_convert(PGconn *pg_link, const char *table_name, const zval *values, zval *result, ulong opt TSRMLS_DC) 
{
	HashPosition pos;
	char *field = NULL;
	uint field_len = -1;
	ulong num_idx = -1;
	zval *meta, **def, **type, **not_null, **has_default, **val, *new_val;
	int new_len, key_type, err = 0, skip_field;
	
	assert(pg_link != NULL);
	assert(Z_TYPE_P(values) == IS_ARRAY);
	assert(Z_TYPE_P(result) == IS_ARRAY);
	assert(!(opt & ~PGSQL_CONV_OPTS));

	if (!table_name) {
		return FAILURE;
	}
	MAKE_STD_ZVAL(meta);
	array_init(meta);
	if (php_pgsql_meta_data(pg_link, table_name, meta TSRMLS_CC) == FAILURE) {
		zval_dtor(meta);
		FREE_ZVAL(meta);
		return FAILURE;
	}
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(values), (void **)&val, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos)) {
		skip_field = 0;
		new_val = NULL;
		
		if ((key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &field, &field_len, &num_idx, 0, &pos)) == HASH_KEY_NON_EXISTANT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to get array key type");
			err = 1;
		}
		if (!err && key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Accepts only string key for values");
			err = 1;
		}
		if (!err && key_type == HASH_KEY_NON_EXISTANT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Accepts only string key for values");
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_P(meta), field, field_len, (void **)&def) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid field name (%s) in values", field);
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_PP(def), "type", sizeof("type"), (void **)&type) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected broken meta data. Missing 'type'");
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_PP(def), "not null", sizeof("not null"), (void **)&not_null) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected broken meta data. Missing 'not null'");
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_PP(def), "has default", sizeof("has default"), (void **)&has_default) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected broken meta data. Missing 'has default'");
			err = 1;
		}
		if (!err && (Z_TYPE_PP(val) == IS_ARRAY ||
			 Z_TYPE_PP(val) == IS_OBJECT ||
			 Z_TYPE_PP(val) == IS_CONSTANT_ARRAY)) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects scaler values as field values");
			err = 1;
		}
		if (err) {
			break; /* break out for() */
		}
		ALLOC_INIT_ZVAL(new_val);
		switch(php_pgsql_get_data_type(Z_STRVAL_PP(type), Z_STRLEN_PP(type)))
		{
			case PG_BOOL:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							if (!strcmp(Z_STRVAL_PP(val), "t") || !strcmp(Z_STRVAL_PP(val), "T") ||
								!strcmp(Z_STRVAL_PP(val), "y") || !strcmp(Z_STRVAL_PP(val), "Y") ||
								!strcmp(Z_STRVAL_PP(val), "true") || !strcmp(Z_STRVAL_PP(val), "True") ||
								!strcmp(Z_STRVAL_PP(val), "yes") || !strcmp(Z_STRVAL_PP(val), "Yes") ||
								!strcmp(Z_STRVAL_PP(val), "1")) {
								ZVAL_STRING(new_val, "'t'", 1);
							}
							else if (!strcmp(Z_STRVAL_PP(val), "f") || !strcmp(Z_STRVAL_PP(val), "F") ||
									 !strcmp(Z_STRVAL_PP(val), "n") || !strcmp(Z_STRVAL_PP(val), "N") ||
									 !strcmp(Z_STRVAL_PP(val), "false") ||  !strcmp(Z_STRVAL_PP(val), "False") ||
									 !strcmp(Z_STRVAL_PP(val), "no") ||  !strcmp(Z_STRVAL_PP(val), "No") ||
									 !strcmp(Z_STRVAL_PP(val), "0")) {
								ZVAL_STRING(new_val, "'f'", 1);
							}
							else {
								php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected invalid value (%s) for PostgreSQL %s field (%s)", Z_STRVAL_PP(val), Z_STRVAL_PP(type), field);
								err = 1;
							}
						}
						break;
						
					case IS_LONG:
					case IS_BOOL:
						if (Z_LVAL_PP(val)) {
							ZVAL_STRING(new_val, "'t'", 1);
						}
						else {
							ZVAL_STRING(new_val, "'f'", 1);
						}
						break;

					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or boolelan value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
					
			case PG_OID:
			case PG_INT2:
			case PG_INT4:
			case PG_INT8:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([+-]{0,1}[0-9]+)$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
							}
						}
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_long_ex(&new_val);
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL, string, long or double value for pgsql '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_NUMERIC:
			case PG_MONEY:
			case PG_FLOAT4:
			case PG_FLOAT8:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([+-]{0,1}[0-9]+)|([+-]{0,1}[0-9]*[\\.][0-9]+)|([+-]{0,1}[0-9]+[\\.][0-9]*)$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
							}
						}
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL, string, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_TEXT:
			case PG_CHAR:
			case PG_VARCHAR:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							if (opt & PGSQL_CONV_FORCE_NULL) {
								ZVAL_STRING(new_val, "NULL", 1);
							} else {
								ZVAL_STRING(new_val, "''", 1);
							}
						}
						else {
							Z_TYPE_P(new_val) = IS_STRING;
#if HAVE_PQESCAPE
							{
								char *tmp;
	 							tmp = (char *)safe_emalloc(Z_STRLEN_PP(val), 2, 1);
								Z_STRLEN_P(new_val) = (int)PQescapeString(tmp, Z_STRVAL_PP(val), Z_STRLEN_PP(val));
								Z_STRVAL_P(new_val) = tmp;
							}
#else					
							Z_STRVAL_P(new_val) = php_addslashes(Z_STRVAL_PP(val), Z_STRLEN_PP(val), &Z_STRLEN_P(new_val), 0 TSRMLS_CC);
#endif
							php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
						}
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;

					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL, string, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
					
			case PG_UNIX_TIME:
			case PG_UNIX_TIME_INTERVAL:
				/* these are the actallay a integer */
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: Better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^[0-9]+$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							} 
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								convert_to_long_ex(&new_val);
							}
						}
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_long_ex(&new_val);
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL, string, long or double value for '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
			case PG_CIDR:
			case PG_INET:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: Better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9]{1,3}\\.){3}[0-9]{1,3}(/[0-9]{1,2}){0,1}$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}				
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL or string for '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
			case PG_TIME_WITH_TIMEZONE:
			case PG_TIMESTAMP:
			case PG_TIMESTAMP_WITH_TIMEZONE:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRINGL(new_val, "NULL", sizeof("NULL")-1, 1);
						} else if (!strcasecmp(Z_STRVAL_PP(val), "now()")) {
							ZVAL_STRINGL(new_val, "NOW()", sizeof("NOW()")-1, 1);
						} else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9]{4}[/-][0-9]{1,2}[/-][0-9]{1,2})([ \\t]+(([0-9]{1,2}:[0-9]{1,2}){1}(:[0-9]{1,2}){0,1}(\\.[0-9]+){0,1}([ \\t]*([+-][0-9]{1,4}(:[0-9]{1,2}){0,1}|[-a-zA-Z_/+]{1,50})){0,1})){0,1}$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							} else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRINGL(new_val, "NULL", sizeof("NULL")-1, 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
			case PG_DATE:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9]{4}[/-][0-9]{1,2}[/-][0-9]{1,2})$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_TIME:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^(([0-9]{1,2}:[0-9]{1,2}){1}(:[0-9]{1,2}){0,1})){0,1}$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_INTERVAL:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {

							/* From the Postgres docs:

							   interval values can be written with the following syntax:
							   [@] quantity unit [quantity unit...] [direction]
							   
							   Where: quantity is a number (possibly signed); unit is second, minute, hour,
							   day, week, month, year, decade, century, millennium, or abbreviations or
							   plurals of these units [note not *all* abbreviations] ; direction can be
							   ago or empty. The at sign (@) is optional noise.
							   
							   ...
							   
							   Quantities of days, hours, minutes, and seconds can be specified without explicit
							   unit markings. For example, '1 12:59:10' is read the same as '1 day 12 hours 59 min 10
							   sec'.
							*/							  
							if (php_pgsql_convert_match(Z_STRVAL_PP(val),
														"^(@?[ \\t]+)?("
														
														/* Textual time units and their abbreviations: */
														"(([-+]?[ \\t]+)?"
														"[0-9]+(\\.[0-9]*)?[ \\t]*"
														"(millenniums|millennia|millennium|mil|mils|"
														"centuries|century|cent|c|"
														"decades|decade|dec|decs|"
														"years|year|y|"
														"months|month|mon|"
														"weeks|week|w|" 
														"days|day|d|"
														"hours|hour|hr|hrs|h|"
														"minutes|minute|mins|min|m|"
														"seconds|second|secs|sec|s))+|"

														/* Textual time units plus (dd)* hh[:mm[:ss]] */
														"((([-+]?[ \\t]+)?"
														"[0-9]+(\\.[0-9]*)?[ \\t]*"
														"(millenniums|millennia|millennium|mil|mils|"
														"centuries|century|cent|c|"
														"decades|decade|dec|decs|"
														"years|year|y|"
														"months|month|mon|"
														"weeks|week|w|"
														"days|day|d))+" 
														"([-+]?[ \\t]+"
														"([0-9]+[ \\t]+)+"				 /* dd */
														"(([0-9]{1,2}:){0,2}[0-9]{0,2})" /* hh:[mm:[ss]] */
														")?))"
														"([ \\t]+ago)?$",
														1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}	
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;
#ifdef HAVE_PQESCAPE
			case PG_BYTEA:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							unsigned char *tmp;
							size_t to_len;
#ifdef HAVE_PQESCAPE_BYTEA_CONN
							tmp = PQescapeByteaConn(pg_link, Z_STRVAL_PP(val), Z_STRLEN_PP(val), &to_len);
#else
							tmp = PQescapeBytea(Z_STRVAL_PP(val), Z_STRLEN_PP(val), &to_len);
#endif
							Z_TYPE_P(new_val) = IS_STRING;
							Z_STRLEN_P(new_val) = to_len-1; /* PQescapeBytea's to_len includes additional '\0' */
							Z_STRVAL_P(new_val) = emalloc(to_len);
							memcpy(Z_STRVAL_P(new_val), tmp, to_len);
							PQfreemem(tmp);
							php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
								
						}
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;

					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL, string, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
#endif
			case PG_MACADDR:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9a-f]{2,2}:){5,5}[0-9a-f]{2,2}$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;

				/* bit */
			case PG_BIT:
			case PG_VARBIT:
				/* geometric */
			case PG_LINE:
			case PG_LSEG:
			case PG_POINT:
			case PG_BOX:
			case PG_PATH:
			case PG_POLYGON:
			case PG_CIRCLE:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "PostgreSQL '%s' type (%s) is not supported", Z_STRVAL_PP(type), field);
				err = 1;
				break;
				
			case PG_UNKNOWN:
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown or system data type '%s' for '%s'", Z_STRVAL_PP(type), field);
				err = 1;
				break;
		} /* switch */
		
		if (err) {
			zval_dtor(new_val);
			FREE_ZVAL(new_val);
			break; /* break out for() */
		}
		if (!skip_field) {
			/* If field is NULL and HAS DEFAULT, should be skipped */
			field = php_addslashes(field, strlen(field), &new_len, 0 TSRMLS_CC);
			add_assoc_zval(result, field, new_val);
			efree(field);
		}
	} /* for */
	zval_dtor(meta);
	FREE_ZVAL(meta);

	if (err) {
		/* shouldn't destroy & free zval here */
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */


/* {{{ proto array pg_convert(resource db, string table, array values[, int options])
   Check and convert values for PostgreSQL SQL statement */
PHP_FUNCTION(pg_convert)
{
	zval *pgsql_link, *values;
	char *table_name;
	int table_name_len;
	ulong option = 0;
	PGconn *pg_link;
	int id = -1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
							  "rsa|l", &pgsql_link, &table_name, &table_name_len, &values, &option) == FAILURE) {
		return;
	}
	if (option & ~PGSQL_CONV_OPTS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is specified");
		RETURN_FALSE;
	}
	if (!table_name_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Table name is invalid");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	array_init(return_value);
	if (php_pgsql_convert(pg_link, table_name, values, return_value, option TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

static int do_exec(smart_str *querystr, int expect, PGconn *pg_link, ulong opt TSRMLS_DC)
{
	if (opt & PGSQL_DML_ASYNC) {
		if (PQsendQuery(pg_link, querystr->c)) {
			return 0;
		}
	}
	else {
		PGresult *pg_result;

		pg_result = PQexec(pg_link, querystr->c);
		if (PQresultStatus(pg_result) == expect) {
			PQclear(pg_result);
			return 0;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", PQresultErrorMessage(pg_result));
			PQclear(pg_result);
		}
	}

	return -1;
}

/* {{{ php_pgsql_insert
 */
PHP_PGSQL_API int php_pgsql_insert(PGconn *pg_link, const char *table, zval *var_array, ulong opt, char **sql TSRMLS_DC)
{
	zval **val, *converted = NULL;
	char buf[256];
	char *fld;
	smart_str querystr = {0};
	int key_type, ret = FAILURE;
	uint fld_len;
	ulong num_idx;
	HashPosition pos;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(var_array) == IS_ARRAY);

	if (zend_hash_num_elements(Z_ARRVAL_P(var_array)) == 0) {
		smart_str_appends(&querystr, "INSERT INTO ");
		smart_str_appends(&querystr, table);
		smart_str_appends(&querystr, " DEFAULT VALUES");

		goto no_values;
	}

	/* convert input array if needed */
	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(converted);
		array_init(converted);
		if (php_pgsql_convert(pg_link, table, var_array, converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		var_array = converted;
	}
	
	smart_str_appends(&querystr, "INSERT INTO ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " (");
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(var_array), &pos);
	while ((key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(var_array), &fld,
					&fld_len, &num_idx, 0, &pos)) != HASH_KEY_NON_EXISTANT) {
		if (key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects associative array for values to be inserted");
			goto cleanup;
		}
		smart_str_appendl(&querystr, fld, fld_len - 1);
		smart_str_appendc(&querystr, ',');
		zend_hash_move_forward_ex(Z_ARRVAL_P(var_array), &pos);
	}
	querystr.len--;
	smart_str_appends(&querystr, ") VALUES (");
	
	/* make values string */
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(var_array), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(var_array), (void **)&val, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(var_array), &pos)) {
		
		/* we can avoid the key_type check here, because we tested it in the other loop */
		switch(Z_TYPE_PP(val)) {
			case IS_STRING:
				smart_str_appendl(&querystr, Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				break;
			case IS_LONG:
				smart_str_append_long(&querystr, Z_LVAL_PP(val));
				break;
			case IS_DOUBLE:
				smart_str_appendl(&querystr, buf, snprintf(buf, sizeof(buf), "%F", Z_DVAL_PP(val)));
				break;
			default:
				/* should not happen */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Report this error to php-dev@lists.php.net, type = %d", Z_TYPE_PP(val));
				goto cleanup;
				break;
		}
		smart_str_appendc(&querystr, ',');
	}
	/* Remove the trailing "," */
	querystr.len--;
	smart_str_appends(&querystr, ");");

no_values:

	smart_str_0(&querystr);

	if ((opt & (PGSQL_DML_EXEC|PGSQL_DML_ASYNC)) &&
		do_exec(&querystr, PGRES_COMMAND_OK, pg_link, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == 0) {
		ret = SUCCESS;
	}
	else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}
	
cleanup:
	if (!(opt & PGSQL_DML_NO_CONV) && converted) {
		zval_dtor(converted);			
		FREE_ZVAL(converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_insert(resource db, string table, array values[, int options])
   Insert values (filed=>value) to table */
PHP_FUNCTION(pg_insert)
{
	zval *pgsql_link, *values;
	char *table, *sql = NULL;
	int table_len;
	ulong option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|l",
							  &pgsql_link, &table, &table_len, &values, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is specified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_insert(pg_link, table, values, option, &sql TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STRING(sql, 0);
	}
	RETURN_TRUE;
}
/* }}} */

static inline int build_assignment_string(smart_str *querystr, HashTable *ht, int where_cond, const char *pad, int pad_len TSRMLS_DC)
{
	HashPosition pos;
	uint fld_len;
	int key_type;
	ulong num_idx;
	char *fld;
	char buf[256];
	zval **val;

	for (zend_hash_internal_pointer_reset_ex(ht, &pos);
		 zend_hash_get_current_data_ex(ht, (void **)&val, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(ht, &pos)) {
		 key_type = zend_hash_get_current_key_ex(ht, &fld, &fld_len, &num_idx, 0, &pos);		
		if (key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects associative array for values to be inserted");
			return -1;
		}
		smart_str_appendl(querystr, fld, fld_len - 1);
		if (where_cond && Z_TYPE_PP(val) == IS_STRING && !strcmp(Z_STRVAL_PP(val), "NULL")) {
			smart_str_appends(querystr, " IS ");
		} else {
			smart_str_appendc(querystr, '=');
		}
		
		switch(Z_TYPE_PP(val)) {
			case IS_STRING:
				smart_str_appendl(querystr, Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				break;
			case IS_LONG:
				smart_str_append_long(querystr, Z_LVAL_PP(val));
				break;
			case IS_DOUBLE:
				smart_str_appendl(querystr, buf, MIN(snprintf(buf, sizeof(buf), "%F", Z_DVAL_PP(val)), sizeof(buf)-1));
				break;
			default:
				/* should not happen */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects scaler values other than NULL. Need to convert?");
				return -1;
		}
		smart_str_appendl(querystr, pad, pad_len);
	}
	querystr->len -= pad_len;

	return 0;
}

/* {{{ php_pgsql_update
 */
PHP_PGSQL_API int php_pgsql_update(PGconn *pg_link, const char *table, zval *var_array, zval *ids_array, ulong opt, char **sql TSRMLS_DC) 
{
	zval *var_converted = NULL, *ids_converted = NULL;
	smart_str querystr = {0};
	int ret = FAILURE;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(var_array) == IS_ARRAY);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING)));

	if (zend_hash_num_elements(Z_ARRVAL_P(var_array)) == 0
			|| zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(var_converted);
		array_init(var_converted);
		if (php_pgsql_convert(pg_link, table, var_array, var_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		var_array = var_converted;
		MAKE_STD_ZVAL(ids_converted);
		array_init(ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, ids_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		ids_array = ids_converted;
	}

	smart_str_appends(&querystr, "UPDATE ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " SET ");

	if (build_assignment_string(&querystr, Z_ARRVAL_P(var_array), 0, ",", 1 TSRMLS_CC))
		goto cleanup;
	
	smart_str_appends(&querystr, " WHERE ");
	
	if (build_assignment_string(&querystr, Z_ARRVAL_P(ids_array), 1, " AND ", sizeof(" AND ")-1 TSRMLS_CC))
		goto cleanup;

	smart_str_appendc(&querystr, ';');	
	smart_str_0(&querystr);

	if ((opt & PGSQL_DML_EXEC) && do_exec(&querystr, PGRES_COMMAND_OK, pg_link, opt TSRMLS_CC) == 0) {
		ret = SUCCESS;
	} else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}

cleanup:
	if (var_converted) {
		zval_dtor(var_converted);
		FREE_ZVAL(var_converted);
	}
	if (ids_converted) {
		zval_dtor(ids_converted);
		FREE_ZVAL(ids_converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_update(resource db, string table, array fields, array ids[, int options])
   Update table using values (field=>value) and ids (id=>value) */
PHP_FUNCTION(pg_update)
{
	zval *pgsql_link, *values, *ids;
	char *table, *sql = NULL;
	int table_len;
	ulong option =  PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsaa|l",
							  &pgsql_link, &table, &table_len, &values, &ids, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is specified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_update(pg_link, table, values, ids, option, &sql TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STRING(sql, 0);
	}
	RETURN_TRUE;
} 
/* }}} */

/* {{{ php_pgsql_delete
 */
PHP_PGSQL_API int php_pgsql_delete(PGconn *pg_link, const char *table, zval *ids_array, ulong opt, char **sql TSRMLS_DC) 
{
	zval *ids_converted = NULL;
	smart_str querystr = {0};
	int ret = FAILURE;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_EXEC|PGSQL_DML_STRING)));
	
	if (zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(ids_converted);
		array_init(ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, ids_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		ids_array = ids_converted;
	}

	smart_str_appends(&querystr, "DELETE FROM ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(&querystr, Z_ARRVAL_P(ids_array), 1, " AND ", sizeof(" AND ")-1 TSRMLS_CC))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	if ((opt & PGSQL_DML_EXEC) && do_exec(&querystr, PGRES_COMMAND_OK, pg_link, opt TSRMLS_CC) == 0) {
		ret = SUCCESS;
	} else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}

cleanup:
	if (!(opt & PGSQL_DML_NO_CONV)) {
		zval_dtor(ids_converted);			
		FREE_ZVAL(ids_converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_delete(resource db, string table, array ids[, int options])
   Delete records has ids (id=>value) */
PHP_FUNCTION(pg_delete)
{
	zval *pgsql_link, *ids;
	char *table, *sql = NULL;
	int table_len;
	ulong option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|l",
							  &pgsql_link, &table, &table_len, &ids, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is specified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_delete(pg_link, table, ids, option, &sql TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STRING(sql, 0);
	}
	RETURN_TRUE;
} 
/* }}} */

/* {{{ php_pgsql_result2array
 */
PHP_PGSQL_API int php_pgsql_result2array(PGresult *pg_result, zval *ret_array TSRMLS_DC) 
{
	zval *row;
	char *field_name;
	size_t num_fields;
	int pg_numrows, pg_row;
	uint i;
	assert(Z_TYPE_P(ret_array) == IS_ARRAY);

	if ((pg_numrows = PQntuples(pg_result)) <= 0) {
		return FAILURE;
	}
	for (pg_row = 0; pg_row < pg_numrows; pg_row++) {
		MAKE_STD_ZVAL(row);
		array_init(row);
		add_index_zval(ret_array, pg_row, row);
		for (i = 0, num_fields = PQnfields(pg_result); i < num_fields; i++) {
			if (PQgetisnull(pg_result, pg_row, i)) {
				field_name = PQfname(pg_result, i);
				add_assoc_null(row, field_name);
			} else {
				char *element = PQgetvalue(pg_result, pg_row, i);
				if (element) {
					char *data;
					size_t data_len;
					const size_t element_len = strlen(element);

					data = safe_estrndup(element, element_len);
					data_len = element_len;
					
					field_name = PQfname(pg_result, i);
					add_assoc_stringl(row, field_name, data, data_len, 0);
				}
			}
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ php_pgsql_select
 */
PHP_PGSQL_API int php_pgsql_select(PGconn *pg_link, const char *table, zval *ids_array, zval *ret_array, ulong opt, char **sql TSRMLS_DC) 
{
	zval *ids_converted = NULL;
	smart_str querystr = {0};
	int ret = FAILURE;
	PGresult *pg_result;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(Z_TYPE_P(ret_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING)));
	
	if (zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(ids_converted);
		array_init(ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, ids_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		ids_array = ids_converted;
	}

	smart_str_appends(&querystr, "SELECT * FROM ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(&querystr, Z_ARRVAL_P(ids_array), 1, " AND ", sizeof(" AND ")-1 TSRMLS_CC))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	pg_result = PQexec(pg_link, querystr.c);
	if (PQresultStatus(pg_result) == PGRES_TUPLES_OK) {
		ret = php_pgsql_result2array(pg_result, ret_array TSRMLS_CC);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to execute '%s'", querystr.c);
	}
	PQclear(pg_result);

cleanup:
	if (!(opt & PGSQL_DML_NO_CONV)) {
		zval_dtor(ids_converted);			
		FREE_ZVAL(ids_converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_select(resource db, string table, array ids[, int options])
   Select records that has ids (id=>value) */
PHP_FUNCTION(pg_select)
{
	zval *pgsql_link, *ids;
	char *table, *sql = NULL;
	int table_len;
	ulong option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|l",
							  &pgsql_link, &table, &table_len, &ids, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is specified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	array_init(return_value);
	if (php_pgsql_select(pg_link, table, ids, return_value, option, &sql TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		zval_dtor(return_value);
		RETURN_STRING(sql, 0);
	}
	return;
} 
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
