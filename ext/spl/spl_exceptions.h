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

#ifndef SPL_EXCEPTIONS_H
#define SPL_EXCEPTIONS_H

#include "php.h"

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
