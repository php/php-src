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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* TODO list:
 *
 * - php.ini flags 
 *   especialliy important for things like oci_ping
 * - Change return-value for OCIFetch*() (1-row read, 0-Normal end, false-error) 
 * - Error mode (print or shut up?)
 * - binding of arrays
 * - Character sets for NCLOBS
 * - split the module into an upper (php-callable) and lower (c-callable) layer!
 * - remove all XXXs
 * - clean up and documentation
 * - make OCIInternalDebug accept a mask of flags....
 * - add some flags to OCIFetchStatement (maxrows etc...)
 * - have one ocifree() call.
 * - make it possible to have persistent statements?
 * - implement connection pooling in ZTS mode.
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
 */

/* {{{ includes & stuff */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

/* #define HAVE_OCI8_TEMP_LOB 1 */
#define WITH_COLLECTIONS 1

#if HAVE_OCI8

#include "php_oci8.h"

/* True globals, no need for thread safety */
static int le_conn;
static int le_stmt;
static int le_desc;
#ifdef WITH_COLLECTIONS 
static int le_coll;
#endif
static int le_server;
static int le_session;
static zend_class_entry *oci_lob_class_entry_ptr;
#ifdef WITH_COLLECTIONS
static zend_class_entry *oci_coll_class_entry_ptr;
#endif

#ifndef SQLT_BFILEE
#define SQLT_BFILEE 114
#endif
#ifndef SQLT_CFILEE
#define SQLT_CFILEE 115
#endif

#define SAFE_STRING(s) ((s)?(s):"")


/* dirty marcos to make sure we _never_ call oracle-functions recursivly 
 *
 * i'm well aware that we should _never_ call exit directly - this core is for
 * pure testing and commented out - as you can see;-)
 * thies@thieso.net 20010723
 */

#define CALL_OCI(call) \
{ \
	if (OCI(in_call)) { \
		php_error(E_WARNING, "OCI8 Recursive call!\n"); \
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
		php_error(E_WARNING, "OCI8 Recursive call!\n"); \
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
#ifdef WITH_COLLECTIONS
static void _oci_coll_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
#endif
static void _oci_server_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _oci_session_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void php_oci_free_conn_list(zend_rsrc_list_entry *rsrc TSRMLS_DC);

static void _oci_column_hash_dtor(void *data);
static void _oci_define_hash_dtor(void *data);
static void _oci_bind_hash_dtor(void *data);

static oci_connection *oci_get_conn(zval ** TSRMLS_DC);
static oci_statement *oci_get_stmt(zval ** TSRMLS_DC);
static oci_descriptor *oci_get_desc(int TSRMLS_DC);
#ifdef WITH_COLLECTIONS
/* Questionable name.  Very close to oci_get_col */
static oci_collection *oci_get_coll(int TSRMLS_DC);
#endif
static oci_out_column *oci_get_col(oci_statement *, int, zval **);

static int _oci_make_zval(zval *, oci_statement *, oci_out_column *, char *, int mode TSRMLS_DC);
static oci_statement *oci_parse(oci_connection *, char *, int);
static int oci_execute(oci_statement *, char *, ub4 mode);
static int oci_fetch(oci_statement *, ub4, char * TSRMLS_DC);
static int oci_loadlob(oci_connection *, oci_descriptor *, char **, ub4 *length);
static int oci_setprefetch(oci_statement *statement, int size);

static void oci_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive);

static oci_server *_oci_open_server(char *dbname,int persistent);
static void _oci_close_server(oci_server *server);

static oci_session *_oci_open_session(oci_server* server,char *username,char *password,int persistent,int exclusive);
static void _oci_close_session(oci_session *session);

static sb4 oci_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
static sb4 oci_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

#if 0
static sb4 oci_failover_callback(dvoid *svchp,dvoid* envhp,dvoid *fo_ctx,ub4 fo_type, ub4 fo_event);
#endif

/* }}} */
/* {{{ extension function prototypes */

PHP_FUNCTION(ocibindbyname);
PHP_FUNCTION(ocidefinebyname);
PHP_FUNCTION(ocicolumnisnull);
PHP_FUNCTION(ocicolumnname);
PHP_FUNCTION(ocicolumnsize);
PHP_FUNCTION(ocicolumnscale);
PHP_FUNCTION(ocicolumnprecision);
PHP_FUNCTION(ocicolumntype);
PHP_FUNCTION(ocicolumntyperaw);
PHP_FUNCTION(ociexecute);
PHP_FUNCTION(ocifetch);
PHP_FUNCTION(ocicancel);
PHP_FUNCTION(ocifetchinto);
PHP_FUNCTION(ocifetchstatement);
PHP_FUNCTION(ocifreestatement);
PHP_FUNCTION(ociinternaldebug);
PHP_FUNCTION(ocilogoff);
PHP_FUNCTION(ocilogon);
PHP_FUNCTION(ocinlogon);
PHP_FUNCTION(ociplogon);
PHP_FUNCTION(ocierror);
PHP_FUNCTION(ocifreedesc);
PHP_FUNCTION(ocisavelob);
PHP_FUNCTION(ocisavelobfile);
PHP_FUNCTION(ociloadlob);
PHP_FUNCTION(ociwritelobtofile);
PHP_FUNCTION(ocicommit);
PHP_FUNCTION(ocirollback);
PHP_FUNCTION(ocinewdescriptor);
PHP_FUNCTION(ocinumcols);
PHP_FUNCTION(ociparse);
PHP_FUNCTION(ocinewcursor);
PHP_FUNCTION(ociresult);
PHP_FUNCTION(ociserverversion);
PHP_FUNCTION(ocistatementtype);
PHP_FUNCTION(ocirowcount);
PHP_FUNCTION(ocisetprefetch);
PHP_FUNCTION(ocipasswordchange);
#ifdef HAVE_OCI8_TEMP_LOB
PHP_FUNCTION(ociwritetemporarylob);
PHP_FUNCTION(ocicloselob);
#endif
#ifdef WITH_COLLECTIONS
PHP_FUNCTION(ocinewcollection);
PHP_FUNCTION(ocifreecollection);
PHP_FUNCTION(ocicollappend);
PHP_FUNCTION(ocicollgetelem);
PHP_FUNCTION(ocicollassignelem);
PHP_FUNCTION(ocicollassign);
PHP_FUNCTION(ocicollsize);
PHP_FUNCTION(ocicollmax);
PHP_FUNCTION(ocicolltrim);
#endif

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

#ifdef WITH_COLLECTIONS
#define OCI_GET_COLL(collection,index) \
	collection = oci_get_coll(index TSRMLS_CC); \
	if (collection == NULL) { \
		RETURN_FALSE; \
	}
#endif

/* }}} */
/* {{{ extension definition structures */

#define OCI_ASSOC               1<<0
#define OCI_NUM                 1<<1
#define OCI_BOTH                (OCI_ASSOC|OCI_NUM)

#define OCI_RETURN_NULLS        1<<2
#define OCI_RETURN_LOBS         1<<3

#define OCI_FETCHSTATEMENT_BY_COLUMN    1<<4
#define OCI_FETCHSTATEMENT_BY_ROW       1<<5
#define OCI_FETCHSTATEMENT_BY           (OCI_FETCHSTATEMENT_BY_COLUMN | OCI_FETCHSTATEMENT_BY_ROW)

static unsigned char a3_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char a2_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };

static zend_function_entry php_oci_functions[] = {
	PHP_FE(ocidefinebyname,        a3_arg_force_ref)
	PHP_FE(ocibindbyname,          a3_arg_force_ref)
	PHP_FE(ocicolumnisnull,        NULL)
	PHP_FE(ocicolumnname,          NULL)
	PHP_FE(ocicolumnsize,          NULL)
	PHP_FE(ocicolumnscale,         NULL)
	PHP_FE(ocicolumnprecision,     NULL)
	PHP_FE(ocicolumntype,          NULL)
	PHP_FE(ocicolumntyperaw,       NULL)
	PHP_FE(ociexecute,             NULL)
	PHP_FE(ocicancel,              NULL)
	PHP_FE(ocifetch,               NULL)
	PHP_FE(ocifetchinto,           a2_arg_force_ref)
	PHP_FE(ocifetchstatement,      a2_arg_force_ref)
	PHP_FE(ocifreestatement,       NULL)
	PHP_FE(ociinternaldebug,       NULL)
	PHP_FE(ocinumcols,             NULL)
	PHP_FE(ociparse,               NULL)
	PHP_FE(ocinewcursor,           NULL)
	PHP_FE(ociresult,              NULL)
	PHP_FE(ociserverversion,       NULL)
	PHP_FE(ocistatementtype,       NULL)
	PHP_FE(ocirowcount,            NULL)
	PHP_FE(ocilogoff,              NULL)
	PHP_FE(ocilogon,               NULL)
	PHP_FE(ocinlogon,              NULL)
	PHP_FE(ociplogon,              NULL)
	PHP_FE(ocierror,               NULL)
	PHP_FE(ocifreedesc,            NULL)
	PHP_FE(ocisavelob,             NULL)
	PHP_FE(ocisavelobfile,         NULL)
	PHP_FE(ociloadlob,             NULL)
	PHP_FE(ociwritelobtofile,      NULL)
	PHP_FE(ocicommit,              NULL)
	PHP_FE(ocirollback,            NULL)
	PHP_FE(ocinewdescriptor,       NULL)
	PHP_FE(ocisetprefetch,         NULL)
	PHP_FE(ocipasswordchange,      NULL)
#ifdef WITH_COLLECTIONS
	PHP_FE(ocifreecollection,      NULL)
	PHP_FE(ocicollappend,          NULL)
	PHP_FE(ocicollgetelem,         NULL)
	PHP_FE(ocicollassignelem,      NULL)
	PHP_FE(ocicollassign,          NULL)
	PHP_FE(ocicollsize,            NULL)
	PHP_FE(ocicollmax,             NULL)
	PHP_FE(ocicolltrim,            NULL)
	PHP_FE(ocinewcollection,       NULL)
#endif

	PHP_FALIAS(ocifreecursor,ocifreestatement,NULL)

	{NULL,NULL,NULL}
};

static zend_function_entry php_oci_lob_class_functions[] = {
	PHP_FALIAS(load,        ociloadlob,             NULL)
	PHP_FALIAS(writetofile, ociwritelobtofile,      NULL)
#ifdef HAVE_OCI8_TEMP_LOB
	PHP_FALIAS(writetemporary, ociwritetemporarylob,NULL)
	PHP_FALIAS(close,       ocicloselob,            NULL)
#endif
	PHP_FALIAS(save,        ocisavelob,             NULL)
	PHP_FALIAS(savefile,    ocisavelobfile,         NULL)
	PHP_FALIAS(free,        ocifreedesc,            NULL)
	{NULL,NULL,NULL}
};

