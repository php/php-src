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
#include "dom_ce.h"

typedef struct _nodeIterator nodeIterator;
struct _nodeIterator {
	int cur;
	int index;
	xmlNode *node;
};

typedef struct _notationIterator notationIterator;
struct _notationIterator {
	int cur;
	int index;
	xmlNotation *notation;
};

static void itemHashScanner (void *payload, void *data, xmlChar *name) /* {{{ */
{
	nodeIterator *priv = (nodeIterator *)data;

	if(priv->cur < priv->index) {
		priv->cur++;
	} else {
		if(priv->node == NULL) {
			priv->node = (xmlNode *)payload;
		}
	}
}
/* }}} */

xmlNodePtr create_notation(const xmlChar *name, const xmlChar *ExternalID, const xmlChar *SystemID) /* {{{ */
{
	xmlEntityPtr ret;

	ret = (xmlEntityPtr) xmlMalloc(sizeof(xmlEntity));
    memset(ret, 0, sizeof(xmlEntity));
    ret->type = XML_NOTATION_NODE;
    ret->name = xmlStrdup(name);
	ret->ExternalID = xmlStrdup(ExternalID);
	ret->SystemID = xmlStrdup(SystemID);
	ret->length = 0;
	ret->content = NULL;
	ret->URI = NULL;
	ret->orig = NULL;
	ret->children = NULL;
	ret->parent = NULL;
	ret->doc = NULL;
	ret->_private = NULL;
	ret->last = NULL;
	ret->prev = NULL;
	return((xmlNodePtr) ret);
}
/* }}} */

xmlNode *php_dom_libxml_hash_iter(xmlHashTable *ht, int index) /* {{{ */
{
	xmlNode *nodep = NULL;
	nodeIterator *iter;
	int htsize;

	if ((htsize = xmlHashSize(ht)) > 0 && index < htsize) {
		iter = emalloc(sizeof(nodeIterator));
		iter->cur = 0;
		iter->index = index;
		iter->node = NULL;
		xmlHashScan(ht, itemHashScanner, iter);
		nodep = iter->node;
		efree(iter);
		return nodep;
	} else {
		return NULL;
	}
}
/* }}} */

xmlNode *php_dom_libxml_notation_iter(xmlHashTable *ht, int index) /* {{{ */
{
	notationIterator *iter;
	xmlNotation *notep = NULL;
	int htsize;

	if ((htsize = xmlHashSize(ht)) > 0 && index < htsize) {
		iter = emalloc(sizeof(notationIterator));
		iter->cur = 0;
		iter->index = index;
		iter->notation = NULL;
		xmlHashScan(ht, itemHashScanner, iter);
		notep = iter->notation;
		efree(iter);
		return create_notation(notep->name, notep->PublicID, notep->SystemID);
	} else {
		return NULL;
	}
}
/* }}} */

static void php_dom_iterator_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	zval_ptr_dtor((zval**)&iterator->intern.data);

	if (iterator->curobj) {
		zval_ptr_dtor((zval**)&iterator->curobj);
	}

	efree(iterator);
}
/* }}} */

static int php_dom_iterator_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{

	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	if (iterator->curobj) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

static void php_dom_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	*data = &iterator->curobj;
}
/* }}} */

static void php_dom_iterator_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;
	zval *object = (zval *)iterator->intern.data;

	if (instanceof_function(Z_OBJCE_P(object), dom_nodelist_class_entry TSRMLS_CC)) {
		ZVAL_LONG(key, iter->index);
	} else {
		dom_object *intern = (dom_object *)zend_object_store_get_object(iterator->curobj TSRMLS_CC);

		if (intern != NULL && intern->ptr != NULL) {
			xmlNodePtr curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->ptr)->node;
			ZVAL_STRINGL(key, (char *) curnode->name, xmlStrlen(curnode->name), 1);
		} else {
			ZVAL_NULL(key);
		}
	}
}
/* }}} */

