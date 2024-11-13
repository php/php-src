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

void dom_token_list_ctor(dom_token_list_object *intern, dom_object *element_obj);
void dom_token_list_free_obj(zend_object *object);
zval *dom_token_list_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
int dom_token_list_has_dimension(zend_object *object, zval *offset, int check_empty);
zend_object_iterator *dom_token_list_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

#endif
