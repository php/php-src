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
#include "main/SAPI.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_alloc.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "zend_objects.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"

zend_class_entry *yaf_ce_RuntimeException;
zend_class_entry *yaf_exception_ce;

static zend_class_entry * yaf_buildin_exceptions[YAF_MAX_BUILDIN_EXCEPTION];

/** {{{void yaf_trigger_error(int type TSRMLS_DC, char *format, ...)
 */
void yaf_trigger_error(int type TSRMLS_DC, char *format, ...) {
	va_list args;

	if (YAF_G(throw_exception)) {
		char *message;
		va_start(args, format);
		vspprintf(&message, 0, format, args); 
		va_end(args);    
		yaf_throw_exception(type, message TSRMLS_CC);
		efree(message); 
	} else { 
		va_start(args, format);
		php_verror(NULL, "", E_ERROR, format, args TSRMLS_CC);
		va_end(args);
	} 
}
/* }}} */

/** {{{ zend_class_entry * yaf_get_exception_base(int root TSRMLS_DC) 
*/
zend_class_entry * yaf_get_exception_base(int root TSRMLS_DC) {
#if can_handle_soft_dependency_on_SPL && defined(HAVE_SPL) && ((PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1))
	if (!root) {
		if (!yaf_ce_RuntimeException) {
			zend_class_entry **pce;

			if (zend_hash_find(CG(class_table), "runtimeexception", sizeof("RuntimeException"), (void **) &pce) == SUCCESS) {
				yaf_ce_RuntimeException = *pce;
				return *pce;
			}
		} else {
			return yaf_ce_RuntimeException;
		}
	}
#endif

#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
	return zend_exception_get_default();
#else
	return zend_exception_get_default(TSRMLS_C);
#endif
}
/* }}} */

/** {{{ void yaf_throw_exception(long code, char *message TSRMLS_DC)
*/
void yaf_throw_exception(long code, char *message TSRMLS_DC) {
	zend_class_entry *base_exception = yaf_exception_ce;

	if ((code & YAF_ERR_BASE) == YAF_ERR_BASE
			&& yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(code)]) {
		base_exception  = yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(code)];
	}

	zend_throw_exception(base_exception, message, code TSRMLS_CC);
}
/* }}} */

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)) || (PHP_MAJOR_VERSION < 5)
/** {{{ proto Yaf_Exception::__construct($mesg = 0, $code = 0, Exception $previous = NULL) 
*/
PHP_METHOD(yaf_exception, __construct) {
	char  	*message = NULL;
	zval  	*object, *previous = NULL;
	int 	message_len, code = 0;
	int    	argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|slO!", &message, &message_len, &code, &previous, yaf_get_exception_base(0 TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Wrong parameters for Exception([string $exception [, long $code [, Exception $previous = NULL]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_string(Z_OBJCE_P(object), object, "message", sizeof("message")-1, message TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(Z_OBJCE_P(object), object, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	if (previous) {
		zend_update_property(Z_OBJCE_P(object), object, ZEND_STRL("previous"), previous TSRMLS_CC);
	}
}
/* }}} */

/** {{{ proto Yaf_Exception::getPrevious(void)
*/
PHP_METHOD(yaf_exception, getPrevious) {
	zval *prev = zend_read_property(Z_OBJCE_P(getThis()), getThis(), ZEND_STRL("previous"), 1 TSRMLS_CC);
	RETURN_ZVAL(prev, 1, 0);
}
/* }}} */
#endif

/** {{{ yaf_exception_methods
*/
zend_function_entry yaf_exception_methods[] = {
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)) || (PHP_MAJOR_VERSION < 5)
	PHP_ME(yaf_exception, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR) 
	PHP_ME(yaf_exception, getPrevious, NULL, ZEND_ACC_PUBLIC) 
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(exception) {
	zend_class_entry ce;
	zend_class_entry startup_ce;
	zend_class_entry route_ce;
	zend_class_entry dispatch_ce;
	zend_class_entry loader_ce;
	zend_class_entry module_notfound_ce;
	zend_class_entry controller_notfound_ce;
	zend_class_entry action_notfound_ce;
	zend_class_entry view_notfound_ce;
	zend_class_entry type_ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Exception", "Yaf\\Exception", yaf_exception_methods);
	yaf_exception_ce = zend_register_internal_class_ex(&ce, yaf_get_exception_base(0 TSRMLS_CC), NULL TSRMLS_CC);
	zend_declare_property_null(yaf_exception_ce, YAF_STRL("message"), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(yaf_exception_ce, YAF_STRL("code"), 0,	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_exception_ce, YAF_STRL("previous"),  ZEND_ACC_PROTECTED TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(startup_ce, "Yaf_Exception_StartupError", "Yaf\\Exception\\StartupError", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_STARTUP_FAILED)] = zend_register_internal_class_ex(&startup_ce, yaf_exception_ce, NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(route_ce, "Yaf_Exception_RouterFailed", "Yaf\\Exception\\RouterFailed", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_ROUTE_FAILED)] = zend_register_internal_class_ex(&route_ce, yaf_exception_ce, NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(dispatch_ce, "Yaf_Exception_DispatchFailed", "Yaf\\Exception\\DispatchFailed", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_DISPATCH_FAILED)] = zend_register_internal_class_ex(&dispatch_ce, yaf_exception_ce, NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(loader_ce, "Yaf_Exception_LoadFailed", "Yaf\\Exception\\LoadFailed", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_AUTOLOAD_FAILED)] = zend_register_internal_class_ex(&loader_ce, yaf_exception_ce, NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(module_notfound_ce, "Yaf_Exception_LoadFailed_Module", "Yaf\\Exception\\LoadFailed\\Module", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_MODULE)] = zend_register_internal_class_ex(&module_notfound_ce, yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_AUTOLOAD_FAILED)], NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(controller_notfound_ce, "Yaf_Exception_LoadFailed_Controller", "Yaf\\Exception\\LoadFailed\\Controller", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_CONTROLLER)] = zend_register_internal_class_ex(&controller_notfound_ce, yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_AUTOLOAD_FAILED)], NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(action_notfound_ce, "Yaf_Exception_LoadFailed_Action", "Yaf\\Exception\\LoadFailed\\Action", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_ACTION)] = zend_register_internal_class_ex(&action_notfound_ce, yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_AUTOLOAD_FAILED)], NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(view_notfound_ce, "Yaf_Exception_LoadFailed_View", "Yaf\\Exception\\LoadFailed\\View", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_NOTFOUND_VIEW)] = zend_register_internal_class_ex(&view_notfound_ce, yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_AUTOLOAD_FAILED)], NULL TSRMLS_CC);

	YAF_INIT_CLASS_ENTRY(type_ce, "Yaf_Exception_TypeError", "Yaf\\Exception\\TypeError", NULL);
	yaf_buildin_exceptions[YAF_EXCEPTION_OFFSET(YAF_ERR_TYPE_ERROR)] = zend_register_internal_class_ex(&type_ce, yaf_exception_ce, NULL TSRMLS_CC);

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
