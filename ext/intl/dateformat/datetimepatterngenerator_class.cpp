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
   | Authors: Mel Dafert (mel@dafert.at)                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../intl_cppshims.h"

#include <unicode/dtptngen.h>

extern "C" {
#define USE_DATETIMEPATTERNGENERATOR_POINTER 1
#include "datetimepatterngenerator_class.h"
#include "datetimepatterngenerator_arginfo.h"
#include <zend_exceptions.h>
#include <assert.h>
}

using icu::DateTimePatternGenerator;
using icu::Locale;

/* {{{ Global variables */
zend_class_entry *IntlDateTimePatternGenerator_ce_ptr;
zend_object_handlers IntlDateTimePatternGenerator_handlers;
/* }}} */

/* {{{ clone handler for IntlDateTimePatternGenerator */
static zend_object *IntlDateTimePatternGenerator_object_clone(zend_object *object)
{
	IntlDateTimePatternGenerator_object		*dtpgo_orig,
											*dtpgo_new;
	zend_object								*ret_val;
	intl_error_reset(NULL);

	dtpgo_orig = php_intl_datetimepatterngenerator_fetch_object(object);
	intl_error_reset(DTPATTERNGEN_ERROR_P(dtpgo_orig));

	ret_val = IntlDateTimePatternGenerator_ce_ptr->create_object(object->ce);
	dtpgo_new  = php_intl_datetimepatterngenerator_fetch_object(ret_val);

	zend_objects_clone_members(&dtpgo_new->zo, &dtpgo_orig->zo);

	if (dtpgo_orig->dtpg != NULL) {
		DateTimePatternGenerator	*newDtpg;

		newDtpg = dtpgo_orig->dtpg->clone();
		if (!newDtpg) {
			zend_string *err_msg;
			intl_errors_set_code(DTPATTERNGEN_ERROR_P(dtpgo_orig),
				U_MEMORY_ALLOCATION_ERROR);
			intl_errors_set_custom_msg(DTPATTERNGEN_ERROR_P(dtpgo_orig),
				"Could not clone IntlDateTimePatternGenerator", 0);
			err_msg = intl_error_get_message(DTPATTERNGEN_ERROR_P(dtpgo_orig));
			zend_throw_exception(NULL, ZSTR_VAL(err_msg), 0);
			zend_string_free(err_msg);
		} else {
			dtpgo_new->dtpg = newDtpg;
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed IntlDateTimePatternGenerator", 0);
	}

	return ret_val;
}
/* }}} */

/* {{{ void IntlDateTimePatternGenerator_object_init(IntlDateTimePatternGenerator_object* to)
 * Initialize internals of IntlDateTimePatternGenerator_object not specific to zend standard objects.
 */
static void IntlDateTimePatternGenerator_object_init(IntlDateTimePatternGenerator_object *co)
{
	intl_error_init(DTPATTERNGEN_ERROR_P(co));
	co->dtpg = NULL;
}
/* }}} */

/* {{{ IntlDateTimePatternGenerator_object_free */
static void IntlDateTimePatternGenerator_object_free(zend_object *object)
{
	IntlDateTimePatternGenerator_object* co = php_intl_datetimepatterngenerator_fetch_object(object);

	if (co->dtpg) {
		delete co->dtpg;
		co->dtpg = NULL;
	}
	intl_error_reset(DTPATTERNGEN_ERROR_P(co));

	zend_object_std_dtor(&co->zo);
}
/* }}} */

/* {{{ IntlDateTimePatternGenerator_object_create */
static zend_object *IntlDateTimePatternGenerator_object_create(zend_class_entry *ce)
{
	IntlDateTimePatternGenerator_object*	intern;

	intern = (IntlDateTimePatternGenerator_object*)ecalloc(1, sizeof(IntlDateTimePatternGenerator_object) + sizeof(zval) * (ce->default_properties_count - 1));

	zend_object_std_init(&intern->zo, ce);
	object_properties_init(&intern->zo, ce);
	IntlDateTimePatternGenerator_object_init(intern);


	intern->zo.handlers = &IntlDateTimePatternGenerator_handlers;

	return &intern->zo;
}
/* }}} */

/*
 * 'IntlDateTimePatternGenerator' class registration structures & functions
 */

/* {{{ dateformat_register_class
 * Initialize 'IntlDateTimePatternGenerator' class
 */
void dateformat_register_IntlDateTimePatternGenerator_class( void )
{
	/* Create and register 'IntlDateTimePatternGenerator' class. */
	IntlDateTimePatternGenerator_ce_ptr = register_class_IntlDateTimePatternGenerator();
	IntlDateTimePatternGenerator_ce_ptr->create_object = IntlDateTimePatternGenerator_object_create;

	memcpy(&IntlDateTimePatternGenerator_handlers, &std_object_handlers,
		sizeof IntlDateTimePatternGenerator_handlers);
	IntlDateTimePatternGenerator_handlers.offset = XtOffsetOf(IntlDateTimePatternGenerator_object, zo);
	IntlDateTimePatternGenerator_handlers.clone_obj = IntlDateTimePatternGenerator_object_clone;
	IntlDateTimePatternGenerator_handlers.free_obj = IntlDateTimePatternGenerator_object_free;
}
/* }}} */
