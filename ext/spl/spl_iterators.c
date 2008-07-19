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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_directory.h"
#include "spl_array.h"
#include "spl_exceptions.h"
#include "ext/standard/php_smart_str.h"

#ifdef accept
#undef accept
#endif

PHPAPI zend_class_entry *spl_ce_RecursiveIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveIteratorIterator;
PHPAPI zend_class_entry *spl_ce_FilterIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveFilterIterator;
PHPAPI zend_class_entry *spl_ce_ParentIterator;
PHPAPI zend_class_entry *spl_ce_SeekableIterator;
PHPAPI zend_class_entry *spl_ce_LimitIterator;
PHPAPI zend_class_entry *spl_ce_CachingIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveCachingIterator;
PHPAPI zend_class_entry *spl_ce_OuterIterator;
PHPAPI zend_class_entry *spl_ce_IteratorIterator;
PHPAPI zend_class_entry *spl_ce_NoRewindIterator;
PHPAPI zend_class_entry *spl_ce_InfiniteIterator;
PHPAPI zend_class_entry *spl_ce_EmptyIterator;
PHPAPI zend_class_entry *spl_ce_AppendIterator;
PHPAPI zend_class_entry *spl_ce_RegexIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveRegexIterator;
PHPAPI zend_class_entry *spl_ce_Countable;
PHPAPI zend_class_entry *spl_ce_RecursiveTreeIterator;

const zend_function_entry spl_funcs_RecursiveIterator[] = {
	SPL_ABSTRACT_ME(RecursiveIterator, hasChildren,  NULL)
	SPL_ABSTRACT_ME(RecursiveIterator, getChildren,  NULL)
	{NULL, NULL, NULL}
};

typedef enum {
	RIT_LEAVES_ONLY = 0,
	RIT_SELF_FIRST  = 1,
	RIT_CHILD_FIRST = 2
} RecursiveIteratorMode;

#define RIT_CATCH_GET_CHILD CIT_CATCH_GET_CHILD

typedef enum {
	RTIT_BYPASS_CURRENT = 4,
	RTIT_BYPASS_KEY	    = 8
} RecursiveTreeIteratorFlags;

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
	int                      flags;
	int                      max_depth;
	zend_bool                in_iteration;
	zend_function            *beginIteration;
	zend_function            *endIteration;
	zend_function            *callHasChildren;
	zend_function            *callGetChildren;
	zend_function            *beginChildren;
	zend_function            *endChildren;
	zend_function            *nextElement;
	zend_class_entry         *ce;
	smart_str                prefix[6];
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
	object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator));
	object->level = 0;

	zval_ptr_dtor(&iter->zobject);	
	efree(iter);
}
	
static int spl_recursive_it_valid_ex(spl_recursive_it_object *object, zval *zthis TSRMLS_DC)
{
	zend_object_iterator      *sub_iter;
	int                       level = object->level;
	
	while (level >=0) {
		sub_iter = object->iterators[level].iterator;
		if (sub_iter->funcs->valid(sub_iter TSRMLS_CC) == SUCCESS) {
			return SUCCESS;
		}
		level--;
	}
	if (object->endIteration && object->in_iteration) {
		zend_call_method_with_0_params(&zthis, object->ce, &object->endIteration, "endIteration", NULL);
	}
	object->in_iteration = 0;
	return FAILURE;
}

static int spl_recursive_it_valid(zend_object_iterator *iter TSRMLS_DC)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)iter->data;
	
	return spl_recursive_it_valid_ex(object, ((spl_recursive_it_iterator*)iter)->zobject TSRMLS_CC);
}

static void spl_recursive_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)iter->data;
	zend_object_iterator      *sub_iter = object->iterators[object->level].iterator;
	
	sub_iter->funcs->get_current_data(sub_iter, data TSRMLS_CC);
}

static int spl_recursive_it_get_current_key(zend_object_iterator *iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
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

static void spl_recursive_it_move_forward_ex(spl_recursive_it_object *object, zval *zthis TSRMLS_DC)
{
	zend_object_iterator      *iterator;
	zval                      *zobject;
	zend_class_entry          *ce;
	zval                      *retval, *child;
	zend_object_iterator      *sub_iter;
	int                       has_children;

	while (!EG(exception)) {
next_step:
		iterator = object->iterators[object->level].iterator;
		switch (object->iterators[object->level].state) {
			case RS_NEXT:
				iterator->funcs->move_forward(iterator TSRMLS_CC);
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception(TSRMLS_C);
					}
				}
			case RS_START:
				if (iterator->funcs->valid(iterator TSRMLS_CC) == FAILURE) {
					break;
				}
				object->iterators[object->level].state = RS_TEST;					
				/* break; */
			case RS_TEST:
				ce = object->iterators[object->level].ce;
				zobject = object->iterators[object->level].zobject;
				if (object->callHasChildren) {
					zend_call_method_with_0_params(&zthis, object->ce, &object->callHasChildren, "callHasChildren", &retval);
				} else {
					zend_call_method_with_0_params(&zobject, ce, NULL, "haschildren", &retval);
				}
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						object->iterators[object->level].state = RS_NEXT;
						return;
					} else {
						zend_clear_exception(TSRMLS_C);
					}
				}
				if (retval) {
					has_children = zend_is_true(retval);
					zval_ptr_dtor(&retval);
					if (has_children) {
						if (object->max_depth == -1 || object->max_depth > object->level) {
							switch (object->mode) {
							case RIT_LEAVES_ONLY:
							case RIT_CHILD_FIRST:
								object->iterators[object->level].state = RS_CHILD;
								goto next_step;
							case RIT_SELF_FIRST:
								object->iterators[object->level].state = RS_SELF;
								goto next_step;
							}
						} else {
							/* do not recurse into */
							if (object->mode == RIT_LEAVES_ONLY) {
								/* this is not a leave, so skip it */
								object->iterators[object->level].state = RS_NEXT;
								goto next_step;
							}
						}
					}
				}
				if (object->nextElement) {
					zend_call_method_with_0_params(&zthis, object->ce, &object->nextElement, "nextelement", NULL);
				}
				object->iterators[object->level].state = RS_NEXT;
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception(TSRMLS_C);
					}
				}
				return /* self */;
			case RS_SELF:
				if (object->nextElement && (object->mode == RIT_SELF_FIRST || object->mode == RIT_CHILD_FIRST)) {
					zend_call_method_with_0_params(&zthis, object->ce, &object->nextElement, "nextelement", NULL);
				}
				if (object->mode == RIT_SELF_FIRST) {
					object->iterators[object->level].state = RS_CHILD;
				} else {
					object->iterators[object->level].state = RS_NEXT;
				}
				return /* self */;
			case RS_CHILD:
				ce = object->iterators[object->level].ce;
				zobject = object->iterators[object->level].zobject;
				if (object->callGetChildren) {
					zend_call_method_with_0_params(&zthis, object->ce, &object->callGetChildren, "callGetChildren", &child);
				} else {
					zend_call_method_with_0_params(&zobject, ce, NULL, "getchildren", &child);
				}

				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception(TSRMLS_C);
						if (child) {
							zval_ptr_dtor(&child);
						}
						object->iterators[object->level].state = RS_NEXT;
						goto next_step;
					}
				}

				ce = child && Z_TYPE_P(child) == IS_OBJECT ? Z_OBJCE_P(child) : NULL;
				if (!ce || !instanceof_function(ce, spl_ce_RecursiveIterator TSRMLS_CC)) {
					if (child) {
						zval_ptr_dtor(&child);
					}
					zend_throw_exception(spl_ce_UnexpectedValueException, "Objects returned by RecursiveIterator::getChildren() must implement RecursiveIterator", 0 TSRMLS_CC);
					return;
				}
				if (object->mode == RIT_CHILD_FIRST) {
					object->iterators[object->level].state = RS_SELF;
				} else {
					object->iterators[object->level].state = RS_NEXT;
				}
				object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator) * (++object->level+1));
				sub_iter = ce->get_iterator(ce, child, 0 TSRMLS_CC);
				object->iterators[object->level].iterator = sub_iter;
				object->iterators[object->level].zobject = child;
				object->iterators[object->level].ce = ce;
				object->iterators[object->level].state = RS_START;
				if (sub_iter->funcs->rewind) {
					sub_iter->funcs->rewind(sub_iter TSRMLS_CC);
				}
				if (object->beginChildren) {
					zend_call_method_with_0_params(&zthis, object->ce, &object->beginChildren, "beginchildren", NULL);
					if (EG(exception)) {
						if (!(object->flags & RIT_CATCH_GET_CHILD)) {
							return;
						} else {
							zend_clear_exception(TSRMLS_C);
						}
					}
				}
				goto next_step;
		}
		/* no more elements */
		if (object->level > 0) {
			if (object->endChildren) {
				zend_call_method_with_0_params(&zthis, object->ce, &object->endChildren, "endchildren", NULL);
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception(TSRMLS_C);
					}
				}
			}
			iterator->funcs->dtor(iterator TSRMLS_CC);
			zval_ptr_dtor(&object->iterators[object->level].zobject);
			object->level--;
		} else {
			return; /* done completeley */
		}
	}
}

static void spl_recursive_it_rewind_ex(spl_recursive_it_object *object, zval *zthis TSRMLS_DC)
{
	zend_object_iterator      *sub_iter;

	while (object->level) {
		sub_iter = object->iterators[object->level].iterator;
		sub_iter->funcs->dtor(sub_iter TSRMLS_CC);
		zval_ptr_dtor(&object->iterators[object->level--].zobject);
		if (!EG(exception) && (!object->endChildren || object->endChildren->common.scope != spl_ce_RecursiveIteratorIterator)) {
			zend_call_method_with_0_params(&zthis, object->ce, &object->endChildren, "endchildren", NULL);
		}
	}
	object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator));
	object->iterators[0].state = RS_START;
	sub_iter = object->iterators[0].iterator;
	if (sub_iter->funcs->rewind) {
		sub_iter->funcs->rewind(sub_iter TSRMLS_CC);
	}
	if (!EG(exception) && object->beginIteration && !object->in_iteration) {
		zend_call_method_with_0_params(&zthis, object->ce, &object->beginIteration, "beginIteration", NULL);
	}
	object->in_iteration = 1;
	spl_recursive_it_move_forward_ex(object, zthis TSRMLS_CC);
}

static void spl_recursive_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_recursive_it_move_forward_ex((spl_recursive_it_object*)iter->data, ((spl_recursive_it_iterator*)iter)->zobject TSRMLS_CC);
}

static void spl_recursive_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	spl_recursive_it_rewind_ex((spl_recursive_it_object*)iter->data, ((spl_recursive_it_iterator*)iter)->zobject TSRMLS_CC);
}

static zend_object_iterator *spl_recursive_it_get_iterator(zend_class_entry *ce, zval *zobject, int by_ref TSRMLS_DC)
{
	spl_recursive_it_iterator *iterator;
	spl_recursive_it_object   *object;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	iterator = emalloc(sizeof(spl_recursive_it_iterator));
	object   = (spl_recursive_it_object*)zend_object_store_get_object(zobject TSRMLS_CC);

	Z_ADDREF_P(zobject);
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = ce->iterator_funcs.funcs;
	iterator->zobject = zobject;
	return (zend_object_iterator*)iterator;
}

zend_object_iterator_funcs spl_recursive_it_iterator_funcs = {
	spl_recursive_it_dtor,
	spl_recursive_it_valid,
	spl_recursive_it_get_current_data,
	spl_recursive_it_get_current_key,
	spl_recursive_it_move_forward,
	spl_recursive_it_rewind
};

