/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "ext/pcre/php_pcre.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_iterators_arginfo.h"
#include "spl_directory.h"
#include "spl_array.h"
#include "spl_exceptions.h"
#include "zend_smart_str.h"

#ifdef accept
#undef accept
#endif

PHPAPI zend_class_entry *spl_ce_RecursiveIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveIteratorIterator;
PHPAPI zend_class_entry *spl_ce_FilterIterator;
PHPAPI zend_class_entry *spl_ce_CallbackFilterIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveFilterIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveCallbackFilterIterator;
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
PHPAPI zend_class_entry *spl_ce_RecursiveTreeIterator;

typedef enum {
	RS_NEXT  = 0,
	RS_TEST  = 1,
	RS_SELF  = 2,
	RS_CHILD = 3,
	RS_START = 4
} RecursiveIteratorState;

typedef struct _spl_sub_iterator {
	zend_object_iterator    *iterator;
	zval                    zobject;
	zend_class_entry        *ce;
	RecursiveIteratorState  state;
	zend_function           *haschildren;
	zend_function           *getchildren;
} spl_sub_iterator;

typedef struct _spl_recursive_it_object {
	spl_sub_iterator         *iterators;
	int                      level;
	RecursiveIteratorMode    mode;
	int                      flags;
	int                      max_depth;
	bool                in_iteration;
	zend_function            *beginIteration;
	zend_function            *endIteration;
	zend_function            *callHasChildren;
	zend_function            *callGetChildren;
	zend_function            *beginChildren;
	zend_function            *endChildren;
	zend_function            *nextElement;
	zend_class_entry         *ce;
	zend_string              *prefix[6];
	zend_string              *postfix[1];
	zend_object              std;
} spl_recursive_it_object;

typedef struct _spl_recursive_it_iterator {
	zend_object_iterator   intern;
} spl_recursive_it_iterator;

typedef struct _spl_cbfilter_it_intern {
	zend_fcall_info       fci;
	zend_fcall_info_cache fcc;
	zend_object           *object;
} _spl_cbfilter_it_intern;

typedef struct _spl_dual_it_object {
	struct {
		zval                 zobject;
		zend_class_entry     *ce;
		zend_object          *object;
		zend_object_iterator *iterator;
	} inner;
	struct {
		zval                 data;
		zval                 key;
		zend_long            pos;
	} current;
	dual_it_type             dit_type;
	union {
		struct {
			zend_long             offset;
			zend_long             count;
		} limit;
		struct {
			zend_long             flags; /* CIT_* */
			zend_string          *zstr;
			zval             zchildren;
			zval             zcache;
		} caching;
		struct {
			zval                  zarrayit;
			zend_object_iterator *iterator;
		} append;
		struct {
			zend_long        flags;
			zend_long        preg_flags;
			pcre_cache_entry *pce;
			zend_string      *regex;
			regex_mode       mode;
			int              use_flags;
		} regex;
		_spl_cbfilter_it_intern *cbfilter;
	} u;
	zend_object              std;
} spl_dual_it_object;

static zend_object_handlers spl_handlers_rec_it_it;
static zend_object_handlers spl_handlers_dual_it;

static inline spl_recursive_it_object *spl_recursive_it_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_recursive_it_object*)((char*)(obj) - XtOffsetOf(spl_recursive_it_object, std));
}
/* }}} */

#define Z_SPLRECURSIVE_IT_P(zv)  spl_recursive_it_from_obj(Z_OBJ_P((zv)))

static inline spl_dual_it_object *spl_dual_it_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_dual_it_object*)((char*)(obj) - XtOffsetOf(spl_dual_it_object, std));
} /* }}} */

#define Z_SPLDUAL_IT_P(zv)  spl_dual_it_from_obj(Z_OBJ_P((zv)))

#define SPL_FETCH_AND_CHECK_DUAL_IT(var, objzval) 												\
	do { 																						\
		spl_dual_it_object *it = Z_SPLDUAL_IT_P(objzval); 										\
		if (it->dit_type == DIT_Unknown) { 														\
			zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called"); 	\
			RETURN_THROWS(); 																			\
		} 																						\
		(var) = it; 																			\
	} while (0)

#define SPL_FETCH_SUB_ELEMENT(var, object, element) \
	do { \
		if(!(object)->iterators) { \
			zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called"); \
			return; \
		} \
		(var) = (object)->iterators[(object)->level].element; \
	} while (0)

#define SPL_FETCH_SUB_ELEMENT_ADDR(var, object, element) \
	do { \
		if(!(object)->iterators) { \
			zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called"); \
			RETURN_THROWS(); \
		} \
		(var) = &(object)->iterators[(object)->level].element; \
	} while (0)

#define SPL_FETCH_SUB_ITERATOR(var, object) SPL_FETCH_SUB_ELEMENT(var, object, iterator)


static void spl_recursive_it_dtor(zend_object_iterator *_iter)
{
	spl_recursive_it_iterator *iter   = (spl_recursive_it_iterator*)_iter;
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(&iter->intern.data);
	zend_object_iterator      *sub_iter;

	if (object->iterators) {
		while (object->level > 0) {
			if (!Z_ISUNDEF(object->iterators[object->level].zobject)) {
				sub_iter = object->iterators[object->level].iterator;
				zend_iterator_dtor(sub_iter);
				zval_ptr_dtor(&object->iterators[object->level].zobject);
			}
			object->level--;
		}
		object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator));
		object->level = 0;
	}

	zval_ptr_dtor(&iter->intern.data);
}

static int spl_recursive_it_valid_ex(spl_recursive_it_object *object, zval *zthis)
{
	zend_object_iterator      *sub_iter;
	int                       level = object->level;

	if(!object->iterators) {
		return FAILURE;
	}
	while (level >=0) {
		sub_iter = object->iterators[level].iterator;
		if (sub_iter->funcs->valid(sub_iter) == SUCCESS) {
			return SUCCESS;
		}
		level--;
	}
	if (object->endIteration && object->in_iteration) {
		zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->endIteration, "endIteration", NULL);
	}
	object->in_iteration = 0;
	return FAILURE;
}

static int spl_recursive_it_valid(zend_object_iterator *iter)
{
	return spl_recursive_it_valid_ex(Z_SPLRECURSIVE_IT_P(&iter->data), &iter->data);
}

static zval *spl_recursive_it_get_current_data(zend_object_iterator *iter)
{
	spl_recursive_it_object *object = Z_SPLRECURSIVE_IT_P(&iter->data);
	zend_object_iterator *sub_iter = object->iterators[object->level].iterator;

	return sub_iter->funcs->get_current_data(sub_iter);
}

static void spl_recursive_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	spl_recursive_it_object *object = Z_SPLRECURSIVE_IT_P(&iter->data);
	zend_object_iterator *sub_iter = object->iterators[object->level].iterator;

	if (sub_iter->funcs->get_current_key) {
		sub_iter->funcs->get_current_key(sub_iter, key);
	} else {
		ZVAL_LONG(key, iter->index);
	}
}

static void spl_recursive_it_move_forward_ex(spl_recursive_it_object *object, zval *zthis)
{
	zend_object_iterator      *iterator;
	zend_class_entry          *ce;
	zval                      retval, child;
	zend_object_iterator      *sub_iter;
	int                       has_children;

	SPL_FETCH_SUB_ITERATOR(iterator, object);

	while (!EG(exception)) {
next_step:
		iterator = object->iterators[object->level].iterator;
		switch (object->iterators[object->level].state) {
			case RS_NEXT:
				iterator->funcs->move_forward(iterator);
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception();
					}
				}
				ZEND_FALLTHROUGH;
			case RS_START:
				if (iterator->funcs->valid(iterator) == FAILURE) {
					break;
				}
				object->iterators[object->level].state = RS_TEST;
				/* break; */
				/* TODO: Check this is correct */
				ZEND_FALLTHROUGH;
			case RS_TEST:
				if (object->callHasChildren) {
					zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->callHasChildren, "callHasChildren", &retval);
				} else {
					zend_class_entry *ce = object->iterators[object->level].ce;
					zend_object *obj = Z_OBJ(object->iterators[object->level].zobject);
					zend_function **cache = &object->iterators[object->level].haschildren;

					zend_call_method_with_0_params(obj, ce, cache, "haschildren", &retval);
				}
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						object->iterators[object->level].state = RS_NEXT;
						return;
					} else {
						zend_clear_exception();
					}
				}
				if (Z_TYPE(retval) != IS_UNDEF) {
					has_children = zend_is_true(&retval);
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
					zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->nextElement, "nextelement", NULL);
				}
				object->iterators[object->level].state = RS_NEXT;
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception();
					}
				}
				return /* self */;
			case RS_SELF:
				if (object->nextElement && (object->mode == RIT_SELF_FIRST || object->mode == RIT_CHILD_FIRST)) {
					zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->nextElement, "nextelement", NULL);
				}
				if (object->mode == RIT_SELF_FIRST) {
					object->iterators[object->level].state = RS_CHILD;
				} else {
					object->iterators[object->level].state = RS_NEXT;
				}
				return /* self */;
			case RS_CHILD:
				if (object->callGetChildren) {
					zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->callGetChildren, "callGetChildren", &child);
				} else {
					zend_class_entry *ce = object->iterators[object->level].ce;
					zend_object *obj = Z_OBJ(object->iterators[object->level].zobject);
					zend_function **cache = &object->iterators[object->level].getchildren;

					zend_call_method_with_0_params(obj, ce, cache, "getchildren", &child);
				}

				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception();
						zval_ptr_dtor(&child);
						object->iterators[object->level].state = RS_NEXT;
						goto next_step;
					}
				}

				if (Z_TYPE(child) == IS_UNDEF || Z_TYPE(child) != IS_OBJECT ||
						!((ce = Z_OBJCE(child)) && instanceof_function(ce, spl_ce_RecursiveIterator))) {
					zval_ptr_dtor(&child);
					zend_throw_exception(spl_ce_UnexpectedValueException, "Objects returned by RecursiveIterator::getChildren() must implement RecursiveIterator", 0);
					return;
				}

				if (object->mode == RIT_CHILD_FIRST) {
					object->iterators[object->level].state = RS_SELF;
				} else {
					object->iterators[object->level].state = RS_NEXT;
				}
				object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator) * (++object->level+1));
				sub_iter = ce->get_iterator(ce, &child, 0);
				ZVAL_COPY_VALUE(&object->iterators[object->level].zobject, &child);
				object->iterators[object->level].iterator = sub_iter;
				object->iterators[object->level].ce = ce;
				object->iterators[object->level].state = RS_START;
				if (object->level > 0
				 && object->iterators[object->level - 1].ce == 0) {
					object->iterators[object->level].haschildren =
						object->iterators[object->level - 1].haschildren;
					object->iterators[object->level].getchildren =
						object->iterators[object->level - 1].getchildren;
				} else {
					object->iterators[object->level].haschildren = NULL;
					object->iterators[object->level].getchildren = NULL;
				}
				if (sub_iter->funcs->rewind) {
					sub_iter->funcs->rewind(sub_iter);
				}
				if (object->beginChildren) {
					zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->beginChildren, "beginchildren", NULL);
					if (EG(exception)) {
						if (!(object->flags & RIT_CATCH_GET_CHILD)) {
							return;
						} else {
							zend_clear_exception();
						}
					}
				}
				goto next_step;
		}
		/* no more elements */
		if (object->level > 0) {
			if (object->endChildren) {
				zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->endChildren, "endchildren", NULL);
				if (EG(exception)) {
					if (!(object->flags & RIT_CATCH_GET_CHILD)) {
						return;
					} else {
						zend_clear_exception();
					}
				}
			}
			if (object->level > 0) {
				zval garbage;
				ZVAL_COPY_VALUE(&garbage, &object->iterators[object->level].zobject);
				ZVAL_UNDEF(&object->iterators[object->level].zobject);
				zval_ptr_dtor(&garbage);
				zend_iterator_dtor(iterator);
				object->level--;
			}
		} else {
			return; /* done completeley */
		}
	}
}

