/*
   +----------------------------------------------------------------------+
   | Yet Another Framework                                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Xinchen Hui  <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/
  
/* $Id$  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_registry.h"

zend_class_entry *yaf_registry_ce;

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_registry_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_registry_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_registry_del_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_registry_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ yaf_registry_t *yaf_registry_instance(yaf_registry_t *this_ptr TSRMLS_DC)
*/
yaf_registry_t *yaf_registry_instance(yaf_registry_t *this_ptr TSRMLS_DC) {
	yaf_registry_t *instance = zend_read_static_property(yaf_registry_ce, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_INSTANCE), 1 TSRMLS_CC);
	
	if (Z_TYPE_P(instance) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(instance), yaf_registry_ce TSRMLS_CC)) {
		zval *regs;

		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, yaf_registry_ce);

		MAKE_STD_ZVAL(regs);
		array_init(regs);
		zend_update_property(yaf_registry_ce, instance, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_ENTRYS), regs TSRMLS_CC);
		zend_update_static_property(yaf_registry_ce, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_INSTANCE), instance TSRMLS_CC);
		zval_ptr_dtor(&regs);
		zval_ptr_dtor(&instance);
	}

	return instance;
}
/* }}} */

/** {{{ int yaf_registry_is_set(char *name, int len TSRMLS_DC)
 */
int yaf_registry_is_set(char *name, int len TSRMLS_DC) {
	yaf_registry_t 	*registry;
	zval 			*entrys;

	registry = yaf_registry_instance(NULL TSRMLS_CC);
	entrys	 = zend_read_property(yaf_registry_ce, registry, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_ENTRYS), 1 TSRMLS_CC);
	return zend_hash_exists(Z_ARRVAL_P(entrys), name, len + 1);
}
/* }}} */

/** {{{ proto private Yaf_Registry::__construct(void)
*/
PHP_METHOD(yaf_registry, __construct) {
}
/* }}} */

/** {{{ proto private Yaf_Registry::__clone(void)
*/
PHP_METHOD(yaf_registry, __clone) {
}
/* }}} */

/** {{{ proto public static Yaf_Registry::get($name)
*/
PHP_METHOD(yaf_registry, get) {
	char *name;
	uint  len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {  
		zval **ppzval, *entrys;
		yaf_registry_t 	*registry;

		registry = yaf_registry_instance(NULL TSRMLS_CC);
		entrys	 = zend_read_property(yaf_registry_ce, registry, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_ENTRYS), 1 TSRMLS_CC);

		if (entrys && Z_TYPE_P(entrys) == IS_ARRAY) {
			if (zend_hash_find(Z_ARRVAL_P(entrys), name, len + 1, (void **) &ppzval) == SUCCESS) {
				RETURN_ZVAL(*ppzval, 1, 0);
			}
		}
	} 

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public staitc Yaf_Registry::set($name, $value)
*/
PHP_METHOD(yaf_registry, set) {
	zval *value;
	char *name;
	uint len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &value) == FAILURE) {
		return;
	} else {
		yaf_registry_t	*registry;
		zval			*entrys;

		registry = yaf_registry_instance(NULL TSRMLS_CC);
		entrys 	 = zend_read_property(yaf_registry_ce, registry, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_ENTRYS), 1 TSRMLS_CC);

		Z_ADDREF_P(value);
		if (zend_hash_update(Z_ARRVAL_P(entrys), name, len + 1, &value, sizeof(zval *), NULL) == SUCCESS) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public staitc Yaf_Registry::del($name)
*/
PHP_METHOD(yaf_registry, del) {
	char *name;
	uint len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		yaf_registry_t	*registry;
		zval *entrys;

		registry = yaf_registry_instance(NULL TSRMLS_CC);
		entrys 	 = zend_read_property(yaf_registry_ce, registry, ZEND_STRL(YAF_REGISTRY_PROPERTY_NAME_ENTRYS), 1 TSRMLS_CC);

		zend_hash_del(Z_ARRVAL_P(entrys), name, len + 1);
	}

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Registry::has($name)
*/
PHP_METHOD(yaf_registry, has) {
	char *name;
	uint len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_registry_is_set(name, len TSRMLS_CC)); 
	}
}
/* }}} */

/** {{{ proto public Yaf_Registry::getInstance(void)
*/
PHP_METHOD(yaf_registry, getInstance) {
	yaf_registry_t *registry = yaf_registry_instance(NULL TSRMLS_CC);
	RETURN_ZVAL(registry, 1, 0);
}
/* }}} */

/** {{{ yaf_registry_methods 
*/
zend_function_entry yaf_registry_methods[] = {
	PHP_ME(yaf_registry, __construct, 	NULL, ZEND_ACC_CTOR|ZEND_ACC_PRIVATE)
	PHP_ME(yaf_registry, __clone, 		NULL, ZEND_ACC_CLONE|ZEND_ACC_PRIVATE)
	PHP_ME(yaf_registry, get, yaf_registry_get_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_registry, has, yaf_registry_has_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_registry, set, yaf_registry_set_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_registry, del, yaf_registry_del_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(registry) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Registry", "Yaf\\Registry", yaf_registry_methods);

	yaf_registry_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_registry_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_registry_ce, YAF_STRL(YAF_REGISTRY_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(yaf_registry_ce, YAF_STRL(YAF_REGISTRY_PROPERTY_NAME_ENTRYS),  ZEND_ACC_PROTECTED TSRMLS_CC);
	
	return SUCCESS;
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
