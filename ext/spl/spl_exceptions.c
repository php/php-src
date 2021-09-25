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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "spl_exceptions_arginfo.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_exceptions.h"

PHPAPI zend_class_entry *spl_ce_LogicException;
PHPAPI zend_class_entry *spl_ce_BadFunctionCallException;
PHPAPI zend_class_entry *spl_ce_BadMethodCallException;
PHPAPI zend_class_entry *spl_ce_DomainException;
PHPAPI zend_class_entry *spl_ce_InvalidArgumentException;
PHPAPI zend_class_entry *spl_ce_LengthException;
PHPAPI zend_class_entry *spl_ce_OutOfRangeException;
PHPAPI zend_class_entry *spl_ce_RuntimeException;
PHPAPI zend_class_entry *spl_ce_OutOfBoundsException;
PHPAPI zend_class_entry *spl_ce_OverflowException;
PHPAPI zend_class_entry *spl_ce_RangeException;
PHPAPI zend_class_entry *spl_ce_UnderflowException;
PHPAPI zend_class_entry *spl_ce_UnexpectedValueException;

#define spl_ce_Exception zend_ce_exception

/* {{{ PHP_MINIT_FUNCTION(spl_exceptions) */
PHP_MINIT_FUNCTION(spl_exceptions)
{
	spl_ce_LogicException = register_class_LogicException(zend_ce_exception);
	spl_ce_BadFunctionCallException = register_class_BadFunctionCallException(spl_ce_LogicException);
	spl_ce_BadMethodCallException = register_class_BadMethodCallException(spl_ce_BadFunctionCallException);
	spl_ce_DomainException = register_class_DomainException(spl_ce_LogicException);
	spl_ce_InvalidArgumentException = register_class_InvalidArgumentException(spl_ce_LogicException);
	spl_ce_LengthException = register_class_LengthException(spl_ce_LogicException);
	spl_ce_OutOfRangeException = register_class_OutOfRangeException(spl_ce_LogicException);

	spl_ce_RuntimeException = register_class_RuntimeException(zend_ce_exception);
	spl_ce_OutOfBoundsException = register_class_OutOfBoundsException(spl_ce_RuntimeException);
	spl_ce_OverflowException = register_class_OverflowException(spl_ce_RuntimeException);
	spl_ce_RangeException = register_class_RangeException(spl_ce_RuntimeException);
	spl_ce_UnderflowException = register_class_UnderflowException(spl_ce_RuntimeException);
	spl_ce_UnexpectedValueException = register_class_UnexpectedValueException(spl_ce_RuntimeException);

	return SUCCESS;
}
/* }}} */
