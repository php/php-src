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
   |          Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "obj_map.h"
#include "token_list.h"

static zend_always_inline void objmap_cache_release_cached_obj(dom_nnodemap_object *objmap)
{
	if (objmap->cached_obj) {
		OBJ_RELEASE(&objmap->cached_obj->std);
		objmap->cached_obj = NULL;
		objmap->cached_obj_index = 0;
	}
}

static zend_always_inline void reset_objmap_cache(dom_nnodemap_object *objmap)
{
	objmap_cache_release_cached_obj(objmap);
	objmap->cached_length = -1;
}

static bool dom_matches_class_name(const dom_nnodemap_object *map, const xmlNode *nodep)
{
	bool ret = false;

	if (nodep->type == XML_ELEMENT_NODE) {
		xmlAttrPtr classes = xmlHasNsProp(nodep, BAD_CAST "class", NULL);
		if (classes != NULL) {
			bool should_free;
			xmlChar *value = php_libxml_attr_value(classes, &should_free);

			bool quirks = map->baseobj->document->quirks_mode == PHP_LIBXML_QUIRKS;
			if (dom_ordered_set_all_contained(map->array, (const char *) value, quirks)) {
				ret = true;
			}

			if (should_free) {
				xmlFree(value);
			}
		}
	}

	return ret;
}

/**************************
 * === Length methods === *
 **************************/

static zend_long dom_map_get_xmlht_length(dom_nnodemap_object *map)
{
	/* Note: if there are, for example, no entities or notations then the hash table can be NULL. */
	return map->ht ? xmlHashSize(map->ht) : 0;
}

static zend_long dom_map_get_nodeset_length(dom_nnodemap_object *map)
{
	return zend_hash_num_elements(map->array);
}

static zend_long dom_map_get_prop_length(dom_nnodemap_object *map)
{
	zend_long count = 0;
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	if (nodep) {
		for (xmlAttrPtr curnode = nodep->properties; curnode; curnode = curnode->next) {
			count++;
		}
	}
	return count;
}

static zend_long dom_map_get_nodes_length(dom_nnodemap_object *map)
{
	zend_long count = 0;
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	if (nodep) {
		for (xmlNodePtr curnode = dom_nodelist_iter_start_first_child(nodep); curnode; curnode = curnode->next) {
			count++;
		}
	}
	return count;
}

static zend_long dom_map_get_elements_length(dom_nnodemap_object *map)
{
	zend_long count = 0;
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	if (nodep) {
		for (xmlNodePtr curnode = dom_nodelist_iter_start_first_child(nodep); curnode; curnode = curnode->next) {
			if (curnode->type == XML_ELEMENT_NODE) {
				count++;
			}
		}
	}
	return count;
}

static zend_long dom_map_get_by_tag_name_length(dom_nnodemap_object *map)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	zend_long count = 0;
	if (nodep) {
		xmlNodePtr basep = nodep;
		nodep = php_dom_first_child_of_container_node(basep);
		dom_get_elements_by_tag_name_ns_raw(
			basep, nodep, map->ns, map->local, map->local_lower, &count, ZEND_LONG_MAX - 1 /* because of <= */);
	}
	return count;
}

static zend_long dom_map_get_by_class_name_length(dom_nnodemap_object *map)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	zend_long count = 0;
	if (nodep) {
		xmlNodePtr basep = nodep;
		nodep = php_dom_first_child_of_container_node(basep);

		while (nodep != NULL) {
			if (dom_matches_class_name(map, nodep)) {
				count++;
			}
			nodep = php_dom_next_in_tree_order(nodep, basep);
		}
	}
	return count;
}

static zend_long dom_map_get_zero_length(dom_nnodemap_object *map)
{
	return 0;
}

/************************
 * === Item lookups === *
 ************************/

static void dom_ret_node_to_zobj(dom_nnodemap_object *map, xmlNodePtr node, zval *return_value)
{
	if (node) {
		DOM_RET_OBJ(node, map->baseobj);
	} else {
		RETURN_NULL();
	}
}