#ifdef WITH_COLLECTIONS
static zend_function_entry php_oci_coll_class_functions[] = {
	PHP_FALIAS(append,        ocicollappend,        NULL)
	PHP_FALIAS(getelem,       ocicollgetelem,       NULL)
	PHP_FALIAS(assignelem,    ocicollassignelem,    NULL)
	PHP_FALIAS(assign,        ocicollassign,        NULL)
	PHP_FALIAS(size,          ocicollsize,          NULL)
	PHP_FALIAS(max,           ocicollmax,           NULL)
	PHP_FALIAS(trim,          ocicolltrim,          NULL)
	PHP_FALIAS(free,          ocifreecollection,    NULL)
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
/* {{{ debug malloc/realloc/free */

#define OCI_USE_EMALLOC 0		/* set this to 1 if you want to use the php memory manager! */

#if OCI_USE_EMALLOC
CONST dvoid *ocimalloc(dvoid *ctx, size_t size)
{
	dvoid *ret;
	ret = (dvoid *)malloc(size);
	oci_debug("ocimalloc(%d) = %08x", size,ret);
	return ret;
}

CONST dvoid *ocirealloc(dvoid *ctx, dvoid *ptr, size_t size)
{
	dvoid *ret;
	oci_debug("ocirealloc(%08x, %d)", ptr, size);
	ret = (dvoid *)realloc(ptr, size);
	return ptr;
}

CONST void ocifree(dvoid *ctx, dvoid *ptr)
{
	oci_debug("ocifree(%08x)", ptr);
	free(ptr);
}
#endif

/* }}} */
/* {{{ startup, shutdown and info functions */

static void php_oci_init_globals(php_oci_globals *oci_globals_p TSRMLS_DC)
{ 
	OCI(shutdown)	= 0;
	OCI(in_call)	= 0;

	OCI(user) = malloc(sizeof(HashTable));
	zend_hash_init(OCI(user), 13, NULL, NULL, 1);

	OCI(server) = malloc(sizeof(HashTable));
	zend_hash_init(OCI(server), 13, NULL, NULL, 1); 

	CALL_OCI(OCIEnvInit(
				&OCI(pEnv), 
				OCI_DEFAULT, 
				0, 
				NULL));
	
	CALL_OCI(OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **)&OCI(pError), 
				OCI_HTYPE_ERROR, 
				0, 
				NULL));
}

