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

#define OCI8_USE_EMALLOC 0		/* set this to 1 if you want to use the php memory manager! */

/* $Id$ */

/* TODO list:
 *
 * - php.ini flags
 * - Error mode (print or shut up?)
 * - OCIPasswordChange()
 * - Prefetching control
 * - binding of arrays
 * - Truncate input values to the bind size
 * - Character sets for NCLOBS
 * - split the module into an upper (php-callable) and lower (c-callable) layer!
 * - make_pval needs some cleanup....
 * - NULLS (retcode/indicator) needs some more work for describing & binding
 * - remove all XXXs
 * - clean up and documentation
 * - there seems to be a bug in OCI where it returns ORA-01406 (value truncated) - whereby it isn't (search for 01406 in the source)
 *   this seems to happen for NUMBER values only...
 * - make OCIInternalDebug accept a mask of flags....
 * - better NULL handling
 * - add some flags to OCIFetchStatement (maxrows etc...)
 */

/* {{{ includes & stuff */

#if defined(COMPILE_DL)
# ifdef THREAD_SAFE
#  undef THREAD_SAFE /* XXX no need in 3.0 */
# endif
# include "dl/phpdl.h"
#endif

#include "php.h"

#if PHP_API_VERSION < 19990421 
  #include "internal_functions.h"
  #include "php3_list.h"
  #include "head.h"
  #define HASH_DTOR (void (*)(void *))
#else
  #include "ext/standard/head.h"
  #define php3tls_pval_destructor(a) zval_dtor(a)
  #define HASH_DTOR (int (*)(void *))
#endif


#include "php3_oci8.h"

#if HAVE_OCI8

/*  XXXXXXXXXXXXXXXXXXXXXXXx
  this is needed due to a brain-dead change in the ocidfn.h header 
  somewhere between 8.0.3 and 8.0.5 - sorry no consistend way to
  do it. (thies@digicol.de)
*/

#ifndef SQLT_BFILEE
  #define SQLT_BFILEE 114
#endif
#ifndef SQLT_CFILEE
  #define SQLT_CFILEE 115
#endif


#define SAFE_STRING(s) ((s)?(s):"")

#if !(WIN32|WINNT)
# include "build-defs.h"
#endif
#include "snprintf.h"

/* }}} */
/* {{{ thread safety stuff */

#ifdef THREAD_SAFE
# define OCI8_GLOBAL(a) oci8_globals->a
# define OCI8_TLS_VARS oci8_global_struct *oci8_globals = TlsGetValue(OCI8Tls); 
void *oci8_mutex;
DWORD OCI8Tls;
static int numthreads=0;

typedef struct oci8_global_struct {
	oci8_module php3_oci8_module;
} oci8_global_struct;
#else /* !defined(THREAD_SAFE) */
#  define OCI8_GLOBAL(a) a
#  define OCI8_TLS_VARS
oci8_module php3_oci8_module;
#endif /* defined(THREAD_SAFE) */

/* }}} */
/* {{{ dynamically loadable module stuff */

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &oci8_module_entry; };

# if (WIN32|WINNT) && defined(THREAD_SAFE)
/* NOTE: You should have an oci8.def file where you export DllMain */
BOOL WINAPI DllMain(HANDLE hModule, DWORD  ul_reason_for_call, 
					LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			if ((OCI8Tls = TlsAlloc()) == 0xFFFFFFFF){
				return 0;
			}
			break;    
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			if (!TlsFree(OCI8Tls)) {
				return 0;
			}
			break;
    }
    return 1;
}

#  endif /* thread safe on Windows */
#endif /* COMPILE_DL */

/* }}} */
/* {{{ startup/shutdown/info/internal function prototypes */

#ifndef ZEND_MODULE_INFO_FUNC_ARGS
#define ZEND_MODULE_INFO_FUNC_ARGS void
#endif

int php3_minit_oci8(INIT_FUNC_ARGS);
int php3_rinit_oci8(INIT_FUNC_ARGS);
int php3_mshutdown_oci8(SHUTDOWN_FUNC_ARGS);
int php3_rshutdown_oci8(SHUTDOWN_FUNC_ARGS);
void php3_info_oci8(ZEND_MODULE_INFO_FUNC_ARGS);

static ub4 oci8_error(OCIError *err_p, char *what, sword status);
/* static int oci8_ping(oci8_connection *conn); XXX NYI */
static void oci8_debug(const char *format,...);

static void _oci8_close_conn(oci8_connection *connection);
static void _oci8_free_stmt(oci8_statement *statement);
static void _oci8_free_column(oci8_out_column *column);
static int _oci8_free_descr(oci8_descriptor *descr);

static oci8_connection *oci8_get_conn(int, const char *, HashTable *);
static oci8_statement *oci8_get_stmt(int, const char *, HashTable *);
static oci8_out_column *oci8_get_col(oci8_statement *, int, pval *, char *);

static int oci8_make_pval(pval *,oci8_statement *,oci8_out_column *, char *, int mode);
static int oci8_parse(oci8_connection *, char *, int, HashTable *);
static int oci8_execute(oci8_statement *, char *,ub4 mode,HashTable *);
static int oci8_fetch(oci8_statement *, ub4, char *);
static ub4 oci8_loaddesc(oci8_connection *, oci8_descriptor *, char **);
static int oci8_setprefetch(oci8_statement *statement,int size);

static void oci8_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive);

/* ServerAttach/Detach */
static oci8_server *oci8_open_server(char *dbname,int persistent);
static void _oci8_close_server(oci8_server *server);

/* SessionBegin/End */
static oci8_session *oci8_open_user(oci8_server* server,char *username,char *password,int persistent,int exclusive);
static void _oci8_close_user(oci8_session *session);

/* bind callback functions */
static sb4 oci8_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
static sb4 oci8_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

/* define callback function */
static sb4 oci8_define_callback(dvoid *, OCIDefine *, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

#if 0
/* failover callback function */
static sb4 oci8_failover_callback(dvoid *svchp,dvoid* envhp,dvoid *fo_ctx,ub4 fo_type, ub4 fo_event);
#endif

/* }}} */
/* {{{ extension function prototypes */

PHP_FUNCTION(oci8_bindbyname);
PHP_FUNCTION(oci8_definebyname);
PHP_FUNCTION(oci8_columnisnull);
PHP_FUNCTION(oci8_columnname);
PHP_FUNCTION(oci8_columnsize);
PHP_FUNCTION(oci8_columntype);
PHP_FUNCTION(oci8_execute);
PHP_FUNCTION(oci8_fetch);
PHP_FUNCTION(oci8_cancel);
PHP_FUNCTION(oci8_fetchinto);
PHP_FUNCTION(oci8_fetchstatement);
PHP_FUNCTION(oci8_freestatement);
PHP_FUNCTION(oci8_internaldebug);
PHP_FUNCTION(oci8_logout);
PHP_FUNCTION(oci8_logon);
PHP_FUNCTION(oci8_nlogon);
PHP_FUNCTION(oci8_plogon);
PHP_FUNCTION(oci8_error);
PHP_FUNCTION(oci8_freedesc);
PHP_FUNCTION(oci8_savedesc);
PHP_FUNCTION(oci8_loaddesc);
PHP_FUNCTION(oci8_commit);
PHP_FUNCTION(oci8_rollback);
PHP_FUNCTION(oci8_newdescriptor);
PHP_FUNCTION(oci8_numcols);
PHP_FUNCTION(oci8_parse);
PHP_FUNCTION(oci8_newcursor);
PHP_FUNCTION(oci8_result);
PHP_FUNCTION(oci8_serverversion);
PHP_FUNCTION(oci8_statementtype);
PHP_FUNCTION(oci8_rowcount);
PHP_FUNCTION(oci8_setprefetch);

/* }}} */
/* {{{ extension definition structures */

#define OCI_ASSOC     			1<<0
#define OCI_NUM       			1<<1
#define OCI_BOTH      			(OCI_ASSOC|OCI_NUM)

#define OCI_RETURN_NULLS       	1<<2
#define OCI_RETURN_LOBS       	1<<3

static unsigned char a3_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char a2_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };

function_entry oci8_functions[] = {
	{"ocidefinebyname",  php3_oci8_definebyname,  a3_arg_force_ref},
	{"ocibindbyname",    php3_oci8_bindbyname,    a3_arg_force_ref},
	{"ocicolumnisnull",  php3_oci8_columnisnull,  NULL},
	{"ocicolumnname",    php3_oci8_columnname,    NULL},
	{"ocicolumnsize",    php3_oci8_columnsize,    NULL},
	{"ocicolumntype",    php3_oci8_columntype,    NULL},
	{"ociexecute",       php3_oci8_execute,       NULL},
	{"ocicancel",        php3_oci8_cancel,         NULL},
	{"ocifetch",         php3_oci8_fetch,         NULL},
	{"ocifetchinto",     php3_oci8_fetchinto,     a2_arg_force_ref},
	{"ocifetchstatement",php3_oci8_fetchstatement,a2_arg_force_ref},
	{"ocifreestatement", php3_oci8_freestatement, NULL},
	{"ocifreecursor",    php3_oci8_freestatement, NULL},
	{"ociinternaldebug", php3_oci8_internaldebug, NULL},
	{"ocinumcols",       php3_oci8_numcols,       NULL},
	{"ociparse",         php3_oci8_parse,         NULL},
	{"ocinewcursor",     php3_oci8_newcursor,     NULL},
	{"ociresult",        php3_oci8_result,        NULL},
	{"ociserverversion", php3_oci8_serverversion, NULL},
	{"ocistatementtype", php3_oci8_statementtype, NULL},
	{"ocirowcount", 	 php3_oci8_rowcount, 	  NULL},
    {"ocilogoff",        php3_oci8_logout,        NULL},
    {"ocilogon",         php3_oci8_logon,         NULL},
    {"ocinlogon",        php3_oci8_nlogon,        NULL},
    {"ociplogon",        php3_oci8_plogon,        NULL},
    {"ocierror",         php3_oci8_error,         NULL},
    {"ocifreedescriptor",php3_oci8_freedesc,      NULL},
    {"ocisavedesc",      php3_oci8_savedesc,      NULL},
    {"ociloaddesc",      php3_oci8_loaddesc,      NULL},
    {"ocicommit",        php3_oci8_commit,        NULL},
    {"ocirollback",      php3_oci8_rollback,      NULL},
    {"ocinewdescriptor", php3_oci8_newdescriptor, NULL},
    {"ocisetprefetch",   php3_oci8_setprefetch,   NULL},
    {NULL,               NULL,                    NULL}
};