static void spl_recursive_it_rewind_ex(spl_recursive_it_object *object, zval *zthis)
{
	zend_object_iterator *sub_iter;

	SPL_FETCH_SUB_ITERATOR(sub_iter, object);

	while (object->level) {
		sub_iter = object->iterators[object->level].iterator;
		zend_iterator_dtor(sub_iter);
		zval_ptr_dtor(&object->iterators[object->level--].zobject);
		if (!EG(exception) && (!object->endChildren || object->endChildren->common.scope != spl_ce_RecursiveIteratorIterator)) {
			zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->endChildren, "endchildren", NULL);
		}
	}
	object->iterators = erealloc(object->iterators, sizeof(spl_sub_iterator));
	object->iterators[0].state = RS_START;
	sub_iter = object->iterators[0].iterator;
	if (sub_iter->funcs->rewind) {
		sub_iter->funcs->rewind(sub_iter);
	}
	if (!EG(exception) && object->beginIteration && !object->in_iteration) {
		zend_call_method_with_0_params(Z_OBJ_P(zthis), object->ce, &object->beginIteration, "beginIteration", NULL);
	}
	object->in_iteration = 1;
	spl_recursive_it_move_forward_ex(object, zthis);
}

static void spl_recursive_it_move_forward(zend_object_iterator *iter)
{
	spl_recursive_it_move_forward_ex(Z_SPLRECURSIVE_IT_P(&iter->data), &iter->data);
}

static void spl_recursive_it_rewind(zend_object_iterator *iter)
{
	spl_recursive_it_rewind_ex(Z_SPLRECURSIVE_IT_P(&iter->data), &iter->data);
}

static const zend_object_iterator_funcs spl_recursive_it_iterator_funcs = {
	spl_recursive_it_dtor,
	spl_recursive_it_valid,
	spl_recursive_it_get_current_data,
	spl_recursive_it_get_current_key,
	spl_recursive_it_move_forward,
	spl_recursive_it_rewind,
	NULL,
	NULL, /* get_gc */
};

static zend_object_iterator *spl_recursive_it_get_iterator(zend_class_entry *ce, zval *zobject, int by_ref)
{
	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	spl_recursive_it_object *object = Z_SPLRECURSIVE_IT_P(zobject);
	if (object->iterators == NULL) {
		zend_throw_error(NULL, "Object is not initialized");
		return NULL;
	}

	spl_recursive_it_iterator *iterator = emalloc(sizeof(spl_recursive_it_iterator));
	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(zobject));
	iterator->intern.funcs = &spl_recursive_it_iterator_funcs;
	return (zend_object_iterator*)iterator;
}

static int spl_get_iterator_from_aggregate(zval *retval, zend_class_entry *ce, zend_object *obj) {
	zend_function **getiterator_cache =
		ce->iterator_funcs_ptr ? &ce->iterator_funcs_ptr->zf_new_iterator : NULL;
	zend_call_method_with_0_params(obj, ce, getiterator_cache, "getiterator", retval);
	if (EG(exception)) {
		return FAILURE;
	}
	if (Z_TYPE_P(retval) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(retval), zend_ce_traversable)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0,
			"%s::getIterator() must return an object that implements Traversable",
			ZSTR_VAL(ce->name));
		zval_ptr_dtor(retval);
		return FAILURE;
	}
	return SUCCESS;
}

static void spl_recursive_it_it_construct(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_base, zend_class_entry *ce_inner, recursive_it_it_type rit_type)
{
	zval *object = ZEND_THIS;
	spl_recursive_it_object *intern;
	zval *iterator;
	zend_class_entry *ce_iterator;
	zend_long mode, flags;
	zval caching_it, aggregate_retval;

	switch (rit_type) {
		case RIT_RecursiveTreeIterator: {
			zval caching_it_flags;
			zend_long user_caching_it_flags = CIT_CATCH_GET_CHILD;
			mode = RIT_SELF_FIRST;
			flags = RTIT_BYPASS_KEY;

			if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|lll", &iterator, &flags, &user_caching_it_flags, &mode) == FAILURE) {
				RETURN_THROWS();
			}

			if (instanceof_function(Z_OBJCE_P(iterator), zend_ce_aggregate)) {
				if (spl_get_iterator_from_aggregate(
						&aggregate_retval, Z_OBJCE_P(iterator), Z_OBJ_P(iterator)) == FAILURE) {
					RETURN_THROWS();
				}
				iterator = &aggregate_retval;
			} else {
				Z_ADDREF_P(iterator);
			}

			ZVAL_LONG(&caching_it_flags, user_caching_it_flags);
			spl_instantiate_arg_ex2(spl_ce_RecursiveCachingIterator, &caching_it, iterator, &caching_it_flags);
			zval_ptr_dtor(&caching_it_flags);
			zval_ptr_dtor(iterator);
			iterator = &caching_it;
			break;
		}
		case RIT_RecursiveIteratorIterator:
		default: {
			mode = RIT_LEAVES_ONLY;
			flags = 0;
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|ll", &iterator, &mode, &flags) == FAILURE) {
				RETURN_THROWS();
			}

			if (instanceof_function(Z_OBJCE_P(iterator), zend_ce_aggregate)) {
				if (spl_get_iterator_from_aggregate(
						&aggregate_retval, Z_OBJCE_P(iterator), Z_OBJ_P(iterator)) == FAILURE) {
					RETURN_THROWS();
				}
				iterator = &aggregate_retval;
			} else {
				Z_ADDREF_P(iterator);
			}
			break;
		}
	}
	if (!instanceof_function(Z_OBJCE_P(iterator), spl_ce_RecursiveIterator)) {
		if (iterator) {
			zval_ptr_dtor(iterator);
		}
		zend_throw_exception(spl_ce_InvalidArgumentException, "An instance of RecursiveIterator or IteratorAggregate creating it is required", 0);
		return;
	}

	intern = Z_SPLRECURSIVE_IT_P(object);
	intern->iterators = emalloc(sizeof(spl_sub_iterator));
	intern->level = 0;
	intern->mode = mode;
	intern->flags = (int)flags;
	intern->max_depth = -1;
	intern->in_iteration = 0;
	intern->ce = Z_OBJCE_P(object);

	intern->beginIteration = zend_hash_str_find_ptr(&intern->ce->function_table, "beginiteration", sizeof("beginiteration") - 1);
	if (intern->beginIteration->common.scope == ce_base) {
		intern->beginIteration = NULL;
	}
	intern->endIteration = zend_hash_str_find_ptr(&intern->ce->function_table, "enditeration", sizeof("enditeration") - 1);
	if (intern->endIteration->common.scope == ce_base) {
		intern->endIteration = NULL;
	}
	intern->callHasChildren = zend_hash_str_find_ptr(&intern->ce->function_table, "callhaschildren", sizeof("callHasChildren") - 1);
	if (intern->callHasChildren->common.scope == ce_base) {
		intern->callHasChildren = NULL;
	}
	intern->callGetChildren = zend_hash_str_find_ptr(&intern->ce->function_table, "callgetchildren", sizeof("callGetChildren") - 1);
	if (intern->callGetChildren->common.scope == ce_base) {
		intern->callGetChildren = NULL;
	}
	intern->beginChildren = zend_hash_str_find_ptr(&intern->ce->function_table, "beginchildren", sizeof("beginchildren") - 1);
	if (intern->beginChildren->common.scope == ce_base) {
		intern->beginChildren = NULL;
	}
	intern->endChildren = zend_hash_str_find_ptr(&intern->ce->function_table, "endchildren", sizeof("endchildren") - 1);
	if (intern->endChildren->common.scope == ce_base) {
		intern->endChildren = NULL;
	}
	intern->nextElement = zend_hash_str_find_ptr(&intern->ce->function_table, "nextelement", sizeof("nextElement") - 1);
	if (intern->nextElement->common.scope == ce_base) {
		intern->nextElement = NULL;
	}

	ce_iterator = Z_OBJCE_P(iterator); /* respect inheritance, don't use spl_ce_RecursiveIterator */
	intern->iterators[0].iterator = ce_iterator->get_iterator(ce_iterator, iterator, 0);
	ZVAL_OBJ(&intern->iterators[0].zobject, Z_OBJ_P(iterator));
	intern->iterators[0].ce = ce_iterator;
	intern->iterators[0].state = RS_START;
	intern->iterators[0].haschildren = NULL;
	intern->iterators[0].getchildren = NULL;

	if (EG(exception)) {
		zend_object_iterator *sub_iter;

		while (intern->level >= 0) {
			sub_iter = intern->iterators[intern->level].iterator;
			zend_iterator_dtor(sub_iter);
			zval_ptr_dtor(&intern->iterators[intern->level--].zobject);
		}
		efree(intern->iterators);
		intern->iterators = NULL;
	}
}

/* {{{ Creates a RecursiveIteratorIterator from a RecursiveIterator. */
PHP_METHOD(RecursiveIteratorIterator, __construct)
{
	spl_recursive_it_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveIteratorIterator, zend_ce_iterator, RIT_RecursiveIteratorIterator);
} /* }}} */

/* {{{ Rewind the iterator to the first element of the top level inner iterator. */
PHP_METHOD(RecursiveIteratorIterator, rewind)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_recursive_it_rewind_ex(object, ZEND_THIS);
} /* }}} */

/* {{{ Check whether the current position is valid */
PHP_METHOD(RecursiveIteratorIterator, valid)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(spl_recursive_it_valid_ex(object, ZEND_THIS) == SUCCESS);
} /* }}} */

/* {{{ Access the current key */
PHP_METHOD(RecursiveIteratorIterator, key)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_object_iterator      *iterator;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_SUB_ITERATOR(iterator, object);

	if (iterator->funcs->get_current_key) {
		iterator->funcs->get_current_key(iterator, return_value);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ Access the current element value */
PHP_METHOD(RecursiveIteratorIterator, current)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_object_iterator      *iterator;
	zval                      *data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_SUB_ITERATOR(iterator, object);

	data = iterator->funcs->get_current_data(iterator);
	if (data) {
		RETURN_COPY_DEREF(data);
	}
} /* }}} */

/* {{{ Move forward to the next element */
PHP_METHOD(RecursiveIteratorIterator, next)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_recursive_it_move_forward_ex(object, ZEND_THIS);
} /* }}} */

/* {{{ Get the current depth of the recursive iteration */
PHP_METHOD(RecursiveIteratorIterator, getDepth)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(object->level);
} /* }}} */

/* {{{ The current active sub iterator or the iterator at specified level */
PHP_METHOD(RecursiveIteratorIterator, getSubIterator)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_long level;
	bool level_is_null = 1;
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &level, &level_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (level_is_null) {
		level = object->level;
	} else if (level < 0 || level > object->level) {
		RETURN_NULL();
	}

	if(!object->iterators) {
		zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called");
		RETURN_THROWS();
	}

	value = &object->iterators[level].zobject;
	RETURN_COPY_DEREF(value);
} /* }}} */

/* {{{ The current active sub iterator */
PHP_METHOD(RecursiveIteratorIterator, getInnerIterator)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zval      *zobject;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_SUB_ELEMENT_ADDR(zobject, object, zobject);

	RETURN_COPY_DEREF(zobject);
} /* }}} */

/* {{{ Called when iteration begins (after first rewind() call) */
PHP_METHOD(RecursiveIteratorIterator, beginIteration)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* nothing to do */
} /* }}} */

/* {{{ Called when iteration ends (when valid() first returns false */
PHP_METHOD(RecursiveIteratorIterator, endIteration)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* nothing to do */
} /* }}} */