static void spl_recursive_it_it_construct(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_base, zend_class_entry *ce_inner, recursive_it_it_type rit_type)
{
	zval                      *object = getThis();
	spl_recursive_it_object   *intern;
	zval                      *iterator;
	zend_class_entry          *ce_iterator;
	long                       mode, flags;
	int                        inc_refcount = 1;

	php_set_error_handling(EH_THROW, spl_ce_InvalidArgumentException TSRMLS_CC);

	switch(rit_type) {
		case RIT_RecursiveTreeIterator: {

			zval *caching_it, *caching_it_flags, *user_caching_it_flags = NULL;
			mode = RIT_SELF_FIRST;
			flags = RTIT_BYPASS_KEY;

			if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "o|lzl", &iterator, &flags, &user_caching_it_flags, &mode) == SUCCESS) {
				if (instanceof_function(Z_OBJCE_P(iterator), zend_ce_aggregate TSRMLS_CC)) {
					zval *aggregate = iterator;
					zend_call_method_with_0_params(&aggregate, Z_OBJCE_P(aggregate), &Z_OBJCE_P(aggregate)->iterator_funcs.zf_new_iterator, "getiterator", &iterator);
					inc_refcount = 0;
				}

				MAKE_STD_ZVAL(caching_it_flags);
				if (user_caching_it_flags) {
					ZVAL_ZVAL(caching_it_flags, user_caching_it_flags, 1, 0);
				} else {
					ZVAL_LONG(caching_it_flags, CIT_CATCH_GET_CHILD);
				}
				spl_instantiate_arg_ex2(spl_ce_RecursiveCachingIterator, &caching_it, 1, iterator, caching_it_flags TSRMLS_CC);
				zval_ptr_dtor(&caching_it_flags);
				if (inc_refcount == 0 && iterator) {
					zval_ptr_dtor(&iterator);
				}
				iterator = caching_it;
				inc_refcount = 0;
			} else {
				iterator = NULL;
			}
			break;
		}
		case RIT_RecursiveIteratorIterator:
		default: {
			mode = RIT_LEAVES_ONLY;
			flags = 0;

			if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "o|ll", &iterator, &mode, &flags) == SUCCESS) {
				if (instanceof_function(Z_OBJCE_P(iterator), zend_ce_aggregate TSRMLS_CC)) {
					zval *aggregate = iterator;
					zend_call_method_with_0_params(&aggregate, Z_OBJCE_P(aggregate), &Z_OBJCE_P(aggregate)->iterator_funcs.zf_new_iterator, "getiterator", &iterator);
					inc_refcount = 0;
				}
			} else {
				iterator = NULL;
			}
			break;
		}
	}
	if (!iterator || !instanceof_function(Z_OBJCE_P(iterator), spl_ce_RecursiveIterator TSRMLS_CC)) {
		if (iterator && !inc_refcount) {
			zval_ptr_dtor(&iterator);
		}
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		zend_throw_exception(spl_ce_InvalidArgumentException, "An instance of RecursiveIterator or IteratorAggregate creating it is required", 0 TSRMLS_CC);
		return;
	}

	intern = (spl_recursive_it_object*)zend_object_store_get_object(object TSRMLS_CC);
	intern->iterators = emalloc(sizeof(spl_sub_iterator));
	intern->level = 0;
	intern->mode = mode;
	intern->flags = flags;
	intern->max_depth = -1;
	intern->in_iteration = 0;
	intern->ce = Z_OBJCE_P(object);

	zend_hash_find(&intern->ce->function_table, "beginiteration", sizeof("beginiteration"), (void **) &intern->beginIteration);
	if (intern->beginIteration->common.scope == ce_base) {
		intern->beginIteration = NULL;
	}
	zend_hash_find(&intern->ce->function_table, "enditeration", sizeof("enditeration"), (void **) &intern->endIteration);
	if (intern->endIteration->common.scope == ce_base) {
		intern->endIteration = NULL;
	}
	zend_hash_find(&intern->ce->function_table, "callhaschildren", sizeof("callHasChildren"), (void **) &intern->callHasChildren);
	if (intern->callHasChildren->common.scope == ce_base) {
		intern->callHasChildren = NULL;
	}
	zend_hash_find(&intern->ce->function_table, "callgetchildren", sizeof("callGetChildren"), (void **) &intern->callGetChildren);
	if (intern->callGetChildren->common.scope == ce_base) {
		intern->callGetChildren = NULL;
	}
	zend_hash_find(&intern->ce->function_table, "beginchildren", sizeof("beginchildren"), (void **) &intern->beginChildren);
	if (intern->beginChildren->common.scope == ce_base) {
		intern->beginChildren = NULL;
	}
	zend_hash_find(&intern->ce->function_table, "endchildren", sizeof("endchildren"), (void **) &intern->endChildren);
	if (intern->endChildren->common.scope == ce_base) {
		intern->endChildren = NULL;
	}
	zend_hash_find(&intern->ce->function_table, "nextelement", sizeof("nextElement"), (void **) &intern->nextElement);
	if (intern->nextElement->common.scope == ce_base) {
		intern->nextElement = NULL;
	}
	ce_iterator = Z_OBJCE_P(iterator); /* respect inheritance, don't use spl_ce_RecursiveIterator */
	intern->iterators[0].iterator = ce_iterator->get_iterator(ce_iterator, iterator, 0 TSRMLS_CC);
	if (inc_refcount) {
		Z_ADDREF_P(iterator);
	}
	intern->iterators[0].zobject = iterator;
	intern->iterators[0].ce = ce_iterator;
	intern->iterators[0].state = RS_START;

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}

/* {{{ proto void RecursiveIteratorIterator::__construct(RecursiveIterator|IteratorAggregate it [, int mode = RIT_LEAVES_ONLY [, int flags = 0]]) throws InvalidArgumentException U
   Creates a RecursiveIteratorIterator from a RecursiveIterator. */
SPL_METHOD(RecursiveIteratorIterator, __construct)
{
	spl_recursive_it_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveIteratorIterator, zend_ce_iterator, RIT_RecursiveIteratorIterator TSRMLS_CC);
} /* }}} */

/* {{{ proto void RecursiveIteratorIterator::rewind() U
   Rewind the iterator to the first element of the top level inner iterator. */
SPL_METHOD(RecursiveIteratorIterator, rewind)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_recursive_it_rewind_ex(object, getThis() TSRMLS_CC);
} /* }}} */

/* {{{ proto bool RecursiveIteratorIterator::valid() U
   Check whether the current position is valid */
SPL_METHOD(RecursiveIteratorIterator, valid)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_recursive_it_valid_ex(object, getThis() TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto mixed RecursiveIteratorIterator::key() U
   Access the current key */
SPL_METHOD(RecursiveIteratorIterator, key)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_object_iterator      *iterator = object->iterators[object->level].iterator;

	if (iterator->funcs->get_current_key) {
		zstr str_key;
		uint str_key_len;
		ulong int_key;
		
		switch (iterator->funcs->get_current_key(iterator, &str_key, &str_key_len, &int_key TSRMLS_CC)) {
			case HASH_KEY_IS_LONG:
				RETURN_LONG(int_key);
				break;
			case HASH_KEY_IS_STRING:
				RETURN_STRINGL(str_key.s, str_key_len-1, 0);
				break;
			case HASH_KEY_IS_UNICODE:
				RETURN_UNICODEL(str_key.u, str_key_len-1, 0);
				break;
			default:
				RETURN_NULL();
		}
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto mixed RecursiveIteratorIterator::current() U
   Access the current element value */
SPL_METHOD(RecursiveIteratorIterator, current)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_object_iterator      *iterator = object->iterators[object->level].iterator;
	zval                      **data;

	iterator->funcs->get_current_data(iterator, &data TSRMLS_CC);
	RETURN_ZVAL(*data, 1, 0);
} /* }}} */

/* {{{ proto void RecursiveIteratorIterator::next() U
   Move forward to the next element */
SPL_METHOD(RecursiveIteratorIterator, next)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_recursive_it_move_forward_ex(object, getThis() TSRMLS_CC);
} /* }}} */

/* {{{ proto int RecursiveIteratorIterator::getDepth() U
   Get the current depth of the recursive iteration */
SPL_METHOD(RecursiveIteratorIterator, getDepth)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	RETURN_LONG(object->level);
} /* }}} */

/* {{{ proto RecursiveIterator RecursiveIteratorIterator::getSubIterator([int level]) U
   The current active sub iterator or the iterator at specified level */
SPL_METHOD(RecursiveIteratorIterator, getSubIterator)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long  level = object->level;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &level) == FAILURE) {
		return;
	}
	if (level < 0 || level > object->level) {
		RETURN_NULL();
	}
	RETURN_ZVAL(object->iterators[level].zobject, 1, 0);
} /* }}} */

/* {{{ proto RecursiveIterator RecursiveIteratorIterator::getInnerIterator() U
   The current active sub iterator */
SPL_METHOD(RecursiveIteratorIterator, getInnerIterator)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long  level = object->level;
	
	RETURN_ZVAL(object->iterators[level].zobject, 1, 0);
} /* }}} */

/* {{{ proto RecursiveIterator RecursiveIteratorIterator::beginIteration() U
   Called when iteration begins (after first rewind() call) */
SPL_METHOD(RecursiveIteratorIterator, beginIteration)
{
	/* nothing to do */
} /* }}} */

/* {{{ proto RecursiveIterator RecursiveIteratorIterator::endIteration() U
   Called when iteration ends (when valid() first returns false */
SPL_METHOD(RecursiveIteratorIterator, endIteration)
{
	/* nothing to do */
} /* }}} */

/* {{{ proto bool RecursiveIteratorIterator::callHasChildren() U
   Called for each element to test whether it has children */
SPL_METHOD(RecursiveIteratorIterator, callHasChildren)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = object->iterators[object->level].ce;
	zval *retval, *zobject;

	zobject = object->iterators[object->level].zobject;
	if (!zobject) {
		RETURN_FALSE;
	} else {
		zend_call_method_with_0_params(&zobject, ce, NULL, "haschildren", &retval);
		if (retval) {
			RETURN_ZVAL(retval, 0, 1);
		} else {
			RETURN_FALSE;
		}
	}
} /* }}} */

/* {{{ proto RecursiveIterator RecursiveIteratorIterator::callGetChildren() U
   Return children of current element */
SPL_METHOD(RecursiveIteratorIterator, callGetChildren)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = object->iterators[object->level].ce;
	zval *retval, *zobject;

	zobject = object->iterators[object->level].zobject;
	if (!zobject) {
		return;
	} else {
		zend_call_method_with_0_params(&zobject, ce, NULL, "getchildren", &retval);
		if (retval) {
			RETURN_ZVAL(retval, 0, 1);
		}
	}
} /* }}} */

/* {{{ proto void RecursiveIteratorIterator::beginChildren() U
   Called when recursing one level down */
SPL_METHOD(RecursiveIteratorIterator, beginChildren)
{
	/* nothing to do */
} /* }}} */

/* {{{ proto void RecursiveIteratorIterator::endChildren() U
   Called when end recursing one level */
SPL_METHOD(RecursiveIteratorIterator, endChildren)
{
	/* nothing to do */
} /* }}} */

/* {{{ proto void RecursiveIteratorIterator::nextElement() U
   Called when the next element is available */
SPL_METHOD(RecursiveIteratorIterator, nextElement)
{
	/* nothing to do */
} /* }}} */

/* {{{ proto void RecursiveIteratorIterator::setMaxDepth([$max_depth = -1]) U
   Set the maximum allowed depth (or any depth if pmax_depth = -1] */
SPL_METHOD(RecursiveIteratorIterator, setMaxDepth)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long  max_depth = -1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &max_depth) == FAILURE) {
		return;
	}
	if (max_depth < -1) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Parameter max_depth must be >= -1", 0 TSRMLS_CC);
		return;
	}
	object->max_depth = max_depth;
} /* }}} */

/* {{{ proto int|false RecursiveIteratorIterator::getMaxDepth() U
   Return the maximum accepted depth or false if any depth is allowed */
SPL_METHOD(RecursiveIteratorIterator, getMaxDepth)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (object->max_depth == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(object->max_depth);
	}
} /* }}} */

static union _zend_function *spl_recursive_it_get_method(zval **object_ptr, zstr method, int method_len TSRMLS_DC)
{
	union _zend_function    *function_handler;
	spl_recursive_it_object *object = (spl_recursive_it_object*)zend_object_store_get_object(*object_ptr TSRMLS_CC);
	long                     level = object->level;
	zval                    *zobj;

	if (!object->iterators) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "The %v instance wasn't initialized properly", Z_OBJCE_PP(object_ptr)->name);
	}
	zobj = object->iterators[level].zobject;

	function_handler = std_object_handlers.get_method(object_ptr, method, method_len TSRMLS_CC);
	if (!function_handler) {
		if (zend_u_hash_find(&Z_OBJCE_P(zobj)->function_table, UG(unicode)?IS_UNICODE:IS_STRING, method, method_len+1, (void **) &function_handler) == FAILURE) {
			if (Z_OBJ_HT_P(zobj)->get_method) {
				*object_ptr = zobj;
				function_handler = Z_OBJ_HT_P(*object_ptr)->get_method(object_ptr, method, method_len TSRMLS_CC);
			}
		}
	}
	return function_handler;
}

/* {{{ spl_RecursiveIteratorIterator_dtor */
static void spl_RecursiveIteratorIterator_free_storage(void *_object TSRMLS_DC)
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
		object->iterators = NULL;
	}

	zend_object_std_dtor(&object->std TSRMLS_CC);
	smart_str_free(&object->prefix[0]);
	smart_str_free(&object->prefix[1]);
	smart_str_free(&object->prefix[2]);
	smart_str_free(&object->prefix[3]);
	smart_str_free(&object->prefix[4]);
	smart_str_free(&object->prefix[5]);

	efree(object);
}
/* }}} */

/* {{{ spl_RecursiveIteratorIterator_new_ex */
static zend_object_value spl_RecursiveIteratorIterator_new_ex(zend_class_entry *class_type, int init_prefix TSRMLS_DC)
{
	zend_object_value retval;
	spl_recursive_it_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_recursive_it_object));
	memset(intern, 0, sizeof(spl_recursive_it_object));

	if (init_prefix) {
		smart_str_appendl(&intern->prefix[0], "",    0);
		smart_str_appendl(&intern->prefix[1], "| ",  2);
		smart_str_appendl(&intern->prefix[2], "  ",  2);
		smart_str_appendl(&intern->prefix[3], "|-",  2);
		smart_str_appendl(&intern->prefix[4], "\\-", 2);
		smart_str_appendl(&intern->prefix[5], "",    0);
	}

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_RecursiveIteratorIterator_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_handlers_rec_it_it;
	return retval;
}
/* }}} */

