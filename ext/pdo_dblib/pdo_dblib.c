/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
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

function_entry pdo_dblib_functions[] = {
	{NULL, NULL, NULL}
};

zend_module_entry pdo_dblib_module_entry = {
	STANDARD_MODULE_HEADER,
#if PDO_DBLIB_IS_MSSQL
	"pdo_mssql",
#else
	"pdo_sybase",
#endif
	pdo_dblib_functions,
	PHP_MINIT(pdo_dblib),
	PHP_MSHUTDOWN(pdo_dblib),
	NULL,
	NULL,
	PHP_MINFO(pdo_dblib),
	"0.1-dev",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PDO_DBLIB
ZEND_GET_MODULE(pdo_dblib)
#endif

static int error_handler(DBPROCESS *dbproc, int severity, int dberr,
	int oserr, char *dberrstr, char *oserrstr)
{
	TSRMLS_FETCH();

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "dblib error: %s (severity %d)", dberrstr, severity);	

	return INT_CANCEL;
}

static int msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate,
	int severity, char *msgtext, char *srvname, char *procname, DBUSMALLINT line)
{
	TSRMLS_FETCH();

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "dblib message: %s (severity %d)", msgtext, severity);

	return 0;
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
		"Sybase"
#endif
		" DB-lib", "enabled");
	php_info_print_table_row(2, "Flavour", PDO_DBLIB_FLAVOUR);
	php_info_print_table_end();
}