static void php_dom_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	zval *curobj, *curattr = NULL;
	zval *object;
	xmlNodePtr curnode = NULL, basenode;
	dom_object *intern;
	dom_object *nnmap;
	dom_nnodemap_object *objmap;
	int ret, previndex=0;
	HashTable *nodeht;
	zval **entry;

	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	object = (zval *)iterator->intern.data;
	nnmap = (dom_object *)zend_object_store_get_object(object TSRMLS_CC);
	objmap = (dom_nnodemap_object *)nnmap->ptr;

	curobj = iterator->curobj;
	intern = (dom_object *)zend_object_store_get_object(curobj TSRMLS_CC);
	if (intern != NULL && intern->ptr != NULL) {
		if (objmap->nodetype != XML_ENTITY_NODE && 
			objmap->nodetype != XML_NOTATION_NODE) {
			if (objmap->nodetype == DOM_NODESET) {
				nodeht = HASH_OF(objmap->baseobjptr);
				zend_hash_move_forward(nodeht);
				if (zend_hash_get_current_data(nodeht, (void **) &entry)==SUCCESS) {
					curattr = *entry;
					Z_ADDREF_P(curattr);
				}
			} else {
				curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->ptr)->node;
				if (objmap->nodetype == XML_ATTRIBUTE_NODE || 
					objmap->nodetype == XML_ELEMENT_NODE) {
					curnode = curnode->next;
				} else {
					/* Nav the tree evey time as this is LIVE */
					basenode = dom_object_get_node(objmap->baseobj);
					if (basenode && (basenode->type == XML_DOCUMENT_NODE || 
						basenode->type == XML_HTML_DOCUMENT_NODE)) {
						basenode = xmlDocGetRootElement((xmlDoc *) basenode);
					} else if (basenode) {
						basenode = basenode->children;
					} else {
						goto err;
					}
					curnode = dom_get_elements_by_tag_name_ns_raw(basenode, objmap->ns, objmap->local, &previndex, iter->index);
				}
			}
		} else {
			if (objmap->nodetype == XML_ENTITY_NODE) {
				curnode = php_dom_libxml_hash_iter(objmap->ht, iter->index);
			} else {
				curnode = php_dom_libxml_notation_iter(objmap->ht, iter->index);
			}
		}
	}
err:
	zval_ptr_dtor((zval**)&curobj);
	if (curnode) {
		MAKE_STD_ZVAL(curattr);
		curattr = php_dom_create_object(curnode, &ret, curattr, objmap->baseobj TSRMLS_CC);
	}

	iterator->curobj = curattr;
}
/* }}} */

zend_object_iterator_funcs php_dom_iterator_funcs = {
	php_dom_iterator_dtor,
	php_dom_iterator_valid,
	php_dom_iterator_current_data,
	php_dom_iterator_current_key,
	php_dom_iterator_move_forward,
	NULL
};

zend_object_iterator *php_dom_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	dom_object *intern;
	dom_nnodemap_object *objmap;
	xmlNodePtr nodep, curnode=NULL;
	zval *curattr = NULL;
	int ret, curindex = 0;
	HashTable *nodeht;
	zval **entry;
	php_dom_iterator *iterator;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	iterator = emalloc(sizeof(php_dom_iterator));

	Z_ADDREF_P(object);
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &php_dom_iterator_funcs;

	intern = (dom_object *)zend_object_store_get_object(object TSRMLS_CC);
	objmap = (dom_nnodemap_object *)intern->ptr;
	if (objmap != NULL) {
		if (objmap->nodetype != XML_ENTITY_NODE && 
			objmap->nodetype != XML_NOTATION_NODE) {
			if (objmap->nodetype == DOM_NODESET) {
				nodeht = HASH_OF(objmap->baseobjptr);
				zend_hash_internal_pointer_reset(nodeht);
				if (zend_hash_get_current_data(nodeht, (void **) &entry)==SUCCESS) {
					curattr = *entry;
					Z_ADDREF_P(curattr);
				}
			} else {
				nodep = (xmlNode *)dom_object_get_node(objmap->baseobj);
				if (!nodep) {
					goto err;
				}
				if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
					if (objmap->nodetype == XML_ATTRIBUTE_NODE) {
						curnode = (xmlNodePtr) nodep->properties;
					} else {
						curnode = (xmlNodePtr) nodep->children;
					}
				} else {
					if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
						nodep = xmlDocGetRootElement((xmlDoc *) nodep);
					} else {
						nodep = nodep->children;
					}
					curnode = dom_get_elements_by_tag_name_ns_raw(nodep, objmap->ns, objmap->local, &curindex, 0);
				}
			}
		} else {
			if (objmap->nodetype == XML_ENTITY_NODE) {
				curnode = php_dom_libxml_hash_iter(objmap->ht, 0);
			} else {
				curnode = php_dom_libxml_notation_iter(objmap->ht, 0);
			}
		}
	}
err:
	if (curnode) {
		MAKE_STD_ZVAL(curattr);
		curattr = php_dom_create_object(curnode, &ret, curattr, objmap->baseobj TSRMLS_CC);
	}

	iterator->curobj = curattr;

	return (zend_object_iterator*)iterator;
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
