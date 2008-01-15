/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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
#include "zend_hash.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_array.h"
#include "spl_dllist.h"
#include "spl_exceptions.h"

zend_object_handlers spl_handler_SplDoublyLinkedList;
PHPAPI zend_class_entry  *spl_ce_SplDoublyLinkedList;
PHPAPI zend_class_entry  *spl_ce_SplQueue;
PHPAPI zend_class_entry  *spl_ce_SplStack;

#define SPL_LLIST_DELREF(elem, dtor) if(!--(elem)->rc) { \
	if(dtor) { \
		dtor(elem); \
	} \
	efree(elem); \
	elem = NULL; \
}

#define SPL_LLIST_CHECK_DELREF(elem, dtor) if((elem) && !--(elem)->rc) { \
	if(dtor) { \
		dtor(elem); \
	} \
	efree(elem); \
	elem = NULL; \
}

#define SPL_LLIST_ADDREF(elem) (elem)->rc++
#define SPL_LLIST_CHECK_ADDREF(elem) if(elem) (elem)->rc++

#define SPL_DLLIST_IT_DELETE 0x00000001 /* Delete flag makes the iterator delete the current element on next */
#define SPL_DLLIST_IT_LIFO   0x00000002 /* LIFO flag makes the iterator traverse the structure as a LastInFirstOut */
#define SPL_DLLIST_IT_MASK   0x00000003 /* Mask to isolate flags related to iterators */
#define SPL_DLLIST_IT_FIX    0x00000004 /* Backward/Forward bit is fixed */

#ifdef accept
#undef accept
#endif

typedef struct _spl_ptr_llist_element {
	struct _spl_ptr_llist_element *prev;
	struct _spl_ptr_llist_element *next;
	int                            rc;
	void                          *data;
} spl_ptr_llist_element;

typedef void (*spl_ptr_llist_dtor_func)(spl_ptr_llist_element *);
typedef void (*spl_ptr_llist_ctor_func)(spl_ptr_llist_element *);

typedef struct _spl_ptr_llist {
	spl_ptr_llist_element   *head;
	spl_ptr_llist_element   *tail;
	spl_ptr_llist_dtor_func  dtor;
	int count;
} spl_ptr_llist;


typedef struct _spl_dllist_object spl_dllist_object;
typedef struct _spl_dllist_it spl_dllist_it;

struct _spl_dllist_object {
	zend_object            std;
	spl_ptr_llist         *llist;
	int                    traverse_position;
	spl_ptr_llist_element *traverse_pointer;
	zval                  *retval;
	int                    flags;
	zend_function         *fptr_offset_get;
	zend_function         *fptr_offset_set;
	zend_function         *fptr_offset_has;
	zend_function         *fptr_offset_del;
	zend_class_entry      *ce_get_iterator;
};

/* define an overloaded iterator structure */
struct _spl_dllist_it {
	zend_user_iterator     intern;
	int                    traverse_position;
	spl_ptr_llist_element *traverse_pointer;
	int                    flags;
	spl_dllist_object     *object;
};

/* {{{  spl_ptr_llist */
static void spl_ptr_llist_zval_dtor(spl_ptr_llist_element *elem) { /* {{{ */
	zval_ptr_dtor((zval **)&elem->data);
}
/* }}} */

static void spl_ptr_llist_zval_ctor(spl_ptr_llist_element *elem) { /* {{{ */
	Z_ADDREF_P((zval *)elem->data);
}
/* }}} */

static spl_ptr_llist *spl_ptr_llist_init(spl_ptr_llist_dtor_func dtor) /* {{{ */
{
	spl_ptr_llist *llist = emalloc(sizeof(spl_ptr_llist));

	llist->head  = NULL;
	llist->tail  = NULL;
	llist->count = 0;
	llist->dtor  = dtor;

	return llist;
}
/* }}} */

static long spl_ptr_llist_count(spl_ptr_llist *llist) /* {{{ */
{
	return (long)llist->count;
}
/* }}} */

static void spl_ptr_llist_destroy(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element   *current = llist->head, *next;
	spl_ptr_llist_dtor_func  dtor    = llist->dtor;

	while (current) {
		next = current->next;
		SPL_LLIST_DELREF(current, dtor);
		current = next;
	}

	efree(llist);
}
/* }}} */

