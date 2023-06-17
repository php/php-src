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
#include "zend_interfaces.h"

/*
* class DOMNodeList
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-536297177
* Since:
*/

int php_dom_get_nodelist_length(dom_object *obj)
{
	dom_nnodemap_object *objmap = (dom_nnodemap_object *) obj->ptr;
	if (!objmap) {
		return 0;
	}

	if (objmap->ht) {
		return xmlHashSize(objmap->ht);
	}

	if (objmap->nodetype == DOM_NODESET) {
		HashTable *nodeht = HASH_OF(&objmap->baseobj_zv);
		return zend_hash_num_elements(nodeht);
	}

	xmlNodePtr nodep = dom_object_get_node(objmap->baseobj);
	if (!nodep) {
		return 0;
	}

	int count = 0;
	if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
		xmlNodePtr curnode = nodep->children;
		if (curnode) {
			count++;
			while (curnode->next != NULL) {
				count++;
				curnode = curnode->next;
			}
		}
	} else {
		if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
			nodep = xmlDocGetRootElement((xmlDoc *) nodep);
		} else {
			nodep = nodep->children;
		}
		dom_get_elements_by_tag_name_ns_raw(
			nodep, (char *) objmap->ns, (char *) objmap->local, &count, -1);
	}

	return count;
}

/* {{{ length	int
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-203510337
Since:
*/
int dom_nodelist_length_read(dom_object *obj, zval *retval)
{
	ZVAL_LONG(retval, php_dom_get_nodelist_length(obj));
	return SUCCESS;
}


/* {{{ */
PHP_METHOD(DOMNodeList, count)
{
	zval *id;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(id);
	RETURN_LONG(php_dom_get_nodelist_length(intern));
}
/* }}} end dom_nodelist_count */

void php_dom_nodelist_get_item_into_zval(dom_nnodemap_object *objmap, zend_long index, zval *return_value)
{
	int ret;
	xmlNodePtr itemnode = NULL;
	if (index >= 0) {
		if (objmap != NULL) {
			if (objmap->ht) {
				if (objmap->nodetype == XML_ENTITY_NODE) {
					itemnode = php_dom_libxml_hash_iter(objmap->ht, index);
				} else {
					itemnode = php_dom_libxml_notation_iter(objmap->ht, index);
				}
			} else {
				if (objmap->nodetype == DOM_NODESET) {
					HashTable *nodeht = HASH_OF(&objmap->baseobj_zv);
					zval *entry = zend_hash_index_find(nodeht, index);
					if (entry) {
						ZVAL_COPY(return_value, entry);
						return;
					}
				} else if (objmap->baseobj) {
					xmlNodePtr nodep = dom_object_get_node(objmap->baseobj);
					if (nodep) {
						int count = 0;
						if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
							xmlNodePtr curnode = nodep->children;
							while (count < index && curnode != NULL) {
								count++;
								curnode = curnode->next;
							}
							itemnode = curnode;
						} else {
							if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
								nodep = xmlDocGetRootElement((xmlDoc *) nodep);
							} else {
								nodep = nodep->children;
							}
							itemnode = dom_get_elements_by_tag_name_ns_raw(nodep, (char *) objmap->ns, (char *) objmap->local, &count, index);
						}
					}
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

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-844377136
Since:
*/
PHP_METHOD(DOMNodeList, item)
{
	zend_long index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
		RETURN_THROWS();
	}

	zval *id = ZEND_THIS;
	dom_object *intern = Z_DOMOBJ_P(id);
	dom_nnodemap_object *objmap = intern->ptr;
	php_dom_nodelist_get_item_into_zval(objmap, index, return_value);
}
/* }}} end dom_nodelist_item */

ZEND_METHOD(DOMNodeList, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

#endif
