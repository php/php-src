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
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"
#include "dom_arginfo.h"

/*
* class DOMNamedNodeMap
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1780488922
* Since:
*/

const zend_function_entry php_dom_namednodemap_class_functions[] = { /* {{{ */
	PHP_ME(domnamednodemap, getNamedItem, arginfo_class_DOMNamedNodeMap_getNamedItem, ZEND_ACC_PUBLIC)
	PHP_ME(domnamednodemap, item, arginfo_class_DOMNamedNodeMap_item, ZEND_ACC_PUBLIC)
	PHP_ME(domnamednodemap, getNamedItemNS, arginfo_class_DOMNamedNodeMap_getNamedItemNS, ZEND_ACC_PUBLIC)
	PHP_ME(domnamednodemap, count, arginfo_class_DOMNamedNodeMap_count, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

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

/* {{{ proto DOMNode dom_namednodemap_get_named_item(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1074577549
Since:
*/
PHP_METHOD(domnamednodemap, getNamedItem)
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
		return;
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

/* {{{ proto DOMNode dom_namednodemap_item(int index);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-349467F9
Since:
*/
PHP_METHOD(domnamednodemap, item)
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
		return;
	}
	if (index >= 0) {
		if (ZEND_LONG_INT_OVFL(index)) {
			php_error_docref(NULL, E_WARNING, "Invalid index");
			RETURN_NULL();
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
	}

	RETVAL_NULL();
}
/* }}} end dom_namednodemap_item */

/* {{{ proto DOMNode dom_namednodemap_get_named_item_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getNamedItemNS
Since: DOM Level 2
*/
PHP_METHOD(domnamednodemap, getNamedItemNS)
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
		return;
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

/* {{{ proto int|bool dom_namednodemap_count();
*/
PHP_METHOD(domnamednodemap, count)
{
	zval *id;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_DOMOBJ_P(id);
	if(dom_namednodemap_length_read(intern, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} end dom_namednodemap_count */

#endif
