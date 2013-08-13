/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

typedef void *spl_ptr_heap_element;

typedef void (*spl_ptr_heap_dtor_func)(spl_ptr_heap_element TSRMLS_DC);
typedef void (*spl_ptr_heap_ctor_func)(spl_ptr_heap_element TSRMLS_DC);
typedef int  (*spl_ptr_heap_cmp_func)(spl_ptr_heap_element, spl_ptr_heap_element, void* TSRMLS_DC);

typedef struct _spl_ptr_heap {
	spl_ptr_heap_element   *elements;
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
	zend_object         std;
	spl_ptr_heap       *heap;
	zval               *retval;
	int                 flags;
	zend_class_entry   *ce_get_iterator;
	zend_function      *fptr_cmp;
	zend_function      *fptr_count;
	HashTable          *debug_info;
};

/* define an overloaded iterator structure */
struct _spl_heap_it {
	zend_user_iterator  intern;
	int                 flags;
	spl_heap_object    *object;
};

/* {{{  spl_ptr_heap */
static void spl_ptr_heap_zval_dtor(spl_ptr_heap_element elem TSRMLS_DC) { /* {{{ */
	if (elem) {
		zval_ptr_dtor((zval **)&elem);
	}
}
/* }}} */

static void spl_ptr_heap_zval_ctor(spl_ptr_heap_element elem TSRMLS_DC) { /* {{{ */
	Z_ADDREF_P((zval *)elem);
}
/* }}} */

static int spl_ptr_heap_cmp_cb_helper(zval *object, spl_heap_object *heap_object, zval *a, zval *b, long *result TSRMLS_DC) { /* {{{ */
		zval *result_p = NULL;

		zend_call_method_with_2_params(&object, heap_object->std.ce, &heap_object->fptr_cmp, "compare", &result_p, a, b);

		if (EG(exception)) {
			return FAILURE;
		}

		convert_to_long(result_p);
		*result = Z_LVAL_P(result_p);

		zval_ptr_dtor(&result_p);

		return SUCCESS;
}
/* }}} */

static zval **spl_pqueue_extract_helper(zval **value, int flags) /* {{{ */
{
	if ((flags & SPL_PQUEUE_EXTR_BOTH) == SPL_PQUEUE_EXTR_BOTH) {
		return value;
	} else if ((flags & SPL_PQUEUE_EXTR_BOTH) > 0) {

		if ((flags & SPL_PQUEUE_EXTR_DATA) == SPL_PQUEUE_EXTR_DATA) {
			zval **data;
			if (zend_hash_find(Z_ARRVAL_PP(value), "data", sizeof("data"), (void **) &data) == SUCCESS) {
				return data;
			}
		} else {
			zval **priority;
			if (zend_hash_find(Z_ARRVAL_PP(value), "priority", sizeof("priority"), (void **) &priority) == SUCCESS) {
				return priority;
			}
		}
	}

	return NULL;
}
/* }}} */

static int spl_ptr_heap_zval_max_cmp(spl_ptr_heap_element a, spl_ptr_heap_element b, void* object TSRMLS_DC) { /* {{{ */
	zval result;

	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object((zval *)object TSRMLS_CC);
		if (heap_object->fptr_cmp) {
			long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper((zval *)object, heap_object, (zval *)a, (zval *)b, &lval TSRMLS_CC) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval;
		}
	}

	INIT_ZVAL(result);
	compare_function(&result, (zval *)a, (zval *)b TSRMLS_CC);
	return Z_LVAL(result);
}
/* }}} */

static int spl_ptr_heap_zval_min_cmp(spl_ptr_heap_element a, spl_ptr_heap_element b, void* object TSRMLS_DC) { /* {{{ */
	zval result;

	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object((zval *)object TSRMLS_CC);
		if (heap_object->fptr_cmp) {
			long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper((zval *)object, heap_object, (zval *)a, (zval *)b, &lval TSRMLS_CC) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval;
		}
	}

	INIT_ZVAL(result);
	compare_function(&result, (zval *)b, (zval *)a TSRMLS_CC);
	return Z_LVAL(result);
}
/* }}} */

