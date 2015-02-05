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
#include "zend_hash.h"

#include "php_spl.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_dllist.h"
#include "spl_exceptions.h"

zend_object_handlers spl_handler_SplDoublyLinkedList;
PHPAPI zend_class_entry  *spl_ce_SplDoublyLinkedList;
PHPAPI zend_class_entry  *spl_ce_SplQueue;
PHPAPI zend_class_entry  *spl_ce_SplStack;

#define SPL_LLIST_DELREF(elem) if(!--(elem)->rc) { \
	efree(elem); \
}

#define SPL_LLIST_CHECK_DELREF(elem) if((elem) && !--(elem)->rc) { \
	efree(elem); \
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
	zval                           data;
} spl_ptr_llist_element;

typedef void (*spl_ptr_llist_dtor_func)(spl_ptr_llist_element *);
typedef void (*spl_ptr_llist_ctor_func)(spl_ptr_llist_element *);

typedef struct _spl_ptr_llist {
	spl_ptr_llist_element   *head;
	spl_ptr_llist_element   *tail;
	spl_ptr_llist_dtor_func  dtor;
	spl_ptr_llist_ctor_func  ctor;
	int count;
} spl_ptr_llist;

typedef struct _spl_dllist_object spl_dllist_object;
typedef struct _spl_dllist_it spl_dllist_it;

struct _spl_dllist_object {
	spl_ptr_llist         *llist;
	int                    traverse_position;
	spl_ptr_llist_element *traverse_pointer;
	int                    flags;
	zend_function         *fptr_offset_get;
	zend_function         *fptr_offset_set;
	zend_function         *fptr_offset_has;
	zend_function         *fptr_offset_del;
	zend_function         *fptr_count;
	zend_class_entry      *ce_get_iterator;
	HashTable             *debug_info;
	zend_object            std;
};

/* define an overloaded iterator structure */
struct _spl_dllist_it {
	zend_user_iterator     intern;
	spl_ptr_llist_element *traverse_pointer;
	int                    traverse_position;
	int                    flags;
};

static inline spl_dllist_object *spl_dllist_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_dllist_object*)((char*)(obj) - XtOffsetOf(spl_dllist_object, std));
}
/* }}} */

#define Z_SPLDLLIST_P(zv)  spl_dllist_from_obj(Z_OBJ_P((zv)))

/* {{{  spl_ptr_llist */
static void spl_ptr_llist_zval_dtor(spl_ptr_llist_element *elem) { /* {{{ */
	if (!Z_ISUNDEF(elem->data)) {
		zval_ptr_dtor(&elem->data);
		ZVAL_UNDEF(&elem->data);
	}
}
/* }}} */

static void spl_ptr_llist_zval_ctor(spl_ptr_llist_element *elem) { /* {{{ */
	if (Z_REFCOUNTED(elem->data)) {
		Z_ADDREF(elem->data);
	}
}
/* }}} */

static spl_ptr_llist *spl_ptr_llist_init(spl_ptr_llist_ctor_func ctor, spl_ptr_llist_dtor_func dtor) /* {{{ */
{
	spl_ptr_llist *llist = emalloc(sizeof(spl_ptr_llist));

	llist->head  = NULL;
	llist->tail  = NULL;
	llist->count = 0;
	llist->dtor  = dtor;
	llist->ctor  = ctor;

	return llist;
}
/* }}} */

static zend_long spl_ptr_llist_count(spl_ptr_llist *llist) /* {{{ */
{
	return (zend_long)llist->count;
}
/* }}} */

static void spl_ptr_llist_destroy(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element   *current = llist->head, *next;
	spl_ptr_llist_dtor_func  dtor    = llist->dtor;

	while (current) {
		next = current->next;
		if(current && dtor) {
			dtor(current);
		}
		SPL_LLIST_DELREF(current);
		current = next;
	}

	efree(llist);
}
/* }}} */

