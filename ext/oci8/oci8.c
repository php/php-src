/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* TODO list:
 *
 * - php.ini flags 
 *   especialliy important for things like oci_ping
 *		allowpconns
 *		timeout
 *		maxlifetime
 *		maxpconns
 * - Change return-value for OCIFetch*() (1-row read, 0-Normal end, false-error) 
 * - Error mode (print or shut up?)
 * - binding of arrays
 * - Character sets for NCLOBS
 * - split the module into an upper (php-callable) and lower (c-callable) layer!
 * - remove all XXXs
 * - clean up and documentation
 * - make OCIInternalDebug accept a mask of flags....
 * - have one ocifree() call.
 * - make it possible to have persistent statements?
 * - failover
 * - change all the lob stuff to work without classes (optional)! 
 * - make sure that the callbacks terminate the strings with \0
 * - cleanup the ociexecute semantics for refcursors
 * - make $lob->savefile use O_BINARY
 * - line 2728: ub4 length = -1; needs fixing
 * - delay OCIInitialize() as far as we can.
 * - add PHP Array <-> OCICollection conversion
 * - add Collection iterator object for INDEX BY tables
 * - make auto-rollback only happen if we have an outstanding transaction
 * - implement ocidisconnect
 * - add OCI9-specific functions and separate them from OCI8 with ifdefs
 */

/* {{{ includes & stuff */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#if HAVE_OCI8

#include "php_oci8.h"

/* True globals, only used by thread safe functions */
static TsHashTable *persistent_servers;
static TsHashTable *persistent_sessions;

static long num_persistent = 0;
static long num_links = 0;

/* True globals, no need for thread safety */
static int le_conn;
static int le_stmt;
static int le_desc;
#ifdef PHP_OCI8_HAVE_COLLECTIONS 
static int le_coll;
#endif
static int le_server;
static int le_session;
static zend_class_entry *oci_lob_class_entry_ptr;
#ifdef PHP_OCI8_HAVE_COLLECTIONS
static zend_class_entry *oci_coll_class_entry_ptr;
#endif

#ifndef SQLT_BFILEE
#define SQLT_BFILEE 114
#endif
#ifndef SQLT_CFILEE
#define SQLT_CFILEE 115
#endif

#define SAFE_STRING(s) ((s)?(s):"")

#ifdef ZTS
MUTEX_T mx_lock;

#define mutex_alloc(mutex) mutex = tsrm_mutex_alloc()
#define mutex_free(mutex) tsrm_mutex_free(mutex)
#define mutex_lock(mutex) tsrm_mutex_lock(mutex)
#define mutex_unlock(mutex) tsrm_mutex_unlock(mutex)
#define thread_id()	tsrm_thread_id()
#else
#define mutex_alloc(mutex)
#define mutex_free(mutex)
#define mutex_lock(mutex)
#define mutex_unlock(mutex)
#define thread_id() 1
#endif

/* dirty marcos to make sure we _never_ call oracle-functions recursivly 
 *
 * i'm well aware that we should _never_ call exit directly - this core is for
 * pure testing and commented out - as you can see;-)
 * thies@thieso.net 20010723
 */

#define CALL_OCI(call) \
{ \
	if (OCI(in_call)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI8 Recursive call!\n"); \
		exit(-1); \
	} else { \
		OCI(in_call)=1; \
		call; \
		OCI(in_call)=0; \
	} \
}

#define CALL_OCI_RETURN(retcode,call) \
{ \
	if (OCI(in_call)) { \
		retcode=-1; \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCI8 Recursive call!\n"); \
		exit(-1); \
	} else { \
		OCI(in_call)=1; \
		retcode=call; \
		OCI(in_call)=0; \
	} \
}

#include <fcntl.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* }}} */
/* {{{ thread safety stuff */

#ifdef ZTS
int oci_globals_id;
#else
PHP_OCI_API php_oci_globals oci_globals;
#endif

/* }}} */
/* {{{ dynamically loadable module stuff */

#ifdef COMPILE_DL_OCI8
ZEND_GET_MODULE(oci8)
# ifdef PHP_WIN32
# include "zend_arg_defs.c"
# endif
#endif /* COMPILE_DL */

/* }}} */
/* {{{ startup/shutdown/info/internal function prototypes */

PHP_MINIT_FUNCTION(oci);
PHP_RINIT_FUNCTION(oci);
PHP_MSHUTDOWN_FUNCTION(oci);
PHP_RSHUTDOWN_FUNCTION(oci);
PHP_MINFO_FUNCTION(oci);

static ub4 oci_handle_error(oci_connection *connection, ub4 errcode);
static ub4 oci_error(OCIError *err_p, char *what, sword status);
static int oci_ping(oci_server *server);
static void oci_debug(const char *format, ...);

static void _oci_conn_list_dtor(oci_connection *connection TSRMLS_DC);
static void _oci_stmt_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _oci_descriptor_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
#ifdef PHP_OCI8_HAVE_COLLECTIONS
static void _oci_coll_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
#endif
static void _oci_server_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _oci_session_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void php_oci_free_conn_list(zend_rsrc_list_entry *rsrc TSRMLS_DC);

static void _oci_column_hash_dtor(void *data);
static void _oci_define_hash_dtor(void *data);
static void _oci_bind_hash_dtor(void *data);
static void _oci_desc_flush_hash_dtor(void *data);

static oci_connection *oci_get_conn(zval ** TSRMLS_DC);
static oci_statement *oci_get_stmt(zval ** TSRMLS_DC);
static oci_descriptor *oci_get_desc(int TSRMLS_DC);
#ifdef PHP_OCI8_HAVE_COLLECTIONS
/* Questionable name.  Very close to oci_get_col */
static oci_collection *oci_get_coll(int TSRMLS_DC);
#endif
static oci_out_column *oci_get_col(oci_statement *, int, zval **);

static int _oci_make_zval(zval *, oci_statement *, oci_out_column *, char *, int mode TSRMLS_DC);
static oci_statement *oci_parse(oci_connection *, char *, int);
static int oci_execute(oci_statement *, char *, ub4 mode);
static int oci_fetch(oci_statement *, ub4, char * TSRMLS_DC);
static int oci_lobgetlen(oci_connection *, oci_descriptor *, ub4 *length);
static int oci_loadlob(oci_connection *, oci_descriptor *, char **, ub4 *length);
static int oci_readlob(oci_connection *, oci_descriptor *, char **, ub4 *len);
static int oci_setprefetch(oci_statement *statement, int size);

static void oci_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive);

static oci_server *_oci_open_server(char *dbname,int persistent);
static void _oci_close_server(oci_server *server);

static oci_session *_oci_open_session(oci_server* server,char *username,char *password,int persistent,int exclusive, char *charset);
static void _oci_close_session(oci_session *session);

static sb4 oci_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
static sb4 oci_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

#if 0
static sb4 oci_failover_callback(dvoid *svchp,dvoid* envhp,dvoid *fo_ctx,ub4 fo_type, ub4 fo_event);
#endif

static int oci_lob_flush(oci_descriptor *, int flush_flag TSRMLS_DC);

/* }}} */

/* {{{ extension macros 
*/
#define OCI_GET_STMT(statement,value) \
	statement = oci_get_stmt(value TSRMLS_CC); \
	if (statement == NULL) { \
		RETURN_FALSE; \
	}

#define OCI_GET_CONN(connection,value) \
	connection = oci_get_conn(value TSRMLS_CC); \
	if (connection == NULL) { \
		RETURN_FALSE; \
	}

#define OCI_GET_DESC(descriptor,index) \
	descriptor = oci_get_desc(index TSRMLS_CC); \
	if (descriptor == NULL) { \
		RETURN_FALSE; \
	}

#ifdef PHP_OCI8_HAVE_COLLECTIONS
#define OCI_GET_COLL(collection,index) \
	collection = oci_get_coll(index TSRMLS_CC); \
	if (collection == NULL) { \
		RETURN_FALSE; \
	}
#endif

#define IS_LOB_INTERNAL(lob) \
	if (lob->type != OCI_DTYPE_LOB) { \
		switch (lob->type) { \
			case OCI_DTYPE_FILE: \
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "internal LOB was expected, FILE locator is given"); \
				break; \
			case OCI_DTYPE_ROWID: \
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "internal LOB was expected, ROWID locator is given"); \
				break; \
			default: \
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "internal LOB was expected, locator of unknown type is given"); \
				break; \
		} \
		RETURN_FALSE; \
	}
/* }}} */

/* {{{ extension function prototypes
*/
PHP_FUNCTION(oci_bind_by_name);
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
#define OCI_ASSOC               1<<0
#define OCI_NUM                 1<<1
#define OCI_BOTH                (OCI_ASSOC|OCI_NUM)

#define OCI_RETURN_NULLS        1<<2
#define OCI_RETURN_LOBS         1<<3

#define OCI_FETCHSTATEMENT_BY_COLUMN    1<<4
#define OCI_FETCHSTATEMENT_BY_ROW       1<<5
#define OCI_FETCHSTATEMENT_BY           (OCI_FETCHSTATEMENT_BY_COLUMN | OCI_FETCHSTATEMENT_BY_ROW)

static zend_function_entry php_oci_functions[] = {
	PHP_FE(oci_define_by_name,          third_arg_force_ref)
	PHP_FE(oci_bind_by_name,            third_arg_force_ref)
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
	PHP_FE(ocifetchinto,                second_arg_force_ref)
	PHP_FE(oci_fetch_all,               second_arg_force_ref)
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
	PHP_FALIAS(ocibindbyname,       oci_bind_by_name,       third_arg_force_ref)
	PHP_FALIAS(ocidefinebyname,     oci_define_by_name,     third_arg_force_ref)
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
	PHP_FALIAS(ocifetchstatement,   oci_fetch_all,          second_arg_force_ref)
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
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ startup, shutdown and info functions
*/
static void php_oci_init_globals(php_oci_globals *oci_globals_p TSRMLS_DC)
{ 
	OCI(shutdown)	= 0;
	OCI(in_call)	= 0;

	CALL_OCI(
		OCIEnvInit(
			&OCI(pEnv), 
			OCI_DEFAULT, 
			0, 
			NULL
		)
	);
	
	CALL_OCI(
		OCIHandleAlloc(
			OCI(pEnv), 
			(dvoid **)&OCI(pError), 
			OCI_HTYPE_ERROR, 
			0, 
			NULL
		)
	);
}

static int _sessions_pcleanup(zend_llist *session_list TSRMLS_DC)
{
	zend_llist_destroy(session_list);

	return 1;
}

static int _session_pcleanup(oci_session *session TSRMLS_DC)
{
	_oci_close_session(session);

	return 1;
}

static int _server_pcleanup(oci_server *server TSRMLS_DC)
{
	_oci_close_server(server);

	return 1;
}

PHP_MINIT_FUNCTION(oci)
{
	zend_class_entry oci_lob_class_entry;
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	zend_class_entry oci_coll_class_entry;
#endif

#ifdef HAVE_OCI8_SHARED_MODE

#ifdef PHP_OCI8_HAVE_COLLECTIONS
#define PHP_OCI_INIT_MODE_TMP OCI_SHARED | OCI_OBJECT
#else
#define PHP_OCI_INIT_MODE_TMP OCI_SHARED
#endif

#else

#ifdef PHP_OCI8_HAVE_COLLECTIONS
#define PHP_OCI_INIT_MODE_TMP OCI_DEFAULT | OCI_OBJECT
#else
#define PHP_OCI_INIT_MODE_TMP OCI_DEFAULT
#endif

#endif

#ifdef ZTS
#define PHP_OCI_INIT_MODE PHP_OCI_INIT_MODE_TMP | OCI_THREADED
#else
#define PHP_OCI_INIT_MODE PHP_OCI_INIT_MODE_TMP
#endif

	mutex_alloc(mx_lock);

	persistent_servers = malloc(sizeof(TsHashTable));
	persistent_sessions = malloc(sizeof(TsHashTable));
	zend_ts_hash_init(persistent_servers, 13, NULL, (dtor_func_t) _server_pcleanup, 1); 
	zend_ts_hash_init(persistent_sessions, 13, NULL, (dtor_func_t) _sessions_pcleanup, 1); 

	OCIInitialize(PHP_OCI_INIT_MODE, NULL, NULL, NULL, NULL);

#ifdef ZTS
	ts_allocate_id(&oci_globals_id, sizeof(php_oci_globals), (ts_allocate_ctor) php_oci_init_globals, NULL);
#else
	php_oci_init_globals(&oci_globals TSRMLS_CC);
#endif

	le_stmt = zend_register_list_destructors_ex(_oci_stmt_list_dtor, NULL, "oci8 statement", module_number);
	le_conn = zend_register_list_destructors_ex(php_oci_free_conn_list, NULL, "oci8 connection", module_number);
	le_desc = zend_register_list_destructors_ex(_oci_descriptor_list_dtor, NULL, "oci8 descriptor", module_number);
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	le_coll = zend_register_list_destructors_ex(_oci_coll_list_dtor, NULL, "oci8 collection", module_number);
#endif
	le_server = zend_register_list_destructors_ex(_oci_server_list_dtor, NULL, "oci8 server", module_number);
	le_session = zend_register_list_destructors_ex(_oci_session_list_dtor, NULL, "oci8 session", module_number);

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
	REGISTER_LONG_CONSTANT("OCI_DESCRIBE_ONLY",OCI_DESCRIBE_ONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_COMMIT_ON_SUCCESS",OCI_COMMIT_ON_SUCCESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_EXACT_FETCH",OCI_EXACT_FETCH, CONST_CS | CONST_PERSISTENT);

/* for $LOB->seek() */
	REGISTER_LONG_CONSTANT("OCI_SEEK_SET",OCI_SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_CUR",OCI_SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_END",OCI_SEEK_END, CONST_CS | CONST_PERSISTENT);

/*	for $LOB->flush() */
	REGISTER_LONG_CONSTANT("OCI_LOB_BUFFER_FREE",OCI_LOB_BUFFER_FREE, CONST_CS | CONST_PERSISTENT);
	
/* for OCIBindByName (real "oci" names + short "php" names*/
	REGISTER_LONG_CONSTANT("SQLT_BFILEE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_RDD",SQLT_RDD, CONST_CS | CONST_PERSISTENT);

#ifdef PHP_OCI8_HAVE_COLLECTIONS
	REGISTER_LONG_CONSTANT("OCI_B_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);	
	REGISTER_STRING_CONSTANT("OCI_SYSDATE","SYSDATE",CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("OCI_B_BFILE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_ROWID",SQLT_RDD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CURSOR",SQLT_RSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BIN",SQLT_BIN, CONST_CS | CONST_PERSISTENT);

/* for OCIFetchStatement */
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_COLUMN", OCI_FETCHSTATEMENT_BY_COLUMN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_ROW", OCI_FETCHSTATEMENT_BY_ROW, CONST_CS | CONST_PERSISTENT);

/* for OCIFetchInto &  OCIResult */
	REGISTER_LONG_CONSTANT("OCI_ASSOC",OCI_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_NUM",OCI_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_BOTH",OCI_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_NULLS",OCI_RETURN_NULLS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_LOBS",OCI_RETURN_LOBS, CONST_CS | CONST_PERSISTENT);

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
	OCI(debug_mode) = 0; /* start "fresh" */
/*	OCI(in_call) = 0; i don't think we want this! */

	oci_debug("php_rinit_oci");

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(oci)
{
	OCI(shutdown) = 1;

	oci_debug("START php_mshutdown_oci");

	zend_ts_hash_destroy(persistent_sessions);
	zend_ts_hash_destroy(persistent_servers);

	free(persistent_sessions);
	free(persistent_servers);

	mutex_free(mx_lock);

	CALL_OCI(
		OCIHandleFree(
				(dvoid *) OCI(pEnv), 
				OCI_HTYPE_ENV
		)
	);

	oci_debug("END   php_mshutdown_oci");

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(oci)
{
	oci_debug("START php_rshutdown_oci");

#if 0
	/* XXX free all statements, rollback all outstanding transactions */

	zend_ts_hash_apply(persistent_sessions, (apply_func_t) _session_cleanup TSRMLS_CC);
	zend_ts_hash_apply(persistent_servers, (apply_func_t) _server_cleanup TSRMLS_CC);
#endif

	oci_debug("END   php_rshutdown_oci");

	return SUCCESS;
}

PHP_MINFO_FUNCTION(oci)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_row(2, "OCI8 Support", "enabled");
	php_info_print_table_row(2, "Revision", "$Revision$");

	sprintf(buf, "%ld", num_persistent);
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", num_links);
	php_info_print_table_row(2, "Active Links", buf);

#ifndef PHP_WIN32
	php_info_print_table_row(2, "Oracle Version", PHP_OCI8_VERSION );
	php_info_print_table_row(2, "Compile-time ORACLE_HOME", PHP_OCI8_DIR );
	php_info_print_table_row(2, "Libraries Used", PHP_OCI8_SHARED_LIBADD );
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
}
/* }}} */

/* {{{ _oci_define_hash_dtor()
*/
static void _oci_define_hash_dtor(void *data)
{
	oci_define *define = (oci_define *) data;

	oci_debug("_oci_define_hash_dtor: %s",define->name);

	zval_ptr_dtor(&define->zval);

	if (define->name) {
		efree(define->name);
		define->name = 0;
	}
}
/* }}} */

/* {{{ _oci_desc_flush_hash_dtor()
 */
static void _oci_desc_flush_hash_dtor(void *data)
{
	oci_descriptor *descr = *(oci_descriptor **)data;
	TSRMLS_FETCH();
	
	if (descr->buffering == 2 && (descr->type == OCI_DTYPE_LOB || descr->type == OCI_DTYPE_FILE)) {
		oci_lob_flush(descr,OCI_LOB_BUFFER_FREE TSRMLS_CC);
		descr->buffering = 1;
	}
}
/* }}} */

/* {{{ _oci_bind_hash_dtor()
*/
static void _oci_bind_hash_dtor(void *data)
{
	oci_bind *bind = (oci_bind *) data;

	oci_debug("_oci_bind_hash_dtor:");

	zval_ptr_dtor(&(bind->zval));
}
/* }}} */

/* {{{ _oci_bind_pre_exec()
*/
static int _oci_bind_pre_exec(void *data TSRMLS_DC)
{
	oci_bind *bind = (oci_bind *) data;

	/* reset all bind stuff to a normal state..-. */

	bind->indicator = 0; 

	return 0;
}
/* }}} */

/* {{{ _oci_bind_post_exec()
*/
static int _oci_bind_post_exec(void *data TSRMLS_DC)
{
	oci_bind *bind = (oci_bind *) data;

	if (bind->indicator == -1) { /* NULL */
		zval *val = bind->zval;
		if (Z_TYPE_P(val) == IS_STRING && (Z_STRVAL_P(val) != empty_string)) {
			*Z_STRVAL_P(val) = '\0'; /* XXX avoid warning in debug mode */
		}
		zval_dtor(val);
		ZVAL_NULL(val);
	} else if (Z_TYPE_P(bind->zval) == IS_STRING && (Z_STRVAL_P(bind->zval) != empty_string)) {
		Z_STRVAL_P(bind->zval) = erealloc(Z_STRVAL_P(bind->zval), Z_STRLEN_P(bind->zval)+1);
		Z_STRVAL_P(bind->zval)[ Z_STRLEN_P(bind->zval) ] = '\0';
	}


	return 0;
}
/* }}} */

/* {{{ _oci_column_hash_dtor()
*/
static void _oci_column_hash_dtor(void *data)
{	
	oci_out_column *column = (oci_out_column *) data;
	TSRMLS_FETCH();

	oci_debug("START _oci_column_hash_dtor: %s",column->name);

	if (column->stmtid) {
		zend_list_delete(column->stmtid);
	}

	if (column->is_descr) {
		zend_list_delete(column->descid);
	} else {
		if (column->data) {
			efree(column->data);
		}
	}

	oci_debug("END   _oci_column_hash_dtor: %s",column->name);

	if (column->name) {
		efree(column->name);
	}
}
/* }}} */

/* {{{ _oci_stmt_list_dtor()
*/
static void _oci_stmt_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_statement *statement = (oci_statement *)rsrc->ptr;
	oci_debug("START _oci_stmt_list_dtor: id=%d last_query=\"%s\"",statement->id,SAFE_STRING(statement->last_query));

 	if (statement->pStmt) {
		CALL_OCI(
			OCIHandleFree(
				statement->pStmt, 
				OCI_HTYPE_STMT
			)
		);
		
		statement->pStmt = 0;
	}

	if (statement->pError) {
		CALL_OCI(
			OCIHandleFree(
				statement->pError, 
				OCI_HTYPE_ERROR
			)
		);
		
		statement->pError = 0;
	}

	if (statement->last_query) {
		efree(statement->last_query);
	}

	if (statement->columns) {
		zend_hash_destroy(statement->columns);
		efree(statement->columns);
	}

	if (statement->binds) {
		zend_hash_destroy(statement->binds);
		efree(statement->binds);
	}

	if (statement->defines) {
		zend_hash_destroy(statement->defines);
		efree(statement->defines);
	}

	oci_debug("END   _oci_stmt_list_dtor: id=%d",statement->id);

	efree(statement);
}
/* }}} */

/* {{{ _oci_conn_list_dtor()
*/
static void _oci_conn_list_dtor(oci_connection *connection TSRMLS_DC)
{
	/* 
	   as the connection is "only" a in memory service context we do not disconnect from oracle.
	*/

	oci_debug("START _oci_conn_list_dtor: id=%d",connection->id);

	if (connection->pServiceContext) {

		if (connection->needs_commit) {
			oci_debug("OCITransRollback");
			CALL_OCI_RETURN(connection->error,
				OCITransRollback(
					connection->pServiceContext,
					connection->pError,
					(ub4)0
				)
			);
	 
			if (connection->error) {
				oci_error(connection->pError, "failed to rollback outstanding transactions!", connection->error);
			}
			connection->needs_commit = 0;
		} else {
			oci_debug("nothing to do..");
		}

		CALL_OCI(
			OCIHandleFree(
				(dvoid *) connection->pServiceContext, 
				(ub4) OCI_HTYPE_SVCCTX
			)
		);
	}

	if (connection->session) {
		/* close associated session when destructed */
		zend_list_delete(connection->session->num);
	}

	if (connection->descriptors) {
		zend_hash_destroy(connection->descriptors);
		efree(connection->descriptors);
	}

	if (connection->pError) {
		CALL_OCI(
			OCIHandleFree(
				(dvoid *) connection->pError, 
				(ub4) OCI_HTYPE_ERROR
			)
		);
	}

	oci_debug("END   _oci_conn_list_dtor: id=%d",connection->id);

	efree(connection);
}
/* }}} */

/* {{{ php_oci_free_conn_list
 */
static void php_oci_free_conn_list(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_connection *conn = (oci_connection *)rsrc->ptr;
	_oci_conn_list_dtor(conn TSRMLS_CC);
}
/* }}} */

#ifdef PHP_OCI8_HAVE_COLLECTIONS

/* {{{ _oci_coll_list_dtor()
 */
static void _oci_coll_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_collection *coll = (oci_collection *)rsrc->ptr;
	oci_debug("START _oci_coll_list_dtor: %d",coll->id);

	/* Note sure if we need to free the object.  Have an
	   oracle TAR out on this one.
	   OCIDescriptorFree(descr->ocidescr, descr->type); */

	oci_debug("END   _oci_coll_list_dtor: %d",coll->id);

	efree(coll);
}
/* }}} */
#endif

/* {{{ _oci_descriptor_list_dtor()
 */
static void _oci_descriptor_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_descriptor *descr = (oci_descriptor *)rsrc->ptr;
	oci_debug("START _oci_descriptor_list_dtor: %d",descr->id);

	/* flushing Lobs & Files with buffering enabled */
	if ((descr->type == OCI_DTYPE_FILE || descr->type == OCI_DTYPE_LOB) && descr->buffering == 2) {
		oci_debug("descriptor #%d needs to be flushed. flushing..",descr->id);
		oci_lob_flush(descr,OCI_LOB_BUFFER_FREE TSRMLS_CC);
	}

	CALL_OCI(
		OCIDescriptorFree(
			descr->ocidescr, 
			descr->type
		)
	);

	oci_debug("END   _oci_descriptor_list_dtor: %d",descr->id);

	efree(descr);
}
/* }}} */

