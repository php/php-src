/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "basic_functions.h"
#include "aggregation.h"
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
#include "ext/pcre/php_pcre.h"
#endif

static void aggregation_info_dtor(aggregation_info *info)
{
	/* FIXME: This is here to make it compile with Engine 2 but part of this module will need rewriting */
	
#ifndef ZEND_ENGINE_2
	destroy_zend_class(info->new_ce);
	efree(info->new_ce);
#else
	/* FIXME: In ZE2, there seems to be an issue with refcounts or something between
	 * this class entry and the original; there are problems when destroying the
	 * function table.
	 * Skipping deleting here will prevent a segfault but will leak
	 * the class name, the static_members hash and the ce itself.
	 * */

	/* destroy_zend_class(&info->new_ce); */
#endif
	zval_ptr_dtor(&info->aggr_members);

}

/* {{{ static zval* array_to_hash */
static zval *array_to_hash(zval *array)
{
	zval *hash, **entry;
	char *name_lc;

	/*
	 * Well, this just transposes the array, popularly known as flipping it, or
	 * giving it the finger.
	 */
	MAKE_STD_ZVAL(hash);
	array_init(hash);
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
		 zend_hash_get_current_data(Z_ARRVAL_P(array), (void**)&entry) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(array))) {
		if (Z_TYPE_PP(entry) == IS_STRING) {
			/*
			 * I hate case-insensitivity. Die, die, die.
			 */
			name_lc = estrndup(Z_STRVAL_PP(entry), Z_STRLEN_PP(entry));
			zend_str_tolower(name_lc, Z_STRLEN_PP(entry));
			add_assoc_bool_ex(hash, name_lc, Z_STRLEN_PP(entry)+1, 1);
			efree(name_lc);
		}
	}

	return hash;
}
/* }}} */


/* {{{ static void aggregate_methods() */
static void aggregate_methods(zend_class_entry *ce, zend_class_entry *from_ce, int aggr_type, zval *aggr_filter, zend_bool exclude, zval *aggr_methods TSRMLS_DC)
{
	HashPosition pos;
	zend_function *function;
	char *func_name;
	uint func_name_len;
	ulong num_key;
	zval *list_hash = NULL;
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	pcre *re = NULL;
	pcre_extra *re_extra = NULL;
	int re_options = 0;
#endif

	/*
	 * Flip the array for easy lookup, or compile the regexp.
	 */
	if (aggr_type == AGGREGATE_BY_LIST) {
		list_hash = array_to_hash(aggr_filter);
	}
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	else if (aggr_type == AGGREGATE_BY_REGEXP) {
		if ((re = pcre_get_compiled_regex(Z_STRVAL_P(aggr_filter), &re_extra, &re_options)) == NULL) {
			return;
		}
	}
#endif

	/*
	 * "Just because it's not nice doesn't mean it's not miraculous."
	 * 			-- _Interesting Times_, Terry Pratchett
	 */

	/*
	 * Aggregating by list without exclusion can be done more efficiently if we
	 * iterate through the list and check against function table instead of the
	 * other way around.
	 */
	if (aggr_type != AGGREGATE_BY_LIST || exclude) {
		zend_hash_internal_pointer_reset_ex(&from_ce->function_table, &pos);
		while (zend_hash_get_current_data_ex(&from_ce->function_table, (void**)&function, &pos) == SUCCESS) {
			zend_hash_get_current_key_ex(&from_ce->function_table, &func_name, &func_name_len, &num_key, 0, &pos);

			/* We do not aggregate:
			 * 1. constructors */
			if (!strncmp(func_name, from_ce->name, MIN(func_name_len-1, from_ce->name_length)) ||
			/* 2. private methods (heh, like we really have them) */
				func_name[0] == '_' ||
			/* 3. explicitly excluded methods */
				(aggr_type == AGGREGATE_BY_LIST && zend_hash_exists(Z_ARRVAL_P(list_hash), func_name, func_name_len))
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
				||
			/* 4. methods matching regexp as modified by the exclusion flag */
				(aggr_type == AGGREGATE_BY_REGEXP && (pcre_exec(re, re_extra, func_name, func_name_len-1, 0, 0, NULL, 0) < 0) ^ exclude) == 1
#endif
				) {
				zend_hash_move_forward_ex(&from_ce->function_table, &pos);
				continue;
			}

			/*
			 * This is where the magic happens.
			 */
			if (zend_hash_add(&ce->function_table, func_name, func_name_len,
							  (void*)function, sizeof(zend_function), NULL) == SUCCESS) {

				add_next_index_stringl(aggr_methods, func_name, func_name_len-1, 1);
			}

			zend_hash_move_forward_ex(&from_ce->function_table, &pos);
		}
	} else {
		/*
		 * This is just like above except the other way around.
		 */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(list_hash));
		while (zend_hash_get_current_key_ex(Z_ARRVAL_P(list_hash), &func_name, &func_name_len, &num_key, 0, NULL) == HASH_KEY_IS_STRING) {
			if (!strncmp(func_name, from_ce->name, MIN(func_name_len-1, from_ce->name_length)) ||
				func_name[0] == '_' ||
				zend_hash_find(&from_ce->function_table, func_name, func_name_len, (void**)&function) == FAILURE) {
				zend_hash_move_forward(Z_ARRVAL_P(list_hash));
				continue;
			}

			if (zend_hash_add(&ce->function_table, func_name, func_name_len,
							  (void*)function, sizeof(zend_function), NULL) == SUCCESS) {
				add_next_index_stringl(aggr_methods, func_name, func_name_len-1, 1);
			}

			zend_hash_move_forward(Z_ARRVAL_P(list_hash));
		}
	}

	if (list_hash) {
		zval_ptr_dtor(&list_hash);
	}
}
/* }}} */


