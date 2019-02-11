/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
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
#include "php_pdo_oci.h"
#include "php_pdo_oci_int.h"
#ifdef ZTS
#include <TSRM/TSRM.h>
#endif

/* {{{ pdo_oci_functions[] */
static const zend_function_entry pdo_oci_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ pdo_oci_module_entry */

static const zend_module_dep pdo_oci_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};

zend_module_entry pdo_oci_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_oci_deps,
	"PDO_OCI",
	pdo_oci_functions,
	PHP_MINIT(pdo_oci),
	PHP_MSHUTDOWN(pdo_oci),
	PHP_RINIT(pdo_oci),
	NULL,
	PHP_MINFO(pdo_oci),
	PHP_PDO_OCI_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_OCI
ZEND_GET_MODULE(pdo_oci)
#endif

const ub4 PDO_OCI_INIT_MODE =
#if 0 && defined(OCI_SHARED)
			/* shared mode is known to be bad for PHP */
			OCI_SHARED
#else
			OCI_DEFAULT
#endif
#ifdef OCI_OBJECT
			|OCI_OBJECT
#endif
#ifdef ZTS
			|OCI_THREADED
#endif
			;

/* true global environment */
OCIEnv *pdo_oci_Env = NULL;

#ifdef ZTS
/* lock for pdo_oci_Env initialization */
static MUTEX_T pdo_oci_env_mutex;
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pdo_oci)
{
	REGISTER_PDO_CLASS_CONST_LONG("OCI_ATTR_ACTION", (zend_long)PDO_OCI_ATTR_ACTION);
	REGISTER_PDO_CLASS_CONST_LONG("OCI_ATTR_CLIENT_INFO", (zend_long)PDO_OCI_ATTR_CLIENT_INFO);
	REGISTER_PDO_CLASS_CONST_LONG("OCI_ATTR_CLIENT_IDENTIFIER", (zend_long)PDO_OCI_ATTR_CLIENT_IDENTIFIER);
	REGISTER_PDO_CLASS_CONST_LONG("OCI_ATTR_MODULE", (zend_long)PDO_OCI_ATTR_MODULE);

	php_pdo_register_driver(&pdo_oci_driver);

	// Defer OCI init to PHP_RINIT_FUNCTION because with php-fpm,
	// NLS_LANG is not yet available here.

#ifdef ZTS
	pdo_oci_env_mutex = tsrm_mutex_alloc();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pdo_oci)
{
	if (!pdo_oci_Env) {
#ifdef ZTS
		tsrm_mutex_lock(pdo_oci_env_mutex);
		if (!pdo_oci_Env) { // double-checked locking idiom
#endif
#if HAVE_OCIENVCREATE
		OCIEnvCreate(&pdo_oci_Env, PDO_OCI_INIT_MODE, NULL, NULL, NULL, NULL, 0, NULL);
#else
		OCIInitialize(PDO_OCI_INIT_MODE, NULL, NULL, NULL, NULL);
		OCIEnvInit(&pdo_oci_Env, OCI_DEFAULT, 0, NULL);
#endif
#ifdef ZTS
		}
		tsrm_mutex_unlock(pdo_oci_env_mutex);
#endif
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdo_oci)
{
	php_pdo_unregister_driver(&pdo_oci_driver);

	if (pdo_oci_Env) {
		OCIHandleFree((dvoid*)pdo_oci_Env, OCI_HTYPE_ENV);
	}

#ifdef ZTS
	tsrm_mutex_free(pdo_oci_env_mutex);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_oci)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for OCI 8 and later", "enabled");
	php_info_print_table_end();
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