php3_module_entry oci8_module_entry = {
    "OCI8",                /* extension name */
    oci8_functions,        /* extension function list */
    php3_minit_oci8,       /* extension-wide startup function */
    php3_mshutdown_oci8,   /* extension-wide shutdown function */
    php3_rinit_oci8,       /* per-request startup function */
    php3_rshutdown_oci8,   /* per-request shutdown function */
    php3_info_oci8,        /* information function */
    STANDARD_MODULE_PROPERTIES
};

/* }}} */
/* {{{ debug malloc/realloc/free */

#if OCI8_USE_EMALLOC
CONST dvoid *ocimalloc(dvoid *ctx, size_t size)
{
    dvoid *ret;
	ret = (dvoid *)malloc(size);
    oci8_debug("ocimalloc(%d) = %08x", size,ret);
    return ret;
}

CONST dvoid *ocirealloc(dvoid *ctx, dvoid *ptr, size_t size)
{
    dvoid *ret;
    oci8_debug("ocirealloc(%08x, %d)", ptr, size);
	ret = (dvoid *)realloc(ptr, size);
    return ptr;
}

CONST void ocifree(dvoid *ctx, dvoid *ptr)
{
    oci8_debug("ocifree(%08x)", ptr);
    free(ptr);
}
#endif

/* }}} */
/* {{{ startup, shutdown and info functions */

int php3_minit_oci8(INIT_FUNC_ARGS)
{
#ifdef THREAD_SAFE
	oci8_global_struct *oci8_globals;
# if !COMPILE_DL
#  if WIN32|WINNT
	CREATE_MUTEX(oci8_mutex,"OCI8_TLS");
#  endif
	SET_MUTEX(oci8_mutex);
	numthreads++;
	if (numthreads == 1) {
		if ((OCI8Tls = TlsAlloc()) == 0xFFFFFFFF){
			FREE_MUTEX(oci8_mutex);
			return 0;
		}
	}
	FREE_MUTEX(oci8_mutex);
# endif /* !COMPILE_DL */
	oci8_globals =
		(oci8_global_struct *) LocalAlloc(LPTR, sizeof(oci8_global_struct)); 
	TlsSetValue(OCI8Tls, (void *) oci8_globals);
#endif /* THREAD_SAFE */

	/* XXX NYI
	if (cfg_get_long("oci8.allow_persistent",
					 &OCI8_GLOBAL(php3_oci8_module).allow_persistent)
		== FAILURE) {
	  OCI8_GLOBAL(php3_oci8_module).allow_persistent = -1;
	}
	if (cfg_get_long("oci8.max_persistent",
					 &OCI8_GLOBAL(php3_oci8_module).max_persistent)
	    == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).max_persistent = -1;
	}
	if (cfg_get_long("oci8.max_links",
					 &OCI8_GLOBAL(php3_oci8_module).max_links)
	    == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).max_links = -1;
	}
	
	OCI8_GLOBAL(php3_oci8_module).num_persistent = 0;
	*/

	OCI8_GLOBAL(php3_oci8_module).user_num   = 1000;
	OCI8_GLOBAL(php3_oci8_module).server_num = 2000;

	OCI8_GLOBAL(php3_oci8_module).le_conn = register_list_destructors(_oci8_close_conn, NULL);
	OCI8_GLOBAL(php3_oci8_module).le_stmt = register_list_destructors(_oci8_free_stmt, NULL);

	OCI8_GLOBAL(php3_oci8_module).user = malloc(sizeof(HashTable));
	_php3_hash_init(OCI8_GLOBAL(php3_oci8_module).user, 13, NULL, NULL, 1);

	OCI8_GLOBAL(php3_oci8_module).server = malloc(sizeof(HashTable));
	_php3_hash_init(OCI8_GLOBAL(php3_oci8_module).server, 13, NULL, NULL, 1); 

	if (cfg_get_long("oci8.debug_mode",
					 &OCI8_GLOBAL(php3_oci8_module).debug_mode) == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).debug_mode = 0;
	}

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

#if OCI8_USE_EMALLOC
    OCIInitialize(OCI_DEFAULT, NULL, ocimalloc, ocirealloc, ocifree);
#else
    OCIInitialize(OCI_DEFAULT, NULL, NULL, NULL, NULL);
#endif

	OCIEnvInit(&OCI8_GLOBAL(php3_oci8_module).pEnv, OCI_DEFAULT, 0, NULL);
	OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
				   (dvoid **)&OCI8_GLOBAL(php3_oci8_module).pError,
				   OCI_HTYPE_ERROR, 
				   0, 
				   NULL);
	
	return SUCCESS;
}

/* ----------------------------------------------------------------- */


int php3_rinit_oci8(INIT_FUNC_ARGS)
{
	OCI8_TLS_VARS;
	
	/* XXX NYI
	OCI8_GLOBAL(php3_oci8_module).num_links = 
		OCI8_GLOBAL(php3_oci8_module).num_persistent;
	*/

	OCI8_GLOBAL(php3_oci8_module).debug_mode = 0; /* start "fresh" */

    oci8_debug("php3_rinit_oci8");

    return SUCCESS;
}

static int _user_pcleanup(oci8_session *session)
{
	_oci8_close_user(session);

	return 1;
}

static int _server_pcleanup(oci8_server *server)
{
	_oci8_close_server(server);

	return 1;
}

int php3_mshutdown_oci8(SHUTDOWN_FUNC_ARGS)
{
    oci8_debug("php3_mshutdown_oci8");

	_php3_hash_apply(OCI8_GLOBAL(php3_oci8_module).user,(int (*)(void *))_user_pcleanup);
	_php3_hash_apply(OCI8_GLOBAL(php3_oci8_module).server,(int (*)(void *))_server_pcleanup);

	_php3_hash_destroy(OCI8_GLOBAL(php3_oci8_module).user);
	_php3_hash_destroy(OCI8_GLOBAL(php3_oci8_module).server);

	free(OCI8_GLOBAL(php3_oci8_module).user);
	free(OCI8_GLOBAL(php3_oci8_module).server);

	OCIHandleFree((dvoid *)OCI8_GLOBAL(php3_oci8_module).pEnv, OCI_HTYPE_ENV);

#ifdef THREAD_SAFE
	oci8_global_struct *oci8_globals;
	oci8_globals = TlsGetValue(OCI8Tls); 
	if (oci8_globals != 0) {
		LocalFree((HLOCAL) oci8_globals);
	}
#if !COMPILE_DL
	SET_MUTEX(oci8_mutex);
	numthreads--;
	if (!numthreads) {
		if (!TlsFree(OCI8Tls)) {
			FREE_MUTEX(oci8_mutex);
			return 0;
		}
	}
	FREE_MUTEX(oci8_mutex);
#endif
#endif

	return SUCCESS;
}

static int _stmt_cleanup(list_entry *le)
{
	oci8_statement *statement;
	OCI8_TLS_VARS;

    if (le->type == OCI8_GLOBAL(php3_oci8_module).le_stmt) {
        statement = (oci8_statement *) le->ptr;

        if (!statement->conn->open) {
            return 1;
        }
    }

    return 0;
}

static int _user_cleanup(oci8_session *session)
{
   	if (session->persistent)
		return 0;

	_oci8_close_user(session);

	return 1;
}

static int _server_cleanup(oci8_server *server)
{
	if (server->persistent)
		return 0;

	_oci8_close_server(server);

	return 1;
}

int php3_rshutdown_oci8(SHUTDOWN_FUNC_ARGS)
{
	OCI8_TLS_VARS;

    oci8_debug("php3_rshutdown_oci8");

	_php3_hash_apply(OCI8_GLOBAL(php3_oci8_module).user,(int (*)(void *))_user_cleanup);
	_php3_hash_apply(OCI8_GLOBAL(php3_oci8_module).server,(int (*)(void *))_server_cleanup);

	/* XXX free all statements, rollback all outstanding transactions */

    return SUCCESS;
}


void php3_info_oci8(ZEND_MODULE_INFO_FUNC_ARGS)
{
#if !(WIN32|WINNT)
	php3_printf("Oracle version: %s<br>\n"
			    "Compile-time ORACLE_HOME: %s<br>\n"
			    "Libraries used: %s",
			    PHP_ORACLE_VERSION, PHP_ORACLE_HOME, PHP_ORACLE_LIBS);
#endif
}

/* }}} */
/* {{{ oci8_free_define() */

static int
oci8_free_define(oci8_define *define)
{
	oci8_debug("oci8_free_define: %s",define->name);

	if (define->name) {
		efree(define->name);
		define->name = 0;
	}
	return 1;
}

/* }}} */
/* {{{ _oci8_free_column() */

static void
_oci8_free_column(oci8_out_column *column)
{
	if (! column) {
		return;
	}

	/*
	oci8_debug("_oci8_free_column: %s",column->name);
	*/

	if (column->data) {
		if (column->is_descr) {
			_php3_hash_index_del(column->statement->conn->descriptors,(int) column->data);
		} else {
			if (column->data) {
				efree(column->data);
			}
		}
	}

	if (column->name) {
		efree(column->name);
	}

	/* efree(column); XXX php cleares this for us */
}

/* }}} */
/* {{{ _oci8_free_stmt() */

static void
_oci8_free_stmt(oci8_statement *statement)
{
	OCI8_TLS_VARS;

	if (! statement) {
		return;
	}

	oci8_debug("_oci8_free_stmt: id=%d last_query=\"%s\"",statement->id,SAFE_STRING(statement->last_query));

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
		_php3_hash_destroy(statement->columns);
		efree(statement->columns);
	}

	if (statement->binds) {
		_php3_hash_destroy(statement->binds);
		efree(statement->binds);
	}

	if (statement->defines) {
		_php3_hash_destroy(statement->defines);
		efree(statement->defines);
	}

	efree(statement);
}

/* }}} */
/* {{{ _oci8_close_conn() */

static void
_oci8_close_conn(oci8_connection *connection)
{
	OCI8_TLS_VARS;

	if (! connection) {
		return;
	}

	/* 
	   as the connection is "only" a in memory service context we do not disconnect from oracle.
	*/

	oci8_debug("_oci8_close_conn: id=%d",connection->id);

	if (connection->descriptors) {
		_php3_hash_destroy(connection->descriptors);
		efree(connection->descriptors);
	}

	if (connection->pServiceContext) {
		OCIHandleFree((dvoid *) connection->pServiceContext, (ub4) OCI_HTYPE_SVCCTX);
	}

	if (connection->pError) {
		OCIHandleFree((dvoid *) connection->pError, (ub4) OCI_HTYPE_ERROR);
	}

	efree(connection);
}

