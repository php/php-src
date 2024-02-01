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
   | Authors: Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_DLLIST_H
#define SPL_DLLIST_H

#include "php.h"
#include "php_spl.h"

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
