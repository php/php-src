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
#include "datepatterngenerator_class.h"
#include "datepatterngenerator_arginfo.h"
#include <zend_exceptions.h>
#include <assert.h>
}

using icu::DateTimePatternGenerator;
using icu::Locale;

zend_class_entry *IntlDatePatternGenerator_ce_ptr;
zend_object_handlers IntlDatePatternGenerator_handlers;

static zend_object *IntlDatePatternGenerator_object_clone(zend_object *object)
{
	intl_error_reset(NULL);

	IntlDatePatternGenerator_object *dtpgo_orig = php_intl_datepatterngenerator_fetch_object(object);
	intl_error_reset(DTPATTERNGEN_ERROR_P(dtpgo_orig));

	zend_object *ret_val = IntlDatePatternGenerator_ce_ptr->create_object(object->ce);
	IntlDatePatternGenerator_object *dtpgo_new  = php_intl_datepatterngenerator_fetch_object(ret_val);

	zend_objects_clone_members(&dtpgo_new->zo, &dtpgo_orig->zo);

	if (dtpgo_orig->dtpg != NULL) {
		DateTimePatternGenerator *newDtpg = dtpgo_orig->dtpg->clone();
		if (!newDtpg) {
			zend_string *err_msg;
			intl_errors_set_code(DTPATTERNGEN_ERROR_P(dtpgo_orig),
				U_MEMORY_ALLOCATION_ERROR);
			intl_errors_set_custom_msg(DTPATTERNGEN_ERROR_P(dtpgo_orig),
				"Could not clone IntlDatePatternGenerator", 0);
			err_msg = intl_error_get_message(DTPATTERNGEN_ERROR_P(dtpgo_orig));
			zend_throw_exception(NULL, ZSTR_VAL(err_msg), 0);
			zend_string_free(err_msg);
		} else {
			dtpgo_new->dtpg = newDtpg;
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed IntlDatePatternGenerator", 0);
	}

	return ret_val;
}

/*
 * Initialize internals of IntlDatePatternGenerator_object not specific to zend standard objects.
 */
static void IntlDatePatternGenerator_object_init(IntlDatePatternGenerator_object *co)
{
	intl_error_init(DTPATTERNGEN_ERROR_P(co));
	co->dtpg = NULL;
}

static void IntlDatePatternGenerator_object_free(zend_object *object)
{
	IntlDatePatternGenerator_object* co = php_intl_datepatterngenerator_fetch_object(object);

	if (co->dtpg) {
		delete co->dtpg;
		co->dtpg = NULL;
	}
	intl_error_reset(DTPATTERNGEN_ERROR_P(co));

	zend_object_std_dtor(&co->zo);
}

static zend_object *IntlDatePatternGenerator_object_create(zend_class_entry *ce)
{
	IntlDatePatternGenerator_object *intern =
		(IntlDatePatternGenerator_object*) zend_object_alloc(sizeof(IntlDatePatternGenerator_object), ce);

	zend_object_std_init(&intern->zo, ce);
	object_properties_init(&intern->zo, ce);
	IntlDatePatternGenerator_object_init(intern);

	intern->zo.handlers = &IntlDatePatternGenerator_handlers;

	return &intern->zo;
}

/*
 * 'IntlDatePatternGenerator' class registration structures & functions
 */

/*
 * Initialize 'IntlDatePatternGenerator' class
 */
void dateformat_register_IntlDatePatternGenerator_class( void )
{
	/* Create and register 'IntlDatePatternGenerator' class. */
	IntlDatePatternGenerator_ce_ptr = register_class_IntlDatePatternGenerator();
	IntlDatePatternGenerator_ce_ptr->create_object = IntlDatePatternGenerator_object_create;

	memcpy(&IntlDatePatternGenerator_handlers, &std_object_handlers,
		sizeof IntlDatePatternGenerator_handlers);
	IntlDatePatternGenerator_handlers.offset = XtOffsetOf(IntlDatePatternGenerator_object, zo);
	IntlDatePatternGenerator_handlers.clone_obj = IntlDatePatternGenerator_object_clone;
	IntlDatePatternGenerator_handlers.free_obj = IntlDatePatternGenerator_object_free;
}