/* {{{ spl_RecursiveIteratorIterator_new */
static zend_object_value spl_RecursiveIteratorIterator_new(zend_class_entry *class_type TSRMLS_DC)
{
	return spl_RecursiveIteratorIterator_new_ex(class_type, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ spl_RecursiveTreeIterator_new */
static zend_object_value spl_RecursiveTreeIterator_new(zend_class_entry *class_type TSRMLS_DC)
{
	return spl_RecursiveIteratorIterator_new_ex(class_type, 1 TSRMLS_CC);
}
/* }}} */

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_recursive_it___construct, 0, 0, 1) 
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_recursive_it_getSubIterator, 0, 0, 0)
	ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_recursive_it_setMaxDepth, 0, 0, 0)
	ZEND_ARG_INFO(0, max_depth)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_RecursiveIteratorIterator[] = {
	SPL_ME(RecursiveIteratorIterator, __construct,       arginfo_recursive_it___construct,    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, rewind,            NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, valid,             NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, key,               NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, current,           NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, next,              NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, getDepth,          NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, getSubIterator,    arginfo_recursive_it_getSubIterator, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, getInnerIterator,  NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, beginIteration,    NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, endIteration,      NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, callHasChildren,   NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, callGetChildren,   NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, beginChildren,     NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, endChildren,       NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, nextElement,       NULL,                                ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, setMaxDepth,       arginfo_recursive_it_setMaxDepth,    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, getMaxDepth,       NULL,                                ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static void spl_recursive_tree_iterator_get_prefix(spl_recursive_it_object *object, zval *return_value, zend_uchar return_type TSRMLS_DC)
{
	smart_str  str = {0};
	zval      *has_next;
	int        level;

	smart_str_appendl(&str, object->prefix[0].c, object->prefix[0].len);
	
	for (level = 0; level < object->level; ++level) {
		zend_call_method_with_0_params(&object->iterators[level].zobject, object->iterators[level].ce, NULL, "hasnext", &has_next);
		if (has_next) {
			if (Z_LVAL_P(has_next)) {
				smart_str_appendl(&str, object->prefix[1].c, object->prefix[1].len);
			} else {
				smart_str_appendl(&str, object->prefix[2].c, object->prefix[2].len);
			}
			zval_ptr_dtor(&has_next);
		}
	}
	zend_call_method_with_0_params(&object->iterators[level].zobject, object->iterators[level].ce, NULL, "hasnext", &has_next);
	if (has_next) {
		if (Z_LVAL_P(has_next)) {
			smart_str_appendl(&str, object->prefix[3].c, object->prefix[3].len);
		} else {
			smart_str_appendl(&str, object->prefix[4].c, object->prefix[4].len);
		}
		zval_ptr_dtor(&has_next);
	}

	smart_str_appendl(&str, object->prefix[5].c, object->prefix[5].len);
	smart_str_0(&str);

	RETVAL_STRINGL(str.c, str.len, 0);
	if (return_type == IS_UNICODE) {
		convert_to_unicode(return_value);
	}
}

static void spl_recursive_tree_iterator_get_entry(spl_recursive_it_object * object, zval * return_value TSRMLS_DC)
{
	zend_object_iterator      *iterator = object->iterators[object->level].iterator;
	zval                      **data;

	iterator->funcs->get_current_data(iterator, &data TSRMLS_CC);

	php_set_error_handling(EH_THROW, spl_ce_UnexpectedValueException TSRMLS_CC);
	RETVAL_ZVAL(*data, 1, 0);
	if (Z_TYPE_PP(data) != IS_UNICODE && Z_TYPE_PP(data) != IS_STRING) {
		convert_to_text(return_value);
	}
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}

static void spl_recursive_tree_iterator_get_postfix(spl_recursive_it_object * object, zval * return_value, zend_uchar return_type TSRMLS_DC)
{
	if (return_type == IS_UNICODE) {
		ZVAL_EMPTY_UNICODE(return_value);
	} else {
		ZVAL_EMPTY_STRING(return_value);
	}
}

/* {{{ proto void RecursiveTreeIterator::__construct(RecursiveIterator|IteratorAggregate it [, int flags = RTIT_BYPASS_KEY [, int cit_flags = CIT_CATCH_GET_CHILD [, mode = RIT_SELF_FIRST ]]]) throws InvalidArgumentException U
   RecursiveIteratorIterator to generate ASCII graphic trees for the entries in a RecursiveIterator */
SPL_METHOD(RecursiveTreeIterator, __construct)
{
	spl_recursive_it_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveTreeIterator, zend_ce_iterator, RIT_RecursiveTreeIterator TSRMLS_CC);
} /* }}} */

/* {{{ proto void RecursiveTreeIterator::setPrefixPart() throws OutOfRangeException
   Sets prefix parts as used in getPrefix() */
SPL_METHOD(RecursiveTreeIterator, setPrefixPart)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long  part;
	char* prefix;
	int   prefix_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &part, &prefix, &prefix_len) == FAILURE) {
		return;
	}
	if (0 > part || part > 5) {
		zend_throw_exception_ex(spl_ce_OutOfRangeException, 0 TSRMLS_CC, "Use RecursiveTreeIterator::PREFIX_* constant");
		return;
	}
	
	smart_str_free(&object->prefix[part]);
	smart_str_appendl(&object->prefix[part], prefix, prefix_len);
} /* }}} */

/* {{{ proto string RecursiveTreeIterator::getPrefix() U
   Returns the string to place in front of current element */
SPL_METHOD(RecursiveTreeIterator, getPrefix)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_uchar                 return_type = (UG(unicode) ? IS_UNICODE : IS_STRING);

	spl_recursive_tree_iterator_get_prefix(object, return_value, return_type TSRMLS_CC);
} /* }}} */

/* {{{ proto string RecursiveTreeIterator::getEntry() U
   Returns the string presentation built for current element */
SPL_METHOD(RecursiveTreeIterator, getEntry)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_recursive_tree_iterator_get_entry(object, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto string RecursiveTreeIterator::getPostfix() U
   Returns the string to place after the current element */
SPL_METHOD(RecursiveTreeIterator, getPostfix)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_uchar                 return_type = (UG(unicode) ? IS_UNICODE : IS_STRING);

	spl_recursive_tree_iterator_get_postfix(object, return_value, return_type TSRMLS_CC);
} /* }}} */

/* {{{ proto mixed RecursiveTreeIterator::current() U
   Returns the current element prefixed and postfixed */
SPL_METHOD(RecursiveTreeIterator, current)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval                       prefix, entry, postfix;
	zend_uchar                 return_type;

	if (object->flags & RTIT_BYPASS_CURRENT) {
		zend_object_iterator      *iterator = object->iterators[object->level].iterator;
		zval                      **data;

		iterator->funcs->get_current_data(iterator, &data TSRMLS_CC);
		RETURN_ZVAL(*data, 1, 0);
	}

	spl_recursive_tree_iterator_get_entry(object, &entry TSRMLS_CC);
	return_type = Z_TYPE(entry);

	spl_recursive_tree_iterator_get_prefix(object, &prefix, return_type TSRMLS_CC);
	spl_recursive_tree_iterator_get_postfix(object, &postfix, return_type TSRMLS_CC);

	if (return_type == IS_STRING) {
		char    *str, *ptr;
		size_t   str_len;

		str_len = Z_STRLEN(prefix) + Z_STRLEN(entry) + Z_STRLEN(postfix);
		str = (char *) emalloc(str_len + 1U);
		ptr = str;
	
		memcpy(ptr, Z_STRVAL(prefix), Z_STRLEN(prefix));
		ptr += Z_STRLEN(prefix);
		memcpy(ptr, Z_STRVAL(entry), Z_STRLEN(entry));
		ptr += Z_STRLEN(entry);
		memcpy(ptr, Z_STRVAL(postfix), Z_STRLEN(postfix)+1U);

		RETVAL_STRINGL(str, str_len, 0);
	} else {
		UChar   *str, *ptr;
		size_t   str_len;

		str_len = Z_USTRLEN(prefix) + Z_USTRLEN(entry) + Z_USTRLEN(postfix);
		str = eumalloc(str_len + 1U);
		ptr = str;
	
		memcpy(ptr, Z_USTRVAL(prefix), UBYTES(Z_USTRLEN(prefix)));
		ptr += Z_USTRLEN(prefix);
		memcpy(ptr, Z_USTRVAL(entry), UBYTES(Z_USTRLEN(entry)));
		ptr += Z_USTRLEN(entry);
		memcpy(ptr, Z_USTRVAL(postfix), UBYTES(Z_USTRLEN(postfix)+1U));
		
		RETVAL_UNICODEL(str, str_len, 0);
	}

	zval_dtor(&prefix);
	zval_dtor(&entry);
	zval_dtor(&postfix);

} /* }}} */

/* {{{ proto mixed RecursiveTreeIterator::key() U
   Returns the current key prefixed and postfixed */