static spl_ptr_llist_element *spl_ptr_llist_offset(spl_ptr_llist *llist, long offset, int backward) /* {{{ */
{

	spl_ptr_llist_element *current;

	if (backward) {
		current = llist->tail;
	} else {
		current = llist->head;
	}

	int pos = 0;

	while (current && pos < offset) {
		pos++;
		if (backward) {
			current = current->prev;
		} else {
			current = current->next;
		}
	}

	return current;
}
/* }}} */

static void spl_ptr_llist_unshift(spl_ptr_llist *llist, void *data) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->data = data;
	elem->rc   = 1;
	elem->prev = NULL;
	elem->next = llist->head;

	if (llist->head) {
		llist->head->prev = elem;
	} else {
		llist->tail = elem;
	}

	llist->head = elem;
	llist->count++;
}
/* }}} */

static void spl_ptr_llist_push(spl_ptr_llist *llist, void *data) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->data = data;
	elem->rc   = 1;
	elem->prev = llist->tail;
	elem->next = NULL;

	if (llist->tail) {
		llist->tail->next = elem;
	} else {
		llist->head = elem;
	}

	llist->tail = elem;
	llist->count++;
}
/* }}} */

static void *spl_ptr_llist_pop(spl_ptr_llist *llist) /* {{{ */
{
	void                     *data;
	spl_ptr_llist_element    *tail = llist->tail;
	spl_ptr_llist_dtor_func   dtor = NULL;

	if (tail == NULL) {
		return NULL;
	}

	if (tail->prev) {
		tail->prev->next = NULL;
	} else {
		llist->head = NULL;
	}

	llist->tail = tail->prev;
	llist->count--;
	data = tail->data;

	SPL_LLIST_DELREF(tail, dtor);

	return data;
}
/* }}} */

static void *spl_ptr_llist_last(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element *tail = llist->tail;

	if (tail == NULL) {
		return NULL;
	} else {
		return tail->data;
	}
}
/* }}} */

static void *spl_ptr_llist_first(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element *head = llist->head;

	if (head == NULL) {
		return NULL;
	} else {
		return head->data;
	}
}
/* }}} */

static void *spl_ptr_llist_shift(spl_ptr_llist *llist) /* {{{ */
{
	void                    *data;
	spl_ptr_llist_element   *head = llist->head;
	spl_ptr_llist_dtor_func  dtor = NULL;

	if (head == NULL) {
		return NULL;
	}

	if (head->next) {
		head->next->prev = NULL;
	} else {
		llist->tail = NULL;
	}

	llist->head = head->next;
	llist->count--;
	data = head->data;

	SPL_LLIST_DELREF(head, dtor);

	return data;
}
/* }}} */

static void spl_dllist_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_dllist_object *intern = (spl_dllist_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	spl_ptr_llist_destroy(intern->llist);
	zval_ptr_dtor(&intern->retval);

	efree(object);
}
/* }}} */

static void spl_ptr_llist_copy(spl_ptr_llist *from, spl_ptr_llist *to, spl_ptr_llist_ctor_func ctor) /* {{{ */
{
	spl_ptr_llist_element *current = from->head, *next;

	while (current) {
		next = current->next;
		if (ctor) {
			ctor(current);
		}
		spl_ptr_llist_push(to, current->data);
		current = next;
	}

}
/* }}} */

/* }}} */

zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

