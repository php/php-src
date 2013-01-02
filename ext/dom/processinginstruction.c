/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_processinginstruction_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMProcessingInstruction extends DOMNode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1004215813
* Since: 
*/

const zend_function_entry php_dom_processinginstruction_class_functions[] = {
	PHP_ME(domprocessinginstruction, __construct, arginfo_dom_processinginstruction_construct, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ proto void DOMProcessingInstruction::__construct(string name, [string value]); */
PHP_METHOD(domprocessinginstruction, __construct)
{

	zval *id;
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	int name_len, value_len, name_valid;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, dom_domexception_class_entry, &error_handling TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|s", &id, dom_processinginstruction_class_entry, &name, &name_len, &value, &value_len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

	nodep = xmlNewPI((xmlChar *) name, (xmlChar *) value);

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
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern TSRMLS_CC);
	}
}
/* }}} end DOMProcessingInstruction::__construct */

/* {{{ target	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1478689192
Since: 
*/
int dom_processinginstruction_target_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, (char *) (nodep->name), 1);

	return SUCCESS;
}

/* }}} */

/* {{{ data	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-837822393
Since: 
*/
int dom_processinginstruction_data_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;
	xmlChar *content;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	
	if ((content = xmlNodeGetContent(nodep)) != NULL) {
		ZVAL_STRING(*retval, content, 1);
		xmlFree(content);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}

	return SUCCESS;
}

int dom_processinginstruction_data_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	zval value_copy;
	xmlNode *nodep;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	if (newval->type != IS_STRING) {
		if(Z_REFCOUNT_P(newval) > 1) {
			value_copy = *newval;
			zval_copy_ctor(&value_copy);
			newval = &value_copy;
		}
		convert_to_string(newval);
	}

	xmlNodeSetContentLen(nodep, Z_STRVAL_P(newval), Z_STRLEN_P(newval) + 1);

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
