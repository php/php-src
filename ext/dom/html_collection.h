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

#ifndef PHP_HTML_COLLECTION_H
#define PHP_HTML_COLLECTION_H

zval *dom_html_collection_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
int dom_html_collection_has_dimension(zend_object *object, zval *member, int check_empty);
HashTable *dom_html_collection_get_gc(zend_object *object, zval **table, int *n);

#endif