PHP_MINIT_FUNCTION(oci)
{
	zend_class_entry oci_lob_class_entry;
#ifdef WITH_COLLECTIONS
	zend_class_entry oci_coll_class_entry;
#endif

#ifdef WITH_COLLECTIONS
#define PHP_OCI_INIT_MODE OCI_DEFAULT | OCI_OBJECT
#else
#define PHP_OCI_INIT_MODE OCI_DEFAULT
#endif

#if OCI_USE_EMALLOC
 	OCIInitialize(PHP_OCI_INIT_MODE, NULL, ocimalloc, ocirealloc, ocifree);
#else
	OCIInitialize(PHP_OCI_INIT_MODE, NULL, NULL, NULL, NULL);
#endif

#ifdef ZTS
	ts_allocate_id(&oci_globals_id, sizeof(php_oci_globals), (ts_allocate_ctor) php_oci_init_globals, NULL);
#else
	php_oci_init_globals(&oci_globals TSRMLS_CC);
#endif

	le_stmt = zend_register_list_destructors_ex(_oci_stmt_list_dtor, NULL, "oci8 statement", module_number);
	le_conn = zend_register_list_destructors_ex(php_oci_free_conn_list, NULL, "oci8 connection", module_number);
	le_desc = zend_register_list_destructors_ex(_oci_descriptor_list_dtor, NULL, "oci8 descriptor", module_number);
#ifdef WITH_COLLECTIONS
	le_coll = zend_register_list_destructors_ex(_oci_coll_list_dtor, NULL, "oci8 collection", module_number);
#endif
	le_server = zend_register_list_destructors_ex(_oci_server_list_dtor, NULL, "oci8 server", module_number);
	le_session = zend_register_list_destructors_ex(_oci_session_list_dtor, NULL, "oci8 session", module_number);

	INIT_CLASS_ENTRY(oci_lob_class_entry, "OCI-Lob", php_oci_lob_class_functions);
#ifdef WITH_COLLECTIONS
	INIT_CLASS_ENTRY(oci_coll_class_entry, "OCI-Collection", php_oci_coll_class_functions);
#endif

	oci_lob_class_entry_ptr = zend_register_internal_class(&oci_lob_class_entry TSRMLS_CC);
#ifdef WITH_COLLECTIONS
	oci_coll_class_entry_ptr = zend_register_internal_class(&oci_coll_class_entry TSRMLS_CC);
#endif

/* thies@thieso.net 990203 i do not think that we will need all of them - just in here for completeness for now! */
	REGISTER_LONG_CONSTANT("OCI_DEFAULT",OCI_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DESCRIBE_ONLY",OCI_DESCRIBE_ONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_COMMIT_ON_SUCCESS",OCI_COMMIT_ON_SUCCESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_EXACT_FETCH",OCI_EXACT_FETCH, CONST_CS | CONST_PERSISTENT);

/* for OCIBindByName (real "oci" names + short "php" names*/
	REGISTER_LONG_CONSTANT("SQLT_BFILEE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_RDD",SQLT_RDD, CONST_CS | CONST_PERSISTENT);

#ifdef WITH_COLLECTIONS
	REGISTER_LONG_CONSTANT("OCI_B_SQLT_NTY",SQLT_NTY, CONST_CS | CONST_PERSISTENT);
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

	return SUCCESS;
}

/* ----------------------------------------------------------------- */


PHP_RINIT_FUNCTION(oci)
{
	/* XXX NYI
	OCI(num_links) = 
		OCI(num_persistent);
	*/

	OCI(debug_mode) = 0; /* start "fresh" */
/*	OCI(in_call) = 0; i don't think we want this! */

	oci_debug("php_rinit_oci");

	return SUCCESS;
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

PHP_MSHUTDOWN_FUNCTION(oci)
{
	OCI(shutdown) = 1;

	oci_debug("START php_mshutdown_oci");

	zend_hash_apply(OCI(user), (apply_func_t)_session_pcleanup TSRMLS_CC);
	zend_hash_apply(OCI(server), (apply_func_t)_server_pcleanup TSRMLS_CC);

	zend_hash_destroy(OCI(user));
	zend_hash_destroy(OCI(server));

	free(OCI(user));
	free(OCI(server));

	CALL_OCI(OCIHandleFree(
				(dvoid *)OCI(pEnv), 
				OCI_HTYPE_ENV));

	oci_debug("END   php_mshutdown_oci");

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(oci)
{
	oci_debug("START php_rshutdown_oci");

#if 0
	/* XXX free all statements, rollback all outstanding transactions */

	zend_hash_apply(OCI(user), (apply_func_t) _session_cleanup TSRMLS_CC);
	zend_hash_apply(OCI(server), (apply_func_t) _server_cleanup TSRMLS_CC);
#endif

	oci_debug("END   php_rshutdown_oci");

	return SUCCESS;
}


PHP_MINFO_FUNCTION(oci)
{

	php_info_print_table_start();
	php_info_print_table_row(2, "OCI8 Support", "enabled");
	php_info_print_table_row(2, "Revision", "$Revision$");
#ifndef PHP_WIN32
	php_info_print_table_row(2, "Oracle Version", PHP_OCI8_VERSION );
	php_info_print_table_row(2, "Compile-time ORACLE_HOME", PHP_OCI8_DIR );
	php_info_print_table_row(2, "Libraries Used", PHP_OCI8_SHARED_LIBADD );
#endif
	php_info_print_table_end();

}

/* }}} */
/* {{{ _oci_define_hash_dtor() */

static void
_oci_define_hash_dtor(void *data)
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
/* {{{ _oci_bind_hash_dtor() */

static void
_oci_bind_hash_dtor(void *data)
{
	oci_bind *bind = (oci_bind *) data;

	oci_debug("_oci_bind_hash_dtor:");

	zval_ptr_dtor(&(bind->zval));
}

/* }}} */
/* {{{ _oci_bind_pre_exec() */

static int
_oci_bind_pre_exec(void *data TSRMLS_DC)
{
	oci_bind *bind = (oci_bind *) data;

	/* reset all bind stuff to a normal state..-. */

	bind->indicator = 0; 

	return 0;
}

/* }}} */
/* {{{ _oci_bind_post_exec() */

static int
_oci_bind_post_exec(void *data TSRMLS_DC)
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
/* {{{ _oci_column_hash_dtor() */

static void
_oci_column_hash_dtor(void *data)
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
/* {{{ _oci_stmt_list_dtor() */
 
static void
_oci_stmt_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_statement *statement = (oci_statement *)rsrc->ptr;
	oci_debug("START _oci_stmt_list_dtor: id=%d last_query=\"%s\"",statement->id,SAFE_STRING(statement->last_query));

 	if (statement->pStmt) {
		CALL_OCI(OCIHandleFree(
					statement->pStmt, 
					OCI_HTYPE_STMT));
		
		statement->pStmt = 0;
	}

	if (statement->pError) {
		CALL_OCI(OCIHandleFree(
					statement->pError, 
					OCI_HTYPE_ERROR));
		
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

	zend_list_delete(statement->conn->id);

	oci_debug("END   _oci_stmt_list_dtor: id=%d",statement->id);

	efree(statement);
}

/* }}} */
/* {{{ _oci_conn_list_dtor() */

static void
_oci_conn_list_dtor(oci_connection *connection TSRMLS_DC)
{
	/* 
	   as the connection is "only" a in memory service context we do not disconnect from oracle.
	*/

	oci_debug("START _oci_conn_list_dtor: id=%d",connection->id);

	if (connection->pServiceContext) {

		if (connection->needs_commit) {
			oci_debug("OCITransRollback");
			CALL_OCI_RETURN(connection->error,OCITransRollback(
						connection->pServiceContext,
						connection->pError,
						(ub4)0));
	 
			if (connection->error) {
				oci_error(connection->pError, "failed to rollback outstanding transactions!", connection->error);
			}
			connection->needs_commit = 0;
		} else {
			oci_debug("nothing to do..");
		}

		CALL_OCI(OCIHandleFree(
					(dvoid *) connection->pServiceContext, 
					(ub4) OCI_HTYPE_SVCCTX));
	}

	if (connection->session && connection->session->exclusive) {
		/* exclusive connection created via OCINLogon() close their 
		   associated session when destructed */
		zend_list_delete(connection->session->num);
	}

	if (connection->pError) {
		CALL_OCI(OCIHandleFree(
					(dvoid *) connection->pError, 
					(ub4) OCI_HTYPE_ERROR));
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

#ifdef WITH_COLLECTIONS

/* {{{ _oci_coll_list_dtor()
 */

static void 
_oci_coll_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_collection *coll = (oci_collection *)rsrc->ptr;
	oci_debug("START _oci_coll_list_dtor: %d",coll->id);

	zend_list_delete(coll->conn->id);
	/* Note sure if we need to free the object.  Have an
	   oracle TAR out on this one.
	   OCIDescriptorFree(descr->ocidescr, Z_TYPE_P(descr)); */

	oci_debug("END   _oci_coll_list_dtor: %d",coll->id);

	efree(coll);
}
/* }}} */
#endif

/* {{{ _oci_descriptor_list_dtor()
 */

static void 
_oci_descriptor_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_descriptor *descr = (oci_descriptor *)rsrc->ptr;
	oci_debug("START _oci_descriptor_list_dtor: %d",descr->id);

	zend_list_delete(descr->conn->id);

	CALL_OCI(OCIDescriptorFree(
				descr->ocidescr, 
				Z_TYPE_P(descr)));

	oci_debug("END   _oci_descriptor_list_dtor: %d",descr->id);

	efree(descr);
}

/* }}} */
/* {{{ _oci_server_list_dtor()
 */

static void 
_oci_server_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_server *server = (oci_server *)rsrc->ptr;
	if (server->persistent)
		return;

	_oci_close_server(server);
}

/* }}} */
/* {{{ _oci_session_list_dtor()
 */

static void 
_oci_session_list_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oci_session *session = (oci_session *)rsrc->ptr;
	if (session->persistent)
		return;

	_oci_close_session(session);
}

/* }}} */

/* {{{ oci_handle_error
 */
static ub4
oci_handle_error(oci_connection *connection, ub4 errcode)
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

/* {{{ oci_error() */

static ub4
oci_error(OCIError *err_p, char *what, sword status)
{
	text errbuf[512];
	sb4 errcode = 0;

	switch (status) {
	case OCI_SUCCESS:
		break;
	case OCI_SUCCESS_WITH_INFO:
		php_error(E_WARNING, "%s: OCI_SUCCESS_WITH_INFO", what);
		break;
	case OCI_NEED_DATA:
		php_error(E_WARNING, "%s: OCI_NEED_DATA", what);
		break;
	case OCI_NO_DATA:
		php_error(E_WARNING, "%s: OCI_NO_DATA", what);
		break;
	case OCI_ERROR: {
			TSRMLS_FETCH();
			CALL_OCI(OCIErrorGet(
					err_p, 
					(ub4)1, 
					NULL, 
					&errcode, 
					errbuf,
					(ub4)sizeof(errbuf), 
					(ub4)OCI_HTYPE_ERROR));
		
			php_error(E_WARNING, "%s: %s", what, errbuf);
			break;
		}
	case OCI_INVALID_HANDLE:
		php_error(E_WARNING, "%s: OCI_INVALID_HANDLE", what);
		break;
	case OCI_STILL_EXECUTING:
		php_error(E_WARNING, "%s: OCI_STILL_EXECUTING", what);
		break;
	case OCI_CONTINUE:
		php_error(E_WARNING, "%s: OCI_CONTINUE", what);
		break;
	default:
		break;
	}
	return errcode;
}

/* }}} */
/* {{{ oci_ping()  */

static int oci_ping(oci_server *server)
{
	char version[256];
	TSRMLS_FETCH();
	
	CALL_OCI_RETURN(OCI(error), OCIServerVersion(
				server->pServer,
			 	OCI(pError), 
				(text*)version, 
				sizeof(version),
				OCI_HTYPE_SERVER));

	if (OCI(error) == OCI_SUCCESS) {
		return 1;
	}

	oci_error(OCI(pError), "oci_ping", OCI(error));

	return 0;
}

/* }}} */

/************************* INTERNAL FUNCTIONS *************************/

/* {{{ oci_debug() */

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
/* {{{ oci_get_conn() */

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
/* {{{ oci_get_stmt() */

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
/* {{{ oci_get_desc() */

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
/* {{{ oci_get_col() */

static oci_out_column *
oci_get_col(oci_statement *statement, int col, zval **value)
{
	oci_out_column *outcol = NULL;
	int i;

	if (statement->columns == 0) { /* we release the columns at the end of a fetch */
		return NULL;
	}

	if (value) {
		if (Z_TYPE_PP(value) == IS_STRING) {
			for (i = 0; i < statement->ncolumns; i++) {
				outcol = oci_get_col(statement, i + 1, 0);
				if (outcol == NULL) {
					continue;
				} else if (((int) outcol->name_len == Z_STRLEN_PP(value)) 
						&& (! strncmp(outcol->name,Z_STRVAL_PP(value),Z_STRLEN_PP(value)))) {
				return outcol;
				}
			}
		} else {
			convert_to_long_ex(value);
			return oci_get_col(statement,Z_LVAL_PP(value),0);
		}
	} else if (col != -1) {
		if (zend_hash_index_find(statement->columns, col, (void **)&outcol) == FAILURE) {
			php_error(E_WARNING, "Invalid column %d", col);
			return NULL;
		}
		return outcol;
	}

	return NULL;
}

/* }}} */
/* {{{ oci_new_desc() */

static oci_descriptor*
oci_new_desc(int type,oci_connection *connection)
{
	oci_descriptor *descr;
	TSRMLS_FETCH();

	descr = emalloc(sizeof(oci_descriptor));
	
	Z_TYPE_P(descr) = type;

	switch (Z_TYPE_P(descr)) {
		case OCI_DTYPE_FILE:
		case OCI_DTYPE_LOB:
		case OCI_DTYPE_ROWID:
			break;

		default:
			php_error(E_WARNING, "Unknown descriptor type %d.",Z_TYPE_P(descr));
			return 0;
	}
	
	CALL_OCI_RETURN(OCI(error), OCIDescriptorAlloc(
				OCI(pEnv),
				(dvoid*)&(descr->ocidescr), 
				Z_TYPE_P(descr), 
				(size_t) 0, 
				(dvoid **) 0));
	
	if (OCI(error)) {
		ub4 error;
		error = oci_error(OCI(pError),"OCIDescriptorAlloc %d",OCI(error));
		oci_handle_error(connection, error);
		return 0;
	}

	descr->id = zend_list_insert(descr,le_desc);
	descr->conn = connection;
	zend_list_addref(connection->id);

	oci_debug("oci_new_desc %d",descr->id);

	return descr;
}

#ifdef WITH_COLLECTIONS

/* {{{ _oci_get_ocicoll() */

static int
_oci_get_ocicoll(zval *id,oci_collection **collection TSRMLS_DC)
{
	zval **coll;
	
	if (zend_hash_find(Z_OBJPROP_P(id), "collection", sizeof("collection"), (void **)&coll) == FAILURE) {
		php_error(E_WARNING, "cannot find collection");
		return 0;
	}
	if ((*collection = oci_get_coll(Z_LVAL_PP(coll) TSRMLS_CC)) == NULL) {
		php_error(E_WARNING, "collection not found");
		return 0;
	}
 
	return Z_LVAL_PP(coll);
}

/* }}} */

#endif

/* }}} */
/* {{{ _oci_get_ocidesc() */

static int
_oci_get_ocidesc(zval *id,oci_descriptor **descriptor TSRMLS_DC)
{
	zval **desc;
	
	if (zend_hash_find(Z_OBJPROP_P(id), "descriptor", sizeof("descriptor"), (void **)&desc) == FAILURE) {
		php_error(E_WARNING, "cannot find descriptor");
		return 0;
	}

	if ((*descriptor = oci_get_desc(Z_LVAL_PP(desc) TSRMLS_CC)) == NULL) {
		php_error(E_WARNING, "descriptor not found");
		return 0;
	}
	
	return Z_LVAL_PP(desc);
}

/* }}} */
/* {{{ _oci_make_zval() */

static int 
_oci_make_zval(zval *value,oci_statement *statement,oci_out_column *column, char *func, int mode TSRMLS_DC)
{
	oci_descriptor *descr;
	ub4 loblen;
	int size;
	char *buffer;
	
	if (column->indicator || column->retcode)
		if ((column->indicator != -1) && (column->retcode != 1405))
			oci_debug("_oci_make_zval: %16s,retlen = %4d,retlen4 = %d,storage_size4 = %4d,indicator %4d, retcode = %4d",
					  column->name,column->retlen,column->retlen4,column->storage_size4,column->indicator,column->retcode);
	
	if ((! statement->has_data) || (column->indicator == -1)) { /* column is NULL or statment has no current data */
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
			if (! descr) {
				php_error(E_WARNING, "unable to find my descriptor %d",column->data);
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
/* {{{ oci_setprefetch() */

static int
oci_setprefetch(oci_statement *statement,int size)
{ 
	ub4   prefetch;
	sword error;
	TSRMLS_FETCH();

	prefetch = size * 1024;

	CALL_OCI_RETURN(error, OCIAttrSet(
				statement->pStmt,
				OCI_HTYPE_STMT,
			  	&prefetch,
				0, 
				OCI_ATTR_PREFETCH_MEMORY,
				statement->pError));

	statement->error = oci_error(statement->pError, "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY", error);

	if (statement->error) {
		oci_handle_error(statement->conn, statement->error);
	}

	prefetch = size;
	CALL_OCI_RETURN(error, OCIAttrSet(
				statement->pStmt,
				OCI_HTYPE_STMT,
				&prefetch,
				0, 
				OCI_ATTR_PREFETCH_ROWS,
				statement->pError));

	statement->error = oci_error(statement->pError, "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY", error);
	if (statement->error) {
		oci_handle_error(statement->conn, statement->error);
	}

	return 1;
}

/* }}} */
/* {{{ oci_parse() */

static oci_statement *oci_parse(oci_connection *connection, char *query, int len)
{
	oci_statement *statement;
	sword error;
	TSRMLS_FETCH();

	statement = ecalloc(1,sizeof(oci_statement));

	CALL_OCI(OCIHandleAlloc(
				OCI(pEnv),
				(dvoid **)&statement->pStmt,
				OCI_HTYPE_STMT, 
				0, 
				NULL));

	CALL_OCI(OCIHandleAlloc(
				OCI(pEnv),
				(dvoid **)&statement->pError,
				OCI_HTYPE_ERROR,
				0,
				NULL));

	if (len > 0) {
		CALL_OCI_RETURN(error, OCIStmtPrepare(
					statement->pStmt,
					connection->pError,
					(text*)query,
					len,
					OCI_NTV_SYNTAX,
					OCI_DEFAULT));

		connection->error = oci_error(connection->pError, "OCIParse", error);
		if (connection->error) {
			CALL_OCI(OCIHandleFree(
						statement->pStmt, 
						OCI_HTYPE_STMT));
			
			CALL_OCI(OCIHandleFree(
						statement->pError, 
						OCI_HTYPE_ERROR));

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
/* {{{ oci_execute() */

static int
oci_execute(oci_statement *statement, char *func,ub4 mode)
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

	if (! statement->stmttype) {
		CALL_OCI_RETURN(error, OCIAttrGet(
					(dvoid *)statement->pStmt,
					OCI_HTYPE_STMT,
					(ub2 *)&statement->stmttype,
					(ub4 *)0,
					OCI_ATTR_STMT_TYPE,
					statement->pError));

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

		CALL_OCI_RETURN(error, OCIStmtExecute(
					statement->conn->pServiceContext,
					statement->pStmt,
					statement->pError,
					iters,
					0,
					NULL,
					NULL,
					mode));

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

		CALL_OCI_RETURN(error, OCIAttrGet(
					(dvoid *)statement->pStmt,
					OCI_HTYPE_STMT,
					(dvoid *)&colcount,
					(ub4 *)0,
					OCI_ATTR_PARAM_COUNT,
					statement->pError));

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

			CALL_OCI_RETURN(error, OCIParamGet(
						(dvoid *)statement->pStmt,
						OCI_HTYPE_STMT,
						statement->pError,
						(dvoid**)&param,
						counter));

			statement->error = oci_error(statement->pError, "OCIParamGet OCI_HTYPE_STMT", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			CALL_OCI_RETURN(error, OCIAttrGet(
						(dvoid *)param,
						OCI_DTYPE_PARAM,
						(dvoid *)&outcol->data_type,
						(ub4 *)0,
						OCI_ATTR_DATA_TYPE,
						statement->pError));

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_TYPE", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			CALL_OCI_RETURN(error, OCIAttrGet(
						(dvoid *)param,
						OCI_DTYPE_PARAM,
						(dvoid *)&outcol->data_size,
						(dvoid *)0,
						OCI_ATTR_DATA_SIZE,
						statement->pError));

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_SIZE", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we loose memory!!! */
			}

			outcol->storage_size4 = outcol->data_size;
			outcol->retlen = outcol->data_size;
	
			CALL_OCI_RETURN(error, OCIAttrGet(
						(dvoid *)param, 
						OCI_DTYPE_PARAM, 
						(dvoid *)&outcol->scale, 
						(dvoid *)0, 
						OCI_ATTR_SCALE, 
						statement->pError));
						
			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_SCALE", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we lose memory!!! */
			}

			CALL_OCI_RETURN(error, OCIAttrGet(
						(dvoid *)param, 
						OCI_DTYPE_PARAM, 
						(dvoid *)&outcol->precision, 
						(dvoid *)0, 
						OCI_ATTR_PRECISION, 
						statement->pError)); 

			statement->error = oci_error(statement->pError, "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_PRECISION", error);
			if (statement->error) {
				oci_handle_error(statement->conn, statement->error);
				return 0; /* XXX we lose memory!!! */
			}
					
			CALL_OCI_RETURN(error, OCIAttrGet(
						(dvoid *)param, 
						OCI_DTYPE_PARAM, 
						(dvoid **)&colname,					/* XXX this string is NOT zero terminated!!!! */ 
						(ub4 *)&outcol->name_len, 
						(ub4)OCI_ATTR_NAME, 
						statement->pError));

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
				CALL_OCI_RETURN(error, OCIDefineByPos(
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
							OCI_DYNAMIC_FETCH));                        /* IN     mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */

				statement->error = oci_error(statement->pError, "OCIDefineByPos", error);
			} else {
				CALL_OCI_RETURN(error, OCIDefineByPos(
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
							OCI_DEFAULT));                              /* IN     mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */

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
/* {{{ oci_fetch() */

static int
_oci_column_pre_fetch(void *data TSRMLS_DC)
{
	oci_out_column *col = (oci_out_column *) data;
	
	if (col->piecewise) {
		col->retlen4 = 0; 
	}
	
	return 0;
}


static int
oci_fetch(oci_statement *statement, ub4 nrows, char *func TSRMLS_DC)
{
	int i;
	oci_out_column *column;

	if (statement->columns) {
		zend_hash_apply(statement->columns, (apply_func_t) _oci_column_pre_fetch TSRMLS_CC);
	}


	CALL_OCI_RETURN(statement->error, OCIStmtFetch(
				statement->pStmt, 
				statement->pError, 
				nrows, 
				OCI_FETCH_NEXT, 
				OCI_DEFAULT));

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
				if (! column->data) {
					column->data = (text *) emalloc(OCI_PIECE_SIZE);
				} else {
					column->data = erealloc(column->data,column->retlen4 + OCI_PIECE_SIZE);
				}

				column->cb_retlen = OCI_PIECE_SIZE;

				CALL_OCI(OCIStmtSetPieceInfo(
							(void *) column->pDefine, 
							OCI_HTYPE_DEFINE, 
							statement->pError, 
							((char*)column->data) + column->retlen4, 
							&(column->cb_retlen), 
							OCI_NEXT_PIECE, 
							&column->indicator, 
							&column->retcode));
			}
		}

		CALL_OCI_RETURN(statement->error, OCIStmtFetch(
					statement->pStmt, 
					statement->pError, 
					nrows, 
					OCI_FETCH_NEXT, 
					OCI_DEFAULT));

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
			
			if (! column->define) {
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
/* {{{ oci_loadlob() */

#define LOBREADSIZE 1048576l /* 1MB */

static int
oci_loadlob(oci_connection *connection, oci_descriptor *mydescr, char **buffer,ub4 *loblen)
{
	ub4 siz = 0;
	ub4 readlen = 0;
	char *buf;
	TSRMLS_FETCH();

	*loblen = 0;
	
	if (Z_TYPE_P(mydescr) == OCI_DTYPE_FILE) {
		CALL_OCI_RETURN(connection->error, OCILobFileOpen(
					connection->pServiceContext, 
					connection->pError, 
					mydescr->ocidescr,
					OCI_FILE_READONLY));

		if (connection->error) {
			oci_error(connection->pError, "OCILobFileOpen",connection->error);
			oci_handle_error(connection, connection->error);
			return -1;
		}
	}
	
	CALL_OCI_RETURN(connection->error, OCILobGetLength(
				connection->pServiceContext, 
				connection->pError, 
				mydescr->ocidescr, 
				&readlen));

	if (connection->error) {
		oci_error(connection->pError, "OCILobGetLength",connection->error);
		oci_handle_error(connection, connection->error);
		return -1;
	}

	buf = emalloc(readlen + 1);

	while (readlen > 0) { /* thies loop should not be entered on readlen == 0 */
		CALL_OCI_RETURN(connection->error, OCILobRead(
					connection->pServiceContext, 
					connection->pError, 
					mydescr->ocidescr, 
					&readlen,					/* IN/OUT bytes toread/read */ 
					siz + 1,					/* offset (starts with 1) */ 
					(dvoid *) ((char *) buf + siz),	
					readlen,		 			/* size of buffer */ 
					(dvoid *)0, 
					(OCICallbackLobRead) 0, 	/* callback... */ 
					(ub2) 0, 					/* The character set ID of the buffer data. */ 
					(ub1) SQLCS_IMPLICIT));		/* The character set form of the buffer data. */

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

	if (Z_TYPE_P(mydescr) == OCI_DTYPE_FILE) {
		CALL_OCI_RETURN(connection->error, OCILobFileClose(
					connection->pServiceContext, 
					connection->pError, 
					mydescr->ocidescr));

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

	oci_debug("OCIloadlob: size=%d",siz);

	return 0;
}
/* }}} */
/* {{{ oci_failover_callback() */
#if 0 /* not needed yet ! */
static sb4 
oci_failover_callback(dvoid *svchp,
						dvoid* envhp,
						dvoid *fo_ctx,
						ub4 fo_type,
						ub4 fo_event)
{
	/* 
	   this stuff is from an oci sample - it will get cleaned up as soon as i understand it!!! (thies@thieso.net 990420) 
	   right now i cant get oracle to even call it;-(((((((((((
	*/

	switch (fo_event)
		{
		case OCI_FO_BEGIN:
			{
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
			
		case OCI_FO_ABORT:
			{
				printf(" Failover aborted. Failover will not take place.\n");
				break;
			}
			
		case OCI_FO_END:
			{
				printf(" Failover ended ...resuming services\n");
				break;
			}
			
		case OCI_FO_REAUTH:
			{
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
			
			
		case OCI_FO_ERROR:
			{
				printf(" Failover error gotten. Sleeping...\n");
				php_sleep(3);
				/* cannot find this blody define !!! return OCI_FO_RETRY; */
				break;
			}
			
		default:
			{
				printf("Bad Failover Event: %ld.\n",  fo_event);
				break;
			}
		}

	return 0;  
}
#endif
/* }}} */
/* {{{ oci_bind_in_callback() */

static sb4
oci_bind_in_callback(dvoid *ictxp,     /* context pointer */
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

	if (!(phpbind=(oci_bind *)ictxp) || !(val = phpbind->zval)) {
		php_error(E_WARNING, "!phpbind || !phpbind->val");
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

static sb4
oci_bind_out_callback(dvoid *octxp,      /* context pointer */
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

	if (!(phpbind=(oci_bind *)octxp) || !(val = phpbind->zval)) {
		php_error(E_WARNING, "!phpbind || !phpbind->val");
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

static oci_session *_oci_open_session(oci_server* server,char *username,char *password,int persistent,int exclusive)
{
	oci_session *session = 0, *psession = 0;
	OCISvcCtx *svchp = 0;
	char *hashed_details;
	TSRMLS_FETCH();

	/* 
	   check if we already have this user authenticated

	   we will reuse authenticated users within a request no matter if the user requested a persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	hashed_details = (char *) malloc(strlen(SAFE_STRING(username))+
									 strlen(SAFE_STRING(password))+
									 strlen(SAFE_STRING(server->dbname))+1);
	
	sprintf(hashed_details,"%s%s%s",
			SAFE_STRING(username),
			SAFE_STRING(password),
			SAFE_STRING(server->dbname));

	if (! exclusive) {
		zend_hash_find(OCI(user), hashed_details, strlen(hashed_details)+1, (void **) &session);

		if (session) {
			if (session->is_open) {
				if (persistent) {
					session->persistent = 1;
				}
				free(hashed_details);
				return session;
			} else {
				_oci_close_session(session);
				/* breakthru to open */
			}
		}
	}

	session = calloc(1,sizeof(oci_session));

	if (! session) {
		goto CLEANUP;
	}

	session->persistent = persistent;
	session->hashed_details = hashed_details;
	session->server = server;
	session->exclusive = exclusive;

	/* allocate temporary Service Context */
	CALL_OCI_RETURN(OCI(error), OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **)&svchp, 
				OCI_HTYPE_SVCCTX, 
				0, 
				NULL));
	
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "_oci_open_session: OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI(error));
		goto CLEANUP;
	}

	/* allocate private session-handle */
	CALL_OCI_RETURN(OCI(error), OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **)&session->pSession, 
				OCI_HTYPE_SESSION, 
				0, 
				NULL));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "_oci_open_session: OCIHandleAlloc OCI_HTYPE_SESSION", OCI(error));
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	CALL_OCI_RETURN(OCI(error), OCIAttrSet(
				svchp, 
				OCI_HTYPE_SVCCTX, 
				server->pServer, 
				0,
				OCI_ATTR_SERVER,
				OCI(pError)));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "_oci_open_session: OCIAttrSet OCI_ATTR_SERVER", OCI(error));
		goto CLEANUP;
	}

	/* set the username in user handle */
	CALL_OCI_RETURN(OCI(error), OCIAttrSet(
				(dvoid *) session->pSession, 
				(ub4) OCI_HTYPE_SESSION, 
				(dvoid *) username, 
				(ub4) strlen(username), 
				(ub4) OCI_ATTR_USERNAME, 
				OCI(pError)));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCIAttrSet OCI_ATTR_USERNAME", OCI(error));
		goto CLEANUP;
	}

	/* set the password in user handle */
	CALL_OCI_RETURN(OCI(error), OCIAttrSet(
				(dvoid *) session->pSession, 
				(ub4) OCI_HTYPE_SESSION, 
				(dvoid *) password, 
				(ub4) strlen(password), 
				(ub4) OCI_ATTR_PASSWORD, 
				OCI(pError)));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCIAttrSet OCI_ATTR_PASSWORD", OCI(error));
		goto CLEANUP;
	}

	CALL_OCI_RETURN(OCI(error), OCISessionBegin(
				svchp, 
				OCI(pError), 
				session->pSession, 
				(ub4) OCI_CRED_RDBMS, 
				(ub4) OCI_DEFAULT));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCISessionBegin", OCI(error));
		goto CLEANUP;
	}

	/* Free Temporary Service Context */
	CALL_OCI(OCIHandleFree(
				(dvoid *) svchp, 
				(ub4) OCI_HTYPE_SVCCTX));

	if (exclusive) {
		psession = session;
	} else {
		zend_hash_update(OCI(user),
						 session->hashed_details,
						 strlen(session->hashed_details)+1, 
						 (void *)session,
						 sizeof(oci_session),
						 (void**)&psession);
	}

	psession->num = zend_list_insert(psession,le_session);
 	psession->is_open = 1;

	oci_debug("_oci_open_session new sess=%d user=%s",psession->num,username);

	if (! exclusive) free(session);

	return psession;

 CLEANUP:
	oci_debug("_oci_open_session: FAILURE -> CLEANUP called");

	_oci_close_session(session);

	return 0;
}

