/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Maxim Maletsky <maxim@maxim.cx>                             |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#include "zend_smart_str.h"

#if HAVE_OCI8

/* PHP 5.2 is the minimum supported version for OCI8 2.0 */
#if PHP_MAJOR_VERSION < 5 || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION <= 1)
#error Use PHP OCI8 1.4 for your version of PHP
#endif

#include "php_oci8.h"
#include "php_oci8_int.h"
#include "zend_hash.h"

#if defined(__PTRDIFF_TYPE__)
# define OCI8_INT_TO_PTR(I)  ((void*)(__PTRDIFF_TYPE__)(I))
# define OCI8_PTR_TO_INT(P)  ((int)(__PTRDIFF_TYPE__)(P))
#elif !defined(__GNUC__)
#define OCI8_INT_TO_PTR(I)  ((void*)&((char*)0)[I])
#define OCI8_PTR_TO_INT(P)  ((int)(((char*)P)-(char*)0))
#elif defined(HAVE_STDINT_H)
#define OCI8_INT_TO_PTR(I)  ((void*)(intptr_t)(I))
#define OCI8_PTR_TO_INT(P)  ((int)(intptr_t)(P))
#else
#define OCI8_INT_TO_PTR(I)  ((void*)(I))
#define OCI8_PTR_TO_INT(P)  ((int)(P))
#endif

ZEND_DECLARE_MODULE_GLOBALS(oci)
static PHP_GINIT_FUNCTION(oci);
static PHP_GSHUTDOWN_FUNCTION(oci);

/* Allow PHP 5.3 branch to be used in PECL for 5.x compatible builds */
#ifndef Z_ADDREF_P
#define Z_ADDREF_P(x) ZVAL_ADDREF(x)
#endif

/* For a user friendly message about environment setup */
#if defined(PHP_WIN32)
#define PHP_OCI8_LIB_PATH_MSG "PATH"
#elif defined(__APPLE__)
#define PHP_OCI8_LIB_PATH_MSG "DYLD_LIBRARY_PATH"
#elif defined(_AIX)
#define PHP_OCI8_LIB_PATH_MSG "LIBPATH"
#elif defined(__hpux)
#define PHP_OCI8_LIB_PATH_MSG "SHLIB_PATH"
#else
#define PHP_OCI8_LIB_PATH_MSG "LD_LIBRARY_PATH"
#endif

/* True globals, no need for thread safety */
int le_connection;
int le_pconnection;
int le_statement;
int le_descriptor;
int le_psessionpool;
int le_collection;

zend_class_entry *oci_lob_class_entry_ptr;
zend_class_entry *oci_coll_class_entry_ptr;

#ifndef SQLT_BFILEE
#define SQLT_BFILEE 114
#endif
#ifndef SQLT_CFILEE
#define SQLT_CFILEE 115
#endif

#ifdef OCI_ERROR_MAXMSG_SIZE2
/* Bigger size is defined from 11.2.0.3 onwards */
#define PHP_OCI_ERRBUF_LEN OCI_ERROR_MAXMSG_SIZE2
#else
#define PHP_OCI_ERRBUF_LEN OCI_ERROR_MAXMSG_SIZE
#endif

#if ZEND_MODULE_API_NO > 20020429
#define ONUPDATELONGFUNC OnUpdateLong
#else
#define ONUPDATELONGFUNC OnUpdateInt
#endif

#ifdef ZTS
#define PHP_OCI_INIT_MODE (OCI_DEFAULT | OCI_OBJECT | OCI_THREADED | OCI_NO_MUTEX)
#else
#define PHP_OCI_INIT_MODE (OCI_DEFAULT | OCI_OBJECT)
#endif

/* {{{ static protos */
static void php_oci_connection_list_dtor (zend_resource *);
static void php_oci_pconnection_list_dtor (zend_resource *);
static void php_oci_pconnection_list_np_dtor (zend_resource *);
static void php_oci_statement_list_dtor (zend_resource *);
static void php_oci_descriptor_list_dtor (zend_resource *);
static void php_oci_spool_list_dtor(zend_resource *entry);
static void php_oci_collection_list_dtor (zend_resource *);

static int php_oci_persistent_helper(zend_resource *le);
static int php_oci_connection_ping(php_oci_connection *);
static int php_oci_connection_status(php_oci_connection *);
static int php_oci_connection_close(php_oci_connection *);
static void php_oci_spool_close(php_oci_spool *session_pool);

static OCIEnv *php_oci_create_env(ub2 charsetid);
static int php_oci_create_session(php_oci_connection *connection, php_oci_spool *session_pool, char *dbname, int dbname_len, char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, int session_mode);
static int php_oci_old_create_session(php_oci_connection *connection, char *dbname, int dbname_len, char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, int session_mode);
static php_oci_spool *php_oci_get_spool(char *username, int username_len, char *password, int password_len, char *dbname, int dbname_len, int charsetid);
static php_oci_spool *php_oci_create_spool(char *username, int username_len, char *password, int password_len, char *dbname, int dbname_len, zend_string *hash_key, int charsetid);
static sword php_oci_ping_init(php_oci_connection *connection, OCIError *errh);
/* }}} */

/* {{{ dynamically loadable module stuff */
#if defined(COMPILE_DL_OCI8) || defined(COMPILE_DL_OCI8_11G) || defined(COMPILE_DL_OCI8_12C)
ZEND_GET_MODULE(oci8)
#endif /* COMPILE_DL */
/* }}} */

#if defined(ZEND_ENGINE_2) || defined(ZEND_ENGINE_3)

/* {{{ Function arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_define_by_name, 0, 0, 3)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_name)
	ZEND_ARG_INFO(1, variable)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_bind_by_name, 0, 0, 3)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_name)
	ZEND_ARG_INFO(1, variable)
	ZEND_ARG_INFO(0, maximum_length)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_bind_array_by_name, 0, 0, 4)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_name)
	ZEND_ARG_INFO(1, variable)
	ZEND_ARG_INFO(0, maximum_array_length)
	ZEND_ARG_INFO(0, maximum_item_length)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_free_descriptor, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_save, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_import, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_load, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_read, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_eof, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_tell, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_rewind, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_seek, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_size, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_write, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_append, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor_to)
	ZEND_ARG_INFO(0, lob_descriptor_from)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_truncate, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_erase, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_flush, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ocisetbufferinglob, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ocigetbufferinglob, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_copy, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor_to)
	ZEND_ARG_INFO(0, lob_descriptor_from)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_is_equal, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, lob_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_export, 0, 0, 2)
	ZEND_ARG_INFO(0, lob_descriptor)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_descriptor, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_rollback, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_commit, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_name, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_size, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_scale, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_precision, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_type, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_type_raw, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_is_null, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_internal_debug, 0, 0, 1)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_execute, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_cancel, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ocifetchinto, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(1, result)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_all, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_INFO(0, skip)
	ZEND_ARG_INFO(0, maximum_rows)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_object, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_row, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_assoc, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_array, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_free_statement, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_close, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_connect, 0, 0, 2)
	ZEND_ARG_INFO(0, username)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, connection_string)
	ZEND_ARG_INFO(0, character_set)
	ZEND_ARG_INFO(0, session_mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_connect, 0, 0, 2)
	ZEND_ARG_INFO(0, username)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, connection_string)
	ZEND_ARG_INFO(0, character_set)
	ZEND_ARG_INFO(0, session_mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_pconnect, 0, 0, 2)
	ZEND_ARG_INFO(0, username)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, connection_string)
	ZEND_ARG_INFO(0, character_set)
	ZEND_ARG_INFO(0, session_mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_error, 0, 0, 0)
	ZEND_ARG_INFO(0, connection_or_statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_num_fields, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, sql_text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_get_implicit_resultset, 0, 0, 1)
ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_prefetch, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, number_of_rows)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_client_identifier, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, client_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_edition, 0, 0, 1)
	ZEND_ARG_INFO(0, edition_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_module_name, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, module_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_action, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_client_info, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, client_information)
ZEND_END_ARG_INFO()

#ifdef WAITIING_ORACLE_BUG_16695981_FIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_set_db_operation, 0, 0, 2)
ZEND_ARG_INFO(0, connection_resource)
ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_password_change, 0, 0, 4)
	ZEND_ARG_INFO(0, connection_resource_or_connection_string)
	ZEND_ARG_INFO(0, username)
	ZEND_ARG_INFO(0, old_password)
	ZEND_ARG_INFO(0, new_password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_cursor, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_result, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_client_version, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_server_version, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_statement_type, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_num_rows, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_free_collection, 0, 0, 1)
	ZEND_ARG_INFO(0, collection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_append, 0, 0, 2)
	ZEND_ARG_INFO(0, collection)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_element_get, 0, 0, 2)
	ZEND_ARG_INFO(0, collection)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_assign, 0, 0, 2)
	ZEND_ARG_INFO(0, collection_to)
	ZEND_ARG_INFO(0, collection_from)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_element_assign, 0, 0, 3)
	ZEND_ARG_INFO(0, collection)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_size, 0, 0, 1)
	ZEND_ARG_INFO(0, collection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_max, 0, 0, 1)
	ZEND_ARG_INFO(0, collection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_trim, 0, 0, 2)
	ZEND_ARG_INFO(0, collection)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_collection, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, type_name)
	ZEND_ARG_INFO(0, schema_name)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ LOB Method arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_save_method, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_import_method, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_load_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_read_method, 0, 0, 1)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_eof_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_tell_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_rewind_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_seek_method, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_size_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_write_method, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_append_method, 0, 0, 1)
	ZEND_ARG_INFO(0, lob_descriptor_from)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_truncate_method, 0, 0, 0)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_erase_method, 0, 0, 0)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_flush_method, 0, 0, 0)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_setbuffering_method, 0, 0, 1)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_getbuffering_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_export_method, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_write_temporary_method, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_lob_close_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_free_descriptor_method, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Collection Method arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_oci_collection_free_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_append_method, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_element_get_method, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_assign_method, 0, 0, 1)
	ZEND_ARG_INFO(0, collection_from)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_element_assign_method, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_collection_size_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_oci_collection_max_method, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_trim_method, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()
/* }}} */

#else /* defined(ZEND_ENGINE_2) || defined(ZEND_ENGINE_3) */
/* {{{ Keep the old arginfo behavior when building with PHP 4 */