/* {{{ Called for each element to test whether it has children */
PHP_METHOD(RecursiveIteratorIterator, callHasChildren)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_class_entry *ce;
	zval *zobject;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!object->iterators) {
		RETURN_FALSE;
	}

	SPL_FETCH_SUB_ELEMENT(ce, object, ce);

	zobject = &object->iterators[object->level].zobject;
	if (Z_TYPE_P(zobject) == IS_UNDEF) {
		RETURN_FALSE;
	} else {
		zend_call_method_with_0_params(Z_OBJ_P(zobject), ce, &object->iterators[object->level].haschildren, "haschildren", return_value);
		if (Z_TYPE_P(return_value) == IS_UNDEF) {
			RETURN_FALSE;
		}
	}
} /* }}} */

/* {{{ Return children of current element */
PHP_METHOD(RecursiveIteratorIterator, callGetChildren)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_class_entry *ce;
	zval *zobject;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_SUB_ELEMENT(ce, object, ce);

	zobject = &object->iterators[object->level].zobject;
	if (Z_TYPE_P(zobject) == IS_UNDEF) {
		RETURN_NULL();
	} else {
		zend_call_method_with_0_params(Z_OBJ_P(zobject), ce, &object->iterators[object->level].getchildren, "getchildren", return_value);
		if (Z_TYPE_P(return_value) == IS_UNDEF) {
			RETURN_NULL();
		}
	}
} /* }}} */

/* {{{ Called when recursing one level down */
PHP_METHOD(RecursiveIteratorIterator, beginChildren)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* nothing to do */
} /* }}} */

/* {{{ Called when end recursing one level */
PHP_METHOD(RecursiveIteratorIterator, endChildren)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* nothing to do */
} /* }}} */

/* {{{ Called when the next element is available */
PHP_METHOD(RecursiveIteratorIterator, nextElement)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* nothing to do */
} /* }}} */

/* {{{ Set the maximum allowed depth (or any depth if pmax_depth = -1] */
PHP_METHOD(RecursiveIteratorIterator, setMaxDepth)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_long  max_depth = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &max_depth) == FAILURE) {
		RETURN_THROWS();
	}
	if (max_depth < -1) {
		zend_argument_value_error(1, "must be greater than or equal to -1");
		RETURN_THROWS();
	} else if (max_depth > INT_MAX) {
		max_depth = INT_MAX;
	}

	object->max_depth = (int)max_depth;
} /* }}} */

/* {{{ Return the maximum accepted depth or false if any depth is allowed */
PHP_METHOD(RecursiveIteratorIterator, getMaxDepth)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (object->max_depth == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(object->max_depth);
	}
} /* }}} */

static zend_function *spl_recursive_it_get_method(zend_object **zobject, zend_string *method, const zval *key)
{
	zend_function           *function_handler;
	spl_recursive_it_object *object = spl_recursive_it_from_obj(*zobject);
	zend_long                     level = object->level;
	zval                    *zobj;

	if (!object->iterators) {
		zend_throw_error(NULL, "The %s instance wasn't initialized properly", ZSTR_VAL((*zobject)->ce->name));
		return NULL;
	}
	zobj = &object->iterators[level].zobject;

	function_handler = zend_std_get_method(zobject, method, key);
	if (!function_handler) {
		if ((function_handler = zend_hash_find_ptr(&Z_OBJCE_P(zobj)->function_table, method)) == NULL) {
			*zobject = Z_OBJ_P(zobj);
			function_handler = (*zobject)->handlers->get_method(zobject, method, key);
		} else {
			*zobject = Z_OBJ_P(zobj);
		}
	}
	return function_handler;
}

/* {{{ spl_RecursiveIteratorIterator_free_storage */
static void spl_RecursiveIteratorIterator_free_storage(zend_object *_object)
{
	spl_recursive_it_object *object = spl_recursive_it_from_obj(_object);

	if (object->iterators) {
		while (object->level >= 0) {
			zend_object_iterator *sub_iter = object->iterators[object->level].iterator;
			zend_iterator_dtor(sub_iter);
			zval_ptr_dtor(&object->iterators[object->level].zobject);
			object->level--;
		}
		efree(object->iterators);
		object->iterators = NULL;
	}

	zend_object_std_dtor(&object->std);
	for (size_t i = 0; i < 6; i++) {
		if (object->prefix[i]) {
			zend_string_release(object->prefix[i]);
		}
	}

	if (object->postfix[0]) {
		zend_string_release(object->postfix[0]);
	}
}
/* }}} */

static HashTable *spl_RecursiveIteratorIterator_get_gc(zend_object *obj, zval **table, int *n)
{
	spl_recursive_it_object *object = spl_recursive_it_from_obj(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	if (object->iterators) {
		for (int level = 0; level <= object->level; level++) {
			zend_get_gc_buffer_add_zval(gc_buffer, &object->iterators[level].zobject);
			zend_get_gc_buffer_add_obj(gc_buffer, &object->iterators[level].iterator->std);
		}
	}

	zend_get_gc_buffer_use(gc_buffer, table, n);
	return zend_std_get_properties(obj);
}

/* {{{ spl_RecursiveIteratorIterator_new_ex */
static zend_object *spl_RecursiveIteratorIterator_new_ex(zend_class_entry *class_type, int init_prefix)
{
	spl_recursive_it_object *intern;

	intern = zend_object_alloc(sizeof(spl_recursive_it_object), class_type);

	if (init_prefix) {
		intern->prefix[0] = ZSTR_EMPTY_ALLOC();
		intern->prefix[1] = zend_string_init("| ",  2, 0);
		intern->prefix[2] = zend_string_init("  ",  2, 0);
		intern->prefix[3] = zend_string_init("|-",  2, 0);
		intern->prefix[4] = zend_string_init("\\-", 2, 0);
		intern->prefix[5] = ZSTR_EMPTY_ALLOC();

		intern->postfix[0] = ZSTR_EMPTY_ALLOC();
	}

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = &spl_handlers_rec_it_it;
	return &intern->std;
}
/* }}} */

/* {{{ spl_RecursiveIteratorIterator_new */
static zend_object *spl_RecursiveIteratorIterator_new(zend_class_entry *class_type)
{
	return spl_RecursiveIteratorIterator_new_ex(class_type, 0);
}
/* }}} */

/* {{{ spl_RecursiveTreeIterator_new */
static zend_object *spl_RecursiveTreeIterator_new(zend_class_entry *class_type)
{
	return spl_RecursiveIteratorIterator_new_ex(class_type, 1);
}
/* }}} */

static zend_string *spl_recursive_tree_iterator_get_prefix(spl_recursive_it_object *object)
{
	smart_str  str = {0};
	zval       has_next;
	int        level;

	smart_str_append(&str, object->prefix[0]);

	for (level = 0; level < object->level; ++level) {
		zend_call_method_with_0_params(Z_OBJ(object->iterators[level].zobject), object->iterators[level].ce, NULL, "hasnext", &has_next);
		if (Z_TYPE(has_next) != IS_UNDEF) {
			if (Z_TYPE(has_next) == IS_TRUE) {
				smart_str_append(&str, object->prefix[1]);
			} else {
				smart_str_append(&str, object->prefix[2]);
			}
			zval_ptr_dtor(&has_next);
		}
	}
	zend_call_method_with_0_params(Z_OBJ(object->iterators[level].zobject), object->iterators[level].ce, NULL, "hasnext", &has_next);
	if (Z_TYPE(has_next) != IS_UNDEF) {
		if (Z_TYPE(has_next) == IS_TRUE) {
			smart_str_append(&str, object->prefix[3]);
		} else {
			smart_str_append(&str, object->prefix[4]);
		}
		zval_ptr_dtor(&has_next);
	}

	smart_str_append(&str, object->prefix[5]);
	smart_str_0(&str);

	return str.s;
}

static zend_string *spl_recursive_tree_iterator_get_entry(spl_recursive_it_object *object)
{
	zend_object_iterator *iterator = object->iterators[object->level].iterator;
	zval *data = iterator->funcs->get_current_data(iterator);
	if (!data) {
		return NULL;
	}

	ZVAL_DEREF(data);
	if (Z_TYPE_P(data) == IS_ARRAY) {
		/* TODO: Remove this special case? */
		return ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED);
	}
	return zval_get_string(data);
}

static zend_string *spl_recursive_tree_iterator_get_postfix(spl_recursive_it_object *object)
{
	return zend_string_copy(object->postfix[0]);
}

/* {{{ RecursiveIteratorIterator to generate ASCII graphic trees for the entries in a RecursiveIterator */
PHP_METHOD(RecursiveTreeIterator, __construct)
{
	spl_recursive_it_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveTreeIterator, zend_ce_iterator, RIT_RecursiveTreeIterator);
} /* }}} */

/* {{{ Sets prefix parts as used in getPrefix() */
PHP_METHOD(RecursiveTreeIterator, setPrefixPart)
{
	zend_long  part;
	zend_string *prefix;
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lS", &part, &prefix) == FAILURE) {
		RETURN_THROWS();
	}

	if (0 > part || part > 5) {
		zend_argument_value_error(1, "must be a RecursiveTreeIterator::PREFIX_* constant");
		RETURN_THROWS();
	}

	zend_string_release(object->prefix[part]);
	object->prefix[part] = zend_string_copy(prefix);
} /* }}} */

/* {{{ Returns the string to place in front of current element */
PHP_METHOD(RecursiveTreeIterator, getPrefix)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if(!object->iterators) {
		zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called");
		RETURN_THROWS();
	}

	RETURN_STR(spl_recursive_tree_iterator_get_prefix(object));
} /* }}} */

/* {{{ Sets postfix as used in getPostfix() */
PHP_METHOD(RecursiveTreeIterator, setPostfix)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_string *postfix;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &postfix) == FAILURE) {
		RETURN_THROWS();
	}

	zend_string_release(object->postfix[0]);
	object->postfix[0] = zend_string_copy(postfix);
} /* }}} */

/* {{{ Returns the string presentation built for current element */
PHP_METHOD(RecursiveTreeIterator, getEntry)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if(!object->iterators) {
		zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called");
		RETURN_THROWS();
	}

	zend_string *entry = spl_recursive_tree_iterator_get_entry(object);
	if (!entry) {
		// TODO: Can this happen? It's not in the stubs.
		RETURN_NULL();
	}
	RETURN_STR(entry);
} /* }}} */

/* {{{ Returns the string to place after the current element */
PHP_METHOD(RecursiveTreeIterator, getPostfix)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if(!object->iterators) {
		zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called");
		RETURN_THROWS();
	}

	RETURN_STR(spl_recursive_tree_iterator_get_postfix(object));
} /* }}} */

/* {{{ Returns the current element prefixed and postfixed */
PHP_METHOD(RecursiveTreeIterator, current)
{
	spl_recursive_it_object *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if(!object->iterators) {
		zend_throw_error(NULL, "The object is in an invalid state as the parent constructor was not called");
		RETURN_THROWS();
	}

	if (object->flags & RTIT_BYPASS_CURRENT) {
		zend_object_iterator      *iterator = object->iterators[object->level].iterator;
		zval                      *data;

		SPL_FETCH_SUB_ITERATOR(iterator, object);
		data = iterator->funcs->get_current_data(iterator);
		if (data) {
			RETURN_COPY_DEREF(data);
		} else {
			RETURN_NULL();
		}
	}

	zend_string *entry = spl_recursive_tree_iterator_get_entry(object);
	if (!entry) {
		RETURN_NULL();
	}

	zend_string *prefix = spl_recursive_tree_iterator_get_prefix(object);
	zend_string *postfix = spl_recursive_tree_iterator_get_postfix(object);

	zend_string *result = zend_string_concat3(
		ZSTR_VAL(prefix), ZSTR_LEN(prefix),
		ZSTR_VAL(entry), ZSTR_LEN(entry),
		ZSTR_VAL(postfix), ZSTR_LEN(postfix));

	zend_string_release(entry);
	zend_string_release(prefix);
	zend_string_release(postfix);

	RETURN_NEW_STR(result);
} /* }}} */