/* {{{ static void aggregate_properties() */
static void aggregate_properties(zval *obj, zend_class_entry *from_ce, int aggr_type, zval *aggr_filter, zend_bool exclude, zval *aggr_props TSRMLS_DC)
{
	HashPosition pos;
	zval **prop;
	char *prop_name;
	uint prop_name_len;
	ulong num_key;
	zval *list_hash = NULL;
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	pcre *re = NULL;
	pcre_extra *re_extra = NULL;
	int re_options = 0;
#endif

	if (!from_ce->constants_updated) {
		zend_hash_apply_with_argument(&from_ce->default_properties, (apply_func_arg_t) zval_update_constant, (void *) 1 TSRMLS_CC);
		from_ce->constants_updated = 1;
	}

	/*
	 * Flip the array for easy lookup, or compile the regexp.
	 */
	if (aggr_type == AGGREGATE_BY_LIST) {
		list_hash = array_to_hash(aggr_filter);
	}
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	else if (aggr_type == AGGREGATE_BY_REGEXP) {
		if ((re = pcre_get_compiled_regex(Z_STRVAL_P(aggr_filter), &re_extra, &re_options)) == NULL) {
			return;
		}
	}
#endif

	/*
	 * "Just because it's not nice doesn't mean it's not miraculous."
	 * 			-- _Interesting Times_, Terry Pratchett
	 */

	/*
	 * Aggregating by list without exclusion can be done more efficiently if we
	 * iterate through the list and check against default properties table
	 * instead of the other way around.
	 */
	if (aggr_type != AGGREGATE_BY_LIST || exclude) {
		zend_hash_internal_pointer_reset_ex(&from_ce->default_properties, &pos);
		while (zend_hash_get_current_data_ex(&from_ce->default_properties, (void**)&prop, &pos) == SUCCESS) {
			zend_hash_get_current_key_ex(&from_ce->default_properties, &prop_name, &prop_name_len, &num_key, 0, &pos);

			/* We do not aggregate:
			 * 1. private properties (heh, like we really have them) */
			if (prop_name[0] == '_' ||
			/* 2. explicitly excluded properties */
				(aggr_type == AGGREGATE_BY_LIST && zend_hash_exists(Z_ARRVAL_P(list_hash), prop_name, prop_name_len))
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
				||
			/* 3. properties matching regexp as modified by the exclusion flag */
				(aggr_type == AGGREGATE_BY_REGEXP && (pcre_exec(re, re_extra, prop_name, prop_name_len-1, 0, 0, NULL, 0) < 0) ^ exclude) == 1
#endif
				) {
				zend_hash_move_forward_ex(&from_ce->default_properties, &pos);
				continue;
			}

			/*
			 * This is where the magic happens.
			 */
			if (zend_hash_add(Z_OBJPROP_P(obj), prop_name, prop_name_len,
							  (void*)prop, sizeof(zval *), NULL) == SUCCESS) {
				zval_add_ref(prop);
				add_next_index_stringl(aggr_props, prop_name, prop_name_len-1, 1);
			}

			zend_hash_move_forward_ex(&from_ce->default_properties, &pos);
		}
	} else {
		/*
		 * This is just like above except the other way around.
		 */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(list_hash));
		while (zend_hash_get_current_key_ex(Z_ARRVAL_P(list_hash), &prop_name, &prop_name_len, &num_key, 0, NULL) == HASH_KEY_IS_STRING) {
			if (prop_name[0] == '_' ||
				zend_hash_find(&from_ce->default_properties, prop_name, prop_name_len, (void**)&prop) == FAILURE) {
				zend_hash_move_forward(Z_ARRVAL_P(list_hash));
				continue;
			}

			if (zend_hash_add(Z_OBJPROP_P(obj), prop_name, prop_name_len,
							  (void*)prop, sizeof(zval *), NULL) == SUCCESS) {
				zval_add_ref(prop);
				add_next_index_stringl(aggr_props, prop_name, prop_name_len-1, 1);
			}

			zend_hash_move_forward(Z_ARRVAL_P(list_hash));
		}
	}

	if (list_hash) {
		zval_ptr_dtor(&list_hash);
	}
}
/* }}} */