/* }}} */
/* {{{ _oci_close_session()
 */

static void
_oci_close_session(oci_session *session)
{
	OCISvcCtx *svchp;
	char *hashed_details;
	TSRMLS_FETCH();

	if (! session) {
		return;
	}

	oci_debug("START _oci_close_session: logging-off sess=%d",session->num);

	if (session->is_open) {
		/* Temporary Service Context */
		CALL_OCI_RETURN(OCI(error), OCIHandleAlloc(
					OCI(pEnv), 
					(dvoid **) &svchp, 
					(ub4) OCI_HTYPE_SVCCTX, 
					(size_t) 0, 
					(dvoid **) 0));
		
		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI(error));
		}
		
		/* Set the server handle in service handle */ 
		CALL_OCI_RETURN(OCI(error), OCIAttrSet(
					svchp, 
					OCI_HTYPE_SVCCTX, 
					session->server->pServer, 
					0, 
					OCI_ATTR_SERVER, 
					OCI(pError)));

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session: OCIAttrSet OCI_ATTR_SERVER", OCI(error));
		}
		
		/* Set the Authentication handle in the service handle */
		CALL_OCI_RETURN(OCI(error), OCIAttrSet(
					svchp, 	
					OCI_HTYPE_SVCCTX, 
					session->pSession, 
					0, 
					OCI_ATTR_SESSION, 
					OCI(pError)));

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session: OCIAttrSet OCI_ATTR_SESSION", OCI(error));
		}
	
		CALL_OCI_RETURN(OCI(error), OCISessionEnd(
					svchp, 
					OCI(pError), 
					session->pSession, 
					(ub4) 0));

		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_session: OCISessionEnd", OCI(error));
		}

		CALL_OCI(OCIHandleFree(
					(dvoid *) svchp, 
					(ub4) OCI_HTYPE_SVCCTX));

	} else {
		oci_debug("_oci_close_session: logging-off DEAD session");
	}

	if (session->pSession) {
		CALL_OCI(OCIHandleFree(
					(dvoid *) session->pSession, 
					(ub4) OCI_HTYPE_SESSION));
	}

	hashed_details = session->hashed_details;

	if (! OCI(shutdown)) {
		zend_hash_del(OCI(user), hashed_details, strlen(hashed_details)+1);
	}

	free(hashed_details);
}

