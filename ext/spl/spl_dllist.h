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

#ifndef SPL_DLLIST_H
#define SPL_DLLIST_H

#include "php.h"

#define SPL_DLLIST_IT_KEEP   0x00000000
#define SPL_DLLIST_IT_FIFO   0x00000000 /* FIFO flag makes the iterator traverse the structure as a FirstInFirstOut */
#define SPL_DLLIST_IT_DELETE 0x00000001 /* Delete flag makes the iterator delete the current element on next */
#define SPL_DLLIST_IT_LIFO   0x00000002 /* LIFO flag makes the iterator traverse the structure as a LastInFirstOut */
#define SPL_DLLIST_IT_MASK   0x00000003 /* Mask to isolate flags related to iterators */
#define SPL_DLLIST_IT_FIX    0x00000004 /* Backward/Forward bit is fixed */

extern PHPAPI zend_class_entry *spl_ce_SplDoublyLinkedList;
extern PHPAPI zend_class_entry *spl_ce_SplQueue;
extern PHPAPI zend_class_entry *spl_ce_SplStack;

PHP_MINIT_FUNCTION(spl_dllist);

#endif /* SPL_DLLIST_H */
