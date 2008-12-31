/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

/* $Id$ */
/* TODO
 *
 * file://localhost/www/docs/oci10/ociaahan.htm#423823 - implement lob_empty() with OCI_ATTR_LOBEMPTY
 *
 * get OCI_ATTR_CHARSET_ID attr of column to detect UTF string and multiply buffer in 4 times
 *
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#include "ext/standard/php_smart_str.h"

#if HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"
#include "zend_hash.h"

ZEND_DECLARE_MODULE_GLOBALS(oci)
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
static PHP_GINIT_FUNCTION(oci);
#endif

/* True globals, no need for thread safety */
int le_connection;
int le_pconnection;
int le_statement;
int le_descriptor;
#ifdef PHP_OCI8_HAVE_COLLECTIONS 
int le_collection;
#endif

zend_class_entry *oci_lob_class_entry_ptr;
#ifdef PHP_OCI8_HAVE_COLLECTIONS
zend_class_entry *oci_coll_class_entry_ptr;
#endif

#ifndef SQLT_BFILEE
#define SQLT_BFILEE 114
#endif
#ifndef SQLT_CFILEE
#define SQLT_CFILEE 115
#endif

#define PHP_OCI_ERRBUF_LEN 512

#if ZEND_MODULE_API_NO > 20020429
#define ONUPDATELONGFUNC OnUpdateLong
#else
#define ONUPDATELONGFUNC OnUpdateInt
#endif

/* static protos {{{ */
static void php_oci_connection_list_dtor (zend_rsrc_list_entry * TSRMLS_DC);
static void php_oci_pconnection_list_dtor (zend_rsrc_list_entry * TSRMLS_DC);
static void php_oci_statement_list_dtor (zend_rsrc_list_entry * TSRMLS_DC);
static void php_oci_descriptor_list_dtor (zend_rsrc_list_entry * TSRMLS_DC);
#ifdef PHP_OCI8_HAVE_COLLECTIONS 
static void php_oci_collection_list_dtor (zend_rsrc_list_entry * TSRMLS_DC);
#endif

static int php_oci_persistent_helper(zend_rsrc_list_entry *le TSRMLS_DC);
#ifdef ZTS
static int php_oci_list_helper(zend_rsrc_list_entry *le, void *le_type TSRMLS_DC);
#endif
static int php_oci_connection_ping(php_oci_connection * TSRMLS_DC);
static int php_oci_connection_status(php_oci_connection * TSRMLS_DC);
static int php_oci_connection_close(php_oci_connection * TSRMLS_DC);
/* }}} */

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_OCI8
ZEND_GET_MODULE(oci8)
#endif /* COMPILE_DL */
/* }}} */

