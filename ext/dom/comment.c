/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_comment_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMComment extends DOMCharacterData 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1728279322
* Since: 
*/

const zend_function_entry php_dom_comment_class_functions[] = {
	PHP_ME(domcomment, __construct, arginfo_dom_comment_construct, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void DOMComment::__construct([string value]); */
PHP_METHOD(domcomment, __construct)
{

	zval *id;
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *value = NULL;
	int value_len;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, dom_domexception_class_entry, &error_handling TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|s", &id, dom_comment_class_entry, &value, &value_len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
	nodep = xmlNewComment((xmlChar *) value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldnode = dom_object_get_node(intern);
		if (oldnode != NULL) {
			php_libxml_node_free_resource(oldnode  TSRMLS_CC);
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)nodep, (void *)intern TSRMLS_CC);
	}
}
/* }}} end DOMComment::__construct */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
