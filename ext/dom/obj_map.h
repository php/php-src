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

typedef struct php_dom_obj_map_handler {
   zend_long (*length)(dom_nnodemap_object *);
   void (*get_item)(dom_nnodemap_object *, zend_long, zval *);
   xmlNodePtr (*get_named_item)(dom_nnodemap_object *, const zend_string *, const char *);
   bool (*has_named_item)(dom_nnodemap_object *, const zend_string *, const char *);
   bool use_cache;
   bool nameless;
} php_dom_obj_map_handler;

extern const php_dom_obj_map_handler php_dom_obj_map_attributes;
extern const php_dom_obj_map_handler php_dom_obj_map_by_tag_name;
extern const php_dom_obj_map_handler php_dom_obj_map_child_nodes;
extern const php_dom_obj_map_handler php_dom_obj_map_nodeset;
extern const php_dom_obj_map_handler php_dom_obj_map_entities;
extern const php_dom_obj_map_handler php_dom_obj_map_notations;
extern const php_dom_obj_map_handler php_dom_obj_map_noop;

#endif
