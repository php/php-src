/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

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

#define spl_ce_Exception zend_exception_get_default(TSRMLS_C)

/* {{{ PHP_MINIT_FUNCTION(spl_exceptions) */
PHP_MINIT_FUNCTION(spl_exceptions)
{
    REGISTER_SPL_SUB_CLASS_EX(LogicException,           Exception,        NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(BadFunctionCallException, LogicException,   NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(BadMethodCallException,   BadFunctionCallException,   NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(DomainException,          LogicException,   NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(InvalidArgumentException, LogicException,   NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(LengthException,          LogicException,   NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(OutOfRangeException,      LogicException,   NULL, NULL);

    REGISTER_SPL_SUB_CLASS_EX(RuntimeException,         Exception,        NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(OutOfBoundsException,     RuntimeException, NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(OverflowException,        RuntimeException, NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(RangeException,           RuntimeException, NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(UnderflowException,       RuntimeException, NULL, NULL);
    REGISTER_SPL_SUB_CLASS_EX(UnexpectedValueException, RuntimeException, NULL, NULL);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