static int spl_ptr_pqueue_zval_cmp(spl_ptr_heap_element a, spl_ptr_heap_element b, void* object TSRMLS_DC) { /* {{{ */
	zval result;
	zval **a_priority_pp = spl_pqueue_extract_helper((zval **)&a, SPL_PQUEUE_EXTR_PRIORITY);
	zval **b_priority_pp = spl_pqueue_extract_helper((zval **)&b, SPL_PQUEUE_EXTR_PRIORITY);

	if ((!a_priority_pp) || (!b_priority_pp)) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		return 0;
	}
	if (EG(exception)) {
		return 0;
	}

	if (object) {
		spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object(object TSRMLS_CC);
		if (heap_object->fptr_cmp) {
			long lval = 0;
			if (spl_ptr_heap_cmp_cb_helper((zval *)object, heap_object, *a_priority_pp, *b_priority_pp, &lval TSRMLS_CC) == FAILURE) {
				/* exception or call failure */
				return 0;
			}
			return lval;
		}
	}

	INIT_ZVAL(result);
	compare_function(&result, *a_priority_pp, *b_priority_pp TSRMLS_CC);
	return Z_LVAL(result);
}
/* }}} */

static spl_ptr_heap *spl_ptr_heap_init(spl_ptr_heap_cmp_func cmp, spl_ptr_heap_ctor_func ctor, spl_ptr_heap_dtor_func dtor) /* {{{ */
{
	spl_ptr_heap *heap = emalloc(sizeof(spl_ptr_heap));

	heap->dtor     = dtor;
	heap->ctor     = ctor;
	heap->cmp      = cmp;
	heap->elements = safe_emalloc(sizeof(spl_ptr_heap_element), PTR_HEAP_BLOCK_SIZE, 0);
	heap->max_size = PTR_HEAP_BLOCK_SIZE;
	heap->count    = 0;
	heap->flags    = 0;

	return heap;
}
/* }}} */

static void spl_ptr_heap_insert(spl_ptr_heap *heap, spl_ptr_heap_element elem, void *cmp_userdata TSRMLS_DC) { /* {{{ */
	int i;

	if (heap->count+1 > heap->max_size) {
		/* we need to allocate more memory */
		heap->elements  = (void **) safe_erealloc(heap->elements, sizeof(spl_ptr_heap_element), (heap->max_size), (sizeof(spl_ptr_heap_element) * (heap->max_size)));
		heap->max_size *= 2;
	}

	heap->ctor(elem TSRMLS_CC);

	/* sifting up */
	for(i = heap->count++; i > 0 && heap->cmp(heap->elements[(i-1)/2], elem, cmp_userdata TSRMLS_CC) < 0; i = (i-1)/2) {
		heap->elements[i] = heap->elements[(i-1)/2];
	}

	if (EG(exception)) {
		/* exception thrown during comparison */
		heap->flags |= SPL_HEAP_CORRUPTED;
	}

	heap->elements[i] = elem;

}
/* }}} */

static spl_ptr_heap_element spl_ptr_heap_top(spl_ptr_heap *heap) { /* {{{ */
	if (heap->count == 0) {
		return NULL;
	}

	return heap->elements[0];
}
/* }}} */