SPL_METHOD(RecursiveTreeIterator, key)
{
	spl_recursive_it_object   *object = (spl_recursive_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_object_iterator      *iterator = object->iterators[object->level].iterator;
	zval                       prefix, key, postfix;
	zend_uchar                 return_type;

	if (iterator->funcs->get_current_key) {
		zstr str_key;
		uint str_key_len;
		ulong int_key;

		switch (iterator->funcs->get_current_key(iterator, &str_key, &str_key_len, &int_key TSRMLS_CC)) {
			case HASH_KEY_IS_LONG:
				ZVAL_LONG(&key, int_key);
				break;
			case HASH_KEY_IS_STRING:
				ZVAL_STRINGL(&key, str_key.s, str_key_len-1, 0);
				break;
			case HASH_KEY_IS_UNICODE:
				ZVAL_UNICODEL(&key, str_key.u, str_key_len-1, 0);
				break;
			default:
				ZVAL_NULL(&key);
		}
	} else {
		ZVAL_NULL(&key);
	}

	if (object->flags & RTIT_BYPASS_KEY) {
		zval *key_ptr = &key;
		RETVAL_ZVAL(key_ptr, 1, 0);
		zval_dtor(&key);
		return;
	}

	if (Z_TYPE(key) != IS_UNICODE && Z_TYPE(key) != IS_STRING) {
		convert_to_text(&key);
	}
	return_type = Z_TYPE(key);

	spl_recursive_tree_iterator_get_prefix(object, &prefix, return_type TSRMLS_CC);
	spl_recursive_tree_iterator_get_postfix(object, &postfix, return_type TSRMLS_CC);

	if (return_type == IS_STRING) {
		char   *str, *ptr;
		size_t  str_len;

		str_len = Z_STRLEN(prefix) + Z_STRLEN(key) + Z_STRLEN(postfix);
		str = (char *) emalloc(str_len + 1U);
		ptr = str;
	
		memcpy(ptr, Z_STRVAL(prefix), Z_STRLEN(prefix));
		ptr += Z_STRLEN(prefix);
		memcpy(ptr, Z_STRVAL(key), Z_STRLEN(key));
		ptr += Z_STRLEN(key);
		memcpy(ptr, Z_STRVAL(postfix), Z_STRLEN(postfix)+1U);
	
		RETVAL_STRINGL(str, str_len, 0);
	} else {
		UChar  *str, *ptr;
		size_t  str_len;

		str_len = Z_USTRLEN(prefix) + Z_USTRLEN(key) + Z_USTRLEN(postfix);
		str = eumalloc(str_len + 1U);
		ptr = str;
	
		memcpy(ptr, Z_USTRVAL(prefix), UBYTES(Z_USTRLEN(prefix)));
		ptr += Z_USTRLEN(prefix);
		memcpy(ptr, Z_USTRVAL(key), UBYTES(Z_USTRLEN(key)));
		ptr += Z_STRLEN(key);
		memcpy(ptr, Z_USTRVAL(postfix), UBYTES(Z_USTRLEN(postfix)+1U));
	
		RETVAL_UNICODEL(str, str_len, 0);
	}

	zval_dtor(&prefix);
	zval_dtor(&key);
	zval_dtor(&postfix);

} /* }}} */

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_recursive_tree_it___construct, 0, 0, 1) 
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, caching_it_flags)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_recursive_tree_it_setPrefixPart, 0, 0, 2)
	ZEND_ARG_INFO(0, part)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_RecursiveTreeIterator[] = {
	SPL_ME(RecursiveTreeIterator,     __construct,       arginfo_recursive_tree_it___construct,   ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, rewind,            NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, valid,             NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveTreeIterator,     key,               NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveTreeIterator,     current,           NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, next,              NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, beginIteration,    NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, endIteration,      NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, callHasChildren,   NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, callGetChildren,   NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, beginChildren,     NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, endChildren,       NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveIteratorIterator, nextElement,       NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveTreeIterator,     getPrefix,         NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveTreeIterator,     setPrefixPart,     arginfo_recursive_tree_it_setPrefixPart, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveTreeIterator,     getEntry,          NULL,                                    ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveTreeIterator,     getPostfix,        NULL,                                    ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

#if MBO_0
static int spl_dual_it_gets_implemented(zend_class_entry *interface, zend_class_entry *class_type TSRMLS_DC)
{
	class_type->iterator_funcs.zf_valid = NULL;
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

static union _zend_function *spl_dual_it_get_method(zval **object_ptr, zstr method, int method_len TSRMLS_DC)
{
	union _zend_function *function_handler;
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(*object_ptr TSRMLS_CC);

	function_handler = std_object_handlers.get_method(object_ptr, method, method_len TSRMLS_CC);
	if (!function_handler) {
		if (zend_u_hash_find(&intern->inner.ce->function_table, UG(unicode)?IS_UNICODE:IS_STRING, method, method_len+1, (void **) &function_handler) == FAILURE) {
			if (Z_OBJ_HT_P(intern->inner.zobject)->get_method) {
				*object_ptr = intern->inner.zobject;
				function_handler = Z_OBJ_HT_P(*object_ptr)->get_method(object_ptr, method, method_len TSRMLS_CC);
			}
		}
	}
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

	ZVAL_TEXT(&func, method, 0);
	if (!zend_is_callable(&func, 0, &method)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Method %v::%R() does not exist", intern->inner.ce->name, Z_TYPE(method), Z_UNIVAL(method));
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
	arg_count = current; /* restore */

	if (call_user_function_ex(EG(function_table), NULL, &func, &retval_ptr, arg_count, func_params, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		RETURN_ZVAL(retval_ptr, 0, 1);
		
		success = SUCCESS;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to call %v::%s()", intern->inner.ce->name, method);
		success = FAILURE;
	}

	efree(func_params); 
	return success;
}
#endif

#define SPL_CHECK_CTOR(intern, classname) \
	if (intern->dit_type == DIT_Unknown) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Classes derived from %v must call %v::__construct()", \
				(spl_ce_##classname)->name, (spl_ce_##classname)->name); \
		return; \
	}

#define APPENDIT_CHECK_CTOR(intern) SPL_CHECK_CTOR(intern, AppendIterator)

static inline int spl_dual_it_fetch(spl_dual_it_object *intern, int check_more TSRMLS_DC);

static inline int spl_cit_check_flags(int flags)
{
	int cnt = 0;

	cnt += (flags & CIT_CALL_TOSTRING) ? 1 : 0;
	cnt += (flags & CIT_TOSTRING_USE_KEY) ? 1 : 0;
	cnt += (flags & CIT_TOSTRING_USE_CURRENT) ? 1 : 0;
	cnt += (flags & CIT_TOSTRING_USE_INNER) ? 1 : 0;
	
	return cnt <= 1 ? SUCCESS : FAILURE;
}

static spl_dual_it_object* spl_dual_it_construct(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_base, zend_class_entry *ce_inner, dual_it_type dit_type)
{
	zval                 *zobject, *retval;
	spl_dual_it_object   *intern;
	zend_class_entry     *ce = NULL;
	int                   inc_refcount = 1;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (intern->dit_type != DIT_Unknown) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v::getIterator() must be called exactly once per instance", ce_base->name);
		return NULL;
	}

	php_set_error_handling(EH_THROW, spl_ce_InvalidArgumentException TSRMLS_CC);

	intern->dit_type = dit_type;
	switch (dit_type) {
		case DIT_LimitIterator: {
			intern->u.limit.offset = 0; /* start at beginning */
			intern->u.limit.count = -1; /* get all */
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|ll", &zobject, ce_inner, &intern->u.limit.offset, &intern->u.limit.count) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			if (intern->u.limit.offset < 0) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				zend_throw_exception(spl_ce_OutOfRangeException, "Parameter offset must be > 0", 0 TSRMLS_CC);
				return NULL;
			}
			if (intern->u.limit.count < 0 && intern->u.limit.count != -1) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				zend_throw_exception(spl_ce_OutOfRangeException, "Parameter count must either be -1 or a value greater than or equal 0", 0 TSRMLS_CC);
				return NULL;
			}
			break;
		}
		case DIT_CachingIterator:
		case DIT_RecursiveCachingIterator: {
			long flags = CIT_CALL_TOSTRING;
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|l", &zobject, ce_inner, &flags) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			if (spl_cit_check_flags(flags) != SUCCESS) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				zend_throw_exception(spl_ce_InvalidArgumentException, "Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_CURRENT", 0 TSRMLS_CC);
				return NULL;
			}
			intern->u.caching.flags |= flags & CIT_PUBLIC;
			MAKE_STD_ZVAL(intern->u.caching.zcache);
			array_init(intern->u.caching.zcache);
			break;
		}
		case DIT_IteratorIterator: {
			zend_class_entry **pce_cast;
			char * class_name;
			int class_name_len;

			/* UTODO: class_name must be zstr */
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|s", &zobject, ce_inner, &class_name, &class_name_len) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			ce = Z_OBJCE_P(zobject);
			if (!instanceof_function(ce, zend_ce_iterator TSRMLS_CC)) {
				if (ZEND_NUM_ARGS() > 1) {
					if (zend_lookup_class(class_name, class_name_len, &pce_cast TSRMLS_CC) == FAILURE 
					|| !instanceof_function(ce, *pce_cast TSRMLS_CC)
					|| !(*pce_cast)->get_iterator
					) {
						zend_throw_exception(spl_ce_LogicException, "Class to downcast to not found or not base class or does not implement Traversable", 0 TSRMLS_CC);
						php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
						return NULL;
					}
					ce = *pce_cast;
				}
				if (instanceof_function(ce, zend_ce_aggregate TSRMLS_CC)) {
					zend_call_method_with_0_params(&zobject, ce, &ce->iterator_funcs.zf_new_iterator, "getiterator", &retval);
					if (EG(exception)) {
						if (retval) {
							zval_ptr_dtor(&retval);
						}
						php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
						return NULL;
					}
					if (!retval || Z_TYPE_P(retval) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(retval), zend_ce_traversable TSRMLS_CC)) {
						zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "%v::getIterator() must return an object that implememnts Traversable", ce->name);
						php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
						return NULL;
					}
					zobject = retval;
					ce = Z_OBJCE_P(zobject);
					inc_refcount = 0;
				}
			}
			break;
		}
		case DIT_AppendIterator:
			spl_instantiate(spl_ce_ArrayIterator, &intern->u.append.zarrayit, 1 TSRMLS_CC);
			zend_call_method_with_0_params(&intern->u.append.zarrayit, spl_ce_ArrayIterator, &spl_ce_ArrayIterator->constructor, "__construct", NULL);
			intern->u.append.iterator = spl_ce_ArrayIterator->get_iterator(spl_ce_ArrayIterator, intern->u.append.zarrayit, 0 TSRMLS_CC);
			php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
			return intern;
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
		case DIT_RegexIterator:
		case DIT_RecursiveRegexIterator: {
			char *regex;
			int regex_len;
			long mode = REGIT_MODE_MATCH;

			intern->u.regex.use_flags = ZEND_NUM_ARGS() >= 5;
			intern->u.regex.flags = 0;
			intern->u.regex.preg_flags = 0;
			/* UTODO: do we need tocare if regex unicode? */
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|lll", &zobject, ce_inner, &regex, &regex_len, &mode, &intern->u.regex.flags, &intern->u.regex.preg_flags) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			if (mode < 0 || mode >= REGIT_MODE_MAX) {
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Illegal mode %ld", mode);
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			intern->u.regex.mode = mode;
			intern->u.regex.regex = estrndup(regex, regex_len);
			intern->u.regex.pce = pcre_get_compiled_regex_cache(ZEND_STR_TYPE, regex, regex_len TSRMLS_CC);
			if (intern->u.regex.pce == NULL) {
				/* pcre_get_compiled_regex_cache has already sent error */
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			intern->u.regex.pce->refcount++;
			break;
		}
#endif
		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zobject, ce_inner) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				return NULL;
			}
			break;
	}

	php_set_error_handling(EH_THROW, zend_exception_get_default(TSRMLS_C) TSRMLS_CC);

	if (inc_refcount) {
		Z_ADDREF_P(zobject);
	}
	intern->inner.zobject = zobject;
	intern->inner.ce = dit_type == DIT_IteratorIterator ? ce : Z_OBJCE_P(zobject);
	intern->inner.object = zend_object_store_get_object(zobject TSRMLS_CC);
	intern->inner.iterator = intern->inner.ce->get_iterator(intern->inner.ce, zobject, 0 TSRMLS_CC);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	return intern;
}

/* {{{ proto void FilterIterator::__construct(Iterator it) U
   Create an Iterator from another iterator */
SPL_METHOD(FilterIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_FilterIterator, zend_ce_iterator, DIT_FilterIterator);
} /* }}} */

/* {{{ proto Iterator FilterIterator::getInnerIterator() U
       proto Iterator CachingIterator::getInnerIterator() U
       proto Iterator LimitIterator::getInnerIterator() U
       proto Iterator ParentIterator::getInnerIterator() U
   Get the inner iterator */
SPL_METHOD(dual_it, getInnerIterator)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->inner.zobject) {
		RETVAL_ZVAL(intern->inner.zobject, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

static inline void spl_dual_it_require(spl_dual_it_object *intern TSRMLS_DC)
{
	if (!intern->inner.iterator) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "The inner constructor wasn't initialized with an iterator instance");
	}
}

static inline void spl_dual_it_free(spl_dual_it_object *intern TSRMLS_DC)
{
	if (intern->inner.iterator && intern->inner.iterator->funcs->invalidate_current) {
		intern->inner.iterator->funcs->invalidate_current(intern->inner.iterator TSRMLS_CC);
	}
	if (intern->current.data) {
		zval_ptr_dtor(&intern->current.data);
		intern->current.data = NULL;
	}
	if (intern->current.str_key.v) {
		efree(intern->current.str_key.v);
		intern->current.str_key.v = NULL;
	}
	if (intern->dit_type == DIT_CachingIterator || intern->dit_type == DIT_RecursiveCachingIterator) {
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

static inline void spl_dual_it_rewind(spl_dual_it_object *intern TSRMLS_DC)
{
	spl_dual_it_free(intern TSRMLS_CC);
	intern->current.pos = 0;
	if (intern->inner.iterator->funcs->rewind) {
		intern->inner.iterator->funcs->rewind(intern->inner.iterator TSRMLS_CC);
	}
}

static inline int spl_dual_it_valid(spl_dual_it_object *intern TSRMLS_DC)
{
	/* FAILURE / SUCCESS */
	return intern->inner.iterator->funcs->valid(intern->inner.iterator TSRMLS_CC);
}

static inline int spl_dual_it_fetch(spl_dual_it_object *intern, int check_more TSRMLS_DC)
{
	zval **data;

	spl_dual_it_free(intern TSRMLS_CC);
	if (!check_more || spl_dual_it_valid(intern TSRMLS_CC) == SUCCESS) {
		intern->inner.iterator->funcs->get_current_data(intern->inner.iterator, &data TSRMLS_CC);
		if (data && *data) {
		  intern->current.data = *data;
		  Z_ADDREF_P(intern->current.data);
		}
		if (intern->inner.iterator->funcs->get_current_key) {
			intern->current.key_type = intern->inner.iterator->funcs->get_current_key(intern->inner.iterator, &intern->current.str_key, &intern->current.str_key_len, &intern->current.int_key TSRMLS_CC);
		} else {
			intern->current.key_type = HASH_KEY_IS_LONG;
			intern->current.int_key = intern->current.pos;
		}
		return EG(exception) ? FAILURE : SUCCESS;
	}
	return FAILURE;
}

static inline void spl_dual_it_next(spl_dual_it_object *intern, int do_free TSRMLS_DC)
{
	if (do_free) {
		spl_dual_it_free(intern TSRMLS_CC);
	} else {
		spl_dual_it_require(intern TSRMLS_CC);
	}
	intern->inner.iterator->funcs->move_forward(intern->inner.iterator TSRMLS_CC);
	intern->current.pos++;
}

/* {{{ proto void ParentIterator::rewind()
       proto void IteratorIterator::rewind()
   Rewind the iterator
   */
SPL_METHOD(dual_it, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_dual_it_fetch(intern, 1 TSRMLS_CC);
} /* }}} */

/* {{{ proto bool FilterIterator::valid() U
       proto bool ParentIterator::valid() U
       proto bool IteratorIterator::valid() U
       proto bool NoRewindIterator::valid() U
   Check whether the current element is valid */
SPL_METHOD(dual_it, valid)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->current.data);
} /* }}} */

/* {{{ proto mixed FilterIterator::key() U
       proto mixed CachingIterator::key() U
       proto mixed LimitIterator::key() U
       proto mixed ParentIterator::key() U
       proto mixed IteratorIterator::key() U
       proto mixed NoRewindIterator::key() U
       proto mixed AppendIterator::key() U
   Get the current key */
SPL_METHOD(dual_it, key)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->current.data) {
		if (intern->current.key_type == HASH_KEY_IS_STRING) {
			RETURN_STRINGL(intern->current.str_key.s, intern->current.str_key_len-1, 1);
		} else if (intern->current.key_type == HASH_KEY_IS_UNICODE) {
			RETURN_UNICODEL(intern->current.str_key.u, intern->current.str_key_len-1, 1);
		} else {
			RETURN_LONG(intern->current.int_key);
		}
	}
	RETURN_NULL();
} /* }}} */

/* {{{ proto mixed FilterIterator::current() U
       proto mixed CachingIterator::current() U
       proto mixed LimitIterator::current() U
       proto mixed ParentIterator::current() U
       proto mixed IteratorIterator::current() U
       proto mixed NoRewindIterator::current() U
       proto mixed AppendIterator::current() U
   Get the current element value */
SPL_METHOD(dual_it, current)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->current.data) {
		RETVAL_ZVAL(intern->current.data, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto void ParentIterator::next() U
       proto void IteratorIterator::next() U
       proto void NoRewindIterator::next() U
   Move the iterator forward */
SPL_METHOD(dual_it, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_dual_it_next(intern, 1 TSRMLS_CC);
	spl_dual_it_fetch(intern, 1 TSRMLS_CC);
} /* }}} */

static inline void spl_filter_it_fetch(zval *zthis, spl_dual_it_object *intern TSRMLS_DC)
{
	zval *retval;

	while (spl_dual_it_fetch(intern, 1 TSRMLS_CC) == SUCCESS) {
		zend_call_method_with_0_params(&zthis, intern->std.ce, NULL, "accept", &retval);
		if (retval) {
			if (zend_is_true(retval)) {
				zval_ptr_dtor(&retval);
				return;
			}
			zval_ptr_dtor(&retval);
		}
		if (EG(exception)) {
			return;
		}
		intern->inner.iterator->funcs->move_forward(intern->inner.iterator TSRMLS_CC);
	}
	spl_dual_it_free(intern TSRMLS_CC);
}

static inline void spl_filter_it_rewind(zval *zthis, spl_dual_it_object *intern TSRMLS_DC)
{
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_filter_it_fetch(zthis, intern TSRMLS_CC);
}

static inline void spl_filter_it_next(zval *zthis, spl_dual_it_object *intern TSRMLS_DC)
{
	spl_dual_it_next(intern, 1 TSRMLS_CC);
	spl_filter_it_fetch(zthis, intern TSRMLS_CC);
}

/* {{{ proto void FilterIterator::rewind() U
   Rewind the iterator */
SPL_METHOD(FilterIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_filter_it_rewind(getThis(), intern TSRMLS_CC);
} /* }}} */

/* {{{ proto void FilterIterator::next() U
   Move the iterator forward */
SPL_METHOD(FilterIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_filter_it_next(getThis(), intern TSRMLS_CC);
} /* }}} */

/* {{{ proto void RecursiveFilterIterator::__construct(RecursiveIterator it) U
   Create a RecursiveFilterIterator from a RecursiveIterator */
SPL_METHOD(RecursiveFilterIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveFilterIterator, spl_ce_RecursiveIterator, DIT_RecursiveFilterIterator);
} /* }}} */

/* {{{ proto bool RecursiveFilterIterator::hasChildren() U
   Check whether the inner iterator's current element has children */
SPL_METHOD(RecursiveFilterIterator, hasChildren)
{
	spl_dual_it_object   *intern;
	zval                 *retval;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "haschildren", &retval);
	if (retval) {
		RETURN_ZVAL(retval, 0, 1);
	} else {
		RETURN_FALSE;
	}
} /* }}} */

