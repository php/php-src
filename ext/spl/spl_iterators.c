/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_default_classes.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_directory.h"

#define INLINE inline

zend_class_entry *spl_ce_RecursiveIterator;
zend_class_entry *spl_ce_RecursiveIteratorIterator;
zend_class_entry *spl_ce_FilterIterator;
zend_class_entry *spl_ce_ParentIterator;
zend_class_entry *spl_ce_SeekableIterator;
zend_class_entry *spl_ce_LimitIterator;
zend_class_entry *spl_ce_CachingIterator;
zend_class_entry *spl_ce_CachingRecursiveIterator;

function_entry spl_funcs_RecursiveIterator[] = {
	SPL_ABSTRACT_ME(RecursiveIterator, hasChildren,  NULL)
	SPL_ABSTRACT_ME(RecursiveIterator, getChildren,  NULL)
	{NULL, NULL, NULL}
};

SPL_METHOD(RecursiveIteratorIterator, __construct);
SPL_METHOD(RecursiveIteratorIterator, rewind);
SPL_METHOD(RecursiveIteratorIterator, hasMore);
SPL_METHOD(RecursiveIteratorIterator, key);
SPL_METHOD(RecursiveIteratorIterator, current);
SPL_METHOD(RecursiveIteratorIterator, next);
SPL_METHOD(RecursiveIteratorIterator, getDepth);
SPL_METHOD(RecursiveIteratorIterator, getSubIterator);

static
ZEND_BEGIN_ARG_INFO(arginfo_recursive_it___construct, 0) 
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_recursive_it_getSubIterator, 0) 
	ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_RecursiveIteratorIterator[] = {
	SPL_ME(RecursiveIteratorIterator, __construct,   arginfo_recursive_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, hasMore,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, getDepth,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, getSubIterator,arginfo_recursive_it_getSubIterator, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

typedef enum {
	RIT_LEAVES_ONLY = 0,
	RIT_SELF_FIRST  = 1,
	RIT_CHILD_FIRST = 2
} RecursiveIteratorMode;

typedef enum {
	RS_NEXT  = 0,
	RS_TEST  = 1,
	RS_SELF  = 2,
	RS_CHILD = 3,
	RS_START = 4
} RecursiveIteratorState;

typedef struct _spl_sub_iterator {
	zend_object_iterator    *iterator;
	zval                    *zobject;
	zend_class_entry        *ce;
	RecursiveIteratorState  state;
} spl_sub_iterator;

typedef struct _spl_recursive_it_object {
	zend_object              std;
	spl_sub_iterator         *iterators;
	int                      level;
	RecursiveIteratorMode    mode;
} spl_recursive_it_object;

typedef struct _spl_recursive_it_iterator {
	zend_object_iterator   intern;
	zval                   *zobject;
} spl_recursive_it_iterator;

static zend_object_handlers spl_handlers_rec_it_it;
static zend_object_handlers spl_handlers_dual_it;

static void spl_recursive_it_dtor(zend_object_iterator *_iter TSRMLS_DC)
{
	spl_recursive_it_iterator *iter   = (spl_recursive_it_iterator*)_iter;
	spl_recursive_it_object   *object = (spl_recursive_it_object*)_iter->data;
	zend_object_iterator      *sub_iter;

	while (object->level) {
		sub_iter = object->iterators[object->level].iterator;
		sub_iter->funcs->dtor(sub_iter TSRMLS_CC);
		zval_ptr_dtor(&object->iterators[object->level--].zobject);
	}
	erealloc(object->iterators, sizeof(spl_sub_iterator));
	object->level = 0;

	zval_ptr_dtor(&iter->zobject);	
	efree(iter);
}
	
static int spl_recursive_it_has_more_ex(spl_recursive_it_object *object TSRMLS_DC)
{
	zend_object_iterator      *sub_iter;
	int                       level = object->level;
	
	while (level >=0) {
		sub_iter = object->iterators[level].iterator;
		if (sub_iter->funcs->has_more(sub_iter TSRMLS_CC) == SUCCESS) {
			return SUCCESS;
		}
		level--;
	}
	return FAILURE;
}

static int spl_recursive_it_has_more(zend_object_iterator *iter TSRMLS_DC)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)iter->data;
	
	return spl_recursive_it_has_more_ex(object TSRMLS_CC);
}

static void spl_recursive_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)iter->data;
	zend_object_iterator      *sub_iter = object->iterators[object->level].iterator;
	
	sub_iter->funcs->get_current_data(sub_iter, data TSRMLS_CC);
}

