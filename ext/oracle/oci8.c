/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
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
 * - Error mode (print or shut up?)
 * - returning refcursors as statement handles
 * - OCIPasswordChange()
 * - Prefetching control
 * - LONG, LONG RAW, RAW is now limited to 2MB (should be user-settable);
 * - binding of arrays
 * - Truncate input values to the bind size
 * - Character sets for NCLOBS
 * - piecewise operation for longs, lobs etc
 * - split the module into an upper (php-callable) and lower (c-callable) layer!
 * - make_pval needs some cleanup....
 * - persistend connections
 * - NULLS (retcode/indicator) needs some more work for describing & binding
 * - remove all XXXs
 * - clean up and documentation
 * - OCINewContext function.
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
/*#include "internal_functions.h"*/
#include "php3_oci8.h"

#if HAVE_OCI8

#define SAFE_STRING(s) ((s)?(s):"")

/*#include "php3_list.h"*/
#if !(WIN32|WINNT)
# include "build-defs.h"
#endif
#include "snprintf.h"
#include "head.h"

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

int php3_minit_oci8(INIT_FUNC_ARGS);
int php3_rinit_oci8(INIT_FUNC_ARGS);
int php3_mshutdown_oci8(SHUTDOWN_FUNC_ARGS);
int php3_rshutdown_oci8(SHUTDOWN_FUNC_ARGS);
void php3_info_oci8(void);

static ub4 oci8_error(OCIError *err_p, char *what, sword status);
/* static int oci8_ping(oci8_connection *conn); XXX NYI */
static void oci8_debug(const char *format,...);

static void _oci8_close_conn(oci8_connection *connection);
static void _oci8_free_stmt(oci8_statement *statement);
static void _oci8_free_column(oci8_out_column *column);
static void _oci8_detach(oci8_server *server);
static void _oci8_logoff(oci8_session *session);
static void _oci8_free_descr(oci8_descriptor *descr);

static oci8_connection *oci8_get_conn(int, const char *, HashTable *, HashTable *);
static oci8_statement *oci8_get_stmt(int, const char *, HashTable *);
static oci8_out_column *oci8_get_col(oci8_statement *, int, pval *, char *);

static int oci8_make_pval(pval *,int,oci8_out_column *, char *,HashTable *, int mode);
static int oci8_parse(oci8_connection *, text *, ub4, HashTable *);
static int oci8_execute(oci8_statement *, char *,ub4 mode);
static int oci8_fetch(oci8_statement *, ub4, char *);
static ub4 oci8_loaddesc(oci8_connection *, oci8_descriptor *, char **);

static void oci8_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
static oci8_server *oci8_attach(char *dbname,int persistent,HashTable *list, HashTable *plist);
static oci8_session *oci8_login(oci8_server* server,char *username,char *password,int persistent,HashTable *list, HashTable *plist);


/* bind callback functions */
static sb4 oci8_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
static sb4 oci8_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

