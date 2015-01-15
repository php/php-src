/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_heap.h"
#include "spl_exceptions.h"

#define PTR_HEAP_BLOCK_SIZE 64

#define SPL_HEAP_CORRUPTED       0x00000001

#define SPL_PQUEUE_EXTR_MASK     0x00000003
#define SPL_PQUEUE_EXTR_BOTH     0x00000003
#define SPL_PQUEUE_EXTR_DATA     0x00000001
#define SPL_PQUEUE_EXTR_PRIORITY 0x00000002

zend_object_handlers spl_handler_SplHeap;
zend_object_handlers spl_handler_SplPriorityQueue;

PHPAPI zend_class_entry  *spl_ce_SplHeap;
PHPAPI zend_class_entry  *spl_ce_SplMaxHeap;
PHPAPI zend_class_entry  *spl_ce_SplMinHeap;
PHPAPI zend_class_entry  *spl_ce_SplPriorityQueue;


typedef void (*spl_ptr_heap_dtor_func)(zval *);
typedef void (*spl_ptr_heap_ctor_func)(zval *);
typedef int  (*spl_ptr_heap_cmp_func)(zval *, zval *, zval *);

typedef struct _spl_ptr_heap {
	zval                    *elements;
	spl_ptr_heap_ctor_func  ctor;
	spl_ptr_heap_dtor_func  dtor;
	spl_ptr_heap_cmp_func   cmp;
	int                     count;
	int                     max_size;
	int                     flags;
} spl_ptr_heap;

typedef struct _spl_heap_object spl_heap_object;
typedef struct _spl_heap_it spl_heap_it;

struct _spl_heap_object {
	spl_ptr_heap       *heap;
	zval                retval;
	int                 flags;
	zend_class_entry   *ce_get_iterator;
	zend_function      *fptr_cmp;
	zend_function      *fptr_count;
	HashTable          *debug_info;
	zend_object         std;
};

/* define an overloaded iterator structure */
struct _spl_heap_it {
	zend_user_iterator  intern;
	int                 flags;
};

static inline spl_heap_object *spl_heap_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_heap_object*)((char*)(obj) - XtOffsetOf(spl_heap_object, std));
}
/* }}} */

#define Z_SPLHEAP_P(zv)  spl_heap_from_obj(Z_OBJ_P((zv)))

static void spl_ptr_heap_zval_dtor(zval *elem) { /* {{{ */
	if (!Z_ISUNDEF_P(elem)) {
		zval_ptr_dtor(elem);
	}
}
/* }}} */

static void spl_ptr_heap_zval_ctor(zval *elem) { /* {{{ */
	if (Z_REFCOUNTED_P(elem)) {
		Z_ADDREF_P(elem);
	}
}
/* }}} */

static int spl_ptr_heap_cmp_cb_helper(zval *object, spl_heap_object *heap_object, zval *a, zval *b, zend_long *result) { /* {{{ */
	zval zresult;

	zend_call_method_with_2_params(object, heap_object->std.ce, &heap_object->fptr_cmp, "compare", &zresult, a, b);

	if (EG(exception)) {
		return FAILURE;
	}

	convert_to_long(&zresult);
	*result = Z_LVAL(zresult);

	zval_ptr_dtor(&zresult);

	return SUCCESS;
}
/* }}} */

static zval *spl_pqueue_extract_helper(zval *value, int flags) /* {{{ */
{
	if ((flags & SPL_PQUEUE_EXTR_BOTH) == SPL_PQUEUE_EXTR_BOTH) {
		return value;
	} else if ((flags & SPL_PQUEUE_EXTR_BOTH) > 0) {
		if ((flags & SPL_PQUEUE_EXTR_DATA) == SPL_PQUEUE_EXTR_DATA) {
			zval *data;
			if ((data = zend_hash_str_find(Z_ARRVAL_P(value), "data", sizeof("data") - 1)) != NULL) {
				return data;
			}
		} else {
			zval *priority;
			if ((priority = zend_hash_str_find(Z_ARRVAL_P(value), "priority", sizeof("priority") - 1)) != NULL) {
				return priority;
			}
		}
	}

	return NULL;
}
/* }}} */

