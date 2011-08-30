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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_objects.h"
#include "main/SAPI.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_session.h"
#include "yaf_exception.h"

zend_class_entry * yaf_session_ce;
#ifdef HAVE_SPL
extern PHPAPI zend_class_entry * spl_ce_Countable;
#endif

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_session_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_del_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_session_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ inline int yaf_session_start(yaf_session_t *session TSRMLS_DC) 
 */
inline int yaf_session_start(yaf_session_t *session TSRMLS_DC) {
	zval *status;

	status = zend_read_property(yaf_session_ce, session, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_STATUS), 1 TSRMLS_CC);
	if (Z_BVAL_P(status)) {
		return 1;
	}

	php_session_start(TSRMLS_C);
	zend_update_property_bool(yaf_session_ce, session, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_STATUS), 1 TSRMLS_CC);
	return 1;
}
/* }}} */

/** {{{ static yaf_session_t * yaf_session_instance(TSRMLS_D)
*/
static yaf_session_t * yaf_session_instance(TSRMLS_D) {
	yaf_session_t *instance;
	zval **sess, *member;
	zend_object *obj; 
	zend_property_info *property_info;

	MAKE_STD_ZVAL(instance);
	object_init_ex(instance, yaf_session_ce);

	yaf_session_start(instance TSRMLS_CC);

	if (zend_hash_find(&EG(symbol_table), ZEND_STRS("_SESSION"), (void **)&sess) == FAILURE || Z_TYPE_PP(sess) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempt to start session failed");
		zval_ptr_dtor(&instance);
		return NULL;
	}

	MAKE_STD_ZVAL(member);
	ZVAL_STRING(member, YAF_SESSION_PROPERTY_NAME_SESSION, 0);

	obj = zend_objects_get_address(instance TSRMLS_CC);

	property_info = zend_get_property_info(obj->ce, member, 1 TSRMLS_CC);

	Z_ADDREF_P(*sess);
	/** This is ugly , because we can't set a ref property through the stadard APIs */
	zend_hash_quick_update(obj->properties, property_info->name,
			property_info->name_length+1, property_info->h, (void **)sess, sizeof(zval *), NULL);

	zend_update_static_property(yaf_session_ce, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_INSTANCE), instance TSRMLS_CC);

	efree(member);

	return instance;
}
/* }}} */

/** {{{ proto private Yaf_Session::__construct(void)
*/
PHP_METHOD(yaf_session, __construct) {
}
/* }}} */

/** {{{ proto private Yaf_Session::__destruct(void)
*/
PHP_METHOD(yaf_session, __destruct) {
}
/* }}} */

/** {{{ proto private Yaf_Session::__sleep(void)
*/
PHP_METHOD(yaf_session, __sleep) {
}
/* }}} */

/** {{{ proto private Yaf_Session::__wakeup(void)
*/
PHP_METHOD(yaf_session, __wakeup) {
}
/* }}} */

/** {{{ proto private Yaf_Session::__clone(void)
*/
PHP_METHOD(yaf_session, __clone) {
}
/* }}} */

