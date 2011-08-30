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

zend_class_entry *yaf_view_interface_ce;

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_view_assign_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_view_display_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_view_render_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_view_setpath_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, template_dir)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_view_getpath_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ yaf_view_interface_methods 
 */
zend_function_entry yaf_view_interface_methods[] = {
	ZEND_ABSTRACT_ME(yaf_view, assign,  yaf_view_assign_arginfo)
	ZEND_ABSTRACT_ME(yaf_view, display, yaf_view_display_arginfo)
	ZEND_ABSTRACT_ME(yaf_view, render, yaf_view_render_arginfo)
	ZEND_ABSTRACT_ME(yaf_view, setScriptPath, yaf_view_setpath_arginfo)
	ZEND_ABSTRACT_ME(yaf_view, getScriptPath, yaf_view_getpath_arginfo)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION 
 */
YAF_STARTUP_FUNCTION(view_interface) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_View_Interface", "Yaf\\View_Interface", yaf_view_interface_methods);
	yaf_view_interface_ce = zend_register_internal_interface(&ce TSRMLS_CC);

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

