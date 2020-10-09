/*
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
#include "spoofchecker_arginfo.h"
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

/* {{{ Spoofchecker_class_functions
 * Every 'Spoofchecker' class method has an entry in this table
 */

static zend_object *spoofchecker_clone_obj(zend_object *object) /* {{{ */
{
	zend_object *new_obj_val;
	Spoofchecker_object *sfo, *new_sfo;

    sfo = php_intl_spoofchecker_fetch_object(object);
    intl_error_reset(SPOOFCHECKER_ERROR_P(sfo));

	new_obj_val = Spoofchecker_ce_ptr->create_object(object->ce);
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
	INIT_CLASS_ENTRY(ce, "Spoofchecker", class_Spoofchecker_methods);
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
