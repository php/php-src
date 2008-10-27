/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: George Schlossnagle <george@omniti.com>                      |
  |         Johannes Schlueter <johannes@mysql.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_mysql.h"
#include "php_pdo_mysql_int.h"

#ifdef COMPILE_DL_PDO_MYSQL
ZEND_GET_MODULE(pdo_mysql)
#endif

#if PDO_USE_MYSQLND
ZEND_DECLARE_MODULE_GLOBALS(pdo_mysql);

#ifndef PHP_WIN32
# ifndef PDO_MYSQL_UNIX_ADDR
#  define PDO_MYSQL_UNIX_ADDR  "/tmp/mysql.sock"
# endif
#endif


/* {{{ PHP_INI_BEGIN
*/
PHP_INI_BEGIN()
#ifndef PHP_WIN32
	STD_PHP_INI_ENTRY("pdo_mysql.default_socket", PDO_MYSQL_UNIX_ADDR, PHP_INI_SYSTEM, OnUpdateString, default_socket, zend_pdo_mysql_globals, pdo_mysql_globals)
#endif
#if PDO_DBG_ENABLED
	STD_PHP_INI_ENTRY("pdo_mysql.debug",	NULL, PHP_INI_SYSTEM, OnUpdateString, debug, zend_pdo_mysql_globals, pdo_mysql_globals)
#endif
	STD_PHP_INI_ENTRY("pdo_mysql.cache_size",			"2000",	PHP_INI_SYSTEM,		OnUpdateLong,		cache_size,			zend_pdo_mysql_globals,		pdo_mysql_globals)
PHP_INI_END()
/* }}} */
#endif

/* true global environment */
#ifdef PDO_USE_MYSQLND
static MYSQLND_ZVAL_PCACHE *pdo_mysqlnd_zval_cache;
#endif
		
		
/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(pdo_mysql)
{
#if PDO_USE_MYSQLND
	REGISTER_INI_ENTRIES();
#endif

	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_USE_BUFFERED_QUERY", (long)PDO_MYSQL_ATTR_USE_BUFFERED_QUERY);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_LOCAL_INFILE", (long)PDO_MYSQL_ATTR_LOCAL_INFILE);	
#ifndef PDO_USE_MYSQLND
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_MAX_BUFFER_SIZE", (long)PDO_MYSQL_ATTR_MAX_BUFFER_SIZE);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_INIT_COMMAND", (long)PDO_MYSQL_ATTR_INIT_COMMAND);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_READ_DEFAULT_FILE", (long)PDO_MYSQL_ATTR_READ_DEFAULT_FILE);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_READ_DEFAULT_GROUP", (long)PDO_MYSQL_ATTR_READ_DEFAULT_GROUP);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_COMPRESS", (long)PDO_MYSQL_ATTR_COMPRESS);
#endif
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_DIRECT_QUERY", (long)PDO_MYSQL_ATTR_DIRECT_QUERY);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_FOUND_ROWS", (long)PDO_MYSQL_ATTR_FOUND_ROWS);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_IGNORE_SPACE", (long)PDO_MYSQL_ATTR_IGNORE_SPACE);

#ifdef PDO_USE_MYSQLND
	pdo_mysqlnd_zval_cache = mysqlnd_palloc_init_cache(PDO_MYSQL_G(cache_size));
#endif
	
	return php_pdo_register_driver(&pdo_mysql_driver);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(pdo_mysql)
{
	php_pdo_unregister_driver(&pdo_mysql_driver);
#if PDO_USE_MYSQLND
	mysqlnd_palloc_free_cache(pdo_mysqlnd_zval_cache);
	UNREGISTER_INI_ENTRIES();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(pdo_mysql)
{
	php_info_print_table_start();

	php_info_print_table_header(2, "PDO Driver for MySQL", "enabled");
	php_info_print_table_row(2, "Client API version", mysql_get_client_info());

#ifdef PDO_USE_MYSQLND
	{
		zval values;

		php_info_print_table_header(2, "Persistent cache", pdo_mysqlnd_zval_cache ? "enabled":"disabled");
		
		if (pdo_mysqlnd_zval_cache) {
			/* Now report cache status */
			mysqlnd_palloc_stats(pdo_mysqlnd_zval_cache, &values);
			mysqlnd_minfo_print_hash(&values);
			zval_dtor(&values);
		}
	}
#endif
	php_info_print_table_end();

#ifdef PDO_USE_MYSQLND
	DISPLAY_INI_ENTRIES();
#endif
}
/* }}} */


#if PDO_USE_MYSQLND
/* {{{ PHP_RINIT_FUNCTION
 */
static PHP_RINIT_FUNCTION(pdo_mysql)
{
	PDO_MYSQL_G(mysqlnd_thd_zval_cache) = mysqlnd_palloc_rinit(pdo_mysqlnd_zval_cache);
	
#if PDO_DBG_ENABLED
	if (PDO_MYSQL_G(debug)) {
		MYSQLND_DEBUG *dbg = mysqlnd_debug_init(TSRMLS_C);
		if (!dbg) {
			return FAILURE;
		}
		dbg->m->set_mode(dbg, PDO_MYSQL_G(debug));
		PDO_MYSQL_G(dbg) = dbg;
	}
#endif
	
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
static PHP_RSHUTDOWN_FUNCTION(pdo_mysql)
{
	mysqlnd_palloc_rshutdown(PDO_MYSQL_G(mysqlnd_thd_zval_cache));
	
#if PDO_DBG_ENABLED
	MYSQLND_DEBUG *dbg = PDO_MYSQL_G(dbg);
	PDO_DBG_ENTER("RSHUTDOWN");
	if (dbg) {
		dbg->m->close(dbg);
		dbg->m->free_handle(dbg);
		PDO_MYSQL_G(dbg) = NULL;
	}
#endif
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(pdo_mysql)
{
	pdo_mysql_globals->mysqlnd_thd_zval_cache = NULL; /* zval cache */
	pdo_mysql_globals->cache_size = 0;
#ifndef PHP_WIN32
	pdo_mysql_globals->default_socket = NULL;
#endif
#if PDO_DBG_ENABLED
	pdo_mysql_globals->debug = NULL;	/* The actual string */
	pdo_mysql_globals->dbg = NULL;	/* The DBG object*/
#endif
}
/* }}} */
#endif


/* {{{ pdo_mysql_functions[] */
const zend_function_entry pdo_mysql_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_mysql_deps[] */
#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_mysql_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
#ifdef PDO_USE_MYSQLND
	ZEND_MOD_REQUIRED("mysqlnd")
#endif
	{NULL, NULL, NULL}
};
#endif
/* }}} */

/* {{{ pdo_mysql_module_entry */
zend_module_entry pdo_mysql_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_mysql_deps,
	"pdo_mysql",
	pdo_mysql_functions,
	PHP_MINIT(pdo_mysql),
	PHP_MSHUTDOWN(pdo_mysql),
#if PDO_USE_MYSQLND
	PHP_RINIT(pdo_mysql),
	PHP_RSHUTDOWN(pdo_mysql),
#else
	NULL,
	NULL,
#endif
	PHP_MINFO(pdo_mysql),
	"1.0.2",
#if PDO_USE_MYSQLND
	PHP_MODULE_GLOBALS(pdo_mysql),
	PHP_GINIT(pdo_mysql),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
#else
	STANDARD_MODULE_PROPERTIES
#endif
};
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
