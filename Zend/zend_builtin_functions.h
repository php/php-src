/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_BUILTIN_FUNCTIONS_H
#define ZEND_BUILTIN_FUNCTIONS_H

zend_result zend_startup_builtin_functions(void);

BEGIN_EXTERN_C()
ZEND_API void zend_fetch_debug_backtrace(zval *return_value, int skip_last, int options, int limit);
END_EXTERN_C()

#endif /* ZEND_BUILTIN_FUNCTIONS_H */
