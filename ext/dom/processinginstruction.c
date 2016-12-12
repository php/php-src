/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
	zval *id = getThis();
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	size_t name_len, value_len;
	int name_valid;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1);
		RETURN_FALSE;
	}

	nodep = xmlNewPI((xmlChar *) name, (xmlChar *) value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_FALSE;
	}

	intern = Z_DOMOBJ_P(id);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_free_resource(oldnode );
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMProcessingInstruction::__construct */

/* {{{ target	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1478689192
Since:
*/
int dom_processinginstruction_target_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	ZVAL_STRING(retval, (char *) (nodep->name));

	return SUCCESS;
}

/* }}} */

/* {{{ data	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-837822393
Since:
*/
int dom_processinginstruction_data_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep;
	xmlChar *content;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if ((content = xmlNodeGetContent(nodep)) != NULL) {
		ZVAL_STRING(retval, (char *) content);
		xmlFree(content);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

int dom_processinginstruction_data_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	zend_string *str;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	str = zval_get_string(newval);

	xmlNodeSetContentLen(nodep, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str) + 1);

	zend_string_release(str);
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