static int spl_ptr_heap_zval_max_cmp(zval *a, zval *b, zval *object) { /* {{{ */
	zval result;

	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = Z_SPLHEAP_P(object);
		if (heap_object->fptr_cmp) {
			zend_long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper(object, heap_object, a, b, &lval) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval > 0 ? 1 : (lval < 0 ? -1 : 0);
		}
	}

	compare_function(&result, a, b);
	return (int)Z_LVAL(result);
}
/* }}} */

static int spl_ptr_heap_zval_min_cmp(zval *a, zval *b, zval *object) { /* {{{ */
	zval result;

	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = Z_SPLHEAP_P(object);
		if (heap_object->fptr_cmp) {
			zend_long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper(object, heap_object, a, b, &lval) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval > 0 ? 1 : (lval < 0 ? -1 : 0);
		}
	}

	compare_function(&result, b, a);
	return (int)Z_LVAL(result);
}
/* }}} */

static int spl_ptr_pqueue_zval_cmp(zval *a, zval *b, zval *object) { /* {{{ */
	zval result;
	zval *a_priority_p = spl_pqueue_extract_helper(a, SPL_PQUEUE_EXTR_PRIORITY);
	zval *b_priority_p = spl_pqueue_extract_helper(b, SPL_PQUEUE_EXTR_PRIORITY);

	if ((!a_priority_p) || (!b_priority_p)) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		return 0;
	}

	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = Z_SPLHEAP_P(object);
		if (heap_object->fptr_cmp) {
			zend_long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper((zval *)object, heap_object, a_priority_p, b_priority_p, &lval) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval > 0 ? 1 : (lval < 0 ? -1 : 0);
		}
	}

	compare_function(&result, a_priority_p, b_priority_p);
	return (int)Z_LVAL(result);
}
/* }}} */

static spl_ptr_heap *spl_ptr_heap_init(spl_ptr_heap_cmp_func cmp, spl_ptr_heap_ctor_func ctor, spl_ptr_heap_dtor_func dtor) /* {{{ */
{
	spl_ptr_heap *heap = emalloc(sizeof(spl_ptr_heap));

	heap->dtor     = dtor;
	heap->ctor     = ctor;
	heap->cmp      = cmp;
	heap->elements = ecalloc(PTR_HEAP_BLOCK_SIZE, sizeof(zval));
	heap->max_size = PTR_HEAP_BLOCK_SIZE;
	heap->count    = 0;
	heap->flags    = 0;

	return heap;
}
/* }}} */

static void spl_ptr_heap_insert(spl_ptr_heap *heap, zval *elem, void *cmp_userdata) { /* {{{ */
	int i;

	if (heap->count+1 > heap->max_size) {
		/* we need to allocate more memory */
		heap->elements  = erealloc(heap->elements, heap->max_size * 2 * sizeof(zval));
		memset(heap->elements + heap->max_size, 0, heap->max_size * sizeof(zval));
		heap->max_size *= 2;
	}

	heap->ctor(elem);

	/* sifting up */
	for (i = heap->count++; i > 0 && heap->cmp(&heap->elements[(i-1)/2], elem, cmp_userdata) < 0; i = (i-1)/2) {
		heap->elements[i] = heap->elements[(i-1)/2];
	}

	if (EG(exception)) {
		/* exception thrown during comparison */
		heap->flags |= SPL_HEAP_CORRUPTED;
	}

	ZVAL_COPY_VALUE(&heap->elements[i], elem);
}
/* }}} */

static zval *spl_ptr_heap_top(spl_ptr_heap *heap) { /* {{{ */
	if (heap->count == 0) {
		return NULL;
	}

	return Z_ISUNDEF(heap->elements[0])? NULL : &heap->elements[0];
}
/* }}} */

