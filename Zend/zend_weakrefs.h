/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE.               |
   +----------------------------------------------------------------------+
   | Authors: krakjoe@php.net                                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_WEAKREFS_H
#define ZEND_WEAKREFS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_weakref;

void zend_register_weakref_ce(void);

void zend_weakrefs_init(void);
void zend_weakrefs_shutdown(void);

ZEND_API void zend_weakrefs_notify(zend_object *object);

END_EXTERN_C()

#endif