/* }}} */
/* {{{ _oci_open_server()
 */

static oci_server *_oci_open_server(char *dbname,int persistent)
{ 
	oci_server *server, *pserver = 0;
	TSRMLS_FETCH();

	/* 
	   check if we already have this server open 
	   
	   we will reuse servers within a request no matter if the user requested persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	zend_hash_find(OCI(server), dbname, strlen(dbname)+1, (void **) &pserver);

	if (pserver) {
		/* XXX ini-flag */
		/*
		if (! oci_ping(pserver)) {
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
	
	server = calloc(1,sizeof(oci_server));

	server->persistent = persistent;
	server->dbname = strdup(SAFE_STRING(dbname));

	CALL_OCI(OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **)&server->pServer, 
				OCI_HTYPE_SERVER, 
				0, 
				NULL)); 
	
	CALL_OCI_RETURN(OCI(error), OCIServerAttach(
				server->pServer, 
				OCI(pError), 
				(text*)server->dbname, 
				strlen(server->dbname), 
				(ub4) OCI_DEFAULT));

	if (OCI(error)) {
		oci_error(OCI(pError), "_oci_open_server", OCI(error));
		goto CLEANUP;
	}
	
	zend_hash_update(OCI(server),
					 server->dbname,
					 strlen(server->dbname)+1, 
					 (void *)server,
					 sizeof(oci_server),
					 (void**)&pserver);

	pserver->num  = zend_list_insert(pserver,le_server);
	pserver->is_open = 1;

	oci_debug("_oci_open_server new conn=%d dname=%s",server->num,server->dbname);

	free(server);

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
	list_entry *le = (list_entry *) data;
	if (Z_TYPE_P(le) == le_session) {
		oci_server *server = ((oci_session*) le->ptr)->server;
		if (server->is_open == 2) 
			return 1;
	}
	return 0;
}


static void
_oci_close_server(oci_server *server)
{
	char *dbname;
	int oldopen;
	TSRMLS_FETCH();

	oldopen = server->is_open;
	server->is_open = 2;
	if (! OCI(shutdown)) {
		zend_hash_apply(&EG(regular_list), (apply_func_t) _oci_session_cleanup TSRMLS_CC);
	}
	server->is_open = oldopen;

	oci_debug("START _oci_close_server: detaching conn=%d dbname=%s",server->num,server->dbname);

	/* XXX close server here */

	if (server->is_open) {
		if (server->pServer && OCI(pError)) {
			CALL_OCI_RETURN(
					OCI(error), 
					OCIServerDetach(server->pServer, 
						OCI(pError), 
						OCI_DEFAULT));

			if (OCI(error)) {
				oci_error(OCI(pError), "oci_close_server OCIServerDetach", OCI(error));
			}
		}
	} else {
		oci_debug("_oci_close_server: closing DEAD server");
	}

	if (server->pServer) {
		CALL_OCI(OCIHandleFree(
					(dvoid *) server->pServer, 
					(ub4) OCI_HTYPE_SERVER));
	}

	dbname = server->dbname;

	if (! OCI(shutdown)) {
		zend_hash_del(OCI(server),dbname,strlen(dbname)+1);
	}

	free(dbname);
}

/* }}} */
/* {{{ oci_do_connect()
  Connect to an Oracle database and log on. returns a new session.
 */
static void oci_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive)
{
	char *username, *password, *dbname;
	zval **userParam, **passParam, **dbParam;
	oci_server *server = 0;
	oci_session *session = 0;
	oci_connection *connection = 0;
	
	if (zend_get_parameters_ex(3, &userParam, &passParam, &dbParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);
		convert_to_string_ex(dbParam);

		username = Z_STRVAL_PP(userParam);
		password = Z_STRVAL_PP(passParam);
		dbname = Z_STRVAL_PP(dbParam);
	} else if (zend_get_parameters_ex(2, &userParam, &passParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);

		username = Z_STRVAL_PP(userParam);
		password = Z_STRVAL_PP(passParam);
		dbname = "";
	} else {
		WRONG_PARAM_COUNT;
	}

	connection = (oci_connection *) ecalloc(1,sizeof(oci_connection));

	if (! connection) {
		goto CLEANUP;
	}

	server = _oci_open_server(dbname,persistent);

	if (! server) {
		goto CLEANUP;
	}

	if (exclusive) {
		/* exlusive session can never be persistent!*/
		persistent = 0;
	} else {
		/* if our server-context is not persistent we can't */
		persistent = server->persistent; 
	}

	session = _oci_open_session(server,username,password,persistent,exclusive);

	if (! session) {
		goto CLEANUP;
	}

	/* set our session */
	connection->session = session;

	/* allocate our private error-handle */
	CALL_OCI_RETURN(OCI(error), OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **)&connection->pError, 
				OCI_HTYPE_ERROR, 
				0, 
				NULL));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_do_connect: OCIHandleAlloc OCI_HTYPE_ERROR",OCI(error));
		goto CLEANUP;
	}

	/* allocate our service-context */
	CALL_OCI_RETURN(OCI(error), OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **)&connection->pServiceContext, 
				OCI_HTYPE_SVCCTX, 
				0, 
				NULL));

	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_do_connect: OCIHandleAlloc OCI_HTYPE_SVCCTX",OCI(error));
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	CALL_OCI_RETURN(connection->error, OCIAttrSet(
				connection->pServiceContext, 
				OCI_HTYPE_SVCCTX, 
				server->pServer, 
				0, 
				OCI_ATTR_SERVER, 
				connection->pError));

	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "oci_do_connect: OCIAttrSet OCI_ATTR_SERVER", connection->error);
		goto CLEANUP;
	}

	/* Set the Authentication handle in the service handle */
	CALL_OCI_RETURN(connection->error, OCIAttrSet(
				connection->pServiceContext, 
				OCI_HTYPE_SVCCTX, 
				session->pSession, 
				0, 
				OCI_ATTR_SESSION, 
				connection->pError));

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

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto bool ocidefinebyname(int stmt, string name, mixed &var [, int type])
   Define a PHP variable to an Oracle column by name */
/* if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!!
 */

PHP_FUNCTION(ocidefinebyname)
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
	Z_TYPE_P(define) = ocitype;
	define->zval = *var;
	zval_add_ref(var);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool ocibindbyname(int stmt, string name, mixed &var, [, int maxlength] [, int type])
   Bind a PHP variable to an Oracle placeholder by name */
/* if you want to bind a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE binding!!!
 */