static void spl_ptr_heap_delete_top(spl_ptr_heap *heap, zval *elem, void *cmp_userdata) { /* {{{ */
	int i, j;
	const int limit = (heap->count-1)/2;
	zval *bottom;

	if (heap->count == 0) {
		ZVAL_UNDEF(elem);
		return;
	}

	ZVAL_COPY_VALUE(elem, &heap->elements[0]);
	bottom = &heap->elements[--heap->count];

	for (i = 0; i < limit; i = j) {
		/* Find smaller child */
		j = i * 2 + 1;
		if(j != heap->count && heap->cmp(&heap->elements[j+1], &heap->elements[j], cmp_userdata) > 0) {
			j++; /* next child is bigger */
		}

		/* swap elements between two levels */
		if(heap->cmp(bottom, &heap->elements[j], cmp_userdata) < 0) {
			heap->elements[i] = heap->elements[j];
		} else {
			break;
		}
	}

	if (EG(exception)) {
		/* exception thrown during comparison */
		heap->flags |= SPL_HEAP_CORRUPTED;
	}

	ZVAL_COPY_VALUE(&heap->elements[i], bottom);
	heap->dtor(elem);
}
/* }}} */

static spl_ptr_heap *spl_ptr_heap_clone(spl_ptr_heap *from) { /* {{{ */
	int i;

	spl_ptr_heap *heap = emalloc(sizeof(spl_ptr_heap));

	heap->dtor     = from->dtor;
	heap->ctor     = from->ctor;
	heap->cmp      = from->cmp;
	heap->max_size = from->max_size;
	heap->count    = from->count;
	heap->flags    = from->flags;

	heap->elements = safe_emalloc(sizeof(zval), from->max_size, 0);
	memcpy(heap->elements, from->elements, sizeof(zval)*from->max_size);

	for (i=0; i < heap->count; ++i) {
		heap->ctor(&heap->elements[i]);
	}

	return heap;
}
/* }}} */

static void spl_ptr_heap_destroy(spl_ptr_heap *heap) { /* {{{ */
	int i;

	for (i=0; i < heap->count; ++i) {
		heap->dtor(&heap->elements[i]);
	}

	efree(heap->elements);
	efree(heap);
}
/* }}} */

static int spl_ptr_heap_count(spl_ptr_heap *heap) { /* {{{ */
	return heap->count;
}
/* }}} */
/* }}} */

zend_object_iterator *spl_heap_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

static void spl_heap_object_free_storage(zend_object *object) /* {{{ */
{
	int i;
	spl_heap_object *intern = spl_heap_from_obj(object);

	zend_object_std_dtor(&intern->std);

	for (i = 0; i < intern->heap->count; ++i) {
		if (!Z_ISUNDEF(intern->heap->elements[i])) {
			zval_ptr_dtor(&intern->heap->elements[i]);
		}
	}

	spl_ptr_heap_destroy(intern->heap);

	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}
}
/* }}} */