static void dom_map_get_entity_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr node = map->ht ? php_dom_libxml_hash_iter(map->ht, index) : NULL;
	dom_ret_node_to_zobj(map, node, return_value);
}

static void dom_map_get_notation_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr node = map->ht ? php_dom_libxml_hash_iter(map->ht, index) : NULL;
	if (node) {
		xmlNotation *notation = (xmlNotation *) node;
		node = create_notation(notation->name, notation->PublicID, notation->SystemID);
	}
	dom_ret_node_to_zobj(map, node, return_value);
}

static void dom_map_get_nodeset_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	zval *entry = zend_hash_index_find(map->array, index);
	if (entry) {
		RETURN_COPY(entry);
	} else {
		RETURN_NULL();
	}
}

typedef struct dom_node_idx_pair {
	xmlNodePtr node;
	zend_long index;
} dom_node_idx_pair;

static dom_node_idx_pair dom_obj_map_get_start_point(dom_nnodemap_object *map, xmlNodePtr basep, zend_long index)
{
	dom_node_idx_pair ret;
	ZEND_ASSERT(basep != NULL);
	/* For now we're only able to use cache for forward search.
	 * TODO: in the future we could extend the logic of the node list such that backwards searches
	 *       are also possible. */
	bool restart = true;
	zend_long relative_index = index;
	if (index >= map->cached_obj_index && map->cached_obj && !php_dom_is_cache_tag_stale_from_node(&map->cache_tag, basep)) {
		xmlNodePtr cached_obj_xml_node = dom_object_get_node(map->cached_obj);

		/* The node cannot be NULL if the cache is valid. If it is NULL, then it means we
		 * forgot an invalidation somewhere. Take the defensive programming approach and invalidate
		 * it here if it's NULL (except in debug mode where we would want to catch this). */
		if (UNEXPECTED(cached_obj_xml_node == NULL)) {
#if ZEND_DEBUG
			ZEND_UNREACHABLE();
#endif
			reset_objmap_cache(map);
		} else {
			restart = false;
			relative_index -= map->cached_obj_index;
			basep = cached_obj_xml_node;
		}
	}
	ret.node = restart ? NULL : basep;
	ret.index = relative_index;
	return ret;
}

static void dom_map_cache_obj(dom_nnodemap_object *map, xmlNodePtr itemnode, zend_long index, zval *return_value)
{
	/* Hold additional reference for the cache, must happen before releasing the cache
	 * because we might be the last reference holder.
	 * Instead of storing and copying zvals, we store the object pointer directly.
	 * This saves us some bytes because a pointer is smaller than a zval.
	 * This also means we have to manually refcount the objects here, and remove the reference count
	 * in reset_objmap_cache() and the destructor. */
	dom_object *cached_obj = Z_DOMOBJ_P(return_value);
	GC_ADDREF(&cached_obj->std);
	/* If the tag is stale, all cached data is useless. Otherwise only the cached object is useless. */
	if (php_dom_is_cache_tag_stale_from_node(&map->cache_tag, itemnode)) {
		php_dom_mark_cache_tag_up_to_date_from_node(&map->cache_tag, itemnode);
		reset_objmap_cache(map);
	} else {
		objmap_cache_release_cached_obj(map);
	}
	map->cached_obj_index = index;
	map->cached_obj = cached_obj;
}

static void dom_map_get_attributes_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	xmlNodePtr itemnode = NULL;
	if (nodep && index >= 0) {
		ZEND_ASSERT(nodep->type == XML_ELEMENT_NODE);
		itemnode = (xmlNodePtr) nodep->properties;
		for (; index > 0 && itemnode; itemnode = itemnode->next, index--);
	}
	dom_ret_node_to_zobj(map, itemnode, return_value);
}

static void dom_map_get_nodes_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	xmlNodePtr itemnode = NULL;
	if (nodep && index >= 0) {
		dom_node_idx_pair start_point = dom_obj_map_get_start_point(map, nodep, index);
		itemnode = start_point.node ? start_point.node : dom_nodelist_iter_start_first_child(nodep);
		for (; start_point.index > 0 && itemnode; itemnode = itemnode->next, start_point.index--);
	}
	dom_ret_node_to_zobj(map, itemnode, return_value);
	if (itemnode) {
		dom_map_cache_obj(map, itemnode, index, return_value);
	}
}

