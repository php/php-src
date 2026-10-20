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

#include "zend.h"
#include "zend_ptr_stack.h"
#include <stdarg.h>

ZEND_API void zend_ptr_stack_init_ex(zend_ptr_stack *stack, bool persistent)
{
	stack->top_element = stack->elements = NULL;
	stack->top = stack->max = 0;
	stack->persistent = persistent;
}

ZEND_API void zend_ptr_stack_init(zend_ptr_stack *stack)
{
	zend_ptr_stack_init_ex(stack, false);
}


ZEND_API void zend_ptr_stack_n_push(zend_ptr_stack *stack, int count, ...)
{
	va_list ptr;
	void *elem;

	ZEND_PTR_STACK_RESIZE_IF_NEEDED(stack, count)

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
		pefree(stack->elements, stack->persistent);
	}
}


ZEND_API void zend_ptr_stack_apply(zend_ptr_stack *stack, void (*func)(void *))
{
	int i = stack->top;

	while (--i >= 0) {
		func(stack->elements[i]);
	}
}

ZEND_API void zend_ptr_stack_reverse_apply(zend_ptr_stack *stack, void (*func)(void *))
{
	int i = 0;

	while (i < stack->top) {
		func(stack->elements[i++]);
	}
}


ZEND_API void zend_ptr_stack_clean(zend_ptr_stack *stack, void (*func)(void *), bool free_elements)
{
	zend_ptr_stack_apply(stack, func);
	if (free_elements) {
		int i = stack->top;

		while (--i >= 0) {
			pefree(stack->elements[i], stack->persistent);
		}
	}
	stack->top = 0;
	stack->top_element = stack->elements;
}


ZEND_API int zend_ptr_stack_num_elements(zend_ptr_stack *stack)
{
	return stack->top;
}
