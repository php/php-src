/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
#include "ext/standard/php_smart_str.h"
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
	elem = NULL; \
}

#define SPL_LLIST_CHECK_DELREF(elem) if((elem) && !--(elem)->rc) { \
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

typedef void (*spl_ptr_llist_dtor_func)(spl_ptr_llist_element * TSRMLS_DC);
typedef void (*spl_ptr_llist_ctor_func)(spl_ptr_llist_element * TSRMLS_DC);

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
	zend_function         *fptr_count;
	zend_class_entry      *ce_get_iterator;
	HashTable             *debug_info;
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
static void spl_ptr_llist_zval_dtor(spl_ptr_llist_element *elem TSRMLS_DC) { /* {{{ */
	if (elem->data) {
		zval_ptr_dtor((zval **)&elem->data);
	}
}
/* }}} */

static void spl_ptr_llist_zval_ctor(spl_ptr_llist_element *elem TSRMLS_DC) { /* {{{ */
	Z_ADDREF_P((zval *)elem->data);
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

static long spl_ptr_llist_count(spl_ptr_llist *llist) /* {{{ */
{
	return (long)llist->count;
}
/* }}} */

static void spl_ptr_llist_destroy(spl_ptr_llist *llist TSRMLS_DC) /* {{{ */
{
	spl_ptr_llist_element   *current = llist->head, *next;
	spl_ptr_llist_dtor_func  dtor    = llist->dtor;

	while (current) {
		next = current->next;
		if(current && dtor) {
			dtor(current TSRMLS_CC);
		}
		SPL_LLIST_DELREF(current);
		current = next;
	}

	efree(llist);
}
/* }}} */

static spl_ptr_llist_element *spl_ptr_llist_offset(spl_ptr_llist *llist, long offset, int backward) /* {{{ */
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

static void spl_ptr_llist_unshift(spl_ptr_llist *llist, void *data TSRMLS_DC) /* {{{ */
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

	if (llist->ctor) {
		llist->ctor(elem TSRMLS_CC);
	}
}
/* }}} */

static void spl_ptr_llist_push(spl_ptr_llist *llist, void *data TSRMLS_DC) /* {{{ */
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

	if (llist->ctor) {
		llist->ctor(elem TSRMLS_CC);
	}
}
/* }}} */

static void *spl_ptr_llist_pop(spl_ptr_llist *llist TSRMLS_DC) /* {{{ */
{
	void                     *data;
	spl_ptr_llist_element    *tail = llist->tail;

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

	if (llist->dtor) {
		llist->dtor(tail TSRMLS_CC);
	}

	tail->data = NULL;

	SPL_LLIST_DELREF(tail);

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

static void *spl_ptr_llist_shift(spl_ptr_llist *llist TSRMLS_DC) /* {{{ */
{
	void                    *data;
	spl_ptr_llist_element   *head = llist->head;

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

	if (llist->dtor) {
		llist->dtor(head TSRMLS_CC);
	}
	head->data = NULL;

	SPL_LLIST_DELREF(head);

	return data;
}
/* }}} */

static void spl_ptr_llist_copy(spl_ptr_llist *from, spl_ptr_llist *to TSRMLS_DC) /* {{{ */
{
	spl_ptr_llist_element   *current = from->head, *next;
	spl_ptr_llist_ctor_func  ctor    = from->ctor;

	while (current) {
		next = current->next;

		if (ctor) {
			ctor(current TSRMLS_CC);
		}

		spl_ptr_llist_push(to, current->data TSRMLS_CC);
		current = next;
	}

}
/* }}} */

/* }}} */

static void spl_dllist_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_dllist_object *intern = (spl_dllist_object *)object;
	zval              *tmp    = NULL;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	while(intern->llist->count > 0) {
		tmp = (zval *)spl_ptr_llist_pop(intern->llist TSRMLS_CC);
		zval_ptr_dtor(&tmp);
	}

	spl_ptr_llist_destroy(intern->llist TSRMLS_CC);
	SPL_LLIST_CHECK_DELREF(intern->traverse_pointer);
	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}

	efree(object);
}
/* }}} */

zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