static int spl_recursive_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)iter->data;
	zend_object_iterator      *sub_iter = object->iterators[object->level].iterator;

	if (sub_iter->funcs->get_current_key) {
		return sub_iter->funcs->get_current_key(sub_iter, str_key, str_key_len, int_key TSRMLS_CC);
	} else {
		*int_key = iter->index;
		return HASH_KEY_IS_LONG;
	}
}

static void spl_recursive_it_move_forward_ex(spl_recursive_it_object *object TSRMLS_DC)
{
	zend_object_iterator      *iterator;
	zval                      *zobject;
	zend_class_entry          *ce;
	zval                      *retval, *child;
	zend_object_iterator      *sub_iter;

	while (1) {
next_step:
		iterator = object->iterators[object->level].iterator;
		switch (object->iterators[object->level].state) {
			case RS_NEXT:
				iterator->funcs->move_forward(iterator TSRMLS_CC);
			case RS_START:
				if (iterator->funcs->has_more(iterator TSRMLS_CC) == FAILURE) {
					break;
				}
				object->iterators[object->level].state = RS_TEST;					
				/* break; */
			case RS_TEST:
				ce = object->iterators[object->level].ce;
				zobject = object->iterators[object->level].zobject;
				zend_call_method_with_0_params(&zobject, ce, NULL, "haschildren", &retval);
				if (zend_is_true(retval)) {
					zval_ptr_dtor(&retval);
					switch (object->mode) {
						case RIT_LEAVES_ONLY:
						case RIT_CHILD_FIRST:
							object->iterators[object->level].state = RS_CHILD;
							goto next_step;
						case RIT_SELF_FIRST:
							object->iterators[object->level].state = RS_SELF;
							goto next_step;
					}
				}
				zval_ptr_dtor(&retval);
				object->iterators[object->level].state = RS_NEXT;
				return /* self */;
			case RS_SELF:
				if (object->mode == RIT_SELF_FIRST) {
					object->iterators[object->level].state = RS_CHILD;
				} else {
					object->iterators[object->level].state = RS_NEXT;
				}
				return /* self */;
			case RS_CHILD:
				ce = object->iterators[object->level].ce;
				zobject = object->iterators[object->level].zobject;
				zend_call_method_with_0_params(&zobject, ce, NULL, "getchildren", &child);
				ce = Z_OBJCE_P(child);
				if (!ce || !instanceof_function(ce, spl_ce_RecursiveIterator TSRMLS_CC)) {
					zval_ptr_dtor(&child);
					zend_throw_exception(zend_exception_get_default(), "Objects returned by RecursiveIterator::getChildren() must implement RecursiveIterator", 0 TSRMLS_CC);
					return;
				}
				if (object->mode == RIT_CHILD_FIRST) {
					object->iterators[object->level].state = RS_SELF;
				} else {
					object->iterators[object->level].state = RS_NEXT;
				}
				object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator) * (++object->level+1));
				sub_iter = ce->get_iterator(ce, child TSRMLS_CC);
				object->iterators[object->level].iterator = sub_iter;
				object->iterators[object->level].zobject = child;
				object->iterators[object->level].ce = ce;
				object->iterators[object->level].state = RS_START;
				if (sub_iter->funcs->rewind) {
					sub_iter->funcs->rewind(sub_iter TSRMLS_CC);
				}
				goto next_step;
		}
		/* no more elements */
		if (object->level > 0) {
			iterator->funcs->dtor(iterator TSRMLS_CC);
			zval_ptr_dtor(&object->iterators[object->level].zobject);
			object->level--;
		} else {
			return; /* done completeley */
		}
	}
}

static void spl_recursive_it_rewind_ex(spl_recursive_it_object *object TSRMLS_DC)
{
	zend_object_iterator      *sub_iter;

	while (object->level) {
		sub_iter = object->iterators[object->level].iterator;
		sub_iter->funcs->dtor(sub_iter TSRMLS_CC);
		zval_ptr_dtor(&object->iterators[object->level--].zobject);
	}
	erealloc(object->iterators, sizeof(spl_sub_iterator));
	object->iterators[0].state = RS_START;
	sub_iter = object->iterators[0].iterator;
	if (sub_iter->funcs->rewind) {
		sub_iter->funcs->rewind(sub_iter TSRMLS_CC);
	}
	spl_recursive_it_move_forward_ex(object TSRMLS_CC);
}

static void spl_recursive_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_recursive_it_move_forward_ex((spl_recursive_it_object*)iter->data TSRMLS_CC);
}

static void spl_recursive_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	spl_recursive_it_rewind_ex((spl_recursive_it_object*)iter->data TSRMLS_CC);
}