/* {{{ Returns the current key prefixed and postfixed */
PHP_METHOD(RecursiveTreeIterator, key)
{
	spl_recursive_it_object   *object = Z_SPLRECURSIVE_IT_P(ZEND_THIS);
	zend_object_iterator      *iterator;
	zval                       key;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_SUB_ITERATOR(iterator, object);

	if (iterator->funcs->get_current_key) {
		iterator->funcs->get_current_key(iterator, &key);
	} else {
		ZVAL_NULL(&key);
	}

	if (object->flags & RTIT_BYPASS_KEY) {
		RETURN_COPY_VALUE(&key);
	}

	zend_string *key_str = zval_get_string(&key);
	zend_string *prefix = spl_recursive_tree_iterator_get_prefix(object);
	zend_string *postfix = spl_recursive_tree_iterator_get_postfix(object);

	zend_string *result = zend_string_concat3(
		ZSTR_VAL(prefix), ZSTR_LEN(prefix),
		ZSTR_VAL(key_str), ZSTR_LEN(key_str),
		ZSTR_VAL(postfix), ZSTR_LEN(postfix));

	zend_string_release(key_str);
	zend_string_release(prefix);
	zend_string_release(postfix);
	zval_ptr_dtor(&key);

	RETURN_NEW_STR(result);
} /* }}} */

static zend_function *spl_dual_it_get_method(zend_object **object, zend_string *method, const zval *key)
{
	zend_function        *function_handler;
	spl_dual_it_object   *intern;

	intern = spl_dual_it_from_obj(*object);

	function_handler = zend_std_get_method(object, method, key);
	if (!function_handler && intern->inner.ce) {
		if ((function_handler = zend_hash_find_ptr(&intern->inner.ce->function_table, method)) == NULL) {
			if (Z_OBJ_HT(intern->inner.zobject)->get_method) {
				*object = Z_OBJ(intern->inner.zobject);
				function_handler = (*object)->handlers->get_method(object, method, key);
			}
		} else {
			*object = Z_OBJ(intern->inner.zobject);
		}
	}
	return function_handler;
}

#define SPL_CHECK_CTOR(intern, classname) \
	if (intern->dit_type == DIT_Unknown) { \
		/* TODO Normal Error? */ \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Classes derived from %s must call %s::__construct()", \
				ZSTR_VAL((spl_ce_##classname)->name), ZSTR_VAL((spl_ce_##classname)->name)); \
		RETURN_THROWS(); \
	}

#define APPENDIT_CHECK_CTOR(intern) SPL_CHECK_CTOR(intern, AppendIterator)

static inline int spl_dual_it_fetch(spl_dual_it_object *intern, int check_more);

static inline int spl_cit_check_flags(zend_long flags)
{
	zend_long cnt = 0;

	cnt += (flags & CIT_CALL_TOSTRING) ? 1 : 0;
	cnt += (flags & CIT_TOSTRING_USE_KEY) ? 1 : 0;
	cnt += (flags & CIT_TOSTRING_USE_CURRENT) ? 1 : 0;
	cnt += (flags & CIT_TOSTRING_USE_INNER) ? 1 : 0;

	return cnt <= 1 ? SUCCESS : FAILURE;
}

static spl_dual_it_object* spl_dual_it_construct(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_base, zend_class_entry *ce_inner, dual_it_type dit_type)
{
	zval                 *zobject, retval;
	spl_dual_it_object   *intern;
	zend_class_entry     *ce = NULL;
	int                   inc_refcount = 1;
	zend_error_handling   error_handling;

	intern = Z_SPLDUAL_IT_P(ZEND_THIS);

	if (intern->dit_type != DIT_Unknown) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s::getIterator() must be called exactly once per instance", ZSTR_VAL(ce_base->name));
		return NULL;
	}

	switch (dit_type) {
		case DIT_LimitIterator: {
			intern->u.limit.offset = 0; /* start at beginning */
			intern->u.limit.count = -1; /* get all */
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|ll", &zobject, ce_inner, &intern->u.limit.offset, &intern->u.limit.count) == FAILURE) {
				return NULL;
			}
			if (intern->u.limit.offset < 0) {
				zend_argument_value_error(2, "must be greater than or equal to 0");
				return NULL;
			}
			if (intern->u.limit.count < -1) {
				zend_argument_value_error(3, "must be greater than or equal to -1");
				return NULL;
			}
			break;
		}
		case DIT_CachingIterator:
		case DIT_RecursiveCachingIterator: {
			zend_long flags = CIT_CALL_TOSTRING;
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &zobject, ce_inner, &flags) == FAILURE) {
				return NULL;
			}
			if (spl_cit_check_flags(flags) != SUCCESS) {
				zend_argument_value_error(2, "must contain only one of CachingIterator::CALL_TOSTRING, "
					"CachingIterator::TOSTRING_USE_KEY, CachingIterator::TOSTRING_USE_CURRENT, "
					"or CachingIterator::TOSTRING_USE_INNER");
				return NULL;
			}
			intern->u.caching.flags |= flags & CIT_PUBLIC;
			array_init(&intern->u.caching.zcache);
			break;
		}
		case DIT_IteratorIterator: {
			zend_class_entry *ce_cast;
			zend_string *class_name = NULL;

			if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|S!", &zobject, ce_inner, &class_name) == FAILURE) {
				return NULL;
			}
			ce = Z_OBJCE_P(zobject);
			if (!instanceof_function(ce, zend_ce_iterator)) {
				if (class_name) {
					if (!(ce_cast = zend_lookup_class(class_name))
					|| !instanceof_function(ce, ce_cast)
					|| !ce_cast->get_iterator
					) {
						zend_throw_exception(spl_ce_LogicException, "Class to downcast to not found or not base class or does not implement Traversable", 0);
						return NULL;
					}
					ce = ce_cast;
				}
				if (instanceof_function(ce, zend_ce_aggregate)) {
					if (spl_get_iterator_from_aggregate(&retval, ce, Z_OBJ_P(zobject)) == FAILURE) {
						return NULL;
					}
					zobject = &retval;
					ce = Z_OBJCE_P(zobject);
					inc_refcount = 0;
				}
			}
			break;
		}
		case DIT_AppendIterator:
			if (zend_parse_parameters_none() == FAILURE) {
				return NULL;
			}
			intern->dit_type = DIT_AppendIterator;
			object_init_ex(&intern->u.append.zarrayit, spl_ce_ArrayIterator);
			zend_call_method_with_0_params(Z_OBJ(intern->u.append.zarrayit), spl_ce_ArrayIterator, &spl_ce_ArrayIterator->constructor, "__construct", NULL);
			intern->u.append.iterator = spl_ce_ArrayIterator->get_iterator(spl_ce_ArrayIterator, &intern->u.append.zarrayit, 0);
			return intern;
		case DIT_RegexIterator:
		case DIT_RecursiveRegexIterator: {
			zend_string *regex;
			zend_long mode = REGIT_MODE_MATCH;

			intern->u.regex.use_flags = ZEND_NUM_ARGS() >= 5;
			intern->u.regex.flags = 0;
			intern->u.regex.preg_flags = 0;
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS|lll", &zobject, ce_inner, &regex, &mode, &intern->u.regex.flags, &intern->u.regex.preg_flags) == FAILURE) {
				return NULL;
			}
			if (mode < 0 || mode >= REGIT_MODE_MAX) {
				zend_argument_value_error(3, "must be RegexIterator::MATCH, RegexIterator::GET_MATCH, "
					"RegexIterator::ALL_MATCHES, RegexIterator::SPLIT, or RegexIterator::REPLACE");
				return NULL;
			}

			/* pcre_get_compiled_regex_cache() might emit E_WARNINGs that we want to promote to exception */
			zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling);
			intern->u.regex.pce = pcre_get_compiled_regex_cache(regex);
			zend_restore_error_handling(&error_handling);

			if (intern->u.regex.pce == NULL) {
				/* pcre_get_compiled_regex_cache has already sent error */
				return NULL;
			}
			intern->u.regex.mode = mode;
			intern->u.regex.regex = zend_string_copy(regex);
			php_pcre_pce_incref(intern->u.regex.pce);
			break;
		}
		case DIT_CallbackFilterIterator:
		case DIT_RecursiveCallbackFilterIterator: {
			_spl_cbfilter_it_intern *cfi = emalloc(sizeof(*cfi));
			cfi->fci.object = NULL;
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "Of", &zobject, ce_inner, &cfi->fci, &cfi->fcc) == FAILURE) {
				efree(cfi);
				return NULL;
			}
			Z_TRY_ADDREF(cfi->fci.function_name);
			cfi->object = cfi->fcc.object;
			if (cfi->object) GC_ADDREF(cfi->object);
			intern->u.cbfilter = cfi;
			break;
		}
		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zobject, ce_inner) == FAILURE) {
				return NULL;
			}
			break;
	}

	intern->dit_type = dit_type;
	if (inc_refcount) {
		Z_ADDREF_P(zobject);
	}
	ZVAL_OBJ(&intern->inner.zobject, Z_OBJ_P(zobject));

	intern->inner.ce = dit_type == DIT_IteratorIterator ? ce : Z_OBJCE_P(zobject);
	intern->inner.object = Z_OBJ_P(zobject);
	intern->inner.iterator = intern->inner.ce->get_iterator(intern->inner.ce, zobject, 0);

	return intern;
}

/* {{{ Create an Iterator from another iterator */
PHP_METHOD(FilterIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_FilterIterator, zend_ce_iterator, DIT_FilterIterator);
} /* }}} */

/* {{{ Create an Iterator from another iterator */
PHP_METHOD(CallbackFilterIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_CallbackFilterIterator, zend_ce_iterator, DIT_CallbackFilterIterator);
} /* }}} */

/* {{{ Get the inner iterator */
PHP_METHOD(IteratorIterator, getInnerIterator)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!Z_ISUNDEF(intern->inner.zobject)) {
		zval *value = &intern->inner.zobject;
		RETURN_COPY_DEREF(value);
	} else {
		RETURN_NULL();
	}
} /* }}} */

static inline void spl_dual_it_free(spl_dual_it_object *intern)
{
	if (intern->inner.iterator && intern->inner.iterator->funcs->invalidate_current) {
		intern->inner.iterator->funcs->invalidate_current(intern->inner.iterator);
	}
	if (Z_TYPE(intern->current.data) != IS_UNDEF) {
		zval_ptr_dtor(&intern->current.data);
		ZVAL_UNDEF(&intern->current.data);
	}
	if (Z_TYPE(intern->current.key) != IS_UNDEF) {
		zval_ptr_dtor(&intern->current.key);
		ZVAL_UNDEF(&intern->current.key);
	}
	if (intern->dit_type == DIT_CachingIterator || intern->dit_type == DIT_RecursiveCachingIterator) {
		if (intern->u.caching.zstr) {
			zend_string_release(intern->u.caching.zstr);
			intern->u.caching.zstr = NULL;
		}
		if (Z_TYPE(intern->u.caching.zchildren) != IS_UNDEF) {
			zval_ptr_dtor(&intern->u.caching.zchildren);
			ZVAL_UNDEF(&intern->u.caching.zchildren);
		}
	}
}

static inline void spl_dual_it_rewind(spl_dual_it_object *intern)
{
	spl_dual_it_free(intern);
	intern->current.pos = 0;
	if (intern->inner.iterator && intern->inner.iterator->funcs->rewind) {
		intern->inner.iterator->funcs->rewind(intern->inner.iterator);
	}
}

static inline int spl_dual_it_valid(spl_dual_it_object *intern)
{
	if (!intern->inner.iterator) {
		return FAILURE;
	}
	/* FAILURE / SUCCESS */
	return intern->inner.iterator->funcs->valid(intern->inner.iterator);
}