static unsigned char arginfo_ocifetchinto[]  = { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char arginfo_oci_fetch_all[] = { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char arginfo_oci_define_by_name[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char arginfo_oci_bind_by_name[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char arginfo_oci_bind_array_by_name[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

#define arginfo_oci_free_descriptor						NULL
#define arginfo_oci_lob_save							NULL
#define arginfo_oci_lob_import							NULL
#define arginfo_oci_lob_load							NULL
#define arginfo_oci_lob_read							NULL
#define arginfo_oci_lob_eof								NULL
#define arginfo_oci_lob_tell							NULL
#define arginfo_oci_lob_rewind							NULL
#define arginfo_oci_lob_seek							NULL
#define arginfo_oci_lob_size							NULL
#define arginfo_oci_lob_write							NULL
#define arginfo_oci_lob_append							NULL
#define arginfo_oci_lob_truncate						NULL
#define arginfo_oci_lob_erase							NULL
#define arginfo_oci_lob_flush							NULL
#define arginfo_ocisetbufferinglob						NULL
#define arginfo_ocigetbufferinglob						NULL
#define arginfo_oci_lob_copy							NULL
#define arginfo_oci_lob_is_equal						NULL
#define arginfo_oci_lob_export							NULL
#define arginfo_oci_new_descriptor						NULL
#define arginfo_oci_rollback							NULL
#define arginfo_oci_commit								NULL
#define arginfo_oci_field_name							NULL
#define arginfo_oci_field_size							NULL
#define arginfo_oci_field_scale							NULL
#define arginfo_oci_field_precision						NULL
#define arginfo_oci_field_type							NULL
#define arginfo_oci_field_type_raw						NULL
#define arginfo_oci_field_is_null						NULL
#define arginfo_oci_internal_debug						NULL
#define arginfo_oci_execute								NULL
#define arginfo_oci_cancel								NULL
#define arginfo_oci_fetch								NULL
#define arginfo_oci_fetch_object						NULL
#define arginfo_oci_fetch_row							NULL
#define arginfo_oci_fetch_assoc							NULL
#define arginfo_oci_fetch_array							NULL
#define arginfo_oci_free_statement						NULL
#define arginfo_oci_close								NULL
#define arginfo_oci_new_connect							NULL
#define arginfo_oci_connect								NULL
#define arginfo_oci_pconnect							NULL
#define arginfo_oci_error								NULL
#define arginfo_oci_num_fields							NULL
#define arginfo_oci_parse								NULL
#define arginfo_oci_get_implicit_resultset				NULL
#define arginfo_oci_set_prefetch						NULL
#define arginfo_oci_set_client_identifier				NULL
#define arginfo_oci_set_edition							NULL
#define arginfo_oci_set_module_name						NULL
#define arginfo_oci_set_action							NULL
#define arginfo_oci_set_client_info						NULL
#ifdef WAITIING_ORACLE_BUG_16695981_FIX
#define arginfo_oci_set_db_operation					NULL
#endif
#define arginfo_oci_password_change						NULL
#define arginfo_oci_new_cursor							NULL
#define arginfo_oci_result								NULL
#define arginfo_oci_client_version						NULL
#define arginfo_oci_server_version						NULL
#define arginfo_oci_statement_type						NULL
#define arginfo_oci_num_rows							NULL
#define arginfo_oci_free_collection						NULL
#define arginfo_oci_collection_append					NULL
#define arginfo_oci_collection_element_get				NULL
#define arginfo_oci_collection_assign					NULL
#define arginfo_oci_collection_element_assign			NULL
#define arginfo_oci_collection_size						NULL
#define arginfo_oci_collection_max						NULL
#define arginfo_oci_collection_trim						NULL
#define arginfo_oci_new_collection						NULL
#define arginfo_oci_lob_size_method						NULL
#define arginfo_oci_lob_getbuffering_method				NULL
#define arginfo_oci_lob_close_method					NULL
#define arginfo_oci_lob_save_method						NULL
#define arginfo_oci_lob_import_method					NULL
#define arginfo_oci_lob_read_method						NULL
#define arginfo_oci_lob_seek_method						NULL
#define arginfo_oci_lob_write_method					NULL
#define arginfo_oci_lob_append_method					NULL
#define arginfo_oci_lob_truncate_method					NULL
#define arginfo_oci_lob_erase_method					NULL
#define arginfo_oci_lob_flush_method					NULL
#define arginfo_oci_lob_setbuffering_method				NULL
#define arginfo_oci_lob_export_method					NULL
#define arginfo_oci_lob_write_temporary_method			NULL
#define arginfo_oci_lob_load_method						NULL
#define arginfo_oci_lob_tell_method						NULL
#define arginfo_oci_lob_rewind_method					NULL
#define arginfo_oci_lob_eof_method						NULL
#define arginfo_oci_free_descriptor_method				NULL
#define arginfo_oci_collection_append_method			NULL
#define arginfo_oci_collection_element_get_method		NULL
#define arginfo_oci_collection_assign_method			NULL
#define arginfo_oci_collection_size_method				NULL
#define arginfo_oci_collection_element_assign_method	NULL
#define arginfo_oci_collection_max_method				NULL
#define arginfo_oci_collection_trim_method				NULL
#define arginfo_oci_collection_free_method				NULL
/* }}} */
#endif /* defined(ZEND_ENGINE_2) || defined(ZEND_ENGINE_3) */

/* {{{ extension function prototypes
*/
PHP_FUNCTION(oci_bind_by_name);
PHP_FUNCTION(oci_bind_array_by_name);
PHP_FUNCTION(oci_define_by_name);
PHP_FUNCTION(oci_field_is_null);
PHP_FUNCTION(oci_field_name);
PHP_FUNCTION(oci_field_size);
PHP_FUNCTION(oci_field_scale);
PHP_FUNCTION(oci_field_precision);
PHP_FUNCTION(oci_field_type);
PHP_FUNCTION(oci_field_type_raw);
PHP_FUNCTION(oci_execute);
PHP_FUNCTION(oci_fetch);
PHP_FUNCTION(oci_cancel);
PHP_FUNCTION(ocifetchinto);
PHP_FUNCTION(oci_fetch_object);
PHP_FUNCTION(oci_fetch_row);
PHP_FUNCTION(oci_fetch_assoc);
PHP_FUNCTION(oci_fetch_array);
PHP_FUNCTION(ocifetchstatement);
PHP_FUNCTION(oci_fetch_all);
PHP_FUNCTION(oci_free_statement);
PHP_FUNCTION(oci_internal_debug);
PHP_FUNCTION(oci_close);
PHP_FUNCTION(oci_connect);
PHP_FUNCTION(oci_new_connect);
PHP_FUNCTION(oci_pconnect);
PHP_FUNCTION(oci_error);
PHP_FUNCTION(oci_free_descriptor);
PHP_FUNCTION(oci_commit);
PHP_FUNCTION(oci_rollback);
PHP_FUNCTION(oci_new_descriptor);
PHP_FUNCTION(oci_num_fields);
PHP_FUNCTION(oci_parse);
PHP_FUNCTION(oci_get_implicit_resultset);
PHP_FUNCTION(oci_new_cursor);
PHP_FUNCTION(oci_result);
PHP_FUNCTION(oci_client_version);
PHP_FUNCTION(oci_server_version);
PHP_FUNCTION(oci_statement_type);
PHP_FUNCTION(oci_num_rows);
PHP_FUNCTION(oci_set_prefetch);
PHP_FUNCTION(oci_set_client_identifier);
#ifdef WAITIING_ORACLE_BUG_16695981_FIX
PHP_FUNCTION(oci_set_db_operation);
#endif
PHP_FUNCTION(oci_set_edition);
PHP_FUNCTION(oci_set_module_name);
PHP_FUNCTION(oci_set_action);
PHP_FUNCTION(oci_set_client_info);
PHP_FUNCTION(oci_password_change);
PHP_FUNCTION(oci_lob_save);
PHP_FUNCTION(oci_lob_import);
PHP_FUNCTION(oci_lob_export);
PHP_FUNCTION(oci_lob_load);
PHP_FUNCTION(oci_lob_tell);
PHP_FUNCTION(oci_lob_write);
PHP_FUNCTION(oci_lob_append);
PHP_FUNCTION(oci_lob_copy);
PHP_FUNCTION(oci_lob_truncate);
PHP_FUNCTION(oci_lob_erase);
PHP_FUNCTION(oci_lob_flush);
PHP_FUNCTION(ocisetbufferinglob);
PHP_FUNCTION(ocigetbufferinglob);
PHP_FUNCTION(oci_lob_is_equal);
PHP_FUNCTION(oci_lob_rewind);
PHP_FUNCTION(oci_lob_read);
PHP_FUNCTION(oci_lob_eof);
PHP_FUNCTION(oci_lob_seek);
PHP_FUNCTION(oci_lob_size);
PHP_FUNCTION(oci_lob_write_temporary);
PHP_FUNCTION(oci_lob_close);
PHP_FUNCTION(oci_new_collection);
PHP_FUNCTION(oci_free_collection);
PHP_FUNCTION(oci_collection_append);
PHP_FUNCTION(oci_collection_element_get);
PHP_FUNCTION(oci_collection_element_assign);
PHP_FUNCTION(oci_collection_assign);
PHP_FUNCTION(oci_collection_size);
PHP_FUNCTION(oci_collection_max);
PHP_FUNCTION(oci_collection_trim);
/* }}} */

/* {{{ extension definition structures
*/
static
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2) || (PHP_MAJOR_VERSION > 5)
/* This "if" allows PECL builds from this file to be portable to older PHP releases */
const
#endif
zend_function_entry php_oci_functions[] = {
	PHP_FE(oci_define_by_name,			arginfo_oci_define_by_name)
	PHP_FE(oci_bind_by_name,			arginfo_oci_bind_by_name)
	PHP_FE(oci_bind_array_by_name,		arginfo_oci_bind_array_by_name)
	PHP_FE(oci_field_is_null,			arginfo_oci_field_is_null)
	PHP_FE(oci_field_name,				arginfo_oci_field_name)
	PHP_FE(oci_field_size,				arginfo_oci_field_size)
	PHP_FE(oci_field_scale,				arginfo_oci_field_scale)
	PHP_FE(oci_field_precision,			arginfo_oci_field_precision)
	PHP_FE(oci_field_type,				arginfo_oci_field_type)
	PHP_FE(oci_field_type_raw,			arginfo_oci_field_type_raw)
	PHP_FE(oci_execute,					arginfo_oci_execute)
	PHP_FE(oci_cancel,					arginfo_oci_cancel)
	PHP_FE(oci_fetch,					arginfo_oci_fetch)
	PHP_FE(oci_fetch_object,			arginfo_oci_fetch_object)
	PHP_FE(oci_fetch_row,				arginfo_oci_fetch_row)
	PHP_FE(oci_fetch_assoc,				arginfo_oci_fetch_assoc)
	PHP_FE(oci_fetch_array,				arginfo_oci_fetch_array)
	PHP_FE(ocifetchinto,				arginfo_ocifetchinto)
	PHP_FE(oci_fetch_all,				arginfo_oci_fetch_all)
	PHP_FE(oci_free_statement,			arginfo_oci_free_statement)
	PHP_FE(oci_internal_debug,			arginfo_oci_internal_debug)
	PHP_FE(oci_num_fields,				arginfo_oci_num_fields)
	PHP_FE(oci_parse,					arginfo_oci_parse)
	PHP_FE(oci_get_implicit_resultset,	arginfo_oci_get_implicit_resultset)
	PHP_FE(oci_new_cursor,				arginfo_oci_new_cursor)
	PHP_FE(oci_result,					arginfo_oci_result)
	PHP_FE(oci_client_version,			arginfo_oci_client_version)
	PHP_FE(oci_server_version,			arginfo_oci_server_version)
	PHP_FE(oci_statement_type,			arginfo_oci_statement_type)
	PHP_FE(oci_num_rows,				arginfo_oci_num_rows)
	PHP_FE(oci_close,					arginfo_oci_close)
	PHP_FE(oci_connect,					arginfo_oci_connect)
	PHP_FE(oci_new_connect,				arginfo_oci_new_connect)
	PHP_FE(oci_pconnect,				arginfo_oci_pconnect)
	PHP_FE(oci_error,					arginfo_oci_error)
	PHP_FE(oci_free_descriptor,			arginfo_oci_free_descriptor)
	PHP_FE(oci_lob_save,				arginfo_oci_lob_save)
	PHP_FE(oci_lob_import,				arginfo_oci_lob_import)
	PHP_FE(oci_lob_size,				arginfo_oci_lob_size)
	PHP_FE(oci_lob_load,				arginfo_oci_lob_load)
	PHP_FE(oci_lob_read,				arginfo_oci_lob_read)
	PHP_FE(oci_lob_eof,					arginfo_oci_lob_eof)
	PHP_FE(oci_lob_tell,				arginfo_oci_lob_tell)
	PHP_FE(oci_lob_truncate,			arginfo_oci_lob_truncate)
	PHP_FE(oci_lob_erase,				arginfo_oci_lob_erase)
	PHP_FE(oci_lob_flush,				arginfo_oci_lob_flush)
	PHP_FE(ocisetbufferinglob,			arginfo_ocisetbufferinglob)
	PHP_FE(ocigetbufferinglob,			arginfo_ocigetbufferinglob)
	PHP_FE(oci_lob_is_equal,			arginfo_oci_lob_is_equal)
	PHP_FE(oci_lob_rewind,				arginfo_oci_lob_rewind)
	PHP_FE(oci_lob_write,				arginfo_oci_lob_write)
	PHP_FE(oci_lob_append,				arginfo_oci_lob_append)
	PHP_FE(oci_lob_copy,				arginfo_oci_lob_copy)
	PHP_FE(oci_lob_export,				arginfo_oci_lob_export)
	PHP_FE(oci_lob_seek,				arginfo_oci_lob_seek)
	PHP_FE(oci_commit,					arginfo_oci_commit)
	PHP_FE(oci_rollback,				arginfo_oci_rollback)
	PHP_FE(oci_new_descriptor,			arginfo_oci_new_descriptor)
	PHP_FE(oci_set_prefetch,			arginfo_oci_set_prefetch)
	PHP_FE(oci_set_client_identifier,	arginfo_oci_set_client_identifier)
#ifdef WAITIING_ORACLE_BUG_16695981_FIX
	PHP_FE(oci_set_db_operation,		arginfo_oci_set_db_operation)
#endif
	PHP_FE(oci_set_edition,				arginfo_oci_set_edition)
	PHP_FE(oci_set_module_name,			arginfo_oci_set_module_name)
	PHP_FE(oci_set_action,				arginfo_oci_set_action)
	PHP_FE(oci_set_client_info,			arginfo_oci_set_client_info)
	PHP_FE(oci_password_change,			arginfo_oci_password_change)
	PHP_FE(oci_free_collection,			arginfo_oci_free_collection)
	PHP_FE(oci_collection_append,		arginfo_oci_collection_append)
	PHP_FE(oci_collection_element_get,	arginfo_oci_collection_element_get)
	PHP_FE(oci_collection_element_assign,	arginfo_oci_collection_element_assign)
	PHP_FE(oci_collection_assign,		arginfo_oci_collection_assign)
	PHP_FE(oci_collection_size,			arginfo_oci_collection_size)
	PHP_FE(oci_collection_max,			arginfo_oci_collection_max)
	PHP_FE(oci_collection_trim,			arginfo_oci_collection_trim)
	PHP_FE(oci_new_collection,			arginfo_oci_new_collection)

	PHP_FALIAS(oci_free_cursor,		oci_free_statement,		arginfo_oci_free_statement)
	PHP_FALIAS(ocifreecursor,		oci_free_statement,		arginfo_oci_free_statement)
	PHP_FALIAS(ocibindbyname,		oci_bind_by_name,		arginfo_oci_bind_by_name)
	PHP_FALIAS(ocidefinebyname,		oci_define_by_name,		arginfo_oci_define_by_name)
	PHP_FALIAS(ocicolumnisnull,		oci_field_is_null,		arginfo_oci_field_is_null)
	PHP_FALIAS(ocicolumnname,		oci_field_name,			arginfo_oci_field_name)
	PHP_FALIAS(ocicolumnsize,		oci_field_size,			arginfo_oci_field_size)
	PHP_FALIAS(ocicolumnscale,		oci_field_scale,		arginfo_oci_field_scale)
	PHP_FALIAS(ocicolumnprecision,	oci_field_precision,	arginfo_oci_field_precision)
	PHP_FALIAS(ocicolumntype,		oci_field_type,			arginfo_oci_field_type)
	PHP_FALIAS(ocicolumntyperaw,	oci_field_type_raw,		arginfo_oci_field_type_raw)
	PHP_FALIAS(ociexecute,			oci_execute,			arginfo_oci_execute)
	PHP_FALIAS(ocicancel,			oci_cancel,				arginfo_oci_cancel)
	PHP_FALIAS(ocifetch,			oci_fetch,				arginfo_oci_fetch)
	PHP_FALIAS(ocifetchstatement,	oci_fetch_all,			arginfo_oci_fetch_all)
	PHP_FALIAS(ocifreestatement,	oci_free_statement,		arginfo_oci_free_statement)
	PHP_FALIAS(ociinternaldebug,	oci_internal_debug,		arginfo_oci_internal_debug)
	PHP_FALIAS(ocinumcols,			oci_num_fields,			arginfo_oci_num_fields)
	PHP_FALIAS(ociparse,			oci_parse,				arginfo_oci_parse)
	PHP_FALIAS(ocinewcursor,		oci_new_cursor,			arginfo_oci_new_cursor)
	PHP_FALIAS(ociresult,			oci_result,				arginfo_oci_result)
	PHP_FALIAS(ociserverversion,	oci_server_version,		arginfo_oci_server_version)
	PHP_FALIAS(ocistatementtype,	oci_statement_type,		arginfo_oci_statement_type)
	PHP_FALIAS(ocirowcount,			oci_num_rows,			arginfo_oci_num_rows)
	PHP_FALIAS(ocilogoff,			oci_close,				arginfo_oci_close)
	PHP_FALIAS(ocilogon,			oci_connect,			arginfo_oci_connect)
	PHP_FALIAS(ocinlogon,			oci_new_connect,		arginfo_oci_new_connect)
	PHP_FALIAS(ociplogon,			oci_pconnect,			arginfo_oci_pconnect)
	PHP_FALIAS(ocierror,			oci_error,				arginfo_oci_error)
	PHP_FALIAS(ocifreedesc,			oci_free_descriptor,	arginfo_oci_free_descriptor)
	PHP_FALIAS(ocisavelob,			oci_lob_save,			arginfo_oci_lob_save)
	PHP_FALIAS(ocisavelobfile,		oci_lob_import,			arginfo_oci_lob_import)
	PHP_FALIAS(ociwritelobtofile,	oci_lob_export,			arginfo_oci_lob_export)
	PHP_FALIAS(ociloadlob,			oci_lob_load,			arginfo_oci_lob_load)
	PHP_FALIAS(ocicommit,			oci_commit,				arginfo_oci_commit)
	PHP_FALIAS(ocirollback,			oci_rollback,			arginfo_oci_rollback)
	PHP_FALIAS(ocinewdescriptor,	oci_new_descriptor,		arginfo_oci_new_descriptor)
	PHP_FALIAS(ocisetprefetch,		oci_set_prefetch,		arginfo_oci_set_prefetch)
	PHP_FALIAS(ocipasswordchange,	oci_password_change,	arginfo_oci_password_change)
	PHP_FALIAS(ocifreecollection,	oci_free_collection,	arginfo_oci_free_collection)
	PHP_FALIAS(ocinewcollection,	oci_new_collection,		arginfo_oci_new_collection)
	PHP_FALIAS(ocicollappend,		oci_collection_append,	arginfo_oci_collection_append)
	PHP_FALIAS(ocicollgetelem,		oci_collection_element_get,		arginfo_oci_collection_element_get)
	PHP_FALIAS(ocicollassignelem,	oci_collection_element_assign,	arginfo_oci_collection_element_assign)
	PHP_FALIAS(ocicollsize,			oci_collection_size,	arginfo_oci_collection_size)
	PHP_FALIAS(ocicollmax,			oci_collection_max,		arginfo_oci_collection_max)
	PHP_FALIAS(ocicolltrim,			oci_collection_trim,	arginfo_oci_collection_trim)
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 4) || (PHP_MAJOR_VERSION > 5)
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2) || (PHP_MAJOR_VERSION > 5)
/* This "if" allows PECL builds from this file to be portable to older PHP releases */
const
#endif
zend_function_entry php_oci_lob_class_functions[] = {
	PHP_FALIAS(load,		oci_lob_load,			arginfo_oci_lob_load_method)
	PHP_FALIAS(tell,		oci_lob_tell,			arginfo_oci_lob_tell_method)
	PHP_FALIAS(truncate,	oci_lob_truncate,		arginfo_oci_lob_truncate_method)
	PHP_FALIAS(erase,		oci_lob_erase,			arginfo_oci_lob_erase_method)
	PHP_FALIAS(flush,		oci_lob_flush,			arginfo_oci_lob_flush_method)
	PHP_FALIAS(setbuffering,ocisetbufferinglob,		arginfo_oci_lob_setbuffering_method)
	PHP_FALIAS(getbuffering,ocigetbufferinglob,		arginfo_oci_lob_getbuffering_method)
	PHP_FALIAS(rewind,		oci_lob_rewind,			arginfo_oci_lob_rewind_method)
	PHP_FALIAS(read,		oci_lob_read,			arginfo_oci_lob_read_method)
	PHP_FALIAS(eof,			oci_lob_eof,			arginfo_oci_lob_eof_method)
	PHP_FALIAS(seek,		oci_lob_seek,			arginfo_oci_lob_seek_method)
	PHP_FALIAS(write,		oci_lob_write,			arginfo_oci_lob_write_method)
	PHP_FALIAS(append,		oci_lob_append,			arginfo_oci_lob_append_method)
	PHP_FALIAS(size,		oci_lob_size,			arginfo_oci_lob_size_method)
	PHP_FALIAS(writetofile, oci_lob_export,			arginfo_oci_lob_export_method)
	PHP_FALIAS(export,		oci_lob_export,			arginfo_oci_lob_export_method)
	PHP_FALIAS(import,		oci_lob_import,			arginfo_oci_lob_import_method)
	PHP_FALIAS(writetemporary,	oci_lob_write_temporary,	arginfo_oci_lob_write_temporary_method)
	PHP_FALIAS(close,			oci_lob_close,				arginfo_oci_lob_close_method)
	PHP_FALIAS(save,		oci_lob_save,			arginfo_oci_lob_save_method)
	PHP_FALIAS(savefile,	oci_lob_import,			arginfo_oci_lob_import_method)
	PHP_FALIAS(free,		oci_free_descriptor,	arginfo_oci_free_descriptor_method)
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 4) || (PHP_MAJOR_VERSION > 5)
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2) || (PHP_MAJOR_VERSION > 5)
/* This "if" allows PECL builds from this file to be portable to older PHP releases */
const
#endif
zend_function_entry php_oci_coll_class_functions[] = {
	PHP_FALIAS(append,		  oci_collection_append,			arginfo_oci_collection_append_method)
	PHP_FALIAS(getelem,		  oci_collection_element_get,		arginfo_oci_collection_element_get_method)
	PHP_FALIAS(assignelem,	  oci_collection_element_assign,	arginfo_oci_collection_element_assign_method)
	PHP_FALIAS(assign,		  oci_collection_assign,			arginfo_oci_collection_assign_method)
	PHP_FALIAS(size,		  oci_collection_size,				arginfo_oci_collection_size_method)
	PHP_FALIAS(max,			  oci_collection_max,				arginfo_oci_collection_max_method)
	PHP_FALIAS(trim,		  oci_collection_trim,				arginfo_oci_collection_trim_method)
	PHP_FALIAS(free,		  oci_free_collection,				arginfo_oci_collection_free_method)
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 4) || (PHP_MAJOR_VERSION > 5)
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

zend_module_entry oci8_module_entry = {
	STANDARD_MODULE_HEADER,
	"oci8",				  /* extension name */
	php_oci_functions,	  /* extension function list */
	PHP_MINIT(oci),		  /* extension-wide startup function */
	PHP_MSHUTDOWN(oci),	  /* extension-wide shutdown function */
	PHP_RINIT(oci),		  /* per-request startup function */
	PHP_RSHUTDOWN(oci),	  /* per-request shutdown function */
	PHP_MINFO(oci),		  /* information function */
	PHP_OCI8_VERSION,
	PHP_MODULE_GLOBALS(oci),  /* globals descriptor */
	PHP_GINIT(oci),			  /* globals ctor */
	PHP_GSHUTDOWN(oci),		  /* globals dtor */
	NULL,					  /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY(	"oci8.max_persistent",			"-1",	PHP_INI_SYSTEM,	ONUPDATELONGFUNC,	max_persistent,			zend_oci_globals,	oci_globals)
	STD_PHP_INI_ENTRY(	"oci8.persistent_timeout",		"-1",	PHP_INI_SYSTEM,	ONUPDATELONGFUNC,	persistent_timeout,		zend_oci_globals,	oci_globals)
	STD_PHP_INI_ENTRY(	"oci8.ping_interval",			"60",	PHP_INI_SYSTEM,	ONUPDATELONGFUNC,	ping_interval,			zend_oci_globals,	oci_globals)
	STD_PHP_INI_BOOLEAN("oci8.privileged_connect",		"0",	PHP_INI_SYSTEM,	OnUpdateBool,		privileged_connect,		zend_oci_globals,	oci_globals)
	STD_PHP_INI_ENTRY(	"oci8.statement_cache_size",	"20",	PHP_INI_SYSTEM,	ONUPDATELONGFUNC,	statement_cache_size,	zend_oci_globals,	oci_globals)
	STD_PHP_INI_ENTRY(	"oci8.default_prefetch",		"100",	PHP_INI_SYSTEM,	ONUPDATELONGFUNC,	default_prefetch,		zend_oci_globals,	oci_globals)
	STD_PHP_INI_BOOLEAN("oci8.old_oci_close_semantics",	"0",	PHP_INI_SYSTEM,	OnUpdateBool,		old_oci_close_semantics,zend_oci_globals,	oci_globals)
#if (OCI_MAJOR_VERSION >= 11)
	STD_PHP_INI_ENTRY(	"oci8.connection_class",		"",		PHP_INI_ALL,	OnUpdateString,		connection_class,		zend_oci_globals,	oci_globals)
#endif
#if ((OCI_MAJOR_VERSION > 10) || ((OCI_MAJOR_VERSION == 10) && (OCI_MINOR_VERSION >= 2)))
	STD_PHP_INI_BOOLEAN("oci8.events",					"0",	PHP_INI_SYSTEM,	OnUpdateBool,		events,					zend_oci_globals,	oci_globals)
#endif
PHP_INI_END()
/* }}} */

/* {{{ startup, shutdown and info functions
*/

/* {{{	php_oci_init_global_handles()
 *
 * Initialize global handles only when they are needed
 */
static void php_oci_init_global_handles(void)
{
	sword errstatus;
	sb4   ora_error_code = 0;
	text  tmp_buf[OCI_ERROR_MAXMSG_SIZE];  /* Use traditional smaller size: non-PL/SQL errors should fit and it keeps the stack smaller */

	errstatus = OCIEnvNlsCreate(&OCI_G(env), PHP_OCI_INIT_MODE, 0, NULL, NULL, NULL, 0, NULL, 0, 0);

	if (errstatus == OCI_ERROR) {
#ifdef HAVE_OCI_INSTANT_CLIENT
		php_error_docref(NULL, E_WARNING, "OCIEnvNlsCreate() failed. There is something wrong with your system - please check that " PHP_OCI8_LIB_PATH_MSG " includes the directory with Oracle Instant Client libraries");
#else
		php_error_docref(NULL, E_WARNING, "OCIEnvNlsCreate() failed. There is something wrong with your system - please check that ORACLE_HOME and " PHP_OCI8_LIB_PATH_MSG " are set and point to the right directories");
#endif
		if (OCI_G(env)
			&& OCIErrorGet(OCI_G(env), (ub4)1, NULL, &ora_error_code, tmp_buf, (ub4)OCI_ERROR_MAXMSG_SIZE, (ub4)OCI_HTYPE_ENV) == OCI_SUCCESS
			&& *tmp_buf) {
			php_error_docref(NULL, E_WARNING, "%s", tmp_buf);
		}

		OCI_G(env) = NULL;
		OCI_G(err) = NULL;
		return;
	}

	errstatus = OCIHandleAlloc (OCI_G(env), (dvoid **)&OCI_G(err), OCI_HTYPE_ERROR, 0, NULL);

	if (errstatus == OCI_SUCCESS) {
#if !defined(OCI_MAJOR_VERSION) || (OCI_MAJOR_VERSION < 11)
		/* This fixes PECL bug 15988 (sqlnet.ora not being read).  The
		 * root cause was fixed in Oracle 10.2.0.4 but there is no
		 * compile time method to check for that precise patch level,
		 * nor can it be guaranteed that runtime will use the same
		 * patch level the code was compiled with.  So, we do this
		 * code for all non 11g versions.
		 */
		OCICPool *cpoolh;
		ub4 cpoolmode = 0x80000000;	/* Pass invalid mode to OCIConnectionPoolCreate */
		PHP_OCI_CALL(OCIHandleAlloc, (OCI_G(env), (dvoid **) &cpoolh, OCI_HTYPE_CPOOL, (size_t) 0, (dvoid **) 0));
		PHP_OCI_CALL(OCIConnectionPoolCreate, (OCI_G(env), OCI_G(err), cpoolh, NULL, 0, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0, cpoolmode));
		PHP_OCI_CALL(OCIConnectionPoolDestroy, (cpoolh, OCI_G(err), OCI_DEFAULT));
		PHP_OCI_CALL(OCIHandleFree, (cpoolh, OCI_HTYPE_CPOOL));
#endif
	} else {
		OCIErrorGet(OCI_G(env), (ub4)1, NULL, &ora_error_code, tmp_buf, (ub4)OCI_ERROR_MAXMSG_SIZE, (ub4)OCI_HTYPE_ERROR);

		if (ora_error_code) {
			int tmp_buf_len = strlen((char *)tmp_buf);

			if (tmp_buf_len > 0 && tmp_buf[tmp_buf_len - 1] == '\n') {
				tmp_buf[tmp_buf_len - 1] = '\0';
			}

			if (errstatus == OCI_SUCCESS_WITH_INFO) {
				php_error_docref(NULL, E_WARNING, "Initialization error: OCI_SUCCESS_WITH_INFO: %s", tmp_buf);
			} else {
				php_error_docref(NULL, E_WARNING, "Initialization error: OCI_ERROR: %s", tmp_buf);

				OCIHandleFree((dvoid *) OCI_G(env), OCI_HTYPE_ENV);

				OCI_G(env) = NULL;
				OCI_G(err) = NULL;
			}
		}
	}
}
/* }}} */

/* {{{ php_oci_cleanup_global_handles()
 *
 * Free global handles (if they were initialized before)
 */
static void php_oci_cleanup_global_handles(void)
{
	if (OCI_G(err)) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) OCI_G(err), OCI_HTYPE_ERROR));
		OCI_G(err) = NULL;
	}

	if (OCI_G(env)) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) OCI_G(env), OCI_HTYPE_ENV));
		OCI_G(env) = NULL;
	}
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
 *
 * Zerofill globals during module init
 */