static spl_ptr_llist_element *spl_ptr_llist_offset(spl_ptr_llist *llist, zend_long offset, int backward) /* {{{ */
{

	spl_ptr_llist_element *current;
	int pos = 0;

	if (backward) {
		current = llist->tail;
	} else {
		current = llist->head;
	}

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

static void spl_ptr_llist_unshift(spl_ptr_llist *llist, zval *data) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->rc   = 1;
	elem->prev = NULL;
	elem->next = llist->head;
	ZVAL_COPY_VALUE(&elem->data, data);

	if (llist->head) {
		llist->head->prev = elem;
	} else {
		llist->tail = elem;
	}

	llist->head = elem;
	llist->count++;

	if (llist->ctor) {
		llist->ctor(elem);
	}
}
/* }}} */

static void spl_ptr_llist_push(spl_ptr_llist *llist, zval *data) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->rc   = 1;
	elem->prev = llist->tail;
	elem->next = NULL;
	ZVAL_COPY_VALUE(&elem->data, data);

	if (llist->tail) {
		llist->tail->next = elem;
	} else {
		llist->head = elem;
	}

	llist->tail = elem;
	llist->count++;

	if (llist->ctor) {
		llist->ctor(elem);
	}
}
/* }}} */

static void spl_ptr_llist_pop(spl_ptr_llist *llist, zval *ret) /* {{{ */
{
	spl_ptr_llist_element    *tail = llist->tail;

	if (tail == NULL) {
		ZVAL_UNDEF(ret);
		return;
	}

	if (tail->prev) {
		tail->prev->next = NULL;
	} else {
		llist->head = NULL;
	}

	llist->tail = tail->prev;
	llist->count--;
	ZVAL_COPY(ret, &tail->data);

	if (llist->dtor) {
		llist->dtor(tail);
	}

	ZVAL_UNDEF(&tail->data);

	SPL_LLIST_DELREF(tail);
}
/* }}} */

static zval *spl_ptr_llist_last(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element *tail = llist->tail;

	if (tail == NULL) {
		return NULL;
	} else {
		return &tail->data;
	}
}
/* }}} */

static zval *spl_ptr_llist_first(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element *head = llist->head;

	if (head == NULL) {
		return NULL;
	} else {
		return &head->data;
	}
}
/* }}} */

static void spl_ptr_llist_shift(spl_ptr_llist *llist, zval *ret) /* {{{ */
{
	spl_ptr_llist_element   *head = llist->head;

	if (head == NULL) {
		ZVAL_UNDEF(ret);
		return;
	}

	if (head->next) {
		head->next->prev = NULL;
	} else {
		llist->tail = NULL;
	}

	llist->head = head->next;
	llist->count--;
	ZVAL_COPY(ret, &head->data);

	if (llist->dtor) {
		llist->dtor(head);
	}
	ZVAL_UNDEF(&head->data);

	SPL_LLIST_DELREF(head);
}
/* }}} */

static void spl_ptr_llist_copy(spl_ptr_llist *from, spl_ptr_llist *to) /* {{{ */
{
	spl_ptr_llist_element *current = from->head, *next;
//???	spl_ptr_llist_ctor_func ctor = from->ctor;

	while (current) {
		next = current->next;

		/*??? FIXME
		if (ctor) {
			ctor(current);
		}
		*/

		spl_ptr_llist_push(to, &current->data);
		current = next;
	}

}
/* }}} */

/* }}} */

static void spl_dllist_object_free_storage(zend_object *object) /* {{{ */
{
	spl_dllist_object *intern = spl_dllist_from_obj(object);
	zval tmp;

	zend_object_std_dtor(&intern->std);

	while (intern->llist->count > 0) {
		spl_ptr_llist_pop(intern->llist, &tmp);
		zval_ptr_dtor(&tmp);
	}

	spl_ptr_llist_destroy(intern->llist);
	SPL_LLIST_CHECK_DELREF(intern->traverse_pointer);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}
}
/* }}} */

zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

