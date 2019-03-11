/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: krakjoe@php.net                                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_WEAKREFS_H
#define ZEND_WEAKREFS_H

BEGIN_EXTERN_C()

ZEND_API zend_class_entry *zend_ce_weakref;
ZEND_API void zend_weakref_construct(zval *weakref, zval *referent);

void zend_register_weakref_ce(void);

void zend_weakrefs_init();
ZEND_API void zend_weakrefs_notify(zend_object *object);
void zend_weakrefs_shutdown();

END_EXTERN_C()

#endif