static PHP_GINIT_FUNCTION(oci)
{
	memset(oci_globals, 0, sizeof(zend_oci_globals));
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION
 *
 * Called for thread shutdown in ZTS, after module shutdown for non-ZTS
 */
static PHP_GSHUTDOWN_FUNCTION(oci)
{
	php_oci_cleanup_global_handles();
}
/* }}} */

PHP_MINIT_FUNCTION(oci)
{
	zend_class_entry oci_lob_class_entry;
	zend_class_entry oci_coll_class_entry;

	REGISTER_INI_ENTRIES();

	le_statement = zend_register_list_destructors_ex(php_oci_statement_list_dtor, NULL, "oci8 statement", module_number);
	le_connection = zend_register_list_destructors_ex(php_oci_connection_list_dtor, NULL, "oci8 connection", module_number);
	le_pconnection = zend_register_list_destructors_ex(php_oci_pconnection_list_np_dtor, php_oci_pconnection_list_dtor, "oci8 persistent connection", module_number);
	le_psessionpool = zend_register_list_destructors_ex(NULL, php_oci_spool_list_dtor, "oci8 persistent session pool", module_number);
	le_descriptor = zend_register_list_destructors_ex(php_oci_descriptor_list_dtor, NULL, "oci8 descriptor", module_number);
	le_collection = zend_register_list_destructors_ex(php_oci_collection_list_dtor, NULL, "oci8 collection", module_number);

	INIT_CLASS_ENTRY(oci_lob_class_entry, "OCI-Lob", php_oci_lob_class_functions);
	INIT_CLASS_ENTRY(oci_coll_class_entry, "OCI-Collection", php_oci_coll_class_functions);

	oci_lob_class_entry_ptr = zend_register_internal_class(&oci_lob_class_entry);
	oci_coll_class_entry_ptr = zend_register_internal_class(&oci_coll_class_entry);

/* thies@thieso.net 990203 i do not think that we will need all of them - just in here for completeness for now! */
	REGISTER_LONG_CONSTANT("OCI_DEFAULT",OCI_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SYSOPER",OCI_SYSOPER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SYSDBA",OCI_SYSDBA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_CRED_EXT",PHP_OCI_CRED_EXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DESCRIBE_ONLY",OCI_DESCRIBE_ONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_COMMIT_ON_SUCCESS",OCI_COMMIT_ON_SUCCESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_NO_AUTO_COMMIT",OCI_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_EXACT_FETCH",OCI_EXACT_FETCH, CONST_CS | CONST_PERSISTENT);

/* for $LOB->seek() */
	REGISTER_LONG_CONSTANT("OCI_SEEK_SET",PHP_OCI_SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_CUR",PHP_OCI_SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_END",PHP_OCI_SEEK_END, CONST_CS | CONST_PERSISTENT);

/*	for $LOB->flush() */
	REGISTER_LONG_CONSTANT("OCI_LOB_BUFFER_FREE",OCI_LOB_BUFFER_FREE, CONST_CS | CONST_PERSISTENT);

/* for OCIBindByName (real "oci" names + short "php" names */
	REGISTER_LONG_CONSTANT("SQLT_BFILEE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_RDD",SQLT_RDD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_INT",SQLT_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_NUM",SQLT_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_RSET",SQLT_RSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_AFC",SQLT_AFC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CHR",SQLT_CHR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_VCS",SQLT_VCS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_AVC",SQLT_AVC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_STR",SQLT_STR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_LVC",SQLT_LVC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_FLT",SQLT_FLT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_UIN",SQLT_UIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_LNG",SQLT_LNG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_LBI",SQLT_LBI, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BIN",SQLT_BIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_ODT",SQLT_ODT, CONST_CS | CONST_PERSISTENT);
#if defined(HAVE_OCI_INSTANT_CLIENT) || (defined(OCI_MAJOR_VERSION) && OCI_MAJOR_VERSION >= 10)
	REGISTER_LONG_CONSTANT("SQLT_BDOUBLE",SQLT_BDOUBLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BFLOAT",SQLT_BFLOAT, CONST_CS | CONST_PERSISTENT);
#endif
#if defined(OCI_MAJOR_VERSION) && OCI_MAJOR_VERSION >= 12
	REGISTER_LONG_CONSTANT("SQLT_BOL",SQLT_BOL, CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("OCI_B_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("OCI_SYSDATE","SYSDATE", CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("OCI_B_BFILE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_ROWID",SQLT_RDD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CURSOR",SQLT_RSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BIN",SQLT_BIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_INT",SQLT_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_NUM",SQLT_NUM, CONST_CS | CONST_PERSISTENT);
#if defined(OCI_MAJOR_VERSION) && OCI_MAJOR_VERSION >= 12
	REGISTER_LONG_CONSTANT("OCI_B_BOL",SQLT_BOL, CONST_CS | CONST_PERSISTENT);
#endif

/* for OCIFetchStatement */
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_COLUMN", PHP_OCI_FETCHSTATEMENT_BY_COLUMN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_ROW", PHP_OCI_FETCHSTATEMENT_BY_ROW, CONST_CS | CONST_PERSISTENT);

/* for OCIFetchInto & OCIResult */
	REGISTER_LONG_CONSTANT("OCI_ASSOC",PHP_OCI_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_NUM",PHP_OCI_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_BOTH",PHP_OCI_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_NULLS",PHP_OCI_RETURN_NULLS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_LOBS",PHP_OCI_RETURN_LOBS, CONST_CS | CONST_PERSISTENT);

/* for OCINewDescriptor (real "oci" names + short "php" names */
	REGISTER_LONG_CONSTANT("OCI_DTYPE_FILE",OCI_DTYPE_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_LOB",OCI_DTYPE_LOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_ROWID",OCI_DTYPE_ROWID, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("OCI_D_FILE",OCI_DTYPE_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_LOB",OCI_DTYPE_LOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_ROWID",OCI_DTYPE_ROWID, CONST_CS | CONST_PERSISTENT);

/* for OCIWriteTemporaryLob */
	REGISTER_LONG_CONSTANT("OCI_TEMP_CLOB",OCI_TEMP_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_TEMP_BLOB",OCI_TEMP_BLOB, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(oci)
{
	OCI_G(num_links) = OCI_G(num_persistent);
	OCI_G(errcode) = 0;
	OCI_G(edition) = NULL;

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(oci)
{
	OCI_G(shutdown) = 1;

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(oci)
{
	/* Check persistent connections and do the necessary actions if needed. If persistent_helper is
	 * unable to process a pconnection because of a refcount, the processing would happen from
	 * np-destructor which is called when refcount goes to zero - php_oci_pconnection_list_np_dtor
	 */
	zend_hash_apply(&EG(persistent_list), (apply_func_t) php_oci_persistent_helper);

	if (OCI_G(edition)) {
		efree(OCI_G(edition));
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(oci)
{
	char buf[32];
#if ((OCI_MAJOR_VERSION > 10) || ((OCI_MAJOR_VERSION == 10) && (OCI_MINOR_VERSION >= 2)))
	char *ver;
#endif

	php_info_print_table_start();
	php_info_print_table_row(2, "OCI8 Support", "enabled");
#if defined(HAVE_OCI8_DTRACE)
	php_info_print_table_row(2, "OCI8 DTrace Support", "enabled");
#else
	php_info_print_table_row(2, "OCI8 DTrace Support", "disabled");
#endif
	php_info_print_table_row(2, "OCI8 Version", PHP_OCI8_VERSION);
	php_info_print_table_row(2, "Revision", "$Id$");

#if ((OCI_MAJOR_VERSION > 10) || ((OCI_MAJOR_VERSION == 10) && (OCI_MINOR_VERSION >= 2)))
	php_oci_client_get_version(&ver);
	php_info_print_table_row(2, "Oracle Run-time Client Library Version", ver);
	efree(ver);
#else
	php_info_print_table_row(2, "Oracle Run-time Client Library Version", "Unknown");
#endif
#if	defined(OCI_MAJOR_VERSION) && defined(OCI_MINOR_VERSION)
	snprintf(buf, sizeof(buf), "%d.%d", OCI_MAJOR_VERSION, OCI_MINOR_VERSION);
#elif defined(PHP_OCI8_ORACLE_VERSION)
	snprintf(buf, sizeof(buf), "%s", PHP_OCI8_ORACLE_VERSION);
#else
	snprintf(buf, sizeof(buf), "Unknown");
#endif
#if defined(HAVE_OCI_INSTANT_CLIENT)
	php_info_print_table_row(2, "Oracle Compile-time Instant Client Version", buf);
#else
	php_info_print_table_row(2, "Oracle Compile-time Version", buf);
#endif

#if !defined(PHP_WIN32) && !defined(HAVE_OCI_INSTANT_CLIENT)
#if defined(PHP_OCI8_DEF_DIR)
	php_info_print_table_row(2, "Compile-time ORACLE_HOME", PHP_OCI8_DEF_DIR);
#endif
#if defined(PHP_OCI8_DEF_SHARED_LIBADD)
	php_info_print_table_row(2, "Libraries Used", PHP_OCI8_DEF_SHARED_LIBADD);
#endif
#endif


	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

	php_info_print_table_start();
	php_info_print_table_header(2, "Statistics", "");
	snprintf(buf, sizeof(buf), "%pd", OCI_G(num_persistent));
	php_info_print_table_row(2, "Active Persistent Connections", buf);
	snprintf(buf, sizeof(buf), "%pd", OCI_G(num_links));
	php_info_print_table_row(2, "Active Connections", buf);
	php_info_print_table_end();
}
/* }}} */

/* {{{ list destructors */

/* {{{ php_oci_connection_list_dtor()
 *
 * Non-persistent connection destructor
 */
static void php_oci_connection_list_dtor(zend_resource *entry)
{
	php_oci_connection *connection = (php_oci_connection *)entry->ptr;

	if (connection) {
		php_oci_connection_close(connection);
		OCI_G(num_links)--;
	}
}
/* }}} */

/* {{{ php_oci_pconnection_list_dtor()
 *
 * Persistent connection destructor
 */
static void php_oci_pconnection_list_dtor(zend_resource *entry)
{
	php_oci_connection *connection = (php_oci_connection *)entry->ptr;

	if (connection) {
		php_oci_connection_close(connection);
		OCI_G(num_persistent)--;
		OCI_G(num_links)--;
	}
}
/* }}} */

/* {{{ php_oci_pconnection_list_np_dtor()
 *
 * Non-Persistent destructor for persistent connection - This gets invoked when
 * the refcount of this goes to zero in the regular list
 */
static void php_oci_pconnection_list_np_dtor(zend_resource *entry)
{
	php_oci_connection *connection = (php_oci_connection *)entry->ptr;
	zval *zvp;
	zend_resource *le;

	/*
	 * We cannot get connection as NULL or as a stub in this function. This is the function that
	 * turns a pconnection to a stub
	 *
	 * If oci_password_change() changed the password of a persistent connection, close the
	 * connection and remove it from the persistent connection cache.  This means subsequent scripts
	 * will be prevented from being able to present the old (now invalid) password to a usable
	 * connection to the database; they must use the new password.
	 *
	 * Check for conditions that warrant removal of the hash entry
	 */

	if (!connection->is_open ||
		connection->passwd_changed ||
		(PG(connection_status) & PHP_CONNECTION_TIMEOUT) ||
		OCI_G(in_call)) {

		/* Remove the hash entry if present */
		zvp = zend_hash_find(&EG(persistent_list), connection->hash_key);
		le = Z_RES_P(zvp); 		/* PHPNG TODO check for null zvp */
		if (le != NULL && le->type == le_pconnection && le->ptr == connection) {
			zend_hash_del(&EG(persistent_list), connection->hash_key);
		}
		else {
			php_oci_connection_close(connection);
			OCI_G(num_persistent)--;
		}

#ifdef HAVE_OCI8_DTRACE
		if (DTRACE_OCI8_CONNECT_P_DTOR_CLOSE_ENABLED()) {
			DTRACE_OCI8_CONNECT_P_DTOR_CLOSE(connection);
		}
#endif /* HAVE_OCI8_DTRACE */
	} else {
		/*
		 * Release the connection to underlying pool.  We do this unconditionally so that
		 * out-of-scope pconnects are now consistent with oci_close and out-of-scope new connect
		 * semantics. With the PECL OCI 1.3.x extensions, we release pconnections when oci_close
		 * takes the refcount to zero.
		 *
		 * If oci_old_close_semantics is set, we artificially bump up the refcount and decremented
		 * only at request shutdown.
		 */
		php_oci_connection_release(connection);

#ifdef HAVE_OCI8_DTRACE
		if (DTRACE_OCI8_CONNECT_P_DTOR_RELEASE_ENABLED()) {
			DTRACE_OCI8_CONNECT_P_DTOR_RELEASE(connection);
		}
#endif /* HAVE_OCI8_DTRACE */
	}
}
/* }}} */

/* {{{ php_oci_statement_list_dtor()
 *
 * Statement destructor
 */
static void php_oci_statement_list_dtor(zend_resource *entry)
{
	php_oci_statement *statement = (php_oci_statement *)entry->ptr;
	php_oci_statement_free(statement);
}
/* }}} */

/* {{{ php_oci_descriptor_list_dtor()
 *
 *	Descriptor destructor
 */
static void php_oci_descriptor_list_dtor(zend_resource *entry)
{
	php_oci_descriptor *descriptor = (php_oci_descriptor *)entry->ptr;
	php_oci_lob_free(descriptor);
}
/* }}} */

/* {{{ php_oci_collection_list_dtor()
 *
 * Collection destructor
 */
static void php_oci_collection_list_dtor(zend_resource *entry)
{
	php_oci_collection *collection = (php_oci_collection *)entry->ptr;
	php_oci_collection_close(collection);
}
/* }}} */

/* }}} */

/* {{{ Hash Destructors */

/* {{{ php_oci_define_hash_dtor()
 *
 * Define hash destructor
 */
void php_oci_define_hash_dtor(void *data)
{
	php_oci_define *define = (php_oci_define *) data;

	zval_ptr_dtor(&define->zval);

	if (define->name) {
		efree(define->name);
		define->name = NULL;
	}
}
/* }}} */

/* {{{ php_oci_bind_hash_dtor()
 *
 * Bind hash destructor
 */
void php_oci_bind_hash_dtor(void *data)
{
	php_oci_bind *bind = (php_oci_bind *) data;

	if (bind->array.elements) {
		efree(bind->array.elements);
	}

	if (bind->array.element_lengths) {
		efree(bind->array.element_lengths);
	}

	if (bind->array.indicators) {
		efree(bind->array.indicators);
	}

	zval_ptr_dtor(&bind->zval);
}
/* }}} */

/* {{{ php_oci_column_hash_dtor()
 *
 * Column hash destructor
 */
void php_oci_column_hash_dtor(void *data)
{
	php_oci_out_column *column = (php_oci_out_column *) data;

	/* if (column->stmtid) { */ /* PHPNG TODO */
		zend_list_close(column->stmtid);
	/* } */

	/* if (column->is_descr) { */ /* PHPNG TODO */
		zend_list_close(column->descid);
	/* } */

	if (column->data) {
		efree(column->data);
	}

	if (column->name) {
		efree(column->name);
	}
}
/* }}} */

/* {{{ php_oci_descriptor_flush_hash_dtor()
 *
 * Flush descriptors on commit
 */
void php_oci_descriptor_flush_hash_dtor(void *data)
{
	php_oci_descriptor *descriptor = *(php_oci_descriptor **)data;

	if (descriptor && descriptor->buffering == PHP_OCI_LOB_BUFFER_USED && (descriptor->type == OCI_DTYPE_LOB || descriptor->type == OCI_DTYPE_FILE)) {
		php_oci_lob_flush(descriptor, OCI_LOB_BUFFER_FREE);
		descriptor->buffering = PHP_OCI_LOB_BUFFER_ENABLED;
	}
	data = NULL;
}
/* }}} */

/* }}} */

/* {{{ php_oci_connection_descriptors_free()
 *
 * Free descriptors for a connection
 */
void php_oci_connection_descriptors_free(php_oci_connection *connection)
{
	zend_hash_destroy(connection->descriptors);
	efree(connection->descriptors);
	connection->descriptors = NULL;
	connection->descriptor_count = 0;
}
/* }}} */

/* {{{ php_oci_error()
 *
 * Fetch & print out error message if we get an error
 * Returns an Oracle error number
 */
sb4 php_oci_error(OCIError *err_p, sword errstatus)
{
	text *errbuf = (text *)NULL;
	sb4 errcode = 0; /* Oracle error number */

	switch (errstatus) {
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
			errcode = php_oci_fetch_errmsg(err_p, &errbuf);
			if (errbuf) {
				php_error_docref(NULL, E_WARNING, "OCI_SUCCESS_WITH_INFO: %s", errbuf);
				efree(errbuf);
			} else {
				php_error_docref(NULL, E_WARNING, "OCI_SUCCESS_WITH_INFO: failed to fetch error message");
			}
			break;
		case OCI_NEED_DATA:
			php_error_docref(NULL, E_WARNING, "OCI_NEED_DATA");
			break;
		case OCI_NO_DATA:
			errcode = php_oci_fetch_errmsg(err_p, &errbuf);
			if (errbuf) {
				php_error_docref(NULL, E_WARNING, "%s", errbuf);
				efree(errbuf);
			} else {
				php_error_docref(NULL, E_WARNING, "OCI_NO_DATA: failed to fetch error message");
			}
			break;
		case OCI_ERROR:
			errcode = php_oci_fetch_errmsg(err_p, &errbuf);
			if (errbuf) {
				php_error_docref(NULL, E_WARNING, "%s", errbuf);
				efree(errbuf);
			} else {
				php_error_docref(NULL, E_WARNING, "failed to fetch error message");
			}
			break;
		case OCI_INVALID_HANDLE:
			php_error_docref(NULL, E_WARNING, "OCI_INVALID_HANDLE");
			break;
		case OCI_STILL_EXECUTING:
			php_error_docref(NULL, E_WARNING, "OCI_STILL_EXECUTING");
			break;
		case OCI_CONTINUE:
			php_error_docref(NULL, E_WARNING, "OCI_CONTINUE");
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown OCI error code: %d", errstatus);
			break;
	}

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_ERROR_ENABLED()) {
		DTRACE_OCI8_ERROR((int)errstatus, (long)errcode);
	}
#endif /* HAVE_OCI8_DTRACE */

	return errcode;
}
/* }}} */

/* {{{ php_oci_fetch_errmsg()
 *
 * Fetch error message into the buffer from the error handle provided
 */
sb4 php_oci_fetch_errmsg(OCIError *error_handle, text **error_buf)
{
	sb4 error_code = 0;
	text err_buf[PHP_OCI_ERRBUF_LEN];

	memset(err_buf, 0, sizeof(err_buf));
	PHP_OCI_CALL(OCIErrorGet, (error_handle, (ub4)1, NULL, &error_code, err_buf, (ub4)PHP_OCI_ERRBUF_LEN, (ub4)OCI_HTYPE_ERROR));

	if (error_code) {
		int err_buf_len = strlen((char *)err_buf);

		if (err_buf_len && err_buf[err_buf_len - 1] == '\n') {
			err_buf[err_buf_len - 1] = '\0';
		}
		if (err_buf_len && error_buf) {
			*error_buf = NULL;
			*error_buf = (text *)estrndup((char *)err_buf, err_buf_len);
		}
	}
	return error_code;
}
/* }}} */

/* {{{ php_oci_fetch_sqltext_offset()
 *
 * Compute offset in the SQL statement
 */
int php_oci_fetch_sqltext_offset(php_oci_statement *statement, text **sqltext, ub2 *error_offset)
{
	sword errstatus;

	*sqltext = NULL;
	*error_offset = 0;
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (dvoid *) sqltext, (ub4 *)0, OCI_ATTR_STATEMENT, statement->err));

	if (errstatus != OCI_SUCCESS) {
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		return 1;
	}

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (ub2 *)error_offset, (ub4 *)0, OCI_ATTR_PARSE_ERROR_OFFSET, statement->err));

	if (errstatus != OCI_SUCCESS) {
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ php_oci_do_connect()
 *
 * Connect wrapper
 */
void php_oci_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent, int exclusive)
{
	php_oci_connection *connection;
	char *username, *password;
	char *dbname = NULL, *charset = NULL;
	size_t username_len = 0, password_len = 0;
	size_t dbname_len = 0, charset_len = 0;
	zend_long session_mode = OCI_DEFAULT;

	/* if a fourth parameter is handed over, it is the charset identifier (but is only used in Oracle 9i+) */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|ssl", &username, &username_len, &password, &password_len, &dbname, &dbname_len, &charset, &charset_len, &session_mode) == FAILURE) {
		return;
	}

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_CONNECT_ENTRY_ENABLED()) {
		DTRACE_OCI8_CONNECT_ENTRY(username, dbname, charset, session_mode, persistent, exclusive);
	}
#endif /* HAVE_OCI8_DTRACE */

	if (!charset_len) {
		charset = NULL;
	}

	connection = php_oci_do_connect_ex(username, username_len, password, password_len, NULL, 0, dbname, dbname_len, charset, session_mode, persistent, exclusive);

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_CONNECT_RETURN_ENABLED()) {
		DTRACE_OCI8_CONNECT_RETURN(connection);
	}
#endif /* HAVE_OCI8_DTRACE */


	if (!connection) {
		RETURN_FALSE;
	}
	RETURN_RES(connection->id);

}
/* }}} */

/* {{{ php_oci_do_connect_ex()
 *
 * The real connect function. Allocates all the resources needed, establishes the connection and
 * returns the result handle (or NULL)
 */
php_oci_connection *php_oci_do_connect_ex(char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, char *dbname, int dbname_len, char *charset, zend_long session_mode, int persistent, int exclusive)
{
	zval *zvp;
	zend_resource *le;
	zend_resource new_le;
	php_oci_connection *connection = NULL;
	smart_str hashed_details = {0};
	time_t timestamp;
	php_oci_spool *session_pool = NULL;
	zend_bool use_spool = 1;	   /* Default is to use client-side session pool */
	zend_bool ping_done = 0;

	ub2 charsetid = 0;
	ub2 charsetid_nls_lang = 0;

	if (session_mode & ~(OCI_SYSOPER | OCI_SYSDBA | PHP_OCI_CRED_EXT)) {
		php_error_docref(NULL, E_WARNING, "Invalid session mode specified (%pd)", session_mode);
		return NULL;
	}
	if (session_mode & (OCI_SYSOPER | OCI_SYSDBA | PHP_OCI_CRED_EXT)) {
		if ((session_mode & OCI_SYSOPER) && (session_mode & OCI_SYSDBA)) {
			php_error_docref(NULL, E_WARNING, "OCI_SYSDBA and OCI_SYSOPER cannot be used together");
			return NULL;
		}
		if (session_mode & PHP_OCI_CRED_EXT) {
#ifdef PHP_WIN32
			/* Disable external authentication on Windows as Impersonation is not yet handled.
			 * TODO: Re-enable this once OCI provides capability.
			 */
			php_error_docref(NULL, E_WARNING, "External Authentication is not supported on Windows");
			return NULL;
#endif
			if (username_len != 1 || username[0] != '/' || password_len != 0) {
				php_error_docref(NULL, E_WARNING, "OCI_CRED_EXT can only be used with a username of \"/\" and a NULL password");
				return NULL;
			}
		}
		if (session_mode & (OCI_SYSOPER | OCI_SYSDBA)) {
			/* Increase security by not caching privileged oci_pconnect() connections. The
			 * connection becomes equivalent to oci_connect() or oci_new_connect().
			 */
			persistent = 0;
			if (!OCI_G(privileged_connect)) {
				php_error_docref(NULL, E_WARNING, "Privileged connect is disabled. Enable oci8.privileged_connect to be able to connect as SYSOPER or SYSDBA");
				return NULL;
			}
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4) || (PHP_MAJOR_VERSION < 5)
			/* Safe mode has been removed in PHP 5.4 */
			if (PG(safe_mode)) {
				php_error_docref(NULL, E_WARNING, "Privileged connect is disabled in Safe Mode");
				return NULL;
			}
#endif
		}
	}

	/* Initialize global handles if they weren't initialized before */
	if (OCI_G(env) == NULL) {
		php_oci_init_global_handles();
		if (OCI_G(env) == NULL) {
			return NULL;
		}
	}

	/* We cannot use the new session create logic (OCISessionGet from
	 * client-side session pool) when privileged connect or password
	 * change is attempted or OCI_CRED_EXT mode is specified.
	 * TODO: Re-enable this when OCI provides support.
	 */
	if ((session_mode & (OCI_SYSOPER | OCI_SYSDBA | PHP_OCI_CRED_EXT)) || (new_password_len)) {
		use_spool = 0;
	}

	smart_str_appendl_ex(&hashed_details, "oci8***", sizeof("oci8***") - 1, 0);
	smart_str_appendl_ex(&hashed_details, username, username_len, 0);
	smart_str_appendl_ex(&hashed_details, "**", sizeof("**") - 1, 0);

	/* DRCP: connection_class is an attribute of a connection */
	if (OCI_G(connection_class)){
		smart_str_appendl_ex(&hashed_details, OCI_G(connection_class), strlen(OCI_G(connection_class)), 0);
	}
	smart_str_appendl_ex(&hashed_details, "**", sizeof("**") - 1, 0);

	/* Add edition attribute to the hash */
	if (OCI_G(edition)){
		smart_str_appendl_ex(&hashed_details, OCI_G(edition), strlen(OCI_G(edition)), 0);
	}
	smart_str_appendl_ex(&hashed_details, "**", sizeof("**") - 1, 0);

	if (password_len) {
		zend_ulong password_hash;
		password_hash = zend_inline_hash_func(password, password_len);
		smart_str_append_unsigned_ex(&hashed_details, password_hash, 0);
	}
	smart_str_appendl_ex(&hashed_details, "**", sizeof("**") - 1, 0);

	if (dbname) {
		smart_str_appendl_ex(&hashed_details, dbname, dbname_len, 0);
	}
	smart_str_appendl_ex(&hashed_details, "**", sizeof("**") - 1, 0);

	if (charset && *charset) {
		PHP_OCI_CALL_RETURN(charsetid, OCINlsCharSetNameToId, (OCI_G(env), (CONST oratext *)charset));
		if (!charsetid) {
			php_error_docref(NULL, E_WARNING, "Invalid character set name: %s", charset);
		} else {
			smart_str_append_unsigned_ex(&hashed_details, charsetid, 0);
		}
	}

	/* use NLS_LANG if no or invalid charset specified */
	if (!charsetid) {
		size_t rsize = 0;
		sword result;

		PHP_OCI_CALL_RETURN(result, OCINlsEnvironmentVariableGet, (&charsetid_nls_lang, 0, OCI_NLS_CHARSET_ID, 0, &rsize));
		if (result != OCI_SUCCESS) {
			charsetid_nls_lang = 0;
		}
		smart_str_append_unsigned_ex(&hashed_details, charsetid_nls_lang, 0);
	}

	timestamp = time(NULL);

	smart_str_append_unsigned_ex(&hashed_details, session_mode, 0);
	smart_str_0(&hashed_details);

	/* make it lowercase */
	/* PHPNG TODO is this safe to do? What about interned strings? */
	php_strtolower(hashed_details.s->val, hashed_details.s->len);

	if (!exclusive && !new_password) {
		zend_bool found = 0;

		/* PHPNG TODO Check hashed_details is used correctly */
		if (persistent && ((zvp = zend_hash_find(&EG(persistent_list), hashed_details.s))) != NULL) {
			zend_resource *le = Z_RES_P(zvp);

			found = 1;
			/* found */
			if (le->type == le_pconnection) {
				connection = (php_oci_connection *)le->ptr;
			}
		} else if (!persistent && ((zvp = zend_hash_find(&EG(regular_list), hashed_details.s)) != NULL)) {
			le = Z_RES_P(zvp);
			found = 1;
			if (le->type == le_index_ptr) {
				int type, link;
				void *ptr;

				ptr = le->ptr; /* PHPNG TODO */
				if (ptr && (le->type == le_connection)) {
					connection = (php_oci_connection *)ptr;
				}
			}
		}

#ifdef HAVE_OCI8_DTRACE
		if (DTRACE_OCI8_CONNECT_LOOKUP_ENABLED()) {
			DTRACE_OCI8_CONNECT_LOOKUP(connection, connection && connection->is_stub ? 1 : 0);
		}
#endif /* HAVE_OCI8_DTRACE */

		/* If we got a pconnection stub, then 'load'(OCISessionGet) the real connection from its
		 * private spool A connection is a stub if it is only a cached structure and the real
		 * connection is released to its underlying private session pool.  We currently do not have
		 * stub support for non-persistent conns.
		 *
		 * TODO: put in negative code for non-persistent stubs
		 */
		if (connection && connection->is_persistent && connection->is_stub) {
			if (php_oci_create_session(connection, NULL, dbname, dbname_len, username, username_len, password, password_len, new_password, new_password_len, session_mode)) {
				smart_str_free(&hashed_details);
				zend_hash_del(&EG(persistent_list), connection->hash_key);

				return NULL;
			}
			/* We do the ping in php_oci_create_session, no need to ping again below */
			ping_done = 1;
		}

		if (connection) {
			if (connection->is_open) {
				/* found an open connection. now ping it */
				if (connection->is_persistent) {
					int rsrc_type;

					/* Check connection liveness in the following order:
					 * 1) always check OCI_ATTR_SERVER_STATUS
					 * 2) see if it's time to ping it
					 * 3) ping it if needed
					 */
					if (php_oci_connection_status(connection)) {
						/* Only ping if:
						 *
						 * 1) next_ping > 0, which means that ping_interval is not -1 (aka "Off")
						 *
						 * 2) current_timestamp > next_ping, which means "it's time to check if it's
						 * still alive"
						 */
						if (!ping_done && (*(connection->next_pingp) > 0) && (timestamp >= *(connection->next_pingp)) && !php_oci_connection_ping(connection)) {
							/* server died */
						} else {
							php_oci_connection *tmp;

							/* okay, the connection is open and the server is still alive */
							connection->used_this_request = 1;
							tmp = (php_oci_connection *)connection->id->ptr;

							if (tmp != NULL && rsrc_type == le_pconnection && tmp->hash_key->len == hashed_details.s->len &&
								memcmp(tmp->hash_key->val, hashed_details.s->val, tmp->hash_key->len) == 0 && ++GC_REFCOUNT(connection->id) == SUCCESS) {
								/* do nothing */
							} else {
								PHP_OCI_REGISTER_RESOURCE(connection, le_pconnection);
								/* Persistent connections: For old close semantics we artificially
								 * bump up the refcount to prevent the non-persistent destructor
								 * from getting called until request shutdown. The refcount is
								 * decremented in the persistent helper
								 */
								if (OCI_G(old_oci_close_semantics)) {
									++GC_REFCOUNT(connection->id);
								}
							}
							smart_str_free(&hashed_details);
							return connection;
						}
					}
					/* server died */
				} else {
					/* we do not ping non-persistent connections */
					smart_str_free(&hashed_details);
					++GC_REFCOUNT(connection->id);
					return connection;
				}
			} /* is_open is true? */

			/* Server died - connection not usable. The is_open=true can also fall through to here,
			 * if ping fails
			 */
			if (persistent){
				int rsrc_type;

				connection->is_open = 0;
				connection->used_this_request = 1;

				/* We have to do a hash_del but need to preserve the resource if there is a positive
				 * refcount. Set the data pointer in the list entry to NULL
				 */
				if (connection == connection->id->ptr && rsrc_type == le_pconnection) {
					le->ptr = NULL;
				}

				zend_hash_del(&EG(persistent_list), hashed_details.s);
			} else {
				/* We only remove the hash entry. The resource and the list entry with its pointer
				 * to the resource are still intact
				 */
				zend_hash_del(&EG(regular_list), hashed_details.s);
			}

			connection = NULL;
		} else if (found) {
			/* found something, but it's not a connection, delete it */
			if (persistent) {
				zend_hash_del(&EG(persistent_list), hashed_details.s);
			} else {
				zend_hash_del(&EG(regular_list), hashed_details.s);
			}
		}
	}

	/* Check if we have reached max_persistent. If so, try to remove a few timed-out connections. As
	 * a last resort, return a non-persistent connection.
	 */
	if (persistent) {
		zend_bool alloc_non_persistent = 0;

		if (OCI_G(max_persistent) != -1 && OCI_G(num_persistent) >= OCI_G(max_persistent)) {
			/* try to find an idle connection and kill it */
			zend_hash_apply(&EG(persistent_list), (apply_func_t) php_oci_persistent_helper);

			if (OCI_G(max_persistent) != -1 && OCI_G(num_persistent) >= OCI_G(max_persistent)) {
				/* all persistent connactions are in use, fallback to non-persistent connection creation */
				php_error_docref(NULL, E_NOTICE, "Too many open persistent connections (%pd)", OCI_G(num_persistent));
				alloc_non_persistent = 1;
			}
		}

		if (alloc_non_persistent) {
			connection = (php_oci_connection *) ecalloc(1, sizeof(php_oci_connection));
			connection->hash_key = zend_string_dup(hashed_details.s, 0);
			connection->is_persistent = 0;
#ifdef HAVE_OCI8_DTRACE
			connection->client_id = NULL;
#endif
		} else {
			connection = (php_oci_connection *) calloc(1, sizeof(php_oci_connection));
			if (connection == NULL) {
				return NULL;
			}
			connection->hash_key = zend_string_dup(hashed_details.s, 1);
			if (connection->hash_key == NULL) {
				free(connection);
				return NULL;
			}
			connection->is_persistent = 1;
#ifdef HAVE_OCI8_DTRACE
			connection->client_id = NULL;
#endif
		}
	} else {
		connection = (php_oci_connection *) ecalloc(1, sizeof(php_oci_connection));
		connection->hash_key = zend_string_dup(hashed_details.s, 0);
		connection->is_persistent = 0;
#ifdef HAVE_OCI8_DTRACE
		connection->client_id = NULL;
#endif
	}

	/* {{{ Get the session pool that suits this connection request from the persistent list. This
	 * step is only for non-persistent connections as persistent connections have private session
	 * pools. Non-persistent conns use shared session pool to allow for optimizations such as
	 * caching the physical connection (for DRCP) even when the non-persistent php connection is
	 * destroyed.
	 *
	 * TODO: Unconditionally do this once OCI provides extended OCISessionGet capability
	 */
	if (use_spool && !connection->is_persistent) {
		if ((session_pool = php_oci_get_spool(username, username_len, password, password_len, dbname, dbname_len, charsetid ? charsetid:charsetid_nls_lang))==NULL)
		{
			php_oci_connection_close(connection);
			smart_str_free(&hashed_details);
			return NULL;
		}
	}
	/* }}} */

	connection->idle_expiry = (OCI_G(persistent_timeout) > 0) ? (timestamp + OCI_G(persistent_timeout)) : 0;

	/* Mark password as unchanged by PHP during the duration of the database session */
	connection->passwd_changed = 0;

	smart_str_free(&hashed_details);

	if (charsetid) {
		connection->charset = charsetid;
	} else {
		connection->charset = charsetid_nls_lang;
	}

	/* Old session creation semantics when session pool cannot be used Eg: privileged
	 * connect/password change
	 */
	if (!use_spool) {
		if (php_oci_old_create_session(connection, dbname, dbname_len, username, username_len, password, password_len, new_password, new_password_len, session_mode)) {
			php_oci_connection_close(connection);
			return NULL;
		}
	} else {
		/* create using the client-side session pool */
		if (php_oci_create_session(connection, session_pool, dbname, dbname_len, username, username_len, password, password_len, new_password, new_password_len, session_mode)) {
			php_oci_connection_close(connection);
			return NULL;
		}
	}

	/* Mark it as open */
	connection->is_open = 1;

	/* add to the appropriate hash */
	if (connection->is_persistent) {
		new_le.ptr = connection;
		new_le.type = le_pconnection;
		connection->used_this_request = 1;
		PHP_OCI_REGISTER_RESOURCE(connection, le_pconnection);

		/* Persistent connections: For old close semantics we artificially bump up the refcount to
		 * prevent the non-persistent destructor from getting called until request shutdown. The
		 * refcount is decremented in the persistent helper
		 */
		if (OCI_G(old_oci_close_semantics)) {
			++GC_REFCOUNT(connection->id);
		}
		zend_hash_update_mem(&EG(persistent_list), connection->hash_key, (void *)&new_le, sizeof(zend_resource));
		OCI_G(num_persistent)++;
		OCI_G(num_links)++;
	} else if (!exclusive) {
		PHP_OCI_REGISTER_RESOURCE(connection, le_connection);
		new_le.ptr = connection->id;
		new_le.type = le_index_ptr;
		zend_hash_update_mem(&EG(regular_list), connection->hash_key, (void *)&new_le, sizeof(zend_resource));
		OCI_G(num_links)++;
	} else {
		PHP_OCI_REGISTER_RESOURCE(connection, le_connection);
		OCI_G(num_links)++;
	}

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_CONNECT_TYPE_ENABLED()) {
		DTRACE_OCI8_CONNECT_TYPE(connection->is_persistent ? 1 : 0, exclusive ? 1 : 0, connection, OCI_G(num_persistent), OCI_G(num_links));
	}
#endif /* HAVE_OCI8_DTRACE */

	return connection;
}
/* }}} */

/* {{{ php_oci_connection_ping()
 *
 * Ping connection. Uses OCIPing() or OCIServerVersion() depending on the Oracle Client version
 */
static int php_oci_connection_ping(php_oci_connection *connection)
{
	sword errstatus;
#if (!((OCI_MAJOR_VERSION > 10) || ((OCI_MAJOR_VERSION == 10) && (OCI_MINOR_VERSION >= 2))))
	char version[256];
#endif

	OCI_G(errcode) = 0;  		/* assume ping is successful */

	/* Use OCIPing instead of OCIServerVersion. If OCIPing returns ORA-1010 (invalid OCI operation)
	 * such as from Pre-10.1 servers, the error is still from the server and we would have
	 * successfully performed a roundtrip and validated the connection. Use OCIServerVersion for
	 * Pre-10.2 clients
	 */
#if ((OCI_MAJOR_VERSION > 10) || ((OCI_MAJOR_VERSION == 10) && (OCI_MINOR_VERSION >= 2)))	/* OCIPing available 10.2 onwards */
	PHP_OCI_CALL_RETURN(errstatus, OCIPing, (connection->svc, OCI_G(err), OCI_DEFAULT));
#else
	/* use good old OCIServerVersion() */
	PHP_OCI_CALL_RETURN(errstatus, OCIServerVersion, (connection->svc, OCI_G(err), (text *)version, sizeof(version), OCI_HTYPE_SVCCTX));
#endif

	if (errstatus == OCI_SUCCESS) {
		return 1;
	} else {
		sb4 error_code = 0;
		text tmp_buf[OCI_ERROR_MAXMSG_SIZE];

		/* Treat ORA-1010 as a successful Ping */
		OCIErrorGet(OCI_G(err), (ub4)1, NULL, &error_code, tmp_buf, (ub4)OCI_ERROR_MAXMSG_SIZE, (ub4)OCI_HTYPE_ERROR);
		if (error_code == 1010) {
			return 1;
		}
		OCI_G(errcode) = error_code;
	}

	return 0;
}
/* }}} */

/* {{{ php_oci_connection_status()
 *
 * Check connection status (pre-ping check)
 */
static int php_oci_connection_status(php_oci_connection *connection)
{
	ub4 ss = OCI_SERVER_NOT_CONNECTED;
	sword errstatus;

	/* get OCI_ATTR_SERVER_STATUS */
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)connection->server, OCI_HTYPE_SERVER, (dvoid *)&ss, (ub4 *)0, OCI_ATTR_SERVER_STATUS, OCI_G(err)));

	if (errstatus == OCI_SUCCESS && ss == OCI_SERVER_NORMAL) {
		return 1;
	}

	/* ignore errors here, just return failure */
	return 0;
}
/* }}} */