static void dom_map_get_elements_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	xmlNodePtr itemnode = NULL;
	if (nodep && index >= 0) {
		dom_node_idx_pair start_point = dom_obj_map_get_start_point(map, nodep, index);
		if (start_point.node) {
			/* Guaranteed to be an element */
			itemnode = start_point.node;
		} else {
			/* Fetch first element child */
			itemnode = nodep->children;
			while (itemnode && itemnode->type != XML_ELEMENT_NODE) {
				itemnode = itemnode->next;
			}
		}

		for (; start_point.index > 0 && itemnode; --start_point.index) {
			do {
				itemnode = itemnode->next;
			} while (itemnode && itemnode->type != XML_ELEMENT_NODE);
		}
		if (itemnode && itemnode->type != XML_ELEMENT_NODE) {
			itemnode = NULL;
		}
	}
	dom_ret_node_to_zobj(map, itemnode, return_value);
	if (itemnode) {
		dom_map_cache_obj(map, itemnode, index, return_value);
	}
}

static void dom_map_collection_named_item_elements_iter(dom_nnodemap_object *map, php_dom_obj_map_collection_iter *iter)
{
	if (iter->candidate != iter->basep->children) {
		iter->candidate = iter->candidate->next;
	}
	while (iter->candidate && iter->candidate->type != XML_ELEMENT_NODE) {
		iter->candidate = iter->candidate->next;
	}
}

static void dom_map_collection_named_item_null(dom_nnodemap_object *map, php_dom_obj_map_collection_iter *iter)
{
}

static void dom_map_get_by_tag_name_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	xmlNodePtr itemnode = NULL;
	if (nodep && index >= 0) {
		zend_long count = 0;
		dom_node_idx_pair start_point = dom_obj_map_get_start_point(map, nodep, index);
		itemnode = start_point.node ? start_point.node : php_dom_first_child_of_container_node(nodep);
		itemnode = dom_get_elements_by_tag_name_ns_raw(nodep, itemnode, map->ns, map->local, map->local_lower, &count, start_point.index);
	}
	dom_ret_node_to_zobj(map, itemnode, return_value);
	if (itemnode) {
		dom_map_cache_obj(map, itemnode, index, return_value);
	}
}

static void dom_map_get_by_class_name_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	xmlNodePtr itemnode = NULL;
	if (nodep && index >= 0) {
		dom_node_idx_pair start_point = dom_obj_map_get_start_point(map, nodep, index);
		if (start_point.node) {
			if (start_point.index > 0) {
				/* Only start iteration at next point if we actually have an index to seek to. */
				itemnode = php_dom_next_in_tree_order(start_point.node, nodep);
			} else {
				itemnode = start_point.node;
			}
		} else {
			itemnode = php_dom_first_child_of_container_node(nodep);
		}

		do {
			--start_point.index;
			while (itemnode != NULL && !dom_matches_class_name(map, itemnode)) {
				itemnode = php_dom_next_in_tree_order(itemnode, nodep);
			}
		} while (start_point.index > 0 && itemnode);
	}
	dom_ret_node_to_zobj(map, itemnode, return_value);
	if (itemnode) {
		dom_map_cache_obj(map, itemnode, index, return_value);
	}
}

static void dom_map_collection_named_item_by_tag_name_iter(dom_nnodemap_object *map, php_dom_obj_map_collection_iter *iter)
{
	iter->candidate = dom_get_elements_by_tag_name_ns_raw(iter->basep, iter->candidate, map->ns, map->local, map->local_lower, &iter->cur, iter->next);
	iter->next = iter->cur + 1;
}

static void dom_map_collection_named_item_by_class_name_iter(dom_nnodemap_object *map, php_dom_obj_map_collection_iter *iter)
{
	xmlNodePtr basep = iter->basep;
	xmlNodePtr nodep = iter->candidate ? php_dom_next_in_tree_order(iter->candidate, basep) : php_dom_first_child_of_container_node(basep);

	while (nodep != NULL && !dom_matches_class_name(map, nodep)) {
		nodep = php_dom_next_in_tree_order(nodep, basep);
	}

	iter->candidate = nodep;
}

