/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Thies C. Arntzen <thies@digicol.de>						  |
   |                                                                      |
   | Initial work sponsored by 											  |
   | Digital Collections, http://www.digicol.de/                          |
   +----------------------------------------------------------------------+
 */

#define OCI_USE_EMALLOC 0		/* set this to 1 if you want to use the php memory manager! */

/* $Id$ */

/* TODO list:
 *
 * - php.ini flags
 * - Error mode (print or shut up?)
 * - OCIPasswordChange()
 * - binding of arrays
 * - Truncate input values to the bind size
 * - Character sets for NCLOBS
 * - split the module into an upper (php-callable) and lower (c-callable) layer!
 * - make_pval needs some cleanup....
 * - NULLS (retcode/indicator) needs some more work for describing & binding
 * - remove all XXXs
 * - clean up and documentation
 * - make OCIInternalDebug accept a mask of flags....
 * - better NULL handling
 * - add some flags to OCIFetchStatement (maxrows etc...)
 */

/* {{{ includes & stuff */

#if defined(COMPILE_DL)
# include "dl/phpdl.h"
#endif

#include "php.h"

#include "ext/standard/head.h"

#if HAVE_OCI8

#include "php3_oci8.h"

/* True globals, no need for thread safety */
static int le_conn; /* active connections */
static int le_stmt; /* active statements */
static zend_class_entry *oci_lob_class_entry_ptr;

#ifndef SQLT_BFILEE
  #define SQLT_BFILEE 114
#endif
#ifndef SQLT_CFILEE
  #define SQLT_CFILEE 115
#endif

#define SAFE_STRING(s) ((s)?(s):"")

#if !(WIN32|WINNT)
#include "build-defs.h"
#endif

#include <fcntl.h>

#include "snprintf.h"

/* }}} */
/* {{{ thread safety stuff */

#ifdef ZTS
int oci_globals_id;
#else
PHP_OCI_API php_oci_globals oci_globals;
#endif

/* }}} */
/* {{{ dynamically loadable module stuff */

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &oci_module_entry; };
#endif /* COMPILE_DL */

/* }}} */
/* {{{ startup/shutdown/info/internal function prototypes */

PHP_MINIT_FUNCTION(oci);
PHP_RINIT_FUNCTION(oci);
PHP_MSHUTDOWN_FUNCTION(oci);
PHP_RSHUTDOWN_FUNCTION(oci);
PHP_MINFO_FUNCTION(oci);

static ub4 oci_error(OCIError *err_p, char *what, sword status);
static int oci_ping(oci_connection *conn);
static void oci_debug(const char *format,...);

static void _oci_connection_dtor(oci_connection *connection);
static void _oci_statement_dtor(oci_statement *statement);

static int _oci_column_dtor(void *data);
static int _oci_descr_dtor(void *data);
static int _oci_define_dtor(void *data);

static oci_connection *oci_get_conn(zval **);
static oci_statement *oci_get_stmt(zval **);
static oci_out_column *oci_get_col(oci_statement *, int, zval **);

static int oci_make_pval(pval *,oci_statement *,oci_out_column *, char *, int mode);
static oci_statement *oci_parse(oci_connection *, char *, int);
static int oci_execute(oci_statement *, char *,ub4 mode,HashTable *);
static int oci_fetch(oci_statement *, ub4, char *);
static ub4 oci_loadlob(oci_connection *, oci_descriptor *, char **);
static int oci_setprefetch(oci_statement *statement,int size);

static void oci_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive);

/* ServerAttach/Detach */
static oci_server *oci_open_server(char *dbname,int persistent);
static void _oci_close_server(oci_server *server);

/* SessionBegin/End */
static oci_session *oci_open_user(oci_server* server,char *username,char *password,int persistent,int exclusive);
static void _oci_close_user(oci_session *session);

/* bind callback functions */
static sb4 oci_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
static sb4 oci_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

#if 0
/* failover callback function */
static sb4 oci_failover_callback(dvoid *svchp,dvoid* envhp,dvoid *fo_ctx,ub4 fo_type, ub4 fo_event);
#endif

/* }}} */
/* {{{ extension function prototypes */

PHP_FUNCTION(ocibindbyname);
PHP_FUNCTION(ocidefinebyname);
PHP_FUNCTION(ocicolumnisnull);
PHP_FUNCTION(ocicolumnname);
PHP_FUNCTION(ocicolumnsize);
PHP_FUNCTION(ocicolumntype);
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

#define OCI_GET_STMT(statement,pval) \
	statement = oci_get_stmt(pval); \
	if (statement == NULL) { \
		RETURN_FALSE; \
	}

#define OCI_GET_CONN(connection,pval) \
	connection = oci_get_conn(pval); \
	if (connection == NULL) { \
		RETURN_FALSE; \
	}


/* }}} */
/* {{{ extension definition structures */

#define OCI_ASSOC     			1<<0
#define OCI_NUM       			1<<1
#define OCI_BOTH      			(OCI_ASSOC|OCI_NUM)

#define OCI_RETURN_NULLS       	1<<2
#define OCI_RETURN_LOBS       	1<<3

static unsigned char a3_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char a2_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };

static zend_function_entry php_oci_functions[] = {
	PHP_FE(ocidefinebyname,  a3_arg_force_ref)
	PHP_FE(ocibindbyname,    a3_arg_force_ref)
	PHP_FE(ocicolumnisnull,  NULL)
	PHP_FE(ocicolumnname,    NULL)
	PHP_FE(ocicolumnsize,    NULL)
	PHP_FE(ocicolumntype,    NULL)
	PHP_FE(ociexecute,       NULL)
	PHP_FE(ocicancel,        NULL)
	PHP_FE(ocifetch,         NULL)
	PHP_FE(ocifetchinto,     a2_arg_force_ref)
	PHP_FE(ocifetchstatement,a2_arg_force_ref)
	PHP_FE(ocifreestatement, NULL)
	PHP_FE(ociinternaldebug, NULL)
	PHP_FE(ocinumcols,       NULL)
	PHP_FE(ociparse,         NULL)
	PHP_FE(ocinewcursor,     NULL)
	PHP_FE(ociresult,        NULL)
	PHP_FE(ociserverversion, NULL)
	PHP_FE(ocistatementtype, NULL)
	PHP_FE(ocirowcount, 	 NULL)
    PHP_FE(ocilogoff,        NULL)
    PHP_FE(ocilogon,         NULL)
    PHP_FE(ocinlogon,        NULL)
    PHP_FE(ociplogon,        NULL)
    PHP_FE(ocierror,         NULL)
    PHP_FE(ocifreedesc,		 NULL)
    PHP_FE(ocisavelob,       NULL)
    PHP_FE(ocisavelobfile,   NULL)
    PHP_FE(ociloadlob,       NULL)
    PHP_FE(ocicommit,        NULL)
    PHP_FE(ocirollback,      NULL)
    PHP_FE(ocinewdescriptor, NULL)
    PHP_FE(ocisetprefetch,   NULL)
    {NULL,NULL,NULL}
};

static zend_function_entry php_oci_lob_class_functions[] = {
    PHP_FALIAS(load,	ociloadlob,       NULL)
    PHP_FALIAS(save,	ocisavelob,       NULL)
    PHP_FALIAS(savefile,ocisavelobfile,   NULL)
    PHP_FALIAS(free,	ocifreedesc,      NULL)
    {NULL,NULL,NULL}
};

php3_module_entry oci8_module_entry = {
    "Oracle-OCI8",        /* extension name */
    php_oci_functions,    /* extension function list */
    PHP_MINIT(oci),       /* extension-wide startup function */
    PHP_MSHUTDOWN(oci),   /* extension-wide shutdown function */
    PHP_RINIT(oci),       /* per-request startup function */
    PHP_RSHUTDOWN(oci),   /* per-request shutdown function */
    PHP_MINFO(oci),       /* information function */
    STANDARD_MODULE_PROPERTIES
};

/* }}} */
/* {{{ debug malloc/realloc/free */

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


#ifdef ZTS
static void php_oci_init_globals(php_oci_globals *oci_globals)
{ 
   	OCI(user_num)   = 1000;
	OCI(server_num) = 2000;

	OCI(user) = malloc(sizeof(HashTable));
	zend_hash_init(OCI(user), 13, NULL, NULL, 1);

	OCI(server) = malloc(sizeof(HashTable));
	zend_hash_init(OCI(server), 13, NULL, NULL, 1); 

	OCIEnvInit(&OCI(pEnv), OCI_DEFAULT, 0, NULL);
	OCIHandleAlloc(OCI(pEnv), 
				   (dvoid **)&OCI(pError),
				   OCI_HTYPE_ERROR, 
				   0, 
				   NULL);
	
}
#endif

PHP_MINIT_FUNCTION(oci)
{
	zend_class_entry oci_lob_class_entry;
	ELS_FETCH();

#ifdef ZTS
	oci_globals_id = ts_allocate_id(sizeof(php_oci_globals), php_oci_init_globals, NULL);
#else
	OCI(user_num)   = 1000;
	OCI(server_num) = 2000;

	OCI(user) = malloc(sizeof(HashTable));
	zend_hash_init(OCI(user), 13, NULL, NULL, 1);

	OCI(server) = malloc(sizeof(HashTable));
	zend_hash_init(OCI(server), 13, NULL, NULL, 1); 

	OCIEnvInit(&OCI(pEnv), OCI_DEFAULT, 0, NULL);
	OCIHandleAlloc(OCI(pEnv), 
				   (dvoid **)&OCI(pError),
				   OCI_HTYPE_ERROR, 
				   0, 
				   NULL);
	
#endif

	le_stmt = register_list_destructors(_oci_statement_dtor, NULL);
	le_conn = register_list_destructors(_oci_connection_dtor, NULL);

	INIT_CLASS_ENTRY(oci_lob_class_entry, "OCI-Lob", php_oci_lob_class_functions);

 	oci_lob_class_entry_ptr = register_internal_class(&oci_lob_class_entry);

/* thies@digicol.de 990203 i do not think that we will need all of them - just in here for completeness for now! */
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

	REGISTER_LONG_CONSTANT("OCI_B_BFILE",SQLT_BFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CFILEE",SQLT_CFILEE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CLOB",SQLT_CLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BLOB",SQLT_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_ROWID",SQLT_RDD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CURSOR",SQLT_RSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BIN",SQLT_BIN, CONST_CS | CONST_PERSISTENT);

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

#ifdef ZTS 
	#define PHP_OCI_INIT_MODE OCI_THREADED
#else
	#define PHP_OCI_INIT_MODE OCI_DEFAULT
#endif

#if OCI_USE_EMALLOC
    OCIInitialize(PHP_OCI_INIT_MODE, NULL, ocimalloc, ocirealloc, ocifree);
#else
    OCIInitialize(PHP_OCI_INIT_MODE, NULL, NULL, NULL, NULL);
#endif

	return SUCCESS;
}

