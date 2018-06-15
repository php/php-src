/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

/* {{{ Spoofchecker_objects_free */
void Spoofchecker_objects_free(zend_object *object)
{
	Spoofchecker_object* co = php_intl_spoofchecker_fetch_object(object);

	zend_object_std_dtor(&co->zo);

	spoofchecker_object_destroy(co);
}
/* }}} */

/* {{{ Spoofchecker_object_create */
zend_object *Spoofchecker_object_create(zend_class_entry *ce)
{
	Spoofchecker_object*     intern;

	intern = zend_object_alloc(sizeof(Spoofchecker_object), ce);
	intl_error_init(SPOOFCHECKER_ERROR_P(intern));
	zend_object_std_init(&intern->zo, ce);
	object_properties_init(&intern->zo, ce);

	intern->zo.handlers = &Spoofchecker_handlers;

	return &intern->zo;
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

#if U_ICU_VERSION_MAJOR_NUM >= 58
ZEND_BEGIN_ARG_INFO_EX(spoofchecker_set_restriction_level, 0, 0, 1)
	ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO()
#endif

/* }}} */

/* {{{ Spoofchecker_class_functions
 * Every 'Spoofchecker' class method has an entry in this table
 */

static const zend_function_entry Spoofchecker_class_functions[] = {
	PHP_ME(Spoofchecker, __construct, spoofchecker_0_args, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Spoofchecker, isSuspicious, spoofchecker_is_suspicous, ZEND_ACC_PUBLIC)
	PHP_ME(Spoofchecker, areConfusable, spoofchecker_are_confusable, ZEND_ACC_PUBLIC)
	PHP_ME(Spoofchecker, setAllowedLocales, spoofchecker_set_allowed_locales, ZEND_ACC_PUBLIC)
	PHP_ME(Spoofchecker, setChecks, spoofchecker_set_checks, ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM >= 58
	PHP_ME(Spoofchecker, setRestrictionLevel, spoofchecker_set_restriction_level, ZEND_ACC_PUBLIC)
#endif
	PHP_FE_END
};
/* }}} */

static zend_object *spoofchecker_clone_obj(zval *object) /* {{{ */
{
	zend_object *new_obj_val;
	Spoofchecker_object *sfo, *new_sfo;

    sfo = Z_INTL_SPOOFCHECKER_P(object);
    intl_error_reset(SPOOFCHECKER_ERROR_P(sfo));

	new_obj_val = Spoofchecker_ce_ptr->create_object(Z_OBJCE_P(object));
	new_sfo = php_intl_spoofchecker_fetch_object(new_obj_val);
	/* clone standard parts */
	zend_objects_clone_members(&new_sfo->zo, &sfo->zo);
	/* clone internal object */
	new_sfo->uspoof = uspoof_clone(sfo->uspoof, SPOOFCHECKER_ERROR_CODE_P(new_sfo));
	if(U_FAILURE(SPOOFCHECKER_ERROR_CODE(new_sfo))) {
		/* set up error in case error handler is interested */
		intl_error_set( NULL, SPOOFCHECKER_ERROR_CODE(new_sfo), "Failed to clone SpoofChecker object", 0 );
		Spoofchecker_objects_free(&new_sfo->zo); /* free new object */
		zend_error(E_ERROR, "Failed to clone SpoofChecker object");
	}
	return new_obj_val;
}
/* }}} */

/* {{{ spoofchecker_register_Spoofchecker_class
 * Initialize 'Spoofchecker' class
 */
void spoofchecker_register_Spoofchecker_class(void)
{
	zend_class_entry ce;

	/* Create and register 'Spoofchecker' class. */
	INIT_CLASS_ENTRY(ce, "Spoofchecker", Spoofchecker_class_functions);
	ce.create_object = Spoofchecker_object_create;
	Spoofchecker_ce_ptr = zend_register_internal_class(&ce);

	memcpy(&Spoofchecker_handlers, &std_object_handlers,
		sizeof Spoofchecker_handlers);
	Spoofchecker_handlers.offset = XtOffsetOf(Spoofchecker_object, zo);
	Spoofchecker_handlers.clone_obj = spoofchecker_clone_obj;
	Spoofchecker_handlers.free_obj = Spoofchecker_objects_free;
}
/* }}} */

/* {{{ void spoofchecker_object_init( Spoofchecker_object* co )
 * Initialize internals of Spoofchecker_object.
 * Must be called before any other call to 'spoofchecker_object_...' functions.
 */
void spoofchecker_object_init(Spoofchecker_object* co)
{
	if (!co) {
		return;
	}

	intl_error_init(SPOOFCHECKER_ERROR_P(co));
}
/* }}} */

/* {{{ void spoofchecker_object_destroy( Spoofchecker_object* co )
 * Clean up mem allocted by internals of Spoofchecker_object
 */
void spoofchecker_object_destroy(Spoofchecker_object* co)
{
	if (!co) {
		return;
	}

	if (co->uspoof) {
		uspoof_close(co->uspoof);
		co->uspoof = NULL;
	}

	intl_error_reset(SPOOFCHECKER_ERROR_P(co));
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