static inline int spl_dual_it_fetch(spl_dual_it_object *intern, int check_more)
{
	zval *data;

	spl_dual_it_free(intern);
	if (!check_more || spl_dual_it_valid(intern) == SUCCESS) {
		data = intern->inner.iterator->funcs->get_current_data(intern->inner.iterator);
		if (data) {
			ZVAL_COPY(&intern->current.data, data);
		}

		if (intern->inner.iterator->funcs->get_current_key) {
			intern->inner.iterator->funcs->get_current_key(intern->inner.iterator, &intern->current.key);
			if (EG(exception)) {
				zval_ptr_dtor(&intern->current.key);
				ZVAL_UNDEF(&intern->current.key);
			}
		} else {
			ZVAL_LONG(&intern->current.key, intern->current.pos);
		}
		return EG(exception) ? FAILURE : SUCCESS;
	}
	return FAILURE;
}

static inline void spl_dual_it_next(spl_dual_it_object *intern, int do_free)
{
	if (do_free) {
		spl_dual_it_free(intern);
	} else if (!intern->inner.iterator) {
		zend_throw_error(NULL, "The inner constructor wasn't initialized with an iterator instance");
		return;
	}
	intern->inner.iterator->funcs->move_forward(intern->inner.iterator);
	intern->current.pos++;
}

/* {{{ Rewind the iterator */
PHP_METHOD(IteratorIterator, rewind)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_dual_it_rewind(intern);
	spl_dual_it_fetch(intern, 1);
} /* }}} */

/* {{{ Check whether the current element is valid */
PHP_METHOD(IteratorIterator, valid)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_BOOL(Z_TYPE(intern->current.data) != IS_UNDEF);
} /* }}} */

/* {{{ Get the current key */
PHP_METHOD(IteratorIterator, key)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (Z_TYPE(intern->current.key) != IS_UNDEF) {
		RETURN_COPY_DEREF(&intern->current.key);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ Get the current element value */
PHP_METHOD(IteratorIterator, current)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (Z_TYPE(intern->current.data) != IS_UNDEF) {
		RETURN_COPY_DEREF(&intern->current.data);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ Move the iterator forward */
PHP_METHOD(IteratorIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_dual_it_next(intern, 1);
	spl_dual_it_fetch(intern, 1);
} /* }}} */

static inline void spl_filter_it_fetch(zval *zthis, spl_dual_it_object *intern)
{
	zval retval;

	while (spl_dual_it_fetch(intern, 1) == SUCCESS) {
		zend_call_method_with_0_params(Z_OBJ_P(zthis), intern->std.ce, NULL, "accept", &retval);
		if (Z_TYPE(retval) != IS_UNDEF) {
			if (zend_is_true(&retval)) {
				zval_ptr_dtor(&retval);
				return;
			}
			zval_ptr_dtor(&retval);
		}
		if (EG(exception)) {
			return;
		}
		intern->inner.iterator->funcs->move_forward(intern->inner.iterator);
	}
	spl_dual_it_free(intern);
}

static inline void spl_filter_it_rewind(zval *zthis, spl_dual_it_object *intern)
{
	spl_dual_it_rewind(intern);
	spl_filter_it_fetch(zthis, intern);
}

static inline void spl_filter_it_next(zval *zthis, spl_dual_it_object *intern)
{
	spl_dual_it_next(intern, 1);
	spl_filter_it_fetch(zthis, intern);
}

/* {{{ Rewind the iterator */
PHP_METHOD(FilterIterator, rewind)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	spl_filter_it_rewind(ZEND_THIS, intern);
} /* }}} */

/* {{{ Move the iterator forward */
PHP_METHOD(FilterIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	spl_filter_it_next(ZEND_THIS, intern);
} /* }}} */

/* {{{ Create a RecursiveCallbackFilterIterator from a RecursiveIterator */
PHP_METHOD(RecursiveCallbackFilterIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveCallbackFilterIterator, spl_ce_RecursiveIterator, DIT_RecursiveCallbackFilterIterator);
} /* }}} */


/* {{{ Create a RecursiveFilterIterator from a RecursiveIterator */
PHP_METHOD(RecursiveFilterIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveFilterIterator, spl_ce_RecursiveIterator, DIT_RecursiveFilterIterator);
} /* }}} */

/* {{{ Check whether the inner iterator's current element has children */
PHP_METHOD(RecursiveFilterIterator, hasChildren)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	zend_call_method_with_0_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "haschildren", return_value);
} /* }}} */

/* {{{ Return the inner iterator's children contained in a RecursiveFilterIterator */
PHP_METHOD(RecursiveFilterIterator, getChildren)
{
	spl_dual_it_object   *intern;
	zval                  retval;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	zend_call_method_with_0_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "getchildren", &retval);
	if (!EG(exception) && Z_TYPE(retval) != IS_UNDEF) {
		spl_instantiate_arg_ex1(Z_OBJCE_P(ZEND_THIS), return_value, &retval);
	}
	zval_ptr_dtor(&retval);
} /* }}} */

/* {{{ Return the inner iterator's children contained in a RecursiveCallbackFilterIterator */
PHP_METHOD(RecursiveCallbackFilterIterator, getChildren)
{
	spl_dual_it_object   *intern;
	zval                  retval;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	zend_call_method_with_0_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "getchildren", &retval);
	if (!EG(exception) && Z_TYPE(retval) != IS_UNDEF) {
		spl_instantiate_arg_ex2(Z_OBJCE_P(ZEND_THIS), return_value, &retval, &intern->u.cbfilter->fci.function_name);
	}
	zval_ptr_dtor(&retval);
} /* }}} */
/* {{{ Create a ParentIterator from a RecursiveIterator */
PHP_METHOD(ParentIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_ParentIterator, spl_ce_RecursiveIterator, DIT_ParentIterator);
} /* }}} */

/* {{{ Create an RegexIterator from another iterator and a regular expression */
PHP_METHOD(RegexIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RegexIterator, zend_ce_iterator, DIT_RegexIterator);
} /* }}} */

/* {{{ Calls the callback with the current value, the current key and the inner iterator as arguments */
PHP_METHOD(CallbackFilterIterator, accept)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (Z_TYPE(intern->current.data) == IS_UNDEF || Z_TYPE(intern->current.key) == IS_UNDEF) {
		RETURN_FALSE;
	}

	zval params[3];
	ZVAL_COPY_VALUE(&params[0], &intern->current.data);
	ZVAL_COPY_VALUE(&params[1], &intern->current.key);
	ZVAL_COPY_VALUE(&params[2], &intern->inner.zobject);

	zend_fcall_info *fci = &intern->u.cbfilter->fci;
	zend_fcall_info_cache *fcc = &intern->u.cbfilter->fcc;
	fci->retval = return_value;
	fci->param_count = 3;
	fci->params = params;

	if (zend_call_function(fci, fcc) != SUCCESS || Z_ISUNDEF_P(return_value)) {
		RETURN_FALSE;
	} else if (Z_ISREF_P(return_value)) {
		zend_unwrap_reference(return_value);
	}
}
/* }}} */

/* {{{ Match (string)current() against regular expression */
PHP_METHOD(RegexIterator, accept)
{
	spl_dual_it_object *intern;
	zend_string *result, *subject;
	size_t count = 0;
	zval zcount, rv;
	pcre2_match_data *match_data;
	pcre2_code *re;
	int rc;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (Z_TYPE(intern->current.data) == IS_UNDEF) {
		RETURN_FALSE;
	}

	if (intern->u.regex.flags & REGIT_USE_KEY) {
		subject = zval_get_string(&intern->current.key);
	} else {
		if (Z_TYPE(intern->current.data) == IS_ARRAY) {
			RETURN_FALSE;
		}
		subject = zval_get_string(&intern->current.data);
	}

	/* Exception during string conversion. */
	if (EG(exception)) {
		RETURN_THROWS();
	}

	switch (intern->u.regex.mode)
	{
		case REGIT_MODE_MAX: /* won't happen but makes compiler happy */
		case REGIT_MODE_MATCH:
			re = php_pcre_pce_re(intern->u.regex.pce);
			match_data = php_pcre_create_match_data(0, re);
			if (!match_data) {
				RETURN_FALSE;
			}
			rc = pcre2_match(re, (PCRE2_SPTR)ZSTR_VAL(subject), ZSTR_LEN(subject), 0, 0, match_data, php_pcre_mctx());
			RETVAL_BOOL(rc >= 0);
			php_pcre_free_match_data(match_data);
			break;

		case REGIT_MODE_ALL_MATCHES:
		case REGIT_MODE_GET_MATCH:
			zval_ptr_dtor(&intern->current.data);
			ZVAL_UNDEF(&intern->current.data);
			php_pcre_match_impl(intern->u.regex.pce, subject, &zcount,
				&intern->current.data, intern->u.regex.mode == REGIT_MODE_ALL_MATCHES, intern->u.regex.use_flags, intern->u.regex.preg_flags, 0);
			RETVAL_BOOL(Z_LVAL(zcount) > 0);
			break;

		case REGIT_MODE_SPLIT:
			zval_ptr_dtor(&intern->current.data);
			ZVAL_UNDEF(&intern->current.data);
			php_pcre_split_impl(intern->u.regex.pce, subject, &intern->current.data, -1, intern->u.regex.preg_flags);
			count = zend_hash_num_elements(Z_ARRVAL(intern->current.data));
			RETVAL_BOOL(count > 1);
			break;

		case REGIT_MODE_REPLACE: {
			zval *replacement = zend_read_property(intern->std.ce, Z_OBJ_P(ZEND_THIS), "replacement", sizeof("replacement")-1, 1, &rv);
			zend_string *replacement_str = zval_try_get_string(replacement);

			if (UNEXPECTED(!replacement_str)) {
				RETURN_THROWS();
			}

			result = php_pcre_replace_impl(intern->u.regex.pce, subject, ZSTR_VAL(subject), ZSTR_LEN(subject), replacement_str, -1, &count);

			if (intern->u.regex.flags & REGIT_USE_KEY) {
				zval_ptr_dtor(&intern->current.key);
				ZVAL_STR(&intern->current.key, result);
			} else {
				zval_ptr_dtor(&intern->current.data);
				ZVAL_STR(&intern->current.data, result);
			}

			zend_string_release(replacement_str);
			RETVAL_BOOL(count > 0);
		}
	}

	if (intern->u.regex.flags & REGIT_INVERTED) {
		RETVAL_BOOL(Z_TYPE_P(return_value) != IS_TRUE);
	}
	zend_string_release_ex(subject, 0);
} /* }}} */

/* {{{ Returns current regular expression */
PHP_METHOD(RegexIterator, getRegex)
{
	spl_dual_it_object *intern = Z_SPLDUAL_IT_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_STR_COPY(intern->u.regex.regex);
} /* }}} */

/* {{{ Returns current operation mode */
PHP_METHOD(RegexIterator, getMode)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_LONG(intern->u.regex.mode);
} /* }}} */

/* {{{ Set new operation mode */
PHP_METHOD(RegexIterator, setMode)
{
	spl_dual_it_object *intern;
	zend_long mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &mode) == FAILURE) {
		RETURN_THROWS();
	}

	if (mode < 0 || mode >= REGIT_MODE_MAX) {
		zend_argument_value_error(1, "must be RegexIterator::MATCH, RegexIterator::GET_MATCH, "
			"RegexIterator::ALL_MATCHES, RegexIterator::SPLIT, or RegexIterator::REPLACE");
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	intern->u.regex.mode = mode;
} /* }}} */

/* {{{ Returns current operation flags */
PHP_METHOD(RegexIterator, getFlags)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_LONG(intern->u.regex.flags);
} /* }}} */

/* {{{ Set operation flags */
PHP_METHOD(RegexIterator, setFlags)
{
	spl_dual_it_object *intern;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	intern->u.regex.flags = flags;
} /* }}} */

/* {{{ Returns current PREG flags (if in use or NULL) */
PHP_METHOD(RegexIterator, getPregFlags)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (intern->u.regex.use_flags) {
		RETURN_LONG(intern->u.regex.preg_flags);
	} else {
		RETURN_LONG(0);
	}
} /* }}} */

/* {{{ Set PREG flags */
PHP_METHOD(RegexIterator, setPregFlags)
{
	spl_dual_it_object *intern;
	zend_long preg_flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &preg_flags) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	intern->u.regex.preg_flags = preg_flags;
	intern->u.regex.use_flags = 1;
} /* }}} */