/* {{{ php_oci_connection_rollback()
 *
 * Rollback connection
 */
int php_oci_connection_rollback(php_oci_connection *connection)
{
	sword errstatus;

	PHP_OCI_CALL_RETURN(errstatus, OCITransRollback, (connection->svc, connection->err, (ub4) 0));
	connection->rb_on_disconnect = 0;

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	return 0;
}
/* }}} */

/* {{{ php_oci_connection_commit()
 *
 * Commit connection
 */
int php_oci_connection_commit(php_oci_connection *connection)
{
	sword errstatus;

	PHP_OCI_CALL_RETURN(errstatus, OCITransCommit, (connection->svc, connection->err, (ub4) 0));
	connection->rb_on_disconnect = 0;

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	return 0;
}
/* }}} */

/* {{{ php_oci_connection_close()
 *
 * Close the connection and free all its resources
 */
static int php_oci_connection_close(php_oci_connection *connection)
{
	int result = 0;
	zend_bool in_call_save = OCI_G(in_call);

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_CONNECTION_CLOSE_ENABLED()) {
		DTRACE_OCI8_CONNECTION_CLOSE(connection);
	}
#endif /* HAVE_OCI8_DTRACE */

	if (!connection->is_stub) {
		/* Release resources associated with connection */
		php_oci_connection_release(connection);
	}

	if (!connection->using_spool && connection->svc) {
		PHP_OCI_CALL(OCISessionEnd, (connection->svc, connection->err, connection->session, (ub4) 0));
	}

	if (connection->err) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->err, (ub4) OCI_HTYPE_ERROR));
	}
	if (connection->authinfo) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->authinfo, (ub4) OCI_HTYPE_AUTHINFO));
	}

	/* No Handlefrees for session pool connections */
	if (!connection->using_spool) {
		if (connection->session) {
			PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->session, OCI_HTYPE_SESSION));
		}

		if (connection->is_attached) {
			PHP_OCI_CALL(OCIServerDetach, (connection->server, OCI_G(err), OCI_DEFAULT));
		}

		if (connection->svc) {
			PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->svc, (ub4) OCI_HTYPE_SVCCTX));
		}

		if (connection->server) {
			PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->server, (ub4) OCI_HTYPE_SERVER));
		}

		if (connection->env) {
			PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->env, OCI_HTYPE_ENV));
		}
	} else if (connection->private_spool) {
	/* Keep this as the last member to be freed, as there are dependencies
	 * (like env) on the session pool
	 */
		php_oci_spool_close(connection->private_spool);
	}

	if (connection->hash_key) {
		pefree(connection->hash_key, connection->is_persistent);
	}
