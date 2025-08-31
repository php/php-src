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
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_H
#define PHP_PDO_H

#include "zend.h"
#include "Zend/zend_compile.h"
#include "Zend/zend_API.h"
#include "Zend/zend_attributes.h"

PHPAPI extern zend_module_entry pdo_module_entry;
#define phpext_pdo_ptr &pdo_module_entry

PHPAPI extern zend_class_entry *pdo_dbh_ce;
PHPAPI extern zend_object *pdo_dbh_new(zend_class_entry *ce);

#include "php_version.h"
#define PHP_PDO_VERSION PHP_VERSION

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

static inline void pdo_declare_deprecated_class_constant_long(
		zend_class_entry *ce, const char *name, zend_long value,
		zend_string *since, const char *message) {

	zval zvalue;
	ZVAL_LONG(&zvalue, value);

	zend_string *name_str = zend_string_init_interned(name, strlen(name), true);

	zend_class_constant *constant = zend_declare_class_constant_ex(
			ce, name_str, &zvalue,
			ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED, NULL);

	zend_attribute *attr = zend_add_class_constant_attribute(ce, constant,
			ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED),
			1 + (message != NULL));

	attr->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	ZVAL_STR(&attr->args[0].value, since);

	if (message) {
		zend_string *message_str = zend_string_init_interned(message, strlen(message), true);

		attr->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);
		ZVAL_STR(&attr->args[1].value, message_str);
	}
}

/* Declare a constant deprecated in 8.5 */
#define REGISTER_PDO_CLASS_CONST_LONG_DEPRECATED_85(const_name, value) 													\
	pdo_declare_deprecated_class_constant_long(php_pdo_get_dbh_ce(), \
			const_name, (zend_long)value, \
			ZSTR_KNOWN(ZEND_STR_8_DOT_5), NULL)

/* Declare one of the constants deprecated in https://wiki.php.net/rfc/deprecations_php_8_5
 * "Deprecate driver specific PDO constants and methods" */
#define REGISTER_PDO_CLASS_CONST_LONG_DEPRECATED_ALIAS_85(name, old_prefix, new_prefix, value) \
	pdo_declare_deprecated_class_constant_long(php_pdo_get_dbh_ce(), \
			old_prefix name, (zend_long)value, \
			ZSTR_KNOWN(ZEND_STR_8_DOT_5), \
			"use " new_prefix name " instead")

#define LONG_CONST(c) (zend_long) c

#define PDO_CONSTRUCT_CHECK_COND dbh->driver
#define PDO_CONSTRUCT_CHECK_FAIL() \
		{ \
			zend_throw_error(NULL, "%s object is uninitialized", ZSTR_VAL(Z_OBJ(EX(This))->ce->name)); \
		} \

#define PDO_CONSTRUCT_CHECK \
	if (!(PDO_CONSTRUCT_CHECK_COND)) { \
		PDO_CONSTRUCT_CHECK_FAIL(); \
		RETURN_THROWS(); \
	} \


#define PDO_CONSTRUCT_CHECK_WITH_CLEANUP(cleanup) \
	if (!(PDO_CONSTRUCT_CHECK_COND)) { \
		PDO_CONSTRUCT_CHECK_FAIL(); \
		goto cleanup; \
	} \


#endif	/* PHP_PDO_H */