/* ----------------------------------------------------------------- */


PHP_RINIT_FUNCTION(oci)
{
	OCILS_FETCH();
	
	/* XXX NYI
	OCI(num_links) = 
		OCI(num_persistent);
	*/

	OCI(debug_mode) = 0; /* start "fresh" */

    oci_debug("php3_rinit_oci8");

    return SUCCESS;
}

static int _user_pcleanup(oci_session *session)
{
	_oci_close_user(session);

	return 1;
}

static int _server_pcleanup(oci_server *server)
{
	_oci_close_server(server);

	return 1;
}

PHP_MSHUTDOWN_FUNCTION(oci)
{
	OCILS_FETCH();

    oci_debug("php3_mshutdown_oci8");

	zend_hash_apply(OCI(user),(int (*)(void *))_user_pcleanup);
	zend_hash_apply(OCI(server),(int (*)(void *))_server_pcleanup);

	zend_hash_destroy(OCI(user));
	zend_hash_destroy(OCI(server));

	free(OCI(user));
	free(OCI(server));

	OCIHandleFree((dvoid *)OCI(pEnv), OCI_HTYPE_ENV);

	return SUCCESS;
}

static int _stmt_cleanup(list_entry *le)
{
	oci_statement *statement;

    if (le->type == le_stmt) {
        statement = (oci_statement *) le->ptr;

        if (!statement->conn->open) {
            return 1;
        }
    }

    return 0;
}

static int _user_cleanup(oci_session *session)
{
   	if (session->persistent)
		return 0;

	_oci_close_user(session);

	return 1;
}

static int _server_cleanup(oci_server *server)
{
	if (server->persistent)
		return 0;

	_oci_close_server(server);

	return 1;
}

PHP_RSHUTDOWN_FUNCTION(oci)
{
	OCILS_FETCH();

    oci_debug("php3_rshutdown_oci8");

	zend_hash_apply(OCI(user),(int (*)(void *))_user_cleanup);
	zend_hash_apply(OCI(server),(int (*)(void *))_server_cleanup);

	/* XXX free all statements, rollback all outstanding transactions */

    return SUCCESS;
}


PHP_MINFO_FUNCTION(oci)
{
#if !(WIN32|WINNT)
	php_printf("Oracle version: %s<br>\n"
			    "Compile-time ORACLE_HOME: %s<br>\n"
			    "Libraries used: %s",
			    PHP_ORACLE_VERSION, PHP_ORACLE_HOME, PHP_ORACLE_LIBS);
#endif
}

/* }}} */
/* {{{ _oci_define_dtor() */

static int
_oci_define_dtor(void *data)
{
	oci_define *define = (oci_define *) data;

	oci_debug("_oci_define_dtor: %s",define->name);

	if (define->name) {
		efree(define->name);
		define->name = 0;
	}

	return 1;
}

/* }}} */
/* {{{ _oci_column_dtor() */

static int
_oci_column_dtor(void *data)
{	
	oci_out_column *column = (oci_out_column *) data;

	oci_debug("START _oci_column_dtor: %s",column->name);

	if (column->data) {
		if (column->is_descr) {
			zend_hash_index_del(column->statement->conn->descriptors,(int) column->data);
		} else {
			if (column->data) {
				efree(column->data);
			}
		}
	}

	oci_debug("END _oci_column_dtor: %s",column->name);

	if (column->name) {
		efree(column->name);
	}

	return 1;
}

/* }}} */
/* {{{ _oci_statement_dtor() */
 