static spl_ptr_heap_element spl_ptr_heap_delete_top(spl_ptr_heap *heap, void *cmp_userdata TSRMLS_DC) { /* {{{ */
	int i, j;
	const int limit = (heap->count-1)/2;
	spl_ptr_heap_element top;
	spl_ptr_heap_element bottom;

	if (heap->count == 0) {
		return NULL;
	}

	top    = heap->elements[0];
	bottom = heap->elements[--heap->count];

	for( i = 0; i < limit; i = j)
	{
		/* Find smaller child */
		j = i*2+1;
		if(j != heap->count && heap->cmp(heap->elements[j+1], heap->elements[j], cmp_userdata TSRMLS_CC) > 0) {
			j++; /* next child is bigger */
		}

		/* swap elements between two levels */
		if(heap->cmp(bottom, heap->elements[j], cmp_userdata TSRMLS_CC) < 0) {
			heap->elements[i] = heap->elements[j];
		} else {
			break;
		}
	}

	if (EG(exception)) {
		/* exception thrown during comparison */
		heap->flags |= SPL_HEAP_CORRUPTED;
	}

	heap->elements[i] = bottom;
	heap->dtor(top TSRMLS_CC);
	return top;
}
/* }}} */

static spl_ptr_heap *spl_ptr_heap_clone(spl_ptr_heap *from TSRMLS_DC) { /* {{{ */
	int i;

	spl_ptr_heap *heap = emalloc(sizeof(spl_ptr_heap));

	heap->dtor     = from->dtor;
	heap->ctor     = from->ctor;
	heap->cmp      = from->cmp;
	heap->max_size = from->max_size;
	heap->count    = from->count;
	heap->flags    = from->flags;

	heap->elements = safe_emalloc(sizeof(spl_ptr_heap_element),from->max_size,0);
	memcpy(heap->elements, from->elements, sizeof(spl_ptr_heap_element)*from->max_size);

	for (i=0; i < heap->count; ++i) {
		heap->ctor(heap->elements[i] TSRMLS_CC);
	}

	return heap;
}
/* }}} */

static void spl_ptr_heap_destroy(spl_ptr_heap *heap TSRMLS_DC) { /* {{{ */
	int i;

	for (i=0; i < heap->count; ++i) {
		heap->dtor(heap->elements[i] TSRMLS_CC);
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

zend_object_iterator *spl_heap_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

static void spl_heap_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	int i;
	spl_heap_object *intern = (spl_heap_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	for (i = 0; i < intern->heap->count; ++i) {
		if (intern->heap->elements[i]) {
			zval_ptr_dtor((zval **)&intern->heap->elements[i]);
		}
	}

	spl_ptr_heap_destroy(intern->heap TSRMLS_CC);

	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}

	efree(object);
}
/* }}} */

static zend_object_value spl_heap_object_new_ex(zend_class_entry *class_type, spl_heap_object **obj, zval *orig, int clone_orig TSRMLS_DC) /* {{{ */
{
	zend_object_value  retval;
	spl_heap_object   *intern;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_heap_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	intern->flags      = 0;
	intern->fptr_cmp   = NULL;
	intern->debug_info = NULL;

	if (orig) {
		spl_heap_object *other = (spl_heap_object*)zend_object_store_get_object(orig TSRMLS_CC);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			int i;
			intern->heap = spl_ptr_heap_clone(other->heap TSRMLS_CC);
			for (i = 0; i < intern->heap->count; ++i) {
				if (intern->heap->elements[i]) {
					Z_ADDREF_P((zval *)intern->heap->elements[i]);
				}
			}
		} else {
			intern->heap = other->heap;
		}

		intern->flags = other->flags;
	} else {
		intern->heap = spl_ptr_heap_init(spl_ptr_heap_zval_max_cmp, spl_ptr_heap_zval_ctor, spl_ptr_heap_zval_dtor);
	}

	retval.handlers = &spl_handler_SplHeap;

	while (parent) {
		if (parent == spl_ce_SplPriorityQueue) {
			intern->heap->cmp = spl_ptr_pqueue_zval_cmp;
			intern->flags     = SPL_PQUEUE_EXTR_DATA;
			retval.handlers   = &spl_handler_SplPriorityQueue;
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

	retval.handle   = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, spl_heap_object_free_storage, NULL TSRMLS_CC);

	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplHeap");
	}

	if (inherited) {
		zend_hash_find(&class_type->function_table, "compare",    sizeof("compare"),    (void **) &intern->fptr_cmp);
		if (intern->fptr_cmp->common.scope == parent) {
			intern->fptr_cmp = NULL;
		}
		zend_hash_find(&class_type->function_table, "count",        sizeof("count"),        (void **) &intern->fptr_count);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return retval;
}
/* }}} */

static zend_object_value spl_heap_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	spl_heap_object *tmp;
	return spl_heap_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
}
/* }}} */