static zend_object_iterator *spl_recursive_it_get_iterator(zend_class_entry *ce, zval *zobject TSRMLS_DC)
{
	spl_recursive_it_iterator *iterator = emalloc(sizeof(spl_recursive_it_iterator));
	spl_recursive_it_object   *object   = (spl_recursive_it_object*)zend_object_store_get_object(zobject TSRMLS_CC);

	zobject->refcount++;
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = ce->iterator_funcs.funcs;
	iterator->zobject = zobject;
	return (zend_object_iterator*)iterator;
}

zend_object_iterator_funcs spl_recursive_it_iterator_funcs = {
	spl_recursive_it_dtor,
	spl_recursive_it_has_more,
	spl_recursive_it_get_current_data,
	spl_recursive_it_get_current_key,
	spl_recursive_it_move_forward,
	spl_recursive_it_rewind
};

SPL_METHOD(RecursiveIteratorIterator, __construct)
{
	zval                      *object = getThis();
	spl_recursive_it_object   *intern;
	zval                      *iterator;
	zend_class_entry          *ce_iterator;
	int                       mode = RIT_LEAVES_ONLY;

	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &iterator, spl_ce_RecursiveIterator, &mode) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	intern = (spl_recursive_it_object*)zend_object_store_get_object(object TSRMLS_CC);
	intern->iterators = emalloc(sizeof(spl_sub_iterator));
	intern->level = 0;
	intern->mode = mode;
	ce_iterator = Z_OBJCE_P(iterator); /* respect inheritance, don't use spl_ce_RecursiveIterator */
	intern->iterators[0].iterator = ce_iterator->get_iterator(ce_iterator, iterator TSRMLS_CC);
	iterator->refcount++;
	intern->iterators[0].zobject = iterator;
	intern->iterators[0].ce = ce_iterator;
	intern->iterators[0].state = RS_START;

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}

SPL_METHOD(RecursiveIteratorIterator, rewind)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_recursive_it_rewind_ex(object TSRMLS_CC);
}

SPL_METHOD(RecursiveIteratorIterator, hasMore)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_recursive_it_has_more_ex(object TSRMLS_CC) == SUCCESS);
}

SPL_METHOD(RecursiveIteratorIterator, key)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_object_iterator      *iterator = object->iterators[object->level].iterator;

	if (iterator->funcs->get_current_key) {
		char *str_key;
		uint str_key_len;
		ulong int_key;
		if (iterator->funcs->get_current_key(iterator, &str_key, &str_key_len, &int_key TSRMLS_CC) == HASH_KEY_IS_LONG) {
			RETURN_LONG(int_key);
		} else {
			RETURN_STRINGL(str_key, str_key_len-1, 0);
		}
	} else {
		RETURN_NULL();
	}
}

SPL_METHOD(RecursiveIteratorIterator, current)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_object_iterator      *iterator = object->iterators[object->level].iterator;
	zval                      **data;

	iterator->funcs->get_current_data(iterator, &data TSRMLS_CC);
	RETURN_ZVAL(*data, 1, 0);
}

SPL_METHOD(RecursiveIteratorIterator, next)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_recursive_it_move_forward_ex(object TSRMLS_CC);
}

SPL_METHOD(RecursiveIteratorIterator, getDepth)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	RETURN_LONG(object->level);
}

SPL_METHOD(RecursiveIteratorIterator, getSubIterator)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int  level = object->level;
	zval *zobject;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &level) == FAILURE) {
		return;
	}
	if (level < 0 || level > object->level) {
		RETURN_NULL();
	}
	zobject = object->iterators[level].zobject;
	RETURN_ZVAL(zobject, 1, 0);
}

/* {{{ spl_RecursiveIteratorIterator_dtor */
static void spl_RecursiveIteratorIterator_dtor(void *_object, zend_object_handle handle TSRMLS_DC)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object *)_object;
	zend_object_iterator      *sub_iter;

	if (object->iterators) {
		while (object->level >= 0) {
			sub_iter = object->iterators[object->level].iterator;
			sub_iter->funcs->dtor(sub_iter TSRMLS_CC);
			zval_ptr_dtor(&object->iterators[object->level--].zobject);
		}
		efree(object->iterators);
	}

	zend_hash_destroy(object->std.properties);
	FREE_HASHTABLE(object->std.properties);

	efree(object);
}
/* }}} */

static int spl_recursive_it_gets_implemented(zend_class_entry *iface, zend_class_entry *class_type TSRMLS_DC)
{
	return SUCCESS;
}

