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


#include "zend.h"
#include "zend_ptr_stack.h"


ZEND_API void zend_ptr_stack_init(zend_ptr_stack *stack)
{
	stack->top_element = stack->elements = (void **) emalloc(sizeof(void *)*STACK_BLOCK_SIZE);
	stack->max = STACK_BLOCK_SIZE;
	stack->top = 0;
}


ZEND_API inline void zend_ptr_stack_push(zend_ptr_stack *stack, void *ptr)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		short diff = stack->top_element-stack->elements;
		
		stack->elements = (void **) erealloc(stack->elements, (sizeof(void *) * (stack->max *= 2 )));
		stack->top_element = stack->elements+diff;
	}
	stack->top++;
	*(stack->top_element++) = ptr;
}


ZEND_API inline void *zend_ptr_stack_pop(zend_ptr_stack *stack)
{
	stack->top--;
	return *(--stack->top_element);
}


ZEND_API void zend_ptr_stack_destroy(zend_ptr_stack *stack)
{
	if (stack->elements) {
		efree(stack->elements);
	}
}


ZEND_API void zend_ptr_stack_apply(zend_ptr_stack *stack, void (*func)(void *))
{
	int i = stack->top;

	while (--i >= 0) {
		func(stack->elements[i]);
	}
}


ZEND_API void zend_ptr_stack_clean(zend_ptr_stack *stack, void (*func)(void *))
{
	zend_ptr_stack_apply(stack, func);
	stack->top = 0;
	stack->top_element = stack->elements;
}