static void
_oci_statement_dtor(oci_statement *statement)
{
	oci_debug("START _oci_statement_dtor: id=%d last_query=\"%s\"",statement->id,SAFE_STRING(statement->last_query));

 	if (statement->pStmt) {
		OCIHandleFree(statement->pStmt, OCI_HTYPE_STMT);
		statement->pStmt = 0;
	}

	if (statement->pError) {
		OCIHandleFree(statement->pError, OCI_HTYPE_ERROR);
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

	zend_list_delete(statement->conn->id); /* delete one ref from the connection */

	oci_debug("END _oci_statement_dtor: id=%d",statement->id);

	efree(statement);
}

/* }}} */
/* {{{ _oci_connection_dtor() */

static void
_oci_connection_dtor(oci_connection *connection)
{
	/* 
	   as the connection is "only" a in memory service context we do not disconnect from oracle.
	*/

	oci_debug("START _oci_connection_dtor: id=%d",connection->id);

	if (connection->descriptors) {
   		zend_hash_destroy(connection->descriptors);
		efree(connection->descriptors);
	}

	if (connection->pServiceContext) {
		OCIHandleFree((dvoid *) connection->pServiceContext, (ub4) OCI_HTYPE_SVCCTX);
	}

	if (connection->pError) {
		OCIHandleFree((dvoid *) connection->pError, (ub4) OCI_HTYPE_ERROR);
	}

	oci_debug("END _oci_connection_dtor: id=%d",connection->id);

	efree(connection);
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
		case OCI_ERROR:
			OCIErrorGet(err_p, (ub4)1, NULL, &errcode, errbuf,
						(ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
			php_error(E_WARNING, "%s: %s", what, errbuf);
			break;
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

static int oci_ping(oci_connection *conn)
{
	char version[256];

	conn->error = 
		OCIServerVersion(conn->pServiceContext,
						 conn->pError, 
						 (text*)version, 
						 sizeof(version),
						 OCI_HTYPE_SVCCTX);
	if (conn->error != OCI_SUCCESS) {
		return 0;
	}

    return 1;
}

/* }}} */

/************************* INTERNAL FUNCTIONS *************************/

/* {{{ oci_debug() */

static void oci_debug(const char *format,...)
{
	OCILS_FETCH();

    if (OCI(debug_mode)) {
		char buffer[1024];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer)-1, format, args);
		va_end(args);
		buffer[sizeof(buffer)-1] = '\0';
		if (php3_header()) {
			php_printf("OCIDebug: %s<br>\n", buffer);
		}
	}
}

/* }}} */
/* {{{ oci_get_conn() */

static oci_connection *oci_get_conn(zval **conn)
{
	oci_connection *connection;

	connection = (oci_connection *) zend_fetch_resource(conn, -1, "OCI8-Connection", NULL, 1, le_conn);

	if (connection && connection->open) {
		return connection;
	} else {
		return (oci_connection *) NULL;
	}
}

/* }}} */
/* {{{ oci_get_stmt() */

static oci_statement *oci_get_stmt(zval **stmt)
{
	oci_statement *statement;

	statement = (oci_statement *) zend_fetch_resource(stmt, -1, "OCI8-Connection", NULL, 1, le_stmt);

	if (statement && statement->conn->open) {
		return statement;
	} else {
		return (oci_statement *) NULL;
	}
}

/* }}} */
/* {{{ oci_get_col() */

static oci_out_column *
oci_get_col(oci_statement *statement, int col, pval **pval)
{
	oci_out_column *outcol = NULL;
	int i;

	if (statement->columns == 0) { /* we release the columns at the end of a fetch */
		return NULL;
	}

	if (pval) {
	    if ((*pval)->type == IS_STRING) {
			for (i = 0; i < statement->ncolumns; i++) {
		   		outcol = oci_get_col(statement, i + 1, 0);
                if (outcol == NULL) {
         	       continue;
                } else if (((int) outcol->name_len == (*pval)->value.str.len) 
                           && (! strncmp(outcol->name,(*pval)->value.str.val,(*pval)->value.str.len))) {
                	return outcol;   	
			   	}
		   	}
       	} else {
			convert_to_long_ex(pval);
			return oci_get_col(statement,(*pval)->value.lval,0);
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
/* {{{ oci_make_pval() */

static int 
oci_make_pval(pval *value,oci_statement *statement,oci_out_column *column, char *func, int mode)
{
	size_t size;
	oci_descriptor *descr;
	ub4 loblen;
	char *buffer;

	if (column->indicator || column->retcode)
		if ((column->indicator != -1) && (column->retcode != 1405))
			oci_debug("oci_make_pval: %16s,retlen = %4d,retlen4 = %d,storage_size4 = %4d,indicator %4d, retcode = %4d",
					   column->name,column->retlen,column->retlen4,column->storage_size4,column->indicator,column->retcode);

	if (column->indicator == -1) { /* column is NULL */
		var_reset(value); /* XXX we NEED to make sure that there's no data attached to this yet!!! */
		return 0;
	}

	if (column->is_cursor) { /* REFCURSOR -> simply return the statement id */
		value->type = IS_LONG;
		value->value.lval = column->stmtid;
	} else if (column->is_descr) {
		if ((column->data_type != SQLT_RDD) && (mode & OCI_RETURN_LOBS)) {
			/* OCI_RETURN_LOBS means that we want the content of the LOB back instead of the locator */

	        if (zend_hash_index_find(statement->conn->descriptors,(int)  column->data, (void **)&descr) == FAILURE) {
   	        	php_error(E_WARNING, "unable to find my descriptor %d",column->data);
            	return -1;
        	}

			loblen = oci_loadlob(statement->conn,descr,&buffer);

			if (loblen >= 0)	{
				value->type = IS_STRING;
				value->value.str.len = loblen;
				value->value.str.val = buffer;
			} else {
				var_reset(value);
			}
		} else { /* return the locator */
			object_init_ex(value, oci_lob_class_entry_ptr);
			add_property_resource(value, "descriptor", (long) column->data);
			add_property_resource(value, "connection", statement->conn->id);
   			zend_list_addref(statement->conn->id);
		}
	} else {
		switch (column->retcode) {
			case 0: /* intact value */
			    /* 
				   oci_debugcol(column,"OK");
				*/
				if (column->piecewise) {
					size = column->retlen4;
				} else {
					size = column->retlen;
				}
				break;

			default:				/* XXX we SHOULD maybe have a different behaviour for unknown results! */
				var_reset(value);
				return 0;
		}

		value->type = IS_STRING;
		value->value.str.len = size;
		value->value.str.val = estrndup(column->data,size);
	}

	return 0;
}

/* }}} */
/* {{{ oci_setprefetch() */

static int
oci_setprefetch(oci_statement *statement,int size)
{ 
	ub4   prefetch;

	prefetch = size * 1024;
	statement->error = 
		oci_error(statement->pError, 
				   "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY",
				   OCIAttrSet(statement->pStmt,
							  OCI_HTYPE_STMT,
							  &prefetch,
							  0, 
							  OCI_ATTR_PREFETCH_MEMORY,
							  statement->pError));
	prefetch = size;
	statement->error = 
		oci_error(statement->pError, 
				   "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY",
				   OCIAttrSet(statement->pStmt,
							  OCI_HTYPE_STMT,
							  &prefetch,
							  0, 
							  OCI_ATTR_PREFETCH_ROWS,
							  statement->pError));

	return 1;
}

/* }}} */
/* {{{ oci_parse() */

static oci_statement *oci_parse(oci_connection *connection, char *query, int len)
{
	oci_statement *statement;
	OCILS_FETCH();

	statement = ecalloc(1,sizeof(oci_statement));

    OCIHandleAlloc(OCI(pEnv),
				   (dvoid **)&statement->pStmt,
				   OCI_HTYPE_STMT, 
				   0, 
				   NULL);
    OCIHandleAlloc(OCI(pEnv),
				   (dvoid **)&statement->pError,
				   OCI_HTYPE_ERROR,
				   0,
				   NULL);

	if (len > 0) {
		connection->error = 
			oci_error(connection->pError,
					   "OCIParse",
					   OCIStmtPrepare(statement->pStmt,
									  connection->pError,
									  (text*)query,
									  len,
									  OCI_NTV_SYNTAX,
									  OCI_DEFAULT));
		if (connection->error) {
			/* XXX loose memory */
			return 0;
		}
	}

	if (query) {
		statement->last_query = estrdup(query);
	}
	statement->conn = connection;
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
oci_execute(oci_statement *statement, char *func,ub4 mode, HashTable *list)
{
	oci_out_column *outcol;
	oci_out_column column;
	OCIParam *param = 0;
	text *colname;
	ub4 counter;
	ub2 define_type;
	ub2 stmttype;
	ub4 iters;
	ub4 colcount;
	ub2 dynamic;
	dvoid *buf;
	oci_descriptor *pdescr, descr;
	OCILS_FETCH();

	statement->error = 
		oci_error(statement->pError,
				   "OCIAttrGet OCI_HTYPE_STMT/OCI_ATTR_STMT_TYPE",
				   OCIAttrGet((dvoid *)statement->pStmt,
							  OCI_HTYPE_STMT,
							  (ub2 *)&stmttype,
							  (ub4 *)0,
							  OCI_ATTR_STMT_TYPE,
							  statement->pError));

	if (statement->error) {
		return 0;
	}

	if (stmttype == OCI_STMT_SELECT) {
		iters = 0;
	} else {
		iters = 1;
	}

	if (statement->last_query) { /* if we execute refcursors we don't have a query and we don't want to execute!!! */
		statement->error = 
			oci_error(statement->pError,
					   "OCIStmtExecute",
					   OCIStmtExecute(statement->conn->pServiceContext,
									  statement->pStmt,
									  statement->pError,
									  iters,
									  0,
									  NULL,
									  NULL,
									  mode));
		
		switch (statement->error) {
		case 0:
			break;
			
		case 3113: /* ORA-03113: end-of-file on communication channel */
			statement->conn->open = 0;
			statement->conn->session->open = 0;
			statement->conn->session->server->open = 0;
			return 0;
			break;

		default:
			return 0;
			break;
		}
	}

	if (stmttype == OCI_STMT_SELECT && (statement->executed == 0)) {
		/* we only need to do the define step is this very statement is executed the first time! */
		statement->executed++;

		statement->columns = emalloc(sizeof(HashTable));
		if (!statement->columns ||
			zend_hash_init(statement->columns, 13, NULL,_oci_column_dtor, 0) == FAILURE) {
			/* out of memory */
			return 0;
		}

		OCIHandleAlloc(
			OCI(pEnv),
			(dvoid **)&param,
			OCI_DTYPE_PARAM,
			0,
			NULL);


		counter = 1;

		statement->error = 
			oci_error(statement->pError,
					   "OCIAttrGet OCI_HTYPE_STMT/OCI_ATTR_PARAM_COUNT",
					   OCIAttrGet((dvoid *)statement->pStmt,
								  OCI_HTYPE_STMT,
								  (dvoid *)&colcount,
								  (ub4 *)0,
								  OCI_ATTR_PARAM_COUNT,
								  statement->pError));
		if (statement->error) {
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

			statement->error = 
				oci_error(statement->pError,
						   "OCIParamGet OCI_HTYPE_STMT",
						   OCIParamGet(
									   (dvoid *)statement->pStmt,
									   OCI_HTYPE_STMT,
									   statement->pError,
									   (dvoid*)&param,
									   counter));
			if (statement->error) {
				return 0; /* XXX we loose memory!!! */
			}

			statement->error = 
				oci_error(statement->pError, 
						   "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_TYPE",
						   OCIAttrGet((dvoid *)param,
									  OCI_DTYPE_PARAM,
									  (dvoid *)&outcol->data_type,
									  (ub4 *)0,
									  OCI_ATTR_DATA_TYPE,
									  statement->pError));
			if (statement->error) {
				return 0; /* XXX we loose memory!!! */
			}


			statement->error = 
				oci_error(statement->pError,
						   "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_SIZE",
						   OCIAttrGet((dvoid *)param,
									  OCI_DTYPE_PARAM,
									  (dvoid *)&outcol->data_size,
									  (dvoid *)0,
									  OCI_ATTR_DATA_SIZE,
									  statement->pError));
			if (statement->error) {
				return 0; /* XXX we loose memory!!! */
			}

			outcol->storage_size4 = outcol->data_size;
			outcol->retlen = outcol->data_size;

			statement->error = 
				oci_error(statement->pError,
						   "OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_NAME",
						   OCIAttrGet((dvoid *)param,
									  OCI_DTYPE_PARAM,
									  (dvoid **)&colname,					/* XXX this string is NOT zero terminated!!!! */
									  (ub4 *)&outcol->name_len,
									  (ub4)OCI_ATTR_NAME,
									  statement->pError));
			if (statement->error) {
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

					define_type = outcol->data_type;
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
						descr.type = OCI_DTYPE_FILE;
					} else if (outcol->data_type == SQLT_RDD ) {
						descr.type = OCI_DTYPE_ROWID;
					} else {
						descr.type = OCI_DTYPE_LOB;
					}

					OCIDescriptorAlloc(OCI(pEnv),
									   (dvoid *)&(descr.ocidescr), 
									   descr.type,
									   (size_t) 0,
									   (dvoid **) 0);
					
					zend_hash_index_update(statement->conn->descriptors, 
											statement->conn->descriptors_count,
											&descr,
											sizeof(oci_descriptor),
											(void **)&pdescr);
				
					outcol->data = (void *) outcol->statement->conn->descriptors_count++;
					buf = outcol->pdescr = pdescr;
					oci_debug("OCIExecute: new descriptor for %s",outcol->name);
					break;

				case SQLT_LNG:
			    case SQLT_LBI:
					if (outcol->data_type == SQLT_LBI) {
						define_type = SQLT_BIN;
					} else {
						define_type = SQLT_STR;
					}
					outcol->storage_size4 = OCI_MAX_DATA_SIZE;
					outcol->piecewise = 1;
					dynamic = OCI_DYNAMIC_FETCH;
					break;

				case SQLT_BIN:
				default:
					define_type = SQLT_STR;
					if ((outcol->data_type == SQLT_DAT) || (outcol->data_type == SQLT_NUM)) {
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
				statement->error = 
					oci_error(statement->pError,
							   "OCIDefineByPos",
							   OCIDefineByPos(statement->pStmt,				/* IN/OUT handle to the requested SQL query */
											  (OCIDefine **)&outcol->pDefine,/* IN/OUT pointer to a pointer to a define handle */
											  statement->pError, 			/* IN/OUT An error handle  */
											  counter,						/* IN     position in the select list */
											  (dvoid *)NULL,				/* IN/OUT pointer to a buffer */
											  outcol->storage_size4, 	    /* IN	  The size of each valuep buffer in bytes */
											  define_type,			 		/* IN	  The data type */
											  (dvoid *)&outcol->indicator, 	/* IN	  pointer to an indicator variable or arr */
											  (ub2 *)NULL, 		            /* IN/OUT Pointer to array of length of data fetched */
											  (ub2 *)NULL,		            /* OUT	  Pointer to array of column-level return codes */
											  OCI_DYNAMIC_FETCH));	        /* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
			} else {
				statement->error = 
					oci_error(statement->pError,
							   "OCIDefineByPos",
							   OCIDefineByPos(statement->pStmt,				/* IN/OUT handle to the requested SQL query */
											  (OCIDefine **)&outcol->pDefine,/* IN/OUT pointer to a pointer to a define handle */
											  statement->pError, 			/* IN/OUT An error handle  */
											  counter,						/* IN     position in the select list */
											  (dvoid *)buf,					/* IN/OUT pointer to a buffer */
											  outcol->storage_size4, 	    /* IN	  The size of each valuep buffer in bytes */
											  define_type,			 		/* IN	  The data type */
											  (dvoid *)&outcol->indicator, 	/* IN	  pointer to an indicator variable or arr */
											  (ub2 *)&outcol->retlen, 		/* IN/OUT Pointer to array of length of data fetched */
											  (ub2 *)&outcol->retcode,		/* OUT	  Pointer to array of column-level return codes */
											  OCI_DEFAULT));		        /* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
			}
			if (statement->error) {
				return 0; /* XXX we loose memory!!! */
			}
		}
	}

	return 1;
}

/* }}} */
/* {{{ oci_fetch() */

static int
oci_fetch(oci_statement *statement, ub4 nrows, char *func)
{
	int i;
	oci_out_column *column;

	/* first we need to reset our piecewise fetch-infos */
	for (i = 0; i < statement->ncolumns; i++) { 
		column = oci_get_col(statement, i + 1, 0);
		if (column == NULL) { /* should not happen... */
			continue;
		}
			
		if (column->piecewise) {
			column->retlen4 = 0;
		}
	}

	statement->error =
		OCIStmtFetch(statement->pStmt,
					 statement->pError, nrows,
					 OCI_FETCH_NEXT,
					 OCI_DEFAULT);

	if ((statement->error == OCI_NO_DATA) || (nrows == 0)) {
		/* XXX this is needed for REFCURSORS! */
		if (statement->columns) {
			zend_hash_destroy(statement->columns);
			efree(statement->columns);
			statement->columns = 0;
		}
		statement->executed = 0;

		statement->error = 0; /* OCI_NO_DATA is NO error for us!!! */

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

				OCIStmtSetPieceInfo((void *) column->pDefine,
									OCI_HTYPE_DEFINE,
									statement->pError,
									((char*)column->data) + column->retlen4,
									&(column->cb_retlen),
									OCI_NEXT_PIECE,
									NULL,
									NULL);
			}
		}	

		statement->error =
			OCIStmtFetch(statement->pStmt,
					 	 statement->pError, nrows,
					 	 OCI_FETCH_NEXT,
					 	 OCI_DEFAULT);

		for (i = 0; i < statement->ncolumns; i++) {
			column = oci_get_col(statement, i + 1, 0);
			if (column->piecewise)	{
				column->retlen4 += column->cb_retlen;
			}
		}
	}

	if (statement->error == OCI_SUCCESS_WITH_INFO || statement->error == OCI_SUCCESS) {
		/* do the stuff needed for OCIDefineByName */
		for (i = 0; i < statement->ncolumns; i++) {
			column = oci_get_col(statement, i + 1, 0);
			if (column == NULL) { /* should not happen... */
				continue;
			}

			if (! column->define) {
				continue;
			}
			
			pval_destructor(column->define->pval);

			oci_make_pval(column->define->pval,statement,column,"OCIFetch",0);
		}

		return 1;
	}

	oci_error(statement->pError, func, statement->error);

	return 0;
}

/* }}} */
/* {{{ oci_loadlob() */
static ub4
oci_loadlob(oci_connection *connection, oci_descriptor *mydescr, char **buffer)
{
	ub4 loblen;

	connection->error = 
		OCILobGetLength(connection->pServiceContext,
						connection->pError,
						mydescr->ocidescr,
						&loblen);

	if (connection->error) {
		oci_error(connection->pError, "OCILobGetLength", connection->error);
		return -1;
	}
		
	*buffer = emalloc(loblen + 1);

	if (! buffer) {
		return -1;
	}

	if (loblen == 0) {
		(*buffer)[ 0 ] = 0;
		return 0;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		connection->error = 
			OCILobFileOpen(connection->pServiceContext,
						   connection->pError,
						   mydescr->ocidescr,
						   OCI_FILE_READONLY);
		if (connection->error) {
			oci_error(connection->pError, "OCILobFileOpen",connection->error);
			efree(buffer);
			return -1;
		}
	}
		
	connection->error = 
		OCILobRead(connection->pServiceContext, 
				   connection->pError,
				   mydescr->ocidescr,
				   &loblen,				/* IN/OUT bytes toread/read */
				   1,					/* offset (starts with 1) */ 
				   (dvoid *) *buffer,	
				   loblen, 				/* size of buffer */
				   (dvoid *)0,
				   (OCICallbackLobRead) 0, /* callback... */
				   (ub2) 0, 			/* The character set ID of the buffer data. */
				   (ub1) SQLCS_IMPLICIT); /* The character set form of the buffer data. */
	
	if (connection->error) {
		oci_error(connection->pError, "OCILobRead", connection->error);
		efree(buffer);
		return -1;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		connection->error = 
			OCILobFileClose(connection->pServiceContext,
							connection->pError,
							mydescr->ocidescr);
		if (connection->error) {
			oci_error(connection->pError, "OCILobFileClose", connection->error);
			efree(buffer);
			return -1;
		}
	}
		
	(*buffer)[ loblen ] = 0;

	oci_debug("OCIloadlob: size=%d",loblen);

	return loblen;
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
	   this stuff is from an oci sample - it will get cleaned up as soon as i understand it!!! (thies@digicol.de 990420) 
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
				sleep(3);
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
oci_bind_in_callback(dvoid *ictxp, /* context pointer */
					  OCIBind *bindp, /* bind handle */
					  ub4 iter, /* 0-based execute iteration value */
					  ub4 index, /* index of current array for PL/SQL or
									row index for SQL */
					  dvoid **bufpp, /* pointer to data */
					  ub4 *alenp, /* size after value/piece has been read */
					  ub1 *piecep, /* which piece */
					  dvoid **indpp) /* indicator value */
{
	oci_bind *phpbind;
	pval *val;

	phpbind = (oci_bind *)ictxp;

	if (!phpbind || !(val = phpbind->value)) {
		php_error(E_WARNING, "!phpbind || !phpbind->val");
		return OCI_ERROR;
	}

	if ((phpbind->descr == 0) && (phpbind->pStmt == 0)) { 		/* "normal string bind */
		convert_to_string(val); 

		*bufpp = val->value.str.val;
		*alenp = phpbind->maxsize;
		*indpp = (dvoid *)&phpbind->indicator;
	} else if (phpbind->pStmt != 0) {
   		*bufpp = phpbind->pStmt;
		*alenp = -1;		/* seems to be allright */
		*indpp = (dvoid *)&phpbind->indicator;
	} else { /* descriptor bind */
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
oci_bind_out_callback(dvoid *ctxp, /* context pointer */
					   OCIBind *bindp, /* bind handle */
					   ub4 iter, /* 0-based execute iteration value */
					   ub4 index, /* index of current array for PL/SQL or
									 row index for SQL */
					   dvoid **bufpp, /* pointer to data */
					   ub4 **alenpp, /* size after value/piece has been read */
					   ub1 *piecep, /* which piece */
					   dvoid **indpp, /* indicator value */
					   ub2 **rcodepp) /* return code */
{
	oci_bind *phpbind;
	pval *val;

	phpbind = (oci_bind *)ctxp;
	if (!phpbind) {
		oci_debug("oci_bind_out_callback: phpbind = NULL");
		return OCI_ERROR;
	}

	val = phpbind->value;
	if (val == NULL) {
		oci_debug("oci_bind_out_callback: phpbind->value = NULL");
		return OCI_ERROR;
	}

	/* XXX risky, if the variable has been freed, nasty things
	 * could happen here.
	 */

	if (val->type == IS_OBJECT) {
	
	} else if (val->type == IS_STRING) {
		STR_FREE(val->value.str.val);

		phpbind->value->value.str.len = phpbind->maxsize;
		phpbind->value->value.str.val = emalloc(phpbind->maxsize);

		oci_debug("oci_bind_out_callback: maxlen=%d",phpbind->maxsize); 

		*alenpp = (ub4*) &phpbind->value->value.str.len; /* XXX we assume that php-pval len has 4 bytes */
		*bufpp = phpbind->value->value.str.val;
		*piecep = OCI_ONE_PIECE;
		*rcodepp = &phpbind->retcode;
		*indpp = &phpbind->indicator;
	}

	return OCI_CONTINUE;
}

/* }}} */
/* {{{ oci_open_user()
 */

static oci_session *oci_open_user(oci_server* server,char *username,char *password,int persistent,int exclusive)
{
	oci_session *session = 0;
	OCISvcCtx *svchp = 0;
	char *hashed_details;
	int hashed_details_length;
    OCILS_FETCH();

	/* 
	   check if we already have this user authenticated

	   we will reuse authenticated users within a request no matter if the user requested a persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	hashed_details_length = strlen(SAFE_STRING(username))+
		strlen(SAFE_STRING(password))+
		strlen(SAFE_STRING(server->dbname));

	hashed_details = (char *) emalloc(hashed_details_length+1);

	sprintf(hashed_details,"%s%s%s",
			SAFE_STRING(username),
			SAFE_STRING(password),
			SAFE_STRING(server->dbname));

	if (! exclusive) {
		zend_hash_find(OCI(user), hashed_details, hashed_details_length+1, (void **) &session);

		if (session) {
			if (session->open) {
				if (persistent) {
					session->persistent = 1;
				}
				efree(hashed_details);
				return session;
			} else {
				_oci_close_user(session);
				/* breakthru to open */
			}
		}
	}

	session = calloc(1,sizeof(oci_session));

	if (! session) {
		goto CLEANUP;
	}

	session->persistent = persistent;
	session->server = server;

	/* allocate temporary Service Context */
	OCI(error) = 
		OCIHandleAlloc(OCI(pEnv), 
					   (dvoid **)&svchp,
					   OCI_HTYPE_SVCCTX,
					   0,
					   NULL);
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_open_user: OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI(error));
		goto CLEANUP;
	}

	/* allocate private session-handle */
	OCI(error) = 
		OCIHandleAlloc(OCI(pEnv),
					   (dvoid **)&session->pSession,
					   OCI_HTYPE_SESSION,
					   0,
					   NULL);
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_open_user: OCIHandleAlloc OCI_HTYPE_SESSION", OCI(error));
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	OCI(error) = 
		OCIAttrSet(svchp,
				   OCI_HTYPE_SVCCTX,
				   server->pServer,
				   0, 
				   OCI_ATTR_SERVER, 
				   OCI(pError));
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_open_user: OCIAttrSet OCI_ATTR_SERVER", OCI(error));
		goto CLEANUP;
	}

	/* set the username in user handle */
	OCI(error) = 
		OCIAttrSet((dvoid *) session->pSession,
				   (ub4) OCI_HTYPE_SESSION, 
				   (dvoid *) username, 
				   (ub4) strlen(username), 
				   (ub4) OCI_ATTR_USERNAME, 
				   OCI(pError));
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCIAttrSet OCI_ATTR_USERNAME", OCI(error));
		goto CLEANUP;
	}

	/* set the password in user handle */
	OCI(error) = 
		OCIAttrSet((dvoid *) session->pSession,
				   (ub4) OCI_HTYPE_SESSION, 
				   (dvoid *) password, 
				   (ub4) strlen(password), 
				   (ub4) OCI_ATTR_PASSWORD, 
				   OCI(pError));
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCIAttrSet OCI_ATTR_PASSWORD", OCI(error));
		goto CLEANUP;
	}

	OCI(error) = 
		OCISessionBegin(svchp,
						OCI(pError),
						session->pSession, 
						(ub4) OCI_CRED_RDBMS, 
						(ub4) OCI_DEFAULT);
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "OCISessionBegin", OCI(error));
		goto CLEANUP;
	}

	/* Free Temporary Service Context */
	OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX);

	session->num = OCI(user_num)++;

 	session->open = 1;

	oci_debug("oci_open_user new sess=%d user=%s",session->num,username);

	if (exclusive) {
		zend_hash_next_index_pointer_insert(OCI(user),
											 (void *) session);
	} else {
		zend_hash_pointer_update(OCI(user),
								  hashed_details,
								  hashed_details_length+1, 
								  (void *) session);
	}

	efree(hashed_details);

	return session;

 CLEANUP:
	oci_debug("oci_open_user: FAILURE -> CLEANUP called");

	if (hashed_details) {
		efree(hashed_details);
	}

	_oci_close_user(session);

	return 0;
}