/* }}} */
/* {{{ oci8_error() */

static ub4
oci8_error(OCIError *err_p, char *what, sword status)
{
    text errbuf[512];
    sb4 errcode = 0;

    switch (status) {
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
			php3_error(E_WARNING, "%s: OCI_SUCCESS_WITH_INFO", what);
			break;
		case OCI_NEED_DATA:
			php3_error(E_WARNING, "%s: OCI_NEED_DATA", what);
			break;
		case OCI_NO_DATA:
			php3_error(E_WARNING, "%s: OCI_NO_DATA", what);
			break;
		case OCI_ERROR:
			OCIErrorGet(err_p, (ub4)1, NULL, &errcode, errbuf,
						(ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
			php3_error(E_WARNING, "%s: %s", what, errbuf);
			break;
		case OCI_INVALID_HANDLE:
			php3_error(E_WARNING, "%s: OCI_INVALID_HANDLE", what);
			break;
		case OCI_STILL_EXECUTING:
			php3_error(E_WARNING, "%s: OCI_STILL_EXECUTING", what);
			break;
		case OCI_CONTINUE:
			php3_error(E_WARNING, "%s: OCI_CONTINUE", what);
			break;
		default:
			break;
    }
    return errcode;
}

/* }}} */
/* {{{ NYI oci8_ping()  */

#if 0 /* XXX NYI */
/* test if a connection is still alive and return 1 if it is */
static int oci8_ping(oci8_connection *conn)
{
    /* XXX FIXME not yet implemented */
    return 1;
}
#endif

/* }}} */

/************************* INTERNAL FUNCTIONS *************************/

/* {{{ oci8_debug() */

static void oci8_debug(const char *format,...)
{
	OCI8_TLS_VARS;

    if (OCI8_GLOBAL(php3_oci8_module).debug_mode) {
		char buffer[1024];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer)-1, format, args);
		va_end(args);
		buffer[sizeof(buffer)-1] = '\0';
		if (php3_header()) {
			php3_printf("OCIDebug: %s<br>\n", buffer);
		}
	}
}

/* }}} */
/* {{{ oci8_get_conn() */

static oci8_connection *
oci8_get_conn(int conn_ind, const char *func, HashTable *list)
{
	int type;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	connection = (oci8_connection *)php3_list_find(conn_ind, &type);
	if (!connection || !OCI8_CONN_TYPE(type)) {
		php3_error(E_WARNING, "%s: invalid connection %d", func, conn_ind);
		return (oci8_connection *)NULL;
	}

	if (connection->open) {
		return connection;
	} else {
		/* _oci8_close_conn(connection); be careful with this!! */
		return (oci8_connection *)NULL;
	}
}

/* }}} */
/* {{{ oci8_get_stmt() */

static oci8_statement *
oci8_get_stmt(int stmt_ind, const char *func, HashTable *list)
{
	int type;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	statement = (oci8_statement *)php3_list_find(stmt_ind, &type);
	if (!statement || !OCI8_STMT_TYPE(type)) {
		php3_error(E_WARNING, "%s: invalid statement %d", func, stmt_ind);
		return (oci8_statement *)NULL;
	}

	if (statement->conn->open) {
		return statement;
	} else {
		/* _oci8_free_stmt(statement); be careful with this!! */
		return (oci8_statement *)NULL;
	}
}

/* }}} */
/* {{{ oci8_get_col() */

static oci8_out_column *
oci8_get_col(oci8_statement *statement, int col, pval *pval, char *func)
{
	oci8_out_column *outcol = NULL;
	int i;
	OCI8_TLS_VARS;

	if (statement->columns == 0) { /* we release the columns at the end of a fetch */
		return NULL;
	}

	if (pval) {
	    if (pval->type == IS_STRING) {
			for (i = 0; i < statement->ncolumns; i++) {
		   		outcol = oci8_get_col(statement, i + 1, 0, func);
                if (outcol == NULL) {
         	       continue;
                } else if (((int) outcol->name_len == pval->value.str.len) 
                           && (! strncmp(outcol->name,pval->value.str.val,pval->value.str.len))) {
                	return outcol;   	
			   	}
		   	}
       	} else {
			convert_to_long(pval);
	
			return oci8_get_col(statement,pval->value.lval,0,func);
		}
	} else if (col != -1) {
		if (_php3_hash_index_find(statement->columns, col, (void **)&outcol) == FAILURE) {
			php3_error(E_WARNING, "%s: invalid column %d", func, col);
			return NULL;
		}
		return outcol;
	}

	return NULL;
}

/* }}} */
/* {{{ oci8_make_pval() */