static zend_object *spl_dllist_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig) /* {{{ */
{
	spl_dllist_object *intern;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_dllist_object) + zend_object_properties_size(parent));

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->flags = 0;
	intern->traverse_position = 0;
	intern->debug_info = NULL;

	if (orig) {
		spl_dllist_object *other = Z_SPLDLLIST_P(orig);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(other->llist->ctor, other->llist->dtor);
			spl_ptr_llist_copy(other->llist, intern->llist);
			intern->traverse_pointer  = intern->llist->head;
			SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
		} else {
			intern->llist = other->llist;
			intern->traverse_pointer  = intern->llist->head;
			SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
		}

		intern->flags = other->flags;
	} else {
		intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(spl_ptr_llist_zval_ctor, spl_ptr_llist_zval_dtor);
		intern->traverse_pointer  = intern->llist->head;
		SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
	}

	while (parent) {
		if (parent == spl_ce_SplStack) {
			intern->flags |= (SPL_DLLIST_IT_FIX | SPL_DLLIST_IT_LIFO);
			intern->std.handlers = &spl_handler_SplDoublyLinkedList;
		} else if (parent == spl_ce_SplQueue) {
			intern->flags |= SPL_DLLIST_IT_FIX;
			intern->std.handlers = &spl_handler_SplDoublyLinkedList;
		}

		if (parent == spl_ce_SplDoublyLinkedList) {
			intern->std.handlers = &spl_handler_SplDoublyLinkedList;
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (!parent) { /* this must never happen */
		php_error_docref(NULL, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplDoublyLinkedList");
	}
	if (inherited) {
		intern->fptr_offset_get = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", sizeof("offsetget") - 1);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		intern->fptr_offset_set = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", sizeof("offsetset") - 1);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		intern->fptr_offset_has = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", sizeof("offsetexists") - 1);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		intern->fptr_offset_del = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset", sizeof("offsetunset") - 1);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		intern->fptr_count = zend_hash_str_find_ptr(&class_type->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return &intern->std;
}
/* }}} */

static zend_object *spl_dllist_object_new(zend_class_entry *class_type) /* {{{ */
{
	return spl_dllist_object_new_ex(class_type, NULL, 0);
}
/* }}} */

static zend_object *spl_dllist_object_clone(zval *zobject) /* {{{ */
{
	zend_object        *old_object;
	zend_object        *new_object;

	old_object  = Z_OBJ_P(zobject);
	new_object = spl_dllist_object_new_ex(old_object->ce, zobject, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

static int spl_dllist_object_count_elements(zval *object, zend_long *count) /* {{{ */
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(object);

	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (!Z_ISUNDEF(rv)) {
			*count = zval_get_long(&rv);
			zval_ptr_dtor(&rv);
			return SUCCESS;
		}
		*count = 0;
		return FAILURE;
	}

	*count = spl_ptr_llist_count(intern->llist);
	return SUCCESS;
}
/* }}} */

static HashTable* spl_dllist_object_get_debug_info(zval *obj, int *is_temp) /* {{{{ */
{
	spl_dllist_object     *intern  = Z_SPLDLLIST_P(obj);
	spl_ptr_llist_element *current = intern->llist->head, *next;
	zval tmp, dllist_array;
	zend_string *pnstr;
	int  i = 0;

	*is_temp = 0;

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		zend_hash_init(intern->debug_info, 1, NULL, ZVAL_PTR_DTOR, 0);
	}

	if (intern->debug_info->u.v.nApplyCount == 0) {

		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref);

		pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "flags", sizeof("flags")-1);
		ZVAL_LONG(&tmp, intern->flags);
		zend_hash_add(intern->debug_info, pnstr, &tmp);
		zend_string_release(pnstr);

		array_init(&dllist_array);

		while (current) {
			next = current->next;

			add_index_zval(&dllist_array, i, &current->data);
			if (Z_REFCOUNTED(current->data)) {
				Z_ADDREF(current->data);
			}
			i++;

			current = next;
		}

		pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "dllist", sizeof("dllist")-1);
		zend_hash_add(intern->debug_info, pnstr, &dllist_array);
		zend_string_release(pnstr);
	}

	return intern->debug_info;
}
/* }}}} */

/* {{{ proto bool SplDoublyLinkedList::push(mixed $value)
	   Push $value on the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, push)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_push(intern->llist, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::unshift(mixed $value)
	   Unshift $value on the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, unshift)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_unshift(intern->llist, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::pop()
	   Pop an element out of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, pop)
{
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_pop(intern->llist, return_value);

	if (Z_ISUNDEF_P(return_value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't pop from an empty datastructure", 0);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::shift()
	   Shift an element out of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, shift)
{
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_shift(intern->llist, return_value);

	if (Z_ISUNDEF_P(return_value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't shift from an empty datastructure", 0);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::top()
	   Peek at the top element of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, top)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	value = spl_ptr_llist_last(intern->llist);

	if (value == NULL || Z_ISUNDEF_P(value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty datastructure", 0);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::bottom()
	   Peek at the bottom element of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, bottom)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	value  = spl_ptr_llist_first(intern->llist);

	if (value == NULL || Z_ISUNDEF_P(value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty datastructure", 0);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::count()
 Return the number of elements in the datastructure. */