#ifdef ZEND_ENGINE_2
ZEND_BEGIN_ARG_INFO(oci_second_arg_force_ref, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(oci_third_arg_force_ref, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()
#else
static unsigned char oci_second_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char oci_third_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
#endif

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
PHP_FUNCTION(oci_new_cursor);
PHP_FUNCTION(oci_result);
PHP_FUNCTION(oci_server_version);
PHP_FUNCTION(oci_statement_type);
PHP_FUNCTION(oci_num_rows);
PHP_FUNCTION(oci_set_prefetch);
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
#ifdef HAVE_OCI8_TEMP_LOB
PHP_FUNCTION(oci_lob_write_temporary);
PHP_FUNCTION(oci_lob_close);
#endif
#ifdef PHP_OCI8_HAVE_COLLECTIONS
PHP_FUNCTION(oci_new_collection);
PHP_FUNCTION(oci_free_collection);
PHP_FUNCTION(oci_collection_append);
PHP_FUNCTION(oci_collection_element_get);
PHP_FUNCTION(oci_collection_element_assign);
PHP_FUNCTION(oci_collection_assign);
PHP_FUNCTION(oci_collection_size);
PHP_FUNCTION(oci_collection_max);
PHP_FUNCTION(oci_collection_trim);
#endif
/* }}} */

/* {{{ extension definition structures
*/
static zend_function_entry php_oci_functions[] = {
	PHP_FE(oci_define_by_name,          oci_third_arg_force_ref)
	PHP_FE(oci_bind_by_name,            oci_third_arg_force_ref)
	PHP_FE(oci_bind_array_by_name,      oci_third_arg_force_ref)
	PHP_FE(oci_field_is_null,           NULL)
	PHP_FE(oci_field_name,              NULL)
	PHP_FE(oci_field_size,              NULL)
	PHP_FE(oci_field_scale,             NULL)
	PHP_FE(oci_field_precision,         NULL)
	PHP_FE(oci_field_type,              NULL)
	PHP_FE(oci_field_type_raw,          NULL)
	PHP_FE(oci_execute,                 NULL)
	PHP_FE(oci_cancel,                  NULL)
	PHP_FE(oci_fetch,                   NULL)
	PHP_FE(oci_fetch_object,            NULL)
	PHP_FE(oci_fetch_row,               NULL)
	PHP_FE(oci_fetch_assoc,             NULL)
	PHP_FE(oci_fetch_array,             NULL)
	PHP_FE(ocifetchinto,                oci_second_arg_force_ref)
	PHP_FE(oci_fetch_all,               oci_second_arg_force_ref)
	PHP_FE(oci_free_statement,          NULL)
	PHP_FE(oci_internal_debug,          NULL)
	PHP_FE(oci_num_fields,              NULL)
	PHP_FE(oci_parse,                   NULL)
	PHP_FE(oci_new_cursor,              NULL)
	PHP_FE(oci_result,                  NULL)
	PHP_FE(oci_server_version,          NULL)
	PHP_FE(oci_statement_type,          NULL)
	PHP_FE(oci_num_rows,                NULL)
	PHP_FE(oci_close,                   NULL)
	PHP_FE(oci_connect,                 NULL)
	PHP_FE(oci_new_connect,             NULL)
	PHP_FE(oci_pconnect,                NULL)
	PHP_FE(oci_error,                   NULL)
	PHP_FE(oci_free_descriptor,         NULL)
	PHP_FE(oci_lob_save,                NULL)
	PHP_FE(oci_lob_import,              NULL)
	PHP_FE(oci_lob_size,                NULL)	
	PHP_FE(oci_lob_load,                NULL)
	PHP_FE(oci_lob_read,                NULL)
	PHP_FE(oci_lob_eof,                 NULL)	
	PHP_FE(oci_lob_tell,                NULL)
	PHP_FE(oci_lob_truncate,            NULL)
	PHP_FE(oci_lob_erase,               NULL)
	PHP_FE(oci_lob_flush,               NULL)
	PHP_FE(ocisetbufferinglob,          NULL)
	PHP_FE(ocigetbufferinglob,          NULL)
	PHP_FE(oci_lob_is_equal,		    NULL)
	PHP_FE(oci_lob_rewind,              NULL)
	PHP_FE(oci_lob_write,               NULL)
	PHP_FE(oci_lob_append,              NULL)
	PHP_FE(oci_lob_copy,                NULL)
	PHP_FE(oci_lob_export,              NULL)
	PHP_FE(oci_lob_seek,                NULL)
	PHP_FE(oci_commit,                  NULL)
	PHP_FE(oci_rollback,                NULL)
	PHP_FE(oci_new_descriptor,          NULL)
	PHP_FE(oci_set_prefetch,            NULL)
	PHP_FE(oci_password_change,         NULL)
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	PHP_FE(oci_free_collection,         NULL)
	PHP_FE(oci_collection_append,       NULL)
	PHP_FE(oci_collection_element_get,      NULL)
	PHP_FE(oci_collection_element_assign,   NULL)
	PHP_FE(oci_collection_assign,       NULL)
	PHP_FE(oci_collection_size,         NULL)
	PHP_FE(oci_collection_max,          NULL)
	PHP_FE(oci_collection_trim,         NULL)
	PHP_FE(oci_new_collection,          NULL)
#endif
	
	PHP_FALIAS(oci_free_cursor,     oci_free_statement,       NULL)
	PHP_FALIAS(ocifreecursor,       oci_free_statement,       NULL)
	PHP_FALIAS(ocibindbyname,       oci_bind_by_name,       oci_third_arg_force_ref)
	PHP_FALIAS(ocidefinebyname,     oci_define_by_name,     oci_third_arg_force_ref)
	PHP_FALIAS(ocicolumnisnull,     oci_field_is_null,      NULL)
	PHP_FALIAS(ocicolumnname,       oci_field_name,         NULL)
	PHP_FALIAS(ocicolumnsize,       oci_field_size,         NULL)
	PHP_FALIAS(ocicolumnscale,      oci_field_scale,        NULL)
	PHP_FALIAS(ocicolumnprecision,  oci_field_precision,    NULL)
	PHP_FALIAS(ocicolumntype,       oci_field_type,         NULL)
	PHP_FALIAS(ocicolumntyperaw,    oci_field_type_raw,     NULL)
	PHP_FALIAS(ociexecute,          oci_execute,            NULL)
	PHP_FALIAS(ocicancel,           oci_cancel,             NULL)
	PHP_FALIAS(ocifetch,            oci_fetch,              NULL)
	PHP_FALIAS(ocifetchstatement,   oci_fetch_all,          oci_second_arg_force_ref)
	PHP_FALIAS(ocifreestatement,    oci_free_statement,     NULL)
	PHP_FALIAS(ociinternaldebug,    oci_internal_debug,     NULL)
	PHP_FALIAS(ocinumcols,          oci_num_fields,         NULL)
	PHP_FALIAS(ociparse,            oci_parse,              NULL)
	PHP_FALIAS(ocinewcursor,        oci_new_cursor,         NULL)
	PHP_FALIAS(ociresult,           oci_result,             NULL)
	PHP_FALIAS(ociserverversion,    oci_server_version,     NULL)
	PHP_FALIAS(ocistatementtype,    oci_statement_type,     NULL)
	PHP_FALIAS(ocirowcount,         oci_num_rows,           NULL)
	PHP_FALIAS(ocilogoff,           oci_close,              NULL)
	PHP_FALIAS(ocilogon,            oci_connect,            NULL)
	PHP_FALIAS(ocinlogon,           oci_new_connect,        NULL)
	PHP_FALIAS(ociplogon,           oci_pconnect,           NULL)
	PHP_FALIAS(ocierror,            oci_error,              NULL)
	PHP_FALIAS(ocifreedesc,         oci_free_descriptor,    NULL)
	PHP_FALIAS(ocisavelob,          oci_lob_save,           NULL)
	PHP_FALIAS(ocisavelobfile,      oci_lob_import,         NULL)
	PHP_FALIAS(ociwritelobtofile,   oci_lob_export,         NULL)
	PHP_FALIAS(ociloadlob,          oci_lob_load,           NULL)
	PHP_FALIAS(ocicommit,           oci_commit,             NULL)
	PHP_FALIAS(ocirollback,         oci_rollback,           NULL)
	PHP_FALIAS(ocinewdescriptor,    oci_new_descriptor,     NULL)
	PHP_FALIAS(ocisetprefetch,      oci_set_prefetch,       NULL)
	PHP_FALIAS(ocipasswordchange,   oci_password_change,    NULL)
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	PHP_FALIAS(ocifreecollection,   oci_free_collection,    NULL)
	PHP_FALIAS(ocinewcollection,    oci_new_collection,     NULL)
	PHP_FALIAS(ocicollappend,       oci_collection_append,  NULL)
	PHP_FALIAS(ocicollgetelem,      oci_collection_element_get,     NULL)
	PHP_FALIAS(ocicollassignelem,   oci_collection_element_assign,  NULL)
	PHP_FALIAS(ocicollsize,         oci_collection_size,    NULL)
	PHP_FALIAS(ocicollmax,          oci_collection_max,     NULL)
	PHP_FALIAS(ocicolltrim,         oci_collection_trim,    NULL)
#endif
	{NULL,NULL,NULL}
};

static zend_function_entry php_oci_lob_class_functions[] = {
	PHP_FALIAS(load,        oci_lob_load,           NULL)
	PHP_FALIAS(tell,        oci_lob_tell,           NULL)
	PHP_FALIAS(truncate,    oci_lob_truncate,       NULL)
	PHP_FALIAS(erase,       oci_lob_erase,          NULL)
	PHP_FALIAS(flush,       oci_lob_flush,          NULL)
	PHP_FALIAS(setbuffering,ocisetbufferinglob,     NULL)
	PHP_FALIAS(getbuffering,ocigetbufferinglob,     NULL)
	PHP_FALIAS(rewind,      oci_lob_rewind,			NULL)
	PHP_FALIAS(read,        oci_lob_read,           NULL)
	PHP_FALIAS(eof,         oci_lob_eof,            NULL)
	PHP_FALIAS(seek,        oci_lob_seek,           NULL)	
	PHP_FALIAS(write,       oci_lob_write,          NULL)
	PHP_FALIAS(append,      oci_lob_append,         NULL)
	PHP_FALIAS(size,        oci_lob_size,           NULL)
	PHP_FALIAS(writetofile, oci_lob_export,         NULL)
	PHP_FALIAS(export,      oci_lob_export,         NULL)
	PHP_FALIAS(import,      oci_lob_import,         NULL)
#ifdef HAVE_OCI8_TEMP_LOB
	PHP_FALIAS(writetemporary,  oci_lob_write_temporary,    NULL)
	PHP_FALIAS(close,           oci_lob_close,              NULL)
#endif
	PHP_FALIAS(save,        oci_lob_save,           NULL)
	PHP_FALIAS(savefile,    oci_lob_import,         NULL)
	PHP_FALIAS(free,        oci_free_descriptor,    NULL)
	{NULL,NULL,NULL}
};

#ifdef PHP_OCI8_HAVE_COLLECTIONS
static zend_function_entry php_oci_coll_class_functions[] = {
	PHP_FALIAS(append,        oci_collection_append,            NULL)
	PHP_FALIAS(getelem,       oci_collection_element_get,       NULL)
	PHP_FALIAS(assignelem,    oci_collection_element_assign,    NULL)
	PHP_FALIAS(assign,        oci_collection_assign,            NULL)
	PHP_FALIAS(size,          oci_collection_size,              NULL)
	PHP_FALIAS(max,           oci_collection_max,               NULL)
	PHP_FALIAS(trim,          oci_collection_trim,              NULL)
	PHP_FALIAS(free,          oci_free_collection,              NULL)
	{NULL,NULL,NULL}
};
#endif

zend_module_entry oci8_module_entry = {
	STANDARD_MODULE_HEADER,
	"oci8",               /* extension name */
	php_oci_functions,    /* extension function list */
	PHP_MINIT(oci),       /* extension-wide startup function */
	PHP_MSHUTDOWN(oci),   /* extension-wide shutdown function */
	PHP_RINIT(oci),       /* per-request startup function */
	PHP_RSHUTDOWN(oci),   /* per-request shutdown function */
	PHP_MINFO(oci),       /* information function */
	"1.2.5",
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
	PHP_MODULE_GLOBALS(oci),  /* globals descriptor */
	PHP_GINIT(oci),           /* globals ctor */
	NULL,                     /* globals dtor */
	NULL,                     /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
#else
	STANDARD_MODULE_PROPERTIES
#endif
};
/* }}} */

/* {{{ PHP_INI */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("oci8.max_persistent",		"-1",	PHP_INI_SYSTEM,		ONUPDATELONGFUNC,		max_persistent,		zend_oci_globals,	oci_globals)
    STD_PHP_INI_ENTRY("oci8.persistent_timeout",	"-1",	PHP_INI_SYSTEM,		ONUPDATELONGFUNC,		persistent_timeout,	zend_oci_globals,	oci_globals)
    STD_PHP_INI_ENTRY("oci8.ping_interval",		"60",	PHP_INI_SYSTEM,		ONUPDATELONGFUNC,		ping_interval,		zend_oci_globals,	oci_globals)
    STD_PHP_INI_BOOLEAN("oci8.privileged_connect",	"0",	PHP_INI_SYSTEM,		OnUpdateBool,		privileged_connect,	zend_oci_globals,	oci_globals)
    STD_PHP_INI_ENTRY("oci8.statement_cache_size",		"20",	PHP_INI_SYSTEM,		ONUPDATELONGFUNC,		statement_cache_size,		zend_oci_globals,	oci_globals)
    STD_PHP_INI_ENTRY("oci8.default_prefetch",		"10",	PHP_INI_SYSTEM,		ONUPDATELONGFUNC,		default_prefetch,		zend_oci_globals,	oci_globals)
    STD_PHP_INI_ENTRY("oci8.old_oci_close_semantics",		"0",	PHP_INI_SYSTEM,		OnUpdateBool,		old_oci_close_semantics,		zend_oci_globals,	oci_globals)
PHP_INI_END()
/* }}} */

/* {{{ startup, shutdown and info functions
*/

/* {{{  php_oci_init_global_handles()
 Initialize global handles only when they are needed 
*/
static void php_oci_init_global_handles(TSRMLS_D)
{
	sword errcode;
	sb4 error_code = 0;
	text tmp_buf[PHP_OCI_ERRBUF_LEN];

	errcode = OCIEnvInit (&OCI_G(env), OCI_DEFAULT, 0, NULL);
	
	if (errcode == OCI_ERROR) {
		goto oci_error;
	}
	
	errcode = OCIHandleAlloc (OCI_G(env), (dvoid **)&OCI_G(err), OCI_HTYPE_ERROR, 0, NULL);
	
	if (errcode == OCI_ERROR || errcode == OCI_SUCCESS_WITH_INFO) {
		goto oci_error;
	}

	return;
	
oci_error:
	
	OCIErrorGet(OCI_G(env), (ub4)1, NULL, &error_code, tmp_buf, (ub4)PHP_OCI_ERRBUF_LEN, (ub4)OCI_HTYPE_ERROR);

	if (error_code) {
		int tmp_buf_len = strlen((char *)tmp_buf);

		if (tmp_buf_len > 0 && tmp_buf[tmp_buf_len - 1] == '\n') {
			tmp_buf[tmp_buf_len - 1] = '\0';
		}
			
		if (errcode != OCI_SUCCESS_WITH_INFO) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_ERROR: %s", tmp_buf);
			
			OCIHandleFree((dvoid *) OCI_G(env), OCI_HTYPE_ENV);
			
			OCI_G(env) = NULL;
			OCI_G(err) = NULL;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_SUCCESS_WITH_INFO: %s", tmp_buf);
		}
	}
} /* }}} */

/* {{{ php_oci_cleanup_global_handles()
 Free global handles (if they were initialized before)
*/
static void php_oci_cleanup_global_handles(TSRMLS_D)
{
	if (OCI_G(err)) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) OCI_G(err), OCI_HTYPE_ERROR));
		OCI_G(err) = NULL;
	}
	
	if (OCI_G(env)) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) OCI_G(env), OCI_HTYPE_ENV));
		OCI_G(env) = NULL;
	}
} /* }}} */

