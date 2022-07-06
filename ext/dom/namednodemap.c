/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "zend_interfaces.h"

/*
* class DOMNamedNodeMap
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1780488922
* Since:
*/

/* {{{ length	int
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6D0FB19E
Since:
*/
int dom_namednodemap_length_read(dom_object *obj, zval *retval)
{
	dom_nnodemap_object *objmap;
	xmlAttrPtr curnode;
	xmlNodePtr nodep;
	int count = 0;

	objmap = (dom_nnodemap_object *)obj->ptr;

	if (objmap != NULL) {
		if ((objmap->nodetype == XML_NOTATION_NODE) ||
			objmap->nodetype == XML_ENTITY_NODE) {
			if (objmap->ht) {
				count = xmlHashSize(objmap->ht);
			}
		} else {
			nodep = dom_object_get_node(objmap->baseobj);
			if (nodep) {
				curnode = nodep->properties;
				if (curnode) {
					count++;
					while (curnode->next != NULL) {
						count++;
						curnode = curnode->next;
					}
				}
			}
		}
	}

	ZVAL_LONG(retval, count);
	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1074577549
Since:
*/
PHP_METHOD(DOMNamedNodeMap, getNamedItem)
{
	zval *id;
	int ret;
	size_t namedlen=0;
	dom_object *intern;
	xmlNodePtr itemnode = NULL;
	char *named;

	dom_nnodemap_object *objmap;
	xmlNodePtr nodep;
	xmlNotation *notep = NULL;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &named, &namedlen) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(id);

	objmap = (dom_nnodemap_object *)intern->ptr;

	if (objmap != NULL) {
		if ((objmap->nodetype == XML_NOTATION_NODE) ||
			objmap->nodetype == XML_ENTITY_NODE) {
			if (objmap->ht) {
				if (objmap->nodetype == XML_ENTITY_NODE) {
					itemnode = (xmlNodePtr)xmlHashLookup(objmap->ht, (xmlChar *) named);
				} else {
					notep = (xmlNotation *)xmlHashLookup(objmap->ht, (xmlChar *) named);
					if (notep) {
						itemnode = create_notation(notep->name, notep->PublicID, notep->SystemID);
					}
				}
			}
		} else {
			nodep = dom_object_get_node(objmap->baseobj);
			if (nodep) {
				itemnode = (xmlNodePtr)xmlHasProp(nodep, (xmlChar *) named);
			}
		}
	}

	if (itemnode) {
		DOM_RET_OBJ(itemnode, &ret, objmap->baseobj);
		return;
	} else {
		RETVAL_NULL();
	}
}
/* }}} end dom_namednodemap_get_named_item */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-349467F9
Since:
*/
PHP_METHOD(DOMNamedNodeMap, item)
{
	zval *id;
	zend_long index;
	int ret;
	dom_object *intern;
	xmlNodePtr itemnode = NULL;

	dom_nnodemap_object *objmap;
	xmlNodePtr nodep, curnode;
	int count;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
		RETURN_THROWS();
	}
	if (index < 0 || ZEND_LONG_INT_OVFL(index)) {
		zend_argument_value_error(1, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(id);

	objmap = (dom_nnodemap_object *)intern->ptr;

	if (objmap != NULL) {
		if ((objmap->nodetype == XML_NOTATION_NODE) ||
			objmap->nodetype == XML_ENTITY_NODE) {
			if (objmap->ht) {
				if (objmap->nodetype == XML_ENTITY_NODE) {
					itemnode = php_dom_libxml_hash_iter(objmap->ht, index);
				} else {
					itemnode = php_dom_libxml_notation_iter(objmap->ht, index);
				}
			}
		} else {
			nodep = dom_object_get_node(objmap->baseobj);
			if (nodep) {
				curnode = (xmlNodePtr)nodep->properties;
				count = 0;
				while (count < index && curnode != NULL) {
					count++;
					curnode = (xmlNodePtr)curnode->next;
				}
				itemnode = curnode;
			}
		}
	}

	if (itemnode) {
		DOM_RET_OBJ(itemnode, &ret, objmap->baseobj);
		return;
	}

	RETVAL_NULL();
}
/* }}} end dom_namednodemap_item */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getNamedItemNS
Since: DOM Level 2
*/
PHP_METHOD(DOMNamedNodeMap, getNamedItemNS)
{
	zval *id;
	int ret;
	size_t namedlen=0, urilen=0;
	dom_object *intern;
	xmlNodePtr itemnode = NULL;
	char *uri, *named;

	dom_nnodemap_object *objmap;
	xmlNodePtr nodep;
	xmlNotation *notep = NULL;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &urilen, &named, &namedlen) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(id);

	objmap = (dom_nnodemap_object *)intern->ptr;

	if (objmap != NULL) {
		if ((objmap->nodetype == XML_NOTATION_NODE) ||
			objmap->nodetype == XML_ENTITY_NODE) {
			if (objmap->ht) {
				if (objmap->nodetype == XML_ENTITY_NODE) {
					itemnode = (xmlNodePtr)xmlHashLookup(objmap->ht, (xmlChar *) named);
				} else {
					notep = (xmlNotation *)xmlHashLookup(objmap->ht, (xmlChar *) named);
					if (notep) {
						itemnode = create_notation(notep->name, notep->PublicID, notep->SystemID);
					}
				}
			}
		} else {
			nodep = dom_object_get_node(objmap->baseobj);
			if (nodep) {
				itemnode = (xmlNodePtr)xmlHasNsProp(nodep, (xmlChar *) named, (xmlChar *) uri);
			}
		}
	}

	if (itemnode) {
		DOM_RET_OBJ(itemnode, &ret, objmap->baseobj);
		return;
	} else {
		RETVAL_NULL();
	}
}
/* }}} end dom_namednodemap_get_named_item_ns */

/* {{{ */
PHP_METHOD(DOMNamedNodeMap, count)
{
	zval *id;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(id);
	if(dom_namednodemap_length_read(intern, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} end dom_namednodemap_count */

PHP_METHOD(DOMNamedNodeMap, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

#endif