static zend_object_value spl_dllist_object_new_ex(zend_class_entry *class_type, spl_dllist_object **obj, zval *orig, int clone_orig TSRMLS_DC) /* {{{ */
{
	zend_object_value  retval;
	spl_dllist_object *intern;
	zval              *tmp;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_dllist_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	intern->flags = 0;
	intern->traverse_position = 0;

	if (orig) {
		spl_dllist_object *other = (spl_dllist_object*)zend_object_store_get_object(orig TSRMLS_CC);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(spl_ptr_llist_zval_dtor);
			spl_ptr_llist_copy(other->llist, intern->llist, spl_ptr_llist_zval_ctor);
			intern->traverse_pointer  = intern->llist->head;
		} else {
			intern->llist = other->llist;
			intern->traverse_pointer  = intern->llist->head;
			SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
		}

		intern->flags = other->flags;
	} else {
		intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(spl_ptr_llist_zval_dtor);
		intern->traverse_pointer  = intern->llist->head;
		SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
	}


	while (parent) {
		if (parent == spl_ce_SplStack) {
			intern->flags |= (SPL_DLLIST_IT_FIX | SPL_DLLIST_IT_LIFO);
		} else if (parent == spl_ce_SplQueue) {
			intern->flags |= SPL_DLLIST_IT_FIX;
		}
		if (parent == spl_ce_SplDoublyLinkedList) {
			retval.handlers = &spl_handler_SplDoublyLinkedList;
			break;
		}
		parent = parent->parent;
		inherited = 1;
	}

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, spl_dllist_object_free_storage, NULL TSRMLS_CC);

	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplDoublyLinkedList");
	}
	if (inherited) {
		zend_hash_find(&class_type->function_table, "offsetget",    sizeof("offsetget"),    (void **) &intern->fptr_offset_get);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetset",    sizeof("offsetset"),    (void **) &intern->fptr_offset_set);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetexists", sizeof("offsetexists"), (void **) &intern->fptr_offset_has);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetunset",  sizeof("offsetunset"),  (void **) &intern->fptr_offset_del);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
	}

	return retval;
}
/* }}} */

static zend_object_value spl_dllist_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	spl_dllist_object *tmp;
	return spl_dllist_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
}
/* }}} */

static zend_object_value spl_dllist_object_clone(zval *zobject TSRMLS_DC) /* {{{ */
{
	zend_object_value   new_obj_val;
	zend_object        *old_object;
	zend_object        *new_object;
	zend_object_handle  handle = Z_OBJ_HANDLE_P(zobject);
	spl_dllist_object  *intern;

	old_object  = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_dllist_object_new_ex(old_object->ce, &intern, zobject, 1 TSRMLS_CC);
	new_object  = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static int spl_dllist_object_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(object TSRMLS_CC);

	*count = spl_ptr_llist_count(intern->llist);

	return SUCCESS;
} 
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::push(mixed $value) U
	   Push $value on the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, push)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}
	SEPARATE_ARG_IF_REF(value);

	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_ptr_llist_push(intern->llist, value);

	RETURN_TRUE;
} 
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::unshift(mixed $value) U
	   Unshift $value on the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, unshift)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}
	SEPARATE_ARG_IF_REF(value);

	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_ptr_llist_unshift(intern->llist, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::pop() U
	   Pop an element ouf of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, pop)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	value = (zval *)spl_ptr_llist_pop(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't pop from an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 1);
} 
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::shift() U
	   Shift an element ouf of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, shift)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	value = (zval *)spl_ptr_llist_shift(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't shift from an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 1);
} 
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::top() U
	   Peak at the top element of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, top)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value = (zval *)spl_ptr_llist_last(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peak at an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::bottom() U
	   Peak at the bottom element of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, bottom)
{
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value = (zval *)spl_ptr_llist_first(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peak at an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::count() U
 Return the number of elements in the datastructure. */
SPL_METHOD(SplDoublyLinkedList, count)
{
	long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	spl_dllist_object_count_elements(getThis(), &count TSRMLS_CC);
	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::isEmpty() U
 Return true of the SplDoublyLinkedList is empty. */
SPL_METHOD(SplDoublyLinkedList, isEmpty)
{
	long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	spl_dllist_object_count_elements(getThis(), &count TSRMLS_CC);
	RETURN_BOOL(count==0);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::setIteratorMode($flags) U
 Set the mode of iteration */
SPL_METHOD(SplDoublyLinkedList, setIteratorMode)
{
	long value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
		return;
	}

	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->flags & SPL_DLLIST_IT_FIX
		&& (intern->flags & SPL_DLLIST_IT_LIFO) != (value & SPL_DLLIST_IT_LIFO)) {
		zend_throw_exception(spl_ce_RuntimeException, "Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen", 0 TSRMLS_CC);
		return;
	}

	intern->flags = value & SPL_DLLIST_IT_MASK;

	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::getIteratorMode() U
 Return the mode of iteration */
SPL_METHOD(SplDoublyLinkedList, getIteratorMode)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->flags);
}
/* }}} */

static long spl_dllist_offset_convert(zval *offset TSRMLS_DC) /* {{{ */
{
	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		ZEND_HANDLE_NUMERIC(Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, idx);
		break;
	case IS_UNICODE:
		ZEND_HANDLE_U_NUMERIC(Z_USTRVAL_P(offset), Z_USTRLEN_P(offset)+1, idx);
		break;
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL:
	case IS_LONG:
		if (Z_TYPE_P(offset) == IS_DOUBLE) {
			return (long)Z_DVAL_P(offset);
		} else {
			return Z_LVAL_P(offset);
		}
	}
	zend_throw_exception(spl_ce_OutOfRangeException, "Invalid offset", 0 TSRMLS_CC);
	return 0;
} 
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::offsetExists(mixed $index) U
 Returns whether the requested $index exists. */