static int 
oci8_make_pval(pval *value,oci8_statement *statement,oci8_out_column *column, char *func, int mode)
{
	size_t size;
	oci8_descriptor *descr;
	ub4 loblen;
	char *buffer;

	if (column->indicator || column->retcode)
		if ((column->indicator != -1) && (column->retcode != 1405))
			oci8_debug("oci8_make_pval: %16s,retlen = %4d,cb_retlen = %d,storage_size4 = %4d,indicator %4d, retcode = %4d",
					   column->name,column->retlen,column->cb_retlen,column->storage_size4,column->indicator,column->retcode);

/*
	memset(value,0,sizeof(pval));
*/

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

	        if (_php3_hash_index_find(statement->conn->descriptors,(int)  column->data, (void **)&descr) == FAILURE) {
   	        	php3_error(E_WARNING, "unable to find my descriptor %d",column->data);
            	return -1;
        	}

			loblen = oci8_loaddesc(statement->conn,descr,&buffer);

			if (loblen > 0)	{
				value->type = IS_STRING;
				value->value.str.len = loblen;
				value->value.str.val = buffer;
			} else {
				var_reset(value);
			}
		} else { /* return the locator */
			object_init(value);

			add_property_long(value, "connection", statement->conn->id);
			add_property_long(value, "descriptor", (long) column->data);

			if (column->data_type != SQLT_RDD) { /* ROWIDs don't have any user-callable methods */
				if ((column->data_type != SQLT_BFILEE) && (column->data_type != SQLT_CFILEE)) { 
					add_method(value, "save", php3_oci8_savedesc); /* oracle does not support writing of files as of now */
				}
				add_method(value, "load", php3_oci8_loaddesc);
			}
			/* there is NO free call here, 'cause the memory gets deallocated together with the statement! */
		}
	} else {
		switch (column->retcode) {
			case 0: /* intact value */
			    /* 
				   oci8_debugcol(column,"OK");
				*/
				if (column->piecewise) {
					size = (column->cursize - column->piecesize) + column->cb_retlen;
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

static int
oci8_setprefetch(oci8_statement *statement,int size)
{ 
	ub4   prefetch;

	prefetch = size * 1024;
	statement->error = 
		oci8_error(statement->pError, 
				   "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY",
				   OCIAttrSet(statement->pStmt,
							  OCI_HTYPE_STMT,
							  &prefetch,
							  0, 
							  OCI_ATTR_PREFETCH_MEMORY,
							  statement->pError));
	prefetch = size;
	statement->error = 
		oci8_error(statement->pError, 
				   "OCIAttrSet OCI_ATTR_PREFETCH_MEMORY",
				   OCIAttrSet(statement->pStmt,
							  OCI_HTYPE_STMT,
							  &prefetch,
							  0, 
							  OCI_ATTR_PREFETCH_ROWS,
							  statement->pError));

	return 1;
}

/* {{{ oci8_parse() */
static int
oci8_parse(oci8_connection *connection, char *query, int len, HashTable *list)
{
	oci8_statement *statement;
	OCI8_TLS_VARS;

	statement = ecalloc(1,sizeof(oci8_statement));
    OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
				   (dvoid **)&statement->pStmt,
				   OCI_HTYPE_STMT, 
				   0, 
				   NULL);
    OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
				   (dvoid **)&statement->pError,
				   OCI_HTYPE_ERROR,
				   0,
				   NULL);

	if (len > 0) {
		connection->error = 
			oci8_error(connection->pError,
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
	statement->id = php3_list_insert(statement, OCI8_GLOBAL(php3_oci8_module).le_stmt);

	oci8_debug("oci8_parse \"%s\" id=%d conn=%d",
			   SAFE_STRING(query),
			   statement->id,
			   statement->conn->id);

	return statement->id;
}

/* }}} */
/* {{{ oci8_execute() */

static int
oci8_execute(oci8_statement *statement, char *func,ub4 mode, HashTable *list)
{
	oci8_out_column *outcol;
	oci8_out_column column;
	OCIParam *param = 0;
	text *colname;
	ub4 counter;
	ub2 define_type;
	ub2 stmttype;
	ub4 iters;
	ub4 colcount;
	ub2 dynamic;
	dvoid *buf;
	oci8_descriptor *pdescr, descr;
	OCI8_TLS_VARS;

	statement->error = 
		oci8_error(statement->pError,
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
			oci8_error(statement->pError,
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
			_php3_hash_init(statement->columns, 13, NULL,HASH_DTOR _oci8_free_column, 0) == FAILURE) {
			/* out of memory */
			return 0;
		}

		OCIHandleAlloc(
			OCI8_GLOBAL(php3_oci8_module).pEnv,
			(dvoid **)&param,
			OCI_DTYPE_PARAM,
			0,
			NULL);


		counter = 1;

		statement->error = 
			oci8_error(statement->pError,
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
			memset(&column,0,sizeof(oci8_out_column));

			if (_php3_hash_index_update(statement->columns, counter, &column,
										sizeof(oci8_out_column), (void**) &outcol) == FAILURE) {
				efree(statement->columns);
				/* out of memory */
				return 0;
			} 

			outcol->statement = statement;

			statement->error = 
				oci8_error(statement->pError,
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
				oci8_error(statement->pError, 
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
				oci8_error(statement->pError,
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
				oci8_error(statement->pError,
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
				_php3_hash_find(statement->defines,outcol->name,outcol->name_len,(void **) &outcol->define);
			}

			buf = 0;
			switch (outcol->data_type) {
				case SQLT_RSET:
					outcol->stmtid = oci8_parse(statement->conn,0,0,list);
					outcol->pstmt = oci8_get_stmt(outcol->stmtid, "OCIExecute",list);

					define_type = outcol->data_type;
					outcol->is_cursor = 1;
					outcol->storage_size4 = -1;
					dynamic = OCI_DYNAMIC_FETCH;

#if 0 /* doesn't work!! */
					outcol->stmtid = oci8_parse(statement->conn,0,0,list);
					outcol->pstmt = oci8_get_stmt(outcol->stmtid, "OCIExecute",list);

					define_type = outcol->data_type;
					outcol->is_cursor = 1;
					outcol->storage_size4 = -1;
					outcol->retlen = -1;
					dynamic = OCI_DEFAULT;
					buf = outcol->pstmt->pStmt;
#endif
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

					OCIDescriptorAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
									   (dvoid *)&(descr.ocidescr), 
									   descr.type,
									   (size_t) 0,
									   (dvoid **) 0);
					
					_php3_hash_index_update(statement->conn->descriptors, 
											statement->conn->descriptors_count,
											&descr,
											sizeof(oci8_descriptor),
											(void **)&pdescr);
				
					outcol->data = (void *) outcol->statement->conn->descriptors_count++;
					buf = outcol->pdescr = pdescr;
					oci8_debug("OCIExecute: new descriptor for %s",outcol->name);
					break;

#if 0 /* RAW (can be up to 2K) is now handled as STRING! */
				case SQLT_BIN:
					define_type = SQLT_BIN;
					outcol->storage_size4++;
					buf = outcol->data = (text *) emalloc(outcol->storage_size4);
					dynamic = OCI_DEFAULT;
					break;
#endif

				case SQLT_LNG:
			    case SQLT_LBI:
					if (outcol->data_type == SQLT_LBI) {
						define_type = SQLT_BIN;
					} else {
						define_type = SQLT_STR;
					}
					outcol->storage_size4 = OCI8_MAX_DATA_SIZE;
					outcol->piecewise = 1;
					outcol->piecesize = OCI8_PIECE_SIZE;
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
					oci8_error(statement->pError,
							   "OCIDefineByPos",
							   OCIDefineByPos(statement->pStmt,				/* IN/OUT handle to the requested SQL query */
											  (OCIDefine **)&outcol->pDefine,/* IN/OUT pointer to a pointer to a define handle */
											  statement->pError, 			/* IN/OUT An error handle  */
											  counter,						/* IN     position in the select list */
											  (dvoid *)buf,					/* IN/OUT pointer to a buffer */
											  outcol->storage_size4, 	    /* IN	  The size of each valuep buffer in bytes */
											  define_type,			 		/* IN	  The data type */
											  (dvoid *)&outcol->indicator, 	/* IN	  pointer to an indicator variable or arr */
											  (ub2 *)NULL, 		            /* IN/OUT Pointer to array of length of data fetched */
											  (ub2 *)NULL,		            /* OUT	  Pointer to array of column-level return codes */
											  dynamic));			        /* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
			} else {
				statement->error = 
					oci8_error(statement->pError,
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
											  dynamic));			        /* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
			}
			if (statement->error) {
				return 0; /* XXX we loose memory!!! */
			}

			if (dynamic == OCI_DYNAMIC_FETCH) {
				statement->error = 
					oci8_error(statement->pError,
							   "OCIDefineDynamic",
							   OCIDefineDynamic(outcol->pDefine,
												statement->pError,
												(dvoid *) outcol,
												(OCICallbackDefine)oci8_define_callback));

				if (statement->error) {
					return 0; /* XXX we loose memory!!! */
				}
			}
		}
	}

	return 1;
}

/* }}} */
/* {{{ oci8_fetch() */

static int
oci8_fetch(oci8_statement *statement, ub4 nrows, char *func)
{
	int i;
	oci8_out_column *column;
	OCI8_TLS_VARS;

	/* first we need to reset our piecewise fetch-infos */
	for (i = 0; i < statement->ncolumns; i++) { 
		column = oci8_get_col(statement, i + 1, 0, "OCIFetch");
		if (column == NULL) { /* should not happen... */
			continue;
		}
			
		if (column->piecewise) {
			if (column->curoffs) {
				column->cursize = column->piecesize; 
				column->curoffs = 0;
			}
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
			_php3_hash_destroy(statement->columns);
			efree(statement->columns);
			statement->columns = 0;
		}
		statement->executed = 0;

		statement->error = 0; /* OCI_NO_DATA is NO error for us!!! */

		return 0;
	}

	if (statement->error == OCI_SUCCESS_WITH_INFO || statement->error == OCI_SUCCESS) {
		/* do the stuff needed for OCIDefineByName */
		for (i = 0; i < statement->ncolumns; i++) {
			column = oci8_get_col(statement, i + 1, 0, "OCIFetch");
			if (column == NULL) { /* should not happen... */
				continue;
			}

			if (! column->define) {
				continue;
			}
			
			php3tls_pval_destructor(column->define->pval);

			oci8_make_pval(column->define->pval,statement,column,"OCIFetch",0);
		}

		return 1;
	}

	oci8_error(statement->pError, func, statement->error);

	return 0;
}

/* }}} */
/* {{{ oci8_loaddesc() */
static ub4
oci8_loaddesc(oci8_connection *connection, oci8_descriptor *mydescr, char **buffer)
{
	ub4 loblen;

	OCI8_TLS_VARS;

	connection->error = 
		OCILobGetLength(connection->pServiceContext,
						connection->pError,
						mydescr->ocidescr,
						&loblen);

	if (connection->error) {
		oci8_error(connection->pError, "OCILobGetLength", connection->error);
		return 0;
	}
		
	*buffer = emalloc(loblen + 1);

	if (! buffer) {
		return 0;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		connection->error = 
			OCILobFileOpen(connection->pServiceContext,
						   connection->pError,
						   mydescr->ocidescr,
						   OCI_FILE_READONLY);
		if (connection->error) {
			oci8_error(connection->pError, "OCILobFileOpen",connection->error);
			efree(buffer);
			return 0;
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
		oci8_error(connection->pError, "OCILobRead", connection->error);
		efree(buffer);
		return 0;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		connection->error = 
			OCILobFileClose(connection->pServiceContext,
							connection->pError,
							mydescr->ocidescr);
		if (connection->error) {
			oci8_error(connection->pError, "OCILobFileClose", connection->error);
			efree(buffer);
			return 0;
		}
	}
		
	(*buffer)[ loblen ] = 0;

	oci8_debug("OCIloaddesc: size=%d",loblen);

	return loblen;
}

/* }}} */
/* {{{ oci8_failover_callback() */
#if 0 /* not needed yet ! */
static sb4 
oci8_failover_callback(dvoid *svchp,
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
/* {{{ oci8_define_callback() */

static sb4
oci8_define_callback(dvoid *octxp,
					 OCIDefine *defnp,
					 ub4 iter, /* 0-based execute iteration value */
					 dvoid **bufpp, /* pointer to data */
					 ub4 **alenp, /* size after value/piece has been read */
					 ub1 *piecep, /* which piece */
					 dvoid **indpp, /* indicator value */
					 ub2 **rcodep) 
{
	oci8_out_column *outcol;

	outcol = (oci8_out_column *)octxp;

	if (outcol->is_cursor) { /* REFCURSOR */
		outcol->cb_retlen = -1;
		*bufpp  = outcol->pstmt->pStmt;
	} else { /* "normal variable" */
		if (! outcol->data) {
			if (outcol->piecewise) {
				outcol->data = (text *) emalloc(outcol->piecesize);
				outcol->cursize = outcol->piecesize;
				outcol->curoffs = 0;
			}
		}

		if (! outcol->data) {
			php3_error(E_WARNING, "OCIFetch: cannot allocate %d bytes!",outcol->storage_size4);
			return OCI_ERROR;
		}

       if (outcol->piecewise) {
			if ((outcol->curoffs + outcol->piecesize) > outcol->cursize) {
				outcol->cursize += outcol->piecesize;
				outcol->data = erealloc(outcol->data,outcol->cursize);
			}
			outcol->cb_retlen = outcol->piecesize;
			*bufpp  = ((char*)outcol->data) + outcol->curoffs;
			outcol->curoffs += outcol->piecesize;
		} 
	}

	outcol->indicator = 0;
	outcol->retcode = 0;

	*alenp  = &outcol->cb_retlen;
 	*indpp  = &outcol->indicator;
	*rcodep	= &outcol->retcode;
	*piecep = OCI_ONE_PIECE;

	/*
	oci8_debug("oci8_define_callback: %s,*bufpp = %x,**alenp = %d,**indpp = %d, **rcodep= %d, *piecep = %d",
		   outcol->name,*bufpp,**alenp,**(ub2**)indpp,**rcodep,*piecep);
	*/
		   
	return OCI_CONTINUE;
}

/* }}} */
/* {{{ oci8_bind_in_callback() */

static sb4
oci8_bind_in_callback(dvoid *ictxp, /* context pointer */
					  OCIBind *bindp, /* bind handle */
					  ub4 iter, /* 0-based execute iteration value */
					  ub4 index, /* index of current array for PL/SQL or
									row index for SQL */
					  dvoid **bufpp, /* pointer to data */
					  ub4 *alenp, /* size after value/piece has been read */
					  ub1 *piecep, /* which piece */
					  dvoid **indpp) /* indicator value */
{
	oci8_bind *phpbind;
	pval *val;

	phpbind = (oci8_bind *)ictxp;

	if (!phpbind || !(val = phpbind->value)) {
		php3_error(E_WARNING, "!phpbind || !phpbind->val");
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
/* {{{ oci8_bind_out_callback() */

static sb4
oci8_bind_out_callback(dvoid *ctxp, /* context pointer */
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
	oci8_bind *phpbind;
	pval *val;

	phpbind = (oci8_bind *)ctxp;
	if (!phpbind) {
		oci8_debug("oci8_bind_out_callback: phpbind = NULL");
		return OCI_ERROR;
	}

	val = phpbind->value;
	if (val == NULL) {
		oci8_debug("oci8_bind_out_callback: phpbind->value = NULL");
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

		oci8_debug("oci8_bind_out_callback: maxlen=%d",phpbind->maxsize); 

		*alenpp = (ub4*) &phpbind->value->value.str.len; /* XXX we assume that php-pval len has 4 bytes */
		*bufpp = phpbind->value->value.str.val;
		*piecep = OCI_ONE_PIECE;
		*rcodepp = &phpbind->retcode;
		*indpp = &phpbind->indicator;
	}

	return OCI_CONTINUE;
}

/* }}} */
/* {{{ oci8_open_user()
 */

static oci8_session *oci8_open_user(oci8_server* server,char *username,char *password,int persistent,int exclusive)
{
	oci8_session *session = 0;
	OCISvcCtx *svchp = 0;
	char *hashed_details;
	int hashed_details_length;
    OCI8_TLS_VARS;

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
		_php3_hash_find(OCI8_GLOBAL(php3_oci8_module).user, hashed_details, hashed_details_length+1, (void **) &session);

		if (session) {
			if (session->open) {
				if (persistent) {
					session->persistent = 1;
				}
				efree(hashed_details);
				return session;
			} else {
				_oci8_close_user(session);
				/* breakthru to open */
			}
		}
	}

	session = calloc(1,sizeof(oci8_session));

	if (! session) {
		goto CLEANUP;
	}

	session->persistent = persistent;
	session->server = server;

	/* allocate temporary Service Context */
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
					   (dvoid **)&svchp,
					   OCI_HTYPE_SVCCTX,
					   0,
					   NULL);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_open_user: OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	/* allocate private session-handle */
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
					   (dvoid **)&session->pSession,
					   OCI_HTYPE_SESSION,
					   0,
					   NULL);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_open_user: OCIHandleAlloc OCI_HTYPE_SESSION", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIAttrSet(svchp,
				   OCI_HTYPE_SVCCTX,
				   server->pServer,
				   0, 
				   OCI_ATTR_SERVER, 
				   OCI8_GLOBAL(php3_oci8_module).pError);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_open_user: OCIAttrSet OCI_ATTR_SERVER", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	/* set the username in user handle */
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIAttrSet((dvoid *) session->pSession,
				   (ub4) OCI_HTYPE_SESSION, 
				   (dvoid *) username, 
				   (ub4) strlen(username), 
				   (ub4) OCI_ATTR_USERNAME, 
				   OCI8_GLOBAL(php3_oci8_module).pError);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "OCIAttrSet OCI_ATTR_USERNAME", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	/* set the password in user handle */
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIAttrSet((dvoid *) session->pSession,
				   (ub4) OCI_HTYPE_SESSION, 
				   (dvoid *) password, 
				   (ub4) strlen(password), 
				   (ub4) OCI_ATTR_PASSWORD, 
				   OCI8_GLOBAL(php3_oci8_module).pError);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "OCIAttrSet OCI_ATTR_PASSWORD", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	OCI8_GLOBAL(php3_oci8_module).error = 
		OCISessionBegin(svchp,
						OCI8_GLOBAL(php3_oci8_module).pError,
						session->pSession, 
						(ub4) OCI_CRED_RDBMS, 
						(ub4) OCI_DEFAULT);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "OCISessionBegin", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	/* Free Temporary Service Context */
	OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX);

	session->num = OCI8_GLOBAL(php3_oci8_module).user_num++;

 	session->open = 1;

	oci8_debug("oci8_open_user new sess=%d user=%s",session->num,username);

	if (exclusive) {
		_php3_hash_next_index_pointer_insert(OCI8_GLOBAL(php3_oci8_module).user,
											 (void *) session);
	} else {
		_php3_hash_pointer_update(OCI8_GLOBAL(php3_oci8_module).user,
								  hashed_details,
								  hashed_details_length+1, 
								  (void *) session);
	}

	efree(hashed_details);

	return session;

 CLEANUP:
	oci8_debug("oci8_open_user: FAILURE -> CLEANUP called");

	if (hashed_details) {
		efree(hashed_details);
	}

	_oci8_close_user(session);

	return 0;
}

/* }}} */
/* {{{ _oci8_close_user()
 */

static void
_oci8_close_user(oci8_session *session)
{
	OCISvcCtx *svchp;

	if (! session) {
		return;
	}

	oci8_debug("_oci8_close_user: logging-off sess=%d",session->num);

	if (session->open) {
		/* Temporary Service Context */
		OCI8_GLOBAL(php3_oci8_module).error = 
			OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
						   (dvoid **) &svchp,
						   (ub4) OCI_HTYPE_SVCCTX,
						   (size_t) 0, 
						   (dvoid **) 0);
		
		if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
			oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "_oci8_close_user OCIHandleAlloc OCI_HTYPE_SVCCTX", OCI8_GLOBAL(php3_oci8_module).error);
		}
		
		/* Set the server handle in service handle */ 
		OCI8_GLOBAL(php3_oci8_module).error = 
			OCIAttrSet(svchp,
					   OCI_HTYPE_SVCCTX,
					   session->server->pServer,
					   0, 
					   OCI_ATTR_SERVER, 
					   OCI8_GLOBAL(php3_oci8_module).pError);
		if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
			oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "_oci8_close_user: OCIAttrSet OCI_ATTR_SERVER", OCI8_GLOBAL(php3_oci8_module).error);
		}
		
		/* Set the Authentication handle in the service handle */
		OCI8_GLOBAL(php3_oci8_module).error = 
			OCIAttrSet(svchp, 
					   OCI_HTYPE_SVCCTX,
					   session->pSession,
					   0, 
					   OCI_ATTR_SESSION,
					   OCI8_GLOBAL(php3_oci8_module).pError);
		if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
			oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "_oci8_close_user: OCIAttrSet OCI_ATTR_SESSION", OCI8_GLOBAL(php3_oci8_module).error);
		}
		
		OCI8_GLOBAL(php3_oci8_module).error = 
			OCISessionEnd(svchp,
						  OCI8_GLOBAL(php3_oci8_module).pError,
						  session->pSession,
						  (ub4) 0); 
		if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
			oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "_oci8_close_user: OCISessionEnd", OCI8_GLOBAL(php3_oci8_module).error);
		}
	} else {
		oci8_debug("_oci8_close_user: logging-off DEAD session");
	}

	OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX);

	if (session->pSession) {
		OCIHandleFree((dvoid *) session->pSession, (ub4) OCI_HTYPE_SESSION);
	}
	
   	free(session);
}