/** {{{ proto public Yaf_Session::getInstance(void)
*/
PHP_METHOD(yaf_session, getInstance) {
	yaf_session_t *instance = zend_read_static_property(yaf_session_ce, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_INSTANCE), 1 TSRMLS_CC);
	
	if (Z_TYPE_P(instance) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(instance), yaf_session_ce TSRMLS_CC)) {
		if ((instance = yaf_session_instance(TSRMLS_C))) {
			RETURN_ZVAL(instance, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(instance, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Yaf_Session::count(void)
*/
PHP_METHOD(yaf_session, count) {
	zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(sess)));
}
/* }}} */

/** {{{ proto public static Yaf_Session::start()
*/
PHP_METHOD(yaf_session, start) {
	yaf_session_start(getThis() TSRMLS_CC);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public static Yaf_Session::get($name)
*/
PHP_METHOD(yaf_session, get) {
	char *name  = NULL;
	int  len	= 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &len) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {  
		zval **ret, *sess;

		sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
		if (!len) {
			RETURN_ZVAL(sess, 1, 0);
		}

		if (zend_hash_find(Z_ARRVAL_P(sess), name, len + 1, (void **)&ret) == FAILURE ){
			RETURN_NULL();
		}

		RETURN_ZVAL(*ret, 1, 0);
	} 
}
/* }}} */

/** {{{ proto public staitc Yaf_Session::set($name, $value)
*/
PHP_METHOD(yaf_session, set) {
	zval *value;
	char *name;
	uint len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &value) == FAILURE) {
		return;
	} else {
		zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
		Z_ADDREF_P(value);
		if (zend_hash_update(Z_ARRVAL_P(sess), name, len + 1, &value, sizeof(zval *), NULL) == FAILURE) {
			Z_DELREF_P(value);
			RETURN_FALSE;
		}
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public staitc Yaf_Session::del($name)
*/
PHP_METHOD(yaf_session, del) {
	char *name;
	uint len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);

		if (zend_hash_del(Z_ARRVAL_P(sess), name, len + 1) == SUCCESS) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Session::has($name)
*/
PHP_METHOD(yaf_session, has) {
	char *name;
	uint  len; 

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(sess), name, len + 1));
	}

}
/* }}} */

/** {{{ proto public Yaf_Session::rewind(void)
*/
PHP_METHOD(yaf_session, rewind) {
	zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(sess));
}
/* }}} */

/** {{{ proto public Yaf_Session::current(void)
*/
PHP_METHOD(yaf_session, current) {
	zval *sess, **ppzval;
	sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
	if (zend_hash_get_current_data(Z_ARRVAL_P(sess), (void **)&ppzval) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_ZVAL(*ppzval, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Session::key(void)
*/
PHP_METHOD(yaf_session, key) {
	zval *sess;
	char *key;
	long index;

	sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
	if (zend_hash_get_current_key(Z_ARRVAL_P(sess), &key, &index, 0) == HASH_KEY_IS_LONG) {
		RETURN_LONG(index);
	} else {
		RETURN_STRING(key, 1);
	}
}
/* }}} */

/** {{{ proto public Yaf_Session::next(void)
*/
PHP_METHOD(yaf_session, next) {
	zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
	zend_hash_move_forward(Z_ARRVAL_P(sess));
}
/* }}} */

/** {{{ proto public Yaf_Session::valid(void)
*/
PHP_METHOD(yaf_session, valid) {
	zval *sess = zend_read_property(yaf_session_ce, getThis(), ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION), 1 TSRMLS_CC);
	RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(sess)) == SUCCESS);
}
/* }}} */

/** {{{ yaf_session_methods 
*/
zend_function_entry yaf_session_methods[] = {
	PHP_ME(yaf_session, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PRIVATE)
	PHP_ME(yaf_session, __clone, NULL, ZEND_ACC_CLONE|ZEND_ACC_PRIVATE)
	PHP_ME(yaf_session, __sleep, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_session, __wakeup, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_session, getInstance, yaf_session_void_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_session, start, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, get, yaf_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, has, yaf_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, set, yaf_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, del, yaf_session_del_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, count, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, rewind, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, next, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, current, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, key, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_session, valid, yaf_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetGet, get, yaf_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetSet, set, yaf_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetExists, has, yaf_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, offsetUnset, del, yaf_session_del_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __get, get, yaf_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __isset, has, yaf_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __set, set, yaf_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(yaf_session, __unset, del, yaf_session_del_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(session) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Session", "Yaf\\Session", yaf_session_methods);

	yaf_session_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_session_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

#ifdef HAVE_SPL
	zend_class_implements(yaf_session_ce TSRMLS_CC, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#else
	zend_class_implements(yaf_session_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

	zend_declare_property_null(yaf_session_ce, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(yaf_session_ce, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_SESSION),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(yaf_session_ce, ZEND_STRL(YAF_SESSION_PROPERTY_NAME_STATUS),   0, ZEND_ACC_PROTECTED TSRMLS_CC);
	
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
