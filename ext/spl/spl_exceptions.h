/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef SPL_EXCEPTIONS_H
#define SPL_EXCEPTIONS_H

#include "php.h"
#include "php_spl.h"

extern PHPAPI zend_class_entry *spl_ce_LogicException;
extern PHPAPI zend_class_entry *spl_ce_BadFunctionCallException;
extern PHPAPI zend_class_entry *spl_ce_BadMethodCallException;
extern PHPAPI zend_class_entry *spl_ce_DomainException;
extern PHPAPI zend_class_entry *spl_ce_InvalidArgumentException;
extern PHPAPI zend_class_entry *spl_ce_LengthException;
extern PHPAPI zend_class_entry *spl_ce_OutOfRangeException;

extern PHPAPI zend_class_entry *spl_ce_RuntimeException;
extern PHPAPI zend_class_entry *spl_ce_OutOfBoundsException;
extern PHPAPI zend_class_entry *spl_ce_OverflowException;
extern PHPAPI zend_class_entry *spl_ce_RangeException;
extern PHPAPI zend_class_entry *spl_ce_UnderflowException;
extern PHPAPI zend_class_entry *spl_ce_UnexpectedValueException;

PHP_MINIT_FUNCTION(spl_exceptions);

#endif /* SPL_EXCEPTIONS_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