static zend_object_value spl_heap_object_clone(zval *zobject TSRMLS_DC) /* {{{ */
{
	zend_object_value   new_obj_val;
	zend_object        *old_object;
	zend_object        *new_object;
	zend_object_handle  handle = Z_OBJ_HANDLE_P(zobject);
	spl_heap_object    *intern;

	old_object  = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_heap_object_new_ex(old_object->ce, &intern, zobject, 1 TSRMLS_CC);
	new_object  = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static int spl_heap_object_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_count) {
		zval *rv;
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			convert_to_long(intern->retval);
			*count = (long) Z_LVAL_P(intern->retval);
			return SUCCESS;
		}
		*count = 0;
		return FAILURE;
	}
	
	*count = spl_ptr_heap_count(intern->heap);

	return SUCCESS;
} 
/* }}} */

static HashTable* spl_heap_object_get_debug_info_helper(zend_class_entry *ce, zval *obj, int *is_temp TSRMLS_DC) { /* {{{ */
	spl_heap_object *intern  = (spl_heap_object*)zend_object_store_get_object(obj TSRMLS_CC);
	zval *tmp, zrv, *heap_array;
	char *pnstr;
	int  pnlen;
	int  i;

	*is_temp = 0;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		ZEND_INIT_SYMTABLE_EX(intern->debug_info, zend_hash_num_elements(intern->std.properties) + 1, 0);
	}

	if (intern->debug_info->nApplyCount == 0) {
		INIT_PZVAL(&zrv);
		Z_ARRVAL(zrv) = intern->debug_info;

		zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		pnstr = spl_gen_private_prop_name(ce, "flags", sizeof("flags")-1, &pnlen TSRMLS_CC);
		add_assoc_long_ex(&zrv, pnstr, pnlen+1, intern->flags);
		efree(pnstr);

		pnstr = spl_gen_private_prop_name(ce, "isCorrupted", sizeof("isCorrupted")-1, &pnlen TSRMLS_CC);
		add_assoc_bool_ex(&zrv, pnstr, pnlen+1, intern->heap->flags&SPL_HEAP_CORRUPTED);
		efree(pnstr);

		ALLOC_INIT_ZVAL(heap_array);
		array_init(heap_array);

		for (i = 0; i < intern->heap->count; ++i) {
			add_index_zval(heap_array, i, (zval *)intern->heap->elements[i]);
			Z_ADDREF_P(intern->heap->elements[i]);
		}

		pnstr = spl_gen_private_prop_name(ce, "heap", sizeof("heap")-1, &pnlen TSRMLS_CC);
		add_assoc_zval_ex(&zrv, pnstr, pnlen+1, heap_array);
		efree(pnstr);
	}

	return intern->debug_info;
}
/* }}} */

static HashTable* spl_heap_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	return spl_heap_object_get_debug_info_helper(spl_ce_SplHeap, obj, is_temp TSRMLS_CC);
}
/* }}} */

static HashTable* spl_pqueue_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	return spl_heap_object_get_debug_info_helper(spl_ce_SplPriorityQueue, obj, is_temp TSRMLS_CC);
}
/* }}} */

/* {{{ proto int SplHeap::count() U
 Return the number of elements in the heap. */