/* {{{ PHP_GINIT_FUNCTION
 Zerofill globals during module init
*/
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
static PHP_GINIT_FUNCTION(oci)
#else
static void php_oci_init_globals(zend_oci_globals *oci_globals TSRMLS_DC)
#endif
{
	memset(oci_globals, 0, sizeof(zend_oci_globals));
}
/* }}} */

PHP_MINIT_FUNCTION(oci)
{
	zend_class_entry oci_lob_class_entry;
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	zend_class_entry oci_coll_class_entry;
#endif

#ifdef PHP_OCI8_HAVE_COLLECTIONS
#define PHP_OCI_INIT_MODE_TMP OCI_DEFAULT | OCI_OBJECT
#else
#define PHP_OCI_INIT_MODE_TMP OCI_DEFAULT
#endif

#ifdef ZTS
#define PHP_OCI_INIT_MODE PHP_OCI_INIT_MODE_TMP | OCI_THREADED
#else
#define PHP_OCI_INIT_MODE PHP_OCI_INIT_MODE_TMP
#endif

#if !HAVE_OCI_ENV_CREATE
	OCIInitialize(PHP_OCI_INIT_MODE, NULL, NULL, NULL, NULL);
#endif

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
	/* this is handled by new globals management code */
#else
	ZEND_INIT_MODULE_GLOBALS(oci, php_oci_init_globals, NULL);
#endif
	REGISTER_INI_ENTRIES();

	le_statement = zend_register_list_destructors_ex(php_oci_statement_list_dtor, NULL, "oci8 statement", module_number);
	le_connection = zend_register_list_destructors_ex(php_oci_connection_list_dtor, NULL, "oci8 connection", module_number);
	le_pconnection = zend_register_list_destructors_ex(NULL, php_oci_pconnection_list_dtor, "oci8 persistent connection", module_number);
	le_descriptor = zend_register_list_destructors_ex(php_oci_descriptor_list_dtor, NULL, "oci8 descriptor", module_number);
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	le_collection = zend_register_list_destructors_ex(php_oci_collection_list_dtor, NULL, "oci8 collection", module_number);
#endif

	INIT_CLASS_ENTRY(oci_lob_class_entry, "OCI-Lob", php_oci_lob_class_functions);
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	INIT_CLASS_ENTRY(oci_coll_class_entry, "OCI-Collection", php_oci_coll_class_functions);
#endif

	oci_lob_class_entry_ptr = zend_register_internal_class(&oci_lob_class_entry TSRMLS_CC);
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	oci_coll_class_entry_ptr = zend_register_internal_class(&oci_coll_class_entry TSRMLS_CC);
#endif

/* thies@thieso.net 990203 i do not think that we will need all of them - just in here for completeness for now! */
	REGISTER_LONG_CONSTANT("OCI_DEFAULT",OCI_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SYSOPER",OCI_SYSOPER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SYSDBA",OCI_SYSDBA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DESCRIBE_ONLY",OCI_DESCRIBE_ONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_COMMIT_ON_SUCCESS",OCI_COMMIT_ON_SUCCESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_EXACT_FETCH",OCI_EXACT_FETCH, CONST_CS | CONST_PERSISTENT);

/* for $LOB->seek() */
	REGISTER_LONG_CONSTANT("OCI_SEEK_SET",PHP_OCI_SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_CUR",PHP_OCI_SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_END",PHP_OCI_SEEK_END, CONST_CS | CONST_PERSISTENT);

/*	for $LOB->flush() */
	REGISTER_LONG_CONSTANT("OCI_LOB_BUFFER_FREE",OCI_LOB_BUFFER_FREE, CONST_CS | CONST_PERSISTENT);
	
/* for OCIBindByName (real "oci" names + short "php" names*/
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
#if defined(HAVE_OCI_INSTANT_CLIENT) || (defined(OCI_MAJOR_VERSION) && OCI_MAJOR_VERSION > 10)
	REGISTER_LONG_CONSTANT("SQLT_BDOUBLE",SQLT_BDOUBLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BFLOAT",SQLT_BFLOAT, CONST_CS | CONST_PERSISTENT);
#endif

#ifdef PHP_OCI8_HAVE_COLLECTIONS
	REGISTER_LONG_CONSTANT("OCI_B_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);	
	REGISTER_STRING_CONSTANT("OCI_SYSDATE","SYSDATE", CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("OCI_B_BFILE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_ROWID",SQLT_RDD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CURSOR",SQLT_RSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BIN",SQLT_BIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_INT",SQLT_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_NUM",SQLT_NUM, CONST_CS | CONST_PERSISTENT);

/* for OCIFetchStatement */
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_COLUMN", PHP_OCI_FETCHSTATEMENT_BY_COLUMN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_ROW", PHP_OCI_FETCHSTATEMENT_BY_ROW, CONST_CS | CONST_PERSISTENT);

/* for OCIFetchInto &  OCIResult */
	REGISTER_LONG_CONSTANT("OCI_ASSOC",PHP_OCI_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_NUM",PHP_OCI_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_BOTH",PHP_OCI_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_NULLS",PHP_OCI_RETURN_NULLS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_LOBS",PHP_OCI_RETURN_LOBS, CONST_CS | CONST_PERSISTENT);

/* for OCINewDescriptor (real "oci" names + short "php" names*/
	REGISTER_LONG_CONSTANT("OCI_DTYPE_FILE",OCI_DTYPE_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_LOB",OCI_DTYPE_LOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_ROWID",OCI_DTYPE_ROWID, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("OCI_D_FILE",OCI_DTYPE_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_LOB",OCI_DTYPE_LOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_ROWID",OCI_DTYPE_ROWID, CONST_CS | CONST_PERSISTENT);

/* for OCIWriteTemporaryLob */
#ifdef HAVE_OCI8_TEMP_LOB
	REGISTER_LONG_CONSTANT("OCI_TEMP_CLOB",OCI_TEMP_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_TEMP_BLOB",OCI_TEMP_BLOB, CONST_CS | CONST_PERSISTENT);
#endif

	return SUCCESS;
}

/* ----------------------------------------------------------------- */

PHP_RINIT_FUNCTION(oci)
{
	OCI_G(debug_mode) = 0; /* start "fresh" */
	OCI_G(num_links) = OCI_G(num_persistent);
	OCI_G(errcode) = 0;

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(oci)
{
	OCI_G(shutdown) = 1;
	
	UNREGISTER_INI_ENTRIES();

#ifndef ZTS
	php_oci_cleanup_global_handles(TSRMLS_C);
#endif

#if !HAVE_OCI_ENV_CREATE
	OCITerminate(OCI_DEFAULT);
#endif

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(oci)
{
#ifdef ZTS
	zend_hash_apply_with_argument(&EG(regular_list), (apply_func_arg_t) php_oci_list_helper, (void *)le_descriptor TSRMLS_CC);
#ifdef PHP_OCI8_HAVE_COLLECTIONS 
	zend_hash_apply_with_argument(&EG(regular_list), (apply_func_arg_t) php_oci_list_helper, (void *)le_collection TSRMLS_CC);
#endif
	while (OCI_G(num_statements)) { 
		zend_hash_apply_with_argument(&EG(regular_list), (apply_func_arg_t) php_oci_list_helper, (void *)le_statement TSRMLS_CC);
	}
#endif

	/* check persistent connections and do the necessary actions if needed */
	zend_hash_apply(&EG(persistent_list), (apply_func_t) php_oci_persistent_helper TSRMLS_CC);

#ifdef ZTS
	zend_hash_apply_with_argument(&EG(regular_list), (apply_func_arg_t) php_oci_list_helper, (void *)le_connection TSRMLS_CC);
	php_oci_cleanup_global_handles(TSRMLS_C);
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(oci)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_row(2, "OCI8 Support", "enabled");
	php_info_print_table_row(2, "Version", "1.2.5");
	php_info_print_table_row(2, "Revision", "$Revision$");

	snprintf(buf, sizeof(buf), "%ld", OCI_G(num_persistent));
	php_info_print_table_row(2, "Active Persistent Connections", buf);
	snprintf(buf, sizeof(buf), "%ld", OCI_G(num_links));
	php_info_print_table_row(2, "Active Connections", buf);

#if !defined(PHP_WIN32) && !defined(HAVE_OCI_INSTANT_CLIENT)
	php_info_print_table_row(2, "Oracle Version", PHP_OCI8_VERSION );
	php_info_print_table_row(2, "Compile-time ORACLE_HOME", PHP_OCI8_DIR );
	php_info_print_table_row(2, "Libraries Used", PHP_OCI8_SHARED_LIBADD );
#else 
#	if defined(HAVE_OCI_INSTANT_CLIENT) && defined(OCI_MAJOR_VERSION) && defined(OCI_MINOR_VERSION) 
	snprintf(buf, sizeof(buf), "%d.%d", OCI_MAJOR_VERSION, OCI_MINOR_VERSION);
	php_info_print_table_row(2, "Oracle Instant Client Version", buf);
#	endif
#endif

#ifdef HAVE_OCI8_TEMP_LOB
	php_info_print_table_row(2, "Temporary Lob support", "enabled" );
#else
	php_info_print_table_row(2, "Temporary Lob support", "disabled" );
#endif

#ifdef PHP_OCI8_HAVE_COLLECTIONS
	php_info_print_table_row(2, "Collections support", "enabled" );
#else
	php_info_print_table_row(2, "Collections support", "disabled" );
#endif

	php_info_print_table_end();
	
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* list destructors {{{ */

/* {{{ php_oci_connection_list_dtor()
 Non-persistent connection destructor */
static void php_oci_connection_list_dtor(zend_rsrc_list_entry *entry TSRMLS_DC)
{
	php_oci_connection *connection = (php_oci_connection *)entry->ptr;
	php_oci_connection_close(connection TSRMLS_CC);
	OCI_G(num_links)--;
} /* }}} */

/* {{{ php_oci_pconnection_list_dtor()
 Persistent connection destructor */
static void php_oci_pconnection_list_dtor(zend_rsrc_list_entry *entry TSRMLS_DC)
{
	php_oci_connection *connection = (php_oci_connection *)entry->ptr;
	php_oci_connection_close(connection TSRMLS_CC);
	OCI_G(num_persistent)--;
} /* }}} */

/* {{{ php_oci_statement_list_dtor()
 Statement destructor */
static void php_oci_statement_list_dtor(zend_rsrc_list_entry *entry TSRMLS_DC)
{
	php_oci_statement *statement = (php_oci_statement *)entry->ptr;
	php_oci_statement_free(statement TSRMLS_CC);
} /* }}} */

/* {{{ php_oci_descriptor_list_dtor()
 Descriptor destructor */
static void php_oci_descriptor_list_dtor(zend_rsrc_list_entry *entry TSRMLS_DC)
{
	php_oci_descriptor *descriptor = (php_oci_descriptor *)entry->ptr;
	php_oci_lob_free(descriptor TSRMLS_CC);
} /* }}} */

#ifdef PHP_OCI8_HAVE_COLLECTIONS 
/* {{{ php_oci_collection_list_dtor()
 Collection destructor */
static void php_oci_collection_list_dtor(zend_rsrc_list_entry *entry TSRMLS_DC)
{
	php_oci_collection *collection = (php_oci_collection *)entry->ptr;
	php_oci_collection_close(collection TSRMLS_CC);
} /* }}} */
#endif

/* }}} */

/* hash destructors {{{ */

/* {{{ php_oci_define_hash_dtor()
 Define hash destructor */
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
 Bind hash destructor */
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

/*
	if (bind->array.retcodes) {
		efree(bind->array.retcodes);
	} 
*/

	zval_ptr_dtor(&bind->zval);
}
/* }}} */

/* {{{ php_oci_column_hash_dtor()
 Column hash destructor */
void php_oci_column_hash_dtor(void *data)
{
	php_oci_out_column *column = (php_oci_out_column *) data;
	TSRMLS_FETCH();

	if (column->stmtid) {
		zend_list_delete(column->stmtid);
	}

	if (column->is_descr) {
		zend_list_delete(column->descid);
	}

	if (column->data) {
		efree(column->data);
	}

	if (column->name) {
		efree(column->name);
	}
}
/* }}} */

/* {{{ php_oci_descriptor_flush_hash_dtor()
 Flush descriptors on commit */
void php_oci_descriptor_flush_hash_dtor(void *data)
{
	php_oci_descriptor *descriptor = *(php_oci_descriptor **)data;
	TSRMLS_FETCH();
	
	if (descriptor && descriptor->buffering == PHP_OCI_LOB_BUFFER_USED && (descriptor->type == OCI_DTYPE_LOB || descriptor->type == OCI_DTYPE_FILE)) {
		php_oci_lob_flush(descriptor, OCI_LOB_BUFFER_FREE TSRMLS_CC);
		descriptor->buffering = PHP_OCI_LOB_BUFFER_ENABLED;
	}
	data = NULL;
}
/* }}} */

/* {{{ php_oci_descriptor_delete_from_hash()
 Delete descriptor from the hash */
int php_oci_descriptor_delete_from_hash(void *data, void *id TSRMLS_DC)
{
	php_oci_descriptor *descriptor = *(php_oci_descriptor **)data;
	int *desc_id = (int *) id;
	
	if (descriptor && desc_id && descriptor->id == *desc_id) {
		return 1;
	}
	return 0;
}
/* }}} */

/* }}} */

/* {{{ php_oci_error()
 Fetch & print out error message if we get an error */
sb4 php_oci_error(OCIError *err_p, sword status TSRMLS_DC)
{
	text *errbuf = (text *)NULL;
	sb4 errcode = 0;

	switch (status) {
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
			errcode = php_oci_fetch_errmsg(err_p, &errbuf TSRMLS_CC);
			if (errbuf) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_SUCCESS_WITH_INFO: %s", errbuf);
				efree(errbuf);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_SUCCESS_WITH_INFO: failed to fetch error message");
			}
			break;
		case OCI_NEED_DATA:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_NEED_DATA");
			break;
		case OCI_NO_DATA:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_NO_DATA");
			break;
		case OCI_ERROR: 
			errcode = php_oci_fetch_errmsg(err_p, &errbuf TSRMLS_CC);
			if (errbuf) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errbuf);
				efree(errbuf);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to fetch error message");
			}
			break;
		case OCI_INVALID_HANDLE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_INVALID_HANDLE");
			break;
		case OCI_STILL_EXECUTING:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_STILL_EXECUTING");
			break;
		case OCI_CONTINUE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI_CONTINUE");
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown OCI error code: %d", status);
			break;
	}
	return errcode;
}
/* }}} */

/* {{{ php_oci_fetch_errmsg()
 Fetch error message into the buffer from the error handle provided */
sb4 php_oci_fetch_errmsg(OCIError *error_handle, text **error_buf TSRMLS_DC)
{
	sb4 error_code = 0;
	text tmp_buf[PHP_OCI_ERRBUF_LEN];

	tmp_buf[0] = '\0';
	
	PHP_OCI_CALL(OCIErrorGet, (error_handle, (ub4)1, NULL, &error_code, tmp_buf, (ub4)PHP_OCI_ERRBUF_LEN, (ub4)OCI_HTYPE_ERROR));
	
	if (error_code) {
		int tmp_buf_len = strlen((char *)tmp_buf);
		
		if (tmp_buf_len && tmp_buf[tmp_buf_len - 1] == '\n') {
			tmp_buf[tmp_buf_len - 1] = '\0';
		}
		if (tmp_buf_len && error_buf) {
			*error_buf = NULL;
			*error_buf = (text *)estrndup((char *)tmp_buf, tmp_buf_len);
		}
	}
	return error_code;
} /* }}} */

#ifdef HAVE_OCI8_ATTR_STATEMENT
/* {{{ php_oci_fetch_sqltext_offset()
 Compute offset in the SQL statement */
int php_oci_fetch_sqltext_offset(php_oci_statement *statement, text **sqltext, ub2 *error_offset TSRMLS_DC)
{
	*sqltext = NULL;
	*error_offset = 0;

	PHP_OCI_CALL_RETURN(statement->errcode, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (text *) sqltext, (ub4 *)0, OCI_ATTR_STATEMENT, statement->err));
	
	if (statement->errcode != OCI_SUCCESS) { 
		php_oci_error(statement->err, statement->errcode TSRMLS_CC);
		return 1;
	}

	PHP_OCI_CALL_RETURN(statement->errcode, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (ub2 *)error_offset, (ub4 *)0, OCI_ATTR_PARSE_ERROR_OFFSET, statement->err));
	
	if (statement->errcode != OCI_SUCCESS) { 
		php_oci_error(statement->err, statement->errcode TSRMLS_CC);
		return 1;
	}
	return 0;
} /* }}} */
#endif

/* {{{ php_oci_do_connect() 
 Connect wrapper */
void php_oci_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent, int exclusive)
{
	php_oci_connection *connection;
	char *username, *password;
	char *dbname = NULL, *charset = NULL;
	int username_len = 0, password_len = 0;
	int dbname_len = 0, charset_len = 0;
	long session_mode = OCI_DEFAULT;

	/* if a fourth parameter is handed over, it is the charset identifier (but is only used in Oracle 9i+) */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ssl", &username, &username_len, &password, &password_len, &dbname, &dbname_len, &charset, &charset_len, &session_mode) == FAILURE) {
		return;
	} 
	
	if (!charset_len) {
		charset = NULL;
	}
	
	connection = php_oci_do_connect_ex(username, username_len, password, password_len, NULL, 0, dbname, dbname_len, charset, session_mode, persistent, exclusive TSRMLS_CC);

	if (!connection) {
		RETURN_FALSE;
	}
	RETURN_RESOURCE(connection->rsrc_id);
	
} /* }}} */

/* {{{ php_oci_do_connect_ex()
 * The real connect function. Allocates all the resources needed, establishes the connection and returns the result handle (or NULL) */
php_oci_connection *php_oci_do_connect_ex(char *username, int username_len, char *password, int password_len, char *new_password, int new_password_len, char *dbname, int dbname_len, char *charset, long session_mode, int persistent, int exclusive TSRMLS_DC) 
{
	zend_rsrc_list_entry *le;
	zend_rsrc_list_entry new_le;
	php_oci_connection *connection = NULL;
	smart_str hashed_details = {0};
	time_t timestamp;
#if HAVE_OCI_ENV_NLS_CREATE
	ub2 charsetid = 0;
	ub2 charsetid_nls_lang = 0;
#endif
	
	switch (session_mode) {
		case OCI_DEFAULT:
			break;
		case OCI_SYSOPER:
		case OCI_SYSDBA:
			if (!OCI_G(privileged_connect)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Privileged connect is disabled. Enable oci8.privileged_connect to be able to connect as SYSOPER or SYSDBA");
				return NULL;
			}
			/*  Disable privileged connections in Safe Mode (N.b. safe mode has been removed in PHP 6 anyway) */
			if (PG(safe_mode)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Privileged connect is disabled in Safe Mode");
				return NULL;
			}
			/* Increase security by not caching privileged
			 * oci_pconnect() connections. The connection becomes
			 * equivalent to oci_connect() or oci_new_connect().
			 */
			persistent = 0;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid session mode specified (%ld)", session_mode);
			return NULL;
			break;
	}

	smart_str_appendl_ex(&hashed_details, "oci8___", sizeof("oci8___") - 1, 0);
	smart_str_appendl_ex(&hashed_details, username, username_len, 0);
	smart_str_appendl_ex(&hashed_details, "__", sizeof("__") - 1, 0);
	if (password_len) {
		ulong password_hash;
		password_hash = zend_inline_hash_func(password, password_len);
		smart_str_append_unsigned_ex(&hashed_details, password_hash, 0);
	}
	smart_str_appendl_ex(&hashed_details, "__", sizeof("__") - 1, 0);

	if (dbname) {
		smart_str_appendl_ex(&hashed_details, dbname, dbname_len, 0);
	}
	smart_str_appendl_ex(&hashed_details, "__", sizeof("__") - 1, 0);

	/* Initialize global handles if the weren't initialized before */
	if (OCI_G(env) == NULL) {
		php_oci_init_global_handles(TSRMLS_C);
	}

#if HAVE_OCI_ENV_NLS_CREATE
	if (charset && *charset) {
		PHP_OCI_CALL_RETURN(charsetid, OCINlsCharSetNameToId, (OCI_G(env), (CONST oratext *)charset));
		if (!charsetid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid character set name: %s", charset);
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
#else
	if (charset && *charset) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Your version of Oracle Client doesn't support setting the charset; bad or no charset conversions may result");
	}
#endif

	timestamp = time(NULL);
	
	smart_str_append_unsigned_ex(&hashed_details, session_mode, 0);
	smart_str_0(&hashed_details);

	/* make it lowercase */
	php_strtolower(hashed_details.c, hashed_details.len);
	
	if (!exclusive && !new_password) {
		zend_bool found = 0;

		if (persistent && zend_hash_find(&EG(persistent_list), hashed_details.c, hashed_details.len+1, (void **) &le) == SUCCESS) {
			found = 1;
			/* found */
			if (le->type == le_pconnection) {
				connection = (php_oci_connection *)le->ptr;	
			}
		} else if (!persistent && zend_hash_find(&EG(regular_list), hashed_details.c, hashed_details.len+1, (void **) &le) == SUCCESS) {
			found = 1;
			if (le->type == le_index_ptr) {
				int type, link;
				void *ptr;

				link = (int) le->ptr;
				ptr = zend_list_find(link,&type);
				if (ptr && (type == le_connection)) {
					connection = (php_oci_connection *)ptr;
				}
			}
		}

		if (connection) {
			if (connection->is_open) {
				/* found an open connection. now ping it */
				if (connection->is_persistent) {
					/* check connection liveness in the following order:
					 * 1) always check OCI_ATTR_SERVER_STATUS
					 * 2) see if it's time to ping it
					 * 3) ping it if needed 
					 * */
					if (php_oci_connection_status(connection TSRMLS_CC)) {
						/* only ping if: 
						 * 1) next_ping > 0, which means that ping_interval is not -1 (aka "Off")
						 * 2) current_timestamp > next_ping, which means "it's time to check if it's still alive"
						 * */
						if ( (connection->next_ping > 0) && (timestamp >= connection->next_ping) && !php_oci_connection_ping(connection TSRMLS_CC)) {
							/* server died */
						} else {
							int rsrc_type;
							php_oci_connection *tmp;

							/* okay, the connection is open and the server is still alive */
							connection->used_this_request = 1;
							tmp = (php_oci_connection *)zend_list_find(connection->rsrc_id, &rsrc_type);

							if (tmp != NULL && rsrc_type == le_pconnection && strlen(tmp->hash_key) == hashed_details.len &&
								memcmp(tmp->hash_key, hashed_details.c, hashed_details.len) == 0 && zend_list_addref(connection->rsrc_id) == SUCCESS) {
								/* do nothing */
							} else {
								connection->rsrc_id = zend_list_insert(connection, le_pconnection);
							}
							smart_str_free_ex(&hashed_details, 0);
							return connection;
						}
					}
					/* server died */
					connection->is_open = 0;
					zend_hash_del(&EG(persistent_list), hashed_details.c, hashed_details.len+1);
					connection = NULL;
					goto open;
				} else {
					/* we do not ping non-persistent connections */
					smart_str_free_ex(&hashed_details, 0);
					zend_list_addref(connection->rsrc_id);
					return connection;
				}
			} else {
				zend_hash_del(&EG(regular_list), hashed_details.c, hashed_details.len+1);
				connection = NULL;
				goto open;
			}
		} else if (found) {
			/* found something, but it's not a connection, delete it */
			if (persistent) {
				zend_hash_del(&EG(persistent_list), hashed_details.c, hashed_details.len+1);
			} else {
				zend_hash_del(&EG(regular_list), hashed_details.c, hashed_details.len+1);
			}
		}
	}
open:
	if (persistent) {
		zend_bool alloc_non_persistent = 0;
			
		if (OCI_G(max_persistent)!=-1 && OCI_G(num_persistent)>=OCI_G(max_persistent)) {
			/* try to find an idle connection and kill it */
			zend_hash_apply(&EG(persistent_list), (apply_func_t) php_oci_persistent_helper TSRMLS_CC);
			
			if (OCI_G(max_persistent)!=-1 && OCI_G(num_persistent)>=OCI_G(max_persistent)) {
				/* all persistent connactions are in use, fallback to non-persistent connection creation */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Too many open persistent connections (%ld)", OCI_G(num_persistent));
				alloc_non_persistent = 1;
			}
		}
		
		if (alloc_non_persistent) {
			connection = (php_oci_connection *) ecalloc(1, sizeof(php_oci_connection));
			connection->hash_key = estrndup(hashed_details.c, hashed_details.len);
			connection->is_persistent = 0;
		} else {
			connection = (php_oci_connection *) calloc(1, sizeof(php_oci_connection));
			connection->hash_key = zend_strndup(hashed_details.c, hashed_details.len);
			connection->is_persistent = 1;
		}
	} else {
		connection = (php_oci_connection *) ecalloc(1, sizeof(php_oci_connection));
		connection->hash_key = estrndup(hashed_details.c, hashed_details.len);
		connection->is_persistent = 0;
	}

	connection->idle_expiry = (OCI_G(persistent_timeout) > 0) ? (timestamp + OCI_G(persistent_timeout)) : 0;
	if (OCI_G(ping_interval) >= 0) {
		connection->next_ping = timestamp + OCI_G(ping_interval);
	} else {
		/* -1 means "Off" */
		connection->next_ping = 0;
	}

	/* mark password as unchanged by PHP during the duration of the database session */
	connection->passwd_changed = 0;
	
	smart_str_free_ex(&hashed_details, 0);

	/* allocate environment handle */
#if HAVE_OCI_ENV_NLS_CREATE
#define PHP_OCI_INIT_FUNC_NAME "OCIEnvNlsCreate"
	
	if (charsetid) {
		connection->charset = charsetid;
	} else {
		connection->charset = charsetid_nls_lang;
	}

	/* create an environment using the character set id, Oracle 9i+ ONLY */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIEnvNlsCreate, (&(connection->env), PHP_OCI_INIT_MODE, 0, NULL, NULL, NULL, 0, NULL, connection->charset, connection->charset));

#elif HAVE_OCI_ENV_CREATE
#define PHP_OCI_INIT_FUNC_NAME "OCIEnvCreate"
	
	/* allocate env handle without NLS support */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIEnvCreate, (&(connection->env), PHP_OCI_INIT_MODE, 0, NULL, NULL, NULL, 0, NULL));
#else
#define PHP_OCI_INIT_FUNC_NAME "OCIEnvInit"
	
	/* the simpliest way */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIEnvInit, (&(connection->env), OCI_DEFAULT, 0, NULL));
#endif

	if (OCI_G(errcode) != OCI_SUCCESS) {
#ifdef HAVE_OCI_INSTANT_CLIENT
# ifdef PHP_WIN32
		php_error_docref(NULL TSRMLS_CC, E_WARNING, PHP_OCI_INIT_FUNC_NAME "() failed. There is something wrong with your system - please check that PATH includes the directory with Oracle Instant Client libraries");
# else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, PHP_OCI_INIT_FUNC_NAME "() failed. There is something wrong with your system - please check that LD_LIBRARY_PATH includes the directory with Oracle Instant Client libraries");
# endif
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, PHP_OCI_INIT_FUNC_NAME "() failed. There is something wrong with your system - please check that ORACLE_HOME is set and points to the right directory");
#endif
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	}
	
	/* allocate our server handle {{{ */	
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->server), OCI_HTYPE_SERVER, 0, NULL));
	
	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */

	/* attach to the server {{{ */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIServerAttach, (connection->server, OCI_G(err), (text*)dbname, dbname_len, (ub4) OCI_DEFAULT));
	
	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */
	connection->is_attached = 1;

	/* allocate our session handle {{{ */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->session), OCI_HTYPE_SESSION, 0, NULL));
	
	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */

	/* allocate our private error-handle {{{ */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->err), OCI_HTYPE_ERROR, 0, NULL));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */

	/* allocate our service-context {{{ */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIHandleAlloc, (connection->env, (dvoid **)&(connection->svc), OCI_HTYPE_SVCCTX, 0, NULL));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */

	/* set the username {{{ */
	if (username) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) username, (ub4) username_len, (ub4) OCI_ATTR_USERNAME, OCI_G(err)));
		
		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
			php_oci_connection_close(connection TSRMLS_CC);
			return NULL;
		} 
	}/* }}} */

	/* set the password {{{ */
	if (password) {
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->session, (ub4) OCI_HTYPE_SESSION, (dvoid *) password, (ub4) password_len, (ub4) OCI_ATTR_PASSWORD, OCI_G(err)));
		
		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
			php_oci_connection_close(connection TSRMLS_CC);
			return NULL;
		} 
	}/* }}} */

	/* set the server handle in the service handle {{{ */ 
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, (connection->svc, OCI_HTYPE_SVCCTX, connection->server, 0, OCI_ATTR_SERVER, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */

	/* set the authentication handle in the service handle {{{ */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, (connection->svc, OCI_HTYPE_SVCCTX, connection->session, 0, OCI_ATTR_SESSION, OCI_G(err)));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		php_oci_connection_close(connection TSRMLS_CC);
		return NULL;
	} /* }}} */

	if (new_password) {
		/* try to change password if new one was provided {{{ */
		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIPasswordChange, (connection->svc, OCI_G(err), (text *)username, username_len, (text *)password, password_len, (text *)new_password, new_password_len, OCI_AUTH));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
			php_oci_connection_close(connection TSRMLS_CC);
			return NULL;
		}

		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)connection->svc, OCI_HTYPE_SVCCTX, (dvoid *)&(connection->session), (ub4 *)0, OCI_ATTR_SESSION, OCI_G(err)));

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
			php_oci_connection_close(connection TSRMLS_CC);
			return NULL;
		} /* }}} */
	} else {
		/* start the session {{{ */
		switch (session_mode) {
			case OCI_DEFAULT:
#if HAVE_OCI_STMT_PREPARE2
				/* statement caching is suported only in Oracle 9+ */
				PHP_OCI_CALL_RETURN(OCI_G(errcode), OCISessionBegin, (connection->svc, OCI_G(err), connection->session, (ub4) OCI_CRED_RDBMS, (ub4) OCI_STMT_CACHE));
#else
				/* others cannot use stmt caching, so we call OCISessionBegin() with OCI_DEFAULT */
				PHP_OCI_CALL_RETURN(OCI_G(errcode), OCISessionBegin, (connection->svc, OCI_G(err), connection->session, (ub4) OCI_CRED_RDBMS, (ub4) OCI_DEFAULT));
#endif
				break;
			case OCI_SYSDBA:
			case OCI_SYSOPER:
			default:
				if (username_len == 1 && username[0] == '/' && password_len == 0) {
					PHP_OCI_CALL_RETURN(OCI_G(errcode), OCISessionBegin, (connection->svc, OCI_G(err), connection->session, (ub4) OCI_CRED_EXT, (ub4) session_mode));
				} else {
					PHP_OCI_CALL_RETURN(OCI_G(errcode), OCISessionBegin, (connection->svc, OCI_G(err), connection->session, (ub4) OCI_CRED_RDBMS, (ub4) session_mode));
				}
				break;
		}

		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
			/* OCISessionBegin returns OCI_SUCCESS_WITH_INFO when
			 * user's password has expired, but is still usable.
			 * */
			if (OCI_G(errcode) != OCI_SUCCESS_WITH_INFO) {
				php_oci_connection_close(connection TSRMLS_CC);
				return NULL;
			}
		} /* }}} */
	}

