/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
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
#include "zend_stack.h"

ZEND_API int zend_stack_init(zend_stack *stack)
{
	stack->top = 0;
	stack->max = 0;
	stack->elements = NULL;
	return SUCCESS;
}

ZEND_API int zend_stack_push(zend_stack *stack, const void *element, int size)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		stack->elements = (void **) erealloc(stack->elements,
				   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)));
		if (!stack->elements) {
			return FAILURE;
		}
	}
	stack->elements[stack->top] = (void *) emalloc(size);
	memcpy(stack->elements[stack->top], element, size);
	return stack->top++;
}


ZEND_API int zend_stack_top(const zend_stack *stack, void **element)
{
	if (stack->top > 0) {
		*element = stack->elements[stack->top - 1];
		return SUCCESS;
	} else {
		*element = NULL;
		return FAILURE;
	}
}


ZEND_API int zend_stack_del_top(zend_stack *stack)
{
	if (stack->top > 0) {
		efree(stack->elements[--stack->top]);
	}
	return SUCCESS;
}


ZEND_API int zend_stack_int_top(const zend_stack *stack)
{
	int *e;

	if (zend_stack_top(stack, (void **) &e) == FAILURE) {
		return FAILURE;			/* this must be a negative number, since negative numbers can't be address numbers */
	} else {
		return *e;
	}
}


ZEND_API int zend_stack_is_empty(const zend_stack *stack)
{
	if (stack->top == 0) {
		return 1;
	} else {
		return 0;
	}
}


ZEND_API int zend_stack_destroy(zend_stack *stack)
{
	int i;

	if (stack->elements) {
		for (i = 0; i < stack->top; i++) {
			efree(stack->elements[i]);
		}
		efree(stack->elements);
		stack->elements = NULL;
	}

	return SUCCESS;
}


ZEND_API void **zend_stack_base(const zend_stack *stack)
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
				if (apply_function(stack->elements[i])) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if (apply_function(stack->elements[i])) {
					break;
				}
			}
			break;
	}
}


ZEND_API void zend_stack_apply_with_argument(zend_stack *stack, int type, int (*apply_function)(void *element, void *arg), void *arg)
{
	int i;

	switch (type) {
		case ZEND_STACK_APPLY_TOPDOWN:
			for (i=stack->top-1; i>=0; i--) {
				if (apply_function(stack->elements[i], arg)) {
					break;
				}
			}
			break;
		case ZEND_STACK_APPLY_BOTTOMUP:
			for (i=0; i<stack->top; i++) {
				if (apply_function(stack->elements[i], arg)) {
					break;
				}
			}
			break;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