/* }}} */
/* {{{ _oci8_free_descr()
 */

static int
_oci8_free_descr(oci8_descriptor *descr)
{
    OCI8_TLS_VARS;

    oci8_debug("oci8_free_descr: %x",descr->ocidescr);

    OCIDescriptorFree(descr->ocidescr, descr->type);

	return 1;
}
/* }}} */
/* {{{ oci8_open_server()
 */
static oci8_server *oci8_open_server(char *dbname,int persistent)
{ 
	oci8_server *server = 0;
	char *hashed_details;
	int hashed_details_length;
    OCI8_TLS_VARS;

	/* 
	   check if we already have this server open 

	   we will reuse servers within a request no matter if the usere requested persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	hashed_details_length = strlen(SAFE_STRING((char *)dbname));
	hashed_details = (char *) emalloc(hashed_details_length+1);
	sprintf(hashed_details,"%s",SAFE_STRING((char *)dbname));

	_php3_hash_find(OCI8_GLOBAL(php3_oci8_module).server, hashed_details, hashed_details_length+1, (void **) &server);

	if (server) {
		if (server->open) {
			/* if our new users uses this connection persistent, we're keeping it! */
			if (persistent) {
				server->persistent = persistent;
			}

			efree(hashed_details);
			return server;
		} else { /* server "died" in the meantime - try to reconnect! */
			_oci8_close_server(server);
			/* breakthru to open */
		}
	}

	server = calloc(1,sizeof(oci8_server));

	if (! server) {
		goto CLEANUP;
	}

	server->persistent = persistent;

	server->dbname = strdup(SAFE_STRING(dbname));
	
	OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
				   (dvoid **)&server->pServer, 
				   OCI_HTYPE_SERVER, 
				   0, 
				   NULL); 

	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIServerAttach(server->pServer,
						OCI8_GLOBAL(php3_oci8_module).pError,
						(text*)dbname,
						strlen(dbname),
						(ub4) OCI_DEFAULT);

	if (OCI8_GLOBAL(php3_oci8_module).error) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_open_server", OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}
	
	server->num = OCI8_GLOBAL(php3_oci8_module).server_num++;

	server->open = 1;


#if 0 
	server->failover.fo_ctx = (dvoid *) server;
	server->failover.callback_function = oci8_failover_callback;

	error = OCIAttrSet((dvoid *)server->pServer,
					   (ub4) OCI_HTYPE_SERVER,
					   (dvoid *) &server->failover, 
					   (ub4) 0,
					   (ub4) OCI_ATTR_FOCBK,
					   OCI8_GLOBAL(php3_oci8_module).pError);   

	if (error) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_open_server OCIAttrSet OCI_ATTR_FOCBK", error);
		goto CLEANUP;
	}
#endif

	oci8_debug("oci8_open_server new conn=%d dname=%s",server->num,server->dbname);

	_php3_hash_pointer_update(OCI8_GLOBAL(php3_oci8_module).server,
							  hashed_details,
							  hashed_details_length+1, 
							  (void *) server);

	efree(hashed_details);

	return server;

 CLEANUP:
	oci8_debug("oci8_open_server: FAILURE -> CLEANUP called");

	if (hashed_details) {
		efree(hashed_details);
	}

	_oci8_close_server(server);
		
	return 0;
}

/* }}} */
/* {{{ _oci8_close_server()
 */

