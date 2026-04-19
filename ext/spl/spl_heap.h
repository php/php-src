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
   | Authors: Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_HEAP_H
#define SPL_HEAP_H

#include "php.h"

#define SPL_PQUEUE_EXTR_MASK     0x00000003
#define SPL_PQUEUE_EXTR_BOTH     0x00000003
#define SPL_PQUEUE_EXTR_DATA     0x00000001
#define SPL_PQUEUE_EXTR_PRIORITY 0x00000002

extern PHPAPI zend_class_entry *spl_ce_SplHeap;
extern PHPAPI zend_class_entry *spl_ce_SplMinHeap;
extern PHPAPI zend_class_entry *spl_ce_SplMaxHeap;

extern PHPAPI zend_class_entry *spl_ce_SplPriorityQueue;

PHP_MINIT_FUNCTION(spl_heap);

#endif /* SPL_HEAP_H */
