/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ard Biesheuvel <abies@php.net>                               |
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
#include "php_pdo_firebird.h"
#include "php_pdo_firebird_int.h"

/* {{{ pdo_firebird_deps */
static const zend_module_dep pdo_firebird_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
/* }}} */

zend_module_entry pdo_firebird_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_firebird_deps,
	"PDO_Firebird",
	NULL,
	PHP_MINIT(pdo_firebird),
	PHP_MSHUTDOWN(pdo_firebird),
	NULL,
	NULL,
	PHP_MINFO(pdo_firebird),
	PHP_PDO_FIREBIRD_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_FIREBIRD
ZEND_GET_MODULE(pdo_firebird)
#endif

PHP_MINIT_FUNCTION(pdo_firebird) /* {{{ */
{
	REGISTER_PDO_CLASS_CONST_LONG("FB_ATTR_DATE_FORMAT", (zend_long) PDO_FB_ATTR_DATE_FORMAT);
	REGISTER_PDO_CLASS_CONST_LONG("FB_ATTR_TIME_FORMAT", (zend_long) PDO_FB_ATTR_TIME_FORMAT);
	REGISTER_PDO_CLASS_CONST_LONG("FB_ATTR_TIMESTAMP_FORMAT", (zend_long) PDO_FB_ATTR_TIMESTAMP_FORMAT);

	if (FAILURE == php_pdo_register_driver(&pdo_firebird_driver)) {
		return FAILURE;
	}

#ifdef ZEND_SIGNALS
	/* firebird replaces some signals at runtime, suppress warnings. */
	SIGG(check) = 0;
#endif

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(pdo_firebird) /* {{{ */
{
	php_pdo_unregister_driver(&pdo_firebird_driver);

	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(pdo_firebird) /* {{{ */
{
	char version[64];
	isc_get_client_version(version);

	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for Firebird", "enabled");
	php_info_print_table_row(2, "Client Library Version", version);
	php_info_print_table_end();
}
/* }}} */