SPL_METHOD(SplDoublyLinkedList, count)
{
	zend_long count;
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	count = spl_ptr_llist_count(intern->llist);
	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::isEmpty()
 Return true if the SplDoublyLinkedList is empty. */
SPL_METHOD(SplDoublyLinkedList, isEmpty)
{
	zend_long count;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_object_count_elements(getThis(), &count);
	RETURN_BOOL(count == 0);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::setIteratorMode($flags)
 Set the mode of iteration */
SPL_METHOD(SplDoublyLinkedList, setIteratorMode)
{
	zend_long value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());

	if (intern->flags & SPL_DLLIST_IT_FIX
		&& (intern->flags & SPL_DLLIST_IT_LIFO) != (value & SPL_DLLIST_IT_LIFO)) {
		zend_throw_exception(spl_ce_RuntimeException, "Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen", 0);
		return;
	}

	intern->flags = value & SPL_DLLIST_IT_MASK;

	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::getIteratorMode()
 Return the mode of iteration */
SPL_METHOD(SplDoublyLinkedList, getIteratorMode)
{
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());

	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::offsetExists(mixed $index)
 Returns whether the requested $index exists. */
SPL_METHOD(SplDoublyLinkedList, offsetExists)
{
	zval              *zindex;
	spl_dllist_object *intern;
	zend_long               index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);

	RETURN_BOOL(index >= 0 && index < intern->llist->count);
} /* }}} */

/* {{{ proto mixed SplDoublyLinkedList::offsetGet(mixed $index)
 Returns the value at the specified $index. */
SPL_METHOD(SplDoublyLinkedList, offsetGet)
{
	zval                  *zindex;
	zend_long                   index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);

	if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0);
		return;
	}

	element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

	if (element != NULL) {
		RETURN_ZVAL(&element->data, 1, 0);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0);
		return;
	}
} /* }}} */

/* {{{ proto void SplDoublyLinkedList::offsetSet(mixed $index, mixed $newval)
 Sets the value at the specified $index to $newval. */
SPL_METHOD(SplDoublyLinkedList, offsetSet)
{
	zval                  *zindex, *value;
	spl_dllist_object     *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());

	if (Z_TYPE_P(zindex) == IS_NULL) {
		/* $obj[] = ... */
		spl_ptr_llist_push(intern->llist, value);
	} else {
		/* $obj[$foo] = ... */
		zend_long                   index;
		spl_ptr_llist_element *element;

		index = spl_offset_convert_to_long(zindex);

		if (index < 0 || index >= intern->llist->count) {
			zval_ptr_dtor(value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0);
			return;
		}

		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		if (element != NULL) {
			/* call dtor on the old element as in spl_ptr_llist_pop */
			if (intern->llist->dtor) {
				intern->llist->dtor(element);
			}

			/* the element is replaced, delref the old one as in
			 * SplDoublyLinkedList::pop() */
			zval_ptr_dtor(&element->data);
			ZVAL_COPY_VALUE(&element->data, value);

			/* new element, call ctor as in spl_ptr_llist_push */
			if (intern->llist->ctor) {
				intern->llist->ctor(element);
			}
		} else {
			zval_ptr_dtor(value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0);
			return;
		}
	}
} /* }}} */

/* {{{ proto void SplDoublyLinkedList::offsetUnset(mixed $index)
 Unsets the value at the specified $index. */
SPL_METHOD(SplDoublyLinkedList, offsetUnset)
{
	zval                  *zindex;
	zend_long             index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;
	spl_ptr_llist         *llist;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);
	llist  = intern->llist;

	if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset out of range", 0);
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

		if(llist->dtor) {
			llist->dtor(element);
		}

		if (intern->traverse_pointer == element) {
			SPL_LLIST_DELREF(element);
			intern->traverse_pointer = NULL;
		}
		zval_ptr_dtor(&element->data);
		ZVAL_UNDEF(&element->data);

		SPL_LLIST_DELREF(element);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0);
		return;
	}
} /* }}} */