/* {{{ spl_RecursiveIteratorIterator_new */
static zend_object_value spl_RecursiveIteratorIterator_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	spl_recursive_it_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_recursive_it_object));
	memset(intern, 0, sizeof(spl_recursive_it_object));
	intern->std.ce = class_type;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, spl_RecursiveIteratorIterator_dtor, NULL TSRMLS_CC);
	retval.handlers = &spl_handlers_rec_it_it;
	return retval;
}
/* }}} */

#if MBO_0
static int spl_dual_it_gets_implemented(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
	class_type->iterator_funcs.zf_has_more = NULL;
	class_type->iterator_funcs.zf_current = NULL;
	class_type->iterator_funcs.zf_key = NULL;
	class_type->iterator_funcs.zf_next = NULL;
	class_type->iterator_funcs.zf_rewind = NULL;
	if (!class_type->iterator_funcs.funcs) {
		class_type->iterator_funcs.funcs = &zend_interface_iterator_funcs_iterator;
	}

	return SUCCESS;
}
#endif

static union _zend_function *spl_dual_it_get_method(zval *object, char *method, int method_len TSRMLS_DC)
{
	union _zend_function *function_handler;
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(object TSRMLS_CC);

	/*fprintf(stderr, "Call method(%s): %s::%s()  ", Z_OBJCE_P(object)->name, intern->inner.ce->name, method);*/
	function_handler = std_object_handlers.get_method(object, method, method_len TSRMLS_CC);
	if (!function_handler) {
		if (zend_hash_find(&intern->inner.ce->function_table, method, method_len+1, (void **) &function_handler) == FAILURE) {
			if (Z_OBJ_HT_P(intern->inner.zobject)->get_method) {
				function_handler = Z_OBJ_HT_P(intern->inner.zobject)->get_method(intern->inner.zobject, method, method_len TSRMLS_CC);
			}
		}
	}
	/*fprintf(stderr, "%sfound\n", function_handler ? "" : "not ");*/
	return function_handler;
}

#if MBO_0
int spl_dual_it_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	zval ***func_params, func;
	zval *retval_ptr;
	int arg_count;
	int current = 0;
	int success;
	void **p;
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	ZVAL_STRING(&func, method, 0);
	if (!zend_is_callable(&func, 0, &method)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Method %s::%s() does not exist", intern->inner.ce->name, method);
		return FAILURE;
	}

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	func_params = safe_emalloc(sizeof(zval **), arg_count, 0);

	current = 0;
	while (arg_count-- > 0) {
		func_params[current] = (zval **) p - (arg_count-current);
		current++;
	}

	if (call_user_function_ex(EG(function_table), NULL, &func, &retval_ptr, arg_count, func_params, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		RETURN_ZVAL(retval_ptr, 0, 1);
		
		success = SUCCESS;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to call %s::%s()", intern->inner.ce->name, method);
		success = FAILURE;
	}

	efree(func_params); 
	return success;
}
#endif

static INLINE spl_dual_it_object* spl_dual_it_construct(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_inner, dual_it_type dit_type)
{
	zval                 *zobject;
	spl_dual_it_object   *intern;

	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->dit_type = dit_type;
	switch (dit_type) {
		case DIT_LimitIterator: {
			intern->u.limit.count = -1; /* get all */
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|ll", &zobject, ce_inner, &intern->u.limit.offset, &intern->u.limit.count) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			if (intern->u.limit.offset < 0) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				zend_throw_exception(zend_exception_get_default(), "Parameter offset must be > 0", 0 TSRMLS_CC);
				return NULL;
			}
			if (intern->u.limit.count < 0 && intern->u.limit.count != -1) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				zend_throw_exception(zend_exception_get_default(), "Parameter count must either be -1 or a value greater than or equal 0", 0 TSRMLS_CC);
				return NULL;
			}
			break;
		}
		case DIT_CachingIterator:
		case DIT_CachingRecursiveIterator: {
			long flags = CIT_CALL_TOSTRING;
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &zobject, ce_inner, &flags) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			intern->u.caching.flags |= flags & CIT_PUBLIC;
			break;
		}
		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zobject, ce_inner) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			break;
	}

	zobject->refcount++;
	intern->inner.zobject = zobject;
	intern->inner.ce = Z_OBJCE_P(zobject);
	intern->inner.object = zend_object_store_get_object(zobject TSRMLS_CC);
	intern->inner.iterator = intern->inner.ce->get_iterator(intern->inner.ce, zobject TSRMLS_CC);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	return intern;
}

SPL_METHOD(dual_it, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, zend_ce_iterator, DIT_Default);
}