static void dom_map_get_null_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	RETURN_NULL();
}

/***********************
 * === Common APIs === *
 ***********************/

zend_long php_dom_get_nodelist_length(dom_object *obj)
{
	dom_nnodemap_object *objmap = obj->ptr;

	if (objmap->handler->use_cache) {
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
	}

	zend_long count = objmap->handler->length(objmap);

	if (objmap->handler->use_cache) {
		objmap->cached_length = count;
	}

	return count;
}

void php_dom_create_obj_map(dom_object *basenode, dom_object *intern, xmlHashTablePtr ht, zend_string *local, zend_string *ns, const php_dom_obj_map_handler *handler)
{
	dom_nnodemap_object *mapptr = intern->ptr;

	ZEND_ASSERT(basenode != NULL);

	GC_ADDREF(&basenode->std);

	xmlDocPtr doc = basenode->document ? basenode->document->ptr : NULL;

	mapptr->handler = handler;
	mapptr->baseobj = basenode;
	mapptr->ht = ht;
	if (EXPECTED(doc != NULL)) {
		mapptr->dict = doc->dict;
		xmlDictReference(doc->dict);
	}

	const xmlChar* tmp;

	if (local) {
		int len = (int) ZSTR_LEN(local);
		if (doc != NULL && (tmp = xmlDictExists(doc->dict, (const xmlChar *)ZSTR_VAL(local), len)) != NULL) {
			mapptr->local = BAD_CAST tmp;
		} else {
			mapptr->local = BAD_CAST ZSTR_VAL(zend_string_copy(local));
			mapptr->release_local = true;
		}
		mapptr->local_lower = zend_string_tolower(local);
	}

	if (ns) {
		int len = (int) ZSTR_LEN(ns);
		if (doc != NULL && (tmp = xmlDictExists(doc->dict, (const xmlChar *)ZSTR_VAL(ns), len)) != NULL) {
			mapptr->ns = BAD_CAST tmp;
		} else {
			mapptr->ns = BAD_CAST ZSTR_VAL(zend_string_copy(ns));
			mapptr->release_ns = true;
		}
	}
}

void php_dom_obj_map_get_item_into_zval(dom_nnodemap_object *objmap, zend_long index, zval *return_value)
{
	if (EXPECTED(objmap)) {
		objmap->handler->get_item(objmap, index, return_value);
	} else {
		RETURN_NULL();
	}
}

void php_dom_obj_map_get_ns_named_item_into_zval(dom_nnodemap_object *objmap, const zend_string *named, const char *ns, zval *return_value)
{
	xmlNodePtr itemnode = objmap->handler->get_ns_named_item(objmap, named, ns);
	if (itemnode) {
		DOM_RET_OBJ(itemnode, objmap->baseobj);
	} else {
		RETURN_NULL();
	}
}

/**********************
 * === Named item === *
 **********************/

static xmlNodePtr dom_map_get_ns_named_item_entity(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return xmlHashLookup(map->ht, BAD_CAST ZSTR_VAL(named));
}

static bool dom_map_has_ns_named_item_xmlht(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return dom_map_get_ns_named_item_entity(map, named, ns) != NULL;
}

static xmlNodePtr dom_map_get_ns_named_item_notation(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	xmlNotationPtr notation = xmlHashLookup(map->ht, BAD_CAST ZSTR_VAL(named));
	if (notation) {
		return create_notation(notation->name, notation->PublicID, notation->SystemID);
	}
	return NULL;
}

static xmlNodePtr dom_map_get_ns_named_item_prop(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	if (nodep) {
		if (php_dom_follow_spec_intern(map->baseobj)) {
			return (xmlNodePtr) php_dom_get_attribute_node(nodep, BAD_CAST ZSTR_VAL(named), ZSTR_LEN(named));
		} else {
			if (ns) {
				return (xmlNodePtr) xmlHasNsProp(nodep, BAD_CAST ZSTR_VAL(named), BAD_CAST ns);
			} else {
				return (xmlNodePtr) xmlHasProp(nodep, BAD_CAST ZSTR_VAL(named));
			}
		}
	}
	return NULL;
}