/* {{{ Create an RecursiveRegexIterator from another recursive iterator and a regular expression */
PHP_METHOD(RecursiveRegexIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveRegexIterator, spl_ce_RecursiveIterator, DIT_RecursiveRegexIterator);
} /* }}} */

/* {{{ Return the inner iterator's children contained in a RecursiveRegexIterator */
PHP_METHOD(RecursiveRegexIterator, getChildren)
{
	spl_dual_it_object   *intern;
	zval                 retval;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	zend_call_method_with_0_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "getchildren", &retval);
	if (!EG(exception)) {
		zval args[5];

		ZVAL_COPY(&args[0], &retval);
		ZVAL_STR_COPY(&args[1], intern->u.regex.regex);
		ZVAL_LONG(&args[2], intern->u.regex.mode);
		ZVAL_LONG(&args[3], intern->u.regex.flags);
		ZVAL_LONG(&args[4], intern->u.regex.preg_flags);

		spl_instantiate_arg_n(Z_OBJCE_P(ZEND_THIS), return_value, 5, args);

		zval_ptr_dtor(&args[0]);
		zval_ptr_dtor(&args[1]);
	}
	zval_ptr_dtor(&retval);
} /* }}} */

PHP_METHOD(RecursiveRegexIterator, accept)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (Z_TYPE(intern->current.data) == IS_UNDEF) {
		RETURN_FALSE;
	} else if (Z_TYPE(intern->current.data) == IS_ARRAY) {
		RETURN_BOOL(zend_hash_num_elements(Z_ARRVAL(intern->current.data)) > 0);
	}

	zend_call_method_with_0_params(Z_OBJ_P(ZEND_THIS), spl_ce_RegexIterator, NULL, "accept", return_value);
}

/* {{{ spl_dual_it_free_storage */
static void spl_dual_it_free_storage(zend_object *_object)
{
	spl_dual_it_object *object = spl_dual_it_from_obj(_object);

	spl_dual_it_free(object);

	if (object->inner.iterator) {
		zend_iterator_dtor(object->inner.iterator);
	}

	if (!Z_ISUNDEF(object->inner.zobject)) {
		zval_ptr_dtor(&object->inner.zobject);
	}

	if (object->dit_type == DIT_AppendIterator) {
		zend_iterator_dtor(object->u.append.iterator);
		if (Z_TYPE(object->u.append.zarrayit) != IS_UNDEF) {
			zval_ptr_dtor(&object->u.append.zarrayit);
		}
	}

	if (object->dit_type == DIT_CachingIterator || object->dit_type == DIT_RecursiveCachingIterator) {
		zval_ptr_dtor(&object->u.caching.zcache);
	}

	if (object->dit_type == DIT_RegexIterator || object->dit_type == DIT_RecursiveRegexIterator) {
		if (object->u.regex.pce) {
			php_pcre_pce_decref(object->u.regex.pce);
		}
		if (object->u.regex.regex) {
			zend_string_release_ex(object->u.regex.regex, 0);
		}
	}

	if (object->dit_type == DIT_CallbackFilterIterator || object->dit_type == DIT_RecursiveCallbackFilterIterator) {
		if (object->u.cbfilter) {
			_spl_cbfilter_it_intern *cbfilter = object->u.cbfilter;
			object->u.cbfilter = NULL;
			zval_ptr_dtor(&cbfilter->fci.function_name);
			if (cbfilter->fci.object) {
				OBJ_RELEASE(cbfilter->fci.object);
			}
			efree(cbfilter);
		}
	}

	zend_object_std_dtor(&object->std);
}
/* }}} */

static HashTable *spl_dual_it_get_gc(zend_object *obj, zval **table, int *n)
{
	spl_dual_it_object *object = spl_dual_it_from_obj(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	if (object->inner.iterator) {
		zend_get_gc_buffer_add_obj(gc_buffer, &object->inner.iterator->std);
	}

	zend_get_gc_buffer_add_zval(gc_buffer, &object->current.data);
	zend_get_gc_buffer_add_zval(gc_buffer, &object->current.key);
	zend_get_gc_buffer_add_zval(gc_buffer, &object->inner.zobject);

	switch (object->dit_type) {
		case DIT_Unknown:
		case DIT_Default:
		case DIT_IteratorIterator:
		case DIT_NoRewindIterator:
		case DIT_InfiniteIterator:
		case DIT_LimitIterator:
		case DIT_RegexIterator:
		case DIT_RecursiveRegexIterator:
			/* Nothing to do */
			break;
		case DIT_AppendIterator:
			zend_get_gc_buffer_add_obj(gc_buffer, &object->u.append.iterator->std);
			if (Z_TYPE(object->u.append.zarrayit) != IS_UNDEF) {
				zend_get_gc_buffer_add_zval(gc_buffer, &object->u.append.zarrayit);
			}
			break;
		case DIT_CachingIterator:
		case DIT_RecursiveCachingIterator:
			zend_get_gc_buffer_add_zval(gc_buffer, &object->u.caching.zcache);
			zend_get_gc_buffer_add_zval(gc_buffer, &object->u.caching.zchildren);
			break;
		case DIT_CallbackFilterIterator:
		case DIT_RecursiveCallbackFilterIterator:
			if (object->u.cbfilter) {
				zend_get_gc_buffer_add_zval(gc_buffer, &object->u.cbfilter->fci.function_name);
				if (object->u.cbfilter->fci.object) {
					zend_get_gc_buffer_add_obj(gc_buffer, object->u.cbfilter->fci.object);
				}
			}
			break;
	}

	zend_get_gc_buffer_use(gc_buffer, table, n);
	return zend_std_get_properties(obj);
}

/* {{{ spl_dual_it_new */
static zend_object *spl_dual_it_new(zend_class_entry *class_type)
{
	spl_dual_it_object *intern;

	intern = zend_object_alloc(sizeof(spl_dual_it_object), class_type);
	intern->dit_type = DIT_Unknown;

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = &spl_handlers_dual_it;
	return &intern->std;
}
/* }}} */

static inline int spl_limit_it_valid(spl_dual_it_object *intern)
{
	/* FAILURE / SUCCESS */
	if (intern->u.limit.count != -1 && intern->current.pos >= intern->u.limit.offset + intern->u.limit.count) {
		return FAILURE;
	} else {
		return spl_dual_it_valid(intern);
	}
}

static inline void spl_limit_it_seek(spl_dual_it_object *intern, zend_long pos)
{
	zval  zpos;

	spl_dual_it_free(intern);
	if (pos < intern->u.limit.offset) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Cannot seek to " ZEND_LONG_FMT " which is below the offset " ZEND_LONG_FMT, pos, intern->u.limit.offset);
		return;
	}
	if (pos >= intern->u.limit.offset + intern->u.limit.count && intern->u.limit.count != -1) {
		zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Cannot seek to " ZEND_LONG_FMT " which is behind offset " ZEND_LONG_FMT " plus count " ZEND_LONG_FMT, pos, intern->u.limit.offset, intern->u.limit.count);
		return;
	}
	if (pos != intern->current.pos && instanceof_function(intern->inner.ce, spl_ce_SeekableIterator)) {
		ZVAL_LONG(&zpos, pos);
		spl_dual_it_free(intern);
		zend_call_method_with_1_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "seek", NULL, &zpos);
		if (!EG(exception)) {
			intern->current.pos = pos;
			if (spl_limit_it_valid(intern) == SUCCESS) {
				spl_dual_it_fetch(intern, 0);
			}
		}
	} else {
		/* emulate the forward seek, by next() calls */
		/* a back ward seek is done by a previous rewind() */
		if (pos < intern->current.pos) {
			spl_dual_it_rewind(intern);
		}
		while (pos > intern->current.pos && spl_dual_it_valid(intern) == SUCCESS) {
			spl_dual_it_next(intern, 1);
		}
		if (spl_dual_it_valid(intern) == SUCCESS) {
			spl_dual_it_fetch(intern, 1);
		}
	}
}

/* {{{ Construct a LimitIterator from an Iterator with a given starting offset and optionally a maximum count */
PHP_METHOD(LimitIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_LimitIterator, zend_ce_iterator, DIT_LimitIterator);
} /* }}} */

/* {{{ Rewind the iterator to the specified starting offset */
PHP_METHOD(LimitIterator, rewind)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	spl_dual_it_rewind(intern);
	spl_limit_it_seek(intern, intern->u.limit.offset);
} /* }}} */

/* {{{ Check whether the current element is valid */
PHP_METHOD(LimitIterator, valid)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

/*	RETURN_BOOL(spl_limit_it_valid(intern) == SUCCESS);*/
	RETURN_BOOL((intern->u.limit.count == -1 || intern->current.pos < intern->u.limit.offset + intern->u.limit.count) && Z_TYPE(intern->current.data) != IS_UNDEF);
} /* }}} */

/* {{{ Move the iterator forward */
PHP_METHOD(LimitIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_dual_it_next(intern, 1);
	if (intern->u.limit.count == -1 || intern->current.pos < intern->u.limit.offset + intern->u.limit.count) {
		spl_dual_it_fetch(intern, 1);
	}
} /* }}} */

/* {{{ Seek to the given position */
PHP_METHOD(LimitIterator, seek)
{
	spl_dual_it_object   *intern;
	zend_long                 pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &pos) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	spl_limit_it_seek(intern, pos);
	RETURN_LONG(intern->current.pos);
} /* }}} */

/* {{{ Return the current position */
PHP_METHOD(LimitIterator, getPosition)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	RETURN_LONG(intern->current.pos);
} /* }}} */

static inline int spl_caching_it_valid(spl_dual_it_object *intern)
{
	return intern->u.caching.flags & CIT_VALID ? SUCCESS : FAILURE;
}

static inline int spl_caching_it_has_next(spl_dual_it_object *intern)
{
	return spl_dual_it_valid(intern);
}

static inline void spl_caching_it_next(spl_dual_it_object *intern)
{
	if (spl_dual_it_fetch(intern, 1) == SUCCESS) {
		intern->u.caching.flags |= CIT_VALID;
		/* Full cache ? */
		if (intern->u.caching.flags & CIT_FULL_CACHE) {
			zval *key = &intern->current.key;
			zval *data = &intern->current.data;

			ZVAL_DEREF(data);
			array_set_zval_key(Z_ARRVAL(intern->u.caching.zcache), key, data);
		}
		/* Recursion ? */
		if (intern->dit_type == DIT_RecursiveCachingIterator) {
			zval retval, zchildren, zflags;
			zend_call_method_with_0_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "haschildren", &retval);
			if (EG(exception)) {
				zval_ptr_dtor(&retval);
				if (intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
					zend_clear_exception();
				} else {
					return;
				}
			} else {
				if (zend_is_true(&retval)) {
					zend_call_method_with_0_params(Z_OBJ(intern->inner.zobject), intern->inner.ce, NULL, "getchildren", &zchildren);
					if (EG(exception)) {
						zval_ptr_dtor(&zchildren);
						if (intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
							zend_clear_exception();
						} else {
							zval_ptr_dtor(&retval);
							return;
						}
					} else {
						ZVAL_LONG(&zflags, intern->u.caching.flags & CIT_PUBLIC);
						spl_instantiate_arg_ex2(spl_ce_RecursiveCachingIterator, &intern->u.caching.zchildren, &zchildren, &zflags);
						zval_ptr_dtor(&zchildren);
					}
				}
				zval_ptr_dtor(&retval);
				if (EG(exception)) {
					if (intern->u.caching.flags & CIT_CATCH_GET_CHILD) {
						zend_clear_exception();
					} else {
						return;
					}
				}
			}
		}
		if (intern->u.caching.flags & (CIT_TOSTRING_USE_INNER|CIT_CALL_TOSTRING)) {
			if (intern->u.caching.flags & CIT_TOSTRING_USE_INNER) {
				intern->u.caching.zstr = zval_get_string(&intern->inner.zobject);
			} else {
				intern->u.caching.zstr = zval_get_string(&intern->current.data);
			}
		}
		spl_dual_it_next(intern, 0);
	} else {
		intern->u.caching.flags &= ~CIT_VALID;
	}
}