/* {{{ static void aggregate() */
static void aggregate(INTERNAL_FUNCTION_PARAMETERS, int aggr_what, int aggr_type)
{
	/* Incoming parameters. */
	zval *obj, *aggr_list = NULL;
	char *class_name, *class_name_lc, *aggr_regexp;
	int class_name_len, aggr_regexp_len;
	zend_bool exclude = 0;

	/* Other variables. */
  	zval **aggr_members, z_aggr_regexp;
	zend_class_entry *ce, *new_ce;
	zend_function tmp_zend_function;
	aggregation_info aggr_info_new, *aggr_info = &aggr_info_new;
	zval *aggr_methods_new, **aggr_methods = &aggr_methods_new;
	zval *aggr_props_new, **aggr_props = &aggr_props_new;
	zval *aggr_filter = NULL;
	int zpp_result = FAILURE;

	/*
	 * Ah, the beauty of the new parameter parsing API. Almost as good as Heidi Klum.
	 */
	switch (aggr_type) {
		case AGGREGATE_ALL:
			zpp_result = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os", &obj,
											   &class_name, &class_name_len);
			break;

		case AGGREGATE_BY_LIST:
			zpp_result = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "osa|b", &obj,
											   &class_name, &class_name_len,
											   &aggr_list, &exclude);
			break;

		case AGGREGATE_BY_REGEXP:
			zpp_result = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oss|b", &obj,
											   &class_name, &class_name_len,
											   &aggr_regexp, &aggr_regexp_len, &exclude);
			ZVAL_STRINGL(&z_aggr_regexp, aggr_regexp, aggr_regexp_len, 0);
			break;
	}

	if (zpp_result == FAILURE) {
		return;
	}

	/*
	 * Case-insensitivity is like that last freaking mutant from horror movies:
	 * irradiated, blown in half, its eyes melting in their sockets, yet still
	 * dragging itself closer and closer to you until it's pulverized into
	 * microscopic pieces via some last-minute contrivance. And even then you
	 * are not sure that it's finally dead. But that's just how I feel.
	 */
	class_name_lc = estrndup(class_name, class_name_len);
	zend_str_tolower(class_name_lc, class_name_len);
	if (zend_hash_find(EG(class_table), class_name_lc,
					   class_name_len+1, (void **)&ce) == FAILURE) {
		php_error(E_WARNING, "%s(): Expects the second parameter to be a valid class name, '%s' given", get_active_function_name(TSRMLS_C), class_name);
		efree(class_name_lc);
		return;
	}