static zend_object_value spl_dllist_object_new_ex(zend_class_entry *class_type, spl_dllist_object **obj, zval *orig, int clone_orig TSRMLS_DC) /* {{{ */
{
	zend_object_value  retval;
	spl_dllist_object *intern;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_dllist_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	intern->flags = 0;
	intern->traverse_position = 0;
	intern->debug_info = NULL;

	if (orig) {
		spl_dllist_object *other = (spl_dllist_object*)zend_object_store_get_object(orig TSRMLS_CC);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(other->llist->ctor, other->llist->dtor);
			spl_ptr_llist_copy(other->llist, intern->llist TSRMLS_CC);
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
			retval.handlers = &spl_handler_SplDoublyLinkedList;
		} else if (parent == spl_ce_SplQueue) {
			intern->flags |= SPL_DLLIST_IT_FIX;
			retval.handlers = &spl_handler_SplDoublyLinkedList;
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
		zend_hash_find(&class_type->function_table, "count",        sizeof("count"),        (void **) &intern->fptr_count);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
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

	*count = spl_ptr_llist_count(intern->llist);
	return SUCCESS;
} 
/* }}} */

static HashTable* spl_dllist_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{{ */
{
	spl_dllist_object     *intern  = (spl_dllist_object*)zend_object_store_get_object(obj TSRMLS_CC);
	spl_ptr_llist_element *current = intern->llist->head, *next;
	zval *tmp, zrv, *dllist_array;
	char *pnstr;
	int  pnlen;
	int  i = 0;

	*is_temp = 0;

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		zend_hash_init(intern->debug_info, 1, NULL, ZVAL_PTR_DTOR, 0);
	}

	if (intern->debug_info->nApplyCount == 0) {
		INIT_PZVAL(&zrv);
		Z_ARRVAL(zrv) = intern->debug_info;

		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "flags", sizeof("flags")-1, &pnlen TSRMLS_CC);
		add_assoc_long_ex(&zrv, pnstr, pnlen+1, intern->flags);
		efree(pnstr);

		ALLOC_INIT_ZVAL(dllist_array);
		array_init(dllist_array);

		while (current) {
			next = current->next;

			add_index_zval(dllist_array, i, (zval *)current->data);
			Z_ADDREF_P(current->data);
			i++;

			current = next;
		}

		pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "dllist", sizeof("dllist")-1, &pnlen TSRMLS_CC);
		add_assoc_zval_ex(&zrv, pnstr, pnlen+1, dllist_array);
		efree(pnstr);
	}

	return intern->debug_info;
}
/* }}}} */

/* {{{ proto bool SplDoublyLinkedList::push(mixed $value) U
	   Push $value on the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, push)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}

	SEPARATE_ARG_IF_REF(value);

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_ptr_llist_push(intern->llist, value TSRMLS_CC);

	RETURN_TRUE;
} 
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::unshift(mixed $value) U
	   Unshift $value on the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, unshift)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}

	SEPARATE_ARG_IF_REF(value);

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_ptr_llist_unshift(intern->llist, value TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::pop() U
	   Pop an element out of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, pop)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value = (zval *)spl_ptr_llist_pop(intern->llist TSRMLS_CC);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't pop from an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 1);
} 
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::shift() U
	   Shift an element out of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, shift)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value  = (zval *)spl_ptr_llist_shift(intern->llist TSRMLS_CC);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't shift from an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 1);
} 
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::top() U
	   Peek at the top element of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, top)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value  = (zval *)spl_ptr_llist_last(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ proto mixed SplDoublyLinkedList::bottom() U
	   Peek at the bottom element of the SplDoublyLinkedList */