SPL_METHOD(SplDoublyLinkedList, offsetExists)
{
	zval              *zindex;
	spl_dllist_object *intern;
	long               index;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	index  = spl_dllist_offset_convert(zindex);

	RETURN_BOOL(index >= 0 && index < intern->llist->count);
} /* }}} */

/* {{{ proto mixed SplDoublyLinkedList::offsetGet(mixed $index) U
 Returns the value at the specified $index. */
SPL_METHOD(SplDoublyLinkedList, offsetGet)
{
	zval                  *zindex, *value;
	long                   index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	index  = spl_dllist_offset_convert(zindex);

    if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset out of range", 0 TSRMLS_CC);
		return;
	}

	element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

	if (element != NULL) {
		value = (zval *)element->data;
		RETURN_ZVAL(value, 1, 0);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0 TSRMLS_CC);
		return;
	}
} /* }}} */

/* {{{ proto void SplDoublyLinkedList::offsetSet(mixed $index, mixed $newval) U
 Sets the value at the specified $index to $newval. */
SPL_METHOD(SplDoublyLinkedList, offsetSet)
{
	zval                  *zindex, *value;
	spl_dllist_object     *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zindex, &value) == FAILURE) {
		return;
	}
	SEPARATE_ARG_IF_REF(value);

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (Z_TYPE_P(zindex) == IS_NULL) {
		/* $obj[] = ... */
		spl_ptr_llist_push(intern->llist, value);
	} else {
		/* $obj[$foo] = ... */
		long                   index;
		spl_ptr_llist_element *element;

		index = spl_dllist_offset_convert(zindex);

		if (index < 0 || index >= intern->llist->count) {
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset out of range", 0 TSRMLS_CC);
			return;
		}

		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		if (element != NULL) {
			if (intern->llist->dtor) {
				intern->llist->dtor(element);
			}
			element->data = value;
		} else {
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0 TSRMLS_CC);
			return;
		}
	}
} /* }}} */

/* {{{ proto void SplDoublyLinkedList::offsetUnset(mixed $index) U
 Unsets the value at the specified $index. */
SPL_METHOD(SplDoublyLinkedList, offsetUnset)
{
	zval                  *zindex;
	long                   index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;
	spl_ptr_llist         *llist;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	index  = (int)spl_dllist_offset_convert(zindex);
    llist  = intern->llist;

    if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset out of range", 0 TSRMLS_CC);
		return;
	}

	element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

	if (element != NULL) {
		/* connect the neightbors */
		if (element->prev) {
			element->prev->next = element->next;
		}
		if (element->next) {
			element->next->prev = element->prev;
		}
		/* take care of head/tail */
		if (element == llist->head) {
			llist->head = element->next;
		}
		if (element == llist->tail) {
			llist->tail = element->prev;
		}
		/* finally, delete the element */
		llist->count--;
		SPL_LLIST_DELREF(element, llist->dtor);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0 TSRMLS_CC);
		return;
	}
} /* }}} */

static void spl_dllist_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	SPL_LLIST_CHECK_DELREF(iterator->traverse_pointer, iterator->object->llist->dtor);

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}
/* }}} */

static int spl_dllist_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	return (element != NULL ? SUCCESS : FAILURE);
}
/* }}} */

static void spl_dllist_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	if (element == NULL) {
		*data = NULL;
	} else {
		*data = (zval **)&element->data;
	}
}
/* }}} */

static int spl_dllist_it_get_current_key(zend_object_iterator *iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	*int_key = (ulong) iterator->traverse_position;
	return HASH_KEY_IS_LONG;
}
/* }}} */

