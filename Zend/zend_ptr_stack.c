/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_ptr_stack.h"
#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

ZEND_API void zend_ptr_stack_init(zend_ptr_stack *stack)
{
	stack->top_element = stack->elements = (void **) emalloc(sizeof(void *)*PTR_STACK_BLOCK_SIZE);
	stack->max = PTR_STACK_BLOCK_SIZE;
	stack->top = 0;
}


ZEND_API void zend_ptr_stack_n_push(zend_ptr_stack *stack, int count, ...)
{
	va_list ptr;
	void *elem;
	
	if (stack->top+count > stack->max) {		/* we need to allocate more memory */
		stack->max *= 2;
		stack->max += count; 
		stack->elements = (void **) erealloc(stack->elements, (sizeof(void *) * (stack->max)));
		stack->top_element = stack->elements+stack->top;
	}
	va_start(ptr, count);
	while (count>0) {
		elem = va_arg(ptr, void *);
		stack->top++;
		*(stack->top_element++) = elem;
		count--;
	}
	va_end(ptr);
}


ZEND_API void zend_ptr_stack_n_pop(zend_ptr_stack *stack, int count, ...)
{
	va_list ptr;
	void **elem;
	
	va_start(ptr, count);
	while (count>0) {
		elem = va_arg(ptr, void **);
		*elem = *(--stack->top_element);
		stack->top--;
		count--;
	}
	va_end(ptr);
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


ZEND_API void zend_ptr_stack_clean(zend_ptr_stack *stack, void (*func)(void *), zend_bool free_elements)
{
	zend_ptr_stack_apply(stack, func);
	if (free_elements) {
		int i = stack->top;

		while (--i >= 0) {
			efree(stack->elements[i]);
		}
	}
	stack->top = 0;
	stack->top_element = stack->elements;
}


ZEND_API int zend_ptr_stack_num_elements(zend_ptr_stack *stack)
{
	return stack->top;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