SPL_METHOD(SplDoublyLinkedList, bottom)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value  = (zval *)spl_ptr_llist_first(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty datastructure", 0 TSRMLS_CC);
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
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	count = spl_ptr_llist_count(intern->llist);
	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto int SplDoublyLinkedList::isEmpty() U
 Return true if the SplDoublyLinkedList is empty. */
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
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

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
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->flags);
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
	index  = spl_offset_convert_to_long(zindex TSRMLS_CC);

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
	index  = spl_offset_convert_to_long(zindex TSRMLS_CC);

    if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0 TSRMLS_CC);
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
		spl_ptr_llist_push(intern->llist, value TSRMLS_CC);
	} else {
		/* $obj[$foo] = ... */
		long                   index;
		spl_ptr_llist_element *element;

		index = spl_offset_convert_to_long(zindex TSRMLS_CC);

		if (index < 0 || index >= intern->llist->count) {
			zval_ptr_dtor(&value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0 TSRMLS_CC);
			return;
		}

		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		if (element != NULL) {
			/* call dtor on the old element as in spl_ptr_llist_pop */
			if (intern->llist->dtor) {
				intern->llist->dtor(element TSRMLS_CC);
			}

			/* the element is replaced, delref the old one as in
			 * SplDoublyLinkedList::pop() */
			zval_ptr_dtor((zval **)&element->data);
			element->data = value;

			/* new element, call ctor as in spl_ptr_llist_push */
			if (intern->llist->ctor) {
				intern->llist->ctor(element TSRMLS_CC);
			}
		} else {
			zval_ptr_dtor(&value);
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
	index  = (int)spl_offset_convert_to_long(zindex TSRMLS_CC);
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

		if(llist->dtor) {
			llist->dtor(element TSRMLS_CC);
		}

		zval_ptr_dtor((zval **)&element->data);
		element->data = NULL;

		SPL_LLIST_DELREF(element);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0 TSRMLS_CC);
		return;
	}
} /* }}} */

static void spl_dllist_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	SPL_LLIST_CHECK_DELREF(iterator->traverse_pointer);

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}
/* }}} */

static void spl_dllist_it_helper_rewind(spl_ptr_llist_element **traverse_pointer_ptr, int *traverse_position_ptr, spl_ptr_llist *llist, int flags TSRMLS_DC) /* {{{ */
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

static void spl_dllist_it_helper_move_forward(spl_ptr_llist_element **traverse_pointer_ptr, int *traverse_position_ptr, spl_ptr_llist *llist, int flags TSRMLS_DC) /* {{{ */
{
	if (*traverse_pointer_ptr) {
		spl_ptr_llist_element *old = *traverse_pointer_ptr;

		if (flags & SPL_DLLIST_IT_LIFO) {
			*traverse_pointer_ptr = old->prev;
			(*traverse_position_ptr)--;

			if (flags & SPL_DLLIST_IT_DELETE) {
				zval *prev = (zval *)spl_ptr_llist_pop(llist TSRMLS_CC);

				if (prev) {
					zval_ptr_dtor((zval **)&prev);
				}
			}
		} else {
			*traverse_pointer_ptr = old->next;

			if (flags & SPL_DLLIST_IT_DELETE) {
				zval *prev = (zval *)spl_ptr_llist_shift(llist TSRMLS_CC);

				if (prev) {
					zval_ptr_dtor((zval **)&prev);
				}
			} else {
				(*traverse_position_ptr)++;
			}
		}

		SPL_LLIST_DELREF(old);
		SPL_LLIST_CHECK_ADDREF(*traverse_pointer_ptr);
	}
}
/* }}} */

static void spl_dllist_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object   = iterator->object;
	spl_ptr_llist     *llist    = object->llist;

	spl_dllist_it_helper_rewind(&iterator->traverse_pointer, &iterator->traverse_position, llist, object->flags TSRMLS_CC);
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

	if (element == NULL || element->data == NULL) {
		*data = NULL;
	} else {
		*data = (zval **)&element->data;
	}
}
/* }}} */

static int spl_dllist_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
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

	spl_dllist_it_helper_move_forward(&iterator->traverse_pointer, &iterator->traverse_position, object->llist, object->flags TSRMLS_CC);
}
/* }}} */

/* {{{  proto int SplDoublyLinkedList::key() U
   Return current array key */
SPL_METHOD(SplDoublyLinkedList, key)
{
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->traverse_position);
}
/* }}} */

/* {{{ proto void SplDoublyLinkedList::prev() U
   Move to next entry */