static void spl_dllist_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object   = iterator->object;

	zend_user_it_invalidate_current(iter TSRMLS_CC);

	if (iterator->traverse_pointer) {
		spl_ptr_llist_element *old = iterator->traverse_pointer;

		if (iterator->flags & SPL_DLLIST_IT_LIFO) {
			iterator->traverse_pointer = old->prev;
			iterator->traverse_position--;
			if (iterator->flags & SPL_DLLIST_IT_DELETE) {
				spl_ptr_llist_pop(object->llist);
			}
		} else {
			iterator->traverse_pointer = old->next;
			iterator->traverse_position++;
			if (iterator->flags & SPL_DLLIST_IT_DELETE) {
				spl_ptr_llist_shift(object->llist);
			}
		}

		SPL_LLIST_DELREF(old, object->llist->dtor);
		SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);
	}
}
/* }}} */

static void spl_dllist_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object   = iterator->object;
	spl_ptr_llist     *llist    = object->llist;

	SPL_LLIST_CHECK_DELREF(iterator->traverse_pointer, llist->dtor);
	if (iterator->flags & SPL_DLLIST_IT_LIFO) {
		iterator->traverse_position = llist->count-1;
		iterator->traverse_pointer  = llist->tail;
	} else {
		iterator->traverse_position = 0;
		iterator->traverse_pointer  = llist->head;
	}
	SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_dllist_it_funcs = {
	spl_dllist_it_dtor,
	spl_dllist_it_valid,
	spl_dllist_it_get_current_data,
	spl_dllist_it_get_current_key,
	spl_dllist_it_move_forward,
	spl_dllist_it_rewind
};

zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator;
	spl_dllist_object *dllist_object = (spl_dllist_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	iterator     = emalloc(sizeof(spl_dllist_it));

	Z_ADDREF_P(object);
	iterator->intern.it.data = (void*)object;
	iterator->intern.it.funcs = &spl_dllist_it_funcs;
	iterator->intern.ce = ce;
	iterator->intern.value = NULL;
	iterator->traverse_position  = dllist_object->traverse_position;
	iterator->traverse_pointer   = dllist_object->traverse_pointer;
	iterator->flags              = dllist_object->flags & SPL_DLLIST_IT_MASK;

	SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);

	iterator->object = dllist_object;

	return (zend_object_iterator*)iterator;
}
/* }}} */

/*  Function/Class/Method definitions */
static
ZEND_BEGIN_ARG_INFO(arginfo_dllist_setiteratormode, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_dllist_push, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_dllist_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_dllist_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_funcs_SplQueue[] = {
	SPL_MA(SplQueue, enqueue, SplDoublyLinkedList, push,  arginfo_dllist_push, ZEND_ACC_PUBLIC)
	SPL_MA(SplQueue, dequeue, SplDoublyLinkedList, shift, NULL,                ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_funcs_SplDoublyLinkedList[] = {
	SPL_ME(SplDoublyLinkedList, pop,             NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, shift,           NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, push,            arginfo_dllist_push,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, unshift,         arginfo_dllist_push,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, top,             NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, bottom,          NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, count,           NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, isEmpty,         NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, setIteratorMode, arginfo_dllist_setiteratormode, ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, getIteratorMode, NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetExists,    arginfo_dllist_offsetGet,       ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetGet,       arginfo_dllist_offsetGet,       ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetSet,       arginfo_dllist_offsetSet,       ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetUnset,     arginfo_dllist_offsetGet,       ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */


PHP_MINIT_FUNCTION(spl_dllist) /* {{{ */
{
	REGISTER_SPL_STD_CLASS_EX(SplDoublyLinkedList, spl_dllist_object_new, spl_funcs_SplDoublyLinkedList);
	memcpy(&spl_handler_SplDoublyLinkedList, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplDoublyLinkedList.clone_obj = spl_dllist_object_clone;
	spl_handler_SplDoublyLinkedList.count_elements = spl_dllist_object_count_elements;

	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_LIFO",  SPL_DLLIST_IT_LIFO);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_FIFO",  0);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_DELETE",SPL_DLLIST_IT_DELETE);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_KEEP",  0);

	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Countable);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, ArrayAccess);

	spl_ce_SplDoublyLinkedList->get_iterator = spl_dllist_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(SplQueue,           SplDoublyLinkedList,        spl_dllist_object_new, spl_funcs_SplQueue);
	REGISTER_SPL_SUB_CLASS_EX(SplStack,           SplDoublyLinkedList,        spl_dllist_object_new, NULL);

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
