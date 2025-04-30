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
#include "dom_ce.h"

typedef struct nodeIterator {
	int cur;
	int index;
	xmlNode *node;
} nodeIterator;

/* Function pointer typedef changed in 2.9.8, see https://github.com/GNOME/libxml2/commit/e03f0a199a67017b2f8052354cf732b2b4cae787 */
#if LIBXML_VERSION >= 20908
static void itemHashScanner (void *payload, void *data, const xmlChar *name) /* {{{ */
#else
static void itemHashScanner (void *payload, void *data, xmlChar *name)
#endif
{
	nodeIterator *priv = data;

	if (priv->cur < priv->index) {
		priv->cur++;
	} else {
		if (priv->node == NULL) {
			priv->node = payload;
		}
	}
}
/* }}} */

static dom_nnodemap_object *php_dom_iterator_get_nnmap(const php_dom_iterator *iterator)
{
	const zval *object = &iterator->intern.data;
	dom_object *nnmap = Z_DOMOBJ_P(object);
	return nnmap->ptr;
}

xmlNodePtr create_notation(const xmlChar *name, const xmlChar *ExternalID, const xmlChar *SystemID) /* {{{ */
{
	xmlEntityPtr ret = xmlMalloc(sizeof(xmlEntity));
	memset(ret, 0, sizeof(xmlEntity));
	ret->type = XML_NOTATION_NODE;
	ret->name = xmlStrdup(name);
	ret->ExternalID = xmlStrdup(ExternalID);
	ret->SystemID = xmlStrdup(SystemID);
	return (xmlNodePtr) ret;
}
/* }}} */

static xmlNode *php_dom_libxml_hash_iter_ex(xmlHashTable *ht, int index)
{
	int htsize;

	if ((htsize = xmlHashSize(ht)) > 0 && index < htsize) {
		nodeIterator iter;
		iter.cur = 0;
		iter.index = index;
		iter.node = NULL;
		xmlHashScan(ht, itemHashScanner, &iter);
		return iter.node;
	} else {
		return NULL;
	}
}

xmlNode *php_dom_libxml_hash_iter(dom_nnodemap_object *objmap, int index)
{
	xmlNode *curnode = php_dom_libxml_hash_iter_ex(objmap->ht, index);

	if (curnode != NULL && objmap->nodetype != XML_ENTITY_NODE) {
		xmlNotation *notation = (xmlNotation *) curnode;
		curnode = create_notation(notation->name, notation->PublicID, notation->SystemID);
	}

	return curnode;
}

static void php_dom_iterator_dtor(zend_object_iterator *iter) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;

	zval_ptr_dtor(&iterator->intern.data);
	zval_ptr_dtor(&iterator->curobj);
}
/* }}} */

static zend_result php_dom_iterator_valid(zend_object_iterator *iter) /* {{{ */
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
	return Z_ISUNDEF(iterator->curobj) ? NULL : &iterator->curobj;
}
/* }}} */

static void php_dom_iterator_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	php_dom_iterator *iterator = (php_dom_iterator *)iter;
	dom_nnodemap_object *objmap = php_dom_iterator_get_nnmap(iterator);

	/* Only dtd named node maps, i.e. the ones based on a libxml hash table or attribute collections,
	 * are keyed by the name because in that case the name is unique. */
	if (!objmap->ht && objmap->nodetype != XML_ATTRIBUTE_NODE) {
		ZVAL_LONG(key, iterator->index);
	} else {
		dom_object *intern = Z_DOMOBJ_P(&iterator->curobj);

		if (intern != NULL && intern->ptr != NULL) {
			xmlNodePtr curnode = ((php_libxml_node_ptr *)intern->ptr)->node;
			if (objmap->nodetype == XML_ATTRIBUTE_NODE && php_dom_follow_spec_intern(intern)) {
				ZVAL_NEW_STR(key, dom_node_get_node_name_attribute_or_element(curnode, false));
			} else {
				ZVAL_STRINGL(key, (const char *) curnode->name, xmlStrlen(curnode->name));
			}
		} else {
			ZVAL_NULL(key);
		}
	}
}
/* }}} */

static xmlNodePtr dom_fetch_first_iteration_item(dom_nnodemap_object *objmap)
{
	xmlNodePtr basep = dom_object_get_node(objmap->baseobj);
	if (!basep) {
		return NULL;
	}
	if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
		if (objmap->nodetype == XML_ATTRIBUTE_NODE) {
			return (xmlNodePtr) basep->properties;
		} else {
			return dom_nodelist_iter_start_first_child(basep);
		}
	} else {
		zend_long curindex = 0;
		xmlNodePtr nodep = php_dom_first_child_of_container_node(basep);
		return dom_get_elements_by_tag_name_ns_raw(
			basep, nodep, objmap->ns, objmap->local, objmap->local_lower, &curindex, 0);
	}
}

