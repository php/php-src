/*
   +----------------------------------------------------------------------+
   | msession 1.0                                                           |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Mark Woodward <markw@mohawksoft.com>                        |
   |    Portions copyright the PHP group.                                 |
   +----------------------------------------------------------------------+
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_msession.h"
#include "msessionapi.h"
#include "ext/standard/info.h"
#include "ext/session/php_session.h"

/* Macros and such */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef SID_LEN
#define SID_LEN 32
#endif

/* Uncomment to get debugging messages */
// #define ERR_DEBUG  

#ifdef ERR_DEBUG
#define ELOG( str )	php_log_err( (char *)(str) )
#else
#define ELOG( str )
#endif

/* Test if system is OK fror use. */
/* Macros may be ugly, but I can globaly add debuging when needed. */
#define	IFCONNECT_BEGIN		{ void *t_conn; REQB t_reqb; if(MSessionGetBuffers(&t_conn, &t_reqb TSRMLS_CC)) {

/* End IFCONNECT block with return FALSE */
#define IFCONNECT_END		 MSessionReleaseBuffers(t_conn, &t_reqb TSRMLS_CC);} else { php_error(E_WARNING, s_szNoInit, get_active_function_name(TSRMLS_C)); RETURN_FALSE; }}

/* End IFCONNECT block without return */
#define IFCONNECT_ENDNR		 MSessionReleaseBuffers(t_conn, &t_reqb TSRMLS_CC);} else { php_error(E_WARNING, s_szNoInit, get_active_function_name(TSRMLS_C)); }}

#ifndef HAVE_PHP_SESSION
#error HAVE_PHP_SESSION not defined
#endif

static int msession_split_host_port(char *host, int cbhost, char *save_path, int def_port);
static int MSessionGetBuffers(void **pconn, REQB *preq TSRMLS_DC);
static void MSessionReleaseBuffers(void *pconn, REQB *preq TSRMLS_DC);
static void msession_logf(char *fmt, ...);
static void php_msession_init_globals(zend_msession_globals *msession_globals);
static int PHPDoRequest(void **pconn, REQB *preq TSRMLS_DC);
static void *PHPMSessionConnect(const char *szhost, int nport TSRMLS_DC);
static void PHPMSessionDisconnect(TSRMLS_DC);
static char * PHPMSessionGetString(int req, const char *session, char *name, char *value, int param TSRMLS_DC);
static int PHPMSessionGetInt(int req, char *session, char *name, char *value, int param TSRMLS_DC);
static int PHPMSessionGetArray(zval *return_value, int req, char *session, char *name, char *value, int param TSRMLS_DC);
static int PHPMSessionGetArrayPairs(zval *return_value, int req, char *session, char *name, char *value, int param TSRMLS_DC);
static int PHPMSessionExec(int req, char *session, char *name, char *value, int param TSRMLS_DC);

/* Test if session module contains custom sesson ID patch */
#ifdef PHP_SESSION_API
#if (PHP_SESSION_API >= 20020330)
#define HAVE_PHP_SESSION_CREATESID
#endif
#endif
	
/* This sets the PHP API version used in the file. */
/* If this module does not compile on the version of PHP you are using, look for */
/* this value in Zend/zend_modules.h, and set appropriately */
#if (ZEND_MODULE_API_NO <=  20001222)
#define PHP_4_0
#define TSRMLS_CC
#define TSRMLS_DC
#define TSRMLS_FETCH()
/* Comment out this line if you wish to have msession without php sessions */
#define HAVE_PHP_SESSION
#warning Backward compatible msession extension requires PHP sessions. If PHP compiles and links, you can ignore this warning.
#elif (ZEND_MODULE_API_NO >= 20010901)
#define PHP_4_1
#endif

/*
 * Please do not remove backward compatibility from this module.
 * this same source must also work with the 4.0.6 version of PHP.
 *
 * Module Variable naming hints:
 * All global scope variables begin with "g_" for global.
 * All static file scoped variables begin with "s_" for static.
 * Zero terminated strings generally start with "sz" for "string followed by zero."
 * integers that hold string or data lengths generally start with "cb" for "count of bytes."
 * Also, please to not reformat braces ;-)
 * -MLW
 */
#if HAVE_MSESSION
#ifdef HAVE_PHP_SESSION
#ifdef HAVE_PHP_SESSION_CREATESID
/* If the PHP Session module is compiled or available, include support */
PS_FUNCS_SID(msession);
ps_module ps_mod_msession = {
	PS_MOD_SID(msession)
};
#else
PS_FUNCS(msession);
ps_module ps_mod_msession = {
	PS_MOD(msession)
};
#endif
#endif

ZEND_DECLARE_MODULE_GLOBALS(msession)

/* Static strings */
static char s_szNoInit[]="%s(): MSession not initialized";
static char s_szErrFmt[]="%s(): %s";

/* Per-process variables need by msession */
static char	s_szdefhost[]="localhost";

#define MAX_HOST	1024
#define DEF_PORT	8086