#if HAVE_OCI_STMT_PREPARE2
	{
		ub4 statement_cache_size = (OCI_G(statement_cache_size) > 0) ? OCI_G(statement_cache_size) : 0;

		PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrSet, ((dvoid *) connection->svc, (ub4) OCI_HTYPE_SVCCTX, (ub4 *) &statement_cache_size, 0, (ub4) OCI_ATTR_STMTCACHESIZE, OCI_G(err)));
		
		if (OCI_G(errcode) != OCI_SUCCESS) {
			php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
			php_oci_connection_close(connection TSRMLS_CC);
			return NULL;
		} 	
	}
#endif
	
	/* mark it as open */
	connection->is_open = 1;
	
	/* add to the appropriate hash */
	if (connection->is_persistent) {
		new_le.ptr = connection;
		new_le.type = le_pconnection;
		connection->used_this_request = 1;
		connection->rsrc_id = zend_list_insert(connection, le_pconnection);
		zend_hash_update(&EG(persistent_list), connection->hash_key, strlen(connection->hash_key)+1, (void *)&new_le, sizeof(zend_rsrc_list_entry), NULL);
		OCI_G(num_persistent)++;
	} else if (!exclusive) {
		connection->rsrc_id = zend_list_insert(connection, le_connection);
		new_le.ptr = (void *)connection->rsrc_id;
		new_le.type = le_index_ptr;
		zend_hash_update(&EG(regular_list), connection->hash_key, strlen(connection->hash_key)+1, (void *)&new_le, sizeof(zend_rsrc_list_entry), NULL);
		OCI_G(num_links)++;
	} else {
		connection->rsrc_id = zend_list_insert(connection, le_connection);	
		OCI_G(num_links)++;
	}
	return connection;
}
/* }}} */