#ifdef HAVE_OCI8_DTRACE
	if (connection->client_id) {
		pefree(connection->client_id, connection->is_persistent);
	}
#endif /* HAVE_OCI8_DTRACE */
	pefree(connection, connection->is_persistent);
	connection = NULL;
	OCI_G(in_call) = in_call_save;
	return result;
}
/* }}} */

/* {{{ php_oci_connection_release()
 *
 * Release the connection's resources. This involves freeing descriptors and rolling back
 * transactions, setting timeout-related parameters etc. For session-pool using connections, the
 * underlying connection is released to its session pool.
 */
int php_oci_connection_release(php_oci_connection *connection)
{
	int result = 0;
	zend_bool in_call_save = OCI_G(in_call);
	time_t timestamp = time(NULL);

	if (connection->is_stub) {
		return 0;
	}

	if (connection->descriptors) {
		php_oci_connection_descriptors_free(connection);
	}

	if (connection->svc) {
		/* rollback outstanding transactions */
		if (connection->rb_on_disconnect) {
			if (php_oci_connection_rollback(connection)) {
				/* rollback failed */
				result = 1;
			}
		}
	}

	if (OCI_G(persistent_timeout) > 0) {
		connection->idle_expiry = timestamp + OCI_G(persistent_timeout);
	}

	/* We may have half-cooked connections to clean up */
	if (connection->next_pingp) {
		if (OCI_G(ping_interval) >= 0) {
			*(connection->next_pingp) = timestamp + OCI_G(ping_interval);
		} else {
			/* ping_interval is -1 */
			*(connection->next_pingp) = 0;
		}
	}

	/* Release the session (stubs are filtered out at the beginning)*/
	if (connection->using_spool) {
		ub4 rlsMode = OCI_DEFAULT;

		if (result) {
			rlsMode |= OCI_SESSRLS_DROPSESS;
		}

		/* Sessions for non-persistent connections should be dropped.  For 11 and above, the session
		 * pool has its own mechanism for doing so for purity NEW connections. We need to do so
		 * explicitly for 10.2 and earlier.
		 */
#if (!(OCI_MAJOR_VERSION >= 11))
		if (!connection->is_persistent) {
			rlsMode |= OCI_SESSRLS_DROPSESS;
		}
#endif

		if (connection->svc) {
			PHP_OCI_CALL(OCISessionRelease, (connection->svc, connection->err, NULL,
										 0, rlsMode));
		}
		/* It no longer has relation with the database session. However authinfo and env are
		 * cached
		 */
		connection->svc = NULL;
		connection->server = NULL;
		connection->session = NULL;

		connection->is_attached = connection->is_open = connection->rb_on_disconnect = connection->used_this_request = 0;
		connection->is_stub = 1;

		/* Cut the link between the connection structure and the time_t structure allocated within
		 * the OCI session
		 */
		connection->next_pingp = NULL;
#ifdef HAVE_OCI8_DTRACE
		if (connection->client_id) {
			pefree(connection->client_id, connection->is_persistent);
			connection->client_id = NULL;
		}
#endif /* HAVE_OCI8_DTRACE */
	}

	OCI_G(in_call) = in_call_save;
	return result;
}
/* }}} */