static void php_dom_iterator_move_forward(zend_object_iterator *iter) /* {{{ */
{
	xmlNodePtr curnode = NULL;

	php_dom_iterator *iterator = (php_dom_iterator *)iter;
	if (Z_ISUNDEF(iterator->curobj)) {
		return;
	}

	iterator->index++;

	dom_object *intern = Z_DOMOBJ_P(&iterator->curobj);
	dom_nnodemap_object *objmap = php_dom_iterator_get_nnmap(iterator);

	if (intern != NULL && intern->ptr != NULL) {
		if (objmap->nodetype != XML_ENTITY_NODE &&
			objmap->nodetype != XML_NOTATION_NODE) {
			if (objmap->nodetype == DOM_NODESET) {
				HashTable *nodeht = Z_ARRVAL_P(&objmap->baseobj_zv);
				zval *entry = zend_hash_index_find(nodeht, iterator->index);
				if (entry) {
					zval_ptr_dtor(&iterator->curobj);
					ZVAL_COPY(&iterator->curobj, entry);
					return;
				}
			} else {
				if (objmap->nodetype == XML_ATTRIBUTE_NODE ||
					objmap->nodetype == XML_ELEMENT_NODE) {

					/* Note: keep legacy behaviour for non-spec mode. */
					if (php_dom_follow_spec_intern(intern) && php_dom_is_cache_tag_stale_from_doc_ptr(&iterator->cache_tag, intern->document)) {
						php_dom_mark_cache_tag_up_to_date_from_doc_ref(&iterator->cache_tag, intern->document);
						curnode = dom_fetch_first_iteration_item(objmap);
						zend_ulong index = 0;
						while (curnode != NULL && index++ < iterator->index) {
							curnode = curnode->next;
						}
					} else {
						curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->ptr)->node;
						curnode = curnode->next;
					}
				} else {
					/* The collection is live, we nav the tree from the base object if we cannot
					 * use the cache to restart from the last point. */
					xmlNodePtr basenode = dom_object_get_node(objmap->baseobj);

					/* We have a strong reference to the base node via baseobj_zv, this cannot become NULL */
					ZEND_ASSERT(basenode != NULL);

					zend_long previndex;
					if (php_dom_is_cache_tag_stale_from_node(&iterator->cache_tag, basenode)) {
						php_dom_mark_cache_tag_up_to_date_from_node(&iterator->cache_tag, basenode);
						previndex = 0;
						curnode = php_dom_first_child_of_container_node(basenode);
					} else {
						previndex = iterator->index - 1;
						curnode = (xmlNodePtr)((php_libxml_node_ptr *)intern->ptr)->node;
					}
					curnode = dom_get_elements_by_tag_name_ns_raw(
						basenode, curnode, objmap->ns, objmap->local, objmap->local_lower, &previndex, iterator->index);
				}
			}
		} else {
			curnode = php_dom_libxml_hash_iter(objmap, iterator->index);
		}
	}

	zval_ptr_dtor(&iterator->curobj);
	ZVAL_UNDEF(&iterator->curobj);

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
	xmlNodePtr curnode=NULL;
	php_dom_iterator *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}
	iterator = emalloc(sizeof(*iterator));
	memset(iterator, 0, sizeof(*iterator));
	zend_iterator_init(&iterator->intern);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &php_dom_iterator_funcs;

	intern = Z_DOMOBJ_P(object);
	objmap = (dom_nnodemap_object *)intern->ptr;
	if (objmap != NULL) {
		if (objmap->nodetype != XML_ENTITY_NODE &&
			objmap->nodetype != XML_NOTATION_NODE) {
			if (objmap->nodetype == DOM_NODESET) {
				HashTable *nodeht = Z_ARRVAL_P(&objmap->baseobj_zv);
				zval *entry = zend_hash_index_find(nodeht, 0);
				if (entry) {
					ZVAL_COPY(&iterator->curobj, entry);
				}
			} else {
				curnode = dom_fetch_first_iteration_item(objmap);
			}
		} else {
			curnode = php_dom_libxml_hash_iter(objmap, 0);
		}
	}

	if (curnode) {
		php_dom_create_object(curnode, &iterator->curobj, objmap->baseobj);
	}

	return &iterator->intern;
}
/* }}} */

#endif