static void spl_dllist_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	SPL_LLIST_CHECK_DELREF(iterator->traverse_pointer);

	zend_user_it_invalidate_current(iter);
	zval_ptr_dtor(&iterator->intern.it.data);
}
/* }}} */

static void spl_dllist_it_helper_rewind(spl_ptr_llist_element **traverse_pointer_ptr, int *traverse_position_ptr, spl_ptr_llist *llist, int flags) /* {{{ */
{
	SPL_LLIST_CHECK_DELREF(*traverse_pointer_ptr);

	if (flags & SPL_DLLIST_IT_LIFO) {
		*traverse_position_ptr = llist->count-1;
		*traverse_pointer_ptr  = llist->tail;
	} else {
		*traverse_position_ptr = 0;
		*traverse_pointer_ptr  = llist->head;
	}

	SPL_LLIST_CHECK_ADDREF(*traverse_pointer_ptr);
}
/* }}} */

static void spl_dllist_it_helper_move_forward(spl_ptr_llist_element **traverse_pointer_ptr, int *traverse_position_ptr, spl_ptr_llist *llist, int flags) /* {{{ */
{
	if (*traverse_pointer_ptr) {
		spl_ptr_llist_element *old = *traverse_pointer_ptr;

		if (flags & SPL_DLLIST_IT_LIFO) {
			*traverse_pointer_ptr = old->prev;
			(*traverse_position_ptr)--;

			if (flags & SPL_DLLIST_IT_DELETE) {
				zval prev;
				spl_ptr_llist_pop(llist, &prev);

				zval_ptr_dtor(&prev);
			}
		} else {
			*traverse_pointer_ptr = old->next;

			if (flags & SPL_DLLIST_IT_DELETE) {
				zval prev;
				spl_ptr_llist_shift(llist, &prev);

				zval_ptr_dtor(&prev);
			} else {
				(*traverse_position_ptr)++;
			}
		}

		SPL_LLIST_DELREF(old);
		SPL_LLIST_CHECK_ADDREF(*traverse_pointer_ptr);
	}
}
/* }}} */

static void spl_dllist_it_rewind(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object = Z_SPLDLLIST_P(&iter->data);
	spl_ptr_llist *llist = object->llist;

	spl_dllist_it_helper_rewind(&iterator->traverse_pointer, &iterator->traverse_position, llist, object->flags);
}
/* }}} */

static int spl_dllist_it_valid(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	return (element != NULL ? SUCCESS : FAILURE);
}
/* }}} */

static zval *spl_dllist_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	if (element == NULL || Z_ISUNDEF(element->data)) {
		return NULL;
	}

	return &element->data;
}
/* }}} */

static void spl_dllist_it_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	ZVAL_LONG(key, iterator->traverse_position);
}
/* }}} */

static void spl_dllist_it_move_forward(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object = Z_SPLDLLIST_P(&iter->data);

	zend_user_it_invalidate_current(iter);

	spl_dllist_it_helper_move_forward(&iterator->traverse_pointer, &iterator->traverse_position, object->llist, object->flags);
}
/* }}} */

/* {{{  proto int SplDoublyLinkedList::key()
   Return current array key */
SPL_METHOD(SplDoublyLinkedList, key)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->traverse_position);
}
/* }}} */

/* {{{ proto void SplDoublyLinkedList::prev()
   Move to next entry */
SPL_METHOD(SplDoublyLinkedList, prev)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_move_forward(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags ^ SPL_DLLIST_IT_LIFO);
}
/* }}} */

/* {{{ proto void SplDoublyLinkedList::next()
   Move to next entry */
SPL_METHOD(SplDoublyLinkedList, next)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_move_forward(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags);
}
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::valid()
   Check whether the datastructure contains more entries */
SPL_METHOD(SplDoublyLinkedList, valid)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->traverse_pointer != NULL);
}
/* }}} */

/* {{{ proto void SplDoublyLinkedList::rewind()
   Rewind the datastructure back to the start */