static zend_object *spl_heap_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig) /* {{{ */
{
	spl_heap_object   *intern;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_heap_object) + sizeof(zval) * (parent->default_properties_count - 1));

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->flags      = 0;
	intern->fptr_cmp   = NULL;
	intern->debug_info = NULL;

	if (orig) {
		spl_heap_object *other = Z_SPLHEAP_P(orig);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			int i;
			intern->heap = spl_ptr_heap_clone(other->heap);
			for (i = 0; i < intern->heap->count; ++i) {
				if (Z_REFCOUNTED(intern->heap->elements[i])) {
					Z_ADDREF(intern->heap->elements[i]);
				}
			}
		} else {
			intern->heap = other->heap;
		}

		intern->flags = other->flags;
	} else {
		intern->heap = spl_ptr_heap_init(spl_ptr_heap_zval_max_cmp, spl_ptr_heap_zval_ctor, spl_ptr_heap_zval_dtor);
	}

	intern->std.handlers = &spl_handler_SplHeap;

	while (parent) {
		if (parent == spl_ce_SplPriorityQueue) {
			intern->heap->cmp = spl_ptr_pqueue_zval_cmp;
			intern->flags = SPL_PQUEUE_EXTR_DATA;
			intern->std.handlers = &spl_handler_SplPriorityQueue;
			break;
		}

		if (parent == spl_ce_SplMinHeap) {
			intern->heap->cmp = spl_ptr_heap_zval_min_cmp;
			break;
		}

		if (parent == spl_ce_SplMaxHeap) {
			intern->heap->cmp = spl_ptr_heap_zval_max_cmp;
			break;
		}

		if (parent == spl_ce_SplHeap) {
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (!parent) { /* this must never happen */
		php_error_docref(NULL, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplHeap");
	}

	if (inherited) {
		intern->fptr_cmp = zend_hash_str_find_ptr(&class_type->function_table, "compare", sizeof("compare") - 1);
		if (intern->fptr_cmp->common.scope == parent) {
			intern->fptr_cmp = NULL;
		}
		intern->fptr_count = zend_hash_str_find_ptr(&class_type->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return &intern->std;
}
/* }}} */

static zend_object *spl_heap_object_new(zend_class_entry *class_type) /* {{{ */
{
	return spl_heap_object_new_ex(class_type, NULL, 0);
}
/* }}} */

static zend_object *spl_heap_object_clone(zval *zobject) /* {{{ */
{
	zend_object        *old_object;
	zend_object        *new_object;

	old_object  = Z_OBJ_P(zobject);
	new_object = spl_heap_object_new_ex(old_object->ce, zobject, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

static int spl_heap_object_count_elements(zval *object, zend_long *count) /* {{{ */
{
	spl_heap_object *intern = Z_SPLHEAP_P(object);

	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (!Z_ISUNDEF(rv)) {
			zval_ptr_dtor(&intern->retval);
			ZVAL_ZVAL(&intern->retval, &rv, 0, 0);
			convert_to_long(&intern->retval);
			*count = (zend_long) Z_LVAL(intern->retval);
			return SUCCESS;
		}
		*count = 0;
		return FAILURE;
	}

	*count = spl_ptr_heap_count(intern->heap);

	return SUCCESS;
}
/* }}} */

static HashTable* spl_heap_object_get_debug_info_helper(zend_class_entry *ce, zval *obj, int *is_temp) { /* {{{ */
	spl_heap_object *intern  = Z_SPLHEAP_P(obj);
	zval tmp, heap_array;
	zend_string *pnstr;
	int  i;

	*is_temp = 0;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		ZEND_INIT_SYMTABLE_EX(intern->debug_info, zend_hash_num_elements(intern->std.properties) + 1, 0);
	}

	if (intern->debug_info->u.v.nApplyCount == 0) {

		zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref);

		pnstr = spl_gen_private_prop_name(ce, "flags", sizeof("flags")-1);
		ZVAL_LONG(&tmp, intern->flags);
		zend_hash_update(intern->debug_info, pnstr, &tmp);
		zend_string_release(pnstr);

		pnstr = spl_gen_private_prop_name(ce, "isCorrupted", sizeof("isCorrupted")-1);
		ZVAL_BOOL(&tmp, intern->heap->flags&SPL_HEAP_CORRUPTED);
		zend_hash_update(intern->debug_info, pnstr, &tmp);
		zend_string_release(pnstr);

		array_init(&heap_array);

		for (i = 0; i < intern->heap->count; ++i) {
			add_index_zval(&heap_array, i, &intern->heap->elements[i]);
			if (Z_REFCOUNTED(intern->heap->elements[i])) {
				Z_ADDREF(intern->heap->elements[i]);
			}
		}

		pnstr = spl_gen_private_prop_name(ce, "heap", sizeof("heap")-1);
		zend_hash_update(intern->debug_info, pnstr, &heap_array);
		zend_string_release(pnstr);
	}

	return intern->debug_info;
}
/* }}} */

static HashTable* spl_heap_object_get_debug_info(zval *obj, int *is_temp) /* {{{ */
{
	return spl_heap_object_get_debug_info_helper(spl_ce_SplHeap, obj, is_temp);
}
/* }}} */

static HashTable* spl_pqueue_object_get_debug_info(zval *obj, int *is_temp) /* {{{ */
{
	return spl_heap_object_get_debug_info_helper(spl_ce_SplPriorityQueue, obj, is_temp);
}
/* }}} */

/* {{{ proto int SplHeap::count()
 Return the number of elements in the heap. */
SPL_METHOD(SplHeap, count)
{
	zend_long count;
	spl_heap_object *intern = Z_SPLHEAP_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	count = spl_ptr_heap_count(intern->heap);
	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto int SplHeap::isEmpty()
 Return true if the heap is empty. */
SPL_METHOD(SplHeap, isEmpty)
{
	spl_heap_object *intern = Z_SPLHEAP_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(spl_ptr_heap_count(intern->heap) == 0);
}
/* }}} */

/* {{{ proto bool SplHeap::insert(mixed $value)
	   Push $value on the heap */
SPL_METHOD(SplHeap, insert)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	if (Z_REFCOUNTED_P(value)) Z_ADDREF_P(value);
	spl_ptr_heap_insert(intern->heap, value, getThis());

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed SplHeap::extract()
	   extract the element out of the top of the heap */
SPL_METHOD(SplHeap, extract)
{
	zval value;
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	spl_ptr_heap_delete_top(intern->heap, &value, getThis());

	if (Z_ISUNDEF(value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't extract from an empty heap", 0);
		return;
	}

	RETURN_ZVAL(&value, 1, 1);
}
/* }}} */

/* {{{ proto bool SplPriorityQueue::insert(mixed $value, mixed $priority)
	   Push $value with the priority $priodiry on the priorityqueue */
SPL_METHOD(SplPriorityQueue, insert)
{
	zval *data, *priority, elem;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &data, &priority) == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	if (Z_REFCOUNTED_P(data)) Z_ADDREF_P(data);
	if (Z_REFCOUNTED_P(priority)) Z_ADDREF_P(priority);

	array_init(&elem);
	add_assoc_zval_ex(&elem, "data", sizeof("data") - 1, data);
	add_assoc_zval_ex(&elem, "priority", sizeof("priority") - 1, priority);

	spl_ptr_heap_insert(intern->heap, &elem, getThis());

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed SplPriorityQueue::extract()
	   extract the element out of the top of the priority queue */
SPL_METHOD(SplPriorityQueue, extract)
{
	zval value, *value_out;
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	spl_ptr_heap_delete_top(intern->heap, &value, getThis());

	if (Z_ISUNDEF(value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't extract from an empty heap", 0);
		return;
	}

	value_out = spl_pqueue_extract_helper(&value, intern->flags);

	if (!value_out) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		zval_ptr_dtor(&value);
		return;
	}

	if (Z_REFCOUNTED_P(value_out)) {
		Z_ADDREF_P(value_out);
	}

	RETVAL_ZVAL(value_out, 1, 0);
	zval_ptr_dtor(&value);
}
/* }}} */

/* {{{ proto mixed SplPriorityQueue::top()
	   Peek at the top element of the priority queue */
SPL_METHOD(SplPriorityQueue, top)
{
	zval *value, *value_out;
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	value = spl_ptr_heap_top(intern->heap);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty heap", 0);
		return;
	}

	value_out = spl_pqueue_extract_helper(value, intern->flags);

	if (!value_out) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		return;
	}

	RETURN_ZVAL(value_out, 1, 0);
}
/* }}} */


/* {{{ proto int SplPriorityQueue::setExtractFlags($flags)
 Set the flags of extraction*/
SPL_METHOD(SplPriorityQueue, setExtractFlags)
{
	zend_long value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	intern->flags = value & SPL_PQUEUE_EXTR_MASK;

	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto int SplPriorityQueue::getExtractFlags()
 Get the flags of extraction*/
SPL_METHOD(SplPriorityQueue, getExtractFlags)
{
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto int SplHeap::recoverFromCorruption()
 Recover from a corrupted state*/
SPL_METHOD(SplHeap, recoverFromCorruption)
{
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	intern->heap->flags = intern->heap->flags & ~SPL_HEAP_CORRUPTED;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int SplHeap::isCorrupted()
 Tells if the heap is in a corrupted state*/
SPL_METHOD(SplHeap, isCorrupted)
{
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	RETURN_BOOL(intern->heap->flags & SPL_HEAP_CORRUPTED);
}
/* }}} */

/* {{{ proto bool SplPriorityQueue::compare(mixed $a, mixed $b)
	   compare the priorities */
SPL_METHOD(SplPriorityQueue, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_max_cmp(a, b, NULL));
}
/* }}} */

/* {{{ proto mixed SplHeap::top()
	   Peek at the top element of the heap */
SPL_METHOD(SplHeap, top)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLHEAP_P(getThis());

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	value = spl_ptr_heap_top(intern->heap);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty heap", 0);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto bool SplMinHeap::compare(mixed $a, mixed $b)
	   compare the values */
SPL_METHOD(SplMinHeap, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_min_cmp(a, b, NULL));
}
/* }}} */

/* {{{ proto bool SplMaxHeap::compare(mixed $a, mixed $b)
	   compare the values */
SPL_METHOD(SplMaxHeap, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_max_cmp(a, b, NULL));
}
/* }}} */

static void spl_heap_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	spl_heap_it *iterator = (spl_heap_it *)iter;

	zend_user_it_invalidate_current(iter);
	zval_ptr_dtor(&iterator->intern.it.data);
}
/* }}} */

static void spl_heap_it_rewind(zend_object_iterator *iter) /* {{{ */
{
	/* do nothing, the iterator always points to the top element */
}
/* }}} */

static int spl_heap_it_valid(zend_object_iterator *iter) /* {{{ */
{
	return ((Z_SPLHEAP_P(&iter->data))->heap->count != 0 ? SUCCESS : FAILURE);
}
/* }}} */

static zval *spl_heap_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	spl_heap_object *object = Z_SPLHEAP_P(&iter->data);
	zval *element = &object->heap->elements[0];

	if (object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return NULL;
	}

	if (object->heap->count == 0 || Z_ISUNDEF_P(element)) {
		return NULL;
	} else {
		return element;
	}
}
/* }}} */

static zval *spl_pqueue_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	spl_heap_object *object = Z_SPLHEAP_P(&iter->data);
	zval *element = &object->heap->elements[0];

	if (object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return NULL;
	}

	if (object->heap->count == 0 || Z_ISUNDEF_P(element)) {
		return NULL;
	} else {
		zval *data = spl_pqueue_extract_helper(element, object->flags);
		if (!data) {
			zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		}
		return data;
	}
}
/* }}} */