static INLINE void spl_dual_it_free(spl_dual_it_object *intern TSRMLS_DC)
{
	if (intern->current.data && *intern->current.data) {
		zval_ptr_dtor(intern->current.data);
		intern->current.data = NULL;
	}
	if (intern->current.str_key) {
		efree(intern->current.str_key);
		intern->current.str_key = NULL;
	}
	if (intern->dit_type == DIT_CachingIterator || intern->dit_type == DIT_CachingRecursiveIterator) {
		if (intern->u.caching.zstr) {
			zval_ptr_dtor(&intern->u.caching.zstr);
			intern->u.caching.zstr = NULL;
		}
		if (intern->u.caching.zchildren) {
			zval_ptr_dtor(&intern->u.caching.zchildren);
			intern->u.caching.zchildren = NULL;
		}
	}
}

static INLINE void spl_dual_it_rewind(spl_dual_it_object *intern TSRMLS_DC)
{
	spl_dual_it_free(intern TSRMLS_CC);
	intern->current.pos = 0;
	if (intern->inner.iterator->funcs->rewind) {
		intern->inner.iterator->funcs->rewind(intern->inner.iterator TSRMLS_CC);
	}
}

static INLINE int spl_dual_it_has_more(spl_dual_it_object *intern TSRMLS_DC)
{
	/* FAILURE / SUCCESS */
	return intern->inner.iterator->funcs->has_more(intern->inner.iterator TSRMLS_CC);
}

static INLINE void spl_dual_it_fetch(spl_dual_it_object *intern, int check_more TSRMLS_DC)
{
	spl_dual_it_free(intern TSRMLS_CC);
	if (!check_more || spl_dual_it_has_more(intern TSRMLS_CC) == SUCCESS) {
		intern->inner.iterator->funcs->get_current_data(intern->inner.iterator, &intern->current.data TSRMLS_CC);
		(*intern->current.data)->refcount++;
		if (intern->inner.iterator->funcs->get_current_key) {
			intern->current.key_type = intern->inner.iterator->funcs->get_current_key(intern->inner.iterator, &intern->current.str_key, &intern->current.str_key_len, &intern->current.int_key TSRMLS_CC);
		} else {
			intern->current.key_type = HASH_KEY_IS_LONG;
			intern->current.int_key = intern->current.pos;
		}
	}
}

static INLINE void spl_dual_it_next(spl_dual_it_object *intern, int do_free TSRMLS_DC)
{
	if (do_free) {
		spl_dual_it_free(intern TSRMLS_CC);
	}
	intern->inner.iterator->funcs->move_forward(intern->inner.iterator TSRMLS_CC);
	intern->current.pos++;
}

SPL_METHOD(dual_it, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_dual_it_fetch(intern, 1 TSRMLS_CC);
}

SPL_METHOD(dual_it, hasMore)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->current.data);
}

SPL_METHOD(dual_it, key)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->current.data) {
		if (intern->current.key_type == IS_STRING) {
			RETURN_STRINGL(intern->current.str_key, intern->current.str_key_len, 1);
		} else {
			RETURN_LONG(intern->current.int_key);
		}
	}
	RETURN_NULL();
}

SPL_METHOD(dual_it, current)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->current.data) {
		RETVAL_ZVAL(*intern->current.data, 1, 0);
	} else {
		RETURN_NULL();
	}
}

SPL_METHOD(dual_it, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_dual_it_next(intern, 1 TSRMLS_CC);
	spl_dual_it_fetch(intern, 1 TSRMLS_CC);
}

static INLINE void spl_filter_it_fetch(zval *zobject, spl_dual_it_object *intern TSRMLS_DC)
{
	zval *retval;

	while (spl_dual_it_has_more(intern TSRMLS_CC) == SUCCESS) {
		spl_dual_it_fetch(intern, 1 TSRMLS_CC);

		zend_call_method_with_0_params(&zobject, intern->std.ce, NULL, "accept", &retval);
		if (zend_is_true(retval)) {
			zval_ptr_dtor(&retval);
			return;
		}
		zval_ptr_dtor(&retval);

		intern->inner.iterator->funcs->move_forward(intern->inner.iterator TSRMLS_CC);
	}
	spl_dual_it_free(intern TSRMLS_CC);
}

SPL_METHOD(FilterIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_filter_it_fetch(getThis(), intern TSRMLS_CC);
}

SPL_METHOD(FilterIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_dual_it_next(intern, 1 TSRMLS_CC);
	spl_filter_it_fetch(getThis(), intern TSRMLS_CC);
}

SPL_METHOD(ParentIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveIterator, DIT_Default);
}

SPL_METHOD(ParentIterator, hasChildren)
{
	spl_dual_it_object   *intern;
	zval                 *retval;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "haschildren", &retval);
	RETURN_ZVAL(retval, 0, 1);
}

