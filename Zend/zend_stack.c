/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_stack.h"

#define ZEND_STACK_ELEMENT(stack, n) ((void *)((char *) (stack)->elements + (stack)->size * (n)))

ZEND_API void zend_stack_init(zend_stack *stack, int size)
{
	stack->size = size;
	stack->top = 0;
	stack->max = 0;
	stack->elements = NULL;
}

ZEND_API int zend_stack_push(zend_stack *stack, const void *element)
{
	/* We need to allocate more memory */
	if (stack->top >= stack->max) {
		stack->max += STACK_BLOCK_SIZE;
		stack->elements = safe_erealloc(stack->elements, stack->size, stack->max, 0);
	}
	memcpy(ZEND_STACK_ELEMENT(stack, stack->top), element, stack->size);
	return stack->top++;
}


ZEND_API void *zend_stack_top(const zend_stack *stack)
{
	if (stack->top > 0) {
		return ZEND_STACK_ELEMENT(stack, stack->top - 1);
	} else {
		return NULL;
	}
}


ZEND_API void zend_stack_del_top(zend_stack *stack)
{
	--stack->top;
}


ZEND_API int zend_stack_int_top(const zend_stack *stack)
{
	int *e = zend_stack_top(stack);
	if (e) {
		return *e;
	} else {
		return FAILURE;
	}
}


ZEND_API bool zend_stack_is_empty(const zend_stack *stack)
{
	return stack->top == 0;
}


ZEND_API void zend_stack_destroy(zend_stack *stack)
{
	if (stack->elements) {
		efree(stack->elements);
		stack->elements = NULL;
	}
}


ZEND_API void *zend_stack_base(const zend_stack *stack)
{
	return stack->elements;
}


ZEND_API int zend_stack_count(const zend_stack *stack)
{
	return stack->top;
}


ZEND_API void zend_stack_apply(zend_stack *stack, int type, int (*apply_function)(void *element))
{
	int i;

	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i))) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i))) {
					break;
				}
			}
			break;
	}
}


ZEND_API void zend_stack_apply_with_argument(zend_stack *stack, zend_stack_apply_direction type, int (*apply_function)(void *element, void *arg), void *arg)
{
	int i;

	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i), arg)) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if (apply_function(ZEND_STACK_ELEMENT(stack, i), arg)) {
					break;
				}
			}
			break;
	}
}

ZEND_API void zend_stack_clean(zend_stack *stack, void (*func)(void *), bool free_elements)
{
	int i;

	if (func) {
		for (i = 0; i < stack->top; i++) {
			func(ZEND_STACK_ELEMENT(stack, i));
		}
	}
	if (free_elements) {
		if (stack->elements) {
			efree(stack->elements);
			stack->elements = NULL;
		}
		stack->top = stack->max = 0;
	}
}
