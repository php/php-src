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

#ifndef ZEND_PTR_STACK_H
#define ZEND_PTR_STACK_H

typedef struct _zend_ptr_stack {
	int top, max;
	void **elements;
	void **top_element;
	zend_bool persistent;
} zend_ptr_stack;


#define PTR_STACK_BLOCK_SIZE 64

BEGIN_EXTERN_C()
ZEND_API void zend_ptr_stack_init(zend_ptr_stack *stack);
ZEND_API void zend_ptr_stack_init_ex(zend_ptr_stack *stack, zend_bool persistent);
ZEND_API void zend_ptr_stack_n_push(zend_ptr_stack *stack, int count, ...);
ZEND_API void zend_ptr_stack_n_pop(zend_ptr_stack *stack, int count, ...);
ZEND_API void zend_ptr_stack_destroy(zend_ptr_stack *stack);
ZEND_API void zend_ptr_stack_apply(zend_ptr_stack *stack, void (*func)(void *));
ZEND_API void zend_ptr_stack_clean(zend_ptr_stack *stack, void (*func)(void *), zend_bool free_elements);
ZEND_API int zend_ptr_stack_num_elements(zend_ptr_stack *stack);
END_EXTERN_C()

#define ZEND_PTR_STACK_RESIZE_IF_NEEDED(stack, count)		\
	if (stack->top+count > stack->max) {					\
		/* we need to allocate more memory */				\
		do {												\
			stack->max += PTR_STACK_BLOCK_SIZE;				\
		} while (stack->top+count > stack->max);			\
		stack->elements = (void **) perealloc(stack->elements, (sizeof(void *) * (stack->max)), stack->persistent);	\
		stack->top_element = stack->elements+stack->top;	\
	}

/*	Not doing this with a macro because of the loop unrolling in the element assignment.
	Just using a macro for 3 in the body for readability sake. */
static zend_always_inline void zend_ptr_stack_3_push(zend_ptr_stack *stack, void *a, void *b, void *c)
{
#define ZEND_PTR_STACK_NUM_ARGS 3

	ZEND_PTR_STACK_RESIZE_IF_NEEDED(stack, ZEND_PTR_STACK_NUM_ARGS)

	stack->top += ZEND_PTR_STACK_NUM_ARGS;
	*(stack->top_element++) = a;
	*(stack->top_element++) = b;
	*(stack->top_element++) = c;

#undef ZEND_PTR_STACK_NUM_ARGS
}

static zend_always_inline void zend_ptr_stack_2_push(zend_ptr_stack *stack, void *a, void *b)
{
#define ZEND_PTR_STACK_NUM_ARGS 2

	ZEND_PTR_STACK_RESIZE_IF_NEEDED(stack, ZEND_PTR_STACK_NUM_ARGS)

	stack->top += ZEND_PTR_STACK_NUM_ARGS;
	*(stack->top_element++) = a;
	*(stack->top_element++) = b;

#undef ZEND_PTR_STACK_NUM_ARGS
}

static zend_always_inline void zend_ptr_stack_3_pop(zend_ptr_stack *stack, void **a, void **b, void **c)
{
	*a = *(--stack->top_element);
	*b = *(--stack->top_element);
	*c = *(--stack->top_element);
	stack->top -= 3;
}

static zend_always_inline void zend_ptr_stack_2_pop(zend_ptr_stack *stack, void **a, void **b)
{
	*a = *(--stack->top_element);
	*b = *(--stack->top_element);
	stack->top -= 2;
}

static zend_always_inline void zend_ptr_stack_push(zend_ptr_stack *stack, void *ptr)
{
	ZEND_PTR_STACK_RESIZE_IF_NEEDED(stack, 1)

	stack->top++;
	*(stack->top_element++) = ptr;
}

static zend_always_inline void *zend_ptr_stack_pop(zend_ptr_stack *stack)
{
	stack->top--;
	return *(--stack->top_element);
}

static inline void *zend_ptr_stack_top(zend_ptr_stack *stack)
{
    return stack->elements[stack->top - 1];
}

#endif /* ZEND_PTR_STACK_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