static void spl_heap_it_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	spl_heap_object *object = Z_SPLHEAP_P(&iter->data);

	ZVAL_LONG(key, object->heap->count - 1);
}
/* }}} */

static void spl_heap_it_move_forward(zend_object_iterator *iter) /* {{{ */
{
	spl_heap_object *object = Z_SPLHEAP_P(&iter->data);
	zval elem;

	if (object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0);
		return;
	}

	spl_ptr_heap_delete_top(object->heap, &elem, &iter->data);

	zval_ptr_dtor(&elem);

	zend_user_it_invalidate_current(iter);
}
/* }}} */

/* {{{  proto int SplHeap::key()
   Return current array key */
SPL_METHOD(SplHeap, key)
{
	spl_heap_object *intern = Z_SPLHEAP_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->heap->count - 1);
}
/* }}} */

/* {{{ proto void SplHeap::next()
   Move to next entry */
SPL_METHOD(SplHeap, next)
{
	spl_heap_object *intern = Z_SPLHEAP_P(getThis());
	zval elem;
	spl_ptr_heap_delete_top(intern->heap, &elem, getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zval_ptr_dtor(&elem);
}
/* }}} */

/* {{{ proto bool SplHeap::valid()
   Check whether the datastructure contains more entries */
SPL_METHOD(SplHeap, valid)
{
	spl_heap_object *intern = Z_SPLHEAP_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->heap->count != 0);
}
/* }}} */