/* {{{ php_oci_password_change()
 *
 * Change password for the user with the username given
 */
int php_oci_password_change(php_oci_connection *connection, char *user, int user_len, char *pass_old, int pass_old_len, char *pass_new, int pass_new_len)
{
	sword errstatus;

	PHP_OCI_CALL_RETURN(errstatus, OCIPasswordChange, (connection->svc, connection->err, (text *)user, user_len, (text *)pass_old, pass_old_len, (text *)pass_new, pass_new_len, OCI_DEFAULT));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	connection->passwd_changed = 1;
	return 0;
}
/* }}} */

/* {{{ php_oci_client_get_version()
 *
 * Get Oracle client library version
 */
void php_oci_client_get_version(char **version)
{
	char  version_buff[256];
#if ((OCI_MAJOR_VERSION > 10) || ((OCI_MAJOR_VERSION == 10) && (OCI_MINOR_VERSION >= 2)))	/* OCIClientVersion only available 10.2 onwards */
	sword major_version = 0;
	sword minor_version = 0;
	sword update_num = 0;
	sword patch_num = 0;
	sword port_update_num = 0;

	PHP_OCI_CALL(OCIClientVersion, (&major_version, &minor_version, &update_num, &patch_num, &port_update_num));
	snprintf(version_buff, sizeof(version_buff), "%d.%d.%d.%d.%d", major_version, minor_version, update_num, patch_num, port_update_num);
#else
	memcpy(version_buff, "Unknown", sizeof("Unknown"));
#endif
	*version = estrdup(version_buff);
}
/* }}} */

/* {{{ php_oci_server_get_version()
 *
 * Get Oracle server version
 */
int php_oci_server_get_version(php_oci_connection *connection, char **version)
{
	sword errstatus;
	char version_buff[256];

	PHP_OCI_CALL_RETURN(errstatus, OCIServerVersion, (connection->svc, connection->err, (text *)version_buff, sizeof(version_buff), OCI_HTYPE_SVCCTX));

	if (errstatus != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, errstatus);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}

	*version = estrdup(version_buff);
	return 0;
}
/* }}} */

/* {{{ php_oci_column_to_zval()
 *
 * Convert php_oci_out_column struct into zval
 */
int php_oci_column_to_zval(php_oci_out_column *column, zval *value, int mode)
{
	php_oci_descriptor *descriptor;
	ub4 lob_length;
	int column_size;
	char *lob_buffer;
	int lob_fetch_status;

	if (column->indicator == -1) { /* column is NULL */
		ZVAL_NULL(value);
		return 0;
	}

	if (column->is_cursor) { /* REFCURSOR -> simply return the statement id */
		zend_register_resource(value, column->stmtid, 0); /* XXX type correct? */
		++GC_REFCOUNT(column->stmtid);
	} else if (column->is_descr) {

		if (column->data_type != SQLT_RDD) {
			int rsrc_type;

			/* reset descriptor's length */
			descriptor = (php_oci_descriptor *) column->descid->ptr;

			if (!descriptor || rsrc_type != le_descriptor) {
				php_error_docref(NULL, E_WARNING, "Unable to find LOB descriptor #%d", column->descid);
				return 1;
			}

			descriptor->lob_size = -1;
			descriptor->lob_current_position = 0;
			descriptor->buffering = 0;
		}

		if (column->data_type != SQLT_RDD && (mode & PHP_OCI_RETURN_LOBS)) {
			/* PHP_OCI_RETURN_LOBS means that we want the content of the LOB back instead of the locator */

			if (column->chunk_size)
				descriptor->chunk_size = column->chunk_size;
			lob_fetch_status = php_oci_lob_read(descriptor, -1, 0, &lob_buffer, &lob_length);
			if (descriptor->chunk_size)  /* Cache the chunk_size to avoid recalling OCILobGetChunkSize */
				column->chunk_size = descriptor->chunk_size;
			php_oci_temp_lob_close(descriptor);
			if (lob_fetch_status) {
				ZVAL_FALSE(value);
				return 1;
			} else {
				if (lob_length > 0) {
					ZVAL_STRINGL(value, lob_buffer, lob_length);
				} else {
					ZVAL_EMPTY_STRING(value);
				}
				return 0;
			}
		} else {
			/* return the locator */
			object_init_ex(value, oci_lob_class_entry_ptr);
			add_property_resource(value, "descriptor", column->descid);
			++GC_REFCOUNT(column->descid);
		}
	} else {
		switch (column->retcode) {
			case 0:
				/* intact value */
				if (column->piecewise) {
					column_size = column->retlen4;
				} else {
					column_size = column->retlen;
				}
				break;

			default:
				ZVAL_FALSE(value);
				return 0;
		}

		ZVAL_STRINGL(value, column->data, column_size);
	}
	return 0;
}
/* }}} */


/* {{{ php_oci_fetch_row()
 *
 * Fetch the next row from the given statement
 * Has logic for Oracle 12c Implicit Result Sets
 */
