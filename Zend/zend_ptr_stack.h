/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _ZEND_PTR_STACK_H
#define _ZEND_PTR_STACK_H

typedef struct _zend_ptr_stack {
	short top, max;
	void **elements;
	void **top_element;
} zend_ptr_stack;


#define PTR_STACK_BLOCK_SIZE 64

ZEND_API void zend_ptr_stack_init(zend_ptr_stack *stack);
ZEND_API void zend_ptr_stack_push(zend_ptr_stack *stack, void *ptr);
ZEND_API void zend_ptr_stack_n_push(zend_ptr_stack *stack, int count, ...);
ZEND_API void *zend_ptr_stack_pop(zend_ptr_stack *stack);
ZEND_API void zend_ptr_stack_n_pop(zend_ptr_stack *stack, int count, ...);
ZEND_API void zend_ptr_stack_destroy(zend_ptr_stack *stack);
ZEND_API void zend_ptr_stack_apply(zend_ptr_stack *stack, void (*func)(void *));
ZEND_API void zend_ptr_stack_clean(zend_ptr_stack *stack, void (*func)(void *));

#endif /* _ZEND_PTR_STACK_H */
