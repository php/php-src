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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_OBSERVER_H
#define SPL_OBSERVER_H

#include "php.h"

typedef enum {
	MIT_NEED_ANY     = 0,
	MIT_NEED_ALL     = 1,
	MIT_KEYS_NUMERIC = 0,
	MIT_KEYS_ASSOC   = 2
} MultipleIteratorFlags;

extern PHPAPI zend_class_entry *spl_ce_SplObserver;
extern PHPAPI zend_class_entry *spl_ce_SplSubject;
extern PHPAPI zend_class_entry *spl_ce_SplObjectStorage;
extern PHPAPI zend_class_entry *spl_ce_MultipleIterator;

PHP_MINIT_FUNCTION(spl_observer);

#endif /* SPL_OBSERVER_H */