SPL_METHOD(SplHeap, count)
{
	long count;
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	count = spl_ptr_heap_count(intern->heap);
	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto int SplHeap::isEmpty() U
 Return true if the heap is empty. */
SPL_METHOD(SplHeap, isEmpty)
{
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	RETURN_BOOL(spl_ptr_heap_count(intern->heap)==0);
}
/* }}} */

/* {{{ proto bool SplHeap::insert(mixed $value) U
	   Push $value on the heap */
SPL_METHOD(SplHeap, insert)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	SEPARATE_ARG_IF_REF(value);

	spl_ptr_heap_insert(intern->heap, value, getThis() TSRMLS_CC);

	RETURN_TRUE;
} 
/* }}} */

/* {{{ proto mixed SplHeap::extract() U
	   extract the element out of the top of the heap */
SPL_METHOD(SplHeap, extract)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	value  = (zval *)spl_ptr_heap_delete_top(intern->heap, getThis() TSRMLS_CC);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't extract from an empty heap", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 1);
} 
/* }}} */

/* {{{ proto bool SplPriorityQueue::insert(mixed $value, mixed $priority) U
	   Push $value with the priority $priodiry on the priorityqueue */
SPL_METHOD(SplPriorityQueue, insert)
{
	zval *data, *priority, *elem;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &data, &priority) == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	SEPARATE_ARG_IF_REF(data);
	SEPARATE_ARG_IF_REF(priority);

	ALLOC_INIT_ZVAL(elem);

	array_init(elem);
	add_assoc_zval_ex(elem, "data",     sizeof("data"),     data);
	add_assoc_zval_ex(elem, "priority", sizeof("priority"), priority);

	spl_ptr_heap_insert(intern->heap, elem, getThis() TSRMLS_CC);

	RETURN_TRUE;
} 
/* }}} */

/* {{{ proto mixed SplPriorityQueue::extract() U
	   extract the element out of the top of the priority queue */
SPL_METHOD(SplPriorityQueue, extract)
{
	zval *value, *value_out, **value_out_pp;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	value  = (zval *)spl_ptr_heap_delete_top(intern->heap, getThis() TSRMLS_CC);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't extract from an empty heap", 0 TSRMLS_CC);
		return;
	}

	value_out_pp = spl_pqueue_extract_helper(&value, intern->flags);


	if (!value_out_pp) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		zval_ptr_dtor(&value);
		return;
	}

	value_out = *value_out_pp;

	Z_ADDREF_P(value_out);
	zval_ptr_dtor(&value);

	RETURN_ZVAL(value_out, 1, 1);
} 
/* }}} */

/* {{{ proto mixed SplPriorityQueue::top() U
	   Peek at the top element of the priority queue */
SPL_METHOD(SplPriorityQueue, top)
{
	zval *value, **value_out;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	value  = (zval *)spl_ptr_heap_top(intern->heap);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty heap", 0 TSRMLS_CC);
		return;
	}

	value_out = spl_pqueue_extract_helper(&value, intern->flags);

	if (!value_out) {
		zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		return;
	}

	RETURN_ZVAL(*value_out, 1, 0);
}
/* }}} */

/* {{{ proto int SplPriorityQueue::setIteratorMode($flags) U
 Set the flags of extraction*/
SPL_METHOD(SplPriorityQueue, setExtractFlags)
{
	long value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->flags = value & SPL_PQUEUE_EXTR_MASK;

	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto int SplHeap::recoverFromCorruption() U
 Recover from a corrupted state*/
SPL_METHOD(SplHeap, recoverFromCorruption)
{
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->heap->flags = intern->heap->flags & ~SPL_HEAP_CORRUPTED;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SplPriorityQueue::compare(mixed $a, mixed $b) U
	   compare the priorities */
SPL_METHOD(SplPriorityQueue, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_max_cmp(a, b, NULL TSRMLS_CC));
} 
/* }}} */

/* {{{ proto mixed SplHeap::top() U
	   Peek at the top element of the heap */
SPL_METHOD(SplHeap, top)
{
	zval *value;
	spl_heap_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	value  = (zval *)spl_ptr_heap_top(intern->heap);

	if (!value) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty heap", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto bool SplMinHeap::compare(mixed $a, mixed $b) U
	   compare the values */
SPL_METHOD(SplMinHeap, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_min_cmp(a, b, NULL TSRMLS_CC));
} 
/* }}} */

/* {{{ proto bool SplMaxHeap::compare(mixed $a, mixed $b) U
	   compare the values */
SPL_METHOD(SplMaxHeap, compare)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_LONG(spl_ptr_heap_zval_max_cmp(a, b, NULL TSRMLS_CC));
} 
/* }}} */