/* define callback function */
static sb4 oci8_define_callback(dvoid *, OCIDefine *, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

/* }}} */
/* {{{ extension function prototypes */

void php3_oci8_bindbyname(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_definebyname(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columnisnull(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columnname(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columnsize(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columntype(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_execute(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_fetch(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_fetchinto(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_fetchstatement(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_freestatement(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_internaldebug(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_logoff(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_logon(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_plogon(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_error(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_freedesc(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_savedesc(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_loaddesc(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_commit(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_rollback(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_newdescriptor(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_numcols(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_parse(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_result(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_serverversion(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_statementtype(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_rowcount(INTERNAL_FUNCTION_PARAMETERS);

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
	{"ocifetch",         php3_oci8_fetch,         NULL},
	{"ocifetchinto",     php3_oci8_fetchinto,     a2_arg_force_ref},
	{"ocifetchstatement",php3_oci8_fetchstatement,a2_arg_force_ref},
	{"ocifreestatement", php3_oci8_freestatement, NULL},
	{"ociinternaldebug", php3_oci8_internaldebug, NULL},
	{"ocinumcols",       php3_oci8_numcols,       NULL},
	{"ociparse",         php3_oci8_parse,         NULL},
	{"ociresult",        php3_oci8_result,        NULL},
	{"ociserverversion", php3_oci8_serverversion, NULL},
	{"ocistatementtype", php3_oci8_statementtype, NULL},
	{"ocirowcount", 	 php3_oci8_rowcount, 	  NULL},
    {"ocilogoff",        php3_oci8_logoff,        NULL},
    {"ocilogon",         php3_oci8_logon,         NULL},
    {"ociplogon",        php3_oci8_plogon,        NULL},
    {"ocierror",         php3_oci8_error,         NULL},
    {"ocifreedescriptor",php3_oci8_freedesc,      NULL},
    {"ocisavedesc",      php3_oci8_savedesc,      NULL},
    {"ociloaddesc",      php3_oci8_loaddesc,      NULL},
    {"ocicommit",        php3_oci8_commit,        NULL},
    {"ocirollback",      php3_oci8_rollback,      NULL},
    {"ocinewdescriptor", php3_oci8_newdescriptor, NULL},
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

	OCI8_GLOBAL(php3_oci8_module).le_conn =
		register_list_destructors(_oci8_close_conn, NULL);

	OCI8_GLOBAL(php3_oci8_module).le_stmt =
		register_list_destructors(_oci8_free_stmt, NULL);

	OCI8_GLOBAL(php3_oci8_module).le_server =
		register_list_destructors(_oci8_detach, NULL);

	OCI8_GLOBAL(php3_oci8_module).le_pserver =
		register_list_destructors(_oci8_detach, NULL);

	OCI8_GLOBAL(php3_oci8_module).le_session =
		register_list_destructors(_oci8_logoff, NULL);

	OCI8_GLOBAL(php3_oci8_module).le_psession =
		register_list_destructors(_oci8_logoff, NULL);


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
    OCIInitialize(OCI_DEFAULT, (dvoid *)connection, ocimalloc, ocirealloc, ocifree);
#else
    OCIInitialize(OCI_DEFAULT, NULL, NULL, NULL, NULL);
#endif

	OCIEnvInit(&OCI8_GLOBAL(php3_oci8_module).pEnv, OCI_DEFAULT, 0, NULL);

	return SUCCESS;
}

/* ----------------------------------------------------------------- */


int php3_rinit_oci8(INIT_FUNC_ARGS)
{
	OCI8_TLS_VARS;
	
	OCI8_GLOBAL(php3_oci8_module).num_links = 
		OCI8_GLOBAL(php3_oci8_module).num_persistent;

	OCI8_GLOBAL(php3_oci8_module).debug_mode = 0; /* start "fresh" */

    oci8_debug("php3_rinit_oci8");

    return SUCCESS;
}


int php3_mshutdown_oci8(SHUTDOWN_FUNC_ARGS)
{
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


int php3_rshutdown_oci8(SHUTDOWN_FUNC_ARGS)
{
    oci8_debug("php3_rshutdown_oci8");
	/* XXX free all statements, rollback all outstanding transactions */
    return SUCCESS;
}


void php3_info_oci8()
{
#if !(WIN32|WINNT)
	php3_printf("Oracle version: %s<br>\n"
			    "Compile-time ORACLE_HOME: %s<br>\n"
			    "Libraries used: %s",
			    PHP_ORACLE_VERSION, PHP_ORACLE_HOME, PHP_ORACLE_LIBS);
#endif
}

/* }}} */
/* {{{ debug malloc/realloc/free */

#if OCI8_USE_EMALLOC
CONST dvoid *ocimalloc(dvoid *ctx, size_t size)
{
    dvoid *ret;
	ret = (dvoid *)emalloc(size);
    oci8_debug("ocimalloc(%d) = %08x", size,ret);
    return ret;
}

CONST dvoid *ocirealloc(dvoid *ctx, dvoid *ptr, size_t size)
{
    dvoid *ret;
    oci8_debug("ocirealloc(%08x, %d)", ptr, size);
	ret = (dvoid *)erealloc(ptr, size);
    return ptr;
}

CONST void ocifree(dvoid *ctx, dvoid *ptr)
{
    oci8_debug("ocifree(%08x)", ptr);
    efree(ptr);
}
#endif

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
	return 0;
}

/* }}} */
/* {{{ _oci8_free_column() */

static void
_oci8_free_column(oci8_out_column *column)
{
	
	if (! column) {
		return;
	}

	oci8_debug("_oci8_free_column: %s",column->name);

	if (column->data) {
		if (column->is_descr) {
			_php3_hash_index_del(column->statement->conn->descriptors,(int) column->data);
		} else {
			if (! column->define) {
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

	oci8_debug("_oci8_free_stmt: id=%d last_query=\"%s\" conn=%d",
			   statement->id,
			   (statement->last_query?(char*)statement->last_query:"???"),
			   statement->conn->id);

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
    ub4 errcode = 0;

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

/* {{{ oci8_debugcol() */
#if 0
static void oci8_debugcol(oci8_out_column *column,const char *format,...)
{
	OCI8_TLS_VARS;

    if (OCI8_GLOBAL(php3_oci8_module).debug_mode) {
		char buffer[1024];
		char colbuffer[1024];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer)-1, format, args);
		va_end(args);
		buffer[sizeof(buffer)-1] = '\0';

		sprintf(colbuffer,"name=%s,type=%d,size4=%ld,size2=%d,storage_size4=%ld,indicator=%d,retcode=%d,rlen=%ld",
				column->name,column->type,column->size4,column->size2,column->storage_size4,column->indicator,column->retcode,column->rlen);

		if (php3_header()) {
			php3_printf("OCIDebug:%s - %s<br>\n",buffer,colbuffer);
		}
	}
}
#endif
/* }}} */
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
oci8_get_conn(int conn_ind, const char *func, HashTable *list, HashTable *plist)
{
	int type;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	connection = (oci8_connection *)php3_list_find(conn_ind, &type);
	if (!connection || !OCI8_CONN_TYPE(type)) {
		php3_error(E_WARNING, "%s: invalid connection %d", func, conn_ind);
		return (oci8_connection *)NULL;
	}
	return connection;
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
	return statement;
}

/* }}} */
/* {{{ oci8_get_col() */

static oci8_out_column *
oci8_get_col(oci8_statement *statement, int col, pval *pval, char *func)
{
	oci8_out_column *outcol = NULL;
	int i;
	OCI8_TLS_VARS;

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
oci8_make_pval(pval *value,int stmt_ind,oci8_out_column *column, char *func,HashTable *list, int mode)
{
	size_t size;
	oci8_statement *statement;
	oci8_descriptor *descr;
	ub4 loblen;
	char *buffer;

	/*
	oci8_debug("oci8_make_pval: %16s,rlen = %4d,storage_size4 = %4d,size2 = %4d,indicator %4d, retcode = %4d",
			   column->name,column->rlen,column->storage_size4,column->size2,column->indicator,column->retcode);
	*/

	memset(value,0,sizeof(pval));

	statement = oci8_get_stmt(stmt_ind, "oci8_make_pval", list);

	if (column->indicator == -1) { /* column is NULL */
		var_reset(value); /* XXX we NEED to make sure that there's no data attached to this yet!!! */
		return 0;
	}

	if (column->is_descr) {
		if ((column->type != SQLT_RDD) && (mode & OCI_RETURN_LOBS)) {
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

			if (column->type != SQLT_RDD) { /* ROWIDs don't have any user-callable methods */
				if ((column->type != SQLT_BFILEE) && (column->type != SQLT_CFILEE)) { 
					add_method(value, "save", php3_oci8_savedesc); /* oracle does not support writing of files as of now */
				}
				add_method(value, "load", php3_oci8_loaddesc);
			}
			/* there is NO free call here, 'cause the memory gets deallocated together with the statement! */
		}
	} else {
		switch (column->retcode) {
			case 1406: /* ORA-01406 XXX truncated value */
				/* this seems to be a BUG in oracle with 1-digit numbers */
				/*
				  oci8_debugcol(column,"truncated");
				*/
				size = column->indicator - 1; /* when value is truncated indicator contains the lenght */
				break;

			case 0: /* intact value */
			    /* 
				   oci8_debugcol(column,"OK");
				*/
				size = column->rlen;
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
/* {{{ oci8_parse() */

static int
oci8_parse(oci8_connection *connection, text *query, ub4 len, HashTable *list)
{
	oci8_statement *statement;
	sword error;
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
    error = oci8_error(statement->pError, "OCIParse",
					   OCIStmtPrepare(statement->pStmt, connection->pError,
									  query, len,
									  OCI_NTV_SYNTAX, OCI_DEFAULT));
	if (error) {
		return 0;
	}

	statement->last_query = estrdup(query);
	statement->conn = connection;
	statement->id = php3_list_insert(statement, OCI8_GLOBAL(php3_oci8_module).le_stmt);

	oci8_debug("oci8_parse \"%s\" id=%d conn=%d",
			   query,
			   statement->id,
			   statement->conn->id);

	return statement->id;
}

/* }}} */
/* {{{ oci8_execute() */

static int
oci8_execute(oci8_statement *statement, char *func,ub4 mode)
{
	oci8_out_column *outcol;
	oci8_out_column column;
	OCIParam *param = 0;
	text *colname;
	ub4 counter;
	sword error;
	ub2 define_type;
	ub2 stmttype;
	ub4 iters;
	ub4 colcount;
	ub2 storage_size2;
	OCI8_TLS_VARS;

	error = oci8_error(
				statement->pError,
			   	"OCIAttrGet OCI_HTYPE_STMT/OCI_ATTR_STMT_TYPE",
			   	OCIAttrGet(
					(dvoid *)statement->pStmt,
				  	OCI_HTYPE_STMT,
					(ub2 *)&stmttype,
				  	(ub4 *)0,
					OCI_ATTR_STMT_TYPE,
					statement->pError));

	if (error) {
		return 0;
	}

	if (stmttype == OCI_STMT_SELECT) {
		iters = 0;
	} else {
		iters = 1;
	}

	error = oci8_error(
				statement->pError,
			   	"OCIStmtExecute",
				OCIStmtExecute(
					statement->conn->pServiceContext,
					statement->pStmt,
					statement->pError,
					iters,
					0,
					NULL,
					NULL,
					mode));
	if (error) {
		return 0;
	}


	if (stmttype == OCI_STMT_SELECT && (statement->executed == 0)) {
		/* we only need to do the define step is this very statement is executed the first time! */
		statement->executed++;

		statement->columns = emalloc(sizeof(HashTable));
		if (!statement->columns ||
			_php3_hash_init(statement->columns, 13, NULL,(void (*)(void *))_oci8_free_column, 0) == FAILURE) {
			/* out of memory */
			return 0;
		}

#if 0
		error = oci8_error(
					statement->pError,
					"OCIHandleAlloc OCI_DTYPE_PARAM",
					OCIHandleAlloc(
						OCI8_GLOBAL(php3_oci8_module).pEnv,
					   	(dvoid **)&param,
					   	OCI_DTYPE_PARAM,
					   	0,
					   	NULL));
		if (error) {
			return 0; /* XXX we loose memory!!! */
		}
#endif
		OCIHandleAlloc(
			OCI8_GLOBAL(php3_oci8_module).pEnv,
			(dvoid **)&param,
			OCI_DTYPE_PARAM,
			0,
			NULL);


		counter = 1;

		error = oci8_error(
					statement->pError,
					"OCIAttrGet OCI_HTYPE_STMT/OCI_ATTR_PARAM_COUNT",
					OCIAttrGet(
						(dvoid *)statement->pStmt,
						OCI_HTYPE_STMT,
			   			(dvoid *)&colcount,
						(ub4 *)0,
						OCI_ATTR_PARAM_COUNT,
			   			statement->pError));
		if (error) {
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

			error = oci8_error(
						statement->pError,
					   	"OCIParamGet OCI_HTYPE_STMT",
					   	OCIParamGet(
							(dvoid *)statement->pStmt,
							OCI_HTYPE_STMT,
							statement->pError,
							(dvoid*)&param,
							counter));
			if (error) {
				return 0; /* XXX we loose memory!!! */
			}

			error = oci8_error(
						statement->pError, 
					   	"OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_TYPE",
					   	OCIAttrGet(
							(dvoid *)param,
							OCI_DTYPE_PARAM,
							(dvoid *)&outcol->type,
							(ub4 *)0,
							OCI_ATTR_DATA_TYPE,
							statement->pError));
			if (error) {
				return 0; /* XXX we loose memory!!! */
			}

			error = oci8_error(
						statement->pError,
					   	"OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_DATA_SIZE",
					   	OCIAttrGet(
							(dvoid *)param,
							OCI_DTYPE_PARAM,
							(dvoid *)&storage_size2,
							(dvoid *)0,
							OCI_ATTR_DATA_SIZE,
							statement->pError));
			if (error) {
				return 0; /* XXX we loose memory!!! */
			}

			outcol->storage_size4 = storage_size2;

			error = oci8_error(
						statement->pError,
					   	"OCIAttrGet OCI_DTYPE_PARAM/OCI_ATTR_NAME",
					   	OCIAttrGet(
							(dvoid *)param,
							OCI_DTYPE_PARAM,
							(dvoid **)&colname,					/* XXX this string is NOT zero terminated!!!! */
							(ub4 *)&outcol->name_len,
							(ub4)OCI_ATTR_NAME,
							statement->pError));
			if (error) {
				return 0; /* XXX we loose memory!!! */
			}

			outcol->name = estrndup(colname,outcol->name_len);

			/* Remember the size Oracle told us, we have to increase the
			 * storage size for some types.
			 */

			outcol->size4 = outcol->storage_size4;

			/* find a user-setted define */
			if (statement->defines) {
				_php3_hash_find(statement->defines,outcol->name,outcol->name_len,(void **) &outcol->define);
			}

			switch (outcol->type) {
				case SQLT_RSET:  /* ref. cursor  XXX NYI */
					define_type = -1;
					break;

			 	case SQLT_RDD:	 /* ROWID */
				case SQLT_BLOB:  /* binary LOB */
				case SQLT_CLOB:  /* character LOB */
				case SQLT_BFILE: /* binary file LOB */
					define_type = outcol->type;
					outcol->is_descr = 1;
					outcol->storage_size4 = -1;
					break;

				case SQLT_LBI:
				case SQLT_LNG:
				case SQLT_BIN:
					define_type = SQLT_STR;
					/* XXX this should be user-settable!! */
					outcol->storage_size4 =  OCI8_MAX_DATA_SIZE; /* 2MB */
					break;

				default:
					define_type = SQLT_STR;
					if ((outcol->type == SQLT_DAT) || (outcol->type == SQLT_NUM)) {
						outcol->storage_size4 = 256; /* XXX this should fit "most" NLS date-formats and Numbers */
					} else {
						outcol->storage_size4++; /* add one for string terminator */
					}
					break;
			}

			error = oci8_error(
						statement->pError,
					   	"OCIDefineByPos",
					   	OCIDefineByPos(
						  	statement->pStmt,				/* IN/OUT handle to the requested SQL query */
						  	(OCIDefine **)&outcol->pDefine,	/* IN/OUT pointer to a pointer to a define handle */
						  	statement->pError, 				/* IN/OUT An error handle  */
						  	counter,						/* IN     position in the select list */
						  	(dvoid *)0,						/* IN/OUT pointer to a buffer */
						  	outcol->storage_size4, 			/* IN	  The size of each valuep buffer in bytes */
						  	define_type,					/* IN	  The data type */
						  	(dvoid *)&outcol->indicator, 	/* IN	  pointer to an indicator variable or arr */
  						  	(ub2 *)&outcol->size2, 			/* IN/OUT Pointer to array of length of data fetched */
						  	(ub2 *)&outcol->retcode,		/* OUT	  Pointer to array of column-level return codes */
						  	OCI_DYNAMIC_FETCH));			/* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
			if (error) {
				return 0; /* XXX we loose memory!!! */
			}

			error = oci8_error(
						statement->pError,
					   	"OCIDefineDynamic",
					   	OCIDefineDynamic(
				  			outcol->pDefine,
						  	statement->pError,
						  	outcol,
						  	oci8_define_callback));
			if (error) {
				return 0; /* XXX we loose memory!!! */
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
	sword error;
	int i;
	oci8_out_column *column;
	pval *pval;
	OCI8_TLS_VARS;

	error = OCIStmtFetch(statement->pStmt, statement->pError, nrows,
						 OCI_FETCH_NEXT, OCI_DEFAULT);

	if (error == OCI_NO_DATA) {
		return 0;
	}

	if (error == OCI_SUCCESS_WITH_INFO || error == OCI_SUCCESS) {
		/* do the stuff needed for OCIDefineByName */
		for (i = 0; i < statement->ncolumns; i++) {
			column = oci8_get_col(statement, i + 1, 0, "OCIFetch");
			if (column == NULL) { /* should not happen... */
				continue;
			}
				
			if ((column->define) && (! column->is_descr)) {
				pval = column->define->pval;

				if (! column->define->data) { /* oracle has NOT called our define_callback (column value is NULL) */
					pval->value.str.val = emalloc(column->storage_size4);
					column->define->data = pval->value.str.val;
				}

				if (column->indicator == -1) { /* NULL */
					pval->value.str.len = 0;
				} else {
					if (column->retcode == 1406) { /*XXX ORA-01406 truncated value */
						/* this seems to be a BUG in oracle with 1-digit numbers */
						/*
						oci8_debugcol(column,"truncated");
						*/
						pval->value.str.len = column->indicator - 1; /* when value is truncated indicator contains the lenght */
					} else { 
						pval->value.str.len = column->rlen;
					}
				}

				pval->value.str.val[ pval->value.str.len ] = 0;
			}
		}

		return 1;
	}

	oci8_error(statement->pError, func, error);

	return 0;
}

/* }}} */
/* {{{ oci8_loaddesc() */
static ub4
oci8_loaddesc(oci8_connection *connection, oci8_descriptor *mydescr, char **buffer)
{
	sword ociresult;
	ub4 loblen;

	OCI8_TLS_VARS;

	ociresult = OCILobGetLength(connection->pServiceContext, connection->pError, mydescr->ocidescr, &loblen);

	if (ociresult) {
		oci8_error(connection->pError, "OCILobGetLength", ociresult);
		return 0;
	}
		
	*buffer = emalloc(loblen + 1);

	if (! buffer) {
		return 0;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		ociresult = OCILobFileOpen(connection->pServiceContext,
								   connection->pError,
								   mydescr->ocidescr,
								   OCI_FILE_READONLY);
		if (ociresult) {
			oci8_error(connection->pError, "OCILobFileOpen", ociresult);
			efree(buffer);
			return 0;
		}
	}
		
	ociresult = OCILobRead(connection->pServiceContext, 
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

	if (ociresult) {
		oci8_error(connection->pError, "OCILobRead", ociresult);
		efree(buffer);
		return 0;
	}

	if (mydescr->type == OCI_DTYPE_FILE) {
		ociresult = OCILobFileClose(connection->pServiceContext,
								    connection->pError,
								    mydescr->ocidescr);
		if (ociresult) {
			oci8_error(connection->pError, "OCILobFileClose", ociresult);
			efree(buffer);
			return 0;
		}
	}
		
	(*buffer)[ loblen ] = 0;

	oci8_debug("OCIloaddesc: size=%d",loblen);

	return loblen;
}
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
	oci8_define *define;
	pval *pval, *tmp;
	oci8_descriptor *pdescr, descr;

	outcol = (oci8_out_column *)octxp;
	define = outcol->define;


	if (outcol->is_descr) {
	  if (define && (! outcol->pdescr)) { /* column has been user-defined */
		  if (_php3_hash_find(define->pval->value.ht, "descriptor", sizeof("descriptor"), (void **)&tmp) == FAILURE) {
			  php3_error(E_WARNING, "unable to find my descriptor property");
			  return OCI_ERROR;
		  } else if (_php3_hash_index_find(outcol->statement->conn->descriptors, tmp->value.lval, (void **)&pdescr) == FAILURE) {
			  php3_error(E_WARNING, "unable to find my descriptor");
			  return OCI_ERROR;
		  } 
		  outcol->pdescr = pdescr;
	  } else if (! outcol->pdescr) { /* we got no define value and teh descriptor hasn't been allocated yet */
		  if (outcol->type == SQLT_BFILE) {
			  descr.type = OCI_DTYPE_FILE;
		  } else if (outcol->type == SQLT_RDD ) {
			  descr.type = OCI_DTYPE_ROWID;
		  } else {
			  descr.type = OCI_DTYPE_LOB;
		  }

		  OCIDescriptorAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,(dvoid *)&(descr.ocidescr), descr.type, (size_t) 0, (dvoid **) 0);

		  _php3_hash_index_update(outcol->statement->conn->descriptors, 
			  outcol->statement->conn->descriptors_count,&descr,sizeof(oci8_descriptor),(void **)&pdescr);

		  outcol->data = (void *) outcol->statement->conn->descriptors_count++;
		  outcol->pdescr = pdescr;

		  oci8_debug("OCIExecute: new descriptor for %d -> %x",outcol->data,descr.ocidescr);
		} 

		if (! outcol->pdescr) {
			php3_error(E_WARNING, "unable to find my descriptor");
			return OCI_ERROR;
		}

		outcol->rlen = -1;
		*bufpp  = outcol->pdescr->ocidescr;
	} else { /* "normal variable" */
		if (define) {
			pval = define->pval;
			convert_to_string(pval);

			if (pval->value.str.val) {
		 	  	if ((pval->value.str.val==undefined_variable_string) || (pval->value.str.val==empty_string)) {
					/* value was empty -> allocate it... */
					pval->value.str.val = 0;
				} else if (pval->value.str.val != define->data) { 
					/* value has changed - and is maybe too small -> reallocate it! */
					efree(pval->value.str.val);
					pval->value.str.val = 0;
				}
			} 

			if (pval->value.str.val == 0) {
				pval->value.str.val = emalloc(outcol->storage_size4);
				define->data = pval->value.str.val;
			}
			outcol->data = pval->value.str.val;
		} else if (! outcol->data) {
			outcol->data = (text *) emalloc(outcol->storage_size4);
		}

		if (! outcol->data) {
			php3_error(E_WARNING, "OCIFetch: cannot allocate %d bytes!",outcol->storage_size4);
			return OCI_ERROR;
		}

		outcol->rlen = outcol->storage_size4;
		*bufpp  = outcol->data;
	}

	outcol->indicator = 0;
	outcol->retcode = 0;

	*alenp  = &outcol->rlen;
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

	if (phpbind->descr == 0) { 		/* "normal string bind */
		convert_to_string(val); 

		*bufpp = val->value.str.val;
		*alenp = phpbind->maxsize;
		*indpp = (dvoid *)&phpbind->indicator;
	} else { 						/* descriptor bind */
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
/* {{{ oci8_login()
 */

static oci8_session *oci8_login(oci8_server* server,char *username,char *password,int persistent,HashTable *list, HashTable *plist)
{
	sword error;
	oci8_session *session = 0;
	OCISvcCtx *svchp = 0;
	list_entry *le;
	list_entry new_le;
	char *hashed_details = 0;
	int hashed_details_length;
    OCI8_TLS_VARS;

	/* 
	   check if we already have this user authenticated

	   we will reuse authenticated users within a request no matter if the user requested a persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	hashed_details_length = sizeof("oci8_user_")-1 + 
		strlen(SAFE_STRING(username))+
		strlen(SAFE_STRING(password))+
		strlen(SAFE_STRING(server->dbname));
	hashed_details = (char *) emalloc(hashed_details_length+1);
	sprintf(hashed_details,"oci8_user_%s%s%s",
			SAFE_STRING(username),
			SAFE_STRING(password),
			SAFE_STRING(server->dbname));

	if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==SUCCESS) {
		session = (oci8_session *) le->ptr;
	} else if (_php3_hash_find(list, hashed_details, hashed_details_length+1, (void **) &le)==SUCCESS) {
		session = (oci8_session *) le->ptr;
	}

	if (session) {
		oci8_debug("oci8_login persistent sess=%d",session->id);
		return session;
	}

	if (persistent) {
		session = calloc(1,sizeof(oci8_session));
	} else {
		session = ecalloc(1,sizeof(oci8_session));
	}

	if (! session) {
		goto CLEANUP;
	}

	session->persistent = persistent;
	session->server = server;

	/* allocate temporary Service Context */
	error = OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
						   (dvoid **)&svchp,
						   OCI_HTYPE_SVCCTX,
						   0,
						   NULL);
	if (error != OCI_SUCCESS) {
		oci8_error(server->pError, "oci8_login: OCIHandleAlloc OCI_HTYPE_SVCCTX", error);
		goto CLEANUP;
	}

	/* allocate private error-handle */
	error = OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
						   (dvoid **)&session->pError, 
						   OCI_HTYPE_ERROR, 
						   0, 
						   NULL);
	if (error != OCI_SUCCESS) {
		oci8_error(server->pError, "oci8_login: OCIHandleAlloc OCI_HTYPE_ERROR", error);
		goto CLEANUP;
	}

	/* allocate private session-handle */
	error = OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,
						   (dvoid **)&session->pSession,
						   OCI_HTYPE_SESSION,
						   0,
						   NULL);
	if (error != OCI_SUCCESS) {
		oci8_error(server->pError, "oci8_login: OCIHandleAlloc OCI_HTYPE_SESSION", error);
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	error = OCIAttrSet(svchp,
					   OCI_HTYPE_SVCCTX,
					   server->pServer,
					   0, 
					   OCI_ATTR_SERVER, 
					   session->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(session->pError, "oci8_login: OCIAttrSet OCI_ATTR_SERVER", error);
		goto CLEANUP;
	}

	/* set the username in user handle */
	error = OCIAttrSet((dvoid *) session->pSession,
					   (ub4) OCI_HTYPE_SESSION, 
					   (dvoid *) username, 
					   (ub4) strlen(username), 
					   (ub4) OCI_ATTR_USERNAME, 
					   session->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(session->pError, "OCIAttrSet OCI_ATTR_USERNAME", error);
		goto CLEANUP;
	}

	/* set the password in user handle */
	error = OCIAttrSet((dvoid *) session->pSession,
					   (ub4) OCI_HTYPE_SESSION, 
					   (dvoid *) password, 
					   (ub4) strlen(password), 
					   (ub4) OCI_ATTR_PASSWORD, 
					   session->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(session->pError, "OCIAttrSet OCI_ATTR_PASSWORD", error);
		goto CLEANUP;
	}

	error = OCISessionBegin(svchp,
							session->pError,
							session->pSession, 
							(ub4) OCI_CRED_RDBMS, 
							(ub4) OCI_DEFAULT);
	if (error != OCI_SUCCESS) {
		oci8_error(session->pError, "OCISessionBegin", error);
		goto CLEANUP;
	}

	/* Free Temporary Service Context */
	OCIHandleFree((dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX);

	new_le.ptr = session;

	if (persistent) {
		session->id = php3_list_insert(session, OCI8_GLOBAL(php3_oci8_module).le_psession);
		_php3_hash_update(plist,
						  hashed_details,
						  hashed_details_length+1, 
						  (void *) &new_le,
						  sizeof(list_entry),
						  NULL);
	} else {
		session->id = php3_list_insert(session, OCI8_GLOBAL(php3_oci8_module).le_session);
		_php3_hash_update(list,
						  hashed_details,
						  hashed_details_length+1, 
						  (void *) &new_le,
						  sizeof(list_entry),
						  NULL);
	}

	oci8_debug("oci8_login new sess=%d",session->id);

	if (hashed_details) {
		efree(hashed_details);
	}

	return session;

 CLEANUP:
	if (hashed_details) {
		efree(hashed_details);
	}

	if (session) {
		_oci8_logoff(session);
	}

	efree(session);

	return 0;
}

/* }}} */
/* {{{ _oci8_logoff()
 */

static void
_oci8_logoff(oci8_session *session)
{
	if (! session) {
		return;
	}

	oci8_debug("_oci8_logoff: sess=%d",session->id);
}

/* }}} */
/* {{{ _oci8_free_descr()
 */

static void
_oci8_free_descr(oci8_descriptor *descr)
{
    OCI8_TLS_VARS;

    oci8_debug("oci8_free_descr: %x",descr->ocidescr);

    OCIDescriptorFree(descr->ocidescr, descr->type);
}
/* }}} */
/* {{{ oci8_attach()
 */
static oci8_server *oci8_attach(char *dbname,int persistent,HashTable *list, HashTable *plist)
{ 
	oci8_server *server = 0;
	list_entry *le;
	list_entry new_le;
	sword error;
	char *hashed_details = 0;
	int hashed_details_length;
    OCI8_TLS_VARS;

	/* 
	   check if we already have this server open 

	   we will reuse servers within a request no matter if the usere requested persistent 
	   connections or not!
	   
	   but only as pesistent requested connections will be kept between requests!
	*/

	hashed_details_length = sizeof("oci8_server_")-1 + strlen(SAFE_STRING((char *)dbname));
	hashed_details = (char *) emalloc(hashed_details_length+1);
	sprintf(hashed_details,"oci8_server_%s",SAFE_STRING((char *)dbname));

	if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==SUCCESS) {
		server = (oci8_server *) le->ptr;
	} else if (_php3_hash_find(list, hashed_details, hashed_details_length+1, (void **) &le)==SUCCESS) {
		server = (oci8_server *) le->ptr;
	}

	if (server) {
		oci8_debug("oci8_attach persistent conn=%d (%s)",server->id,server->dbname);
		return server;
	}


	if (persistent) {
		server = calloc(1,sizeof(oci8_server));
	} else {
		server = ecalloc(1,sizeof(oci8_server));
	}

	if (! server) {
		goto CLEANUP;
	}

	server->persistent = persistent;

	strcpy(server->dbname,dbname);
	
	OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
				   (dvoid **)&server->pError,
				   OCI_HTYPE_ERROR,
				   0,
				   NULL); 

	OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
				   (dvoid **)&server->pServer, 
				   OCI_HTYPE_SERVER, 
				   0, 
				   NULL); 

	error = OCIServerAttach(server->pServer,
							server->pError,
							dbname,
							strlen(dbname),
							(ub4) OCI_DEFAULT);

	if (error) {
		oci8_error(server->pError, "oci8_attach", error);
		goto CLEANUP;
	}
	
	new_le.ptr = server;

	if (persistent) {
		server->id = php3_list_insert(server,OCI8_GLOBAL(php3_oci8_module).le_pserver);
		_php3_hash_update(plist,
						  hashed_details,
						  hashed_details_length+1, 
						  (void *) &new_le,
						  sizeof(list_entry),
						  NULL);
	} else {
		server->id = php3_list_insert(server,OCI8_GLOBAL(php3_oci8_module).le_server);
		_php3_hash_update(list,
						  hashed_details,
						  hashed_details_length+1, 
						  (void *) &new_le,
						  sizeof(list_entry),
						  NULL);
	}

	oci8_debug("oci8_attach new conn=%d (%s)",server->id,server->dbname);

	if (hashed_details) {
		efree(hashed_details);
	}

	return server;

 CLEANUP:
	if (hashed_details) {
		efree(hashed_details);
	}

	if (server) {
		_oci8_detach(server);
	}
		
	return 0;
}

/* }}} */
/* {{{ _oci8_detach()
 */

static void
_oci8_detach(oci8_server *server)
{
	if (! server) {
		return;
	}

	oci8_debug("_oci8_detach: conn=%d",server->id);
}

/* }}} */
/* {{{ oci8_do_connect()
  Connect to an Oracle database and log on. returns a new session.
 */
static void oci8_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
    text *username, *password, *dbname;
    pval *userParam, *passParam, *dbParam;
    oci8_server *server = 0;
    oci8_session *session = 0;
    oci8_connection *connection = 0;
    sword error;
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
		dbname = (text *)"";
    } else {
		WRONG_PARAM_COUNT;
    }

	connection = (oci8_connection *) ecalloc(1,sizeof(oci8_connection));

	if (! connection) {
		goto CLEANUP;
	}

	server = oci8_attach(dbname,persistent,list,plist);

	if (! server) {
		goto CLEANUP;
	}

	persistent = server->persistent; /* if our server-context is not persistent we can't */

	session = oci8_login(server,username,password,persistent,list,plist);

	if (! session) {
		goto CLEANUP;
	}

	persistent = session->persistent; /* if our session-context is not persistent we can't */

	/* set our session */
	connection->session = session;

	/* allocate our private error-handle */
	error = OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
						   (dvoid **)&connection->pError, 
						   OCI_HTYPE_ERROR, 
						   0, 
						   NULL);
	if (error != OCI_SUCCESS) {
		oci8_error(server->pError, "oci8_do_connect: OCIHandleAlloc OCI_HTYPE_ERROR", error);
		goto CLEANUP;
	}

	/* allocate our service-context */
  	error = OCIHandleAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv, 
						   (dvoid **)&connection->pServiceContext, 
						   OCI_HTYPE_SVCCTX, 
						   0, 
						   NULL);
	if (error != OCI_SUCCESS) {
		oci8_error(connection->pError, "oci8_do_connect: OCIHandleAlloc OCI_HTYPE_SVCCTX", error);
		goto CLEANUP;
	}

	/* Set the server handle in service handle */ 
	error = OCIAttrSet(connection->pServiceContext, 
					   OCI_HTYPE_SVCCTX,
					   server->pServer, 
					   0, 
					   OCI_ATTR_SERVER,
					   connection->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(connection->pError, "oci8_do_connect: OCIAttrSet OCI_ATTR_SERVER", error);
		goto CLEANUP;
	}

  /* Set the Authentication handle in the service handle */
	error = OCIAttrSet(connection->pServiceContext, 
					   OCI_HTYPE_SVCCTX,
					   session->pSession,
					   0, 
					   OCI_ATTR_SESSION,
					   connection->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(connection->pError, "oci8_do_connect: OCIAttrSet OCI_ATTR_SESSION", error);
		goto CLEANUP;
	}

	connection->id = php3_list_insert(connection, OCI8_GLOBAL(php3_oci8_module).le_conn);
	connection->descriptors = emalloc(sizeof(HashTable));
	if (!connection->descriptors ||
		_php3_hash_init(connection->descriptors, 13, NULL,(void (*)(void *))_oci8_free_descr, 0) == FAILURE) {
        goto CLEANUP;
    }

	oci8_debug("oci8_do_connect: id=%d",connection->id);

	RETURN_LONG(connection->id);
	
 CLEANUP:
	RETURN_FALSE;
}

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto int    OCIDefineByName(int stmt, string name, mixed &var [,int type])
  Define a PHP variable to an Oracle column by name.
  if you want to define a LOB/CLOB etc make sure you allocate it via OCINewDescriptor BEFORE defining!!!
 */

void php3_oci8_definebyname(INTERNAL_FUNCTION_PARAMETERS)
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
			_php3_hash_init(statement->defines, 13, NULL, (void (*)(void *))oci8_free_define, 0) == FAILURE) {
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

	define->name = estrndup(name->value.str.val,name->value.str.len);
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

void php3_oci8_bindbyname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *name, *var, *maxlen, *tmp,*type;
	oci8_statement *statement;
	oci8_bind *bind, *tmp_bind;
	oci8_descriptor *descr;
	sword error;
	ub2	ocitype;
	sb4 ocimaxlen;
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
			convert_to_string(var);
			if (ocimaxlen == -1) {
				if (var->value.str.len == 0) {
					php3_error(E_WARNING, "OCIBindByName bindlength is 0");
				}
				ocimaxlen = var->value.str.len + 1; /* SQLT_STR needs a trailing 0 - maybe we need to resize the var buffers????? */
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
	bind->maxsize = ocimaxlen;

	error = OCIBindByName(statement->pStmt, /* statement handle */
						  (OCIBind **)&bind->pBind, /* bind hdl (will alloc) */
						  statement->pError, /* error handle */
						  name->value.str.val, /* placeholder name */
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
	if (error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIBindByName", error);
		RETURN_FALSE;
	}
	error = OCIBindDynamic(bind->pBind,
						   statement->pError,
						   (dvoid *)bind,
						   oci8_bind_in_callback,
						   (dvoid *)bind,
						   oci8_bind_out_callback);
	if (error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIBindDynamic", error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string ocifreedesc(object lob)
 */

void php3_oci8_freedesc(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *id, *conn, *desc;
	oci8_connection *connection;

	OCI8_TLS_VARS;

	if (getThis(&id) == SUCCESS) {
        if (_php3_hash_find(id->value.ht, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
            php3_error(E_WARNING, "unable to find my statement property");
            RETURN_FALSE;
        }

        connection = oci8_get_conn(conn->value.lval, "OCIfreedesc", list, plist);
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

void php3_oci8_savedesc(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *id, *tmp, *conn, *arg;
	OCILobLocator *mylob;
	oci8_connection *connection;
	oci8_descriptor *descr;
	sword ociresult;
	ub4 loblen;

	OCI8_TLS_VARS;

	if (getThis(&id) == SUCCESS) {
   		if (_php3_hash_find(id->value.ht, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php3_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		connection = oci8_get_conn(conn->value.lval, "OCIsavedesc", list, plist); 
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

    	ociresult = OCILobWrite(connection->pServiceContext,
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

		if (ociresult) {
			oci8_error(connection->pError, "OCILobWrite", ociresult);
			RETURN_FALSE;
		}

	 	RETURN_TRUE;
	}

  RETURN_FALSE;
}

/* }}} */
/* {{{ proto string ociloaddesc(object lob)
 */

void php3_oci8_loaddesc(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *id, *tmp, *conn;
	oci8_connection *connection;
	oci8_descriptor *descr;
	char *buffer;
	ub4 loblen;

	OCI8_TLS_VARS;

	if (getThis(&id) == SUCCESS) {
   		if (_php3_hash_find(id->value.ht, "connection", sizeof("connection"), (void **)&conn) == FAILURE) {
			php3_error(E_WARNING, "unable to find my statement property");
			RETURN_FALSE;
		}

		connection = oci8_get_conn(conn->value.lval, "OCIsavedesc", list, plist); 
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

void php3_oci8_newdescriptor(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *conn, *type;
	sword ociresult;
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

	connection = oci8_get_conn(conn->value.lval, "OCINewDescriptor", list, plist);
    if (connection == NULL) {
        RETURN_FALSE;
    }

	ociresult = OCIDescriptorAlloc(OCI8_GLOBAL(php3_oci8_module).pEnv,(dvoid*)&(descr.ocidescr), descr.type, (size_t) 0, (dvoid **) 0);

	if (ociresult) {
		oci8_error(connection->pError,"OCIDescriptorAlloc %d",ociresult);
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

void php3_oci8_rollback(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *conn;
	oci8_connection *connection;
	sword ociresult;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(conn);

	connection = oci8_get_conn(conn->value.lval, "OCIRollback", list, plist);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	ociresult = OCITransRollback(connection->pServiceContext,connection->pError, (ub4)0);

	if (ociresult) {
		oci8_error(connection->pError, "OCIRollback", ociresult);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string OCICommit(int conn)
  commit the current context
 */

void php3_oci8_commit(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *conn;
	oci8_connection *connection;
	sword ociresult;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(conn);

	connection = oci8_get_conn(conn->value.lval, "OCICommit", list, plist);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	ociresult = OCITransCommit(connection->pServiceContext,connection->pError, (ub4)0);

	if (ociresult) {
		oci8_error(connection->pError, "OCICommit", ociresult);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

/* }}} */
/* {{{ proto string OCIColumnName(int stmt, int col)
  Tell the name of a column.
 */

void php3_oci8_columnname(INTERNAL_FUNCTION_PARAMETERS)
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

void php3_oci8_columnsize(INTERNAL_FUNCTION_PARAMETERS)
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
	RETURN_LONG(outcol->size4);
}

/* }}} */
/* {{{ proto mixed  OCIColumnType(int stmt, int col)
  Tell the data type of a column.
 */

void php3_oci8_columntype(INTERNAL_FUNCTION_PARAMETERS)
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
	switch (outcol->type) {
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
			RETVAL_LONG(outcol->type);
	}
}

/* }}} */
/* {{{ proto int    OCIColumnIsNULL(int stmt, int col)
  Tell whether a column is NULL.
 */

void php3_oci8_columnisnull(INTERNAL_FUNCTION_PARAMETERS)
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
void php3_oci8_internaldebug(INTERNAL_FUNCTION_PARAMETERS)
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

void php3_oci8_execute(INTERNAL_FUNCTION_PARAMETERS)
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

	if (oci8_execute(statement, "OCIExecute",execmode)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCIFetch(int stmt)
  Prepare a new row of data for reading.
 */

void php3_oci8_fetch(INTERNAL_FUNCTION_PARAMETERS)
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

void php3_oci8_fetchinto(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *array, element, *fmode;
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

	for (i = 0; i < statement->ncolumns; i++) {
		column = oci8_get_col(statement, i + 1, 0, "OCIFetchInto");
		if (column == NULL) { /* should not happen... */
			continue;
		}
	
		if ((column->indicator == -1) && ((mode & OCI_RETURN_NULLS) == 0)) {
			continue;
		}

		if ((mode & OCI_NUM) || (! (mode & OCI_ASSOC))) { /* OCI_NUM is default */
			oci8_make_pval(&element,stmt->value.lval,column, "OCIFetchInto",list,mode);
		  	_php3_hash_index_update(array->value.ht, i, (void *)&element, sizeof(pval), NULL);
		}

		if (mode & OCI_ASSOC) {
			oci8_make_pval(&element,stmt->value.lval,column, "OCIFetchInto",list,mode);
		  	_php3_hash_update(array->value.ht, column->name, column->name_len+1, (void *)&element, sizeof(pval), NULL);
		}
	}

	RETURN_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ proto int    OCIFetchStatement(int stmt, array &output)
  Fetch all rows of result data into an array.
 */

void php3_oci8_fetchstatement(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *array, element, *fmode;
	oci8_statement *statement;
	oci8_out_column **columns;
	pval **outarrs;
	pval tmp;
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
	outarrs = emalloc(statement->ncolumns * sizeof(pval));

	for (i = 0; i < statement->ncolumns; i++) {
		columns[ i ] = oci8_get_col(statement, i + 1, 0, "OCIFetchStatement");

		array_init(&tmp);

		memcpy(namebuf,columns[ i ]->name, columns[ i ]->name_len);
		namebuf[ columns[ i ]->name_len ] = 0;
				
		_php3_hash_update(array->value.ht, namebuf, columns[ i ]->name_len+1, (void *) &tmp, sizeof(pval), (void **) &(outarrs[ i ]));
	}

	while (oci8_fetch(statement, nrows, "OCIFetchStatement")) {
		for (i = 0; i < statement->ncolumns; i++) {
			oci8_make_pval(&element,stmt->value.lval,columns[ i ], "OCIFetchStatement",list,OCI_RETURN_LOBS);
			_php3_hash_index_update(outarrs[ i ]->value.ht, rows, (void *)&element, sizeof(pval), NULL);
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

void php3_oci8_freestatement(INTERNAL_FUNCTION_PARAMETERS)
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
void php3_oci8_logoff(INTERNAL_FUNCTION_PARAMETERS)
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
void php3_oci8_logon(INTERNAL_FUNCTION_PARAMETERS)
{
	oci8_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

/* }}} */
/* {{{ proto int    OCIPLogon(string user, string pass[, string db])
  Connect to an Oracle database and log on. returns a new session.
 */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
void php3_oci8_plogon(INTERNAL_FUNCTION_PARAMETERS)
{
	oci8_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

/* }}} */
/* {{{ proto int    OCIError(int stmt|conn)
  Return the last error of stmt|conn. If no error happened returns false.
 */

void php3_oci8_error(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mixed;
	oci8_statement *statement;
	oci8_connection *connection;
    text errbuf[512];
    ub4 errcode = 0;
	int type;
	dvoid *errh = NULL;

	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &mixed) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(mixed);

    statement = (oci8_statement *)php3_list_find(mixed->value.lval, &type);
    if (statement && OCI8_STMT_TYPE(type)) {
		errh = statement->pError;
	} else {
		connection = (oci8_connection *)php3_list_find(mixed->value.lval, &type);
		if (connection && OCI8_CONN_TYPE(type)) {
			errh = connection->pError;
		}
	}

	if (! errh) {
		php3_error(E_WARNING, "OCIError: unable to find Error handle");
		RETURN_FALSE;
	}

	OCIErrorGet(errh,1,NULL,&errcode,errbuf,(ub4)sizeof(errbuf),(ub4)OCI_HTYPE_ERROR);

	if (errcode) {
		array_init(return_value);
		add_assoc_long(return_value, "code", errcode);
		add_assoc_string(return_value, "message", errbuf, 1);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int    OCINumCols(int stmt)
  Return the number of result columns in a statement.
 */

void php3_oci8_numcols(INTERNAL_FUNCTION_PARAMETERS)
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

void php3_oci8_parse(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *conn, *query;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &conn, &query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(conn);
	convert_to_string(query);

	connection = oci8_get_conn(conn->value.lval, "OCIParse", list, plist);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(oci8_parse(connection,
						   query->value.str.val,
						   query->value.str.len,
						   list));
}

/* }}} */
/* {{{ proto string OCIResult(int stmt, mixed column)
  Return a single column of result data.
 */

void php3_oci8_result(INTERNAL_FUNCTION_PARAMETERS)
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

	oci8_make_pval(return_value,stmt->value.lval,outcol, "OCIResult",list,0);
}

/* }}} */
/* {{{ proto string OCIServerVersion(int conn)
  Return a string containing server version information.
 */

void php3_oci8_serverversion(INTERNAL_FUNCTION_PARAMETERS)
{
	oci8_connection *connection;
	pval *arg;
	int index, index_t;
	sword error;
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
	error = OCIServerVersion(connection->pServiceContext,
							 connection->pError, version, sizeof(version),
							 OCI_HTYPE_SVCCTX);
	if (error != OCI_SUCCESS) {
		oci8_error(connection->pError, "OCIServerVersion", error);
		RETURN_FALSE;
	}
	RETURN_STRING(version,1);
}

/* }}} */
/* {{{ proto int    OCIStatementType(int stmt)
  Return the query type of an OCI statement.
 */

/* XXX it would be better with a general interface to OCIAttrGet() */

void php3_oci8_statementtype(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	ub2 stmttype;
	sword error;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIStatementType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	error = OCIAttrGet((dvoid *)statement->pStmt, OCI_HTYPE_STMT,
					   (ub2 *)&stmttype, (ub4 *)0, OCI_ATTR_STMT_TYPE,
					   statement->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIStatementType", error);
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

void php3_oci8_rowcount(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	ub4 rowcount;
	sword error;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIStatementType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	error = OCIAttrGet((dvoid *)statement->pStmt, OCI_HTYPE_STMT,
					   (ub2 *)&rowcount, (ub4 *)0, OCI_ATTR_ROW_COUNT,
					   statement->pError);

	if (error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIRowCount", error);
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