/* {{{ php_oci_connection_ping()
 * Ping connection. Uses OCIPing() or OCIServerVersion() depending on the Oracle Client version */
static int php_oci_connection_ping(php_oci_connection *connection TSRMLS_DC)
{
	/* OCIPing() crashes Oracle servers older than 10.2 */
#if 0
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIPing, (connection->svc, OCI_G(err), OCI_DEFAULT));
#else
	char version[256];
	/* use good old OCIServerVersion() by default */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIServerVersion, (connection->server, OCI_G(err), (text*)version, sizeof(version), OCI_HTYPE_SERVER));
#endif
	
	if (OCI_G(errcode) == OCI_SUCCESS) {
		return 1;
	}

	/* ignore errors here, just return failure 
	 * php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC); */
	return 0;
}
/* }}} */

/* {{{ php_oci_connection_status()
 Check connection status (pre-ping check) */
static int php_oci_connection_status(php_oci_connection *connection TSRMLS_DC)
{
	ub4 ss = 0;

	/* get OCI_ATTR_SERVER_STATUS */
	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIAttrGet, ((dvoid *)connection->server, OCI_HTYPE_SERVER, (dvoid *)&ss, (ub4 *)0, OCI_ATTR_SERVER_STATUS, OCI_G(err)));
	
	if (OCI_G(errcode) == OCI_SUCCESS && ss == OCI_SERVER_NORMAL) {
		return 1;
	}

	/* ignore errors here, just return failure 
	 * php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC); */
	return 0;
}
/* }}} */