static void spl_heap_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_heap_it *iterator = (spl_heap_it *)iter;

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}
/* }}} */

static void spl_heap_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	/* do nothing, the iterator always points to the top element */
}
/* }}} */

static int spl_heap_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_heap_it         *iterator = (spl_heap_it *)iter;

	return (iterator->object->heap->count != 0 ? SUCCESS : FAILURE);
}
/* }}} */

static void spl_heap_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_heap_it  *iterator = (spl_heap_it *)iter;
	zval        **element  = (zval **)&iterator->object->heap->elements[0];

	if (iterator->object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	if (iterator->object->heap->count == 0 || !*element) {
		*data = NULL;
	} else {
		*data = element;
	}
}
/* }}} */

static void spl_pqueue_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_heap_it  *iterator = (spl_heap_it *)iter;
	zval        **element  = (zval **)&iterator->object->heap->elements[0];

	if (iterator->object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	if (iterator->object->heap->count == 0 || !*element) {
		*data = NULL;
	} else {
		*data = spl_pqueue_extract_helper(element, iterator->object->flags);
		if (!*data) {
			zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
		}
	}
}
/* }}} */

static void spl_heap_it_get_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC) /* {{{ */
{
	spl_heap_it *iterator = (spl_heap_it *)iter;

	ZVAL_LONG(key, iterator->object->heap->count - 1);
}
/* }}} */

static void spl_heap_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	zval                 *object   = (zval*)((zend_user_iterator *)iter)->it.data;
	spl_heap_it          *iterator = (spl_heap_it *)iter;
	spl_ptr_heap_element elem;

	if (iterator->object->heap->flags & SPL_HEAP_CORRUPTED) {
		zend_throw_exception(spl_ce_RuntimeException, "Heap is corrupted, heap properties are no longer ensured.", 0 TSRMLS_CC);
		return;
	}

	elem = spl_ptr_heap_delete_top(iterator->object->heap, object TSRMLS_CC);

	if (elem != NULL) {
		zval_ptr_dtor((zval **)&elem);
	}

	zend_user_it_invalidate_current(iter TSRMLS_CC);
}
/* }}} */

/* {{{  proto int SplHeap::key() U
   Return current array key */
SPL_METHOD(SplHeap, key)
{
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}		
	
	RETURN_LONG(intern->heap->count - 1);
}
/* }}} */

/* {{{ proto void SplHeap::next() U
   Move to next entry */
SPL_METHOD(SplHeap, next)
{
	spl_heap_object      *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_ptr_heap_element  elem   = spl_ptr_heap_delete_top(intern->heap, getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (elem != NULL) {
		zval_ptr_dtor((zval **)&elem);
	}
}
/* }}} */

/* {{{ proto bool SplHeap::valid() U
   Check whether the datastructure contains more entries */
SPL_METHOD(SplHeap, valid)
{
	spl_heap_object *intern = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->heap->count != 0);
}
/* }}} */

/* {{{ proto void SplHeap::rewind() U
   Rewind the datastructure back to the start */
SPL_METHOD(SplHeap, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	/* do nothing, the iterator always points to the top element */
}
/* }}} */

/* {{{ proto mixed|NULL SplHeap::current() U
   Return current datastructure entry */
