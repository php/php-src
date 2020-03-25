/*
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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_OBSERVER_H
#define SPL_OBSERVER_H

#include "php.h"
#include "php_spl.h"

extern PHPAPI zend_class_entry *spl_ce_SplObserver;
extern PHPAPI zend_class_entry *spl_ce_SplSubject;
extern PHPAPI zend_class_entry *spl_ce_SplObjectStorage;
extern PHPAPI zend_class_entry *spl_ce_MultipleIterator;

PHP_MINIT_FUNCTION(spl_observer);

#endif /* SPL_OBSERVER_H */
