/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

typedef struct dom_token_list_object {
	HashTable token_set;
	/* Used to check if the token set is up to date. */
	char *cached_string;
	php_libxml_cache_tag cache_tag;
	dom_object dom;
} dom_token_list_object;

static inline dom_token_list_object *php_dom_token_list_from_obj(zend_object *obj)
{
	return (dom_token_list_object *)((char *) obj - XtOffsetOf(dom_token_list_object, dom.std));
}

static inline dom_token_list_object *php_dom_token_list_from_dom_obj(dom_object *obj)
{
	return (dom_token_list_object *)((char *) obj - XtOffsetOf(dom_token_list_object, dom));
}

void dom_ordered_set_parser(HashTable *token_set, const char *position, bool to_lowercase);
bool dom_ordered_set_all_contained(HashTable *token_set, const char *value, bool to_lowercase);
void dom_token_list_ctor(dom_token_list_object *intern, dom_object *element_obj);
void dom_token_list_free_obj(zend_object *object);
zval *dom_token_list_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
int dom_token_list_has_dimension(zend_object *object, zval *offset, int check_empty);
zend_object_iterator *dom_token_list_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

#endif