void php_oci_fetch_row (INTERNAL_FUNCTION_PARAMETERS, int mode, int expected_args)
{
	zval *z_statement, *array;
	php_oci_statement *statement;		  /* statement that will be fetched from */
#if (OCI_MAJOR_VERSION >= 12)
	php_oci_statement *invokedstatement;  /* statement this function was invoked with */
#endif /* OCI_MAJOR_VERSION */
	php_oci_out_column *column;
	ub4 nrows = 1;
	int i;
	zend_long fetch_mode = 0;

	if (expected_args > 2) {
		/* only for ocifetchinto BC */

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz|l", &z_statement, &array, &fetch_mode) == FAILURE) {
			return;
		}

		if (ZEND_NUM_ARGS() == 2) {
			fetch_mode = mode;
		}
	} else if (expected_args == 2) {
		/* only for oci_fetch_array() */

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &z_statement, &fetch_mode) == FAILURE) {
			return;
		}

		if (ZEND_NUM_ARGS() == 1) {
			fetch_mode = mode;
		}
	} else {
		/* for all oci_fetch_*() */

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_statement) == FAILURE) {
			return;
		}

		fetch_mode = mode;
	}

	if (!(fetch_mode & PHP_OCI_NUM) && !(fetch_mode & PHP_OCI_ASSOC)) {
		/* none of the modes present, use the default one */
		if (mode & PHP_OCI_ASSOC) {
			fetch_mode |= PHP_OCI_ASSOC;
		}
		if (mode & PHP_OCI_NUM) {
			fetch_mode |= PHP_OCI_NUM;
		}
	}

#if (OCI_MAJOR_VERSION < 12)
	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_fetch(statement, nrows)) {
		RETURN_FALSE;			/* end of fetch */
	}
#else /* OCI_MAJOR_VERSION */
	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, invokedstatement);

	if (invokedstatement->impres_flag == PHP_OCI_IMPRES_NO_CHILDREN) {
		/* Already know there are no Implicit Result Sets */
	    statement = invokedstatement;
	} else if (invokedstatement->impres_flag == PHP_OCI_IMPRES_HAS_CHILDREN) {
		/* Previously saw an Implicit Result Set in an earlier invocation of php_oci_fetch_row */
		statement = (php_oci_statement *)invokedstatement->impres_child_stmt;
	} else {
		sword errstatus;

		/* Check for an Implicit Result Set on this statement handle */
		PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)invokedstatement->stmt, OCI_HTYPE_STMT,
						    (dvoid *) &invokedstatement->impres_count,
						    (ub4 *)NULL, OCI_ATTR_IMPLICIT_RESULT_COUNT, invokedstatement->err));
		if (errstatus) {
			RETURN_FALSE;
		}
		if (invokedstatement->impres_count > 0) {
			/* Make it so the fetch occurs on the first Implicit Result Set */
			statement = php_oci_get_implicit_resultset(invokedstatement);
			if (!statement || php_oci_statement_execute(statement, (ub4)OCI_DEFAULT))
				RETURN_FALSE;
			invokedstatement->impres_count--;
			invokedstatement->impres_child_stmt = (struct php_oci_statement *)statement;
			invokedstatement->impres_flag = PHP_OCI_IMPRES_HAS_CHILDREN;
		} else {
			statement = invokedstatement; /* didn't find Implicit Result Sets */
			invokedstatement->impres_flag = PHP_OCI_IMPRES_NO_CHILDREN;  /* Don't bother checking again */
		}
	}

	if (php_oci_statement_fetch(statement, nrows)) {
		/* End of fetch */
		if (invokedstatement->impres_count > 0) {
			/* Check next Implicit Result Set */
	        statement = php_oci_get_implicit_resultset(invokedstatement);
			if (!statement || php_oci_statement_execute(statement, (ub4)OCI_DEFAULT))
				RETURN_FALSE;
			invokedstatement->impres_count--;
			invokedstatement->impres_child_stmt = (struct php_oci_statement *)statement;
			if (php_oci_statement_fetch(statement, nrows)) {
				/* End of all fetches */
	            RETURN_FALSE;
			}
		} else {
			RETURN_FALSE;
		}
    }
#endif /* OCI_MAJOR_VERSION */

	array_init(return_value);

	for (i = 0; i < statement->ncolumns; i++) {

		column = php_oci_statement_get_column(statement, i + 1, NULL, 0);

		if (column == NULL) {
			continue;
		}
		if ((column->indicator == -1) && ((fetch_mode & PHP_OCI_RETURN_NULLS) == 0)) {
			continue;
		}

		if (!(column->indicator == -1)) {
			zval element;

			php_oci_column_to_zval(column, &element, fetch_mode);

			if (fetch_mode & PHP_OCI_NUM || !(fetch_mode & PHP_OCI_ASSOC)) {
				add_index_zval(return_value, i, &element);
			}
			if (fetch_mode & PHP_OCI_ASSOC) {
				if (fetch_mode & PHP_OCI_NUM) {
					Z_ADDREF(element);
				}
				add_assoc_zval(return_value, column->name, &element);
			}

		} else {
			if (fetch_mode & PHP_OCI_NUM || !(fetch_mode & PHP_OCI_ASSOC)) {
				add_index_null(return_value, i);
			}
			if (fetch_mode & PHP_OCI_ASSOC) {
				add_assoc_null(return_value, column->name);
			}
		}
	}

	if (expected_args > 2) {
		/* Only for ocifetchinto BC.  In all other cases we return array, not long */
		ZVAL_COPY_VALUE(array, return_value); /* copy return_value to given reference */
		zval_dtor(return_value);
		RETURN_LONG(statement->ncolumns);
	}
}
/* }}} */

/* {{{ php_oci_persistent_helper()
 *
 * Helper function to close/rollback persistent connections at the end of request. A return value of
 * 1 indicates that the connection is to be destroyed
 */
static int php_oci_persistent_helper(zend_resource *le)
{
	time_t timestamp;
	php_oci_connection *connection;

	timestamp = time(NULL);

	/* Persistent connection stubs are also counted as they have private session pools */
	if (le->type == le_pconnection) {
		connection = (php_oci_connection *)le->ptr;

		if (!connection->used_this_request && OCI_G(persistent_timeout) != -1) {
#ifdef HAVE_OCI8_DTRACE
			if (DTRACE_OCI8_CONNECT_EXPIRY_ENABLED()) {
				DTRACE_OCI8_CONNECT_EXPIRY(connection, connection->is_stub ? 1 : 0, (long)connection->idle_expiry, (long)timestamp);
			}
#endif /* HAVE_OCI8_DTRACE */
			if (connection->idle_expiry < timestamp) {
				/* connection has timed out */
				return ZEND_HASH_APPLY_REMOVE;
			}
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ php_oci_create_spool()
 *
 *	 Create(alloc + Init) Session pool for the given dbname and charsetid
 */
static php_oci_spool *php_oci_create_spool(char *username, int username_len, char *password, int password_len, char *dbname, int dbname_len, zend_string *hash_key, int charsetid)
{
	php_oci_spool *session_pool = NULL;
	zend_bool iserror = 0;
	ub4 poolmode = OCI_DEFAULT;	/* Mode to be passed to OCISessionPoolCreate */
	OCIAuthInfo *spoolAuth = NULL;
	sword errstatus;

	/* Allocate sessionpool out of persistent memory */
	session_pool = (php_oci_spool *) calloc(1, sizeof(php_oci_spool));
	if (session_pool == NULL) {
		iserror = 1;
		goto exit_create_spool;
	}

	/* Populate key if passed */
	if (hash_key->val) {
		session_pool->spool_hash_key = zend_string_dup(hash_key, 1);
		if (session_pool->spool_hash_key == NULL) {
			iserror = 1;
			goto exit_create_spool;
		}
	}

	/* Create the session pool's env */
	if (!(session_pool->env = php_oci_create_env(charsetid))) {
		iserror = 1;
		goto exit_create_spool;
	}

	/* Allocate the pool handle */
	PHP_OCI_CALL_RETURN(errstatus, OCIHandleAlloc, (session_pool->env, (dvoid **) &session_pool->poolh, OCI_HTYPE_SPOOL, (size_t) 0, (dvoid **) 0));

	if (errstatus != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
		iserror = 1;
		goto exit_create_spool;
	}

	/* Allocate the session pool error handle - This only for use in the destructor, as there is a
	 * generic bug which can free up the OCI_G(err) variable before destroying connections. We
	 * cannot use this for other roundtrip calls as there is no way the user can access this error
	 */
	PHP_OCI_CALL_RETURN(errstatus, OCIHandleAlloc, ((dvoid *) session_pool->env, (dvoid **)&(session_pool->err), (ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0));

	if (errstatus != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
		iserror = 1;
		goto exit_create_spool;
	}

/* Disable RLB as we mostly have single-connection pools */
#if (OCI_MAJOR_VERSION > 10)
	poolmode = OCI_SPC_NO_RLB | OCI_SPC_HOMOGENEOUS;
#else
	poolmode = OCI_SPC_HOMOGENEOUS;
#endif

#if ((OCI_MAJOR_VERSION > 11) || ((OCI_MAJOR_VERSION == 11) && (OCI_MINOR_VERSION >= 2)))
	/* {{{ Allocate auth handle for session pool */
	PHP_OCI_CALL_RETURN(errstatus, OCIHandleAlloc, (session_pool->env, (dvoid **)&(spoolAuth), OCI_HTYPE_AUTHINFO, 0, NULL));

	if (errstatus != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
		iserror = 1;
		goto exit_create_spool;
 	}
	/* }}} */

	/* {{{ Set the edition attribute on the auth handle */
	if (OCI_G(edition)) {
		PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) spoolAuth, (ub4) OCI_HTYPE_AUTHINFO, (dvoid *) OCI_G(edition), (ub4)(strlen(OCI_G(edition))), (ub4)OCI_ATTR_EDITION, OCI_G(err)));

		if (errstatus != OCI_SUCCESS) {
			OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
			iserror = 1;
			goto exit_create_spool;
		}
	}
	/* }}} */

	/* {{{ Set the driver name attribute on the auth handle */
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) spoolAuth, (ub4) OCI_HTYPE_AUTHINFO, (dvoid *) PHP_OCI8_DRIVER_NAME, (ub4) sizeof(PHP_OCI8_DRIVER_NAME)-1, (ub4) OCI_ATTR_DRIVER_NAME, OCI_G(err)));

	if (errstatus != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
		iserror = 1;
		goto exit_create_spool;
	}
	/* }}} */

	/* {{{ Set the auth handle on the session pool */
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, ((dvoid *) (session_pool->poolh),(ub4) OCI_HTYPE_SPOOL, (dvoid *) spoolAuth, (ub4)0, (ub4)OCI_ATTR_SPOOL_AUTH, OCI_G(err)));

	if (errstatus != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
		iserror = 1;
		goto exit_create_spool;
	}
	/* }}} */
#endif

	/* Create the homogeneous session pool - We have different session pools for every different
	 * username, password, charset and dbname.
	 */
	PHP_OCI_CALL_RETURN(errstatus, OCISessionPoolCreate,(session_pool->env, OCI_G(err), session_pool->poolh, (OraText **)&session_pool->poolname, &session_pool->poolname_len, (OraText *)dbname, (ub4)dbname_len, 0, UB4MAXVAL, 1,(OraText *)username, (ub4)username_len, (OraText *)password,(ub4)password_len, poolmode));

	if (errstatus != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), errstatus);
		iserror = 1;
	}

exit_create_spool:
	if (iserror && session_pool) {
		php_oci_spool_close(session_pool);
		session_pool = NULL;
	}

	if (spoolAuth) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) spoolAuth, (ub4) OCI_HTYPE_AUTHINFO));
	}

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_SESSPOOL_CREATE_ENABLED()) {
		DTRACE_OCI8_SESSPOOL_CREATE(session_pool);
	}
#endif /* HAVE_OCI8_DTRACE */

	return session_pool;
}
/* }}} */

/* {{{ php_oci_get_spool()
 *
 * Get Session pool for the given dbname and charsetid from the persistent list. Function called for
 * non-persistent connections.
 */
static php_oci_spool *php_oci_get_spool(char *username, int username_len, char *password, int password_len, char *dbname, int dbname_len, int charsetid)
{
	smart_str spool_hashed_details = {0};
	php_oci_spool *session_pool = NULL;
	zend_resource spool_le = {0};
	zend_resource *spool_out_le = NULL;
	zend_bool iserror = 0;
	zval *spool_out_zv = NULL;

	/* {{{ Create the spool hash key */
	smart_str_appendl_ex(&spool_hashed_details, "oci8spool***", sizeof("oci8spool***") - 1, 0);
	smart_str_appendl_ex(&spool_hashed_details, username, username_len, 0);
	smart_str_appendl_ex(&spool_hashed_details, "**", sizeof("**") - 1, 0);
	/* Add edition attribute to the hash */
	if (OCI_G(edition)){
		smart_str_appendl_ex(&spool_hashed_details, OCI_G(edition), strlen(OCI_G(edition)), 0);
	}
	smart_str_appendl_ex(&spool_hashed_details, "**", sizeof("**") - 1, 0);
	if (password_len) {
		zend_ulong password_hash;
		password_hash = zend_inline_hash_func(password, password_len);
		smart_str_append_unsigned_ex(&spool_hashed_details, password_hash, 0);
	}
	smart_str_appendl_ex(&spool_hashed_details, "**", sizeof("**") - 1, 0);

	if (dbname_len) {
		smart_str_appendl_ex(&spool_hashed_details, dbname, dbname_len, 0);
	}
	smart_str_appendl_ex(&spool_hashed_details, "**", sizeof("**") - 1, 0);

	smart_str_append_unsigned_ex(&spool_hashed_details, charsetid, 0);

	/* Session Pool Hash Key : oci8spool***username**edition**hashedpassword**dbname**charset */

	smart_str_0(&spool_hashed_details);
	/* PHPNG TODO is this safe to do? */
	php_strtolower(spool_hashed_details.s->val, spool_hashed_details.s->len);
	/* }}} */

	spool_out_zv = zend_hash_find(&EG(persistent_list), spool_hashed_details.s);
	if (spool_out_zv != NULL) {
		spool_out_le = Z_RES_P(spool_out_zv);
	}

	if (spool_out_le == NULL) {

		session_pool = php_oci_create_spool(username, username_len, password, password_len, dbname, dbname_len, spool_hashed_details.s, charsetid);

		if (session_pool == NULL) {
			iserror = 1;
			goto exit_get_spool;
		}
		spool_le.ptr  = session_pool;
		spool_le.type = le_psessionpool;
		PHP_OCI_REGISTER_RESOURCE(session_pool, le_psessionpool);
		zend_hash_update_mem(&EG(persistent_list), session_pool->spool_hash_key, (void *)&spool_le, sizeof(zend_resource));
	} else if (spool_out_le->type == le_psessionpool &&
		((php_oci_spool *)(spool_out_le->ptr))->spool_hash_key->len == spool_hashed_details.s->len &&
		memcmp(((php_oci_spool *)(spool_out_le->ptr))->spool_hash_key->val, spool_hashed_details.s->val, spool_hashed_details.s->len) == 0) {
		/* retrieve the cached session pool */
		session_pool = (php_oci_spool *)(spool_out_le->ptr);
	}

exit_get_spool:
	smart_str_free(&spool_hashed_details);
	if (iserror && session_pool) {
		php_oci_spool_close(session_pool);
		session_pool = NULL;
	}

	return session_pool;

}
/* }}} */

/* {{{ php_oci_create_env()
 *
 * Create the OCI environment choosing the correct function for the OCI version
 */
static OCIEnv *php_oci_create_env(ub2 charsetid)
{
	OCIEnv *retenv = NULL;

	/* create an environment using the character set id */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIEnvNlsCreate, (&retenv, OCI_G(events) ? PHP_OCI_INIT_MODE | OCI_EVENTS : PHP_OCI_INIT_MODE, 0, NULL, NULL, NULL, 0, NULL, charsetid, charsetid));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		sb4   ora_error_code = 0;
		text  ora_msg_buf[OCI_ERROR_MAXMSG_SIZE];  /* Use traditional smaller size: non-PL/SQL errors should fit and it keeps the stack smaller */

