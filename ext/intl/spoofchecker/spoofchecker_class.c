/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
	Spoofchecker_object *spoofchecker_orig = php_intl_spoofchecker_fetch_object(object);
	zend_object *new_obj_val               = Spoofchecker_ce_ptr->create_object(object->ce);
	Spoofchecker_object *spoofchecker_new  = php_intl_spoofchecker_fetch_object(new_obj_val);

	zend_objects_clone_members(&spoofchecker_new->zo, &spoofchecker_orig->zo);

	if (spoofchecker_orig->uspoof != NULL) {
		/* guaranteed to return NULL if it fails */
		UErrorCode error = U_ZERO_ERROR;
		spoofchecker_new->uspoof = uspoof_clone(spoofchecker_orig->uspoof, &error);
		if (U_FAILURE(error)) {
			/* free new object */
			Spoofchecker_objects_free(&spoofchecker_new->zo);
			zend_throw_error(NULL, "Failed to clone SpoofChecker");
		}
	} else {
		zend_throw_error(NULL, "Cannot clone uninitialized SpoofChecker");
	}

	return new_obj_val;
}
/* }}} */

/* {{{ spoofchecker_register_Spoofchecker_class
 * Initialize 'Spoofchecker' class
 */
void spoofchecker_register_Spoofchecker_class(void)
{
	/* Create and register 'Spoofchecker' class. */
	Spoofchecker_ce_ptr = register_class_Spoofchecker();
	Spoofchecker_ce_ptr->create_object = Spoofchecker_object_create;
	Spoofchecker_ce_ptr->default_object_handlers = &Spoofchecker_handlers;

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

#if U_ICU_VERSION_MAJOR_NUM >= 58
	if (co->uspoofres) {
		uspoof_closeCheckResult(co->uspoofres);
		co->uspoofres = NULL;
	}
#endif

	intl_error_reset(SPOOFCHECKER_ERROR_P(co));
}
/* }}} */