/* {{{ proto void SplHeap::rewind()
   Rewind the datastructure back to the start */
SPL_METHOD(SplHeap, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	/* do nothing, the iterator always points to the top element */
}
/* }}} */

/* {{{ proto mixed|NULL SplHeap::current()
   Return current datastructure entry */
SPL_METHOD(SplHeap, current)
{
	spl_heap_object *intern  = Z_SPLHEAP_P(getThis());
	zval *element = &intern->heap->elements[0];

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->heap->count || Z_ISUNDEF_P(element)) {
		RETURN_NULL();
	} else {
		RETURN_ZVAL(element, 1, 0);
	}
}
/* }}} */

/* {{{ proto mixed|NULL SplPriorityQueue::current()
   Return current datastructure entry */
SPL_METHOD(SplPriorityQueue, current)
{
	spl_heap_object  *intern  = Z_SPLHEAP_P(getThis());
	zval *element = &intern->heap->elements[0];

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->heap->count || Z_ISUNDEF_P(element)) {
		RETURN_NULL();
	} else {
		zval *data = spl_pqueue_extract_helper(element, intern->flags);

		if (!data) {
			zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
			RETURN_NULL();
		}

		RETURN_ZVAL(data, 1, 0);
	}
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_heap_it_funcs = {
	spl_heap_it_dtor,
	spl_heap_it_valid,
	spl_heap_it_get_current_data,
	spl_heap_it_get_current_key,
	spl_heap_it_move_forward,
	spl_heap_it_rewind
};

