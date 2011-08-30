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

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_request.h"
#include "yaf_response.h"
#include "yaf_view.h"
#include "yaf_exception.h"
#include "yaf_controller.h"
#include "yaf_action.h"

zend_class_entry *yaf_action_ce;

/** {{{ ARG_INFO 
 */

/* }}} */

/** {{{ proto public Yaf_Action_Abstract::getController(void)
*/
PHP_METHOD(yaf_action, getController) {
	yaf_controller_t *controller = zend_read_property(yaf_action_ce, getThis(), ZEND_STRL(YAF_ACTION_PROPERTY_NAME_CTRL), 1 TSRMLS_CC);
	RETURN_ZVAL(controller, 1, 0);
}
/* }}} */

/** {{{ yaf_controller_methods 
*/
zend_function_entry yaf_action_methods[] = {
	PHP_ABSTRACT_ME(yaf_action_controller, execute, NULL)
	PHP_ME(yaf_action, getController, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(action) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Action_Abstract", "Yaf\\Action_Abstract", yaf_action_methods);
	yaf_action_ce = zend_register_internal_class_ex(&ce, yaf_controller_ce, NULL TSRMLS_CC);
	yaf_action_ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_action_ce, YAF_STRL(YAF_ACTION_PROPERTY_NAME_CTRL),	ZEND_ACC_PROTECTED TSRMLS_CC);

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
