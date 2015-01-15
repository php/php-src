/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_PDO_H
#define PHP_PDO_H

#include "zend.h"

extern zend_module_entry pdo_module_entry;
#define phpext_pdo_ptr &pdo_module_entry

#ifdef PHP_WIN32
#	if defined(PDO_EXPORTS) || (!defined(COMPILE_DL_PDO))
#		define PDO_API __declspec(dllexport)
#	elif defined(COMPILE_DL_PDO)
#		define PDO_API __declspec(dllimport)
#	else
#		define PDO_API /* nothing special */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PDO_API __attribute__ ((visibility("default")))
#else
#	define PDO_API /* nothing special */
#endif

#ifdef ZTS
# include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo);
PHP_MSHUTDOWN_FUNCTION(pdo);
PHP_MINFO_FUNCTION(pdo);

ZEND_BEGIN_MODULE_GLOBALS(pdo)
	zend_long  global_value;
ZEND_END_MODULE_GLOBALS(pdo)

#ifdef ZTS
# define PDOG(v) TSRMG(pdo_globals_id, zend_pdo_globals *, v)
#else
# define PDOG(v) (pdo_globals.v)
#endif

#define REGISTER_PDO_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(php_pdo_get_dbh_ce(), const_name, sizeof(const_name)-1, (zend_long)value);

#define REGISTER_PDO_CONST_LONG(const_name, value) { \
	zend_class_entry **pce;	\
	if (zend_hash_find(CG(class_table), "pdo", sizeof("pdo"), (void **) &pce) != FAILURE)	\
		zend_declare_class_constant_long(*pce, const_name, sizeof(const_name)-1, (zend_long)value);	\
}	\

#define REGISTER_PDO_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(php_pdo_get_dbh_ce(), const_name, sizeof(const_name)-1, value, sizeof(value)-1);

#define PDO_CONSTRUCT_CHECK	\
	if (!dbh->driver) {	\
		pdo_raise_impl_error(dbh, NULL, "00000", "PDO constructor was not called");	\
		return;	\
	}	\


#endif	/* PHP_PDO_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