/* {{{ php_oci_connection_rollback()
 Rollback connection */
int php_oci_connection_rollback(php_oci_connection *connection TSRMLS_DC)
{
	PHP_OCI_CALL_RETURN(connection->errcode, OCITransRollback, (connection->svc, connection->err, (ub4) 0));
	connection->needs_commit = 0;

	if (connection->errcode != OCI_SUCCESS) {
		php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	return 0;
} /* }}} */

/* {{{ php_oci_connection_commit()
 Commit connection */
int php_oci_connection_commit(php_oci_connection *connection TSRMLS_DC)
{
	PHP_OCI_CALL_RETURN(connection->errcode, OCITransCommit, (connection->svc, connection->err, (ub4) 0));
	connection->needs_commit = 0;

	if (connection->errcode != OCI_SUCCESS) {
		php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	return 0;
} /* }}} */

/* {{{ php_oci_connection_close() 
 Close the connection and free all its resources */
static int php_oci_connection_close(php_oci_connection *connection TSRMLS_DC)
{
	int result = 0;
	zend_bool in_call_save = OCI_G(in_call);

	if (connection->descriptors) {
		zend_hash_destroy(connection->descriptors);
		efree(connection->descriptors);
	}

	if (connection->svc) {	
		/* rollback outstanding transactions */
		if (connection->needs_commit) {
			if (php_oci_connection_rollback(connection TSRMLS_CC)) {
				/* rollback failed */
				result = 1;
			}
		}
	}

	if (connection->svc && connection->session && connection->is_open) {
		PHP_OCI_CALL(OCISessionEnd, (connection->svc, OCI_G(err), connection->session, (ub4) 0));
	}
	
	if (connection->session) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->session, OCI_HTYPE_SESSION));
	}
	
	if (connection->is_attached) {
		PHP_OCI_CALL(OCIServerDetach, (connection->server, OCI_G(err), OCI_DEFAULT));
	}
	
	if (connection->svc) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->svc,	(ub4) OCI_HTYPE_SVCCTX));
	}
	
	if (connection->err) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->err, (ub4) OCI_HTYPE_ERROR));
	}
	
	if (connection->server) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->server, (ub4) OCI_HTYPE_SERVER));
	}
	
	if (connection->env) {
		PHP_OCI_CALL(OCIHandleFree, ((dvoid *) connection->env, OCI_HTYPE_ENV));
	}

	if (connection->is_persistent) {
		if (connection->hash_key) {
			free(connection->hash_key);
		}
		free(connection);
	} else {
		if (connection->hash_key) {
			efree(connection->hash_key);
		}
		efree(connection);
	}
	connection = NULL;
	OCI_G(in_call) = in_call_save;
	return result;
} /* }}} */

