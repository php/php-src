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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_DOM_OBJ_MAP_H
#define PHP_DOM_OBJ_MAP_H

typedef struct dom_nnodemap_object dom_nnodemap_object;

typedef struct php_dom_obj_map_collection_iter {
	zend_long cur, next;
	xmlNodePtr candidate, basep;
} php_dom_obj_map_collection_iter;

typedef struct php_dom_obj_map_handler {
	zend_long (*length)(dom_nnodemap_object *);
	void (*get_item)(dom_nnodemap_object *, zend_long, zval *);
	xmlNodePtr (*get_ns_named_item)(dom_nnodemap_object *, const zend_string *, const char *);
	bool (*has_ns_named_item)(dom_nnodemap_object *, const zend_string *, const char *);
	void (*collection_named_item_iter)(dom_nnodemap_object *, php_dom_obj_map_collection_iter *);
	bool use_cache;
	bool nameless;
} php_dom_obj_map_handler;

typedef struct dom_nnodemap_object {
	dom_object *baseobj;
	zend_long cached_length;
	union {
		xmlHashTable *ht;
		HashTable *array;
		struct {
			xmlChar *local;
			zend_string *local_lower;
			xmlChar *ns;
		};
	};
	php_libxml_cache_tag cache_tag;
	dom_object *cached_obj;
	zend_long cached_obj_index;
	xmlDictPtr dict;
	const php_dom_obj_map_handler *handler;
	bool release_local;
	bool release_ns;
	bool release_array;
} dom_nnodemap_object;

void php_dom_create_obj_map(dom_object *basenode, dom_object *intern, xmlHashTablePtr ht, zend_string *local, zend_string *ns, const php_dom_obj_map_handler *handler);
void php_dom_obj_map_get_ns_named_item_into_zval(dom_nnodemap_object *objmap, const zend_string *named, const char *ns, zval *return_value);
void php_dom_obj_map_get_item_into_zval(dom_nnodemap_object *objmap, zend_long index, zval *return_value);
zend_long php_dom_get_nodelist_length(dom_object *obj);

extern const php_dom_obj_map_handler php_dom_obj_map_attributes;
extern const php_dom_obj_map_handler php_dom_obj_map_by_tag_name;
extern const php_dom_obj_map_handler php_dom_obj_map_by_class_name;
extern const php_dom_obj_map_handler php_dom_obj_map_child_elements;
extern const php_dom_obj_map_handler php_dom_obj_map_child_nodes;
extern const php_dom_obj_map_handler php_dom_obj_map_nodeset;
extern const php_dom_obj_map_handler php_dom_obj_map_entities;
extern const php_dom_obj_map_handler php_dom_obj_map_notations;
extern const php_dom_obj_map_handler php_dom_obj_map_noop;

#endif