static void
_oci8_close_server(oci8_server *server)
{
	OCI8_TLS_VARS;

	if (! server) {
		return;
	}

	oci8_debug("_oci8_close_server: detaching conn=%d dbname=%s",server->num,server->dbname);

	/* XXX close server here */

	if (server->open) {
		if (server->pServer && OCI8_GLOBAL(php3_oci8_module).pError) {
			OCI8_GLOBAL(php3_oci8_module).error = 
				OCIServerDetach(server->pServer,
								OCI8_GLOBAL(php3_oci8_module).pError,
								OCI_DEFAULT);
			
			if (OCI8_GLOBAL(php3_oci8_module).error) {
				oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_close_server OCIServerDetach", OCI8_GLOBAL(php3_oci8_module).error);
			}
		}
	} else {
		oci8_debug("_oci8_close_server: closind DEAD server");
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
/* {{{ oci8_do_connect()
  Connect to an Oracle database and log on. returns a new session.
 */
static void oci8_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent,int exclusive)
{
    char *username, *password, *dbname;
    pval *userParam, *passParam, *dbParam;
    oci8_server *server = 0;
    oci8_session *session = 0;
    oci8_connection *connection = 0;
    OCI8_TLS_VARS;
	
    if (getParameters(ht, 3, &userParam, &passParam, &dbParam) == SUCCESS) {
		convert_to_string(userParam);
		convert_to_string(passParam);
		convert_to_string(dbParam);

		username = userParam->value.str.val;
		password = passParam->value.str.val;
		dbname = dbParam->value.str.val;
    } else if (getParameters(ht, 2, &userParam, &passParam) == SUCCESS) {
		convert_to_string(userParam);
		convert_to_string(passParam);

		username = userParam->value.str.val;
		password = passParam->value.str.val;
		dbname = "";
    } else {
		WRONG_PARAM_COUNT;
    }

	connection = (oci8_connection *) ecalloc(1,sizeof(oci8_connection));

	if (! connection) {
		goto CLEANUP;
	}

	server = oci8_open_server(dbname,persistent);

	if (! server) {
		goto CLEANUP;
	}

	persistent = server->persistent; /* if our server-context is not persistent we can't */

	session = oci8_open_user(server,username,password,persistent,exclusive);

	if (! session) {
		goto CLEANUP;
	}

	/* set our session */
	connection->session = session;

	/* allocate our private error-handle */
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
					   (dvoid **)&connection->pError, 
					   OCI_HTYPE_ERROR, 
					   0, 
					   NULL);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_do_connect: OCIHandleAlloc OCI_HTYPE_ERROR",OCI8_GLOBAL(php3_oci8_module).error);
		goto CLEANUP;
	}

	/* allocate our service-context */
	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
					   (dvoid **)&connection->pServiceContext, 
					   OCI_HTYPE_SVCCTX, 
					   0, 
					   NULL);
	if (OCI8_GLOBAL(php3_oci8_module).error != OCI_SUCCESS) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError, "oci8_do_connect: OCIHandleAlloc OCI_HTYPE_SVCCTX",OCI8_GLOBAL(php3_oci8_module).error);
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
		oci8_error(connection->pError, "oci8_do_connect: OCIAttrSet OCI_ATTR_SERVER", connection->error);
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
		oci8_error(connection->pError, "oci8_do_connect: OCIAttrSet OCI_ATTR_SESSION", connection->error);
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


	connection->id = php3_list_insert(connection, OCI8_GLOBAL(php3_oci8_module).le_conn);

	connection->descriptors = emalloc(sizeof(HashTable));
	if (!connection->descriptors ||
		_php3_hash_init(connection->descriptors, 13, NULL,HASH_DTOR _oci8_free_descr, 0) == FAILURE) {
        goto CLEANUP;
    }

	connection->open = 1;

	oci8_debug("oci8_do_connect: id=%d",connection->id);

	RETURN_RESOURCE(connection->id);
	
 CLEANUP:
	oci8_debug("oci8_do_connect: FAILURE -> CLEANUP called");

	if (connection->id) {
		php3_list_delete(connection->id);
	} else {
		_oci8_close_conn(connection);
	}

	RETURN_FALSE;
}

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto int    OCIDefineByName(int stmt, string name, mixed &var [,int type])
  Define a PHP variable to an Oracle column by name.
  if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!!
 */