#ifdef ZEND_ENGINE_2
	ce = *(zend_class_entry**)ce;
#endif

	/*
	 * And God said, Let there be light; and there was light. But only once.
	 */
	if (!BG(aggregation_table)) {
		BG(aggregation_table) = (HashTable *) emalloc(sizeof(HashTable));
		zend_hash_init(BG(aggregation_table), 5, NULL, (dtor_func_t) aggregation_info_dtor, 0);
	}

	/*
	 * Digging deep in the rabbit hole with a long object.. and coming up
	 * more empty than the imagination of whoever made "Battlefield Earth".
	 */ 
	if (zend_hash_index_find(BG(aggregation_table), (long)obj, (void**)&aggr_info) == FAILURE) {
		zval *tmp;

		/*
		 * You are not expected to understand this.
		 */
		new_ce = emalloc(sizeof(zend_class_entry));
		new_ce->type = ZEND_USER_CLASS;
		new_ce->name = estrndup(Z_OBJCE_P(obj)->name, Z_OBJCE_P(obj)->name_length);
		new_ce->name_length = Z_OBJCE_P(obj)->name_length;
		new_ce->parent = Z_OBJCE_P(obj)->parent;
#ifdef ZEND_ENGINE_2
		new_ce->refcount = 1;
#else
		new_ce->refcount = (int *) emalloc(sizeof(int));
		*new_ce->refcount = 1;
#endif
		new_ce->constants_updated = Z_OBJCE_P(obj)->constants_updated;
		zend_hash_init(&new_ce->function_table, 10, NULL, ZEND_FUNCTION_DTOR, 0);
		zend_hash_init(&new_ce->default_properties, 10, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(&new_ce->function_table, &Z_OBJCE_P(obj)->function_table, (copy_ctor_func_t) function_add_ref, &tmp_zend_function, sizeof(zend_function));
		zend_hash_copy(&new_ce->default_properties, &Z_OBJCE_P(obj)->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

#ifdef ZEND_ENGINE_2
		ALLOC_HASHTABLE(new_ce->static_members);
		zend_hash_init(new_ce->static_members, 10, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(new_ce->static_members, Z_OBJCE_P(obj)->static_members, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
		zend_hash_init(&new_ce->constants_table, 10, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(&new_ce->constants_table, &Z_OBJCE_P(obj)->constants_table, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
		zend_hash_init(&new_ce->class_table, 10, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(&new_ce->class_table, &Z_OBJCE_P(obj)->class_table, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		zend_hash_init(&new_ce->private_properties, 10, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(&new_ce->private_properties, &Z_OBJCE_P(obj)->private_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		new_ce->constructor = Z_OBJCE_P(obj)->constructor;
		new_ce->destructor = Z_OBJCE_P(obj)->destructor;
		new_ce->clone = Z_OBJCE_P(obj)->clone;
#endif
		new_ce->builtin_functions = Z_OBJCE_P(obj)->builtin_functions;
		new_ce->handle_function_call = Z_OBJCE_P(obj)->handle_function_call;
		new_ce->handle_property_get  = Z_OBJCE_P(obj)->handle_property_get;
		new_ce->handle_property_set  = Z_OBJCE_P(obj)->handle_property_set;

		/*
		 * Okay, that was kind of exhausting. Let's invoke programmer virtue #1
		 * and stuff this where it belongs so we don't have to work so hard next
		 * time.
		 */
		/* OBJECT FIXME!! won't work with non-standard objects */
		(Z_OBJ_P(obj))->ce = new_ce;
		aggr_info_new.new_ce = new_ce;
		MAKE_STD_ZVAL(aggr_info_new.aggr_members);
		array_init(aggr_info_new.aggr_members);

		zend_hash_index_update(BG(aggregation_table), (long)obj,
							   (void *)&aggr_info_new, sizeof(aggregation_info), NULL);

	} else {
		/*
		 * Seek and ye shall find.
		 */
		new_ce = aggr_info->new_ce;
	}

	/*
	 * This should be easy to understand. If not, ask Rasmus about it at his
	 * next tutorial.
	 */
	if (zend_hash_find(Z_ARRVAL_P(aggr_info->aggr_members), class_name_lc,
					   class_name_len+1, (void **)&aggr_members) == FAILURE) {
		zval *tmp;

		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		MAKE_STD_ZVAL(aggr_methods_new);
		array_init(aggr_methods_new);
		MAKE_STD_ZVAL(aggr_props_new);
		array_init(aggr_props_new);
		add_assoc_zval_ex(tmp, "methods", sizeof("methods"), aggr_methods_new);
		add_assoc_zval_ex(tmp, "properties", sizeof("properties"), aggr_props_new);

		zend_hash_add(Z_ARRVAL_P(aggr_info->aggr_members), class_name_lc,
					  class_name_len+1, &tmp, sizeof(zval *), NULL);
	} else {
		zend_hash_find(Z_ARRVAL_PP(aggr_members), "methods", sizeof("methods"), (void**)&aggr_methods);
		zend_hash_find(Z_ARRVAL_PP(aggr_members), "properties", sizeof("properties"), (void**)&aggr_props);
	}

	if (aggr_type == AGGREGATE_BY_LIST) {
		aggr_filter = aggr_list;
	} else if (aggr_type == AGGREGATE_BY_REGEXP) {
		aggr_filter = &z_aggr_regexp;
	}

	if (aggr_what == AGGREGATE_METHODS || aggr_what == AGGREGATE_ALL) {
		aggregate_methods(new_ce, ce, aggr_type, aggr_filter, exclude, *aggr_methods TSRMLS_CC);
	}

	if (aggr_what == AGGREGATE_PROPERTIES || aggr_what == AGGREGATE_ALL) {
		aggregate_properties(obj, ce, aggr_type, aggr_filter, exclude, *aggr_props TSRMLS_CC);
	}

	/*
	 * Yes, we have to clean up after monsters. Tsk-tsk.
	 */
	efree(class_name_lc);
}
/* }}} */


/* {{{ proto void aggregate(object obj, string class)
   */
PHP_FUNCTION(aggregate)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_ALL, AGGREGATE_ALL);
}
/* }}} */


/* {{{ proto void aggregate_methods(object obj, string class)
   */
PHP_FUNCTION(aggregate_methods)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_METHODS, AGGREGATE_ALL);
}
/* }}} */


/* {{{ proto void aggregate_methods_by_list(object obj, string class, array method_list [, bool exclude])
   */
PHP_FUNCTION(aggregate_methods_by_list)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_METHODS, AGGREGATE_BY_LIST);
}
/* }}} */


/* {{{ proto void aggregate_properties(object obj, string class)
   */
PHP_FUNCTION(aggregate_properties)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_PROPERTIES, AGGREGATE_ALL);
}
/* }}} */


/* {{{ proto void aggregate_properties_by_list(object obj, string class, array props_list [, bool exclude])
   */
PHP_FUNCTION(aggregate_properties_by_list)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_PROPERTIES, AGGREGATE_BY_LIST);
}
/* }}} */