SPL_METHOD(SplDoublyLinkedList, rewind)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_rewind(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags);
}
/* }}} */

/* {{{ proto mixed|NULL SplDoublyLinkedList::current()
   Return current datastructure entry */
SPL_METHOD(SplDoublyLinkedList, current)
{
	spl_dllist_object     *intern  = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_element *element = intern->traverse_pointer;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (element == NULL || Z_ISUNDEF(element->data)) {
		RETURN_NULL();
	} else {
		RETURN_ZVAL(&element->data, 1, 0);
	}
}
/* }}} */

/* {{{ proto string SplDoublyLinkedList::serialize()
 Serializes storage */
SPL_METHOD(SplDoublyLinkedList, serialize)
{
	spl_dllist_object     *intern   = Z_SPLDLLIST_P(getThis());
	smart_str              buf      = {0};
	spl_ptr_llist_element *current  = intern->llist->head, *next;
	zval                   flags;
	php_serialize_data_t   var_hash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	/* flags */
	ZVAL_LONG(&flags, intern->flags);
	php_var_serialize(&buf, &flags, &var_hash);
	zval_ptr_dtor(&flags);

	/* elements */
	while (current) {
		smart_str_appendc(&buf, ':');
		next = current->next;

		php_var_serialize(&buf, &current->data, &var_hash);

		current = next;
	}

	smart_str_0(&buf);

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.s) {
		RETURN_STR(buf.s);
	} else {
		RETURN_NULL();
	}

} /* }}} */

/* {{{ proto void SplDoublyLinkedList::unserialize(string serialized)
 Unserializes storage */
SPL_METHOD(SplDoublyLinkedList, unserialize)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());
	zval flags, elem;
	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		return;
	}

	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	/* flags */
	if (!php_var_unserialize(&flags, &p, s + buf_len, &var_hash)) {
		goto error;
	}

	if (Z_TYPE(flags) != IS_LONG) {
		zval_ptr_dtor(&flags);
		goto error;
	}

	intern->flags = (int)Z_LVAL(flags);
	zval_ptr_dtor(&flags);

	/* elements */
	while(*p == ':') {
		++p;
		if (!php_var_unserialize(&elem, &p, s + buf_len, &var_hash)) {
			goto error;
		}

		spl_ptr_llist_push(intern->llist, &elem);
		zval_ptr_dtor(&elem);
	}

	if (*p != '\0') {
		goto error;
	}

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

error:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset %pd of %d bytes", (zend_long)((char*)p - buf), buf_len);
	return;

} /* }}} */

/* {{{ proto void SplDoublyLinkedList::add(mixed $index, mixed $newval)
 Inserts a new entry before the specified $index consisting of $newval. */
SPL_METHOD(SplDoublyLinkedList, add)
{
	zval                  *zindex, *value;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;
	zend_long                  index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);

	if (index < 0 || index > intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0);
		return;
	}

	if (Z_REFCOUNTED_P(value)) {
		Z_ADDREF_P(value);
	}
	if (index == intern->llist->count) {
		/* If index is the last entry+1 then we do a push because we're not inserting before any entry */
		spl_ptr_llist_push(intern->llist, value);
	} else {
		/* Create the new element we want to insert */
		spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

		/* Get the element we want to insert before */
		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		ZVAL_COPY_VALUE(&elem->data, value);
		elem->rc   = 1;
		/* connect to the neighbours */
		elem->next = element;
		elem->prev = element->prev;

		/* connect the neighbours to this new element */
		if (elem->prev == NULL) {
			intern->llist->head = elem;
		} else {
			element->prev->next = elem;
		}
		element->prev = elem;

		intern->llist->count++;

		if (intern->llist->ctor) {
			intern->llist->ctor(elem);
		}
	}
} /* }}} */

/* {{{ iterator handler table */
zend_object_iterator_funcs spl_dllist_it_funcs = {
	spl_dllist_it_dtor,
	spl_dllist_it_valid,
	spl_dllist_it_get_current_data,
	spl_dllist_it_get_current_key,
	spl_dllist_it_move_forward,
	spl_dllist_it_rewind
}; /* }}} */

zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	spl_dllist_it *iterator;
	spl_dllist_object *dllist_object = Z_SPLDLLIST_P(object);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0);
		return NULL;
	}

	iterator = emalloc(sizeof(spl_dllist_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_COPY(&iterator->intern.it.data, object);
	iterator->intern.it.funcs    = &spl_dllist_it_funcs;
	iterator->intern.ce          = ce;
	iterator->traverse_position  = dllist_object->traverse_position;
	iterator->traverse_pointer   = dllist_object->traverse_pointer;
	iterator->flags              = dllist_object->flags & SPL_DLLIST_IT_MASK;
	ZVAL_UNDEF(&iterator->intern.value);

	SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);

	return &iterator->intern.it;
}
/* }}} */

/*  Function/Class/Method definitions */
ZEND_BEGIN_ARG_INFO(arginfo_dllist_setiteratormode, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dllist_push, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dllist_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dllist_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dllist_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dllist_serialized, 0)
	ZEND_ARG_INFO(0, serialized)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_SplQueue[] = {
	SPL_MA(SplQueue, enqueue, SplDoublyLinkedList, push,  arginfo_dllist_push, ZEND_ACC_PUBLIC)
	SPL_MA(SplQueue, dequeue, SplDoublyLinkedList, shift, arginfo_dllist_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry spl_funcs_SplDoublyLinkedList[] = {
	SPL_ME(SplDoublyLinkedList, pop,             arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, shift,           arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, push,            arginfo_dllist_push,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, unshift,         arginfo_dllist_push,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, top,             arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, bottom,          arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, isEmpty,         arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, setIteratorMode, arginfo_dllist_setiteratormode, ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, getIteratorMode, arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	/* Countable */
	SPL_ME(SplDoublyLinkedList, count,           arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	/* ArrayAccess */
	SPL_ME(SplDoublyLinkedList, offsetExists,    arginfo_dllist_offsetGet,       ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetGet,       arginfo_dllist_offsetGet,       ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetSet,       arginfo_dllist_offsetSet,       ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, offsetUnset,     arginfo_dllist_offsetGet,       ZEND_ACC_PUBLIC)

	SPL_ME(SplDoublyLinkedList, add,             arginfo_dllist_offsetSet,       ZEND_ACC_PUBLIC)

	/* Iterator */
	SPL_ME(SplDoublyLinkedList, rewind,          arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, current,         arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, key,             arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, next,            arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, prev,            arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList, valid,           arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	/* Serializable */
	SPL_ME(SplDoublyLinkedList,  unserialize,    arginfo_dllist_serialized,      ZEND_ACC_PUBLIC)
	SPL_ME(SplDoublyLinkedList,  serialize,      arginfo_dllist_void,            ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(spl_dllist) /* {{{ */
{
	REGISTER_SPL_STD_CLASS_EX(SplDoublyLinkedList, spl_dllist_object_new, spl_funcs_SplDoublyLinkedList);
	memcpy(&spl_handler_SplDoublyLinkedList, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplDoublyLinkedList.offset = XtOffsetOf(spl_dllist_object, std);
	spl_handler_SplDoublyLinkedList.clone_obj = spl_dllist_object_clone;
	spl_handler_SplDoublyLinkedList.count_elements = spl_dllist_object_count_elements;
	spl_handler_SplDoublyLinkedList.get_debug_info = spl_dllist_object_get_debug_info;
	spl_handler_SplDoublyLinkedList.dtor_obj = zend_objects_destroy_object;
	spl_handler_SplDoublyLinkedList.free_obj = spl_dllist_object_free_storage;

	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_LIFO",  SPL_DLLIST_IT_LIFO);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_FIFO",  0);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_DELETE",SPL_DLLIST_IT_DELETE);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_KEEP",  0);

	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Countable);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Serializable);

	spl_ce_SplDoublyLinkedList->get_iterator = spl_dllist_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(SplQueue,           SplDoublyLinkedList,        spl_dllist_object_new, spl_funcs_SplQueue);
	REGISTER_SPL_SUB_CLASS_EX(SplStack,           SplDoublyLinkedList,        spl_dllist_object_new, NULL);

	spl_ce_SplQueue->get_iterator = spl_dllist_get_iterator;
	spl_ce_SplStack->get_iterator = spl_dllist_get_iterator;

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
