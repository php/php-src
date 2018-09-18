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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#include <unicode/unum.h>

#include "dateformat_class.h"
#include "php_intl.h"
#include "dateformat_data.h"
#include "dateformat_format.h"
#include "dateformat_format_object.h"
#include "dateformat_parse.h"
#include "dateformat.h"
#include "dateformat_attr.h"
#include "dateformat_attrcpp.h"

#include <zend_exceptions.h>

zend_class_entry *IntlDateFormatter_ce_ptr = NULL;
static zend_object_handlers IntlDateFormatter_handlers;

/*
 * Auxiliary functions needed by objects of 'IntlDateFormatter' class
 */

/* {{{ IntlDateFormatter_objects_dtor */
static void IntlDateFormatter_object_dtor(zend_object *object )
{
	zend_objects_destroy_object( object );
}
/* }}} */

/* {{{ IntlDateFormatter_objects_free */
void IntlDateFormatter_object_free( zend_object *object )
{
	IntlDateFormatter_object* dfo = php_intl_dateformatter_fetch_object(object);

	zend_object_std_dtor( &dfo->zo );

	if (dfo->requested_locale) {
		efree( dfo->requested_locale );
	}

	dateformat_data_free( &dfo->datef_data );
}
/* }}} */

/* {{{ IntlDateFormatter_object_create */
zend_object *IntlDateFormatter_object_create(zend_class_entry *ce)
{
	IntlDateFormatter_object*     intern;

	intern = zend_object_alloc(sizeof(IntlDateFormatter_object), ce);
	dateformat_data_init( &intern->datef_data );
	zend_object_std_init( &intern->zo, ce );
	object_properties_init(&intern->zo, ce);
	intern->date_type			= 0;
	intern->time_type			= 0;
	intern->calendar			= -1;
	intern->requested_locale	= NULL;

	intern->zo.handlers = &IntlDateFormatter_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ IntlDateFormatter_object_clone */
zend_object *IntlDateFormatter_object_clone(zval *object)
{
	IntlDateFormatter_object *dfo, *new_dfo;
	zend_object *new_obj;

	DATE_FORMAT_METHOD_FETCH_OBJECT_NO_CHECK;

	new_obj = IntlDateFormatter_ce_ptr->create_object(Z_OBJCE_P(object));
	new_dfo = php_intl_dateformatter_fetch_object(new_obj);
	/* clone standard parts */
	zend_objects_clone_members(&new_dfo->zo, &dfo->zo);
	/* clone formatter object */
	if (dfo->datef_data.udatf != NULL) {
		DATE_FORMAT_OBJECT(new_dfo) = udat_clone(DATE_FORMAT_OBJECT(dfo),  &INTL_DATA_ERROR_CODE(dfo));
		if (U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
			/* set up error in case error handler is interested */
			intl_errors_set(INTL_DATA_ERROR_P(dfo), INTL_DATA_ERROR_CODE(dfo),
					"Failed to clone IntlDateFormatter object", 0 );
			zend_throw_exception(NULL, "Failed to clone IntlDateFormatter object", 0);
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed IntlDateFormatter", 0);
	}
	return new_obj;
}
/* }}} */

/*
 * 'IntlDateFormatter' class registration structures & functions
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(datefmt_parse_args, 0, 0, 1)
		ZEND_ARG_INFO(0, string)
		ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_format, 0, 0, 0)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_format_object, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, locale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_intldateformatter_getdatetype, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_settimezoneid, 0, 0, 1)
	ZEND_ARG_INFO(0, zone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_setpattern, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_setlenient, 0, 0, 1)
	ZEND_ARG_INFO(0, lenient)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_setcalendar, 0, 0, 1)
	ZEND_ARG_INFO(0, which)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter___construct, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, datetype)
	ZEND_ARG_INFO(0, timetype)
	ZEND_ARG_INFO(0, timezone)
	ZEND_ARG_INFO(0, calendar)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ IntlDateFormatter_class_functions
 * Every 'IntlDateFormatter' class method has an entry in this table
 */
static const zend_function_entry IntlDateFormatter_class_functions[] = {
	PHP_ME( IntlDateFormatter, __construct, arginfo_intldateformatter___construct, ZEND_ACC_PUBLIC )
	ZEND_FENTRY(  create, ZEND_FN( datefmt_create ), arginfo_intldateformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( getDateType, ZEND_FN( datefmt_get_datetype ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getTimeType, ZEND_FN( datefmt_get_timetype ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getCalendar, ZEND_FN( datefmt_get_calendar ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getCalendarObject, ZEND_FN( datefmt_get_calendar_object ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( setCalendar, ZEND_FN( datefmt_set_calendar ), arginfo_intldateformatter_setcalendar )
	PHP_NAMED_FE( getTimeZoneId, ZEND_FN( datefmt_get_timezone_id ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getTimeZone, ZEND_FN( datefmt_get_timezone ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( setTimeZone, ZEND_FN( datefmt_set_timezone ), arginfo_intldateformatter_settimezoneid )
	PHP_NAMED_FE( setPattern, ZEND_FN( datefmt_set_pattern ), arginfo_intldateformatter_setpattern )
	PHP_NAMED_FE( getPattern, ZEND_FN( datefmt_get_pattern ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getLocale, ZEND_FN( datefmt_get_locale ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( setLenient, ZEND_FN( datefmt_set_lenient ), arginfo_intldateformatter_setlenient )
	PHP_NAMED_FE( isLenient, ZEND_FN( datefmt_is_lenient ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( format, ZEND_FN( datefmt_format ), arginfo_intldateformatter_format )
	PHP_ME_MAPPING( formatObject, datefmt_format_object, arginfo_intldateformatter_format_object, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_NAMED_FE( parse, ZEND_FN( datefmt_parse), datefmt_parse_args )
	PHP_NAMED_FE( localtime, ZEND_FN( datefmt_localtime ), datefmt_parse_args )
	PHP_NAMED_FE( getErrorCode, ZEND_FN( datefmt_get_error_code ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getErrorMessage, ZEND_FN( datefmt_get_error_message ), arginfo_intldateformatter_getdatetype )
	PHP_FE_END
};
/* }}} */

/* {{{ dateformat_register_class
 * Initialize 'IntlDateFormatter' class
 */
void dateformat_register_IntlDateFormatter_class( void )
{
	zend_class_entry ce;

	/* Create and register 'IntlDateFormatter' class. */
	INIT_CLASS_ENTRY( ce, "IntlDateFormatter", IntlDateFormatter_class_functions );
	ce.create_object = IntlDateFormatter_object_create;
	IntlDateFormatter_ce_ptr = zend_register_internal_class( &ce );

	memcpy(&IntlDateFormatter_handlers, &std_object_handlers,
		sizeof IntlDateFormatter_handlers);
	IntlDateFormatter_handlers.offset = XtOffsetOf(IntlDateFormatter_object, zo);
	IntlDateFormatter_handlers.clone_obj = IntlDateFormatter_object_clone;
	IntlDateFormatter_handlers.dtor_obj = IntlDateFormatter_object_dtor;
	IntlDateFormatter_handlers.free_obj = IntlDateFormatter_object_free;
}
/* }}} */
