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
#include "nodelist.h"
#include "zend_interfaces.h"

/*
* class DOMNodeList
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-536297177
* Since:
*/

xmlNodePtr dom_nodelist_iter_start_first_child(xmlNodePtr nodep)
{
	if (nodep->type == XML_ENTITY_REF_NODE) {
		/* See entityreference.c */
		dom_entity_reference_fetch_and_sync_declaration(nodep);
	}

	return nodep->children;
}

/* {{{ length	int
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-203510337
Since:
*/
zend_result dom_nodelist_length_read(dom_object *obj, zval *retval)
{
	ZVAL_LONG(retval, php_dom_get_nodelist_length(obj));
	return SUCCESS;
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMNodeList, count)
{
	ZEND_PARSE_PARAMETERS_NONE();
	dom_object *intern = Z_DOMOBJ_P(ZEND_THIS);
	RETURN_LONG(php_dom_get_nodelist_length(intern));
}
/* }}} end dom_nodelist_count */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-844377136
Since:
*/
PHP_METHOD(DOMNodeList, item)
{
	zend_long index;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();

	zval *id = ZEND_THIS;
	dom_object *intern = Z_DOMOBJ_P(id);
	dom_nnodemap_object *objmap = intern->ptr;
	php_dom_obj_map_get_item_into_zval(objmap, index, return_value);
}
/* }}} end dom_nodelist_item */

ZEND_METHOD(DOMNodeList, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

dom_nodelist_dimension_index dom_modern_nodelist_get_index(const zval *offset)
{
	dom_nodelist_dimension_index ret;

	ZVAL_DEREF(offset);

	if (Z_TYPE_P(offset) == IS_LONG) {
		ret.type = DOM_NODELIST_DIM_LONG;
		ret.lval = Z_LVAL_P(offset);
	} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
		ret.type = DOM_NODELIST_DIM_LONG;
		ret.lval = zend_dval_to_lval_safe(Z_DVAL_P(offset));
	} else if (Z_TYPE_P(offset) == IS_STRING) {
		zend_ulong lval;
		if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), lval)) {
			ret.type = DOM_NODELIST_DIM_LONG;
			ret.lval = (zend_long) lval;
		} else {
			ret.type = DOM_NODELIST_DIM_STRING;
			ret.str = Z_STR_P(offset);
		}
	} else {
		ret.type = DOM_NODELIST_DIM_ILLEGAL;
	}

	return ret;
}

zval *dom_modern_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot append to %s", ZSTR_VAL(object->ce->name));
		return NULL;
	}

	dom_nodelist_dimension_index index = dom_modern_nodelist_get_index(offset);
	if (UNEXPECTED(index.type == DOM_NODELIST_DIM_ILLEGAL || index.type == DOM_NODELIST_DIM_STRING)) {
		zend_illegal_container_offset(object->ce->name, offset, type);
		return NULL;
	}

	php_dom_obj_map_get_item_into_zval(php_dom_obj_from_obj(object)->ptr, index.lval, rv);
	return rv;
}

int dom_modern_nodelist_has_dimension(zend_object *object, zval *member, int check_empty)
{
	/* If it exists, it cannot be empty because nodes aren't empty. */
	ZEND_IGNORE_VALUE(check_empty);

	dom_nodelist_dimension_index index = dom_modern_nodelist_get_index(member);
	if (UNEXPECTED(index.type == DOM_NODELIST_DIM_ILLEGAL || index.type == DOM_NODELIST_DIM_STRING)) {
		zend_illegal_container_offset(object->ce->name, member, BP_VAR_IS);
		return 0;
	}

	return index.lval >= 0 && index.lval < php_dom_get_nodelist_length(php_dom_obj_from_obj(object));
}

#endif