#if HAVE_PCRE || HAVE_BUNDLED_PCRE
/* {{{ proto void aggregate_methods_by_regexp(object obj, string class, string regexp [, bool exclude])
   */
PHP_FUNCTION(aggregate_methods_by_regexp)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_METHODS, AGGREGATE_BY_REGEXP);
}
/* }}} */


/* {{{ proto void aggregate_properties_by_regexp(object obj, string class, string regexp [, bool exclude])
   */
PHP_FUNCTION(aggregate_properties_by_regexp)
{
	aggregate(INTERNAL_FUNCTION_PARAM_PASSTHRU, AGGREGATE_PROPERTIES, AGGREGATE_BY_REGEXP);
}
/* }}} */
#endif


/* {{{ proto array aggregation_info(object obj)
 */
PHP_FUNCTION(aggregation_info)
{
	zval *obj;
	aggregation_info *aggr_info;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

	if (!BG(aggregation_table) ||
		zend_hash_index_find(BG(aggregation_table), (long)obj, (void**)&aggr_info) == FAILURE) {
		RETURN_FALSE;
	}

	*return_value = *aggr_info->aggr_members;
	zval_copy_ctor(return_value);
}
/* }}} */


/* {{{ proto void deaggregate(object obj [, string class])
 */
PHP_FUNCTION(deaggregate)
{
	zval *obj;
	char *class_name = NULL, *class_name_lc;
	int class_name_len;
	aggregation_info *aggr_info;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|s", &obj,
							  &class_name, &class_name_len) == FAILURE) {
		return;
	}

	if (!BG(aggregation_table) ||
		zend_hash_index_find(BG(aggregation_table), (long)obj, (void**)&aggr_info) == FAILURE) {
		return;
	}

	if (class_name) {
		zval **aggr_members,
			 **aggr_methods,
			 **aggr_props,
			 **method, **prop;

		class_name_lc = estrndup(class_name, class_name_len);
		zend_str_tolower(class_name_lc, class_name_len);

		if (zend_hash_find(Z_ARRVAL_P(aggr_info->aggr_members), class_name_lc,
						   class_name_len+1, (void **)&aggr_members) == FAILURE) {
			efree(class_name_lc);
			return;
		}

		zend_hash_find(Z_ARRVAL_PP(aggr_members), "methods", sizeof("methods"), (void**)&aggr_methods);
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(aggr_methods));
			 zend_hash_get_current_data(Z_ARRVAL_PP(aggr_methods), (void**)&method) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_PP(aggr_methods))) {
			zend_hash_del(&Z_OBJCE_P(obj)->function_table, Z_STRVAL_PP(method), Z_STRLEN_PP(method)+1);
		}

		zend_hash_find(Z_ARRVAL_PP(aggr_members), "properties", sizeof("properties"), (void**)&aggr_props);
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(aggr_props));
			 zend_hash_get_current_data(Z_ARRVAL_PP(aggr_props), (void**)&prop) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_PP(aggr_props))) {
			zend_hash_del(Z_OBJPROP_P(obj), Z_STRVAL_PP(prop), Z_STRLEN_PP(prop)+1);
		}

		zend_hash_del(Z_ARRVAL_P(aggr_info->aggr_members), class_name_lc, class_name_len+1);

		efree(class_name_lc);
	} else {
		zend_class_entry *orig_ce;
		zval **aggr_members;
		zval **aggr_props, **prop;

		if (zend_hash_find(EG(class_table), Z_OBJCE_P(obj)->name,
						   Z_OBJCE_P(obj)->name_length+1, (void **)&orig_ce) == FAILURE) {
			php_error(E_WARNING, "%s(): Internal deaggregation error", get_active_function_name (TSRMLS_C));
			return;
		}

		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(aggr_info->aggr_members));
			 zend_hash_get_current_data(Z_ARRVAL_P(aggr_info->aggr_members), (void **)&aggr_members) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_P(aggr_info->aggr_members))) {
			zend_hash_find(Z_ARRVAL_PP(aggr_members), "properties", sizeof("properties"), (void**)&aggr_props);
			for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(aggr_props));
				 zend_hash_get_current_data(Z_ARRVAL_PP(aggr_props), (void**)&prop) == SUCCESS;
				 zend_hash_move_forward(Z_ARRVAL_PP(aggr_props))) {
				zend_hash_del(Z_OBJPROP_P(obj), Z_STRVAL_PP(prop), Z_STRLEN_PP(prop)+1);
			}
		}

		/* OBJECT FIXME!! won't work with non-standard objects */
		(Z_OBJ_P(obj))->ce = orig_ce;
		zend_hash_index_del(BG(aggregation_table), (long)obj);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
