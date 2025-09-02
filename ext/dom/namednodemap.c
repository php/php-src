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
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "obj_map.h"
#include "zend_interfaces.h"

/*
* class DOMNamedNodeMap
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1780488922
* Since:
*/

zend_long php_dom_get_namednodemap_length(dom_object *obj)
{
	dom_nnodemap_object *objmap = obj->ptr;
	return objmap->handler->length(objmap);
}

/* {{{ length	int
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6D0FB19E
Since:
*/
zend_result dom_namednodemap_length_read(dom_object *obj, zval *retval)
{
	ZVAL_LONG(retval, php_dom_get_namednodemap_length(obj));
	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1074577549
Since:
*/
PHP_METHOD(DOMNamedNodeMap, getNamedItem)
{
	zend_string *named;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &named) == FAILURE) {
		RETURN_THROWS();
	}

	dom_nnodemap_object *objmap = Z_DOMOBJ_P(ZEND_THIS)->ptr;
	php_dom_obj_map_get_ns_named_item_into_zval(objmap, named, NULL, return_value);
}
/* }}} end dom_namednodemap_get_named_item */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-349467F9
Since:
*/
PHP_METHOD(DOMNamedNodeMap, item)
{
	zend_long index;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	if (index < 0 || ZEND_LONG_INT_OVFL(index)) {
		zend_argument_value_error(1, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	}

	dom_object *intern = Z_DOMOBJ_P(ZEND_THIS);
	dom_nnodemap_object *objmap = intern->ptr;
	objmap->handler->get_item(objmap, index, return_value);
}
/* }}} end dom_namednodemap_item */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getNamedItemNS
Since: DOM Level 2
*/
PHP_METHOD(DOMNamedNodeMap, getNamedItemNS)
{
	size_t urilen=0;
	dom_object *intern;
	char *uri;
	zend_string *named;

	dom_nnodemap_object *objmap;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!S", &uri, &urilen, &named) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);

	objmap = (dom_nnodemap_object *)intern->ptr;

	if (objmap != NULL) {
		php_dom_obj_map_get_ns_named_item_into_zval(objmap, named, uri, return_value);
	}
}
/* }}} end dom_namednodemap_get_named_item_ns */

/* {{{ */
PHP_METHOD(DOMNamedNodeMap, count)
{
	ZEND_PARSE_PARAMETERS_NONE();
	dom_object *intern = Z_DOMOBJ_P(ZEND_THIS);
	RETURN_LONG(php_dom_get_namednodemap_length(intern));
}
/* }}} end dom_namednodemap_count */

PHP_METHOD(DOMNamedNodeMap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

#endif