/* {{{ php_oci_password_change()
 Change password for the user with the username given */
int php_oci_password_change(php_oci_connection *connection, char *user, int user_len, char *pass_old, int pass_old_len, char *pass_new, int pass_new_len TSRMLS_DC)
{
	PHP_OCI_CALL_RETURN(connection->errcode, OCIPasswordChange, (connection->svc, connection->err, (text *)user, user_len, (text *)pass_old, pass_old_len, (text *)pass_new, pass_new_len, OCI_DEFAULT));

	if (connection->errcode != OCI_SUCCESS) {
		php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	connection->passwd_changed = 1;
	return 0;
} /* }}} */

/* {{{ php_oci_server_get_version() 
 Get Oracle server version */
int php_oci_server_get_version(php_oci_connection *connection, char **version TSRMLS_DC) 
{
	char version_buff[256];

	PHP_OCI_CALL_RETURN(connection->errcode, OCIServerVersion, (connection->svc, connection->err, (text*)version_buff, sizeof(version_buff), OCI_HTYPE_SVCCTX));
	
	if (connection->errcode != OCI_SUCCESS) {
		php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	
	*version = estrdup(version_buff);
	return 0;
} /* }}} */

/* {{{ php_oci_column_to_zval() 
 Convert php_oci_out_column struct into zval */
int php_oci_column_to_zval(php_oci_out_column *column, zval *value, int mode TSRMLS_DC)
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
		ZVAL_RESOURCE(value, column->stmtid);
		zend_list_addref(column->stmtid);
	} else if (column->is_descr) {
		
		if (column->data_type != SQLT_RDD) {
			int rsrc_type;
			
			/* reset descriptor's length */
			descriptor = (php_oci_descriptor *) zend_list_find(column->descid, &rsrc_type);

			if (!descriptor || rsrc_type != le_descriptor) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find LOB descriptor #%d", column->descid);
				return 1;
			}
			                                          
			descriptor->lob_size = -1;
			descriptor->lob_current_position = 0;
			descriptor->buffering = 0;
		}

		if (column->data_type != SQLT_RDD && (mode & PHP_OCI_RETURN_LOBS)) {
			/* PHP_OCI_RETURN_LOBS means that we want the content of the LOB back instead of the locator */
			
			lob_fetch_status = php_oci_lob_read(descriptor, -1, 0, &lob_buffer, &lob_length TSRMLS_CC);
#ifdef HAVE_OCI8_TEMP_LOB
			php_oci_temp_lob_close(descriptor TSRMLS_CC);
#endif
			if (lob_fetch_status) {
				ZVAL_FALSE(value);
				return 1;
			} else {
				if (lob_length > 0) {
					ZVAL_STRINGL(value, lob_buffer, lob_length, 0);
				} else {
					ZVAL_EMPTY_STRING(value);
				}
				return 0;
			}
		} else { 
			/* return the locator */
			object_init_ex(value, oci_lob_class_entry_ptr);
			add_property_resource(value, "descriptor", column->descid);
			zend_list_addref(column->descid);
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
		
		ZVAL_STRINGL(value, column->data, column_size, 1);
	}
	return 0;
}
/* }}} */