PHP_FUNCTION(ocibindbyname)
{
	zval **stmt, **name, **var, **maxlen, **type;
	oci_statement *statement;
	oci_statement *bindstmt;
	oci_bind bind, *bindp;
	oci_descriptor *descr;
#ifdef WITH_COLLECTIONS
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
#ifdef WITH_COLLECTIONS
		case SQLT_NTY:
			if(Z_TYPE_PP(var) != IS_OBJECT) {
				php_error(E_WARNING,"Variable must be allocated using OCINewCollection()");
				RETURN_FALSE;
			}
			if ((inx = _oci_get_ocicoll(*var,&coll TSRMLS_CC)) == 0) {
				php_error(E_WARNING,"Variable must be allocated using OCINewCollection()");
				RETURN_FALSE;
			}
			if (! (mycoll = (dvoid *) coll->coll)) {
				php_error(E_WARNING,"Collection empty");
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
				php_error(E_WARNING,"Variable must be allocated using OCINewDescriptor()");
				RETURN_FALSE;
			}

			if ((inx = _oci_get_ocidesc(*var,&descr TSRMLS_CC)) == 0) {
				php_error(E_WARNING,"Variable must be allocated using OCINewDescriptor()");
				RETURN_FALSE;
			}
		
			if (! (mydescr = (dvoid *) descr->ocidescr)) {
				php_error(E_WARNING,"Descriptor empty");
				RETURN_FALSE;
			}
			value_sz = sizeof(void*);
			break;

		case SQLT_RSET:
			OCI_GET_STMT(bindstmt,var);

			if (! (mystmt = bindstmt->pStmt)) {
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

	if (! statement->binds) {
		ALLOC_HASHTABLE(statement->binds);
		zend_hash_init(statement->binds, 13, NULL, _oci_bind_hash_dtor, 0);
	}

	memset((void*)&bind,0,sizeof(oci_bind));
	zend_hash_next_index_insert(statement->binds,&bind,sizeof(oci_bind),(void **)&bindp);

	bindp->descr = mydescr;
	bindp->pStmt = mystmt;
	bindp->zval = *var;
	zval_add_ref(var); 
	
	CALL_OCI_RETURN(statement->error, OCIBindByName(
				statement->pStmt,                /* statement handle */
				(OCIBind **)&bindp->pBind,       /* bind hdl (will alloc) */
				statement->pError,               /* error handle */
				(text*) Z_STRVAL_PP(name),  /* placeholder name */					  
				Z_STRLEN_PP(name),          /* placeholder length */
				(dvoid *)0,                      /* in/out data */
				value_sz, /* OCI_MAX_DATA_SIZE, */               /* max size of input/output data */
				(ub2)ocitype,                    /* in/out data type */
				(dvoid *)&bindp->indicator,      /* indicator (ignored) */
				(ub2 *)0,                        /* size array (ignored) */
				(ub2 *)&bindp->retcode,          /* return code (ignored) */
				(ub4)0,                          /* maxarr_len (PL/SQL only?) */
				(ub4 *)0,                        /* actual array size (PL/SQL only?) */
				mode                             /* mode */
				));

	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIBindByName", statement->error);
		oci_handle_error(statement->conn, statement->error);
		RETURN_FALSE;
	}

	if (mode == OCI_DATA_AT_EXEC) {
		CALL_OCI_RETURN(statement->error, OCIBindDynamic(
					bindp->pBind, 
					statement->pError, 
					(dvoid *)bindp, 
					oci_bind_in_callback, 
					(dvoid *)bindp, 
					oci_bind_out_callback));

		if (statement->error != OCI_SUCCESS) {
			oci_error(statement->pError, "OCIBindDynamic", statement->error);
			oci_handle_error(statement->conn, statement->error);
			RETURN_FALSE;
		}
	}

#ifdef WITH_COLLECTIONS
	if (ocitype == SQLT_NTY) {
		/* Bind object */
		CALL_OCI_RETURN(statement->error, OCIBindObject(
					bindp->pBind, 
					statement->pError, 
					coll->tdo, 
					(dvoid **) &(coll->coll), 
					(ub4 *) 0, 
					(dvoid **) 0, 
					(ub4 *) 0));

		if (statement->error) {
			oci_error(statement->pError, "OCIBindObject", statement->error);
			RETURN_FALSE;
		}
	}
#endif
	
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool ocifreedesc(object lob)
   Deletes large object description */

PHP_FUNCTION(ocifreedesc)
{
	zval *id;
	int inx;
	oci_descriptor *descriptor;

	if ((id = getThis()) != 0) {
		inx = _oci_get_ocidesc(id,&descriptor TSRMLS_CC);
		if (inx) {
			oci_debug("OCIfreedesc: descr=%d",inx);
			zend_list_delete(inx);
			RETURN_TRUE;
		}
	}

	php_error(E_NOTICE, "OCIFreeDesc() should not be called like this. Use $somelob->free() to free a LOB");

  	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocisavelob(object lob)
   Saves a large object */

PHP_FUNCTION(ocisavelob)
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

		if (! mylob) {
			RETURN_FALSE;
		}

		connection = descr->conn;

		offset = 0;	
		if (zend_get_parameters_ex(2, &arg, &oarg) == SUCCESS) {
			convert_to_long_ex(oarg);
			offparam = Z_LVAL_PP(oarg);

			CALL_OCI_RETURN(connection->error, OCILobGetLength(
						connection->pServiceContext, 
						connection->pError, 
						mylob, 
						&curloblen));

			oci_debug("OCIsavedesc: curloblen=%d",curloblen);

			if (offparam == -1) {
				offset = curloblen;
			} else if ((ub4) offparam >= curloblen) {
				php_error(E_WARNING, "Offset smaller than current LOB-Size - appending");
				offset = curloblen;
			} else {
				offset = offparam;
			}
		} else if (zend_get_parameters_ex(1, &arg) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		offset++;
		convert_to_string_ex(arg);
		loblen = Z_STRLEN_PP(arg);
	
		if (loblen < 1) {
			php_error(E_WARNING, "Cannot save a lob which size is less than 1 byte");
			RETURN_FALSE;
		}

		CALL_OCI_RETURN(connection->error, OCILobWrite(
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
					(ub1) SQLCS_IMPLICIT));

		oci_debug("OCIsavedesc: size=%d offset=%d",loblen,offset);

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

/* {{{ proto bool ocisavelobfile(object lob)
   Saves a large object file */

PHP_FUNCTION(ocisavelobfile)
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

		if (! mylob) {
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
			php_error(E_WARNING, "Can't open file %s", filename);
			RETURN_FALSE;
		}

		while ((loblen = read(fp, &buf, sizeof(buf))) > 0) {	
			CALL_OCI_RETURN(connection->error, OCILobWrite(
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
						(ub1) SQLCS_IMPLICIT));

			oci_debug("OCIsavelob: size=%d",loblen);

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

/* {{{ proto string ociloadlob(object lob)
   Loads a large object */

PHP_FUNCTION(ociloadlob)
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
		}
	}

  RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ociwritelobtofile(object lob [, string filename] [, int start] [, int length])
   Writes a large object into a file */

PHP_FUNCTION(ociwritelobtofile)
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

		if (! mylob) {
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
				php_error(E_WARNING, "Can't create file %s", filename);
				goto bail;
			} 
		}
	
		CALL_OCI_RETURN(connection->error, OCILobGetLength(
					connection->pServiceContext, 
					connection->pError, 
					descr->ocidescr, 
					&loblen));

		if (connection->error) {
			oci_error(connection->pError, "OCILobGetLength", connection->error);
			oci_handle_error(connection, connection->error);
			goto bail;
		}
		
		if (Z_TYPE_P(descr) == OCI_DTYPE_FILE) {
			CALL_OCI_RETURN(connection->error, OCILobFileOpen(
						connection->pServiceContext, 
						connection->pError, 
						descr->ocidescr, 
						OCI_FILE_READONLY));

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

		oci_debug("ociwritelobtofile(start = %d, length = %d, loblen = %d",start,length,loblen);

		while (length > 0) {
			ub4 toread;

			if (length > OCI_LOB_READ_BUFFER) {
				toread = OCI_LOB_READ_BUFFER;
			} else {
				toread = length;
			}

			oci_debug("OCILobRead(coffs = %d, toread = %d",coffs,toread);

			CALL_OCI_RETURN(connection->error, OCILobRead(
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
						(ub1) SQLCS_IMPLICIT)); /* The character set form of the buffer data. */
			
			oci_debug("OCILobRead(read - %d",toread);

			if (connection->error) {
				oci_error(connection->pError, "OCILobRead", connection->error);
				oci_handle_error(connection, connection->error);
				goto bail;
			}

			if (fp != -1) {
				if ((ub4) write(fp,buffer,toread) != toread) {
					php_error(E_WARNING, "cannot write file!");
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

		if (Z_TYPE_P(descr) == OCI_DTYPE_FILE) {
			CALL_OCI_RETURN(connection->error,OCILobFileClose(
						connection->pServiceContext, 
						connection->pError, 
						descr->ocidescr)); 
			
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
/* {{{ proto bool ociwritetemporarylob(int stmt, int loc, string var)
   Return the row count of an OCI statement */

PHP_FUNCTION(ociwritetemporarylob)
{
	zval *id, **var;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	ub4 offset = 1;
	ub4 loblen;

	oci_debug ("oci_write_temporary_lob");

	if ((id = getThis()) == 0) {
		RETURN_FALSE;
	}

	if (_oci_get_ocidesc(id,&descr TSRMLS_CC) == 0) {
		RETURN_FALSE;
	}

	mylob = (OCILobLocator *) descr->ocidescr;

	if (! mylob) {
		RETURN_FALSE;
	}

	connection = descr->conn;

	if (zend_get_parameters_ex(1, &var) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(var);

	CALL_OCI_RETURN(connection->error, OCILobCreateTemporary(
				connection->pServiceContext, 
				connection->pError, 
				mylob, 
				OCI_DEFAULT, 
				OCI_DEFAULT, 
				OCI_TEMP_CLOB, 
				OCI_ATTR_NOCACHE, 
				OCI_DURATION_STATEMENT));

	if (connection->error) {
		oci_error(connection->pError, "OCILobCreateTemporary", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error, OCILobOpen(
				connection->pServiceContext, 
				connection->pError, 
				mylob, 
				OCI_LOB_READWRITE));

	if (connection->error) {
		oci_error(connection->pError, "OCILobOpen", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	convert_to_string_ex(var);
	loblen = Z_STRLEN_PP(var);
	
	if (loblen < 1) {
		php_error(E_WARNING, "Cannot save a lob that is less than 1 byte");
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error, OCILobWrite(
				connection->pServiceContext, 
				connection->pError, 
				mylob, 
				(ub4 *) &loblen, 
				(ub4) offset, 
				(dvoid *) Z_STRVAL_PP(var), 
				(ub4) loblen, 
				OCI_ONE_PIECE, 
				(dvoid *)0, 
				(sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0, 
				(ub2) 0, 
				(ub1) SQLCS_IMPLICIT));

	if (connection->error) {
		oci_error(connection->pError, "OCILobWrite", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool ocicloselob(object lob)
   Closes lob descriptor */

PHP_FUNCTION(ocicloselob)
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

			if (! mylob) {
				RETURN_FALSE;
			}

			connection = descriptor->conn;

			CALL_OCI_RETURN(connection->error, OCILobClose(
						connection->pServiceContext, 
						connection->pError, 
						mylob)); 

			if (connection->error) {
				oci_error(connection->pError, "OCILobClose", connection->error);
				oci_handle_error(connection, connection->error);
				RETURN_FALSE;
			}

			connection->error = 
				OCILobIsTemporary(OCI(pEnv),
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

	php_error(E_NOTICE, "OCICloselob() should not be called like this. Use $somelob->close() to close a LOB");

	RETURN_FALSE;
}

/* }}} */
#endif 

/* {{{ proto string ocinewdescriptor(int connection [, int type])
   Initialize a new empty descriptor LOB/FILE (LOB is default) */

PHP_FUNCTION(ocinewdescriptor)
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

	object_init_ex(return_value, oci_lob_class_entry_ptr);
	add_property_resource(return_value, "descriptor", descr->id);
}

/* }}} */

/* {{{ proto bool ocirollback(int conn)
   Rollback the current context */

PHP_FUNCTION(ocirollback)
{
	zval **conn;
	oci_connection *connection;

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	oci_debug("<OCITransRollback");

	CALL_OCI_RETURN(connection->error, OCITransRollback(
				connection->pServiceContext, 
				connection->pError, 
				(ub4) 0));

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

/* {{{ proto bool ocicommit(int conn)
   Commit the current context */

PHP_FUNCTION(ocicommit)
{
	zval **conn;
	oci_connection *connection;

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	oci_debug("<OCITransCommit");

	CALL_OCI_RETURN(connection->error, OCITransCommit(
				connection->pServiceContext, 
				connection->pError, 
				(ub4) 0));

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

/* {{{ proto string ocicolumnname(int stmt, int col)
   Tell the name of a column */

PHP_FUNCTION(ocicolumnname)
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

/* {{{ proto int ocicolumnsize(int stmt, int col)
   Tell the maximum data size of a column */

PHP_FUNCTION(ocicolumnsize)
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

	oci_debug("ocicolumnsize: %16s, retlen = %4d, retlen4 = %d, data_size = %4d, storage_size4 = %4d, indicator %4d, retcode = %4d",
				  outcol->name,outcol->retlen,outcol->retlen4,outcol->data_size,outcol->storage_size4,outcol->indicator,outcol->retcode);
		
	/* Handle data type of LONG */
	if (outcol->data_type == SQLT_LNG){
		RETURN_LONG(outcol->storage_size4);
	} else {
		RETURN_LONG(outcol->data_size);
	}
}

/* }}} */

/* {{{ proto int ocicolumnscale(int stmt, int col)
   Tell the scale of a column */

PHP_FUNCTION(ocicolumnscale)
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

/* {{{ proto int ocicolumnprecision(int stmt, int col)
   Tell the precision of a column */

PHP_FUNCTION(ocicolumnprecision)
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

/* {{{ proto mixed ocicolumntype(int stmt, int col)
   Tell the data type of a column */

PHP_FUNCTION(ocicolumntype)
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

/* {{{ proto mixed ocicolumntyperaw(int stmt, int col)
   Tell the raw oracle data type of a column */

PHP_FUNCTION(ocicolumntyperaw)
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

/* {{{ proto bool ocicolumnisnull(int stmt, int col)
   Tell whether a column is NULL */

PHP_FUNCTION(ocicolumnisnull)
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

/* {{{ proto void ociinternaldebug(int onoff)
   Toggle internal debugging output for the OCI extension */
/* Disables or enables the internal debug output.
 * By default it is disabled.
 */
PHP_FUNCTION(ociinternaldebug)
{
	zval **arg;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg);
	OCI(debug_mode) = Z_LVAL_PP(arg);
}


/* }}} */

/* {{{ proto bool ociexecute(int stmt [, int mode])
   Execute a parsed statement */

PHP_FUNCTION(ociexecute)
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

/* {{{ proto bool ocicancel(int stmt)
   Prepare a new row of data for reading */

PHP_FUNCTION(ocicancel)
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

/* {{{ proto bool ocifetch(int stmt)
   Prepare a new row of data for reading */

PHP_FUNCTION(ocifetch)
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

/* {{{ proto int ocifetchinto(int stmt, array &output [, int mode])
   Fetch a row of result data into an array */

PHP_FUNCTION(ocifetchinto)
{
	zval **stmt, **array, *element, **fmode;
	oci_statement *statement;
	oci_out_column *column;
	ub4 nrows = 1;
	int i, used;
	int mode = OCI_NUM;
	int ac = ZEND_NUM_ARGS();
	
	if (ac < 2 || ac > 3 || zend_get_parameters_ex(ac, &stmt, &array, &fmode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	switch (ac) {
	case 3:
		convert_to_long_ex(fmode);
		mode = Z_LVAL_PP(fmode);
		/* possible breakthru */
	}

	OCI_GET_STMT(statement,stmt);

	if (!oci_fetch(statement, nrows, "OCIFetchInto" TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	zval_dtor(*array);
	if (array_init(*array) == FAILURE) {
		php_error(E_WARNING, "OCIFetchInto: unable to convert arg 2 to array");
		RETURN_FALSE;
	}

	for (i = 0; i < statement->ncolumns; i++) {
		column = oci_get_col(statement, i + 1, 0);
		if (column == NULL) {
			continue;
		}
		
		if ((column->indicator == -1) && ((mode & OCI_RETURN_NULLS) == 0)) {
			continue;
		}
		
		used = 0;
		MAKE_STD_ZVAL(element);
		_oci_make_zval(element,statement,column,"OCIFetchInto",mode TSRMLS_CC);
		
		if ((mode & OCI_NUM) || (! (mode & OCI_ASSOC))) {
			zend_hash_index_update(Z_ARRVAL_PP(array),i,(void *)&element,sizeof(zval*),NULL);
			used=1;
		}

		if (mode & OCI_ASSOC) {
			if (used) {
				element->refcount++;
			}
		  	zend_hash_update(Z_ARRVAL_PP(array),column->name,column->name_len+1,(void *)&element,sizeof(zval*),NULL);
		}
	}

	RETURN_LONG(statement->ncolumns);
}

/* }}} */

/* {{{ proto int ocifetchstatement(int stmt, array &output[, int skip][, int maxrows][, int flags])
   Fetch all rows of result data into an array */

PHP_FUNCTION(ocifetchstatement)
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
		if (! oci_fetch(statement, nrows, "OCIFetchStatement" TSRMLS_CC))
			RETURN_LONG(0);
	}

	if (flags & OCI_FETCHSTATEMENT_BY_ROW) {
		columns = emalloc(statement->ncolumns * sizeof(oci_out_column *));

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

			zend_hash_next_index_insert(Z_ARRVAL_PP(array), &row, sizeof(zval*), NULL),

			rows++;

			if ((maxrows != -1) && (rows == maxrows)) {
				oci_fetch(statement, 0, "OCIFetchStatement" TSRMLS_CC);
				break;
			}
		}

		efree(columns);
	} else { /* default to BY_COLUMN */
		columns = emalloc(statement->ncolumns * sizeof(oci_out_column *));
		outarrs = emalloc(statement->ncolumns * sizeof(zval*));
		
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

/* {{{ proto bool ocifreestatement(int stmt)
   Free all resources associated with a statement */

PHP_FUNCTION(ocifreestatement)
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

/* {{{ proto bool ocilogoff(int conn)
   Disconnect from database */

PHP_FUNCTION(ocilogoff)
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

/* {{{ proto int ocinlogon(string user, string pass [, string db])
   Connect to an Oracle database and log on. returns a new session */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(ocinlogon)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0,1);
}

/* }}} */

/* {{{ proto int ocilogon(string user, string pass [, string db])
   Connect to an Oracle database and log on. Returns a new session.
 */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(ocilogon)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0,0);
}

/* }}} */

/* {{{ proto int ociplogon(string user, string pass [, string db])
   Connect to an Oracle database using a persistent connection and log on. Returns a new session. */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(ociplogon)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1,0);
}

/* }}} */

/* {{{ proto array ocierror([int stmt|conn|global])
   Return the last error of stmt|conn|global. If no error happened returns false. */

PHP_FUNCTION(ocierror)
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
			CALL_OCI_RETURN(statement->error, OCIAttrGet(
				(dvoid *)statement->pStmt,
				OCI_HTYPE_STMT,
				(text *) &sqltext,
				(ub4 *)0,
				OCI_ATTR_STATEMENT,
				statement->pError));

			CALL_OCI_RETURN(statement->error, OCIAttrGet(
				(dvoid *)statement->pStmt,
				OCI_HTYPE_STMT,
				(ub2 *)&errorofs,
				(ub4 *)0,
				OCI_ATTR_PARSE_ERROR_OFFSET,
				statement->pError));
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

	if (! error) { /* no error set in the handle */
		RETURN_FALSE;
	}

	if (! errh) {
		php_error(E_WARNING, "OCIError: unable to find Error handle");
		RETURN_FALSE;
	}

	CALL_OCI(OCIErrorGet(
				errh,
				1,
				NULL,
				&errcode,
				errbuf,
				(ub4) sizeof(errbuf),
				(ub4) OCI_HTYPE_ERROR));

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

/* {{{ proto int ocinumcols(int stmt)
   Return the number of result columns in a statement */

PHP_FUNCTION(ocinumcols)
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

/* {{{ proto int ociparse(int conn, string query)
   Parse a query and return a statement */

PHP_FUNCTION(ociparse)
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

/* {{{ proto int ocisetprefetch(int stmt, int prefetch_rows)
  sets the number of rows to be prefetched on execute to prefetch_rows for stmt */

PHP_FUNCTION(ocisetprefetch)
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

/* {{{ proto bool ocipasswordchange(int conn, string user, string old_password, string new_password)
  changes the password of an account*/

PHP_FUNCTION(ocipasswordchange)
{
	zval **conn, **user_param, **pass_old_param, **pass_new_param;
	text *user, *pass_old, *pass_new;
	oci_connection *connection;

	if (zend_get_parameters_ex(4, &conn, &user_param, &pass_old_param, &pass_new_param) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	user      = Z_STRVAL_PP(user_param);
	pass_old  = Z_STRVAL_PP(pass_old_param);
	pass_new  = Z_STRVAL_PP(pass_new_param);

	OCI_GET_CONN(connection, conn);

	CALL_OCI_RETURN(
		 connection->error
		,OCIPasswordChange(
			 connection->pServiceContext
			,connection->pError
			,user
			,strlen(user)+1
			,pass_old
			,strlen(pass_old)+1
			,pass_new
			,strlen(pass_new)+1
			,OCI_DEFAULT
		)
	);

	if (connection->error == OCI_SUCCESS) {
		RETURN_TRUE;
	}
	else {
		oci_error(connection->pError, "OCIPasswordChange", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto int ocinewcursor(int conn)
   Return a new cursor (Statement-Handle) - use this to bind ref-cursors! */

PHP_FUNCTION(ocinewcursor)
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

/* {{{ proto string ociresult(int stmt, mixed column)
   Return a single column of result data */

PHP_FUNCTION(ociresult)
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

/* {{{ proto string ociserverversion(int conn)
   Return a string containing server version information */

PHP_FUNCTION(ociserverversion)
{
	oci_connection *connection;
	zval **conn;
	char version[256];

	if (zend_get_parameters_ex(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	CALL_OCI_RETURN(connection->error, OCIServerVersion(
				connection->pServiceContext, 
				connection->pError, 
				(text*)version, 
				sizeof(version), 
				OCI_HTYPE_SVCCTX));
	
	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "OCIServerVersion", connection->error);
		oci_handle_error(connection, connection->error);
		RETURN_FALSE;
	}

	RETURN_STRING(version,1);
}

/* }}} */

/* {{{ proto string ocistatementtype(int stmt)
   Return the query type of an OCI statement */
 
/* XXX it would be better with a general interface to OCIAttrGet() */

PHP_FUNCTION(ocistatementtype)
{
	zval **stmt;
	oci_statement *statement;
	ub2 stmttype;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	CALL_OCI_RETURN(statement->error, OCIAttrGet(
				(dvoid *)statement->pStmt, 
				OCI_HTYPE_STMT, 
				(ub2 *)&stmttype, 
				(ub4 *)0, 
				OCI_ATTR_STMT_TYPE, 
				statement->pError));

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

/* {{{ proto int ocirowcount(int stmt)
   Return the row count of an OCI statement */

PHP_FUNCTION(ocirowcount)
{
	zval **stmt;
	oci_statement *statement;
	ub4 rowcount;

	if (zend_get_parameters_ex(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	CALL_OCI_RETURN(statement->error, OCIAttrGet(
				(dvoid *)statement->pStmt, 
				OCI_HTYPE_STMT, 
				(ub2 *)&rowcount, 
				(ub4 *)0, 
				OCI_ATTR_ROW_COUNT,
				statement->pError));

	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIRowCount", statement->error);
		oci_handle_error(statement->conn, statement->error);
		RETURN_FALSE;
	}

	RETURN_LONG(rowcount);
}

/* }}} */

#ifdef WITH_COLLECTIONS
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

/* {{{ proto bool ocifreecollection(object lob)
   Deletes collection object*/

PHP_FUNCTION(ocifreecollection)
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
			oci_debug("OCIfreecollection: coll=%d",inx);

			CALL_OCI_RETURN(connection->error, OCIObjectFree(
						OCI(pEnv), 
						connection->pError, 
						(dvoid *)coll->coll, 
						(ub2)(OCI_OBJECTFREE_FORCE)));

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

/* {{{ proto bool ocicollappend(object collection,value)
   Append an object to the collection */

PHP_FUNCTION(ocicollappend)
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
		if(Z_STRLEN_PP(arg) == 0) {
			CALL_OCI_RETURN(connection->error, OCICollAppend(
				  OCI(pEnv), 
				  connection->pError, 
				  (dword *)0, 
				  &null_ind, 
				  coll->coll));
			if (connection->error) {
				oci_error(connection->pError, "OCICollAppend - NULL", connection->error);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}

		switch(coll->element_typecode) {
			case OCI_TYPECODE_DATE:
				convert_to_string_ex(arg);

				CALL_OCI_RETURN(connection->error, OCIDateFromText(
							connection->pError, 
							Z_STRVAL_PP(arg), 
							Z_STRLEN_PP(arg), 
							0,
							0,
							0,
							0,
							&dt));

				if (connection->error) {
					oci_error(connection->pError, "OCIDateFromText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error, OCICollAppend(
							OCI(pEnv), 
							connection->pError, 
							(dvoid *) &dt, 
							(dvoid *) &new_ind, 
							(OCIColl *) coll->coll));

				if (connection->error) {
					oci_error(connection->pError, "OCICollAppend", connection->error);
					RETURN_FALSE;
				}
				RETURN_TRUE;
				break;
			case OCI_TYPECODE_VARCHAR2 :
				convert_to_string_ex(arg);

				CALL_OCI_RETURN(connection->error, OCIStringAssignText(
							OCI(pEnv), 
							connection->pError, 
							Z_STRVAL_PP(arg), 
							Z_STRLEN_PP(arg), 
							&ocistr));

				if (connection->error) {
					oci_error(connection->pError, "OCIStringAssignText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error, OCICollAppend(
							OCI(pEnv), 
							connection->pError, 
							(dvoid *) ocistr, 
							(dvoid *) &new_ind, 
							(OCIColl *) coll->coll));

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

				CALL_OCI_RETURN(connection->error, OCINumberFromReal(
							connection->pError,
							&ndx, 
							sizeof(double),&num));

				if (connection->error) {
					oci_error(connection->pError, "OCINumberFromReal", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error, OCICollAppend(
							OCI(pEnv), 
							connection->pError, 
							(dvoid *) &num, 
							(dvoid *) &new_ind, 
							(OCIColl *) coll->coll));

				RETURN_TRUE;
				break;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string ocicollgetelem(object collection,ndx)
   Retrieve the value at collection index ndx */

PHP_FUNCTION(ocicollgetelem)
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

		CALL_OCI_RETURN(connection->error, OCICollGetElem(
					OCI(pEnv), 
					connection->pError, 
					coll->coll, 
					ndx, 
					&exists, 
					&elem, 
					(dvoid **)&elemind));

		if (connection->error) {
			oci_error(connection->pError, "OCICollGetElem", connection->error);
			RETURN_NULL();
		}
		
		/* Return false if value does not exist at that location */
		if(exists == 0) {
			php_error(E_WARNING, "OCICollGetElem - Invalid index %d", ndx);
			RETURN_FALSE;
		}

		/* Return null if the value is null */
		if(*elemind == OCI_IND_NULL) {
			RETURN_FALSE;
		} 

		switch (coll->element_typecode) {
			case OCI_TYPECODE_DATE:
				len = 1024;
				CALL_OCI(OCIDateToText(
							connection->pError, 
							elem, 
							0, /* fmt */ 
							0, /* fmt_length */ 
							0, /* lang_name */ 
							0, /* lang_length */ 
							&len,
							buff));

				RETURN_STRINGL(buff,len,1);
			case OCI_TYPECODE_VARCHAR2 :
				ocistr = *(OCIString **)elem;
				str = OCIStringPtr(OCI(pEnv),ocistr); /* XXX not protected against recursion! */
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
				CALL_OCI_RETURN(connection->error, OCINumberToReal(
							connection->pError, 
							(CONST OCINumber *) elem, 
							(uword) sizeof(dnum), 
							(dvoid *) &dnum));

				if (connection->error) {
					oci_error(connection->pError, "OCINumberToReal", connection->error);
					RETURN_FALSE;
				}
				RETURN_DOUBLE(dnum);
				break;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocicollassign(object collection,object)
   Assign a collection from another existing collection */

PHP_FUNCTION(ocicollassign)
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
	
		CALL_OCI_RETURN(connection->error, OCICollAssign(
					OCI(pEnv),
					connection->pError, 
					from_coll->coll,
					coll->coll));

		if (connection->error) {
			oci_error(connection->pError, "OCICollAssignElem", connection->error);
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocicollassignelem(object collection,ndx,val)
   Assign element val to collection at index ndx */

PHP_FUNCTION(ocicollassignelem)
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
		 * Handle NULLS.  For consistency with the rest of the OCI8 library, when
		 * a value passed in is a 0 length string, consider it a null
		 */
		convert_to_string_ex(val);

		if(Z_STRLEN_PP(val) == 0) {
			CALL_OCI_RETURN(connection->error, OCICollAssignElem(
				  OCI(pEnv), 
				  connection->pError, 
				  ndx, 
				  (dword *)0, 
				  &null_ind, 
				  coll->coll));
			if (connection->error) {
				oci_error(connection->pError, "OCICollAssignElem - NULL", connection->error);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}

		switch(coll->element_typecode) {
			case OCI_TYPECODE_DATE:
				convert_to_string_ex(val);
				CALL_OCI_RETURN(connection->error, OCIDateFromText(
							connection->pError, 
							Z_STRVAL_PP(val), 
							Z_STRLEN_PP(val), 
							0,
							0,
							0,
							0,
							&dt));

				if (connection->error) {
					oci_error(connection->pError, "OCIDateFromText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error, OCICollAssignElem(
							OCI(pEnv), 
							connection->pError, 
							ndx, 
							(dword *)&dt, 
							&new_ind, 
							coll->coll));

				if (connection->error) {
					oci_error(connection->pError, "OCICollAssignElem", connection->error);
					RETURN_FALSE;
				}
				break;
			case OCI_TYPECODE_VARCHAR2 :
				convert_to_string_ex(val);

				CALL_OCI_RETURN(connection->error, OCIStringAssignText(
							OCI(pEnv), 
							connection->pError, 
							Z_STRVAL_PP(val), 
							Z_STRLEN_PP(val), 
							&ocistr));

				if (connection->error) {
					oci_error(connection->pError, "OCIStringAssignText", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error, OCICollAssignElem(
							OCI(pEnv), 
							connection->pError, 
							ndx, 
							(dword *)ocistr, 
							&new_ind, 
							coll->coll));

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

				CALL_OCI_RETURN(connection->error, OCINumberFromReal(
							connection->pError,
							&dnum, 
							sizeof(double),
							&num));

				if (connection->error) {
					oci_error(connection->pError, "OCINumberFromReal", connection->error);
					RETURN_FALSE;
				}

				CALL_OCI_RETURN(connection->error, OCICollAssignElem(
							OCI(pEnv), 
							connection->pError, 
							ndx, 
							(dword *)&num, 
							&new_ind, 
							coll->coll));

				if (connection->error) {
					oci_error(connection->pError, "OCICollAssignElem", connection->error);
					RETURN_FALSE;
				}
				RETURN_TRUE;
				break;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int ocicollsize(object collection)
   Return the size of a collection */

PHP_FUNCTION(ocicollsize)
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

		CALL_OCI_RETURN(connection->error, OCICollSize(
					OCI(pEnv),
					coll->conn->pError,
					coll->coll,
					&sz));

		if (connection->error) {
			oci_error(connection->pError, "OCICollSize", connection->error);
			RETURN_FALSE;
		}

		RETURN_LONG(sz);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int ocicollmax(object collection)
   Return the max value of a collection.  For a varray this is the maximum length of the array */

PHP_FUNCTION(ocicollmax)
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
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocicolltrim(object collection,num)
   Trim num elements from the end of a collection */

PHP_FUNCTION(ocicolltrim)
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

		CALL_OCI_RETURN(coll->conn->error, OCICollTrim(
					OCI(pEnv),
					coll->conn->pError,
					Z_LVAL_PP(arg),
					coll->coll));

		if (coll->conn->error) {
			oci_error(coll->conn->pError, "OCICollTrim", coll->conn->error);
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ocinewcollection(int connection, string tdo,[string schema])
   Initialize a new collection */

PHP_FUNCTION(ocinewcollection)
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

	if(ac == 3) {
		convert_to_string_ex(schema);
	}

	coll = emalloc(sizeof(oci_collection));

	OCI_GET_CONN(connection,conn);

	coll->conn = connection;
	coll->id = zend_list_insert(coll,le_coll);
	zend_list_addref(connection->id);

	CALL_OCI_RETURN(connection->error, OCITypeByName(
				OCI(pEnv), 
				connection->pError, 
				connection->pServiceContext, 
				ac==3?(text *)Z_STRVAL_PP(schema):(text *)0, 
				ac==3?(ub4)Z_STRLEN_PP(schema): (ub4)0, 
				(text *) Z_STRVAL_PP(tdo), 
				(ub4)    Z_STRLEN_PP(tdo), 
				(CONST text *) 0, 
				(ub4) 0, 
				OCI_DURATION_SESSION, 
				OCI_TYPEGET_ALL, 
				&(coll->tdo)));

	if (connection->error) {
		oci_error(connection->pError, "OCITypeByName", connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error, OCIHandleAlloc(
				OCI(pEnv), 
				(dvoid **) &dschp1, 
				(ub4) OCI_HTYPE_DESCRIBE, 
				(size_t) 0, 
				(dvoid **) 0)); 

	if (connection->error) {
		oci_error(connection->pError, "OCI_HTYPE_DESCRIBE", connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error, OCIDescribeAny(
				connection->pServiceContext, 
				connection->pError, 
				(dvoid *) coll->tdo, 
				(ub4) 0, 
				OCI_OTYPE_PTR, 
				(ub1)1, 
				(ub1) OCI_PTYPE_TYPE, 
				dschp1));

	if (connection->error) {
		oci_error(connection->pError, "OCI_OTYPE_PTR", connection->error);
		RETURN_FALSE;
	}

	CALL_OCI_RETURN(connection->error, OCIAttrGet(
				(dvoid *) dschp1, 
				(ub4) OCI_HTYPE_DESCRIBE, 
				(dvoid *)&parmp1, 
				(ub4 *)0, 
				(ub4)OCI_ATTR_PARAM,
				connection->pError));

	if (connection->error) {
		oci_error(connection->pError, "OCI_ATTR_PARAM", connection->error);
		RETURN_FALSE;
	}

	/* get the collection type code of the attribute */

	CALL_OCI_RETURN(connection->error, OCIAttrGet(
				(dvoid*) parmp1, 
				(ub4) OCI_DTYPE_PARAM, 
				(dvoid*) &(coll->coll_typecode), 
				(ub4 *) 0, 
				(ub4) OCI_ATTR_COLLECTION_TYPECODE, 
				connection->pError));

	if (connection->error) {
		oci_error(connection->pError, "OCI_ATTR_COLLECTION_TYPECODE", connection->error);
		RETURN_FALSE;
	}

	switch(coll->coll_typecode) {
		case OCI_TYPECODE_TABLE:
		case OCI_TYPECODE_VARRAY:
			CALL_OCI_RETURN(connection->error, OCIAttrGet(
						(dvoid*) parmp1, 
						(ub4) OCI_DTYPE_PARAM, 
						(dvoid*) &parmp2, 
						(ub4 *) 0, 
						(ub4) OCI_ATTR_COLLECTION_ELEMENT, 
						connection->pError));

			if (connection->error) {
				oci_error(connection->pError, "OCI_ATTR_COLLECTION_ELEMENT", connection->error);
				RETURN_FALSE;
			}

			CALL_OCI_RETURN(connection->error, OCIAttrGet(
						(dvoid*) parmp2, 
						(ub4) OCI_DTYPE_PARAM, 
						(dvoid*) &(coll->elem_ref), 
						(ub4 *) 0, 
						(ub4) OCI_ATTR_REF_TDO, 
						connection->pError));

			if (connection->error) {
				oci_error(connection->pError, "OCI_ATTR_REF_TDO", connection->error);
				RETURN_FALSE;
			}

			CALL_OCI_RETURN(connection->error, OCITypeByRef(
						OCI(pEnv), 
						connection->pError, 
						coll->elem_ref, 
						OCI_DURATION_SESSION, 
						OCI_TYPEGET_HEADER, 
						&(coll->element_type)));

			if (connection->error) {
				oci_error(connection->pError, "OCI_TYPEGET_HEADER", connection->error);
				RETURN_FALSE;
			}

			CALL_OCI_RETURN(connection->error, OCIAttrGet(
						(dvoid*) parmp2, 
						(ub4) OCI_DTYPE_PARAM, 
						(dvoid*) &(coll->element_typecode), 
						(ub4 *) 0, 
						(ub4) OCI_ATTR_TYPECODE, 
						connection->pError));

			if (connection->error) {
				oci_error(connection->pError, "OCI_ATTR_TYPECODE", connection->error);
				RETURN_FALSE;
			}
			break;
		default:
			php_error(E_WARNING, "OCINewCollection - Unknown Type %d", coll->coll_typecode);
			break;
	}	

	/* Create object to hold return table */
	CALL_OCI_RETURN(connection->error, OCIObjectNew(
				OCI(pEnv), 
				connection->pError, 
				connection->pServiceContext, 
				OCI_TYPECODE_TABLE, 
				coll->tdo, 
				(dvoid *)0, 
				OCI_DURATION_DEFAULT, 
				TRUE, 
				(dvoid **) &(coll->coll)));

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