SPL_METHOD(ParentIterator, getChildren)
{
	spl_dual_it_object   *intern;
	zval                 *retval;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "getchildren", &retval);
	spl_instantiate_arg_ex1(spl_ce_ParentIterator, &return_value, 0, retval TSRMLS_CC);
	zval_ptr_dtor(&retval);
}

/* {{{ spl_dual_it_dtor */
static INLINE void spl_dual_it_dtor(void *_object, zend_object_handle handle TSRMLS_DC)
{
	spl_dual_it_object        *object = (spl_dual_it_object *)_object;

	spl_dual_it_free(object TSRMLS_CC);

	if (object->inner.zobject) {
		zval_ptr_dtor(&object->inner.zobject);
	}

	if (object->inner.iterator) {
		object->inner.iterator->funcs->dtor(object->inner.iterator TSRMLS_CC);
	}

	zend_hash_destroy(object->std.properties);
	FREE_HASHTABLE(object->std.properties);

	efree(object);
}
/* }}} */

/* {{{ spl_dual_it_new */
static zend_object_value spl_dual_it_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	spl_dual_it_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_dual_it_object));
	memset(intern, 0, sizeof(spl_dual_it_object));
	intern->std.ce = class_type;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, spl_dual_it_dtor, NULL TSRMLS_CC);
	retval.handlers = &spl_handlers_dual_it;
	return retval;
}
/* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_filter_it___construct, 0) 
	ZEND_ARG_INFO(0, iterator)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_FilterIterator[] = {
	SPL_ME(dual_it,        __construct,   arginfo_filter_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(FilterIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,        hasMore,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,        key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,        current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(FilterIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ABSTRACT_ME(FilterIterator, accept, NULL)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO(arginfo_parent_it___construct, 0) 
	ZEND_ARG_INFO(0, iterator)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_ParentIterator[] = {
	SPL_ME(ParentIterator, __construct,   arginfo_parent_it___construct, ZEND_ACC_PUBLIC)
	SPL_MA(ParentIterator, accept,        ParentIterator, hasChildren, NULL, ZEND_ACC_PUBLIC)
	SPL_ME(ParentIterator, hasChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(ParentIterator, getChildren,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static INLINE void spl_limit_it_seek(spl_dual_it_object *intern, long pos TSRMLS_DC)
{
	zval  zpos;

	spl_dual_it_free(intern TSRMLS_CC);
	if (pos < intern->u.limit.offset) {
		zend_throw_exception_ex(zend_exception_get_default(), 0 TSRMLS_CC, "Cannot seek to %ld which is below the offset %ld", pos, intern->u.limit.offset);
		return;
	}
	if (pos > intern->u.limit.offset + intern->u.limit.count && intern->u.limit.count != -1) {
		zend_throw_exception_ex(zend_exception_get_default(), 0 TSRMLS_CC, "Cannot seek to %ld which is behind offest %ld plus count %ld", pos, intern->u.limit.offset, intern->u.limit.count);
		return;
	}
	if (instanceof_function(intern->inner.ce, spl_ce_SeekableIterator TSRMLS_CC)) {
		INIT_PZVAL(&zpos);
		ZVAL_LONG(&zpos, pos);
		zend_call_method_with_1_params(&intern->inner.zobject, intern->inner.ce, NULL, "seek", NULL, &zpos);
		intern->current.pos = pos;
	} else {
		/* emulate the forward seek, by next() calls */
		/* a back ward seek is done by a previous rewind() */
		if (pos < intern->current.pos) {
			spl_dual_it_rewind(intern TSRMLS_CC);
		}
		while (pos > intern->current.pos && spl_dual_it_has_more(intern TSRMLS_CC) == SUCCESS) {
			spl_dual_it_next(intern, 1 TSRMLS_CC);
		}
		if (spl_dual_it_has_more(intern TSRMLS_CC) == SUCCESS) {
			spl_dual_it_fetch(intern, 1 TSRMLS_CC);
		}
	}
}

SPL_METHOD(LimitIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, zend_ce_iterator, DIT_LimitIterator);
}

SPL_METHOD(LimitIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_limit_it_seek(intern, intern->u.limit.offset TSRMLS_CC);
}

SPL_METHOD(LimitIterator, hasMore)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL((intern->u.limit.count == -1 || intern->current.pos < intern->u.limit.offset + intern->u.limit.count) && intern->current.data);
}

SPL_METHOD(LimitIterator, seek)
{
	spl_dual_it_object   *intern;
	long                 pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pos) == FAILURE) {
		return;
	}

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_limit_it_seek(intern, pos TSRMLS_CC);
	RETURN_LONG(intern->current.pos);
}

