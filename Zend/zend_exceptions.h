/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_DEFAULT_CLASSES_H
#define ZEND_DEFAULT_CLASSES_H

BEGIN_EXTERN_C()

ZEND_API zend_class_entry *zend_exception_get_default(void);
ZEND_API void zend_register_default_classes(TSRMLS_D);

/* exception_ce   NULL or zend_exception_get_default() or a derived class 
 * message        NULL or the message of the exception */
ZEND_API void zend_throw_exception(zend_class_entry *exception_ce, char *message, long code TSRMLS_DC);
ZEND_API void zend_throw_exception_ex(zend_class_entry *exception_ce, long code TSRMLS_DC, char *format, ...);

/* show an exception using zend_error(E_ERROR,...) */
ZEND_API void zend_exception_error(zval *exception TSRMLS_DC);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