/* {{{ proto RecursiveFilterIterator RecursiveFilterIterator::getChildren() U
   Return the inner iterator's children contained in a RecursiveFilterIterator */
SPL_METHOD(RecursiveFilterIterator, getChildren)
{
	spl_dual_it_object   *intern;
	zval                 *retval;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "getchildren", &retval);
	if (!EG(exception) && retval) {
		spl_instantiate_arg_ex1(Z_OBJCE_P(getThis()), &return_value, 0, retval TSRMLS_CC);
	}
	if (retval) {
		zval_ptr_dtor(&retval);
	}
} /* }}} */

/* {{{ proto void ParentIterator::__construct(RecursiveIterator it) U
   Create a ParentIterator from a RecursiveIterator */
SPL_METHOD(ParentIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_ParentIterator, spl_ce_RecursiveIterator, DIT_ParentIterator);
} /* }}} */

#if HAVE_PCRE || HAVE_BUNDLED_PCRE
/* {{{ proto void RegexIterator::__construct(Iterator it, string regex [, int mode [, int flags [, int preg_flags]]]) U
   Create an RegexIterator from another iterator and a regular expression */
SPL_METHOD(RegexIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RegexIterator, zend_ce_iterator, DIT_RegexIterator);
} /* }}} */

/* {{{ proto bool RegexIterator::accept() U
   Match (string)current() against regular expression */
SPL_METHOD(RegexIterator, accept)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char       *subject, tmp[32], *result;
	int        subject_len, use_copy, count, result_len;
	zval       subject_copy, zcount, *replacement;

	if (intern->u.regex.flags & REGIT_USE_KEY) {
		if (intern->current.key_type == HASH_KEY_IS_LONG) {
			subject_len = snprintf(tmp, sizeof(tmp), "%ld", intern->current.int_key);
			subject = &tmp[0];
			use_copy = 0;
		} else if (intern->current.key_type == HASH_KEY_IS_UNICODE) {
			subject_len = intern->current.str_key_len - 1;
			subject = zend_unicode_to_ascii(intern->current.str_key.u, subject_len TSRMLS_CC);
			if (!subject) {
				/* FIXME: Unicode support??? : how to handle this error, with that exception? */
				if (intern->u.regex.mode != REGIT_MODE_MATCH) {
					zval_ptr_dtor(&intern->current.data);
					MAKE_STD_ZVAL(intern->current.data);
					array_init(intern->current.data);
				}
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Unicode key could not be converted to ascii");
				RETURN_FALSE;
			}
			use_copy = 1;
		} else {
			subject_len = intern->current.str_key_len - 1;
			subject = estrndup(intern->current.str_key.s, subject_len);
			use_copy = 1;
		}
	} else {
		zend_make_printable_zval(intern->current.data, &subject_copy, &use_copy);
		if (use_copy) {
			subject = Z_STRVAL(subject_copy);
			subject_len = Z_STRLEN(subject_copy);
		} else {
			subject = Z_STRVAL_P(intern->current.data);
			subject_len = Z_STRLEN_P(intern->current.data);
		}
	}

	switch (intern->u.regex.mode)
	{
	case REGIT_MODE_MAX: /* won't happen but makes compiler happy */
	case REGIT_MODE_MATCH:
		count = pcre_exec(intern->u.regex.pce->re, intern->u.regex.pce->extra, subject, subject_len, 0, 0, NULL, 0);
		RETVAL_BOOL(count >= 0);
		break;

	case REGIT_MODE_ALL_MATCHES:
	case REGIT_MODE_GET_MATCH:
		if (!use_copy) {
			subject = estrndup(subject, subject_len);
			use_copy = 1;
		}
		zval_ptr_dtor(&intern->current.data);
		ALLOC_INIT_ZVAL(intern->current.data);
		php_pcre_match_impl(intern->u.regex.pce, ZEND_STR_TYPE, subject, subject_len, &zcount, 
			intern->current.data, intern->u.regex.mode == REGIT_MODE_ALL_MATCHES, intern->u.regex.use_flags, intern->u.regex.preg_flags, 0 TSRMLS_CC);
		count = zend_hash_num_elements(Z_ARRVAL_P(intern->current.data));
		RETVAL_BOOL(count > 0);
		break;

	case REGIT_MODE_SPLIT:
		if (!use_copy) {
			subject = estrndup(subject, subject_len);
			use_copy = 1;
		}
		zval_ptr_dtor(&intern->current.data);
		ALLOC_INIT_ZVAL(intern->current.data);
		php_pcre_split_impl(intern->u.regex.pce, ZEND_STR_TYPE, subject, subject_len, intern->current.data, -1, intern->u.regex.preg_flags TSRMLS_CC);
		count = zend_hash_num_elements(Z_ARRVAL_P(intern->current.data));
		RETVAL_BOOL(count > 1);
		break;

	case REGIT_MODE_REPLACE:
		replacement = zend_read_property(intern->std.ce, getThis(), "replacement", sizeof("replacement")-1, 1 TSRMLS_CC);
		result = php_pcre_replace_impl(intern->u.regex.pce, ZEND_STR_TYPE, subject, subject_len, replacement, 0, &result_len, 0, NULL TSRMLS_CC);
		
		if (intern->u.regex.flags & REGIT_USE_KEY) {
			if (intern->current.key_type != HASH_KEY_IS_LONG) {
				efree(intern->current.str_key.v);
			}
			intern->current.key_type = HASH_KEY_IS_STRING;
			intern->current.str_key.s = result;
			intern->current.str_key_len = result_len + 1;
		} else {
			zval_ptr_dtor(&intern->current.data);
			MAKE_STD_ZVAL(intern->current.data);
			ZVAL_STRINGL(intern->current.data, result, result_len, 0);
		}
	}

	if (intern->u.regex.flags & REGIT_INVERTED) {
		RETVAL_BOOL(Z_LVAL_P(return_value));
	}

	if (use_copy) {
		efree(subject);
	}
} /* }}} */

/* {{{ proto bool RegexIterator::getMode() U
   Returns current operation mode */
SPL_METHOD(RegexIterator, getMode)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->u.regex.mode);
} /* }}} */

/* {{{ proto bool RegexIterator::setMode(int new_mode) U
   Set new operation mode */
SPL_METHOD(RegexIterator, setMode)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &mode) == FAILURE) {
		return;
	}

	if (mode < 0 || mode >= REGIT_MODE_MAX) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Illegal mode %ld", mode);
		return;/* NULL */
	}

	intern->u.regex.mode = mode;
} /* }}} */

/* {{{ proto bool RegexIterator::getFlags() U
   Returns current operation flags */
SPL_METHOD(RegexIterator, getFlags)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->u.regex.flags);
} /* }}} */

/* {{{ proto bool RegexIterator::setFlags(int new_flags) U
   Set operation flags */
SPL_METHOD(RegexIterator, setFlags)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}

	intern->u.regex.flags = flags;
} /* }}} */

/* {{{ proto bool RegexIterator::getFlags() U
   Returns current PREG flags (if in use or NULL) */
SPL_METHOD(RegexIterator, getPregFlags)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->u.regex.use_flags) {
		RETURN_LONG(intern->u.regex.preg_flags);
	} else {
		return;
	}
} /* }}} */

/* {{{ proto bool RegexIterator::setPregFlags(int new_flags) U
   Set PREG flags */
SPL_METHOD(RegexIterator, setPregFlags)
{
	spl_dual_it_object *intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long preg_flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &preg_flags) == FAILURE) {
		return;
	}

	intern->u.regex.preg_flags = preg_flags;
	intern->u.regex.use_flags = 1;
} /* }}} */

/* {{{ proto void RecursiveRegexIterator::__construct(RecursiveIterator it, string regex [, int mode [, int flags [, int preg_flags]]]) U
   Create an RecursiveRegexIterator from another recursive iterator and a regular expression */
SPL_METHOD(RecursiveRegexIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveRegexIterator, spl_ce_RecursiveIterator, DIT_RecursiveRegexIterator);
} /* }}} */

/* {{{ proto RecursiveRegexIterator RecursiveRegexIterator::getChildren() U
   Return the inner iterator's children contained in a RecursiveRegexIterator */
SPL_METHOD(RecursiveRegexIterator, getChildren)
{
	spl_dual_it_object   *intern;
	zval                 *retval, *regex;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "getchildren", &retval);
	if (!EG(exception)) {
		MAKE_STD_ZVAL(regex);
		ZVAL_STRING(regex, intern->u.regex.regex, 1);
		spl_instantiate_arg_ex2(Z_OBJCE_P(getThis()), &return_value, 0, retval, regex TSRMLS_CC);
		zval_ptr_dtor(&regex);
	}
	if (retval) {
		zval_ptr_dtor(&retval);
	}
} /* }}} */

#endif

/* {{{ spl_dual_it_free_storage */
static void spl_dual_it_free_storage(void *_object TSRMLS_DC)
{
	spl_dual_it_object        *object = (spl_dual_it_object *)_object;

	spl_dual_it_free(object TSRMLS_CC);

	if (object->inner.iterator) {
		object->inner.iterator->funcs->dtor(object->inner.iterator TSRMLS_CC);
	}

	if (object->inner.zobject) {
		zval_ptr_dtor(&object->inner.zobject);
	}
	
	if (object->dit_type == DIT_AppendIterator) {
		object->u.append.iterator->funcs->dtor(object->u.append.iterator TSRMLS_CC);
		if (object->u.append.zarrayit) {
			zval_ptr_dtor(&object->u.append.zarrayit);
		}
	}

	if (object->dit_type == DIT_CachingIterator || object->dit_type == DIT_RecursiveCachingIterator) {
		if (object->u.caching.zcache) {
			zval_ptr_dtor(&object->u.caching.zcache);
			object->u.caching.zcache = NULL;
		}
	}

#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	if (object->dit_type == DIT_RegexIterator || object->dit_type == DIT_RecursiveRegexIterator) {
		if (object->u.regex.pce) {
			object->u.regex.pce->refcount--;
		}
		if (object->u.regex.regex) {
			efree(object->u.regex.regex);
		}
	}
#endif

	zend_object_std_dtor(&object->std TSRMLS_CC);

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
	intern->dit_type = DIT_Unknown;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_dual_it_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_handlers_dual_it;
	return retval;
}
/* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_filter_it___construct, 0) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_FilterIterator[] = {
	SPL_ME(FilterIterator,  __construct,      arginfo_filter_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(FilterIterator,  rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(FilterIterator,  next,             NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         getInnerIterator, NULL, ZEND_ACC_PUBLIC)
	SPL_ABSTRACT_ME(FilterIterator, accept, NULL)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO(arginfo_parent_it___construct, 0) 
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_RecursiveFilterIterator[] = {
	SPL_ME(RecursiveFilterIterator,  __construct,      arginfo_parent_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveFilterIterator,  hasChildren,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveFilterIterator,  getChildren,      NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_funcs_ParentIterator[] = {
	SPL_ME(ParentIterator,  __construct,      arginfo_parent_it___construct, ZEND_ACC_PUBLIC)
	SPL_MA(ParentIterator,  accept,           RecursiveFilterIterator, hasChildren, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

#if HAVE_PCRE || HAVE_BUNDLED_PCRE
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_regex_it___construct, 0, 0, 2) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_INFO(0, regex)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, preg_flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_regex_it_set_mode, 0, 0, 1) 
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_regex_it_set_flags, 0, 0, 1) 
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_regex_it_set_preg_flags, 0, 0, 1) 
	ZEND_ARG_INFO(0, preg_flags)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_RegexIterator[] = {
	SPL_ME(RegexIterator,   __construct,      arginfo_regex_it___construct,    ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   accept,           NULL,                            ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   getMode,          NULL,                            ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   setMode,          arginfo_regex_it_set_mode,       ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   getFlags,         NULL,                            ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   setFlags,         arginfo_regex_it_set_flags,      ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   getPregFlags,     NULL,                            ZEND_ACC_PUBLIC)
	SPL_ME(RegexIterator,   setPregFlags,     arginfo_regex_it_set_preg_flags, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_rec_regex_it___construct, 0, 0, 2) 
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
	ZEND_ARG_INFO(0, regex)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, preg_flags)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_RecursiveRegexIterator[] = {
	SPL_ME(RecursiveRegexIterator,  __construct,      arginfo_rec_regex_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveFilterIterator, hasChildren,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveRegexIterator,  getChildren,      NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
#endif

static inline int spl_limit_it_valid(spl_dual_it_object *intern TSRMLS_DC)
{
	/* FAILURE / SUCCESS */
	if (intern->u.limit.count != -1 && intern->current.pos >= intern->u.limit.offset + intern->u.limit.count) {
		return FAILURE;
	} else {
		return spl_dual_it_valid(intern TSRMLS_CC);
	}
}