static bool dom_map_has_ns_named_item_prop(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return dom_map_get_ns_named_item_prop(map, named, ns) != NULL;
}

static xmlNodePtr dom_map_get_ns_named_item_null(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return NULL;
}

static bool dom_map_has_ns_named_item_null(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return false;
}

/**************************
 * === Handler tables === *
 **************************/

const php_dom_obj_map_handler php_dom_obj_map_attributes = {
	.length = dom_map_get_prop_length,
	.get_item = dom_map_get_attributes_item,
	.get_ns_named_item = dom_map_get_ns_named_item_prop,
	.has_ns_named_item = dom_map_has_ns_named_item_prop,
	.collection_named_item_iter = NULL,
	.use_cache = false,
	.nameless = false,
};

const php_dom_obj_map_handler php_dom_obj_map_by_tag_name = {
	.length = dom_map_get_by_tag_name_length,
	.get_item = dom_map_get_by_tag_name_item,
	.get_ns_named_item = dom_map_get_ns_named_item_null,
	.has_ns_named_item = dom_map_has_ns_named_item_null,
	.collection_named_item_iter = dom_map_collection_named_item_by_tag_name_iter,
	.use_cache = true,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_by_class_name = {
	.length = dom_map_get_by_class_name_length,
	.get_item = dom_map_get_by_class_name_item,
	.get_ns_named_item = dom_map_get_ns_named_item_null,
	.has_ns_named_item = dom_map_has_ns_named_item_null,
	.collection_named_item_iter = dom_map_collection_named_item_by_class_name_iter,
	.use_cache = true,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_child_nodes = {
	.length = dom_map_get_nodes_length,
	.get_item = dom_map_get_nodes_item,
	.get_ns_named_item = dom_map_get_ns_named_item_null,
	.has_ns_named_item = dom_map_has_ns_named_item_null,
	.collection_named_item_iter = NULL,
	.use_cache = true,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_nodeset = {
	.length = dom_map_get_nodeset_length,
	.get_item = dom_map_get_nodeset_item,
	.get_ns_named_item = dom_map_get_ns_named_item_null,
	.has_ns_named_item = dom_map_has_ns_named_item_null,
	.collection_named_item_iter = NULL,
	.use_cache = false,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_entities = {
	.length = dom_map_get_xmlht_length,
	.get_item = dom_map_get_entity_item,
	.get_ns_named_item = dom_map_get_ns_named_item_entity,
	.has_ns_named_item = dom_map_has_ns_named_item_xmlht,
	.collection_named_item_iter = NULL,
	.use_cache = false,
	.nameless = false,
};

const php_dom_obj_map_handler php_dom_obj_map_notations = {
	.length = dom_map_get_xmlht_length,
	.get_item = dom_map_get_notation_item,
	.get_ns_named_item = dom_map_get_ns_named_item_notation,
	.has_ns_named_item = dom_map_has_ns_named_item_xmlht,
	.collection_named_item_iter = NULL,
	.use_cache = false,
	.nameless = false,
};

const php_dom_obj_map_handler php_dom_obj_map_child_elements = {
	.length = dom_map_get_elements_length,
	.get_item = dom_map_get_elements_item,
	.get_ns_named_item = dom_map_get_ns_named_item_null,
	.has_ns_named_item = dom_map_has_ns_named_item_null,
	.collection_named_item_iter = dom_map_collection_named_item_elements_iter,
	.use_cache = true,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_noop = {
	.length = dom_map_get_zero_length,
	.get_item = dom_map_get_null_item,
	.get_ns_named_item = dom_map_get_ns_named_item_null,
	.has_ns_named_item = dom_map_has_ns_named_item_null,
	.collection_named_item_iter = dom_map_collection_named_item_null,
	.use_cache = false,
	.nameless = true,
};

#endif
