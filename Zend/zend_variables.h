/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#ifndef _VARIABLES_H
#define _VARIABLES_H


ZEND_API int zend_print_variable(zval *var);

BEGIN_EXTERN_C()
ZEND_API int zval_copy_ctor(zval *zvalue);
ZEND_API int zval_dtor(zval *zvalue);
END_EXTERN_C()

ZEND_API int zval_ptr_dtor(zval **zval_ptr);
void zval_add_ref(zval **p);

#define PVAL_DESTRUCTOR (int (*)(void *)) zval_dtor
#define PVAL_PTR_DTOR (int (*)(void *)) zval_ptr_dtor
#define PVAL_COPY_CTOR (void (*)(void *)) zval_copy_ctor

ZEND_API void var_reset(zval *var);
ZEND_API void var_uninit(zval *var);

#endif
