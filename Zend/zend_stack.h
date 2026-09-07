/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_STACK_H
#define ZEND_STACK_H

#include "zend_portability.h"

typedef struct _zend_stack {
	int size, top, max;
	void *elements;
} zend_stack;


#define STACK_BLOCK_SIZE 16

typedef enum {
	ZEND_STACK_APPLY_TOPDOWN,
	ZEND_STACK_APPLY_BOTTOMUP,
} zend_stack_apply_direction;

BEGIN_EXTERN_C()
ZEND_API void zend_stack_init(zend_stack *stack, int size);
ZEND_API int zend_stack_push(zend_stack *stack, const void *element);
ZEND_API void *zend_stack_top(const zend_stack *stack);
ZEND_API void zend_stack_del_top(zend_stack *stack);
ZEND_API int zend_stack_int_top(const zend_stack *stack);
ZEND_API bool zend_stack_is_empty(const zend_stack *stack);
ZEND_API void zend_stack_destroy(zend_stack *stack);
ZEND_API void *zend_stack_base(const zend_stack *stack);
ZEND_API int zend_stack_count(const zend_stack *stack);
ZEND_API void zend_stack_apply(zend_stack *stack, int type, int (*apply_function)(void *element));
ZEND_API void zend_stack_apply_with_argument(zend_stack *stack, zend_stack_apply_direction type, int (*apply_function)(void *element, void *arg), void *arg);
ZEND_API void zend_stack_clean(zend_stack *stack, void (*func)(void *), bool free_elements);
END_EXTERN_C()

#endif /* ZEND_STACK_H */