function_entry msession_functions[] = {
	PHP_FE(msession_connect,NULL)
	PHP_FE(msession_disconnect,NULL)
	PHP_FE(msession_lock,NULL)
	PHP_FE(msession_unlock,NULL)
	PHP_FE(msession_count,NULL)
	PHP_FE(msession_create,NULL)
	PHP_FE(msession_destroy,NULL)
	PHP_FE(msession_set,NULL)
	PHP_FE(msession_get,NULL)
	PHP_FE(msession_find,NULL)
	PHP_FE(msession_get_array,NULL)
	PHP_FE(msession_save_object,NULL)
	PHP_FE(msession_get_object,NULL)
	PHP_FE(msession_set_array,NULL)
	PHP_FE(msession_timeout,NULL)
	PHP_FE(msession_inc,NULL)
	PHP_FE(msession_add,NULL)
	PHP_FE(msession_muldiv,NULL)
	PHP_FE(msession_setdata,NULL)
	PHP_FE(msession_getdata,NULL)
	PHP_FE(msession_listvar,NULL)
	PHP_FE(msession_list,NULL)
	PHP_FE(msession_uniq,NULL)
	PHP_FE(msession_randstr,NULL)
	PHP_FE(msession_plugin,NULL)
	PHP_FE(msession_call,NULL)
	PHP_FE(msession_ctl,NULL)
	{NULL, NULL, NULL}
};

zend_module_entry msession_module_entry = {
#ifdef PHP_4_1
	STANDARD_MODULE_HEADER,
#endif
	"msession",
	msession_functions,
	PHP_MINIT(msession),
	PHP_MSHUTDOWN(msession),
	NULL, /* PHP_RINIT(msession),*/
	NULL, /* PHP_RSHUTDOWN(msession), */
	PHP_MINFO(msession),
#ifdef PHP_4_1
	NO_VERSION_YET,
#endif
	STANDARD_MODULE_PROPERTIES
};
#ifdef COMPILE_DL_MSESSION
ZEND_GET_MODULE(msession)
#endif

// Split host port syntax: host:port
// Warning, make sure buffer is big enough!
static int msession_split_host_port(char *host, int cbhost, char *save_path, int def_port)
{
	int port;
	char * szport;
	strncpy(host, save_path, cbhost);
	szport = strchr(host, ':');
	if(szport)
	{
		*szport++=0;
		port = atoi(szport);
	}
	else
	{
		port = def_port;
	}
	return port;
}
// Called by internal functions to get buffers and connection
static int MSessionGetBuffers(void **pconn, REQB *preqb TSRMLS_DC)
{
	*pconn = (void *)MSESSION_G(conn);
	AllocateRequestBuffer(preqb,2048);
	if(!*pconn)
		*pconn = PHPMSessionConnect(MSESSION_G(host), MSESSION_G(port) TSRMLS_CC);
	return (int) *pconn;
}

// Called after GetBuffers, updates preqb if nessisary.
// The syntax of this function, along with allows for 
// flexibility of the nature of the interface changes.
static void MSessionReleaseBuffers(void *pconn, REQB *preqb TSRMLS_DC)
{
	MSESSION_G(conn) = pconn;
	FreeRequestBuffer(preqb);
}
/* Uses printf syntax to write to php_log_err
 * Must remain in ERR_DEBUG ifdef to ensure it
 * does not get compiled into release code
 */
static void msession_logf(char *fmt, ...)
{
	va_list arglist;
	char buffer[2048];
	va_start(arglist, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arglist);
	va_end(arglist);
	php_log_err(buffer);
}

// Initialize global structure
static void php_msession_init_globals(zend_msession_globals *msession_globals)
{
	char host[MAX_HOST];
	int global_variable;
	char *init = INI_STR("session.save_path");
	msession_globals->conn = NULL;
	msession_globals->reqb = NULL;

	if(!init)
	{
		ELOG("Could not get session.save_path from INI_STR");
		msession_globals->host = pestrdup(s_szdefhost,1);
		msession_globals->port = DEF_PORT;
	}
	else
	{
		msession_globals->port = msession_split_host_port(host, sizeof(host), init, DEF_PORT);
		msession_globals->host = pestrdup(host,1);
	}
}

