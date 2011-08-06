/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#include "spoofchecker_class.h"
#include "spoofchecker_main.h"
#include "spoofchecker_create.h"
#include "php_intl.h"
#include "intl_error.h"

#include <unicode/uspoof.h>

zend_class_entry *Spoofchecker_ce_ptr = NULL;
static zend_object_handlers Spoofchecker_handlers;

/*
 * Auxiliary functions needed by objects of 'Spoofchecker' class
 */

/* {{{ Spoofchecker_objects_dtor */
static void Spoofchecker_objects_dtor(
	void *object,
	zend_object_handle handle TSRMLS_DC)
{
	zend_objects_destroy_object(object, handle TSRMLS_CC);
}
/* }}} */

/* {{{ Spoofchecker_objects_free */
void Spoofchecker_objects_free(zend_object *object TSRMLS_DC)
{
	Spoofchecker_object* co = (Spoofchecker_object*)object;

	zend_object_std_dtor(&co->zo TSRMLS_CC);

	spoofchecker_object_destroy(co TSRMLS_CC);

	efree(co);
}
/* }}} */

/* {{{ Spoofchecker_object_create */
zend_object_value Spoofchecker_object_create(
	zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value    retval;
	Spoofchecker_object*     intern;

	intern = ecalloc(1, sizeof(Spoofchecker_object));
	intl_error_init(SPOOFCHECKER_ERROR_P(intern) TSRMLS_CC);
	zend_object_std_init(&intern->zo, ce TSRMLS_CC);

	retval.handle = zend_objects_store_put(
		intern,
		Spoofchecker_objects_dtor,
		(zend_objects_free_object_storage_t)Spoofchecker_objects_free,
		NULL TSRMLS_CC);

	retval.handlers = &Spoofchecker_handlers;

	return retval;
}
/* }}} */

/*
 * 'Spoofchecker' class registration structures & functions
 */

/* {{{ Spoofchecker methods arguments info */
ZEND_BEGIN_ARG_INFO_EX(spoofchecker_0_args, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(spoofchecker_set_checks, 0, 0, 1)
	ZEND_ARG_INFO(0, checks)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(spoofchecker_set_allowed_locales, 0, 0, 1)
	ZEND_ARG_INFO(0, locale_list)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(spoofchecker_is_suspicous, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(spoofchecker_are_confusable, 0, 0, 2)
	ZEND_ARG_INFO(0, s1)
	ZEND_ARG_INFO(0, s2)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ Spoofchecker_class_functions
 * Every 'Spoofchecker' class method has an entry in this table
 */

zend_function_entry Spoofchecker_class_functions[] = {
	PHP_ME(Spoofchecker, __construct, spoofchecker_0_args, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Spoofchecker, isSuspicious, spoofchecker_is_suspicous, ZEND_ACC_PUBLIC)
	PHP_ME(Spoofchecker, areConfusable, spoofchecker_are_confusable, ZEND_ACC_PUBLIC)
	PHP_ME(Spoofchecker, setAllowedLocales, spoofchecker_set_allowed_locales, ZEND_ACC_PUBLIC)
	PHP_ME(Spoofchecker, setChecks, spoofchecker_set_checks, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

static zend_object_value spoofchecker_clone_obj(zval *object TSRMLS_DC) /* {{{ */
{
	zend_object_value new_obj_val;
	zend_object_handle handle = Z_OBJ_HANDLE_P(object);
	Spoofchecker_object *sfo, *new_sfo;

    sfo = (Spoofchecker_object *) zend_object_store_get_object(object TSRMLS_CC);
    intl_error_reset(SPOOFCHECKER_ERROR_P(sfo) TSRMLS_CC);

	new_obj_val = Spoofchecker_ce_ptr->create_object(Spoofchecker_ce_ptr TSRMLS_CC);
	new_sfo = (Spoofchecker_object *)zend_object_store_get_object_by_handle(new_obj_val.handle TSRMLS_CC);
	/* clone standard parts */	
	zend_objects_clone_members(&new_sfo->zo, new_obj_val, &sfo->zo, handle TSRMLS_CC);
	/* clone internal object */
	new_sfo->uspoof = uspoof_clone(sfo->uspoof, SPOOFCHECKER_ERROR_CODE_P(new_sfo));
	if(U_FAILURE(SPOOFCHECKER_ERROR_CODE(new_sfo))) {
		/* set up error in case error handler is interested */
		intl_error_set( NULL, SPOOFCHECKER_ERROR_CODE(new_sfo), "Failed to clone SpoofChecker object", 0 TSRMLS_CC );
		Spoofchecker_objects_dtor(new_sfo, new_obj_val.handle TSRMLS_CC); /* free new object */
		zend_error(E_ERROR, "Failed to clone SpoofChecker object");
	}
	return new_obj_val;
}
/* }}} */

/* {{{ spoofchecker_register_Spoofchecker_class
 * Initialize 'Spoofchecker' class
 */
void spoofchecker_register_Spoofchecker_class(TSRMLS_D)
{
	zend_class_entry ce;

	/* Create and register 'Spoofchecker' class. */
	INIT_CLASS_ENTRY(ce, "Spoofchecker", Spoofchecker_class_functions);
	ce.create_object = Spoofchecker_object_create;
	Spoofchecker_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

	memcpy(&Spoofchecker_handlers, zend_get_std_object_handlers(),
		sizeof Spoofchecker_handlers);
	Spoofchecker_handlers.clone_obj = spoofchecker_clone_obj; 

	if (!Spoofchecker_ce_ptr) {
		zend_error(E_ERROR,
			"Spoofchecker: attempt to create properties "
			"on a non-registered class.");
		return;
	}
}
/* }}} */

/* {{{ void spoofchecker_object_init( Spoofchecker_object* co )
 * Initialize internals of Spoofchecker_object.
 * Must be called before any other call to 'spoofchecker_object_...' functions.
 */
void spoofchecker_object_init(Spoofchecker_object* co TSRMLS_DC)
{
	if (!co) {
		return;
	}

	intl_error_init(SPOOFCHECKER_ERROR_P(co) TSRMLS_CC);
}
/* }}} */

/* {{{ void spoofchecker_object_destroy( Spoofchecker_object* co )
 * Clean up mem allocted by internals of Spoofchecker_object
 */
void spoofchecker_object_destroy(Spoofchecker_object* co TSRMLS_DC)
{
	if (!co) {
		return;
	}

	if (co->uspoof) {
		uspoof_close(co->uspoof);
		co->uspoof = NULL;
	}

	intl_error_reset(SPOOFCHECKER_ERROR_P(co) TSRMLS_CC);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