/* }}} */
/* {{{ _oci_close_user()
 */

static void
_oci_close_user(oci_session *session)
{
	OCISvcCtx *svchp;
	OCILS_FETCH();

	if (! session) {
		return;
	}

	oci_debug("_oci_close_user: logging-off sess=%d",session->num);

	if (session->open) {
		/* Temporary Service Context */
		OCI(error) = 
			OCIHandleAlloc(OCI(pEnv), 
						   (dvoid **) &svchp,
						   (ub4) OCI_HTYPE_SVCCTX,
						   (size_t) 0, 
						   (dvoid **) 0);
		
		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_user OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI(error));
		}
		
		/* Set the server handle in service handle */ 
		OCI(error) = 
			OCIAttrSet(svchp,
					   OCI_HTYPE_SVCCTX,
					   session->server->pServer,
					   0, 
					   OCI_ATTR_SERVER, 
					   OCI(pError));
		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_user: OCIAttrSet OCI_ATTR_SERVER", OCI(error));
		}
		
		/* Set the Authentication handle in the service handle */
		OCI(error) = 
			OCIAttrSet(svchp, 
					   OCI_HTYPE_SVCCTX,
					   session->pSession,
					   0, 
					   OCI_ATTR_SESSION,
					   OCI(pError));
		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_user: OCIAttrSet OCI_ATTR_SESSION", OCI(error));
		}
		
		OCI(error) = 
			OCISessionEnd(svchp,
						  OCI(pError),
						  session->pSession,
						  (ub4) 0); 
		if (OCI(error) != OCI_SUCCESS) {
			oci_error(OCI(pError), "_oci_close_user: OCISessionEnd", OCI(error));
		}
	} else {
		oci_debug("_oci_close_user: logging-off DEAD session");
	}

	OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX);

	if (session->pSession) {
		OCIHandleFree((dvoid *) session->pSession, (ub4) OCI_HTYPE_SESSION);
	}
	
   	free(session);
}