PHP_MINIT_FUNCTION(msession)
{
	ZEND_INIT_MODULE_GLOBALS(msession, php_msession_init_globals, NULL);
#ifdef HAVE_PHP_SESSION
	php_session_register_module(&ps_mod_msession);
#endif
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(msession)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(msession)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(msession)
{
	TSRMLS_FETCH();
	void *t_conn = (void *)MSESSION_G(conn);
	if(t_conn)
	{
		MSESSION_G(conn) = NULL;
		CloseReqConn(t_conn);
		t_conn = NULL;
	}
	return SUCCESS;
}

PHP_MINFO_FUNCTION(msession)
{
	ELOG("PHP_MINFO");
	php_info_print_table_start();
	php_info_print_table_header(2, "msession support", "enabled");
	php_info_print_table_end();
}

/* The PHP Version of DoRequest */
/* Manages connections that have been disconnected */
static int PHPDoRequest(void **pconn, REQB *preq TSRMLS_DC)
{
	int stat = preq->req->stat;
	int param = preq->req->param;
	int result = DoRequest(*pconn, preq);
	ELOG("PHPDoRequest");
	/* Server closed the connection! */
	if(preq->req->stat == MREQ_ERR && (result == REQE_BADCONN || result == REQE_NOSEND))
	{
		/* In the case of a bad socket, this should be  */
		/* all that changs!				*/
		preq->req->stat = stat;
		preq->req->param = param;
		ELOG("Connection was closed by server");
		if(ReopenReqConn(*pconn))
			result = DoRequest(*pconn, preq);
	}
#ifdef ERR_DEBUG
	if(preq->req->stat == MREQ_ERR && (result == REQE_BADCONN || result == REQE_NOSEND))
		ELOG("Socket reports closed");
#endif
	return result;
}
static void *PHPMSessionConnect(const char *szhost, int nport TSRMLS_DC)
{
	int fNewHost=FALSE;
	int fNewPort=FALSE;
	int fResult=FALSE;
	void *t_conn; 
	REQB *preqb;
	TSRMLS_FETCH();

	ELOG("PHPMSessionConnect");

	t_conn = (void *)MSESSION_G(conn);

	char *gszhost = MSESSION_G(host);
	int gport = MSESSION_G(port);
	
	if(nport == 0)
		nport = DEF_PORT;

	if(strcmp(gszhost, szhost))
	{
		if(gszhost != s_szdefhost)
			free(gszhost);
		gszhost = strdup(szhost);
		MSESSION_G(host) = gszhost;
		fNewHost = TRUE;
	}
	if(nport != gport)
	{
		fNewPort = TRUE;
		gport = nport;
		MSESSION_G(port) = gport;
	}
	if(!fNewHost && !fNewPort && t_conn)
		return t_conn;	// Nothing to do, already there

	if(t_conn)
	{
		MSESSION_G(conn) = NULL;
		CloseReqConn(t_conn);
		php_log_err("Call to PHPMSessionConnect with non-null t_conn" TSRMLS_CC);
		t_conn = NULL;
	}

	t_conn = OpenReqConn(gszhost, gport);

	MSESSION_G(conn) = t_conn;
	return t_conn;
}

static void PHPMSessionDisconnect(TSRMLS_DC)
{
	TSRMLS_FETCH();
	void *t_conn = (void *)MSESSION_G(conn);
	if(t_conn)
	{
		CloseReqConn(t_conn);
		MSESSION_G(conn) = NULL;
		t_conn = NULL;
	}
}

static char * PHPMSessionGetString(int req, const char *session, char *name, char *value, int param TSRMLS_DC)
{
	char *szvalue = NULL;
	TSRMLS_FETCH();
	ELOG("PHPMSessionGetString");

	IFCONNECT_BEGIN
	FormatRequest(&t_reqb, req, (char *)session, name, value, param);
	PHPDoRequest(&t_conn, &t_reqb);

	if(t_reqb.req->stat == MREQ_OK && t_reqb.req->datum[0])
		szvalue = safe_estrdup(t_reqb.req->datum);
	else
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));
	IFCONNECT_ENDNR

#ifdef ERR_DEBUG
	if(szvalue)
		ELOG(szvalue);
	else
		ELOG("GetString returned NULL");
#endif
	return szvalue;
}

static int PHPMSessionGetInt(int req, char *session, char *name, char *value, int param TSRMLS_DC)
{
	TSRMLS_FETCH();
	ELOG("PHPMSessionGetInt");
	IFCONNECT_BEGIN
	FormatRequest(&t_reqb, req, session, name, value, param);
	PHPDoRequest(&t_conn, &t_reqb);

	param = t_reqb.req->param;

	if(t_reqb.req->stat != MREQ_OK)
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	IFCONNECT_ENDNR
	return param;
}
static int StringsToFixedArray(char *strings, char **array, int ceArray)
{
        int i;
        char *p = strings;
        int term=ceArray-1;
        for(i=0; i < term; i++)
        {
                if(!*p)
                        break;
                array[i] = p;
                p += strlen(p)+1;
        }
        array[i]=0;
        return i;
}
static int PHPMSessionGetArray(zval *return_value, int req, char *session, char *name, char *value, int param TSRMLS_DC)
{
	int stat=0;
	TSRMLS_FETCH();
	ELOG("PHPMSessionGetArray");
	IFCONNECT_BEGIN
	FormatRequest(&t_reqb, req, session, name, value, param);
	PHPDoRequest(&t_conn,&t_reqb);

	if(t_reqb.cmd==MREQ_OK && t_reqb.param)
	{
		int  i=0;
		int term = 0;
		int string_count = t_reqb.param + 4;
		char ** strings = (char **)emalloc(sizeof(char *) * (t_reqb.param + 4));
		term = StringsToFixedArray(t_reqb.req->datum, strings, string_count);

		array_init(return_value);
		for(i=0; i < term; i++)
		{
			ELOG(strings[i]);
			add_index_string(return_value, i, safe_estrdup(strings[i]), 0);
		}
		efree(strings);
	}
	else if(t_reqb.req->stat != MREQ_OK)
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	stat = t_reqb.req->stat;

	IFCONNECT_ENDNR
	return stat;
}