SPL_METHOD(LimitIterator, getPosition)
{
	spl_dual_it_object   *intern;
	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_LONG(intern->current.pos);
}

static
ZEND_BEGIN_ARG_INFO(arginfo_seekable_it_seek, 0) 
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_SeekableIterator[] = {
	SPL_ABSTRACT_ME(SeekableIterator, seek, arginfo_seekable_it_seek)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO(arginfo_limit_it___construct, 0) 
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_limit_it_seek, 0) 
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_LimitIterator[] = {
	SPL_ME(LimitIterator, __construct,   arginfo_limit_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator, hasMore,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,       key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,       current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,       next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator, seek,          arginfo_limit_it_seek, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator, getPosition,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static INLINE int spl_caching_it_has_more(spl_dual_it_object *intern TSRMLS_DC)
{
	return intern->u.caching.flags & CIT_HAS_MORE ? SUCCESS : FAILURE;
}

static INLINE int spl_caching_it_has_next(spl_dual_it_object *intern TSRMLS_DC)
{
	return spl_dual_it_has_more(intern TSRMLS_CC);
}

static INLINE void spl_caching_it_next(spl_dual_it_object *intern TSRMLS_DC)
{
	if (spl_dual_it_has_more(intern TSRMLS_CC) == SUCCESS) {
		intern->u.caching.flags |= CIT_HAS_MORE;
		spl_dual_it_fetch(intern, 0 TSRMLS_CC);
		if (intern->dit_type == DIT_CachingRecursiveIterator) {
			zval *retval, *zchildren, zflags;
			zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "haschildren", &retval);
			if (zend_is_true(retval)) {
				zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "getchildren", &zchildren);
				if (EG(exception) && intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
					zval_ptr_dtor(&EG(exception));
					EG(exception) = NULL;
					if (zchildren) {
						zval_ptr_dtor(&zchildren);
					}
				} else {
					INIT_PZVAL(&zflags);
					ZVAL_LONG(&zflags, intern->u.caching.flags & CIT_PUBLIC);
					spl_instantiate_arg_ex2(spl_ce_CachingRecursiveIterator, &intern->u.caching.zchildren, 1, zchildren, &zflags TSRMLS_CC);
					zval_ptr_dtor(&zchildren);
				}
			}
			zval_ptr_dtor(&retval);		
		}
		if (intern->u.caching.flags & CIT_CALL_TOSTRING) {
			zend_class_entry *ce_data = spl_get_class_entry(*intern->current.data TSRMLS_CC);
			if (ce_data && zend_hash_exists(&ce_data->function_table, "__tostring", sizeof("__tostring"))) {
				zend_call_method_with_0_params(intern->current.data, ce_data, NULL, "__tostring", &intern->u.caching.zstr);
			} else {
				ALLOC_ZVAL(intern->u.caching.zstr);
				 *intern->u.caching.zstr = **intern->current.data;
				zval_copy_ctor(intern->u.caching.zstr);
				INIT_PZVAL(intern->u.caching.zstr);
				convert_to_string(intern->u.caching.zstr);
			}
		}
		spl_dual_it_next(intern, 0 TSRMLS_CC);	
	} else {
		intern->u.caching.flags &= ~CIT_HAS_MORE;
		spl_dual_it_next(intern, 1 TSRMLS_CC);	
	}
}

static INLINE void spl_caching_it_rewind(spl_dual_it_object *intern TSRMLS_DC)
{
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_caching_it_next(intern TSRMLS_CC);
}

SPL_METHOD(CachingIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, zend_ce_iterator, DIT_CachingIterator);
}

SPL_METHOD(CachingIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_caching_it_rewind(intern TSRMLS_CC);
}

SPL_METHOD(CachingIterator, hasMore)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_caching_it_has_more(intern TSRMLS_CC) == SUCCESS);
}

SPL_METHOD(CachingIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_caching_it_next(intern TSRMLS_CC);
}

SPL_METHOD(CachingIterator, hasNext)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_caching_it_has_next(intern TSRMLS_CC) == SUCCESS);
}

SPL_METHOD(CachingIterator, __toString)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_CALL_TOSTRING))	{
		zend_throw_exception_ex(zend_exception_get_default(), 0 TSRMLS_CC, "%s does not fetch string value (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
	}
	if (intern->u.caching.zstr) {
		RETURN_STRINGL(Z_STRVAL_P(intern->u.caching.zstr), Z_STRLEN_P(intern->u.caching.zstr), 1);
	} else {
		RETURN_NULL();
	}
}

