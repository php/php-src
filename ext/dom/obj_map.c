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
	HashTable *nodeht = Z_ARRVAL(map->baseobj_zv);
	return zend_hash_num_elements(nodeht);
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
	HashTable *nodeht = Z_ARRVAL(map->baseobj_zv);
	zval *entry = zend_hash_index_find(nodeht, index);
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

static xmlNodePtr dom_map_get_attr_start(xmlNodePtr node)
{
	ZEND_ASSERT(node->type == XML_ELEMENT_NODE);
	return (xmlNodePtr) node->properties;
}

static void dom_map_get_chain_item(dom_nnodemap_object *map, zend_long index, zval *return_value, xmlNodePtr (*get_start)(xmlNodePtr))
{
	xmlNodePtr nodep = dom_object_get_node(map->baseobj);
	xmlNodePtr itemnode = NULL;
	if (nodep && index >= 0) {
		dom_node_idx_pair start_point = dom_obj_map_get_start_point(map, nodep, index);
		itemnode = start_point.node ? start_point.node : get_start(nodep);
		for (; start_point.index > 0 && itemnode; itemnode = itemnode->next, start_point.index--);
	}
	dom_ret_node_to_zobj(map, itemnode, return_value);
	if (itemnode) {
		dom_map_cache_obj(map, itemnode, index, return_value);
	}
}

static void dom_map_get_attributes_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	dom_map_get_chain_item(map, index, return_value, dom_map_get_attr_start);
}

static void dom_map_get_nodes_item(dom_nnodemap_object *map, zend_long index, zval *return_value)
{
	dom_map_get_chain_item(map, index, return_value, dom_nodelist_iter_start_first_child);
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
	if (!objmap) {
		return 0;
	}

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

/**********************
 * === Named item === *
 **********************/

static xmlNodePtr dom_map_get_named_item_entity(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return xmlHashLookup(map->ht, BAD_CAST ZSTR_VAL(named));
}

static bool dom_map_has_named_item_xmlht(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return dom_map_get_named_item_entity(map, named, ns) != NULL;
}

static xmlNodePtr dom_map_get_named_item_notation(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	xmlNotationPtr notation = xmlHashLookup(map->ht, BAD_CAST ZSTR_VAL(named));
	if (notation) {
		return create_notation(notation->name, notation->PublicID, notation->SystemID);
	}
	return NULL;
}

static xmlNodePtr dom_map_get_named_item_prop(dom_nnodemap_object *map, const zend_string *named, const char *ns)
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

static bool dom_map_has_named_item_prop(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return dom_map_get_named_item_prop(map, named, ns) != NULL;
}

static xmlNodePtr dom_map_get_named_item_null(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return NULL;
}

static bool dom_map_has_named_item_null(dom_nnodemap_object *map, const zend_string *named, const char *ns)
{
	return false;
}

/**************************
 * === Handler tables === *
 **************************/

const php_dom_obj_map_handler php_dom_obj_map_attributes = {
	.length = dom_map_get_prop_length,
	.get_item = dom_map_get_attributes_item,
	.get_named_item = dom_map_get_named_item_prop,
	.has_named_item = dom_map_has_named_item_prop,
	.use_cache = true,
	.nameless = false,
};

const php_dom_obj_map_handler php_dom_obj_map_by_tag_name = {
	.length = dom_map_get_by_tag_name_length,
	.get_item = dom_map_get_by_tag_name_item,
	.get_named_item = dom_map_get_named_item_null,
	.has_named_item = dom_map_has_named_item_null,
	.use_cache = true,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_child_nodes = {
	.length = dom_map_get_nodes_length,
	.get_item = dom_map_get_nodes_item,
	.get_named_item = dom_map_get_named_item_null,
	.has_named_item = dom_map_has_named_item_null,
	.use_cache = true,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_nodeset = {
	.length = dom_map_get_nodeset_length,
	.get_item = dom_map_get_nodeset_item,
	.get_named_item = dom_map_get_named_item_null,
	.has_named_item = dom_map_has_named_item_null,
	.use_cache = false,
	.nameless = true,
};

const php_dom_obj_map_handler php_dom_obj_map_entities = {
	.length = dom_map_get_xmlht_length,
	.get_item = dom_map_get_entity_item,
	.get_named_item = dom_map_get_named_item_entity,
	.has_named_item = dom_map_has_named_item_xmlht,
	.use_cache = false,
	.nameless = false,
};

const php_dom_obj_map_handler php_dom_obj_map_notations = {
	.length = dom_map_get_xmlht_length,
	.get_item = dom_map_get_notation_item,
	.get_named_item = dom_map_get_named_item_notation,
	.has_named_item = dom_map_has_named_item_xmlht,
	.use_cache = false,
	.nameless = false,
};

const php_dom_obj_map_handler php_dom_obj_map_noop = {
	.length = dom_map_get_zero_length,
	.get_item = dom_map_get_null_item,
	.get_named_item = dom_map_get_named_item_null,
	.has_named_item = dom_map_has_named_item_null,
	.use_cache = false,
	.nameless = true,
};

#endif