zend_object_iterator_funcs spl_pqueue_it_funcs = {
	spl_heap_it_dtor,
	spl_heap_it_valid,
	spl_pqueue_it_get_current_data,
	spl_heap_it_get_current_key,
	spl_heap_it_move_forward,
	spl_heap_it_rewind
};

zend_object_iterator *spl_heap_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	spl_heap_it     *iterator;
	spl_heap_object *heap_object = Z_SPLHEAP_P(object);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0);
		return NULL;
	}

	iterator = emalloc(sizeof(spl_heap_it));

	zend_iterator_init(&iterator->intern.it);

	ZVAL_COPY(&iterator->intern.it.data, object);
	iterator->intern.it.funcs = &spl_heap_it_funcs;
	iterator->intern.ce       = ce;
	iterator->flags           = heap_object->flags;
	ZVAL_UNDEF(&iterator->intern.value);

	return &iterator->intern.it;
}
/* }}} */

zend_object_iterator *spl_pqueue_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	spl_heap_it     *iterator;
	spl_heap_object *heap_object = Z_SPLHEAP_P(object);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0);
		return NULL;
	}

	iterator = emalloc(sizeof(spl_heap_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_COPY(&iterator->intern.it.data, object);
	iterator->intern.it.funcs = &spl_pqueue_it_funcs;
	iterator->intern.ce       = ce;
	iterator->flags           = heap_object->flags;

	ZVAL_UNDEF(&iterator->intern.value);

	return &iterator->intern.it;
}
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_heap_insert, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_heap_compare, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pqueue_insert, 0)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pqueue_setflags, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_splheap_void, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_funcs_SplMinHeap[] = {
	SPL_ME(SplMinHeap, compare, arginfo_heap_compare, ZEND_ACC_PROTECTED)
	{NULL, NULL, NULL}
};
static const zend_function_entry spl_funcs_SplMaxHeap[] = {
	SPL_ME(SplMaxHeap, compare, arginfo_heap_compare, ZEND_ACC_PROTECTED)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_funcs_SplPriorityQueue[] = {
	SPL_ME(SplPriorityQueue, compare,               arginfo_heap_compare,    ZEND_ACC_PUBLIC)
	SPL_ME(SplPriorityQueue, insert,                arginfo_pqueue_insert,   ZEND_ACC_PUBLIC)
	SPL_ME(SplPriorityQueue, setExtractFlags,       arginfo_pqueue_setflags, ZEND_ACC_PUBLIC)
	SPL_ME(SplPriorityQueue, getExtractFlags,       arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplPriorityQueue, top,                   arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplPriorityQueue, extract,               arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          count,                 arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          isEmpty,               arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          rewind,                arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplPriorityQueue, current,               arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          key,                   arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          next,                  arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          valid,                 arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          recoverFromCorruption, arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap,          isCorrupted,           arginfo_splheap_void,    ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_funcs_SplHeap[] = {
	SPL_ME(SplHeap, extract,               arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, insert,                arginfo_heap_insert, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, top,                   arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, count,                 arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, isEmpty,               arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, rewind,                arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, current,               arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, key,                   arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, next,                  arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, valid,                 arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, recoverFromCorruption, arginfo_splheap_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplHeap, isCorrupted,           arginfo_splheap_void, ZEND_ACC_PUBLIC)
	ZEND_FENTRY(compare, NULL, NULL, ZEND_ACC_PROTECTED|ZEND_ACC_ABSTRACT)
	{NULL, NULL, NULL}
};
/* }}} */

PHP_MINIT_FUNCTION(spl_heap) /* {{{ */
{
	REGISTER_SPL_STD_CLASS_EX(SplHeap, spl_heap_object_new, spl_funcs_SplHeap);
	memcpy(&spl_handler_SplHeap, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplHeap.offset         = XtOffsetOf(spl_heap_object, std);
	spl_handler_SplHeap.clone_obj      = spl_heap_object_clone;
	spl_handler_SplHeap.count_elements = spl_heap_object_count_elements;
	spl_handler_SplHeap.get_debug_info = spl_heap_object_get_debug_info;
	spl_handler_SplHeap.dtor_obj = zend_objects_destroy_object;
	spl_handler_SplHeap.free_obj = spl_heap_object_free_storage;

	REGISTER_SPL_IMPLEMENTS(SplHeap, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplHeap, Countable);

	spl_ce_SplHeap->get_iterator = spl_heap_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(SplMinHeap,           SplHeap,        spl_heap_object_new, spl_funcs_SplMinHeap);
	REGISTER_SPL_SUB_CLASS_EX(SplMaxHeap,           SplHeap,        spl_heap_object_new, spl_funcs_SplMaxHeap);

	spl_ce_SplMaxHeap->get_iterator = spl_heap_get_iterator;
	spl_ce_SplMinHeap->get_iterator = spl_heap_get_iterator;

	REGISTER_SPL_STD_CLASS_EX(SplPriorityQueue, spl_heap_object_new, spl_funcs_SplPriorityQueue);
	memcpy(&spl_handler_SplPriorityQueue, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplPriorityQueue.offset         = XtOffsetOf(spl_heap_object, std);
	spl_handler_SplPriorityQueue.clone_obj      = spl_heap_object_clone;
	spl_handler_SplPriorityQueue.count_elements = spl_heap_object_count_elements;
	spl_handler_SplPriorityQueue.get_debug_info = spl_pqueue_object_get_debug_info;
	spl_handler_SplPriorityQueue.dtor_obj = zend_objects_destroy_object;
	spl_handler_SplPriorityQueue.free_obj = spl_heap_object_free_storage;

	REGISTER_SPL_IMPLEMENTS(SplPriorityQueue, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplPriorityQueue, Countable);

	spl_ce_SplPriorityQueue->get_iterator = spl_pqueue_get_iterator;

	REGISTER_SPL_CLASS_CONST_LONG(SplPriorityQueue, "EXTR_BOTH",      SPL_PQUEUE_EXTR_BOTH);
	REGISTER_SPL_CLASS_CONST_LONG(SplPriorityQueue, "EXTR_PRIORITY",  SPL_PQUEUE_EXTR_PRIORITY);
	REGISTER_SPL_CLASS_CONST_LONG(SplPriorityQueue, "EXTR_DATA",      SPL_PQUEUE_EXTR_DATA);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

