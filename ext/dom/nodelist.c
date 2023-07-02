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

static zend_always_inline void objmap_cache_release_cached_obj(dom_nnodemap_object *objmap)
{
	if (objmap->cached_obj) {
		/* Since the DOM is a tree there can be no cycles. */
		if (GC_DELREF(&objmap->cached_obj->std) == 0) {
			zend_objects_store_del(&objmap->cached_obj->std);
		}
		objmap->cached_obj = NULL;
		objmap->cached_obj_index = 0;
	}
}

static zend_always_inline void reset_objmap_cache(dom_nnodemap_object *objmap)
{
	objmap_cache_release_cached_obj(objmap);
	objmap->cached_length = -1;
}

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

	if (!php_dom_is_cache_tag_stale_from_node(&objmap->cache_tag, nodep)) {
		if (objmap->cached_length >= 0) {
			return objmap->cached_length;
		}
		/* Only the length is out-of-date, the cache tag is still valid.
		 * Therefore, only overwrite the length and keep the currently cached object. */
	} else {
		php_dom_mark_cache_tag_up_to_date_from_node(&objmap->cache_tag, nodep);
		reset_objmap_cache(objmap);
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
		xmlNodePtr basep = nodep;
		if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
			nodep = xmlDocGetRootElement((xmlDoc *) nodep);
		} else {
			nodep = nodep->children;
		}
		dom_get_elements_by_tag_name_ns_raw(
			basep, nodep, (char *) objmap->ns, (char *) objmap->local, &count, INT_MAX - 1 /* because of <= */);
	}

	objmap->cached_length = count;

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
	bool cache_itemnode = false;
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
					xmlNodePtr basep = dom_object_get_node(objmap->baseobj);
					if (basep) {
						xmlNodePtr nodep = basep;
						/* For now we're only able to use cache for forward search.
						 * TODO: in the future we could extend the logic of the node list such that backwards searches
						 *       are also possible. */
						bool restart = true;
						int relative_index = index;
						if (index >= objmap->cached_obj_index && objmap->cached_obj && !php_dom_is_cache_tag_stale_from_node(&objmap->cache_tag, nodep)) {
							xmlNodePtr cached_obj_xml_node = dom_object_get_node(objmap->cached_obj);

							/* The node cannot be NULL if the cache is valid. If it is NULL, then it means we
							 * forgot an invalidation somewhere. Take the defensive programming approach and invalidate
							 * it here if it's NULL (except in debug mode where we would want to catch this). */
							if (UNEXPECTED(cached_obj_xml_node == NULL)) {
#if ZEND_DEBUG
								ZEND_UNREACHABLE();
#endif
								reset_objmap_cache(objmap);
							} else {
								restart = false;
								relative_index -= objmap->cached_obj_index;
								nodep = cached_obj_xml_node;
							}
						}
						int count = 0;
						if (objmap->nodetype == XML_ATTRIBUTE_NODE || objmap->nodetype == XML_ELEMENT_NODE) {
							if (restart) {
								nodep = nodep->children;
							}
							while (count < relative_index && nodep != NULL) {
								count++;
								nodep = nodep->next;
							}
							itemnode = nodep;
						} else {
							if (restart) {
								if (basep->type == XML_DOCUMENT_NODE || basep->type == XML_HTML_DOCUMENT_NODE) {
									nodep = xmlDocGetRootElement((xmlDoc*) basep);
								} else {
									nodep = basep->children;
								}
							}
							itemnode = dom_get_elements_by_tag_name_ns_raw(basep, nodep, (char *) objmap->ns, (char *) objmap->local, &count, relative_index);
						}
						cache_itemnode = true;
					}
				}
			}
		}

		if (itemnode) {
			DOM_RET_OBJ(itemnode, &ret, objmap->baseobj);
			if (cache_itemnode) {
				/* Hold additional reference for the cache, must happen before releasing the cache
				 * because we might be the last reference holder.
				 * Instead of storing and copying zvals, we store the object pointer directly.
				 * This saves us some bytes because a pointer is smaller than a zval.
				 * This also means we have to manually refcount the objects here, and remove the reference count
				 * in reset_objmap_cache() and the destructor. */
				dom_object *cached_obj = Z_DOMOBJ_P(return_value);
				GC_ADDREF(&cached_obj->std);
				/* If the tag is stale, all cached data is useless. Otherwise only the cached object is useless. */
				if (php_dom_is_cache_tag_stale_from_node(&objmap->cache_tag, itemnode)) {
					php_dom_mark_cache_tag_up_to_date_from_node(&objmap->cache_tag, itemnode);
					reset_objmap_cache(objmap);
				} else {
					objmap_cache_release_cached_obj(objmap);
				}
				objmap->cached_obj_index = index;
				objmap->cached_obj = cached_obj;
			}
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
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();

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
