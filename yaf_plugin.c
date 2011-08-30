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
#include "yaf_plugin.h"

zend_class_entry * yaf_plugin_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(plugin_arg, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, request, Yaf_Request_Abstract, 0)
	ZEND_ARG_OBJ_INFO(0, response, Yaf_Response_Abstract, 0)
ZEND_END_ARG_INFO()

#ifdef YAF_HAVE_NAMESPACE
ZEND_BEGIN_ARG_INFO_EX(plugin_arg_ns, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, request, Yaf\\Request_Abstract, 0)
	ZEND_ARG_OBJ_INFO(0, response, Yaf\\Response_Abstract, 0)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/** {{{ proto public Yaf_Plugin::routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, routerStartup) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::routerShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, routerShutdown) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::dispatchLoopStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, dispatchLoopStartup) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::preDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, preDispatch) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::postDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, postDispatch) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::dispatchLoopShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, dispatchLoopShutdown) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Yaf_Plugin::preResponse(Yaf_Request_Abstract $request, Yaf_Response_Abstarct $response)
*/
PHP_METHOD(yaf_plugin, preResponse) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ yaf_plugin_methods 
*/
zend_function_entry yaf_plugin_methods[] = {
	PHP_ME(yaf_plugin, routerStartup,  		 plugin_arg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, routerShutdown,  		 plugin_arg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopStartup,   plugin_arg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopShutdown,  plugin_arg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preDispatch,  		 plugin_arg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, postDispatch, 		 plugin_arg, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preResponse, 			 plugin_arg, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

#ifdef YAF_HAVE_NAMESPACE
zend_function_entry yaf_plugin_methods_ns[] = {
	PHP_ME(yaf_plugin, routerStartup,  		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, routerShutdown,  		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopStartup,   plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, dispatchLoopShutdown,  plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preDispatch,  		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, postDispatch, 		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_plugin, preResponse, 			 plugin_arg_ns, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
#endif
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION 
*/
YAF_STARTUP_FUNCTION(plugin) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Plugin_Abstract", "Yaf\\Plugin\\Abstract", namespace_switch(yaf_plugin_methods));
	yaf_plugin_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_plugin_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

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