#ifdef HAVE_OCI_INSTANT_CLIENT
		php_error_docref(NULL, E_WARNING, "OCIEnvNlsCreate() failed. There is something wrong with your system - please check that " PHP_OCI8_LIB_PATH_MSG " includes the directory with Oracle Instant Client libraries");
#else
		php_error_docref(NULL, E_WARNING, "OCIEnvNlsCreate() failed. There is something wrong with your system - please check that ORACLE_HOME and " PHP_OCI8_LIB_PATH_MSG " are set and point to the right directories");
#endif
		if (retenv
			&& OCIErrorGet(retenv, (ub4)1, NULL, &ora_error_code, ora_msg_buf, (ub4)OCI_ERROR_MAXMSG_SIZE, (ub4)OCI_HTYPE_ENV) == OCI_SUCCESS
			&& *ora_msg_buf) {
			php_error_docref(NULL, E_WARNING, "%s", ora_msg_buf);
		}

		return NULL;
	}
	return retenv;
}
/* }}} */

/* {{{ php_oci_old_create_session()
 *
 * This function is to be deprecated in future in favour of OCISessionGet which is used in
 * php_oci_do_connect_ex
 */
static int php_oci_old_create_session(php_oci_connection *connection, char *dbname, int dbname_len, char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, int session_mode)
{
	ub4 statement_cache_size = (OCI_G(statement_cache_size) > 0) ? OCI_G(statement_cache_size) : 0;

	/* Create the OCI environment separate for each connection */
	if (!(connection->env = php_oci_create_env(connection->charset))) {
		return 1;
	}

	/* {{{ Allocate our server handle */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->server), OCI_HTYPE_SERVER, 0, NULL));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */

	/* {{{ Attach to the server */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIServerAttach, (connection->server, OCI_G(err), (text *)dbname, dbname_len, (ub4) OCI_DEFAULT));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */
	connection->is_attached = 1;

	/* {{{ Allocate our session handle */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->session), OCI_HTYPE_SESSION, 0, NULL));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */

	/* {{{ Allocate our private error-handle */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->err), OCI_HTYPE_ERROR, 0, NULL));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */

	/* {{{ Allocate our service-context */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->svc), OCI_HTYPE_SVCCTX, 0, NULL));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */

	/* {{{ Set the username */
	if (username) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) username, (ub4) username_len, (ub4) OCI_ATTR_USERNAME, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}
	}
	/* }}} */

	/* {{{ Set the password */
	if (password) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) password, (ub4) password_len, (ub4) OCI_ATTR_PASSWORD, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}
	}
	/* }}} */

	/* {{{ Set the edition attribute on the session handle */
#if ((OCI_MAJOR_VERSION > 11) || ((OCI_MAJOR_VERSION == 11) && (OCI_MINOR_VERSION >= 2)))
	if (OCI_G(edition)) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) OCI_G(edition), (ub4) (strlen(OCI_G(edition))), (ub4) OCI_ATTR_EDITION, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}
	}
#endif
/* }}} */

	/* {{{ Set the driver name attribute on the session handle */
#if (OCI_MAJOR_VERSION >= 11)
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) PHP_OCI8_DRIVER_NAME, (ub4) sizeof(PHP_OCI8_DRIVER_NAME)-1, (ub4) OCI_ATTR_DRIVER_NAME, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
#endif
/* }}} */

	/* {{{ Set the server handle in the service handle */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, (connection->svc, OCI_HTYPE_SVCCTX, connection->server, 0, OCI_ATTR_SERVER, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */

	/* {{{ Set the authentication handle in the service handle */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, (connection->svc, OCI_HTYPE_SVCCTX, connection->session, 0, OCI_ATTR_SESSION, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}
	/* }}} */

	if (new_password) {
		/* {{{ Try to change password if new one was provided */
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIPasswordChange, (connection->svc, OCI_G(err), (text *)username, username_len, (text *)password, password_len, (text *)new_password, new_password_len, OCI_AUTH));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}

		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)connection->svc, OCI_HTYPE_SVCCTX, (dvoid *)&(connection->session), (ub4 *)0, OCI_ATTR_SESSION, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}
		/* }}} */
	} else {
		/* {{{ start the session */
		ub4 cred_type = OCI_CRED_RDBMS;

		/* Extract the overloaded session_mode parameter into valid Oracle credential and session mode values */
		if (session_mode & PHP_OCI_CRED_EXT) {
			cred_type = OCI_CRED_EXT;
			session_mode ^= PHP_OCI_CRED_EXT;
		}

		session_mode |= OCI_STMT_CACHE;

		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCISessionBegin, (connection->svc, OCI_G(err), connection->session, (ub4) cred_type, (ub4) session_mode));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			/* OCISessionBegin returns OCI_SUCCESS_WITH_INFO when
			 * user's password has expired, but is still usable.
			 */
			if (OCI_G(errcode) != OCI_SUCCESS_WITH_INFO) {
				return 1;
			}
		}
		/* }}} */
	}

	/* Brand new connection: Init and update the next_ping in the connection */
	if (php_oci_ping_init(connection, OCI_G(err)) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->svc, (ub4) OCI_HTYPE_SVCCTX, (ub4 *) &statement_cache_size, 0, (ub4) OCI_ATTR_STMTCACHESIZE, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}

	/* Successfully created session */
	return 0;
}
/* }}} */

/* {{{ php_oci_create_session()
 *
 * Create session using client-side session pool - new norm
 */
static int php_oci_create_session(php_oci_connection *connection, php_oci_spool *session_pool, char *dbname, int dbname_len, char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, int session_mode)
{
	php_oci_spool *actual_spool = NULL;
#if (OCI_MAJOR_VERSION > 10)
	ub4 purity = -2;				/* Illegal value to initialize */
#endif
	time_t timestamp = time(NULL);
	ub4 statement_cache_size = (OCI_G(statement_cache_size) > 0) ? OCI_G(statement_cache_size) : 0;

	/* Persistent connections have private session pools */
	if (connection->is_persistent && !connection->private_spool &&
		!(connection->private_spool = php_oci_create_spool(username, username_len, password, password_len, dbname, dbname_len, NULL, connection->charset))) {
			return 1;
	}
	actual_spool = (connection->is_persistent) ? (connection->private_spool) : (session_pool);

	connection->env = actual_spool->env;

	/* Do this upfront so that connection close on an error would know that this is a session pool
	 * connection. Failure to do this would result in crashes in error scenarios
	 */
	if (!connection->using_spool) {
		connection->using_spool = 1;
	}

#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_SESSPOOL_TYPE_ENABLED()) {
		DTRACE_OCI8_SESSPOOL_TYPE(session_pool ? 1 : 0, session_pool ? session_pool : connection->private_spool);
	}
#endif /* HAVE_OCI8_DTRACE */

	/* The passed in "connection" can be a cached stub from plist or freshly created. In the former
	 * case, we do not have to allocate any handles
	 */

	if (!connection->err) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->err), OCI_HTYPE_ERROR, 0, NULL));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}
	}

	/* {{{ Allocate and initialize the connection-private authinfo handle if not allocated yet */
	if (!connection->authinfo) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->authinfo), OCI_HTYPE_AUTHINFO, 0, NULL));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}

		/* Set the Connection class and purity if OCI client version >= 11g */
#if (OCI_MAJOR_VERSION > 10)
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->authinfo,(ub4) OCI_HTYPE_SESSION, (dvoid *) OCI_G(connection_class), (ub4)(strlen(OCI_G(connection_class))), (ub4)OCI_ATTR_CONNECTION_CLASS, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}

		if (connection->is_persistent)
			purity = OCI_ATTR_PURITY_SELF;
		else
			purity = OCI_ATTR_PURITY_NEW;

		PHP_OCI_CALL_RETURN(OCI_G(errcode),OCIAttrSet, ((dvoid *) connection->authinfo,(ub4) OCI_HTYPE_AUTHINFO, (dvoid *) &purity, (ub4)0, (ub4)OCI_ATTR_PURITY, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}
#endif
	}
	/* }}} */

	/* {{{ Debug statements */
#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_SESSPOOL_STATS_ENABLED()) {
		ub4 numfree = 0, numbusy = 0, numopen = 0;
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)actual_spool->poolh, OCI_HTYPE_SPOOL, (dvoid *)&numopen, (ub4 *)0, OCI_ATTR_SPOOL_OPEN_COUNT, OCI_G(err)));
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)actual_spool->poolh, OCI_HTYPE_SPOOL, (dvoid *)&numbusy, (ub4 *)0, OCI_ATTR_SPOOL_BUSY_COUNT, OCI_G(err)));
		numfree = numopen - numbusy;	/* number of free connections in the pool */
		DTRACE_OCI8_SESSPOOL_STATS(numfree, numbusy, numopen);
	}
#endif /* HAVE_OCI8_DTRACE */
	/* }}} */

		/* Ping loop: Ping and loop till we get a good connection. When a database instance goes
		 * down, it can leave several bad connections that need to be flushed out before getting a
		 * good one. In non-RAC, we always get a brand new connection at the end of the loop and in
		 * RAC, we can get a good connection from a different instance before flushing out all bad
		 * ones. We do not need to ping brand new connections.
		 */
	do {
		/* Continue to use the global error handle as the connection is closed when an error occurs */
		PHP_OCI_CALL_RETURN(OCI_G(errcode),OCISessionGet, (connection->env, OCI_G(err), &(connection->svc), (OCIAuthInfo *)connection->authinfo, (OraText *)actual_spool->poolname, (ub4)actual_spool->poolname_len, NULL, 0, NULL, NULL, NULL, OCI_SESSGET_SPOOL));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));

			/* Session creation returns OCI_SUCCESS_WITH_INFO when user's password has expired, but
			 * is still usable.
			 */

			if (OCI_G(errcode) != OCI_SUCCESS_WITH_INFO) {
				return 1;
			}
		}

		/* {{{ Populate the session and server fields of the connection */
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)connection->svc, OCI_HTYPE_SVCCTX, (dvoid *)&(connection->server), (ub4 *)0, OCI_ATTR_SERVER, OCI_G(err)));

		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)connection->svc, OCI_HTYPE_SVCCTX, (dvoid *)&(connection->session), (ub4 *)0, OCI_ATTR_SESSION, OCI_G(err)));
		/* }}} */

		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIContextGetValue, (connection->session, OCI_G(err), (ub1 *)"NEXT_PING", (ub1)sizeof("NEXT_PING"), (void **)&(connection->next_pingp)));
		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode));
			return 1;
		}

		if (!(connection->next_pingp)){
			/* This is a brand new connection, we need not ping, but have to initialize ping */
			if (php_oci_ping_init(connection, OCI_G(err)) != OCI_SUCCESS) {
				php_oci_error(OCI_G(err), OCI_G(errcode));
				return 1;
			}
		} else if ((*(connection->next_pingp) > 0) && (timestamp >= *(connection->next_pingp))) {
			if (php_oci_connection_ping(connection)) {
				/* Got a good connection - update next_ping and get out of ping loop */
				*(connection->next_pingp) = timestamp + OCI_G(ping_interval);
			} else {
				/* Bad connection - remove from pool */
				PHP_OCI_CALL(OCISessionRelease, (connection->svc, connection->err, NULL,0, (ub4) OCI_SESSRLS_DROPSESS));
				connection->svc = NULL;
				connection->server = NULL;
				connection->session = NULL;
			}
		}	/* If ping applicable */
	} while (!(connection->svc));

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->svc, (ub4) OCI_HTYPE_SVCCTX, (ub4 *) &statement_cache_size, 0, (ub4) OCI_ATTR_STMTCACHESIZE, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode));
		return 1;
	}

	/* Session is now taken from the session pool and attached and open */
	connection->is_stub = 0;
	connection->is_attached = connection->is_open = 1;

	return 0;
}
/* }}} */

/* {{{ php_oci_spool_list_dtor()
 *
 * Session pool destructor function
 */
static void php_oci_spool_list_dtor(zend_resource *entry)
{
	php_oci_spool *session_pool = (php_oci_spool *)entry->ptr;

	if (session_pool) {
		php_oci_spool_close(session_pool);
	}

	return;
}
/* }}} */

/* {{{	php_oci_spool_close()
 *
 * Destroys the OCI Session Pool
 */
static void php_oci_spool_close(php_oci_spool *session_pool)
{
	if (session_pool->poolname_len) {
		PHP_OCI_CALL(OCISessionPoolDestroy, ((dvoid *) session_pool->poolh,
			(dvoid *) session_pool->err, OCI_SPD_FORCE));
	}

	if (session_pool->poolh) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) session_pool->poolh, OCI_HTYPE_SPOOL));
	}

	if (session_pool->err) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) session_pool->err, OCI_HTYPE_ERROR));
	}

	if (session_pool->env) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) session_pool->env, OCI_HTYPE_ENV));
	}

	if (session_pool->spool_hash_key) {
		free(session_pool->spool_hash_key);
	}

	free(session_pool);
}
/* }}} */

/* {{{ php_oci_ping_init()
 *
 * Initializes the next_ping time as a context value in the connection.	 We now use
 * OCIContext{Get,Set}Value to store the next_ping because we need to support ping for
 * non-persistent DRCP connections
 */
static sword php_oci_ping_init(php_oci_connection *connection, OCIError *errh)
{
	time_t *next_pingp = NULL;

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIContextGetValue, (connection->session, errh, (ub1 *)"NEXT_PING", (ub1)sizeof("NEXT_PING"), (void **)&next_pingp));
	if (OCI_G(errcode) != OCI_SUCCESS) {
		return OCI_G(errcode);
	}

	/* This must be a brand-new connection. Allocate memory for the ping */
	if (!next_pingp) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIMemoryAlloc, (connection->session, errh, (void **)&next_pingp, OCI_DURATION_SESSION, sizeof(time_t), OCI_MEMORY_CLEARED));
		if (OCI_G(errcode) != OCI_SUCCESS) {
			return OCI_G(errcode);
		}
	}

	if (OCI_G(ping_interval) >= 0) {
		time_t timestamp = time(NULL);
		*next_pingp = timestamp + OCI_G(ping_interval);
	} else {
		*next_pingp = 0;
	}

	/* Set the new ping value into the connection */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIContextSetValue, (connection->session, errh, OCI_DURATION_SESSION, (ub1 *)"NEXT_PING", (ub1)sizeof("NEXT_PING"), next_pingp));
	if (OCI_G(errcode) != OCI_SUCCESS) {
		OCIMemoryFree(connection->session, errh, next_pingp);
		return OCI_G(errcode);
	}

	/* Cache the pointer so we do not have to do OCIContextGetValue repeatedly */
	connection->next_pingp = next_pingp;

	return OCI_SUCCESS;
}
/* }}} */

/* {{{ php_oci_dtrace_check_connection()
 *
 * DTrace output for connections that may have become invalid and marked for reopening
 */
void php_oci_dtrace_check_connection(php_oci_connection *connection, sb4 errcode, ub4 serverStatus)
{
#ifdef HAVE_OCI8_DTRACE
	if (DTRACE_OCI8_CHECK_CONNECTION_ENABLED()) {
		DTRACE_OCI8_CHECK_CONNECTION(connection, connection->client_id, connection->is_open ? 1 : 0, (long)errcode, (unsigned long)serverStatus);
	}
#endif /* HAVE_OCI8_DTRACE */
}
/* }}} */

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
