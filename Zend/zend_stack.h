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

#ifndef _ZEND_STACK_H
#define _ZEND_STACK_H

typedef struct {
	int top, max;
	void **elements;
} zend_stack;


#define STACK_BLOCK_SIZE 64

ZEND_API int zend_stack_init(zend_stack *stack);
ZEND_API int zend_stack_push(zend_stack *stack, void *element, int size);
ZEND_API int zend_stack_top(zend_stack *stack, void **element);
ZEND_API int zend_stack_del_top(zend_stack *stack);
ZEND_API int zend_stack_int_top(zend_stack *stack);
ZEND_API int zend_stack_is_empty(zend_stack *stack);
ZEND_API int zend_stack_destroy(zend_stack *stack);
ZEND_API void **zend_stack_base(zend_stack *stack);
ZEND_API int zend_stack_count(zend_stack *stack);


#endif /* _ZEND_STACK_H */