static int PHPMSessionGetArrayPairs(zval *return_value, int req, char *session, char *name, char *value, int param TSRMLS_DC)
{
	int stat=0;
	TSRMLS_FETCH();
	ELOG("PHPMSessionGetArrayPairs");
	IFCONNECT_BEGIN
	FormatRequest(&t_reqb, req, session, name, value, param);
	PHPDoRequest(&t_conn,&t_reqb);

	if(t_reqb.cmd==MREQ_OK && t_reqb.param)
	{
		int  ndx=0;
		int term = 0;
		int string_count = t_reqb.param + 4;
		char ** strings = (char **)emalloc(sizeof(char *) * (t_reqb.param + 4));
		term = StringsToFixedArray(t_reqb.req->datum, strings, string_count);
		array_init(return_value);
		if(req == MREQ_GETOBJECT)
		{
			ELOG("Using MREQ_GETOBJECT classname and classdata");
			add_assoc_string(return_value, safe_estrdup("classname"), safe_estrdup(strings[ndx]), 0);
			ELOG(strings[ndx]);
			ndx++;
			add_assoc_string(return_value, safe_estrdup("classdata"), safe_estrdup(strings[ndx]), 0);
			ELOG(strings[ndx]);
			ndx++;
		}
		for(; ndx < term; ndx+=2)
		{
			ELOG(strings[ndx]);
			ELOG(strings[ndx+1]);
			add_assoc_string(return_value, safe_estrdup(strings[ndx]), 
							safe_estrdup(strings[ndx+1]), 0);
		}

		efree(strings);
	}
	else
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	stat = t_reqb.req->stat;
	IFCONNECT_ENDNR

	return stat;
}
static int PHPMSessionExec(int req, char *session, char *name, char *value, int param TSRMLS_DC)
{
	int stat=0;
	TSRMLS_FETCH();
	ELOG("PHPMSessionExec");
	IFCONNECT_BEGIN
	FormatRequest(&t_reqb, req, session, name, value, param);
	PHPDoRequest(&t_conn, &t_reqb);

	stat = t_reqb.req->stat;

	if(t_reqb.req->stat != MREQ_OK)
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	IFCONNECT_ENDNR
	return stat;
}
static int PHPMSessionExecData(int req, char *session, char *name, char *value, void *data, int cb, int param TSRMLS_DC)
{
	int stat=0;
	TSRMLS_FETCH();
	ELOG("PHPMSessionExec");
	IFCONNECT_BEGIN
	FormatRequestData(&t_reqb, req, session, name, value, data, cb, param);
	PHPDoRequest(&t_conn, &t_reqb);

	stat = t_reqb.req->stat;

	if(t_reqb.req->stat != MREQ_OK)
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	IFCONNECT_ENDNR
	return stat;
}
/* {{{ proto bool msession_connect(string host, string port)
	 Connect to msession sever */
