/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_HEAP_H
#define SPL_HEAP_H

#include "php.h"
#include "php_spl.h"

extern PHPAPI zend_class_entry *spl_ce_SplHeap;
extern PHPAPI zend_class_entry *spl_ce_SplMinHeap;
extern PHPAPI zend_class_entry *spl_ce_SplMaxHeap;

extern PHPAPI zend_class_entry *spl_ce_SplPriorityQueue;

PHP_MINIT_FUNCTION(spl_heap);

#endif /* SPL_HEAP_H */