/* {{{ _oci_server_list_dtor()
 */
static void _oci_server_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
#if 0
	oci_server *server = (oci_server *)rsrc->ptr;
	if (server->persistent)
		return;

	_oci_close_server(server);
#endif
}
/* }}} */

/* {{{ _oci_session_list_dtor()
 */
static void _oci_session_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_session *session = (oci_session *)rsrc->ptr;
	if (session->persistent) {
		/* clear thread assignment */
		session->thread = 0;
		return;
	}

	_oci_close_session(session);
}
/* }}} */

/* {{{ oci_handle_error
 */
static ub4 oci_handle_error(oci_connection *connection, ub4 errcode)
{
	switch (errcode) {
		case 1013: /* user requested cancel of current operation */
			zend_bailout();
			break;

		case 22:   /* ORA-00022 Invalid session id */
		case 1012: /* ORA-01012: */
		case 3113: /* ORA-03113: end-of-file on communication channel */
		case 604:
		case 1041:
			connection->is_open = 0;
			connection->session->is_open = 0;
			connection->session->server->is_open = 0;
			return 1; /* fatal error */
	}

	return 0; /* no fatal error */
}
/* }}} */

/* {{{ oci_error()
*/
static ub4 oci_error(OCIError *err_p, char *what, sword status)
{
	text errbuf[512];
	sb4 errcode = 0;
	TSRMLS_FETCH();

	switch (status) {
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
			CALL_OCI(
				OCIErrorGet(
					err_p, 
					(ub4)1, 
					NULL, 
					&errcode, 
					errbuf,
					(ub4)sizeof(errbuf), 
					(ub4)OCI_HTYPE_ERROR
				)
			);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: OCI_SUCCESS_WITH_INFO: %s", what, errbuf);
			break;
		case OCI_NEED_DATA:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: OCI_NEED_DATA", what);
			break;
		case OCI_NO_DATA:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: OCI_NO_DATA", what);
			break;
		case OCI_ERROR: {
			CALL_OCI(
				OCIErrorGet(
					err_p, 
					(ub4)1, 
					NULL, 
					&errcode, 
					errbuf,
					(ub4)sizeof(errbuf), 
					(ub4)OCI_HTYPE_ERROR
				)
			);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: %s", what, errbuf);
			break;
		}
		case OCI_INVALID_HANDLE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: OCI_INVALID_HANDLE", what);
			break;
		case OCI_STILL_EXECUTING:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: OCI_STILL_EXECUTING", what);
			break;
		case OCI_CONTINUE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: OCI_CONTINUE", what);
			break;
		default:
			break;
	}
	return errcode;
}
/* }}} */

/* {{{ oci_ping()
*/
static int oci_ping(oci_server *server)
{
	char version[256];
	TSRMLS_FETCH();
	
	CALL_OCI_RETURN(OCI(error),
		OCIServerVersion(
			server->pServer,
		 	OCI(pError), 
			(text*)version, 
			sizeof(version),
			OCI_HTYPE_SERVER
		)
	);

	if (OCI(error) == OCI_SUCCESS) {
		return 1;
	}

	oci_error(OCI(pError), "oci_ping", OCI(error));

	return 0;
}
/* }}} */

/************************* INTERNAL FUNCTIONS *************************/

/* {{{ oci_debug()
*/
static void oci_debug(const char *format, ...)
{
	TSRMLS_FETCH();

	if (OCI(debug_mode)) {
		char buffer[1024];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer)-1, format, args);
		va_end(args);
		buffer[sizeof(buffer)-1] = '\0';
		php_printf("OCIDebug: %s<br />\n", buffer);
	}
}
/* }}} */

/* {{{ oci_get_conn()
*/
static oci_connection *oci_get_conn(zval **conn TSRMLS_DC)
{
	oci_connection *connection;

	connection = (oci_connection *) zend_fetch_resource(conn TSRMLS_CC, -1, "OCI8-Connection", NULL, 1, le_conn);

	if (connection && connection->is_open) {
		return connection;
	} else {
		return (oci_connection *) NULL;
	}
}
/* }}} */

/* {{{ oci_get_stmt()
*/
static oci_statement *oci_get_stmt(zval **stmt TSRMLS_DC)
{
	oci_statement *statement;

	statement = (oci_statement *) zend_fetch_resource(stmt TSRMLS_CC, -1, "OCI8-Statement", NULL, 1, le_stmt);

	if (statement && statement->conn->is_open) {
		return statement;
	} else {
		return (oci_statement *) NULL;
	}
}
/* }}} */

/* {{{ oci_get_desc()
*/
static oci_descriptor *oci_get_desc(int ind TSRMLS_DC)
{
	oci_descriptor *descriptor;
	int actual_resource_type;

	descriptor = (oci_descriptor *) zend_list_find(ind, &actual_resource_type);

	if (descriptor && (actual_resource_type == le_desc)) {
		return descriptor;
	} else {
		return (oci_descriptor *) NULL;
	}
}
/* }}} */

/* {{{ oci_get_col()
*/
static oci_out_column *oci_get_col(oci_statement *statement, int col, zval **value)
{
	oci_out_column *outcol = NULL;
	int i;
	TSRMLS_FETCH();

	if (statement->columns == 0) { /* we release the columns at the end of a fetch */
		return NULL;
	}

	if (value) {
		if (Z_TYPE_PP(value) == IS_STRING) {
			for (i = 0; i < statement->ncolumns; i++) {
				outcol = oci_get_col(statement, i + 1, 0);
				if (outcol == NULL) {
					continue;
				} else if (((int) outcol->name_len == Z_STRLEN_PP(value)) && 
							(!strncmp(outcol->name, Z_STRVAL_PP(value), Z_STRLEN_PP(value)))
				) {
					return outcol;
				}
			}
		} else {
			convert_to_long_ex(value);
			return oci_get_col(statement,Z_LVAL_PP(value),0);
		}
	} else if (col != -1) {
		if (zend_hash_index_find(statement->columns, col, (void **)&outcol) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid column %d", col);
			return NULL;
		}
		return outcol;
	}

	return NULL;
}
/* }}} */

/* {{{ oci_new_desc()
*/
static oci_descriptor *oci_new_desc(int type, oci_connection *connection)
{
	oci_descriptor *descr;
	TSRMLS_FETCH();

	descr = emalloc(sizeof(oci_descriptor));
	
	descr->type = type;

	switch (descr->type) {
		case OCI_DTYPE_FILE:
		case OCI_DTYPE_LOB:
		case OCI_DTYPE_ROWID:
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown descriptor type %d.", descr->type);
			efree(descr);
			return 0;
	}
	
	CALL_OCI_RETURN(OCI(error),
		OCIDescriptorAlloc(
			connection->session->pEnv,
			(dvoid*)&(descr->ocidescr), 
			descr->type, 
			(size_t) 0, 
			(dvoid **) 0
		)
	);
	
	if (OCI(error)) {
		ub4 error;
		error = oci_error(OCI(pError),"OCIDescriptorAlloc %d",OCI(error));
		oci_handle_error(connection, error);
		efree(descr);
		return 0;
	}

	descr->id = zend_list_insert(descr,le_desc);
	descr->conn = connection;
	descr->lob_current_position = 0;
	descr->lob_size = -1;				/* we should set it to -1 to know, that it's just not initialized */
	descr->buffering = 0;				/* buffering is off by default */
	zend_list_addref(connection->id);
   
	if (descr->type == OCI_DTYPE_LOB || descr->type == OCI_DTYPE_FILE) {
		/* add Lobs & Files to hash. we'll flush them ate the end */
		if (!connection->descriptors) {
			ALLOC_HASHTABLE(connection->descriptors);
			zend_hash_init(connection->descriptors, 13, NULL, _oci_desc_flush_hash_dtor, 0);
		}

		zend_hash_next_index_insert(connection->descriptors,&descr,sizeof(oci_descriptor *),NULL);
	}
	oci_debug("oci_new_desc %d",descr->id);

	return descr;
}
/* }}} */

#ifdef PHP_OCI8_HAVE_COLLECTIONS

/* {{{ _oci_get_ocicoll()
*/
static int _oci_get_ocicoll(zval *id,oci_collection **collection TSRMLS_DC)
{
	zval **coll;
	
	if (zend_hash_find(Z_OBJPROP_P(id), "collection", sizeof("collection"), (void **)&coll) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot find collection");
		return 0;
	}
	if ((*collection = oci_get_coll(Z_LVAL_PP(coll) TSRMLS_CC)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "collection not found");
		return 0;
	}
 
	return Z_LVAL_PP(coll);
}
/* }}} */

#endif

/* {{{ _oci_get_ocidesc()
*/
static int _oci_get_ocidesc(zval *id,oci_descriptor **descriptor TSRMLS_DC)
{
	zval **desc;
	
	if (zend_hash_find(Z_OBJPROP_P(id), "descriptor", sizeof("descriptor"), (void **)&desc) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot find descriptor");
		return 0;
	}

	if ((*descriptor = oci_get_desc(Z_LVAL_PP(desc) TSRMLS_CC)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "descriptor not found");
		return 0;
	}
	
	return Z_LVAL_PP(desc);
}
/* }}} */

/* {{{ _oci_make_zval()
*/
static int _oci_make_zval(zval *value,oci_statement *statement,oci_out_column *column, char *func, int mode TSRMLS_DC)
{
	oci_descriptor *descr;
	ub4 loblen;
	int size;
	char *buffer;
	
	if (column->indicator || column->retcode)
		if ((column->indicator != -1) && (column->retcode != 1405))
			oci_debug("_oci_make_zval: %16s,retlen = %4d,retlen4 = %d,storage_size4 = %4d,indicator %4d, retcode = %4d",
					column->name,column->retlen,column->retlen4,column->storage_size4,column->indicator,column->retcode);
	
	if ((!statement->has_data) || (column->indicator == -1)) { /* column is NULL or statment has no current data */
		ZVAL_NULL(value); 
		return 0;
	}
	
	if (column->is_cursor) { /* REFCURSOR -> simply return the statement id */
		ZVAL_RESOURCE(value,column->stmtid);
		zend_list_addref(column->stmtid);
	} else if (column->is_descr) {
		if ((column->data_type != SQLT_RDD) && (mode & OCI_RETURN_LOBS)) {
			/* OCI_RETURN_LOBS means that we want the content of the LOB back instead of the locator */

			descr = oci_get_desc(column->descid TSRMLS_CC);
			if (!descr) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to find my descriptor %p",column->data);
				return -1;
			}
			
			if (oci_loadlob(statement->conn,descr,&buffer,&loblen)) {
				ZVAL_FALSE(value);
			} else {
				ZVAL_STRINGL(value,buffer,loblen,0);
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
				size = column->retlen4;
			} else {
				size = column->retlen;
			}
			break;
			
		default:				
			/* XXX we SHOULD maybe have a different behaviour for unknown results! */
			ZVAL_FALSE(value); 
			return 0;
		}
		
		ZVAL_STRINGL(value,column->data,size,1);
	}

	return 0;
}
/* }}} */

/* {{{ oci_setprefetch()
*/
static int oci_setprefetch(oci_statement *statement,int size)
{ 
	ub4   prefetch;
	sword error;
	TSRMLS_FETCH();

	prefetch = size * 1024;

	CALL_OCI_RETURN(error,
		OCIAttrSet(
			statement->pStmt,
			OCI_HTYPE_STMT,
		  	&prefetch,
			0, 
			OCI_ATTR_PREFETCH_MEMORY,
			statement->pError
		)
	);

	statement->error = oci_error(statement->pError, "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY", error);

	if (statement->error) {
		oci_handle_error(statement->conn, statement->error);
	}

	prefetch = size;
	CALL_OCI_RETURN(error,
		OCIAttrSet(
			statement->pStmt,
			OCI_HTYPE_STMT,
			&prefetch,
			0, 
			OCI_ATTR_PREFETCH_ROWS,
			statement->pError
		)
	);

	statement->error = oci_error(statement->pError, "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY", error);
	if (statement->error) {
		oci_handle_error(statement->conn, statement->error);
	}

	return 1;
}
/* }}} */

/* {{{ oci_parse()
*/
static oci_statement *oci_parse(oci_connection *connection, char *query, int len)
{
	oci_statement *statement;
	sword error;
	TSRMLS_FETCH();

	statement = ecalloc(1,sizeof(oci_statement));

	CALL_OCI(
		OCIHandleAlloc(
			connection->session->pEnv,
			(dvoid **)&statement->pStmt,
			OCI_HTYPE_STMT, 
			0, 
			NULL
		)
	);

	CALL_OCI(
		OCIHandleAlloc(
			connection->session->pEnv,
			(dvoid **)&statement->pError,
			OCI_HTYPE_ERROR,
			0,
			NULL
		)
	);

	if (len > 0) {
		CALL_OCI_RETURN(error, 
			OCIStmtPrepare(
				statement->pStmt,
				connection->pError,
				(text*)query,
				len,
				OCI_NTV_SYNTAX,
				OCI_DEFAULT
			)
		);

		connection->error = oci_error(connection->pError, "OCIParse", error);
		if (connection->error) {
			CALL_OCI(
				OCIHandleFree(
					statement->pStmt, 
					OCI_HTYPE_STMT
				)
			);
			
			CALL_OCI(
				OCIHandleFree(
					statement->pError, 
					OCI_HTYPE_ERROR
				)
			);

			efree(statement);
			oci_handle_error(connection, connection->error);
			return 0;
		}
	}

	if (query) {
		statement->last_query = estrdup(query);
	}

	statement->conn = connection;
	statement->has_data = 0;

	statement->id = zend_list_insert(statement,le_stmt);

	oci_debug("oci_parse \"%s\" id=%d conn=%d",
				SAFE_STRING(query),
				statement->id,
				statement->conn->id);

	zend_list_addref(statement->conn->id);

	return statement;
}
/* }}} */