SPL_METHOD(SplHeap, current)
{
	spl_heap_object *intern  = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval            *element = (zval *)intern->heap->elements[0];
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->heap->count || !element) {
		RETURN_NULL();
	} else {
		RETURN_ZVAL(element, 1, 0);
	}
}
/* }}} */

/* {{{ proto mixed|NULL SplPriorityQueue::current() U
   Return current datastructure entry */
SPL_METHOD(SplPriorityQueue, current)
{
	spl_heap_object  *intern  = (spl_heap_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval            **element = (zval **)&intern->heap->elements[0];
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->heap->count || !*element) {
		RETURN_NULL();
	} else {
		zval **data = spl_pqueue_extract_helper(element, intern->flags);

		if (!data) {
			zend_error(E_RECOVERABLE_ERROR, "Unable to extract from the PriorityQueue node");
			RETURN_NULL();
		}

		RETURN_ZVAL(*data, 1, 0);
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

zend_object_iterator *spl_heap_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_heap_it     *iterator;
	spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	Z_ADDREF_P(object);

	iterator                  = emalloc(sizeof(spl_heap_it));
	iterator->intern.it.data  = (void*)object;
	iterator->intern.it.funcs = &spl_heap_it_funcs;
	iterator->intern.ce       = ce;
	iterator->intern.value    = NULL;
	iterator->flags           = heap_object->flags;
	iterator->object          = heap_object;

	return (zend_object_iterator*)iterator;
}
/* }}} */

zend_object_iterator *spl_pqueue_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_heap_it     *iterator;
	spl_heap_object *heap_object = (spl_heap_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	Z_ADDREF_P(object);

	iterator                  = emalloc(sizeof(spl_heap_it));
	iterator->intern.it.data  = (void*)object;
	iterator->intern.it.funcs = &spl_pqueue_it_funcs;
	iterator->intern.ce       = ce;
	iterator->intern.value    = NULL;
	iterator->flags           = heap_object->flags;
	iterator->object          = heap_object;

	return (zend_object_iterator*)iterator;
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
	ZEND_FENTRY(compare, NULL, NULL, ZEND_ACC_PROTECTED|ZEND_ACC_ABSTRACT)
	{NULL, NULL, NULL}
};
/* }}} */

PHP_MINIT_FUNCTION(spl_heap) /* {{{ */
{
	REGISTER_SPL_STD_CLASS_EX(SplHeap, spl_heap_object_new, spl_funcs_SplHeap);
	memcpy(&spl_handler_SplHeap, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplHeap.clone_obj      = spl_heap_object_clone;
	spl_handler_SplHeap.count_elements = spl_heap_object_count_elements;
	spl_handler_SplHeap.get_debug_info = spl_heap_object_get_debug_info;

	REGISTER_SPL_IMPLEMENTS(SplHeap, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplHeap, Countable);

	spl_ce_SplHeap->get_iterator = spl_heap_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(SplMinHeap,           SplHeap,        spl_heap_object_new, spl_funcs_SplMinHeap);
	REGISTER_SPL_SUB_CLASS_EX(SplMaxHeap,           SplHeap,        spl_heap_object_new, spl_funcs_SplMaxHeap);

	spl_ce_SplMaxHeap->get_iterator = spl_heap_get_iterator;
	spl_ce_SplMinHeap->get_iterator = spl_heap_get_iterator;

	REGISTER_SPL_STD_CLASS_EX(SplPriorityQueue, spl_heap_object_new, spl_funcs_SplPriorityQueue);
	memcpy(&spl_handler_SplPriorityQueue, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplPriorityQueue.clone_obj      = spl_heap_object_clone;
	spl_handler_SplPriorityQueue.count_elements = spl_heap_object_count_elements;
	spl_handler_SplPriorityQueue.get_debug_info = spl_pqueue_object_get_debug_info;

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