static inline void spl_caching_it_rewind(spl_dual_it_object *intern)
{
	spl_dual_it_rewind(intern);
	zend_hash_clean(Z_ARRVAL(intern->u.caching.zcache));
	spl_caching_it_next(intern);
}

/* {{{ Construct a CachingIterator from an Iterator */
PHP_METHOD(CachingIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_CachingIterator, zend_ce_iterator, DIT_CachingIterator);
} /* }}} */

/* {{{ Rewind the iterator */
PHP_METHOD(CachingIterator, rewind)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_caching_it_rewind(intern);
} /* }}} */

/* {{{ Check whether the current element is valid */
PHP_METHOD(CachingIterator, valid)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_BOOL(spl_caching_it_valid(intern) == SUCCESS);
} /* }}} */

/* {{{ Move the iterator forward */
PHP_METHOD(CachingIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_caching_it_next(intern);
} /* }}} */

/* {{{ Check whether the inner iterator has a valid next element */
PHP_METHOD(CachingIterator, hasNext)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_BOOL(spl_caching_it_has_next(intern) == SUCCESS);
} /* }}} */

/* {{{ Return the string representation of the current element */
PHP_METHOD(CachingIterator, __toString)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!(intern->u.caching.flags & (CIT_CALL_TOSTRING|CIT_TOSTRING_USE_KEY|CIT_TOSTRING_USE_CURRENT|CIT_TOSTRING_USE_INNER)))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not fetch string value (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	if (intern->u.caching.flags & CIT_TOSTRING_USE_KEY) {
		ZVAL_COPY(return_value, &intern->current.key);
		convert_to_string(return_value);
		return;
	} else if (intern->u.caching.flags & CIT_TOSTRING_USE_CURRENT) {
		ZVAL_COPY(return_value, &intern->current.data);
		convert_to_string(return_value);
		return;
	}
	if (intern->u.caching.zstr) {
		RETURN_STR_COPY(intern->u.caching.zstr);
	} else {
		RETURN_EMPTY_STRING();
	}
} /* }}} */

/* {{{ Set given index in cache */
PHP_METHOD(CachingIterator, offsetSet)
{
	spl_dual_it_object   *intern;
	zend_string *key;
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &key, &value) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not use a full cache (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	Z_TRY_ADDREF_P(value);
	zend_symtable_update(Z_ARRVAL(intern->u.caching.zcache), key, value);
}
/* }}} */

/* {{{ Return the internal cache if used */
PHP_METHOD(CachingIterator, offsetGet)
{
	spl_dual_it_object   *intern;
	zend_string *key;
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not use a full cache (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	if ((value = zend_symtable_find(Z_ARRVAL(intern->u.caching.zcache), key)) == NULL) {
		zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(key));
		return;
	}

	RETURN_COPY_DEREF(value);
}
/* }}} */

/* {{{ Unset given index in cache */
PHP_METHOD(CachingIterator, offsetUnset)
{
	spl_dual_it_object   *intern;
	zend_string *key;

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not use a full cache (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	zend_symtable_del(Z_ARRVAL(intern->u.caching.zcache), key);
}
/* }}} */

/* {{{ Return whether the requested index exists */
PHP_METHOD(CachingIterator, offsetExists)
{
	spl_dual_it_object   *intern;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not use a full cache (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	RETURN_BOOL(zend_symtable_exists(Z_ARRVAL(intern->u.caching.zcache), key));
}
/* }}} */

/* {{{ Return the cache */
PHP_METHOD(CachingIterator, getCache)
{
	spl_dual_it_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not use a full cache (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	ZVAL_COPY(return_value, &intern->u.caching.zcache);
}
/* }}} */

/* {{{ Return the internal flags */
PHP_METHOD(CachingIterator, getFlags)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_LONG(intern->u.caching.flags);
}
/* }}} */

/* {{{ Set the internal flags */
PHP_METHOD(CachingIterator, setFlags)
{
	spl_dual_it_object   *intern;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (spl_cit_check_flags(flags) != SUCCESS) {
		zend_argument_value_error(1, "must contain only one of CachingIterator::CALL_TOSTRING, "
			"CachingIterator::TOSTRING_USE_KEY, CachingIterator::TOSTRING_USE_CURRENT, "
			"or CachingIterator::TOSTRING_USE_INNER");
		RETURN_THROWS();
	}
	if ((intern->u.caching.flags & CIT_CALL_TOSTRING) != 0 && (flags & CIT_CALL_TOSTRING) == 0) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Unsetting flag CALL_TO_STRING is not possible", 0);
		RETURN_THROWS();
	}
	if ((intern->u.caching.flags & CIT_TOSTRING_USE_INNER) != 0 && (flags & CIT_TOSTRING_USE_INNER) == 0) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Unsetting flag TOSTRING_USE_INNER is not possible", 0);
		RETURN_THROWS();
	}
	if ((flags & CIT_FULL_CACHE) != 0 && (intern->u.caching.flags & CIT_FULL_CACHE) == 0) {
		/* clear on (re)enable */
		zend_hash_clean(Z_ARRVAL(intern->u.caching.zcache));
	}
	intern->u.caching.flags = (intern->u.caching.flags & ~CIT_PUBLIC) | (flags & CIT_PUBLIC);
}
/* }}} */

/* {{{ Number of cached elements */
PHP_METHOD(CachingIterator, count)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (!(intern->u.caching.flags & CIT_FULL_CACHE))	{
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s does not use a full cache (see CachingIterator::__construct)", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL(intern->u.caching.zcache)));
}
/* }}} */

/* {{{ Create an iterator from a RecursiveIterator */
PHP_METHOD(RecursiveCachingIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_RecursiveCachingIterator, spl_ce_RecursiveIterator, DIT_RecursiveCachingIterator);
} /* }}} */

/* {{{ Check whether the current element of the inner iterator has children */
PHP_METHOD(RecursiveCachingIterator, hasChildren)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_BOOL(Z_TYPE(intern->u.caching.zchildren) != IS_UNDEF);
} /* }}} */

/* {{{ Return the inner iterator's children as a RecursiveCachingIterator */
PHP_METHOD(RecursiveCachingIterator, getChildren)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (Z_TYPE(intern->u.caching.zchildren) != IS_UNDEF) {
		zval *value = &intern->u.caching.zchildren;

		RETURN_COPY_DEREF(value);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ Create an iterator from anything that is traversable */
PHP_METHOD(IteratorIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_IteratorIterator, zend_ce_traversable, DIT_IteratorIterator);
} /* }}} */

/* {{{ Create an iterator from another iterator */
PHP_METHOD(NoRewindIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_NoRewindIterator, zend_ce_iterator, DIT_NoRewindIterator);
} /* }}} */

/* {{{ Prevent a call to inner iterators rewind() */
PHP_METHOD(NoRewindIterator, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* nothing to do */
} /* }}} */

/* {{{ Return inner iterators valid() */
PHP_METHOD(NoRewindIterator, valid)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	RETURN_BOOL(intern->inner.iterator->funcs->valid(intern->inner.iterator) == SUCCESS);
} /* }}} */

/* {{{ Return inner iterators key() */
PHP_METHOD(NoRewindIterator, key)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (intern->inner.iterator->funcs->get_current_key) {
		intern->inner.iterator->funcs->get_current_key(intern->inner.iterator, return_value);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ Return inner iterators current() */
PHP_METHOD(NoRewindIterator, current)
{
	spl_dual_it_object   *intern;
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	data = intern->inner.iterator->funcs->get_current_data(intern->inner.iterator);
	if (data) {
		RETURN_COPY_DEREF(data);
	}
} /* }}} */

/* {{{ Return inner iterators next() */
PHP_METHOD(NoRewindIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);
	intern->inner.iterator->funcs->move_forward(intern->inner.iterator);
} /* }}} */

/* {{{ Create an iterator from another iterator */
PHP_METHOD(InfiniteIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_InfiniteIterator, zend_ce_iterator, DIT_InfiniteIterator);
} /* }}} */

/* {{{ Prevent a call to inner iterators rewind() (internally the current data will be fetched if valid()) */
PHP_METHOD(InfiniteIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_dual_it_next(intern, 1);
	if (spl_dual_it_valid(intern) == SUCCESS) {
		spl_dual_it_fetch(intern, 0);
	} else {
		spl_dual_it_rewind(intern);
		if (spl_dual_it_valid(intern) == SUCCESS) {
			spl_dual_it_fetch(intern, 0);
		}
	}
} /* }}} */

/* {{{ Does nothing  */
PHP_METHOD(EmptyIterator, rewind)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
} /* }}} */

/* {{{ Return false */
PHP_METHOD(EmptyIterator, valid)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_FALSE;
} /* }}} */

/* {{{ Throws exception BadMethodCallException */
PHP_METHOD(EmptyIterator, key)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_throw_exception(spl_ce_BadMethodCallException, "Accessing the key of an EmptyIterator", 0);
} /* }}} */

/* {{{ Throws exception BadMethodCallException */
PHP_METHOD(EmptyIterator, current)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_throw_exception(spl_ce_BadMethodCallException, "Accessing the value of an EmptyIterator", 0);
} /* }}} */

/* {{{ Does nothing */
PHP_METHOD(EmptyIterator, next)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
} /* }}} */

int spl_append_it_next_iterator(spl_dual_it_object *intern) /* {{{*/
{
	spl_dual_it_free(intern);

	if (!Z_ISUNDEF(intern->inner.zobject)) {
		zval_ptr_dtor(&intern->inner.zobject);
		ZVAL_UNDEF(&intern->inner.zobject);
		intern->inner.ce = NULL;
		if (intern->inner.iterator) {
			zend_iterator_dtor(intern->inner.iterator);
			intern->inner.iterator = NULL;
		}
	}
	if (intern->u.append.iterator->funcs->valid(intern->u.append.iterator) == SUCCESS) {
		zval *it;

		it  = intern->u.append.iterator->funcs->get_current_data(intern->u.append.iterator);
		ZVAL_COPY(&intern->inner.zobject, it);
		intern->inner.ce = Z_OBJCE_P(it);
		intern->inner.iterator = intern->inner.ce->get_iterator(intern->inner.ce, it, 0);
		spl_dual_it_rewind(intern);
		return SUCCESS;
	} else {
		return FAILURE;
	}
} /* }}} */

void spl_append_it_fetch(spl_dual_it_object *intern) /* {{{*/
{
	while (spl_dual_it_valid(intern) != SUCCESS) {
		intern->u.append.iterator->funcs->move_forward(intern->u.append.iterator);
		if (spl_append_it_next_iterator(intern) != SUCCESS) {
			return;
		}
	}
	spl_dual_it_fetch(intern, 0);
} /* }}} */

void spl_append_it_next(spl_dual_it_object *intern) /* {{{ */
{
	if (spl_dual_it_valid(intern) == SUCCESS) {
		spl_dual_it_next(intern, 1);
	}
	spl_append_it_fetch(intern);
} /* }}} */

/* {{{ Create an AppendIterator */
PHP_METHOD(AppendIterator, __construct)
{
	spl_dual_it_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, spl_ce_AppendIterator, zend_ce_iterator, DIT_AppendIterator);
} /* }}} */