static
ZEND_BEGIN_ARG_INFO(arginfo_caching_it___construct, 0) 
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_INFO(0, getStrVal)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_CachingIterator[] = {
	SPL_ME(CachingIterator, __construct,   arginfo_caching_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, hasMore,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, hasNext,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, __toString,    NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

SPL_METHOD(CachingRecursiveIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveIterator, DIT_CachingRecursiveIterator);
}

SPL_METHOD(CachingRecursiveIterator, hasChildren)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->u.caching.zchildren);
}

SPL_METHOD(CachingRecursiveIterator, getChildren)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->u.caching.zchildren) {
		RETURN_ZVAL(intern->u.caching.zchildren, 1, 0);
	} else {
		RETURN_NULL();
	}
}

static
ZEND_BEGIN_ARG_INFO(arginfo_caching_rec_it___construct, 0) 
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_INFO(0, getStrVal)
	ZEND_ARG_INFO(0, catch_getChildren)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_CachingRecursiveIterator[] = {
	SPL_ME(CachingRecursiveIterator, __construct,   arginfo_caching_rec_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(CachingRecursiveIterator, hasChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingRecursiveIterator, getChildren,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION(spl_iterators)
 */
PHP_MINIT_FUNCTION(spl_iterators)
{
	REGISTER_SPL_INTERFACE(RecursiveIterator);
	REGISTER_SPL_ITERATOR(RecursiveIterator);

	REGISTER_SPL_STD_CLASS_EX(RecursiveIteratorIterator, spl_RecursiveIteratorIterator_new, spl_funcs_RecursiveIteratorIterator);
	REGISTER_SPL_ITERATOR(RecursiveIteratorIterator);

	memcpy(&spl_handlers_rec_it_it, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_handlers_rec_it_it.clone_obj = NULL;

	memcpy(&spl_handlers_dual_it, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_handlers_dual_it.get_method = spl_dual_it_get_method;
	/*spl_handlers_dual_it.call_method = spl_dual_it_call_method;*/
	spl_handlers_dual_it.clone_obj = NULL;
	
	spl_ce_RecursiveIterator->interface_gets_implemented = spl_recursive_it_gets_implemented;

	spl_ce_RecursiveIteratorIterator->get_iterator = spl_recursive_it_get_iterator;
	spl_ce_RecursiveIteratorIterator->iterator_funcs.funcs = &spl_recursive_it_iterator_funcs;

	REGISTER_LONG_CONSTANT("RIT_LEAVES_ONLY",  (long)RIT_LEAVES_ONLY,  CONST_CS | CONST_PERSISTENT); 
	REGISTER_LONG_CONSTANT("RIT_SELF_FIRST",   (long)RIT_SELF_FIRST,   CONST_CS | CONST_PERSISTENT); 
	REGISTER_LONG_CONSTANT("RIT_CHILD_FIRST",  (long)RIT_CHILD_FIRST,  CONST_CS | CONST_PERSISTENT); 

	REGISTER_SPL_STD_CLASS_EX(FilterIterator, spl_dual_it_new, spl_funcs_FilterIterator);
	REGISTER_SPL_ITERATOR(FilterIterator);
	spl_ce_FilterIterator->ce_flags |= ZEND_ACC_ABSTRACT_CLASS;

	REGISTER_SPL_SUB_CLASS_EX(ParentIterator, FilterIterator, spl_dual_it_new, spl_funcs_ParentIterator);
	REGISTER_SPL_IMPLEMENTS(ParentIterator, RecursiveIterator);

	REGISTER_SPL_INTERFACE(SeekableIterator);
	REGISTER_SPL_ITERATOR(SeekableIterator);

	REGISTER_SPL_STD_CLASS_EX(LimitIterator, spl_dual_it_new, spl_funcs_LimitIterator);
	REGISTER_SPL_ITERATOR(LimitIterator);

	REGISTER_SPL_STD_CLASS_EX(CachingIterator, spl_dual_it_new, spl_funcs_CachingIterator);
	REGISTER_SPL_ITERATOR(CachingIterator);

	REGISTER_LONG_CONSTANT("CIT_CALL_TOSTRING",    (long)CIT_CALL_TOSTRING,    CONST_CS | CONST_PERSISTENT); 
	REGISTER_LONG_CONSTANT("CIT_CATCH_GET_CHILD",  (long)CIT_CATCH_GET_CHILD,  CONST_CS | CONST_PERSISTENT); 

	REGISTER_SPL_SUB_CLASS_EX(CachingRecursiveIterator, CachingIterator, spl_dual_it_new, spl_funcs_CachingRecursiveIterator);
	REGISTER_SPL_IMPLEMENTS(CachingRecursiveIterator, RecursiveIterator);
  
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