static inline void spl_limit_it_seek(spl_dual_it_object *intern, long pos TSRMLS_DC)
{
	zval  *zpos;

	spl_dual_it_free(intern TSRMLS_CC);
	if (pos < intern->u.limit.offset) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0 TSRMLS_CC, "Cannot seek to %ld which is below the offset %ld", pos, intern->u.limit.offset);
		return;
	}
	if (pos >= intern->u.limit.offset + intern->u.limit.count && intern->u.limit.count != -1) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0 TSRMLS_CC, "Cannot seek to %ld which is behind offest %ld plus count %ld", pos, intern->u.limit.offset, intern->u.limit.count);
		return;
	}
	if (instanceof_function(intern->inner.ce, spl_ce_SeekableIterator TSRMLS_CC)) {
		MAKE_STD_ZVAL(zpos);
		ZVAL_LONG(zpos, pos);
		spl_dual_it_free(intern TSRMLS_CC);
		zend_call_method_with_1_params(&intern->inner.zobject, intern->inner.ce, NULL, "seek", NULL, zpos);
		zval_ptr_dtor(&zpos);
		if (!EG(exception)) {
			intern->current.pos = pos;
			if (spl_limit_it_valid(intern TSRMLS_CC) == SUCCESS) {
				spl_dual_it_fetch(intern, 0 TSRMLS_CC);
			}
		}
	} else {
		/* emulate the forward seek, by next() calls */
		/* a back ward seek is done by a previous rewind() */
		if (pos < intern->current.pos) {
			spl_dual_it_rewind(intern TSRMLS_CC);
		}
		while (pos > intern->current.pos && spl_dual_it_valid(intern TSRMLS_CC) == SUCCESS) {
			spl_dual_it_next(intern, 1 TSRMLS_CC);
		}
		if (spl_dual_it_valid(intern TSRMLS_CC) == SUCCESS) {
			spl_dual_it_fetch(intern, 1 TSRMLS_CC);
		}
	}
}

/* {{{ proto LimitIterator::__construct(Iterator it [, int offset, int count]) U
   Construct a LimitIterator from an Iterator with a given starting offset and optionally a maximum count */
SPL_METHOD(LimitIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_LimitIterator, zend_ce_iterator, DIT_LimitIterator);
} /* }}} */

/* {{{ proto void LimitIterator::rewind() U
   Rewind the iterator to the specified starting offset */
SPL_METHOD(LimitIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_dual_it_rewind(intern TSRMLS_CC);
	spl_limit_it_seek(intern, intern->u.limit.offset TSRMLS_CC);
} /* }}} */

/* {{{ proto bool LimitIterator::valid() U
   Check whether the current element is valid */
SPL_METHOD(LimitIterator, valid)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

/*	RETURN_BOOL(spl_limit_it_valid(intern TSRMLS_CC) == SUCCESS);*/
	RETURN_BOOL((intern->u.limit.count == -1 || intern->current.pos < intern->u.limit.offset + intern->u.limit.count) && intern->current.data);
} /* }}} */

/* {{{ proto void LimitIterator::next() U
   Move the iterator forward */
SPL_METHOD(LimitIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_dual_it_next(intern, 1 TSRMLS_CC);
	if (intern->u.limit.count == -1 || intern->current.pos < intern->u.limit.offset + intern->u.limit.count) {
		spl_dual_it_fetch(intern, 1 TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto void LimitIterator::seek(int position) U
   Seek to the given position */
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
} /* }}} */

/* {{{ proto int LimitIterator::getPosition() U
   Return the current position */
SPL_METHOD(LimitIterator, getPosition)
{
	spl_dual_it_object   *intern;
	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_LONG(intern->current.pos);
} /* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_seekable_it_seek, 0) 
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_SeekableIterator[] = {
	SPL_ABSTRACT_ME(SeekableIterator, seek, arginfo_seekable_it_seek)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_limit_it___construct, 0, 0, 1) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_limit_it_seek, 0) 
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_LimitIterator[] = {
	SPL_ME(LimitIterator,   __construct,      arginfo_limit_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator,   rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator,   valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator,   next,             NULL, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator,   seek,             arginfo_limit_it_seek, ZEND_ACC_PUBLIC)
	SPL_ME(LimitIterator,   getPosition,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         getInnerIterator, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static inline int spl_caching_it_valid(spl_dual_it_object *intern TSRMLS_DC)
{
	return intern->u.caching.flags & CIT_VALID ? SUCCESS : FAILURE;
}

static inline int spl_caching_it_has_next(spl_dual_it_object *intern TSRMLS_DC)
{
	return spl_dual_it_valid(intern TSRMLS_CC);
}

static inline void spl_caching_it_next(spl_dual_it_object *intern TSRMLS_DC)
{
	if (spl_dual_it_fetch(intern, 1 TSRMLS_CC) == SUCCESS) {
		intern->u.caching.flags |= CIT_VALID;
		/* Full cache ? */
		if (intern->u.caching.flags & CIT_FULL_CACHE) {
			zval *zcacheval;
			
			MAKE_STD_ZVAL(zcacheval);
			ZVAL_ZVAL(zcacheval, intern->current.data, 1, 0);
			if (intern->current.key_type == HASH_KEY_IS_LONG) {
				add_index_zval(intern->u.caching.zcache, intern->current.int_key, zcacheval);
			} else {
				zend_u_symtable_update(HASH_OF(intern->u.caching.zcache), intern->current.key_type, intern->current.str_key, intern->current.str_key_len, &zcacheval, sizeof(void*), NULL);
			}
		}
		/* Recursion ? */
		if (intern->dit_type == DIT_RecursiveCachingIterator) {
			zval *retval, *zchildren, zflags;
			zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "haschildren", &retval);
			if (EG(exception)) {
				if (retval) {
					zval_ptr_dtor(&retval);
				}
				if (intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
					zend_clear_exception(TSRMLS_C);
				} else {
					return;
				}
			} else {
				if (zend_is_true(retval)) {
					zend_call_method_with_0_params(&intern->inner.zobject, intern->inner.ce, NULL, "getchildren", &zchildren);
					if (EG(exception)) {
						if (zchildren) {
							zval_ptr_dtor(&zchildren);
						}
						if (intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
							zend_clear_exception(TSRMLS_C);
						} else {
							zval_ptr_dtor(&retval);
							return;
						}
					} else {
						INIT_PZVAL(&zflags);
						ZVAL_LONG(&zflags, intern->u.caching.flags & CIT_PUBLIC);
						spl_instantiate_arg_ex2(spl_ce_RecursiveCachingIterator, &intern->u.caching.zchildren, 1, zchildren, &zflags TSRMLS_CC);
						zval_ptr_dtor(&zchildren);
					}
				}
				zval_ptr_dtor(&retval);
				if (EG(exception)) {
					if (intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
						zend_clear_exception(TSRMLS_C);
					} else {
						return;
					}
				}
			}
		}
		if (intern->u.caching.flags & (CIT_TOSTRING_USE_INNER|CIT_CALL_TOSTRING)) {
			int  use_copy;
			zval expr_copy;
			ALLOC_ZVAL(intern->u.caching.zstr);
			if (intern->u.caching.flags & CIT_TOSTRING_USE_INNER) {
				*intern->u.caching.zstr = *intern->inner.zobject;
			} else {
				*intern->u.caching.zstr = *intern->current.data;
			}
			if (UG(unicode)) {
				zend_make_unicode_zval(intern->u.caching.zstr, &expr_copy, &use_copy);
			} else {
				zend_make_string_zval(intern->u.caching.zstr, &expr_copy, &use_copy);
			}
			if (use_copy) {
				*intern->u.caching.zstr = expr_copy;
				INIT_PZVAL(intern->u.caching.zstr);
				zval_copy_ctor(intern->u.caching.zstr);
				zval_dtor(&expr_copy);
			} else {
				INIT_PZVAL(intern->u.caching.zstr);
				zval_copy_ctor(intern->u.caching.zstr);
			}
		}
		spl_dual_it_next(intern, 0 TSRMLS_CC);	
	} else {
		intern->u.caching.flags &= ~CIT_VALID;
	}
}

static inline void spl_caching_it_rewind(spl_dual_it_object *intern TSRMLS_DC)
{
	spl_dual_it_rewind(intern TSRMLS_CC);
	zend_hash_clean(HASH_OF(intern->u.caching.zcache));
	spl_caching_it_next(intern TSRMLS_CC);
}

/* {{{ proto void CachingIterator::__construct(Iterator it [, flags = CIT_CALL_TOSTRING]) U
   Construct a CachingIterator from an Iterator */
SPL_METHOD(CachingIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_CachingIterator, zend_ce_iterator, DIT_CachingIterator);
} /* }}} */

/* {{{ proto void CachingIterator::rewind() U
   Rewind the iterator */
SPL_METHOD(CachingIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_caching_it_rewind(intern TSRMLS_CC);
} /* }}} */

/* {{{ proto bool CachingIterator::valid() U
   Check whether the current element is valid */
SPL_METHOD(CachingIterator, valid)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_caching_it_valid(intern TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto void CachingIterator::next() U
   Move the iterator forward */
SPL_METHOD(CachingIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_caching_it_next(intern TSRMLS_CC);
} /* }}} */

/* {{{ proto bool CachingIterator::hasNext() U
   Check whether the inner iterator has a valid next element */
SPL_METHOD(CachingIterator, hasNext)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_caching_it_has_next(intern TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto string CachingIterator::__toString() U
   Return the string representation of the current element */
SPL_METHOD(CachingIterator, __toString)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & (CIT_CALL_TOSTRING|CIT_TOSTRING_USE_KEY|CIT_TOSTRING_USE_CURRENT|CIT_TOSTRING_USE_INNER)))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not fetch string value (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
	}
	if (intern->u.caching.flags & CIT_TOSTRING_USE_KEY) {
		if (intern->current.key_type == HASH_KEY_IS_STRING) {
			RETURN_STRINGL(intern->current.str_key.s, intern->current.str_key_len-1, 1);
		} else if (intern->current.key_type == HASH_KEY_IS_UNICODE) {
			RETURN_UNICODEL(intern->current.str_key.u, intern->current.str_key_len-1, 1);
		} else {
			RETVAL_LONG(intern->current.int_key);
			convert_to_text(return_value);
			return;
		}
	} else if (intern->u.caching.flags & CIT_TOSTRING_USE_CURRENT) {
		*return_value = *intern->current.data;
		zval_copy_ctor(return_value);
		convert_to_text(return_value);
		INIT_PZVAL(return_value);
		return;
	}
	if (intern->u.caching.zstr) {
		RETURN_ZVAL(intern->u.caching.zstr, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto void CachingIterator::offsetSet(mixed index, mixed newval) U
   Set given index in cache */
SPL_METHOD(CachingIterator, offsetSet)
{
	spl_dual_it_object   *intern;
	zstr arKey;
	uint nKeyLength;
	zend_uchar type;
	zval *value;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not use a full cache (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Tz", &arKey, &nKeyLength, &type, &value) == FAILURE) {
		return;
	}

	Z_ADDREF_P(value);
	zend_u_symtable_update(HASH_OF(intern->u.caching.zcache), type, arKey, nKeyLength+1, &value, sizeof(value), NULL);
}
/* }}} */

/* {{{ proto string CachingIterator::offsetGet(mixed index) U
   Return the internal cache if used */
SPL_METHOD(CachingIterator, offsetGet)
{
	spl_dual_it_object   *intern;
	zstr arKey;
	uint nKeyLength;
	zend_uchar type;
	zval **value;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not use a full cache (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "T", &arKey, &nKeyLength, &type) == FAILURE) {
		return;
	}

	if (zend_u_symtable_find(HASH_OF(intern->u.caching.zcache), type, arKey, nKeyLength+1, (void**)&value) == FAILURE) {
		zend_error(E_NOTICE, "Undefined index:  %R", type, arKey);
		return;
	}
	
	RETURN_ZVAL(*value, 1, 0);
}
/* }}} */

/* {{{ proto void CachingIterator::offsetUnset(mixed index) U
   Unset given index in cache */
SPL_METHOD(CachingIterator, offsetUnset)
{
	spl_dual_it_object   *intern;
	zstr arKey;
	uint nKeyLength;
	zend_uchar type;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not use a full cache (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "T", &arKey, &nKeyLength, &type) == FAILURE) {
		return;
	}

	zend_u_symtable_del(HASH_OF(intern->u.caching.zcache), type, arKey, nKeyLength+1);
}
/* }}} */

/* {{{ proto bool CachingIterator::offsetExists(mixed index) U
   Return whether the requested index exists */
SPL_METHOD(CachingIterator, offsetExists)
{
	spl_dual_it_object   *intern;
	zstr arKey;
	uint nKeyLength;
	zend_uchar type;
	
	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not use a full cache (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
		return;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "T", &arKey, &nKeyLength, &type) == FAILURE) {
		return;
	}

	RETURN_BOOL(zend_u_symtable_exists(HASH_OF(intern->u.caching.zcache), type, arKey, nKeyLength+1));
}
/* }}} */

/* {{{ proto bool CachingIterator::getCache() U
   Return the cache */
SPL_METHOD(CachingIterator, getCache)
{
	spl_dual_it_object   *intern;
	
	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not use a full cache (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
		return;
	}

	RETURN_ZVAL(intern->u.caching.zcache, 1, 0);
}
/* }}} */

