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

#if LIBXML_VERSION >= 20908
static void itemHashScanner (void *payload, void *data, const xmlChar *name) /* {{{ */
#else
static void itemHashScanner (void *payload, void *data, xmlChar *name)
#endif
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

static void php_dom_iterator_dtor(zend_object_iterator *iter) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	zval_ptr_dtor(&iterator->intern.data);
	zval_ptr_dtor(&iterator->curobj);
}
/* }}} */

static int php_dom_iterator_valid(zend_object_iterator *iter) /* {{{ */
{

	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	if (Z_TYPE(iterator->curobj) != IS_UNDEF) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

zval *php_dom_iterator_current_data(zend_object_iterator *iter) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	return &iterator->curobj;
}
/* }}} */

static void php_dom_iterator_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;
	zval *object = &iterator->intern.data;

	if (instanceof_function(Z_OBJCE_P(object), dom_nodelist_class_entry)) {
		ZVAL_LONG(key, iter->index);
	} else {
		dom_object *intern = Z_DOMOBJ_P(&iterator->curobj);

		if (intern != NULL && intern->ptr != NULL) {
			xmlNodePtr curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->ptr)->node;
			ZVAL_STRINGL(key, (char *) curnode->name, xmlStrlen(curnode->name));
		} else {
			ZVAL_NULL(key);
		}
	}
}
/* }}} */

static void php_dom_iterator_move_forward(zend_object_iterator *iter) /* {{{ */
{
	zval *object;
	xmlNodePtr curnode = NULL, basenode;
	dom_object *intern;
	dom_object *nnmap;
	dom_nnodemap_object *objmap;
	int previndex=0;
	HashTable *nodeht;
	zval *entry;
	bool do_curobj_undef = 1;

	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	object = &iterator->intern.data;
	nnmap = Z_DOMOBJ_P(object);
	objmap = (dom_nnodemap_object *)nnmap->ptr;

	intern = Z_DOMOBJ_P(&iterator->curobj);

	if (intern != NULL && intern->ptr != NULL) {
		if (objmap->nodetype != XML_ENTITY_NODE &&
			objmap->nodetype != XML_NOTATION_NODE) {
			if (objmap->nodetype == DOM_NODESET) {
				nodeht = HASH_OF(&objmap->baseobj_zv);
				zend_hash_move_forward_ex(nodeht, &iterator->pos);
				if ((entry = zend_hash_get_current_data_ex(nodeht, &iterator->pos))) {
					zval_ptr_dtor(&iterator->curobj);
					ZVAL_UNDEF(&iterator->curobj);
					ZVAL_COPY(&iterator->curobj, entry);
					do_curobj_undef = 0;
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
					curnode = dom_get_elements_by_tag_name_ns_raw(
						basenode, (char *) objmap->ns, (char *) objmap->local, &previndex, iter->index);
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
	if (do_curobj_undef) {
		zval_ptr_dtor(&iterator->curobj);
		ZVAL_UNDEF(&iterator->curobj);
	}
	if (curnode) {
		php_dom_create_object(curnode, &iterator->curobj, objmap->baseobj);
	}
}
/* }}} */

static const zend_object_iterator_funcs php_dom_iterator_funcs = {
	php_dom_iterator_dtor,
	php_dom_iterator_valid,
	php_dom_iterator_current_data,
	php_dom_iterator_current_key,
	php_dom_iterator_move_forward,
	NULL,
	NULL,
	NULL, /* get_gc */
};

zend_object_iterator *php_dom_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	dom_object *intern;
	dom_nnodemap_object *objmap;
	xmlNodePtr nodep, curnode=NULL;
	int curindex = 0;
	HashTable *nodeht;
	zval *entry;
	php_dom_iterator *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}
	iterator = emalloc(sizeof(php_dom_iterator));
	zend_iterator_init(&iterator->intern);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &php_dom_iterator_funcs;

	ZVAL_UNDEF(&iterator->curobj);

	intern = Z_DOMOBJ_P(object);
	objmap = (dom_nnodemap_object *)intern->ptr;
	if (objmap != NULL) {
		if (objmap->nodetype != XML_ENTITY_NODE &&
			objmap->nodetype != XML_NOTATION_NODE) {
			if (objmap->nodetype == DOM_NODESET) {
				nodeht = HASH_OF(&objmap->baseobj_zv);
				zend_hash_internal_pointer_reset_ex(nodeht, &iterator->pos);
				if ((entry = zend_hash_get_current_data_ex(nodeht, &iterator->pos))) {
					ZVAL_COPY(&iterator->curobj, entry);
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
					curnode = dom_get_elements_by_tag_name_ns_raw(
						nodep, (char *) objmap->ns, (char *) objmap->local, &curindex, 0);
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
		php_dom_create_object(curnode, &iterator->curobj, objmap->baseobj);
	}

	return &iterator->intern;
}
/* }}} */

#endif