/* {{{ php_oci_fetch_row() 
 Fetch the next row from the given statement */
void php_oci_fetch_row (INTERNAL_FUNCTION_PARAMETERS, int mode, int expected_args)
{
	zval *z_statement, *array;
	php_oci_statement *statement;
	php_oci_out_column *column;
	ub4 nrows = 1;
	int i;
	long fetch_mode = 0;

	if (expected_args > 2) {
		/* only for ocifetchinto BC */

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|l", &z_statement, &array, &fetch_mode) == FAILURE) {
			return;
		}
		
		if (ZEND_NUM_ARGS() == 2) {
			fetch_mode = mode;
		}
	} else if (expected_args == 2) {
		/* only for oci_fetch_array() */

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &z_statement, &fetch_mode) == FAILURE) {
			return;
		}
		
		if (ZEND_NUM_ARGS() == 1) {
			fetch_mode = mode;
		}
	} else {
		/* for all oci_fetch_*() */
		
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_statement) == FAILURE) {
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

	PHP_OCI_ZVAL_TO_STATEMENT(z_statement, statement);

	if (php_oci_statement_fetch(statement, nrows TSRMLS_CC)) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < statement->ncolumns; i++) {
		
		column = php_oci_statement_get_column(statement, i + 1, NULL, 0 TSRMLS_CC);
		
		if (column == NULL) {
			continue;
		}
		if ((column->indicator == -1) && ((fetch_mode & PHP_OCI_RETURN_NULLS) == 0)) {
			continue;
		}

		if (!(column->indicator == -1)) {
			zval *element;
			
			MAKE_STD_ZVAL(element);
			php_oci_column_to_zval(column, element, fetch_mode TSRMLS_CC);

			if (fetch_mode & PHP_OCI_NUM || !(fetch_mode & PHP_OCI_ASSOC)) {
				add_index_zval(return_value, i, element);
			}
			if (fetch_mode & PHP_OCI_ASSOC) {
				if (fetch_mode & PHP_OCI_NUM) {
					ZVAL_ADDREF(element);
				}
				add_assoc_zval(return_value, column->name, element);
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
		/* only for ocifetchinto BC
		 * in all other cases we return array, not long
		 */
		REPLACE_ZVAL_VALUE(&array, return_value, 1); /* copy return_value to given reference */
		zval_dtor(return_value);
		RETURN_LONG(statement->ncolumns);
	}
}
/* }}} */

/* {{{ php_oci_persistent_helper() 
 Helper function to close/rollback persistent connections at the end of request */
static int php_oci_persistent_helper(zend_rsrc_list_entry *le TSRMLS_DC)
{
	time_t timestamp;
	php_oci_connection *connection;

	timestamp = time(NULL);
	
	if (le->type == le_pconnection) {
		connection = (php_oci_connection *)le->ptr;

		if (connection->used_this_request) {
			if ((PG(connection_status) & PHP_CONNECTION_TIMEOUT) || OCI_G(in_call)) {
				return ZEND_HASH_APPLY_REMOVE;
			}

			if (connection->descriptors) {
				zend_hash_destroy(connection->descriptors);
				efree(connection->descriptors);
				connection->descriptors = NULL;
			}
			
			if (connection->needs_commit) {
				php_oci_connection_rollback(connection TSRMLS_CC);
			}
			
			/* If oci_password_change() changed the password of a
			 * persistent connection, close the connection and remove
			 * it from the persistent connection cache.  This means
			 * subsequent scripts will be prevented from being able to
			 * present the old (now invalid) password to a usable
			 * connection to the database; they must use the new
			 * password.
			 */
			if (connection->passwd_changed) {
				return ZEND_HASH_APPLY_REMOVE;
			}

			if (OCI_G(persistent_timeout) > 0) {
				connection->idle_expiry = timestamp + OCI_G(persistent_timeout);
			}
		
			if (OCI_G(ping_interval) >= 0) {
				connection->next_ping = timestamp + OCI_G(ping_interval);
			} else {
				/* ping_interval is -1 */
				connection->next_ping = 0;
			}

			connection->used_this_request = 0;

		} else if (OCI_G(persistent_timeout) != -1) {
			if (connection->idle_expiry < timestamp) {
				/* connection has timed out */
				return ZEND_HASH_APPLY_REMOVE;
			}
		}
	}
	return ZEND_HASH_APPLY_KEEP;
} /* }}} */

#ifdef ZTS
/* {{{ php_oci_list_helper() 
 Helper function to destroy data on thread shutdown in ZTS mode */
static int php_oci_list_helper(zend_rsrc_list_entry *le, void *le_type TSRMLS_DC)
{
	int type = (int) le_type;

	if (le->type == type) {
		if (le->ptr != NULL && --le->refcount<=0) {
			return ZEND_HASH_APPLY_REMOVE;
		}
	}
	return ZEND_HASH_APPLY_KEEP;
} /* }}} */
#endif

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
