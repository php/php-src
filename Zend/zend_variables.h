/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.90 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_90.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
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