/* {{{ oci_execute()
*/
static int oci_execute(oci_statement *statement, char *func,ub4 mode)
{
	oci_out_column *outcol;
	oci_out_column column;
	OCIParam *param = 0;
	text *colname;
	ub4 counter;
	ub2 define_type;
	ub4 iters;
	ub4 colcount;
	ub2 dynamic;
	int dtype;
	dvoid *buf;
	oci_descriptor *descr;
	sword error;
	TSRMLS_FETCH();

	if (!statement->stmttype) {
		CALL_OCI_RETURN(error,
			OCIAttrGet(
				(dvoid *)statement->pStmt,
				OCI_HTYPE_STMT,
				(ub2 *)&statement->stmttype,
				(ub4 *)0,
				OCI_ATTR_STMT_TYPE,
				statement->pError
			)
		);

		statement->error = oci_error(statement->pError, "OCIAttrGet OCI_HTYPE_STMT/OCI_ATTR_STMT_TYPE", error);

		if (statement->error) {
			oci_handle_error(statement->conn, statement->error);
			return 0;
		}
	}

	if (statement->stmttype == OCI_STMT_SELECT) {
		iters = 0;
	} else {
		iters = 1;
	}
	
	if (statement->last_query) { 
		/* if we execute refcursors we don't have a query and 
		   we don't want to execute!!! */

		if (statement->binds) {
			zend_hash_apply(statement->binds, (apply_func_t) _oci_bind_pre_exec TSRMLS_CC);
		}

		CALL_OCI_RETURN(error,
			OCIStmtExecute(
				statement->conn->pServiceContext,
				statement->pStmt,
				statement->pError,
				iters,
				0,
				NULL,
				NULL,
				mode
			)
		);

		statement->error = oci_error(statement->pError, "OCIStmtExecute", error);

		if (statement->binds) {
			zend_hash_apply(statement->binds, (apply_func_t) _oci_bind_post_exec TSRMLS_CC);
		}

		oci_handle_error(statement->conn, statement->error);

		if (statement->error) {
			return 0;
		}

		if (mode & OCI_COMMIT_ON_SUCCESS) {
			statement->conn->needs_commit = 0;
		} else {
			statement->conn->needs_commit = 1;
		}
	}

	if ((statement->stmttype == OCI_STMT_SELECT) && (statement->executed == 0)) {
		/* we only need to do the define step is this very statement is executed the first time! */
		statement->executed = 1;
		
		ALLOC_HASHTABLE(statement->columns);
		zend_hash_init(statement->columns, 13, NULL, _oci_column_hash_dtor, 0);
		
		counter = 1;

		CALL_OCI_RETURN(error,
			OCIAttrGet(
				(dvoid *)statement->pStmt,
				OCI_HTYPE_STMT,
				(dvoid *)&colcount,
				(ub4 *)0,
				OCI_ATTR_PARAM_COUNT,
				statement->pError
			)
		);

		statement->error = oci_error(statement->pError, "OCIAttrGet OCI_HTYPE_STMT/OCI_ATTR_PARAM_COUNT", error);
		if (statement->error) {
			oci_handle_error(statement->conn, statement->error);
			return 0; /* XXX we loose memory!!! */
		}

		statement->ncolumns = colcount;
		
		for (counter = 1; counter <= colcount; counter++) {
			memset(&column,0,sizeof(oci_out_column));
			
			if (zend_hash_index_update(statement->columns, counter, &column,
										sizeof(oci_out_column), (void**) &outcol) == FAILURE) {
				efree(statement->columns);
				/* out of memory */
				return 0;
			} 
			
			outcol->statement = statement;

			CALL_OCI_RETURN(error,
				OCIParamGet(
					(dvoid *)statement->pStmt,
					OCI_HTYPE_STMT,
					statement->pError,
					(dvoid**)&param,
					counter
				)
			);

			statement->error = oci_error(statement->pError, "OCIParamGet OCI_HTYPE_STMT", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			CALL_OCI_RETURN(error,
				OCIAttrGet(
					(dvoid *)param,
					OCI_DTYPE_PARAM,
					(dvoid *)&outcol->data_type,
					(ub4 *)0,
					OCI_ATTR_DATA_TYPE,
					statement->pError
				)
			);

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_TYPE", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			CALL_OCI_RETURN(error,
				OCIAttrGet(
					(dvoid *)param,
					OCI_DTYPE_PARAM,
					(dvoid *)&outcol->data_size,
					(dvoid *)0,
					OCI_ATTR_DATA_SIZE,
					statement->pError
				)
			);

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_SIZE", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			outcol->storage_size4 = outcol->data_size;
			outcol->retlen = outcol->data_size;
	
			CALL_OCI_RETURN(error,
				OCIAttrGet(
					(dvoid *)param, 
					OCI_DTYPE_PARAM, 
					(dvoid *)&outcol->scale, 
					(dvoid *)0, 
					OCI_ATTR_SCALE, 
					statement->pError
				)
			);
						
			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_SCALE", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we lose memory!!! */
			}

			CALL_OCI_RETURN(error,
				OCIAttrGet(
					(dvoid *)param, 
					OCI_DTYPE_PARAM, 
					(dvoid *)&outcol->precision, 
					(dvoid *)0, 
					OCI_ATTR_PRECISION, 
					statement->pError
				)
			);

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_PRECISION", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we lose memory!!! */
			}
					
			CALL_OCI_RETURN(error,
				OCIAttrGet(
					(dvoid *)param, 
					OCI_DTYPE_PARAM, 
					(dvoid **)&colname,					/* XXX this string is NOT zero terminated!!!! */ 
					(ub4 *)&outcol->name_len, 
					(ub4)OCI_ATTR_NAME, 
					statement->pError
				)
			);

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_NAME", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			outcol->name =  estrndup((char*) colname,outcol->name_len);

			/* find a user-setted define */
			if (statement->defines) {
				zend_hash_find(statement->defines,outcol->name,outcol->name_len,(void **) &outcol->define);
			}

			buf = 0;
			switch (outcol->data_type) {
				case SQLT_RSET:
					outcol->pstmt = oci_parse(statement->conn,0,0);
					outcol->stmtid = outcol->pstmt->id;

					define_type = SQLT_RSET;
					outcol->is_cursor = 1;
					outcol->storage_size4 = -1;
					outcol->retlen = -1;
					dynamic = OCI_DEFAULT;
					buf = &(outcol->pstmt->pStmt);
					break;

			 	case SQLT_RDD:	 /* ROWID */
				case SQLT_BLOB:  /* binary LOB */
				case SQLT_CLOB:  /* character LOB */
				case SQLT_BFILE: /* binary file LOB */
					define_type = outcol->data_type;
					outcol->is_descr = 1;
					outcol->storage_size4 = -1;
					dynamic = OCI_DEFAULT;

					if (outcol->data_type == SQLT_BFILE) {
						dtype = OCI_DTYPE_FILE;
					} else if (outcol->data_type == SQLT_RDD ) {
						dtype = OCI_DTYPE_ROWID;
					} else {
						dtype = OCI_DTYPE_LOB;
					}
					
					descr = oci_new_desc(dtype,statement->conn);
					if (!descr) {
						/* need better error checking XXX */
					}
					outcol->descid = descr->id;
					buf =  &(descr->ocidescr);
					break;

				case SQLT_LNG:
				case SQLT_LBI:
					if (outcol->data_type == SQLT_LBI) {
						define_type = SQLT_BIN;
					} else {
						define_type = SQLT_CHR;
					}
					outcol->storage_size4 = OCI_MAX_DATA_SIZE;
					outcol->piecewise = 1;
					dynamic = OCI_DYNAMIC_FETCH;
					break;

				case SQLT_BIN:
				default:
					define_type = SQLT_CHR;
					if ((outcol->data_type == SQLT_DAT) || (outcol->data_type == SQLT_NUM)
#ifdef SQLT_TIMESTAMP
						|| (outcol->data_type == SQLT_TIMESTAMP)
#endif
#ifdef SQLT_TIMESTAMP_TZ
						|| (outcol->data_type == SQLT_TIMESTAMP_TZ)
#endif
						) {
						outcol->storage_size4 = 512; /* XXX this should fit "most" NLS date-formats and Numbers */
					} else {
						outcol->storage_size4++; /* add one for string terminator */
					}
					if (outcol->data_type == SQLT_BIN) {
						outcol->storage_size4 *= 3;
					}
					dynamic = OCI_DEFAULT;
					buf = outcol->data = (text *) emalloc(outcol->storage_size4);
					break;
			}

			if (dynamic == OCI_DYNAMIC_FETCH) {
				CALL_OCI_RETURN(error,
					OCIDefineByPos(
						statement->pStmt,                           /* IN/OUT handle to the requested SQL query */
						(OCIDefine **)&outcol->pDefine,             /* IN/OUT pointer to a pointer to a define handle */
						statement->pError,                          /* IN/OUT An error handle  */
						counter,                                    /* IN     position in the select list */
						(dvoid *)NULL,                              /* IN/OUT pointer to a buffer */
						outcol->storage_size4,                      /* IN     The size of each valuep buffer in bytes */
						define_type,                                /* IN     The data type */
						(dvoid *)&outcol->indicator,                /* IN     pointer to an indicator variable or arr */
						(ub2 *)NULL,                                /* IN/OUT Pointer to array of length of data fetched */
						(ub2 *)NULL,                                /* OUT    Pointer to array of column-level return codes */
						OCI_DYNAMIC_FETCH	                        /* IN     mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
					)
				);

				statement->error = oci_error(statement->pError, "OCIDefineByPos", error);
			} else {
				CALL_OCI_RETURN(error,
					OCIDefineByPos(
						statement->pStmt,                           /* IN/OUT handle to the requested SQL query */ 
						(OCIDefine **)&outcol->pDefine,             /* IN/OUT pointer to a pointer to a define handle */
						statement->pError,                          /* IN/OUT An error handle  */
						counter,                                    /* IN     position in the select list */
						(dvoid *)buf,                               /* IN/OUT pointer to a buffer */
						outcol->storage_size4,                      /* IN     The size of each valuep buffer in bytes */
						define_type,                                /* IN     The data type */
						(dvoid *)&outcol->indicator,                /* IN     pointer to an indicator variable or arr */
						(ub2 *)&outcol->retlen,                     /* IN/OUT Pointer to array of length of data fetched */
						(ub2 *)&outcol->retcode,                    /* OUT    Pointer to array of column-level return codes */
						OCI_DEFAULT	                                /* IN     mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
					)
				);

				statement->error = oci_error(statement->pError, "OCIDefineByPos", error);
			}
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}
		}
	}

	return 1;
}
/* }}} */

/* {{{ oci_fetch()
*/
static int _oci_column_pre_fetch(void *data TSRMLS_DC)
{
	oci_out_column *col = (oci_out_column *) data;
	
	if (col->piecewise) {
		col->retlen4 = 0; 
	}
	
	return 0;
}

static int oci_fetch(oci_statement *statement, ub4 nrows, char *func TSRMLS_DC)
{
	int i;
	oci_out_column *column;

	if (statement->columns) {
		zend_hash_apply(statement->columns, (apply_func_t) _oci_column_pre_fetch TSRMLS_CC);
	}


	CALL_OCI_RETURN(statement->error,
		OCIStmtFetch(
			statement->pStmt, 
			statement->pError, 
			nrows, 
			OCI_FETCH_NEXT, 
			OCI_DEFAULT
		)
	);

	if ((statement->error == OCI_NO_DATA) || (nrows == 0)) {
		if (statement->last_query == 0) {
			/* reset define-list for refcursors */
			if (statement->columns) {
				zend_hash_destroy(statement->columns);
				efree(statement->columns);
				statement->columns = 0;
				statement->ncolumns = 0;
			}
			statement->executed = 0;
		}

		statement->error = 0; /* OCI_NO_DATA is NO error for us!!! */
		statement->has_data = 0;

		return 0;
	}

	while (statement->error == OCI_NEED_DATA) {
		for (i = 0; i < statement->ncolumns; i++) {
			column = oci_get_col(statement, i + 1, 0);
			if (column->piecewise)	{
				if (!column->data) {
					column->data = (text *) emalloc(OCI_PIECE_SIZE);
				} else {
					column->data = erealloc(column->data,column->retlen4 + OCI_PIECE_SIZE);
				}

				column->cb_retlen = OCI_PIECE_SIZE;

				CALL_OCI(
					OCIStmtSetPieceInfo(
						(void *) column->pDefine, 
						OCI_HTYPE_DEFINE, 
						statement->pError, 
						((char*)column->data) + column->retlen4, 
						&(column->cb_retlen), 
						OCI_NEXT_PIECE, 
						&column->indicator, 
						&column->retcode
					)
				);
			}
		}

		CALL_OCI_RETURN(statement->error,
			OCIStmtFetch(
				statement->pStmt, 
				statement->pError, 
				nrows, 
				OCI_FETCH_NEXT, 
				OCI_DEFAULT
			)
		);

		for (i = 0; i < statement->ncolumns; i++) {
			column = oci_get_col(statement, i + 1, 0);
			if (column->piecewise)	{
				column->retlen4 += column->cb_retlen;
			}
		}
	}

	if (statement->error == OCI_SUCCESS_WITH_INFO || statement->error == OCI_SUCCESS) {
		statement->has_data = 1;

		/* do the stuff needed for OCIDefineByName */
		for (i = 0; i < statement->ncolumns; i++) {
			column = oci_get_col(statement, i + 1, 0);
			if (column == NULL) {
				continue;
			}
			
			if (!column->define) {
				continue;
			}
			
			zval_dtor(column->define->zval);
			_oci_make_zval(column->define->zval,statement,column,"OCIFetch",0 TSRMLS_CC);
		}

		return 1;
	}

	oci_error(statement->pError, func, statement->error);
	oci_handle_error(statement->conn, statement->error);

	statement->has_data = 0;

	return 0;
}
/* }}} */

/* {{{ oci_lobgetlen()
*/
static int oci_lobgetlen(oci_connection *connection, oci_descriptor *mydescr, ub4 *loblen)
{
	TSRMLS_FETCH();

	*loblen = 0;
	
	/* do we need to ask oracle about LOB's length, if we do already know it? I think no. */
	if (mydescr->lob_size >= 0) {
		*loblen = mydescr->lob_size;
	} else {
		if (mydescr->type == OCI_DTYPE_FILE) {
			CALL_OCI_RETURN(connection->error,
				OCILobFileOpen(
					connection->pServiceContext, 
					connection->pError, 
					mydescr->ocidescr,
					OCI_FILE_READONLY
				)
			);
			if (connection->error) {
				oci_error(connection->pError, "OCILobFileOpen",connection->error);
				oci_handle_error(connection, connection->error);
				return -1;
			}
		}
		
		CALL_OCI_RETURN(connection->error, 
			OCILobGetLength(
					connection->pServiceContext, 
					connection->pError, 
					mydescr->ocidescr, 
					loblen
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobGetLength",connection->error);
			oci_handle_error(connection, connection->error);
			return -1;
		}
		mydescr->lob_size = *loblen;

		if (mydescr->type == OCI_DTYPE_FILE) {
			CALL_OCI_RETURN(connection->error,
				OCILobFileClose(
					connection->pServiceContext, 
					connection->pError, 
					mydescr->ocidescr
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCILobFileClose", connection->error);
				oci_handle_error(connection, connection->error);
				return -1;
			}
		}
	}
	
	oci_debug("oci_lobgetlen: len=%d",*loblen);

	return 0;
}
/* }}} */

/* {{{ oci_loadlob()
*/
#define LOBREADSIZE 1048576l /* 1MB */
static int oci_loadlob(oci_connection *connection, oci_descriptor *mydescr, char **buffer, ub4 *loblen)
{
	ub4 siz = 0;
	ub4 readlen = 0;
	char *buf;
	TSRMLS_FETCH();

	*loblen = 0;
	
	if (mydescr->type == OCI_DTYPE_FILE) {
		CALL_OCI_RETURN(connection->error,
			OCILobFileOpen(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr,
				OCI_FILE_READONLY
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobFileOpen",connection->error);
			oci_handle_error(connection, connection->error);
			return -1;
		}
	}
	
	CALL_OCI_RETURN(connection->error,
		OCILobGetLength(
			connection->pServiceContext, 
			connection->pError, 
			mydescr->ocidescr, 
			&readlen
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCILobGetLength",connection->error);
		oci_handle_error(connection, connection->error);
		return -1;
	}

	buf = emalloc(readlen + 1);

	while (readlen > 0) { /* thies loop should not be entered on readlen == 0 */
		CALL_OCI_RETURN(connection->error,
			OCILobRead(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr, 
				&readlen,								/* IN/OUT bytes toread/read */ 
				siz + 1,								/* offset (starts with 1) */ 
				(dvoid *) ((char *) buf + siz),	
				readlen,		 						/* size of buffer */ 
				(dvoid *)0, 
				(OCICallbackLobRead) 0,					/* callback... */ 
				(ub2) connection->session->charsetId,	/* The character set ID of the buffer data. */ 
				(ub1) SQLCS_IMPLICIT					/* The character set form of the buffer data. */
			)
		);

		siz += readlen;
		readlen = LOBREADSIZE;

		if (connection->error == OCI_NEED_DATA) {
			buf = erealloc(buf,siz + LOBREADSIZE + 1);	
			continue;
		} else {
			break;
		}
	}

	if (connection->error) {
		oci_error(connection->pError, "OCILobRead", connection->error);
		oci_handle_error(connection, connection->error);
		efree(buf);
		return -1;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		CALL_OCI_RETURN(connection->error,
			OCILobFileClose(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobFileClose", connection->error);
			oci_handle_error(connection, connection->error);
			efree(buf);
			return -1;
		}
	}

	buf = erealloc(buf,siz+1);
	buf[ siz ] = 0;

	*buffer = buf;
	*loblen = siz;

	oci_debug("oci_loadlob: size=%d",siz);

	return 0;
}
/* }}} */

/* {{{ oci_readlob()
*/
static int oci_readlob(oci_connection *connection, oci_descriptor *mydescr, char **buffer, ub4 *len)
{
	ub4 siz = 0;
	ub4 readlen = 0;
	ub4 loblen = 0;
	ub4 bytes = 0;
	char *buf;
	TSRMLS_FETCH();

	/* we're not going to read LOB, if length is not known */
	if (!len || (int)*len <= 0) {
		return -1;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		CALL_OCI_RETURN(connection->error,
			OCILobFileOpen(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr,
				OCI_FILE_READONLY
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobFileOpen",connection->error);
			oci_handle_error(connection, connection->error);
			return -1;
		}
	}
	
	if (oci_lobgetlen(connection, mydescr, &loblen) != 0) {
		*len = 0;
		return -1;
	}
	
	/* check if we're in LOB's borders */
	if ((mydescr->lob_current_position + *len) > loblen) {
		*len = loblen - mydescr->lob_current_position;
	}

	if ((int)*len > 0) {
		buf = emalloc(*len + 1);

		/* set offset to current LOB's position */
		siz = mydescr->lob_current_position;

		/* check if len is smaller, if not - using LOBREADSIZE' sized buffer */
		if (*len > LOBREADSIZE) {
			readlen = LOBREADSIZE;
		} else {
			readlen = *len;
		}
	} else {
		*len = 0;
		return -1;
	}

	while (readlen > 0 && bytes < *len && siz < loblen) {  /* paranoia */
		CALL_OCI_RETURN(connection->error,
			OCILobRead(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr, 
				&readlen,								/* IN/OUT bytes toread/read */ 
				siz + 1,								/* offset (starts with 1) */ 
				(dvoid *) ((char *) buf + bytes),	
				readlen,		 						/* size of buffer */ 
				(dvoid *)0, 
				(OCICallbackLobRead) 0, 				/* callback... */ 
				(ub2) connection->session->charsetId,	/* The character set ID of the buffer data. */ 
				(ub1) SQLCS_IMPLICIT					/* The character set form of the buffer data. */
			)
		);

		siz += readlen;
		bytes += readlen;

		if ((*len - bytes) > LOBREADSIZE) {
			readlen = LOBREADSIZE;
		} else {
			readlen = *len - bytes;
		}

		if (connection->error == OCI_NEED_DATA) {
			buf = erealloc(buf,bytes + LOBREADSIZE + 1);	
			continue;
		} else {
			break;
		}
	}

	/* moving current position */
	mydescr->lob_current_position = siz;

	if (connection->error) {
		oci_error(connection->pError, "OCILobRead", connection->error);
		oci_handle_error(connection, connection->error);
		efree(buf);
		return -1;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		CALL_OCI_RETURN(connection->error,
			OCILobFileClose(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobFileClose", connection->error);
			oci_handle_error(connection, connection->error);
			efree(buf);
			return -1;
		}
	}

	buf = erealloc(buf,bytes+1);
	buf[ bytes ] = 0;

	*buffer = buf;
	*len = bytes;

	oci_debug("oci_readlob: size=%d",bytes);

	return 0;
}
/* }}} */

/* {{{ oci_failover_callback()
*/
#if 0 /* not needed yet ! */
static sb4 oci_failover_callback(dvoid *svchp, dvoid *envhp, dvoid *fo_ctx, ub4 fo_type, ub4 fo_event)
{
	/* 
	   this stuff is from an oci sample - it will get cleaned up as soon as i understand it!!! (thies@thieso.net 990420) 
	   right now i cant get oracle to even call it;-(((((((((((
	*/

	switch (fo_event) {
		case OCI_FO_BEGIN: {
			printf(" Failing Over ... Please stand by \n");
			printf(" Failover type was found to be %s \n",
					((fo_type==OCI_FO_NONE) ? "NONE"
					:(fo_type==OCI_FO_SESSION) ? "SESSION"
					:(fo_type==OCI_FO_SELECT) ? "SELECT"
					: "UNKNOWN!"));
			printf(" Failover Context is :%s\n",
					(fo_ctx?(char *)fo_ctx:"NULL POINTER!"));
			break;
		}

		case OCI_FO_ABORT: {
			printf(" Failover aborted. Failover will not take place.\n");
			break;
		}

		case OCI_FO_END: {
			printf(" Failover ended ...resuming services\n");
			break;
		}

		case OCI_FO_REAUTH: {
			printf(" Failed over user. Resuming services\n");
				
			/* Application can check the OCI_ATTR_SESSION attribute of
			   the service handle to find out the user being
			   re-authenticated.
				   
			   After this, the application can replay any ALTER SESSION
			   commands associated with this session.  These must have
			   been saved by the application in the fo_ctx
			*/
			break;
		}

		case OCI_FO_ERROR: {
			printf(" Failover error gotten. Sleeping...\n");
			php_sleep(3);
			/* cannot find this blody define !!! return OCI_FO_RETRY; */
			break;
		}
			
		default: {
			printf("Bad Failover Event: %ld.\n",  fo_event);
			break;
		}
	}

	return 0;  
}
#endif
/* }}} */

/* {{{ oci_bind_in_callback()
*/
static sb4 oci_bind_in_callback(
					dvoid *ictxp,     /* context pointer */
					OCIBind *bindp,   /* bind handle */
					ub4 iter,         /* 0-based execute iteration value */
					ub4 index,        /* index of current array for PL/SQL or row index for SQL */
					dvoid **bufpp,    /* pointer to data */
					ub4 *alenp,       /* size after value/piece has been read */
					ub1 *piecep,      /* which piece */
					dvoid **indpp)    /* indicator value */
{
	oci_bind *phpbind;
	zval *val;
	TSRMLS_FETCH();

	if (!(phpbind=(oci_bind *)ictxp) || !(val = phpbind->zval)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "!phpbind || !phpbind->val");
		return OCI_ERROR;
	}

	if (ZVAL_IS_NULL(val)) {
		/* we're going to insert a NULL column */
		phpbind->indicator = -1;
		*bufpp = 0;
		*alenp = -1;
		*indpp = (dvoid *)&phpbind->indicator;
	} else 	if ((phpbind->descr == 0) && (phpbind->pStmt == 0)) {
		/* "normal string bind */
		convert_to_string(val); 

		*bufpp = Z_STRVAL_P(val);
		*alenp = Z_STRLEN_P(val);
		*indpp = (dvoid *)&phpbind->indicator;
	} else if (phpbind->pStmt != 0) {
		/* RSET */
		*bufpp = phpbind->pStmt;
		*alenp = -1;		/* seems to be allright */
		*indpp = (dvoid *)&phpbind->indicator;
	} else { 
		/* descriptor bind */
		*bufpp = phpbind->descr;
		*alenp = -1;		/* seems to be allright */
		*indpp = (dvoid *)&phpbind->indicator;
	}

	*piecep = OCI_ONE_PIECE; /* pass all data in one go */

	return OCI_CONTINUE;
}
/* }}} */

/* {{{ oci_bind_out_callback() */
static sb4 oci_bind_out_callback(
					dvoid *octxp,      /* context pointer */
					OCIBind *bindp,    /* bind handle */
					ub4 iter,          /* 0-based execute iteration value */
					ub4 index,         /* index of current array for PL/SQL or row index for SQL */
					dvoid **bufpp,     /* pointer to data */
					ub4 **alenpp,      /* size after value/piece has been read */
					ub1 *piecep,       /* which piece */
					dvoid **indpp,     /* indicator value */
					ub2 **rcodepp)     /* return code */
{
	oci_bind *phpbind;
	zval *val;
	sb4 retval = OCI_ERROR;
	TSRMLS_FETCH();

	if (!(phpbind=(oci_bind *)octxp) || !(val = phpbind->zval)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "!phpbind || !phpbind->val");
		return retval;
	}

	if ((Z_TYPE_P(val) == IS_OBJECT) || (Z_TYPE_P(val) == IS_RESOURCE)) {
		retval = OCI_CONTINUE;
	} else {
		convert_to_string(val);
		zval_dtor(val);
		
		Z_STRLEN_P(val) = OCI_PIECE_SIZE; /* 64K-1 is max XXX */
		Z_STRVAL_P(val) = emalloc(Z_STRLEN_P(phpbind->zval));
		
		/* XXX we assume that zend-zval len has 4 bytes */
		*alenpp = (ub4*) &Z_STRLEN_P(phpbind->zval); 
		*bufpp = Z_STRVAL_P(phpbind->zval);
		*piecep = OCI_ONE_PIECE;
		*rcodepp = &phpbind->retcode;
		*indpp = &phpbind->indicator;
		retval = OCI_CONTINUE;
	}

	return retval;
}
/* }}} */

/* {{{ _oci_open_session()
*/
#include "ext/standard/php_smart_str.h"
static oci_session *_oci_open_session(oci_server* server,char *username,char *password,int persistent,int exclusive,char *charset)
{
	zend_llist *session_list;
	oci_session *session = NULL;
	OCISvcCtx *svchp = NULL;
	smart_str hashed_details = {0};
#ifdef HAVE_OCI_9_2
	ub2 charsetid = 0;
#endif
	TSRMLS_FETCH();

	/* 
	   check if we already have this user authenticated

	   we will reuse authenticated users within a request no matter if the user requested a persistent 
	   connections or not!
	   
	   but only as persistent requested connections will be kept between requests!
	*/

#if defined(HAVE_OCI_9_2)
	if (*charset) {
		smart_str_appends_ex(&hashed_details, charset, 1);
	} else {
		size_t rsize;

		/* Safe, charsetid is initialized to 0 */
		CALL_OCI(
			OCINlsEnvironmentVariableGet(
				&charsetid, 
				2, 
				OCI_NLS_CHARSET_ID, 
				0,
				&rsize
			)
		);

		smart_str_append_unsigned_ex(&hashed_details, charsetid, 1);

		charsetid = 0;
	}
#else
	{
		char *nls_lang = getenv("NLS_LANG");

		/* extract charset from NLS_LANG=LANUAGE_TERRITORY.CHARSET */
		if (nls_lang) {
			char *p = strchr(nls_lang, '.');

			if (p) {
				smart_str_appends_ex(&hashed_details, p + 1, 1);
			}
		}
	}
#endif

	smart_str_appends_ex(&hashed_details, SAFE_STRING(username), 1);
	smart_str_appends_ex(&hashed_details, SAFE_STRING(password), 1);
	smart_str_appends_ex(&hashed_details, SAFE_STRING(server->dbname), 1);
	smart_str_0(&hashed_details);

	if (!exclusive) {
		if (zend_ts_hash_find(persistent_sessions, hashed_details.c, hashed_details.len+1, (void **) &session_list) != SUCCESS) {
			zend_llist tmp;
			/* first session, set up a session list */
			zend_llist_init(&tmp, sizeof(oci_session), (llist_dtor_func_t) _session_pcleanup, 1);
			zend_ts_hash_update(persistent_sessions, hashed_details.c, hashed_details.len+1, &tmp, sizeof(zend_llist), (void **) &session_list);
		} else {
			mutex_lock(mx_lock);

			/* session list found, search for an idle session or an already opened session by the current thread */
			session = zend_llist_get_first(session_list);
			while ((session != NULL) && session->thread && (session->thread != thread_id())) {
				session = zend_llist_get_next(session_list);
			}

			if (session != NULL) {
				/* mark session as busy */
				session->thread = thread_id();
			}

			mutex_unlock(mx_lock);
		}

		if (session) {
			if (session->is_open) {
				if (persistent) {
					session->persistent = 1;
				}
				smart_str_free_ex(&hashed_details, 1);
				return session;
			} else {
				_oci_close_session(session);
				/* breakthru to open */
			}
		}
	}

	session = ecalloc(1,sizeof(oci_session));

	if (!session) {
		goto CLEANUP;
	}

	session->persistent = persistent;
	session->server = server;
	session->exclusive = exclusive;
	session->sessions_list = session_list;
	session->thread = thread_id();

#ifdef HAVE_OCI_9_2

	/* following chunk is Oracle 9i+ ONLY */
	if (*charset) {
		/*
		   get ub2 charset id based on charset
		   this is pretty secure, since if we don't have a valid character set name,
		   0 comes back and we can still use the 0 in all further statements -> OCI uses NLS_LANG
		   setting in that case
		*/
		CALL_OCI_RETURN(charsetid,
			OCINlsCharSetNameToId(
				OCI(pEnv),
				charset
			)
		);
		
		oci_debug("oci_do_connect: using charset id=%d",charsetid);
	}

	session->charsetId = charsetid;
	
	/* create an environment using the character set id, Oracle 9i+ ONLY */
	CALL_OCI(
		OCIEnvNlsCreate(
			&session->pEnv,
			PHP_OCI_INIT_MODE, 
			0, 
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			charsetid,
			charsetid
		)
	);

#else

	/* fallback solution (simply use global env and charset, same behaviour as always been) */
	session->pEnv = OCI(pEnv);
	session->charsetId = 0;

#endif  /* HAVE_OCI_9_2 */

	/* allocate temporary Service Context */
	CALL_OCI_RETURN(OCI(error),
		OCIHandleAlloc(
			session->pEnv, 
			(dvoid **)&svchp, 
			OCI_HTYPE_SVCCTX, 
			0, 
			NULL
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "_oci_open_session: OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI(error));
		goto CLEANUP;
	}

	/* allocate private session-handle */
	CALL_OCI_RETURN(OCI(error),
		OCIHandleAlloc(
			session->pEnv, 
			(dvoid **)&session->pSession, 
			OCI_HTYPE_SESSION, 
			0, 
			NULL
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "_oci_open_session: OCIHandleAlloc OCI_HTYPE_SESSION", OCI(error));
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	CALL_OCI_RETURN(OCI(error),
		OCIAttrSet(
			svchp, 
			OCI_HTYPE_SVCCTX, 
			server->pServer, 
			0,
			OCI_ATTR_SERVER,
			OCI(pError)
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "_oci_open_session: OCIAttrSet OCI_ATTR_SERVER", OCI(error));
		goto CLEANUP;
	}

	/* set the username in user handle */
	CALL_OCI_RETURN(OCI(error),
		OCIAttrSet(
			(dvoid *) session->pSession, 
			(ub4) OCI_HTYPE_SESSION, 
			(dvoid *) username, 
			(ub4) strlen(username), 
			(ub4) OCI_ATTR_USERNAME, 
			OCI(pError)
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCIAttrSet OCI_ATTR_USERNAME", OCI(error));
		goto CLEANUP;
	}

	/* set the password in user handle */
	CALL_OCI_RETURN(OCI(error),
		OCIAttrSet(
			(dvoid *) session->pSession, 
			(ub4) OCI_HTYPE_SESSION, 
			(dvoid *) password, 
			(ub4) strlen(password), 
			(ub4) OCI_ATTR_PASSWORD, 
			OCI(pError)
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCIAttrSet OCI_ATTR_PASSWORD", OCI(error));
		goto CLEANUP;
	}

	CALL_OCI_RETURN(OCI(error),
		OCISessionBegin(
			svchp, 
			OCI(pError), 
			session->pSession, 
			(ub4) OCI_CRED_RDBMS, 
			(ub4) OCI_DEFAULT
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCISessionBegin", OCI(error));
		goto CLEANUP;
	}

	/* Free Temporary Service Context */
	CALL_OCI(
		OCIHandleFree(
			(dvoid *) svchp, 
			(ub4) OCI_HTYPE_SVCCTX
		)
	);

	session->num = zend_list_insert(session, le_session);
 	session->is_open = 1;

	mutex_lock(mx_lock);
		num_links++;
		if (!exclusive) {
			zend_llist_add_element(session_list, session);
			efree(session);
			session = (oci_session*) session_list->tail->data;
			num_persistent++;
		}
	mutex_unlock(mx_lock);

	oci_debug("_oci_open_session new sess=%d user=%s",session->num,username);

	return session;

CLEANUP:
	oci_debug("_oci_open_session: FAILURE -> CLEANUP called");

	_oci_close_session(session);

	return 0;
}
/* }}} */

/* {{{ _oci_close_session()
*/
static int _session_compare(void *a, void *b)
{
	oci_session *sess1 = (oci_session*) a;
	oci_session *sess2 = (oci_session*) b;
	
	return sess1->num = sess2->num;
}

static void _oci_close_session(oci_session *session)
{
	OCISvcCtx *svchp;
	TSRMLS_FETCH();

	if (!session) {
		return;
	}

	oci_debug("START _oci_close_session: logging-off sess=%d",session->num);

	if (session->is_open) {
		/* Temporary Service Context */
		CALL_OCI_RETURN(OCI(error),
			OCIHandleAlloc(
				session->pEnv, 
				(dvoid **) &svchp, 
				(ub4) OCI_HTYPE_SVCCTX, 
				(size_t) 0, 
				(dvoid **) 0
			)
		);

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI(error));
		}
		
		/* Set the server handle in service handle */ 
		CALL_OCI_RETURN(OCI(error),
			OCIAttrSet(
				svchp, 
				OCI_HTYPE_SVCCTX, 
				session->server->pServer, 
				0, 
				OCI_ATTR_SERVER, 
				OCI(pError)
			)
		);

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session: OCIAttrSet OCI_ATTR_SERVER", OCI(error));
		}
		
		/* Set the Authentication handle in the service handle */
		CALL_OCI_RETURN(OCI(error),
			OCIAttrSet(
				svchp, 	
				OCI_HTYPE_SVCCTX, 
				session->pSession, 
				0, 
				OCI_ATTR_SESSION, 
				OCI(pError)
			)
		);

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session: OCIAttrSet OCI_ATTR_SESSION", OCI(error));
		}
	
		CALL_OCI_RETURN(OCI(error),
			OCISessionEnd(
				svchp, 
				OCI(pError), 
				session->pSession, 
				(ub4) 0
			)
		);

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session: OCISessionEnd", OCI(error));
		}

		CALL_OCI(
			OCIHandleFree(
				(dvoid *) svchp, 
				(ub4) OCI_HTYPE_SVCCTX
			)
		);

	} else {
		oci_debug("_oci_close_session: logging-off DEAD session");
	}

	if (session->pSession) {
		CALL_OCI(
			OCIHandleFree(
				(dvoid *) session->pSession, 
				(ub4) OCI_HTYPE_SESSION
			)
		);
	}

	mutex_lock(mx_lock);
		num_links--;
		if (!OCI(shutdown) && session->persistent) {
			zend_llist_del_element(session->sessions_list, session, _session_compare);
			num_persistent--;
		}
	mutex_unlock(mx_lock);

	if (session->exclusive) {
		efree(session);
	}
}
/* }}} */

/* {{{ _oci_open_server()
*/
static oci_server *_oci_open_server(char *dbname,int persistent)
{ 
	oci_server *server, *pserver = NULL;
	TSRMLS_FETCH();

	/* 
	   check if we already have this server open 
	   
	   we will reuse servers within a request no matter if the user requested persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	/* TODO either keep servers global or don't reuse them at all */
	zend_ts_hash_find(persistent_servers, dbname, strlen(dbname)+1, (void **) &pserver);

	if (pserver) {
		/* XXX ini-flag */
		/*
		if (!oci_ping(pserver)) {
			pserver->is_open = 0;
		}
		*/
		if (pserver->is_open) {
			/* if our new users uses this connection persistent, we're keeping it! */
			if (persistent) {
				pserver->persistent = persistent;
			}

			return pserver;
		} else { /* server "died" in the meantime - try to reconnect! */
			_oci_close_server(pserver);
			/* breakthru to open */
		}
	}
	
	server = ecalloc(1,sizeof(oci_server));

	server->persistent = persistent;
	server->dbname = strdup(SAFE_STRING(dbname));

	CALL_OCI(
		OCIHandleAlloc(
			OCI(pEnv), 
			(dvoid **)&server->pServer, 
			OCI_HTYPE_SERVER, 
			0, 
			NULL
		)
	);

	CALL_OCI_RETURN(OCI(error),
		OCIServerAttach(
			server->pServer, 
			OCI(pError), 
			(text*)server->dbname, 
			strlen(server->dbname), 
			(ub4) OCI_DEFAULT
		)
	);

	if (OCI(error)) {
		oci_error(OCI(pError), "_oci_open_server", OCI(error));
		goto CLEANUP;
	}
	
	zend_ts_hash_update(persistent_servers,
					 server->dbname,
					 strlen(server->dbname)+1, 
					 (void *)server,
					 sizeof(oci_server),
					 (void**)&pserver);

	pserver->num  = zend_list_insert(pserver,le_server);
	pserver->is_open = 1;

	oci_debug("_oci_open_server new conn=%d dname=%s",server->num,server->dbname);

	efree(server);

	return pserver;

CLEANUP:
	oci_debug("_oci_open_server: FAILURE -> CLEANUP called");

	_oci_close_server(server);
		
	return 0;
}

#if 0 
	server->failover.fo_ctx = (dvoid *) server;
	server->failover.callback_function = oci_failover_callback;

	error = OCIAttrSet((dvoid *)server->pServer,
						(ub4) OCI_HTYPE_SERVER,
						(dvoid *) &server->failover, 
						(ub4) 0,
						(ub4) OCI_ATTR_FOCBK,
						OCI(pError));

	if (error) {
		oci_error(OCI(pError), "_oci_open_server OCIAttrSet OCI_ATTR_FOCBK", error);
		goto CLEANUP;
	}
#endif
/* }}} */

/* {{{ _oci_close_server()
*/
static int _oci_session_cleanup(void *data TSRMLS_DC)
{
	zend_rsrc_list_entry *le = (zend_rsrc_list_entry *) data;

	if (le->type == le_session) {
		oci_server *server = ((oci_session*) le->ptr)->server;
		if (server->is_open == 2) 
			return 1;
	}
	return 0;
}

static void _oci_close_server(oci_server *server)
{
	char *dbname;
	int oldopen;
	TSRMLS_FETCH();

	oldopen = server->is_open;
	server->is_open = 2;
	if (!OCI(shutdown)) {
		zend_hash_apply(&EG(regular_list), (apply_func_t) _oci_session_cleanup TSRMLS_CC);
	}
	server->is_open = oldopen;

	oci_debug("START _oci_close_server: detaching conn=%d dbname=%s",server->num,server->dbname);

	/* XXX close server here */

	if (server->is_open) {
		if (server->pServer && OCI(pError)) {
			CALL_OCI_RETURN(OCI(error), 
				OCIServerDetach(
					server->pServer, 
					OCI(pError), 
					OCI_DEFAULT
				)
			);

			if (OCI(error)) {
				oci_error(OCI(pError), "oci_close_server OCIServerDetach", OCI(error));
			}
		}
	} else {
		oci_debug("_oci_close_server: closing DEAD server");
	}

	if (server->pServer) {
		CALL_OCI(
			OCIHandleFree(
				(dvoid *) server->pServer, 
				(ub4) OCI_HTYPE_SERVER
			)
		);
	}

	dbname = server->dbname;

	if (!OCI(shutdown)) {
		zend_ts_hash_del(persistent_servers, dbname, strlen(dbname)+1);
	}

	free(dbname);
}
/* }}} */

/* {{{ oci_do_connect()
   Connect to an Oracle database and log on. returns a new session. */
static void oci_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive)
{
	char *username, *password, *dbname, *charset;
	zval **userParam, **passParam, **dbParam, **charParam;
	oci_server *server = 0;
	oci_session *session = 0;
	oci_connection *connection = 0;
	
	/* if a forth parameter is handed over, it is the charset identifier (but is only used in Oracle 9i+) */
	if (zend_get_parameters_ex(4, &userParam, &passParam, &dbParam, &charParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);
		convert_to_string_ex(dbParam);
		convert_to_string_ex(charParam);

		username = Z_STRVAL_PP(userParam);
		password = Z_STRVAL_PP(passParam);
		dbname = Z_STRVAL_PP(dbParam);
		charset = Z_STRVAL_PP(charParam);
		oci_debug("oci_do_connect: using charset=%s",charset);
	} else if (zend_get_parameters_ex(3, &userParam, &passParam, &dbParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);
		convert_to_string_ex(dbParam);

		username = Z_STRVAL_PP(userParam);
		password = Z_STRVAL_PP(passParam);
		dbname = Z_STRVAL_PP(dbParam);
		charset = "";
	} else if (zend_get_parameters_ex(2, &userParam, &passParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);

		username = Z_STRVAL_PP(userParam);
		password = Z_STRVAL_PP(passParam);
		dbname = "";
		charset = "";
	} else {
		WRONG_PARAM_COUNT;
	}

	connection = (oci_connection *) ecalloc(1,sizeof(oci_connection));

	if (!connection) {
		goto CLEANUP;
	}

	server = _oci_open_server(dbname,persistent);

	if (!server) {
		goto CLEANUP;
	}

	if (exclusive) {
		/* exlusive session can never be persistent!*/
		persistent = 0;
	} else {
		/* if our server-context is not persistent we can't */
		persistent = (server->persistent) ? persistent : 0;
	}

	session = _oci_open_session(server,username,password,persistent,exclusive,charset);

	if (!session) {
		goto CLEANUP;
	}

	/* set our session */
	connection->session = session;

	/* allocate our private error-handle */
	CALL_OCI_RETURN(OCI(error),
		OCIHandleAlloc(
			connection->session->pEnv, 
			(dvoid **)&connection->pError, 
			OCI_HTYPE_ERROR, 
			0, 
			NULL
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_do_connect: OCIHandleAlloc OCI_HTYPE_ERROR",OCI(error));
		goto CLEANUP;
	}

	/* allocate our service-context */
	CALL_OCI_RETURN(OCI(error),
		OCIHandleAlloc(
			connection->session->pEnv, 
			(dvoid **)&connection->pServiceContext, 
			OCI_HTYPE_SVCCTX, 
			0, 
			NULL
		)
	);

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_do_connect: OCIHandleAlloc OCI_HTYPE_SVCCTX",OCI(error));
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	CALL_OCI_RETURN(connection->error,
		OCIAttrSet(
			connection->pServiceContext, 
			OCI_HTYPE_SVCCTX, 
			server->pServer, 
			0, 
			OCI_ATTR_SERVER, 
			connection->pError
		)
	);

	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "oci_do_connect: OCIAttrSet OCI_ATTR_SERVER", connection->error);
		goto CLEANUP;
	}

	/* Set the Authentication handle in the service handle */
	CALL_OCI_RETURN(connection->error,
		OCIAttrSet(
			connection->pServiceContext, 
			OCI_HTYPE_SVCCTX, 
			session->pSession, 
			0, 
			OCI_ATTR_SESSION, 
			connection->pError
		)
	);

	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "oci_do_connect: OCIAttrSet OCI_ATTR_SESSION", connection->error);
		goto CLEANUP;
	}

	/*
	OCIAttrSet((dvoid *)session->server->pServer, 
				OCI_HTYPE_SERVER,
				(dvoid *) "demo",
				0,
				OCI_ATTR_EXTERNAL_NAME,
				connection->pError);

	OCIAttrSet((dvoid *)session->server->pServer,
				OCI_HTYPE_SERVER,
				(dvoid *) "txn demo2",
				0,
				OCI_ATTR_INTERNAL_NAME,
				connection->pError);
	*/

	connection->id = zend_list_insert(connection, le_conn);

	connection->is_open = 1;

	oci_debug("oci_do_connect: id=%d",connection->id);

	RETURN_RESOURCE(connection->id);
	
CLEANUP:
	oci_debug("oci_do_connect: FAILURE -> CLEANUP called");

	if (connection->id) {
		zend_list_delete(connection->id);
	} else {
		_oci_conn_list_dtor(connection TSRMLS_CC);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ oci_lob_flush()
*/
static int oci_lob_flush(oci_descriptor* descr, int flush_flag TSRMLS_DC)
{
	OCILobLocator *mylob;
	oci_connection *connection;
	
	mylob = (OCILobLocator *) descr->ocidescr;

	if (!mylob) {
		return 0;
	}

	/* do not really flush buffer, but reporting success
	 * to suppress OCI error when flushing not used buffer
	 * */
	if (descr->buffering != 2) {
		return 1;
	}

	connection = descr->conn;

	CALL_OCI_RETURN(connection->error,
		OCILobFlushBuffer(
			connection->pServiceContext,
			connection->pError,
			mylob,
			flush_flag
		)
	);

	oci_debug("oci_lob_flush: flush_flag=%d",flush_flag);

	if (connection->error) {
		oci_error(connection->pError, "OCILobFlushBuffer", connection->error);
		oci_handle_error(connection, connection->error);
		return 0;
	}

	/* marking buffer as enabled and not used */
	descr->buffering = 1;
	return 1;
}
/* }}} */

/* {{{ php_oci_fetch_row()
*/
static void php_oci_fetch_row (INTERNAL_FUNCTION_PARAMETERS, int mode, int expected_args)
{
	zval **stmt, **arg2, **arg3;
	oci_statement *statement;
	oci_out_column *column;
	ub4 nrows = 1;
	int i;

	if (ZEND_NUM_ARGS() > expected_args) {
		WRONG_PARAM_COUNT;
	}

	if (expected_args > 2) {
		/* only for ocifetchinto BC */

		switch (ZEND_NUM_ARGS()) {
			case 2:
				if (zend_get_parameters_ex(2, &stmt, &arg2) == FAILURE) {
					RETURN_FALSE;
				}
				if (!mode) {
					mode = OCI_NUM;
				}
				break;

			case 3:
				if (zend_get_parameters_ex(3, &stmt, &arg2, &arg3) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_long_ex(arg3);
				mode = Z_LVAL_PP(arg3);
				break;

			default:
				WRONG_PARAM_COUNT;
				break;
		}

	} else {

		switch (ZEND_NUM_ARGS()) {
			case 1:
				if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
					RETURN_FALSE;
				}
				if (!mode) {
					mode = OCI_BOTH;
				}
				break;

			case 2:
				if (zend_get_parameters_ex(2, &stmt, &arg2)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_long_ex(arg2);
				mode = Z_LVAL_PP(arg2);
				break;
			
			default:
				WRONG_PARAM_COUNT;
				break;
		}
	}

	OCI_GET_STMT(statement,stmt);

	if (!oci_fetch(statement, nrows, "OCIFetchInto" TSRMLS_CC)) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < statement->ncolumns; i++) {
		column = oci_get_col(statement, i + 1, 0);
		if (column == NULL) {
			continue;
		}
		if ((column->indicator == -1) && ((mode & OCI_RETURN_NULLS) == 0)) {
			continue;
		}

		if (!(column->indicator == -1)) {
			zval *element;
			
			MAKE_STD_ZVAL(element);
			_oci_make_zval(element,statement,column,"OCIFetchInto",mode TSRMLS_CC);

			if (mode & OCI_NUM || !(mode & OCI_ASSOC)) {
				add_index_zval(return_value, i, element);
			}
			if (mode & OCI_ASSOC) {
				if (mode & OCI_NUM) {
					ZVAL_ADDREF(element);
				}
				add_assoc_zval(return_value, column->name, element);
			}
		
		} else {
			if (mode & OCI_NUM || !(mode & OCI_ASSOC)) {
				add_index_null(return_value, i);
			}
			if (mode & OCI_ASSOC) {
				add_assoc_null(return_value, column->name);
			}
		}
	}

	if (expected_args > 2) {
		/* only for ocifetchinto BC
		 * in all other cases we return array, not long
		 */
		REPLACE_ZVAL_VALUE(arg2, return_value, 1); /* copy return_value to given reference */
		zval_dtor(return_value);
		RETURN_LONG(statement->ncolumns);
	}
}
/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto bool oci_define_by_name(resource stmt, string name, mixed &var [, int type])
   Define a PHP variable to an Oracle column by name */
/* if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!! */
PHP_FUNCTION(oci_define_by_name)
{
	zval **stmt, **name, **var, **type;
	oci_statement *statement;
	oci_define *define, *tmp_define;
	ub2	ocitype = SQLT_CHR; /* zero terminated string */
	int ac = ZEND_NUM_ARGS();

	if (ac < 3 || ac > 4 || zend_get_parameters_ex(ac, &stmt, &name, &var, &type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (ac) {
		case 4:
			convert_to_long_ex(type);
			ocitype = (ub2) Z_LVAL_PP(type);
			/* possible breakthru */
	}

	OCI_GET_STMT(statement,stmt);

	convert_to_string_ex(name);

	if (statement->defines == NULL) {
		ALLOC_HASHTABLE(statement->defines);
		zend_hash_init(statement->defines, 13, NULL, _oci_define_hash_dtor, 0);
	}

	define = ecalloc(1,sizeof(oci_define));

	if (zend_hash_add(statement->defines,
					  Z_STRVAL_PP(name),
					  Z_STRLEN_PP(name),
					  define,
					  sizeof(oci_define),
					  (void **)&tmp_define) == SUCCESS) {
		efree(define);
		define = tmp_define;
	} else {
		efree(define);
		RETURN_FALSE;
	}

	define->name = (text*) estrndup(Z_STRVAL_PP(name),Z_STRLEN_PP(name));
	define->name_len = Z_STRLEN_PP(name);
	define->type = ocitype;
	define->zval = *var;
	zval_add_ref(var);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_bind_by_name(resource stmt, string name, mixed &var, [, int maxlength [, int type]])
   Bind a PHP variable to an Oracle placeholder by name */
/* if you want to bind a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE binding!!! */
PHP_FUNCTION(oci_bind_by_name)
{
	zval **stmt, **name, **var, **maxlen, **type;
	oci_statement *statement;
	oci_statement *bindstmt;
	oci_bind bind, *bindp;
	oci_descriptor *descr;
#ifdef PHP_OCI8_HAVE_COLLECTIONS
	oci_collection *coll;
	dvoid *mycoll = 0;
#endif
	int mode = OCI_DATA_AT_EXEC;
	ub2	ocitype = SQLT_CHR; /* unterminated string */
	OCIStmt *mystmt = 0;
	dvoid *mydescr = 0;
	sb4 value_sz = -1;
	int ac = ZEND_NUM_ARGS(), inx;

	if (ac < 3 || ac > 5 || zend_get_parameters_ex(ac, &stmt, &name, &var, &maxlen, &type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (ac) {
		case 5:
			convert_to_long_ex(type);
			ocitype = (ub2) Z_LVAL_PP(type);
			/* possible breakthru */
		case 4:
			convert_to_long_ex(maxlen);
			value_sz = Z_LVAL_PP(maxlen);
			/* possible breakthru */
	}

	OCI_GET_STMT(statement,stmt);

	switch (ocitype) {
#ifdef PHP_OCI8_HAVE_COLLECTIONS
		case SQLT_NTY:
			if (Z_TYPE_PP(var) != IS_OBJECT) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Variable must be allocated using OCINewCollection()");
				RETURN_FALSE;
			}
			if ((inx = _oci_get_ocicoll(*var,&coll TSRMLS_CC)) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Variable must be allocated using OCINewCollection()");
				RETURN_FALSE;
			}
			if (!(mycoll = (dvoid *) coll->coll)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Collection empty");
				RETURN_FALSE;
			}
			value_sz = sizeof(void*);
			mode = OCI_DEFAULT;
break;
#endif
		case SQLT_BFILEE:
		case SQLT_CFILEE:
		case SQLT_CLOB:
		case SQLT_BLOB:
		case SQLT_RDD:
			if (Z_TYPE_PP(var) != IS_OBJECT) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Variable must be allocated using OCINewDescriptor()");
				RETURN_FALSE;
			}

			if ((inx = _oci_get_ocidesc(*var,&descr TSRMLS_CC)) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Variable must be allocated using OCINewDescriptor()");
				RETURN_FALSE;
			}
		
			if (!(mydescr = (dvoid *) descr->ocidescr)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Descriptor empty");
				RETURN_FALSE;
			}
			value_sz = sizeof(void*);
			break;

		case SQLT_RSET:
			OCI_GET_STMT(bindstmt,var);

			if (!(mystmt = bindstmt->pStmt)) {
				RETURN_FALSE;
			}
			value_sz = sizeof(void*);
			break;
	}
	
	if ((ocitype == SQLT_CHR) && (value_sz == -1)) {
		convert_to_string_ex(var);
		value_sz = Z_STRLEN_PP(var);
	}

	if (value_sz == 0) { 
		value_sz = 1;
	}

	convert_to_string_ex(name);

	if (!statement->binds) {
		ALLOC_HASHTABLE(statement->binds);
		zend_hash_init(statement->binds, 13, NULL, _oci_bind_hash_dtor, 0);
	}

	memset((void*)&bind,0,sizeof(oci_bind));
	zend_hash_update(statement->binds, Z_STRVAL_PP(name), Z_STRLEN_PP(name) + 1, &bind, sizeof(oci_bind), (void **)&bindp);
	
	bindp->descr = mydescr;
	bindp->pStmt = mystmt;
	bindp->zval = *var;
	zval_add_ref(var); 
	
	CALL_OCI_RETURN(statement->error,
		OCIBindByName(
			statement->pStmt,                /* statement handle */
			(OCIBind **)&bindp->pBind,       /* bind hdl (will alloc) */
			statement->pError,               /* error handle */
			(text*) Z_STRVAL_PP(name),       /* placeholder name */					  
			Z_STRLEN_PP(name),               /* placeholder length */
			(dvoid *)0,                      /* in/out data */
			value_sz, /* OCI_MAX_DATA_SIZE, */ /* max size of input/output data */
			(ub2)ocitype,                    /* in/out data type */
			(dvoid *)&bindp->indicator,      /* indicator (ignored) */
			(ub2 *)0,                        /* size array (ignored) */
			(ub2 *)&bindp->retcode,          /* return code (ignored) */
			(ub4)0,                          /* maxarr_len (PL/SQL only?) */
			(ub4 *)0,                        /* actual array size (PL/SQL only?) */
			mode                             /* mode */
		)
	);

	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIBindByName", statement->error);
		oci_handle_error(statement->conn, statement->error);
		RETURN_FALSE;
	}

	if (mode == OCI_DATA_AT_EXEC) {
		CALL_OCI_RETURN(statement->error,
			OCIBindDynamic(
				bindp->pBind, 
				statement->pError, 
				(dvoid *)bindp, 
				oci_bind_in_callback, 
				(dvoid *)bindp, 
				oci_bind_out_callback
			)
		);

		if (statement->error != OCI_SUCCESS) {
			oci_error(statement->pError, "OCIBindDynamic", statement->error);
			oci_handle_error(statement->conn, statement->error);
			RETURN_FALSE;
		}
	}

#ifdef PHP_OCI8_HAVE_COLLECTIONS
	if (ocitype == SQLT_NTY) {
		/* Bind object */
		CALL_OCI_RETURN(statement->error,
			OCIBindObject(
				bindp->pBind, 
				statement->pError, 
				coll->tdo, 
				(dvoid **) &(coll->coll), 
				(ub4 *) 0, 
				(dvoid **) 0, 
				(ub4 *) 0
			)
		);

		if (statement->error) {
			oci_error(statement->pError, "OCIBindObject", statement->error);
			RETURN_FALSE;
		}
	}
#endif
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_free_descriptor()
   Deletes large object description */
PHP_FUNCTION(oci_free_descriptor)
{
	zval *id;
	int inx;
	oci_descriptor *descriptor;

	if ((id = getThis()) != 0) {
		inx = _oci_get_ocidesc(id,&descriptor TSRMLS_CC);
		if (inx) {
			oci_debug("oci_free_descriptor: descr=%d",inx);
			zend_list_delete(inx);
			RETURN_TRUE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_free_descriptor() should not be called like this. Use $somelob->free() to free a LOB");

  	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_save( string data [, int offset ])
   Saves a large object */
PHP_FUNCTION(oci_lob_save)
{
	zval *id, **arg,**oarg;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int offparam,inx;
	ub4 loblen;
	ub4 curloblen;
	ub4 offset;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		offset = 0;	
		if (zend_get_parameters_ex(2, &arg, &oarg) == SUCCESS) {
			convert_to_long_ex(oarg);
			offparam = Z_LVAL_PP(oarg);

			CALL_OCI_RETURN(connection->error,
				OCILobGetLength(
					connection->pServiceContext, 
					connection->pError, 
					mylob, 
					&curloblen
				)
			);

			oci_debug("oci_lob_save: curloblen=%d",curloblen);

			if (offparam == -1) {
				offset = curloblen;
			} else if ((ub4)offparam >= curloblen) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is bigger than current LOB-Size - appending");
				offset = curloblen;
			} else {
				offset = (ub4)offparam;
			}
		} else if (zend_get_parameters_ex(1, &arg) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_string_ex(arg);
		loblen = Z_STRLEN_PP(arg);
	
		if (loblen < 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot save a lob which size is less than 1 byte");
			RETURN_FALSE;
		}

		if (offset <= 0) {
			offset = 1;
		}
		
		CALL_OCI_RETURN(connection->error,
			OCILobWrite(
				connection->pServiceContext, 
				connection->pError,
				mylob,
				&loblen,
				(ub4) offset,
				(dvoid *) Z_STRVAL_PP(arg),
				(ub4) loblen,
				OCI_ONE_PIECE,
				(dvoid *)0,
				(OCICallbackLobWrite) 0,
				(ub2) 0,
				(ub1) SQLCS_IMPLICIT
			)
		);

		oci_debug("oci_lob_save: size=%d offset=%d",loblen,offset);

		if (connection->error) {
			oci_error(connection->pError, "OCILobWrite", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_import( string filename )
   Saves a large object to file */
PHP_FUNCTION(oci_lob_import)
{
	zval *id, **arg;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	char *filename;
	int fp,inx;
	char buf[8192];
	ub4 offset = 1;
	ub4 loblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (zend_get_parameters_ex(1, &arg) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_string_ex(arg);

		if (php_check_open_basedir(Z_STRVAL_PP(arg) TSRMLS_CC)) {
			RETURN_FALSE;
		}

		filename = Z_STRVAL_PP(arg);

		if ((fp = VCWD_OPEN(filename, O_RDONLY|O_BINARY)) == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't open file %s", filename);
			RETURN_FALSE;
		}

		while ((loblen = read(fp, &buf, sizeof(buf))) > 0) {	
			CALL_OCI_RETURN(connection->error,
				OCILobWrite(
					connection->pServiceContext, 
					connection->pError, 
					mylob, 
					&loblen, 
					(ub4) offset, 
					(dvoid *) &buf, 
					(ub4) loblen, 
					OCI_ONE_PIECE, 
					(dvoid *)0, 
					(OCICallbackLobWrite) 0, 
					(ub2) 0, 
					(ub1) SQLCS_IMPLICIT
				)
			);

			oci_debug("oci_lob_import: size=%d",loblen);

			if (connection->error) {
				oci_error(connection->pError, "OCILobWrite", connection->error);
				oci_handle_error(connection, connection->error);
				close(fp);
				RETURN_FALSE;
			}

			offset += loblen;
		}
		close(fp);

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}
/* }}} */

/* {{{ proto string oci_lob_load()
   Loads a large object */
PHP_FUNCTION(oci_lob_load)
{
	zval *id;
	oci_descriptor *descr;
	char *buffer;
	int inx;
	ub4 loblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		if (!oci_loadlob(descr->conn,descr,&buffer,&loblen)) {
			RETURN_STRINGL(buffer,loblen,0);
		} else {
			RETURN_FALSE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_load() should not be called like this. Use $somelob->load() to load a LOB");

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string oci_lob_read( int length )
   Reads particular part of a large object */
PHP_FUNCTION(oci_lob_read)
{
	zval *id;
	zval **len;
	oci_descriptor *descr;
	char *buffer;
	int inx;
	ub4 loblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		if (zend_get_parameters_ex(1, &len) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		loblen = Z_LVAL_PP(len);
		if (oci_readlob(descr->conn,descr,&buffer,&loblen) == 0) {
			RETURN_STRINGL(buffer,loblen,0);
		} else {
			RETURN_FALSE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_read() should not be called like this. Use $somelob->read($len) to read a LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_eof()
   Checks if EOF is reached */
PHP_FUNCTION(oci_lob_eof)
{
	zval *id;
	oci_descriptor *descr;
	int inx;
	int len;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		if (oci_lobgetlen(descr->conn,descr,&len) == 0 && descr->lob_size >= 0) {
			if (descr->lob_size == descr->lob_current_position) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
		}
		RETURN_FALSE;
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_eof() should not be called like this. Use $somelob->eof() to check if end of LOB is reached");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_lob_tell()
   Tells LOB pointer position */
PHP_FUNCTION(oci_lob_tell)
{
	zval *id;
	oci_descriptor *descr;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		RETURN_LONG(descr->lob_current_position);	
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_tell() should not be called like this. Use $somelob->tell() to get current position of LOB pointer");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_rewind()
   Rewind pointer of a LOB */
PHP_FUNCTION(oci_lob_rewind)
{
	zval *id;
	oci_descriptor *descr;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		descr->lob_current_position = 0;
		RETURN_TRUE;
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_rewind() should not be called like this. Use $somelob->rewind() to set current position of LOB pointer to beginning");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_seek( int offset [, int whence ])
   Moves the pointer of a LOB */
PHP_FUNCTION(oci_lob_seek)
{
	zval *id;
	zval **arg1, **arg2;
	int argcount = ZEND_NUM_ARGS(), whence = OCI_SEEK_SET;
	oci_descriptor *descr;
	int inx, len;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		if (argcount < 1 || argcount > 2 || zend_get_parameters_ex(argcount, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		
		convert_to_long_ex(arg1);
		
		if (oci_lobgetlen(descr->conn,descr,&len) == 0 && descr->lob_size >= 0) {
			if (argcount > 1) {
				convert_to_long_ex(arg2);
				whence = Z_LVAL_PP(arg2);
				switch (whence) {
					case OCI_SEEK_CUR:
						descr->lob_current_position += Z_LVAL_PP(arg1);
						break;
					
					case OCI_SEEK_END:
						if (descr->lob_size + Z_LVAL_PP(arg1) >= 0) {
							descr->lob_current_position = descr->lob_size + Z_LVAL_PP(arg1);
						} else {
							descr->lob_current_position = 0;
						}
						break;
					
					case OCI_SEEK_SET:
					default:
						descr->lob_current_position = Z_LVAL_PP(arg1);
						break;
				}
			} else {
				/* OCI_SEEK_SET by default */
				descr->lob_current_position = Z_LVAL_PP(arg1);
			}
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_seek() should not be called like this. Use $somelob->seek($offset) to move pointer");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_lob_size()
   Returns size of a large object */
PHP_FUNCTION(oci_lob_size)
{
	zval *id;
	oci_descriptor *descr;
	int inx;
	ub4 loblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		if (!oci_lobgetlen(descr->conn,descr,&loblen)) {
	 		RETURN_LONG(loblen);
		} else {
			RETURN_FALSE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_size() should not be called like this. Use $somelob->size() to get size of a LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_lob_write( string string [, int length ])
   Writes data to current position of a LOB */
PHP_FUNCTION(oci_lob_write)
{
	zval *id, **data,**length;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int write_length,inx;
	ub4 loblen;
	ub4 curloblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		IS_LOB_INTERNAL(descr);
			
		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (oci_lobgetlen(descr->conn,descr,&curloblen) != 0) {
			RETURN_FALSE;
		}
		
		if (zend_get_parameters_ex(2, &data, &length) == SUCCESS) {
			convert_to_string_ex(data);
			convert_to_long_ex(length);
			write_length = Z_LVAL_PP(length);
		} else if (zend_get_parameters_ex(1, &data) == SUCCESS) {
			convert_to_string_ex(data);	
			write_length = Z_STRLEN_PP(data);
		} else {
			WRONG_PARAM_COUNT;
		}

		if (write_length < 1) {
			RETURN_LONG(0);
		}
		
		loblen = write_length;
		
		CALL_OCI_RETURN(connection->error,
			OCILobWrite(
				connection->pServiceContext, 
				connection->pError,
				mylob,
				&loblen,
				(ub4) descr->lob_current_position+1,
				(dvoid *) Z_STRVAL_PP(data),
				(ub4) loblen,
				OCI_ONE_PIECE,
				(dvoid *)0,
				(OCICallbackLobWrite) 0,
				(ub2) 0,
				(ub1) SQLCS_IMPLICIT
			)
		);

		oci_debug("oci_lob_write: size=%d offset=%d",loblen,descr->lob_current_position);

		if (connection->error) {
			oci_error(connection->pError, "OCILobWrite", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

		descr->lob_current_position += loblen;
	
		if (descr->lob_current_position > descr->lob_size) {
			descr->lob_size = descr->lob_current_position;
		}
		
		/* marking buffer as used */
		if (descr->buffering == 1) {
			descr->buffering = 2;
		}
		RETURN_LONG(loblen);
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_write() should not be called like this. Use $somelob->write($data,$len) to write to a LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_append( object lob )
   Appends data from a LOB to another LOB */
PHP_FUNCTION(oci_lob_append)
{
	zval *id, **arg;
	OCILobLocator *mylob,*my_fromlob;
	oci_connection *connection;
	oci_descriptor *descr,*from_descr;
	int inx;
	ub4 curloblen,from_curloblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
	
		IS_LOB_INTERNAL(descr);
		
		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (oci_lobgetlen(descr->conn,descr,&curloblen) != 0) {
			RETURN_FALSE;
		}
		
		if (zend_get_parameters_ex(1, &arg) == SUCCESS) {
			convert_to_object_ex(arg);
			if ((inx = _oci_get_ocidesc(*arg,&from_descr TSRMLS_CC)) == 0) {
				RETURN_FALSE;
			}
			
			my_fromlob = (OCILobLocator *) from_descr->ocidescr;

			if (!my_fromlob) {
				RETURN_FALSE;
			}
			
			if (oci_lobgetlen(from_descr->conn,from_descr,&from_curloblen) != 0) {
				RETURN_FALSE;
			}
		} else {
			WRONG_PARAM_COUNT;
		}

		if (from_descr->lob_size == 0) {
			RETURN_LONG(0);
		}

		CALL_OCI_RETURN(connection->error,
			OCILobAppend(
				connection->pServiceContext, 
				connection->pError,
				mylob,
				my_fromlob
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobAppend", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

	 	RETURN_TRUE;
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_append() should not be called like this. Use $somelob->append($LOB_from) to append data from a LOB to another LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_truncate( [ int length ])
   Truncates a LOB */
PHP_FUNCTION(oci_lob_truncate)
{
	zval *id, **length;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int inx;
	ub4 trim_length;
	ub4 curloblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		IS_LOB_INTERNAL(descr);

		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (oci_lobgetlen(descr->conn,descr,&curloblen) != 0) {
			RETURN_FALSE;
		}
		
		if (zend_get_parameters_ex(1, &length) == SUCCESS) {
			convert_to_long_ex(length);	
			trim_length = Z_LVAL_PP(length);
		} else {
			trim_length = 0;
		}

		if (trim_length < 0) {
			/* negative length is not allowed */
			RETURN_FALSE;
		}
		
		CALL_OCI_RETURN(connection->error,
			OCILobTrim(
				connection->pServiceContext, 
				connection->pError,
				mylob,
				trim_length
			)
		);

		oci_debug("oci_lob_truncate: trim_length=%d",trim_length);

		if (connection->error) {
			oci_error(connection->pError, "OCILobTrim", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

		descr->lob_size = trim_length;
		RETURN_TRUE;
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_truncate() should not be called like this. Use $somelob->truncate($length) to truncate a LOB to a specified length");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_lob_erase( [ int offset [, int length ] ] )
   Erases a specified portion of the internal LOB, starting at a specified offset */
PHP_FUNCTION(oci_lob_erase)
{
	zval *id, **length, **offset;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int inx;
	ub4 erase_length, erase_offset;
	ub4 curloblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		IS_LOB_INTERNAL(descr);

		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (oci_lobgetlen(descr->conn,descr,&curloblen) != 0) {
			RETURN_FALSE;
		}
		
		if (zend_get_parameters_ex(2, &offset, &length) == SUCCESS) {
			convert_to_long_ex(offset);
			convert_to_long_ex(length);	
			
			erase_offset = Z_LVAL_PP(offset);
			erase_length = Z_LVAL_PP(length);
		} else if (zend_get_parameters_ex(1, &offset) == SUCCESS) {
			convert_to_long_ex(offset);

			erase_offset = Z_LVAL_PP(offset);
			erase_length = descr->lob_size - erase_offset;
		} else {
			erase_offset = 0;
			erase_length = descr->lob_size;
		}

		if (erase_length < 1) {
			RETURN_LONG(0);
		}

		if (erase_offset > descr->lob_size) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "oci_lob_erase(): offset is greater than LOB's length");
		}
		
		CALL_OCI_RETURN(connection->error,
			OCILobErase(
				connection->pServiceContext, 
				connection->pError,
				mylob,
				&erase_length,
				erase_offset+1
			)
		);

		oci_debug("oci_lob_erase: erase_length=%d, erase_offset=%d",erase_length,erase_offset);

		if (connection->error) {
			oci_error(connection->pError, "OCILobErase", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

		RETURN_LONG(erase_length);
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_erase() should not be called like this. Use $somelob->erase($offset, $length) to erase specified part of LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_flush( [ int flag ] )
   Flushes the LOB buffer */
PHP_FUNCTION(oci_lob_flush)
{
	zval *id, **flag;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int inx, flush_flag;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		IS_LOB_INTERNAL(descr);
			
		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (zend_get_parameters_ex(1, &flag) == SUCCESS) {
			convert_to_long_ex(flag);	
			flush_flag = Z_LVAL_PP(flag);
		} else {
			flush_flag = 0;
		}

		if (descr->buffering == 0) {
			/* buffering wasn't enabled, there is nothing to flush */
			RETURN_FALSE;
		}

		if (oci_lob_flush(descr,flush_flag TSRMLS_CC) == 1) {
			RETURN_TRUE;
		}
		RETURN_FALSE;
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_flush() should not be called like this. Use $somelob->flush() to flush LOB buffer");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocisetbufferinglob( boolean flag )
   Enables/disables buffering for a LOB */
PHP_FUNCTION(ocisetbufferinglob)
{
	zval *id, **flag;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int inx, buffering_flag;
	ub4 curloblen;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		IS_LOB_INTERNAL(descr);
			
		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (oci_lobgetlen(descr->conn,descr,&curloblen) != 0) {
			RETURN_FALSE;
		}
		
		if (zend_get_parameters_ex(1, &flag) == SUCCESS) {
			convert_to_boolean_ex(flag);
			buffering_flag = Z_LVAL_PP(flag);
		} else {
			WRONG_PARAM_COUNT;
		}

		/* we'll return true if function was called twice with the same parameter */
		if (buffering_flag == 0 && descr->buffering == 0) {
			RETURN_TRUE;
		} else if (buffering_flag == 1 && descr->buffering > 0) {
			RETURN_TRUE;
		}
		
		switch (buffering_flag) {
			case 0:
				CALL_OCI_RETURN(connection->error,
					OCILobDisableBuffering(
						connection->pServiceContext, 
						connection->pError,
						mylob
					)
				);
				break;
			case 1:
				CALL_OCI_RETURN(connection->error, 
					OCILobEnableBuffering(
						connection->pServiceContext,
						connection->pError,
						mylob
					)
				);
			break;
		}
		
		oci_debug("oci_lob_set_buffering: buffering_flag=%d",buffering_flag);

		if (connection->error) {
			oci_error(connection->pError, "OCILobFlushBuffer", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

		descr->buffering = buffering_flag;
		RETURN_TRUE;
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "OCISetBufferingLob() should not be called like this. Use $somelob->setBuffering($flag) to set buffering on/off for a LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocigetbufferinglob()
   Returns current state of buffering for a LOB */
PHP_FUNCTION(ocigetbufferinglob)
{
	zval *id;
	OCILobLocator *mylob;
	oci_descriptor *descr;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		IS_LOB_INTERNAL(descr);
			
		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		switch (descr->buffering) {
			case 1:
			case 2:
				RETURN_TRUE;
				break;

			case 0:
			default:
				RETURN_FALSE;
				break;
		}
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "OCIGetBufferingLob() should not be called like this. Use $somelob->getBuffering() to get current state of buffering for a LOB");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_lob_copy( object lob_to, object lob_from [, int length ] )
   Copies data from a LOB to another LOB */
PHP_FUNCTION(oci_lob_copy)
{
	zval **arg1, **arg2, **arg3;
	OCILobLocator *mylob,*my_fromlob;
	oci_connection *connection;
	oci_descriptor *descr,*from_descr;
	int inx, ac = ZEND_NUM_ARGS();
	ub4 curloblen,from_curloblen, copylen;

		if (ac < 2 || ac > 3 || zend_get_parameters_ex(ac, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_object_ex(arg1);
		convert_to_object_ex(arg2);
		
		if ((inx = _oci_get_ocidesc(*arg1,&descr TSRMLS_CC)) == 0 || (inx = _oci_get_ocidesc(*arg2,&from_descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		IS_LOB_INTERNAL(descr);
		IS_LOB_INTERNAL(from_descr);

		mylob = (OCILobLocator *) descr->ocidescr;
		my_fromlob = (OCILobLocator *) from_descr->ocidescr;
			
		if (!mylob || !my_fromlob) {
			RETURN_FALSE;
		}
			
		if (oci_lobgetlen(descr->conn,descr,&curloblen) != 0 || oci_lobgetlen(from_descr->conn,from_descr,&from_curloblen) != 0) {
			RETURN_FALSE;
		}

		if (ac == 3) {
			convert_to_long_ex(arg3);
			copylen = Z_LVAL_PP(arg3);
		} else {
			copylen = from_descr->lob_size - from_descr->lob_current_position;
		}

		if ((int)copylen <= 0) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		CALL_OCI_RETURN(connection->error,
			OCILobCopy(
				connection->pServiceContext, 
				connection->pError,
				mylob,
				my_fromlob,
				copylen,
				descr->lob_current_position+1,
				from_descr->lob_current_position+1
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobCopy", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

	 	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_is_equal( object lob1, object lob2 )
   Tests to see if two LOB/FILE locators are equal */
PHP_FUNCTION(oci_lob_is_equal)
{
	zval **arg1, **arg2;
	OCILobLocator *first_lob,*second_lob;
	oci_connection *connection;
	oci_descriptor *first_descr,*second_descr;
	int inx;
	boolean is_equal;

		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_object_ex(arg1);
		convert_to_object_ex(arg2);
		
		if ((inx = _oci_get_ocidesc(*arg1,&first_descr TSRMLS_CC)) == 0 || (inx = _oci_get_ocidesc(*arg2,&second_descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		
		first_lob = (OCILobLocator *) first_descr->ocidescr;
		second_lob = (OCILobLocator *) second_descr->ocidescr;
			
		if (!first_lob || !second_lob) {
			RETURN_FALSE;
		}
			
		connection = first_descr->conn;

		CALL_OCI_RETURN(connection->error,
			OCILobIsEqual(
				connection->session->pEnv, 
				first_lob,
				second_lob,
				&is_equal
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobIsEqual", connection->error);
			oci_handle_error(connection, connection->error);
			RETURN_FALSE;
		}

		if (is_equal == TRUE) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
}
/* }}} */

/* {{{ proto bool oci_lob_export([string filename [, int start [, int length]]])
   Writes a large object into a file */
PHP_FUNCTION(oci_lob_export)
{
	zval *id, **zfilename, **zstart, **zlength;
	char *filename = NULL;
	int start = -1;
	ub4 length = -1;
	oci_connection *connection;
	oci_descriptor *descr;
	char *buffer=0;
	ub4 loblen;
	int ac = ZEND_NUM_ARGS();
	int fp = -1,inx;
	OCILobLocator *mylob;
	int coffs;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocidesc(id,&descr TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		mylob = (OCILobLocator *) descr->ocidescr;

		if (!mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		if (ac < 0 || ac > 3 || zend_get_parameters_ex(ac, &zfilename, &zstart, &zlength) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		switch (ac) {
		case 3:
			convert_to_long_ex(zlength);
			length = Z_LVAL_PP(zlength);
		case 2:
			convert_to_long_ex(zstart);
			start = Z_LVAL_PP(zstart);
		case 1:
			convert_to_string_ex(zfilename);
			filename = Z_STRVAL_PP(zfilename);
		}

		if (filename && *filename) {
			if (php_check_open_basedir(filename TSRMLS_CC)) {
				goto bail;
			}

			if ((fp = VCWD_OPEN_MODE(filename,O_CREAT | O_RDWR | O_BINARY | O_TRUNC, 0600)) == -1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't create file %s", filename);
				goto bail;
			} 
		}
	
		CALL_OCI_RETURN(connection->error,
			OCILobGetLength(
				connection->pServiceContext, 
				connection->pError, 
				descr->ocidescr, 
				&loblen
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCILobGetLength", connection->error);
			oci_handle_error(connection, connection->error);
			goto bail;
		}
		
		if (descr->type == OCI_DTYPE_FILE) {
			CALL_OCI_RETURN(connection->error,
				OCILobFileOpen(
					connection->pServiceContext, 
					connection->pError, 
					descr->ocidescr, 
					OCI_FILE_READONLY
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCILobFileOpen",connection->error);
				oci_handle_error(connection, connection->error);
				goto bail;
			}
		}

		if (start == -1) {
			start = 0;
		}

		if (length == -1) {
			length = loblen - start;
		}
		
		if ((start + length) > loblen) {
			length = loblen - start;
		}

#define OCI_LOB_READ_BUFFER 128*1024

		buffer = emalloc(OCI_LOB_READ_BUFFER);

		coffs = start;

		oci_debug("oci_lob_export(start = %d, length = %d, loblen = %d",start,length,loblen);

		while (length > 0) {
			ub4 toread;

			if (length > OCI_LOB_READ_BUFFER) {
				toread = OCI_LOB_READ_BUFFER;
			} else {
				toread = length;
			}

			oci_debug("oci_lob_read(coffs = %d, toread = %d",coffs,toread);

			CALL_OCI_RETURN(connection->error,
				OCILobRead(
					connection->pServiceContext, 
					connection->pError,
					descr->ocidescr,
					&toread,                /* IN/OUT bytes toread/read */
					coffs+1,                /* offset (starts with 1) */ 
					(dvoid *) buffer,
					toread,                 /* size of buffer */
					(dvoid *)0,
					(OCICallbackLobRead) 0, /* callback... */
					(ub2) 0,                /* The character set ID of the buffer data. */
					(ub1) SQLCS_IMPLICIT	/* The character set form of the buffer data. */
				)
			);

			oci_debug("oci_lob_read(read - %d",toread);

			if (connection->error) {
				oci_error(connection->pError, "OCILobRead", connection->error);
				oci_handle_error(connection, connection->error);
				goto bail;
			}

			if (fp != -1) {
				if ((ub4) write(fp,buffer,toread) != toread) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot write file!");
					goto bail;
				}
			} else {
				PHPWRITE(buffer,toread);
			}

			length -= toread;
			coffs += toread;
		}

		efree(buffer);
		buffer = 0;
		
		if (fp != -1) {
			close(fp);
			fp = 0;
		}

		if (descr->type == OCI_DTYPE_FILE) {
			CALL_OCI_RETURN(connection->error,
				OCILobFileClose(
					connection->pServiceContext, 
					connection->pError, 
					descr->ocidescr
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCILobFileClose", connection->error);
				oci_handle_error(connection, connection->error);
				goto bail;
			}
		}
		RETURN_TRUE;
	}
	
bail:
	if (fp != -1) {
		close(fp);
	}
	
	if (buffer) {
		efree(buffer);
	}

	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_OCI8_TEMP_LOB
/* {{{ proto bool oci_lob_write_temporary(string var [, int lob_type])
   Writes temporary blob */
PHP_FUNCTION(oci_lob_write_temporary)
{
	zval *id, *var;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	ub4 offset = 1;
	ub4 loblen;
	long lob_type = OCI_TEMP_CLOB;

	oci_debug ("oci_write_temporary_lob");

	if ((id = getThis()) == 0) {
		RETURN_FALSE;
	}

	if (_oci_get_ocidesc(id,&descr TSRMLS_CC) == 0) {
		RETURN_FALSE;
	}

	mylob = (OCILobLocator *) descr->ocidescr;

	if (!mylob) {
		RETURN_FALSE;
	}

	connection = descr->conn;

	if (ZEND_NUM_ARGS() < 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &var, &lob_type) == FAILURE) {
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error,
		OCILobCreateTemporary(
			connection->pServiceContext, 
			connection->pError, 
			mylob, 
			OCI_DEFAULT, 
			OCI_DEFAULT, 
			lob_type, 
			OCI_ATTR_NOCACHE, 
			OCI_DURATION_SESSION
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCILobCreateTemporary", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error,
		OCILobOpen(
			connection->pServiceContext, 
			connection->pError, 
			mylob, 
			OCI_LOB_READWRITE
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCILobOpen", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	convert_to_string_ex(&var);
	loblen = Z_STRLEN_P(var);
	
	if (loblen < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot save a lob that is less than 1 byte");
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error,
		OCILobWrite(
			connection->pServiceContext, 
			connection->pError, 
			mylob, 
			(ub4 *) &loblen, 
			(ub4) offset, 
			(dvoid *) Z_STRVAL_P(var), 
			(ub4) loblen, 
			OCI_ONE_PIECE, 
			(dvoid *)0, 
			(sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0, 
			(ub2) 0, 
			(ub1) SQLCS_IMPLICIT
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCILobWrite", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_lob_close()
   Closes lob descriptor */
PHP_FUNCTION(oci_lob_close)
{
	zval *id;
	int inx;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descriptor;
	int is_temporary;

	if ((id = getThis()) != 0) {
		inx = _oci_get_ocidesc(id,&descriptor TSRMLS_CC);
		if (inx) {

			mylob = (OCILobLocator *) descriptor->ocidescr;

			if (!mylob) {
				RETURN_FALSE;
			}

			connection = descriptor->conn;

			CALL_OCI_RETURN(connection->error,
				OCILobClose(
					connection->pServiceContext, 
					connection->pError, 
					mylob
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCILobClose", connection->error);
				oci_handle_error(connection, connection->error);
				RETURN_FALSE;
			}

			connection->error = 
				OCILobIsTemporary(connection->session->pEnv,
									connection->pError,
									mylob,
									&is_temporary);
			if (is_temporary) {
				connection->error = 
					OCILobFreeTemporary(connection->pServiceContext,
										connection->pError,
										mylob);

				if (connection->error) {
					oci_error(connection->pError, "OCILobFreeTemporary", 
							  connection->error);
					oci_handle_error(connection, connection->error);
					RETURN_FALSE;
				}
				oci_debug("oci_lob_free_temporary: descr=%d",inx);
			}

			oci_debug("oci_close_lob: descr=%d",inx);
			RETURN_TRUE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_lob_close() should not be called like this. Use $somelob->close() to close a LOB");

	RETURN_FALSE;
}
/* }}} */
#endif 

/* {{{ proto object oci_new_descriptor(resource connection [, int type])
   Initialize a new empty descriptor LOB/FILE (LOB is default) */
PHP_FUNCTION(oci_new_descriptor)
{
	zval **conn, **type;
	oci_connection *connection;
	oci_descriptor *descr;
	int dtype;

	dtype = OCI_DTYPE_LOB;

	if (zend_get_parameters_ex(2, &conn, &type) == SUCCESS) {
		convert_to_long_ex(type);
		dtype = Z_LVAL_PP(type);
	} else if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	descr = oci_new_desc(dtype,connection);

	if (!descr) {
		RETURN_NULL();
	}

	object_init_ex(return_value, oci_lob_class_entry_ptr);
	add_property_resource(return_value, "descriptor", descr->id);
}
/* }}} */

/* {{{ proto bool oci_rollback(resource conn)
   Rollback the current context */
PHP_FUNCTION(oci_rollback)
{
	zval **conn;
	oci_connection *connection;

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	if (connection->descriptors) {
		zend_hash_apply(connection->descriptors,(apply_func_t) _oci_desc_flush_hash_dtor TSRMLS_CC);
	}

	oci_debug("<OCITransRollback");

	CALL_OCI_RETURN(connection->error,
		OCITransRollback(
			connection->pServiceContext, 
			connection->pError, 
			(ub4) 0
		)
	);

	connection->needs_commit = 0;

	oci_debug(">OCITransRollback");

	if (connection->error) {
		oci_error(connection->pError, "OCIRollback", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_commit(resource conn)
   Commit the current context */
PHP_FUNCTION(oci_commit)
{
	zval **conn;
	oci_connection *connection;

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	if (connection->descriptors) {
		zend_hash_apply(connection->descriptors,(apply_func_t) _oci_desc_flush_hash_dtor TSRMLS_CC);
	}

	oci_debug("<OCITransCommit");

	CALL_OCI_RETURN(connection->error,
		OCITransCommit(
			connection->pServiceContext, 
			connection->pError, 
			(ub4) 0
		)
	);

	connection->needs_commit = 0;

	oci_debug(">OCITransCommit");

	if (connection->error) {
		oci_error(connection->pError, "OCICommit", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string oci_field_name(resource stmt, int col)
   Tell the name of a column */
PHP_FUNCTION(oci_field_name)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(outcol->name, outcol->name_len, 1);
}
/* }}} */

/* {{{ proto int oci_field_size(resource stmt, int col)
   Tell the maximum data size of a column */
PHP_FUNCTION(oci_field_size)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}

	oci_debug("oci_field_size: %16s, retlen = %4d, retlen4 = %d, data_size = %4d, storage_size4 = %4d, indicator %4d, retcode = %4d",
				  outcol->name,outcol->retlen,outcol->retlen4,outcol->data_size,outcol->storage_size4,outcol->indicator,outcol->retcode);
		
	/* Handle data type of LONG */
	if (outcol->data_type == SQLT_LNG){
		RETURN_LONG(outcol->storage_size4);
	} else {
		RETURN_LONG(outcol->data_size);
	}
}
/* }}} */

/* {{{ proto int oci_field_scale(resource stmt, int col)
   Tell the scale of a column */
PHP_FUNCTION(oci_field_scale)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(outcol->scale);
}
/* }}} */

/* {{{ proto int oci_field_precision(resource stmt, int col)
   Tell the precision of a column */
PHP_FUNCTION(oci_field_precision)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(outcol->precision);
}
/* }}} */

/* {{{ proto mixed oci_field_type(resource stmt, int col)
   Tell the data type of a column */
PHP_FUNCTION(oci_field_type)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;
	
	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	switch (outcol->data_type) {
#ifdef SQLT_TIMESTAMP
		case SQLT_TIMESTAMP:
			RETVAL_STRING("TIMESTAMP",1);
			break;
#endif
#ifdef SQLT_TIMESTAMP_TZ
		case SQLT_TIMESTAMP_TZ:
			RETVAL_STRING("TIMESTAMP_TZ",1);
			break;
#endif
		case SQLT_DAT:
			RETVAL_STRING("DATE",1);
			break;
		case SQLT_NUM:
			RETVAL_STRING("NUMBER",1);
			break;
		case SQLT_LNG:
			RETVAL_STRING("LONG",1);
			break;
		case SQLT_BIN:
			RETVAL_STRING("RAW",1);
			break;
		case SQLT_LBI:
			RETVAL_STRING("LONG RAW",1);
			break;
		case SQLT_CHR:
			RETVAL_STRING("VARCHAR",1);
			break;
		case SQLT_RSET:
			RETVAL_STRING("REFCURSOR",1);
			break;
		case SQLT_AFC:
			RETVAL_STRING("CHAR",1);
			break;
		case SQLT_BLOB:
			RETVAL_STRING("BLOB",1);
			break;
		case SQLT_CLOB:
			RETVAL_STRING("CLOB",1);
			break;
		case SQLT_BFILE:
			RETVAL_STRING("BFILE",1);
			break;
		case SQLT_RDD:
			RETVAL_STRING("ROWID",1);
			break;
		default:
			RETVAL_LONG(outcol->data_type);
	}
}
/* }}} */

/* {{{ proto int oci_field_type_raw(resource stmt, int col)
   Tell the raw oracle data type of a column */
PHP_FUNCTION(oci_field_type_raw)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;
	
	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG(outcol->data_type);
}
/* }}} */

/* {{{ proto bool oci_field_is_null(resource stmt, int col)
   Tell whether a column is NULL */
PHP_FUNCTION(oci_field_is_null)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	if (outcol->indicator == -1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void oci_internal_debug(int onoff)
   Toggle internal debugging output for the OCI extension */
PHP_FUNCTION(oci_internal_debug)
{
	zval **arg;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg);
	OCI(debug_mode) = Z_LVAL_PP(arg);
}
/* }}} */

/* {{{ proto bool oci_execute(resource stmt [, int mode])
   Execute a parsed statement */
PHP_FUNCTION(oci_execute)
{
	zval **stmt,**mode;
	oci_statement *statement;
	ub4 execmode;

	if (zend_get_parameters_ex(2, &stmt, &mode) == SUCCESS) {
		convert_to_long_ex(mode);
		execmode = Z_LVAL_PP(mode);
	} else if (zend_get_parameters_ex(1, &stmt) == SUCCESS) {
		execmode = OCI_COMMIT_ON_SUCCESS;
	} else {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	if (oci_execute(statement, "OCIExecute",execmode)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool oci_cancel(resource stmt)
   Cancel reading from a cursor */
PHP_FUNCTION(oci_cancel)
{
	zval **stmt;
	oci_statement *statement;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	if (oci_fetch(statement, 0, "OCICancel" TSRMLS_CC)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool oci_fetch(resource stmt)
   Prepare a new row of data for reading */
PHP_FUNCTION(oci_fetch)
{
	zval **stmt;
	oci_statement *statement;
	ub4 nrows = 1; /* only one row at a time is supported for now */

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	if (oci_fetch(statement, nrows, "OCIFetch" TSRMLS_CC)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int ocifetchinto(resource stmt, array &output [, int mode])
   Fetch a row of result data into an array */
PHP_FUNCTION(ocifetchinto)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 3);
}
/* }}} */

/* {{{ proto int oci_fetch_all(resource stmt, array &output[, int skip[, int maxrows[, int flags]]])
   Fetch all rows of result data into an array */
PHP_FUNCTION(oci_fetch_all)
{
	zval **stmt, **array, *element, **zskip, **zmaxrows, **zflags, *tmp;
	oci_statement *statement;
	oci_out_column **columns;
	zval ***outarrs;
	ub4 nrows = 1;
	int i;
	int skip = 0, maxrows = -1;
	int flags = 0;
	int rows = 0;
	int ac = ZEND_NUM_ARGS();

	if (ac < 2 || ac > 5 || zend_get_parameters_ex(ac, &stmt, &array, &zskip, &zmaxrows, &zflags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	switch (ac) {
		case 5:
			convert_to_long_ex(zflags);
			flags = Z_LVAL_PP(zflags);
		case 4:
			convert_to_long_ex(zmaxrows);
			maxrows = Z_LVAL_PP(zmaxrows);
		case 3:
			convert_to_long_ex(zskip);
			skip = Z_LVAL_PP(zskip);
	}

	OCI_GET_STMT(statement,stmt);

	zval_dtor(*array);
	array_init(*array);

	while (skip--) {
		if (!oci_fetch(statement, nrows, "OCIFetchStatement" TSRMLS_CC)) {
			RETURN_LONG(0);
		}
	}

	if (flags & OCI_FETCHSTATEMENT_BY_ROW) {
		columns = safe_emalloc(statement->ncolumns, sizeof(oci_out_column *), 0);

		for (i = 0; i < statement->ncolumns; i++) {
			columns[ i ] = oci_get_col(statement, i + 1, 0);
		}

		while (oci_fetch(statement, nrows, "OCIFetchStatement" TSRMLS_CC)) {
			zval *row;

			MAKE_STD_ZVAL(row);
			array_init(row);

			for (i = 0; i < statement->ncolumns; i++) {
				MAKE_STD_ZVAL(element);

				_oci_make_zval(element,statement,columns[ i ], "OCIFetchStatement",OCI_RETURN_LOBS TSRMLS_CC);

				if (flags & OCI_NUM) {
					zend_hash_next_index_insert(Z_ARRVAL_P(row), &element, sizeof(zval*), NULL);
				} else { /* default to ASSOC */
					zend_hash_update(Z_ARRVAL_P(row), 
							columns[ i ]->name, columns[ i ]->name_len+1, 
							&element, sizeof(zval*), NULL);
				}
			}

			zend_hash_next_index_insert(Z_ARRVAL_PP(array), &row, sizeof(zval*), NULL);

			rows++;

			if ((maxrows != -1) && (rows == maxrows)) {
				oci_fetch(statement, 0, "OCIFetchStatement" TSRMLS_CC);
				break;
			}
		}
		efree(columns);

	} else { /* default to BY_COLUMN */
		columns = safe_emalloc(statement->ncolumns, sizeof(oci_out_column *), 0);
		outarrs = safe_emalloc(statement->ncolumns, sizeof(zval*), 0);
		
		if (flags & OCI_NUM) {
			for (i = 0; i < statement->ncolumns; i++) {
				columns[ i ] = oci_get_col(statement, i + 1, 0);
				
				MAKE_STD_ZVAL(tmp);
				array_init(tmp);
						
				zend_hash_next_index_insert(Z_ARRVAL_PP(array), 
						&tmp, sizeof(zval*), (void **) &(outarrs[ i ]));
			}
		} else { /* default to ASSOC */
			for (i = 0; i < statement->ncolumns; i++) {
				columns[ i ] = oci_get_col(statement, i + 1, 0);
				
				MAKE_STD_ZVAL(tmp);
				array_init(tmp);
						
				zend_hash_update(Z_ARRVAL_PP(array), 
						columns[ i ]->name, columns[ i ]->name_len+1, 
						(void *) &tmp, sizeof(zval*), (void **) &(outarrs[ i ]));
			}
		}

		while (oci_fetch(statement, nrows, "OCIFetchStatement" TSRMLS_CC)) {
			for (i = 0; i < statement->ncolumns; i++) {
				MAKE_STD_ZVAL(element);

				_oci_make_zval(element,statement,columns[ i ], "OCIFetchStatement",OCI_RETURN_LOBS TSRMLS_CC);

				zend_hash_index_update((*(outarrs[ i ]))->value.ht, rows, (void *)&element, sizeof(zval*), NULL);
			}

			rows++;

			if ((maxrows != -1) && (rows == maxrows)) {
				oci_fetch(statement, 0, "OCIFetchStatement" TSRMLS_CC);
				break;
			}
		}
		
		efree(columns);
		efree(outarrs);
	} 

	RETURN_LONG(rows);
}
/* }}} */

/* {{{ proto object oci_fetch_object( resource stmt )
   Fetch a result row as an object */
PHP_FUNCTION(oci_fetch_object)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, OCI_ASSOC, 2);

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto array oci_fetch_row( resource stmt )
   Fetch a result row as an enumerated array */
PHP_FUNCTION(oci_fetch_row)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, OCI_NUM, 1);
}
/* }}} */

/* {{{ proto array oci_fetch_assoc( resource stmt )
   Fetch a result row as an associative array */
PHP_FUNCTION(oci_fetch_assoc)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, OCI_ASSOC, 1);
}
/* }}} */

/* {{{ proto array oci_fetch_array( resource stmt [, int mode ])
   Fetch a result row as an array */
PHP_FUNCTION(oci_fetch_array)
{
	php_oci_fetch_row(INTERNAL_FUNCTION_PARAM_PASSTHRU, OCI_BOTH, 2);
}
/* }}} */

/* {{{ proto bool oci_free_statement(resource stmt)
   Free all resources associated with a statement */
PHP_FUNCTION(oci_free_statement)
{
	zval **stmt;
	oci_statement *statement;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	zend_list_delete(statement->id);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_close(resource conn)
   Disconnect from database */
PHP_FUNCTION(oci_close)
{
#if 0
	this function does nothing any more. server-connections get automagiclly closed on 
	request-end. connection handles will "dissappear" as soon as they are no longer 
	referenced. as this module makes heavy use of zends reference-counting mechanism
	this is the desired behavior. it has always been a bad idea to close a connection that
	has outstanding transactions. this way we have a nice-clean approach.
	(thies@thieso.net 20000110)

	oci_connection *connection;
	zval **conn;

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	connection->is_open = 0;

	zend_hash_apply(list, (apply_func_t) _stmt_cleanup TSRMLS_CC);

	if (zend_list_delete(connection->id) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
#endif
}
/* }}} */

/* {{{ proto resource oci_new_connect(string user, string pass [, string db])
   Connect to an Oracle database and log on. Returns a new session. */
PHP_FUNCTION(oci_new_connect)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}
/* }}} */

/* {{{ proto resource oci_connect(string user, string pass [, string db])
   Connect to an Oracle database and log on. Returns a new session. */
PHP_FUNCTION(oci_connect)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto resource oci_pconnect(string user, string pass [, string db])
   Connect to an Oracle database using a persistent connection and log on. Returns a new session. */
PHP_FUNCTION(oci_pconnect)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1,0);
}
/* }}} */

/* {{{ proto array oci_error([resource stmt|conn|global])
   Return the last error of stmt|conn|global. If no error happened returns false. */
PHP_FUNCTION(oci_error)
{
	zval **arg;
	oci_statement *statement;
	oci_connection *connection;
	text errbuf[512];
	sb4 errcode = 0;
	sword error = 0;
	dvoid *errh = NULL;
#ifdef HAVE_OCI8_ATTR_STATEMENT
	ub2 errorofs = 0;
	text *sqltext = NULL;
#endif

	if (zend_get_parameters_ex(1, &arg) == SUCCESS) {
		statement = (oci_statement *) zend_fetch_resource(arg TSRMLS_CC, -1, NULL, NULL, 1, le_stmt);
		if (statement) {
			errh = statement->pError;
			error = statement->error;

#ifdef HAVE_OCI8_ATTR_STATEMENT
			CALL_OCI_RETURN(statement->error,
				OCIAttrGet(
					(dvoid *)statement->pStmt,
					OCI_HTYPE_STMT,
					(text *) &sqltext,
					(ub4 *)0,
					OCI_ATTR_STATEMENT,
					statement->pError
				)
			);

			CALL_OCI_RETURN(statement->error,
				OCIAttrGet(
					(dvoid *)statement->pStmt,
					OCI_HTYPE_STMT,
					(ub2 *)&errorofs,
					(ub4 *)0,
					OCI_ATTR_PARSE_ERROR_OFFSET,
					statement->pError
				)
			);
#endif

		} else {
			connection = (oci_connection *) zend_fetch_resource(arg TSRMLS_CC, -1, NULL, NULL, 1, le_conn);
			if (connection) {
				errh = connection->pError;
				error = connection->error;
			}
		}
	} else {
		errh = OCI(pError);
		error = OCI(error);
	}

	if (!error) { /* no error set in the handle */
		RETURN_FALSE;
	}

	if (!errh) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCIError: unable to find Error handle");
		RETURN_FALSE;
	}

	CALL_OCI(
		OCIErrorGet(
			errh,
			1,
			NULL,
			&errcode,
			errbuf,
			(ub4) sizeof(errbuf),
			(ub4) OCI_HTYPE_ERROR
		)
	);

	if (errcode) {
		array_init(return_value);
		add_assoc_long(return_value, "code", errcode);
		add_assoc_string(return_value, "message", (char*) errbuf, 1);
#ifdef HAVE_OCI8_ATTR_STATEMENT
		add_assoc_long(return_value, "offset", errorofs);
		add_assoc_string(return_value, "sqltext", sqltext ? (char *) sqltext : "", 1);
#endif
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int oci_num_fields(resource stmt)
   Return the number of result columns in a statement */
PHP_FUNCTION(oci_num_fields)
{
	zval **stmt;
	oci_statement *statement;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	RETURN_LONG(statement->ncolumns);
}
/* }}} */

/* {{{ proto resource oci_parse(resource conn, string query)
   Parse a query and return a statement */
PHP_FUNCTION(oci_parse)
{
	zval **conn, **query;
	oci_connection *connection;
	oci_statement *statement;

	if (zend_get_parameters_ex(2, &conn, &query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	convert_to_string_ex(query);

	statement = oci_parse(connection,Z_STRVAL_PP(query),Z_STRLEN_PP(query));

	if (statement) {
		RETURN_RESOURCE(statement->id);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool oci_set_prefetch(resource stmt, int prefetch_rows)
  Sets the number of rows to be prefetched on execute to prefetch_rows for stmt */
PHP_FUNCTION(oci_set_prefetch)
{
	zval **stmt, **size;
	oci_statement *statement;

	if (zend_get_parameters_ex(2, &stmt, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(size);

	OCI_GET_STMT(statement,stmt);

	oci_setprefetch(statement,Z_LVAL_PP(size)); 

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool oci_password_change(resource conn, string username, string old_password, string new_password)
  Changes the password of an account */
PHP_FUNCTION(oci_password_change)
{
	zval **conn, **user_param, **pass_old_param, **pass_new_param;
	text *user, *pass_old, *pass_new;
	oci_connection *connection;

	/*  Disable in Safe Mode  */
	if (PG(safe_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "is disabled in Safe Mode");
		RETURN_FALSE;
	}

	if (zend_get_parameters_ex(4, &conn, &user_param, &pass_old_param, &pass_new_param) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(user_param);
	convert_to_string_ex(pass_old_param);
	convert_to_string_ex(pass_new_param);
	
	user      = Z_STRVAL_PP(user_param);
	pass_old  = Z_STRVAL_PP(pass_old_param);
	pass_new  = Z_STRVAL_PP(pass_new_param);

	OCI_GET_CONN(connection, conn);

	CALL_OCI_RETURN(connection->error,
		OCIPasswordChange(
			connection->pServiceContext,
			connection->pError,
			user,
			strlen(user)+1,
			pass_old,
			strlen(pass_old)+1,
			pass_new,
			strlen(pass_new)+1,
			OCI_DEFAULT
		)
	);

	if (connection->error == OCI_SUCCESS) {
		RETURN_TRUE;
	} else {
		oci_error(connection->pError, "OCIPasswordChange", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource oci_new_cursor(resource conn)
   Return a new cursor (Statement-Handle) - use this to bind ref-cursors! */
PHP_FUNCTION(oci_new_cursor)
{
	zval **conn;
	oci_connection *connection;
	oci_statement *statement;

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	statement = oci_parse(connection,0,0);
	
	RETURN_RESOURCE(statement->id);
}
/* }}} */

/* {{{ proto string oci_result(resource stmt, mixed column)
   Return a single column of result data */
PHP_FUNCTION(oci_result)
{
	zval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol = NULL;

	if (zend_get_parameters_ex(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);

	if (outcol == NULL) {
		RETURN_FALSE;
	}

	_oci_make_zval(return_value,statement,outcol, "OCIResult",0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto string oci_server_version(resource conn)
   Return a string containing server version information */
PHP_FUNCTION(oci_server_version)
{
	oci_connection *connection;
	zval **conn;
	char version[256];

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	CALL_OCI_RETURN(connection->error,
		OCIServerVersion(
			connection->pServiceContext,
			connection->pError, 
			(text*)version, 
			sizeof(version), 
			OCI_HTYPE_SVCCTX
		)
	);
	
	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "OCIServerVersion", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	RETURN_STRING(version,1);
}
/* }}} */

/* {{{ proto string oci_statement_type(resource stmt)
   Return the query type of an OCI statement */
/* XXX it would be better with a general interface to OCIAttrGet() */
PHP_FUNCTION(oci_statement_type)
{
	zval **stmt;
	oci_statement *statement;
	ub2 stmttype;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	CALL_OCI_RETURN(statement->error,
		OCIAttrGet(
			(dvoid *)statement->pStmt, 
			OCI_HTYPE_STMT, 
			(ub2 *)&stmttype, 
			(ub4 *)0, 
			OCI_ATTR_STMT_TYPE, 
			statement->pError
		)
	);

	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIStatementType", statement->error);
		oci_handle_error(statement->conn, statement->error);
		RETURN_FALSE;
	}

	switch (stmttype) {
		case OCI_STMT_SELECT:
			RETVAL_STRING("SELECT",1);
			break;
		case OCI_STMT_UPDATE:
			RETVAL_STRING("UPDATE",1);
			break;
		case OCI_STMT_DELETE:
			RETVAL_STRING("DELETE",1);
			break;
		case OCI_STMT_INSERT:
			RETVAL_STRING("INSERT",1);
			break;
		case OCI_STMT_CREATE:
			RETVAL_STRING("CREATE",1);
			break;
		case OCI_STMT_DROP:
			RETVAL_STRING("DROP",1);
			break;
		case OCI_STMT_ALTER:
			RETVAL_STRING("ALTER",1);
			break;
		case OCI_STMT_BEGIN:
			RETVAL_STRING("BEGIN",1);
			break;
		case OCI_STMT_DECLARE:
			RETVAL_STRING("DECLARE",1);
			break;
		default:
			RETVAL_STRING("UNKNOWN",1);
	}
}
/* }}} */

/* {{{ proto int oci_num_rows(resource stmt)
   Return the row count of an OCI statement */
PHP_FUNCTION(oci_num_rows)
{
	zval **stmt;
	oci_statement *statement;
	ub4 rowcount;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	CALL_OCI_RETURN(statement->error,
		OCIAttrGet(
			(dvoid *)statement->pStmt, 
			OCI_HTYPE_STMT, 
			(ub2 *)&rowcount, 
			(ub4 *)0, 
			OCI_ATTR_ROW_COUNT,
			statement->pError
		)
	);

	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIRowCount", statement->error);
		oci_handle_error(statement->conn, statement->error);
		RETURN_FALSE;
	}

	RETURN_LONG(rowcount);
}
/* }}} */

#ifdef PHP_OCI8_HAVE_COLLECTIONS
/* {{{ oci_get_coll() */
static oci_collection *oci_get_coll(int ind TSRMLS_DC)
{
	oci_collection *collection;
	int actual_resource_type;

	collection = (oci_collection *) zend_list_find(ind, &actual_resource_type);

	if (collection && (actual_resource_type == le_coll)) {
		return collection;
	} else {
		return (oci_collection *) NULL;
	}
}
/* }}} */

/* {{{ proto bool oci_free_collection()
   Deletes collection object*/
PHP_FUNCTION(oci_free_collection)
{
	zval *id;
	int inx;
	oci_collection *coll;
	oci_connection *connection;
	
	if ((id = getThis()) != 0) {
		inx = _oci_get_ocicoll(id,&coll TSRMLS_CC);
		if (inx) {
			/*
			 * Do we need to free the object?
			 * 
			 */
			connection = coll->conn;
			oci_debug("oci_free_collection: coll=%d",inx);

			CALL_OCI_RETURN(connection->error,
				OCIObjectFree(
					connection->session->pEnv, 
					connection->pError, 
					(dvoid *)coll->coll, 
					(ub2)(OCI_OBJECTFREE_FORCE)
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCIObjectFree", connection->error);
				RETURN_FALSE;
			}

			zend_list_delete(inx);
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_collection_append(string value)
   Append an object to the collection */
PHP_FUNCTION(oci_collection_append)
{
	zval *id, **arg;
	oci_connection *connection;
	oci_collection *coll;
	OCINumber num;
	OCIString *ocistr = (OCIString *)0;
	OCIInd new_ind = OCI_IND_NOTNULL;
	OCIInd null_ind = OCI_IND_NULL;
	OCIDate dt;
	int inx;
	double ndx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		connection = coll->conn;
		if (zend_get_parameters_ex(1, &arg) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		/* 
		 * Handle NULLS.  For consistency with the rest of the OCI8 library, when
		 * a value passed in is a 0 length string, consider it a null
		 */
		convert_to_string_ex(arg);
		if (Z_STRLEN_PP(arg) == 0) {
			CALL_OCI_RETURN(connection->error,
				OCICollAppend(
					connection->session->pEnv, 
					connection->pError, 
					(dvoid *)0, 
					&null_ind, 
					coll->coll
				)
			);
			if (connection->error) {
				oci_error(connection->pError, "OCICollAppend - NULL", connection->error);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}

		switch(coll->element_typecode) {
			case OCI_TYPECODE_DATE:
				convert_to_string_ex(arg);

				CALL_OCI_RETURN(connection->error,
					OCIDateFromText(
						connection->pError, 
						Z_STRVAL_PP(arg), 
						Z_STRLEN_PP(arg), 
						0,
						0,
						0,
						0,
						&dt
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCIDateFromText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error,
					OCICollAppend(
						connection->session->pEnv, 
						connection->pError, 
						(dvoid *) &dt, 
						(dvoid *) &new_ind, 
						(OCIColl *) coll->coll
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCICollAppend", connection->error);
					RETURN_FALSE;
				}
				RETURN_TRUE;
				break;
			case OCI_TYPECODE_VARCHAR2 :
				convert_to_string_ex(arg);

				CALL_OCI_RETURN(connection->error,
					OCIStringAssignText(
						connection->session->pEnv, 
						connection->pError, 
						Z_STRVAL_PP(arg), 
						Z_STRLEN_PP(arg), 
						&ocistr
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCIStringAssignText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error,
					OCICollAppend(
						connection->session->pEnv, 
						connection->pError, 
						(dvoid *) ocistr, 
						(dvoid *) &new_ind, 
						(OCIColl *) coll->coll
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCICollAppend", connection->error);
					RETURN_FALSE;
				}
				RETURN_TRUE;
				break;
			case OCI_TYPECODE_UNSIGNED16 :                       /* UNSIGNED SHORT  */
			case OCI_TYPECODE_UNSIGNED32 :                        /* UNSIGNED LONG  */
			case OCI_TYPECODE_REAL :                                     /* REAL    */
			case OCI_TYPECODE_DOUBLE :                                   /* DOUBLE  */
			case OCI_TYPECODE_INTEGER :                                     /* INT  */
			case OCI_TYPECODE_SIGNED16 :                                  /* SHORT  */
			case OCI_TYPECODE_SIGNED32 :                                   /* LONG  */
			case OCI_TYPECODE_DECIMAL :                                 /* DECIMAL  */
			case OCI_TYPECODE_FLOAT :                                   /* FLOAT    */
			case OCI_TYPECODE_NUMBER :                                  /* NUMBER   */
			case OCI_TYPECODE_SMALLINT :                                /* SMALLINT */
				convert_to_double_ex(arg);
				ndx = (double)Z_DVAL_PP(arg);

				CALL_OCI_RETURN(connection->error,
					OCINumberFromReal(
						connection->pError,
						&ndx, 
						sizeof(double),
						&num
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCINumberFromReal", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error,
					OCICollAppend(
						connection->session->pEnv, 
						connection->pError, 
						(dvoid *) &num, 
						(dvoid *) &new_ind, 
						(OCIColl *) coll->coll
					)
				);

				RETURN_TRUE;
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown or unsupported type of element");
				RETURN_FALSE;
				break;
		}
	}
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_append() should not be called like this. Use $collection->append($element) to append an element to the collection");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string oci_collection_element_get(int ndx)
   Retrieve the value at collection index ndx */
PHP_FUNCTION(oci_collection_element_get)
{
	zval *id,**arg;
	oci_connection *connection;
	oci_collection *coll;
	ub4  ndx;
	int inx;
	dvoid *elem;
	OCIInd *elemind;
	boolean exists;
	OCIString *ocistr = (OCIString *)0;
	text *str;
	char buff[1024];
	int len;
	double dnum;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		if (zend_get_parameters_ex(1, &arg) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(arg);
		ndx = Z_LVAL_PP(arg);

		connection = coll->conn;

		CALL_OCI_RETURN(connection->error,
			OCICollGetElem(
				connection->session->pEnv, 
				connection->pError, 
				coll->coll, 
				ndx, 
				&exists, 
				&elem, 
				(dvoid **)&elemind
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCICollGetElem", connection->error);
			RETURN_NULL();
		}
		
		/* Return false if value does not exist at that location */
		if (exists == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCICollGetElem - Invalid index %d", ndx);
			RETURN_FALSE;
		}

		/* Return null if the value is null */
		if (*elemind == OCI_IND_NULL) {
			RETURN_NULL();
		} 

		switch (coll->element_typecode) {
			case OCI_TYPECODE_DATE:
				len = 1024;
				CALL_OCI(
					OCIDateToText(
						connection->pError, 
						elem, 
						0, /* fmt */ 
						0, /* fmt_length */ 
						0, /* lang_name */ 
						0, /* lang_length */ 
						&len,
						buff
					)
				);

				RETURN_STRINGL(buff,len,1);
			case OCI_TYPECODE_VARCHAR2 :
				ocistr = *(OCIString **)elem;
				str = OCIStringPtr(connection->session->pEnv,ocistr); /* XXX not protected against recursion! */
				RETURN_STRINGL(str,strlen(str),1);
				break;
			case OCI_TYPECODE_UNSIGNED16 :                       /* UNSIGNED SHORT  */
			case OCI_TYPECODE_UNSIGNED32 :                       /* UNSIGNED LONG  */
			case OCI_TYPECODE_REAL :                             /* REAL    */
			case OCI_TYPECODE_DOUBLE :                           /* DOUBLE  */
			case OCI_TYPECODE_INTEGER :                          /* INT  */
			case OCI_TYPECODE_SIGNED16 :                         /* SHORT  */
			case OCI_TYPECODE_SIGNED32 :                         /* LONG  */
			case OCI_TYPECODE_DECIMAL :                          /* DECIMAL  */
			case OCI_TYPECODE_FLOAT :                            /* FLOAT    */
			case OCI_TYPECODE_NUMBER :                           /* NUMBER   */
			case OCI_TYPECODE_SMALLINT :                         /* SMALLINT */
				CALL_OCI_RETURN(connection->error,
					OCINumberToReal(
						connection->pError, 
						(CONST OCINumber *) elem, 
						(uword) sizeof(dnum), 
						(dvoid *) &dnum
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCINumberToReal", connection->error);
					RETURN_FALSE;
				}
				RETURN_DOUBLE(dnum);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown or unsupported type of element");
				RETURN_FALSE;
				break;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_element_get() should not be called like this. Use $collection->getelem($index) to get an element of the collection with the given index");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_collection_assign(object from)
   Assign a collection from another existing collection */
PHP_FUNCTION(oci_collection_assign)
{
	zval *id,**from;
	oci_connection *connection;
	oci_collection *coll,*from_coll;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		if (zend_get_parameters_ex(1, &from) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		if ((inx = _oci_get_ocicoll(*from,&from_coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		connection = coll->conn;
	
		CALL_OCI_RETURN(connection->error,
			OCICollAssign(
				connection->session->pEnv,
				connection->pError, 
				from_coll->coll,
				coll->coll
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCICollAssignElem", connection->error);
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_assign() should not be called like this. Use $collection->assign($collection_value) to assign value to the collection");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_collection_element_assign(int index, string val)
   Assign element val to collection at index ndx */
PHP_FUNCTION(oci_collection_element_assign)
{
	zval *id,**index,**val;
	oci_connection *connection;
	oci_collection *coll;
	OCINumber num;
	OCIInd new_ind = OCI_IND_NOTNULL;
	OCIInd null_ind = OCI_IND_NULL;
	ub4  ndx;
	int inx;
	OCIString *ocistr = (OCIString *)0;
	OCIDate dt;
	double  dnum;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}

		if (zend_get_parameters_ex(2, &index,&val) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(index);
		ndx = Z_LVAL_PP(index);

		connection = coll->conn;

		if (connection->error) {
			oci_error(connection->pError, "OCICollAssignElem", connection->error);
			RETURN_FALSE;
		}

		/* 
		 * Handle NULLs.  For consistency with the rest of the OCI8 library, when
		 * a value passed in is a 0 length string, consider it a null
		 */
		convert_to_string_ex(val);

		if (Z_STRLEN_PP(val) == 0) {
			CALL_OCI_RETURN(connection->error,
				OCICollAssignElem(
					connection->session->pEnv, 
					connection->pError, 
					ndx, 
					(dvoid *)0, 
					&null_ind, 
					coll->coll
				)
			);
			if (connection->error) {
				oci_error(connection->pError, "OCICollAssignElem - NULL", connection->error);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}

		switch (coll->element_typecode) {
			case OCI_TYPECODE_DATE:
				convert_to_string_ex(val);
				CALL_OCI_RETURN(connection->error,
					OCIDateFromText(
						connection->pError, 
						Z_STRVAL_PP(val), 
						Z_STRLEN_PP(val), 
						0,
						0,
						0,
						0,
						&dt
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCIDateFromText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error,
					OCICollAssignElem(
						connection->session->pEnv, 
						connection->pError, 
						ndx, 
						(dvoid *)&dt, 
						&new_ind, 
						coll->coll
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCICollAssignElem", connection->error);
					RETURN_FALSE;
				}
				break;
			case OCI_TYPECODE_VARCHAR2 :
				convert_to_string_ex(val);

				CALL_OCI_RETURN(connection->error,
					OCIStringAssignText(
						connection->session->pEnv, 
						connection->pError, 
						Z_STRVAL_PP(val), 
						Z_STRLEN_PP(val), 
						&ocistr
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCIStringAssignText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error,
					OCICollAssignElem(
						connection->session->pEnv, 
						connection->pError, 
						ndx, 
						(dvoid *)ocistr, 
						&new_ind, 
						coll->coll
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCICollAssignElem", connection->error);
					RETURN_FALSE;
				}
				RETURN_TRUE;
				break;
			case OCI_TYPECODE_UNSIGNED16 :                       /* UNSIGNED SHORT  */
			case OCI_TYPECODE_UNSIGNED32 :                       /* UNSIGNED LONG  */
			case OCI_TYPECODE_REAL :                             /* REAL    */
			case OCI_TYPECODE_DOUBLE :                           /* DOUBLE  */
			case OCI_TYPECODE_INTEGER :                          /* INT  */
			case OCI_TYPECODE_SIGNED16 :                         /* SHORT  */
			case OCI_TYPECODE_SIGNED32 :                         /* LONG  */
			case OCI_TYPECODE_DECIMAL :                          /* DECIMAL  */
			case OCI_TYPECODE_FLOAT :                            /* FLOAT    */
			case OCI_TYPECODE_NUMBER :                           /* NUMBER   */
			case OCI_TYPECODE_SMALLINT :                         /* SMALLINT */
				convert_to_double_ex(val);
				dnum = (double)Z_DVAL_PP(val);

				CALL_OCI_RETURN(connection->error,
					OCINumberFromReal(
						connection->pError,
						&dnum, 
						sizeof(double),
						&num
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCINumberFromReal", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error,
					OCICollAssignElem(
						connection->session->pEnv, 
						connection->pError, 
						ndx, 
						(dvoid *)&num, 
						&new_ind, 
						coll->coll
					)
				);

				if (connection->error) {
					oci_error(connection->pError, "OCICollAssignElem", connection->error);
					RETURN_FALSE;
				}
				RETURN_TRUE;
				break;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_element_assign() should not be called like this. Use $collection->assignelem($index, $value) to assign value to an element of the collection");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_collection_size()
   Return the size of a collection */
PHP_FUNCTION(oci_collection_size)
{
	zval *id;
	oci_connection *connection;
	oci_collection *coll;
	sb4 sz;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		connection = coll->conn;

		CALL_OCI_RETURN(connection->error,
			OCICollSize(
				connection->session->pEnv,
				coll->conn->pError,
				coll->coll,
				&sz
			)
		);

		if (connection->error) {
			oci_error(connection->pError, "OCICollSize", connection->error);
			RETURN_FALSE;
		}

		RETURN_LONG(sz);
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_size() should not be called like this. Use $collection->size() to get size of the collection");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int oci_collection_max()
   Return the max value of a collection. For a varray this is the maximum length of the array */
PHP_FUNCTION(oci_collection_max)
{
	zval *id;
	oci_collection *coll;
	sb4 sz;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		sz = OCICollMax(OCI(pEnv),coll->coll); /* XXX not protected against recursion */

		RETURN_LONG(sz);
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_max() should not be called like this. Use $collection->max() to get maximum number of elements in the collection");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool oci_collection_trim(int num)
   Trim num elements from the end of a collection */
PHP_FUNCTION(oci_collection_trim)
{
	zval *id,**arg;
	oci_collection *coll;
	int inx;

	if ((id = getThis()) != 0) {
		if ((inx = _oci_get_ocicoll(id,&coll TSRMLS_CC)) == 0) {
			RETURN_FALSE;
		}
		if (zend_get_parameters_ex(1, &arg) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg);

		CALL_OCI_RETURN(coll->conn->error,
			OCICollTrim(
				OCI(pEnv),
				coll->conn->pError,
				Z_LVAL_PP(arg),
				coll->coll
			)
		);

		if (coll->conn->error) {
			oci_error(coll->conn->pError, "OCICollTrim", coll->conn->error);
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "oci_collection_trim() should not be called like this. Use $collection->trim($length) to trim collection to the given length");
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto object oci_new_collection(resource connection, string tdo [, string schema])
   Initialize a new collection */
PHP_FUNCTION(oci_new_collection)
{
	dvoid *dschp1;
	dvoid *parmp1;
	dvoid *parmp2;
	zval **conn, **tdo, **schema;
	oci_connection *connection;
	oci_collection *coll;
	int ac = ZEND_NUM_ARGS();

	if (ac < 2 || ac > 3 || zend_get_parameters_ex(ac, &conn, &tdo, &schema) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_string_ex(tdo);

	if (ac == 3) {
		convert_to_string_ex(schema);
	}

	coll = emalloc(sizeof(oci_collection));

	OCI_GET_CONN(connection,conn);

	coll->conn = connection;
	coll->id = zend_list_insert(coll,le_coll);
	zend_list_addref(connection->id);

	CALL_OCI_RETURN(connection->error,
		OCITypeByName(
			connection->session->pEnv, 
			connection->pError, 
			connection->pServiceContext, 
			ac == 3 ? (text *) Z_STRVAL_PP(schema) : (text *) 0, 
			ac == 3 ? (ub4) Z_STRLEN_PP(schema) : (ub4) 0, 
			(text *) Z_STRVAL_PP(tdo), 
			(ub4)    Z_STRLEN_PP(tdo), 
			(CONST text *) 0, 
			(ub4) 0, 
			OCI_DURATION_SESSION, 
			OCI_TYPEGET_ALL, 
			&(coll->tdo)
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCITypeByName", connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error, 
		OCIHandleAlloc(
			connection->session->pEnv, 
			(dvoid **) &dschp1, 
			(ub4) OCI_HTYPE_DESCRIBE, 
			(size_t) 0, 
			(dvoid **) 0
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCI_HTYPE_DESCRIBE", connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error,
		OCIDescribeAny(
			connection->pServiceContext, 
			connection->pError, 
			(dvoid *) coll->tdo, 
			(ub4) 0, 
			OCI_OTYPE_PTR, 
			(ub1)1, 
			(ub1) OCI_PTYPE_TYPE, 
			dschp1
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCI_OTYPE_PTR", connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error,
		OCIAttrGet(
			(dvoid *) dschp1, 
			(ub4) OCI_HTYPE_DESCRIBE, 
			(dvoid *)&parmp1, 
			(ub4 *)0, 
			(ub4)OCI_ATTR_PARAM,
			connection->pError
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCI_ATTR_PARAM", connection->error);
		RETURN_FALSE;
	}

	/* get the collection type code of the attribute */

	CALL_OCI_RETURN(connection->error,
		OCIAttrGet(
			(dvoid*) parmp1, 
			(ub4) OCI_DTYPE_PARAM, 
			(dvoid*) &(coll->coll_typecode), 
			(ub4 *) 0, 
			(ub4) OCI_ATTR_COLLECTION_TYPECODE, 
			connection->pError
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCI_ATTR_COLLECTION_TYPECODE", connection->error);
		RETURN_FALSE;
	}

	switch(coll->coll_typecode) {
		case OCI_TYPECODE_TABLE:
		case OCI_TYPECODE_VARRAY:
			CALL_OCI_RETURN(connection->error,
				OCIAttrGet(
					(dvoid*) parmp1, 
					(ub4) OCI_DTYPE_PARAM, 
					(dvoid*) &parmp2, 
					(ub4 *) 0, 
					(ub4) OCI_ATTR_COLLECTION_ELEMENT, 
					connection->pError
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCI_ATTR_COLLECTION_ELEMENT", connection->error);
				RETURN_FALSE;
			}

			CALL_OCI_RETURN(connection->error,
				OCIAttrGet(
					(dvoid*) parmp2, 
					(ub4) OCI_DTYPE_PARAM, 
					(dvoid*) &(coll->elem_ref), 
					(ub4 *) 0, 
					(ub4) OCI_ATTR_REF_TDO, 
					connection->pError
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCI_ATTR_REF_TDO", connection->error);
				RETURN_FALSE;
			}

			CALL_OCI_RETURN(connection->error,
				OCITypeByRef(
					connection->session->pEnv, 
					connection->pError, 
					coll->elem_ref, 
					OCI_DURATION_SESSION, 
					OCI_TYPEGET_HEADER, 
					&(coll->element_type)
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCI_TYPEGET_HEADER", connection->error);
				RETURN_FALSE;
			}

			CALL_OCI_RETURN(connection->error,
				OCIAttrGet(
					(dvoid*) parmp2, 
					(ub4) OCI_DTYPE_PARAM, 
					(dvoid*) &(coll->element_typecode), 
					(ub4 *) 0, 
					(ub4) OCI_ATTR_TYPECODE, 
					connection->pError
				)
			);

			if (connection->error) {
				oci_error(connection->pError, "OCI_ATTR_TYPECODE", connection->error);
				RETURN_FALSE;
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "OCINewCollection - Unknown Type %d", coll->coll_typecode);
			break;
	}	

	/* Create object to hold return table */
	CALL_OCI_RETURN(connection->error,
		OCIObjectNew(
			connection->session->pEnv, 
			connection->pError, 
			connection->pServiceContext, 
			OCI_TYPECODE_TABLE, 
			coll->tdo, 
			(dvoid *)0, 
			OCI_DURATION_DEFAULT, 
			TRUE, 
			(dvoid **) &(coll->coll)
		)
	);

	if (connection->error) {
		oci_error(connection->pError, "OCIObjectNew", connection->error);
		RETURN_FALSE;
	}

	object_init_ex(return_value, oci_coll_class_entry_ptr);
	add_property_resource(return_value, "collection",coll->id);
}
/* }}} */
#endif

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