/* {{{ proto int CachingIterator::getFlags() U
   Return the internal flags */
SPL_METHOD(CachingIterator, getFlags)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->u.caching.flags);
}
/* }}} */

/* {{{ proto void CachingIterator::setFlags(int flags) U
   Set the internal flags */
SPL_METHOD(CachingIterator, setFlags)
{
	spl_dual_it_object   *intern;
	long flags;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}

	if (spl_cit_check_flags(flags) != SUCCESS) {
		zend_throw_exception(spl_ce_InvalidArgumentException , "Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER", 0 TSRMLS_CC);
		return;
	}
	if ((intern->u.caching.flags & CIT_CALL_TOSTRING) != 0 && (flags & CIT_CALL_TOSTRING) == 0) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Unsetting flag CALL_TO_STRING is not possible", 0 TSRMLS_CC);
		return;
	}
	if ((intern->u.caching.flags & CIT_TOSTRING_USE_INNER) != 0 && (flags & CIT_TOSTRING_USE_INNER) == 0) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Unsetting flag TOSTRING_USE_INNER is not possible", 0 TSRMLS_CC);
		return;
	}
	if ((flags && CIT_FULL_CACHE) != 0 && (intern->u.caching.flags & CIT_FULL_CACHE) == 0) {
		/* clear on (re)enable */
		zend_hash_clean(HASH_OF(intern->u.caching.zcache));
	}
	intern->u.caching.flags = (intern->u.caching.flags & ~CIT_PUBLIC) | (flags & CIT_PUBLIC);
}
/* }}} */

/* {{{ proto void CachingIterator::count()
   Number of cached elements */
SPL_METHOD(CachingIterator, count)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%v does not use a full cache (see CachingIterator::__construct)", Z_OBJCE_P(getThis())->name);
		return;
	}

	RETURN_LONG(zend_hash_num_elements(HASH_OF(intern->u.caching.zcache)));
}
/* }}} */

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_caching_it___construct, 0, 0, 1) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_caching_it_setFlags, 0) 
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_caching_it_offsetGet, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_caching_it_offsetSet, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_CachingIterator[] = {
	SPL_ME(CachingIterator, __construct,      arginfo_caching_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, next,             NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, hasNext,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, __toString,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,         getInnerIterator, NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, getFlags,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, setFlags,         arginfo_caching_it_setFlags,    ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, offsetGet,        arginfo_caching_it_offsetGet,   ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, offsetSet,        arginfo_caching_it_offsetSet,   ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, offsetUnset,      arginfo_caching_it_offsetGet,   ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, offsetExists,     arginfo_caching_it_offsetGet,   ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, getCache,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(CachingIterator, count,            NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void RecursiveCachingIterator::__construct(RecursiveIterator it [, flags = CIT_CALL_TOSTRING]) U
   Create an iterator from a RecursiveIterator */
SPL_METHOD(RecursiveCachingIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveCachingIterator, spl_ce_RecursiveIterator, DIT_RecursiveCachingIterator);
} /* }}} */

/* {{{ proto bool RecursiveCachingIterator::hasChildren() U
   Check whether the current element of the inner iterator has children */
SPL_METHOD(RecursiveCachingIterator, hasChildren)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->u.caching.zchildren);
} /* }}} */

/* {{{ proto RecursiveCachingIterator RecursiveCachingIterator::getChildren() U
  Return the inner iterator's children as a RecursiveCachingIterator */
SPL_METHOD(RecursiveCachingIterator, getChildren)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->u.caching.zchildren) {
		RETURN_ZVAL(intern->u.caching.zchildren, 1, 0);
	} else {
		RETURN_NULL();
	}
} /* }}} */

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_caching_rec_it___construct, 0, ZEND_RETURN_VALUE, 1) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_RecursiveCachingIterator[] = {
	SPL_ME(RecursiveCachingIterator, __construct,   arginfo_caching_rec_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveCachingIterator, hasChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveCachingIterator, getChildren,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void IteratorIterator::__construct(Traversable it) U
   Create an iterator from anything that is traversable */
SPL_METHOD(IteratorIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_IteratorIterator, zend_ce_traversable, DIT_IteratorIterator);
} /* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_iterator_it___construct, 0) 
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_IteratorIterator[] = {
	SPL_ME(IteratorIterator, __construct,      arginfo_iterator_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          next,             NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          getInnerIterator, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void NoRewindIterator::__construct(Iterator it) U
   Create an iterator from another iterator */
SPL_METHOD(NoRewindIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_NoRewindIterator, zend_ce_iterator, DIT_NoRewindIterator);
} /* }}} */

/* {{{ proto void NoRewindIterator::rewind() U
   Prevent a call to inner iterators rewind() */
SPL_METHOD(NoRewindIterator, rewind)
{
	/* nothing to do */
} /* }}} */

/* {{{ proto bool NoRewindIterator::valid() U
   Return inner iterators valid() */
SPL_METHOD(NoRewindIterator, valid)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_BOOL(intern->inner.iterator->funcs->valid(intern->inner.iterator TSRMLS_CC) == SUCCESS);
} /* }}} */

/* {{{ proto mixed NoRewindIterator::key() U
   Return inner iterators key() */
SPL_METHOD(NoRewindIterator, key)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->inner.iterator->funcs->get_current_key) {
		zstr str_key;
		uint str_key_len;
		ulong int_key;
		switch (intern->inner.iterator->funcs->get_current_key(intern->inner.iterator, &str_key, &str_key_len, &int_key TSRMLS_CC)) {
			case HASH_KEY_IS_LONG:
				RETURN_LONG(int_key);
				break;	
			case HASH_KEY_IS_STRING:
				RETURN_STRINGL(str_key.s, str_key_len-1, 0);
				break;
			case HASH_KEY_IS_UNICODE:
				RETURN_UNICODEL(str_key.u, str_key_len-1, 0);
				break;
			default:
				RETURN_NULL();
		}
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto mixed NoRewindIterator::current() U
   Return inner iterators current() */
SPL_METHOD(NoRewindIterator, current)
{
	spl_dual_it_object   *intern;
	zval **data;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	intern->inner.iterator->funcs->get_current_data(intern->inner.iterator, &data TSRMLS_CC);
	RETURN_ZVAL(*data, 1, 0);
} /* }}} */

/* {{{ proto void NoRewindIterator::next() U
   Return inner iterators next() */
SPL_METHOD(NoRewindIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	intern->inner.iterator->funcs->move_forward(intern->inner.iterator TSRMLS_CC);
} /* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_norewind_it___construct, 0) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_NoRewindIterator[] = {
	SPL_ME(NoRewindIterator, __construct,      arginfo_norewind_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(NoRewindIterator, rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(NoRewindIterator, valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(NoRewindIterator, key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(NoRewindIterator, current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(NoRewindIterator, next,             NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,          getInnerIterator, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void InfiniteIterator::__construct(Iterator it) U
   Create an iterator from another iterator */
SPL_METHOD(InfiniteIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_InfiniteIterator, zend_ce_iterator, DIT_InfiniteIterator);
} /* }}} */

/* {{{ proto void InfiniteIterator::next() U
   Prevent a call to inner iterators rewind() (internally the current data will be fetched if valid()) */
SPL_METHOD(InfiniteIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_dual_it_next(intern, 1 TSRMLS_CC);
	if (spl_dual_it_valid(intern TSRMLS_CC) == SUCCESS) {
		spl_dual_it_fetch(intern, 0 TSRMLS_CC);
	} else {
		spl_dual_it_rewind(intern TSRMLS_CC);
		if (spl_dual_it_valid(intern TSRMLS_CC) == SUCCESS) {
			spl_dual_it_fetch(intern, 0 TSRMLS_CC);
		}
	}
} /* }}} */