/* }}} */
/* {{{ _oci_descr_dtor()
 */

static int
_oci_descr_dtor(void *data)
{
	oci_descriptor *descr = (oci_descriptor *) data;

    oci_debug("_oci_descr_dtor: %x",descr->ocidescr);

    OCIDescriptorFree(descr->ocidescr, descr->type);

	return 1;
}
/* }}} */
/* {{{ oci_open_server()
 */
static oci_server *oci_open_server(char *dbname,int persistent)
{ 
	oci_server *server = 0;
	char *hashed_details;
	int hashed_details_length;
    OCILS_FETCH();

	/* 
	   check if we already have this server open 

	   we will reuse servers within a request no matter if the usere requested persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	hashed_details_length = strlen(SAFE_STRING((char *)dbname));
	hashed_details = (char *) emalloc(hashed_details_length+1);
	sprintf(hashed_details,"%s",SAFE_STRING((char *)dbname));

	zend_hash_find(OCI(server), hashed_details, hashed_details_length+1, (void **) &server);

	if (server) {
		if (server->open) {
			/* if our new users uses this connection persistent, we're keeping it! */
			if (persistent) {
				server->persistent = persistent;
			}

			efree(hashed_details);
			return server;
		} else { /* server "died" in the meantime - try to reconnect! */
			_oci_close_server(server);
			/* breakthru to open */
		}
	}

	server = calloc(1,sizeof(oci_server));

	if (! server) {
		goto CLEANUP;
	}

	server->persistent = persistent;

	server->dbname = strdup(SAFE_STRING(dbname));
	
	OCIHandleAlloc(OCI(pEnv), 
				   (dvoid **)&server->pServer, 
				   OCI_HTYPE_SERVER, 
				   0, 
				   NULL); 

	OCI(error) = 
		OCIServerAttach(server->pServer,
						OCI(pError),
						(text*)dbname,
						strlen(dbname),
						(ub4) OCI_DEFAULT);

	if (OCI(error)) {
		oci_error(OCI(pError), "oci_open_server", OCI(error));
		goto CLEANUP;
	}
	
	server->num = OCI(server_num)++;

	server->open = 1;


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
		oci_error(OCI(pError), "oci_open_server OCIAttrSet OCI_ATTR_FOCBK", error);
		goto CLEANUP;
	}
#endif

	oci_debug("oci_open_server new conn=%d dname=%s",server->num,server->dbname);

	zend_hash_pointer_update(OCI(server),
							  hashed_details,
							  hashed_details_length+1, 
							  (void *) server);

	efree(hashed_details);

	return server;

 CLEANUP:
	oci_debug("oci_open_server: FAILURE -> CLEANUP called");

	if (hashed_details) {
		efree(hashed_details);
	}

	_oci_close_server(server);
		
	return 0;
}

/* }}} */
/* {{{ _oci_close_server()
 */

static void
_oci_close_server(oci_server *server)
{
	OCILS_FETCH();

	if (! server) {
		return;
	}

	oci_debug("_oci_close_server: detaching conn=%d dbname=%s",server->num,server->dbname);

	/* XXX close server here */

	if (server->open) {
		if (server->pServer && OCI(pError)) {
			OCI(error) = 
				OCIServerDetach(server->pServer,
								OCI(pError),
								OCI_DEFAULT);
			
			if (OCI(error)) {
				oci_error(OCI(pError), "oci_close_server OCIServerDetach", OCI(error));
			}
		}
	} else {
		oci_debug("_oci_close_server: closing DEAD server");
	}

	if (server->pServer) {
		OCIHandleFree((dvoid *) server->pServer, (ub4) OCI_HTYPE_SERVER);
	}

	if (server->dbname) {
		free(server->dbname);
	}

	free(server);
}

/* }}} */
/* {{{ oci_do_connect()
  Connect to an Oracle database and log on. returns a new session.
 */
