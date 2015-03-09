/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef SPL_ARRAY_H
#define SPL_ARRAY_H

#include "php.h"
#include "php_spl.h"
#include "spl_iterators.h"

extern PHPAPI zend_class_entry *spl_ce_ArrayObject;
extern PHPAPI zend_class_entry *spl_ce_ArrayIterator;
extern PHPAPI zend_class_entry *spl_ce_RecursiveArrayIterator;

PHP_MINIT_FUNCTION(spl_array);

extern void spl_array_iterator_append(zval *object, zval *append_value);
extern void spl_array_iterator_key(zval *object, zval *return_value);

#endif /* SPL_ARRAY_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