static const zend_function_entry spl_funcs_InfiniteIterator[] = {
	SPL_ME(InfiniteIterator, __construct,      arginfo_norewind_it___construct, ZEND_ACC_PUBLIC)
	SPL_ME(InfiniteIterator, next,             NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void EmptyIterator::rewind() U
   Does nothing  */
SPL_METHOD(EmptyIterator, rewind)
{
} /* }}} */

/* {{{ proto false EmptyIterator::valid() U
   Return false */
SPL_METHOD(EmptyIterator, valid)
{
	RETURN_FALSE;
} /* }}} */

/* {{{ proto void EmptyIterator::key() U
   Throws exception BadMethodCallException */
SPL_METHOD(EmptyIterator, key)
{
	zend_throw_exception(spl_ce_BadMethodCallException, "Accessing the key of an EmptyIterator", 0 TSRMLS_CC);
} /* }}} */

/* {{{ proto void EmptyIterator::current() U
   Throws exception BadMethodCallException */
SPL_METHOD(EmptyIterator, current)
{
	zend_throw_exception(spl_ce_BadMethodCallException, "Accessing the value of an EmptyIterator", 0 TSRMLS_CC);
} /* }}} */

/* {{{ proto void EmptyIterator::next() U
   Does nothing */
SPL_METHOD(EmptyIterator, next)
{
} /* }}} */

static const zend_function_entry spl_funcs_EmptyIterator[] = {
	SPL_ME(EmptyIterator, rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(EmptyIterator, valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(EmptyIterator, key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(EmptyIterator, current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(EmptyIterator, next,             NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

int spl_append_it_next_iterator(spl_dual_it_object *intern TSRMLS_DC) /* {{{*/
{
	spl_dual_it_free(intern TSRMLS_CC);

	if (intern->inner.zobject) {
		zval_ptr_dtor(&intern->inner.zobject);
		intern->inner.zobject = NULL;
		intern->inner.ce = NULL;
		intern->inner.object = NULL;
		if (intern->inner.iterator) {
			intern->inner.iterator->funcs->dtor(intern->inner.iterator TSRMLS_CC);
			intern->inner.iterator = NULL;
		}
	}
	if (intern->u.append.iterator->funcs->valid(intern->u.append.iterator TSRMLS_CC) == SUCCESS) {
		zval **it;

		intern->u.append.iterator->funcs->get_current_data(intern->u.append.iterator, &it TSRMLS_CC);
		Z_ADDREF_PP(it);
		intern->inner.zobject = *it;
		intern->inner.ce = Z_OBJCE_PP(it);
		intern->inner.object = zend_object_store_get_object(*it TSRMLS_CC);
		intern->inner.iterator = intern->inner.ce->get_iterator(intern->inner.ce, *it, 0 TSRMLS_CC);
		spl_dual_it_rewind(intern TSRMLS_CC);
		return SUCCESS;
	} else {
		return FAILURE;
	}
} /* }}} */

void spl_append_it_fetch(spl_dual_it_object *intern TSRMLS_DC) /* {{{*/
{
	while (spl_dual_it_valid(intern TSRMLS_CC) != SUCCESS) {
		intern->u.append.iterator->funcs->move_forward(intern->u.append.iterator TSRMLS_CC);
		if (spl_append_it_next_iterator(intern TSRMLS_CC) != SUCCESS) {
			return;
		}
	}
	spl_dual_it_fetch(intern, 0 TSRMLS_CC);
} /* }}} */

void spl_append_it_next(spl_dual_it_object *intern TSRMLS_DC) /* {{{ */
{
	if (spl_dual_it_valid(intern TSRMLS_CC) == SUCCESS) {
		spl_dual_it_next(intern, 1 TSRMLS_CC);
	}
	spl_append_it_fetch(intern TSRMLS_CC);
} /* }}} */

/* {{{ proto void AppendIterator::__construct() U
   Create an AppendIterator */
SPL_METHOD(AppendIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_AppendIterator, zend_ce_iterator, DIT_AppendIterator);
} /* }}} */

/* {{{ proto void AppendIterator::append(Iterator it) U
   Append an iterator */
SPL_METHOD(AppendIterator, append)
{
	spl_dual_it_object   *intern;
	zval *it;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	APPENDIT_CHECK_CTOR(intern);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O", &it, zend_ce_iterator) == FAILURE) {
		return;
	}
	spl_array_iterator_append(intern->u.append.zarrayit, it TSRMLS_CC);

	if (!intern->inner.iterator || spl_dual_it_valid(intern TSRMLS_CC) != SUCCESS) {
		if (intern->u.append.iterator->funcs->valid(intern->u.append.iterator TSRMLS_CC) != SUCCESS) {
			intern->u.append.iterator->funcs->rewind(intern->u.append.iterator TSRMLS_CC);
		}
		do {
			spl_append_it_next_iterator(intern TSRMLS_CC);
		} while (intern->inner.zobject != it);
		spl_append_it_fetch(intern TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto void AppendIterator::rewind() U
   Rewind to the first iterator and rewind the first iterator, too */
SPL_METHOD(AppendIterator, rewind)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	intern->u.append.iterator->funcs->rewind(intern->u.append.iterator TSRMLS_CC);
	if (spl_append_it_next_iterator(intern TSRMLS_CC) == SUCCESS) {
		spl_append_it_fetch(intern TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto bool AppendIterator::valid() U
   Check if the current state is valid */
SPL_METHOD(AppendIterator, valid)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->current.data);
} /* }}} */

/* {{{ proto void AppendIterator::next() U
   Forward to next element */
SPL_METHOD(AppendIterator, next)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	spl_append_it_next(intern TSRMLS_CC);
} /* }}} */

/* {{{ proto int AppendIterator::getIteratorIndex() U
   Get index of iterator */
SPL_METHOD(AppendIterator, getIteratorIndex)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	APPENDIT_CHECK_CTOR(intern);
	spl_array_iterator_key(intern->u.append.zarrayit, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto ArrayIterator AppendIterator::getArrayIterator() U
   Get access to inner ArrayIterator */
SPL_METHOD(AppendIterator, getArrayIterator)
{
	spl_dual_it_object   *intern;

	intern = (spl_dual_it_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	APPENDIT_CHECK_CTOR(intern);
	RETURN_ZVAL(intern->u.append.zarrayit, 1, 0);
} /* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_append_it_append, 0) 
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_AppendIterator[] = {
	SPL_ME(AppendIterator, __construct,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(AppendIterator, append,           arginfo_append_it_append, ZEND_ACC_PUBLIC)
	SPL_ME(AppendIterator, rewind,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(AppendIterator, valid,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,        key,              NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,        current,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(AppendIterator, next,             NULL, ZEND_ACC_PUBLIC)
	SPL_ME(dual_it,        getInnerIterator, NULL, ZEND_ACC_PUBLIC)
	SPL_ME(AppendIterator, getIteratorIndex, NULL, ZEND_ACC_PUBLIC)
	SPL_ME(AppendIterator, getArrayIterator, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

PHPAPI int spl_iterator_apply(zval *obj, spl_iterator_apply_func_t apply_func, void *puser TSRMLS_DC)
{
	zend_object_iterator   *iter;
	zend_class_entry       *ce = Z_OBJCE_P(obj);

	iter = ce->get_iterator(ce, obj, 0 TSRMLS_CC);

	if (EG(exception)) {
		goto done;
	}

	if (iter->funcs->rewind) {
		iter->funcs->rewind(iter TSRMLS_CC);
		if (EG(exception)) {
			goto done;
		}
	}

	while (iter->funcs->valid(iter TSRMLS_CC) == SUCCESS) {
		if (EG(exception)) {
			goto done;
		}
		if (apply_func(iter, puser TSRMLS_CC) == ZEND_HASH_APPLY_STOP || EG(exception)) {
			goto done;
		}
		iter->funcs->move_forward(iter TSRMLS_CC);
		if (EG(exception)) {
			goto done;
		}
	}

done:
	iter->funcs->dtor(iter TSRMLS_CC);
	return EG(exception) ? FAILURE : SUCCESS;
}
/* }}} */

static int spl_iterator_to_array_apply(zend_object_iterator *iter, void *puser TSRMLS_DC) /* {{{ */
{
	zval                    **data, *return_value = (zval*)puser;
	zstr                    str_key;
	uint                    str_key_len;
	ulong                   int_key;
	int                     key_type;

	iter->funcs->get_current_data(iter, &data TSRMLS_CC);
	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (iter->funcs->get_current_key) {
		key_type = iter->funcs->get_current_key(iter, &str_key, &str_key_len, &int_key TSRMLS_CC);
		if (EG(exception)) {
			return ZEND_HASH_APPLY_STOP;
		}
		Z_ADDREF_PP(data);
		switch(key_type) {
			case HASH_KEY_IS_STRING:
				add_assoc_zval_ex(return_value, str_key.s, str_key_len, *data);
				efree(str_key.s);
				break;
			case HASH_KEY_IS_UNICODE:
				add_u_assoc_zval_ex(return_value, IS_UNICODE, str_key, str_key_len, *data);
				efree(str_key.u);
				break;
			case HASH_KEY_IS_LONG:
				add_index_zval(return_value, int_key, *data);
				break;
		}
	} else {
		Z_ADDREF_PP(data);
		add_next_index_zval(return_value, *data);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int spl_iterator_to_values_apply(zend_object_iterator *iter, void *puser TSRMLS_DC) /* {{{ */
{
	zval **data, *return_value = (zval*)puser;

	iter->funcs->get_current_data(iter, &data TSRMLS_CC);
	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	Z_ADDREF_PP(data);
	add_next_index_zval(return_value, *data);
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto array iterator_to_array(Traversable it [, bool use_keys = true]) U
   Copy the iterator into an array */
PHP_FUNCTION(iterator_to_array)
{
	zval  *obj;
	zend_bool use_keys = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|b", &obj, zend_ce_traversable, &use_keys) == FAILURE) {
		RETURN_FALSE;
	}

	array_init(return_value);

	if (spl_iterator_apply(obj, use_keys ? spl_iterator_to_array_apply : spl_iterator_to_values_apply, (void*)return_value TSRMLS_CC) != SUCCESS) {
		zval_dtor(return_value);
		RETURN_NULL();
	}
} /* }}} */

static int spl_iterator_count_apply(zend_object_iterator *iter, void *puser TSRMLS_DC) /* {{{ */
{
	(*(long*)puser)++;
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto int iterator_count(Traversable it) U
   Count the elements in an iterator */
PHP_FUNCTION(iterator_count)
{
	zval  *obj;
	long  count = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &obj, zend_ce_traversable) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (spl_iterator_apply(obj, spl_iterator_count_apply, (void*)&count TSRMLS_CC) == SUCCESS) {
		RETURN_LONG(count);
	}
}
/* }}} */

typedef struct {
	zval                   *obj;
	zval                   *args;
	long                   count;
	zend_fcall_info        fci;
	zend_fcall_info_cache  fcc;
} spl_iterator_apply_info;

static int spl_iterator_func_apply(zend_object_iterator *iter, void *puser TSRMLS_DC) /* {{{ */
{
	zval *retval;
	spl_iterator_apply_info  *apply_info = (spl_iterator_apply_info*)puser;
	int result;

	apply_info->count++;
	zend_fcall_info_call(&apply_info->fci, &apply_info->fcc, &retval, NULL TSRMLS_CC);
	if (retval) {
		result = zend_is_true(retval) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_STOP;
		zval_ptr_dtor(&retval);
	} else {
		result = ZEND_HASH_APPLY_STOP;
	}
	return result;
}
/* }}} */

/* {{{ proto int iterator_apply(Traversable it, mixed function [, mixed params]) U
   Calls a function for every element in an iterator */
PHP_FUNCTION(iterator_apply)
{
	spl_iterator_apply_info  apply_info;

	apply_info.args = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Of|a!", &apply_info.obj, zend_ce_traversable, &apply_info.fci, &apply_info.fcc, &apply_info.args) == FAILURE) {
		return;
	}

	apply_info.count = 0;
	zend_fcall_info_args(&apply_info.fci, apply_info.args TSRMLS_CC);
	if (spl_iterator_apply(apply_info.obj, spl_iterator_func_apply, (void*)&apply_info TSRMLS_CC) == SUCCESS) {
		RETVAL_LONG(apply_info.count);
	} else {
		RETVAL_FALSE;
	}
	zend_fcall_info_args(&apply_info.fci, NULL TSRMLS_CC);
}
/* }}} */

static const zend_function_entry spl_funcs_OuterIterator[] = {
	SPL_ABSTRACT_ME(OuterIterator, getInnerIterator,   NULL)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_funcs_Countable[] = {
	SPL_ABSTRACT_ME(Countable, count,   NULL)
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
	spl_handlers_rec_it_it.get_method = spl_recursive_it_get_method;
	spl_handlers_rec_it_it.clone_obj = NULL;

	memcpy(&spl_handlers_dual_it, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_handlers_dual_it.get_method = spl_dual_it_get_method;
	/*spl_handlers_dual_it.call_method = spl_dual_it_call_method;*/
	spl_handlers_dual_it.clone_obj = NULL;
	
	spl_ce_RecursiveIteratorIterator->get_iterator = spl_recursive_it_get_iterator;
	spl_ce_RecursiveIteratorIterator->iterator_funcs.funcs = &spl_recursive_it_iterator_funcs;

	REGISTER_SPL_CLASS_CONST_LONG(RecursiveIteratorIterator, "LEAVES_ONLY",     RIT_LEAVES_ONLY);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveIteratorIterator, "SELF_FIRST",      RIT_SELF_FIRST);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveIteratorIterator, "CHILD_FIRST",     RIT_CHILD_FIRST);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveIteratorIterator, "CATCH_GET_CHILD", RIT_CATCH_GET_CHILD);

	REGISTER_SPL_INTERFACE(OuterIterator);
	REGISTER_SPL_ITERATOR(OuterIterator);

	REGISTER_SPL_STD_CLASS_EX(IteratorIterator, spl_dual_it_new, spl_funcs_IteratorIterator);
	REGISTER_SPL_ITERATOR(IteratorIterator);
	REGISTER_SPL_IMPLEMENTS(IteratorIterator, OuterIterator);

	REGISTER_SPL_SUB_CLASS_EX(FilterIterator, IteratorIterator, spl_dual_it_new, spl_funcs_FilterIterator);
	spl_ce_FilterIterator->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	REGISTER_SPL_SUB_CLASS_EX(RecursiveFilterIterator, FilterIterator, spl_dual_it_new, spl_funcs_RecursiveFilterIterator);
	REGISTER_SPL_IMPLEMENTS(RecursiveFilterIterator, RecursiveIterator);

	REGISTER_SPL_SUB_CLASS_EX(ParentIterator, RecursiveFilterIterator, spl_dual_it_new, spl_funcs_ParentIterator);

	REGISTER_SPL_INTERFACE(Countable);
	REGISTER_SPL_INTERFACE(SeekableIterator);
	REGISTER_SPL_ITERATOR(SeekableIterator);

	REGISTER_SPL_SUB_CLASS_EX(LimitIterator, IteratorIterator, spl_dual_it_new, spl_funcs_LimitIterator);

	REGISTER_SPL_SUB_CLASS_EX(CachingIterator, IteratorIterator, spl_dual_it_new, spl_funcs_CachingIterator);
	REGISTER_SPL_IMPLEMENTS(CachingIterator, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(CachingIterator, Countable);

	REGISTER_SPL_CLASS_CONST_LONG(CachingIterator, "CALL_TOSTRING",        CIT_CALL_TOSTRING); 
	REGISTER_SPL_CLASS_CONST_LONG(CachingIterator, "CATCH_GET_CHILD",      CIT_CATCH_GET_CHILD); 
	REGISTER_SPL_CLASS_CONST_LONG(CachingIterator, "TOSTRING_USE_KEY",     CIT_TOSTRING_USE_KEY);
	REGISTER_SPL_CLASS_CONST_LONG(CachingIterator, "TOSTRING_USE_CURRENT", CIT_TOSTRING_USE_CURRENT);
	REGISTER_SPL_CLASS_CONST_LONG(CachingIterator, "TOSTRING_USE_INNER",   CIT_TOSTRING_USE_INNER);
	REGISTER_SPL_CLASS_CONST_LONG(CachingIterator, "FULL_CACHE",           CIT_FULL_CACHE); 

	REGISTER_SPL_SUB_CLASS_EX(RecursiveCachingIterator, CachingIterator, spl_dual_it_new, spl_funcs_RecursiveCachingIterator);
	REGISTER_SPL_IMPLEMENTS(RecursiveCachingIterator, RecursiveIterator);
	
	REGISTER_SPL_SUB_CLASS_EX(NoRewindIterator, IteratorIterator, spl_dual_it_new, spl_funcs_NoRewindIterator);

	REGISTER_SPL_SUB_CLASS_EX(AppendIterator, IteratorIterator, spl_dual_it_new, spl_funcs_AppendIterator);

	REGISTER_SPL_IMPLEMENTS(RecursiveIteratorIterator, OuterIterator);

	REGISTER_SPL_SUB_CLASS_EX(InfiniteIterator, IteratorIterator, spl_dual_it_new, spl_funcs_InfiniteIterator);
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	REGISTER_SPL_SUB_CLASS_EX(RegexIterator, FilterIterator, spl_dual_it_new, spl_funcs_RegexIterator);
	REGISTER_SPL_CLASS_CONST_LONG(RegexIterator, "USE_KEY",     REGIT_USE_KEY);
	REGISTER_SPL_CLASS_CONST_LONG(RegexIterator, "MATCH",       REGIT_MODE_MATCH);
	REGISTER_SPL_CLASS_CONST_LONG(RegexIterator, "GET_MATCH",   REGIT_MODE_GET_MATCH);
	REGISTER_SPL_CLASS_CONST_LONG(RegexIterator, "ALL_MATCHES", REGIT_MODE_ALL_MATCHES);
	REGISTER_SPL_CLASS_CONST_LONG(RegexIterator, "SPLIT",       REGIT_MODE_SPLIT);
	REGISTER_SPL_CLASS_CONST_LONG(RegexIterator, "REPLACE",     REGIT_MODE_REPLACE);
	REGISTER_SPL_PROPERTY(RegexIterator, "replacement", 0);
	REGISTER_SPL_SUB_CLASS_EX(RecursiveRegexIterator, RegexIterator, spl_dual_it_new, spl_funcs_RecursiveRegexIterator);
	REGISTER_SPL_IMPLEMENTS(RecursiveRegexIterator, RecursiveIterator);
#else
	spl_ce_RegexIterator = NULL;
	spl_ce_RecursiveRegexIterator = NULL;
#endif

	REGISTER_SPL_STD_CLASS_EX(EmptyIterator, NULL, spl_funcs_EmptyIterator);
	REGISTER_SPL_ITERATOR(EmptyIterator);

	REGISTER_SPL_SUB_CLASS_EX(RecursiveTreeIterator, RecursiveIteratorIterator, spl_RecursiveTreeIterator_new, spl_funcs_RecursiveTreeIterator);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "BYPASS_CURRENT",      RTIT_BYPASS_CURRENT);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "BYPASS_KEY",          RTIT_BYPASS_KEY);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "PREFIX_LEFT",         0);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "PREFIX_MID_HAS_NEXT", 1);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "PREFIX_MID_LAST",     2);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "PREFIX_END_HAS_NEXT", 3);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "PREFIX_END_LAST",     4);
	REGISTER_SPL_CLASS_CONST_LONG(RecursiveTreeIterator, "PREFIX_RIGHT",        5);

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