static void oci_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive)
{
    char *username, *password, *dbname;
    pval **userParam, **passParam, **dbParam;
    oci_server *server = 0;
    oci_session *session = 0;
    oci_connection *connection = 0;
    OCILS_FETCH();
	
    if (getParametersEx(3, &userParam, &passParam, &dbParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);
		convert_to_string_ex(dbParam);

		username = (*userParam)->value.str.val;
		password = (*passParam)->value.str.val;
		dbname = (*dbParam)->value.str.val;
    } else if (getParametersEx(2, &userParam, &passParam) == SUCCESS) {
		convert_to_string_ex(userParam);
		convert_to_string_ex(passParam);

		username = (*userParam)->value.str.val;
		password = (*passParam)->value.str.val;
		dbname = "";
    } else {
		WRONG_PARAM_COUNT;
    }

	connection = (oci_connection *) ecalloc(1,sizeof(oci_connection));

	if (! connection) {
		goto CLEANUP;
	}

	server = oci_open_server(dbname,persistent);

	if (! server) {
		goto CLEANUP;
	}

	persistent = server->persistent; /* if our server-context is not persistent we can't */

	session = oci_open_user(server,username,password,persistent,exclusive);

	if (! session) {
		goto CLEANUP;
	}

	/* set our session */
	connection->session = session;

	/* allocate our private error-handle */
	OCI(error) = 
		OCIHandleAlloc(OCI(pEnv), 
					   (dvoid **)&connection->pError, 
					   OCI_HTYPE_ERROR, 
					   0, 
					   NULL);
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_do_connect: OCIHandleAlloc OCI_HTYPE_ERROR",OCI(error));
		goto CLEANUP;
	}

	/* allocate our service-context */
	OCI(error) = 
		OCIHandleAlloc(OCI(pEnv), 
					   (dvoid **)&connection->pServiceContext, 
					   OCI_HTYPE_SVCCTX, 
					   0, 
					   NULL);
	if (OCI(error) != OCI_SUCCESS) {
		oci_error(OCI(pError), "oci_do_connect: OCIHandleAlloc OCI_HTYPE_SVCCTX",OCI(error));
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	connection->error = 
		OCIAttrSet(connection->pServiceContext, 
				   OCI_HTYPE_SVCCTX,
				   server->pServer, 
				   0, 
				   OCI_ATTR_SERVER,
				   connection->pError);
	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "oci_do_connect: OCIAttrSet OCI_ATTR_SERVER", connection->error);
		goto CLEANUP;
	}

	/* Set the Authentication handle in the service handle */
	connection->error = 
		OCIAttrSet(connection->pServiceContext, 
				   OCI_HTYPE_SVCCTX,
				   session->pSession,
				   0, 
				   OCI_ATTR_SESSION,
				   connection->pError);
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

	connection->descriptors = emalloc(sizeof(HashTable));
	if (!connection->descriptors ||
		zend_hash_init(connection->descriptors, 13, NULL, _oci_descr_dtor, 0) == FAILURE) {
        goto CLEANUP;
    }

	connection->open = 1;

	oci_debug("oci_do_connect: id=%d",connection->id);

	RETURN_RESOURCE(connection->id);
	
 CLEANUP:
	oci_debug("oci_do_connect: FAILURE -> CLEANUP called");

	if (connection->id) {
		zend_list_delete(connection->id);
	} else {
		_oci_connection_dtor(connection);
	}

	RETURN_FALSE;
}

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto int    OCIDefineByName(int stmt, string name, mixed &var [,int type])
  Define a PHP variable to an Oracle column by name.
  if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!!
 */