/* {{{ Append an iterator */
PHP_METHOD(AppendIterator, append)
{
	spl_dual_it_object   *intern;
	zval *it;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &it, zend_ce_iterator) == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	if (intern->u.append.iterator->funcs->valid(intern->u.append.iterator) == SUCCESS && spl_dual_it_valid(intern) != SUCCESS) {
		spl_array_iterator_append(&intern->u.append.zarrayit, it);
		intern->u.append.iterator->funcs->move_forward(intern->u.append.iterator);
	}else{
		spl_array_iterator_append(&intern->u.append.zarrayit, it);
	}

	if (!intern->inner.iterator || spl_dual_it_valid(intern) != SUCCESS) {
		if (intern->u.append.iterator->funcs->valid(intern->u.append.iterator) != SUCCESS) {
			intern->u.append.iterator->funcs->rewind(intern->u.append.iterator);
		}
		do {
			spl_append_it_next_iterator(intern);
		} while (Z_OBJ(intern->inner.zobject) != Z_OBJ_P(it));
		spl_append_it_fetch(intern);
	}
} /* }}} */

/* {{{ Get the current element value */
PHP_METHOD(AppendIterator, current)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_dual_it_fetch(intern, 1);
	if (Z_TYPE(intern->current.data) != IS_UNDEF) {
		RETURN_COPY_DEREF(&intern->current.data);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ Rewind to the first iterator and rewind the first iterator, too */
PHP_METHOD(AppendIterator, rewind)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	intern->u.append.iterator->funcs->rewind(intern->u.append.iterator);
	if (spl_append_it_next_iterator(intern) == SUCCESS) {
		spl_append_it_fetch(intern);
	}
} /* }}} */

/* {{{ Check if the current state is valid */
PHP_METHOD(AppendIterator, valid)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	RETURN_BOOL(Z_TYPE(intern->current.data) != IS_UNDEF);
} /* }}} */

/* {{{ Forward to next element */
PHP_METHOD(AppendIterator, next)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	spl_append_it_next(intern);
} /* }}} */

/* {{{ Get index of iterator */
PHP_METHOD(AppendIterator, getIteratorIndex)
{
	spl_dual_it_object   *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	APPENDIT_CHECK_CTOR(intern);
	spl_array_iterator_key(&intern->u.append.zarrayit, return_value);
} /* }}} */

/* {{{ Get access to inner ArrayIterator */
PHP_METHOD(AppendIterator, getArrayIterator)
{
	spl_dual_it_object   *intern;
	zval *value;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	SPL_FETCH_AND_CHECK_DUAL_IT(intern, ZEND_THIS);

	value = &intern->u.append.zarrayit;
	RETURN_COPY_DEREF(value);
} /* }}} */

PHPAPI int spl_iterator_apply(zval *obj, spl_iterator_apply_func_t apply_func, void *puser)
{
	zend_object_iterator   *iter;
	zend_class_entry       *ce = Z_OBJCE_P(obj);

	iter = ce->get_iterator(ce, obj, 0);

	if (EG(exception)) {
		goto done;
	}

	iter->index = 0;
	if (iter->funcs->rewind) {
		iter->funcs->rewind(iter);
		if (EG(exception)) {
			goto done;
		}
	}

	while (iter->funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			goto done;
		}
		if (apply_func(iter, puser) == ZEND_HASH_APPLY_STOP || EG(exception)) {
			goto done;
		}
		iter->index++;
		iter->funcs->move_forward(iter);
		if (EG(exception)) {
			goto done;
		}
	}

done:
	if (iter) {
		zend_iterator_dtor(iter);
	}
	return EG(exception) ? FAILURE : SUCCESS;
}
/* }}} */

static int spl_iterator_to_array_apply(zend_object_iterator *iter, void *puser) /* {{{ */
{
	zval *data, *return_value = (zval*)puser;

	data = iter->funcs->get_current_data(iter);
	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (data == NULL) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (iter->funcs->get_current_key) {
		zval key;
		iter->funcs->get_current_key(iter, &key);
		if (EG(exception)) {
			return ZEND_HASH_APPLY_STOP;
		}
		array_set_zval_key(Z_ARRVAL_P(return_value), &key, data);
		zval_ptr_dtor(&key);
	} else {
		Z_TRY_ADDREF_P(data);
		add_next_index_zval(return_value, data);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int spl_iterator_to_values_apply(zend_object_iterator *iter, void *puser) /* {{{ */
{
	zval *data, *return_value = (zval*)puser;

	data = iter->funcs->get_current_data(iter);
	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (data == NULL) {
		return ZEND_HASH_APPLY_STOP;
	}
	Z_TRY_ADDREF_P(data);
	add_next_index_zval(return_value, data);
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ Copy the iterator into an array */
PHP_FUNCTION(iterator_to_array)
{
	zval  *obj;
	bool use_keys = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ITERABLE(obj)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_keys)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(obj) == IS_ARRAY) {
		if (use_keys) {
			RETURN_COPY(obj);
		} else {
			RETURN_ARR(zend_array_to_list(Z_ARRVAL_P(obj)));
		}
	}

	array_init(return_value);
	spl_iterator_apply(obj, use_keys ? spl_iterator_to_array_apply : spl_iterator_to_values_apply, (void*)return_value);
} /* }}} */

static int spl_iterator_count_apply(zend_object_iterator *iter, void *puser) /* {{{ */
{
	if (UNEXPECTED(*(zend_long*)puser == ZEND_LONG_MAX)) {
		return ZEND_HASH_APPLY_STOP;
	}
	(*(zend_long*)puser)++;
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ Count the elements in an iterator */
PHP_FUNCTION(iterator_count)
{
	zval  *obj;
	zend_long  count = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ITERABLE(obj)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(obj) == IS_ARRAY) {
		count =  zend_hash_num_elements(Z_ARRVAL_P(obj));
	} else {
		if (spl_iterator_apply(obj, spl_iterator_count_apply, (void*)&count) == FAILURE) {
			RETURN_THROWS();
		}
	}

	RETURN_LONG(count);
}
/* }}} */

typedef struct {
	zval                   *obj;
	zval                   *args;
	zend_long              count;
	zend_fcall_info        fci;
	zend_fcall_info_cache  fcc;
} spl_iterator_apply_info;

static int spl_iterator_func_apply(zend_object_iterator *iter, void *puser) /* {{{ */
{
	zval retval;
	spl_iterator_apply_info  *apply_info = (spl_iterator_apply_info*)puser;
	int result;

	apply_info->count++;
	zend_fcall_info_call(&apply_info->fci, &apply_info->fcc, &retval, NULL);
	result = zend_is_true(&retval) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_STOP;
	zval_ptr_dtor(&retval);
	return result;
}
/* }}} */

/* {{{ Calls a function for every element in an iterator */
PHP_FUNCTION(iterator_apply)
{
	spl_iterator_apply_info  apply_info;

	apply_info.args = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Of|a!", &apply_info.obj, zend_ce_traversable, &apply_info.fci, &apply_info.fcc, &apply_info.args) == FAILURE) {
		RETURN_THROWS();
	}

	apply_info.count = 0;
	zend_fcall_info_args(&apply_info.fci, apply_info.args);
	if (spl_iterator_apply(apply_info.obj, spl_iterator_func_apply, (void*)&apply_info) == FAILURE) {
		zend_fcall_info_args(&apply_info.fci, NULL);
		return;
	}

	zend_fcall_info_args(&apply_info.fci, NULL);
	RETURN_LONG(apply_info.count);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_iterators) */
PHP_MINIT_FUNCTION(spl_iterators)
{
	spl_ce_RecursiveIterator = register_class_RecursiveIterator(zend_ce_iterator);

	spl_ce_OuterIterator = register_class_OuterIterator(zend_ce_iterator);

	spl_ce_RecursiveIteratorIterator = register_class_RecursiveIteratorIterator(spl_ce_OuterIterator);
	spl_ce_RecursiveIteratorIterator->create_object = spl_RecursiveIteratorIterator_new;
	spl_ce_RecursiveIteratorIterator->get_iterator = spl_recursive_it_get_iterator;

	memcpy(&spl_handlers_rec_it_it, &std_object_handlers, sizeof(zend_object_handlers));
	spl_handlers_rec_it_it.offset = XtOffsetOf(spl_recursive_it_object, std);
	spl_handlers_rec_it_it.get_method = spl_recursive_it_get_method;
	spl_handlers_rec_it_it.clone_obj = NULL;
	spl_handlers_rec_it_it.free_obj = spl_RecursiveIteratorIterator_free_storage;
	spl_handlers_rec_it_it.get_gc = spl_RecursiveIteratorIterator_get_gc;

	memcpy(&spl_handlers_dual_it, &std_object_handlers, sizeof(zend_object_handlers));
	spl_handlers_dual_it.offset = XtOffsetOf(spl_dual_it_object, std);
	spl_handlers_dual_it.get_method = spl_dual_it_get_method;
	spl_handlers_dual_it.clone_obj = NULL;
	spl_handlers_dual_it.free_obj = spl_dual_it_free_storage;
	spl_handlers_dual_it.get_gc = spl_dual_it_get_gc;

	spl_ce_IteratorIterator = register_class_IteratorIterator(spl_ce_OuterIterator);
	spl_ce_IteratorIterator->create_object = spl_dual_it_new;

	spl_ce_FilterIterator = register_class_FilterIterator(spl_ce_IteratorIterator);
	spl_ce_FilterIterator->create_object = spl_dual_it_new;

	spl_ce_RecursiveFilterIterator = register_class_RecursiveFilterIterator(spl_ce_FilterIterator, spl_ce_RecursiveIterator);
	spl_ce_RecursiveFilterIterator->create_object = spl_dual_it_new;

	spl_ce_CallbackFilterIterator = register_class_CallbackFilterIterator(spl_ce_FilterIterator);
	spl_ce_CallbackFilterIterator->create_object = spl_dual_it_new;

	spl_ce_RecursiveCallbackFilterIterator = register_class_RecursiveCallbackFilterIterator(spl_ce_CallbackFilterIterator, spl_ce_RecursiveIterator);
	spl_ce_RecursiveCallbackFilterIterator->create_object = spl_dual_it_new;

	spl_ce_ParentIterator = register_class_ParentIterator(spl_ce_RecursiveFilterIterator);
	spl_ce_ParentIterator->create_object = spl_dual_it_new;

	spl_ce_SeekableIterator = register_class_SeekableIterator(zend_ce_iterator);

	spl_ce_LimitIterator = register_class_LimitIterator(spl_ce_IteratorIterator);
	spl_ce_LimitIterator->create_object = spl_dual_it_new;

	spl_ce_CachingIterator = register_class_CachingIterator(spl_ce_IteratorIterator, zend_ce_arrayaccess, zend_ce_countable, zend_ce_stringable);
	spl_ce_CachingIterator->create_object = spl_dual_it_new;

	spl_ce_RecursiveCachingIterator = register_class_RecursiveCachingIterator(spl_ce_CachingIterator, spl_ce_RecursiveIterator);
	spl_ce_RecursiveCachingIterator->create_object = spl_dual_it_new;

	spl_ce_NoRewindIterator = register_class_NoRewindIterator(spl_ce_IteratorIterator);
	spl_ce_NoRewindIterator->create_object = spl_dual_it_new;

	spl_ce_AppendIterator = register_class_AppendIterator(spl_ce_IteratorIterator);
	spl_ce_AppendIterator->create_object = spl_dual_it_new;

	spl_ce_InfiniteIterator = register_class_InfiniteIterator(spl_ce_IteratorIterator);
	spl_ce_InfiniteIterator->create_object = spl_dual_it_new;

	spl_ce_RegexIterator = register_class_RegexIterator(spl_ce_FilterIterator);
	spl_ce_RegexIterator->create_object = spl_dual_it_new;

	spl_ce_RecursiveRegexIterator = register_class_RecursiveRegexIterator(spl_ce_RegexIterator, spl_ce_RecursiveIterator);
	spl_ce_RecursiveRegexIterator->create_object = spl_dual_it_new;

	spl_ce_EmptyIterator = register_class_EmptyIterator(zend_ce_iterator);

	spl_ce_RecursiveTreeIterator = register_class_RecursiveTreeIterator(spl_ce_RecursiveIteratorIterator);
	spl_ce_RecursiveTreeIterator->create_object = spl_RecursiveTreeIterator_new;

	return SUCCESS;
}
/* }}} */
