/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_generators.h"

ZEND_API zend_class_entry *zend_ce_generator;
static zend_object_handlers zend_generator_handlers;

static void zend_generator_free_storage(zend_generator *generator TSRMLS_DC) /* {{{ */
{
	zend_object_std_dtor(&generator->std TSRMLS_CC);

	if (generator->execute_data) {
		zend_execute_data *execute_data = generator->execute_data;

		if (!execute_data->symbol_table) {
			int i;
			for (i = 0; i < execute_data->op_array->last_var; ++i) {
				if (execute_data->CVs[i]) {
					zval_ptr_dtor(execute_data->CVs[i]);
				}
			}
		} else {
			if (EG(symtable_cache_ptr) >= EG(symtable_cache_limit)) {
				zend_hash_destroy(execute_data->symbol_table);
				FREE_HASHTABLE(execute_data->symbol_table);
			} else {
				zend_hash_clean(execute_data->symbol_table);
				*(++EG(symtable_cache_ptr)) = execute_data->symbol_table;
			}
		}

		efree(execute_data);
	}

	efree(generator);
}
/* }}} */

static zend_object_value zend_generator_create(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_generator *generator;
	zend_object_value object;

	generator = emalloc(sizeof(zend_generator));
	memset(generator, 0, sizeof(zend_generator));

	zend_object_std_init(&generator->std, class_type TSRMLS_CC);

	object.handle = zend_objects_store_put(generator, NULL,
		(zend_objects_free_object_storage_t) zend_generator_free_storage,
		NULL /* no clone handler for now */
		TSRMLS_CC
	);
	object.handlers = &zend_generator_handlers;

	return object;
}
/* }}} */

static zend_function *zend_generator_get_constructor(zval *object TSRMLS_DC) /* {{{ */
{
	zend_error(E_RECOVERABLE_ERROR, "The \"Generator\" class is reserved for internal use and cannot be manually instantiated");

	return NULL;
}
/* }}} */

static void zend_generator_resume(zval *object, zend_generator *generator TSRMLS_DC) /* {{{ */
{
	/* Go to next opcode (we don't want to run the last one again) */
	generator->execute_data->opline++;

	/* We (mis) use the return_value_ptr_ptr to provide the generator object
	 * to the executor. This way YIELD will be able to set the yielded value */
	EG(return_value_ptr_ptr) = &object;

	execute_ex(generator->execute_data TSRMLS_CC);
}
/* }}} */

static void zend_generator_ensure_initialized(zval *object, zend_generator *generator TSRMLS_DC) /* {{{ */
{
	if (!generator->value) {
		zend_generator_resume(object, generator TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto void Generator::rewind()
 * Rewind the generator */
ZEND_METHOD(Generator, rewind)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool Generator::valid()
 * Check whether the generator is valid */
ZEND_METHOD(Generator, valid)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Generator::current()
 * Get the current value */
ZEND_METHOD(Generator, current)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Generator::key()
 * Get the current key */
ZEND_METHOD(Generator, key)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	object = getThis();
	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Generator::next()
 * Advances the generator */
ZEND_METHOD(Generator, next)
{
	zval *object;
	zend_generator *generator;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	generator = (zend_generator *) zend_object_store_get_object(object TSRMLS_CC);

	zend_generator_ensure_initialized(object, generator TSRMLS_CC);
}
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_generator_void, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry generator_functions[] = {
	ZEND_ME(Generator, rewind,  arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, valid,   arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, current, arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, key,     arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, next,    arginfo_generator_void, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_generator_ce(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Generator", generator_functions);
	zend_ce_generator = zend_register_internal_class(&ce TSRMLS_CC);
	zend_ce_generator->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_ce_generator->create_object = zend_generator_create;

	zend_class_implements(zend_ce_generator TSRMLS_CC, 1, zend_ce_iterator);

	memcpy(&zend_generator_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_generator_handlers.get_constructor = zend_generator_get_constructor;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