PHP_FUNCTION(ocidefinebyname)
{
	pval **stmt, **name, **var, **type;
	oci_statement *statement;
	oci_define *define, *tmp_define;
	ub2	ocitype = SQLT_STR; /* zero terminated string */
	int ac = ARG_COUNT(ht);

    if (ac < 3 || ac > 4 || getParametersEx(ac, &stmt, &name, &var, &type) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    switch (ac) {
	case 4:
		convert_to_long_ex(type);
		ocitype = (ub2) (*type)->value.lval;
		/* possible breakthru */
	}

	OCI_GET_STMT(statement,stmt);

	convert_to_string_ex(name);

	if (statement->defines == NULL) {
		statement->defines = emalloc(sizeof(HashTable));
		zend_hash_init(statement->defines, 13, NULL, _oci_define_dtor, 0);
	}

	define = ecalloc(1,sizeof(oci_define));

	if (zend_hash_add(statement->defines,
					  (*name)->value.str.val,
					  (*name)->value.str.len,
					  define,
					  sizeof(oci_define),
					  (void **)&tmp_define) == SUCCESS) {
		efree(define);
		define = tmp_define;
	} else {
		RETURN_FALSE;
	}

	define->name = (text*) estrndup((*name)->value.str.val,(*name)->value.str.len);
	define->name_len = (*name)->value.str.len;
	define->type = ocitype;
	define->pval = *var;

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int    OCIBindByName(int stmt, string name, mixed &var, int maxlength [,int type])
  Bind a PHP variable to an Oracle placeholder by name.
  if you want to bind a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE binding!!!
 */

PHP_FUNCTION(ocibindbyname)
{
	pval **stmt, **name, **var, **maxlen, **type, **tmp;
	oci_statement *statement;
	oci_statement *bindstmt;
	oci_bind *bind, *tmp_bind;
	oci_descriptor *descr;
	ub2	ocitype = SQLT_STR; /* zero terminated string */
	sb4 ocimaxlen;
    OCIStmt *mystmt = 0;
	dvoid *mydescr = 0;
	int ac = ARG_COUNT(ht);

    if (ac < 4 || ac > 5 || getParametersEx(ac, &stmt, &name, &var, &maxlen, &type) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    switch (ac) {
	case 5:
		convert_to_long_ex(type);
		ocitype = (*type)->value.lval;
		/* possible breakthru */
	}

	convert_to_long_ex(maxlen); 
	ocimaxlen = (*maxlen)->value.lval;

	OCI_GET_STMT(statement,stmt);

	switch ((*var)->type) {
		case IS_OBJECT :
	        if (zend_hash_find((*var)->value.obj.properties, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my descriptor property");
				RETURN_FALSE;
        	}

			if (zend_hash_index_find(statement->conn->descriptors, (*tmp)->value.lval, (void **)&descr) == FAILURE) {
				php_error(E_WARNING, "unable to find my descriptor");
				RETURN_FALSE;
			}
			
			mydescr = (dvoid *) descr->ocidescr;
			
	        if (! mydescr) {
				RETURN_FALSE;
        	}
			break;
		
		default:
			if (ocitype == SQLT_RSET) { /* refcursor binding */
				OCI_GET_STMT(bindstmt,var);
				
				ocimaxlen = 0;
				mystmt = bindstmt->pStmt;
			} else { /* everything else is now handled as a string */ 
				convert_to_string_ex(var);
				if (ocimaxlen == -1) {
					if ((*var)->value.str.len == 0) {
						php_error(E_WARNING, "OCIBindByName bindlength is 0");
					}
					
					if (ocitype == SQLT_BIN) {
						ocimaxlen = (*var)->value.str.len; 
					} else {
						ocimaxlen = (*var)->value.str.len + 1; 
						/* SQLT_STR needs a trailing 0 - maybe we need to resize the var buffers????? */
					}
				}
			}
			break;
	}

	convert_to_string_ex(name);

	bind = ecalloc(1,sizeof(oci_bind));
	if (statement->binds == NULL) {
		statement->binds = emalloc(sizeof(HashTable));
		zend_hash_init(statement->binds, 13, NULL, NULL, 0);
	}
	if (zend_hash_next_index_insert(statement->binds, bind,
									sizeof(oci_bind),
									(void **)&tmp_bind) == SUCCESS) {
		efree(bind);
		bind = tmp_bind;
	}

	bind->value = *var;
	bind->descr = mydescr;
	bind->pStmt = mystmt;
	bind->maxsize = ocimaxlen;

	statement->error = 
		OCIBindByName(statement->pStmt, /* statement handle */
					  (OCIBind **)&bind->pBind, /* bind hdl (will alloc) */
					  statement->pError, /* error handle */
					  (text*) (*name)->value.str.val, /* placeholder name */
					  (*name)->value.str.len, /* placeholder length */
					  (dvoid *)0, /* in/out data */
					  ocimaxlen,	/* max size of input/output data */
					  (ub2)ocitype, /* in/out data type */
					  (dvoid *)&bind->indicator, /* indicator (ignored) */
					  (ub2 *)0, /* size array (ignored) */
					  (ub2 *)&bind->retcode, /* return code (ignored) */
					  (ub4)0, /* maxarr_len (PL/SQL only?) */
					  (ub4 *)0, /* actual array size (PL/SQL only?) */
					  OCI_DATA_AT_EXEC /* mode */);
	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIBindByName", statement->error);
		RETURN_FALSE;
	}
	statement->error = 
		OCIBindDynamic(bind->pBind,
					   statement->pError,
					   (dvoid *)bind,
					   oci_bind_in_callback,
					   (dvoid *)bind,
					   oci_bind_out_callback);
	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIBindDynamic", statement->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string ocifreedesc(object lob)
 */

PHP_FUNCTION(ocifreedesc)
{
	pval *id, **conn, **desc;
	oci_connection *connection;

	if ((id = getThis()) != 0) {
        if (zend_hash_find(id->value.obj.properties, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
            php_error(E_WARNING, "unable to find my statement property");
            RETURN_FALSE;
        }

		OCI_GET_CONN(connection,conn);

   		if (zend_hash_find(id->value.obj.properties, "descriptor", sizeof("descriptor"), (void **)&desc) == FAILURE) {
			php_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

		oci_debug("OCOfreedesc: descr=%d",(*desc)->value.lval);

		zend_hash_index_del(connection->descriptors,(*desc)->value.lval);

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}
/* }}} */
/* {{{ proto string ocisavelob(object lob)
 */

PHP_FUNCTION(ocisavelob)
{
	pval *id, **tmp, **conn, **arg,**oarg;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	int offparam;
	ub4 loblen;
    ub4 curloblen;
    ub4 offset;

	if ((id = getThis()) != 0) {
   		if (zend_hash_find(id->value.obj.properties, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		OCI_GET_CONN(connection,conn);

   		if (zend_hash_find(id->value.obj.properties, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

        if (zend_hash_index_find(connection->descriptors, (*tmp)->value.lval, (void **)&descr) == FAILURE) {
        	php_error(E_WARNING, "unable to find my descriptor %d",(*tmp)->value.lval);
            RETURN_FALSE;
        }

		mylob = (OCILobLocator *) descr->ocidescr;

		if (! mylob) {
			RETURN_FALSE;
		}

		offset = 0;	
	    if (getParametersEx(2, &arg, &oarg) == SUCCESS) {
			convert_to_long_ex(oarg);
			offparam = (*oarg)->value.lval;

			connection->error =
				OCILobGetLength(connection->pServiceContext,
								connection->pError,
								mylob,
								&curloblen);

			oci_debug("OCIsavedesc: curloblen=%d",curloblen);

			if (offparam == -1) {
				offset = curloblen;
			} else if (offparam >= curloblen) {
				php3_error(E_WARNING, "Offset smaller than current LOB-Size - appending");
				offset = curloblen;
			} else {
				offset = offparam;
			}
    	} else if (getParametersEx(1, &arg) == FAILURE) {
        	WRONG_PARAM_COUNT;
    	}

		offset++;
		convert_to_string_ex(arg);
		loblen = (*arg)->value.str.len;
	
		if (loblen < 1) {
			php3_error(E_WARNING, "Cannot save a lob wich size is less than 1 byte");
			RETURN_FALSE;
		}

		connection->error = 
			OCILobWrite(connection->pServiceContext,
					connection->pError,
					mylob,
					&loblen,
					(ub4) offset,
					(dvoid *) (*arg)->value.str.val,
					(ub4) loblen,
					OCI_ONE_PIECE,
					(dvoid *)0,
					(OCICallbackLobWrite) 0,
					(ub2) 0,
					(ub1) SQLCS_IMPLICIT );

		oci_debug("OCIsavedesc: size=%d offset=%d",loblen,offset);

		if (connection->error) {
			oci_error(connection->pError, "OCILobWrite", connection->error);
			RETURN_FALSE;
		}

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}

/* }}} */
/* {{{ proto string ocisavelobfile(object lob)
 */

PHP_FUNCTION(ocisavelobfile)
{
	pval *id, **tmp, **conn, **arg;
	OCILobLocator *mylob;
	oci_connection *connection;
	oci_descriptor *descr;
	char *filename;
	int fp;
	char buf[8192];
	ub4 offset = 1;
	ub4 loblen;

	if ((id = getThis()) != 0) {
   		if (zend_hash_find(id->value.obj.properties, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		OCI_GET_CONN(connection,conn);

   		if (zend_hash_find(id->value.obj.properties, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

        if (zend_hash_index_find(connection->descriptors, (*tmp)->value.lval, (void **)&descr) == FAILURE) {
        	php_error(E_WARNING, "unable to find my descriptor %d",(*tmp)->value.lval);
            RETURN_FALSE;
        }

		mylob = (OCILobLocator *) descr->ocidescr;

		if (! mylob) {
			RETURN_FALSE;
		}

	    if (getParametersEx(1, &arg) == FAILURE) {
        	WRONG_PARAM_COUNT;
    	}

		convert_to_string_ex(arg);

		if (_php3_check_open_basedir((*arg)->value.str.val)) {
			RETURN_FALSE;
		}

		filename = (*arg)->value.str.val;

		if ((fp = open(filename, O_RDONLY)) == -1) {
			php_error(E_WARNING, "Can't open file %s", filename);
			RETURN_FALSE;
        } 

		while ((loblen = read(fp, &buf, sizeof(buf))) > 0) {	
			connection->error = 
				OCILobWrite(connection->pServiceContext,
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
							(ub1) SQLCS_IMPLICIT);

			oci_debug("OCIsavelob: size=%d",loblen);

			if (connection->error) {
				oci_error(connection->pError, "OCILobWrite", connection->error);
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
 */

PHP_FUNCTION(ociloadlob)
{
	pval *id, **tmp, **conn;
	oci_connection *connection;
	oci_descriptor *descr;
	char *buffer;
	ub4 loblen;

	if ((id = getThis()) != 0) {
   		if (zend_hash_find(id->value.obj.properties, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		OCI_GET_CONN(connection,conn);

   		if (zend_hash_find(id->value.obj.properties, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
			php_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

        if (zend_hash_index_find(connection->descriptors, (*tmp)->value.lval, (void **)&descr) == FAILURE) {
        	php_error(E_WARNING, "unable to find my descriptor %d",(*tmp)->value.lval);
            RETURN_FALSE;
        }

		loblen = oci_loadlob(connection,descr,&buffer);

		if (loblen >= 0) {
	 		RETURN_STRINGL(buffer,loblen,0);
		}
	}

  RETURN_FALSE;
}
/* }}} */
/* {{{ proto string OCINewDescriptor(int connection [,int type ])
  initialize a new empty descriptor LOB/FILE (LOB is default)
 */

PHP_FUNCTION(ocinewdescriptor)
{
	pval **conn, **type;
	oci_connection *connection;
	oci_descriptor descr;
	int mylob;

	OCILS_FETCH();

    descr.type = OCI_DTYPE_LOB;

	if (getParametersEx(2, &conn, &type) == SUCCESS) {
		convert_to_long_ex(type);
		descr.type = (*type)->value.lval;
	} else if (getParametersEx(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (descr.type) {
	    case OCI_DTYPE_FILE:
	    case OCI_DTYPE_LOB:
	    case OCI_DTYPE_ROWID:
			break;
		   
	    default:
			php_error(E_WARNING, "Unknown descriptor type %d.",descr.type);
			RETURN_FALSE;
	}

	OCI_GET_CONN(connection,conn);

	OCI(error) = 
		OCIDescriptorAlloc(OCI(pEnv),
						   (dvoid*)&(descr.ocidescr), 
						   descr.type, 
						   (size_t) 0, 
						   (dvoid **) 0);

	if (OCI(error)) {
		oci_error(OCI(pError),"OCIDescriptorAlloc %d",OCI(error));
		RETURN_FALSE;
	}

	zend_hash_index_update(connection->descriptors, connection->descriptors_count,&descr,sizeof(oci_descriptor),NULL);

	mylob = connection->descriptors_count++;

	oci_debug("OCINewDescriptor: new descriptor for %d -> %x",mylob,descr.ocidescr);
	
	object_init_ex(return_value, oci_lob_class_entry_ptr);
	add_property_resource(return_value, "descriptor", (long) mylob);
	add_property_resource(return_value, "connection", connection->id);
	zend_list_addref(connection->id);
}

/* }}} */
/* {{{ proto string OCIRollback(int conn)
  rollback the current context
 */

PHP_FUNCTION(ocirollback)
{
	pval **conn;
	oci_connection *connection;

	if (getParametersEx(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	connection->error = 
		OCITransRollback(connection->pServiceContext,
						 connection->pError, 
						 (ub4)0);

	if (connection->error) {
		oci_error(connection->pError, "OCIRollback", connection->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string OCICommit(int conn)
  commit the current context
 */

PHP_FUNCTION(ocicommit)
{
	pval **conn;
	oci_connection *connection;

	if (getParametersEx(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	connection->error = 
		OCITransCommit(connection->pServiceContext,
					   connection->pError, 
					   (ub4)0);

	if (connection->error) {
		oci_error(connection->pError, "OCICommit", connection->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string OCIColumnName(int stmt, int col)
  Tell the name of a column.
 */

PHP_FUNCTION(ocicolumnname)
{
	pval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (getParametersEx(2, &stmt, &col) == FAILURE) {
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
/* {{{ proto int    OCIColumnSize(int stmt, int col)
  Tell the maximum data size of a column.
 */

PHP_FUNCTION(ocicolumnsize)
{
	pval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (getParametersEx(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(outcol->data_size);
}

/* }}} */
/* {{{ proto mixed  OCIColumnType(int stmt, int col)
  Tell the data type of a column.
 */

PHP_FUNCTION(ocicolumntype)
{
	pval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (getParametersEx(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	switch (outcol->data_type) {
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
/* {{{ proto int    OCIColumnIsNULL(int stmt, int col)
  Tell whether a column is NULL.
 */

PHP_FUNCTION(ocicolumnisnull)
{
	pval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol;

	if (getParametersEx(2, &stmt, &col) == FAILURE) {
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
/* {{{ Proto void   OCIDebug(int onoff)
  Toggle internal debugging output for the OCI extension.
 */

/* Disables or enables the internal debug output.
 * By default it is disabled.
 */
PHP_FUNCTION(ociinternaldebug)
{
	pval **arg;
	OCILS_FETCH();

	if (getParametersEx(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg);
	OCI(debug_mode) = (*arg)->value.lval;
}


/* }}} */
/* {{{ proto int    OCIExecute(int stmt [,int mode])
  Execute a parsed statement.
 */

PHP_FUNCTION(ociexecute)
{
	pval **stmt,**mode;
	oci_statement *statement;
	ub4 execmode;

	if (getParametersEx(2, &stmt, &mode) == SUCCESS) {
		convert_to_long_ex(mode);
		execmode = (*mode)->value.lval;
	} else if (getParametersEx(1, &stmt) == SUCCESS) {
		execmode = OCI_COMMIT_ON_SUCCESS;
	} else {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	if (oci_execute(statement, "OCIExecute",execmode,list)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCICancel(int stmt)
  Prepare a new row of data for reading.
 */

PHP_FUNCTION(ocicancel)
{
	pval **stmt;
	oci_statement *statement;

	if (getParametersEx(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	if (oci_fetch(statement, 0, "OCICancel")) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCIFetch(int stmt)
  Prepare a new row of data for reading.
 */

PHP_FUNCTION(ocifetch)
{
	pval **stmt;
	oci_statement *statement;
	ub4 nrows = 1; /* only one row at a time is supported for now */

	if (getParametersEx(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	if (oci_fetch(statement, nrows, "OCIFetch")) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCIFetchInto(int stmt, array &output [, int mode])
  Fetch a row of result data into an array.
 */

PHP_FUNCTION(ocifetchinto)
{
	pval **stmt, **array, *element, **fmode;
	oci_statement *statement;
	oci_out_column *column;
	ub4 nrows = 1;
	int i;
	int mode = OCI_NUM;
	int ac = ARG_COUNT(ht);

    if (ac < 2 || ac > 3 || getParametersEx(ac, &stmt, &array, &fmode) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	
    switch (ac) {
	case 3:
		convert_to_long_ex(fmode);
		mode = (*fmode)->value.lval;
		/* possible breakthru */
	}

	OCI_GET_STMT(statement,stmt);

	if (!oci_fetch(statement, nrows, "OCIFetchInto")) {
		RETURN_FALSE;
	}

	/* 
	   if we don't want NULL columns back, we need to recreate the array
	   as it could have a different number of enties for each fetched row
	*/

	if (! (mode & OCI_RETURN_NULLS)) { 
		if ((*array)->type == IS_ARRAY) { 
			/* XXX is that right?? */
			zend_hash_destroy((*array)->value.ht);
			efree((*array)->value.ht);
			var_reset(*array);
		}
	}

	if ((*array)->type != IS_ARRAY) {
		pval_destructor(*array);
		if (array_init(*array) == FAILURE) {
			php_error(E_WARNING, "OCIFetchInto: unable to convert arg 2 to array");
			RETURN_FALSE;
		}
	}

	zend_hash_internal_pointer_reset((*array)->value.ht);

	for (i = 0; i < statement->ncolumns; i++) {
		column = oci_get_col(statement, i + 1, 0);
		if (column == NULL) { /* should not happen... */
			continue;
		}
	
		if ((column->indicator == -1) && ((mode & OCI_RETURN_NULLS) == 0)) {
			continue;
		}

		if ((mode & OCI_NUM) || (! (mode & OCI_ASSOC))) { /* OCI_NUM is default */
			MAKE_STD_ZVAL(element);

			oci_make_pval(element,statement,column, "OCIFetchInto",mode);

			zend_hash_index_update((*array)->value.ht, i, (void *)&element, sizeof(pval*), NULL);
		}

		if (mode & OCI_ASSOC) {
			MAKE_STD_ZVAL(element);

			oci_make_pval(element,statement,column, "OCIFetchInto",mode);

		  	zend_hash_update((*array)->value.ht, column->name, column->name_len+1, (void *)&element, sizeof(pval*), NULL);
		}
	}

	RETURN_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ proto int    OCIFetchStatement(int stmt, array &output)
  Fetch all rows of result data into an array.
 */

PHP_FUNCTION(ocifetchstatement)
{
	pval **stmt, **array, *element, **fmode, *tmp;
	oci_statement *statement;
	oci_out_column **columns;
	pval ***outarrs;
	ub4 nrows = 1;
	int i;
	int mode = OCI_NUM;
	int rows = 0;
	char namebuf[ 128 ];
	int ac = ARG_COUNT(ht);

	if (ac < 2 || ac > 3 || getParametersEx(ac, &stmt, &array, &fmode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
    switch (ac) {
	case 3:
		convert_to_long_ex(fmode);
		mode = (*fmode)->value.lval;
		/* possible breakthru */
	}

	OCI_GET_STMT(statement,stmt);

	pval_destructor(*array);
	array_init(*array);

	columns = emalloc(statement->ncolumns * sizeof(oci_out_column *));
	outarrs = emalloc(statement->ncolumns * sizeof(pval*));

	for (i = 0; i < statement->ncolumns; i++) {
		columns[ i ] = oci_get_col(statement, i + 1, 0);

		MAKE_STD_ZVAL(tmp);

		array_init(tmp);

		memcpy(namebuf,columns[ i ]->name, columns[ i ]->name_len);
		namebuf[ columns[ i ]->name_len ] = 0;
				
		zend_hash_update((*array)->value.ht, namebuf, columns[ i ]->name_len+1, (void *) &tmp, sizeof(pval*), (void **) &(outarrs[ i ]));
	}

	while (oci_fetch(statement, nrows, "OCIFetchStatement")) {
		for (i = 0; i < statement->ncolumns; i++) {
			MAKE_STD_ZVAL(element);

			oci_make_pval(element,statement,columns[ i ], "OCIFetchStatement",OCI_RETURN_LOBS);

			zend_hash_index_update((*(outarrs[ i ]))->value.ht, rows, (void *)&element, sizeof(pval*), NULL);
		}
		rows++;
	}
	
	efree(columns);
	efree(outarrs);

	RETURN_LONG(rows);
}

/* }}} */
/* {{{ proto int    OCIFreeStatement(int stmt)
  Free all resources associated with a statement.
 */

PHP_FUNCTION(ocifreestatement)
{
	pval **stmt;
	oci_statement *statement;

	if (getParametersEx(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	zend_list_delete(statement->id);

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int    OCILogoff(int conn)
  Disconnect from database.
 */

/* Logs off and disconnects.
 */
PHP_FUNCTION(ocilogoff)
{
	oci_connection *connection;
	pval **conn;

	if (getParametersEx(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	connection->open = 0;

	zend_hash_apply(list,(int (*)(void *))_stmt_cleanup);

	if (zend_list_delete(connection->id) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCILogon(string user, string pass[, string db])
  Connect to an Oracle database and log on. returns a new session.
 */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(ocinlogon)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0,1);
}

/* }}} */
/* {{{ proto int    OCILogon(string user, string pass[, string db])
  Connect to an Oracle database and log on. returns a new session.
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
/* {{{ proto int    OCIPLogon(string user, string pass[, string db])
  Connect to an Oracle database and log on. returns a new session.
 */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(ociplogon)
{
	oci_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1,0);
}

/* }}} */
/* {{{ proto int    OCIError([int stmt|conn])
  Return the last error of stmt|conn|global. If no error happened returns false.
 */

PHP_FUNCTION(ocierror)
{
	pval **arg;
	oci_statement *statement;
	oci_connection *connection;
    text errbuf[512];
    sb4 errcode = 0;
	sword error = 0;
	dvoid *errh = NULL;

	OCILS_FETCH();

	if (getParametersEx(1, &arg) == SUCCESS) {
		statement = (oci_statement *) zend_fetch_resource(arg, -1, NULL, NULL, 1, le_stmt);
		if (statement) {
			errh = statement->pError;
			error = statement->error;
		} else {
			connection = (oci_connection *) zend_fetch_resource(arg, -1, NULL, NULL, 1, le_conn);
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

	OCIErrorGet(errh,1,NULL,&errcode,errbuf,(ub4)sizeof(errbuf),(ub4)OCI_HTYPE_ERROR);

	if (errcode) {
		array_init(return_value);
		add_assoc_long(return_value, "code", errcode);
		add_assoc_string(return_value, "message", (char*) errbuf, 1);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCINumCols(int stmt)
  Return the number of result columns in a statement.
 */

PHP_FUNCTION(ocinumcols)
{
	pval **stmt;
	oci_statement *statement;

	if (getParametersEx(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	RETURN_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ proto int    OCIParse(int conn, string query)
  Parse a query and return a statement.
 */

PHP_FUNCTION(ociparse)
{
	pval **conn, **query;
	oci_connection *connection;
	oci_statement *statement;

	if (getParametersEx(2, &conn, &query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	convert_to_string_ex(query);

	statement = oci_parse(connection,(*query)->value.str.val,(*query)->value.str.len);

	RETURN_RESOURCE(statement->id);
}

/* }}} */
/* {{{ proto int    OCIParse(int conn, string query)
  Parse a query and return a statement.
 */

PHP_FUNCTION(ocisetprefetch)
{
	pval **stmt, **size;
	oci_statement *statement;

	if (getParametersEx(2, &stmt, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(size);

	OCI_GET_STMT(statement,stmt);

	oci_setprefetch(statement,(*size)->value.lval); 

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int    OCINewCursor(int conn)
   return a new cursor (Statement-Handle) - use this to bind ref-cursors!
 
 */

PHP_FUNCTION(ocinewcursor)
{
	pval **conn;
	oci_connection *connection;
	oci_statement *statement;

	if (getParametersEx(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	statement = oci_parse(connection,0,0);
	
	RETURN_RESOURCE(statement->id);
}

/* }}} */
/* {{{ proto string OCIResult(int stmt, mixed column)
  Return a single column of result data.
 */

PHP_FUNCTION(ociresult)
{
	pval **stmt, **col;
	oci_statement *statement;
	oci_out_column *outcol = NULL;

	if (getParametersEx(2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	outcol = oci_get_col(statement, -1, col);

	if (outcol == NULL) {
		RETURN_FALSE;
	}

	oci_make_pval(return_value,statement,outcol, "OCIResult",0);
}

/* }}} */
/* {{{ proto string OCIServerVersion(int conn)
  Return a string containing server version information.
 */

PHP_FUNCTION(ociserverversion)
{
	oci_connection *connection;
	pval **conn;
	char version[256];

	if (getParametersEx(1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_CONN(connection,conn);

	connection->error = 
		OCIServerVersion(connection->pServiceContext,
						 connection->pError, 
						 (text*)version, 
						 sizeof(version),
						 OCI_HTYPE_SVCCTX);
	
	if (connection->error != OCI_SUCCESS) {
		oci_error(connection->pError, "OCIServerVersion", connection->error);
		RETURN_FALSE;
	}

	RETURN_STRING(version,1);
}

/* }}} */
/* {{{ proto int    OCIStatementType(int stmt)
  Return the query type of an OCI statement.
 */
 
/* XXX it would be better with a general interface to OCIAttrGet() */

PHP_FUNCTION(ocistatementtype)
{
	pval **stmt;
	oci_statement *statement;
	ub2 stmttype;

	if (getParametersEx(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	statement->error = 
		OCIAttrGet((dvoid *)statement->pStmt, 
				   OCI_HTYPE_STMT,
				   (ub2 *)&stmttype, 
				   (ub4 *)0, 
				   OCI_ATTR_STMT_TYPE,
				   statement->pError);
	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIStatementType", statement->error);
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

PHP_FUNCTION(ocirowcount)
{
	pval **stmt;
	oci_statement *statement;
	ub4 rowcount;

	if (getParametersEx(1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	OCI_GET_STMT(statement,stmt);

	statement->error = 
		OCIAttrGet((dvoid *)statement->pStmt, 
				   OCI_HTYPE_STMT,
				   (ub2 *)&rowcount, 
				   (ub4 *)0, 
				   OCI_ATTR_ROW_COUNT,
				   statement->pError);

	if (statement->error != OCI_SUCCESS) {
		oci_error(statement->pError, "OCIRowCount", statement->error);
		RETURN_FALSE;
	}

	RETURN_LONG(rowcount);
}

/* }}} */

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */

