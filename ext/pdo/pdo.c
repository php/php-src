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
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Marcus Boerger <helly@php.net>                               |
  |         Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"
#include "zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(pdo)

/* True global resources - no need for thread safety here */

/* the registry of PDO drivers */
static HashTable pdo_driver_hash;

/* we use persistent resources for the driver connection stuff */
static int le_ppdo;

/* for exceptional circumstances */
zend_class_entry *pdo_exception_ce;

zend_class_entry *pdo_dbh_ce, *pdo_dbstmt_ce;

/* {{{ pdo_functions[] */
function_entry pdo_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_module_entry */
zend_module_entry pdo_module_entry = {
	STANDARD_MODULE_HEADER,
	"pdo",
	pdo_functions,
	PHP_MINIT(pdo),
	PHP_MSHUTDOWN(pdo),
	PHP_RINIT(pdo),
	PHP_RSHUTDOWN(pdo),
	PHP_MINFO(pdo),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO
ZEND_GET_MODULE(pdo)
#endif

/* {{{ PHP_INI */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("pdo.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_pdo_globals, pdo_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_pdo_init_globals */
static void php_pdo_init_globals(zend_pdo_globals *pdo_globals)
{
	pdo_globals->global_value = 0;
}
/* }}} */

PDO_API int php_pdo_register_driver(pdo_driver_t *driver)
{
	if (driver->api_version != PDO_DRIVER_API) {
		zend_error(E_ERROR, "failed api version check");
		return FAILURE;
	}
	if (!zend_hash_exists(&module_registry, "pdo", sizeof("pdo"))) {
		zend_error(E_ERROR, "You MUST load PDO before loading any PDO drivers");
		return FAILURE;	/* NOTREACHED */
	}

	return zend_hash_add(&pdo_driver_hash, (char*)driver->driver_name, driver->driver_name_len,
			(void**)&driver, sizeof(driver), NULL);
}

PDO_API void php_pdo_unregister_driver(pdo_driver_t *driver)
{
	if (!zend_hash_exists(&module_registry, "pdo", sizeof("pdo"))) {
		return;
	}

	zend_hash_del(&pdo_driver_hash, (char*)driver->driver_name, driver->driver_name_len);
}

pdo_driver_t *pdo_find_driver(const char *name, int namelen)
{
	pdo_driver_t **driver = NULL;
	
	zend_hash_find(&pdo_driver_hash, (char*)name, namelen, (void**)&driver);

	return driver ? *driver : NULL;
}

PDO_API int php_pdo_parse_data_source(const char *data_source,
		unsigned long data_source_len, struct pdo_data_src_parser *parsed,
		int nparams)
{
	int i, j;
	int valstart = -1;
	int semi = -1;
	int optstart = 0;
	int nlen;
	int n_matches = 0;

	i = 0;
	while (i < data_source_len) {
		/* looking for NAME= */

		if (data_source[i] == '\0') {
			break;
		}

		if (data_source[i] != '=') {
			++i;
			continue;
		}

		valstart = ++i;

		/* now we're looking for VALUE; or just VALUE<NUL> */
		semi = -1;
		while (i < data_source_len) {
			if (data_source[i] == '\0') {
				semi = i++;
				break;
			}
			if (data_source[i] == ';') {
				semi = i++;
				break;
			}
			++i;
		}

		if (semi == -1) {
			semi = i;
		}

		/* find the entry in the array */
		nlen = valstart - optstart - 1;
		for (j = 0; j < nparams; j++) {
			if (0 == strncmp(data_source + optstart, parsed[j].optname, nlen) && parsed[j].optname[nlen] == '\0') {
				/* got a match */
				if (parsed[j].freeme) {
					efree(parsed[j].optval);
				}
				parsed[j].optval = estrndup(data_source + valstart, semi - valstart);
				parsed[j].freeme = 1;
				++n_matches;
				break;
			}
		}

		while (i < data_source_len && isspace(data_source[i])) {
			i++;
		}

		optstart = i;
	}

	return n_matches;
}
	

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo)
{
	zend_class_entry ce;

	ZEND_INIT_MODULE_GLOBALS(pdo, php_pdo_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	zend_hash_init(&pdo_driver_hash, 0, NULL, NULL, 1);

	REGISTER_LONG_CONSTANT("PDO_PARAM_NULL", (long)PDO_PARAM_NULL,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_PARAM_INT",  (long)PDO_PARAM_INT,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_PARAM_STR",  (long)PDO_PARAM_STR,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_PARAM_LOB",  (long)PDO_PARAM_LOB,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_PARAM_STMT", (long)PDO_PARAM_STMT,	CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PDO_FETCH_LAZY", (long)PDO_FETCH_LAZY,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_FETCH_ASSOC",(long)PDO_FETCH_ASSOC,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_FETCH_NUM",  (long)PDO_FETCH_NUM,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_FETCH_BOTH", (long)PDO_FETCH_BOTH,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_FETCH_OBJ",  (long)PDO_FETCH_OBJ,	CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PDO_ATTR_AUTOCOMMIT",	(long)PDO_ATTR_AUTOCOMMIT,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_ATTR_SCROLL", 		(long)PDO_ATTR_SCROLL,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PDO_ATTR_TIMEOUT", 		(long)PDO_ATTR_TIMEOUT,		CONST_CS|CONST_PERSISTENT);

	INIT_CLASS_ENTRY(ce, "PDOException", NULL);
	pdo_exception_ce = zend_register_internal_class_ex(&ce, zend_exception_get_default(), NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "PDO", pdo_dbh_functions);
	ce.create_object = pdo_dbh_new;
	pdo_dbh_ce = zend_register_internal_class(&ce TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "PDOStatement", pdo_dbstmt_functions);
	ce.create_object = pdo_dbstmt_new;
	pdo_dbstmt_ce = zend_register_internal_class(&ce TSRMLS_CC);


	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo)
{
	UNREGISTER_INI_ENTRIES();
	zend_hash_destroy(&pdo_driver_hash);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(pdo)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(pdo)
{
	/* TODO: visit persistent handles: for each persistent statement handle,
	 * remove bound parameter associations */
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdo)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pdo support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
