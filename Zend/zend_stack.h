/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies USA, Inc. (https://www.zend.com)     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.zend.com/sites/zend/files/pdfs/zend-engine-license-2.01.txt |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_STACK_H
#define ZEND_STACK_H

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
