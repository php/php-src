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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"

/*
* class DOMProcessingInstruction extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1004215813
* Since:
*/

/* {{{ */
PHP_METHOD(DOMProcessingInstruction, __construct)
{
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL;
	size_t name_len, value_len;
	int name_valid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	name_valid = xmlValidateName(BAD_CAST name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, true);
		RETURN_THROWS();
	}

	nodep = xmlNewPI(BAD_CAST name, BAD_CAST value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMProcessingInstruction::__construct */

/* {{{ target	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1478689192
Since:
*/
zend_result dom_processinginstruction_target_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
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
zend_result dom_processinginstruction_data_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		return FAILURE;
	}

	php_dom_get_content_into_zval(nodep, retval, false);

	return SUCCESS;
}

zend_result dom_processinginstruction_data_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		return FAILURE;
	}

	/* Typed property, this is already a string */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING);
	zend_string *str = Z_STR_P(newval);

	xmlNodeSetContentLen(nodep, BAD_CAST ZSTR_VAL(str), ZSTR_LEN(str));

	return SUCCESS;
}

/* }}} */

#endif