PHP_FUNCTION(msession_connect)
{
	char *szhost;
	int nport;
	zval **zhost;
	zval **zport;
	ELOG("msession_connect");
	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &zhost, &zport) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(zhost);
	convert_to_string_ex(zport);
	szhost = Z_STRVAL_PP(zhost);
	nport = atoi(Z_STRVAL_PP(zport));

	if(PHPMSessionConnect(szhost,nport))
	{
		RETURN_TRUE;
	}
	else
	{
		php_error(E_WARNING, "%s(): MSession connect failed", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void msession_disconnect(void)
   Disconnect from msession server */
PHP_FUNCTION(msession_disconnect)
{
	ELOG("msession_disconnect");
	PHPMSessionDisconnect();
	RETURN_NULL();
}
/* }}} */

/* {{{ proto int msession_count(void)
   Get session count */
PHP_FUNCTION(msession_count)
{
	int count=0;
	ELOG("msession_count");
	count = PHPMSessionGetInt(MREQ_COUNT, "","","",0 TSRMLS_CC);
	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto bool msession_create(string session[,string classname][,string data])
   Create a session cacheid */
PHP_FUNCTION(msession_create)
{
	int stat=0;
	int ret;
	char *szsession;
	zval **session;
	zval **zclassname;
	zval **zdata;
	char *classname="phpsession";
	char *data = NULL;
	int nargs = ZEND_NUM_ARGS();

	ELOG("msession_create");
	if(nargs < 1)
 	{
		WRONG_PARAM_COUNT;
	}

	if(nargs == 2)
	{
		ret = zend_get_parameters_ex(2, &session, &zclassname);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
	}
	else if(nargs == 3)
	{
		ret = zend_get_parameters_ex(3, &session, &zclassname, &zdata);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		convert_to_string_ex(zdata);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
		data = Z_STRVAL_PP(zdata);
	}
	if(ret = FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	stat =	PHPMSessionExec(MREQ_CREATE, szsession,classname ,data,0 TSRMLS_CC);

	if(stat==MREQ_OK)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */
/* {{{ proto bool msession_save_object(string session, string data[, array tuples[,string classname]])
   Create a session cacheid 
   This function really needs to be written to handle objects and not arrays.
*/
PHP_FUNCTION(msession_save_object)
{
	zval **session=NULL;
	zval **zdata=NULL;
	zval **tuples=NULL;
	zval **zclassname=NULL;
	HashPosition pos;
	zval **entry;
	char *key;
	uint keylen;
	ulong numndx;
	int ndx=0;
	char **pairs;
	HashTable *htTuples;
	int i;
	int nargs = ZEND_NUM_ARGS();
	int res;	
	int countpair; 
	TSRMLS_FETCH();
	char *szsession;
	char *classname;
	char *data;
	int ret;
	
	ELOG("msession_save_object");
	if(nargs < 2 )
 	{
		WRONG_PARAM_COUNT;
	}
	if(nargs == 2)
	{
		ret = zend_get_parameters_ex(2, &session, &zdata);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
	}
	else if(nargs == 3)
	{
		ret = zend_get_parameters_ex(3, &session, &zdata, &tuples);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		convert_to_string_ex(zdata);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
		data = Z_STRVAL_PP(zdata);
	}
	else if(nargs == 4)
	{
		ret = zend_get_parameters_ex(4, &session, &zdata, &tuples, &zclassname);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		convert_to_string_ex(zdata);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
		data = Z_STRVAL_PP(zdata);
	}
	
	htTuples = Z_ARRVAL_PP(tuples);

	countpair = zend_hash_num_elements(htTuples);

	pairs = (char **)emalloc(sizeof(char *) * (countpair+1) * 2);

	if(!pairs)
	{
		ELOG("no pairs");
		RETURN_FALSE;
	}

	ELOG("have pairs");

	/* Initializes pos */
	zend_hash_internal_pointer_reset_ex(htTuples, &pos);

	ELOG("reset pointer");

	for(i=0; i < countpair; i++)
	{
		if(zend_hash_get_current_data_ex(htTuples, (void **)&entry, &pos) != SUCCESS) 
			break;

		if(entry)
		{
			char *szentry;
			convert_to_string_ex(entry);
			szentry = Z_STRVAL_PP(entry);
			
       			if(zend_hash_get_current_key_ex(htTuples,&key,&keylen,&numndx,0,&pos)== HASH_KEY_IS_STRING)
			{
				pairs[ndx++] = key;
				pairs[ndx++] = szentry;
			}
		}
		zend_hash_move_forward_ex(htTuples, &pos);
	}

	IFCONNECT_BEGIN
	ELOG("FormatMulti");
	FormatRequestStrings(&t_reqb, MREQ_CREATE | MSTAT_TYPE_OBJECT, 
		szsession, classname, data, ndx, pairs);
	PHPDoRequest(&t_conn,&t_reqb);

	if(t_reqb.req->stat != MREQ_OK)
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));
	efree((void *)pairs);

	IFCONNECT_END

	RETURN_TRUE;

}
/* }}} */
/* {{{ proto bool msession_destroy(string name)
   Destroy a session */
PHP_FUNCTION(msession_destroy)
{
	char *szsession;
	zval **session;
	ELOG("msession_destroy");
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &session) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	szsession = Z_STRVAL_PP(session);
	PHPMSessionExec(MREQ_DROP, szsession, "", "", 0 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int msession_lock(string name)
   Lock a session */
PHP_FUNCTION(msession_lock)
{
	int stat;
	int param;

	char *szsession;
	zval **session;
	
	ELOG("msession_lock");
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &session) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	szsession = Z_STRVAL_PP(session);

	param = PHPMSessionGetInt(MREQ_SLOCK, szsession, "","",0 TSRMLS_CC);

	RETURN_LONG(param);
}
/* }}} */

/* {{{ proto int msession_stat(string name)
   Lock a session */
PHP_FUNCTION(msession_ctl)
{
	static char *parray[] =
	{	"EXIST",
		"TTL",
		"AGE",
		"TLA",
		"CTIME",
		"TOUCH",
		"NOW",
		NULL
	};
	int stat = 0;
	int param;

	char *szsession;
	zval **session;
	zval **which;
	int fn = MREQ_CTL_EXIST;
			
	int n = ZEND_NUM_ARGS();

	ELOG("msession_ctl");
	if(n != 1 && n != 2)
 	{
		WRONG_PARAM_COUNT;
	}
	if(zend_get_parameters_ex(n,&session,&which) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	szsession = Z_STRVAL_PP(session);

	if(n > 1)
	{
		char *szwhich;
		int i;
		convert_to_string_ex(which);
		szwhich = Z_STRVAL_PP(which);
		for(i=0; parray[i]; i++)
		{
			if(strcasecmp(parray[i], szwhich) == 0)
			{
				fn = i;
				break;
			}
		}
	}
	param = PHPMSessionGetInt(MREQ_CTL, szsession, "", "", fn TSRMLS_CC);
	RETURN_LONG(param);
}

/* {{{ proto int msession_unlock(string session, int key)
   Unlock a session */
PHP_FUNCTION(msession_unlock)
{
	int stat;
	int param;

	char *szsession;
	long lkey;
	zval **session;
	zval **key;
	
	ELOG("msession_unlock");
	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &session, &key) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	szsession = Z_STRVAL_PP(session);
	convert_to_long_ex(key);
	lkey = Z_LVAL_PP(key);

	param = PHPMSessionGetInt(MREQ_SUNLOCK, szsession, "", "", lkey TSRMLS_CC);
	RETURN_LONG(param);
}
/* }}} */

/* {{{ proto bool msession_set(string session, string name, string value)
   Set value in session */
PHP_FUNCTION(msession_set)
{
	int stat;
	char *szsession;
	char *szname;
	char *szvalue;
	zval **session;
	zval **name;
	zval **value;
	ELOG("msession_get");
	
	if(ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3,&session,&name,&value) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(name);
	convert_to_string_ex(value);

	szsession = Z_STRVAL_PP(session);
	szname = Z_STRVAL_PP(name);
	szvalue = Z_STRVAL_PP(value);

	stat = PHPMSessionExec(MREQ_SETVAL, szsession, szname, szvalue, 0 TSRMLS_CC);

	if(stat==MREQ_OK)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string msession_get(string session, string name, string default_value)
   Get value from session */
PHP_FUNCTION(msession_get)
{
	int stat;
	char *szvalue=NULL;
	char *szsession;
	char *szname;
	zval **session;
	zval **name;
	zval **value;
	ELOG("msession_get");
	
	if(ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3,&session,&name,&value) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(name);
	convert_to_string_ex(value);
	szsession = Z_STRVAL_PP(session);
	szname = Z_STRVAL_PP(name);
	szvalue = Z_STRVAL_PP(value);

	szvalue = PHPMSessionGetString(MREQ_GETVAL, szsession,szname, szvalue, 0 TSRMLS_CC);

	if(szvalue)
	{
		RETURN_STRING(szvalue,0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto string msession_uniq(int num_chars[, string classname[, string data]])
   Get uniq id */
PHP_FUNCTION(msession_uniq)
{
	int stat=0;
	int ret = FAILURE;
	int cbname;
	zval **session;
	zval **zclassname;
	zval **zdata;
	char *szsession;
	char *classname="phpsession";
	char *data = NULL;
	int nargs = ZEND_NUM_ARGS();

	ELOG("msession_uniq");
	if(nargs < 1)
 	{
		WRONG_PARAM_COUNT;
	}
	if(nargs == 1)
	{
		ret = zend_get_parameters_ex(1, &session);
		convert_to_string_ex(session);
		szsession = Z_STRVAL_PP(session);
	}
	else if(nargs == 2)
	{
		ret = zend_get_parameters_ex(2, &session, &zclassname);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
	}
	else if(nargs == 3)
	{
		ret = zend_get_parameters_ex(3, &session, &zclassname, &zdata);
		convert_to_string_ex(session);
		convert_to_string_ex(zclassname);
		convert_to_string_ex(zdata);
		szsession = Z_STRVAL_PP(session);
		classname = Z_STRVAL_PP(zclassname);
		data = Z_STRVAL_PP(zdata);
	}
	else
	{
		WRONG_PARAM_COUNT;
	}

	cbname = atoi(szsession);
	szsession = PHPMSessionGetString(MREQ_UNIQ, NULL, classname, data,MPARM_CREATE(0,cbname) TSRMLS_CC);

	if(szsession)
	{
		RETURN_STRING(szsession,0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto string msession_randstr(int num_chars)
   Get random string */
PHP_FUNCTION(msession_randstr)
{
	int stat;
	char *szvalue=NULL;
	long val;
	zval **param;
	ELOG("msession_randstr");
	
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1,&param) == FAILURE)
	{
			WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(param);
	val = Z_LVAL_PP(param);

	szvalue = PHPMSessionGetString(MREQ_RANDSTR, "","","",val TSRMLS_CC);

	if(szvalue)
	{
		RETURN_STRING(szvalue,0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto array msession_find(string name, string value)
   Find all sessions with name and value */
PHP_FUNCTION(msession_find)
{
	int stat;
	char *szname;
	char *szvalue;
	zval **name;
	zval **value;
	ELOG("msession_find");
	
	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &name, &value) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	convert_to_string_ex(value);
	szname = Z_STRVAL_PP(name);
	szvalue = Z_STRVAL_PP(value);

	stat = PHPMSessionGetArray(return_value, MREQ_FIND, "", szname, szvalue, 0 TSRMLS_CC);

}
/* }}} */

/* {{{ proto array msession_list(void)
   List all sessions  */
PHP_FUNCTION(msession_list)
{
	ELOG("msession_list");
	int stat = PHPMSessionGetArray(return_value, MREQ_LIST, "", "","", 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto array msession_get_array(string session)
   Get array of msession variables  */
PHP_FUNCTION(msession_get_array)
{
	int stat;
	char *szsession;
	zval **session;
	ELOG("msession_getarray");
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &session) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
  	}
	convert_to_string_ex(session);
	szsession = Z_STRVAL_PP(session);

	stat = PHPMSessionGetArrayPairs(return_value, MREQ_GETALL, szsession, "","", 0 TSRMLS_CC);
}
/* }}} */
/* {{{ proto object msession_get_object(string object)
   Get msession variable object  */
PHP_FUNCTION(msession_get_object)
{
// FIXME FIXME FIXME
	int stat;
	char *szsession;
	zval **session;
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &session) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
  	}
	convert_to_string_ex(session);
	szsession = Z_STRVAL_PP(session);

	stat = PHPMSessionGetArrayPairs(return_value, MREQ_GETOBJECT, szsession, NULL, NULL, 0 TSRMLS_CC);
}
/* }}} */
/* {{{ proto bool msession_set_array(string session, array tuples)
   Set msession variables from an array*/
PHP_FUNCTION(msession_set_array)
{
	zval **session;
	zval **tuples;
	HashPosition pos;
	zval **entry;
	char *key;
#ifdef PHP_4_1
	uint keylen;
#endif
#ifdef PHP_4_0
	ulong keylen;
#endif
	ulong numndx;
	int ndx=0;
	char **pairs;
	HashTable *htTuples;
	int i;
	int countpair; 

	ELOG("msession_set_array");
	TSRMLS_FETCH();
	
	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &session, &tuples) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	htTuples = Z_ARRVAL_PP(tuples);

	countpair = zend_hash_num_elements(htTuples);

	pairs = (char **)emalloc(sizeof(char *) * countpair * 2);

	if(!pairs)
	{
		ELOG("no pairs");
		RETURN_FALSE;
	}

	ELOG("have pairs");

	/* Initializes pos */
	zend_hash_internal_pointer_reset_ex(htTuples, &pos);

	ELOG("reset pointer");

	for(i=0; i < countpair; i++)
	{
		if(zend_hash_get_current_data_ex(htTuples, (void **)&entry, &pos) != SUCCESS) 
			break;

		if(entry)
		{
			char *szentry;
			convert_to_string_ex(entry);
			szentry = Z_STRVAL_PP(entry);
			
       			if(zend_hash_get_current_key_ex(htTuples,&key,&keylen,&numndx,0,&pos)== HASH_KEY_IS_STRING)
			{
#ifdef ERR_DEBUG
{
				char buffer [256];
				sprintf(buffer, "%s=%s\n", key, szentry);
				ELOG(buffer);
}
#endif
				pairs[ndx++] = key;
				pairs[ndx++] = szentry;
			}
		}
		zend_hash_move_forward_ex(htTuples, &pos);
	}

	IFCONNECT_BEGIN
	ELOG("FormatMulti");
	FormatRequestStrings(&t_reqb, MREQ_SETVAL, Z_STRVAL_PP(session), NULL, NULL, ndx, pairs);
	PHPDoRequest(&t_conn,&t_reqb);

	if(t_reqb.req->stat != MREQ_OK)
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));
	efree((void *)pairs);
	IFCONNECT_END

}
/* }}} */

/* {{{ proto array msession_listvar(string name)
   return associative array of value:session for all sessions with a variable named 'name' */
PHP_FUNCTION(msession_listvar)
{
	int stat;
	zval **name;
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);

	stat = PHPMSessionGetArrayPairs(return_value, MREQ_LISTVAR, "", Z_STRVAL_PP(name),"",0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto int msession_timeout(string session [, int param ])
   Set/get session timeout */
PHP_FUNCTION(msession_timeout)
{
	int stat;
	int param;
	zval **session;
	int ac = ZEND_NUM_ARGS();
	int zstat = FAILURE;
	int timeout = 0;
	ELOG("msession_timeout");
	if(ac == 1)
	{
		zstat = zend_get_parameters_ex(1, &session);
	}
	else if(ac == 2)
	{
		zval **param;
		zstat = zend_get_parameters_ex(2, &session, &param);
		convert_to_long_ex(param);
		timeout = Z_LVAL_PP(param);
	}
	if(zstat == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);

	param = PHPMSessionGetInt(MREQ_TIMEOUT, Z_STRVAL_PP(session),"","",timeout TSRMLS_CC);

	RETURN_LONG(param);
}
/* }}} */

/* {{{ proto string msession_inc(string session, string name) 
   Increment value in session */
PHP_FUNCTION(msession_inc)
{
	int stat;
	char *szvalue;
	zval **session;
	zval **name;
	
	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &session, &name) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(name);

	szvalue = PHPMSessionGetString(MREQ_ADD, Z_STRVAL_PP(session), Z_STRVAL_PP(name),"1",0 TSRMLS_CC);

	if(szvalue)
	{
		RETURN_STRING(szvalue,0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */
/* {{{ proto string msession_add(string session, string name, string value) 
   Increment value in session */
PHP_FUNCTION(msession_add)
{
	int stat;
	char *szvalue;
	zval **session;
	zval **name;
	zval **value;
	ELOG("msession_add");
	
	if(ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &session, &name, &value) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(name);
	convert_to_string_ex(value);

	szvalue = PHPMSessionGetString(MREQ_ADD, Z_STRVAL_PP(session), Z_STRVAL_PP(name),Z_STRVAL_PP(value),0 TSRMLS_CC);

	if(szvalue)
	{
		RETURN_STRING(szvalue,0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */
/* {{{ proto string msession_muldiv(string session, string name, string mul, string div) 
   Increment value in session */
PHP_FUNCTION(msession_muldiv)
{
	int stat;
	char *szvalue=NULL;
	zval **session;
	zval **name;
	zval **mul;
	zval **div;
	char *pdata;
	ELOG("msession_muldiv");
	
	if(ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &session, &name, &mul, &div) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(name);
	convert_to_string_ex(mul);
	convert_to_string_ex(div);

	pdata = Z_STRVAL_PP(div);

	IFCONNECT_BEGIN
	FormatRequestData(&t_reqb, MREQ_MULDIV, 
		Z_STRVAL_PP(session), Z_STRVAL_PP(name), Z_STRVAL_PP(mul), pdata, strlen(pdata)+1, 0);

	PHPDoRequest(&t_conn, &t_reqb);

	if(t_reqb.req->stat == MREQ_OK)
		szvalue = safe_estrdup(t_reqb.req->datum);
	else
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	IFCONNECT_ENDNR

	if(szvalue)
	{
		RETURN_STRING(szvalue,0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto string msession_getdata(string session)
   Get data session unstructured data. (PHP sessions use this)  */
PHP_FUNCTION(msession_getdata)
{
	char *val;
	zval **session;
	ELOG("msession_getdata");
	
	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &session) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);

	val = PHPMSessionGetString(MREQ_DATAGET, Z_STRVAL_PP(session), "", "", 0 TSRMLS_CC);

	if(val)
	{
		RETURN_STRING(val, 0)
	}
	else
	{
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto bool msession_setdata(string session, string value)
   Set data session unstructured data. (PHP sessions use this)  */
PHP_FUNCTION(msession_setdata)
{
	int stat;
	zval **session;
	zval **value;
	ELOG("msession_setdata");
	
	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &session, &value) == FAILURE)
 	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(value);
	char *szdata = Z_STRVAL_PP(value);

	stat = PHPMSessionExecData(MREQ_DATASET, Z_STRVAL_PP(session),NULL,NULL,szdata,strlen(szdata),0 TSRMLS_CC);
	if(stat==MREQ_OK)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string msession_plugin(string session, string val [, string param ])
   Call the personality plugin escape function */
PHP_FUNCTION(msession_plugin)
{
	int ret;
	char *retval=NULL;
	zval **session;
	zval **val;
	zval **param=NULL;
	char *szparam;
	
	ELOG("msession_plugin");
	if(ZEND_NUM_ARGS() == 3)
	{
		ret = zend_get_parameters_ex(3, &session, &val, &param);
		convert_to_string_ex(param);
	}
	else if(ZEND_NUM_ARGS() == 2)
	{
		ret = zend_get_parameters_ex(2, &session, &val);
	}
	else
 	{
		WRONG_PARAM_COUNT;
	}
	if(ret == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(session);
	convert_to_string_ex(val);

	ret = atoi(Z_STRVAL_PP(val));
	szparam = (param) ? Z_STRVAL_PP(param) : "";

	retval =PHPMSessionGetString(MREQ_PLUGIN, Z_STRVAL_PP(session), Z_STRVAL_PP(val), szparam, ret TSRMLS_CC);

	if(retval)
	{	
		RETURN_STRING(retval, 0)
	}
	else 
	{
		RETURN_FALSE;
	}
}
/* }}} */
#define MAX_EXT_VAL	6
/* {{{ proto string msession_call(string fn_name, [, string param1 ], ... [,string param4])
   Call the plugin function named fn_name */
PHP_FUNCTION(msession_call)
{
	int n;
	int i;
	int ret;
	char *retval=NULL;
	zval **val[MAX_EXT_VAL];
	char *strings[MAX_EXT_VAL+1];
/* 	zval **param=NULL; */
	
	TSRMLS_FETCH();
	n = ZEND_NUM_ARGS();
	ELOG("msession_call");

	if((n < 1) || (n > MAX_EXT_VAL))
 	{
		WRONG_PARAM_COUNT;
	}

	ret = zend_get_parameters_ex(n, &val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6]);

	if(ret == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	for(i=0; i < n; i++)
	{
		convert_to_string_ex(val[i]);
		strings[i] = Z_STRVAL_PP(val[i]);
	}
	strings[i]=0;

	IFCONNECT_BEGIN
	FormatRequestStrings(&t_reqb, MREQ_CALL, NULL , strings[0], NULL, n-1, (n>1) ? &strings[1] : NULL);
	PHPDoRequest(&t_conn, &t_reqb);

	if(t_reqb.req->stat==MREQ_OK && t_reqb.req->len)
		retval = safe_estrdup(t_reqb.req->datum);
	else
		php_error(E_WARNING, s_szErrFmt, get_active_function_name(TSRMLS_C), ReqbErr(&t_reqb));

	IFCONNECT_ENDNR

	if(retval)
	{	
		RETURN_STRING(retval, 0)
	}
	else 
	{
		RETURN_FALSE;
	}
}
/* }}} */

#ifdef HAVE_PHP_SESSION

PS_OPEN_FUNC(msession)
{
	char *szport;
	char host[MAX_HOST];
	int port = msession_split_host_port(host, sizeof(host), (char *)save_path, DEF_PORT);

	ELOG("PS_OPEN_FUNC");

	ELOG( "ps_open_msession");
	PS_SET_MOD_DATA((void *)1); /* session.c needs a non-zero here! */
	if (PHPMSessionConnect(host, port)) {
		ELOG("Success");
		return SUCCESS;
	} else {
		ELOG("Failure");
		return FAILURE;
	}
}

PS_CLOSE_FUNC(msession)
{
	ELOG( "ps_close_msession");
	PHPMSessionDisconnect();
	PS_SET_MOD_DATA((void *)0); 
	return SUCCESS;
}

PS_READ_FUNC(msession)
{
	ELOG( "ps_read_msession");
	*val = PHPMSessionGetString(MREQ_DATAGET, (char *)key, "", "", 0 TSRMLS_CC);
	if(*val)
	{
		*vallen = strlen(*val);
		ELOG( *val );
	}
	else
	{
		*val = emalloc(1);
		**val=0;
		*vallen = 0;
	}
	return SUCCESS;
}

PS_WRITE_FUNC(msession)
{
	ELOG("PS_WRITE_FUNC");
	ELOG(key);
	ELOG(val);
	int stat = PHPMSessionExecData(MREQ_DATASET, key, NULL, NULL, val, strlen(val), 0 TSRMLS_CC);
	return (stat == MREQ_OK) ? SUCCESS : FAILURE;
}

PS_DESTROY_FUNC(msession)
{
	ELOG( "ps_destroy_msession");
	int stat = PHPMSessionExec(MREQ_DROP, (char *)key, "", "", 0 TSRMLS_CC);
	return (stat == MREQ_OK) ? SUCCESS : FAILURE;
}

PS_GC_FUNC(msession)
{
	ELOG( "ps_gc_msession");
	return SUCCESS;
}

#ifdef HAVE_PHP_SESSION_CREATESID
PS_CREATE_SID_FUNC(msession)
{
	ELOG("PS_CREATE_SID_FUNCx");
	char * sessid = PHPMSessionGetString(MREQ_UNIQ, "","phpsess","",MPARM_CREATE(0,SID_LEN) TSRMLS_CC);

	if(!sessid || !strlen(sessid))
		ELOG("No SID returned from UNIQ");
	else
		ELOG(sessid);

	if(!sessid)
		sessid = php_session_create_id(mod_data, newlen TSRMLS_CC);

	ELOG(sessid);
	return sessid;
}
#endif  /* HAVE_PHP_SESSION_CREATESID */
#endif	/* HAVE_PHP_SESSION */
#endif	/* HAVE_MSESSION */

