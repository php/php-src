/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

/* $Id$ */

#ifndef SPL_DLLIST_H
#define SPL_DLLIST_H

#include "php.h"
#include "php_spl.h"

extern PHPAPI zend_class_entry *spl_ce_SplDoublyLinkedList;
extern PHPAPI zend_class_entry *spl_ce_SplQueue;
extern PHPAPI zend_class_entry *spl_ce_SplStack;

PHP_MINIT_FUNCTION(spl_dllist);

#endif /* SPL_DLLIST_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
