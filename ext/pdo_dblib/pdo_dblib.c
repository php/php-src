/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_dblib.h"
#include "php_pdo_dblib_int.h"
#include "zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(dblib)
static PHP_GINIT_FUNCTION(dblib);

const zend_function_entry pdo_dblib_functions[] = {
	PHP_FE_END
};

#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_dblib_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
#endif

#if PDO_DBLIB_IS_MSSQL
zend_module_entry pdo_mssql_module_entry = {
#else
zend_module_entry pdo_dblib_module_entry = {
#endif
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_dblib_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
#if PDO_DBLIB_IS_MSSQL
	"pdo_mssql",
#elif defined(PHP_WIN32)
	"pdo_sybase",
#else
	"pdo_dblib",
#endif
	pdo_dblib_functions,
	PHP_MINIT(pdo_dblib),
	PHP_MSHUTDOWN(pdo_dblib),
	NULL,
	PHP_RSHUTDOWN(pdo_dblib),
	PHP_MINFO(pdo_dblib),
	"1.0.1",
	PHP_MODULE_GLOBALS(dblib),
	PHP_GINIT(dblib),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#if defined(COMPILE_DL_PDO_DBLIB) || defined(COMPILE_DL_PDO_MSSQL)
#if PDO_DBLIB_IS_MSSQL
ZEND_GET_MODULE(pdo_mssql)
#else
ZEND_GET_MODULE(pdo_dblib)
#endif
#endif

int error_handler(DBPROCESS *dbproc, int severity, int dberr,
	int oserr, char *dberrstr, char *oserrstr)
{
	pdo_dblib_err *einfo;
	char *state = "HY000";
	TSRMLS_FETCH();

	if(dbproc) {
		einfo = (pdo_dblib_err*)dbgetuserdata(dbproc);
		if (!einfo) einfo = &DBLIB_G(err);
	} else {
		einfo = &DBLIB_G(err);
	}	

	einfo->severity = severity;
	einfo->oserr = oserr;
	einfo->dberr = dberr;
	if (einfo->oserrstr) {
		efree(einfo->oserrstr);
	}
	if (einfo->dberrstr) {
		efree(einfo->dberrstr);
	}
	if (oserrstr) {
		einfo->oserrstr = estrdup(oserrstr);
	} else {
		einfo->oserrstr = NULL;
	}
	if (dberrstr) {
		einfo->dberrstr = estrdup(dberrstr);
	} else {
		einfo->dberrstr = NULL;
	}

	switch (dberr) {
		case SYBESEOF:
		case SYBEFCON:	state = "01002"; break;
		case SYBEMEM:	state = "HY001"; break;
		case SYBEPWD:	state = "28000"; break;
	}
	strcpy(einfo->sqlstate, state);

#if 0
	php_error_docref(NULL TSRMLS_CC, E_WARNING,
		"dblib error: %d %s (severity %d)",
		dberr, dberrstr, severity);	
#endif

	return INT_CANCEL;
}

int msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate,
	int severity, char *msgtext, char *srvname, char *procname, DBUSMALLINT line)
{
	pdo_dblib_err *einfo;
	TSRMLS_FETCH();

	if (severity) {
		einfo = (pdo_dblib_err*)dbgetuserdata(dbproc);
		if (!einfo) {
			einfo = &DBLIB_G(err);
		}

		if (einfo->lastmsg) {
			efree(einfo->lastmsg);
		}

		einfo->lastmsg = estrdup(msgtext);
	}

#if 0
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "dblib message: %s (severity %d)", msgtext, severity);
#endif

	return 0;
}

static PHP_GINIT_FUNCTION(dblib)
{
	memset(dblib_globals, 0, sizeof(*dblib_globals));
	dblib_globals->err.sqlstate = dblib_globals->sqlstate;
}

PHP_RSHUTDOWN_FUNCTION(pdo_dblib)
{
	if (DBLIB_G(err).oserrstr) {
		efree(DBLIB_G(err).oserrstr);
		DBLIB_G(err).oserrstr = NULL;
	}
	if (DBLIB_G(err).dberrstr) {
		efree(DBLIB_G(err).dberrstr);
		DBLIB_G(err).dberrstr = NULL;
	}
	if (DBLIB_G(err).lastmsg) {
		efree(DBLIB_G(err).lastmsg);
		DBLIB_G(err).lastmsg = NULL;
	}
	return SUCCESS;
}

PHP_MINIT_FUNCTION(pdo_dblib)
{
	if (FAIL == dbinit()) {
		return FAILURE;
	}
	
	if (FAILURE == php_pdo_register_driver(&pdo_dblib_driver)) {
		return FAILURE;
	}
	
	/* TODO: 
	
	dbsetifile()
	dbsetmaxprocs()
	dbsetlogintime()
	dbsettime()
	
	 */

#if !PHP_DBLIB_IS_MSSQL
	dberrhandle(error_handler);
	dbmsghandle(msg_handler);
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pdo_dblib)
{
	php_pdo_unregister_driver(&pdo_dblib_driver);
	dbexit();
	return SUCCESS;
}

PHP_MINFO_FUNCTION(pdo_dblib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for "
#if PDO_DBLIB_IS_MSSQL
		"MSSQL"
#elif defined(PHP_WIN32)
		"FreeTDS/Sybase/MSSQL"
#else
		"FreeTDS/Sybase"
#endif
		" DB-lib", "enabled");
	php_info_print_table_row(2, "Flavour", PDO_DBLIB_FLAVOUR);
	php_info_print_table_end();
}