PHP_FUNCTION(oci8_definebyname)
{
	pval *stmt, *name, *var, *type;
	oci8_statement *statement;
	oci8_define *define, *tmp_define;
	ub2	ocitype;

	ocitype = SQLT_STR; /* zero terminated string */

	if (getParameters(ht, 4, &stmt, &name, &var, &type) == SUCCESS) {
		convert_to_long(type); 
		ocitype = (ub2) type->value.lval;
	} else if (getParameters(ht, 3, &stmt, &name, &var) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIDefineByName", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	convert_to_string(name);

	define = ecalloc(1,sizeof(oci8_define));
	if (!define) {
		/* out of memory */
		RETURN_FALSE;
	}
	if (statement->defines == NULL) {
		statement->defines = emalloc(sizeof(HashTable));
		if (statement->defines == NULL ||
			_php3_hash_init(statement->defines, 13, NULL,HASH_DTOR oci8_free_define, 0) == FAILURE) {
			/* out of memory */
			RETURN_FALSE;
		}
	}
	if (_php3_hash_add(statement->defines,
					   name->value.str.val,
					   name->value.str.len,
					   define,
					   sizeof(oci8_define),
					   (void **)&tmp_define) == SUCCESS) {
		efree(define);
		define = tmp_define;
	} else {
		RETURN_FALSE;
	}

	define->name = (text*) estrndup(name->value.str.val,name->value.str.len);
	define->name_len = name->value.str.len;
	define->type = ocitype;
	define->pval = var;

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int    OCIBindByName(int stmt, string name, mixed &var, int maxlength [,int type])
  Bind a PHP variable to an Oracle placeholder by name.
  if you want to bind a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE binding!!!
 */

PHP_FUNCTION(oci8_bindbyname)
{
	pval *stmt, *name, *var, *maxlen, *tmp,*type;
	oci8_statement *statement;
	oci8_statement *bindstmt;
	oci8_bind *bind, *tmp_bind;
	oci8_descriptor *descr;
	ub2	ocitype;
	sb4 ocimaxlen;
    OCIStmt *mystmt = 0;
	dvoid *mydescr = 0;

	ocitype = SQLT_STR; /* zero terminated string */

	if (getParameters(ht, 5, &stmt, &name, &var, &maxlen,&type) == SUCCESS) {
		convert_to_long(type); 
		ocitype = (ub2) type->value.lval;
		convert_to_long(maxlen); 
		ocimaxlen = maxlen->value.lval;
	} else if (getParameters(ht, 4, &stmt, &name, &var, &maxlen) == SUCCESS) {
		convert_to_long(maxlen); ocimaxlen = maxlen->value.lval; 
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIBindByName", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	switch (var->type) {
		case IS_OBJECT :
	        if (_php3_hash_find(var->value.ht, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
   		         php3_error(E_WARNING, "unable to find my descriptor property");
   		         RETURN_FALSE;
        	}

			if (_php3_hash_index_find(statement->conn->descriptors, tmp->value.lval, (void **)&descr) == FAILURE) {
   		         php3_error(E_WARNING, "unable to find my descriptor");
   		         RETURN_FALSE;
			}

			mydescr = (dvoid *) descr->ocidescr;

	        if (! mydescr) {
       		     RETURN_FALSE;
        	}
			break;
		
		default:
			if (ocitype == SQLT_RSET) { /* refcursor binding */
				convert_to_long(var);
				
				bindstmt = oci8_get_stmt(var->value.lval, "OCIBindByName", list);
				if (! bindstmt) {
					RETURN_FALSE;
				}
				ocimaxlen = 0;
				mystmt = bindstmt->pStmt;
			} else { /* everything else is now handled as a string */ 
				convert_to_string(var);
				if (ocimaxlen == -1) {
					if (var->value.str.len == 0) {
						php3_error(E_WARNING, "OCIBindByName bindlength is 0");
					}

					if (ocitype == SQLT_BIN) {
						ocimaxlen = var->value.str.len; 
					} else {
						ocimaxlen = var->value.str.len + 1; 
						/* SQLT_STR needs a trailing 0 - maybe we need to resize the var buffers????? */
					}
				}
			}
			break;
	}

	convert_to_string(name);

	bind = ecalloc(1,sizeof(oci8_bind));
	if (!bind) {
		/* out of memory */
		RETURN_FALSE;
	}
	if (statement->binds == NULL) {
		statement->binds = emalloc(sizeof(HashTable));
		if (statement->binds == NULL ||
			_php3_hash_init(statement->binds, 13, NULL, NULL, 0) == FAILURE) {
			/* out of memory */
			RETURN_FALSE;
		}
	}
	if (_php3_hash_next_index_insert(statement->binds, bind,
									 sizeof(oci8_bind),
									 (void **)&tmp_bind) == SUCCESS) {
		efree(bind);
		bind = tmp_bind;
	}

	bind->value = var;
	bind->descr = mydescr;
	bind->pStmt = mystmt;
	bind->maxsize = ocimaxlen;

	statement->error = 
		OCIBindByName(statement->pStmt, /* statement handle */
					  (OCIBind **)&bind->pBind, /* bind hdl (will alloc) */
					  statement->pError, /* error handle */
					  (text*) name->value.str.val, /* placeholder name */
					  name->value.str.len, /* placeholder length */
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
		oci8_error(statement->pError, "OCIBindByName", statement->error);
		RETURN_FALSE;
	}
	statement->error = 
		OCIBindDynamic(bind->pBind,
					   statement->pError,
					   (dvoid *)bind,
					   oci8_bind_in_callback,
					   (dvoid *)bind,
					   oci8_bind_out_callback);
	if (statement->error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIBindDynamic", statement->error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string ocifreedesc(object lob)
 */

PHP_FUNCTION(oci8_freedesc)
{
	pval *id, *conn, *desc;
	oci8_connection *connection;

	OCI8_TLS_VARS;

#if PHP_API_VERSION < 19990421       
	if (getThis(&id) == SUCCESS) {
#else
	if ((id = getThis()) != 0) {
#endif
        if (_php3_hash_find(id->value.ht, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
            php3_error(E_WARNING, "unable to find my statement property");
            RETURN_FALSE;
        }

        connection = oci8_get_conn(conn->value.lval, "OCIfreedesc", list);
        if (connection == NULL) {
            RETURN_FALSE;
        }

   		if (_php3_hash_find(id->value.ht, "descriptor", sizeof("descriptor"), (void **)&desc) == FAILURE) {
			php3_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

		oci8_debug("OCOfreedesc: descr=%d",desc->value.lval);

		_php3_hash_index_del(connection->descriptors,desc->value.lval);

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}
/* }}} */
/* {{{ proto string ocisavedesc(object lob)
 */

PHP_FUNCTION(oci8_savedesc)
{
	pval *id, *tmp, *conn, *arg;
	OCILobLocator *mylob;
	oci8_connection *connection;
	oci8_descriptor *descr;
	ub4 loblen;

	OCI8_TLS_VARS;

#if PHP_API_VERSION < 19990421       
	if (getThis(&id) == SUCCESS) {
#else
	if ((id = getThis()) != 0) {
#endif
   		if (_php3_hash_find(id->value.ht, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php3_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		connection = oci8_get_conn(conn->value.lval, "OCIsavedesc", list); 
		if (connection == NULL) {
			RETURN_FALSE;
		}

   		if (_php3_hash_find(id->value.ht, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
			php3_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

        if (_php3_hash_index_find(connection->descriptors, tmp->value.lval, (void **)&descr) == FAILURE) {
        	php3_error(E_WARNING, "unable to find my descriptor %d",tmp->value.lval);
            RETURN_FALSE;
        }

		mylob = (OCILobLocator *) descr->ocidescr;

		if (! mylob) {
			RETURN_FALSE;
		}

	    if (getParameters(ht, 1, &arg) == FAILURE) {
        	WRONG_PARAM_COUNT;
    	}

		loblen = arg->value.str.len;
	
		if (loblen < 1) {
			php3_error(E_WARNING, "Cannot save a lob wich size is less than 1 byte");
			RETURN_FALSE;
		}

		
		connection->error = 
			OCILobWrite(connection->pServiceContext,
						connection->pError,
						mylob,
						&loblen,
						(ub4) 1,
						(dvoid *) arg->value.str.val,
						(ub4) loblen,
						OCI_ONE_PIECE,
						(dvoid *)0,
						(OCICallbackLobWrite) 0,
						(ub2) 0,
						(ub1) SQLCS_IMPLICIT );

		oci8_debug("OCIsavedesc: size=%d",loblen);

		if (connection->error) {
			oci8_error(connection->pError, "OCILobWrite", connection->error);
			RETURN_FALSE;
		}

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}

/* }}} */
/* {{{ proto string ociloaddesc(object lob)
 */

PHP_FUNCTION(oci8_loaddesc)
{
	pval *id, *tmp, *conn;
	oci8_connection *connection;
	oci8_descriptor *descr;
	char *buffer;
	ub4 loblen;

	OCI8_TLS_VARS;

#if PHP_API_VERSION < 19990421       
	if (getThis(&id) == SUCCESS) {
#else
	if ((id = getThis()) != 0) {
#endif
   		if (_php3_hash_find(id->value.ht, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php3_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		connection = oci8_get_conn(conn->value.lval, "OCIsavedesc", list); 
		if (connection == NULL) {
			RETURN_FALSE;
		}

   		if (_php3_hash_find(id->value.ht, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
			php3_error(E_WARNING, "unable to find my locator property");
			RETURN_FALSE;
		}

        if (_php3_hash_index_find(connection->descriptors, tmp->value.lval, (void **)&descr) == FAILURE) {
        	php3_error(E_WARNING, "unable to find my descriptor %d",tmp->value.lval);
            RETURN_FALSE;
        }

		loblen = oci8_loaddesc(connection,descr,&buffer);

		if (loblen > 0) {
	 		RETURN_STRINGL(buffer,loblen,0);
		}
	}

  RETURN_FALSE;
}
/* }}} */
/* {{{ proto string OCINewDescriptor(int connection [,int type ])
  initialize a new empty descriptor LOB/FILE (LOB is default)
 */

PHP_FUNCTION(oci8_newdescriptor)
{
	pval *conn, *type;
	oci8_connection *connection;
	oci8_descriptor descr;
	int mylob;

	OCI8_TLS_VARS;

    descr.type = OCI_DTYPE_LOB;

	if (getParameters(ht, 2, &conn, &type) == SUCCESS) {
		descr.type = type->value.lval;
	} else if (getParameters(ht, 1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (descr.type) {
	    case OCI_DTYPE_FILE:
	    case OCI_DTYPE_LOB:
	    case OCI_DTYPE_ROWID:
			break;
		   
	    default:
			php3_error(E_WARNING, "Unknown descriptor type %d.",descr.type);
			RETURN_FALSE;
	}

	convert_to_long(conn);

	connection = oci8_get_conn(conn->value.lval, "OCINewDescriptor", list);
    if (connection == NULL) {
        RETURN_FALSE;
    }

	OCI8_GLOBAL(php3_oci8_module).error = 
		OCIDescriptorAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
						   (dvoid*)&(descr.ocidescr), 
						   descr.type, 
						   (size_t) 0, 
						   (dvoid **) 0);

	if (OCI8_GLOBAL(php3_oci8_module).error) {
		oci8_error(OCI8_GLOBAL(php3_oci8_module).pError,"OCIDescriptorAlloc %d",OCI8_GLOBAL(php3_oci8_module).error);
		RETURN_FALSE;
	}

	_php3_hash_index_update(connection->descriptors, connection->descriptors_count,&descr,sizeof(oci8_descriptor),NULL);

	mylob = connection->descriptors_count++;

	oci8_debug("OCINewDescriptor: new descriptor for %d -> %x",mylob,descr.ocidescr);
	
	object_init(return_value);
	add_property_long(return_value, "descriptor", (long) mylob);
	add_property_long(return_value, "connection", conn->value.lval);
	add_method(return_value, "free", php3_oci8_freedesc);

	switch (descr.type) {
		case OCI_DTYPE_LOB :
			add_method(return_value, "save", php3_oci8_savedesc);
			/* breaktruh */
		case OCI_DTYPE_FILE :
			add_method(return_value, "load", php3_oci8_loaddesc);
			break;
			
	}

	add_method(return_value, "free", php3_oci8_freedesc);
}

/* }}} */
/* {{{ proto string OCIRollback(int conn)
  rollback the current context
 */

PHP_FUNCTION(oci8_rollback)
{
	pval *conn;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(conn);

	connection = oci8_get_conn(conn->value.lval, "OCIRollback", list);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	connection->error = 
		OCITransRollback(connection->pServiceContext,
						 connection->pError, 
						 (ub4)0);

	if (connection->error) {
		oci8_error(connection->pError, "OCIRollback", connection->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string OCICommit(int conn)
  commit the current context
 */

PHP_FUNCTION(oci8_commit)
{
	pval *conn;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(conn);

	connection = oci8_get_conn(conn->value.lval, "OCICommit", list);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	connection->error = 
		OCITransCommit(connection->pServiceContext,
					   connection->pError, 
					   (ub4)0);

	if (connection->error) {
		oci8_error(connection->pError, "OCICommit", connection->error);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string OCIColumnName(int stmt, int col)
  Tell the name of a column.
 */

PHP_FUNCTION(oci8_columnname)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnName", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, -1, col, "OCIColumnName");
	if (outcol == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(outcol->name, outcol->name_len, 1);
}

/* }}} */
/* {{{ proto int    OCIColumnSize(int stmt, int col)
  Tell the maximum data size of a column.
 */

PHP_FUNCTION(oci8_columnsize)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnSize", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, -1, col, "OCIColumnSize");
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(outcol->data_size);
}

/* }}} */
/* {{{ proto mixed  OCIColumnType(int stmt, int col)
  Tell the data type of a column.
 */

PHP_FUNCTION(oci8_columntype)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, -1, col, "OCIColumnType");
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

PHP_FUNCTION(oci8_columnisnull)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnIsNULL", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, -1, col, "OCIColumnIsNULL");
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
PHP_FUNCTION(oci8_internaldebug)
{
	pval *arg;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	OCI8_GLOBAL(php3_oci8_module).debug_mode = arg->value.lval;
}


/* }}} */
/* {{{ proto int    OCIExecute(int stmt [,int mode])
  Execute a parsed statement.
 */

PHP_FUNCTION(oci8_execute)
{
	pval *stmt,*mode;
	oci8_statement *statement;
	ub4 execmode;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &mode) == SUCCESS) {
		convert_to_long(mode);
		execmode = mode->value.lval;
	} else if (getParameters(ht, 1, &stmt) == SUCCESS) {
		execmode = OCI_COMMIT_ON_SUCCESS;
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIExecute", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	if (oci8_execute(statement, "OCIExecute",execmode,list)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCICancel(int stmt)
  Prepare a new row of data for reading.
 */

PHP_FUNCTION(oci8_cancel)
{
	pval *stmt;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);

	statement = oci8_get_stmt(stmt->value.lval, "OCICancel", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	if (oci8_fetch(statement, 0, "OCICancel")) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCIFetch(int stmt)
  Prepare a new row of data for reading.
 */

PHP_FUNCTION(oci8_fetch)
{
	pval *stmt;
	oci8_statement *statement;
	ub4 nrows = 1; /* only one row at a time is supported for now */
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);

	statement = oci8_get_stmt(stmt->value.lval, "OCIFetch", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	if (oci8_fetch(statement, nrows, "OCIFetch")) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCIFetchInto(int stmt, array &output [, int mode])
  Fetch a row of result data into an array.
 */

PHP_FUNCTION(oci8_fetchinto)
{
	pval *stmt, *array, *element, *fmode;
	oci8_statement *statement;
	oci8_out_column *column;
	ub4 nrows = 1;
	int i;
	int mode = OCI_NUM;
	OCI8_TLS_VARS;

	if (getParameters(ht, 3, &stmt, &array, &fmode) == SUCCESS) {
		convert_to_long(fmode);
		mode = fmode->value.lval;
	} else if (getParameters(ht, 2, &stmt, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIFetchInto", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	if (!oci8_fetch(statement, nrows, "OCIFetchInto")) {
		RETURN_FALSE;
	}

	/* 
	   if we don't want NULL columns back, we need to recreate the array
	   as it could have a different number of enties for each fetched row
	*/

	if (! (mode & OCI_RETURN_NULLS)) { 
		if (array->type == IS_ARRAY) { 
			/* XXX is that right?? */
			_php3_hash_destroy(array->value.ht);
			efree(array->value.ht);
			var_reset(array);
		}
	}

	if (array->type != IS_ARRAY) {
		if (array_init(array) == FAILURE) {
			php3_error(E_WARNING, "OCIFetchInto: unable to convert arg 2 to array");
			RETURN_FALSE;
		}
	}

#if PHP_API_VERSION < 19990421
	element = emalloc(sizeof(pval));
#endif

	for (i = 0; i < statement->ncolumns; i++) {
		column = oci8_get_col(statement, i + 1, 0, "OCIFetchInto");
		if (column == NULL) { /* should not happen... */
			continue;
		}
	
		if ((column->indicator == -1) && ((mode & OCI_RETURN_NULLS) == 0)) {
			continue;
		}

		if ((mode & OCI_NUM) || (! (mode & OCI_ASSOC))) { /* OCI_NUM is default */
#if PHP_API_VERSION >= 19990421
			MAKE_STD_ZVAL(element);
#endif
			oci8_make_pval(element,statement,column, "OCIFetchInto",mode);

#if PHP_API_VERSION >= 19990421
			_php3_hash_index_update(array->value.ht, i, (void *)&element, sizeof(pval*), NULL);
#else
			_php3_hash_index_update(array->value.ht, i, (void *)element, sizeof(pval), NULL);
#endif
		}

		if (mode & OCI_ASSOC) {
#if PHP_API_VERSION >= 19990421
			MAKE_STD_ZVAL(element);
#endif
			oci8_make_pval(element,statement,column, "OCIFetchInto",mode);

#if PHP_API_VERSION >= 19990421
		  	_php3_hash_update(array->value.ht, column->name, column->name_len+1, (void *)&element, sizeof(pval*), NULL);
#else
		  	_php3_hash_update(array->value.ht, column->name, column->name_len+1, (void *)element, sizeof(pval), NULL);
#endif
		}
	}

#if PHP_API_VERSION < 19990421
	efree(element); 
#endif

	RETURN_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ proto int    OCIFetchStatement(int stmt, array &output)
  Fetch all rows of result data into an array.
 */

PHP_FUNCTION(oci8_fetchstatement)
{
	pval *stmt, *array, *element, *fmode, *tmp;
	oci8_statement *statement;
	oci8_out_column **columns;
#if PHP_API_VERSION < 19990421
	pval **outarrs;
#else
	pval ***outarrs;
#endif
	ub4 nrows = 1;
	int i;
	int mode = OCI_NUM;
	int rows = 0;
	char namebuf[ 128 ];
	OCI8_TLS_VARS;

	if (getParameters(ht, 3, &stmt, &array, &fmode) == SUCCESS) {
		convert_to_long(fmode);
		mode = fmode->value.lval;
	} else if (getParameters(ht, 2, &stmt, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIFetchStatement", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	if (array->type != IS_ARRAY) {
		if (array_init(array) == FAILURE) {
			php3_error(E_WARNING, "OCIFetchStatement: unable to convert arg 2 to array");
			RETURN_FALSE;
		}
	}

	columns = emalloc(statement->ncolumns * sizeof(oci8_out_column *));
#if PHP_API_VERSION < 19990421
	outarrs = emalloc(statement->ncolumns * sizeof(pval));
#else
	outarrs = emalloc(statement->ncolumns * sizeof(pval*));
#endif

#if PHP_API_VERSION < 19990421
	tmp = emalloc(sizeof(pval));
#endif

	for (i = 0; i < statement->ncolumns; i++) {
		columns[ i ] = oci8_get_col(statement, i + 1, 0, "OCIFetchStatement");

#if PHP_API_VERSION >= 19990421
		MAKE_STD_ZVAL(tmp);
#endif

		array_init(tmp);

		memcpy(namebuf,columns[ i ]->name, columns[ i ]->name_len);
		namebuf[ columns[ i ]->name_len ] = 0;
				
#if PHP_API_VERSION < 19990421
		_php3_hash_update(array->value.ht, namebuf, columns[ i ]->name_len+1, (void *) tmp, sizeof(pval), (void **) &(outarrs[ i ]));
#else
		_php3_hash_update(array->value.ht, namebuf, columns[ i ]->name_len+1, (void *) &tmp, sizeof(pval*), (void **) &(outarrs[ i ]));
#endif
	}

#if PHP_API_VERSION < 19990421
	efree(tmp);
#endif

#if PHP_API_VERSION < 19990421
	element = emalloc(sizeof(pval));
#endif

	while (oci8_fetch(statement, nrows, "OCIFetchStatement")) {
		for (i = 0; i < statement->ncolumns; i++) {
#if PHP_API_VERSION >= 19990421
			MAKE_STD_ZVAL(element);
#endif
			oci8_make_pval(element,statement,columns[ i ], "OCIFetchStatement",OCI_RETURN_LOBS);

#if PHP_API_VERSION < 19990421
			_php3_hash_index_update(outarrs[ i ]->value.ht, rows, (void *)element, sizeof(pval), NULL);
#else
			_php3_hash_index_update((*(outarrs[ i ]))->value.ht, rows, (void *)&element, sizeof(pval*), NULL);
#endif
		}
		rows++;
	}
	
#if PHP_API_VERSION < 19990421
	efree(element);
#endif

	efree(columns);
	efree(outarrs);

	RETURN_LONG(rows);
}

/* }}} */
/* {{{ proto int    OCIFreeStatement(int stmt)
  Free all resources associated with a statement.
 */

PHP_FUNCTION(oci8_freestatement)
{
	pval *stmt;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIFreeStatement", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	php3_list_delete(stmt->value.lval);

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int    OCILogoff(int conn)
  Disconnect from database.
 */

/* Logs off and disconnects.
 */
PHP_FUNCTION(oci8_logout)
{
	oci8_connection *connection;
	pval *arg;
	int index, index_t;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	index = arg->value.lval;
	connection = (oci8_connection *)php3_list_find(index, &index_t);

	if (!connection) {
		oci8_debug("OCILogoff: connection == NULL.");
		RETURN_FALSE;
	}

	if (! OCI8_CONN_TYPE(index_t)) {
		oci8_debug("OCILogoff: connection not found...");
		RETURN_FALSE;
	}

	connection->open = 0;

	_php3_hash_apply(list,(int (*)(void *))_stmt_cleanup);

	if (php3_list_delete(index) == SUCCESS) {
		RETURN_TRUE;
	} else {
		oci8_debug("OCILogoff: php3_list_delete failed.");
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
PHP_FUNCTION(oci8_nlogon)
{
	oci8_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0,1);
}

/* }}} */
/* {{{ proto int    OCILogon(string user, string pass[, string db])
  Connect to an Oracle database and log on. returns a new session.
 */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(oci8_logon)
{
	oci8_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0,0);
}

/* }}} */
/* {{{ proto int    OCIPLogon(string user, string pass[, string db])
  Connect to an Oracle database and log on. returns a new session.
 */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
PHP_FUNCTION(oci8_plogon)
{
	oci8_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1,0);
}

/* }}} */
/* {{{ proto int    OCIError([int stmt|conn])
  Return the last error of stmt|conn|global. If no error happened returns false.
 */

PHP_FUNCTION(oci8_error)
{
	pval *mixed;
	oci8_statement *statement;
	oci8_connection *connection;
    text errbuf[512];
    sb4 errcode = 0;
	int type;
	sword error = 0;
	dvoid *errh = NULL;

	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &mixed) == SUCCESS) {
		convert_to_long(mixed);
		statement = (oci8_statement *)php3_list_find(mixed->value.lval, &type);
		if (statement && OCI8_STMT_TYPE(type)) {
			errh = statement->pError;
			error = statement->error;
		} else {
			connection = (oci8_connection *)php3_list_find(mixed->value.lval, &type);
			if (connection && OCI8_CONN_TYPE(type)) {
				errh = connection->pError;
				error = connection->error;
			}
		}
	} else {
		errh = OCI8_GLOBAL(php3_oci8_module).pError;
		error = OCI8_GLOBAL(php3_oci8_module).error;
	}

	if (! error) { /* no error set in the handle */
		RETURN_FALSE;
	}

	if (! errh) {
		php3_error(E_WARNING, "OCIError: unable to find Error handle");
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

PHP_FUNCTION(oci8_numcols)
{
	pval *stmt;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCINumCols", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ proto int    OCIParse(int conn, string query)
  Parse a query and return a statement.
 */

PHP_FUNCTION(oci8_parse)
{
	pval *conn, *query;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &conn, &query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(conn);
	convert_to_string(query);

	connection = oci8_get_conn(conn->value.lval, "OCIParse", list);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	RETURN_RESOURCE(oci8_parse(connection,
							   query->value.str.val,
							   query->value.str.len,
							   list));
}

/* }}} */
/* {{{ proto int    OCIParse(int conn, string query)
  Parse a query and return a statement.
 */

PHP_FUNCTION(oci8_setprefetch)
{
	pval *stmt, *size;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);
	convert_to_long(size);

	statement = oci8_get_stmt(stmt->value.lval, "OCISetPreFetch", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	oci8_setprefetch(statement,size->value.lval);

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto int    OCINewCursor(int conn)
   return a new cursor (Statement-Handle) - use this to bind ref-cursors!
 
 */

PHP_FUNCTION(oci8_newcursor)
{
	pval *conn;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(conn);

	connection = oci8_get_conn(conn->value.lval, "OCINewCursor", list);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	RETURN_RESOURCE(oci8_parse(connection,
							   0,
							   0,
							   list));
}

/* }}} */
/* {{{ proto string OCIResult(int stmt, mixed column)
  Return a single column of result data.
 */

PHP_FUNCTION(oci8_result)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol = NULL;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(stmt);

	statement = oci8_get_stmt(stmt->value.lval, "OCIResult", list);

	if (statement == NULL) {
		RETURN_FALSE;
	}

	outcol = oci8_get_col(statement, -1, col, "OCIResult");

	if (outcol == NULL) {
		RETURN_FALSE;
	}

	oci8_make_pval(return_value,statement,outcol, "OCIResult",0);
}

/* }}} */
/* {{{ proto string OCIServerVersion(int conn)
  Return a string containing server version information.
 */

PHP_FUNCTION(oci8_serverversion)
{
	oci8_connection *connection;
	pval *arg;
	int index, index_t;
	char version[256];
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	index = arg->value.lval;
	connection = (oci8_connection *)php3_list_find(index, &index_t);
	if (!connection || !OCI8_CONN_TYPE(index_t)) {
		RETURN_FALSE;
	}
	connection->error = 
		OCIServerVersion(connection->pServiceContext,
						 connection->pError, 
						 (text*)version, 
						 sizeof(version),
						 OCI_HTYPE_SVCCTX);
	if (connection->error != OCI_SUCCESS) {
		oci8_error(connection->pError, "OCIServerVersion", connection->error);
		RETURN_FALSE;
	}
	RETURN_STRING(version,1);
}

/* }}} */
/* {{{ proto int    OCIStatementType(int stmt)
  Return the query type of an OCI statement.
 */
 
/* XXX it would be better with a general interface to OCIAttrGet() */

PHP_FUNCTION(oci8_statementtype)
{
	pval *stmt;
	oci8_statement *statement;
	ub2 stmttype;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIStatementType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	statement->error = 
		OCIAttrGet((dvoid *)statement->pStmt, 
				   OCI_HTYPE_STMT,
				   (ub2 *)&stmttype, 
				   (ub4 *)0, 
				   OCI_ATTR_STMT_TYPE,
				   statement->pError);
	if (statement->error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIStatementType", statement->error);
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

PHP_FUNCTION(oci8_rowcount)
{
	pval *stmt;
	oci8_statement *statement;
	ub4 rowcount;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIStatementType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	statement->error = 
		OCIAttrGet((dvoid *)statement->pStmt, 
				   OCI_HTYPE_STMT,
				   (ub2 *)&rowcount, 
				   (ub4 *)0, 
				   OCI_ATTR_ROW_COUNT,
				   statement->pError);

	if (statement->error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIRowCount", statement->error);
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