SPL_METHOD(SplDoublyLinkedList, prev)
{
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_move_forward(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags ^ SPL_DLLIST_IT_LIFO TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SplDoublyLinkedList::next() U
   Move to next entry */
SPL_METHOD(SplDoublyLinkedList, next)
{
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_move_forward(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool SplDoublyLinkedList::valid() U
   Check whether the datastructure contains more entries */
SPL_METHOD(SplDoublyLinkedList, valid)
{
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->traverse_pointer != NULL);
}
/* }}} */

/* {{{ proto void SplDoublyLinkedList::rewind() U
   Rewind the datastructure back to the start */
SPL_METHOD(SplDoublyLinkedList, rewind)
{
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_rewind(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed|NULL SplDoublyLinkedList::current() U
   Return current datastructure entry */
SPL_METHOD(SplDoublyLinkedList, current)
{
	spl_dllist_object     *intern  = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_ptr_llist_element *element = intern->traverse_pointer;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (element == NULL || element->data == NULL) {
		RETURN_NULL();
	} else {
		zval *data    = (zval *)element->data;
		RETURN_ZVAL(data, 1, 0);
	}
}
/* }}} */
/* {{{ proto string SplDoublyLinkedList::serialize()
 Serializes storage */
SPL_METHOD(SplDoublyLinkedList, serialize)
{
	spl_dllist_object     *intern   = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	smart_str              buf      = {0};
	spl_ptr_llist_element *current  = intern->llist->head, *next;
    zval                   *flags;
	php_serialize_data_t   var_hash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	/* flags */
	MAKE_STD_ZVAL(flags);
	ZVAL_LONG(flags, intern->flags);
	php_var_serialize(&buf, &flags, &var_hash TSRMLS_CC);
	zval_ptr_dtor(&flags);

	/* elements */
	while (current) {
		smart_str_appendc(&buf, ':');
		next = current->next;

		php_var_serialize(&buf, (zval **)&current->data, &var_hash TSRMLS_CC);

		current = next;
	}

	smart_str_0(&buf);

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.c) {
		RETURN_STRINGL(buf.c, buf.len, 0);
	} else {
		RETURN_NULL();
	}
	
} /* }}} */

/* {{{ proto void SplDoublyLinkedList::unserialize(string serialized)
 Unserializes storage */
SPL_METHOD(SplDoublyLinkedList, unserialize)
{
	spl_dllist_object     *intern   = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval                  *flags, *elem;
	char *buf;
	int buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Serialized string cannot be empty");
		return;
	}

	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	/* flags */
	ALLOC_INIT_ZVAL(flags);
	if (!php_var_unserialize(&flags, &p, s + buf_len, &var_hash TSRMLS_CC) || Z_TYPE_P(flags) != IS_LONG) {
		zval_ptr_dtor(&flags);
		goto error;
	}
	intern->flags = Z_LVAL_P(flags);
	zval_ptr_dtor(&flags);

	/* elements */
	while(*p == ':') {
		++p;
		ALLOC_INIT_ZVAL(elem);
		if (!php_var_unserialize(&elem, &p, s + buf_len, &var_hash TSRMLS_CC)) {
			zval_ptr_dtor(&elem);
			goto error;
		}

		spl_ptr_llist_push(intern->llist, elem TSRMLS_CC);
	}

	if (*p != '\0') {
		goto error;
	}

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

error:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Error at offset %ld of %d bytes", (long)((char*)p - buf), buf_len);
	return;

} /* }}} */

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

	Z_ADDREF_P(object);

	iterator                     = emalloc(sizeof(spl_dllist_it));
	iterator->intern.it.data     = (void*)object;
	iterator->intern.it.funcs    = &spl_dllist_it_funcs;
	iterator->intern.ce          = ce;
	iterator->intern.value       = NULL;
	iterator->traverse_position  = dllist_object->traverse_position;
	iterator->traverse_pointer   = dllist_object->traverse_pointer;
	iterator->flags              = dllist_object->flags & SPL_DLLIST_IT_MASK;
	iterator->object             = dllist_object;

	SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);


	return (zend_object_iterator*)iterator;
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

	spl_handler_SplDoublyLinkedList.clone_obj      = spl_dllist_object_clone;
	spl_handler_SplDoublyLinkedList.count_elements = spl_dllist_object_count_elements;
	spl_handler_SplDoublyLinkedList.get_debug_info = spl_dllist_object_get_debug_info;

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
