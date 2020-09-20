/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_odbc.h"
#include "php_pdo_odbc_int.h"

/* {{{ pdo_odbc_deps[] */
static const zend_module_dep pdo_odbc_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
/* }}} */

/* {{{ pdo_odbc_module_entry */
zend_module_entry pdo_odbc_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_odbc_deps,
	"PDO_ODBC",
	NULL,
	PHP_MINIT(pdo_odbc),
	PHP_MSHUTDOWN(pdo_odbc),
	NULL,
	NULL,
	PHP_MINFO(pdo_odbc),
	PHP_PDO_ODBC_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_ODBC
ZEND_GET_MODULE(pdo_odbc)
#endif

#ifdef SQL_ATTR_CONNECTION_POOLING
zend_ulong pdo_odbc_pool_on = SQL_CP_OFF;
zend_ulong pdo_odbc_pool_mode = SQL_CP_ONE_PER_HENV;
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_odbc)
{
#ifdef SQL_ATTR_CONNECTION_POOLING
	char *pooling_val = NULL;
#endif

	if (FAILURE == php_pdo_register_driver(&pdo_odbc_driver)) {
		return FAILURE;
	}

#ifdef SQL_ATTR_CONNECTION_POOLING
	/* ugh, we don't really like .ini stuff in PDO, but since ODBC connection
	 * pooling is process wide, we can't set it from within the scope of a
	 * request without affecting others, which goes against our isolated request
	 * policy.  So, we use cfg_get_string here to check it this once.
	 * */
	if (FAILURE == cfg_get_string("pdo_odbc.connection_pooling", &pooling_val) || pooling_val == NULL) {
		pooling_val = "strict";
	}
	if (strcasecmp(pooling_val, "strict") == 0 || strcmp(pooling_val, "1") == 0) {
		pdo_odbc_pool_on = SQL_CP_ONE_PER_HENV;
		pdo_odbc_pool_mode = SQL_CP_STRICT_MATCH;
	} else if (strcasecmp(pooling_val, "relaxed") == 0) {
		pdo_odbc_pool_on = SQL_CP_ONE_PER_HENV;
		pdo_odbc_pool_mode = SQL_CP_RELAXED_MATCH;
	} else if (*pooling_val == '\0' || strcasecmp(pooling_val, "off") == 0) {
		pdo_odbc_pool_on = SQL_CP_OFF;
	} else {
		php_error_docref(NULL, E_CORE_ERROR, "Error in pdo_odbc.connection_pooling configuration. Value must be one of \"strict\", \"relaxed\", or \"off\"");
		return FAILURE;
	}

	if (pdo_odbc_pool_on != SQL_CP_OFF) {
		SQLSetEnvAttr(SQL_NULL_HANDLE, SQL_ATTR_CONNECTION_POOLING, (void*)pdo_odbc_pool_on, 0);
	}
#endif

	REGISTER_PDO_CLASS_CONST_LONG("ODBC_ATTR_USE_CURSOR_LIBRARY", PDO_ODBC_ATTR_USE_CURSOR_LIBRARY);
	REGISTER_PDO_CLASS_CONST_LONG("ODBC_ATTR_ASSUME_UTF8", PDO_ODBC_ATTR_ASSUME_UTF8);
	REGISTER_PDO_CLASS_CONST_LONG("ODBC_SQL_USE_IF_NEEDED", SQL_CUR_USE_IF_NEEDED);
	REGISTER_PDO_CLASS_CONST_LONG("ODBC_SQL_USE_DRIVER", SQL_CUR_USE_DRIVER);
	REGISTER_PDO_CLASS_CONST_LONG("ODBC_SQL_USE_ODBC", SQL_CUR_USE_ODBC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo_odbc)
{
	php_pdo_unregister_driver(&pdo_odbc_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdo_odbc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for ODBC (" PDO_ODBC_TYPE ")" , "enabled");
#ifdef SQL_ATTR_CONNECTION_POOLING
	php_info_print_table_row(2, "ODBC Connection Pooling",	pdo_odbc_pool_on == SQL_CP_OFF ?
			"Disabled" : (pdo_odbc_pool_mode == SQL_CP_STRICT_MATCH ? "Enabled, strict matching" : "Enabled, relaxed matching"));
#else
	php_info_print_table_row(2, "ODBC Connection Pooling", "Not supported in this build");
#endif
	php_info_print_table_end();
}
/* }}} */
